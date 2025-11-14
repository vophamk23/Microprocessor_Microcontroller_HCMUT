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

// Mảng chứa các chân LED từ LED0 → LED11
// Mỗi LED sẽ tương ứng với 1 vị trí trên mặt đồng hồ (0→11)
uint16_t LED_Pins[12] = {
    LED0_Pin, LED1_Pin, LED2_Pin, LED3_Pin,
    LED4_Pin, LED5_Pin, LED6_Pin, LED7_Pin,
    LED8_Pin, LED9_Pin, LED10_Pin, LED11_Pin};

// Hàm tắt tất cả LED (clear toàn bộ mặt đồng hồ)
void clearAllClock()
{
  for (int i = 0; i < 12; i++)
  {
    HAL_GPIO_WritePin(GPIOA, LED_Pins[i], SET); // Tắt LED: SET = mức cao → LED tắt (vì LED active-low)
  }
}

// Hàm bật LED tại vị trí index (0 → 11)
// Ví dụ: setNumberOnClock(3) sẽ bật LED số 3
void setNumberOnClock(int index)
{
  HAL_GPIO_WritePin(GPIOA, LED_Pins[index], RESET); // Bật LED: RESET = mức thấp → LED sáng
}

// Hàm tắt LED tại vị trí index (0 → 11)
void clearNumberOnClock(int index)
{
  HAL_GPIO_WritePin(GPIOA, LED_Pins[index], SET); // Tắt LED: SET = mức cao → LED tắt
}

/*
// Hàm tắt toàn bộ LED thủ công
void clearAllClock() {
    HAL_GPIO_WritePin(GPIOA, LED0_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED1_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED2_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED3_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED4_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED5_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED6_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED7_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED8_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED9_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED10_Pin, SET);
    HAL_GPIO_WritePin(GPIOA, LED11_Pin, SET);
}

// Hàm bật LED theo số thứ tự
void setNumberOnClock(int index) {
    switch(index) {
        case 0:  HAL_GPIO_WritePin(GPIOA, LED0_Pin, RESET); break;
        case 1:  HAL_GPIO_WritePin(GPIOA, LED1_Pin, RESET); break;
        case 2:  HAL_GPIO_WritePin(GPIOA, LED2_Pin, RESET); break;
        case 3:  HAL_GPIO_WritePin(GPIOA, LED3_Pin, RESET); break;
        case 4:  HAL_GPIO_WritePin(GPIOA, LED4_Pin, RESET); break;
        case 5:  HAL_GPIO_WritePin(GPIOA, LED5_Pin, RESET); break;
        case 6:  HAL_GPIO_WritePin(GPIOA, LED6_Pin, RESET); break;
        case 7:  HAL_GPIO_WritePin(GPIOA, LED7_Pin, RESET); break;
        case 8:  HAL_GPIO_WritePin(GPIOA, LED8_Pin, RESET); break;
        case 9:  HAL_GPIO_WritePin(GPIOA, LED9_Pin, RESET); break;
        case 10: HAL_GPIO_WritePin(GPIOA, LED10_Pin, RESET); break;
        case 11: HAL_GPIO_WritePin(GPIOA, LED11_Pin, RESET); break;
        default: break;
    }
}

// Hàm tắt LED theo số thứ tự
void clearNumberOnClock(int index) {
    switch(index) {
        case 0:  HAL_GPIO_WritePin(GPIOA, LED0_Pin, SET); break;
        case 1:  HAL_GPIO_WritePin(GPIOA, LED1_Pin, SET); break;
        case 2:  HAL_GPIO_WritePin(GPIOA, LED2_Pin, SET); break;
        case 3:  HAL_GPIO_WritePin(GPIOA, LED3_Pin, SET); break;
        case 4:  HAL_GPIO_WritePin(GPIOA, LED4_Pin, SET); break;
        case 5:  HAL_GPIO_WritePin(GPIOA, LED5_Pin, SET); break;
        case 6:  HAL_GPIO_WritePin(GPIOA, LED6_Pin, SET); break;
        case 7:  HAL_GPIO_WritePin(GPIOA, LED7_Pin, SET); break;
        case 8:  HAL_GPIO_WritePin(GPIOA, LED8_Pin, SET); break;
        case 9:  HAL_GPIO_WritePin(GPIOA, LED9_Pin, SET); break;
        case 10: HAL_GPIO_WritePin(GPIOA, LED10_Pin, SET); break;
        case 11: HAL_GPIO_WritePin(GPIOA, LED11_Pin, SET); break;
        default: break;
    }
}
*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/**
 * @brief  Điểm bắt đầu của chương trình
 * @retval int
 */
int main(void)
{
  /* Khởi tạo HAL Library (bắt buộc trong STM32) */
  HAL_Init();

  /* Cấu hình xung nhịp hệ thống */
  SystemClock_Config();

  /* Khởi tạo GPIO để điều khiển LED */
  MX_GPIO_Init();

  /* Tắt tất cả LED khi bắt đầu */
  clearAllClock();

  // Thiết lập giá trị giờ, phút, giây ban đầu
  int clkHr = 1;  // Giờ bắt đầu tại LED1
  int clkMin = 3; // Phút bắt đầu tại LED3
  int clkSec = 0; // Giây bắt đầu tại LED0

  // Bật LED ban đầu cho giờ, phút, giây
  setNumberOnClock(clkHr);
  setNumberOnClock(clkMin);
  setNumberOnClock(clkSec);

  int secRound = 0; // Đếm số vòng giây (mỗi vòng = 60 giây)

  while (1)
  {
    HAL_Delay(50); // 1s thực tế = 1 bước giây

    // ==== BƯỚC 1: XÓA LED GIÂY CŨ ====
    if (clkSec != clkMin && clkSec != clkHr)
    {
      clearNumberOnClock(clkSec);
    }

    // ==== BƯỚC 2: TĂNG GIÂY ====
    clkSec++;
    if (clkSec > 11)
    {
      clkSec = 0;
      secRound++; // Đếm 1 vòng giây

      // ==== MỖI 5 VÒNG GIÂY → PHÚT NHÍCH 1 NẤC ====
      if (secRound >= 5)
      {
        secRound = 0; // Reset đếm vòng giây

        // XÓA LED PHÚT CŨ
        if (clkMin != clkHr && clkMin != clkSec)
        {
          clearNumberOnClock(clkMin);
        }

        // TĂNG PHÚT
        clkMin++;
        if (clkMin > 11)
        {
          clkMin = 0;

          // ==== MỖI VÒNG PHÚT → GIỜ NHÍCH 1 NẤC ====
          if (clkHr != clkMin && clkHr != clkSec)
          {
            clearNumberOnClock(clkHr);
          }

          clkHr++;
          if (clkHr > 11)
            clkHr = 0;

          setNumberOnClock(clkHr); // Bật giờ mới
        }
        setNumberOnClock(clkMin); // Bật phút mới
      }
    }

    // ==== BẬT LED GIÂY MỚI ====
    setNumberOnClock(clkSec);
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
