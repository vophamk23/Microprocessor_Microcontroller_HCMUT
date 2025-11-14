/* uart_communication.h
 * Module quản lý giao thức truyền thông UART
 * Xử lý timeout và retransmission
 */

#ifndef INC_UART_COMMUNICATION_H_
#define INC_UART_COMMUNICATION_H_


#include "global.h"

/* ============= HẰNG SỐ ============= */
#define TIMEOUT_DURATION 6000   // Timeout 3 giây (3000ms)

/* ============= ĐỊNH NGHĨA TRẠNG THÁI FSM ============= */
typedef enum {
    COMM_IDLE,        // Trạng thái chờ
    COMM_WAIT_RST,    // Chờ lệnh !RST#
    COMM_WAIT_OK,     // Chờ xác nhận !OK#
    COMM_TIMEOUT      // Xử lý timeout
} CommunicationState;

/* ============= KHAI BÁO HÀM ============= */

/**
 * @brief Khởi tạo communication FSM
 */
void uart_communication_init(void);

/**
 * @brief FSM quản lý giao thức truyền thông
 * Gọi hàm này liên tục trong main loop
 */
void uart_communication_fsm(void);

/**
 * @brief Gửi packet ADC theo format !ADC=xxxx#
 * @param value: Giá trị ADC cần gửi
 */
void send_adc_packet(uint32_t value);

/**
 * @brief Lấy trạng thái hiện tại của communication FSM
 * @return Trạng thái hiện tại
 */
CommunicationState get_communication_state(void);

#endif /* INC_UART_COMMUNICATION_H_ */
