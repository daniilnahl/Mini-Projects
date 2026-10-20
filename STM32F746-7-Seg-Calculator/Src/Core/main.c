/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "stm32f7xx.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN PD */
#define NUM1 42U        //1st two digit var
#define NUM2 17U        //2nd two digit var

//blank digit and minus sign patterns
#define BLANK 0x00U
#define MINUS 0x40U

//one bit for each switch so we can check which one was pressed
#define SW_ADD (1U << 0)
#define SW_SUB (1U << 1)
#define SW_MEM (1U << 2)
#define SW_EQ  (1U << 3)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* D2-D8 = A-G: PG6, PB4, PG7, PI0, PH6, PI3, PI2 */
static GPIO_TypeDef *const segment_port[7] =
    {GPIOG, GPIOB, GPIOG, GPIOI, GPIOH, GPIOI, GPIOI};
static const uint8_t segment_pin[7] = {6, 4, 7, 0, 6, 3, 2};

/* D10-D13 = Digit 1-4: PA8, PB15, PB14, PI1 */
static GPIO_TypeDef *const digit_port[4] =
    {GPIOA, GPIOB, GPIOB, GPIOI};
static const uint8_t digit_pin[4] = {8, 15, 14, 1};

/* A0-A3: PA0, PF10, PF9, PF8 */
static GPIO_TypeDef *const button_port[4] =
    {GPIOA, GPIOF, GPIOF, GPIOF};
static const uint8_t button_pin[4] = {0, 10, 9, 8};

