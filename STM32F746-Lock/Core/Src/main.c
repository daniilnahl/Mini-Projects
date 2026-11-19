
//PB8 / D15 = I2C1 SCL
//PB9 / D14 = I2C1 SDA
//both af4 in alt functions

#include "main.h"
#include "i2c_lcd.h"

//0 = scan PCF8574 addresses 0x20-0x27 and PCF8574A addresses 0x38-0x3F.
//Set a nonzero 7-bit address here for different address-jumper settings.
#define LCD_ADDRESS_7BIT 0U
#define I2C_TIMEOUT_US 20000U
#define CLOCK_STARTUP_LIMIT 1000000U

//Register addresses = base address + byte offset, then dereference.
//RCC_BASE=0x40023800, GPIOB_BASE=0x40020400, I2C1_BASE=0x40005400,
//TIM2_BASE=0x40000000, defined by the STM32F746 device header.
#define RCC_CR_OFFSET       0x00U
#define RCC_CFGR_OFFSET     0x08U
#define RCC_APB1RSTR_OFFSET 0x20U
#define RCC_AHB1ENR_OFFSET  0x30U
#define RCC_APB1ENR_OFFSET  0x40U
#define RCC_DCKCFGR2_OFFSET 0x90U
#define RCC_CR       (*(volatile uint32_t *)(RCC_BASE + RCC_CR_OFFSET))
#define RCC_CFGR     (*(volatile uint32_t *)(RCC_BASE + RCC_CFGR_OFFSET))
#define RCC_APB1RSTR (*(volatile uint32_t *)(RCC_BASE + RCC_APB1RSTR_OFFSET))
#define RCC_AHB1ENR  (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))
#define RCC_APB1ENR  (*(volatile uint32_t *)(RCC_BASE + RCC_APB1ENR_OFFSET))
#define RCC_DCKCFGR2 (*(volatile uint32_t *)(RCC_BASE + RCC_DCKCFGR2_OFFSET))

#define GPIOB_MODER_OFFSET   0x00U
#define GPIOB_OTYPER_OFFSET  0x04U
#define GPIOB_OSPEEDR_OFFSET 0x08U
#define GPIOB_PUPDR_OFFSET   0x0CU
#define GPIOB_AFRH_OFFSET    0x24U
#define GPIOB_MODER   (*(volatile uint32_t *)(GPIOB_BASE + GPIOB_MODER_OFFSET))
#define GPIOB_OTYPER  (*(volatile uint32_t *)(GPIOB_BASE + GPIOB_OTYPER_OFFSET))
#define GPIOB_OSPEEDR (*(volatile uint32_t *)(GPIOB_BASE + GPIOB_OSPEEDR_OFFSET))
#define GPIOB_PUPDR   (*(volatile uint32_t *)(GPIOB_BASE + GPIOB_PUPDR_OFFSET))
#define GPIOB_AFRH    (*(volatile uint32_t *)(GPIOB_BASE + GPIOB_AFRH_OFFSET))

#define I2C1_CR1_OFFSET     0x00U
#define I2C1_CR2_OFFSET     0x04U
#define I2C1_TIMINGR_OFFSET 0x10U
#define I2C1_ISR_OFFSET     0x18U
#define I2C1_ICR_OFFSET     0x1CU
#define I2C1_TXDR_OFFSET    0x28U
#define I2C1_CR1     (*(volatile uint32_t *)(I2C1_BASE + I2C1_CR1_OFFSET))
#define I2C1_CR2     (*(volatile uint32_t *)(I2C1_BASE + I2C1_CR2_OFFSET))
#define I2C1_TIMINGR (*(volatile uint32_t *)(I2C1_BASE + I2C1_TIMINGR_OFFSET))
#define I2C1_ISR     (*(volatile uint32_t *)(I2C1_BASE + I2C1_ISR_OFFSET))
#define I2C1_ICR     (*(volatile uint32_t *)(I2C1_BASE + I2C1_ICR_OFFSET))
#define I2C1_TXDR    (*(volatile uint32_t *)(I2C1_BASE + I2C1_TXDR_OFFSET))

#define TIM2_CR1  (*(volatile uint32_t *)(TIM2_BASE + 0x00U))
#define TIM2_DIER (*(volatile uint32_t *)(TIM2_BASE + 0x0CU))
#define TIM2_SR   (*(volatile uint32_t *)(TIM2_BASE + 0x10U))
#define TIM2_EGR  (*(volatile uint32_t *)(TIM2_BASE + 0x14U))
#define TIM2_CNT  (*(volatile uint32_t *)(TIM2_BASE + 0x24U))
#define TIM2_PSC  (*(volatile uint32_t *)(TIM2_BASE + 0x28U))
#define TIM2_ARR  (*(volatile uint32_t *)(TIM2_BASE + 0x2CU))

