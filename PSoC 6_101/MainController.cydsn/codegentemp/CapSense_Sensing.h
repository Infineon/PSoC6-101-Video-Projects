/***************************************************************************//**
* \file CapSense_Sensing.h
* \version 2.0
*
* \brief
*   This file provides the headers of APIs specific to implementation of the
*   sensing FW block.
*
* \see CapSense v2.0 Datasheet
*
*//*****************************************************************************
* Copyright (2016-2017), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#if !defined(CY_SENSE_CapSense_SENSING_H)
#define CY_SENSE_CapSense_SENSING_H

#include "gpio/cy_gpio.h"
#include "cyfitter.h"
#include "syslib/cy_syslib.h"
#include "sysclk/cy_sysclk.h"
#include "CapSense_Configuration.h"
#include "CapSense_Structure.h"

#if (CY_CPU_CORTEX_M0P)
    #define CapSense_CPU_CLOCK_FREQ_HZ  (CYDEV_CLK_SLOWCLK__HZ)
    #define CapSense_CPU_CLOCK_FREQ_KHZ (CYDEV_CLK_SLOWCLK__KHZ)
    #define CapSense_CPU_CLOCK_FREQ_MHZ (CYDEV_CLK_SLOWCLK__MHZ)
#else
    #define CapSense_CPU_CLOCK_FREQ_HZ  (CYDEV_CLK_FASTCLK__HZ)
    #define CapSense_CPU_CLOCK_FREQ_KHZ (CYDEV_CLK_FASTCLK__KHZ)
    #define CapSense_CPU_CLOCK_FREQ_MHZ (CYDEV_CLK_FASTCLK__MHZ)
#endif

/*******************************************************************************
* Enumeration types definition
*******************************************************************************/

/* Pin states */
#define CapSense_GROUND                                     (0u)
#define CapSense_HIGHZ                                      (1u)
#define CapSense_SHIELD                                     (2u)
#define CapSense_SENSOR                                     (3u)
#define CapSense_TX_PIN                                     (4u)
#define CapSense_RX_PIN                                     (5u)

#if (CapSense_ENABLE != CapSense_ADC_STANDALONE_EN)
/*******************************************************************************
* Function Prototypes
*******************************************************************************/

/**
* \cond SECTION_CYSENSE_HIGH_LEVEL
* \addtogroup group_cysense_high_level
* \{
*/

cy_status CapSense_SetupWidget(uint32 widgetId);
cy_status CapSense_Scan(void);
cy_status CapSense_ScanAllWidgets(void);
uint32 CapSense_IsBusy(void);

/** \}
* \endcond */

/**
* \cond SECTION_CYSENSE_LOW_LEVEL
* \addtogroup group_cysense_low_level
* \{
*/

void CapSense_SetPinState(uint32 widgetId, uint32 sensorElement, uint32 state);

#if ((CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN) || \
     (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN))
cy_status CapSense_CalibrateWidget(uint32 widgetId);
cy_status CapSense_CalibrateAllWidgets(void);
#endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN) || \
           (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN)) */

/** \}
* \endcond */

/**
* \cond SECTION_CYSENSE_INTERNAL
* \addtogroup group_cysense_internal
* \{
*/

cy_status CapSense_SsInitialize(void);
void CapSense_SsSetSnsClockDivider(uint32 snsClk);
void CapSense_SsSetSnsFirstPhaseWidth(uint32 phaseWidth);
void CapSense_SsSetClockDividers(uint32 snsClk, uint32 modClk);
void CapSense_SsIsrInitialize(cy_israddress address);
void CapSense_SsPostAllWidgetsScan(void);
void CapSense_SsSetIOsInDefaultState(void);

#if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
     (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
     (CapSense_ENABLE == CapSense_ADC_EN))
    void CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_ENUM mode);
#endif

#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    cy_status CapSense_SsAutoTune(void);
#endif

#if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
    void CapSense_SsChangeClkFreq(uint32 chId);
#endif

#if (CapSense_ENABLE == CapSense_ADC_EN)
    cy_status CapSense_SsReleaseResources(void);
#endif

#if((CapSense_ENABLE == CapSense_CSD_EN) || \
    (CapSense_ENABLE == CapSense_CSX_EN))
    void CapSense_SsInitializeSourceSenseClk(void);
#endif

#if (CapSense_ENABLE == CapSense_CSD_EN)
    void CapSense_SsClearCSDSensors(void);
    uint32 CapSense_SsCSDGetColSnsClkDivider(uint32 widgetId);
    void CapSense_SsCSDSetColSnsClkDivider(uint32 widgetId, uint32 dividerVal);

    #if ((CapSense_ENABLE == CapSense_CSD_MATRIX_WIDGET_EN) || \
         (CapSense_ENABLE == CapSense_CSD_TOUCHPAD_WIDGET_EN))
        uint32 CapSense_SsCSDGetRowSnsClkDivider(uint32 widgetId);
        void CapSense_SsCSDSetRowSnsClkDivider(uint32 widgetId, uint32 dividerVal);
    #endif

    #if (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
        uint8 CapSense_SsCSDCalcPrsSize(uint32 snsClkDivider, uint32 resolution);
    #endif
#endif

void CapSense_BistDischargeExtCapacitors(void);

/** \}
* \endcond */

/***************************************
* Global software/external variables
***************************************/
extern volatile uint8 CapSense_widgetIndex;
extern volatile uint8 CapSense_sensorIndex;
extern uint8 CapSense_requestScanAllWidget;

extern CapSense_RAM_SNS_STRUCT *CapSense_curRamSnsPtr;

#if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
     (CapSense_ENABLE == CapSense_ADC_EN))
    extern CapSense_SENSE_METHOD_ENUM CapSense_currentSenseMethod;
#endif

#if(CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
    extern uint8 CapSense_scanFreqIndex;
#else
    extern const uint8 CapSense_scanFreqIndex;
#endif

#if ((CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) || \
     (CapSense_ENABLE == CapSense_CSX_EN))
    extern CapSense_FLASH_WD_STRUCT const *CapSense_curFlashWdgtPtr;
#endif

#if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
    /* Pointer to Flash structure holding info of sensor to be scanned */
    extern CapSense_FLASH_SNS_STRUCT const *CapSense_curFlashSnsPtr;
#endif

extern CapSense_FLASH_IO_STRUCT const *CapSense_curSnsIOPtr;

#endif /* (CapSense_ENABLE != CapSense_ADC_STANDALONE_EN) */

/****************************************************************************
* HW CSD Block Definition
****************************************************************************/
#define CapSense_CSD_HW                             ((CSD_Type*)CapSense_CSD__HW)

