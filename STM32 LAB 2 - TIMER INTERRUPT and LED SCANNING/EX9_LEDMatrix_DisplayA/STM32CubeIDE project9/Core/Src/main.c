/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body - With LED Matrix
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
/* Thư viện cần thiết ------------------------------------------------------------------*/
#include "main.h"
#include "software_timer.h"

/* Khai báo hàm ------------------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

TIM_HandleTypeDef htim2;

/* ========================================================================== */
/* ===== PHẦN 1: BIẾN TOÀN CỤC CHO ĐỒNG HỒ LED 7 ĐOẠN ===== */
/* ========================================================================== */
const int MAX_LED = 4;                  // Có 4 LED 7-đoạn (hiển thị HH:MM)
int index_led = 0;                      // LED nào đang được bật (0->1->2->3->0...)
int hour = 15, minute = 8, second = 50; // Thời gian ban đầu 15:08:50
int led_buffer[4] = {0, 0, 0, 0};       // Lưu số cần hiển thị trên mỗi LED
                                        // led_buffer[0] = chữ số hàng chục của giờ
                                        // led_buffer[1] = chữ số hàng đơn vị của giờ
                                        // led_buffer[2] = chữ số hàng chục của phút
                                        // led_buffer[3] = chữ số hàng đơn vị của phút

/* ========================================================================== */
/* ===== PHẦN 2: BIẾN TOÀN CỤC CHO LED MATRIX 8x8 VỚI HIỆU ỨNG CHẠY ===== */
/* ========================================================================== */
const int MAX_LED_MATRIX = 8; // LED Matrix có 8 hàng
int index_led_matrix = 0;     // Hàng nào đang được bật (0->1->2->...->7->0...)

// Mẫu hiển thị chữ "A" trên LED Matrix 8x8
// Mỗi byte đại diện cho 1 hàng, mỗi bit đại diện cho 1 LED
uint8_t matrix_buffer[8] = {
    0x18, // 00011000 - Hàng 0: Đỉnh chữ A
    0x24, // 00100100 - Hàng 1: Hai bên chữ A
    0x42, // 01000010 - Hàng 2: Hai bên chữ A mở rộng
    0x42, // 01000010 - Hàng 3: Hai bên chữ A
    0x7E, // 01111110 - Hàng 4: Thanh ngang chữ A
    0x42, // 01000010 - Hàng 5: Hai bên chữ A
    0x42, // 01000010 - Hàng 6: Hai bên chữ A
    0x42  // 01000010 - Hàng 7: Chân chữ A
};


/* ========================================================================== */
/* ===== PHẦN 3: HÀM XỬ LÝ LED MATRIX ===== */
/* ========================================================================== */

/**
 * ===== HÀM LẤY TRẠNG THÁI BIT TẠI VỊ TRÍ CỤ THỂ =====
 * Mục đích: Lấy trạng thái của 1 bit cụ thể trong 1 byte
 * Input: hexa = giá trị hex (ví dụ: 0x18), index = vị trí bit cần lấy (0-7)
 * Output: GPIO_PIN_SET nếu bit = 1, GPIO_PIN_RESET nếu bit = 0
 * Ví dụ: getBitState(0x18, 3) sẽ trả về GPIO_PIN_SET vì bit thứ 3 của 0x18 = 1
 */
