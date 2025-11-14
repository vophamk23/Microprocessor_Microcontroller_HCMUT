/* ═══════════════════════════════════════════════════════════════════════════
 * FILE: uart_communication.c
 * MÔ TẢ: Quản lý giao thức truyền thông UART sử dụng FSM
 *
 * QUY TRÌNH GIAO TIẾP (Thực hiện Hành động in Dựa vào Chuỗi truyền vào):
 * 1. STM32 chờ nhận lệnh !RST# từ máy tính
 * 2. STM32 đọc ADC và gửi: !ADC=xxxx#
 * 3. STM32 chờ nhận xác nhận !OK# trong 3 giây
 * 4. Nếu quá 3 giây không nhận được !OK# → Gửi lại (dùng giá trị cũ)
 *
 * VÍ DỤ GIAO TIẾP THÀNH CÔNG:
 * PC → STM32: !RST#
 * STM32 → PC: !ADC=2048#
 * PC → STM32: !OK#
 * STM32 → PC: [SUCCESS] Communication complete!
 *
 * VÍ DỤ TIMEOUT:
 * PC → STM32: !RST#
 * STM32 → PC: !ADC=2048#
 * (Chờ 3 giây - không nhận !OK#)
 * STM32 → PC: [TIMEOUT] The value is kept as the previous packet.
 * STM32 → PC: !ADC=2048# (gửi lại giá trị cũ)
 * ═══════════════════════════════════════════════════════════════════════════ */

#include "uart_communication.h"
#include "command_parser.h"
#include "sensor.h"
#include "stdio.h"
#include "string.h"

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 1: BIẾN TOÀN CỤC (Static - chỉ dùng trong file này)
 * ═════════════════════════════════════════════════════════════════════════ */
static CommunicationState comm_state = COMM_IDLE; // Trạng thái hiện tại của FSM giao tiếp

static uint32_t timeout_start = 0; // Lưu thời điểm bắt đầu đếm timeout (đơn vị: millisecond)

static uint32_t last_adc_value = 0; // Lưu giá trị ADC đã gửi lần cuối (dùng khi timeout để gửi lại)

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 2: HÀM KHỞI TẠO
 * ═════════════════════════════════════════════════════════════════════════ */
void uart_communication_init(void)
{
    comm_state = COMM_WAIT_RST; // Ban đầu chờ lệnh !RST# từ PC
    timeout_start = 0;          // Chưa bắt đầu đếm timeout
    last_adc_value = 0;         // Chưa có giá trị ADC nào
}

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 3: HÀM GỬI PACKET ADC
 * ═════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Gửi giá trị ADC qua UART theo format: !ADC=xxxx#
 * @param value Giá trị ADC cần gửi (0-4095)
 *
 * Hàm này cũng lưu giá trị vừa gửi vào last_adc_value để dùng lại khi timeout
 */
void send_adc_packet(uint32_t value)
{
    char packet[50]; // Buffer tạm để tạo chuỗi

    // Tạo chuỗi theo format: !ADC=xxxx#
    // %lu = in số nguyên không dấu dạng long
    sprintf(packet, "!ADC=%lu#\r\n", value);

    // Gửi chuỗi qua UART (UART2 đã cấu hình ở CubeMX)
    // Timeout 1000ms = 1 giây
    HAL_UART_Transmit(&huart2, (uint8_t *)packet, strlen(packet), 1000);

    // Lưu giá trị vừa gửi để dùng khi retransmit (gửi lại)
    last_adc_value = value;
}

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 4: HÀM FSM CHÍNH - QUẢN LÝ GIAO TIẾP
 * ═════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Máy trạng thái FSM quản lý giao tiếp UART
 *
 * CÁC TRẠNG THÁI:
 * 1. COMM_IDLE: Trạng thái nhàn rỗi (chuyển ngay sang COMM_WAIT_RST)
 * 2. COMM_WAIT_RST: Chờ lệnh !RST# từ PC
 * 3. COMM_WAIT_OK: Chờ xác nhận !OK# trong 3 giây
 * 4. COMM_TIMEOUT: Xử lý timeout (gửi lại packet cũ)
 *
 * @note Gọi hàm này liên tục trong vòng lặp while(1)
 */
