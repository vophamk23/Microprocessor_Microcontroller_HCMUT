/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Chương trình chính - Hệ thống điều khiển đèn giao thông
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

// Định nghĩa trạng thái đèn giao thông
enum State {
	red,
	green,
	yellow}; // đỏ, xanh lá, vàng

// Biến đếm thời gian cho mỗi ngã tư
int numDisplay1 = 4; // Thời gian đếm ngược cho ngã tư 1 (Đặt thời gian bắt đầu với đèn đỏ 4 giây)
int numDisplay2 = 3; // Thời gian đếm ngược cho ngã tư 2 (Đặt thời gian bắt đầu với đèn xanh 1 giây)

// Trạng thái hiện tại của từng ngã tư
enum State currentStateVar1 = red;   // Ngã tư 1 (bắt đầu) với đèn đỏ
enum State currentStateVar2 = green; // Ngã tư 2 (bắt đầu) với đèn xanh

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Hàm bật đèn theo trạng thái
void setTrafficLight(GPIO_TypeDef* RED_Port, uint16_t RED_Pin,
                     GPIO_TypeDef* YELLOW_Port, uint16_t YELLOW_Pin,
                     GPIO_TypeDef* GREEN_Port, uint16_t GREEN_Pin,
                     enum State state) {
    if (state == red) {
        HAL_GPIO_WritePin(RED_Port, RED_Pin, RESET);      // BẬT đỏ
        HAL_GPIO_WritePin(YELLOW_Port, YELLOW_Pin, SET);  // TẮT vàng
        HAL_GPIO_WritePin(GREEN_Port, GREEN_Pin, SET);    // TẮT xanh
    }
    else if (state == green) {
        HAL_GPIO_WritePin(RED_Port, RED_Pin, SET);        // TẮT đỏ
        HAL_GPIO_WritePin(YELLOW_Port, YELLOW_Pin, SET);  // TẮT vàng
        HAL_GPIO_WritePin(GREEN_Port, GREEN_Pin, RESET);  // BẬT xanh
    }
    else if (state == yellow) {
        HAL_GPIO_WritePin(RED_Port, RED_Pin, SET);        // TẮT đỏ
        HAL_GPIO_WritePin(YELLOW_Port, YELLOW_Pin, RESET);// BẬT vàng
        HAL_GPIO_WritePin(GREEN_Port, GREEN_Pin, SET);    // TẮT xanh
    }
}

/**
 * @brief Hàm hiển thị số trên LED 7 đoạn
 * @param num: Số cần hiển thị (0-9)
 * @param A,B,C,D,E,F,G: Các chân GPIO tương ứng với từng segment của LED 7 đoạn
 * @retval None
 *
 * Sơ đồ LED 7 đoạn:
 *     A
 *   -----
 *  |     |
 * F|  G  |B
 *  |-----|
 *  |     |
 * E|     |C
 *  -----
 *    D
 */
