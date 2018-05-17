/***************************************************************************//**
* \file CapSense_Sensing.c
* \version 2.0
*
* \brief
*   This file contains the source of functions common for
*   different sensing methods.
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

#include <stdlib.h>
#include "cyfitter.h"
#include "gpio/cy_gpio.h"
#include "cyfitter_sysint_cfg.h"
#include "CapSense_ModClk.h"
#include "CapSense_Configuration.h"
#include "CapSense_Structure.h"
#include "CapSense_Sensing.h"
#if (CapSense_ENABLE == CapSense_CSX_EN)
    #include "CapSense_SensingCSX_LL.h"
#endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
#if (CapSense_ENABLE == CapSense_CSD_EN)
    #include "CapSense_SensingCSD_LL.h"
#endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    #include "CapSense_SmartSense_LL.h"
#endif  /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */
#if (CapSense_ENABLE == CapSense_ADC_EN)
    #include "CapSense_Adc.h"
#endif /* (CapSense_ENABLE == CapSense_ADC_EN) */
#if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
    #include "CapSense_SelfTest.h"
#endif

/***************************************
* API Constants
***************************************/

#define CapSense_CALIBRATION_RESOLUTION                 (12u)
#define CapSense_CALIBRATION_FREQ_KHZ                   (1500u)
#define CapSense_CSD_AUTOTUNE_CAL_LEVEL                 (CapSense_CSD_RAWCOUNT_CAL_LEVEL)
#define CapSense_CSD_AUTOTUNE_CAL_UNITS                 (1000u)
#define CapSense_CP_MIN                                 (0u)
#define CapSense_CP_MAX                                 (65000Lu)
#define CapSense_CP_ERROR                               (4000Lu)
#define CapSense_CLK_SOURCE_LFSR_SCALE_OFFSET           (4u)
#define CapSense_CSD_SNS_FREQ_KHZ_MAX                   (6000u)

#define CapSense_MOD_CSD_CLK_12000KHZ                   (12000uL)
#define CapSense_MOD_CSD_CLK_24000KHZ                   (24000uL)
#define CapSense_MOD_CSD_CLK_48000KHZ                   (48000uL)

#define CapSense_EXT_CAP_DISCHARGE_TIME                 (1u)

/*****************************************************************************/
/* Enumeration types definition                                              */
/*****************************************************************************/

typedef enum
{
    CapSense_RES_PULLUP_E   = 0x02u,
    CapSense_RES_PULLDOWN_E = 0x03u
} CapSense_PORT_TEST_DM;

typedef enum
{
    CapSense_STS_RESET      = 0x01u,
    CapSense_STS_NO_RESET   = 0x02u
} CapSense_TEST_TYPE;


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
/**
* \cond SECTION_CYSENSE_INTERNAL
* \addtogroup group_cysense_internal
* \{
*/

#if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
     (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
     (CapSense_ENABLE == CapSense_ADC_EN))
    #if (CapSense_ENABLE == CapSense_CSD_EN)
        static void CapSense_SsCSDDisableMode(void);
    #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
    #if (CapSense_ENABLE == CapSense_CSX_EN)
        static void CapSense_SsDisableCSXMode(void);
    #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
#endif

#if(((CapSense_ENABLE == CapSense_CSX_EN) && \
     (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE) && \
     (CapSense_DISABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES)) ||\
    ((CapSense_ENABLE == CapSense_CSD_EN) && \
     (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)))
    static uint8 CapSense_SsCalcLfsrSize(uint32 snsClkDivider, uint32 conversionsNum);
    static uint8 CapSense_SsCalcLfsrScale(uint32 snsClkDivider, uint8 lfsrSize);
#endif

#if (CapSense_ENABLE == CapSense_CSD_EN)
    static void CapSense_SsSetWidgetSenseClkSrc(uint32 wdgtIndex, CapSense_RAM_WD_BASE_STRUCT * ptrWdgt);
#endif

#if (CapSense_ENABLE == CapSense_CSX_EN)
    __STATIC_INLINE void CapSense_SsSetWidgetTxClkSrc(uint32 wdgtIndex, CapSense_RAM_WD_BASE_STRUCT * ptrWdgt);
#endif

/** \}
* \endcond */

/*******************************************************************************
* Defines module variables
*******************************************************************************/

#if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
     (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
     (CapSense_ENABLE == CapSense_ADC_EN))
    CapSense_SENSE_METHOD_ENUM CapSense_currentSenseMethod = CapSense_UNDEFINED_E;
#endif

#if(CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
    /* Module variable keep track of multi-frequency scan channel index */
    uint8 CapSense_scanFreqIndex = 0u;
#else
    /* const allows C-compiler to do optimization */
    const uint8 CapSense_scanFreqIndex = 0u;
#endif

/* Global software variables */
volatile uint8 CapSense_widgetIndex = 0u;    /* Index of the scanning widget */
volatile uint8 CapSense_sensorIndex = 0u;    /* Index of the scanning sensor */
uint8 CapSense_requestScanAllWidget = 0u;

/* Pointer to RAM_SNS_STRUCT structure */
CapSense_RAM_SNS_STRUCT *CapSense_curRamSnsPtr;

#if ((CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) || \
     (CapSense_ENABLE == CapSense_CSX_EN))
    /* Pointer to Flash structure holding configuration of widget to be scanned */
    CapSense_FLASH_WD_STRUCT const *CapSense_curFlashWdgtPtr = 0u;
#endif

#if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
    /* Pointer to Flash structure holding info of sensor to be scanned */
    CapSense_FLASH_SNS_STRUCT const *CapSense_curFlashSnsPtr = 0u;
#endif

/* Pointer to Flash structure to hold Sns electrode that was connected previously */
CapSense_FLASH_IO_STRUCT const *CapSense_curSnsIOPtr;


/*******************************************************************************
* Function Name: CapSense_IsBusy
****************************************************************************//**
*
* \brief
*  Returns the current status of the Component (Scan is completed or Scan is in
*  progress).
*
* \details
*  This function returns a status of the hardware block whether a scan is
*  currently in progress or not. If the Component is busy, no new scan or setup
*  widgets is made. The critical section (i.e. disable global interrupt)
*  is recommended for the application when the device transitions from
*  the active mode to sleep or deep sleep modes.
*
* \return
*  Returns the current status of the Component:
*    - CapSense_NOT_BUSY - No scan is in progress and a next scan
*      can be initiated.
*    - CapSense_SW_STS_BUSY - The previous scanning is not completed
*      and the hardware block is busy.
*
*******************************************************************************/
uint32 CapSense_IsBusy(void)
{
    return (CapSense_dsRam.status & CapSense_SW_STS_BUSY);
}