/* Bit 0=A to 6=G*/
static const uint8_t font[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

//patterns currently shown on the four digits
static volatile uint8_t display[4] = {BLANK, BLANK, BLANK, BLANK};

//counts ms from systick that is used for button timing
static volatile uint32_t milliseconds = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/

/* USER CODE BEGIN 0 */

/*write HIGH or LOW to a pin using BSRR instead of modifying ODR. */
static void pin_write(GPIO_TypeDef *port, uint8_t pin, uint8_t high)
{
    port->BSRR = high ? (1UL << pin) : (1UL << (pin + 16U));
}

//setup a pin as a GPIO output
static void output_init(GPIO_TypeDef *port, uint8_t pin, uint8_t high)
{
    uint32_t shift = 2U * pin;
    pin_write(port, pin, high);                   //Safe initial state
    port->OTYPER &= ~(1UL << pin);               //Push-pull
    port->OSPEEDR &= ~(3UL << shift);             // Low speed
    port->PUPDR &= ~(3UL << shift);               // No pull resistor
    port->MODER = (port->MODER & ~(3UL << shift)) | (1UL << shift);
}

//switches go to ground, so pull-ups keep inputs HIGH when not pressed
static void input_pullup_init(GPIO_TypeDef *port, uint8_t pin)
{
    uint32_t shift = 2U * pin;
    port->MODER &= ~(3UL << shift);               // Input mode (00)
    port->PUPDR = (port->PUPDR & ~(3UL << shift)) | (1UL << shift);
}

//init all segment, digit-select, and switch GPIOs
static void gpio_init(void)
{
    /* Clocks for ports A, B, F, G, H, I (AHB1ENR bits 0,1,5,6,7,8). */
    RCC->AHB1ENR |= (1UL << 0) | (1UL << 1) | (1UL << 5) |
                    (1UL << 6) | (1UL << 7) | (1UL << 8);
    (void)RCC->AHB1ENR;                           //read back after enabling

    for (uint8_t i = 0; i < 7; ++i)
        output_init(segment_port[i], segment_pin[i], 1); //segments OFF = HIGH

    for (uint8_t i = 0; i < 4; ++i)
        output_init(digit_port[i], digit_pin[i], 0);     //digits OFF = LOW

    //set up four switch inputs
    for (uint8_t i = 0; i < 4; ++i)
        input_pullup_init(button_port[i], button_pin[i]);
}

/*fefresh one digit every 1 ms so all four appear constantly ON. */
static void refresh_display(void)
{
    static uint8_t pos = 0;
    for (uint8_t i = 0; i < 4; ++i)
        pin_write(digit_port[i], digit_pin[i], 0);// ALL anodes OFF

    //sse the saved seg pattern for this #
    uint8_t pattern = display[pos];
    for (uint8_t i = 0; i < 7; ++i)
        pin_write(segment_port[i], segment_pin[i],
                  (pattern & (1U << i)) ? 0 : 1);        //seg active LOW

    pin_write(digit_port[pos], digit_pin[pos], 1);      //selected anode ON
    //go to next digit (wrap around after digit 4)
    pos = (uint8_t)((pos + 1U) % 4U);
}

//run once per ms and keeps the display refreshing
void SysTick_Handler(void)
{
    ++milliseconds;
    refresh_display();
}

/* Show NUM1 and NUM2 together: e.g. 42 and 17 -> "4217". */
static void show_operands(void)
{
    //split both variables into tens and ones digits
    display[0] = font[NUM1 / 10U];
    display[1] = font[NUM1 % 10U];
    display[2] = font[NUM2 / 10U];
    display[3] = font[NUM2 % 10U];
}

/*display an integer, with a minus sign if needed and no leading zeros. */
static void show_number(int32_t value)
{
    //4 digits cannot display larger values so use dashes for overflow
    if (value < -999 || value > 9999) {
        for (uint8_t i = 0; i < 4; ++i) display[i] = MINUS;
        return;
    }

    //work with the positive value first then add minus if needed
    uint8_t negative = (value < 0);
    uint32_t mag = (uint32_t)(negative ? -value : value);
    //clear any digits left over from the previous number
    for (uint8_t i = 0; i < 4; ++i) display[i] = BLANK;

    //fill digits from right to left starting with the ones place
    for (int8_t pos = 3; pos >= 0; --pos) {
        display[pos] = font[mag % 10U];
        mag /= 10U;
        if (mag == 0U) {
            if (negative && pos > 0) display[pos - 1] = MINUS;
            break;
        }
    }
}

/*read the switches. Pressed is LOW at the pin, but 1 in the result. */
static uint8_t buttons_raw(void)
{
    uint8_t result = 0;
    for (uint8_t i = 0; i < 4; ++i)
        if ((button_port[i]->IDR & (1UL << button_pin[i])) == 0)
            result |= (uint8_t)(1U << i);
    return result;
}

/*debounce the switches */
static uint8_t buttons_new_presses(void)
{
    static uint8_t last_raw = 0;
    static uint8_t stable = 0;
    static uint8_t count = 0;
    uint8_t raw = buttons_raw();

    //reset the counter whenever the switch inputs change
    if (raw != last_raw) {
        last_raw = raw;
        count = 0;
    } else if (count < 4U) {
        ++count;
    }

    //4 unchanged polls about 20 ms of stable input
    if (count == 4U && raw != stable) {
        //only return newly switched switches
        uint8_t presses = (uint8_t)(raw & (uint8_t)~stable);
        stable = raw;
        return presses;
    }
    return 0;
}

/* USER CODE END 0 */
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();
  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  gpio_init();
  show_operands();

  /* USER CODE END SysInit */

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */
  /* Infinite loop */

  /* USER CODE BEGIN WHILE */

  /*set SysTick to interrupt every 1 ms for display refresh */
  SystemCoreClockUpdate();
  SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
  SysTick->VAL = 0U;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                  SysTick_CTRL_TICKINT_Msk |
                  SysTick_CTRL_ENABLE_Msk;

  uint8_t operation = 0;        // 0=none, 1=add, 2=subtract
  //keep latest result and one older result for memory recall
  uint8_t have_current = 0, have_previous = 0;
  int32_t current = 0, previous = 0;
  uint32_t last_poll = 0;

  while (1) {
      //only check the switches every 5 ms
      uint32_t now = milliseconds;
      if ((uint32_t)(now - last_poll) < 5U) continue;
      last_poll = now;

      uint8_t pressed = buttons_new_presses();
      //choose an operation first then press equals to calculate
      if (pressed & SW_ADD) {
          operation = 1;
          show_operands();
      } else if (pressed & SW_SUB) {
          operation = 2;
          show_operands();
      } else if ((pressed & SW_EQ) && operation != 0) {
          //save the last ops result before replacing it
          if (have_current) {
              previous = current;
              have_previous = 1;
          }

          //calculate using the two preset vars
          current = (operation == 1) ?
              (int32_t)NUM1 + (int32_t)NUM2 :
              (int32_t)NUM1 - (int32_t)NUM2;
          have_current = 1;
          show_number(current);
      } else if (pressed & SW_MEM) {
          //show prev answer or current if there is no prev one
          if (have_previous) show_number(previous);
          else if (have_current) show_number(current);
      }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
