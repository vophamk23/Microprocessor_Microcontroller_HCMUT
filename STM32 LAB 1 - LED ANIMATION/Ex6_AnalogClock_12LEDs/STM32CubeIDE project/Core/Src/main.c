/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
// LED trên PORTA
#define LED0_Pin GPIO_PIN_4
#define LED0_GPIO_Port GPIOA

#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA

#define LED2_Pin GPIO_PIN_6
#define LED2_GPIO_Port GPIOA

#define LED3_Pin GPIO_PIN_7
#define LED3_GPIO_Port GPIOA

#define LED4_Pin GPIO_PIN_8
#define LED4_GPIO_Port GPIOA

#define LED5_Pin GPIO_PIN_9
#define LED5_GPIO_Port GPIOA

#define LED6_Pin GPIO_PIN_10
#define LED6_GPIO_Port GPIOA

#define LED7_Pin GPIO_PIN_11
#define LED7_GPIO_Port GPIOA

#define LED8_Pin GPIO_PIN_12
#define LED8_GPIO_Port GPIOA

#define LED9_Pin GPIO_PIN_13
#define LED9_GPIO_Port GPIOA

#define LED10_Pin GPIO_PIN_14
#define LED10_GPIO_Port GPIOA

#define LED11_Pin GPIO_PIN_15
#define LED11_GPIO_Port GPIOA

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
static void MX_GPIO_Init(void);

// Mảng chứa các chân LED từ LED0 → LED11
uint16_t LED_Pins[12] = {
    LED0_Pin, LED1_Pin, LED2_Pin, LED3_Pin,
    LED4_Pin, LED5_Pin, LED6_Pin, LED7_Pin,
    LED8_Pin, LED9_Pin, LED10_Pin, LED11_Pin};

// Hàm tắt tất cả LED
void TurnOffAllLEDs(void)
{
  for (int i = 0; i < 12; i++)
  {
    HAL_GPIO_WritePin(GPIOA, LED_Pins[i], SET); // Tắt LED
  }
}

// Hàm bật LED theo chỉ số
void TurnOnLED(int index)
{
  HAL_GPIO_WritePin(GPIOA, LED_Pins[index], RESET); // Bật LED
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  TurnOffAllLEDs(); // Đảm bảo tất cả LED tắt khi khởi động

  int currentLED = 0; // LED0 = vị trí số 12 trên đồng hồ

  while (1)
  {
    // Bật LED hiện tại
    HAL_GPIO_WritePin(GPIOA, LED_Pins[currentLED], GPIO_PIN_RESET);
    HAL_Delay(200);
    // Tắt LED sau khi sáng
    HAL_GPIO_WritePin(GPIOA, LED_Pins[currentLED], GPIO_PIN_SET);

    // Chuyển sang LED tiếp theo
    currentLED++;
    if (currentLED >= 12)
    {
      currentLED = 0; // Quay lại LED0 khi hết 12 LED
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED0_Pin | LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin | LED5_Pin | LED6_Pin | LED7_Pin | LED8_Pin | LED9_Pin | LED10_Pin | LED11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED0_Pin LED1_Pin LED2_Pin LED3_Pin
                           LED4_Pin LED5_Pin LED6_Pin LED7_Pin
                           LED8_Pin LED9_Pin LED10_Pin LED11_Pin */
  GPIO_InitStruct.Pin = LED0_Pin | LED1_Pin | LED2_Pin | LED3_Pin | LED4_Pin | LED5_Pin | LED6_Pin | LED7_Pin | LED8_Pin | LED9_Pin | LED10_Pin | LED11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