#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x44U))
#define SYSCFG_EXTICR1 (*(volatile uint32_t *)(SYSCFG_BASE + 0x08U))
#define EXTI_IMR (*(volatile uint32_t *)(EXTI_BASE + 0x00U))
#define EXTI_RTSR (*(volatile uint32_t *)(EXTI_BASE + 0x08U))
#define EXTI_FTSR (*(volatile uint32_t *)(EXTI_BASE + 0x0CU))
#define EXTI_PR (*(volatile uint32_t *)(EXTI_BASE + 0x14U))
#define TIM6_CR1 (*(volatile uint32_t *)(TIM6_BASE + 0x00U))
#define TIM6_DIER (*(volatile uint32_t *)(TIM6_BASE + 0x0CU))
#define TIM6_SR (*(volatile uint32_t *)(TIM6_BASE + 0x10U))
#define TIM6_EGR (*(volatile uint32_t *)(TIM6_BASE + 0x14U))
#define TIM6_CNT (*(volatile uint32_t *)(TIM6_BASE + 0x24U))
#define TIM6_PSC (*(volatile uint32_t *)(TIM6_BASE + 0x28U))
#define TIM6_ARR (*(volatile uint32_t *)(TIM6_BASE + 0x2CU))

//Each GPIO has the same layout. These helpers still access registers directly.
//Passing the base lets us reuse the same operation for GPIOA, C, G, and I.
#define GPIO_MODE(base) (*(volatile uint32_t *)((base) + 0x00U))
#define GPIO_TYPE(base) (*(volatile uint32_t *)((base) + 0x04U))
#define GPIO_PULL(base) (*(volatile uint32_t *)((base) + 0x0CU))
#define GPIO_INPUT(base) (*(volatile uint32_t *)((base) + 0x10U))

//16 MHz HSI / (PRESC+1=4) = 250 ns per timing tick.
//SCLL=SCLH=19: 5 us low + 5 us high BEFORE synchronization/filter delays.
//Slightly below 100 kHz nominally, leaving margin versus the transcript's
//0x30420F13 example. Analog filter enabled, digital filter disabled.
#define I2C1_TIMING_100KHZ 0x30421313U
#define I2C_ERROR_FLAGS (I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR)
#define I2C_CLEAR_FLAGS (I2C_ICR_NACKCF | I2C_ICR_STOPCF | I2C_ICR_BERRCF | \
                         I2C_ICR_ARLOCF | I2C_ICR_OVRCF)

//The two-pin active buzzer is replaced by an LED for this version
//D3 drives that led
#define KEY_FLASH_US 50000U
#define SECRET_PIN "1234"
_Static_assert(sizeof(SECRET_PIN) == 5U, "SECRET_PIN must contain four keys");
#define LOCKOUT_SECONDS 30U
#define DEBOUNCE_US 20000U
#define RELEASE_US 30000U

#define GPIO_SET_RESET(base) (*(volatile uint32_t *)((base) + 0x18U))

typedef enum {
    I2C_ERROR_NONE = 0,
    I2C_ERROR_NACK,
    I2C_ERROR_TIMEOUT,
    I2C_ERROR_BUS,
    I2C_ERROR_ARBITRATION,
    I2C_ERROR_OVERRUN,
    I2C_ERROR_EARLY_STOP
} I2C_Error;

//g_lcd_status: 1 = starting. IF = 0 then text displayed if = -1/-2 then driver error
volatile int32_t g_lcd_status = 1;
volatile uint8_t g_lcd_address = 0U;
volatile I2C_Error g_i2c_error = I2C_ERROR_NONE;
volatile I2C_Error g_i2c_last_error = I2C_ERROR_NONE;
volatile uint32_t g_i2c_last_isr = 0U;
volatile uint32_t g_lcd_attempts = 0U;
static uint8_t current_i2c_address = 0U;
static uint32_t current_i2c_length = 0U;
volatile uint8_t g_i2c_last_address = 0U;
volatile uint32_t g_i2c_last_length = 0U;
volatile uint8_t g_lcd_detected_address = 0U;
//Bit0..7 = ACK at 0x20..0x27, bit8..15 = ACK at 0x38..0x3F.
volatile uint32_t g_lcd_scan_acks = 0U;
volatile uint8_t g_lcd_scan_complete = 0U;
/*informs what stage the program got to:
1=clock
2=TIM2
3=I2C
4=LCD retry
5=LCD ready
6=TIM6 init
7=keypad init
8=application running
*/
volatile uint32_t g_boot_stage = 0U;

//KEYPAD:
//R1=D4/PG7, R2=D2/PG6, R3=D1/PC6, R4=D0/PC7
//C1=A0/PA0, C2=D13/PI1, C3=D8/PI2, C4=D7/PI3
static const uint32_t row_port[4] = {GPIOG_BASE, GPIOG_BASE, GPIOC_BASE, GPIOC_BASE};
static const uint8_t row_pin[4] = {7U, 6U, 6U, 7U};
static const uint32_t column_port[4] = {GPIOA_BASE, GPIOI_BASE, GPIOI_BASE, GPIOI_BASE};
static const uint8_t column_pin[4] = {0U, 1U, 2U, 3U};
static const char key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

//volatile tells the compiler these values can change inside an interrupt
//The ISR fills one event, then masks further keypad interrupts until release
static volatile uint8_t key_waiting = 0U;
static volatile uint8_t keypad_armed = 0U;
static volatile char captured_key = '\0';
static volatile uint8_t captured_column = 0U;
static volatile uint32_t captured_at = 0U;
static volatile uint8_t captured_during_entry = 0U;
volatile char g_last_key = '\0';
volatile uint32_t g_key_events = 0U;
volatile uint8_t g_columns_low = 0U;