void display7SEG(int num, uint16_t A, uint16_t B, uint16_t C, uint16_t D, uint16_t E, uint16_t F, uint16_t G) {
  if (num == 0) {
    // Hiển thị số 0: bật A,B,C,D,E,F - tắt G
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_RESET); // a - BẬT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_RESET); // b - BẬT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_RESET); // d - BẬT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_RESET); // e - BẬT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_RESET); // f - BẬT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_SET);   // g - TẮT
  }
  else if (num == 1) {
    // Hiển thị số 1: bật B,C - tắt A,D,E,F,G
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_SET);   // a - TẮT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_RESET); // b - BẬT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_SET);   // d - TẮT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_SET);   // e - TẮT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_SET);   // f - TẮT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_SET);   // g - TẮT
  }
  else if (num == 2) {
    // Hiển thị số 2: bật A,B,D,E,G - tắt C,F
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_RESET); // a - BẬT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_RESET); // b - BẬT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_SET);   // c - TẮT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_RESET); // d - BẬT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_RESET); // e - BẬT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_SET);   // f - TẮT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_RESET); // g - BẬT
  }
  else if (num == 3) {
    // Hiển thị số 3: bật A,B,C,D,G - tắt E,F
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_RESET); // a - BẬT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_RESET); // b - BẬT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_RESET); // d - BẬT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_SET);   // e - TẮT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_SET);   // f - TẮT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_RESET); // g - BẬT
  }
  else if (num == 4) {
    // Hiển thị số 4: bật B,C,F,G - tắt A,D,E
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_SET);   // a - TẮT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_RESET); // b - BẬT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_SET);   // d - TẮT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_SET);   // e - TẮT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_RESET); // f - BẬT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_RESET); // g - BẬT
  }
  else if (num == 5) {
    // Hiển thị số 5: bật A,C,D,F,G - tắt B,E
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_RESET); // a - BẬT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_SET);   // b - TẮT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_RESET); // d - BẬT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_SET);   // e - TẮT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_RESET); // f - BẬT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_RESET); // g - BẬT
  }
  else if (num == 6) {
    // Hiển thị số 6: bật A,C,D,E,F,G - tắt B
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_RESET); // a - BẬT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_SET);   // b - TẮT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_RESET); // d - BẬT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_RESET); // e - BẬT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_RESET); // f - BẬT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_RESET); // g - BẬT
  }
  else if (num == 7) {
    // Hiển thị số 7: bật A,B,C - tắt D,E,F,G
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_RESET); // a - BẬT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_RESET); // b - BẬT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_SET);   // d - TẮT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_SET);   // e - TẮT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_SET);   // f - TẮT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_SET);   // g - TẮT
  }
  else if (num == 8) {
    // Hiển thị số 8: bật tất cả A,B,C,D,E,F,G
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_RESET); // a - BẬT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_RESET); // b - BẬT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_RESET); // d - BẬT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_RESET); // e - BẬT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_RESET); // f - BẬT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_RESET); // g - BẬT
  }
  else if (num == 9) {
    // Hiển thị số 9: bật A,B,C,D,F,G - tắt E
    HAL_GPIO_WritePin(GPIOB, A, GPIO_PIN_RESET); // a - BẬT
    HAL_GPIO_WritePin(GPIOB, B, GPIO_PIN_RESET); // b - BẬT
    HAL_GPIO_WritePin(GPIOB, C, GPIO_PIN_RESET); // c - BẬT
    HAL_GPIO_WritePin(GPIOB, D, GPIO_PIN_RESET); // d - BẬT
    HAL_GPIO_WritePin(GPIOB, E, GPIO_PIN_SET);   // e - TẮT
    HAL_GPIO_WritePin(GPIOB, F, GPIO_PIN_RESET); // f - BẬT
    HAL_GPIO_WritePin(GPIOB, G, GPIO_PIN_RESET); // g - BẬT
  }
}

/**
  * @brief  Điểm bắt đầu của ứng dụng
  * @retval int
  */
int main(void)
{
  /* Khởi động lại tất cả các thiết bị ngoại vi, khởi tạo giao diện Flash và Systick. */
  HAL_Init();

  /* Cấu hình đồng hồ hệ thống */
  SystemClock_Config();

  /* Khởi tạo tất cả các thiết bị ngoại vi đã cấu hình */
  MX_GPIO_Init();

  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    //-------------------HIỂN THỊ SỐ TRÊN LED 7 ĐOẠN------------------------//
    // Hiển thị số đếm ngược trên LED 7 đoạn của ngã tư 1
    display7SEG(numDisplay1, LED_A1_Pin, LED_B1_Pin, LED_C1_Pin, LED_D1_Pin, LED_E1_Pin, LED_F1_Pin, LED_G1_Pin);
    // Hiển thị số đếm ngược trên LED 7 đoạn của ngã tư 2
    display7SEG(numDisplay2, LED_A2_Pin, LED_B2_Pin, LED_C2_Pin, LED_D2_Pin, LED_E2_Pin, LED_F2_Pin, LED_G2_Pin);

	  // Bật đèn theo trạng thái hiện tại
	setTrafficLight(LED_RED1_GPIO_Port, LED_RED1_Pin,
	                     LED_YELLOW1_GPIO_Port, LED_YELLOW1_Pin,
	                     LED_GREEN1_GPIO_Port, LED_GREEN1_Pin,
	                     currentStateVar1);

	setTrafficLight(LED_RED2_GPIO_Port, LED_RED2_Pin,
	                     LED_YELLOW2_GPIO_Port, LED_YELLOW2_Pin,
	                     LED_GREEN2_GPIO_Port, LED_GREEN2_Pin,
	                     currentStateVar2);
	 HAL_Delay(1000); // Delay 1 giây


    //-------------------ĐẾM NGƯỢC THỜI GIAN------------------------//
    // Giảm thời gian đếm ngược của cả 2 ngã tư
    --numDisplay1;
    --numDisplay2;

    // CHUYỂN TRẠNG THÁI ĐÈN Ở NGÃ 1 HƯỚNG ĐÔNG TÂY
    // Nếu hết thời gian đèn đỏ -> chuyển sang xanh lá (3 giây)
    if (numDisplay1 < 0) {
        if (currentStateVar1 == red) {
            currentStateVar1 = green;
            numDisplay1 = 2;  // Xanh 3s
        }
        // Nếu hết thời gian đèn xanh lá -> chuyển sang vàng (2 giây)
        else if (currentStateVar1 == green) {
            currentStateVar1 = yellow;
            numDisplay1 = 1;  // Vàng 2s
        }
        // Nếu hết thời gian đèn vàng -> chuyển sang đỏ (4 giây)
        else { // yellow -> red
            currentStateVar1 = red;
            numDisplay1 = 4;  // Đỏ 4s
        }
    }

    // CHUYỂN TRẠNG THÁI ĐÈN Ở NGÃ 2 HƯỚNG BẮC NAM
    // Nếu hết thời gian đèn đỏ -> chuyển sang xanh lá (3 giây)
    if (numDisplay2 < 0) {
        if (currentStateVar2 == red) {
            currentStateVar2 = green;
            numDisplay2 = 2;  // Xanh 3s
        }
        // Nếu hết thời gian đèn xanh lá -> chuyển sang vàng (2 giây)
        else if (currentStateVar2 == green) {
            currentStateVar2 = yellow;
            numDisplay2 = 1;  // Vàng 2s
        }
        // Nếu hết thời gian đèn vàng -> chuyển sang đỏ (4 giây)
        else { // yellow -> red
            currentStateVar2 = red;
            numDisplay2 = 4;  // Đỏ 4s
        }
    }
  }
}

