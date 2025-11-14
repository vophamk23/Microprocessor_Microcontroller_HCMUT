/* global.h
 * File chứa các định nghĩa chung, biến toàn cục cho toàn dự án
 * Lab 5 - UART Protocol với Flow Control
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"

/* ============= HẰNG SỐ ============= */
#define MAX_BUFFER_SIZE 30      // Kích thước buffer UART
#define MAX_CMD_LENGTH 10       // Độ dài tối đa của lệnh

/* ============= ĐỊNH NGHĨA LỆNH ============= */
#define CMD_NONE 0              // Không có lệnh
#define CMD_RST  1              // Lệnh !RST#
#define CMD_OK   2              // Lệnh !OK#

/* ============= BIẾN TOÀN CỤC ============= */
// Buffer UART
extern uint8_t buffer[MAX_BUFFER_SIZE];
extern uint8_t index_buffer;
extern uint8_t buffer_flag;
extern uint8_t temp;

// Command data
extern uint8_t command_flag;
extern uint8_t command_data[MAX_CMD_LENGTH];

// ADC value
extern uint32_t adc_value;

// UART và ADC handles (khai báo trong main.c)
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;

#endif /* INC_GLOBAL_H_ */