/*******************************************************************************
* Function Name: CapSense_SetupWidget
****************************************************************************//**
*
* \brief
*  Performs the initialization required to scan the specified widget.
*
* \details
*  This function prepares the Component to scan all the sensors in the specified
*  widget by executing the following tasks:
*    1. Re-initialize the hardware if it is not configured to perform the
*       sensing method used by the specified widget, this happens only if the
*       CSD and CSX methods are used in the Component.
*    2. Initialize the hardware with specific sensing configuration (e.g.
*       sensor clock, scan resolution) used by the widget.
*    3. Disconnect all previously connected electrodes, if the electrodes
*       connected by the CapSense_CSDSetupWidgetExt(),
*       CapSense_CSXSetupWidgetExt() or CapSense_CSDConnectSns()
*       functions and not disconnected.
*
*  This function does not start sensor scanning, the CapSense_Scan()
*  function must be called to start the scan sensors in the widget. If this
*  function is called more than once, it does not break the Component operation,
*  but only the last initialized widget is in effect.
*
* \param widgetId
*  Specifies the ID number of the widget to be initialized for scanning.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID.
*
* \return
*  Returns the status of the widget setting up operation:
*    - CY_RET_SUCCESS - The operation is successfully completed.
*    - CY_RET_BAD_PARAM - The widget is invalid or if the specified widget is
*      disabled
*    - CY_RET_INVALID_STATE - The previous scanning is not completed and the
*      hardware block is busy.
*    - CY_RET_UNKNOWN - An unknown sensing method is used by the widget or any
*      other spurious error occurred.
*
**********************************************************************************/
cy_status CapSense_SetupWidget(uint32 widgetId)
{
    cy_status widgetStatus;

    if (CapSense_WDGT_SW_STS_BUSY == (CapSense_dsRam.status & CapSense_WDGT_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        widgetStatus = CY_RET_INVALID_STATE;
    }
    /*
     *  Check if widget id is valid, specified widget is enabled and widget did not
     *  detect any fault conditions if BIST is enabled. If all conditions are met,
     *  set widgetStatus as good, if not, set widgetStatus as bad.
     */
    else if ((CapSense_TOTAL_WIDGETS > widgetId) &&
        (0uL != CapSense_GET_WIDGET_EN_STATUS(widgetId)))

    {
        widgetStatus = CY_RET_SUCCESS;
    }
    else
    {
        widgetStatus = CY_RET_BAD_PARAM;
    }

    /*
     * Check widgetStatus flag that is set earlier, if flag is good, then set up only
     * widget
     */
    if (CY_RET_SUCCESS == widgetStatus)
    {
        #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
            /* Check widget sensing method is CSX and call CSX APIs */
            if (CapSense_SENSE_METHOD_CSX_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
            {
                /* Set up widget for CSX scan */
                CapSense_CSXSetupWidget(widgetId);
            }
            /* Check widget sensing method is CSD and call appropriate API */
            else if (CapSense_SENSE_METHOD_CSD_E ==
                     CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
            {
                /* Set up widget for CSD scan */
                CapSense_CSDSetupWidget(widgetId);
            }
            else
            {
                /* Sensing method is invalid, return error to caller */
                widgetStatus = CY_RET_UNKNOWN;
            }
        #elif (CapSense_ENABLE == CapSense_CSD_EN)
            /* Set up widget for scan */
            CapSense_CSDSetupWidget(widgetId);
        #elif (CapSense_ENABLE == CapSense_CSX_EN)
            /* Set up widgets for scan */
            CapSense_CSXSetupWidget(widgetId);
        #else
            widgetStatus = CY_RET_UNKNOWN;
            #error "No sensing method enabled, Component cannot work in this mode"
        #endif
    }

    return (widgetStatus);
}


/*******************************************************************************
* Function Name: CapSense_Scan
****************************************************************************//**
*
* \brief
*  Initiates scanning of all the sensors in the widget initialized by
*  CapSense_SetupWidget(), if no scan is in progress.
*
* \details
*  This function is called only after the CapSense_SetupWidget()
*  function is called to start the scanning of the sensors in the widget. The
*  status of a sensor scan must be checked using the CapSense_IsBusy()
*  API prior to starting a next scan or setting up another widget.
*
* \return
*  Returns the status of the scan initiation operation:
*    - CY_RET_SUCCESS - Scanning is successfully started.
*    - CY_RET_INVALID_STATE - The previous scanning is not completed and the
*      hardware block is busy.
*    - CY_RET_UNKNOWN - An unknown sensing method is used by the widget.
*
********************************************************************************/
cy_status CapSense_Scan(void)
{
    cy_status scanStatus = CY_RET_SUCCESS;

    if (CapSense_WDGT_SW_STS_BUSY == (CapSense_dsRam.status & CapSense_WDGT_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        scanStatus = CY_RET_INVALID_STATE;
    }
    else
    {
        /* If both CSD and CSX are enabled, call scan API based on widget sensing method */
        #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
            /* Check widget sensing method and call appropriate APIs */
            switch (CapSense_currentSenseMethod)
            {
                case CapSense_SENSE_METHOD_CSX_E:
                    CapSense_CSXScan();
                    break;

                case CapSense_SENSE_METHOD_CSD_E:
                     CapSense_CSDScan();
                    break;

                default:
                    scanStatus = CY_RET_UNKNOWN;
                    break;
            }

        /* If only CSD is enabled, call CSD scan */
        #elif (CapSense_ENABLE == CapSense_CSD_EN)
            CapSense_CSDScan();

        /* If only CSX is enabled, call CSX scan */
        #elif (CapSense_ENABLE == CapSense_CSX_EN)
            CapSense_CSXScan();

        #else
            scanStatus = CY_RET_UNKNOWN;
            #error "No sensing method enabled, Component cannot work in this mode"
        #endif /* (CapSense_ENABLE == CapSense_CSD_CSX_EN) */
    }

    return (scanStatus);
}


/*******************************************************************************
* Function Name: CapSense_ScanAllWidgets
****************************************************************************//**
*
* \brief
*  Initializes the first enabled widget and scanning of all the sensors in the
*  widget, then the same process is repeated for all the widgets in the Component,
*  i.e. scanning of all the widgets in the Component.
*
* \details
*  This function initializes a widget and scans all the sensors in the widget,
*  and then repeats the same for all the widgets in the Component. The tasks of
*  the CapSense_SetupWidget() and CapSense_Scan() functions are
*  executed by these functions. The status of a sensor scan must be checked
*  using the CapSense_IsBusy() API prior to starting a next scan
*  or setting up another widget.
*
* \return
*  Returns the status of the operation:
*    - CY_RET_SUCCESS - Scanning is successfully started.
*    - CY_RET_BAD_PARAM - All the widgets are disabled.
*    - CY_RET_INVALID_STATE - The previous scanning is not completed and the HW block is busy.
*    - CY_RET_UNKNOWN - There are unknown errors.
*
*******************************************************************************/
cy_status CapSense_ScanAllWidgets(void)
{
    cy_status scanStatus = CY_RET_UNKNOWN;

    uint32 wdgtIndex;

    if (CapSense_SW_STS_BUSY == (CapSense_dsRam.status & CapSense_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        scanStatus = CY_RET_INVALID_STATE;
    }
    else
    {
        /*
         *  Set up widget first widget.
         *  If widget returned error, set up next, continue same until widget does not return error.
         */
        for (wdgtIndex = 0u;
             wdgtIndex < CapSense_TOTAL_WIDGETS;
             wdgtIndex++)
        {

            scanStatus = CapSense_SetupWidget(wdgtIndex);

            if (CY_RET_SUCCESS == scanStatus)
            {
                #if (0u != (CapSense_TOTAL_WIDGETS - 1u))
                    /* If there are more than one widget to be scanned, request callback to scan next widget */
                    if ((CapSense_TOTAL_WIDGETS - 1u) > wdgtIndex)
                    {
                         /* Request callback to scan next widget in ISR */
                        CapSense_requestScanAllWidget = CapSense_ENABLE;
                    }
                    else
                    {
                        /* Request to exit in ISR (Do not scan the next widgets) */
                        CapSense_requestScanAllWidget = CapSense_DISABLE;
                    }
                #else
                    {
                        /* Request to exit in ISR (We have only one widget) */
                        CapSense_requestScanAllWidget = CapSense_DISABLE;
                    }
                #endif  /* (0u != (CapSense_TOTAL_WIDGETS - 1u)) */

                /* Initiate scan and quit loop */
                scanStatus = CapSense_Scan();

                break;
            }
        }
    }

    return (scanStatus);
}


/*******************************************************************************
* Function Name: CapSense_SsInitialize
****************************************************************************//**
*
* \brief
*  Performs hardware and firmware initialization required for proper operation
*  of the CapSense Component. This function is called from
*  the CapSense_Start() API prior to calling any other APIs of the Component.
*
* \details
*  Performs hardware and firmware initialization required for proper operation
*  of the CapSense Component. This function is called from
*  the CapSense_Start() API prior to calling any other APIs of the Component.
*  1. Depending on the configuration, the function initializes the CSD block
*     for the corresponding sensing mode.
*  2. The function updates the dsRam.wdgtWorking variable with 1 when Self-test
*     is enabled.
*
*  Calling the CapSense_Start API is the recommended method to initialize
*  the CapSense Component at power-up. The CapSense_SsInitialize()
*  API should not be used for initialization, resume, or wake-up operations.
*  The dsRam.wdgtWorking variable is updated.
*
* \return status
*  Returns status of operation:
*  - Zero        - Indicates successful initialization.
*  - Non-zero    - One or more errors occurred in the initialization process.
*
*******************************************************************************/
cy_status CapSense_SsInitialize(void)
{
    cy_status initStatus = CY_RET_SUCCESS;

    #if((CapSense_ENABLE == CapSense_CSD_EN) ||\
        (CapSense_ENABLE == CapSense_CSX_EN))
        CapSense_SsInitializeSourceSenseClk();
    #endif /* ((CapSense_ENABLE == CapSense_CSD_EN) ||\
               (CapSense_ENABLE == CapSense_CSX_EN)) */

    /* Set all IO states to default state */
    CapSense_SsSetIOsInDefaultState();

    #if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
         (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
         (CapSense_ENABLE == CapSense_ADC_EN))
        /*
         * CSD hardware block is initialized in the Setup Widget based
         * on widget sensing method. Release previously captured HW resources
         * if it is second call of CapSense_Start() function.
         */
        CapSense_SsSwitchSensingMode(CapSense_UNDEFINED_E);
    #elif (CapSense_ENABLE == CapSense_CSD_EN)
        /* Initialize CSD block for CSD scanning */
        CapSense_SsCSDInitialize();

    #elif (CapSense_ENABLE == CapSense_CSX_EN)
        /* Initialize CSD block for CSX scanning */
        CapSense_CSXInitialize();

    #else
        #error "No sensing method enabled, Component cannot work in this mode"
        initStatus = CY_RET_UNKNOWN;
    #endif /* (CapSense_ENABLE == CapSense_CSD_CSX_EN) */

    return (initStatus);
}


/*******************************************************************************
* Function Name: CapSense_SetPinState
****************************************************************************//**
*
* \brief
*  Sets the state (drive mode and output state) of the port pin used by a sensor.
*  The possible states are GND, Shield, High-Z, Tx or Rx, Sensor. If the sensor
*  specified in the input parameter is a ganged sensor, then the state of all pins
*  associated with the ganged sensor is updated.
*
* \details
*  This function sets a specified state for a specified sensor element. For the
*  CSD widgets, sensor element is a sensor ID, for the CSX widgets, it is either
*  an Rx or Tx electrode ID. If the specified sensor is a ganged sensor, then
*  the specified state is set for all the electrodes belong to the sensor.
*  This function must not be called while the Component is in the busy state.
*
*  This function accepts the CapSense_SHIELD and
*  CapSense_SENSOR states as an input only if there is at least
*  one CSD widget. Similarly, this function accepts the CapSense_TX_PIN
*  and CapSense_RX_PIN states as an input only if there is at least
*  one CSX widget in the project.

*  Calling this function directly from the application layer is not
*  recommended. This function is used to implement only the custom-specific
*  use cases. Functions that perform a setup and scan of a sensor/widget
*  automatically set the required pin states. They ignore changes
*  in the design made by the CapSense_SetPinState() function.
*  This function neither check wdgtIndex nor sensorElement for the correctness.
*
* \param widgetId
*  Specifies the ID of the widget to change the pin state of the specified
*  sensor.
*  A macro for the widget ID can be found in the CapSense Configuration
*  header file defined as CapSense_<WidgetName>_WDGT_ID.
*
* \param sensorElement
*  Specifies the ID of the sensor element within the widget to change
*  its pin state.
*  For the CSD widgets, sensorElement is the sensor ID and can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID.
*  For the CSX widgets, sensorElement is defined either as Rx ID or Tx ID.
*  The first Rx in a widget corresponds to sensorElement = 0, the second
*  Rx in a widget corresponds to sensorElement = 1, and so on.
*  The last Tx in a widget corresponds to sensorElement = (RxNum + TxNum).
*  Macros for Rx and Tx IDs can be found in the
*  CapSense Configuration header file defined as:
*  - CapSense_<WidgetName>_RX<RXNumber>_ID
*  - CapSense_<WidgetName>_TX<TXNumber>_ID.
*
* \param state
*  Specifies the state of the sensor to be set:
*     1. CapSense_GROUND - The pin is connected to the ground.
*     2. CapSense_HIGHZ - The drive mode of the pin is set to High-Z
*        Analog.
*     3. CapSense_SHIELD - The shield signal is routed to the pin
*        (available only if CSD sensing method with shield electrode is enabled).
*     4. CapSense_SENSOR - The pin is connected to the scanning bus
*        (available only if CSD sensing method is enabled).
*     5. CapSense_TX_PIN - The Tx signal is routed to the sensor
*        (available only if CSX sensing method is enabled).
*     6. CapSense_RX_PIN - The pin is connected to the scanning bus
*        (available only if CSX sensing method is enabled).
*
*******************************************************************************/
void CapSense_SetPinState(uint32 widgetId, uint32 sensorElement, uint32 state)
{
    uint32 eltdNum;
    uint32 eltdIndex;
    uint32 interruptState;
    CapSense_FLASH_IO_STRUCT const *ioPtr;
    #if (CapSense_ENABLE == CapSense_GANGED_SNS_EN)
        CapSense_FLASH_SNS_STRUCT const *curFlashSnsPtr;
    #endif

    /* Getting sensor element pointer and number of electrodes */
    #if (CapSense_ENABLE == CapSense_GANGED_SNS_EN)
        /* Check the ganged sns flag */
        if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[widgetId].staticConfig & CapSense_GANGED_SNS_MASK))
        {
            curFlashSnsPtr = CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash;
            curFlashSnsPtr += sensorElement;
            ioPtr = &CapSense_ioList[curFlashSnsPtr->firstPinId];
            eltdNum = curFlashSnsPtr->numPins;
        }
        else
        {
            ioPtr = (CapSense_FLASH_IO_STRUCT const *)CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash + sensorElement;
            eltdNum = 1u;
        }
    #else
        ioPtr = (CapSense_FLASH_IO_STRUCT const *)CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash + sensorElement;
        eltdNum = 1u;
    #endif

    /* Loop through all electrodes of the specified sensor element */
    for (eltdIndex = 0u; eltdIndex < eltdNum; eltdIndex++)
    {
        /* Reset HSIOM and PC registers */
        interruptState = Cy_SysLib_EnterCriticalSection();
        Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, (en_hsiom_sel_t)CapSense_HSIOM_SEL_GPIO);
        Cy_SysLib_ExitCriticalSection(interruptState);

        switch (state)
        {
        case CapSense_GROUND:
            interruptState = Cy_SysLib_EnterCriticalSection();
            Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF);
            Cy_GPIO_Clr((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber);
            Cy_SysLib_ExitCriticalSection(interruptState);
            break;

        case CapSense_HIGHZ:
            interruptState = Cy_SysLib_EnterCriticalSection();
            Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_ANALOG);
            Cy_GPIO_Clr((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber);
            Cy_SysLib_ExitCriticalSection(interruptState);
            break;

        #if (CapSense_ENABLE == CapSense_CSD_EN)
            case CapSense_SENSOR:
                /* Enable sensor */
                CapSense_CSDConnectSns(ioPtr);
                break;

            #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                case CapSense_SHIELD:
                    interruptState = Cy_SysLib_EnterCriticalSection();
                    Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF);
                    Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, (en_hsiom_sel_t)CapSense_HSIOM_SEL_CSD_SHIELD);
                    Cy_SysLib_ExitCriticalSection(interruptState);
                    break;
            #endif /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
        #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */

        #if (CapSense_ENABLE == CapSense_CSX_EN)
            case CapSense_TX_PIN:
                interruptState = Cy_SysLib_EnterCriticalSection();
                Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, (en_hsiom_sel_t)CapSense_HSIOM_SEL_CSD_SHIELD);
                Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF);
                Cy_SysLib_ExitCriticalSection(interruptState);
                break;

            case CapSense_RX_PIN:
                interruptState = Cy_SysLib_EnterCriticalSection();
                Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, (en_hsiom_sel_t)CapSense_HSIOM_SEL_AMUXA);
                Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_ANALOG);
                Cy_SysLib_ExitCriticalSection(interruptState);
                break;
        #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */

        default:
            /* Wrong state */
            break;
        }

        ioPtr++;
    }
}