/**
  * @brief Cấu hình đồng hồ hệ thống
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Khởi tạo các bộ dao động RCC theo các tham số được chỉ định
  * trong cấu trúc RCC_OscInitTypeDef.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Khởi tạo đồng hồ CPU, AHB và APB buses
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
  * @brief Hàm khởi tạo GPIO
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Kích hoạt clock cho các cổng GPIO */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Cấu hình mức đầu ra GPIO - Tất cả đèn LED bắt đầu ở trạng thái TẮT */
  HAL_GPIO_WritePin(GPIOA, LED_RED1_Pin|LED_YELLOW1_Pin|LED_GREEN1_Pin|LED_RED2_Pin
                          |LED_YELLOW2_Pin|LED_GREEN2_Pin, GPIO_PIN_SET);

  /* Cấu hình mức đầu ra cho LED 7 đoạn - Tất cả segment bắt đầu ở trạng thái TẮT */
  HAL_GPIO_WritePin(GPIOB, LED_A1_Pin|LED_B1_Pin|LED_C1_Pin|LED_D2_Pin
                          |LED_E2_Pin|LED_F2_Pin|LED_G2_Pin|LED_D1_Pin
                          |LED_E1_Pin|LED_F1_Pin|LED_G1_Pin|LED_A2_Pin
                          |LED_B2_Pin|LED_C2_Pin, GPIO_PIN_SET);

  /* Cấu hình các chân GPIO cho đèn LED giao thông */
  GPIO_InitStruct.Pin = LED_RED1_Pin|LED_YELLOW1_Pin|LED_GREEN1_Pin|LED_RED2_Pin
                          |LED_YELLOW2_Pin|LED_GREEN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Chế độ đầu ra push-pull
  GPIO_InitStruct.Pull = GPIO_NOPULL;         // Không có pull-up/pull-down
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Tốc độ thấp
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Cấu hình các chân GPIO cho LED 7 đoạn */
  GPIO_InitStruct.Pin = LED_A1_Pin|LED_B1_Pin|LED_C1_Pin|LED_D2_Pin
                          |LED_E2_Pin|LED_F2_Pin|LED_G2_Pin|LED_D1_Pin
                          |LED_E1_Pin|LED_F1_Pin|LED_G1_Pin|LED_A2_Pin
                          |LED_B2_Pin|LED_C2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Chế độ đầu ra push-pull
  GPIO_InitStruct.Pull = GPIO_NOPULL;          // Không có pull-up/pull-down
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Tốc độ thấp
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  Hàm này được thực thi khi có lỗi xảy ra.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq(); // Tắt ngắt
  while (1)
  {
    // Vòng lặp vô tận khi có lỗi
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