GPIO_PinState getBitState(uint8_t hexa, int index)
{
  int arr[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Mảng lưu từng bit

  // Chuyển đổi số hex thành từng bit và lưu vào mảng
  for (int i = 7; i >= 0; --i)
  {
    int mod = hexa % 2;  // Lấy bit cuối cùng
    hexa = hexa / 2;     // Dịch sang phải 1 bit
    arr[i] = mod;        // Lưu bit vào mảng
  }

  // Trả về trạng thái của bit tại vị trí index
  if (arr[index] == 1)
    return GPIO_PIN_SET;   // Bit = 1 → Bật LED
  return GPIO_PIN_RESET;   // Bit = 0 → Tắt LED
}


/**
 * ===== HÀM CÀI ĐẶT DATA CHO 1 HÀNG LED MATRIX =====
 * Mục đích: Xuất dữ liệu từ matrix_buffer ra các chân GPIO để điều khiển 8 cột LED
 * Input: index = hàng cần cài đặt (0-7)
 * Hoạt động: Lấy byte tương ứng trong matrix_buffer, tách thành 8 bit,
 *           xuất mỗi bit ra 1 chân GPIO (PA2, PA3, PA10-PA15)
 */
void updateLEDMatrix(int index)
{
  switch (index)
  {
  case 0:
    // Cài đặt data cho hàng 0: lấy matrix_buffer[0], tách thành 8 bit
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, getBitState(matrix_buffer[0], 0));   // Cột 0
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, getBitState(matrix_buffer[0], 1));   // Cột 1
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, getBitState(matrix_buffer[0], 2));  // Cột 2
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, getBitState(matrix_buffer[0], 3));  // Cột 3
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, getBitState(matrix_buffer[0], 4));  // Cột 4
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, getBitState(matrix_buffer[0], 5));  // Cột 5
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, getBitState(matrix_buffer[0], 6));  // Cột 6
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, getBitState(matrix_buffer[0], 7));  // Cột 7
    break;
  case 1:
    // Cài đặt data cho hàng 1: lấy matrix_buffer[1]
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, getBitState(matrix_buffer[1], 0));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, getBitState(matrix_buffer[1], 1));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, getBitState(matrix_buffer[1], 2));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, getBitState(matrix_buffer[1], 3));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, getBitState(matrix_buffer[1], 4));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, getBitState(matrix_buffer[1], 5));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, getBitState(matrix_buffer[1], 6));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, getBitState(matrix_buffer[1], 7));
    break;
    // Tương tự cho case 2, 3, 4, 5, 6, 7...
  case 2:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, getBitState(matrix_buffer[2], 0));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, getBitState(matrix_buffer[2], 1));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, getBitState(matrix_buffer[2], 2));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, getBitState(matrix_buffer[2], 3));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, getBitState(matrix_buffer[2], 4));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, getBitState(matrix_buffer[2], 5));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, getBitState(matrix_buffer[2], 6));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, getBitState(matrix_buffer[2], 7));
    break;
  case 3:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, getBitState(matrix_buffer[3], 0));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, getBitState(matrix_buffer[3], 1));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, getBitState(matrix_buffer[3], 2));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, getBitState(matrix_buffer[3], 3));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, getBitState(matrix_buffer[3], 4));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, getBitState(matrix_buffer[3], 5));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, getBitState(matrix_buffer[3], 6));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, getBitState(matrix_buffer[3], 7));
    break;
  case 4:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, getBitState(matrix_buffer[4], 0));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, getBitState(matrix_buffer[4], 1));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, getBitState(matrix_buffer[4], 2));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, getBitState(matrix_buffer[4], 3));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, getBitState(matrix_buffer[4], 4));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, getBitState(matrix_buffer[4], 5));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, getBitState(matrix_buffer[4], 6));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, getBitState(matrix_buffer[4], 7));
    break;
  case 5:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, getBitState(matrix_buffer[5], 0));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, getBitState(matrix_buffer[5], 1));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, getBitState(matrix_buffer[5], 2));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, getBitState(matrix_buffer[5], 3));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, getBitState(matrix_buffer[5], 4));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, getBitState(matrix_buffer[5], 5));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, getBitState(matrix_buffer[5], 6));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, getBitState(matrix_buffer[5], 7));
    break;
  case 6:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, getBitState(matrix_buffer[6], 0));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, getBitState(matrix_buffer[6], 1));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, getBitState(matrix_buffer[6], 2));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, getBitState(matrix_buffer[6], 3));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, getBitState(matrix_buffer[6], 4));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, getBitState(matrix_buffer[6], 5));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, getBitState(matrix_buffer[6], 6));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, getBitState(matrix_buffer[6], 7));
    break;
  case 7:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, getBitState(matrix_buffer[7], 0));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, getBitState(matrix_buffer[7], 1));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, getBitState(matrix_buffer[7], 2));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, getBitState(matrix_buffer[7], 3));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, getBitState(matrix_buffer[7], 4));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, getBitState(matrix_buffer[7], 5));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, getBitState(matrix_buffer[7], 6));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, getBitState(matrix_buffer[7], 7));
    break;
  default:
    break;
  }
}


/**
 * ===== HÀM QUÉT LED MATRIX (MULTIPLEXING) =====
 * Mục đích: Hiển thị LED Matrix bằng cách quét từng hàng một cách nhanh chóng
 * Nguyên lý: Chỉ bật 1 hàng tại 1 thời điểm, các hàng khác tắt
 *           Do quét rất nhanh (10ms/hàng) nên mắt người thấy như tất cả hàng đều sáng
 */