//A state records what the safe is doing right now
//The ISR only reads this. The main loop is the only place that changes it
typedef enum {PIN_ENTRY, ACCESS_GRANTED, ACCESS_DENIED, LOCKED_OUT} SafeState;
static volatile SafeState safe_state = PIN_ENTRY;
static char entered_pin[4];
static uint8_t entered_count = 0U;
static uint8_t wrong_attempts = 0U;
static uint8_t seconds_left = 0U;
static uint32_t state_started = 0U;
static uint32_t flash_started = 0U;
static uint8_t short_flash = 0U;
static uint8_t lockout_led_on = 0U;
static uint8_t keypad_ready = 0U;
static uint8_t release_tracking = 0U;
static uint32_t release_started = 0U;
static uint32_t keypad_started = 0U;

//Two pictures of the screen: what we want and what we last sent
//Only one changed character is sent per loop, keeping timer polling frequent
static char desired_screen[2][17];
static char shown_screen[2][16];
static uint8_t display_dirty = 1U;
static uint32_t display_retry_at = 0U;
static uint8_t display_retry_wait = 0U;

static void Clock_Init(void);
static void Timebase_Init(void);
static void Delay_Ms(uint32_t milliseconds);
static void I2C1_Init(void);
static void I2C1_Reset(void);
static int32_t I2C1_Write(void *context, uint8_t address,
                         const uint8_t *data, size_t length);
static void LCD_Delay(void *context, uint32_t milliseconds);
static int32_t LCD_Start(I2C_LCD_HandleTypeDef *lcd);

static void Clock_Init(void)
{
    uint32_t remaining = CLOCK_STARTUP_LIMIT;
    RCC_CR |= (1U << 0);                  //enable HSI internal oscillator
    while ((RCC_CR & (1U << 1)) == 0U) {  //loops until HSI becomes ready (bit 1 = status of HSI)
        if (--remaining == 0U) {
            Error_Handler();
        }
    }
    //RCC_CFGR is RCC Clock config register where
    //bits[1:0] represent system clock switch field
    //00 - HSI, internal oscillator
    //01 - HSE, external oscillator
    //10 - PLL, phase locked loop

    RCC_CFGR &= ~(3U << 0); //select HSI
    remaining = CLOCK_STARTUP_LIMIT;

    //loop until its verified that HSI has been actually switched to
    //on hardware level
    while ((RCC_CFGR & (3U << 2)) != 0U) { //bits [3:2], SWS field, informs which clock source is being used
        if (--remaining == 0U) {
            Error_Handler();
        }
    }

    //HPRE=[7:4], AHB prescaler
    //PPRE1=[12:10], APB1 prescaler
    //PPRE2=[15:13], APB2 clock prescaler
    //reseting them to 0 sets the prescalers' division factor to 1
    //meaning 16Mhz clock is supplied to all of those buses
    RCC_CFGR &= ~((15U << 4));
    RCC_CFGR &= ~((7U << 10));
    RCC_CFGR &= ~((7U << 13));

    SystemCoreClock = 16000000U;
    //No HAL tick or timer ISR is needed for this display stage.
    SysTick->CTRL = 0U;
}

//Configure Tim2 as a continuously running 32bit hardware timer
//with a resolution of 1*10^-6 sec
static void Timebase_Init(void)
{
    RCC_APB1ENR |= (1U << 0);             //enable TIM2 clock
    (void)RCC_APB1ENR;                    //clock-enable readback
    RCC_APB1RSTR |= (1U << 0);		//bit0=tim2rst, assert tim2 reset
    RCC_APB1RSTR &= ~(1U << 0);		//release tim2 reset
    TIM2_CR1 = 0U;					//TIM2 control register 1, config timer basic continuous up-couning
    								//+ disable counter for now(bit0=0, counter disabled)
    TIM2_DIER = 0U;                 //DIER=dma/interrupt enable register, if = 0 then those are disabled
    TIM2_PSC = 15U;                 //prescaler register, 16 MHz / 16 = 1 MHz
    TIM2_ARR = UINT32_MAX;          //configure max counter value. This makes free-running 32-bit microseconds
    TIM2_EGR |= (1U << 0);          //EGR=event generation register where bit 0 is update generation and ensures the tim2 prescalers takes effect
    								//by generating a software interupt. Basically load the buffered prescaler
    TIM2_SR = 0U;                   //Clear Update Interrupt flag caused by UG
    TIM2_CNT = 0U;					//set starting value of counter
    TIM2_CR1 |= (1U << 0);          //enable counter
}

static void Delay_Ms(uint32_t milliseconds)
{
    while (milliseconds-- != 0U) {
        uint32_t start = TIM2_CNT;
        //Unsigned subtraction also works across counter wraparound.
        while ((uint32_t)(TIM2_CNT - start) < 1000U) {
        }
    }
}

static void I2C1_Reset(void)
{
    RCC_APB1RSTR |= (1U << 21);           //Reset I2C1, including stale flags
    RCC_APB1RSTR &= ~(1U << 21);
    I2C1_CR1 = 0U;                       //PE=0; analog filter on, DNF=0
    I2C1_TIMINGR = I2C1_TIMING_100KHZ;
    I2C1_ICR = I2C_CLEAR_FLAGS;
    I2C1_CR1 = 1U;                       //PE=1
}

