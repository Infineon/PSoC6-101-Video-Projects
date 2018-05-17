/***************************************************************************//**
* \file     ADC_1_INT.c
* \version  2.10
*
* \brief
* Provides the initialization data structure for the ADC_1 Component.
*
********************************************************************************
* \copyright
* (c) 2017, Cypress Semiconductor Corporation. All rights reserved.
* This software, including source code, documentation and related
* materials ("Software"), is owned by Cypress Semiconductor
* Corporation ("Cypress") and is protected by and subject to worldwide
* patent protection (United States and foreign), United States copyright
* laws and international treaty provisions. Therefore, you may use this
* Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the
* Software source code solely for use in connection with Cypress's
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. Cypress reserves the right to make
* changes to the Software without notice. Cypress does not assume any
* liability arising out of the application or use of the Software or any
* product or circuit described in the Software. Cypress does not
* authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
#include "ADC_1.h"
#include "cyapicallbacks.h"

/******************************************************************************
* Custom Declarations and Variables
* - add user include files, prototypes and variables between the following
*   #START and #END tags
******************************************************************************/
/* `#START ADC_1_SYS_VAR`  */

/* `#END`  */

/******************************************************************************
* Function Name: ADC_1_ISR
*******************************************************************************
*
* \brief Handle Interrupt Service Routine.
*
* \param None
*
* \return None
*
* \sideeffect None
*
******************************************************************************/
void ADC_1_ISR(void)
{
    uint32_t intr_status;

    /* Read interrupt status register */
    intr_status = Cy_SAR_GetInterruptStatus(ADC_1_SAR__HW);

    /* ISR Macro Callback */
    #ifdef ADC_1_ISR_CALLBACK
        ADC_1_ISR_Callback();
    #endif

    /************************************************************************
    *  Custom Code
    *  - add user ISR code between the following #START and #END tags
    *************************************************************************/
    /* `#START MAIN_ADC_1_ISR`  */

    /* `#END`  */

    /* Clear handled interrupt */
    Cy_SAR_ClearInterrupt(ADC_1_SAR__HW, intr_status);
    /* Read interrupt status register to ensure write completed due to buffered writes */
    (void)Cy_SAR_GetInterruptStatus(ADC_1_SAR__HW);
}

/* [] END OF FILE */
