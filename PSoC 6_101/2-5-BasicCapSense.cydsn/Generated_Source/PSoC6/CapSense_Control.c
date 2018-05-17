/***************************************************************************//**
* \file CapSense_Control.c
* \version 2.0
*
* \brief
*   This file provides the source code to the Control module API of the Component.
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

#include <stddef.h>
#include "syslib/cy_syslib.h"
#include "syspm/cy_syspm.h"
#include "cyfitter_sysint_cfg.h"
#include "CapSense_Configuration.h"
#include "CapSense_Structure.h"
#include "CapSense_Control.h"
#include "CapSense_Processing.h"
#include "CapSense_Filter.h"
#include "CapSense_Sensing.h"
#include "CapSense_Tuner.h"

#if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
    #include "CapSense_SelfTest.h"
#endif

#if (0u != CapSense_ADC_EN)
    #include "CapSense_Adc.h"
#endif /* (0u != CapSense_ADC_EN) */

#if ((CapSense_ENABLE == CapSense_GES_GLOBAL_EN) ||\
     (CapSense_ENABLE == CapSense_BALLISTIC_MULTIPLIER_EN))
    #include "CapSense_Gesture.h"
#endif /* (CapSense_ENABLE == CapSense_GES_GLOBAL_EN) */

/***********************************************************************************************************************
* Local definition
***********************************************************************************************************************/
#define CapSense_INIT_DONE   (1u)
#define CapSense_INIT_NEEDED (0u)

/***********************************************************************************************************************
* Local variables
***********************************************************************************************************************/
static uint8 CapSense_InitVar = CapSense_INIT_NEEDED;

/***********************************************************************************************************************
* Local function
***********************************************************************************************************************/
static cy_en_syspm_status_t CapSense_SwitchPowerModeCallback(cy_stc_syspm_callback_params_t *callbackParams);

/*******************************************************************************
* Function Name: CapSense_Start
****************************************************************************//**
*
* \brief
*  Initializes the Component hardware and firmware modules. This function is
*  called by the application program prior to calling any other function of the
*  Component.
*
* \details
*  This function initializes the Component hardware and firmware modules and
*  is called by the application program prior to calling any other API
*  of the Component. When this function is called, the following tasks are
*  executed as part of the initialization process:
*    1. Initialize the registers of the \ref group_structures variable
*       CapSense_dsRam based on the user selection in the Component
*       configuration wizard.
*    2. Configure the hardware to perform capacitive sensing.
*    3. If SmartSense Auto-tuning is selected for the CSD Tuning mode in the
*       Basic tab, the auto-tuning algorithm is executed to set the optimal
*       values for the hardware parameters of the widgets/sensors.
*    4. Calibrate the sensors and find the optimal values for IDACs of each
*       widget / sensor, if the Enable IDAC auto-calibration is enabled in the
*       CSD Setting or CSX Setting tabs.
*    5. Perform scanning for all the sensors and initialize the baseline history.
*    6. If the firmware filters are enabled in the Advanced General tab, the
*       filter histories are also initialized.

*  Any next call of this API repeats an initialization process except for
*  data structure initialization. Therefore, it is possible to change the
*  Component configuration from the application program by writing registers to the
*  data structure and calling this function again. This is also
*  done inside the CapSense_RunTuner() function when a restart command
*  is received.
*
*  When the Component operation is stopped by the CapSense_Stop()
*  function, the CapSense_Start() function repeats an initialization
*  process including data structure initialization.
*
* \return
*  Returns the status of the initialization process. If CY_RET_SUCCESS is not
*  received, some of the initialization fails and the Component may not operate
*  as expected.
*
*******************************************************************************/
cy_status CapSense_Start(void)
{
    cy_status result;

    /* Initialize CapSense modules */
    result = CapSense_Initialize();

    #if (CapSense_CSD_AUTOTUNE != CapSense_CSD_SS_DIS)
        if (CY_RET_SUCCESS == result)
        {
            result = CapSense_SsAutoTune();
        }
    #endif /* #if (CapSense_CSD_AUTOTUNE != CapSense_CSD_SS_DIS) */

    #if ((0u != CapSense_CSX_IDAC_AUTOCAL_EN) || \
        ((CapSense_CSD_AUTOTUNE == CapSense_CSD_SS_DIS) && (0u != CapSense_CSD_IDAC_AUTOCAL_EN)))
        if (CY_RET_SUCCESS == result)
        {
            result = CapSense_CalibrateAllWidgets();
        }
    #endif

    if (CY_RET_SUCCESS == result)
    {
        result = CapSense_ScanAllWidgets();
    }

    while(CapSense_NOT_BUSY != CapSense_IsBusy()){}

    CapSense_FtInitialize();
    CapSense_InitializeAllBaselines();

    #if(CapSense_ENABLE == CapSense_GES_GLOBAL_EN)
        if (CY_RET_SUCCESS == result)
        {
            CapSense_InitializeGestures();
        }
    #endif /* (CapSense_ENABLE == CapSense_GES_GLOBAL_EN) */

    #if (0u != CapSense_BALLISTIC_MULTIPLIER_EN)
        if (CY_RET_SUCCESS == result)
        {
            CapSense_InitializeBallisticMultiplier();
        }
    #endif /* (0u != CapSense_BALLISTIC_MULTIPLIER_EN) */

    #if (0u != CapSense_ADC_EN)
        CapSense_AdcInitialize();
    #endif /* (0u != CapSense_ADC_EN) */

    return result;
}