static void I2C1_Init(void)
{
    RCC_AHB1ENR |= (1U << 1);            //GPIOB clock
    RCC_APB1ENR |= (1U << 21);           //I2C1 clock

    //creates a short hardware delay by reading the values of
    //registers. This delay is necessary because it takes a little bit of
    //time for the peripheral clock propagate to the hardware.
    //For example not doing this, and instead trying to set some peripheral register
    //right after could cause the change not to get posted and lead to unpredictable behavior.
    (void)RCC_AHB1ENR;
    (void)RCC_APB1ENR;

    I2C1_CR1 &= ~1U; //disabled I2C peripheral 1 during the configuration process

    //selects HSI as i2c1 clock
    RCC_DCKCFGR2 &=  ~(3U << 16);
    RCC_DCKCFGR2 |=  (2U << 16);

    //Set properties of PB8 & 9 before connecting AF4 to the pads
    GPIOB_OTYPER |= (1U << 8) | (1U << 9); //set PB8 & PB9 pins to open drain

    //clear speed reg to ensure i2c pins are configured in known state
    GPIOB_OSPEEDR &= ~((3U << 16) | (3U << 18));
    GPIOB_OSPEEDR |= ((1U << 16) | (1U << 18)); //set the speed to medium

    GPIOB_PUPDR &= ~(3U << 16); //[17:16]-PB8 - no internal pull up or down R
    GPIOB_PUPDR &= ~(3U << 18); //[19:18]-PB9 - no internal pull up or down R
    GPIOB_AFRH = (GPIOB_AFRH & ~0xFFU) | 0x44U; //selects AF4 for alt function mode for both pb8&9

    //rest and set alt function for PB8 & PB9
    GPIOB_MODER &= ~((3U << 16) | (3U << 18));
    GPIOB_MODER |= ((2U << 16) | (2U << 18));

    I2C1_Reset();
}

static int32_t I2C1_Fail(I2C_Error error, uint32_t status)
{
    g_i2c_error = error;
    g_i2c_last_error = error;
    g_i2c_last_isr = status;
    g_i2c_last_address = current_i2c_address;
    g_i2c_last_length = current_i2c_length;
    return I2C_LCD_ERR_IO;
}

static int32_t I2C1_Wait(uint32_t flag, uint32_t start, uint32_t timeout)
{
    for (;;) {
        uint32_t status = I2C1_ISR;
        //Check errors first: NACK and STOP can arrive together.
        if ((status & I2C_ERROR_FLAGS) != 0U) {
            I2C_Error error = I2C_ERROR_BUS;
            if ((status & I2C_ISR_ARLO) != 0U) {
                error = I2C_ERROR_ARBITRATION;
            } else if ((status & I2C_ISR_NACKF) != 0U) {
                error = I2C_ERROR_NACK;
            } else if ((status & I2C_ISR_OVR) != 0U) {
                error = I2C_ERROR_OVERRUN;
            }
            return I2C1_Fail(error, status);
        }
        if ((status & flag) != 0U) {
            return I2C_LCD_OK;
        }
        if ((status & I2C_ISR_STOPF) != 0U) {
            return I2C1_Fail(I2C_ERROR_EARLY_STOP, status);
        }
        if ((uint32_t)(TIM2_CNT - start) >= timeout) {
            return I2C1_Fail(I2C_ERROR_TIMEOUT, status);
        }
    }
}

static void I2C1_Abort(void)
{
    uint32_t status = I2C1_ISR;
    //After arbitration loss we no longer own the bus: do not send STOP.
    //A received NACK already causes the hardware to generate STOP.
    if (((status & I2C_ISR_BUSY) != 0U) &&
        ((status & (I2C_ISR_ARLO | I2C_ISR_NACKF)) == 0U)) {
        I2C1_CR2 |= I2C_CR2_STOP;
    }
    uint32_t start = TIM2_CNT;
    while ((I2C1_ISR & I2C_ISR_BUSY) != 0U) {
        if ((uint32_t)(TIM2_CNT - start) >= 2000U) {
            break;
        }
    }
    I2C1_Reset();
}

