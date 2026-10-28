/**
 * STM32F746G-DISCO — TCES 430 Lab 3
 * Nested EXTI interrupts + TIM2 LED sequencer + 1602A LCD (4-bit interface).
 *
 * Drop-in for STM32CubeIDE HAL project: Core/Src/main.c
 * Requires the project-generated main.h, HAL and startup files.
 *
 * RGB LED (four-pin, shared common):
 *   Red    D3 = PB4      Green  D4 = PG7
 *   Blue   D7 = PI3      Common D5 = PA8
 *   SW1    D8 = PI2      SW2    D2 = PG6
 *
 * Each RGB channel requires its OWN series current-limiting resistor.
 * Do not exceed the allowable combined current of PA8 when all 3 channels
 * illuminate. For higher LED current, connect the common terminal to a
 * properly rated supply/driver instead of a GPIO (and adjust the firmware).
 */

#include "main.h"
#include <stdint.h>
#include "stm32f7xx.h"

/* Set 1 for common-anode (+ at common), 0 for common-cathode (- at common).
 * D5/PA8 is actively driven to the proper common potential in either case. */
#define RGB_COMMON_ANODE       0U

#define RED_MASK               (1U << 0)
#define GREEN_MASK             (1U << 1)
#define BLUE_MASK              (1U << 2)
#define ALL_COLORS             (RED_MASK | GREEN_MASK | BLUE_MASK)
#define SW1_BIT                2U  /* PI2 / Arduino D8  */
#define SW2_BIT                6U  /* PG6 / Arduino D2  */

/* The actual clock configuration below uses HSI=16MHz and APB1=16MHz. */
#define TIMER_TICK_HZ          1000UL
#define LCD_LINE_CHARS         16U

typedef enum {
    MODE_RED,
    MODE_GREEN,
    MODE_PAUSE,
    MODE_BLUE,
    MODE_LIGHT
} DisplayMode;

static volatile uint8_t current_colors = 0U;
static volatile DisplayMode current_mode = MODE_RED;
static volatile uint8_t current_blink = 0U;

/* Cumulative time in higher priority SW2, excluded from SW1's delay. */
static volatile uint32_t sw2_pause_us = 0U;

/* Debugger watch variables. */
volatile uint32_t sw1_irq_count = 0U;
volatile uint32_t sw2_irq_count = 0U;

void SystemClock_Config(void);
static void MPU_Config(void);

static void gpio_output(GPIO_TypeDef *port, uint32_t pin)
{
    port->MODER = (port->MODER & ~(3UL << (pin * 2U)))
                | (1UL << (pin * 2U));
    port->OTYPER &= ~(1UL << pin);
    port->PUPDR &= ~(3UL << (pin * 2U));
}

static void gpio_input_pullup(GPIO_TypeDef *port, uint32_t pin)
{
    port->MODER &= ~(3UL << (pin * 2U));
    port->PUPDR = (port->PUPDR & ~(3UL << (pin * 2U)))
                | (1UL << (pin * 2U));
}

static void gpio_write(GPIO_TypeDef *port, uint32_t pin, uint32_t high)
{
    port->BSRR = high ? (1UL << pin) : (1UL << (pin + 16U));
}

/* TIM5 runs freely at 1 MHz. All delays are independent of compiler speed. */
static void delay_us(uint32_t us)
{
    const uint32_t start = TIM5->CNT;
    while ((uint32_t)(TIM5->CNT - start) < us) { }
}

static void delay_ms(uint32_t ms)
{
    while (ms--) delay_us(1000U);
}

static uint32_t sw1_time_us(void)
{
    /* Read both counters consistently, even with nested SW2 interrupts. */
    const uint32_t original_primask = __get_PRIMASK();
    __disable_irq();
    const uint32_t elapsed = TIM5->CNT - sw2_pause_us;
    __set_PRIMASK(original_primask);
    return elapsed;
}

static void sw1_delay_ms(uint32_t ms)
{
    const uint32_t start = sw1_time_us();
    while ((uint32_t)(sw1_time_us() - start) < ms * 1000UL) { }
}

static void rgb_write(uint8_t colors)
{
    const uint8_t red_on = ((colors & RED_MASK) != 0U);
    const uint8_t green_on = ((colors & GREEN_MASK) != 0U);
    const uint8_t blue_on = ((colors & BLUE_MASK) != 0U);

#if RGB_COMMON_ANODE
    /* Anode at 3.3V, each cathode goes LOW to turn ON. */
    gpio_write(GPIOB, 4U, !red_on);
    gpio_write(GPIOG, 7U, !green_on);
    gpio_write(GPIOI, 3U, !blue_on);
#else
    /* Cathode at 0V, each anode goes HIGH to turn ON. */
    gpio_write(GPIOB, 4U, red_on);
    gpio_write(GPIOG, 7U, green_on);
    gpio_write(GPIOI, 3U, blue_on);
#endif
    current_colors = colors;
}

