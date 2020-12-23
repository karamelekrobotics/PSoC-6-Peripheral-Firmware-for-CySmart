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
#include <limits.h>
#include <stdio.h>
#include <string.h>

static uint8_t  in[1] = {0};
static uint8_t out[1] = {0};

/* Generic event handler BLE tarafından gelen olayları işler... */
void genericEventHandler(uint32_t event, void *eventParameter)
{
    switch(event)
    {
        case CY_BLE_EVT_STACK_ON:
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST,CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
            break;
        
            
        case CY_BLE_EVT_GATT_CONNECT_IND:

            break;
            
        case CY_BLE_EVT_GATTS_WRITE_REQ:
            {
            cy_stc_ble_gatts_write_cmd_req_param_t *writeReqParameters = (cy_stc_ble_gatts_write_cmd_req_param_t *) eventParameter;
            
            if (CY_BLE_LED_GREEN_CHAR_HANDLE == writeReqParameters->handleValPair.attrHandle)
            {
                in[0] = writeReqParameters->handleValPair.value.val[0];
                if (in[0] == 1) { 
                    Cy_GPIO_Write(GREEN_PORT,1,0);
                } else {
                    Cy_GPIO_Write(GREEN_PORT,1,1);
                }
                
            }
            
            Cy_BLE_GATTS_WriteRsp(writeReqParameters->connHandle);
            }
            break;
            
        default:
            break;
    }
}

/* Registered Application. */
void bleInterruptNotify()
{
    Cy_BLE_ProcessEvents();
}

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 
    
    /* UART hattı aktifleştirilir. */
    UART_1_Start();
    setvbuf(stdin,0,_IONBF,0);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    /* Start BLE */
    Cy_BLE_Start(genericEventHandler);
    
    while(Cy_BLE_GetState() != CY_BLE_STATE_ON)
    {
        Cy_BLE_ProcessEvents();
    }
    
    /* Register Application Callback */
    Cy_BLE_RegisterAppHostCallback(bleInterruptNotify);
    
    char c;
    
    for(;;)
    {
        /* Place your application code here. */
        cy_stc_ble_gatt_handle_value_pair_t serviceHandle;
        cy_stc_ble_gatt_value_t serviceData;
        
        serviceData.val = (uint8_t*)out;
        serviceData.len = 1;
                    
        serviceHandle.attrHandle = CY_BLE_LED_RED_CHAR_HANDLE;
        serviceHandle.value = serviceData;
                    
        Cy_BLE_GATTS_WriteAttributeValueLocal(&serviceHandle);
       
        /* Place your application code here. */
        if (Cy_SCB_UART_GetNumInRxFifo(UART_1_HW))
        {
            c = getchar();
            switch(c)
            {
                case '+':
                    out[0] = 1;
                    Cy_GPIO_Write(RED_PORT,3,0);
                break;
                case '-': 
                    out[0] = 0;
                    Cy_GPIO_Write(RED_PORT,3,1);
                break;
                default:
                break;
            }
            
        }
        CyDelay(100);
    }
}

/* [] END OF FILE */
