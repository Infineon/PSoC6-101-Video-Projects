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

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    /* 1 byte I2C data buffer */
    volatile uint8_t myBuffer;
    
    PWM_Start();
    
    EZI2C_Start();
    /* Setup buffer of 1 byte with 1 byte writable from the master */
    EZI2C_SetBuffer1((uint8_t *) &myBuffer, sizeof(myBuffer), 1);

    for(;;)
    {
        Cy_TCPWM_PWM_SetCompare0(PWM_HW,PWM_CNT_NUM,myBuffer);
        Cy_SysPm_Sleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
}

/* [] END OF FILE */
