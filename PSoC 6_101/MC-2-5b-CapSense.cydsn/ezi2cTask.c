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
#include "project.h"
#include "FreeRTOS.h"
#include "pwmTask.h"
#include "global.h"

void ezi2cTask(void *arg)
{
    (void)arg;
    uint8_t motorPercent[2];
    
    EZI2C_Start();
    EZI2C_SetBuffer1(motorPercent,sizeof(motorPercent),0);
    
    while(1)
    {
        /* Read motor values and set I2C registers with those values */
        motorPercent[0] = getMotorPercent(M1);
        motorPercent[1] = getMotorPercent(M2);
 
        /* Wait for an event before continuing. The event is set when a motor value changes in the PWM task */
        xEventGroupWaitBits(
                       pwmEventGroup,      // which event group
                       PWM_EVENT_I2C,      // which bits to wait for
                       pdTRUE,             // clear the bit after it is set
                       pdFALSE,            // only wait for the i2c bit
                       portMAX_DELAY );    // wait forever
        
    }
}
/* [] END OF FILE */
