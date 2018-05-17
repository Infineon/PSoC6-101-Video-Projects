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
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/*****************************************************************************\
 * Function:    writeLed
 * Input:       uint8_t - brightnes ... a number between 0 and 100
 * Returns:     void
 * Description: 
 *   If there is a connection and you have discovered the brightness characteristic
 *   then write it/
\*****************************************************************************/
void writeLed(uint8_t brightness)
{
    if(Cy_BLE_GetConnectionState( cy_ble_connHandle[0]) != CY_BLE_CONN_STATE_CLIENT_DISCOVERED)
    {
        printf("Not connected\n");
        return;
    }
    printf("Brightness = %d\r\n",brightness);
    
    cy_stc_ble_gattc_write_req_t myVal;
    myVal.handleValPair.attrHandle = cy_ble_customCServ [CY_BLE_CUSTOMC_LED_SERVICE_INDEX].customServChar[CY_BLE_CUSTOMC_LED_GREEN_CHAR_INDEX].customServCharHandle[0];
    myVal.handleValPair.value.val = &brightness;
    myVal.handleValPair.value.len = 1;
    myVal.connHandle = cy_ble_connHandle[0];
    
    if(Cy_BLE_GATTC_WriteCharacteristicValue( &myVal ) != CY_BLE_SUCCESS)
        printf("BLE GATTC write error\n\r");
}

/*****************************************************************************\
 * Function:    findAdvInfo
 * Input:       uint8_t *adv A pointer to the bytes of the advertising packet
 *              uint8_t len  The number of bytes in the packet
 * Returns:     
 * Description: 
 *   This function scans through the the advertising packet looking for the name
 *   field and the service field.  If it finds them then it copies pointers and
 *   lengths into the GLOBAL ... yes GLOBAL structure called currentAdvInfo
 *   this is NOT a perfect parser as it only looks for the first occurance of 
 *   those field types... and it assume that there are only one
\*****************************************************************************/

typedef struct  {
    char *name;
    int name_len;
    uint8_t *serviceUUID;
    uint8 servUUID_len;
} advInfo_t;

advInfo_t currentAdvInfo;

void findAdvInfo(uint8_t *adv,uint8_t len)
{
    memset(&currentAdvInfo,0,sizeof(currentAdvInfo));
    
    for(uint8_t i=0;i<len;)
    {
        switch(adv[i+1])
        {
            case 0x07: // Service UUID
                currentAdvInfo.serviceUUID = &adv[i+2];
                currentAdvInfo.servUUID_len = adv[i]-1;
            break;
            
            case 0x09: // Name
                currentAdvInfo.name = (char *)&adv[i+2];
                currentAdvInfo.name_len = adv[i]-1;
            break;
        }
        i = i + adv[i]+1;
    }
}

/*****************************************************************************\
 * Function:  genericEventHandler
 * Input:     an event .... and a parameter for that event
 * Returns:   void... nothing
 * Description: 
 *   This is the main BLE event handler
 *    
\*****************************************************************************/
void genericEventHandler(uint32_t event, void *eventParameter)
{   
   
    switch (event)
    {
        /* This event is received when the BLE stack is Started */
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            printf("Starting Scan\r\n");
            Cy_BLE_GAPC_StartScan(CY_BLE_SCANNING_FAST,0);  
            Cy_GPIO_Set(LED9_PORT,LED9_NUM);
        break;

        case CY_BLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            // Print Out Information about the Device that was found
            printf("Device ");
            cy_stc_ble_gapc_adv_report_param_t  *scanProgressParam = (cy_stc_ble_gapc_adv_report_param_t  *)eventParameter;
            printf("BD Addr = ");
            for(unsigned int i=0;i<CY_BLE_BD_ADDR_SIZE;i++)
                printf("%02X",scanProgressParam->peerBdAddr[i]);
            
            printf(" Length = %d ",scanProgressParam->dataLen);
            
            findAdvInfo(scanProgressParam->data,scanProgressParam->dataLen);
            if(currentAdvInfo.name != 0)
                printf("%.*s",currentAdvInfo.name_len,currentAdvInfo.name);
                
            printf("\r\n");

            // If the advertising packet has a service UUID in it... AND
            // the service UUID matches the LED Service... then try to connect
            if(currentAdvInfo.servUUID_len > 0 
                && memcmp(currentAdvInfo.serviceUUID,cy_ble_customCServ [CY_BLE_CUSTOMC_LED_SERVICE_INDEX].uuid,
                currentAdvInfo.servUUID_len) == 0)
            {
                printf("Found LED Service\r\n");
                cy_stc_ble_bd_addr_t connectAddr;
                memcpy(&connectAddr.bdAddr[0], &scanProgressParam->peerBdAddr[0] , CY_BLE_BD_ADDR_SIZE);
                connectAddr.type = scanProgressParam->peerAddrType;
                Cy_BLE_GAPC_ConnectDevice(&connectAddr,0);
                Cy_BLE_GAPC_StopScan();
            }
        break;

        case CY_BLE_EVT_GATT_CONNECT_IND:
            Cy_GPIO_Clr(LED9_PORT,LED9_NUM);
            printf("Made a connection starting service discovery\r\n");
            Cy_BLE_GATTC_StartDiscovery(cy_ble_connHandle[0]);
        break;
        
        case CY_BLE_EVT_GATTC_DISCOVERY_COMPLETE:
            printf("Discovery Complete\r\n" );
        break;
            
        case CY_BLE_EVT_GATTC_ERROR_RSP:
            printf("GATTC Error Response\r\n");
        break;
            
        case CY_BLE_EVT_GATTC_WRITE_RSP:
            printf("GATTC Write Succeeded\r\n");
        break;
         
        default:
        break;
    }
}

/*****************************************************************************\
 * Function:    bleTask
 * Input:       void * optional argument... not used
 * Returns:     void
 * Description: 
 *   This is the main ble processing task.  It loops repeatedly... processing
 *    ble events... then seeing if the user pressed + or -... if they did
 *    then it sends the new value of the brightness
\*****************************************************************************/
void bleTask(void *arg)
{
    (void)arg;
    printf("Started BLE Task\r\n");
    
    Cy_BLE_Start(genericEventHandler);
    
    char c;
    uint8_t brightness = 0;
    for(;;)
    {
        Cy_BLE_ProcessEvents();
        if(Cy_SCB_UART_GetNumInRxFifo(UART_1_HW))
        {
            c = getchar();
            switch(c)
            {
                case '+': // Increase the brightness and then send it via BLE - wrap around at 100
                    brightness = (brightness + 10) % 100; 
                    writeLed(brightness);
                break;
                case '-': // Decrease the brightness and then send it via BLE - wrap around at 100
                    brightness -= 10;
                    if(brightness > 100) brightness = 100;
                    writeLed(brightness);
                break;    
            }
        }
    }
}

/*****************************************************************************\
 * Function:   main
 * Input:      void
 * Returns:    int (this function acutally never returns...)
 * Description: Starts the system ... initalizes the BLE task.. then starts the RTOS
 *   
 *    
\*****************************************************************************/
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    UART_1_Start();
    setvbuf(stdin,0,_IONBF,0);
    xTaskCreate(bleTask,"bleTask",8*1024,0,1,0);
    vTaskStartScheduler();
}

/* [] END OF FILE */
