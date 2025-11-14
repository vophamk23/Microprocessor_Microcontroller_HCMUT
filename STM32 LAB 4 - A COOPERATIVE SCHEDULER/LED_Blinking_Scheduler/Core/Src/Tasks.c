
/*
 * Tasks.c
 * Task function implementations
 */

#include "Tasks.h"
#include "main.h"

/* ==================== TASK IMPLEMENTATIONS ==================== */

/**
 * @brief Task 1: Toggle LED1 every 500ms (50 ticks)
 */
void Task_LED1(void) {
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}

/**
 * @brief Task 2: Toggle LED2 every 1000ms (100 ticks)
 */
void Task_LED2(void) {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
}

/**
 * @brief Task 3: Toggle LED3 every 1500ms (150 ticks)
 */
void Task_LED3(void) {
    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
}

/**
 * @brief Task 4: Toggle LED4 every 2000ms (200 ticks)
 */
void Task_LED4(void) {
    HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
}

/**
 * @brief Task 5: Toggle LED5 every 2500ms (250 ticks)
 */
void Task_LED5(void) {
    HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
}