static int32_t I2C1_Write(void *context, uint8_t address,
                         const uint8_t *data, size_t length)
{
    (void)context;
    //The LCD driver writes one expander byte at a time. Also support up to
    //255 bytes; zero bytes intentionally performs an address-only probe.
    if ((address < 0x08U) || (address > 0x77U) || (length > 255U) ||
        ((length != 0U) && (data == NULL))) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    current_i2c_address = address;
    current_i2c_length = (uint32_t)length;
    g_i2c_error = I2C_ERROR_NONE;
    uint32_t start = TIM2_CNT;
    while ((I2C1_ISR & I2C_ISR_BUSY) != 0U) {
        if ((uint32_t)(TIM2_CNT - start) >= I2C_TIMEOUT_US) {
            //No START was sent; do not force STOP onto someone else's bus.
            I2C1_Fail(I2C_ERROR_TIMEOUT, I2C1_ISR);
            I2C1_Reset();
            return I2C_LCD_ERR_IO;
        }
    }

    I2C1_ICR = I2C_CLEAR_FLAGS;           //W1C flags, never read-modify-write
    start = TIM2_CNT;
    uint32_t timeout = I2C_TIMEOUT_US + (uint32_t)length * 1000U;
    //SADD[7:1] holds the 7-bit address; RD_WRN=0 means write.
//NBYTES[23:16], AUTOEND[25], START[13]. No reload or 10-bit addressing.
    I2C1_CR2 = ((uint32_t)address << 1) | ((uint32_t)length << 16) |
               I2C_CR2_AUTOEND | I2C_CR2_START;

    for (size_t index = 0U; index < length; ++index) {
        if (I2C1_Wait(I2C_ISR_TXIS, start, timeout) != I2C_LCD_OK) {
            I2C1_Abort();
            return I2C_LCD_ERR_IO;
        }
        I2C1_TXDR = data[index];
    }
    if (I2C1_Wait(I2C_ISR_STOPF, start, timeout) != I2C_LCD_OK) {
        I2C1_Abort();
        return I2C_LCD_ERR_IO;
    }
    I2C1_ICR = I2C_ICR_STOPCF;
    I2C1_CR2 = 0U;
    return I2C_LCD_OK;
}

static void LCD_Delay(void *context, uint32_t milliseconds)
{
    (void)context;
    Delay_Ms(milliseconds);
}

static int32_t LCD_Start(I2C_LCD_HandleTypeDef *lcd)
{
    //A0-A2 jumpers can select any of these 16 backpack addresses.
    //Probe without sending data so scanning does not change expander outputs.
    const uint8_t candidates[] = {
        0x27U, 0x3FU, 0x20U, 0x21U, 0x22U, 0x23U, 0x24U, 0x25U,
        0x26U, 0x38U, 0x39U, 0x3AU, 0x3BU, 0x3CU, 0x3DU, 0x3EU
    };
    uint8_t address = LCD_ADDRESS_7BIT;
    g_lcd_address = 0U;
    g_lcd_detected_address = 0U;
    g_lcd_scan_acks = 0U;
    g_lcd_scan_complete = 0U;
    //Allow the external LCD/backpack supply to settle before probing.
    Delay_Ms(50U);

    if (address == 0U) {
        for (size_t index = 0U; index < sizeof(candidates); ++index) {
            if (I2C1_Write(NULL, candidates[index], NULL, 0U) == I2C_LCD_OK) {
                uint8_t found = candidates[index];
                uint32_t bit = found < 0x30U ? (uint32_t)found - 0x20U :
                                               (uint32_t)found - 0x38U + 8U;
                g_lcd_scan_acks |= (1U << bit);
                //Keep the first responder, but finish scanning for diagnosis.
                if (address == 0U) {
                    address = found;
                    g_lcd_detected_address = found;
                }
            } else if (g_i2c_error != I2C_ERROR_NACK) {
                return I2C_LCD_ERR_IO;
            }
        }
        g_lcd_scan_complete = 1U;
        if (address == 0U) {
            return I2C_LCD_ERR_IO;
        }
    }

    I2C_LCD_Config config = {
        .i2c_address_7bit = address,
        .row_count = 2U,
        .col_count = 16U,
        .backlight_on = 1U,
        .user_context = NULL,
        .write_fn = I2C1_Write,
        .delay_ms_fn = LCD_Delay
    };
    int32_t result = I2C_LCD_Init(lcd, &config);
    if (result != I2C_LCD_OK) {
        return result;
    }
    g_lcd_address = address;

    if ((I2C_LCD_SetCursor(lcd, 0U, 0U) != I2C_LCD_OK) ||
        (I2C_LCD_WriteString(lcd, "Lab 5 I2C LCD") != I2C_LCD_OK) ||
        (I2C_LCD_SetCursor(lcd, 1U, 0U) != I2C_LCD_OK) ||
        (I2C_LCD_WriteString(lcd, "Display ready!") != I2C_LCD_OK)) {
        return I2C_LCD_ERR_IO;
    }
    return I2C_LCD_OK;
}

void Error_Handler(void)
{
    //Clock startup failure; inspect RCC_CR/RCC_CFGR with the debugger.
    g_lcd_status = -3;
    for (;;) {
    }
}

//keypad functions
static void Pin_InputPullup(uint32_t base, uint8_t pin)
{
    //Each MODER/PUPDR pin takes 2 bits. MODER=00 input, PUPDR=01 pull-up.
    uint32_t shift = (uint32_t)pin * 2U;
    GPIO_MODE(base) &= ~(3U << shift);
    GPIO_PULL(base) = (GPIO_PULL(base) & ~(3U << shift)) | (1U << shift);
}

static void Pin_OutputLow(uint32_t base, uint8_t pin)
{
    uint32_t shift = (uint32_t)pin * 2U;
    //Preload LOW before turning on the output so we never briefly drive HIGH.
    //BSRR upper 16 bits reset pins; lower 16 bits set pins.
    GPIO_SET_RESET(base) = (1U << ((uint32_t)pin + 16U));
    GPIO_TYPE(base) &= ~(1U << pin);
    GPIO_PULL(base) &= ~(3U << shift);
    GPIO_MODE(base) = (GPIO_MODE(base) & ~(3U << shift)) | (1U << shift);
}