#if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
     (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
     (CapSense_ENABLE == CapSense_ADC_EN))

    #if (CapSense_ENABLE == CapSense_CSD_EN)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDDisableMode
        ****************************************************************************//**
        *
        * \brief
        *  This function disables CSD mode.
        *
        * \details
        *  To disable CSD mode the following tasks are performed:
        *  1. Disconnect Cmod from AMUXBUS-A.
        *  2. Disconnect previous CSX electrode if it has been connected.
        *  3. Disconnect Csh from AMUXBUS-B.
        *  4. Disable Shield Electrodes.
        *
        *******************************************************************************/
        static void CapSense_SsCSDDisableMode(void)
        {
            uint32 newRegValue = 0uL;

            /* To remove unreferenced local variable warning */
            (void)newRegValue;
            Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)CapSense_CSD_CMOD_PORT_PTR, CapSense_CSD_CMOD_PIN,
                                                                             (en_hsiom_sel_t)CapSense_HSIOM_SEL_GPIO);

            #if ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                 (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN))
                /* Disconnect IDACA and IDACB */
                newRegValue = CY_GET_REG32(CapSense_CSD_SW_REFGEN_SEL_PTR);
                newRegValue &= (uint32)(~CapSense_CSD_SW_REFGEN_SEL_SW_IAIB_MSK);
                CY_SET_REG32(CapSense_CSD_SW_REFGEN_SEL_PTR, newRegValue);
            #endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                       (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)) */

            /* Disconnect previous CSD electrode if it has been connected */
            CapSense_SsCSDElectrodeCheck();

            /* Disconnect Csh from AMUXBUS-B */
            #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                 (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))

                Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)CapSense_CSD_CTANK_PORT_PTR, CapSense_CSD_CTANK_PIN,
                                                           (en_hsiom_sel_t)CapSense_HSIOM_SEL_GPIO);

            #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                       (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

            #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                 (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))
                CapSense_SsCSDDisableShieldElectrodes();
            #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                       (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */
        }
    #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */


    #if (CapSense_ENABLE == CapSense_CSX_EN)
        /*******************************************************************************
        * Function Name: CapSense_SsDisableCSXMode
        ****************************************************************************//**
        *
        * \brief
        *  This function disables CSX mode.
        *
        * \details
        *  To disable CSX mode the following tasks are performed:
        *  1. Disconnect CintA and CintB from AMUXBUS-A.
        *  2. Disconnect previous CSX electrode if it has been connected.
        *
        *******************************************************************************/
        static void CapSense_SsDisableCSXMode(void)
        {
            /* Disconnect previous CSX electrode if it has been connected */
            CapSense_CSXElectrodeCheck();
        }
    #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */


    /*******************************************************************************
    * Function Name: CapSense_SsSwitchSensingMode
    ****************************************************************************//**
    *
    * \brief
    *  This function changes the mode for case when both CSD and CSX widgets are
    *  scanned.
    *
    * \details
    *  To switch to the CSD mode the following tasks are performed:
    *  1. Disconnect CintA and CintB from AMUXBUS-A.
    *  2. Disconnect previous CSD electrode if it has been connected.
    *  3. Initialize CSD mode.
    *
    *  To switch to the CSX mode the following tasks are performed:
    *  1. Disconnect Cmod from AMUXBUS-A.
    *  2. Disconnect previous CSX electrode if it has been connected.
    *  3. Initialize CSX mode.
    *
    * \param mode
    *  Specifies the scan mode:
    *  - CapSense_SENSE_METHOD_CSD_E
    *  - CapSense_SENSE_METHOD_CSX_E
    *  - CapSense_SENSE_METHOD_BIST_E
    *  - CapSense_UNDEFINED_E
    *
    *******************************************************************************/
    void CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_ENUM mode)
    {
        if (CapSense_currentSenseMethod != mode)
        {
            /* The requested mode differes to the current one. Disable the current mode */
            if (CapSense_SENSE_METHOD_CSD_E == CapSense_currentSenseMethod)
            {
                #if (CapSense_ENABLE == CapSense_CSD_EN)
                    CapSense_SsCSDDisableMode();
                #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
            }
            else if (CapSense_SENSE_METHOD_CSX_E == CapSense_currentSenseMethod)
            {
                #if (CapSense_ENABLE == CapSense_CSX_EN)
                    CapSense_SsDisableCSXMode();
                #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
            }
            else if (CapSense_SENSE_METHOD_BIST_E == CapSense_currentSenseMethod)
            {
                #if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
                    CapSense_BistDisableMode();
                #endif /* (CapSense_ENABLE == CapSense_SELF_TEST_EN) */
            }
            else
            {
                #if (CapSense_ENABLE == CapSense_ADC_EN)
                    /* Release ADC resources */
                    (void)CapSense_AdcReleaseResources();
                #endif /* (CapSense_ENABLE == CapSense_ADC_EN) */
            }

            /* Enable the specified mode */
            if (CapSense_SENSE_METHOD_CSD_E == mode)
            {
                #if (CapSense_ENABLE == CapSense_CSD_EN)
                    /* Initialize CSD mode to guarantee configured CSD mode */
                    CapSense_SsCSDInitialize();
                    CapSense_currentSenseMethod = CapSense_SENSE_METHOD_CSD_E;
                #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
            }
            else if (CapSense_SENSE_METHOD_CSX_E == mode)
            {
                #if (CapSense_ENABLE == CapSense_CSX_EN)
                    /* Initialize CSX mode to guarantee configured CSX mode */
                    CapSense_CSXInitialize();
                    CapSense_currentSenseMethod = CapSense_SENSE_METHOD_CSX_E;
                #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
            }
            else if (CapSense_SENSE_METHOD_BIST_E == mode)
            {
                #if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
                    CapSense_BistInitialize();
                    CapSense_currentSenseMethod = CapSense_SENSE_METHOD_BIST_E;
                #endif /* (CapSense_ENABLE == CapSense_SELF_TEST_EN) */
            }
            else
            {
                CapSense_currentSenseMethod = CapSense_UNDEFINED_E;
            }
        }
    }
#endif  /* ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
            (CapSense_ENABLE == CapSense_ADC_EN)) */


/*******************************************************************************
* Function Name: CapSense_SsSetIOsInDefaultState
****************************************************************************//**
*
* \brief
*  Sets all electrodes into a default state.
*
* \details
*  Sets all the CSD/CSX IOs into a default state:
*  - HSIOM   - Disconnected, the GPIO mode.
*  - DM      - Strong drive.
*  - State   - Zero.
*
*  Sets all the ADC channels into a default state:
*  - HSIOM   - Disconnected, the GPIO mode.
*  - DM      - HiZ-Analog.
*  - State   - Zero.
*
*  It is not recommended to call this function directly from the application
*  layer.
*
*******************************************************************************/
void CapSense_SsSetIOsInDefaultState(void)
{
    CapSense_FLASH_IO_STRUCT const *ioPtr = &CapSense_ioList[0u];
    uint32 loopIndex;

    /* Loop through all electrodes */
    for (loopIndex = 0u; loopIndex < CapSense_TOTAL_ELECTRODES; loopIndex++)
    {
        /*
        *   1. Disconnect HSIOM
        *   2. Set strong DM
        *   3. Set pin state to logic 0
        */
        Cy_GPIO_Pin_FastInit((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_STRONG, 0u,
                                                        (en_hsiom_sel_t)CapSense_HSIOM_SEL_GPIO);

        /* Get next electrode */
        ioPtr++;
    }

    #if(CapSense_ENABLE == CapSense_ADC_EN)
        CapSense_ClearAdcChannels();
    #endif /* (CapSense_ENABLE == CapSense_ADC_EN) */
}

#if (CapSense_ENABLE == CapSense_ADC_EN)
/*******************************************************************************
* Function Name: CapSense_SsReleaseResources()
****************************************************************************//**
*
* \brief
*  This function sets the resources that do not belong to the sensing HW block to
*  default state.
*
* \details
*  The function performs following tasks:
*  1. Checks if CSD block busy and returns error if it is busy
*  2. Disconnects integration capacitors (CintA, CintB for CSX mode and
*     Cmod for CSD mode)
*  3. Disconnect electroded if they have been connected.
*
* \return
*  Returns the status of the operation:
*  - Zero        - Resources released successfully.
*  - Non-zero    - One or more errors occurred in releasing process.
*
*******************************************************************************/
cy_status CapSense_SsReleaseResources(void)
{
    cy_status busyStatus = CY_RET_SUCCESS;

    if (CapSense_SW_STS_BUSY == (CapSense_dsRam.status & CapSense_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        busyStatus = CY_RET_INVALID_STATE;
    }
    else
    {
        #if (CapSense_ENABLE == CapSense_CSX_EN)
            CapSense_SsDisableCSXMode();
        #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */

        #if (CapSense_ENABLE == CapSense_CSD_EN)
            CapSense_SsCSDDisableMode();
        #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */

        #if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
            CapSense_BistDisableMode();
        #endif /* (CapSense_ENABLE == CapSense_SELF_TEST_EN) */

        #if ((CapSense_ENABLE == CapSense_CSD_EN) && \
             (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) &&  \
             (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION))
            CapSense_SsSetIOsInDefaultState();
        #endif /* ((CapSense_ENABLE == CapSense_CSD_EN) && \
             (CapSense_DISABLE != CapSense_CSD_SHIELD_EN) &&  \
             (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)) */

        /*
         * Reset of the currentSenseMethod variable to make sure that the next
         * call of SetupWidget() API setups the correct widget mode
         */
        CapSense_currentSenseMethod = CapSense_UNDEFINED_E;
    }

    return busyStatus;
}
#endif /* (CapSense_ENABLE == CapSense_ADC_EN) */