#define CapSense_CSD_CONFIG_REG                     ( CapSense_CSD_HW->CONFIG)
#define CapSense_CSD_CONFIG_PTR                     (&CapSense_CSD_HW->CONFIG)
#define CapSense_CSD_SPARE_REG                      ( CapSense_CSD_HW->SPARE)
#define CapSense_CSD_SPARE_PTR                      (&CapSense_CSD_HW->SPARE)
#define CapSense_CSD_STATUS_REG                     ( CapSense_CSD_HW->STATUS)
#define CapSense_CSD_STATUS_PTR                     (&CapSense_CSD_HW->STATUS)
#define CapSense_CSD_STAT_SEQ_REG                   ( CapSense_CSD_HW->STAT_SEQ)
#define CapSense_CSD_STAT_SEQ_PTR                   (&CapSense_CSD_HW->STAT_SEQ)
#define CapSense_CSD_STAT_CNTS_REG                  ( CapSense_CSD_HW->STAT_CNTS)
#define CapSense_CSD_STAT_CNTS_PTR                  (&CapSense_CSD_HW->STAT_CNTS)
#define CapSense_CSD_RESULT_VAL1_REG                ( CapSense_CSD_HW->RESULT_VAL1)
#define CapSense_CSD_RESULT_VAL1_PTR                (&CapSense_CSD_HW->RESULT_VAL1)
#define CapSense_CSD_RESULT_VAL2_REG                ( CapSense_CSD_HW->RESULT_VAL2)
#define CapSense_CSD_RESULT_VAL2_PTR                (&CapSense_CSD_HW->RESULT_VAL2)
#define CapSense_CSD_ADC_RES_REG                    ( CapSense_CSD_HW->ADC_RES)
#define CapSense_CSD_ADC_RES_PTR                    (&CapSense_CSD_HW->ADC_RES)
#define CapSense_CSD_INTR_REG                       ( CapSense_CSD_HW->INTR)
#define CapSense_CSD_INTR_PTR                       (&CapSense_CSD_HW->INTR)
#define CapSense_CSD_INTR_SET_REG                   ( CapSense_CSD_HW->INTR_SET)
#define CapSense_CSD_INTR_SET_PTR                   (&CapSense_CSD_HW->INTR_SET)
#define CapSense_CSD_INTR_MASK_REG                  ( CapSense_CSD_HW->INTR_MASK)
#define CapSense_CSD_INTR_MASK_PTR                  (&CapSense_CSD_HW->INTR_MASK)
#define CapSense_CSD_INTR_MASKED_REG                ( CapSense_CSD_HW->INTR_MASKED)
#define CapSense_CSD_INTR_MASKED_PTR                (&CapSense_CSD_HW->INTR_MASKED)
#define CapSense_CSD_HSCMP_REG                      ( CapSense_CSD_HW->HSCMP)
#define CapSense_CSD_HSCMP_PTR                      (&CapSense_CSD_HW->HSCMP)
#define CapSense_CSD_AMBUF_REG                      ( CapSense_CSD_HW->AMBUF)
#define CapSense_CSD_AMBUF_PTR                      (&CapSense_CSD_HW->AMBUF)
#define CapSense_CSD_REFGEN_REG                     ( CapSense_CSD_HW->REFGEN)
#define CapSense_CSD_REFGEN_PTR                     (&CapSense_CSD_HW->REFGEN)
#define CapSense_CSD_CSDCMP_REG                     ( CapSense_CSD_HW->CSDCMP)
#define CapSense_CSD_CSDCMP_PTR                     (&CapSense_CSD_HW->CSDCMP)
#define CapSense_CSD_SW_RES_REG                     ( CapSense_CSD_HW->SW_RES)
#define CapSense_CSD_SW_RES_PTR                     (&CapSense_CSD_HW->SW_RES)
#define CapSense_CSD_SENSE_PERIOD_REG               ( CapSense_CSD_HW->SENSE_PERIOD)
#define CapSense_CSD_SENSE_PERIOD_PTR               (&CapSense_CSD_HW->SENSE_PERIOD)
#define CapSense_CSD_SENSE_DUTY_REG                 ( CapSense_CSD_HW->SENSE_DUTY)
#define CapSense_CSD_SENSE_DUTY_PTR                 (&CapSense_CSD_HW->SENSE_DUTY)
#define CapSense_CSD_SW_HS_P_SEL_REG                ( CapSense_CSD_HW->SW_HS_P_SEL)
#define CapSense_CSD_SW_HS_P_SEL_PTR                (&CapSense_CSD_HW->SW_HS_P_SEL)
#define CapSense_CSD_SW_HS_N_SEL_REG                ( CapSense_CSD_HW->SW_HS_N_SEL)
#define CapSense_CSD_SW_HS_N_SEL_PTR                (&CapSense_CSD_HW->SW_HS_N_SEL)
#define CapSense_CSD_SW_SHIELD_SEL_REG              ( CapSense_CSD_HW->SW_SHIELD_SEL)
#define CapSense_CSD_SW_SHIELD_SEL_PTR              (&CapSense_CSD_HW->SW_SHIELD_SEL)
#define CapSense_CSD_SW_AMUXBUF_SEL_REG             ( CapSense_CSD_HW->SW_AMUXBUF_SEL)
#define CapSense_CSD_SW_AMUXBUF_SEL_PTR             (&CapSense_CSD_HW->SW_AMUXBUF_SEL)
#define CapSense_CSD_SW_BYP_SEL_REG                 ( CapSense_CSD_HW->SW_BYP_SEL)
#define CapSense_CSD_SW_BYP_SEL_PTR                 (&CapSense_CSD_HW->SW_BYP_SEL)
#define CapSense_CSD_SW_CMP_P_SEL_REG               ( CapSense_CSD_HW->SW_CMP_P_SEL)
#define CapSense_CSD_SW_CMP_P_SEL_PTR               (&CapSense_CSD_HW->SW_CMP_P_SEL)
#define CapSense_CSD_SW_CMP_N_SEL_REG               ( CapSense_CSD_HW->SW_CMP_N_SEL)
#define CapSense_CSD_SW_CMP_N_SEL_PTR               (&CapSense_CSD_HW->SW_CMP_N_SEL)
#define CapSense_CSD_SW_REFGEN_SEL_REG              ( CapSense_CSD_HW->SW_REFGEN_SEL)
#define CapSense_CSD_SW_REFGEN_SEL_PTR              (&CapSense_CSD_HW->SW_REFGEN_SEL)
#define CapSense_CSD_SW_FW_MOD_SEL_REG              ( CapSense_CSD_HW->SW_FW_MOD_SEL)
#define CapSense_CSD_SW_FW_MOD_SEL_PTR              (&CapSense_CSD_HW->SW_FW_MOD_SEL)
#define CapSense_CSD_SW_FW_TANK_SEL_REG             ( CapSense_CSD_HW->SW_FW_TANK_SEL)
#define CapSense_CSD_SW_FW_TANK_SEL_PTR             (&CapSense_CSD_HW->SW_FW_TANK_SEL)
#define CapSense_CSD_SW_DSI_SEL_REG                 ( CapSense_CSD_HW->SW_DSI_SEL)
#define CapSense_CSD_SW_DSI_SEL_PTR                 (&CapSense_CSD_HW->SW_DSI_SEL)
#define CapSense_CSD_IO_SEL_REG                     ( CapSense_CSD_HW->IO_SEL)
#define CapSense_CSD_IO_SEL_PTR                     (&CapSense_CSD_HW->IO_SEL)
#define CapSense_CSD_SEQ_TIME_REG                   ( CapSense_CSD_HW->SEQ_TIME)
#define CapSense_CSD_SEQ_TIME_PTR                   (&CapSense_CSD_HW->SEQ_TIME)
#define CapSense_CSD_SEQ_INIT_CNT_REG               ( CapSense_CSD_HW->SEQ_INIT_CNT)
#define CapSense_CSD_SEQ_INIT_CNT_PTR               (&CapSense_CSD_HW->SEQ_INIT_CNT)
#define CapSense_CSD_SEQ_NORM_CNT_REG               ( CapSense_CSD_HW->SEQ_NORM_CNT)
#define CapSense_CSD_SEQ_NORM_CNT_PTR               (&CapSense_CSD_HW->SEQ_NORM_CNT)
#define CapSense_CSD_ADC_CTL_REG                    ( CapSense_CSD_HW->ADC_CTL)
#define CapSense_CSD_ADC_CTL_PTR                    (&CapSense_CSD_HW->ADC_CTL)
#define CapSense_CSD_SEQ_START_REG                  ( CapSense_CSD_HW->SEQ_START)
#define CapSense_CSD_SEQ_START_PTR                  (&CapSense_CSD_HW->SEQ_START)
#define CapSense_CSD_IDACA_REG                      ( CapSense_CSD_HW->IDACA)
#define CapSense_CSD_IDACA_PTR                      (&CapSense_CSD_HW->IDACA)
#define CapSense_CSD_IDACB_REG                      ( CapSense_CSD_HW->IDACB)
#define CapSense_CSD_IDACB_PTR                      (&CapSense_CSD_HW->IDACB)

