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

/* USER CODE BEGIN 0 */
void display7SEG(int num)
{
  // Tắt tất cả các chân trước
  HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_B_Pin | LED_C_Pin | LED_D_Pin | LED_E_Pin | LED_F_Pin | LED_G_Pin, SET);

  switch (num)
  {
  case 0: // 0s
    HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_B_Pin | LED_C_Pin | LED_D_Pin | LED_E_Pin | LED_F_Pin, RESET);
    break;
  case 1: // 1s
    HAL_GPIO_WritePin(GPIOB, LED_B_Pin | LED_C_Pin, RESET);
    break;
  case 2: // 2s
    HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_B_Pin | LED_D_Pin | LED_E_Pin | LED_G_Pin, RESET);
    break;
  case 3: // 3s
    HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_B_Pin | LED_C_Pin | LED_D_Pin | LED_G_Pin, RESET);
    break;
  case 4: // 4s
    HAL_GPIO_WritePin(GPIOB, LED_B_Pin | LED_C_Pin | LED_F_Pin | LED_G_Pin, RESET);
    break;
  case 5: // 5s
    HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_C_Pin | LED_D_Pin | LED_F_Pin | LED_G_Pin, RESET);
    break;
  case 6: // 6s
    HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_C_Pin | LED_D_Pin | LED_E_Pin | LED_F_Pin | LED_G_Pin, RESET);
    break;
  case 7: // 7s
    HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_B_Pin | LED_C_Pin, RESET);
    break;
  case 8: // 8s
    HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_B_Pin | LED_C_Pin | LED_D_Pin | LED_E_Pin | LED_F_Pin | LED_G_Pin, RESET);
    break;
  case 9: // 9s
    HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_B_Pin | LED_C_Pin | LED_D_Pin | LED_F_Pin | LED_G_Pin, RESET);
    break;
  default:
    // Hiển thị gạch ngang nếu num không hợp lệ
    HAL_GPIO_WritePin(GPIOB, LED_G_Pin, RESET);
    break;
  }
}

// Hàm đếm giây của đèn giao thông
void delayWith7SEG(int counter)
{
  for (int i = 0; i < counter + 1; i++)
  {
    display7SEG(i);
    HAL_Delay(1000);
  }
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

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    // LED RED 1 ON, LED GREEN 2 ON AND OTHERS OFF - 5 giây
    HAL_GPIO_WritePin(GPIOA, LED_RED1_Pin, RESET);
    HAL_GPIO_WritePin(GPIOA, LED_YELLOW1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_RED2_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_YELLOW2_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN2_Pin, RESET);

    // Đếm 3 giây với LED_GREEN 2 7 đoạn
    delayWith7SEG(2);

    // LED RED 1 ON, LED YELLOW 2 ON, OTHERS LIGHT OFF - 2 giây
    HAL_GPIO_WritePin(GPIOA, LED_RED1_Pin, RESET);
    HAL_GPIO_WritePin(GPIOA, LED_YELLOW1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_RED2_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_YELLOW2_Pin, RESET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN2_Pin, SET);

    // Đếm 2 giây với LED_YELLOW 2 (7 đoạn)
    delayWith7SEG(1);

    // LED GREEN 1 ON, LED RED 2 ON, OTHERS LIGHT OFF - 3 giây
    HAL_GPIO_WritePin(GPIOA, LED_RED1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_YELLOW1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN1_Pin, RESET);
    HAL_GPIO_WritePin(GPIOA, LED_RED2_Pin, RESET);
    HAL_GPIO_WritePin(GPIOA, LED_YELLOW2_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN2_Pin, SET);

    // LED YELLOW 1 ON, LED RED 2 ON, OTHERS LIGHT OFF - 2 giây
    HAL_GPIO_WritePin(GPIOA, LED_GREEN1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_RED1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_YELLOW1_Pin, RESET);
    HAL_GPIO_WritePin(GPIOA, LED_RED2_Pin, RESET);
    HAL_GPIO_WritePin(GPIOA, LED_YELLOW2_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED_GREEN2_Pin, SET);

    // Đếm 5 giây với LED_RED 2 (7 đoạn)
    delayWith7SEG(4);
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
   * in the RCC_OscInitStruct structure.
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
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_RED1_Pin | LED_YELLOW1_Pin | LED_GREEN1_Pin | LED_RED2_Pin | LED_YELLOW2_Pin | LED_GREEN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_A_Pin | LED_B_Pin | LED_C_Pin | LED_D_Pin | LED_E_Pin | LED_F_Pin | LED_G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_RED1_Pin LED_YELLOW1_Pin LED_GREEN1_Pin LED_RED2_Pin
                           LED_YELLOW2_Pin LED_GREEN2_Pin */
  GPIO_InitStruct.Pin = LED_RED1_Pin | LED_YELLOW1_Pin | LED_GREEN1_Pin | LED_RED2_Pin | LED_YELLOW2_Pin | LED_GREEN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_A_Pin LED_B_Pin LED_C_Pin LED_D_Pin
                           LED_E_Pin LED_F_Pin LED_G_Pin */
  GPIO_InitStruct.Pin = LED_A_Pin | LED_B_Pin | LED_C_Pin | LED_D_Pin | LED_E_Pin | LED_F_Pin | LED_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
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
