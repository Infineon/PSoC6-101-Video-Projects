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

#include "FreeRTOS.h"
#include "semphr.h"

typedef enum {
    M1,
    M2
} motors_t;

typedef enum {
    POS_ABSOLUTE,
    POS_RELATIVE
} motor_pos_type_t;

typedef struct PWM_Message {
    motors_t motor;
    motor_pos_type_t changeType;
    int percent;
} PWM_Message_t;

int getMotorPercent(motors_t motor);

void pwmTask(void *);

/* [] END OF FILE */
