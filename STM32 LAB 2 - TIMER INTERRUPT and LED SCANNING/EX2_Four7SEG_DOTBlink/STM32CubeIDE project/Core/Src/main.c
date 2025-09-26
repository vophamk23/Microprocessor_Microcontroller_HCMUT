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
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
/**
 * @brief Hiển thị số trên màn hình 7 đoạn
 * @param num: Số cần hiển thị (0-9)
 */
void display7SEG(int num)
{
  // Tắt tất cả các đoạn LED trước (SET = tắt do logic nghịch đảo)
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_SET);

  switch (num)
  {
  case 0: // Hiển thị số 0 - bật 6 đoạn (a,b,c,d,e,f)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin, GPIO_PIN_RESET);
    break;
  case 1: // Hiển thị số 1 - chỉ bật 2 đoạn (b,c)
    HAL_GPIO_WritePin(GPIOB, SEG1_Pin | SEG2_Pin, GPIO_PIN_RESET);
    break;
  case 2: // Hiển thị số 2 - bật 5 đoạn (a,b,g,e,d)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG3_Pin | SEG4_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 3: // Hiển thị số 3 - bật 5 đoạn (a,b,c,d,g)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 4: // Hiển thị số 4 - bật 4 đoạn (f,g,b,c)
    HAL_GPIO_WritePin(GPIOB, SEG1_Pin | SEG2_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 5: // Hiển thị số 5 - bật 5 đoạn (a,f,g,c,d)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG2_Pin | SEG3_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 6: // Hiển thị số 6 - bật 6 đoạn (a,f,g,e,d,c)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 7: // Hiển thị số 7 - bật 3 đoạn (a,b,c)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin, GPIO_PIN_RESET);
    break;
  case 8: // Hiển thị số 8 - bật tất cả 7 đoạn
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 9: // Hiển thị số 9 - bật 6 đoạn (a,b,c,d,f,g)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  default:
    // Hiển thị gạch ngang (đoạn g) nếu số không hợp lệ
    HAL_GPIO_WritePin(GPIOB, SEG6_Pin, GPIO_PIN_RESET);
    break;
  }
}

/**
 * @brief Bật một LED 7-đoạn cụ thể (trong hệ thống 4 LED)
 * @param index: Vị trí LED cần bật (0, 1, 2, hoặc 3)
 */
void enableLED(int index)
{
  // Tắt tất cả LED trước (SET = tắt do transistor PNP)
  HAL_GPIO_WritePin(GPIOA, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);

  switch (index)
  {
  case 0:
    HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_RESET); // Bật LED đầu tiên
    break;
  case 1:
    HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET); // Bật LED thứ hai
    break;
  case 2:
    HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_RESET); // Bật LED thứ ba
    break;
  case 3:
    HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_RESET); // Bật LED thứ tư
    break;
  default:
    break;
  }
}
/* USER CODE END PFP */

int currentLED = 0;  // Chỉ số LED hiện tại: từ 0 đến 3

/**
 * @brief Hàm callback được gọi khi timer tràn (mỗi 10ms)
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    // Chạy các timer phần mềm mỗi 10ms
    timerRun();

    // -------- Quét 4 LED 7-đoạn mỗi 500ms --------
    if (isTimerExpired(0))
    {
      setTimer(0, 50); // Reset timer 0 (50 x 10ms = 500ms)

      // Chọn LED nào để hiển thị và hiển thị số gì
      switch (currentLED)
      {
      case 0:
        enableLED(0);     // Bật LED đầu tiên
        display7SEG(1);   // Hiển thị số 1
        break;
      case 1:
        enableLED(1);     // Bật LED thứ hai
        display7SEG(2);   // Hiển thị số 2
        break;
      case 2:
        enableLED(2);     // Bật LED thứ ba
        display7SEG(3);   // Hiển thị số 3
        break;
      case 3:
        enableLED(3);     // Bật LED thứ tư
        display7SEG(0);   // Hiển thị số 0
        break;
      default:
        // Trường hợp lỗi - hiển thị gạch ngang
        enableLED(currentLED);
        display7SEG(10);  // Hiển thị dấu "–"
        break;
      }
      // Chuyển sang LED tiếp theo - QUAN TRỌNG: Reset về 0 khi đạt 4
      ++currentLED;
      if (currentLED >= 4) {
        currentLED = 0;   // Quay lại LED đầu tiên
      }
    }

    // -------- Nhấp nháy LED chấm mỗi 1 giây --------
    if (isTimerExpired(1))
    {
      setTimer(1, 100); // Reset timer 1 (100 x 10ms = 1 giây)

      HAL_GPIO_TogglePin(GPIOA, DOT_Pin); // Đảo trạng thái LED chấm
    }

    // -------- Nhấp nháy LED đỏ mỗi 500ms --------
     if (isTimerExpired(2))
     {
       setTimer(2, 50); // Reset timer 2 (50 x 10ms = 500ms)

       HAL_GPIO_TogglePin(GPIOA, LED_RED_Pin); // Đảo trạng thái LED đỏ
     }
  }
}

/**
  * @brief  Hàm chính của chương trình
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* Cấu hình MCU */

  /* Reset tất cả ngoại vi, khởi tạo Flash và Systick */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Cấu hình đồng hồ hệ thống */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Khởi tạo tất cả ngoại vi đã cấu hình */
  MX_GPIO_Init();   // Khởi tạo các chân GPIO
  MX_TIM2_Init();   // Khởi tạo Timer 2

  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);  // Bắt đầu Timer 2 với ngắt
  /* USER CODE END 2 */

  // Thiết lập các timer ban đầu
  setTimer(0, 5);   // Timer cho quét 7-segment (50ms)
  setTimer(1, 100); // Timer cho nhấp nháy chấm (1s)
  setTimer(2, 50);  // Timer cho nhấp nháy LED đỏ (500ms)

  /* Vòng lặp vô hạn */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // Chương trình chính chạy ở đây
    // Tất cả xử lý được thực hiện trong hàm callback timer

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
  HAL_GPIO_WritePin(GPIOA, DOT_Pin|LED_RED_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DOT_Pin LED_RED_Pin EN0_Pin EN1_Pin
                           EN2_Pin EN3_Pin */
  GPIO_InitStruct.Pin = DOT_Pin|LED_RED_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin;
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