void scanLEDMatrix()
{
  // BƯỚC 1: Chọn hàng sẽ được bật (tắt hàng trước đó, bật hàng hiện tại)
  switch (index_led_matrix)
  {
  case 0:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET); // Tắt hàng 7 (hàng cuối)
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);    // Bật hàng 0 (hàng đầu)
    break;
  case 1:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET); // Tắt hàng 0
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);   // Bật hàng 1
    break;
  case 2:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET); // Tắt hàng 1
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);  // Bật hàng 2
    break;
  case 3:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET); // Tắt hàng 2
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);   // Bật hàng 3
    break;
  case 4:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); // Tắt hàng 3
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);   // Bật hàng 4
    break;
  case 5:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); // Tắt hàng 4
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);   // Bật hàng 5
    break;
  case 6:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); // Tắt hàng 5
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);   // Bật hàng 6
    break;
  case 7:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); // Tắt hàng 6
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);   // Bật hàng 7
    break;
  default:
    break;
  }

  // BƯỚC 2: Cài đặt dữ liệu cho hàng đã chọn
  updateLEDMatrix(index_led_matrix);

  // BƯỚC 3: Chuyển sang hàng tiếp theo để quét
  index_led_matrix++;
  if (index_led_matrix >= MAX_LED_MATRIX)
  {
    index_led_matrix = 0; // Quay lại hàng đầu tiên để quét lại từ đầu
  }
}


/* ========================================================================== */
/* ===== PHẦN 4: CÁC HÀM XỬ LÝ ĐỒNG HỒ LED 7 ĐOẠN ===== */
/* ========================================================================== */

/**
 * ===== HÀM CÀI ĐẶT BUFFER HIỂN THỊ ĐỒNG HỒ =====
 * Mục đích: Chuyển đổi thời gian (hour:minute) thành 4 số riêng biệt để hiển thị
 * Ví dụ: 15:08 → led_buffer[0]=1, led_buffer[1]=5, led_buffer[2]=0, led_buffer[3]=8
 */
void updateClockBuffer(void)
{
  led_buffer[0] = hour / 10;   // Chữ số hàng chục của giờ (1 từ 15:xx)
  led_buffer[1] = hour % 10;   // Chữ số hàng đơn vị của giờ (5 từ 15:xx)
  led_buffer[2] = minute / 10; // Chữ số hàng chục của phút (0 từ xx:08)
  led_buffer[3] = minute % 10; // Chữ số hàng đơn vị của phút (8 từ xx:08)
}

/**
 * ===== HÀM HIỂN THỊ 1 SỐ TRÊN LED 7 ĐOẠN =====
 * Mục đích: Điều khiển 7 đoạn LED để hiển thị số từ 0-9
 * Input: num = số cần hiển thị (0-9)
 * Lưu ý: Logic nghịch đảo - RESET = bật LED, SET = tắt LED
 */
void display7SEG(int num)
{
  // Tắt tất cả 7 đoạn LED trước khi bật các đoạn cần thiết
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_SET);

  switch (num)
  {
  case 0: // Hiển thị số 0: bật 6 đoạn (a,b,c,d,e,f)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin, GPIO_PIN_RESET);
    break;
  case 1: // Hiển thị số 1: chỉ bật 2 đoạn (b,c)
    HAL_GPIO_WritePin(GPIOB, SEG1_Pin | SEG2_Pin, GPIO_PIN_RESET);
    break;
  case 2: // Hiển thị số 2: bật 5 đoạn (a,b,g,e,d)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG3_Pin | SEG4_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 3: // Hiển thị số 3: bật 5 đoạn (a,b,g,c,d)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 4: // Hiển thị số 4: bật 4 đoạn (f,g,b,c)
    HAL_GPIO_WritePin(GPIOB, SEG1_Pin | SEG2_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 5: // Hiển thị số 5: bật 5 đoạn (a,f,g,c,d)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG2_Pin | SEG3_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 6: // Hiển thị số 6: bật 6 đoạn (a,f,g,e,d,c)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 7: // Hiển thị số 7: bật 3 đoạn (a,b,c)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin, GPIO_PIN_RESET);
    break;
  case 8: // Hiển thị số 8: bật tất cả 7 đoạn
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  case 9: // Hiển thị số 9: bật 6 đoạn (a,b,c,d,f,g)
    HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | SEG5_Pin | SEG6_Pin, GPIO_PIN_RESET);
    break;
  default: // Số không hợp lệ: chỉ hiển thị dấu gạch ngang
    HAL_GPIO_WritePin(GPIOB, SEG6_Pin, GPIO_PIN_RESET);
    break;
  }
}

