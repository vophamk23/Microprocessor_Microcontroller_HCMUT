/* ═══════════════════════════════════════════════════════════════════════════
 * FILE: command_parser.c
 * MÔ TẢ: Bộ phân tích lệnh từ UART sử dụng Finite State Machine (FSM)
 *
 * CHỨC NĂNG CHÍNH (Xử Lí Chuỗi Nhập Từ Màn Hình):
 * - Đọc dữ liệu từ buffer UART (circular buffer)
 * - Phân tích các lệnh có format: !TÊN_LỆNH#
 * - Hỗ trợ 2 lệnh: !RST# và !OK#
 *
 * VÍ DỤ HOẠT ĐỘNG:
 * Buffer nhận: "abc!RST#xyz"
 * → Bỏ qua "abc", nhận "!RST#", bỏ qua "xyz"
 * → Kết quả: command_flag = CMD_RST
 * ═══════════════════════════════════════════════════════════════════════════ */

#include "command_parser.h"
#include "string.h"

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 1: BIẾN TOÀN CỤC (Static - chỉ dùng trong file này)
 * ═════════════════════════════════════════════════════════════════════════ */
static ParserState parser_state = PARSER_INIT; // Trạng thái hiện tại của máy FSM

static uint8_t cmd_index = 0; // Vị trí hiện tại khi ghi vào buffer tạm

static uint8_t temp_cmd[MAX_CMD_LENGTH]; // Buffer tạm để lưu lệnh đang nhận (ví dụ: "RST", "OK")

static uint8_t last_processed_index = 0; // Đánh dấu vị trí đã xử lý trong buffer UART

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 2: HÀM KHỞI TẠO
 * ═════════════════════════════════════════════════════════════════════════ */
void command_parser_init(void)
{
    parser_state = PARSER_INIT; // FSM bắt đầu ở trạng thái chờ '!'
    cmd_index = 0;              // Chưa có ký tự nào trong buffer tạm
    command_flag = CMD_NONE;    // Chưa có lệnh nào được nhận
    last_processed_index = 0;   // Bắt đầu từ đầu buffer

    // Xóa sạch các buffer
    memset(command_data, 0, MAX_CMD_LENGTH);
    memset(temp_cmd, 0, MAX_CMD_LENGTH);
}

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 3: HÀM FSM CHÍNH - XỬ LÝ PHÂN TÍCH LỆNH/ Chuỗi Ký Tự
 * ═════════════════════════════════════════════════════════════════════════ */
/**
 * @brief Máy trạng thái FSM phân tích lệnh từ buffer UART
 *
 * CÁCH HOẠT ĐỘNG:
 * 1. Duyệt qua TẤT CẢ ký tự mới trong buffer (chưa xử lý)
 * 2. Với mỗi ký tự, xử lý theo trạng thái hiện tại
 * 3. Cập nhật trạng thái và buffer tạm
 */
