/* sensor.h
 * Module đọc dữ liệu từ cảm biến ADC
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "global.h"

/* ============= KHAI BÁO HÀM ============= */

/**
 * @brief Khởi tạo module sensor
 * Khởi động ADC conversion
 */
void sensor_init(void);

/**
 * @brief Đọc giá trị ADC từ cảm biến
 * @return Giá trị ADC (0-4095 cho ADC 12-bit)
 *
 * Lưu ý: ADC trong STM32F1 là 12-bit
 * - 0V    -> ADC = 0
 * - 3.3V  -> ADC = 4095
 * - 1.65V -> ADC = 2048
 */
uint32_t read_adc_value(void);

/**
 * @brief Chuyển đổi giá trị ADC sang điện áp (mV)
 * @param adc_value: Giá trị ADC đọc được
 * @return Điện áp tính bằng millivolt (mV)
 */
uint32_t adc_to_voltage_mv(uint32_t adc_value);

#endif /* INC_SENSOR_H_ */
