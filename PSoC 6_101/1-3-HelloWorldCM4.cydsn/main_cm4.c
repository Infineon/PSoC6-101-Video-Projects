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


    for(;;)
    {
        /* Toggle LED on/off every 500ms */
        Cy_GPIO_Write(RED_PORT,RED_NUM,0);
        CyDelay(500);
        Cy_GPIO_Write(RED_PORT,RED_NUM,1);
        CyDelay(500);
    }
}

/* [] END OF FILE */