void command_parser_fsm(void)
{
    /* ─────────────────────────────────────────────────────────────────────
     * BƯỚC 1: Duyệt qua TẤT CẢ ký tự mới trong buffer
     * ───────────────────────────────────────────────────────────────────── */
    while (last_processed_index != index_buffer)
    {
        // Lấy ký tự tại vị trí chưa xử lý
        uint8_t received_char = buffer[last_processed_index];

        // Di chuyển con trỏ đến vị trí tiếp theo (circular buffer)
        last_processed_index++;
        if (last_processed_index >= MAX_BUFFER_SIZE)
        {
            last_processed_index = 0; // Quay về đầu buffer (vòng tròn)
        }

        /* ─────────────────────────────────────────────────────────────────
         * BƯỚC 2: Xử lý ký tự theo trạng thái FSM hiện tại
         * ───────────────────────────────────────────────────────────────── */
        switch (parser_state)
        {
        /* ═════════════════════════════════════════════════════════════
         * TRẠNG THÁI 1: PARSER_INIT
         * Đang chờ ký tự bắt đầu lệnh '!'
         * ═════════════════════════════════════════════════════════════ */
        case PARSER_INIT:

            if (received_char == '!')
            {
                // Phát hiện bắt đầu lệnh
                parser_state = PARSER_WAIT_COMMAND;  // Chuyển sang nhận lệnh
                cmd_index = 0;                       // Reset vị trí ghi
                memset(temp_cmd, 0, MAX_CMD_LENGTH); // Xóa buffer tạm
            }
            // Các ký tự khác → Bỏ qua
            // Ví dụ: Nếu nhận "abc!" thì bỏ qua "a", "b", "c"
            break;

            /* ═════════════════════════════════════════════════════════════
             * TRẠNG THÁI 2: PARSER_WAIT_COMMAND
             * Đang nhận các ký tự của lệnh (R, S, T, O, K, v.v.)
             * ═════════════════════════════════════════════════════════════ */
        case PARSER_WAIT_COMMAND:

            /* ─────────────────────────────────────────────────────────
             * TRƯỜNG HỢP 1: Nhận ký tự kết thúc '#'
             * ───────────────────────────────────────────────────────── */
            if (received_char == '#')
            {
                // Kết thúc chuỗi trong temp_cmd
                temp_cmd[cmd_index] = '\0';

                /* So sánh lệnh nhận được với các lệnh hợp lệ
                 *
                 * VÍ DỤ:
                 * - Nhận "!RST#" → temp_cmd = "RST" → CMD_RST
                 * - Nhận "!OK#"  → temp_cmd = "OK"  → CMD_OK
                 * - Nhận "!ABC#" → temp_cmd = "ABC" → CMD_NONE (không hợp lệ)
                 */
                if (strcmp((char *)temp_cmd, "RST") == 0)
                {
                    command_flag = CMD_RST;
                    strcpy((char *)command_data, "RST");
                }
                else if (strcmp((char *)temp_cmd, "OK") == 0)
                {
                    command_flag = CMD_OK;
                    strcpy((char *)command_data, "OK");
                }
                else
                {
                    // Lệnh không hợp lệ → Không làm gì
                    command_flag = CMD_NONE;
                }

                // Reset FSM để chờ lệnh mới
                parser_state = PARSER_INIT;
                cmd_index = 0;
            }

            /* ─────────────────────────────────────────────────────────
             * TRƯỜNG HỢP 2: Nhận ký tự '!' giữa chừng
             * Có thể do lệnh cũ bị lỗi, bắt đầu lệnh mới
             * ───────────────────────────────────────────────────────── */
            else if (received_char == '!')
            {
                // Restart: Bắt đầu nhận lệnh mới
                cmd_index = 0;
                memset(temp_cmd, 0, MAX_CMD_LENGTH);
                // Không đổi state, vẫn ở PARSER_WAIT_COMMAND
            }

            /* ─────────────────────────────────────────────────────────
             * TRƯỜNG HỢP 3: Nhận ký tự bình thường (R, S, T, O, K...)
             * ───────────────────────────────────────────────────────── */
            else
            {
                // Kiểm tra buffer còn chỗ không
                if (cmd_index < MAX_CMD_LENGTH - 1)
                {
                    temp_cmd[cmd_index++] = received_char; // Lưu ký tự
                }
                else
                {
                    // Buffer đầy → Lệnh quá dài → Reset
                    parser_state = PARSER_INIT;
                    cmd_index = 0;
                }
            }
            break;

        /* ═════════════════════════════════════════════════════════════
         * TRẠNG THÁI MẶC ĐỊNH: Xử lý lỗi
         * ═════════════════════════════════════════════════════════════ */
        default:
            parser_state = PARSER_INIT; // Reset về trạng thái an toàn
            break;
        }
    }
}

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 4: HÀM HỖ TRỢ
 * ═════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Lấy lệnh đã phân tích được
 * @return CMD_RST, CMD_OK, hoặc CMD_NONE
 */
uint8_t get_command_flag(void)
{
    return command_flag;
}

void clear_command_flag(void)
{
    command_flag = CMD_NONE;
    memset(command_data, 0, MAX_CMD_LENGTH);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * KẾT THÚC FILE
 * ═══════════════════════════════════════════════════════════════════════════ */
