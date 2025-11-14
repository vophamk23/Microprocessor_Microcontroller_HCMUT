/*
 * scheduler_config.h
 *
 *  Created on: Oct 30, 2025
 *      Author: ASUS
 */

#ifndef INC_SCHEDULER_CONFIG_H_
#define INC_SCHEDULER_CONFIG_H_
/*============================================================================
 * CẤU HÌNH SCHEDULER
 *============================================================================*/

/**
 * TICK INTERVAL (ms)
 * Khoảng thời gian giữa các lần gọi SCH_Update()
 * Mặc định: 10ms
 */
#define SCHEDULER_TICK_MS       10

/**
 * SỐ LƯỢNG TASK TỐI ĐA
 * Số lượng task tối đa có thể thêm vào scheduler
 * Tăng số này nếu cần nhiều task hơn
 * Lưu ý: Mỗi task chiếm ~17 bytes RAM
 */
#define SCH_MAX_TASKS           40

/**
 * BÁO CÁO LỖI
 * Bật/tắt chức năng báo cáo lỗi
 * Comment dòng này để tắt báo cáo lỗi (tiết kiệm memory)
 */
#define SCH_REPORT_ERRORS

/**
 * SỬ DỤNG WATCHDOG
 * Bật/tắt chức năng watchdog timer
 * Comment dòng này để tắt watchdog
 */
//#define USE_WATCHDOG

/**
 * SỬ DỤNG CHẾ ĐỘ TIẾT KIỆM NĂNG LƯỢNG
 * Bật/tắt chế độ sleep khi idle
 * Comment dòng này để tắt low power mode
 */
//#define USE_LOW_POWER_MODE

/**
 * THỜI GIAN HIỂN THỊ LỖI (ticks)
 * Mỗi lỗi sẽ được hiển thị trong bao lâu
 * Mặc định: 60000 ticks = 60 giây (với tick 10ms)
 */
#define ERROR_DISPLAY_TIME      60000

/**
 * DEBUG MODE
 * Bật/tắt chế độ debug (in thông tin qua UART)
 */
#define SCHEDULER_DEBUG

/**
 * TIMER SỬ DỤNG
 * Chọn timer nào sẽ được dùng cho scheduler
 * Mặc định: TIM2
 */
#define SCHEDULER_TIMER         TIM2

/*============================================================================
 * CẤU HÌNH HARDWARE
 *============================================================================*/

/**
 * ERROR LED PORT & PIN
 * Port và Pin để hiển thị mã lỗi
 * Nếu dùng 8 LED thì cần 8 pin
 */
#ifdef SCH_REPORT_ERRORS

#define ERROR_LED_PORT          GPIOA
#define ERROR_LED_PIN_0         GPIO_PIN_0
#define ERROR_LED_PIN_1         GPIO_PIN_1
#define ERROR_LED_PIN_2         GPIO_PIN_2
#define ERROR_LED_PIN_3         GPIO_PIN_3
#define ERROR_LED_PIN_4         GPIO_PIN_4
#define ERROR_LED_PIN_5         GPIO_PIN_5
#define ERROR_LED_PIN_6         GPIO_PIN_6
#define ERROR_LED_PIN_7         GPIO_PIN_7

#endif

/*============================================================================
 * CẤU HÌNH UART DEBUG
 *============================================================================*/

#ifdef SCHEDULER_DEBUG

// UART handle để debug
extern UART_HandleTypeDef huart1;

// Macro để in debug message
#define SCH_DEBUG_PRINT(...)    printf(__VA_ARGS__)

#else

// Tắt debug print
#define SCH_DEBUG_PRINT(...)

#endif

/*============================================================================
 * KIỂM TRA CẤU HÌNH
 *============================================================================*/

// Kiểm tra TICK phải > 0
#if SCHEDULER_TICK_MS <= 0
#error "SCHEDULER_TICK_MS must be greater than 0"
#endif

// Kiểm tra số lượng task phải > 0
#if SCH_MAX_TASKS <= 0
#error "SCH_MAX_TASKS must be greater than 0"
#endif

// Cảnh báo nếu số task quá lớn (tốn RAM)
#if SCH_MAX_TASKS > 50
#warning "SCH_MAX_TASKS is very large. Check RAM usage!"
#endif


#endif /* INC_SCHEDULER_CONFIG_H_ */
