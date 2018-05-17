/***************************************************************************//**
* \file cy_ble_hal_pvt.h
* \version 2.0
*
* \brief
*  Contains the function prototypes and constants for the HAL section
*  of the BLE Middleware.
*
********************************************************************************
* \copyright
* Copyright 2017-2018, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#ifndef CY_BLE_HALL_PVT_H
#define CY_BLE_HALL_PVT_H

#include "cy_ble_stack_pvt.h"
#include "cy_ble.h"
#include "cy_ble_clk.h"

#include "syspm/cy_syspm.h"
#include "flash/cy_flash.h"
#include "ipc/cy_ipc_pipe.h"
#include "cy_device_headers.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/***************************************
* Enumerated Types and Structures
***************************************/

/**
 * \addtogroup group_ble_common_api_definitions
 * @{
 */
/** Interrupt callback feature */
typedef enum
{
    /** Interrupt callback feature is disabled */
    CY_BLE_INTR_CALLBACK_NONE,
    
    /** Executed on every trigger of BLESS interrupt. */
    CY_BLE_INTR_CALLBACK_BLESS_STACK_ISR             = (0x01u << 1u),
    
    /** Executed when the BLESS exits DeepSleep mode and enters Active mode. 
     *  BLESS DeepSleep exit can be triggered automatically by link layer hardware
     *  or by different BLE_PDL data transfer APIs which needs BLESS to be active. 
     */
    CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_EXITED  = (0x01u << 2u),
    
    /** Executed when the BLESS connection engine in slave mode detects a BLE packet 
     *  which matches its access address. 
     */
    CY_BLE_INTR_CALLBACK_BLELL_CONN_EXT_INTR_EARLY   = (0x01u << 3u),
    
    /** Executed when the BLESS connection engine receives a non-empty packet from 
     *  the peer device.
     */
    CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_RX       = (0x01u << 4u),
    
    /** Executed when the BLESS connection engine receives an ACK packet from the peer
     *  device for the previously transmitted packet.
     */
    CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_TX_ACK   = (0x01u << 5u),
    
    /** Executed when the BLESS connection engine closes the connection event. 
     *  This interrupt will be executed on every connection interval for a connection,
     *  irrespective of data tx/rx state. 
     */
    CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CLOSE_CE    = (0x01u << 6u),
    
    /** Executed when the BLESS enters DeepSleep mode. User call to Cy_SysPm_DeepSleep API 
     *  will trigger BLESS DeepSleep entry sequence. 
     */
    CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_ENTERED = (0x01u << 7u),
    
    /** Executed when the BLESS scan engine receives an advertisement packet from the
     *  peer device 
     */
    CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_ADV_RX      = (0x01u << 8u),
    
    /** Executed when the BLESS scan engine receives a scan response packet from the peer
     *  device in response to a scan request from the scanner. 
     */
    CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_SCAN_RSP_RX = (0x01u << 9u),
    
    /** Executed when the BLESS advertisement engine receives a connection request from 
     *  the peer central device
     */
    CY_BLE_INTR_CALLBACK_BLELL_ADV_INTR_CONN_REQ_RX  = (0x01u << 10u),

} cy_en_ble_interrupt_callback_feature_t;

/** @} group_ble_common_api_definitions */

/** \cond IGNORE */

typedef struct 
{
    uint8_t  clientID;
    uint8_t  pktType;
    uint16_t intrRelMask;
    uint32_t data; 
    uint16_t dataLen;
} cy_stc_ble_ipc_msg_t;

/** \endcond */

/***************************************
*   HAL Constants
***************************************/
#define CY_BLE_HCI_COMMAND_SUCCEEDED        (0x00u)
#define CY_BLE_UNKNOWN_HCI_COMMAND_ERROR    (0x01u)

#define CY_BLE_CYPIPE_MSG_COMPLETE_ID       (0u)
#define CY_BLE_CYPIPE_MSG_SEND_ID           (1u)
#define CY_BLE_CYPIPE_MSG_INTR_NOTIFY_ID    (3u)

#define CY_BLE_FLASH_OPR_TIMEOUT            (200u)

#if (CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M4)
    #define CY_BLE_IPC_CONTROLLER_ADDR      (CY_IPC_EP_CYPIPE_CM0_ADDR)
    #define CY_BLE_IPC_HOST_ADDR            (CY_IPC_EP_CYPIPE_CM4_ADDR)   
#else
    #define CY_BLE_IPC_CONTROLLER_ADDR      (CY_IPC_EP_CYPIPE_CM4_ADDR)
    #define CY_BLE_IPC_HOST_ADDR            (CY_IPC_EP_CYPIPE_CM0_ADDR)   
#endif /* (CY_BLE_CONFIG_HOST_CORE == CY_BLE_CORE_CORTEX_M4) */

