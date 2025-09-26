//* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body - Fixed for Proteus
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "software_timer.h"

/* Private includes ----------------------------------------------------------*/
TIM_HandleTypeDef htim2;

const int MAX_LED = 4;               // Số lượng LED 7-đoạn tối đa
int index_led = 0;                   // Chỉ số LED hiện tại đang quét
int led_buffer[4] = {1, 2, 3, 4};   // Mảng chứa giá trị hiển thị cho từng LED

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */
/**
 * @brief Hiển thị số trên LED 7-đoạn
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
    // Hiển thị gạch ngang (đoạn g) cho số không hợp lệ
    HAL_GPIO_WritePin(GPIOB, SEG6_Pin, GPIO_PIN_RESET);
    break;
  }
}

/**
 * @brief Bật một LED 7-đoạn cụ thể
 * @param index: Chỉ số LED (0-3)
 */
void enableLED(int index)
{
  // Tắt tất cả LED trước (SET = tắt do transistor PNP)
  HAL_GPIO_WritePin(GPIOA, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);

  switch (index)
  {
  case 0:
    HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_RESET); // Bật LED thứ 0
    break;
  case 1:
    HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET); // Bật LED thứ 1
    break;
  case 2:
    HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_RESET); // Bật LED thứ 2
    break;
  case 3:
    HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_RESET); // Bật LED thứ 3
    break;
  default:
    break;
  }
}

/**
 * @brief Exercise 3: Cập nhật hiển thị LED 7-đoạn tại vị trí cụ thể
 * @param index: Chỉ số LED (0-3)
 */
void update7SEG(int index)
{
  switch (index)
  {
  case 0:
    // Hiển thị LED đầu tiên với giá trị led_buffer[0]
    enableLED(0);
    display7SEG(led_buffer[0]);  // Hiển thị số 1
    break;
  case 1:
    // Hiển thị LED thứ hai với giá trị led_buffer[1]
    enableLED(1);
    display7SEG(led_buffer[1]);  // Hiển thị số 2
    break;
  case 2:
    // Hiển thị LED thứ ba với giá trị led_buffer[2]
    enableLED(2);
    display7SEG(led_buffer[2]);  // Hiển thị số 3
    break;
  case 3:
    // Hiển thị LED thứ tư với giá trị led_buffer[3]
    enableLED(3);
    display7SEG(led_buffer[3]);  // Hiển thị số 4
    break;
  default:
    // Tắt tất cả LED nếu chỉ số không hợp lệ
    HAL_GPIO_WritePin(GPIOA, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);
    break;
  }
}

/**
 * @brief Quét tuần tự tất cả LED 7-đoạn
 */
void scanLED()
{
  update7SEG(index_led);  // Cập nhật LED tại vị trí hiện tại

  // Chuyển sang LED tiếp theo
  index_led++;
  if (index_led >= MAX_LED)
  {
    index_led = 0;  // Quay lại LED đầu tiên
  }
}
/* USER CODE END PFP */

/**
 * @brief Hàm callback ngắt timer - PHIÊN BẢN ĐÃ SỬA
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    // Chạy các timer phần mềm mỗi 10ms
    timerRun();

    // -------- Quét LED 7-đoạn mỗi 500ms (EX3) hoặc 250ms (EX4) --------
    if (isTimerExpired(0))
    {
      setTimer(0, 50);    // 50 × 10ms = 500ms - EX3: 0.5s cho mỗi LED - 2s để quét hết 4 LED
    //  setTimer(0, 25); // 25 × 10ms = 250ms - EX4: tần số quét 4 LED giảm xuống 1Hz (1s để quét hết)

      scanLED();          // Quét qua tất cả LED 7-đoạn
    }

    // -------- Nhấp nháy LED chấm mỗi 1 giây --------
    if (isTimerExpired(1))
    {
      setTimer(1, 100);   // 150 × 10ms = 1500ms = 1.5s
      HAL_GPIO_TogglePin(GPIOA, DOT_Pin);  // Đảo trạng thái LED chấm
    }

    // -------- Nhấp nháy LED đỏ mỗi 500ms --------
    if (isTimerExpired(2))
    {
      setTimer(2, 50);    // 50 × 10ms = 500ms
      HAL_GPIO_TogglePin(GPIOA, LED_RED_Pin);  // Đảo trạng thái LED đỏ
    }
  }
}

/**
 * @brief Chương trình chính
 */
int main(void)
{
  /* Cấu hình MCU */
  HAL_Init();             // Khởi tạo HAL
  SystemClock_Config();   // Cấu hình đồng hồ hệ thống

  /* Khởi tạo ngoại vi */
  MX_GPIO_Init();         // Khởi tạo GPIO
  MX_TIM2_Init();         // Khởi tạo Timer 2

  /* USER CODE BEGIN 2 */
  // Khởi tạo các timer phần mềm TRƯỚC KHI bật timer ngắt
  setTimer(0, 50);        // Timer 0: 50 × 10ms = 500ms để quét LED
  setTimer(1, 150);       // Timer 1: 150 × 10ms = 1500ms để nhấp nháy LED chấm
  setTimer(2, 50);        // Timer 2: 50 × 10ms = 500ms để nhấp nháy LED đỏ

  // Bật ngắt timer SAU KHI đã khởi tạo timer
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Vòng lặp vô hạn */
  while (1)
  {
    // Chương trình chính chạy ở đây
    // Tất cả logic được xử lý trong hàm callback timer
  }
}
/**
 * @brief System Clock Configuration
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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
 * @brief TIM2 Initialization Function
 */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

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
}

/**
 * @brief GPIO Initialization Function
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, DOT_Pin | LED_RED_Pin | EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = DOT_Pin | LED_RED_Pin | EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User implementation */
}
#endif
