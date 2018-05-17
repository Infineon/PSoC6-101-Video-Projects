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
#include <project.h>
#include <stdio.h>
#include "pwmTask.h"
#include "global.h"

// Range of pulses for Servo Motor in uS
#define MIN_US 1000
#define MAX_US 2000

// Parameters of PWM... must be same as schematic
#define PWM_CLK            4000000
#define PWM_DIV            4
#define US_PER_SEC         1000000
#define PWMTICK_PER_US     (PWM_CLK/PWM_DIV/US_PER_SEC)

// Turn a percent (0 --> 100) into a pulse width in uS
static int inline percentToPulse(int percent)
{
    int pulse;
    pulse = percent * (MAX_US-MIN_US) / (100-0) + MIN_US;
    return pulse;
}

// turn a pulse width in uS into percent
static int inline pulseToPercent(int pulse)
{
    int percent;
    percent = pulse * (100 - 0) / (MAX_US-MIN_US) - 100;
    return percent;
}

// turn a pwm compare value into a pulse width in uS
static int inline compareToPulse(int compare)
{
    int pulse;
    pulse = compare / PWMTICK_PER_US;
    return pulse;
}
// turn a pulse width in uS into a pwm compare value
static inline int pulseToCompare(int pulse)
{
    int compare;
    compare = pulse * PWMTICK_PER_US;
    return compare;
}

// turn a pwm compare value into a percent
static inline int compareToPercent(int compare)
{
    int pulse;
    int percent;
    pulse = compareToPulse(compare);
    percent = pulseToPercent(pulse);
    return percent;
}

// turn a percent into a pwm compare vlaue
static inline int percentToCompare(int percent)
{
    int pulse;
    int compare;
    pulse = percentToPulse(percent);
    compare = pulseToCompare(pulse);
    return compare;
}

// find the 0-100 current value of the PWM for a motor
int getMotorPercent(motors_t motor)
{
    int compare = Cy_TCPWM_PWM_GetCompare0 (
        (motor == M1)?PWM_1_HW:PWM_2_HW, 
        (motor == M1)?PWM_1_CNT_NUM : PWM_2_CNT_NUM);
    return compareToPercent(compare);
}

// The main task that manages the PWM
void pwmTask(void *arg)
{
    (void)arg;
    PWM_Message_t pwmMessage;
    TCPWM_Type* hw;
    int cntrNum=0;
    int compareTmp;
    int percentTmp;
    
    PWM_1_Start();
    PWM_2_Start();
    
    while(1)
    {
        // wait for someone to ask for the motors to change
        xQueueReceive(pwmQueue,&pwmMessage,portMAX_DELAY);
    
        // figure out which pwm to change
        switch(pwmMessage.motor)
        {
            case M1:
                hw = PWM_1_HW;
                cntrNum = PWM_1_CNT_NUM;
            break;
            case M2:
                hw = PWM_2_HW;
                cntrNum = PWM_2_CNT_NUM;
            break;
        }
    
        // figure out relative or absolute.. if relative get current %
        if(pwmMessage.changeType == POS_RELATIVE) 
        {
            compareTmp = Cy_TCPWM_PWM_GetCompare0(hw,cntrNum);
            percentTmp = compareToPercent(compareTmp) + pwmMessage.percent;
            
            // Make sure they dont go below 0% or above 100%
            if(percentTmp < 0)
                percentTmp = 0;
            if (percentTmp>100)
                percentTmp=100;
        }
        else // they want an absolute percent
        {
            percentTmp = pwmMessage.percent;
        }
        // change the PWM and set the event group bits so that other tasks know the PWM has changed
        Cy_TCPWM_PWM_SetCompare0 (hw, cntrNum, percentToCompare(percentTmp) );
        xEventGroupSetBits(pwmEventGroup ,PWM_EVENT_ALL);
    }
}
/* [] END OF FILE */