/*******************************************************************************
* Function Name: CapSense_Initialize
****************************************************************************//**
*
* \brief
*  This function initializes the CapSense Component.
*
* \details
*  This API initializes all sub-modules of the CapSense Component:
*   - Data Structure - set the default Component parameters defined in the Customizer.
*   - Data Processing - resets all widget statuses.
*   - Tuner - resets tuning state.
*   - Sensing - prepares CSD HW for operation.
*
*  Note that Data Structure module is initialized only once after the reset or
*  CapSense_Stop() API is called. The repeated calls of Initialize API
*  will not re-initialize Data Structure. This is done to preserve Component
*  parameters that user may set in runtime.
*
* \return
*  Return CY_RET_SUCCESS if the initialization was successful.
*
*******************************************************************************/
cy_status CapSense_Initialize(void)
{
    cy_status result;

    /* The Data Structure and Tuner are initialized only once */
    if (CapSense_INIT_NEEDED == CapSense_InitVar)
    {
        CapSense_DsInitialize();
        CapSense_TuInitialize();
        CapSense_InitVar = CapSense_INIT_DONE;
    }

    CapSense_DpInitialize();

    result = CapSense_SsInitialize();

    return result;
}

/*******************************************************************************
* Function Name: CapSense_Stop
****************************************************************************//**
*
* \brief
*  Stops the Component operation.
*
* \details
*  This function stops the Component operation, no sensor scanning can be
*  executed when the Component is stopped. Once stopped, the hardware block may
*  be reconfigured by the application program for any other special usage. The
*  Component operation can be resumed by calling the CapSense_Resume()
*  function or the Component can be reset by calling the
*  CapSense_Start() function.
*
*  This function is called when no scanning is in progress.
*  I.e. CapSense_IsBusy() returns a non-busy status.
*
* \return
*  Returns the status of the stop process. If CY_RET_SUCCESS is not received,
*  the stop process fails and retries may be required.
*
*******************************************************************************/
cy_status CapSense_Stop(void)
{
    cy_status result = CY_RET_SUCCESS;

    #if (CapSense_ENABLE == CapSense_ADC_EN)
        /* Release CSD resources */
        result = CapSense_SsReleaseResources();

        /* Release ADC resources */
        CapSense_AdcStop();
    #endif /* (CapSense_ENABLE == CapSense_ADC_EN) */

    CapSense_InitVar = CapSense_INIT_NEEDED;

    return result;
}

/*******************************************************************************
* Function Name: CapSense_Resume
****************************************************************************//**
*
* \brief
*  Resumes the Component operation if the CapSense_Stop() function was
*  called previously.
*
* \details
*  This function resumes the Component operation if the operation is stopped
*  previously by the CapSense_Stop() function. The following tasks are
*  executed as part of the operation resume process:
*    1. Reset all the Widgets/Sensors statuses.
*    2. Configure the hardware to perform capacitive sensing.
*
* \return
*  Returns the status of the resume process. If CY_RET_SUCCESS is not received,
*  the resume process fails and retries may be required.
*
*******************************************************************************/
cy_status CapSense_Resume(void)
{
    cy_status result;

    /* The Tuner are initialized only once in order not to break communication */
    if (CapSense_INIT_NEEDED == CapSense_InitVar)
    {
        CapSense_TuInitialize();
        CapSense_InitVar = CapSense_INIT_DONE;
    }
    CapSense_DpInitialize();

    result = CapSense_SsInitialize();

    return result;
}

