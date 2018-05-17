/***************************************************************************//**
* \file     ADC_1.c
* \version  2.10
*
* \brief
* Provides the source code to the API for the ADC_1 Component.
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
#include <sysint/cy_sysint.h>
#include <cyfitter_sysint_cfg.h>

uint8_t ADC_1_initVar = 0u;
uint8_t ADC_1_selected = 0u; /* 0 if no configuration selected. 1 otherwise. */
uint32_t ADC_1_currentConfig = 0u; /* Currently active configuration */

/*******************************************************************************
* Function Name: ADC_1_Start
****************************************************************************//**
*
* \brief Performs all required initialization for this component and enables the
*  power. The power will be set to the appropriate power based on the clock
*  frequency.
*
* \param None
*
* \return None
*
* \sideeffect None
*
* \globalvars
*  \ref ADC_1_initVar (RW)
*
*******************************************************************************/
void ADC_1_Start(void)
{
    if (ADC_1_INIT_VAR_INIT_FLAG != (ADC_1_INIT_VAR_INIT_FLAG & ADC_1_initVar))
    {
        ADC_1_Init();
        ADC_1_initVar |= ADC_1_INIT_VAR_INIT_FLAG;
    }

    ADC_1_Enable();

    return;
}

/*******************************************************************************
* Function Name: ADC_1_StartEx
****************************************************************************//**
*
* \brief This function starts the ADC_1 and sets the Interrupt
* Service Routine to the provided address using the
* Cy_SysInt_Init() function. Refer to the Interrupt component
* datasheet for more information on the Cy_SysInt_Init() function.
*
* \param address This is the address of a user defined function for the ISR.
*
* \return None
*
* \sideeffect None
*
*******************************************************************************/
void ADC_1_StartEx(cy_israddress userIsr)
{
    ADC_1_Start();

    /* Interrupt core assignment will be up to the user. Initialize and enable the interrupt*/
    #ifdef ADC_1_IRQ__INTC_CORTEXM4_ASSIGNED
    #if (CY_CPU_CORTEX_M4)
        (void)Cy_SysInt_Init(&ADC_1_IRQ_cfg, userIsr);
        NVIC_EnableIRQ(ADC_1_IRQ_cfg.intrSrc);
    #endif
    #endif

    #ifdef ADC_1_IRQ__INTC_CORTEXM0P_ASSIGNED
    #if (CY_CPU_CORTEX_M0P)
        (void)Cy_SysInt_Init(&ADC_1_IRQ_cfg, userIsr);
        NVIC_EnableIRQ(ADC_1_IRQ_cfg.intrSrc);
    #endif
    #endif

}

/* ****************************************************************************
* Function Name: ADC_1_InitConfig
****************************************************************************//*
*
* \brief Configures all of the registers for a given configuration for scanning.
*
* \param scan Number of scan defined in the ADC_1.
*
* \return None
*
* \sideeffect None
*
*******************************************************************************/
void ADC_1_InitConfig(const ADC_1_CONFIG_STRUCT *config)
{
    bool deInitRouting = false;

    /* If there is an internal SAR clock, set up its divider values. */
    #if (ADC_1_CLOCK_INTERNAL)
        ADC_1_intSarClock_Disable();
        ADC_1_intSarClock_SetDivider(config->clkDivider);
        ADC_1_intSarClock_Enable();
    #endif /* ADC_1_CLOCK_INTERNAL */

    /* Init SAR and MUX registers */
    (void)Cy_SAR_DeInit(ADC_1_SAR__HW, deInitRouting);
    (void)Cy_SAR_Init(ADC_1_SAR__HW, config->hwConfigStc);

    /* Connect Vminus to VSSA when even one channel is single-ended or multiple channels configured */
    if(1uL == ADC_1_MUX_SWITCH0_INIT)
    {
        Cy_SAR_SetVssaVminusSwitch(ADC_1_SAR__HW, CY_SAR_SWITCH_CLOSE);

        /* Set MUX_HW_CTRL_VSSA in MUX_SWITCH_HW_CTRL when multiple channels enabled */
        if(1uL < config->numChannels)
        {
            Cy_SAR_SetVssaSarSeqCtrl(ADC_1_SAR__HW, CY_SAR_SWITCH_SEQ_CTRL_ENABLE);
        }
    }

    return;
}