/**
 * ===== HÀM BẬT 1 LED 7 ĐOẠN CỤ THỂ =====
 * Mục đích: Chọn LED 7-đoạn nào sẽ được bật (trong 4 LED)
 * Input: index = LED cần bật (0, 1, 2, 3)
 * Hoạt động: Tắt tất cả LED, chỉ bật LED được chọn
 */
void enableLED(int index)
{
  // Tắt tất cả 4 LED 7-đoạn trước
  HAL_GPIO_WritePin(GPIOA, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);

  // Chỉ bật LED được chọn
  switch (index)
  {
  case 0: // Bật LED đầu tiên (hàng chục giờ)
    HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_RESET);
    break;
  case 1: // Bật LED thứ hai (hàng đơn vị giờ)
    HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET);
    break;
  case 2: // Bật LED thứ ba (hàng chục phút)
    HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_RESET);
    break;
  case 3: // Bật LED thứ tư (hàng đơn vị phút)
    HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_RESET);
    break;
  default:
    break;
  }
}

/**
 * ===== HÀM CÀI ĐẶT HIỂN THỊ CHO 1 LED 7 ĐOẠN =====
 * Mục đích: Bật LED cụ thể và hiển thị số tương ứng từ buffer
 */
void update7SEG(int index)
{
  switch (index)
  {
  case 0:
    enableLED(0);               // Bật LED đầu tiên
    display7SEG(led_buffer[0]); // Hiển thị số từ buffer[0]
    break;
  case 1:
    enableLED(1);               // Bật LED thứ hai
    display7SEG(led_buffer[1]); // Hiển thị số từ buffer[1]
    break;
  case 2:
    enableLED(2);               // Bật LED thứ ba
    display7SEG(led_buffer[2]); // Hiển thị số từ buffer[2]
    break;
  case 3:
    enableLED(3);               // Bật LED thứ tư
    display7SEG(led_buffer[3]); // Hiển thị số từ buffer[3]
    break;
  default:
    // Tắt tất cả LED nếu index không hợp lệ
    HAL_GPIO_WritePin(GPIOA, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);
    break;
  }
}

/**
 * ===== HÀM QUÉT LED 7 ĐOẠN (MULTIPLEXING) =====
 * Mục đích: Quét qua 4 LED 7-đoạn để hiển thị đồng hồ HH:MM
 * Nguyên lý: Giống như LED Matrix, chỉ bật 1 LED tại 1 thời điểm
 *           Quét nhanh (25ms/LED) để tạo cảm giác tất cả LED đều sáng
 */
void scanLED()
{
  update7SEG(index_led); // Cập nhật LED hiện tại

  // Chuyển sang LED tiếp theo
  index_led++;
  if (index_led >= MAX_LED)
  {
    index_led = 0; // Quay lại LED đầu tiên
  }
}

// =================================================================== //
/**
 * ===== HÀM XỬ LÝ NGẮT TIMER =====
 * Mục đích: Được gọi mỗi 10ms để chạy các timer phần mềm
 * Chức năng: Cập nhật trạng thái timer và quét LED Matrix
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    // Cập nhật tất cả timer phần mềm mỗi 10ms
    timerRun();

    // Timer cho quét LED Matrix (mỗi 100ms = 10 × 10ms)
    if (isTimerExpired(4))
    {
      setTimer(4, 10); // Đặt lại timer 100ms
      scanLEDMatrix(); // Quét 1 hàng LED Matrix
    }
  }
}

/**
 * @brief Chương trình chính - ĐỒNG HỒ VÀ LED MATRIX
 */