static void set_state(DisplayMode mode, uint8_t blink, uint8_t colors)
{
    const uint32_t original_primask = __get_PRIMASK();
    __disable_irq();
    current_mode = mode;
    current_blink = blink;
    rgb_write(colors);
    __set_PRIMASK(original_primask);
}

/* ===================== 1602A LCD: 4-bit, write-only =====================
 * RS  A0/PA0; E A1/PF10; DB4 A2/PF9; DB5 A3/PF8;
 * DB6 A4/PF7; DB7 A5/PF6; R/W connected to GND.
 * LCD DB0..DB3 are not connected.
 */
static void lcd_write_nibble(uint8_t nibble)
{
    gpio_write(GPIOF, 9U, nibble & 0x1U);
    gpio_write(GPIOF, 8U, nibble & 0x2U);
    gpio_write(GPIOF, 7U, nibble & 0x4U);
    gpio_write(GPIOF, 6U, nibble & 0x8U);
    delay_us(2U);
    gpio_write(GPIOF, 10U, 1U);
    delay_us(2U);
    gpio_write(GPIOF, 10U, 0U);
    delay_us(2U);
}

static void lcd_write_byte(uint8_t value, uint8_t data)
{
    gpio_write(GPIOA, 0U, data);                 /* RS */
    lcd_write_nibble(value >> 4);
    lcd_write_nibble(value & 0x0FU);
    delay_us(50U);
}

static void lcd_cmd(uint8_t cmd)
{
    lcd_write_byte(cmd, 0U);
    if (cmd == 0x01U || cmd == 0x02U) delay_ms(2U);
}

static void lcd_text(const char *text)
{
    while (*text) lcd_write_byte((uint8_t)*text++, 1U);
}

static void lcd_init(void)
{
    gpio_output(GPIOA, 0U);
    gpio_output(GPIOF, 10U);
    gpio_output(GPIOF, 9U);
    gpio_output(GPIOF, 8U);
    gpio_output(GPIOF, 7U);
    gpio_output(GPIOF, 6U);
    gpio_write(GPIOA, 0U, 0U);
    gpio_write(GPIOF, 10U, 0U);

    /* Power-up defaults to 8-bit. Enter 4-bit mode using single nibbles. */
    delay_ms(50U);
    lcd_write_nibble(0x3U); delay_ms(5U);
    lcd_write_nibble(0x3U); delay_us(200U);
    lcd_write_nibble(0x3U); delay_us(200U);
    lcd_write_nibble(0x2U); delay_us(200U);
    lcd_cmd(0x28U);       /* 4-bit bus, 2 rows, 5x8 dots */
    lcd_cmd(0x08U);       /* Display off */
    lcd_cmd(0x01U);       /* Clear */
    lcd_cmd(0x06U);       /* Auto-increment address */
    lcd_cmd(0x0CU);       /* Display on, no cursor */
    lcd_cmd(0xC0U);       /* Second line, col 0 */
    lcd_text("Lab 3 Interrupt");
}

static void lcd_status(DisplayMode mode, uint8_t blink)
{
    const char *prefix;
    const char *color;
    char line[LCD_LINE_CHARS + 1U];
    unsigned n = 0U;

    switch (mode) {
        case MODE_RED:   prefix = "NOR";  color = "RED";  break;
        case MODE_GREEN: prefix = "NOR";  color = "GRN";  break;
        case MODE_BLUE:  prefix = "INT1"; color = "BLUE"; break;
        case MODE_LIGHT: prefix = "INT2"; color = "LGHT"; break;
        default:         prefix = "NOR";  color = "WAIT"; break;
    }

    while (*prefix && n < LCD_LINE_CHARS) line[n++] = *prefix++;
    line[n++] = ' '; line[n++] = '>'; line[n++] = ' ';
    while (*color && n < LCD_LINE_CHARS) line[n++] = *color++;
    if (blink && (n + 4U <= LCD_LINE_CHARS)) {
        line[n++] = ' '; line[n++] = '>'; line[n++] = ' ';
        line[n++] = (char)('0' + blink);
    }
    while (n < LCD_LINE_CHARS) line[n++] = ' ';
    line[LCD_LINE_CHARS] = '\0';

    /* LCD bus is shared by main, SW1 ISR and SW2 ISR. Keep an entire
       line write indivisible; input edges are pending until re-enabled. */
    const uint32_t original_primask = __get_PRIMASK();
    __disable_irq();
    lcd_cmd(0x80U);
    lcd_text(line);
    __set_PRIMASK(original_primask);
}