/* ****************************************************************************
* Function Name: ADC_1_SelectConfig
****************************************************************************//*
*
* \brief Selects the predefined configuration for scanning.
*
* \param config Number of configuration in the ADC_1.
*
* \param restart Set to 1u if the ADC_1 should be  restarted after
* selecting the configuration.
*
*******************************************************************************/
void ADC_1_SelectConfig(uint32_t config, uint32_t restart)
{
    /* Check whether the configuration number is valid or not */
    if(ADC_1_TOTAL_CONFIGS > config)
    {
        /* Stop the ADC before changing configurations */
        ADC_1_Stop();
        ADC_1_selected = 1u;

        if(0u == ADC_1_initVar)
        {
            ADC_1_Init();
            ADC_1_initVar |= ADC_1_INIT_VAR_INIT_FLAG;
        }
        #if (ADC_1_VREF_ROUTED)
            ADC_1_vrefAMux_DisconnectAll();
        #endif

        ADC_1_InitConfig(&ADC_1_allConfigs[config]);

        #if (ADC_1_VREF_ROUTED)
            ADC_1_vrefAMux_Select((uint8)config);
        #endif

        ADC_1_currentConfig = config;

        if(1u == restart)
        {
            /* Restart the ADC */
            ADC_1_Start();

            /* Restart the scan */
            ADC_1_StartConvert();
        }
    }
    return;
}

/*******************************************************************************
* Function Name: ADC_1_StartConvert
****************************************************************************//**
*
* \brief In continuous mode, this API starts the conversion process and it runs
* continuously.

* In Single Shot mode, the function triggers a single scan and
* every scan requires a call of this function. The mode is set with the
* Sample Mode parameter in the customizer. The customizer setting can be
* overridden at run time with the ADC_1_SetConvertMode() function.
*
* \param None
*
* \return None
*
* \sideeffect None
*
*******************************************************************************/
void ADC_1_StartConvert(void)
{
    if (SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_Msk == (ADC_1_SAR__HW->SAMPLE_CTRL & SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_Msk))
    {
        Cy_SAR_StartConvert(ADC_1_SAR__HW, CY_SAR_START_CONVERT_CONTINUOUS);
    }
    else
    {
        Cy_SAR_StartConvert(ADC_1_SAR__HW, CY_SAR_START_CONVERT_SINGLE_SHOT);
    }
}

/*******************************************************************************
* Function Name: ADC_1_SetConvertMode
****************************************************************************//**
*
* \brief Sets the conversion mode to either Single-Shot or continuous. This
* function overrides the settings applied in the customizer. Changing
* configurations will restore the values set in the customizer.
*
* \param mode Sets the conversion mode.
*
* \return None
*
* \sideeffect None
*
*******************************************************************************/
void ADC_1_SetConvertMode(cy_en_sar_start_convert_sel_t mode)
{
    switch(mode)
    {
    case CY_SAR_START_CONVERT_CONTINUOUS:
        ADC_1_SAR__HW->SAMPLE_CTRL |= SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_Msk;
        break;
    case CY_SAR_START_CONVERT_SINGLE_SHOT:
    default:
        ADC_1_SAR__HW->SAMPLE_CTRL &= ~SAR_SAMPLE_CTRL_DSI_TRIGGER_LEVEL_Msk;
        break;
    }
}

/* ****************************************************************************
* Function Name: ADC_1_IRQ_Enable
****************************************************************************//*
*
* \brief Enables interrupts to occur at the end of a conversion. Global
* interrupts must also be enabled for the ADC_1 interrupts to occur.
*
* \param None
*
* \return None
*
* \sideeffect None
*
*******************************************************************************/
void ADC_1_IRQ_Enable(void){
    /* Interrupt core assignment will be up to the user. */
    #ifdef ADC_1_IRQ__INTC_CORTEXM4_ASSIGNED
    #if (CY_CPU_CORTEX_M4)
        NVIC_EnableIRQ(ADC_1_IRQ_cfg.intrSrc);
    #endif
    #endif

    #ifdef ADC_1_IRQ__INTC_CORTEXM0P_ASSIGNED
    #if (CY_CPU_CORTEX_M0P)
        NVIC_EnableIRQ(ADC_1_IRQ_cfg.intrSrc);
    #endif
    #endif
}

/* ****************************************************************************
* Function Name: ADC_1_IRQ_Disable
****************************************************************************//*
*
* \brief Disables end of conversion interrupts.
*
* \param None
*
* \return None
*
* \sideeffect None
*
*******************************************************************************/
void ADC_1_IRQ_Disable(void){
    /* Interrupt core assignment will be up to the user. */
    #ifdef ADC_1_IRQ__INTC_CORTEXM4_ASSIGNED
    #if (CY_CPU_CORTEX_M4)
        NVIC_DisableIRQ(ADC_1_IRQ_cfg.intrSrc);
    #endif
    #endif

    #ifdef ADC_1_IRQ__INTC_CORTEXM0P_ASSIGNED
    #if (CY_CPU_CORTEX_M0P)
        NVIC_DisableIRQ(ADC_1_IRQ_cfg.intrSrc);
    #endif
    #endif
}

