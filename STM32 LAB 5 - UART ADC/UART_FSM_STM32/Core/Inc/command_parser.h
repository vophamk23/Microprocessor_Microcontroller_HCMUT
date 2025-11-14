/* command_parser.h
 * Module phân tích lệnh từ buffer UART
 * Nhận dạng các lệnh: !RST# và !OK#
 */

#ifndef INC_COMMAND_PARSER_H_
#define INC_COMMAND_PARSER_H_

#include "global.h"

/* ============= ĐỊNH NGHĨA TRẠNG THÁI FSM ============= */
typedef enum {
    PARSER_INIT,           // Chờ ký tự bắt đầu '!'
    PARSER_WAIT_COMMAND    // Đang nhận lệnh cho đến '#'
} ParserState;

/* ============= KHAI BÁO HÀM ============= */

/**
 * @brief Khởi tạo command parser FSM
 */
void command_parser_init(void);

/**
 * @brief FSM phân tích lệnh từ buffer
 * Gọi hàm này trong main loop khi buffer_flag = 1
 */
void command_parser_fsm(void);

/**
 * @brief Lấy giá trị command flag hiện tại
 * @return CMD_NONE, CMD_RST, hoặc CMD_OK
 */
uint8_t get_command_flag(void);

/**
 * @brief Xóa command flag sau khi đã xử lý
 */
void clear_command_flag(void);


#endif /* INC_COMMAND_PARSER_H_ */