/****************************************************************************
* HW CSD Waveform Selection values
****************************************************************************/

#define CapSense_CSD_WAVEFORM_STATIC_OPEN           (0x00000000uL)
#define CapSense_CSD_WAVEFORM_STATIC_CLOSED         (0x00000001uL)
#define CapSense_CSD_WAVEFORM_PHI1                  (0x00000002uL)
#define CapSense_CSD_WAVEFORM_PHI2                  (0x00000003uL)
#define CapSense_CSD_WAVEFORM_PHI1_HSCMP            (0x00000004uL)
#define CapSense_CSD_WAVEFORM_PHI2_HSCMP            (0x00000005uL)
#define CapSense_CSD_WAVEFORM_HSCMP                 (0x00000006uL)
#define CapSense_CSD_WAVEFORM_SENSE_INV             (0x00000007uL)
#define CapSense_CSD_WAVEFORM_PHI1_DELAY            (0x00000008uL)
#define CapSense_CSD_WAVEFORM_PHI2_DELAY            (0x00000009uL)
#define CapSense_CSD_WAVEFORM_PHI1_INV              (0x0000000AuL)
#define CapSense_CSD_WAVEFORM_PHI2_INV              (0x0000000BuL)
#define CapSense_CSD_WAVEFORM_PHI1_HSCMP_INV        (0x0000000CuL)
#define CapSense_CSD_WAVEFORM_PHI2_HSCMP_INV        (0x0000000DuL)
#define CapSense_CSD_WAVEFORM_HSCMP_INV             (0x0000000EuL)
#define CapSense_CSD_WAVEFORM_SENSE                 (0x0000000FuL)

/****************************************************************************
* HW CSD Block Register Mask Definition
****************************************************************************/

/* CSD_CONFIG register masks */
#define CapSense_CSD_CONFIG_IREF_SEL_MSK                    (CSD_CONFIG_IREF_SEL_Msk)
#define CapSense_CSD_CONFIG_FILTER_DELAY_2_CYCLES           (0x00000002Lu)
#define CapSense_CSD_CONFIG_FILTER_DELAY_3_CYCLES           (0x00000003Lu)
#define CapSense_CSD_CONFIG_FILTER_DELAY_4_CYCLES           (0x00000004Lu)
#define CapSense_CSD_CONFIG_FILTER_DELAY_POS                (CSD_CONFIG_FILTER_DELAY_Pos)
#define CapSense_CSD_CONFIG_FILTER_DELAY_MSK                (CSD_CONFIG_FILTER_DELAY_Msk)
#define CapSense_CSD_CONFIG_FILTER_DELAY_12MHZ              (CapSense_CSD_CONFIG_FILTER_DELAY_2_CYCLES << CSD_CONFIG_FILTER_DELAY_Pos)
#define CapSense_CSD_CONFIG_FILTER_DELAY_24MHZ              (CapSense_CSD_CONFIG_FILTER_DELAY_3_CYCLES << CSD_CONFIG_FILTER_DELAY_Pos)
#define CapSense_CSD_CONFIG_FILTER_DELAY_48MHZ              (CapSense_CSD_CONFIG_FILTER_DELAY_4_CYCLES << CSD_CONFIG_FILTER_DELAY_Pos)
#define CapSense_CSD_CONFIG_SHIELD_DELAY_MSK                (CSD_CONFIG_SHIELD_DELAY_Msk)
#define CapSense_CSD_CONFIG_SHIELD_DELAY_POS                (CSD_CONFIG_SHIELD_DELAY_Pos)
#define CapSense_CSD_CONFIG_SENSE_EN_MSK                    (CSD_CONFIG_SENSE_EN_Msk)
#define CapSense_CSD_CONFIG_DSI_COUNT_SEL_POS               (CSD_CONFIG_DSI_COUNT_SEL_Pos)
#define CapSense_CSD_CONFIG_DSI_COUNT_SEL_MSK               (CSD_CONFIG_DSI_COUNT_SEL_Msk)
#define CapSense_CSD_CONFIG_SAMPLE_SYNC_MSK                 (CSD_CONFIG_SAMPLE_SYNC_Msk)
#define CapSense_CSD_CONFIG_ENABLE_MSK                      (CSD_CONFIG_ENABLE_Msk)

/* CSD_STAT_SEQ register masks */
#define CapSense_CSD_STAT_SEQ_SEQ_STATE_MSK                 (CSD_STAT_SEQ_SEQ_STATE_Msk)
#define CapSense_ADC_STAT_SEQ_SEQ_STATE_MSK                 (CSD_STAT_SEQ_ADC_STATE_Msk)

/* CSD_RESULT_VAL1 register masks */
#define CapSense_CSD_RESULT_VAL1_VALUE_MSK                  (CSD_RESULT_VAL1_VALUE_Msk)
#define CapSense_CSD_RESULT_VAL1_BAD_CONVS_MSK              (CSD_RESULT_VAL1_BAD_CONVS_Msk)
#define CapSense_CSD_RESULT_VAL1_BAD_CONVS_POS              (CSD_RESULT_VAL1_BAD_CONVS_Pos)

/* CSD_RESULT_VAL2 register masks */
#define CapSense_CSD_RESULT_VAL2_VALUE_MSK                  (CSD_RESULT_VAL2_VALUE_Msk)

/* CSD_INTR register masks */
#define CapSense_CSD_INTR_SAMPLE_MSK                        (CSD_INTR_SAMPLE_Msk)
#define CapSense_CSD_INTR_INIT_MSK                          (CSD_INTR_INIT_Msk)
#define CapSense_CSD_INTR_ADC_RES_MSK                       (CSD_INTR_ADC_RES_Msk)
#define CapSense_CSD_INTR_ALL_MSK                           (CSD_INTR_SAMPLE_Msk | CSD_INTR_INIT_Msk | CSD_INTR_ADC_RES_Msk)

/* CSD_INTR_MASK register masks */
#define CapSense_CSD_INTR_MASK_SAMPLE_MSK                   (CSD_INTR_MASK_SAMPLE_Msk)
#define CapSense_CSD_INTR_MASK_INIT_MSK                     (CSD_INTR_MASK_INIT_Msk)
#define CapSense_CSD_INTR_MASK_ADC_RES_MSK                  (CSD_INTR_MASK_ADC_RES_Msk)
#define CapSense_CSD_INTR_MASK_CLEAR_MSK                    (0uL)

