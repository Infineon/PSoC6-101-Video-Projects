/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#pragma once

#include <stdint.h>

typedef enum {
    M1,
    M2
} motors_t;

typedef enum {
    ABSOLUTE,
    RELATIVE
} motor_change_t;

void writeMotorPosition(motors_t motor, motor_change_t type, int8_t percent);

void bleTask(void *arg);

/* [] END OF FILE */