/*******************************************************************************
* Function Name: ADC_1_SetEosMask
****************************************************************************//**
*
* \brief Sets or clears the End of Scan (EOS) interrupt mask.
*
* \param mask 1 to set the mask, 0 to clear the mask.
*
* \return None
*
* \sideeffect All other bits in the INTR register are cleared by this function.
*
*******************************************************************************/
void ADC_1_SetEosMask(uint32_t mask)
{
    uint32_t intrMaskReg;

    intrMaskReg = (0uL == mask) ? CY_SAR_DEINIT : SAR_INTR_MASK_EOS_MASK_Msk;

    Cy_SAR_SetInterruptMask(ADC_1_SAR__HW, intrMaskReg);
}

/******************************************************************************
* Function Name: ADC_1_SetChanMask
****************************************************************************//*
*
* \brief Sets enable/disable mask for all channels in current configuration.
*
*
* \param enableMask
*  Channel enable/disable mask.
*
* \sideeffect
*  None.
*
*******************************************************************************/
void ADC_1_SetChanMask(uint32_t enableMask)
{
    uint32 chanCount = ADC_1_allConfigs[ADC_1_currentConfig].numChannels;
    enableMask &= (uint32)((uint32)(1ul << chanCount) - 1ul);

    Cy_SAR_SetChanMask(ADC_1_SAR__HW, enableMask);
}

/*******************************************************************************
* Function Name: ADC_1_IsEndConversion
****************************************************************************//**
*
* \brief Immediately returns the status of the conversion or does not return
* (blocking) until the conversion completes, depending on the retMode parameter.
* In blocking mode, there is a time out of about 10 seconds for a CPU speed of
* 100 Mhz.
*
* \param retMode Check conversion return mode.
*
* \return uint32_t: If a nonzero value is returned, the last conversion is complete.
* If the returned value is zero, the ADC_1 is still calculating the last result.
*
* \sideeffect This function reads the end of conversion status, and clears it afterward.
*
*******************************************************************************/
uint32_t ADC_1_IsEndConversion(cy_en_sar_return_mode_t retMode)
{
    uint32 endOfConversion = 0u;
    cy_en_sar_status_t result;

    result = Cy_SAR_IsEndConversion(ADC_1_SAR__HW, retMode);

    if (result == CY_SAR_SUCCESS)
    {
        endOfConversion = 1u;
    }

    return endOfConversion;
}

/* ****************************************************************************
* Function Name: ADC_1_Init
****************************************************************************//*
*
* \brief Initialize the component according to parameters defined in the
* customizer.
*
* \param None
*
* \return None
*
* \sideeffect None
*
*******************************************************************************/
void ADC_1_Init(void)
{
    uint32_t configNum = 0u;

    if(0u == ADC_1_initVar)
    {
        /* Interrupt core assignment will be up to the user. Initialize but do not enable the interrupt*/
        #ifdef ADC_1_IRQ__INTC_CORTEXM4_ASSIGNED
        #if (CY_CPU_CORTEX_M4)
            (void)Cy_SysInt_Init(&ADC_1_IRQ_cfg, &ADC_1_ISR);
        #endif
        #endif

        #ifdef ADC_1_IRQ__INTC_CORTEXM0P_ASSIGNED
        #if (CY_CPU_CORTEX_M0P)
            (void)Cy_SysInt_Init(&ADC_1_IRQ_cfg, &ADC_1_ISR);
        #endif
        #endif

        /* Initialize configuration zero if SelectConfig has not been called */
        if(0u == ADC_1_selected)
        {
            ADC_1_selected = 1u;
            configNum = 0uL;

            /* Change Vref selection if is was routed by Creator. Break. */
            #if (ADC_1_VREF_ROUTED)
                ADC_1_vrefAMux_DisconnectAll();
            #endif

            ADC_1_InitConfig(&ADC_1_allConfigs[configNum]);

            /* Change Vref selection if is was routed by Creator. Make. */
            #if (ADC_1_VREF_ROUTED)
                ADC_1_vrefAMux_Select((uint8)configNum);
            #endif
        }
    }
}
/* [] END OF FILE */