/*******************************************************************************
* Function Name: CapSense_SsPostAllWidgetsScan
****************************************************************************//**
*
* \brief
*  The ISR function for multiple widget scanning implementation.
*
* \details
*  This is the function used by the CapSense ISR to implement multiple widget
*  scanning.
*  Should not be used by the application layer.
*
*******************************************************************************/
void CapSense_SsPostAllWidgetsScan(void)
{
    /*
    *   1. Increment widget index
    *   2. Check if all the widgets are scanned
    *   3. If all the widgets are not scanned, set up and scan next widget
    */
    #if (1u != CapSense_TOTAL_WIDGETS)
        cy_status postScanStatus;

        do
        {
            CapSense_widgetIndex++;

            postScanStatus = CapSense_SetupWidget((uint32)CapSense_widgetIndex);

            if (CY_RET_SUCCESS == postScanStatus)
            {
                if((CapSense_TOTAL_WIDGETS - 1u) == CapSense_widgetIndex)
                {
                    /* The last widget will be scanned. Reset flag to skip configuring the next widget setup in ISR. */
                    CapSense_requestScanAllWidget = CapSense_DISABLE;
                }
                (void)CapSense_Scan();
            }
            else if((CapSense_TOTAL_WIDGETS - 1u) == CapSense_widgetIndex)
            {
                #if ((CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) && \
                     (CapSense_ENABLE == CapSense_CSD_EN))
                    if (CapSense_SENSE_METHOD_CSD_E ==
                        CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[CapSense_widgetIndex]))
                    {
                        /* Disable the CSD block */
                        CY_SET_REG32(CapSense_CSD_CONFIG_PTR, CapSense_configCsd);
                    }
                #endif /* ((CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) && \
                           (CapSense_ENABLE == CapSense_CSD_EN)) */

                /* Update scan Counter */
                CapSense_dsRam.scanCounter++;
                /* all the widgets are totally processed. Reset BUSY flag */
                CapSense_dsRam.status &= ~CapSense_SW_STS_BUSY;

                /* Update status with with the failure */
                CapSense_dsRam.status &= ~CapSense_STATUS_ERR_MASK;
                CapSense_dsRam.status |= ((postScanStatus & CapSense_STATUS_ERR_SIZE) << CapSense_STATUS_ERR_SHIFT);

                /* Set postScanStatus to exit the while loop */
                postScanStatus = CY_RET_SUCCESS;
            }
            else
            {
                /* Update status with with the failure. Configure the next widget in while() loop */
                CapSense_dsRam.status &= ~CapSense_STATUS_ERR_MASK;
                CapSense_dsRam.status |= ((postScanStatus & CapSense_STATUS_ERR_SIZE) << CapSense_STATUS_ERR_SHIFT);
            }
        } while (CY_RET_SUCCESS != postScanStatus);
    #endif /* (1u != CapSense_TOTAL_WIDGETS) */
}


/*******************************************************************************
* Function Name: CapSense_SsIsrInitialize
****************************************************************************//**
*
* \brief
*  Enables and initializes for the function pointer for a callback for the ISR.
*
* \details
*  The "address" is a special type cy_israddress defined by syslib. This function
*  is used by Component APIs and should not be used by an application program for
*  proper working of the Component.
*
* \param address
*  The address of the function to be called when interrupt is fired.
*
*******************************************************************************/
void CapSense_SsIsrInitialize(cy_israddress address)
{
    /* Disable interrupt */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        NVIC_DisableIRQ(CapSense_ISR_cfg.intrSrc);
    #endif

    /* Configure interrupt with priority and vector */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        (void)Cy_SysInt_Init(&CapSense_ISR_cfg, address);
    #endif

    /* Enable interrupt */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        NVIC_EnableIRQ(CapSense_ISR_cfg.intrSrc);
    #endif
}


/*******************************************************************************
* Function Name: CapSense_SsSetSnsFirstPhaseWidth
****************************************************************************//**
*
* \brief
*  Defines the length of the first phase of the sense clock in clk_csd cycles.
*
* \details
*  It is not recommended to call this function directly by the application layer.
*  It is used by initialization, widget APIs or wakeup functions to
*  enable the clocks.
*  At all times it must be assured that the phases are at least 2 clk_csd cycles
*  (1 for non overlap, if used), if this rule is violated the result is undefined.
*
* \param
*  snsClk The divider value for the sense clock.
*
*******************************************************************************/
void CapSense_SsSetSnsFirstPhaseWidth(uint32 phaseWidth)
{
    uint32 newRegValue;

    newRegValue = CY_GET_REG32(CapSense_CSD_SENSE_DUTY_PTR);
    newRegValue &= (uint32)(~CapSense_CSD_SENSE_DUTY_SENSE_WIDTH_MSK);
    newRegValue |= phaseWidth;
    CY_SET_REG32(CapSense_CSD_SENSE_DUTY_PTR, newRegValue);
}


/*******************************************************************************
* Function Name: CapSense_SsSetSnsClockDivider
****************************************************************************//**
*
* \brief
*  Sets the divider values for the sense clock and then starts
*  the sense clock.
*
* \details
*  It is not recommended to call this function directly by the application layer.
*  It is used by initialization, widget APIs or wakeup functions to
*  enable the clocks.
*
* \param
*  snsClk The divider value for the sense clock.
*
*******************************************************************************/
void CapSense_SsSetSnsClockDivider(uint32 snsClk)
{
    uint32 newRegValue;

    /*
     * Set divider value for sense clock.
     * 1u is subtracted from snsClk because SENSE_DIV value 0 corresponds
     * to dividing by 1.
     */
    newRegValue = CY_GET_REG32(CapSense_CSD_SENSE_PERIOD_PTR);
    newRegValue &= (uint32)(~CapSense_CSD_SENSE_PERIOD_SENSE_DIV_MSK);
    newRegValue |= snsClk - 1u;
    CY_SET_REG32(CapSense_CSD_SENSE_PERIOD_PTR, newRegValue);
}


/*******************************************************************************
* Function Name: CapSense_SsSetClockDividers
****************************************************************************//**
*
* \brief
*  Sets the divider values for sense and modulator clocks and then starts
*  a modulator clock-phase aligned to HFCLK and sense clock-phase aligned to
*  the modulator clock.
*
* \details
*  It is not recommended to call this function directly by the application layer.
*  It is used by initialization, widget APIs or wakeup functions to
*  enable the clocks.
*
* \param
*  snsClk The divider value for the sense clock.
*  modClk The divider value for the modulator clock.
*
*******************************************************************************/
void CapSense_SsSetClockDividers(uint32 snsClk, uint32 modClk)
{
    /* Configure Mod clock */
    CapSense_ModClk_SetDivider((uint32)modClk - 1uL);

    /* Configure Sns clock */
    CapSense_SsSetSnsClockDivider(snsClk);
}


#if ((CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN) || \
     (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN))
    /*******************************************************************************
    * Function Name: CapSense_CalibrateWidget
    ****************************************************************************//**
    *
    * \brief
    *  Calibrates the IDACs for all the sensors in the specified widget to the default
    *  target, this function detects the sensing method used by the
    *  widget prior to calibration.
    *
    * \details
    *  This function performs exactly the same tasks as
    *  CapSense_CalibrateAllWidgets, but only for a specified widget.
    *  This function detects the sensing method used by the widgets and uses
    *  the Enable compensation IDAC parameter.
    *
    *  This function is available when the CSD and/or CSX Enable IDAC
    *  auto-calibration parameter is enabled.

    *
    * \param widgetId
    *  Specifies the ID number of the widget to calibrate its raw count.
    *  A macro for the widget ID can be found in the
    *  CapSense Configuration header file defined as
    *  CapSense_<WidgetName>_WDGT_ID.
    *
    * \return
    *  Returns the status of the specified widget calibration:
    *  - CY_RET_SUCCESS - The operation is successfully completed.
    *  - CY_RET_BAD_PARAM - The input parameter is invalid.
    *  - CY_RET_BAD_DATA - The calibration failed and the Component may not
    *    operate as expected.
    *
    *******************************************************************************/
    cy_status CapSense_CalibrateWidget(uint32 widgetId)
    {
        cy_status calibrateStatus = CY_RET_SUCCESS;

        do
        {
            if (CapSense_TOTAL_WIDGETS <= widgetId)
            {
                calibrateStatus = CY_RET_BAD_PARAM;
            }

            /*
             *  Check if widget id is valid, specified widget did not
             *  detect any faults conditions if BIST is enabled.
             */
            #if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
                if (0u != CapSense_GET_WIDGET_EN_STATUS(widgetId))
                {
                    calibrateStatus = CY_RET_SUCCESS;
                }
                else
                {
                    calibrateStatus = CY_RET_INVALID_STATE;
                }
            #endif  /* (CapSense_ENABLE == CapSense_SELF_TEST_EN) */

            if (CY_RET_SUCCESS != calibrateStatus)
            {
                /* Exit from the loop because of a fail */
                break;
            }

            /* If both CSD and CSX are enabled, calibrate widget using sensing method */
            #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
                /* Check widget sensing method and call appropriate APIs */
                #if (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN)
                    if (CapSense_SENSE_METHOD_CSX_E ==
                        CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
                    {
                        /* Calibrate CSX widget */
                       CapSense_CSXCalibrateWidget(widgetId, CapSense_CSX_RAWCOUNT_CAL_LEVEL);
                    }
                #endif  /* (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN) */

                #if (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN)
                    if (CapSense_SENSE_METHOD_CSD_E ==
                        CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
                    {
                        /* Calibrate CSD widget */
                        calibrateStatus = CapSense_CSDCalibrateWidget(widgetId, CapSense_CSD_RAWCOUNT_CAL_LEVEL);
                    }
                #endif  /* (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN) */

            /* If only CSD is enabled, calibrate CSD sensor */
            #elif (CapSense_ENABLE == CapSense_CSD_EN)
                calibrateStatus = CapSense_CSDCalibrateWidget(widgetId, CapSense_CSD_RAWCOUNT_CAL_LEVEL);

            /* If only CSX is enabled, call CSX scan */
            #elif (CapSense_ENABLE == CapSense_CSX_EN)
                CapSense_CSXCalibrateWidget(widgetId, CapSense_CSX_RAWCOUNT_CAL_LEVEL);

            #else
                calibrateStatus = CY_RET_UNKNOWN;
            #endif /* (CapSense_ENABLE == CapSense_CSD_CSX_EN) */

             /* Update CRC */
            #if (CapSense_ENABLE ==CapSense_TST_WDGT_CRC_EN)
                CapSense_DsUpdateWidgetCrc(widgetId);
            #endif /* (CapSense_ENABLE ==CapSense_TST_WDGT_CRC_EN) */

        } while (0u);

        return calibrateStatus;
    }


    /*******************************************************************************
    * Function Name: CapSense_CalibrateAllWidgets
    ****************************************************************************//**
    *
    * \brief
    *  Calibrates the IDACs for all the widgets in the Component to the default
    *  target, this function detects the sensing method used by the widgets
    *  prior to calibration.
    *
    * \details
    *  Calibrates the IDACs for all the widgets in the Component to the default
    *  target value. This function detects the sensing method used by the widgets
    *  and regards the Enable compensation IDAC parameter.
    *
    *  This function is available when the CSD and/or CSX Enable IDAC
    *  auto-calibration parameter is enabled.
    *
    * \return
    *  Returns the status of the calibration process:
    *  - CY_RET_SUCCESS - The operation is successfully completed.
    *  - CY_RET_BAD_DATA - The calibration failed and the Component may not
    *    operate as expected.
    *
    *******************************************************************************/
    cy_status CapSense_CalibrateAllWidgets(void)
    {
        cy_status calibrateStatus = CY_RET_SUCCESS;
        uint32 wdgtIndex;

        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            calibrateStatus |= CapSense_CalibrateWidget(wdgtIndex);
        }

        return calibrateStatus;
    }
#endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN) ||
           (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN)) */