#define CY_BLE_SFLASH_BLE_RADIO_CALL_ADDRESS   ((uint32_t)SFLASH->BLE_DEVICE_ADDRESS + sizeof(cy_stc_ble_gap_bd_addr_t))

/***************************************
*   Function Prototypes
***************************************/

/* HAL ISR prototypes */
void Cy_BLE_BlessInterrupt(void);

#if (CY_BLE_STACK_MODE_HOST_UART)
void Cy_BLE_HAL_HOST_UART_Interrupt(void);
#endif /* (CY_BLE_STACK_MODE_HOST_UART) */
    
#if (CY_BLE_STACK_MODE_CONTR_UART)
void Cy_BLE_HAL_UART_Interrupt(void);
#endif /* (CY_BLE_STACK_MODE_CONTR_UART) */

void Cy_BLE_HAL_DelayUs(uint16_t delayVal);
void Cy_BLE_HAL_DelayMs(uint32_t delayVal);
void Cy_BLE_HAL_EnableGlobalInterrupts(void);
void Cy_BLE_HAL_DisableGlobalInterrupts(void);
void Cy_BLE_HAL_BlessStart(void);
uint32_t Cy_BLE_HAL_GetIpBlockVersion(void);
uint32_t Cy_BLE_HAL_GetIcPackageType(void);
bool Cy_BLE_HAL_IsDefinedTx5dbm(void);

uint32_t Cy_BLE_HAL_EnterCriticalSection(void);
void Cy_BLE_HAL_ExitCriticalSection(uint32_t interruptState);
cy_en_syspm_status_t Cy_BLE_HAL_SysPmSleep(cy_en_syspm_waitfor_t enWaitFor);
void Cy_BLE_HAL_SimoBuckSetVoltage2(cy_en_syspm_simo_buck_voltage2_t voltage);

/* Stack Interface to clock */
int32_t Cy_BLE_HAL_ClkMeasurementCountersDone(void);
uint32_t Cy_BLE_HAL_ClkMeasurementCountersGetFreq(bool measuredClock, uint32_t refClkFreq);
uint32_t Cy_BLE_HAL_StartClkMeasurementCounters(cy_en_meas_clks_t clock1, uint32_t count1, cy_en_meas_clks_t clock2);
int32_t Cy_BLE_HAL_PiloTrim(uint32_t piloFreq);
cy_en_clklf_in_sources_t Cy_BLE_HAL_LfClkGetSource(void);

/* Stack Interface to flash */
cy_en_ble_api_result_t Cy_BLE_HAL_NvramWrite(const uint8_t buffer[], const uint8_t varFlash[], uint32_t length);
cy_en_ble_api_result_t Cy_BLE_HAL_NvramNonBlockingRowWrite(const uint8_t buffer[], const uint8_t varFlash[], uint32_t length);
cy_en_ble_api_result_t Cy_BLE_HAL_StackNvramWrite(const uint8_t buffer[], const uint8_t varFlash[], uint32_t length);
bool Cy_BLE_HAL_NvramWriteIsBusy(void);

/* CRC APIs */
uint8_t Cy_BLE_HAL_CalcCRC8(uint8_t data[], uint32_t length);
uint8_t Cy_BLE_HAL_BlockCalcCRC8(uint8_t data[], uint32_t length, uint8_t crcSeed);

#if(CY_BLE_STACK_CONTR_CORE)
    cy_en_ble_api_result_t Cy_BLE_HAL_RadioCalibrationRead(void *data, uint32_t length);
    cy_en_ble_api_result_t Cy_BLE_HAL_RadioCalibrationWrite(const void *data, uint32_t length);
#endif /* CY_BLE_STACK_CONTR_CORE */

#if (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_STACK_MODE_IPC) && (CY_BLE_MODE_PROFILE)
    void Cy_BLE_HAL_IntrNotifyIpcHandler(void);
#endif /* (CY_BLE_INTR_NOTIFY_FEATURE_ENABLED) && (CY_BLE_STACK_MODE_IPC) && (CY_BLE_MODE_PROFILE) */    