/*******************************************************************************
* Function Name: CapSense_ProcessAllWidgets
****************************************************************************//**
*
* \brief
*  Performs full data processing of all enabled widgets.
*
* \details
*  This function performs all data processes for all enabled widgets in the
*  Component. The following tasks are executed as part of processing all the
*  widgets:
*    1. Apply raw count filters to the raw counts, if they are enabled in the
*       customizer.
*    2. Update the thresholds if the SmartSense Full Auto-Tuning is enabled in
*       the customizer.
*    3. Update the baselines and difference counts for all the sensors.
*    4. Update the sensor and widget status (on/off), update the centroid for
*       the sliders and the X/Y position for the touchpads.
*
*  This function is called by an application program only after all the enabled
*  widgets (and sensors) in the Component is scanned. Calling this function
*  multiple times without sensor scanning causes unexpected behavior.
*
*  The disabled widgets are not processed by this function. To disable/enable
*  a widget, set the appropriate values in the
*  CapSense_WDGT_ENABLE<RegisterNumber>_PARAM_ID register using the
*  CapSense_SetParam() function.
*
*  If the Ballistic multiplier filter is enabled the Timestamp must be
*  updated before calling this function using the
*  CapSense_IncrementGestureTimestamp() function.
*
*  If the Self-test library is enabled, this function executes the baseline duplication
*  test. Refer to CapSense_CheckBaselineDuplication() for details.
*
* \return
*  Returns the status of the processing operation. If CY_RET_SUCCESS is not received,
*  the processing fails and retries may be required.
*
*******************************************************************************/
cy_status CapSense_ProcessAllWidgets(void)
{
    uint32 wdgtId;
    cy_status result = CY_RET_SUCCESS;
    CapSense_FLASH_WD_STRUCT const *ptrFlashWdgt;

    ptrFlashWdgt = CapSense_dsFlash.wdgtArray;

    for (wdgtId = 0u; wdgtId < CapSense_TOTAL_WIDGETS; wdgtId++)
    {
        if (0uL != CapSense_GET_WIDGET_EN_STATUS(wdgtId))
        {
            switch(CapSense_GET_SENSE_METHOD(ptrFlashWdgt))
            {
            #if (0u != CapSense_TOTAL_CSD_WIDGETS)
                case CapSense_SENSE_METHOD_CSD_E:
                    result = CapSense_DpProcessCsdWidgetRawCounts(ptrFlashWdgt);
                    CapSense_DpProcessCsdWidgetStatus(wdgtId, ptrFlashWdgt);
                    break;
            #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

            #if (0u != CapSense_TOTAL_CSX_WIDGETS)
                case CapSense_SENSE_METHOD_CSX_E:
                    result = CapSense_DpProcessCsxWidgetRawCounts(ptrFlashWdgt);
                    CapSense_DpProcessCsxWidgetStatus(wdgtId, ptrFlashWdgt);
                    break;
            #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

            default:
                CY_ASSERT(0 != 0);
                break;
            }

            #if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
                if (CY_RET_SUCCESS != result)
                {
                    CapSense_UpdateTestResultBaselineDuplication(wdgtId, result);
                    result = CY_RET_BAD_DATA;
                }
            #endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */

        }
        ptrFlashWdgt++;
    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_ProcessWidget
****************************************************************************//**
*
* \brief
*  Performs full data processing of the specified widget if it is enabled.
*
* \details
*  This function performs exactly the same tasks as
*  CapSense_ProcessAllWidgets(), but only for a specified widget. This
*  function can be used along with the CapSense_SetupWidget() and
*  CapSense_Scan() functions to scan and process data for a specific
*  widget. This function is called only after all the sensors in the
*  widgets are scanned. A disabled widget is not processed by this function.
*
*  A pipeline scan method (i.e. during scanning of a widget perform processing
*  of the previously scanned widget) can be implemented using this function and
*  it may reduce the total execution time, increase the refresh rate and
*  decrease the average power consumption.
*
*  If the Ballistic multiplier filter is enabled the Timestamp must be
*  updated before calling this function using the
*  CapSense_IncrementGestureTimestamp() function.
*
*  If the Self-test library is enabled, this function executes the baseline duplication
*  test. Refer to CapSense_CheckBaselineDuplication() for details.
*
* \param widgetId
*  Specifies the ID number of the widget to be processed.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID
*
* \return
*  Returns the status of the widget processing:
*  - CY_RET_SUCCESS - The operation is successfully completed
*  - CY_RET_BAD_PARAM - The input parameter is invalid
*  - CY_RET_INVALID_STATE - The specified widget is disabled
*  - CY_RET_BAD_DATA - The processing is failed
*
*******************************************************************************/
cy_status CapSense_ProcessWidget(uint32 widgetId)
{
    cy_status result = CY_RET_SUCCESS;
    CapSense_FLASH_WD_STRUCT const *ptrFlashWdgt;

    if (widgetId >= CapSense_TOTAL_WIDGETS)
    {
        result = CY_RET_BAD_PARAM;
    }

    if ((CY_RET_SUCCESS == result) && (0uL == CapSense_GET_WIDGET_EN_STATUS(widgetId)))
    {
        result = CY_RET_INVALID_STATE;
    }

    if (CY_RET_SUCCESS == result)
    {
        ptrFlashWdgt = &CapSense_dsFlash.wdgtArray[widgetId];

        switch(CapSense_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            case CapSense_SENSE_METHOD_CSD_E:
                result = CapSense_DpProcessCsdWidgetRawCounts(ptrFlashWdgt);
                CapSense_DpProcessCsdWidgetStatus(widgetId, ptrFlashWdgt);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

        #if (0u != CapSense_TOTAL_CSX_WIDGETS)
            case CapSense_SENSE_METHOD_CSX_E:
                result = CapSense_DpProcessCsxWidgetRawCounts(ptrFlashWdgt);
                CapSense_DpProcessCsxWidgetStatus(widgetId, ptrFlashWdgt);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

        default:
            CY_ASSERT(0 != 0);
            break;
        }

        #if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
            if (CY_RET_SUCCESS != result)
            {
                CapSense_UpdateTestResultBaselineDuplication(widgetId, result);
                result = CY_RET_BAD_DATA;
            }
        #endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */
    }
    return result;
}


/*******************************************************************************
* Function Name: CapSense_ProcessWidgetExt
****************************************************************************//**
*
* \brief
*  Performs customized data processing on the selected widget.
*
* \details
*  This function performs data processes for the specified widget specified by
*  the mode parameter. The execution order of the requested operations is from
*  LSB to MSB of the mode parameter. For a different order, this API
*  can be called multiple times with the required mode parameter.
*
*  This function can be used with any of the available scan functions. This
*  function is called only after all the sensors in the specified widget are
*  scanned. Calling this function multiple times with the same mode without
*  sensor scanning causes unexpected behavior. This function ignores the value
*  of the wdgtEnable register.
*
*  The CapSense_PROCESS_CALC_NOISE and
*  CapSense_PROCESS_THRESHOLDS flags are supported by the CSD sensing
*  method only when Auto-tuning mode is enabled.
*
*  The pipeline scan method (i.e. during scanning of a widget, processing
*  of a previously scanned widget is performed) can be implemented using this
*  function and it may reduce the total scan/process time, increase the refresh
*  rate and decrease the power consumption.
*
*  If the Self-test library is enabled, this function executes the baseline duplication
*  test. Refer to CapSense_CheckBaselineDuplication() for details.
*
*  If the Ballistic multiplier filter is enabled the Timestamp must be
*  updated before calling this function using the
*  CapSense_IncrementGestureTimestamp() function.
*
* \param widgetId
*  Specifies the ID number of the widget to be processed.
*  A macro for the widget ID can be found in the CapSense Configuration header
*  file defined as CapSense_<WidgetName>_WDGT_ID.

* \param mode
*  Specifies the type of widget processing to be executed for the
*  specified widget:
*    1. Bits [31..6] - Reserved.
*    2. Bits [5..0] - CapSense_PROCESS_ALL - Execute all the tasks.
*    3. Bit [5] - CapSense_PROCESS_STATUS - Update the status (on/off,
*       centroid position).
*    4. Bit [4] - CapSense_PROCESS_THRESHOLDS - Update the thresholds
*       (only in CSD auto-tuning mode).
*    5. Bit [3] - CapSense_PROCESS_CALC_NOISE - Calculate the noise (only in
*       CSD auto-tuning mode).
*    6. Bit [2] - CapSense_PROCESS_DIFFCOUNTS - Update the difference counts.
*    7. Bit [1] - CapSense_PROCESS_BASELINE - Update the baselines.
*    8. Bit [0] - CapSense_PROCESS_FILTER - Run the firmware filters.
*
* \return
*  Returns the status of the widget processing operation:
*  - CY_RET_SUCCESS - The processing is successfully performed.
*  - CY_RET_BAD_PARAM - The input parameter is invalid.
*  - CY_RET_BAD_DATA - The processing failed.
*
*******************************************************************************/
cy_status CapSense_ProcessWidgetExt(uint32 widgetId, uint32 mode)
{
    uint32 snsCount;
    cy_status result = CY_RET_BAD_PARAM;
    CapSense_FLASH_WD_STRUCT const *ptrFlashWdgt;
    CapSense_PTR_FILTER_VARIANT fltrHistV;
    CapSense_RAM_SNS_STRUCT *ptrSnsTmp;

    #if (0u != CapSense_TOTAL_CSD_WIDGETS)
        uint32 isProxWdgt;
    #endif

    if (widgetId < CapSense_TOTAL_WIDGETS)
    {
        ptrFlashWdgt = &CapSense_dsFlash.wdgtArray[widgetId];
        snsCount = CapSense_GET_SNS_CNT_BY_PTR(ptrFlashWdgt);
        ptrSnsTmp = ptrFlashWdgt->ptr2SnsRam;
        fltrHistV.ptr = ptrFlashWdgt->ptr2FltrHistory;

        switch(CapSense_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            case CapSense_SENSE_METHOD_CSD_E:
            {
                /* Determine if widget is type of proximity.
                 * The Proximity widgets use different filters and
                 * therefore have different filter history object structure */
                isProxWdgt = (CapSense_GET_WIDGET_TYPE(ptrFlashWdgt) == CapSense_WD_PROXIMITY_E) ? 1Lu : 0Lu;

                /* Run the desired processing for the all CSD widget sensors */
                for (;snsCount-- > 0u;)
                {
                    result = CapSense_DpProcessCsdSensorRawCountsExt(ptrFlashWdgt, ptrSnsTmp, fltrHistV, mode);

                    #if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
                        if (CY_RET_SUCCESS != result)
                        {
                            result = (result | CapSense_TST_LSBYTE) & snsCount;
                        }
                    #endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */

                    /* Move to the next sensor and filter history objects */
                    ptrSnsTmp++;
                    CapSense_INC_FLTR_OBJ_VARIANT(isProxWdgt, fltrHistV);
                }

                if (0u != (mode & CapSense_PROCESS_STATUS))
                {
                    CapSense_DpProcessCsdWidgetStatus(widgetId, ptrFlashWdgt);
                }
                break;
            }
        #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

        #if (0u != CapSense_TOTAL_CSX_WIDGETS)
            case CapSense_SENSE_METHOD_CSX_E:

                #if (0u != CapSense_CSX_MULTIPHASE_TX_EN)
                    if ((0u != (mode & CapSense_PROCESS_DECONVOLUTION)) &&
                        (0u != (ptrFlashWdgt->staticConfig & CapSense_MULTIPHASE_TX_MASK)))
                    {
                        CapSense_DpDeconvolution(ptrFlashWdgt);
                    }
                #endif /* #if (0u != CapSense_CSX_MULTIPHASE_TX_EN) */

                /* Run the desired processing for the all CSX widget sensors */
                for (;snsCount-- > 0u;)
                {
                    result = CapSense_DpProcessCsxSensorRawCountsExt(ptrFlashWdgt, ptrSnsTmp, fltrHistV, mode);

                    #if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
                        if (CY_RET_SUCCESS != result)
                        {
                            result = (result | CapSense_TST_LSBYTE) & snsCount;
                        }
                    #endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */

                    /* Move to the next sensor and filter history objects */
                    ptrSnsTmp++;
                    CapSense_INC_REG_FLTR_OBJ(fltrHistV);
                }

                if (0u != (mode & CapSense_PROCESS_STATUS))
                {
                    CapSense_DpProcessCsxWidgetStatus(widgetId, ptrFlashWdgt);
                }
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

        default:
            CY_ASSERT(0 != 0);
            break;
        }

        #if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
            if (CY_RET_SUCCESS != result)
            {
                CapSense_UpdateTestResultBaselineDuplication(widgetId, snsCount);
                result = CY_RET_BAD_DATA;
            }
        #endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */

    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_ProcessSensorExt
****************************************************************************//**
*
* \brief
*  Performs customized data processing on the selected widget's sensor.
*
* \details
*  This function performs data processes for the specified sensor specified by
*  the mode parameter. The execution order of the requested operations is from
*  LSB to MSB of the mode parameter. For a different order, this
*  function can be called multiple times with the required mode parameter.
*
*  This function can be used with any of the available scan functions. This
*  function is called only after a specified sensor in the widget is
*  scanned. Calling this function multiple times with the same mode without
*  sensor scanning causes unexpected behavior. This function ignores the value
*  of the wdgtEnable register.
*
*  The CapSense_PROCESS_CALC_NOISE and
*  CapSense_PROCESS_THRESHOLDS flags are supported by the CSD sensing
*  method only when Auto-tuning mode is enabled.
*
*  The pipeline scan method (i.e. during scanning of a sensor, processing
*  of a previously scanned sensor is performed) can be implemented using this
*  function and it may reduce the total scan/process time, increase the refresh
*  rate and decrease the power consumption.
*
*  If the Self-test library is enabled, this function executes the baseline duplication
*  test. Refer to CapSense_CheckBaselineDuplication() for details.
*
* \param widgetId
*  Specifies the ID number of the widget to process one of its sensors.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID.
*
* \param sensorId
*  Specifies the ID number of the sensor within the widget to process it.
*  A macro for the sensor ID within a specified widget can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID.
*
* \param mode
*  Specifies the type of the sensor processing that needs to be executed for the
*  specified sensor:
*    1. Bits [31..5] - Reserved
*    2. Bits [4..0] - CapSense_PROCESS_ALL - Executes all the tasks
*    3. Bit [4] - CapSense_PROCESS_THRESHOLDS - Updates the thresholds (only
*       in auto-tuning mode)
*    4. Bit [3] - CapSense_PROCESS_CALC_NOISE - Calculates the noise (only
*       in auto-tuning mode)
*    5. Bit [2] - CapSense_PROCESS_DIFFCOUNTS - Updates the difference count
*    6. Bit [1] - CapSense_PROCESS_BASELINE - Updates the baseline
*    7. Bit [0] - CapSense_PROCESS_FILTER - Runs the firmware filters
*
* \return
*  Returns the status of the sensor process operation:
*  - CY_RET_SUCCESS - The processing is successfully performed.
*  - CY_RET_BAD_PARAM - The input parameter is invalid.
*  - CY_RET_BAD_DATA - The processing failed.
*
*******************************************************************************/
cy_status CapSense_ProcessSensorExt(uint32 widgetId, uint32 sensorId, uint32 mode)
{
    cy_status result = CY_RET_BAD_PARAM;
    CapSense_FLASH_WD_STRUCT const *ptrFlashWdgt;
    CapSense_PTR_FILTER_VARIANT fltrHistV;
    CapSense_RAM_SNS_STRUCT *ptrSnsTmp;

    if ((widgetId < CapSense_TOTAL_WIDGETS) && (sensorId < CapSense_GET_SENSOR_COUNT(widgetId)))
    {
        result = CY_RET_SUCCESS;

        ptrFlashWdgt = &CapSense_dsFlash.wdgtArray[widgetId];
        fltrHistV.ptr = ptrFlashWdgt->ptr2FltrHistory;
        ptrSnsTmp = ptrFlashWdgt->ptr2SnsRam;
        ptrSnsTmp += sensorId;

        switch(CapSense_GET_SENSE_METHOD(ptrFlashWdgt))
        {
        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            case CapSense_SENSE_METHOD_CSD_E:
                if (CapSense_WD_PROXIMITY_E == (CapSense_WD_TYPE_ENUM)ptrFlashWdgt->wdgtType)
                {
                    #if (0u != CapSense_PROX_RC_ALP_FILTER_EN)
                        fltrHistV.ptr = &fltrHistV.ptrAlp[sensorId];
                    #elif (0u != CapSense_PROX_RC_FILTER_EN)
                        fltrHistV.ptr = &fltrHistV.ptrProx[sensorId];
                    #endif
                }
                else
                {
                    #if (0u != CapSense_REGULAR_RC_ALP_FILTER_EN)
                        fltrHistV.ptr = &fltrHistV.ptrAlp[sensorId];
                    #elif (0u != CapSense_REGULAR_RC_FILTER_EN)
                        fltrHistV.ptr = &fltrHistV.ptrRegular[sensorId];
                    #endif
                }

                result = CapSense_DpProcessCsdSensorRawCountsExt(ptrFlashWdgt, ptrSnsTmp, fltrHistV, mode);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

        #if (0u != CapSense_TOTAL_CSX_WIDGETS)
            case CapSense_SENSE_METHOD_CSX_E:
                #if (0u != CapSense_REGULAR_RC_ALP_FILTER_EN)
                    fltrHistV.ptr = &fltrHistV.ptrAlp[sensorId];
                #elif (0u != CapSense_REGULAR_RC_FILTER_EN)
                    fltrHistV.ptr = &fltrHistV.ptrRegular[sensorId];
                #endif

                result = CapSense_DpProcessCsxSensorRawCountsExt(ptrFlashWdgt, ptrSnsTmp, fltrHistV, mode);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

        default:
            CY_ASSERT(0 != 0);
            break;
        }

        #if (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN)
            if (CY_RET_SUCCESS != result)
            {
                CapSense_UpdateTestResultBaselineDuplication(widgetId, sensorId);
                result = CY_RET_BAD_DATA;
            }
        #endif /* (CapSense_ENABLE == CapSense_TST_BSLN_DUPLICATION_EN) */

    }
    return result;
}

/*******************************************************************************
* Function Name: CapSense_Sleep
****************************************************************************//**
*
* \brief
*  Prepares the Component for deep sleep.
*
* \details
*  Currently this function is empty and exists as a place for future updates,
*  this function will be used to prepare the Component to enter deep sleep.
*
*******************************************************************************/
void CapSense_Sleep(void)
{
}


/*******************************************************************************
* Function Name: CapSense_Wakeup
****************************************************************************//**
*
* \brief
*  Resumes the Component after sleep.
*
* \details
*  Resumes the Component after sleep. This function shall be used to resume
*  the Component after exiting deep sleep.
*
*******************************************************************************/
void CapSense_Wakeup(void)
{
    #if(CapSense_BLOCK_ANALOG_WAKEUP_DELAY_US > 0uL)
        Cy_SysLib_DelayUs(CapSense_BLOCK_ANALOG_WAKEUP_DELAY_US);
    #endif /* (CapSense_BLOCK_ANALOG_WAKEUP_DELAY_US > 0uL) */
}


/*******************************************************************************
* Function Name: CapSense_DeepSleepCallback
****************************************************************************//**
*
* \brief
*  Handles Active to DeepSleep power mode transition for the CapSense
*  Component.
*
* \details
*  This function handles Active to DeepSleep power mode transition
*  for the CapSense Component.
*  Calling this function directly by application layer is not recommended.
*  Instead, Cy_SysPm_DeepSleep() should be used for the Active to DeepSleep
*  power mode transition of the device.
*
*  For proper operation of the CapSense Component during the Active to
*  DeepSleep mode transition, a callback to this API should be registered
*  using the Cy_SysPm_RegisterCallback() function with CY_SYSPM_DEEPSLEEP
*  type. Once the callback is registered, this function is called by the
*  Cy_SysPm_DeepSleep() function to prepare the Component to the device
*  power mode transition.
*
*  When this function is called with CY_SYSPM_CHECK_READY as input, this
*  function returns CY_SYSPM_SUCCESS if no scanning is in progress, otherwise
*  CY_SYSPM_FAIL is returned. If CY_SYSPM_FAIL status is returned, a device
*  cannot change the power mode without completing the current scan as
*  a transition to DeepSleep during the scan can disrupt the Component
*  operation.
*
* \param callbackParams
*  Refer to the description of the cy_stc_syspm_callback_params_t type in the
*  Peripheral Driver Library documentation.
*
* \return
*  Returns the status of the operation requested by the mode parameter:
*  - CY_SYSPM_SUCCESS  - DeepSleep power mode can be entered.
*  - CY_SYSPM_FAIL     - DeepSleep power mode cannot be entered.
*
*******************************************************************************/
cy_en_syspm_status_t CapSense_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    return(CapSense_SwitchPowerModeCallback(callbackParams));
}


/*******************************************************************************
* Function Name: CapSense_EnterLowPowerCallback
****************************************************************************//**
*
* \brief
*  Handles Active to Low Power Active (LPActive) power mode transition for
*  the CapSense Component.
*
* \details
*  This function handles the Active to LPActive power mode transition for
*  the CapSense Component.
*  Calling this function directly by application layer is not recommended.
*  Instead, Cy_SysPm_EnterLpMode() should be used for the Active to LPActive
*  power mode transition of the device.
*
*  For proper operation of the CapSense Component during the Active to
*  LPActive mode transition, a callback to this function should be registered
*  using the Cy_SysPm_RegisterCallback() function with CY_SYSPM_ENTER_LP_MODE
*  type. Once the callback is registered, this function is called by the
*  Cy_SysPm_EnterLpMode() function to prepare the Component to the device
*  power mode transition.
*
*  When this function is called with CY_SYSPM_CHECK_READY as input, this
*  function returns CY_SYSPM_SUCCESS if no scanning is in progress, otherwise
*  CY_SYSPM_FAIL is returned. If CY_SYSPM_FAIL status is returned, a device
*  cannot change the power mode without completing the current scan as
*  a transition to LPActive during the scan can disrupt the Component
*  operation.
*
* \param callbackParams
*  Refer to the description of the cy_stc_syspm_callback_params_t type in the
*  Peripheral Driver Library documentation.
*
* \return
*  Returns the status of the operation requested by the mode parameter:
*  - CY_SYSPM_SUCCESS  - LPActive power mode can be entered.
*  - CY_SYSPM_FAIL     - LPActive power mode cannot be entered.
*
*******************************************************************************/
cy_en_syspm_status_t CapSense_EnterLowPowerCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    return(CapSense_SwitchPowerModeCallback(callbackParams));
}


/*******************************************************************************
* Function Name: CapSense_ExitLowPowerCallback
****************************************************************************//**
*
* \brief
*  Handles Low Power Active (LPActive) to Active power mode transition for
*  the CapSense Component.
*
* \details
*  This function handles LPActive to Active power mode transition for
*  the CapSense Component.
*  Calling this function directly by application layer is not recommended.
*  Instead, Cy_SysPm_ExitLpMode() should be used for the LPActive to Active
*  power mode transition of the device.
*
*  For proper operation of the CapSense Component during the LPActive to
*  Active mode transition, a callback to this function should be registered
*  using Cy_SysPm_RegisterCallback() function with CY_SYSPM_EXIT_LP_MODE
*  type. Once the callback is registered, this function is called by the
*  Cy_SysPm_ExitLpMode() function to prepare the Component to the device
*  power mode transition.
*
*  When this function is called with CY_SYSPM_CHECK_READY as input, this
*  function returns CY_SYSPM_SUCCESS if no scanning is in progress, otherwise
*  CY_SYSPM_FAIL is returned. If CY_SYSPM_FAIL status is returned, a device
*  cannot change the power mode without completing the current scan as
*  a transition to Active during the scan can disrupt the Component
*  operation.

* \param callbackParams
*  Refer to the description of the cy_stc_syspm_callback_params_t type in the
*  Peripheral Driver Library documentation.
*
* \return
*  Returns the status of the operation requested by the mode parameter:
*  - CY_SYSPM_SUCCESS  - Active power mode can be entered.
*  - CY_SYSPM_FAIL     - Active power mode cannot be entered.
*
*******************************************************************************/
cy_en_syspm_status_t CapSense_ExitLowPowerCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    return(CapSense_SwitchPowerModeCallback(callbackParams));
}


/*******************************************************************************
* Function Name: CapSense_SwitchPowerModeCallback
****************************************************************************//**
*
* \brief
*  Handles the switching power mode for the CapSense Component.
*
* \details
*  This function handles swithing of system power mode.
*  When this function is called with the mode parameter set to CY_SYSPM_CHECK_READY,
*  the function returns CY_SYSPM_SUCCESS if no scanning is in progress, otherwise
*  CY_SYSPM_FAIL is returned that means that the device cannot switch the power mode
*  without finishing the current scan that is in progress (switching power mode
*  during the scan can disrupt a sensor scan result and produce an unexpected
*  behavior).
*
* \param callbackParams
*  Refer to the description of the cy_stc_syspm_callback_params_t type.
*
* \return
*  Returns the status of the operation requeted by mode parameter:
*  - CY_SYSPM_SUCCESS  - Switching power mode can be done
*  - CY_SYSPM_FAIL     - Switching power mode cannot be done.
*
*******************************************************************************/
static cy_en_syspm_status_t CapSense_SwitchPowerModeCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    cy_en_syspm_status_t retVal = CY_SYSPM_SUCCESS;

    #if (0u != CapSense_ADC_EN)
        uint8 temp;
    #endif /* (0u != CapSense_ADC_EN) */

    switch(callbackParams->mode)
    {
    case (CY_SYSPM_CHECK_READY):
        if (CapSense_NOT_BUSY != CapSense_IsBusy())
        {
            /* Scanning in progress */
            retVal = CY_SYSPM_FAIL;
        }
        #if (0u != CapSense_ADC_EN)
            temp = CapSense_AdcIsBusy();
            if ((CapSense_AdcSTATUS_IDLE != temp) &&
                (CapSense_AdcSTATUS_OVERFLOW != temp))
            {
                /* Scanning in progress */
                retVal = CY_SYSPM_FAIL;
            }
        #endif /* (0u != CapSense_ADC_EN) */
        break;

    default:
        break;
    }

    return(retVal);
}


/* [] END OF FILE */