/* CSD_HSCMP register masks */
#define CapSense_CSD_HSCMP_HSCMP_EN_MSK                     (CSD_HSCMP_HSCMP_EN_Msk)
#define CapSense_CSD_HSCMP_HSCMP_INVERT_MSK                 (CSD_HSCMP_HSCMP_INVERT_Msk)
#define CapSense_CSD_HSCMP_AZ_EN_MSK                        (CSD_HSCMP_AZ_EN_Msk)

/* CSD_AMBUF register masks */
#define CapSense_CSD_AMBUF_PWR_MODE_OFF                     (0uL)
#define CapSense_CSD_AMBUF_PWR_MODE_NORM                    (1uL)
#define CapSense_CSD_AMBUF_PWR_MODE_HI                      (2uL)

/* CSD_REFGEN register masks */
#define CapSense_CSD_REFGEN_REFGEN_EN_MSK                   (CSD_REFGEN_REFGEN_EN_Msk)
#define CapSense_CSD_REFGEN_BYPASS_MSK                      (CSD_REFGEN_BYPASS_Msk)
#define CapSense_CSD_REFGEN_VDDA_EN_MSK                     (CSD_REFGEN_VDDA_EN_Msk)
#define CapSense_CSD_REFGEN_RES_EN_MSK                      (CSD_REFGEN_RES_EN_Msk)
#define CapSense_CSD_REFGEN_GAIN_POS                        (CSD_REFGEN_GAIN_Pos)
#define CapSense_CSD_REFGEN_GAIN_MSK                        (CSD_REFGEN_GAIN_Msk)
#define CapSense_CSD_REFGEN_VREFLO_SEL_MSK                  (CSD_REFGEN_VREFLO_SEL_Msk)
#define CapSense_CSD_REFGEN_VREFLO_INT_MSK                  (CSD_REFGEN_VREFLO_INT_Msk)

#define CapSense_VREF_HI_OVERSHOOT_CORRECTION               (0x00000001uL)

#if(CapSense_ENABLE != CapSense_LP_MODE_EN)
    #define CapSense_DEFAULT_CSD_REFGEN_RES_CFG             (CapSense_CSD_REFGEN_RES_EN_MSK)
#else
    #define CapSense_DEFAULT_CSD_REFGEN_RES_CFG             (0u)
#endif /* (CapSense_ENABLE != CapSense_LP_MODE_EN) */

#define CapSense_DEFAULT_CSD_REFGEN_GAIN                    (CapSense_CSD_GAIN << CSD_REFGEN_GAIN_Pos)
#define CapSense_DEFAULT_CSD_REFGEN_VREFLO_SEL              (((CSD_REFGEN_VREFLO_SEL_Msk >> CSD_REFGEN_VREFLO_SEL_Pos) - CapSense_VREF_HI_OVERSHOOT_CORRECTION) <<  CSD_REFGEN_VREFLO_SEL_Pos)
#define CapSense_CSD_REFGEN_NO_SHIELD_CFG                   (CapSense_CSD_REFGEN_REFGEN_EN_MSK | CapSense_DEFAULT_CSD_REFGEN_RES_CFG | CapSense_DEFAULT_CSD_REFGEN_GAIN)
#define CapSense_CSD_REFGEN_WITH_SHIELD_CFG                 (CapSense_CSD_REFGEN_REFGEN_EN_MSK | CapSense_CSD_REFGEN_RES_EN_MSK | CapSense_DEFAULT_CSD_REFGEN_GAIN)

/* CSD_CSDCMP register masks */
#define CapSense_CSD_CSDCMP_CSDCMP_DISABLED                 (0uL)
#define CapSense_CSD_CSDCMP_CSDCMP_EN_MSK                   (CSD_CSDCMP_CSDCMP_EN_Msk)
#define CapSense_CSD_CSDCMP_CMP_PHASE_FULL                  (0uL << CSD_CSDCMP_CMP_PHASE_Pos)
#define CapSense_CSD_CSDCMP_CMP_PHASE_PHI1                  (1uL << CSD_CSDCMP_CMP_PHASE_Pos)
#define CapSense_CSD_CSDCMP_CMP_PHASE_PHI2                  (2uL << CSD_CSDCMP_CMP_PHASE_Pos)
#define CapSense_CSD_CSDCMP_CMP_PHASE_PHI1_2                (3uL << CSD_CSDCMP_CMP_PHASE_Pos)
#define CapSense_CSD_CSDCMP_AZ_EN_MSK                       (CSD_CSDCMP_AZ_EN_Msk)

/* CSD_SENSE_PERIOD register masks */
#define CapSense_CSD_SENSE_PERIOD_SENSE_DIV_MSK             (CSD_SENSE_PERIOD_SENSE_DIV_Msk)
#define CapSense_CSD_SENSE_PERIOD_LFSR_SIZE_MSK             (CSD_SENSE_PERIOD_LFSR_SIZE_Msk)
#define CapSense_CSD_SENSE_PERIOD_LFSR_SIZE_POS             (CSD_SENSE_PERIOD_LFSR_SIZE_Pos)
#define CapSense_CSD_SENSE_PERIOD_LFSR_SCALE_MSK            (CSD_SENSE_PERIOD_LFSR_SCALE_Msk)
#define CapSense_CSD_SENSE_PERIOD_LFSR_CLEAR_MSK            (CSD_SENSE_PERIOD_LFSR_CLEAR_Msk)
#define CapSense_CSD_SENSE_PERIOD_SEL_LFSR_MSB_MSK          (CSD_SENSE_PERIOD_SEL_LFSR_MSB_Msk)
#define CapSense_CSD_SENSE_PERIOD_LFSR_BITS_MSK             (CSD_SENSE_PERIOD_LFSR_BITS_Msk)

/* CSD_SENSE_DUTY register masks */
#define CapSense_CSD_SENSE_DUTY_SENSE_WIDTH_MSK             (CSD_SENSE_DUTY_SENSE_WIDTH_Msk)
#define CapSense_CSD_SENSE_DUTY_SENSE_POL_MSK               (CSD_SENSE_DUTY_SENSE_POL_Msk)
#define CapSense_CSD_SENSE_DUTY_SENSE_POL_PHI_LOW           (0uL)
#define CapSense_CSD_SENSE_DUTY_SENSE_POL_PHI_HIGH          (CSD_SENSE_DUTY_SENSE_POL_Msk)
#define CapSense_CSD_SENSE_DUTY_OVERLAP_PHI1_MSK            (CSD_SENSE_DUTY_OVERLAP_PHI1_Msk)
#define CapSense_CSD_SENSE_DUTY_OVERLAP_PHI2_MSK            (CSD_SENSE_DUTY_OVERLAP_PHI2_Msk)

#define CapSense_CSD_SW_HS_P_SEL_SW_HMPM_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_P_SEL_SW_HMPM_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMPM_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_P_SEL_SW_HMPM_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMPT_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_P_SEL_SW_HMPT_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMPT_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_P_SEL_SW_HMPT_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMPS_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_P_SEL_SW_HMPS_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMPS_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_P_SEL_SW_HMPS_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMMA_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_P_SEL_SW_HMMA_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMMA_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_P_SEL_SW_HMMA_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMMB_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_P_SEL_SW_HMMB_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMMB_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_P_SEL_SW_HMMB_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMRH_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_P_SEL_SW_HMRH_Pos)
#define CapSense_CSD_SW_HS_P_SEL_SW_HMRH_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_P_SEL_SW_HMRH_Pos)

