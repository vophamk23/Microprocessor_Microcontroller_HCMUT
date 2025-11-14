/*
 * Tasks.h
 * Task function declarations for the scheduler
 */

#ifndef INC_TASKS_H_
#define INC_TASKS_H_

#include "main.h"

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
