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
#include "event_groups.h"  

extern QueueHandle_t pwmQueue;

extern EventGroupHandle_t pwmEventGroup;
#define PWM_EVENT_I2C (1<<0)
#define PWM_EVENT_BLE (1<<1)
#define PWM_EVENT_ALL (PWM_EVENT_I2C | PWM_EVENT_BLE)    
/* [] END OF FILE */