static uint8_t timer_pause(void)
{
    const uint8_t was_running = !!(TIM2->CR1 & TIM_CR1_CEN);
    TIM2->CR1 &= ~TIM_CR1_CEN;
    return was_running;
}

static void timer_resume(uint8_t was_running)
{
    if (was_running) TIM2->CR1 |= TIM_CR1_CEN;
}

int main(void)
{
    MPU_Config();
    HAL_Init();
    SystemClock_Config();
    SystemCoreClockUpdate();

    /* GPIOA, GPIOB, GPIOF, GPIOG, GPIOI; SYSCFG, TIM2, TIM5. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN
                  | RCC_AHB1ENR_GPIOFEN | RCC_AHB1ENR_GPIOGEN
                  | RCC_AHB1ENR_GPIOIEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM5EN;
    (void)RCC->AHB1ENR;
    (void)RCC->APB1ENR;

    /* HSI=16MHz, APB1 prescaler=1 (see SystemClock_Config below). */
    TIM5->CR1 = 0U;
    TIM5->PSC = (SystemCoreClock / 1000000UL) - 1UL;
    TIM5->ARR = 0xFFFFFFFFUL;
    TIM5->EGR = TIM_EGR_UG;
    TIM5->SR = 0U;
    TIM5->CR1 = TIM_CR1_CEN;

    /* Set data latch before enabling RGB pins to avoid startup flashes. */
#if RGB_COMMON_ANODE
    gpio_write(GPIOB, 4U, 1U);
    gpio_write(GPIOG, 7U, 1U);
    gpio_write(GPIOI, 3U, 1U);
    gpio_write(GPIOA, 8U, 1U);        /* D5/PA8 common anode (+) */
#else
    gpio_write(GPIOB, 4U, 0U);
    gpio_write(GPIOG, 7U, 0U);
    gpio_write(GPIOI, 3U, 0U);
    gpio_write(GPIOA, 8U, 0U);        /* D5/PA8 common cathode (-) */
#endif
    gpio_output(GPIOB, 4U);
    gpio_output(GPIOG, 7U);
    gpio_output(GPIOI, 3U);
    gpio_output(GPIOA, 8U);
    rgb_write(0U);

    gpio_input_pullup(GPIOI, SW1_BIT);
    gpio_input_pullup(GPIOG, SW2_BIT);

    /* Power-on LED test, then start the LCD. */
    rgb_write(RED_MASK); delay_ms(200U);
    rgb_write(GREEN_MASK); delay_ms(200U);
    rgb_write(BLUE_MASK); delay_ms(200U);
    rgb_write(0U);
    lcd_init();
    lcd_status(MODE_RED, 0U);

    /* EXTI2: switch on PI2 -> port I code 8. */
    SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & ~(0xFUL << 8U))
                       | (0x8UL << 8U);
    /* EXTI6: switch on PG6 -> port G code 6. */
    SYSCFG->EXTICR[1] = (SYSCFG->EXTICR[1] & ~(0xFUL << 8U))
                       | (0x6UL << 8U);
    EXTI->IMR |= (1UL << SW1_BIT) | (1UL << SW2_BIT);
    EXTI->FTSR |= (1UL << SW1_BIT) | (1UL << SW2_BIT);
    EXTI->RTSR &= ~((1UL << SW1_BIT) | (1UL << SW2_BIT));
    EXTI->PR = (1UL << SW1_BIT) | (1UL << SW2_BIT);

    /* All implemented priority bits are preempt priorities. */
    NVIC_SetPriorityGrouping(0U);
    NVIC_SetPriority(EXTI2_IRQn, 5U);
    NVIC_SetPriority(EXTI9_5_IRQn, 2U);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    __enable_irq();

    /* TIM2 update every 250ms; no TIM2 interrupt, main polls UIF. */
    TIM2->CR1 = 0U;
    TIM2->PSC = (SystemCoreClock / TIMER_TICK_HZ) - 1UL;
    TIM2->ARR = 249U;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0U;
    TIM2->CR1 = TIM_CR1_CEN;

    DisplayMode sequence = MODE_RED;
    uint8_t toggles = 0U;

    while (1) {
        if (!(TIM2->SR & TIM_SR_UIF)) continue;
        TIM2->SR = 0U;

        switch (sequence) {
            case MODE_RED:
                if (toggles < 10U) {
                    const uint8_t blink = (toggles / 2U) + 1U;
                    set_state(MODE_RED, blink,
                              !(toggles & 1U) ? RED_MASK : 0U);
                    lcd_status(MODE_RED, blink);
                    ++toggles;
                } else {
                    set_state(MODE_RED, 0U, 0U);
                    sequence = MODE_GREEN;
                    toggles = 0U;
                }
                break;

            case MODE_GREEN:
                if (toggles < 16U) {
                    const uint8_t blink = (toggles / 2U) + 1U;
                    set_state(MODE_GREEN, blink,
                              !(toggles & 1U) ? GREEN_MASK : 0U);
                    lcd_status(MODE_GREEN, blink);
                    ++toggles;
                } else {
                    set_state(MODE_PAUSE, 0U, 0U);
                    lcd_status(MODE_PAUSE, 0U);
                    TIM2->ARR = 1999U; /* Wait ~2 seconds */
                    sequence = MODE_PAUSE;
                }
                break;

            case MODE_PAUSE:
                TIM2->ARR = 249U;
                set_state(MODE_RED, 0U, 0U);
                lcd_status(MODE_RED, 0U);
                sequence = MODE_RED;
                toggles = 0U;
                break;

            default:
                sequence = MODE_RED;
                toggles = 0U;
                break;
        }
    }
}