#define CapSense_CSD_SW_HS_N_SEL_SW_HCRH_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_N_SEL_SW_HCRH_Pos)
#define CapSense_CSD_SW_HS_N_SEL_SW_HCRH_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_N_SEL_SW_HCRH_Pos)
#define CapSense_CSD_SW_HS_N_SEL_SW_HCRL_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_N_SEL_SW_HCRL_Pos)
#define CapSense_CSD_SW_HS_N_SEL_SW_HCRL_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_N_SEL_SW_HCRL_Pos)
#define CapSense_CSD_SW_HS_N_SEL_SW_HCCD_STATIC_OPEN        (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_HS_N_SEL_SW_HCCD_Pos)
#define CapSense_CSD_SW_HS_N_SEL_SW_HCCD_STATIC_CLOSE       (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_HS_N_SEL_SW_HCCD_Pos)

#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_HSCMP            (CapSense_CSD_WAVEFORM_HSCMP << CSD_SW_SHIELD_SEL_SW_HCAV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_PHI1             (CapSense_CSD_WAVEFORM_PHI1 << CSD_SW_SHIELD_SEL_SW_HCAV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_PHI2             (CapSense_CSD_WAVEFORM_PHI2 << CSD_SW_SHIELD_SEL_SW_HCAV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_PHI1_HSCMP       (CapSense_CSD_WAVEFORM_PHI1_HSCMP << CSD_SW_SHIELD_SEL_SW_HCAV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_PHI2_HSCMP       (CapSense_CSD_WAVEFORM_PHI2_HSCMP << CSD_SW_SHIELD_SEL_SW_HCAV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_STATIC_OPEN      (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_SHIELD_SEL_SW_HCAV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAV_STATIC_CLOSE     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_SHIELD_SEL_SW_HCAV_Pos)

#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_HSCMP            (CapSense_CSD_WAVEFORM_HSCMP << CSD_SW_SHIELD_SEL_SW_HCAG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_PHI1             (CapSense_CSD_WAVEFORM_PHI1 << CSD_SW_SHIELD_SEL_SW_HCAG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_PHI2             (CapSense_CSD_WAVEFORM_PHI2 << CSD_SW_SHIELD_SEL_SW_HCAG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_PHI1_HSCMP       (CapSense_CSD_WAVEFORM_PHI1_HSCMP << CSD_SW_SHIELD_SEL_SW_HCAG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_PHI2_HSCMP       (CapSense_CSD_WAVEFORM_PHI2_HSCMP << CSD_SW_SHIELD_SEL_SW_HCAG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_STATIC_OPEN      (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_SHIELD_SEL_SW_HCAG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCAG_STATIC_CLOSE     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_SHIELD_SEL_SW_HCAG_Pos)

#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_HSCMP            (CapSense_CSD_WAVEFORM_HSCMP << CSD_SW_SHIELD_SEL_SW_HCBV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_PHI1             (CapSense_CSD_WAVEFORM_PHI1 << CSD_SW_SHIELD_SEL_SW_HCBV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_PHI2             (CapSense_CSD_WAVEFORM_PHI2 << CSD_SW_SHIELD_SEL_SW_HCBV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_PHI1_HSCMP       (CapSense_CSD_WAVEFORM_PHI1_HSCMP << CSD_SW_SHIELD_SEL_SW_HCBV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_PHI2_HSCMP       (CapSense_CSD_WAVEFORM_PHI2_HSCMP << CSD_SW_SHIELD_SEL_SW_HCBV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_STATIC_OPEN      (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_SHIELD_SEL_SW_HCBV_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBV_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_SHIELD_SEL_SW_HCBV_Pos)

#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_HSCMP            (CapSense_CSD_WAVEFORM_HSCMP << CSD_SW_SHIELD_SEL_SW_HCBG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_PHI1             (CapSense_CSD_WAVEFORM_PHI1 << CSD_SW_SHIELD_SEL_SW_HCBG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_PHI2             (CapSense_CSD_WAVEFORM_PHI2 << CSD_SW_SHIELD_SEL_SW_HCBG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_PHI1_HSCMP       (CapSense_CSD_WAVEFORM_PHI1_HSCMP << CSD_SW_SHIELD_SEL_SW_HCBG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_PHI2_HSCMP       (CapSense_CSD_WAVEFORM_PHI2_HSCMP << CSD_SW_SHIELD_SEL_SW_HCBG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_STATIC_OPEN      (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_SHIELD_SEL_SW_HCBG_Pos)
#define CapSense_CSD_SW_SHIELD_SEL_SW_HCBG_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_SHIELD_SEL_SW_HCBG_Pos)

#define CapSense_CSD_SW_AMUXBUF_SEL_SW_IRLB_STATIC_CLOSE    (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_AMUXBUF_SEL_SW_IRLB_Pos)
#define CapSense_CSD_SW_AMUXBUF_SEL_SW_IRH_STATIC_CLOSE     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_AMUXBUF_SEL_SW_IRH_Pos)
#define CapSense_CSD_SW_AMUXBUF_SEL_SW_ICB_PHI2             (CapSense_CSD_WAVEFORM_PHI2 << CSD_SW_AMUXBUF_SEL_SW_ICB_Pos)
#define CapSense_CSD_SW_AMUXBUF_SEL_SW_DEFAULT              (CapSense_CSD_WAVEFORM_STATIC_OPEN)

/* CSD_SW_BYP_SEL register masks */
#define CapSense_CSD_SW_BYP_SEL_SW_BYA_MSK                  (CSD_SW_BYP_SEL_SW_BYA_Msk)
#define CapSense_CSD_SW_BYP_SEL_SW_BYB_MSK                  (CSD_SW_BYP_SEL_SW_BYB_Msk)

#define CapSense_CSD_SW_CMP_P_SEL_SW_SFPM_STATIC_OPEN       (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_CMP_P_SEL_SW_SFPM_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFPM_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_CMP_P_SEL_SW_SFPM_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFPT_STATIC_OPEN       (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_CMP_P_SEL_SW_SFPT_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFPT_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_CMP_P_SEL_SW_SFPT_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFPS_STATIC_OPEN       (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_CMP_P_SEL_SW_SFPS_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFPS_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_CMP_P_SEL_SW_SFPS_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFMA_STATIC_OPEN       (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_CMP_P_SEL_SW_SFMA_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFMA_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_CMP_P_SEL_SW_SFMA_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFCA_STATIC_OPEN       (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_CMP_P_SEL_SW_SFCA_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFCA_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_CMP_P_SEL_SW_SFCA_Pos)
#define CapSense_CSD_SW_CMP_P_SEL_SW_SFCA_MSK               (CSD_SW_CMP_P_SEL_SW_SFCA_Msk)

#define CapSense_CSD_SW_CMP_N_SEL_SW_SCRH_STATIC_OPEN       (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_CMP_N_SEL_SW_SCRH_Pos)
#define CapSense_CSD_SW_CMP_N_SEL_SW_SCRH_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_CMP_N_SEL_SW_SCRH_Pos)
#define CapSense_CSD_SW_CMP_N_SEL_SW_SCRL_STATIC_OPEN       (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_SW_CMP_N_SEL_SW_SCRL_Pos)
#define CapSense_CSD_SW_CMP_N_SEL_SW_SCRL_STATIC_CLOSE      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_CMP_N_SEL_SW_SCRL_Pos)

