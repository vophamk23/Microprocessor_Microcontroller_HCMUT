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
#include "software_timer.h"
TIM_HandleTypeDef htim2;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/*
 * display7SEG(int num)
 */
void display7SEG(int num)
{
  // Turn ALL segments off first (set = off for active-low segments).
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_SET);

  switch (num)
  {
  case 0: // Display '0' -> segments a b c d e f ON (g OFF)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin, GPIO_PIN_RESET);
    break;
  case 1: // Display '1' -> segments b c ON
    HAL_GPIO_WritePin(GPIOB, SEG1_Pin | SEG2_Pin, GPIO_PIN_RESET);
    break;
  case 2: // Display '2' -> a b g e d ON
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG3_Pin | SEG4_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 3: // Display '3' -> a b c d g ON
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 4: // Display '4' -> f g b c ON (note mapping depends on your SEG pins)
    HAL_GPIO_WritePin(GPIOB, SEG1_Pin | SEG2_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 5: // Display '5' -> a f g c d ON
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG2_Pin | SEG3_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 6: // Display '6' -> a f e d c g ON
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 7: // Display '7' -> a b c ON
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin, GPIO_PIN_RESET);
    break;
  case 8: // Display '8' -> all segments ON
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 9: // Display '9' -> a b c d f g ON
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  default:
    // If the input is invalid, show a '-' by lighting the middle segment (g).
    HAL_GPIO_WritePin(GPIOB, SEG6_Pin, GPIO_PIN_RESET);
    break;
  }
}

/**
 * enableLED(int index)
 * - Enables one of two 7-segment digits by controlling enable pins (EN0, EN1).
 * - NOTE: The code assumes the enable pins are active-LOW:
 *   ENx = 0 -> enable that digit; ENx = 1 -> disable that digit.
 */
void enableLED(int index)
{
  if (index == 0)
  {
    HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_RESET); // Enable digit 0
    HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_SET);   // Disable digit 1
  }
  else if (index == 1)
  {
    HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_SET);   // Disable digit 0
    HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET); // Enable digit 1
  }
}


/* Currently active LED index (0 or 1).
 * Because this variable is accessed in an interrupt handler and (possibly) in main,
 * it should be declared volatile to prevent compiler optimizations.
 */
volatile int currentLED = 0;


/**
 * HAL_TIM_PeriodElapsedCallback - every 10ms)
 * - This is the timer interrupt callback executed when TIM2 period elapses.
 * - You call timerRun() here to advance your software timers
 * - When timer 0 expires, you toggle which 7-seg digit shows and toggle a debug LED.
 *
 * IMPORTANT: Keep ISR code short. Calling many HAL functions in an ISR is okay for simple projects,
 * but for more robust/faster systems consider setting flags here and doing the heavy work in main().
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    // Call software timer tick (your timer implementation likely expects this every 10 ms).
    timerRun();

    if (isTimerExpired(0))
    {
      // Reset timer 0 to 500 ms (50 ticks * 10 ms per tick).
      // (Your code comment earlier implies timerRun is a 10ms tick.)
      setTimer(0, 50);

      if (currentLED == 0)
      {
        enableLED(0);
        display7SEG(1);
        currentLED = 1; // Next time, show the other digit
      }
      else
      {
        // Enable the second digit and display '2' there.
        enableLED(1);
        display7SEG(2);
        currentLED = 0; // Next time, show the first digit
      }

      HAL_GPIO_TogglePin(GPIOA, LED_RED_Pin);
    }
  }
}


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();

  // Start TIM2 in interrupt mode (so HAL will call HAL_TIM_PeriodElapsedCallback).
  HAL_TIM_Base_Start_IT(&htim2);

  // Set initial timer for 500ms (50 * 10ms)
  setTimer(0, 50);

  /* Infinite loop */
  while (1)
  {
    /*
    // Old version: handled in while loop instead of interrupt
    if (isTimerExpired(0))
    {
      setTimer(0, 50);

      if (currentLED == 0)
      {
        enableLED(0);
        display7SEG(1);
        currentLED = 1;
      }
      else
      {
        enableLED(1);
        display7SEG(2);
        currentLED = 0;
      }

      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    }
    */
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

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, LED_RED_Pin|EN0_Pin|EN1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_RED_Pin EN0_Pin EN1_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin|EN0_Pin|EN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG0_Pin SEG1_Pin SEG2_Pin SEG3_Pin
                           SEG4_Pin SEG5_Pin SEG6_Pin */
  GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin;
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

#ifdef  USE_FULL_ASSERT
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