/* SW1 priority 5: blue LED 3 blinks. SW2 priority 2 may preempt. */
void EXTI2_IRQHandler(void)
{
    if (!(EXTI->PR & (1UL << SW1_BIT))) return;
    EXTI->PR = 1UL << SW1_BIT;
    ++sw1_irq_count;

    const uint8_t was_running = timer_pause();
    const uint8_t saved_color = current_colors;
    const DisplayMode saved_mode = current_mode;
    const uint8_t saved_blink = current_blink;

    for (uint8_t n = 1U; n <= 3U; ++n) {
        set_state(MODE_BLUE, n, BLUE_MASK);
        lcd_status(MODE_BLUE, n);
        sw1_delay_ms(250U);
        set_state(MODE_BLUE, n, 0U);
        sw1_delay_ms(250U);
    }

    set_state(saved_mode, saved_blink, saved_color);
    lcd_status(saved_mode, saved_blink);
    timer_resume(was_running);
    EXTI->PR = 1UL << SW1_BIT; /* Drop bounces accumulated in ISR */
}

/* SW2 priority 2: all RGB channels (white) blink 5 times, preempting SW1. */
void EXTI9_5_IRQHandler(void)
{
    if (!(EXTI->PR & (1UL << SW2_BIT))) return;
    const uint32_t started = TIM5->CNT;
    EXTI->PR = 1UL << SW2_BIT;
    ++sw2_irq_count;

    const uint8_t was_running = timer_pause();
    const uint8_t saved_color = current_colors;
    const DisplayMode saved_mode = current_mode;
    const uint8_t saved_blink = current_blink;

    for (uint8_t n = 1U; n <= 5U; ++n) {
        set_state(MODE_LIGHT, n, ALL_COLORS);
        lcd_status(MODE_LIGHT, n);
        delay_ms(250U);
        set_state(MODE_LIGHT, n, 0U);
        delay_ms(250U);
    }

    set_state(saved_mode, saved_blink, saved_color);
    lcd_status(saved_mode, saved_blink);
    timer_resume(was_running);
    EXTI->PR = 1UL << SW2_BIT;
    sw2_pause_us += (uint32_t)(TIM5->CNT - started);
}

/* HAL startup and clock configuration: HSI=16 MHz, no PLL, APB1=1. */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    osc.HSIState = RCC_HSI_ON;
    osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    osc.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) Error_Handler();

    clk.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV1;
    clk.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_0) != HAL_OK) Error_Handler();
}

/* Retains the default STM32CubeIDE F7 MPU setup from the supplied project. */
static void MPU_Config(void)
{
    MPU_Region_InitTypeDef mpu = {0};
    HAL_MPU_Disable();
    mpu.Enable = MPU_REGION_ENABLE;
    mpu.Number = MPU_REGION_NUMBER0;
    mpu.BaseAddress = 0x0U;
    mpu.Size = MPU_REGION_SIZE_4GB;
    mpu.SubRegionDisable = 0x87U;
    mpu.TypeExtField = MPU_TEX_LEVEL0;
    mpu.AccessPermission = MPU_REGION_NO_ACCESS;
    mpu.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    mpu.IsShareable = MPU_ACCESS_SHAREABLE;
    mpu.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    mpu.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    HAL_MPU_ConfigRegion(&mpu);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
}
#endif