/* CSD_SW_REFGEN_SEL register masks */
#define CapSense_CSD_SW_REFGEN_SEL_SW_IAIB_MSK              (CSD_SW_REFGEN_SEL_SW_IAIB_Msk)
#define CapSense_CSD_SW_REFGEN_SEL_SW_IBCB_MSK              (CSD_SW_REFGEN_SEL_SW_IBCB_Msk)
#define CapSense_CSD_SW_REFGEN_SEL_SW_SGMB_MSK              (CSD_SW_REFGEN_SEL_SW_SGMB_Msk)
#define CapSense_CSD_SW_REFGEN_SEL_SW_SGRP_MSK              (CSD_SW_REFGEN_SEL_SW_SGRP_Msk)
#define CapSense_CSD_SW_REFGEN_SEL_SW_SGRE_MSK              (CSD_SW_REFGEN_SEL_SW_SGRE_Msk)
#define CapSense_CSD_SW_REFGEN_SEL_SW_SGR_MSK               (CSD_SW_REFGEN_SEL_SW_SGR_Msk)

#define CapSense_CSD_SW_FW_MOD_SEL_SW_F1PM_MSK              (CSD_SW_FW_MOD_SEL_SW_F1PM_Msk)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_F1PM_STATIC_CLOSE     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_MOD_SEL_SW_F1PM_Pos)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_F1MA_MSK              (CSD_SW_FW_MOD_SEL_SW_F1MA_Msk)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_F1MA_STATIC_CLOSE     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_MOD_SEL_SW_F1MA_Pos)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_F1CA_MSK              (CSD_SW_FW_MOD_SEL_SW_F1CA_Msk)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_F1CA_STATIC_CLOSE     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_MOD_SEL_SW_F1CA_Pos)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_F1CA_PHI2             (CapSense_CSD_WAVEFORM_PHI2 << CSD_SW_FW_MOD_SEL_SW_F1CA_Pos)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_C1CC_STATIC_OPEN      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_MOD_SEL_SW_C1CC_Pos)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_C1CC_STATIC_CLOSE     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_MOD_SEL_SW_C1CC_Pos)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_C1CD_STATIC_OPEN      (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_MOD_SEL_SW_C1CD_Pos)
#define CapSense_CSD_SW_FW_MOD_SEL_SW_C1CD_STATIC_CLOSE     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_MOD_SEL_SW_C1CD_Pos)

#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2PT_MSK             (CSD_SW_FW_TANK_SEL_SW_F2PT_Msk)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2PT_STATIC_CLOSE    (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_TANK_SEL_SW_F2PT_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2MA_MSK             (CSD_SW_FW_TANK_SEL_SW_F2MA_Msk)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2MA_STATIC_CLOSE    (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_TANK_SEL_SW_F2MA_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2CA_MSK             (CSD_SW_FW_TANK_SEL_SW_F2CA_Msk)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2CA_STATIC_CLOSE    (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_TANK_SEL_SW_F2CA_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2CB_MSK             (CSD_SW_FW_TANK_SEL_SW_F2CB_Msk)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2CB_STATIC_CLOSE    (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_TANK_SEL_SW_F2CB_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2CA_PHI2            (CapSense_CSD_WAVEFORM_PHI2 << CSD_SW_FW_TANK_SEL_SW_F2CA_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_F2CB_PHI2            (CapSense_CSD_WAVEFORM_PHI2 << CSD_SW_FW_TANK_SEL_SW_F2CB_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_C2CC_STATIC_OPEN     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_TANK_SEL_SW_C2CC_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_C2CC_STATIC_CLOSE    (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_TANK_SEL_SW_C2CC_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_C2CD_STATIC_OPEN     (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_TANK_SEL_SW_C2CD_Pos)
#define CapSense_CSD_SW_FW_TANK_SEL_SW_C2CD_STATIC_CLOSE    (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_SW_FW_TANK_SEL_SW_C2CD_Pos)

#define CapSense_CSD_TX_OUT_MSK                             (CSD_IO_SEL_CSD_TX_OUT_Msk)
#define CapSense_CSD_TX_OUT_STATIC_OPEN                     (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_IO_SEL_CSD_TX_OUT_Pos)

#define CapSense_CSD_TX_OUT_EN_MSK                          (CSD_IO_SEL_CSD_TX_OUT_EN_Msk)
#define CapSense_CSD_TX_OUT_EN_PHI1_DELAY                   (CapSense_CSD_WAVEFORM_PHI1_DELAY << 4u)
#define CapSense_CSD_TX_AMUXB_EN_MSK                        (CSD_IO_SEL_CSD_TX_AMUXB_EN_Msk)
#define CapSense_CSD_TX_AMUXB_EN_PHI2_DELAY                 (CapSense_CSD_WAVEFORM_PHI2_DELAY << 12u)
#define CapSense_CSD_TX_N_OUT_MSK                           (CSD_IO_SEL_CSD_TX_N_OUT_Msk)
#define CapSense_CSD_TX_N_OUT_STATIC_OPEN                   (CapSense_CSD_WAVEFORM_STATIC_OPEN << CSD_IO_SEL_CSD_TX_N_OUT_Pos)
#define CapSense_CSD_TX_N_OUT_STATIC_CLOSE                  (CapSense_CSD_WAVEFORM_STATIC_CLOSED << CSD_IO_SEL_CSD_TX_N_OUT_Pos)
#define CapSense_CSD_TX_N_OUT_EN_MSK                        (CSD_IO_SEL_CSD_TX_N_OUT_EN_Msk)
#define CapSense_CSD_TX_N_OUT_EN_PHI1                       (CapSense_CSD_WAVEFORM_PHI1 << CSD_IO_SEL_CSD_TX_N_OUT_EN_Pos)
#define CapSense_CSD_TX_N_AMUXA_EN_MSK                      (CSD_IO_SEL_CSD_TX_N_AMUXA_EN_Msk)
#define CapSense_CSD_TX_N_AMUXA_EN_PHI2                     (CapSense_CSD_WAVEFORM_PHI2 << CSD_IO_SEL_CSD_TX_N_AMUXA_EN_Pos)

/* IDACB used */
#if ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
     (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN))
        #define CapSense_IDACB_USED                         (1u)
#else
        #define CapSense_IDACB_USED                         (0u)
#endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
           (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)) */

/* CSD_SEQ_INIT_CNT register masks */
#define CapSense_CSD_SEQ_INIT_CNT_CONV_CNT_MSK              (CSD_SEQ_INIT_CNT_CONV_CNT_Msk)

/* CSD_SEQ_NORM_CNT register masks */
#define CapSense_CSD_SEQ_NORM_CNT_CONV_CNT_MSK              (CSD_SEQ_NORM_CNT_CONV_CNT_Msk)

/* CSD_SEQ_START register masks */
#define CapSense_CSD_SEQ_START_START_MSK                    (CSD_SEQ_START_START_Msk)
#define CapSense_CSD_SEQ_START_SEQ_MODE_MSK                 (CSD_SEQ_START_SEQ_MODE_Msk)
#define CapSense_CSD_SEQ_START_ABORT_MSK                    (CSD_SEQ_START_ABORT_Msk)
#define CapSense_CSD_SEQ_START_DSI_START_EN_MSK             (CSD_SEQ_START_DSI_START_EN_Msk)
#define CapSense_CSD_SEQ_START_AZ0_SKIP_MSK                 (CSD_SEQ_START_AZ0_SKIP_Msk)
#define CapSense_CSD_SEQ_START_AZ1_SKIP_MSK                 (CSD_SEQ_START_AZ1_SKIP_Msk)