#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    /*******************************************************************************
    * Function Name: CapSense_SsAutoTune
    ****************************************************************************//**
    *
    * \brief
    *  This API performs the parameters auto-tuning for the optimal CapSense operation.
    *
    * \details
    *  This API performs the following:
    *  - Calibrates Modulator and Compensation IDACs.
    *  - Tunes the Sense Clock optimal value to get a Sense Clock period greater than
    *    2*5*R*Cp.
    *  - Calculates the resolution for the optimal finger capacitance.
    *
    * \return
    *  Returns the status of the operation:
    *  - Zero     - All the widgets are auto-tuned successfully.
    *  - Non-zero - Auto-tuning failed for any widget.
    *
    *******************************************************************************/
    cy_status CapSense_SsAutoTune(void)
    {
        cy_status autoTuneStatus = CY_RET_SUCCESS;
        uint32 wdgtIndex;

        uint32 cp = 0uL;
        #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
            uint32 cpRow = 0uL;
        #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */

        uint32 cpWidget[CapSense_TOTAL_WIDGETS];
        CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;
        AUTO_TUNE_CONFIG_TYPE autoTuneConfig;

        /* Configure common config variables */
        autoTuneConfig.snsClkConstantR = CapSense_CSD_SNSCLK_R_CONST;
        autoTuneConfig.vRef = CapSense_CSD_VREF_MV;
        autoTuneConfig.iDacGain = CapSense_CSD_IDAC_GAIN_VALUE_NA;

        /* Calculate snsClk Input Clock in KHz */
        /* Dividers are chained */
        autoTuneConfig.snsClkInputClock = (CYDEV_CLK_PERICLK__KHZ / CapSense_dsRam.modCsdClk);

        /* If both CSD and CSX are enabled, calibrate widget using sensing method */
        #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
            /* Initialize CSD mode */
            CapSense_SsCSDInitialize();
        #endif /* (CapSense_ENABLE == CapSense_CSD_CSX_EN) */

        /*
        * Autotune phase #1:
        * - performing the first calibration at fixed settings
        * - getting sensor Cp
        * - getting sense clock frequency based on Cp
        */

        /* Tune sense clock for all the widgets */
        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapSense_SENSE_METHOD_CSD_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
            {
                ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                          CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

                /* Set calibration resolution to 12 bits */
                ptrWdgt->resolution = CapSense_CALIBRATION_RESOLUTION;

                /* Set clock source direct and sense clock frequency to 1.5 MHz */
                ptrWdgt->snsClkSource = (uint8)CapSense_CLK_SOURCE_DIRECT;
                ptrWdgt->snsClk = (uint16)((uint32)autoTuneConfig.snsClkInputClock / CapSense_CALIBRATION_FREQ_KHZ);
                #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                        (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType))
                    {
                        ptrWdgt->rowSnsClkSource = (uint8)CapSense_CLK_SOURCE_DIRECT;
                        ptrWdgt->rowSnsClk = (uint16)((uint32)autoTuneConfig.snsClkInputClock / CapSense_CALIBRATION_FREQ_KHZ);
                    }
                #endif

                /* Calibrate CSD widget to the default calibration target */
                (void)CapSense_CSDCalibrateWidget(wdgtIndex, CapSense_CSD_AUTOTUNE_CAL_LEVEL);

                #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                        (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType))
                    {
                        /* Get pointer to Sense Clock Divider for columns */
                        autoTuneConfig.ptrSenseClk = &ptrWdgt->rowSnsClk;

                        /* Get IDAC */
                        autoTuneConfig.iDac = CapSense_calibratedIdacRow;

                        /* Calculate achived calibration level */
                        autoTuneConfig.calTarget = (uint16)(((uint32)CapSense_calibratedRawcountRow * CapSense_CSD_AUTOTUNE_CAL_UNITS) /
                                ((uint32)(0x01uL << CapSense_CALIBRATION_RESOLUTION) - 1u));

                        /* Find correct sense clock value */
                        cpRow = SmartSense_TunePrescalers(&autoTuneConfig);

                        if ((CapSense_CP_MAX + CapSense_CP_ERROR) <= cpRow)
                        {
                            autoTuneStatus = CY_RET_BAD_DATA;
                        }

                        /*
                        * Multiply the sense Clock Divider by 2 while the desired Sense Clock Frequency is greater
                        * than maximum supported Sense Clock Frequency.
                        */
                        while((((uint32)autoTuneConfig.snsClkInputClock) > ((uint32)ptrWdgt->snsClk * CapSense_CSD_SNS_FREQ_KHZ_MAX)) ||
                                (CapSense_MIN_SNS_CLK_DIVIDER > ptrWdgt->snsClk))
                        {
                            ptrWdgt->snsClk <<= 1u;
                        }
                    }
                #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */

                /* Get pointer to Sense Clock Divider for columns */
                autoTuneConfig.ptrSenseClk = &ptrWdgt->snsClk;

                /* Get IDAC */
                autoTuneConfig.iDac = CapSense_calibratedIdac;

                /* Calculate achived calibration level */
                autoTuneConfig.calTarget = (uint16)(((uint32)CapSense_calibratedRawcount * CapSense_CSD_AUTOTUNE_CAL_UNITS) /
                        ((uint32)(0x01uL << CapSense_CALIBRATION_RESOLUTION) - 1u));

                /* Find correct sense clock value */
                cp = SmartSense_TunePrescalers(&autoTuneConfig);

                if ((CapSense_CP_MAX + CapSense_CP_ERROR) <= cp)
                {
                    autoTuneStatus = CY_RET_BAD_DATA;
                }

                /*
                * Multiply the sense Clock Divider by 2 while the desired Sense Clock Frequency is greater
                * than MAX supported Sense Clock Frequency.
                */
                while((((uint32)autoTuneConfig.snsClkInputClock) > ((uint32)ptrWdgt->snsClk * CapSense_CSD_SNS_FREQ_KHZ_MAX)) ||
                        (CapSense_MIN_SNS_CLK_DIVIDER > ptrWdgt->snsClk))
                {
                    ptrWdgt->snsClk <<= 1u;
                }

                cpWidget[wdgtIndex] = cp;

                #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                        (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType))
                    {
                        if (cpRow > cp)
                        {
                            cpWidget[wdgtIndex] = cpRow;
                        }
                    }
                #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */
            }
            else
            {
                #if (CapSense_ENABLE == CapSense_CSX_EN)
                    /* Non-CSD widget */
                    cpWidget[wdgtIndex] = 1u;
                #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
            }
        }

        /*
        * Autotune phase #2:
        * - repeating calibration with new sense clock frequency
        * - getting resolution
        */

        /* Tune resolution for all the widgets */
        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapSense_SENSE_METHOD_CSD_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
            {
                ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                          CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

                /* Calibrate CSD widget to the default calibration target */
                autoTuneStatus |= CapSense_CSDCalibrateWidget(wdgtIndex, CapSense_CSD_AUTOTUNE_CAL_LEVEL);

                /* Get pointer to Sense Clock Divider (column or row) */
                autoTuneConfig.ptrSenseClk = &ptrWdgt->snsClk;

                /* Set parasitic capacitance for columns */
                autoTuneConfig.sensorCap = cpWidget[wdgtIndex];

                /* Get IDAC */
                autoTuneConfig.iDac = CapSense_calibratedIdac;

                #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                        (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType))

                    {
                        /* Set the minimum sense clock frequency to calculate the resolution */
                        if (ptrWdgt->snsClk < ptrWdgt->rowSnsClk)
                        {
                            /* Rewrite pointer to Sense Clock Divider for rows */
                            autoTuneConfig.ptrSenseClk = &ptrWdgt->rowSnsClk;

                            /* Set parasitic capacitance for rows */
                            autoTuneConfig.sensorCap = cpWidget[wdgtIndex];

                            /* Get IDAC */
                            autoTuneConfig.iDac = CapSense_calibratedIdacRow;
                        }
                    }
                #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */

                /* Get finger capacitance */
                autoTuneConfig.fingerCap = ptrWdgt->fingerCap;

                /* Init pointer to sigPFC */
                autoTuneConfig.sigPFC = &ptrWdgt->sigPFC;

                /* Find resolution */
                ptrWdgt->resolution = SmartSense_TuneSensitivity(&autoTuneConfig);
            }
        }

        /*
        * Autotune phase #3:
        * - selecting a widget clock source if AUTO
        * - repeating calibration with found clock frequency, resolution and clock source
        * - updating sensitivity
        */

        /* Tune sensitivity for all the widgets */
        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapSense_SENSE_METHOD_CSD_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
            {
                ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                          CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

                CapSense_SsSetWidgetSenseClkSrc(wdgtIndex, ptrWdgt);

                /* Calibrate CSD widget to the default calibration target */
                autoTuneStatus |= CapSense_CSDCalibrateWidget(wdgtIndex, CapSense_CSD_AUTOTUNE_CAL_LEVEL);

                #if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
                    CapSense_DsUpdateWidgetCrc(wdgtIndex);
                #endif /* (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */
            }
        }

        return autoTuneStatus;
    }
#endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)) */


#if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
    /*******************************************************************************
    * Function Name: CapSense_SsChangeClkFreq
    ****************************************************************************//**
    *
    * \brief
    *  This function changes the sensor clock frequency by configuring
    *  the corresponding divider.
    *
    * \details
    *  This function changes the sensor clock frequency by configuring
    *  the corresponding divider.
    *
    * \param chId
    *  The frequency channel ID.
    *
    *******************************************************************************/
    void CapSense_SsChangeClkFreq(uint32 chId)
    {
        uint32 snsClkDivider;
        uint32 freqOffset1 = 0u;
        uint32 freqOffset2 = 0u;

        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            uint32 conversionsNum;

            #if((CapSense_CLK_SOURCE_PRS8  == CapSense_CSD_SNS_CLK_SOURCE) ||\
                (CapSense_CLK_SOURCE_PRS12 == CapSense_CSD_SNS_CLK_SOURCE) ||\
                (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))
                uint32 snsClkSrc;
            #endif
        #endif

        #if ((0u != CapSense_TOTAL_CSD_WIDGETS) || \
             ((CapSense_DISABLE == CapSense_CSX_COMMON_TX_CLK_EN) && (0u != CapSense_TOTAL_CSX_WIDGETS)))
            CapSense_FLASH_WD_STRUCT const *ptrFlashWdgt = &CapSense_dsFlash.wdgtArray[CapSense_widgetIndex];
            CapSense_RAM_WD_BASE_STRUCT const *ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)ptrFlashWdgt->ptr2WdgtRam;
        #endif

        switch(CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[CapSense_widgetIndex]))
        {
        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            case CapSense_SENSE_METHOD_CSD_E:
                /* Get sensor clock divider value */
                #if (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN)
                    snsClkDivider = CapSense_dsRam.snsCsdClk;
                #else /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */
                    #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
                        /* Get SnsClck divider for rows or columns */
                        if (CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].numCols <= CapSense_sensorIndex)
                        {
                            snsClkDivider = ptrWdgt->rowSnsClk;
                        }
                        else
                        {
                            snsClkDivider = ptrWdgt->snsClk;
                        }
                    #else
                        snsClkDivider = ptrWdgt->snsClk;
                    #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */
                #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */

                freqOffset1 = CapSense_CSD_MFS_DIVIDER_OFFSET_F1;
                freqOffset2 = CapSense_CSD_MFS_DIVIDER_OFFSET_F2;

                #if((CapSense_CLK_SOURCE_PRS8  == CapSense_CSD_SNS_CLK_SOURCE) ||\
                    (CapSense_CLK_SOURCE_PRS12 == CapSense_CSD_SNS_CLK_SOURCE) ||\
                    (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))
                    /* Get sense clk source */
                    #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
                        /* Get SnsClc Source for rows or columns */
                        if (CapSense_dsFlash.wdgtArray[CapSense_widgetIndex].numCols <= CapSense_sensorIndex)
                        {
                            snsClkSrc = (uint32)ptrWdgt->rowSnsClkSource;
                        }
                        else
                        {
                            snsClkSrc = (uint32)ptrWdgt->snsClkSource;
                        }
                    #else
                        snsClkSrc = (uint32)ptrWdgt->snsClkSource;
                    #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */

                    switch (snsClkSrc)
                    {
                    case CapSense_CLK_SOURCE_PRS8:
                    case CapSense_CLK_SOURCE_PRS12:
                        /* Multiply by 2 for PRS8/PRS12 mode */
                        freqOffset1 <<= 1u;
                        freqOffset2 <<= 1u;
                        break;

                    default:
                        break;
                    }
                #endif

                /* Change the divider based on the chId */
                switch (chId)
                {
                    case CapSense_FREQ_CHANNEL_1:
                    {
                        snsClkDivider += freqOffset1;
                        break;
                    }
                    case CapSense_FREQ_CHANNEL_2:
                    {
                        snsClkDivider += freqOffset2;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                /* Set Number Of Conversions based on scanning resolution */
                conversionsNum = CapSense_SsCSDGetNumberOfConversions(snsClkDivider, (uint32)ptrWdgt->resolution, (uint32)ptrWdgt->snsClkSource);
                CY_SET_REG32(CapSense_CSD_SEQ_NORM_CNT_PTR, (conversionsNum & CapSense_CSD_SEQ_NORM_CNT_CONV_CNT_MSK));

                #if((CapSense_CLK_SOURCE_PRS8  == CapSense_CSD_SNS_CLK_SOURCE) ||\
                    (CapSense_CLK_SOURCE_PRS12 == CapSense_CSD_SNS_CLK_SOURCE) ||\
                    (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))
                    switch (snsClkSrc)
                    {
                    case CapSense_CLK_SOURCE_PRS8:
                    case CapSense_CLK_SOURCE_PRS12:
                        /* Divide by 2 for PRS8/PRS12 mode */
                        snsClkDivider >>= 1;
                        if (snsClkDivider == 0u)
                        {
                            snsClkDivider = 1u;
                        }
                        break;

                    default:
                        break;
                    }
                #endif

                /* Configure the new divider */
                CapSense_SsSetSnsClockDivider(snsClkDivider);

                break;
        #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

        #if (0u != CapSense_TOTAL_CSX_WIDGETS)
            case CapSense_SENSE_METHOD_CSX_E:
                #if (CapSense_ENABLE == CapSense_CSX_COMMON_TX_CLK_EN)
                    snsClkDivider = CapSense_dsRam.snsCsxClk;
                #else /* (CapSense_ENABLE == CapSense_CSX_COMMON_TX_CLK_EN) */
                    snsClkDivider = ptrWdgt->snsClk;
                #endif /* (CapSense_ENABLE == CapSense_CSX_COMMON_TX_CLK_EN) */
                freqOffset1 = CapSense_CSX_MFS_DIVIDER_OFFSET_F1;
                freqOffset2 = CapSense_CSX_MFS_DIVIDER_OFFSET_F2;

                /* Change the divider based on the chId */
                switch (chId)
                {
                    case CapSense_FREQ_CHANNEL_1:
                    {
                        snsClkDivider += freqOffset1;
                        break;
                    }
                    case CapSense_FREQ_CHANNEL_2:
                    {
                        snsClkDivider += freqOffset2;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }

                /* Configure the new divider */
                CapSense_SsSetSnsClockDivider(snsClkDivider);

                break;
        #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

        default:
            CY_ASSERT(0 != 0);
            break;
        }
    }
#endif  /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */


#if((CapSense_ENABLE == CapSense_CSD_EN) ||\
    (CapSense_ENABLE == CapSense_CSX_EN))
/*******************************************************************************
* Function Name: CapSense_SsInitializeSourceSenseClk
****************************************************************************//**
*
* \brief
*  Sets a source for Sense Clk for all CSD widgets.
*
* \details
*  Updates snsClkSource and rowSnsClkSource with a source for the sense Clk.
*  for all CSD widgets.
*
*******************************************************************************/
void CapSense_SsInitializeSourceSenseClk(void)
{
    uint32 wdgtIndex;
    CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

    for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
    {
        ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

        switch(CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
        {
        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            case CapSense_SENSE_METHOD_CSD_E:
                CapSense_SsSetWidgetSenseClkSrc(wdgtIndex, ptrWdgt);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

        #if (0u != CapSense_TOTAL_CSX_WIDGETS)
            case CapSense_SENSE_METHOD_CSX_E:
                CapSense_SsSetWidgetTxClkSrc(wdgtIndex, ptrWdgt);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

        default:
            CY_ASSERT(0 != 0);
            break;
        }

        #if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
            CapSense_DsUpdateWidgetCrc(wdgtIndex);
        #endif /* (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */
    }
}
#endif /* ((CapSense_ENABLE == CapSense_CSD_EN) ||\
           (CapSense_ENABLE == CapSense_CSX_EN)) */


#if (CapSense_ENABLE == CapSense_CSD_EN)
    /*******************************************************************************
    * Function Name: CapSense_SsSetWidgetSenseClkSrc
    ****************************************************************************//**
    *
    * \brief
    *  Sets a source for the sense clock for a widget.
    *
    * \param wdgtIndex
    *  Specifies the ID of the widget.
    * \param ptrWdgt
    *  The pointer to the RAM_WD_BASE_STRUCT structure.
    *
    * \details
    *  Updates snsClkSource and rowSnsClkSource with a source for the sense Clk for a
    *  widget.
    *
    *******************************************************************************/
    static void CapSense_SsSetWidgetSenseClkSrc(uint32 wdgtIndex, CapSense_RAM_WD_BASE_STRUCT * ptrWdgt)
    {
        uint8 lfsrSize;
        uint8 lfsrScale;

        #if(CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
            uint32 conversionsNum;
            uint32 snsClkDivider;
        #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */

        #if(CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
            snsClkDivider = CapSense_SsCSDGetColSnsClkDivider(wdgtIndex);

                conversionsNum = CapSense_SsCSDGetNumberOfConversions(snsClkDivider, (uint32)ptrWdgt->resolution,
                                                                                             CapSense_CLK_SOURCE_DIRECT);
                lfsrSize = CapSense_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                if (CapSense_CLK_SOURCE_DIRECT == lfsrSize)
                {
                    lfsrSize = CapSense_SsCSDCalcPrsSize(snsClkDivider << 1uL, (uint32)ptrWdgt->resolution);
                }
                lfsrScale = CapSense_SsCalcLfsrScale(snsClkDivider, lfsrSize);

                if((lfsrSize == CapSense_CLK_SOURCE_PRS8) || (lfsrSize == CapSense_CLK_SOURCE_PRS12))
                {
                    CapSense_SsCSDSetColSnsClkDivider(wdgtIndex, snsClkDivider);
                }
        #else
            lfsrSize = (uint8)CapSense_DEFAULT_MODULATION_MODE;
            lfsrScale = 0u;
            (void)wdgtIndex; /* This parameter is unused in such configurations */
        #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */

        ptrWdgt->snsClkSource = lfsrSize | (uint8)(lfsrScale << CapSense_CLK_SOURCE_LFSR_SCALE_OFFSET);

        #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
            if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType))
            {
                #if(CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
                    snsClkDivider = CapSense_SsCSDGetRowSnsClkDivider(wdgtIndex);

                        lfsrSize = CapSense_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                        if (CapSense_CLK_SOURCE_DIRECT == lfsrSize)
                        {
                            lfsrSize = CapSense_SsCSDCalcPrsSize(snsClkDivider << 1uL, (uint32)ptrWdgt->resolution);
                        }
                        lfsrScale = CapSense_SsCalcLfsrScale(snsClkDivider, lfsrSize);

                        if((lfsrSize == CapSense_CLK_SOURCE_PRS8) || (lfsrSize == CapSense_CLK_SOURCE_PRS12))
                        {
                            CapSense_SsCSDSetRowSnsClkDivider(wdgtIndex, snsClkDivider);
                        }
                #else
                    lfsrSize = (uint8)CapSense_DEFAULT_MODULATION_MODE;
                    lfsrScale = 0u;
                #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */
                ptrWdgt->rowSnsClkSource = lfsrSize | (uint8)(lfsrScale << CapSense_CLK_SOURCE_LFSR_SCALE_OFFSET);
            }
        #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */
    }
