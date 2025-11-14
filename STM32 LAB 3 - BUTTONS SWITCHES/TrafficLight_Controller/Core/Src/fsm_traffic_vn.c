///*
// * ============================================================================
// * fsm_traffic.c - HỆ THỐNG ĐIỀU KHIỂN ĐÈN GIAO THÔNG
// * ============================================================================
// *
// * MÔ TẢ TỔNG QUAN:
// * Hệ thống điều khiển đèn giao thông 2 đường giao nhau với 4 chế độ:
// *
// * MODE 1: Hoạt động tự động (đèn giao thông chuyển đổi tự động)
// * MODE 2: Điều chỉnh thời gian đèn ĐỎ
// * MODE 3: Điều chỉnh thời gian đèn VÀNG
// * MODE 4: Điều chỉnh thời gian đèn XANH
// *
// * RÀNG BUỘC QUAN TRỌNG: duration_RED = duration_GREEN + duration_AMBER
// *
// * GIAO DIỆN PHẦN CỨNG:
// * - Button 1 (MODE): Chuyển đổi giữa các chế độ
// * - Button 2 (MODIFY/INC): Tăng giá trị thời gian trong chế độ điều chỉnh
// * - Button 3 (SET): Lưu giá trị và quay về chế độ tự động
// * - LED: 6 LED điều khiển đèn giao thông (3 đèn x 2 đường)
// * - LED 7 đoạn: Hiển thị thời gian đếm ngược
// *
// * TẦN SUẤT CẬP NHẬT: 10ms (100Hz)
// * ============================================================================
// */
//
//#include "fsm_traffic.h"
//
///* ============================================================================
// * PHẦN 1: KHỞI TẠO HỆ THỐNG
// * ============================================================================ */
//
///**
// * traffic_init() - Khởi tạo toàn bộ hệ thống đèn giao thông
// *
// * CHỨC NĂNG:
// * 1. Thiết lập thời gian mặc định cho 3 loại đèn
// * 2. Đặt chế độ hoạt động ban đầu (MODE_1_NORMAL)
// * 3. Reset tất cả biến đếm và cờ hiệu
// * 4. Tắt tất cả LED để bắt đầu sạch sẽ
// * 5. Khởi tạo trạng thái nút nhấn
// *
// * ĐƯỢC GỌI: Một lần duy nhất trong main() trước khi vào vòng lặp chính
// *
// * LƯU Ý: Phải gọi hàm này trước khi sử dụng bất kỳ chức năng nào khác
// */
//void traffic_init(void)
//{
//	/* ------------------------------------------------------------------------
//	 * BƯỚC 1: THIẾT LẬP THỜI GIAN MẶC ĐỊNH
//	 * ------------------------------------------------------------------------
//	 */
//	duration_RED = 5;      // Đèn đỏ: 5 giây
//	duration_AMBER = 2;    // Đèn vàng: 2 giây
//	duration_GREEN = 3;    // Đèn xanh: 3 giây
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 2: THIẾT LẬP CHẾ ĐỘ HOẠT ĐỘNG BAN ĐẦU
//	 * ------------------------------------------------------------------------ */
//
//	// Bắt đầu ở chế độ hoạt động bình thường (tự động)
//    current_mode = MODE_1_NORMAL;
//	// Trạng thái khởi tạo - sẽ chuyển sang RED_GREEN trong lần chạy đầu tiên
//    traffic_state = INIT;
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 3: KHỞI TẠO CÁC COUNTER (BỘ ĐẾM THỜI GIAN)
//	 * ------------------------------------------------------------------------
//	 * counter_road1: Hiển thị thời gian còn lại của đèn trên đường 1
//	 * counter_road2: Hiển thị thời gian còn lại của đèn trên đường 2
//	 */
//    counter_road1 = 0;
//    counter_road2 = 0;
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 4: TẮT TẤT CẢ LED
//	 * ------------------------------------------------------------------------
//	 */
//    turn_off_all_leds();
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 5: KHỞI TẠO HỆ THỐNG PHÁT HIỆN NÚT NHẤN (EDGE DETECTION)
//	 * ------------------------------------------------------------------------
//	 * Cơ chế edge detection yêu cầu 2 biến:
//	 * - prevState[]: Trạng thái nút ở lần quét trước
//	 * - currState[]: Trạng thái nút ở lần quét hiện tại
//	 *
//	 * Phát hiện sự kiện "vừa nhấn": prevState=RELEASE && currState=PRESS
//	 *
//	 * prevState[0], currState[0]: Nút MODE (Button 1)
//	 * prevState[1], currState[1]: Nút MODIFY (Button 2)
//	 * prevState[2], currState[2]: Nút SET (Button 3)
//	 */
//    prevState[0] = BTN_RELEASE;     // Nút MODE
//    prevState[1] = BTN_RELEASE;     // Nút MODIFY/INC
//    prevState[2] = BTN_RELEASE;     // Nút SET
//
//    currState[0] = BTN_RELEASE;
//    currState[1] = BTN_RELEASE;
//    currState[2] = BTN_RELEASE;
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 6: KHỞI TẠO HỆ THỐNG NHẤP NHÁY LED (BLINKING)
//	 * ------------------------------------------------------------------------
//	 * Trong chế độ điều chỉnh, LED sẽ nhấp nháy với chu kỳ:
//	 * - 0.25s sáng + 0.25s tắt = 0.5s một chu kỳ
//	 *
//	 * blink_counter: Đếm số lần gọi timer (mỗi 10ms)
//	 *                50 lần = 500ms = 0.5 giây
//	 * flag_blink: 0 = LED tắt, 1 = LED sáng
//	 */
//    blink_counter = 0;
//    flag_blink = 0;
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 7: RESET TẤT CẢ CỜ HIỆU LED
//	 * ------------------------------------------------------------------------
//	 * Mỗi loại đèn có 2 cờ (cho 2 đường):
//	 * - flagRed[0], flagRed[1]: Điều khiển LED đỏ đường 1 và 2
//	 * - flagGreen[0], flagGreen[1]: Điều khiển LED xanh đường 1 và 2
//	 * - flagYellow[0], flagYellow[1]: Điều khiển LED vàng đường 1 và 2
//	 *
//	 * Giá trị: 0 = tắt, 1 = sáng
//	 */
//    flagRed[0] = flagRed[1] = 0;
//    flagGreen[0] = flagGreen[1] = 0;
//    flagYellow[0] = flagYellow[1] = 0;
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 8: RESET BIẾN TẠM
//	 * ------------------------------------------------------------------------
//	 * temp_duration: Lưu giá trị tạm thời trong chế độ điều chỉnh
//	 *                Chỉ lưu vào duration_* khi nhấn nút SET
//	 */
//    temp_duration = 0;
//}
//
///* ============================================================================
// * PHẦN 2: HÀM CHÍNH - TRÁI TIM CỦA HỆ THỐNG
// * ============================================================================ */
//
///**
// * traffic_run() - Hàm chạy chính được gọi định kỳ mỗi 10ms
// *
// * LUỒNG XỬ LÝ (thứ tự QUAN TRỌNG):
// *
// * 1. update_button_state()    → Đọc trạng thái nút nhấn
// *    └─ Phải đọc TRƯỚC để edge detection hoạt động chính xác
// *
// * 2. fsm_*_mode()            → Xử lý logic theo chế độ hiện tại
// *    ├─ fsm_normal_mode()    → Chế độ tự động
// *    ├─ fsm_red_modify_mode()   → Điều chỉnh đèn đỏ
// *    ├─ fsm_amber_modify_mode() → Điều chỉnh đèn vàng
// *    └─ fsm_green_modify_mode() → Điều chỉnh đèn xanh
// *
// * 3. update_led_display()    → Bật/tắt LED theo cờ hiệu
// *    └─ Cập nhật phần cứng dựa trên flagRed[], flagGreen[], flagYellow[]
// *
// * 4. update_7seg_display()   → Hiển thị số trên LED 7 đoạn
// *    └─ Hiển thị counter_road1/2 hoặc temp_duration tùy theo mode
// *
// * ĐƯỢC GỌI: Trong timer interrupt handler, mỗi 10ms (tần suất 100Hz)
// * TẦN SUẤT: 100 lần/giây = 100Hz
// */
//void traffic_run(void)
//{
//	/* ------------------------------------------------------------------------
//	 * BƯỚC 1: ĐỌC TRẠNG THÁI NÚT NHẤN
//	 * ------------------------------------------------------------------------
//	 */
//	update_button_state();
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 2: XỬ LÝ LOGIC THEO CHẾ ĐỘ HIỆN TẠI
//	 * ------------------------------------------------------------------------
//	 * Mỗi mode có một FSM riêng để xử lý logic:
//	 *
//	 * MODE 1: Tự động chuyển đèn, xử lý nút MODE để vào chế độ điều chỉnh
//	 * MODE 2-4: Xử lý nút MODIFY (tăng giá trị), SET (lưu), MODE (chuyển mode)
//	 */
//    switch(current_mode) {
//        case MODE_1_NORMAL:
//            fsm_normal_mode();    // Chế độ hoạt động tự động
//            break;
//
//        case MODE_2_RED_MODIFY:
//            fsm_red_modify_mode();    // Điều chỉnh thời gian đèn đỏ
//            break;
//
//        case MODE_3_AMBER_MODIFY:
//            fsm_amber_modify_mode();  // Điều chỉnh thời gian đèn vàng
//            break;
//
//        case MODE_4_GREEN_MODIFY:
//            fsm_green_modify_mode();  // Điều chỉnh thời gian đèn xanh
//            break;
//    }
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 3: CẬP NHẬT HIỂN THỊ LED
//	 * ------------------------------------------------------------------------
//	 * Bật/tắt LED phần cứng dựa trên giá trị của các cờ:
//	 * - flagRed[0], flagRed[1]
//	 * - flagGreen[0], flagGreen[1]
//	 * - flagYellow[0], flagYellow[1]
//	 */
//    update_led_display();
//
//    /* ------------------------------------------------------------------------
//	 * BƯỚC 4: CẬP NHẬT HIỂN THỊ LED 7 ĐOẠN
//	 * ------------------------------------------------------------------------
//	 * Hiển thị số trên LED 7 đoạn:
//	 * - MODE 1: Hiển thị counter_road1, counter_road2 (đếm ngược)
//	 * - MODE 2-4: Hiển thị temp_duration (giá trị đang điều chỉnh)
//	 */
//    update_7seg_display();
//}
//
///* ============================================================================
// * PHẦN 3: FSM MODE 1 - CHẾ ĐỘ HOẠT ĐỘNG TỰ ĐỘNG
// * ============================================================================ */
//
///**
// * fsm_normal_mode() - Máy trạng thái cho chế độ hoạt động tự động
// *
// * CHỨC NĂNG CHÍNH:
// * 1. Tự động chuyển đổi giữa các trạng thái đèn giao thông
// * 2. Đếm ngược thời gian mỗi giây (1000ms)
// * 3. Cập nhật counter_road1 và counter_road2
// * 4. Xử lý nút MODE để chuyển sang chế độ điều chỉnh
// *
// * CHU TRÌNH ĐÈN GIAO THÔNG (4 trạng thái):
// *
// *   INIT (khởi tạo)
// *     ↓
// *   RED_GREEN (Đường 1: ĐỎ, Đường 2: XANH)
// *     ↓ (khi đèn xanh đường 2 hết)
// *   RED_AMBER (Đường 1: ĐỎ, Đường 2: VÀNG)
// *     ↓ (khi đèn vàng đường 2 hết)
// *   GREEN_RED (Đường 1: XANH, Đường 2: ĐỎ)
// *     ↓ (khi đèn xanh đường 1 hết)
// *   AMBER_RED (Đường 1: VÀNG, Đường 2: ĐỎ)
// *     ↓ (khi đèn vàng đường 1 hết)
// *   RED_GREEN (quay lại chu trình)
// *
// * THỜI GIAN CẬP NHẬT: 1 giây (đếm ngược mỗi giây)
// */
//void fsm_normal_mode(void)
//{
//    /* ------------------------------------------------------------------------
//	 * BIẾN TĨNH (STATIC) - GIỮ GIÁ TRỊ QUA CÁC LẦN GỌI HÀM
//	 * ------------------------------------------------------------------------
//	 * timer_counter: Đếm số lần gọi hàm traffic_run()
//	 *                Mỗi lần gọi = 10ms
//	 *                100 lần = 1000ms = 1 giây
//	 *
//	 * TIMER_CYCLE: Ngưỡng để xác định đã đủ 1 giây
//	 *              = 100 (100 x 10ms = 1000ms = 1s)
//	 */
//    static int timer_counter = 0;
//
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT MODE - CHUYỂN SANG CHẾ ĐỘ ĐIỀU CHỈNH
//	 * ------------------------------------------------------------------------
//	 * Điều kiện phát hiện "vừa nhấn" (rising edge):
//	 * - prevState[0] == BTN_RELEASE: Nút chưa nhấn ở lần quét trước
//	 * - currState[0] == BTN_PRESS: Nút đang nhấn ở lần quét hiện tại
//	 * → KẾT LUẬN: Người dùng VỪA NHẤN nút (không phải đang giữ)
//	 *
//	 * Khi phát hiện sự kiện nhấn:
//	 * 1. Chuyển sang MODE 2 (điều chỉnh đèn đỏ)
//	 * 2. Load giá trị hiện tại vào temp_duration
//	 * 3. Tắt tất cả LED (chuẩn bị hiệu ứng nhấp nháy)
//	 * 4. return ngay để không xử lý logic đèn giao thông
//	 */
//    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
//        // Chuyển sang chế độ điều chỉnh đèn đỏ
//        current_mode = MODE_2_RED_MODIFY;
//
//        // Load giá trị hiện tại để bắt đầu điều chỉnh
//        temp_duration = duration_RED;
//
//        // Tắt tất cả LED trước khi vào chế độ mới
//        turn_off_all_leds();
//
//        return;  // Thoát ngay, không xử lý phần còn lại
//    }
//
//    /* ------------------------------------------------------------------------
//	 * ĐẾM THỜI GIAN ĐỂ XÁC ĐỊNH ĐÃ ĐỦ 1 GIÂY CHƯA
//	 * ------------------------------------------------------------------------
//	 * Mỗi lần gọi traffic_run() (10ms) → tăng timer_counter
//	 * Khi timer_counter = 100 → đã đủ 1 giây
//	 */
//    timer_counter++;
//    // Nếu chưa đủ 1 giây → thoát và chờ
//    if(timer_counter < TIMER_CYCLE) {
//        return;  // Chưa đủ thời gian, không làm gì cả
//    }
//    /* ------------------------------------------------------------------------
//	 * ĐÃ ĐỦ 1 GIÂY - RESET COUNTER VÀ XỬ LÝ FSM
//	 * ------------------------------------------------------------------------ */
//    timer_counter = 0;  // Reset để bắt đầu đếm chu kỳ mới
//
//
//    /* ------------------------------------------------------------------------
//	 * FSM ĐÈN GIAO THÔNG - CẬP NHẬT MỖI GIÂY
//	 * ------------------------------------------------------------------------ */
//    switch(traffic_state) {
//        /* ====================================================================
//         * TRẠNG THÁI INIT - KHỞI TẠO BAN ĐẦU
//         * ====================================================================
//         * Chỉ chạy 1 lần duy nhất khi:
//         * - Vừa khởi động hệ thống, hoặc
//         * - Vừa thoát khỏi chế độ điều chỉnh về MODE 1
//         *
//         * Nhiệm vụ:
//         * - Chuyển sang trạng thái đầu tiên của chu trình (RED_GREEN)
//         * - Khởi tạo counter cho 2 đường
//         */
//        case INIT:
//            // Chuyển sang trạng thái đầu tiên
//            traffic_state = RED_GREEN;
//
//            // Khởi tạo thời gian đếm ngược cho 2 đường
//            counter_road1 = duration_RED;    // Đường 1: đèn đỏ
//            counter_road2 = duration_GREEN;  // Đường 2: đèn xanh
//            break;
//
//        /* ====================================================================
//         * TRẠNG THÁI RED_GREEN
//         * ====================================================================
//         * Đường 1: ĐÈN ĐỎ    (counter_road1 = duration_RED)
//         * Đường 2: ĐÈN XANH  (counter_road2 = duration_GREEN)
//         */
//        case RED_GREEN:
//            // Giảm cả 2 counter
//            counter_road1--;  // Đếm ngược đèn đỏ đường 1
//            counter_road2--;  // Đếm ngược đèn xanh đường 2
//
//            // Kiểm tra điều kiện chuyển trạng thái
//            if(counter_road2 <= 0) {
//                // Đèn xanh đường 2 đã hết → chuyển sang đèn vàng
//                traffic_state = RED_AMBER;
//
//                // Cập nhật counter cho trạng thái mới
//                counter_road1 = duration_AMBER;  // Đường 1 còn thời gian đèn vàng
//                counter_road2 = duration_AMBER;  // Đường 2 bắt đầu đèn vàng
//            }
//            break;
//
//        /* ====================================================================
//         * TRẠNG THÁI RED_AMBER
//         * ====================================================================
//         * Đường 1: ĐÈN ĐỎ    (counter_road1 = duration_AMBER còn lại)
//         * Đường 2: ĐÈN VÀNG  (counter_road2 = duration_AMBER)
//         */
//        case RED_AMBER:
//            counter_road1--;
//            counter_road2--;
//
//            // Khi đèn vàng đường 2 hết → đổi lượt
//            if(counter_road2 <= 0) {
//                traffic_state = GREEN_RED;
//
//                // Đường 1 được đi (đèn xanh), đường 2 dừng (đèn đỏ)
//                counter_road1 = duration_GREEN;  // Đường 1: đèn xanh
//                counter_road2 = duration_RED;    // Đường 2: đèn đỏ
//            }
//            break;
//
//        /* ====================================================================
//         * TRẠNG THÁI GREEN_RED
//         * ====================================================================
//         */
//        case GREEN_RED:
//            counter_road1--;
//            counter_road2--;
//
//            // Khi đèn xanh đường 1 hết → chuyển sang đèn vàng
//            if(counter_road1 <= 0) {
//                traffic_state = AMBER_RED;
//
//                counter_road1 = duration_AMBER;  // Đường 1: đèn vàng
//                counter_road2 = duration_AMBER;  // Đường 2 còn thời gian đèn đỏ
//            }
//            break;
//
//        /* ====================================================================
//         * TRẠNG THÁI AMBER_RED
//         * ====================================================================
//         * Đường 1: ĐÈN VÀNG  (counter_road1 = duration_AMBER)
//         * Đường 2: ĐÈN ĐỎ    (counter_road2 = duration_AMBER còn lại)
//         */
//        case AMBER_RED:
//            counter_road1--;
//            counter_road2--;
//
//            // Khi đèn vàng đường 1 hết → quay lại chu trình ban đầu
//            if(counter_road2 <= 0) {
//                traffic_state = RED_GREEN;
//
//                // Bắt đầu lại từ đầu
//                counter_road1 = duration_RED;    // Đường 1: đèn đỏ
//                counter_road2 = duration_GREEN;  // Đường 2: đèn xanh
//            }
//            break;
//    }
//
//    /* ------------------------------------------------------------------------
//	 * BẢO VỆ - ĐẢM BẢO COUNTER KHÔNG ÂM
//	 * ------------------------------------------------------------------------
//	 * Trong một số trường hợp edge case, counter có thể bị âm
//	 * (do timing hoặc lỗi logic)
//	 *
//	 * Đảm bảo counter >= 0 để:
//	 * - LED 7 đoạn hiển thị đúng (không hiện số âm)
//	 * - Tránh lỗi hiển thị kỳ lạ
//	 */
//    if(counter_road1 < 0) counter_road1 = 0;
//    if(counter_road2 < 0) counter_road2 = 0;
//}
//
///* ============================================================================
// * PHẦN 4: FSM MODE 2 - ĐIỀU CHỈNH THỜI GIAN ĐÈN ĐỎ
// * ============================================================================ */
//
///**
// * fsm_red_modify_mode() - Máy trạng thái cho chế độ điều chỉnh đèn đỏ
// *
// * CHỨC NĂNG:
// * 1. Hiển thị temp_duration trên LED 7 đoạn
// * 2. Nhấp nháy LED đỏ (hiệu ứng)
// * 3. Xử lý 3 nút nhấn:
// *    - Nút MODE: Chuyển sang MODE 3 (điều chỉnh đèn vàng)
// *    - Nút MODIFY: Tăng temp_duration (1→99, rồi quay về 1)
// *    - Nút SET: Lưu giá trị và tự động điều chỉnh 2 đèn còn lại
// *
// * CƠ CHẾ TỰ ĐỘNG ĐIỀU CHỈNH KHI LƯU:
// * - Lưu duration_RED = temp_duration
// * - Tự động tính: duration_GREEN = duration_RED - duration_AMBER
// * - Nếu không hợp lệ → Reset về mặc định
// * - Quay về MODE 1
// */
//void fsm_red_modify_mode(void)
//{
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT MODE - CHUYỂN SANG MODE 3 (ĐIỀU CHỈNH ĐÈN VÀNG)
//	 * ------------------------------------------------------------------------
//	 */
//    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
//        // Chuyển sang chế độ điều chỉnh đèn vàng
//        current_mode = MODE_3_AMBER_MODIFY;
//
//        // Load giá trị đèn vàng hiện tại
//        temp_duration = duration_AMBER;
//
//        return;  // Thoát ngay
//    }
//
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT MODIFY/INC - TĂNG GIÁ TRỊ THỜI GIAN
//	 * ------------------------------------------------------------------------
//	 * Edge detection đảm bảo:
//	 * - Nhấn 1 lần = tăng 1 lần (không bị tăng liên tục khi giữ nút)
//	 */
//    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
//        temp_duration++;  // Tăng giá trị
//        // Kiểm tra vượt giới hạn → quay về 1
//        if(temp_duration > 99) {
//            temp_duration = 1;
//        }
//    }
//
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT SET - LƯU GIÁ TRỊ VÀ TỰ ĐỘNG ĐIỀU CHỈNH
//	 * ------------------------------------------------------------------------
//	 * Quy trình 3 bước:
//	 *
//	 * BƯỚC 1: Lưu giá trị đèn đỏ mới
//	 *         duration_RED = temp_duration
//	 *
//	 * BƯỚC 2: Tự động điều chỉnh 2 đèn còn lại (GREEN và AMBER)
//	 *         Chiến lược: Giữ AMBER, tính GREEN = RED - AMBER
//	 *         VD: RED=10, AMBER=2 → GREEN=8
//	 *
//	 * BƯỚC 3: Quay về MODE 1 (hoạt động bình thường)
//	 *         - Reset traffic_state = INIT
//	 *         - Tắt tất cả LED (chuẩn bị cho chu trình mới)
//	 */
//    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
//        // ===== BƯỚC 1: LƯU GIÁ TRỊ ĐÈN ĐỎ MỚI =====
//        duration_RED = temp_duration;
//
//        // ===== BƯỚC 2: TỰ ĐỘNG ĐIỀU CHỈNH 2 ĐÈN CÒN LẠI =====
//        // Gọi hàm auto_adjust_duration() với tham số 0 (đã chỉnh đèn ĐỎ)
//        // Hàm sẽ tự động tính toán GREEN và AMBER để thỏa mãn:
//        // RED = GREEN + AMBER
//        auto_adjust_duration(0);
//
//        // ===== BƯỚC 3: QUAY VỀ MODE 1 =====
//        current_mode = MODE_1_NORMAL;    // Chuyển về chế độ tự động
//        traffic_state = INIT;             // Reset trạng thái (sẽ khởi tạo lại counter)
//        turn_off_all_leds();              // Tắt LED trước khi bắt đầu chu trình mới
//
//        return;  // Thoát ngay
//    }
//
//    /* ------------------------------------------------------------------------
//	 * HIỆU ỨNG NHẤP NHÁY LED ĐỎ
//	 * ------------------------------------------------------------------------
//	 * Gọi hàm handle_led_blinking() với tham số 2 (đèn đỏ)
//	 * Kết quả: LED đỏ nhấp nháy với chu kỳ 1 giây (0.5s sáng + 0.5s tắt)
//	 */
//    handle_led_blinking(0);  // 2 = RED LED
//}
//
///* ============================================================================
// * PHẦN 5: FSM MODE 3 - ĐIỀU CHỈNH THỜI GIAN ĐÈN VÀNG
// * ============================================================================ */
//
///**
// * fsm_amber_modify_mode() - Máy trạng thái cho chế độ điều chỉnh đèn vàng
// *
// * CHỨC NĂNG:
// * 1. Hiển thị temp_duration trên LED 7 đoạn
// * 2. Nhấp nháy LED vàng (hiệu ứng)
// * 3. Xử lý 3 nút nhấn:
// *    - Nút MODE: Chuyển sang MODE 4 (điều chỉnh đèn xanh)
// *    - Nút MODIFY: Tăng temp_duration (1→99→1)
// *    - Nút SET: Lưu giá trị và tự động điều chỉnh
// *
// * CƠ CHẾ TỰ ĐỘNG ĐIỀU CHỈNH KHI LƯU:
// * - Lưu duration_AMBER = temp_duration
// * - Tự động tính: duration_RED = duration_GREEN + duration_AMBER (mới)
// * - Giữ nguyên duration_GREEN
// * - Nếu RED > 99 → điều chỉnh GREEN
// */
//void fsm_amber_modify_mode(void)
//{
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT MODE - CHUYỂN SANG MODE 4 (ĐIỀU CHỈNH ĐÈN XANH)
//	 * ------------------------------------------------------------------------ */
//    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
//        current_mode = MODE_4_GREEN_MODIFY;  // Chuyển sang điều chỉnh đèn xanh
//        temp_duration = duration_GREEN;       // Load giá trị đèn xanh hiện tại
//        return;
//    }
//
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT MODIFY - TĂNG GIÁ TRỊ
//	 * ------------------------------------------------------------------------
//	 * Tăng từ 1 → 99, sau đó quay về 1
//	 */
//    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
//        temp_duration++;
//        if(temp_duration > 99) temp_duration = 1;
//    }
//
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT SET - LƯU VÀ TỰ ĐỘNG ĐIỀU CHỈNH
//	 * ------------------------------------------------------------------------
//	 * Quy trình:
//	 * 1. Lưu duration_AMBER = temp_duration
//	 * 2. Tự động điều chỉnh: duration_RED = duration_GREEN + duration_AMBER
//	 * 3. Quay về MODE 1
//	 */
//    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
//        // BƯỚC 1: Lưu giá trị đèn vàng mới
//        duration_AMBER = temp_duration;
//
//        // BƯỚC 2: Tự động điều chỉnh
//        // Tham số 1 = vừa chỉnh đèn VÀNG
//        // Chiến lược: Giữ GREEN, tính RED = GREEN + AMBER (mới)
//        // VD: GREEN=5, AMBER(mới)=3 → RED=8
//        auto_adjust_duration(1);
//
//        // BƯỚC 3: Quay về MODE 1
//        current_mode = MODE_1_NORMAL;
//        traffic_state = INIT;
//        turn_off_all_leds();
//        return;
//    }
//
//    /* ------------------------------------------------------------------------
//	 * HIỆU ỨNG NHẤP NHÁY LED VÀNG
//	 * ------------------------------------------------------------------------
//	 * Tham số 1 = AMBER/YELLOW LED
//	 */
//    handle_led_blinking(1);  // 1 = AMBER/YELLOW LED
//}
//
///* ============================================================================
// * PHẦN 6: FSM MODE 4 - ĐIỀU CHỈNH THỜI GIAN ĐÈN XANH
// * ============================================================================ */
//
///**
// * fsm_green_modify_mode() - Máy trạng thái cho chế độ điều chỉnh đèn xanh
// *
// * CHỨC NĂNG:
// * 1. Hiển thị temp_duration trên LED 7 đoạn
// * 2. Nhấp nháy LED xanh (hiệu ứng)
// * 3. Xử lý 3 nút nhấn:
// *    - Nút MODE: Quay về MODE 1 (không lưu)
// *    - Nút MODIFY: Tăng temp_duration (1→99→1)
// *    - Nút SET: Lưu giá trị và tự động điều chỉnh
// *
// * CƠ CHẾ TỰ ĐỘNG ĐIỀU CHỈNH KHI LƯU:
// * - Lưu duration_GREEN = temp_duration
// * - Tự động tính: duration_RED = duration_GREEN (mới) + duration_AMBER
// * - Giữ nguyên duration_AMBER
// * - Nếu RED > 99 → điều chỉnh AMBER
// */
//void fsm_green_modify_mode(void)
//{
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT MODE - QUAY VỀ MODE 1 (KHÔNG LƯU)
//	 * ------------------------------------------------------------------------
//	 * Đây là mode cuối cùng trong chuỗi điều chỉnh
//	 * Nếu nhấn MODE → Quay về chế độ tự động mà KHÔNG lưu giá trị
//	 * (Khác với MODE 2 và MODE 3 chuyển sang mode kế tiếp)
//	 */
//    if(currState[0] == BTN_PRESS && prevState[0] == BTN_RELEASE) {
//        current_mode = MODE_1_NORMAL;  // Quay về chế độ tự động
//        traffic_state = INIT;           // Reset trạng thái
//        turn_off_all_leds();            // Tắt LED
//        return;
//    }
//
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT MODIFY - TĂNG GIÁ TRỊ
//	 * ------------------------------------------------------------------------ */
//    if(currState[1] == BTN_PRESS && prevState[1] == BTN_RELEASE) {
//        temp_duration++;
//        if(temp_duration > 99) temp_duration = 1;
//    }
//
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ NÚT SET - LƯU VÀ TỰ ĐỘNG ĐIỀU CHỈNH
//	 * ------------------------------------------------------------------------
//	 * Quy trình:
//	 * 1. Lưu duration_GREEN = temp_duration
//	 * 2. Tự động điều chỉnh: duration_RED = duration_GREEN + duration_AMBER
//	 * 3. Quay về MODE 1
//	 */
//    if(currState[2] == BTN_PRESS && prevState[2] == BTN_RELEASE) {
//        // BƯỚC 1: Lưu giá trị đèn xanh mới
//        duration_GREEN = temp_duration;
//
//        // BƯỚC 2: Tự động điều chỉnh
//        // Tham số 2 = vừa chỉnh đèn XANH
//        // Chiến lược: Giữ AMBER, tính RED = GREEN (mới) + AMBER
//        // VD: GREEN(mới)=7, AMBER=2 → RED=9
//        auto_adjust_duration(2);
//
//        // BƯỚC 3: Quay về MODE 1
//        current_mode = MODE_1_NORMAL;
//        traffic_state = INIT;
//        turn_off_all_leds();
//        return;
//    }
//
//    /* ------------------------------------------------------------------------
//	 * HIỆU ỨNG NHẤP NHÁY LED XANH
//	 * ------------------------------------------------------------------------
//	 * Tham số 0 = GREEN LED
//	 */
//    handle_led_blinking(2);  // 0 = GREEN LED
//}
//
///* ============================================================================
// * PHẦN 7: HÀM TỰ ĐỘNG ĐIỀU CHỈNH THỜI GIAN
// * ============================================================================ */
//
///**
// * auto_adjust_duration() - Tự động điều chỉnh 2 đèn còn lại để đảm bảo ràng buộc
// *
// * RÀNG BUỘC QUAN TRỌNG: duration_RED = duration_GREEN + duration_AMBER
// *
// * Khi người dùng thay đổi 1 trong 3 đèn, hàm này tự động tính toán lại
// * 2 đèn còn lại để đảm bảo ràng buộc luôn được thỏa mãn.
// *
// * CHIẾN LƯỢC ĐIỀU CHỈNH:
// *
// * ┌─────────────────────────────────────────────────────────────────────────┐
// * │ TRƯỜNG HỢP 1: VỪA CHỈNH ĐÈN ĐỎ (modified_light = 2)                     │
// * ├─────────────────────────────────────────────────────────────────────────┤
// * │ Chiến lược: Giữ AMBER, tính GREEN = RED - AMBER                         │
// * │                                                                         │
// * │ Ví dụ:                                                                  │
// * │   RED(mới) = 10, AMBER(cũ) = 2                                          │
// * │   → GREEN = 10 - 2 = 8 ✓                                                │
// * │                                                                         │
// * │ Nếu GREEN không hợp lệ (<1 hoặc >99):                                   │
// * │   → Thử giữ GREEN, tính AMBER = RED - GREEN                             │
// * │   → Nếu vẫn không hợp lệ: Reset về mặc định (5-3-2)                     │
// * └─────────────────────────────────────────────────────────────────────────┘
// *
// * ┌─────────────────────────────────────────────────────────────────────────┐
// * │ TRƯỜNG HỢP 2: VỪA CHỈNH ĐÈN VÀNG (modified_light = 1)                   │
// * ├─────────────────────────────────────────────────────────────────────────┤
// * │ Chiến lược:GREEN(mới) = AMBER(mới) + delta x , tính RED = GREEN + AMBER │
// * │                                                                         │
// * │ Ví dụ:                                                                  │
// * │   AMBER(mới) = 3, GREEN(mới) = 3 + x                                         │
// * │   → RED = 3 + 3 + x = 8 + x ✓                                                   │
// * │                                                                         │
// * │ Nếu RED > 99:                                                           │
// * │   → Giảm GREEN để RED = 99                                              │
// * │   → GREEN = 99 - AMBER                                                  │
// * │   VD: AMBER=30 → GREEN=69, RED=99                                       │
// * │                                                                         │
// * │ Nếu GREEN < 1:                                                          │
// * │   → Reset về mặc định                                                   │
// * └─────────────────────────────────────────────────────────────────────────┘
// *
// * ┌─────────────────────────────────────────────────────────────────────────┐
// * │ TRƯỜNG HỢP 3: VỪA CHỈNH ĐÈN XANH (modified_light = 0)                   │
// * ├─────────────────────────────────────────────────────────────────────────┤
// * │ Chiến lược: Giữ AMBER, tính RED = GREEN + AMBER                         │
// * │                                                                         │
// * │ Ví dụ:                                                                  │
// * │   GREEN(mới) = 7, AMBER(cũ) = 2                                         │
// * │   → RED = 7 + 2 = 9 ✓                                                   │
// * │                                                                         │
// * │ Nếu RED > 99:                                                           │
// * │   → Giảm AMBER để RED = 99                                              │
// * │   → AMBER = 99 - GREEN                                                  │
// * │   VD: GREEN=80 → AMBER=19, RED=99                                       │
// * │                                                                         │
// * │ Nếu AMBER < 1:                                                          │
// * │   → Reset về mặc định                                                   │
// * └─────────────────────────────────────────────────────────────────────────┘
// *
// * THAM SỐ:
// *   modified_light: Đèn vừa được điều chỉnh
// *                   0 = GREEN (xanh)
// *                   1 = AMBER (vàng)
// *                   2 = RED (đỏ)
// *
// * TRẢ VỀ:
// *   1: Đã điều chỉnh hoặc reset
// *   0: Không cần điều chỉnh (đã đúng ràng buộc)
// *
// * GIÁ TRỊ MẶC ĐỊNH KHI RESET: RED=5, GREEN=3, AMBER=2
// */
//int auto_adjust_duration(int modified_light)
//{
//    /* ------------------------------------------------------------------------
//	 * KIỂM TRA RÀNG BUỘC - NẾU ĐÃ ĐÚNG THÌ KHÔNG CẦN ĐIỀU CHỈNH
//	 * ------------------------------------------------------------------------
//	 * Trước khi làm bất cứ điều gì, kiểm tra xem ràng buộc có đang
//	 * thỏa mãn hay không. Nếu đã đúng → return 0 (không làm gì)
//	 */
//    if(duration_RED == (duration_GREEN + duration_AMBER)) {
//        return 0;  // Đã đúng ràng buộc, không cần điều chỉnh
//    }
//
//    /* ------------------------------------------------------------------------
//	 * XỬ LÝ THEO LOẠI ĐÈN VỪA ĐƯỢC ĐIỀU CHỈNH
//	 * ------------------------------------------------------------------------ */
//    switch(modified_light) {
//
//        /* ====================================================================
//         * CASE 0: VỪA CHỈNH ĐÈN ĐỎ
//         * ==================================================================== */
//        case 0:
//            /* ----------------------------------------------------------------
//			 * CHIẾN LƯỢC: Ưu tiên giữ AMBER, tính lại GREEN
//			 * ----------------------------------------------------------------
//			 * Công thức: GREEN = RED - AMBER
//			 *
//			 * Ví dụ 1 (thành công):
//			 *   RED=10, AMBER=2 → GREEN=8 ✓
//			 *
//			 * Ví dụ 2 (thất bại - GREEN âm):
//			 *   RED=3, AMBER=5 → GREEN=-2 ✗
//			 *   → Phải thử cách khác
//			 */
//            duration_GREEN = duration_RED - duration_AMBER;
//
//            /* ----------------------------------------------------------------
//			 * KIỂM TRA TÍNH HỢP LỆ CỦA GREEN
//			 * ----------------------------------------------------------------
//			 * Điều kiện hợp lệ: 1 ≤ GREEN ≤ 99
//			 */
//            if(duration_GREEN < 1 || duration_GREEN > 99) {
//                /* ------------------------------------------------------------
//				 * GREEN KHÔNG HỢP LỆ - THỬ CÁCH KHÁC
//				 * ------------------------------------------------------------
//				 * Chiến lược dự phòng: Giữ GREEN, tính lại AMBER
//				 * Công thức: AMBER = RED - GREEN
//				 *
//				 * LƯU Ý: Dòng này có vẻ dư thừa vì GREEN vừa được tính
//				 * nhưng nó đảm bảo logic rõ ràng
//				 */
//                duration_GREEN = duration_RED - duration_AMBER;  // Khôi phục giá trị
//                duration_AMBER = duration_RED - duration_GREEN;
//
//                /* ------------------------------------------------------------
//				 * KIỂM TRA AMBER MỚI
//				 * ------------------------------------------------------------ */
//                if(duration_AMBER < 1 || duration_AMBER > 99) {
//                    /* --------------------------------------------------------
//					 * CẢ 2 CÁCH ĐỀU THẤT BẠI - RESET VỀ MẶC ĐỊNH
//					 * --------------------------------------------------------
//					 * Không thể tìm được cấu hình hợp lệ
//					 * → Quay về giá trị an toàn
//					 */
//                    duration_RED = 5;
//                    duration_GREEN = 3;
//                    duration_AMBER = 2;
//                }
//            }
//            break;
//
//            /* ====================================================================
//             * CASE 1: VỪA CHỈNH ĐÈN VÀNG
//             * ==================================================================== */
//            case 1:
//                /* ----------------------------------------------------------------
//                 * CHIẾN LƯỢC MỚI: GREEN = AMBER(mới) + 3
//                 * ----------------------------------------------------------------
//                 * Công thức:
//                 *   GREEN = AMBER + 3
//                 *   RED = GREEN + AMBER = (AMBER + 3) + AMBER = 2×AMBER + 3
//                 *
//                 * Ví dụ 1:
//                 *   AMBER(mới) = 2
//                 *   → GREEN = 2 + 3 = 5
//                 *   → RED = 5 + 2 = 7 ✓
//                 *
//                 * Ví dụ 2:
//                 *   AMBER(mới) = 10
//                 *   → GREEN = 10 + 3 = 13
//                 *   → RED = 13 + 10 = 23 ✓
//                 *
//                 * Ví dụ 3:
//                 *   AMBER(mới) = 30
//                 *   → GREEN = 30 + 3 = 33
//                 *   → RED = 33 + 30 = 63 ✓
//                 */
//
//                // Tính GREEN theo công thức mới
//                duration_GREEN = duration_AMBER + 4;
//
//                // Tính RED
//                duration_RED = duration_GREEN + duration_AMBER;
//
//                /* ----------------------------------------------------------------
//                 * KIỂM TRA TÍNH HỢP LỆ
//                 * ---------------------------------------------------------------- */
//
//                // Kiểm tra RED có vượt giới hạn không
//                if(duration_RED > 99) {
//                    /* ------------------------------------------------------------
//                     * RED VƯỢT QUÁ 99 - ĐIỀU CHỈNH AMBER
//                     * ------------------------------------------------------------
//                     * Từ công thức: RED = 2×AMBER + 3
//                     * Để RED = 99: AMBER = (99 - 3) / 2 = 48
//                     *
//                     * Giới hạn: AMBER tối đa = 48 để RED không vượt 99
//                     */
//                    duration_AMBER = (99 - 3) / 2;  // = 48
//                    duration_GREEN = duration_AMBER + 3;  // = 51
//                    duration_RED = 99;
//
//                    // Kiểm tra AMBER có hợp lệ không
//                    if(duration_AMBER < 1) {
//                        // AMBER < 1 → Reset về mặc định
//                        duration_RED = 5;
//                        duration_GREEN = 3;
//                        duration_AMBER = 2;
//                    }
//                }
//
//                // Kiểm tra GREEN có hợp lệ không (phòng trường hợp AMBER = 0 hoặc âm)
//                if(duration_GREEN < 1 || duration_GREEN > 99) {
//                    // GREEN không hợp lệ → Reset
//                    duration_RED = 5;
//                    duration_GREEN = 3;
//                    duration_AMBER = 2;
//                }
//                break;
//        /* ====================================================================
//         * CASE 2: VỪA CHỈNH ĐÈN XANH
//         * ==================================================================== */
//        case 2:
//            /* ----------------------------------------------------------------
//			 * CHIẾN LƯỢC: Giữ AMBER, tính lại RED
//			 * ----------------------------------------------------------------
//			 * Công thức: RED = GREEN (mới) + AMBER
//			 *
//			 * Ví dụ 1 (thành công):
//			 *   GREEN(mới)=7, AMBER=2 → RED=9 ✓
//			 *
//			 * Ví dụ 2 (vượt giới hạn):
//			 *   GREEN(mới)=80, AMBER=30 → RED=110 ✗ (>99)
//			 */
//            duration_RED = duration_GREEN + duration_AMBER;
//
//            /* ----------------------------------------------------------------
//			 * KIỂM TRA RED CÓ VƯỢT GIỚI HẠN KHÔNG
//			 * ---------------------------------------------------------------- */
//            if(duration_RED > 99) {
//                /* ------------------------------------------------------------
//				 * RED VƯỢT QUÁ 99 - ĐIỀU CHỈNH AMBER
//				 * ------------------------------------------------------------
//				 * Chiến lược: Giảm AMBER để RED = 99
//				 * Công thức: AMBER = 99 - GREEN
//				 *
//				 * Ví dụ:
//				 *   GREEN=80 → AMBER=19, RED=99 ✓
//				 */
//                duration_AMBER = 99 - duration_GREEN;
//                duration_RED = 99;
//
//                /* ------------------------------------------------------------
//				 * KIỂM TRA AMBER MỚI
//				 * ------------------------------------------------------------ */
//                if(duration_AMBER < 1) {
//                    /* --------------------------------------------------------
//					 * KHÔNG THỂ ĐIỀU CHỈNH - RESET
//					 * --------------------------------------------------------
//					 * Ví dụ gây lỗi: GREEN=99 → AMBER=0 ✗
//					 */
//                    duration_RED = 5;
//                    duration_GREEN = 3;
//                    duration_AMBER = 2;
//                }
//            }
//            break;
//    }
//
//    return 1;  // Đã thực hiện điều chỉnh hoặc reset
//}
//
///* ============================================================================
// * PHẦN 8: EDGE DETECTION CHO NÚT NHẤN
// * ============================================================================ */
//
///**
// * update_button_state() - Cập nhật trạng thái nút nhấn và phát hiện sự kiện
// *
// * Cơ CHẾ EDGE DETECTION:
// *
// * Edge detection là kỹ thuật phát hiện thời điểm chuyển trạng thái của nút.
// * Thay vì kiểm tra "nút có đang nhấn không", ta kiểm tra "nút VỪA MỚI được nhấn".
// *
// * ┌──────────────────────────────────────────────────────────────────────┐
// * │ SỰ KIỆN             │ prevState  │ currState  │ Ý NGHĨA              │
// * ├──────────────────────────────────────────────────────────────────────┤
// * │ RISING EDGE         │ RELEASE    │ PRESS      │ VỪA NHẤN XUỐNG       │
// * │ (vừa nhấn)          │            │            │ → XỬ LÝ SỰ KIỆN      │
// * ├──────────────────────────────────────────────────────────────────────┤
// * │ HOLDING             │ PRESS      │ PRESS      │ ĐANG GIỮ NÚT         │
// * │ (đang giữ)          │            │            │ → BỎ QUA             │
// * ├──────────────────────────────────────────────────────────────────────┤
// * │ FALLING EDGE        │ PRESS      │ RELEASE    │ VỪA THẢ RA           │
// * │ (vừa thả)           │            │            │ → THƯỜNG BỎ QUA      │
// * ├──────────────────────────────────────────────────────────────────────┤
// * │ RELEASED            │ RELEASE    │ RELEASE    │ KHÔNG NHẤN           │
// * └──────────────────────────────────────────────────────────────────────┘
// *
// * LỢI ÍCH CỦA EDGE DETECTION:
// * - Tránh xử lý lặp lại khi người dùng giữ nút
// * - Mỗi lần nhấn = 1 lần xử lý
// * - Không cần delay hoặc debounce phức tạp (giả sử đã có debounce ở hardware)
// *
// * GIỚI HẠN:
// * - Giả định các hàm isButton*Pressed() đã xử lý debounce
// * - Nếu không có debounce → có thể phát hiện nhiều sự kiện do rung phím
// *
// * ĐƯỢC GỌI: Mỗi 10ms trong hàm traffic_run()
// *
// * CẤU TRÚC DỮ LIỆU:
// *   prevState[3]: Trạng thái nút ở lần quét TRƯỚC (10ms trước)
// *   currState[3]: Trạng thái nút ở lần quét HIỆN TẠI
// *
// *   Index 0: Button 1 (MODE)
// *   Index 1: Button 2 (MODIFY/INC)
// *   Index 2: Button 3 (SET)
// */
//void update_button_state(void)
//{
//    /* ------------------------------------------------------------------------
//	 * DUYỆT QUA CẢ 3 NÚT NHẤN
//	 * ------------------------------------------------------------------------
//	 * i = 0: Nút MODE (Button 1)
//	 * i = 1: Nút MODIFY/INC (Button 2)
//	 * i = 2: Nút SET (Button 3)
//	 */
//    for(int i = 0; i < 3; i++) {
//        /* ====================================================================
//		 * BƯỚC 1: LƯU TRẠNG THÁI HIỆN TẠI THÀNH TRẠNG THÁI TRƯỚC
//		 * ====================================================================
//		 * Chuyển giá trị currState → prevState
//		 * Chuẩn bị cho việc đọc trạng thái mới
//		 *
//		 * Ví dụ:
//		 *   Lần trước: prevState[0]=RELEASE, currState[0]=RELEASE
//		 *   Lần này: prevState[0]=RELEASE (copy từ currState cũ)
//		 */
//        prevState[i] = currState[i];
//
//        /* ====================================================================
//		 * BƯỚC 2: ĐỌC TRẠNG THÁI MỚI TỪ PHẦN CỨNG
//		 * ====================================================================
//		 * Gọi các hàm isButton*Pressed() để đọc trạng thái GPIO
//		 *
//		 * QUAN TRỌNG: Giả định các hàm này đã xử lý:
//		 * - Debounce (chống rung phím)
//		 * - Active HIGH/LOW (tùy thiết kế phần cứng)
//		 *
//		 * Trả về:
//		 *   1 (true): Nút đang được nhấn
//		 *   0 (false): Nút không được nhấn
//		 */
//        switch(i) {
//            /* ----------------------------------------------------------------
//			 * NÚT 1: MODE (Chuyển đổi chế độ)
//			 * ---------------------------------------------------------------- */
//            case 0:
//                if(isButton1Pressed()) {
//                    currState[i] = BTN_PRESS;    // Nút đang nhấn
//                } else {
//                    currState[i] = BTN_RELEASE;  // Nút không nhấn
//                }
//                break;
//
//            /* ----------------------------------------------------------------
//			 * NÚT 2: MODIFY/INC (Tăng giá trị)
//			 * ---------------------------------------------------------------- */
//            case 1:
//                if(isButton2Pressed()) {
//                    currState[i] = BTN_PRESS;
//                } else {
//                    currState[i] = BTN_RELEASE;
//                }
//                break;
//
//            /* ----------------------------------------------------------------
//			 * NÚT 3: SET (Lưu giá trị)
//			 * ---------------------------------------------------------------- */
//            case 2:
//                if(isButton3Pressed()) {
//                    currState[i] = BTN_PRESS;
//                } else {
//                    currState[i] = BTN_RELEASE;
//                }
//                break;
//        }
//    }
//
//    /* ------------------------------------------------------------------------
//	 * SAU KHI THOÁT HÀM NÀY:
//	 * ------------------------------------------------------------------------
//	 * - prevState[] và currState[] đã được cập nhật
//	 * - Các hàm fsm_*_mode() có thể so sánh 2 giá trị này để phát hiện:
//	 *
//	 *   Rising Edge: currState[i] == PRESS && prevState[i] == RELEASE
//	 *                → XỬ LÝ SỰ KIỆN "VỪA NHẤN"
//	 *
//	 * VÍ DỤ TIMELINE:
//	 *   t=0ms:   prevState=RELEASE, currState=RELEASE (không nhấn)
//	 *   t=10ms:  prevState=RELEASE, currState=RELEASE (vẫn không nhấn)
//	 *   t=20ms:  prevState=RELEASE, currState=PRESS   (VỪA NHẤN!) ← Xử lý
//	 *   t=30ms:  prevState=PRESS,   currState=PRESS   (đang giữ, bỏ qua)
//	 *   t=40ms:  prevState=PRESS,   currState=PRESS   (đang giữ, bỏ qua)
//	 *   t=50ms:  prevState=PRESS,   currState=RELEASE (vừa thả, bỏ qua)
//	 *   t=60ms:  prevState=RELEASE, currState=RELEASE (không nhấn)
//	 */
//}
//
///* ============================================================================
// * PHẦN 9: SƠ ĐỒ HOẠT ĐỘNG TỔNG QUAN VÀ LƯU ĐỒ
// * ============================================================================ */
//
///*
// * ============================================================================
// * SƠ ĐỒ 1: QUY TRÌNH KHỞI ĐỘNG VÀ HOẠT ĐỘNG
// * ============================================================================
// *
// *     ┌─────────────────────┐
// *     │   SYSTEM STARTUP    │
// *     │   (main function)   │
// *     └──────────┬──────────┘
// *                │
// *                ↓
// *     ┌─────────────────────┐
// *     │  traffic_init()     │  ← Khởi tạo 1 lần duy nhất
// *     │  - duration = 5/3/2 │
// *     │  - mode = MODE_1    │
// *     │  - state = INIT     │
// *     │  - Tắt tất cả LED   │
// *     └──────────┬──────────┘
// *                │
// *                ↓
// *     ┌─────────────────────┐
// *     │   TIMER INTERRUPT   │  ← Mỗi 10ms
// *     │      (10ms)         │
// *     └──────────┬──────────┘
// *                │
// *                ↓
// *     ┌─────────────────────────────────────────┐
// *     │         traffic_run()                   │
// *     │  ┌────────────────────────────────────┐ │
// *     │  │ 1. update_button_state()           │ │ ← Đọc nút nhấn
// *     │  │    - Đọc GPIO                      │ │
// *     │  │    - Edge detection                │ │
// *     │  └────────────────────────────────────┘ │
// *     │                                         │
// *     │  ┌────────────────────────────────────┐ │
// *     │  │ 2. fsm_*_mode()                    │ │ ← Xử lý logic
// *     │  │    - MODE 1: Auto traffic          │ │
// *     │  │    - MODE 2-4: Adjust duration     │ │
// *     │  └────────────────────────────────────┘ │
// *     │                                         │
// *     │  ┌────────────────────────────────────┐ │
// *     │  │ 3. update_led_display()            │ │ ← Cập nhật LED
// *     │  │    - Bật/tắt LED theo cờ hiệu      │ │
// *     │  └────────────────────────────────────┘ │
// *     │                                         │
// *     │  ┌────────────────────────────────────┐ │
// *     │  │ 4. update_7seg_display()           │ │ ← Hiển thị số
// *     │  │    - Hiển thị counter hoặc temp    │ │
// *     │  └────────────────────────────────────┘ │
// *     └──────────────┬──────────────────────────┘
// *                    │
// *                    ↓
// *           ┌────────────────┐
// *           │  Lặp lại 10ms  │
// *           │      sau       │
// *           └────────┬───────┘
// *                    │
// *                    └─────→ (quay lại TIMER INTERRUPT)
// *
// *
// * ============================================================================
// * SƠ ĐỒ 2: CHU TRÌNH CHUYỂN ĐỔI MODE
// * ============================================================================
// *
// *    ┌─────────────────────────────────────────────────────────────┐
// *    │                       MODE 1                                │
// *    │                  (Normal Operation)                         │
// *    │  ┌───────────────────────────────────────────────────────┐  │
// *    │  │ INIT → RED_GREEN → RED_AMBER → GREEN_RED → AMBER_RED  │  │
// *    │  │   ↑                                              │    │  │
// *    │  │   └──────────────────────────────────────────────┘    │  │
// *    │  └───────────────────────────────────────────────────────┘  │
// *    └──────────────────────────┬──────────────────────────────────┘
// *                               │ Nhấn nút MODE
// *                               ↓
// *    ┌──────────────────────────────────────────────────────────────┐
// *    │                       MODE 2                                 │
// *    │                  (Red Adjust Mode)                           │
// *    │  ┌────────────────────────────────────────────────────────┐  │
// *    │  │ - Nhấp nháy LED đỏ                                     │  │
// *    │  │ - Hiển thị temp_duration                               │  │
// *    │  │ - Nút MODIFY: Tăng giá trị (1→99→1)                    │  │
// *    │  │ - Nút SET: Lưu → auto_adjust → Về MODE 1               │  │
// *    │  │ - Nút MODE: Chuyển MODE 3                              │  │
// *    │  └────────────────────────────────────────────────────────┘  │
// *    └──────────────────────────┬──────────────────────────────────┘
// *                               │ Nhấn nút MODE
// *                               ↓
// *    ┌──────────────────────────────────────────────────────────────┐
// *    │                       MODE 3                                │
// *    │                 (Amber Adjust Mode)                         │
// *    │  ┌────────────────────────────────────────────────────────┐ │
// *    │  │ - Nhấp nháy LED vàng                                   │ │
// *    │  │ - Hiển thị temp_duration                               │ │
// *    │  │ - Nút MODIFY: Tăng giá trị                             │ │
// *    │  │ - Nút SET: Lưu → auto_adjust → Về MODE 1               │ │
// *    │  │ - Nút MODE: Chuyển MODE 4                              │ │
// *    │  └────────────────────────────────────────────────────────┘ │
// *    └──────────────────────────┬──────────────────────────────────┘
// *                               │ Nhấn nút MODE
// *                               ↓
// *    ┌──────────────────────────────────────────────────────────────┐
// *    │                       MODE 4                                 │
// *    │                 (Green Adjust Mode)                          │
// *    │  ┌────────────────────────────────────────────────────────┐ │
// *    │  │ - Nhấp nháy LED xanh                                   │ │
// *    │  │ - Hiển thị temp_duration                               │ │
// *    │  │ - Nút MODIFY: Tăng giá trị                             │ │
// *    │  │ - Nút SET: Lưu → auto_adjust → Về MODE 1               │ │
// *    │  │ - Nút MODE: Về MODE 1 (không lưu)                      │ │
// *    │  └────────────────────────────────────────────────────────┘ │
// *    └──────────────────────────┬──────────────────────────────────┘
// *                               │ Nhấn nút MODE hoặc SET
// *                               ↓
// *                    ┌─────────────────────┐
// *                    │   Quay về MODE 1    │
// *                    │   (traffic_state    │
// *                    │     = INIT)         │
// *                    └─────────────────────┘
// *
// *
// * ============================================================================
// * SƠ ĐỒ 3: CHU TRÌNH ĐÈN GIAO THÔNG TRONG MODE 1
// * ============================================================================
// *
// *  Giả sử: duration_RED=5, duration_GREEN=3, duration_AMBER=2
// *
// *  ┌────────────────────────────────────────────────────────────────────┐
// *  │                           INIT                                     │
// *  │  - counter_road1 = 5 (RED)                                         │
// *  │  - counter_road2 = 3 (GREEN)                                       │
// *  └───────────────────────────┬────────────────────────────────────────┘
// *                              │
// *                              ↓
// *  ┌────────────────────────────────────────────────────────────────────┐
// *  │                       RED_GREEN                                    │
// *  │  Đường 1: ĐỎ (5→4→3→2)  |  Đường 2: XANH (3→2→1→0)                 │
// *  │                                                                    │
// *  │  Giây 1: road1=5, road2=3    LED: [Đ1:ĐỎ]   [Đ2:XANH]              │
// *  │  Giây 2: road1=4, road2=2    LED: [Đ1:ĐỎ]   [Đ2:XANH]              │
// *  │  Giây 3: road1=3, road2=1    LED: [Đ1:ĐỎ]   [Đ2:XANH]              │
// *  │  Giây 4: road1=2, road2=0    → Chuyển trạng thái                   │
// *  └───────────────────────────┬────────────────────────────────────────┘
// *                              │ counter_road2 <= 0
// *                              ↓
// *  ┌────────────────────────────────────────────────────────────────────┐
// *  │                       RED_AMBER                                    │
// *  │  Đường 1: ĐỎ (2→1→0)  |  Đường 2: VÀNG (2→1→0)                     │
// *  │                                                                    │
// *  │  Giây 5: road1=2, road2=2    LED: [Đ1:ĐỎ]   [Đ2:VÀNG]              │
// *  │  Giây 6: road1=1, road2=1    LED: [Đ1:ĐỎ]   [Đ2:VÀNG]              │
// *  │  Giây 7: road1=0, road2=0    → Chuyển trạng thái                   │
// *  └───────────────────────────┬────────────────────────────────────────┘
// *                              │ counter_road2 <= 0
// *                              ↓
// *  ┌────────────────────────────────────────────────────────────────────┐
// *  │                       GREEN_RED                                    │
// *  │  Đường 1: XANH (3→2→1→0)  |  Đường 2: ĐỎ (5→4→3→2)                 │
// *  │                                                                    │
// *  │  Giây 8:  road1=3, road2=5    LED: [Đ1:XANH] [Đ2:ĐỎ]               │
// *  │  Giây 9:  road1=2, road2=4    LED: [Đ1:XANH] [Đ2:ĐỎ]               │
// *  │  Giây 10: road1=1, road2=3    LED: [Đ1:XANH] [Đ2:ĐỎ]               │
// *  │  Giây 11: road1=0, road2=2    → Chuyển trạng thái                  │
// *  └───────────────────────────┬────────────────────────────────────────┘
// *                              │ counter_road1 <= 0
// *                              ↓
// *  ┌────────────────────────────────────────────────────────────────────┐
// *  │                       AMBER_RED                                    │
// *  │  Đường 1: VÀNG (2→1→0)  |  Đường 2: ĐỎ (2→1→0)                     │
// *  │                                                                    │
// *  │  Giây 12: road1=2, road2=2    LED: [Đ1:VÀNG] [Đ2:ĐỎ]               │
// *  │  Giây 13: road1=1, road2=1    LED: [Đ1:VÀNG] [Đ2:ĐỎ]               │
// *  │  Giây 14: road1=0, road2=0    → Quay lại RED_GREEN                 │
// *  └───────────────────────────┬────────────────────────────────────────┘
// *                              │ counter_road2 <= 0
// *                              ↓
// *                    ┌─────────────────────┐
// *                    │   RED_GREEN (lặp)   │
// *                    └─────────────────────┘
// *
// *  TỔNG CHU KỲ: 14 giây (5+2+5+2)
// *  ĐẢM BẢO: Mỗi đường có thời gian đèn đỏ đủ để đường kia đi hết
// *
// *
// * ============================================================================
// * SƠ ĐỒ 4: CƠ CHẾ AUTO_ADJUST_DURATION
// * ============================================================================
// *
// *  RÀNG BUỘC: duration_RED = duration_GREEN + duration_AMBER
// *
// *  ┌─────────────────────────────────────────────────────────────────────┐
// *  │              VỪA CHỈNH ĐÈN ĐỎ (modified_light = 2)                  │
// *  ├─────────────────────────────────────────────────────────────────────┤
// *  │ Chiến lược: Giữ AMBER, tính GREEN                                   │
// *  │                                                                     │
// *  │  ┌──────────┐                                                       │
// *  │  │ RED(mới) │ = 10                                                  │
// *  │  └────┬─────┘                                                       │
// *  │       │                                                             │
// *  │       ↓                                                             │
// *  │  ┌───────────────────────────────────────┐                          │
// *  │  │ GREEN = RED - AMBER = 10 - 2 = 8 ✓   │                           │
// *  │  └───────────────────────────────────────┘                          │
// *  │                                                                     │
// *  │  Kết quả: RED=10, GREEN=8, AMBER=2                                  │
// *  │  Kiểm tra: 10 = 8 + 2 ✓                                             │
// *  └─────────────────────────────────────────────────────────────────────┘
// *
// *  ┌─────────────────────────────────────────────────────────────────────┐
// *  │             VỪA CHỈNH ĐÈN VÀNG (modified_light = 1)                 │
// *  ├─────────────────────────────────────────────────────────────────────┤
// *  │ Chiến lược: Cập nhật GREEN, tính RED                                │
// *  │                                                                     │
// *  │  ┌────────────┐                                                     │
// *  │  │ AMBER(mới) │ = 3                                                 │
// *  │  └──────┬─────┘                                                     │
// *  │         │                                                           │
// *  │         ↓                                                           │
// *  │  ┌───────────────────────────────────────┐                          │
// *  │  │ RED = GREEN(mới) + AMBER = 5 + 3 = 8 ✓    │                      │
// *  │  └───────────────────────────────────────┘                          │
// *  │                                                                     │
// *  │  Kết quả: RED=8, GREEN=5, AMBER=3                                   │
// *  │  Kiểm tra: 8 = 5 + 3 ✓                                              │
// *  └─────────────────────────────────────────────────────────────────────┘
// *
// *  ┌─────────────────────────────────────────────────────────────────────┐
// *  │             VỪA CHỈNH ĐÈN XANH (modified_light = 0)                 │
// *  ├─────────────────────────────────────────────────────────────────────┤
// *  │ Chiến lược: Giữ AMBER, tính RED                                     │
// *  │                                                                     │
// *  │  ┌────────────┐                                                     │
// *  │  │ GREEN(mới) │ = 7                                                 │
// *  │  └──────┬─────┘                                                     │
// *  │         │                                                           │
// *  │         ↓                                                           │
// *  │  ┌───────────────────────────────────────┐                          │
// *  │  │ RED = GREEN + AMBER = 7 + 2 = 9 ✓    │                           │
// *  │  └───────────────────────────────────────┘                          │
// *  │                                                                     │
// *  │  Kết quả: RED=9, GREEN=7, AMBER=2                                   │
// *  │  Kiểm tra: 9 = 7 + 2 ✓                                              │
// *  └─────────────────────────────────────────────────────────────────────┘
// *
// *  ┌─────────────────────────────────────────────────────────────────────┐
// *  │                    TRƯỜNG HỢP ĐẶC BIỆT                              │
// *  ├─────────────────────────────────────────────────────────────────────┤
// *  │  Ví dụ: GREEN=80, AMBER(mới)=30                                     │
// *  │  → RED = 80 + 30 = 110 ✗ (vượt giới hạn 99)                         │
// *  │                                                                     │
// *  │  Điều chỉnh:                                                        │
// *  │  → GREEN = 99 - AMBER = 99 - 30 = 69                                │
// *  │  → RED = 99                                                         │
// *  │                                                                     │
// *  │  Kết quả: RED=99, GREEN=69, AMBER=30                                │
// *  │  Kiểm tra: 99 = 69 + 30 ✓                                           │
// *  └─────────────────────────────────────────────────────────────────────┘
// *
// *  ┌─────────────────────────────────────────────────────────────────────┐
// *  │               TRƯỜNG HỢP KHÔNG THỂ ĐIỀU CHỈNH                       │
// *  ├─────────────────────────────────────────────────────────────────────┤
// *  │  Ví dụ: GREEN=1, AMBER(mới)=99                                      │
// *  │  → RED = 1 + 99 = 100 ✗ (>99)                                       │
// *  │  → Thử điều chỉnh: GREEN = 99 - 99 = 0 ✗ (<1)                       │
// *  │                                                                     │
// *  │  → RESET về mặc định: RED=5, GREEN=3, AMBER=2                       │
// *  └─────────────────────────────────────────────────────────────────────┘
// *
// *
// * ============================================================================
// * SƠ ĐỒ 5: TIMELINE EDGE DETECTION (10ms resolution)
// * ============================================================================
// *
// *  Thời gian  │ Nút vật lý │ prevState │ currState │ Phát hiện │ Hành động
// *  ───────────┼────────────┼───────────┼───────────┼───────────┼───────────
// *   t=0ms     │  KHÔNG     │  RELEASE  │  RELEASE  │    -      │    -
// *   t=10ms    │  KHÔNG     │  RELEASE  │  RELEASE  │    -      │    -
// *   t=20ms    │  NHẤN      │  RELEASE  │  PRESS    │ RISING ✓  │ XỬ LÝ!
// *   t=30ms    │  GIỮ       │  PRESS    │  PRESS    │    -      │ Bỏ qua
// *   t=40ms    │  GIỮ       │  PRESS    │  PRESS    │    -      │ Bỏ qua
// *   t=50ms    │  GIỮ       │  PRESS    │  PRESS    │    -      │ Bỏ qua
// *   t=60ms    │  THẢ       │  PRESS    │  RELEASE  │ FALLING   │ Bỏ qua
// *   t=70ms    │  KHÔNG     │  RELEASE  │  RELEASE  │    -      │    -
// *
// *  ⚠️  CHÚ Ý: Chỉ xử lý tại thời điểm RISING EDGE (t=20ms)
// *     Mặc dù người dùng giữ nút từ t=20ms đến t=60ms (40ms),
// *     nhưng chỉ có 1 lần xử lý duy nhất!
// */
//
//
//