/* CSD_IDACA register masks */
#define CapSense_CSD_IDACA_VAL_MSK                          (CSD_IDACA_VAL_Msk)
#define CapSense_CSD_IDACA_VAL_POS                          (CSD_IDACA_VAL_Pos)

#define CapSense_CSD_IDACA_POL_DYN_MSK                      (CSD_IDACA_POL_DYN_Msk)
#define CapSense_CSD_IDACA_POL_DYN_POS                      (CSD_IDACA_POL_DYN_Pos)

#define CapSense_CSD_IDACA_POLARITY_MSK                     (CSD_IDACA_POLARITY_Msk)
#define CapSense_CSD_IDACA_POLARITY_POS                     (CSD_IDACA_POLARITY_Pos)
#define CapSense_CSD_IDACA_POLARITY_VSSA_SRC                (0uL)
#define CapSense_CSD_IDACA_POLARITY_VDDA_SNK                (1uL)
#define CapSense_CSD_IDACA_POLARITY_SENSE                   (2uL)
#define CapSense_CSD_IDACA_POLARITY_SENSE_INV               (3uL)

#define CapSense_CSD_IDACA_BALL_MODE_MSK                    (CSD_IDACA_BAL_MODE_Msk)
#define CapSense_CSD_IDACA_BALL_MODE_POS                    (CSD_IDACA_BAL_MODE_Pos)
#define CapSense_CSD_IDACA_BALL_MODE_FULL                   (0uL)
#define CapSense_CSD_IDACA_BALL_MODE_PHI1                   (1uL)
#define CapSense_CSD_IDACA_BALL_MODE_PHI2                   (2uL)
#define CapSense_CSD_IDACA_BALL_MODE_PHI1_2                 (3uL)

#define CapSense_CSD_IDACA_LEG1_MODE_MSK                    (CSD_IDACA_LEG1_MODE_Msk)
#define CapSense_CSD_IDACA_LEG1_MODE_POS                    (CSD_IDACA_LEG1_MODE_Pos)
#define CapSense_CSD_IDACA_LEG1_MODE_GP_STATIC              (0uL)
#define CapSense_CSD_IDACA_LEG1_MODE_GP                     (1uL)
#define CapSense_CSD_IDACA_LEG1_MODE_CSD_STATIC             (2uL)
#define CapSense_CSD_IDACA_LEG1_MODE_CSD                    (3uL)

#define CapSense_CSD_IDACA_LEG2_MODE_MSK                    (CSD_IDACA_LEG2_MODE_Msk)
#define CapSense_CSD_IDACA_LEG2_MODE_POS                    (CSD_IDACA_LEG2_MODE_Pos)
#define CapSense_CSD_IDACA_LEG2_MODE_GP_STATIC              (0uL)
#define CapSense_CSD_IDACA_LEG2_MODE_GP                     (1uL)
#define CapSense_CSD_IDACA_LEG2_MODE_CSD_STATIC             (2uL)
#define CapSense_CSD_IDACA_LEG2_MODE_CSD                    (3uL)

#define CapSense_CSD_IDACA_RANGE_MSK                        (CSD_IDACA_RANGE_Msk)
#define CapSense_CSD_IDACA_RANGE_POS                        (CSD_IDACA_RANGE_Pos)
#define CapSense_CSD_IDACA_RANGE_IDAC_LO                    (0uL)
#define CapSense_CSD_IDACA_RANGE_IDAC_MED                   (1uL)
#define CapSense_CSD_IDACA_RANGE_IDAC_HI                    (2uL)

#define CapSense_CSD_IDACA_LEG1_EN_MSK                      (CSD_IDACA_LEG1_EN_Msk)
#define CapSense_CSD_IDACA_LEG2_EN_MSK                      (CSD_IDACA_LEG2_EN_Msk)

/* CSD_IDACB register masks */
#define CapSense_CSD_IDACB_VAL_MSK                          (CSD_IDACB_VAL_Msk)
#define CapSense_CSD_IDACB_VAL_POS                          (CSD_IDACB_VAL_Pos)

#define CapSense_CSD_IDACB_POL_DYN_MSK                      (CSD_IDACB_POL_DYN_Msk)
#define CapSense_CSD_IDACB_POL_DYN_POS                      (CSD_IDACB_POL_DYN_Pos)
#define CapSense_CSD_IDACB_POL_DYN_STATIC                   (0uL)
#define CapSense_CSD_IDACB_POL_DYN_DYNAMIC                  (1uL)

#define CapSense_CSD_IDACB_POLARITY_MSK                     (CSD_IDACB_POLARITY_Msk)
#define CapSense_CSD_IDACB_POLARITY_POS                     (CSD_IDACB_POLARITY_Pos)
#define CapSense_CSD_IDACB_POLARITY_VSSA_SRC                (0uL)
#define CapSense_CSD_IDACB_POLARITY_VDDA_SNK                (1uL)
#define CapSense_CSD_IDACB_POLARITY_SENSE                   (2uL)
#define CapSense_CSD_IDACB_POLARITY_SENSE_INV               (3uL)

#define CapSense_CSD_IDACB_BALL_MODE_MSK                    (CSD_IDACB_BAL_MODE_Msk)
#define CapSense_CSD_IDACB_BALL_MODE_POS                    (CSD_IDACB_BAL_MODE_Pos)
#define CapSense_CSD_IDACB_BALL_MODE_FULL                   (0uL)
#define CapSense_CSD_IDACB_BALL_MODE_PHI1                   (1uL)
#define CapSense_CSD_IDACB_BALL_MODE_PHI2                   (2uL)
#define CapSense_CSD_IDACB_BALL_MODE_PHI1_2                 (3uL)

#define CapSense_CSD_IDACB_LEG1_MODE_MSK                    (CSD_IDACB_LEG1_MODE_Msk)
#define CapSense_CSD_IDACB_LEG1_MODE_POS                    (CSD_IDACB_LEG1_MODE_Pos)
#define CapSense_CSD_IDACB_LEG1_MODE_GP_STATIC              (0uL)
#define CapSense_CSD_IDACB_LEG1_MODE_GP                     (1uL)
#define CapSense_CSD_IDACB_LEG1_MODE_CSD_STATIC             (2uL)
#define CapSense_CSD_IDACB_LEG1_MODE_CSD                    (3uL)

#define CapSense_CSD_IDACB_LEG2_MODE_MSK                    (CSD_IDACB_LEG2_MODE_Msk)
#define CapSense_CSD_IDACB_LEG2_MODE_POS                    (CSD_IDACB_LEG2_MODE_Pos)
#define CapSense_CSD_IDACB_LEG2_MODE_GP_STATIC              (0uL)
#define CapSense_CSD_IDACB_LEG2_MODE_GP                     (1uL)
#define CapSense_CSD_IDACB_LEG2_MODE_CSD_STATIC             (2uL)
#define CapSense_CSD_IDACB_LEG2_MODE_CSD                    (3uL)