static void Keypad_Settle(void)
{
    //Give the pull-ups and wiring 10us to settle after changing directions.
    //This short electrical settling time is separate from 20ms key debounce.
    uint32_t start = TIM2_CNT;
    while ((uint32_t)(TIM2_CNT - start) < 10U) {
    }
}

static void Keypad_Idle(void)
{
    //Release every column BEFORE making the rows LOW outputs.
    for (uint8_t column = 0U; column < 4U; ++column) {
        Pin_InputPullup(column_port[column], column_pin[column]);
    }
    for (uint8_t row = 0U; row < 4U; ++row) {
        Pin_OutputLow(row_port[row], row_pin[row]);
    }
}

static uint8_t Keypad_LowColumns(void)
{
    uint8_t low = 0U;
    for (uint8_t column = 0U; column < 4U; ++column) {
        if ((GPIO_INPUT(column_port[column]) & (1U << column_pin[column])) == 0U) {
            low |= (uint8_t)(1U << column);
        }
    }
    return low;
}

static char Keypad_FindRow(uint8_t column)
{
    //The lab requires reversing directions to identify the row.
    //Rows become pull-up inputs. Only the triggered column becomes LOW output.
    for (uint8_t row = 0U; row < 4U; ++row) {
        Pin_InputPullup(row_port[row], row_pin[row]);
    }
    Pin_OutputLow(column_port[column], column_pin[column]);
    Keypad_Settle();
    char key = '\0';
    uint8_t matches = 0U;
    for (uint8_t row = 0U; row < 4U; ++row) {
        if ((GPIO_INPUT(row_port[row]) & (1U << row_pin[row])) == 0U) {
            key = key_map[row][column];
            ++matches;
        }
    }
    Keypad_Idle();
    Keypad_Settle();
    //Reject ambiguous presses instead of guessing a PIN character.
    return matches == 1U ? key : '\0';
}

static void Keypad_Interrupt(uint8_t column)
{
    uint32_t line = (1U << column);
    if ((EXTI_PR & line) == 0U) {
        return;
    }
    if (keypad_armed == 0U) {
        EXTI_PR = line;
        return;
    }
    //Mask all four lines while scanning and while the key is held.
    //This stops contact bounce and direction changes from flooding the CPU.
    EXTI_IMR &= ~0x0FU;
    keypad_armed = 0U;
    uint8_t low = Keypad_LowColumns();
    char key = '\0';
    if (low == (uint8_t)line) {
        key = Keypad_FindRow(column);
    }
    captured_key = key;
    captured_column = column;
    captured_at = TIM2_CNT;
    captured_during_entry = (safe_state == PIN_ENTRY) ? 1U : 0U;
    key_waiting = 1U;
    //EXTI pending bits clear by WRITING 1. Never use &= to clear them.
    //GPIO directions are back to idle before we acknowledge these events.
    EXTI_PR = 0x0FU;
    __DSB();
}

//The startup vector table looks for these exact function names.
void EXTI0_IRQHandler(void) { Keypad_Interrupt(0U); }
void EXTI1_IRQHandler(void) { Keypad_Interrupt(1U); }
void EXTI2_IRQHandler(void) { Keypad_Interrupt(2U); }
void EXTI3_IRQHandler(void) { Keypad_Interrupt(3U); }

static void Keypad_Init(void)
{
    //GPIOA bit0, GPIOB bit1, GPIOC bit2, GPIOG bit6, GPIOI bit8.
    RCC_AHB1ENR |= (1U << 0) | (1U << 1) | (1U << 2) | (1U << 6) | (1U << 8);
    RCC_APB2ENR |= (1U << 14); //SYSCFG clock is on APB2, bit14.
    (void)RCC_AHB1ENR;
    (void)RCC_APB2ENR;
    EXTI_IMR &= ~0x0FU;
    Keypad_Idle();
    //Each EXTI route takes 4 bits. Port A=0, port I=8.
    //EXTI0=PA0, EXTI1=PI1, EXTI2=PI2, EXTI3=PI3 -> low 16 bits=0x8880.
    SYSCFG_EXTICR1 = (SYSCFG_EXTICR1 & ~0xFFFFU) | 0x8880U;
    EXTI_RTSR &= ~0x0FU;
    EXTI_FTSR |= 0x0FU; //A pressed key pulls a column from HIGH to LOW.
    EXTI_PR = 0x0FU;
    //NVIC interrupts 6,7,8,9 correspond to EXTI0,1,2,3.
    //CMSIS gives the NVIC register layout; we still set the registers ourselves.
    NVIC->ICPR[0] = (0x0FU << 6);
    NVIC->ISER[0] = (0x0FU << 6);
    keypad_started = TIM2_CNT;
    //Main waits for a stable release before unmasking EXTI.
    //A key held at reset must be released before we accept input.
}

static void Countdown_Init(void)
{
    RCC_APB1ENR |= (1U << 4); //TIM6 clock, bit4.
    (void)RCC_APB1ENR;
    RCC_APB1RSTR |= (1U << 4);
    RCC_APB1RSTR &= ~(1U << 4);
    TIM6_CR1 = 0U;
    TIM6_DIER = 0U; //No timer interrupt. We poll UIF in the main loop.
    TIM6_PSC = 15999U; //16MHz / (15999+1) = 1000 ticks per second.
    TIM6_ARR = 999U; //Counts 0 through 999, so exactly 1000 ticks nominally.
    TIM6_EGR = 1U; //Load PSC now instead of waiting for the first overflow.
    TIM6_SR = 0U;
}

