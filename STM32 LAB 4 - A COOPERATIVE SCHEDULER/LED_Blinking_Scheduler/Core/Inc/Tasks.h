/*
 * Tasks.h
 * Task function declarations for the scheduler
 */

#ifndef INC_TASKS_H_
#define INC_TASKS_H_

#include "main.h"

/* ==================== TASK TIMING CONSTANTS ==================== */
// Delay in milliseconds
#define TASK_LED1_DELAY_MS      500
#define TASK_LED2_DELAY_MS      1000
#define TASK_LED3_DELAY_MS      1500
#define TASK_LED4_DELAY_MS      2000
#define TASK_LED5_DELAY_MS      2500

// Delay in ticks (assuming TIMER_TICK_MS is defined)
#define TASK_LED1_TICKS         (TASK_LED1_DELAY_MS / TIMER_TICK_MS)
#define TASK_LED2_TICKS         (TASK_LED2_DELAY_MS / TIMER_TICK_MS)
#define TASK_LED3_TICKS         (TASK_LED3_DELAY_MS / TIMER_TICK_MS)
#define TASK_LED4_TICKS         (TASK_LED4_DELAY_MS / TIMER_TICK_MS)
#define TASK_LED5_TICKS         (TASK_LED5_DELAY_MS / TIMER_TICK_MS)
/* ==================== TASK FUNCTION PROTOTYPES ==================== */

/**
 * @brief Task 1: Toggle LED1 every 500ms
 */
void Task_LED1(void);

/**
 * @brief Task 2: Toggle LED2 every 1000ms
 */
void Task_LED2(void);

/**
 * @brief Task 3: Toggle LED3 every 1500ms
 */
void Task_LED3(void);

/**
 * @brief Task 4: Toggle LED4 every 2000ms
 */
void Task_LED4(void);

/**
 * @brief Task 5: Toggle LED5 every 2500ms
 */
void Task_LED5(void);

#endif /* INC_TASKS_H_ */