#define CapSense_CSD_IDACB_RANGE_MSK                        (CSD_IDACB_RANGE_Msk)
#define CapSense_CSD_IDACB_RANGE_POS                        (CSD_IDACB_RANGE_Pos)
#define CapSense_CSD_IDACB_RANGE_IDAC_LO                    (0uL)
#define CapSense_CSD_IDACB_RANGE_IDAC_MED                   (1uL)
#define CapSense_CSD_IDACB_RANGE_IDAC_HI                    (2uL)

#define CapSense_CSD_IDACB_LEG1_EN_MSK                      (CSD_IDACB_LEG1_EN_Msk)
#define CapSense_CSD_IDACB_LEG2_EN_MSK                      (CSD_IDACB_LEG2_EN_Msk)
#define CapSense_CSD_IDACB_LEG3_EN_MSK                      (CSD_IDACB_LEG3_EN_Msk)

/***************************************
* PRS & LFSR masks
***************************************/
#define CapSense_LFSR_TABLE_SIZE                            (4u)

#define CapSense_PRS_LENGTH_2_BITS                          (0x00000003Lu)
#define CapSense_PRS_LENGTH_3_BITS                          (0x00000007Lu)
#define CapSense_PRS_LENGTH_4_BITS                          (0x0000000FLu)
#define CapSense_PRS_LENGTH_5_BITS                          (0x0000001FLu)
#define CapSense_PRS_LENGTH_8_BITS                          (0x000000FFLu)
#define CapSense_PRS_LENGTH_12_BITS                         (0x00000FFFLu)

#define CapSense_SNSCLK_SSC1_PERIOD                         (63u)
#define CapSense_SNSCLK_SSC2_PERIOD                         (127u)
#define CapSense_SNSCLK_SSC3_PERIOD                         (511u)
#define CapSense_SNSCLK_SSC4_PERIOD                         (1023u)

#define CapSense_SNSCLK_SSC1_RANGE                          (16u)
#define CapSense_SNSCLK_SSC2_RANGE                          (16u)
#define CapSense_SNSCLK_SSC3_RANGE                          (16u)
#define CapSense_SNSCLK_SSC4_RANGE                          (16u)

#define CapSense_LFSR_DITHER_PERCENTAGE                     (10uL)
#define CapSense_SNSCLK_SSC1_THRESHOLD                      (CapSense_LFSR_DITHER_PERCENTAGE * CapSense_SNSCLK_SSC1_RANGE)
#define CapSense_SNSCLK_SSC2_THRESHOLD                      (CapSense_LFSR_DITHER_PERCENTAGE * CapSense_SNSCLK_SSC2_RANGE)
#define CapSense_SNSCLK_SSC3_THRESHOLD                      (CapSense_LFSR_DITHER_PERCENTAGE * CapSense_SNSCLK_SSC3_RANGE)
#define CapSense_SNSCLK_SSC4_THRESHOLD                      (CapSense_LFSR_DITHER_PERCENTAGE * CapSense_SNSCLK_SSC4_RANGE)
#define CapSense_HFCLK_SNSCLK_FACTOR                        (160uL)

/* Clock register masks */
#define CapSense_MODCLK_CMD_DIV_SHIFT                       (0uL)
#define CapSense_MODCLK_CMD_PA_DIV_SHIFT                    (8uL)
#define CapSense_MODCLK_CMD_DISABLE_SHIFT                   (30uL)
#define CapSense_MODCLK_CMD_ENABLE_SHIFT                    (31uL)
#define CapSense_MODCLK_CMD_DISABLE_MASK                    ((uint32)(1uL << CapSense_MODCLK_CMD_DISABLE_SHIFT))
#define CapSense_MODCLK_CMD_ENABLE_MASK                     ((uint32)(1uL << CapSense_MODCLK_CMD_ENABLE_SHIFT))

#define CapSense_HSIOM_SEL_MASK                             (GPIO_HSIOM_MASK)
#define CapSense_HSIOM_SEL_GPIO                             (HSIOM_SEL_GPIO)
#define CapSense_HSIOM_SEL_CSD_SENSE                        (HSIOM_SEL_ACT_3)
#define CapSense_HSIOM_SEL_CSD_SHIELD                       (HSIOM_SEL_ACT_2)
#define CapSense_HSIOM_SEL_AMUXA                            (HSIOM_SEL_AMUXA)
#define CapSense_HSIOM_SEL_AMUXB                            (HSIOM_SEL_AMUXB)

/***************************************
* API Constants
***************************************/

#define CapSense_MOD_CSD_CLK_12MHZ                          (12000000uL)
#define CapSense_MOD_CSD_CLK_24MHZ                          (24000000uL)
#define CapSense_MOD_CSD_CLK_48MHZ                          (48000000uL)

#define CapSense_MIN_SNS_CLK_DIVIDER                        (4u)

/* Multi-scanning types and constants */
#define CapSense_FREQ_CHANNEL_0                             (0u)
#define CapSense_FREQ_CHANNEL_1                             (1u)
#define CapSense_FREQ_CHANNEL_2                             (2u)

/* Scanning status bit-mask */
#define CapSense_SW_STS_BUSY                                (CapSense_STATUS_CSD0_MASK)
#define CapSense_NOT_BUSY                                   (0uL)

#define CapSense_WDGT_SW_STS_BUSY                           (CapSense_STATUS_WDGT0_BUSY_MASK)

#define CapSense_PERCENTAGE_100                             (0x00000064uL)
#define CapSense_DISCONNECT_IO_FLAG                         (1u)

/* CMOD and CSH capacitor port-pins registers */
#if (CapSense_ENABLE == CapSense_CSD_EN)

    # if defined(CapSense_CSD__DEDICATED_IO1)
        #define CapSense_CTANK_CONNECTION                   (CapSense_CSD__DEDICATED_IO1)
    #else
        #define CapSense_CTANK_CONNECTION                   (0x00000000uL)
    #endif /* CapSense_CSD__DEDICATED_IO1 */

    #if(0u != CapSense_CSX_EN)
        #define CapSense_CMOD_CONNECTION                    (CapSense_CSD__CSHIELD_PAD)
    #else
        #define CapSense_CMOD_CONNECTION                    (CapSense_CSD__DEDICATED_IO0)
    #endif /* (0u != CapSense_CSX_EN) */

    #define CapSense_CSD_CMOD_PORT_PTR                      (CapSense_Cmod_0_PORT)
    #define CapSense_CSD_CMOD_PIN                           (CapSense_Cmod_0_NUM)

    #define CapSense_CSD_CTANK_PORT_PTR                     (CapSense_Csh_0_PORT)
    #define CapSense_CSD_CTANK_PIN                          (CapSense_Csh_0_NUM)

#endif  /* (CapSense_ENABLE == CapSense_CSD_EN) */

#if(CapSense_IREF_SRSS != CapSense_CSD_IREF_SOURCE)
    #define CapSense_IREF_SRC_CFG   (CSD_CONFIG_IREF_SEL_Msk)
#else
    #define CapSense_IREF_SRC_CFG   (0u)
#endif /* (CapSense_CSD_IREF_SRSS != CapSense_CSD_IREF_SOURCE) */

#if(CapSense_ENABLE != CapSense_LP_MODE_EN)
    #define CapSense_PWR_MODE_CFG   (0u)
#else
    #define CapSense_PWR_MODE_CFG   (CSD_CONFIG_LP_MODE_Msk)
#endif /* (CapSense_ENABLE != CapSense_LP_MODE_EN) */

#endif /* End CY_SENSE_CapSense_SENSING_H */


/* [] END OF FILE */