static void LED_Init(void)
{
    //GPIOB's clock was enabled by I2C/keypad setup before this function.
    //Preload PB4 LOW, then set output mode. This starts the LED OFF.
    Pin_OutputLow(GPIOB_BASE, 4U);
}

static void LED_Process(uint32_t now)
{
    //Original lab: a short beep per key. Here: one 50ms LED flash per key.
    //Checking elapsed time lets the main loop keep handling other work.
    if ((short_flash != 0U) && ((uint32_t)(now - flash_started) >= KEY_FLASH_US)) {
        short_flash = 0U;
    }
    uint8_t on = short_flash;
    if (safe_state == ACCESS_GRANTED) {
        //Original two-second continuous tone becomes a two-second steady light.
        //App_Process() ends this state when its two-second deadline is reached.
        on = 1U;
    } else if (safe_state == LOCKED_OUT) {
        //Original alternating-second siren becomes 1s ON, 1s OFF blinking.
        //TIM6 polling changes lockout_led_on once per elapsed second.
        on = lockout_led_on;
    }
    //BSRR bit4 sets PB4 HIGH; bit20 (4+16) resets PB4 LOW.
    GPIO_SET_RESET(GPIOB_BASE) = on ? (1U << 4) : (1U << 20);
}

static void Entry_Reset(void)
{
    entered_count = 0U;
    for (uint8_t i = 0U; i < 4U; ++i) {
        entered_pin[i] = '\0';
    }
    safe_state = PIN_ENTRY;
    short_flash = 0U;
    display_dirty = 1U;
}

static void App_Key(char key, uint32_t now)
{
    if (safe_state != PIN_ENTRY) {
        return;
    }
    //Every key counts, including A-D, * and #. The lab says any four keys.
    //A fixed four-byte buffer needs no string terminator because we compare bytes.
    entered_pin[entered_count] = key;
    ++entered_count;
    short_flash = 1U;
    flash_started = now;
    display_dirty = 1U;
    if (entered_count < 4U) {
        return;
    }
    uint8_t correct = 1U;
    for (uint8_t i = 0U; i < 4U; ++i) {
        if (entered_pin[i] != SECRET_PIN[i]) {
            correct = 0U;
        }
    }
    state_started = now;
    if (correct != 0U) {
        wrong_attempts = 0U;
        safe_state = ACCESS_GRANTED;
    } else {
        ++wrong_attempts;
        if (wrong_attempts >= 3U) {
            safe_state = LOCKED_OUT;
            seconds_left = LOCKOUT_SECONDS;
            lockout_led_on = 1U;
            //Restart TIM6 at lockout entry so the first second is a full second.
            TIM6_CR1 = 0U;
            TIM6_CNT = 0U;
            TIM6_EGR = 1U;
            TIM6_SR = 0U;
            TIM6_CR1 = 1U;
        } else {
            safe_state = ACCESS_DENIED;
        }
    }
}

static void App_Process(uint32_t now)
{
    //TIM6 runs in hardware. UIF is a flag we check, not an interrupt callback.
    if ((TIM6_SR & 1U) != 0U) {
        TIM6_SR = 0U;
        if ((safe_state == LOCKED_OUT) && (seconds_left > 0U)) {
            --seconds_left;
            lockout_led_on ^= 1U;
            display_dirty = 1U;
            if (seconds_left == 0U) {
                TIM6_CR1 = 0U;
                wrong_attempts = 0U;
                lockout_led_on = 0U;
                Entry_Reset();
            }
        }
    }
    if ((safe_state == ACCESS_GRANTED) &&
        ((uint32_t)(now - state_started) >= 2000000U)) {
        Entry_Reset();
    } else if ((safe_state == ACCESS_DENIED) &&
               ((uint32_t)(now - state_started) >= 800000U)) {
        Entry_Reset();
    }
}

static void Keypad_Process(uint32_t now)
{
    uint8_t low = Keypad_LowColumns();
    g_columns_low = low; //Bit0=C1, bit1=C2, bit2=C3, bit3=C4.
    //An interrupt may have happened after main sampled now. Refresh the time
    //after noticing the event so subtraction cannot use a pre-interrupt timestamp.
    if (key_waiting != 0U) {
        now = TIM2_CNT;
    }
    if ((key_waiting != 0U) &&
        ((uint32_t)(now - captured_at) >= DEBOUNCE_US)) {
        key_waiting = 0U;
        //Check that the same key survived debounce. Interrupts remain masked.
        //Decode again to reject a changed row or ambiguous two-key press.
        if ((captured_key != '\0') &&
            (low == (uint8_t)(1U << captured_column)) &&
            (Keypad_FindRow(captured_column) == captured_key)) {
            g_last_key = captured_key;
            ++g_key_events;
            if (captured_during_entry != 0U) {
                App_Key(captured_key, now);
            }
        }
    }
    if (keypad_armed != 0U) {
        return;
    }
    //Do not rearm until ALL keys have been released for 30ms.
    //Holding a key therefore produces one input, not repeated characters.
    if (low != 0U) {
        release_tracking = 0U;
        return;
    }
    if (release_tracking == 0U) {
        release_tracking = 1U;
        release_started = now;
    }
    if ((key_waiting == 0U) && ((uint32_t)(now - release_started) >= RELEASE_US)) {
        if (keypad_ready == 0U) {
            keypad_ready = 1U;
            display_dirty = 1U;
        }
        EXTI_PR = 0x0FU;
        NVIC->ICPR[0] = (0x0FU << 6);
        keypad_armed = 1U;
        EXTI_IMR |= 0x0FU;
        release_tracking = 0U;
    }
}