#endif /* (CapSense_ENABLE == CapSense_CSD_EN) */


#if (CapSense_ENABLE == CapSense_CSX_EN)
    /*******************************************************************************
    * Function Name: CapSense_SsSetWidgetTxClkSrc
    ****************************************************************************//**
    *
    * \brief
    *  Sets a source for the Tx clock for a widget.
    *
    * \param wdgtIndex
    *  Specifies the ID of the widget.
    * \param ptrWdgt
    *  The pointer to the RAM_WD_BASE_STRUCT structure.
    *
    * \details
    *  Updates snsClkSource with with a source for Tx Clk for a widget.
    *
    *******************************************************************************/
    __STATIC_INLINE void CapSense_SsSetWidgetTxClkSrc(uint32 wdgtIndex, CapSense_RAM_WD_BASE_STRUCT * ptrWdgt)
    {
        uint8 lfsrSize;
        uint8 lfsrScale;

        #if ((CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE) && \
             (CapSense_DISABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES))
            uint32 conversionsNum;
            uint32 snsClkDivider;
        #endif

        #if(CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE)
            #if (CapSense_DISABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES)
                conversionsNum = (uint32)ptrWdgt->resolution;
                snsClkDivider = CapSense_SsCSXGetTxClkDivider(wdgtIndex);
                lfsrSize = CapSense_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                lfsrScale = CapSense_SsCalcLfsrScale(snsClkDivider, lfsrSize);
            #else
                lfsrSize = (uint8)CapSense_CLK_SOURCE_DIRECT;
                lfsrScale = 0u;
                /* Unused function argument */
                (void)wdgtIndex;
            #endif /* (CapSense_ENABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES) */
        #else
            lfsrSize = (uint8)CapSense_CSX_TX_CLK_SOURCE;
            lfsrScale = 0u;
            /* Unused function argument */
            (void)wdgtIndex;
        #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE) */

        ptrWdgt->snsClkSource = lfsrSize | (uint8)(lfsrScale << CapSense_CLK_SOURCE_LFSR_SCALE_OFFSET);
    }
#endif /* (CapSense_ENABLE == CapSense_CSX_EN) */


#if(((CapSense_ENABLE == CapSense_CSX_EN) && \
     (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE) && \
     (CapSense_DISABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES)) ||\
    ((CapSense_ENABLE == CapSense_CSD_EN) && \
     (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)))
/*******************************************************************************
* Function Name: CapSense_SsCalcLfsrSize
****************************************************************************//**
*
* \brief
*  This is an internal function that finds a SSC polynomial size in the Auto mode.
*
* \details
*  The SSC polynomial size in the auto mode is found based on the following
*  requirements:
*  - an LFSR value should be selected so that the max clock dither is limited with +/-10%.
*  - at least one full spread spectrum polynomial should pass during the scan time.
*  - the value of the number of conversions should be an integer multiple of the
*    repeat period of the programmed LFSR_SIZE.
*
* \param
*  snsClkDivider The divider value for the sense clock.
*  resolution The widget resolution.
*
* \return lfsrSize The LFSRSIZE value for the SENSE_PERIOD register.
*
*******************************************************************************/
static uint8 CapSense_SsCalcLfsrSize(uint32 snsClkDivider, uint32 conversionsNum)
{
    uint8 lfsrSize = 0u;

    /* Find LFSR value */
    if((CapSense_SNSCLK_SSC4_THRESHOLD <= snsClkDivider) &&
       (CapSense_SNSCLK_SSC4_PERIOD <= conversionsNum) &&
       (0uL == (conversionsNum % CapSense_SNSCLK_SSC4_PERIOD)))
    {
        lfsrSize = CapSense_CLK_SOURCE_SSC4;
    }
    else if((CapSense_SNSCLK_SSC3_THRESHOLD <= snsClkDivider) &&
            (CapSense_SNSCLK_SSC3_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapSense_SNSCLK_SSC3_PERIOD)))
    {
        lfsrSize = CapSense_CLK_SOURCE_SSC3;
    }
    else if((CapSense_SNSCLK_SSC2_THRESHOLD <= snsClkDivider) &&
            (CapSense_SNSCLK_SSC2_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapSense_SNSCLK_SSC2_PERIOD)))
    {
        lfsrSize = CapSense_CLK_SOURCE_SSC2;
    }
    else if((CapSense_SNSCLK_SSC1_THRESHOLD <= snsClkDivider) &&
            (CapSense_SNSCLK_SSC1_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapSense_SNSCLK_SSC1_PERIOD)))
    {
        lfsrSize = CapSense_CLK_SOURCE_SSC1;
    }
    else
    {
        lfsrSize = (uint8)CapSense_CLK_SOURCE_DIRECT;
    }

    return (lfsrSize);
}


/*******************************************************************************
* Function Name: CapSense_SsCalcLfsrScale
****************************************************************************//**
*
* \brief
*  This is an internal function that calculates the LFSR scale value.
*
* \details
*  The LFSR scale value is used to increase the clock dither if the desired dither
*  is wider than can be achieved with the current Sense Clock Divider and current LFSR
*  period.
*
*  This returns the LFSR scale value needed to keep the clock dither in
*  range +/-10%.
*
* \param
*  snsClkDivider The divider value for the sense clock.
*  lfsrSize The period of the LFSR sequence.
*          CapSense_CLK_SOURCE_DIRECT The spreadspectrum is not used.
*          CapSense_CLK_SOURCE_SSC1   The length of LFSR sequence is 63 cycles.
*          CapSense_CLK_SOURCE_SSC2   The length of LFSR sequence is 127 cycles.
*          CapSense_CLK_SOURCE_SSC3   The length of LFSR sequence is 255 cycles.
*          CapSense_CLK_SOURCE_SSC4   The length of LFSR sequence is 511 cycles.
*
* \return The LFSR scale value needed to keep the clock dither in range +/-10%.
*
*******************************************************************************/
static uint8 CapSense_SsCalcLfsrScale(uint32 snsClkDivider, uint8 lfsrSize)
{
    uint32 lfsrScale;
    uint32 lfsrRange;
    uint32 lfsrDither;

    /* Initialize the lfsrSize variable with the LFSR Range for given Lfsr Size. */
    switch(lfsrSize)
    {
        case CapSense_CLK_SOURCE_SSC1:
        {
            lfsrRange = CapSense_SNSCLK_SSC1_RANGE;
            break;
        }
        case CapSense_CLK_SOURCE_SSC2:
        {
            lfsrRange = CapSense_SNSCLK_SSC2_RANGE;
            break;
        }
        case CapSense_CLK_SOURCE_SSC3:
        {
            lfsrRange = CapSense_SNSCLK_SSC3_RANGE;
            break;
        }
        case CapSense_CLK_SOURCE_SSC4:
        {
            lfsrRange = CapSense_SNSCLK_SSC4_RANGE;
            break;
        }
        default:
        {
            lfsrRange = 0u;
            break;
        }
    }

    /* Calculate the LFSR Scale value that is required to keep the Clock dither
     * as close as possible to the +/-10% limit of the used frequency.
     */
    if((lfsrSize != CapSense_CLK_SOURCE_DIRECT) && (0u != lfsrRange))
    {
        /* Calculate the LFSR Dither in percents. */
        lfsrDither  = ((lfsrRange * 100uL) / snsClkDivider);
        lfsrScale = 0uL;

        while(lfsrDither < CapSense_LFSR_DITHER_PERCENTAGE)
        {
            lfsrScale++;
            lfsrDither <<=1uL;
        }

        if(lfsrDither > CapSense_LFSR_DITHER_PERCENTAGE)
        {
            lfsrScale--;
        }
    }
    else
    {
        lfsrScale = 0uL;
    }

    return ((uint8)lfsrScale);
}

#endif /* (((CapSense_ENABLE == CapSense_CSX_EN) && \
            (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE) && \
            (CapSense_DISABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES)) ||\
           ((CapSense_ENABLE == CapSense_CSD_EN) && \
            (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))) */