#if (CY_BLE_SECURE_CONN_FEATURE_ENABLED)
   #if (CY_BLE_MODE_PROFILE)
    void Cy_BLE_HAL_MappingEccHeapInit(uint8_t *heapMem, uint8_t numOfConn); 
    void Cy_BLE_HAL_MappingEccHeapDeInit(void);
    uint16_t Cy_BLE_HAL_MappingEccGetFeatureHeapReq(uint8_t numOfConn);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingEccGenerateSecurityKeypair(uint8_t p_publicKey[], 
                                                                        uint8_t p_privateKey[], 
                                                                        uint8_t random[]);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingEccGenerateDHKey(const uint8_t p_publicKey[], 
                                                              const uint8_t p_privateKey[], 
                                                              uint8_t p_secret[], uint8_t ci);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingEccValidPublicKey(const uint8_t p_publicKey[]);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingPairingLocalPublicKeyHandler(void *param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingPairingRemoteKeyHandler(void *param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingPairingDhkeyHandler(void *param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingPairingDhkeyCheckHandler(void *param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingPairingKeypressNotificationHandler(void *param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingPairingRandHandler(void * param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingPairingConfirmHandler(void *param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingPairingLrConfirmingHandler(void *param);
    void Cy_BLE_HAL_MappingTbxDhkeyGenerateComplete(void *param);
    void Cy_BLE_HAL_MappingTbxLocalPubkeyGenerateComplete(void *param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingTbxGenerateLocalP256PublicKey(uint8_t param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingTbxGenerateDHkey(void  *param1, void  *param2, uint8_t param3);
    void Cy_BLE_HAL_MappingSmpScCmacComplete(uint8_t param);
    cy_en_ble_api_result_t Cy_BLE_HAL_MappingSeSmpScUserPasskeyHandler(void *param, void *param2);
    void Cy_BLE_HAL_MappingEccPointMult(uint8_t param);
    #endif /* (CY_BLE_MODE_PROFILE) */
#endif     /* (CY_BLE_SECURE_CONN_FEATURE_ENABLED) */

#if (CY_BLE_STACK_MODE_HOST_UART)
    void Cy_BLE_HAL_HOST_UART_Start(void);
    void Cy_BLE_HAL_HOST_UART_Stop(void);
    void Cy_BLE_HAL_HOST_UART_Transmit(const uint8_t *dataBuf, uint8_t length);
    void Cy_BLE_HAL_HOST_UART_IsrEnable(void);
    void Cy_BLE_HAL_HOST_UART_IsrDisable(void);
#endif    /* (CY_BLE_STACK_MODE_HOST_UART) */
    
#if (CY_BLE_STACK_MODE_CONTR_UART)
    void Cy_BLE_HAL_UART_Start(void);
    void Cy_BLE_HAL_UART_Stop(void);
    void Cy_BLE_HAL_UART_Transmit(uint8_t *dataBuf, uint8_t length);
    void Cy_BLE_HAL_UART_IsrEnable(void);
    void Cy_BLE_HAL_UART_IsrDisable(void);
#endif /* (CY_BLE_STACK_MODE_CONTR_UART) */

#if (CY_BLE_STACK_MODE_IPC)
    void Cy_BLE_Ipc_Cypipe_Isr_Enable(void);
    void Cy_BLE_Ipc_Cypipe_Isr_Disable(void);
#if (CY_BLE_STACK_CONTR_CORE)
cy_en_ipc_pipe_status_t Cy_BLE_IPC_ControllerRegisterClientCallbacks(cy_ipc_pipe_callback_ptr_t ctrlMsgRecvCallBack,
                                                                     cy_ipc_pipe_callback_ptr_t ctrlMsgFlushRecvCallBack);
cy_en_ipc_pipe_status_t Cy_BLE_IPC_SendMessageToHost(uint32_t *msg,
                                                     cy_ipc_pipe_relcallback_ptr_t controllerIpcRelCallBack,
                                                     cy_ipc_pipe_relcallback_ptr_t controllerPollCallBack);
#endif /* (CY_BLE_STACK_CONTR_CORE) */
#if (CY_BLE_HOST_CORE)
cy_en_ipc_pipe_status_t Cy_BLE_IPC_HostRegisterClientCallbacks(cy_ipc_pipe_callback_ptr_t hostMsgRecvCallBack,
                                                               cy_ipc_pipe_callback_ptr_t hostMsgFlushRecvCallBack);
cy_en_ipc_pipe_status_t Cy_BLE_IPC_SendMessageToController(uint32_t *msg,
                                                           cy_ipc_pipe_relcallback_ptr_t hostIpcRelCallBack,
                                                           cy_ipc_pipe_relcallback_ptr_t hostPollCallBack);

#endif /* (CY_BLE_HOST_CORE) */
#endif /* (CY_BLE_STACK_MODE_IPC) */

#if (CY_BLE_HOST_CORE)
extern cy_ble_intr_callback_t Cy_BLE_InterruptCallback;   
#endif /* (CY_BLE_HOST_CORE) */

#if CY_BLE_STACK_CONTR_CORE
extern cy_stc_ble_intr_notify_t *intrNotifyPtr;
#endif /* CY_BLE_STACK_CONTR_CORE */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* CY_BLE_HALL_PVT_H  */

/* [] END OF FILE */