void uart_communication_fsm(void)
{

    // Lấy lệnh từ command parser.c (CMD_RST / CMD_OK / CMD_NONE)

    uint8_t cmd = get_command_flag();

    switch (comm_state)
    {

        /* ═════════════════════════════════════════════════════════════════
         * TRẠNG THÁI 1: COMM_IDLE
         * Trạng thái nhàn rỗi ban đầu
         * ═════════════════════════════════════════════════════════════════ */
    case COMM_IDLE:
        // Chuyển ngay sang chờ lệnh RST
        comm_state = COMM_WAIT_RST;
        break;

        /* ═════════════════════════════════════════════════════════════════
         * TRẠNG THÁI 2: COMM_WAIT_RST
         * Chờ lệnh !RST# từ máy tính
         * ═════════════════════════════════════════════════════════════════ */
    case COMM_WAIT_RST:
        if (cmd == CMD_RST)
        {
            /* ─────────────────────────────────────────────────────────
             *  NHẬN ĐƯỢC LỆNH !RST# → BẮT ĐẦU QUY TRÌNH
             * ───────────────────────────────────────────────────────── */

            // Bước 1: Xóa flag lệnh (tránh xử lý lại)
            clear_command_flag();

            // Bước 2: Đọc giá trị ADC từ cảm biến
            adc_value = read_adc_value();

            // Bước 3: Gửi thông báo debug (có thể bỏ qua)
            char debug_msg[50];
            sprintf(debug_msg, "[INFO] ADC Read: %lu\r\n", adc_value);
            HAL_UART_Transmit(&huart2, (uint8_t *)debug_msg, strlen(debug_msg), 100);

            // Bước 4: Gửi packet ADC: !ADC=xxxx#
            send_adc_packet(adc_value);

            // Bước 5: Bắt đầu đếm timeout (lưu thời điểm hiện tại)
            timeout_start = HAL_GetTick(); // HAL_GetTick() = millisecond

            // Bước 6: Chuyển sang trạng thái chờ OK
            comm_state = COMM_WAIT_OK;
        }

        // Nếu chưa nhận !RST# → Không làm gì, tiếp tục chờ
        break;

        /* ═════════════════════════════════════════════════════════════════
         * TRẠNG THÁI 3: COMM_WAIT_OK
         * Chờ xác nhận !OK# từ máy tính (timeout 3 giây)
         * ═════════════════════════════════════════════════════════════════ */
    case COMM_WAIT_OK:

        /* ─────────────────────────────────────────────────────────────
         * TRƯỜNG HỢP 1: Nhận được !OK# → Giao tiếp thành công
         * ───────────────────────────────────────────────────────────── */
        if (cmd == CMD_OK)
        {

            // Bước 1: Xóa flag lệnh
            clear_command_flag();

            // Bước 2: Gửi thông báo thành công
            char success_msg[] = "[SUCCESS] Communication complete!\r\n\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t *)success_msg, strlen(success_msg), 100);

            // Bước 3: Quay về chờ lệnh !RST# mới
            comm_state = COMM_WAIT_RST;
        }

        /* ─────────────────────────────────────────────────────────────
         * TRƯỜNG HỢP 2: Quá 3 giây chưa nhận !OK# → Timeout
         * ───────────────────────────────────────────────────────────── */
        else if (HAL_GetTick() - timeout_start >= TIMEOUT_DURATION)
        {
            // HAL_GetTick() - timeout_start = thời gian đã trôi qua
            comm_state = COMM_TIMEOUT; // Chuyển sang xử lý timeout
        }
        // Nếu chưa timeout và chưa nhận !OK# → Tiếp tục chờ
        break;

    /* ═════════════════════════════════════════════════════════════════
     * TRẠNG THÁI 4: COMM_TIMEOUT
     * Xử lý timeout - Gửi lại packet với giá trị cũ
     * ═════════════════════════════════════════════════════════════════ */
    case COMM_TIMEOUT:
    {
        /* ─────────────────────────────────────────────────────────────
         * ⚠️ TIMEOUT
         * ───────────────────────────────────────────────────────────── */

        // Bước 1: Thông báo timeout
        char timeout_msg[] = "[TIMEOUT] The value is kept as the previous packet.\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t *)timeout_msg,
                          strlen(timeout_msg), 100);

        // Bước 2: Gửi lại packet với giá trị cũ
        send_adc_packet(last_adc_value);

        // Bước 3: Reset timeout và chờ !OK# lại
        timeout_start = HAL_GetTick(); // Bắt đầu đếm timeout mới

        comm_state = COMM_WAIT_OK; // Quay lại chờ OK
        break;
    }

    /* ═════════════════════════════════════════════════════════════════
     * TRẠNG THÁI MẶC ĐỊNH: Xử lý lỗi
     * ═════════════════════════════════════════════════════════════════ */
    default:
        comm_state = COMM_IDLE; // Reset về trạng thái an toàn
        break;
    }
}

/* ═════════════════════════════════════════════════════════════════════════
 * PHẦN 5: HÀM HỖ TRỢ
 * ═════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Lấy trạng thái hiện tại của FSM
 * @return COMM_IDLE, COMM_WAIT_RST, COMM_WAIT_OK, hoặc COMM_TIMEOUT
 */

CommunicationState get_communication_state(void)
{
    return comm_state;
}
