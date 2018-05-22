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

#include "event_groups.h"

extern EventGroupHandle_t systemInputMode;
#define MODE_MOTION (1<<0)
#define MODE_CAPSENSE (1<<1)

void motionTask(void *arg);

/* [] END OF FILE */
