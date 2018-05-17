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

    CapSense_Start();
    CapSense_ScanAllWidgets(); // Start Initial Scan

    PWM_Start();

    for(;;)
    {
        if(!CapSense_IsBusy())
        {
            CapSense_ProcessAllWidgets();
            int pos;
            pos=CapSense_GetCentroidPos(CapSense_LINEARSLIDER0_WDGT_ID); // Get finger position on slider
            if(pos<0xFFFF) // This is true if the slider is being touched
            {
                Cy_TCPWM_PWM_SetCompare0(PWM_HW,PWM_CNT_NUM,pos); // Set red LED brighness from slider position
            }
            if(CapSense_IsWidgetActive(CapSense_BUTTON0_WDGT_ID)) // Check button 0 state
            {
                /* Turn off PWM and turn on Blue LED */
                Cy_TCPWM_PWM_Disable(PWM_HW,PWM_CNT_NUM);
                Cy_GPIO_Write(BLUE_PORT,BLUE_NUM,0);
            
            }
            if(CapSense_IsWidgetActive(CapSense_BUTTON1_WDGT_ID)) // Check button 1 state
            {
                /* Turn on PWM and turn off blue LED */
                Cy_TCPWM_PWM_Enable(PWM_HW,PWM_CNT_NUM);
                Cy_TCPWM_TriggerStart(PWM_HW, PWM_CNT_MASK);
                Cy_GPIO_Write(BLUE_PORT,BLUE_NUM,1);
            }
                
            CapSense_UpdateAllBaselines();
            CapSense_ScanAllWidgets(); // Start next scan
        }
    }
}
/* [] END OF FILE */