int main(void)
{
  /* Cấu hình MCU */
  HAL_Init();
  SystemClock_Config();

  /* Khởi tạo ngoại vi */
  MX_GPIO_Init();
  MX_TIM2_Init();

  /* ===== KHỞI TẠO CÁC TIMER PHẦN MỀM ===== */
  // Timer 0: Quét LED 7-đoạn mỗi 250ms (25 × 10ms)
  setTimer(0, 25);
  // Timer 1: Nhấp nháy LED chấm mỗi 750ms (75 × 10ms)
  setTimer(1, 75);
  // Timer 2: Nhấp nháy LED đỏ mỗi 500ms (50 × 10ms)
  setTimer(2, 50);
  // Timer 3: Cập nhật đồng hồ mỗi 1 giây (100 × 10ms)
  setTimer(3, 100);
  // Timer 4: Quét LED Matrix mỗi 100ms (10 × 10ms)
  setTimer(4, 10);
  /* Cập nhật buffer hiển thị đồng hồ ban đầu */
  updateClockBuffer();

  // Bắt đầu ngắt timer để chạy hệ thống
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Vòng lặp chính */
  while (1)
  {
    // -------- TIMER 0: Quét LED 7-đoạn hiển thị đồng hồ --------
    if (isTimerExpired(0))
    {
      setTimer(0, 25); // Đặt lại timer 250ms
      scanLED();       // Quét sang LED 7-đoạn tiếp theo
    }

    // -------- TIMER 1: Nhấp nháy LED chấm (dấu : trong HH:MM) --------
    if (isTimerExpired(1))
    {
      setTimer(1, 75);                    // Đặt lại timer 750ms
      HAL_GPIO_TogglePin(GPIOA, DOT_Pin); // Đảo trạng thái LED chấm
    }

    // -------- TIMER 2: Nhấp nháy LED đỏ (báo hiệu hoạt động) --------
    if (isTimerExpired(2))
    {
      setTimer(2, 50);                        // Đặt lại timer 500ms
      HAL_GPIO_TogglePin(GPIOA, LED_RED_Pin); // Đảo trạng thái LED đỏ
    }

    // -------- TIMER 3: Cập nhật thời gian đồng hồ mỗi giây --------
    if (isTimerExpired(3))
    {
      // Tăng giây
      second++;

      // Kiểm tra tràn giây (60 giây = 1 phút)
      if (second >= 60)
      {
        second = 0; // Reset giây về 0
        minute++;   // Tăng phút
      }

      // Kiểm tra tràn phút (60 phút = 1 giờ)
      if (minute >= 60)
      {
        minute = 0; // Reset phút về 0
        hour++;     // Tăng giờ
      }

      // Kiểm tra tràn giờ (24 giờ = 1 ngày)
      if (hour >= 24)
      {
        hour = 0; // Reset giờ về 0 (bắt đầu ngày mới)
      }

      // Cập nhật buffer hiển thị với thời gian mới
      updateClockBuffer();

      // Đặt lại timer 1 giây
      setTimer(3, 100);
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
  HAL_GPIO_WritePin(GPIOA, ENM0_Pin | ENM1_Pin | DOT_Pin | LED_RED_Pin | EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin | ENM2_Pin | ENM3_Pin | ENM4_Pin | ENM5_Pin | ENM6_Pin | ENM7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin | SEG1_Pin | SEG2_Pin | ROW2_Pin | ROW3_Pin | ROW4_Pin | ROW5_Pin | ROW6_Pin | ROW7_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin | ROW0_Pin | ROW1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ENM0_Pin ENM1_Pin DOT_Pin LED_RED_Pin
                           EN0_Pin EN1_Pin EN2_Pin EN3_Pin
                           ENM2_Pin ENM3_Pin ENM4_Pin ENM5_Pin
                           ENM6_Pin ENM7_Pin */
  GPIO_InitStruct.Pin = ENM0_Pin | ENM1_Pin | DOT_Pin | LED_RED_Pin | EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin | ENM2_Pin | ENM3_Pin | ENM4_Pin | ENM5_Pin | ENM6_Pin | ENM7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG0_Pin SEG1_Pin SEG2_Pin ROW2_Pin
                           ROW3_Pin ROW4_Pin ROW5_Pin ROW6_Pin
                           ROW7_Pin SEG3_Pin SEG4_Pin SEG5_Pin
                           SEG6_Pin ROW0_Pin ROW1_Pin */
  GPIO_InitStruct.Pin = SEG0_Pin | SEG1_Pin | SEG2_Pin | ROW2_Pin | ROW3_Pin | ROW4_Pin | ROW5_Pin | ROW6_Pin | ROW7_Pin | SEG3_Pin | SEG4_Pin | SEG5_Pin | SEG6_Pin | ROW0_Pin | ROW1_Pin;
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