#if (CapSense_ENABLE == CapSense_CSD_EN)
    /*******************************************************************************
    * Function Name: CapSense_SsClearCSDSensors
    ****************************************************************************//**
    *
    * \brief
    *  Resets all the CSD sensors to the non-sampling state by sequentially
    *  disconnecting all the sensors from the Analog MUX bus and putting them to
    *  an inactive state.
    *
    * \details
    *  The function goes through all the widgets and updates appropriate bits in
    *  the IO HSIOM, PC and DR registers depending on the Inactive sensor connection
    *  parameter. DR register bits are set to zero when the Inactive sensor
    *  connection is Ground or Hi-Z.
    *
    *******************************************************************************/
    void CapSense_SsClearCSDSensors(void)
    {
        uint32 wdgtIndex;
        uint32 snsIndex;

        #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
            uint32 pinIndex;
        #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */

        #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
            /* Declare ptr to sensor IO structure */
            CapSense_FLASH_IO_STRUCT const *curDedicatedSnsIOPtr;
            /* Pointer to Flash structure holding info of sensor to be scanned */
            CapSense_FLASH_SNS_STRUCT const *curFlashSnsPtr;
        #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */
        CapSense_FLASH_IO_STRUCT const *curSnsIOPtr;

        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapSense_SENSE_METHOD_CSD_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
            {
                curSnsIOPtr = (CapSense_FLASH_IO_STRUCT const *)
                                                CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2SnsFlash;

                /* Go through all the sensors in widget */
                for (snsIndex = 0u; snsIndex < (uint8)CapSense_dsFlash.wdgtArray[wdgtIndex].totalNumSns; snsIndex++)
                {
                    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
                        /* Check ganged sns flag */
                        if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[wdgtIndex].staticConfig &
                                                                 CapSense_GANGED_SNS_MASK))
                        {
                            /* Get sns pointer */
                            curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)
                                                               CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2SnsFlash +
                                                               snsIndex;

                            for(pinIndex = 0u; pinIndex < curFlashSnsPtr->numPins; pinIndex++)
                            {
                                /* Get IO pointer for dedicated pin */
                                curDedicatedSnsIOPtr = &CapSense_ioList[curFlashSnsPtr->firstPinId + pinIndex];

                                /* Disconnect dedicated pin */
                                CapSense_CSDDisconnectSns(curDedicatedSnsIOPtr);
                            }
                        }
                        else
                        {
                            /* Disable sensor */
                            CapSense_CSDDisconnectSns(&curSnsIOPtr[snsIndex]);
                        }
                    #else
                        /* Disable sensor */
                        CapSense_CSDDisconnectSns(&curSnsIOPtr[snsIndex]);
                    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */
                }
            }
        }
    }

    /*******************************************************************************
    * Function Name: CapSense_SsCSDGetColSnsClkDivider
    ****************************************************************************//**
    *
    * \brief
    *  This function gets the Sense Clock Divider value for one-dimension widgets
    *  and Column Sense Clock divider value for two-dimension widgets.
    *
    * \details
    *  This function gets the Sense Clock Divider value based on the Component
    *  configuration. The function is applicable for one-dimension widgets and for
    *  two-dimension widgets.
    *
    * \param
    *  widgetId Specifies the ID of the widget.
    *
    * \return The Sense Clock Divider value for one-dimension widgets
    *        and the Column Sense Clock divider value for two-dimension widgets.
    *
    *******************************************************************************/
    uint32 CapSense_SsCSDGetColSnsClkDivider(uint32 widgetId)
    {
        uint32 retVal;

        /* Get sense divider based on configuration */
        #if (CapSense_ENABLE != CapSense_CSD_COMMON_SNS_CLK_EN)
            CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

            ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
            CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

            retVal = (uint32)(ptrWdgt->snsClk);
        #else
            retVal = (uint32)CapSense_dsRam.snsCsdClk;

            (void)widgetId; /* This parameter is unused in such configurations */
        #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */

        return (retVal);
    }


    /*******************************************************************************
    * Function Name: CapSense_SsCSDSetColSnsClkDivider
    ****************************************************************************//**
    *
    * \brief
    *  This function sets the Sense Clock Divider value for one-dimension widgets
    *  and Column Sense Clock divider value for two-dimension widgets.
    *
    * \details
    *  This function sets the Sense Clock Divider value based on the Component
    *  configuration. The function is applicable for one-dimension widgets and for
    *  two-dimension widgets.
    *
    * \param
    *  widgetId Specifies the ID of the widget.
    *
    * \return The Sense Clock Divider value for one-dimension widgets
    *        and the Column Sense Clock divider value for two-dimension widgets.
    *
    *******************************************************************************/
    void CapSense_SsCSDSetColSnsClkDivider(uint32 widgetId, uint32 dividerVal)
    {
        /* Get sense divider based on configuration */
        #if (CapSense_ENABLE != CapSense_CSD_COMMON_SNS_CLK_EN)
            CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

            ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
            CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

            ptrWdgt->snsClk = (uint16)dividerVal;
        #else
            CapSense_dsRam.snsCsdClk = (uint16)dividerVal;

            (void)widgetId; /* This parameter is unused in such configurations */
        #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */
    }


    #if (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDGetRowSnsClkDivider
        ****************************************************************************//**
        *
        * \brief
        *  This function gets the Sense Clock Divider value for one-dimension widgets
        *  and the Column Sense Clock divider value for two-dimension widgets.
        *
        * \details
        *  This function gets the Sense Clock Divider value based on the Component
        *  configuration. The function is applicable for one-dimension widgets and for
        *  two-dimension widgets.
        *
        * \param
        *  widgetId Specifies the ID of the widget.
        *
        * \return
        *  Returns the sense clock divider value for one-dimension widgets
        *  and column sense clock divider value for two-dimension widgets.
        *
        *******************************************************************************/
        uint32 CapSense_SsCSDGetRowSnsClkDivider(uint32 widgetId)
        {
            uint32 retVal;

            /* Get sense divider based on configuration */
            #if (CapSense_ENABLE != CapSense_CSD_COMMON_SNS_CLK_EN)
                CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

                ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

                retVal = ptrWdgt->rowSnsClk;
            #else
                retVal = (uint32)CapSense_dsRam.snsCsdClk;

                (void)widgetId; /* This parameter is unused in such configurations */
            #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */

            return (retVal);
        }


        /*******************************************************************************
        * Function Name: CapSense_SsCSDSetRowSnsClkDivider
        ****************************************************************************//**
        *
        * \brief
        *  This function sets the Sense Clock Divider value for one-dimension widgets
        *  and the Column Sense Clock divider value for two-dimension widgets.
        *
        * \details
        *  This function sets the Sense Clock Divider value based on the Component
        *  configuration. The function is applicable for one-dimension widgets and for
        *  two-dimension widgets.
        *
        * \param
        *  widgetId Specifies the ID of the widget.
        *
        * \param
        *  dividerVal Specifies the Sense Clock Divider value.
        *
        *******************************************************************************/
        void CapSense_SsCSDSetRowSnsClkDivider(uint32 widgetId, uint32 dividerVal)
        {
            /* Get sense divider based on configuration */
            #if (CapSense_ENABLE != CapSense_CSD_COMMON_SNS_CLK_EN)
                CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

                ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

                ptrWdgt->rowSnsClk = (uint16)dividerVal;
            #else
                CapSense_dsRam.snsCsdClk = (uint16)dividerVal;

                (void)widgetId; /* This parameter is unused in such configurations */
            #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */
        }
    #endif /* (CapSense_CSD_MATRIX_WIDGET_EN || CapSense_CSD_TOUCHPAD_WIDGET_EN) */


    #if (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDCalcPrsSize
        ****************************************************************************//**
        *
        * \brief
        *  The function finds PRS polynomial size in the Auto mode.
        *
        * \details
        *  The PRS polynomial size in the Auto mode is found based on the following
        *  requirements:
        *  - at least one full spread spectrum polynomial should pass during scan time.
        *
        * \param
        *  snsClkDivider The divider value for the sense clock.
        *  resolution The widget resolution.
        *
        * \return prsSize PRS value for SENSE_PERIOD register.
        *
        *******************************************************************************/
        uint8 CapSense_SsCSDCalcPrsSize(uint32 snsClkDivider, uint32 resolution)
        {
            uint32 prsSize;

            if ((snsClkDivider * CapSense_PRS_LENGTH_12_BITS) <= ((0x00000001Lu << resolution) - 1u))
            {
                /* Set PRS12 mode */
                prsSize = CapSense_CLK_SOURCE_PRS12;
            }
            else if ((snsClkDivider * CapSense_PRS_LENGTH_8_BITS) <= ((0x00000001Lu << resolution) - 1u))
            {
                /* Set PRS8 mode */
                prsSize = CapSense_CLK_SOURCE_PRS8;
            }
            else
            {
                /* Set Direct clock mode */
                prsSize = CapSense_CLK_SOURCE_DIRECT;
            }

            return (uint8)prsSize;
        }
    #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */
#endif /* (CapSense_ENABLE == CapSense_CSD_EN) */


/*******************************************************************************
* Function Name: CapSense_BistDischargeExtCapacitors
****************************************************************************//**
*
* \brief
*  The function discharge available external capacitors.
*
* \details
*  The function discharge available external capacitors by connection them
*  to GND using STRONG GPIO drive mode. Additionaly, the function disconnects
*  the capacitors from analog mux buses if connected.
*  Note: the function does not restore the connection to analog mux busses
*  and supposes that all the capacitors belong to a single device port.
*
*******************************************************************************/
void CapSense_BistDischargeExtCapacitors(void)
{
    #if (CapSense_ENABLE == CapSense_CSD_EN)
        Cy_GPIO_SetHSIOM(CapSense_Cmod_0_PORT, CapSense_Cmod_0_NUM, HSIOM_SEL_GPIO);
        Cy_GPIO_Clr(CapSense_Cmod_0_PORT, CapSense_Cmod_0_NUM);
        Cy_GPIO_SetDrivemode(CapSense_Cmod_0_PORT, CapSense_Cmod_0_NUM, CY_GPIO_DM_STRONG_IN_OFF);

        #if((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
            (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
            Cy_GPIO_SetHSIOM(CapSense_Csh_0_PORT, CapSense_Csh_0_NUM, HSIOM_SEL_GPIO);
            Cy_GPIO_Clr(CapSense_Csh_0_PORT, CapSense_Csh_0_NUM);
            Cy_GPIO_SetDrivemode(CapSense_Csh_0_PORT, CapSense_Csh_0_NUM, CY_GPIO_DM_STRONG_IN_OFF);
        #endif
    #endif

    #if (CapSense_ENABLE == CapSense_CSX_EN)
        Cy_GPIO_SetHSIOM(CapSense_CintA_0_PORT, CapSense_CintA_0_NUM, HSIOM_SEL_GPIO);
        Cy_GPIO_Clr(CapSense_CintA_0_PORT, CapSense_CintA_0_NUM);
        Cy_GPIO_SetDrivemode(CapSense_CintA_0_PORT, CapSense_CintA_0_NUM, CY_GPIO_DM_STRONG_IN_OFF);

        Cy_GPIO_SetHSIOM(CapSense_CintB_0_PORT, CapSense_CintB_0_NUM, HSIOM_SEL_GPIO);
        Cy_GPIO_Clr(CapSense_CintB_0_PORT, CapSense_CintB_0_NUM);
        Cy_GPIO_SetDrivemode(CapSense_CintB_0_PORT, CapSense_CintB_0_NUM, CY_GPIO_DM_STRONG_IN_OFF);
    #endif

    Cy_SysLib_DelayUs(CapSense_EXT_CAP_DISCHARGE_TIME);

    #if (CapSense_ENABLE == CapSense_CSD_EN)
        Cy_GPIO_SetDrivemode(CapSense_Cmod_0_PORT, CapSense_Cmod_0_NUM, CY_GPIO_DM_ANALOG);

        #if((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
            (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))
            Cy_GPIO_SetDrivemode(CapSense_Csh_0_PORT, CapSense_Csh_0_NUM, CY_GPIO_DM_ANALOG);
        #endif
    #endif

    #if (CapSense_ENABLE == CapSense_CSX_EN)
        Cy_GPIO_SetDrivemode(CapSense_CintA_0_PORT, CapSense_CintA_0_NUM, CY_GPIO_DM_ANALOG);
        Cy_GPIO_SetDrivemode(CapSense_CintB_0_PORT, CapSense_CintB_0_NUM, CY_GPIO_DM_ANALOG);
    #endif
}


/* [] END OF FILE */