static void Screen_Text(uint8_t row, const char *text)
{
    //Always pad to 16 characters so shorter text erases leftover characters.
    uint8_t column = 0U;
    while ((column < 16U) && (*text != '\0')) {
        desired_screen[row][column++] = *text++;
    }
    while (column < 16U) {
        desired_screen[row][column++] = ' ';
    }
    desired_screen[row][16] = '\0';
}

static void Screen_Prepare(uint32_t now)
{
    //A column stuck LOW at startup can be a held key, a wiring error, or D13's LED.
    //The program cannot disconnect SB8. Check the circuit if bit1 stays LOW.
    if ((keypad_ready == 0U) &&
        ((uint32_t)(now - keypad_started) >= 500000U)) {
        Screen_Text(0U, "Release all keys");
        Screen_Text(1U, (g_columns_low & 2U) ? "Check D13 / SB8" : "Check keypad");
        return;
    }
    if (safe_state == PIN_ENTRY) {
        Screen_Text(0U, "Enter PIN:");
        Screen_Text(1U, "");
        for (uint8_t i = 0U; i < entered_count; ++i) {
            desired_screen[1][i] = '*';
        }
    } else if (safe_state == ACCESS_GRANTED) {
        Screen_Text(0U, "Access Granted!");
        Screen_Text(1U, "");
    } else if (safe_state == ACCESS_DENIED) {
        Screen_Text(0U, "Incorrect PIN");
        Screen_Text(1U, "Attempts: 0/3");
        desired_screen[1][10] = (char)('0' + wrong_attempts);
    } else {
        Screen_Text(0U, "LOCKED!");
        Screen_Text(1U, "Time: 00");
        desired_screen[1][6] = (char)('0' + seconds_left / 10U);
        desired_screen[1][7] = (char)('0' + seconds_left % 10U);
    }
}

static void Screen_Process(I2C_LCD_HandleTypeDef *lcd, uint32_t now)
{
    //Prepare text in RAM quickly. Sending it to the LCD is the slower part.
    if ((display_dirty != 0U) || (keypad_ready == 0U)) {
        Screen_Prepare(now);
        display_dirty = 0U;
    }
    if (display_retry_wait != 0U) {
        if ((uint32_t)(now - display_retry_at) < 100000U) {
            return;
        }
        display_retry_wait = 0U;
    }
    for (uint8_t row = 0U; row < 2U; ++row) {
        for (uint8_t column = 0U; column < 16U; ++column) {
            if (desired_screen[row][column] != shown_screen[row][column]) {
                //One cursor command and one character, about 8ms plus I2C time.
                //Return to the main loop after this so deadlines get checked.
                if ((I2C_LCD_SetCursor(lcd, row, column) == I2C_LCD_OK) &&
                    (I2C_LCD_WriteChar(lcd, desired_screen[row][column]) == I2C_LCD_OK)) {
                    shown_screen[row][column] = desired_screen[row][column];
                    g_lcd_status = I2C_LCD_OK;
                } else {
                    g_lcd_status = I2C_LCD_ERR_IO;
                    display_retry_at = TIM2_CNT;
                    display_retry_wait = 1U;
                }
                return;
            }
        }
    }
}

int main(void)
{
    I2C_LCD_HandleTypeDef lcd = {0};
    g_boot_stage = 1U;
    Clock_Init();
    g_boot_stage = 2U;
    Timebase_Init(); //TIM2 gives microseconds for debounce and short deadlines.
    g_boot_stage = 3U;
    I2C1_Init();
    g_boot_stage = 4U;
    for (;;) {
        ++g_lcd_attempts;
        g_lcd_status = LCD_Start(&lcd);
        if (g_lcd_status == I2C_LCD_OK) {
            g_boot_stage = 5U;
            break;
        }
        Delay_Ms(1000U); //Only startup waits here; the application has not begun.
        I2C1_Reset();
    }
    g_boot_stage = 6U;
    Countdown_Init(); //TIM6 supplies the one-second countdown flag.
    g_boot_stage = 7U;
    Keypad_Init();
    LED_Init();
    Entry_Reset();
    g_boot_stage = 8U;
    for (;;) {
        uint32_t now = TIM2_CNT;
        App_Process(now); //Check countdown and success/failure deadlines first.
        Keypad_Process(now); //Debounce the event captured by EXTI, then accept it.
        LED_Process(now); //Update the replacement LED without a long delay.
        Screen_Process(&lcd, now); //Send one changed character, then start over.
    }
}
