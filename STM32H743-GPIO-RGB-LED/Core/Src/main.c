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

#define RCC_BASE_ADDR 0x58024400U
#define RCC_AHB4ENR_OFFSET 0x0E0U
#define RCC_AHB4ENR (*(volatile uint32_t *)(RCC_BASE_ADDR + RCC_AHB4ENR_OFFSET))

#define GPIOA_MODER_ADDR 0x58020000U

#define GPIOA_MODER_OFFSET 0x00U
#define GPIOA_OTYPER_OFFSET 0x04U
#define GPIOA_PUPDR_OFFSET 0x0CU
#define GPIOA_ODR_OFFSET 0x14U
#define GPIOA_IDR_OFFSET 0x10U

#define GPIOA_MODER (*(volatile uint32_t *)(GPIOA_MODER_ADDR + GPIOA_MODER_OFFSET))
#define GPIOA_OTYPER (*(volatile uint32_t *)(GPIOA_MODER_ADDR + GPIOA_OTYPER_OFFSET))
#define GPIOA_PUPDR (*(volatile uint32_t *)(GPIOA_MODER_ADDR + GPIOA_PUPDR_OFFSET))
#define GPIOA_ODR (*(volatile uint32_t *)(GPIOA_MODER_ADDR + GPIOA_ODR_OFFSET))
#define GPIOA_IDR (*(volatile uint32_t *)(GPIOA_MODER_ADDR + GPIOA_IDR_OFFSET))

#define OUTPUT_PIN_STATE 0x1U
#define INPUT_PIN_STATE 0x0U
#define CLEAR_PIN_STATE 0x3U
#define PULL_UP_STATE 0x1U
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  //enable GPIOA RCC
  RCC_AHB4ENR |= (1U << 0);

  //clear all 4 pins
  GPIOA_MODER &= ~(CLEAR_PIN_STATE << 0); //PA0
  GPIOA_MODER &= ~(CLEAR_PIN_STATE << 2); //PA1
  GPIOA_MODER &= ~(CLEAR_PIN_STATE << 4); //PA2
  GPIOA_MODER &= ~(CLEAR_PIN_STATE << 6); //PA3

  //set pins A0-2 to be OUTPUT
  GPIOA_MODER |= (OUTPUT_PIN_STATE << 0);
  GPIOA_MODER |= (OUTPUT_PIN_STATE << 2);
  GPIOA_MODER |= (OUTPUT_PIN_STATE << 4);

  //set pin A3 to be INPUT
  GPIOA_MODER |= (INPUT_PIN_STATE << 6);

  //set pins A0-2 to push-pull
  GPIOA_OTYPER &= ~(1U << 0);
  GPIOA_OTYPER &= ~(1U << 1);
  GPIOA_OTYPER &= ~(1U << 2);

  //set pins A3 to pull-up
  GPIOA_PUPDR &= ~(3U << 6);
  GPIOA_PUPDR |= (PULL_UP_STATE << 6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  const uint32_t colors[3] = {
	  (1U << 0),
	  (1U << 1),
	  (1U << 2),
  };

  volatile uint8_t buttonPressed = 0U;

  while (1) {

	  if ((GPIOA_IDR & (1U << 3)) == 0U) buttonPressed = 1U;
	  else buttonPressed = 0U;

	  if (buttonPressed == 0U){ //button pressed
		  GPIOA_ODR &= ~(1U << 0);
		  GPIOA_ODR &= ~(1U << 1);
		  GPIOA_ODR &= ~(1U << 2);

	  } else { //cycle LED, clear pins and set next color
		  GPIOA_ODR &= ~(1U << 0);
		  GPIOA_ODR &= ~(1U << 1);
		  GPIOA_ODR &= ~(1U << 2);

		  for (uint8_t i = 0; i < 3U; i++){
			  HAL_Delay(1000);
			  GPIOA_ODR |= colors[i];
			  HAL_Delay(1000);
			  GPIOA_ODR &= ~(colors[i]);
		  }
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
