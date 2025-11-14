/* main.c
 * File chính của dự án Lab 5 - UART Protocol
 * COMPLETE FIXED VERSION
 *
 * Chức năng:
 * - Giao tiếp UART với terminal
 * - Đọc giá trị ADC từ cảm biến
 * - Giao thức: Terminal gửi !RST# -> STM32 trả về !ADC=xxxx# -> Terminal xác nhận !OK#
 * - Timeout 3 giây nếu không nhận được !OK#
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "global.h"
#include "command_parser.h"
#include "uart_communication.h"
#include "sensor.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
ADC_HandleTypeDef hadc1;

// Khai báo các biến toàn cục (định nghĩa trong global.h)
uint8_t buffer[MAX_BUFFER_SIZE];
uint8_t index_buffer = 0;
uint8_t buffer_flag = 0;
uint8_t temp = 0;
uint8_t command_flag = CMD_NONE;
uint8_t command_data[MAX_CMD_LENGTH];
uint32_t adc_value = 0;
// LED timer
uint32_t led_timer = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);

/**
 * @brief UART Receive Complete Callback
 * Hàm này được gọi tự động khi nhận được 1 byte từ UART
 *
 * Chức năng:
 * - Lưu byte nhận được vào buffer circular
 * - Set flag báo có dữ liệu mới
 * - Kích hoạt lại interrupt để nhận byte tiếp theo
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        // ⭐ Echo ký tự nhận được ngay lập tức
        HAL_UART_Transmit(&huart2, &temp, 1, 50);

        // Lưu vào buffer...
        if (index_buffer < MAX_BUFFER_SIZE)
        {
            buffer[index_buffer] = temp;
            index_buffer++;

            if (index_buffer >= MAX_BUFFER_SIZE)
            {
                index_buffer = 0;
            }

            buffer_flag = 1;
        }

        HAL_UART_Receive_IT(&huart2, &temp, 1);
    }
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
    MX_USART2_UART_Init();
    MX_ADC1_Init();

    // === KHỞI TẠO CÁC MODULE ===
    command_parser_init();     // Khởi tạo command parser FSM
    uart_communication_init(); // Khởi tạo communication FSM
    sensor_init();             // Khởi tạo ADC (continuous mode)

    // Kích hoạt UART interrupt để bắt đầu nhận dữ liệu
    HAL_UART_Receive_IT(&huart2, &temp, 1);

    // Khởi tạo LED timer
    led_timer = HAL_GetTick();

    while (1)
    {
        // === BƯỚC 1: Xử lý buffer khi có dữ liệu mới ===
        if (buffer_flag == 1)
        {
            // *** Chạy command parser FSM để phân tích lệnh
            command_parser_fsm();
            // Clear flag
            buffer_flag = 0;
        }

        // === BƯỚC 2: Chạy communication FSM ===
        // ****FSM này chạy liên tục để xử lý giao thức
        uart_communication_fsm();

        // === BƯỚC 3: LED BLINKING ===
        // Lấy trạng thái hiện tại của giao thức
        CommunicationState state = get_communication_state();

        switch (state)
        {
        case COMM_WAIT_RST:
            // Nhấp nháy CHẬM (500ms) - Đang chờ lệnh RST
            if (HAL_GetTick() - led_timer >= 500)
            {
                HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
                led_timer = HAL_GetTick();
            }
            break;

        case COMM_WAIT_OK:
            // Nhấp nháy NHANH (100ms) - Đang chờ xác nhận OK
            if (HAL_GetTick() - led_timer >= 100)
            {
                HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
                led_timer = HAL_GetTick();
            }
            break;

        case COMM_TIMEOUT:
            // Nhấp nháy RẤT NHANH (50ms) - Timeout, đang gửi lại
            if (HAL_GetTick() - led_timer >= 50)
            {
                HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
                led_timer = HAL_GetTick();
            }
            break;

        default:
            // Sáng liên tục - Lỗi không xác định
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
            break;
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
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */
    /** Common config
     */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
    /** Configure Regular Channel
     */
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

    /* USER CODE BEGIN USART2_Init 0 */

    /* USER CODE END USART2_Init 0 */

    /* USER CODE BEGIN USART2_Init 1 */

    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN USART2_Init 2 */

    /* USER CODE END USART2_Init 2 */
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
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : LED_RED_Pin */
    GPIO_InitStruct.Pin = LED_RED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);
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
