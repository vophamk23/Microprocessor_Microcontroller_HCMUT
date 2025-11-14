/* sensor.c
 * Triển khai các hàm đọc dữ liệu từ ADC
 * FIXED VERSION - Sử dụng Continuous Mode đúng cách
 */

#include "sensor.h"

/* ============= HẰNG SỐ ============= */
#define ADC_MAX_VALUE 4095    // Giá trị max của ADC 12-bit
#define VREF_MV 3300          // Điện áp tham chiếu 3.3V = 3300mV


/* ============= HÀM KHỞI TẠO ============= */
void sensor_init(void) {
    HAL_ADC_Start(&hadc1);
}


/* ============= HÀM ĐỌC ADC ============= */
/*
 * Vì ADC đang chạy liên tục, ta chỉ cần:
 * KẾT QUẢ: Số từ 0 đến 4095
 *
 * Ví dụ thực tế:
 * - Không có điện áp (0V)    → Trả về 0
 * - Điện áp trung bình (1.65V) → Trả về 2048
 * - Điện áp tối đa (3.3V)    → Trả về 4095
 */
uint32_t read_adc_value(void) {

    HAL_Delay(1);  // Delay ngắn để đảm bảo có giá trị mới

    // Đọc và trả về giá trị ADC
    return HAL_ADC_GetValue(&hadc1);
}

/* ============= HÀM CHUYỂN ĐỔI ============= */
/*
     * Hàm này chuyển giá trị ADC thành điện áp thật (mV)
     *
     * CÔNG THỨC:
     * Điện áp (mV) = (Giá trị ADC × 3300mV) ÷ 4095
     *
     * VÍ DỤ CỤ THỂ:
     *
     * Input: adc_value = 2048
     * Tính toán: (2048 × 3300) ÷ 4095
     *          = 6758400 ÷ 4095
     *          = 1650 mV
     *          = 1.65 V
     *
     * Input: adc_value = 4095
     * Tính toán: (4095 × 3300) ÷ 4095
     *          = 3300 mV
     *          = 3.3 V
     */
uint32_t adc_to_voltage_mv(uint32_t adc_value) {

    uint32_t voltage_mv = (adc_value * VREF_MV) / ADC_MAX_VALUE;

    return voltage_mv;
}
