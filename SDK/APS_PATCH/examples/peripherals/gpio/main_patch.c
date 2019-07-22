/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Opulinks Technology Ltd. (C) 2018
******************************************************************************/

/******************************************************************************
*  Filename:
*  ---------
*  main_patch.c
*
*  Project:
*  --------
*  OPL1000 Project - the main patch implement file
*
*  Description:
*  ------------
*  This implement file is include the main patch function and api.
*
*  Author:
*  -------
*  SH SDK 
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file
/******************************************************************************
*  Test code brief
*  These examples show how to configure gpio settings and use gpio driver APIs.
*
*  gpio_int_test() is an example that generate the output pulses from one gpio
*  to another, then the input side will be triggered the interrupt.
*  - port: GPIO2(io2), GPIO3(io3), GPIO4(io4), GPIO5(io5)
*--------------2019/1/9		GPIO6(io6), GPIO7(io7), GPIO16(io16), GPIO17(io17)
*  - interrupt: on
*  - connection:
*    connect GPIO2 with GPIO4
*    connect GPIO3 with GPIO5
*  - behavior:
*    GPIO2: output
*    GPIO3: output
*    GPIO4: input, interrupt from rising edge and falling edge
*    GPIO5: input, interrupt from rising edge
******************************************************************************/


// Sec 1: Include File
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sys_init.h"
#include "sys_init_patch.h"
#include "hal_system.h"
#include "mw_fim.h"
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "Hal_pinmux_gpio.h"
#include "hal_pin.h"
#include "hal_pin_def.h"
#include "hal_pin_config_project.h"
#include "at_cmd_common_patch.h"
//#include "hal_wdt.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the number of elements in the message queue
#define APP_MESSAGE_Q_SIZE  16


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
// the content of message queue
typedef struct
{
    uint32_t ulGpioIdx;
    uint32_t ulLevel;
} S_MessageQ;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype
typedef void (*T_Main_AppInit_fp)(void);
extern T_Main_AppInit_fp Main_AppInit;


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable
static osThreadId g_tAppThread_1;
static osThreadId g_tAppThread_2;
static osMessageQId g_tAppMessageQ;
static osPoolId g_tAppMemPoolId;


// Sec 7: declaration of static function prototype
void __Patch_EntryPoint(void) __attribute__((section("ENTRY_POINT")));
void __Patch_EntryPoint(void) __attribute__((used));
static void Main_PinMuxUpdate(void);
static void Main_FlashLayoutUpdate(void);
void Main_AppInit_patch(void);
static void Main_AppThread_1(void *argu);
static void Main_AppThread_2(void *argu);
static osStatus Main_AppMessageQSend(S_MessageQ *ptMsg);
static void gpio_int_test(void);
static void gpio_int_callback(E_GpioIdx_t tGpioIdx);


static void Main_MiscModulesInit(void);

/***********
C Functions
***********/
// Sec 8: C Functions

/*************************************************************************
* FUNCTION:
*   __Patch_EntryPoint
*
* DESCRIPTION:
*   the entry point of SW patch
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void __Patch_EntryPoint(void)
{
    // don't remove this code
    SysInit_EntryPoint();
    
    // update the pin mux
    Hal_SysPinMuxAppInit = Main_PinMuxUpdate;
    
    // update the flash layout
    MwFim_FlashLayoutUpdate = Main_FlashLayoutUpdate;

    // the initial of driver part for cold and warm boot
    Sys_MiscModulesInit = Main_MiscModulesInit;

    
    Sys_SetUnsuedSramEndBound(0x440000);
    // application init
    Sys_AppInit = Main_AppInit_patch;
}


/*************************************************************************
* FUNCTION:
*   Main_PinMuxUpdate
*
* DESCRIPTION:
*   update the pinmux setting 
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_PinMuxUpdate(void)
{
    Hal_Pin_ConfigSet(0, HAL_PIN_TYPE_IO_0, HAL_PIN_DRIVING_IO_0);
    Hal_Pin_ConfigSet(1, HAL_PIN_TYPE_IO_1, HAL_PIN_DRIVING_IO_1);
    Hal_Pin_ConfigSet(2, HAL_PIN_TYPE_IO_2, HAL_PIN_DRIVING_IO_2);
    Hal_Pin_ConfigSet(3, HAL_PIN_TYPE_IO_3, HAL_PIN_DRIVING_IO_3);
    Hal_Pin_ConfigSet(4, HAL_PIN_TYPE_IO_4, HAL_PIN_DRIVING_IO_4);
    Hal_Pin_ConfigSet(5, HAL_PIN_TYPE_IO_5, HAL_PIN_DRIVING_IO_5);
    Hal_Pin_ConfigSet(6, HAL_PIN_TYPE_IO_6, HAL_PIN_DRIVING_IO_6);
    Hal_Pin_ConfigSet(7, HAL_PIN_TYPE_IO_7, HAL_PIN_DRIVING_IO_7);
    Hal_Pin_ConfigSet(8, HAL_PIN_TYPE_IO_8, HAL_PIN_DRIVING_IO_8);
    Hal_Pin_ConfigSet(9, HAL_PIN_TYPE_IO_9, HAL_PIN_DRIVING_IO_9);
    Hal_Pin_ConfigSet(10, HAL_PIN_TYPE_IO_10, HAL_PIN_DRIVING_IO_10);
    Hal_Pin_ConfigSet(11, HAL_PIN_TYPE_IO_11, HAL_PIN_DRIVING_IO_11);
    Hal_Pin_ConfigSet(12, HAL_PIN_TYPE_IO_12, HAL_PIN_DRIVING_IO_12);
    Hal_Pin_ConfigSet(13, HAL_PIN_TYPE_IO_13, HAL_PIN_DRIVING_IO_13);
    Hal_Pin_ConfigSet(14, HAL_PIN_TYPE_IO_14, HAL_PIN_DRIVING_IO_14);
    Hal_Pin_ConfigSet(15, HAL_PIN_TYPE_IO_15, HAL_PIN_DRIVING_IO_15);
    Hal_Pin_ConfigSet(16, HAL_PIN_TYPE_IO_16, HAL_PIN_DRIVING_IO_16);
    Hal_Pin_ConfigSet(17, HAL_PIN_TYPE_IO_17, HAL_PIN_DRIVING_IO_17);
    Hal_Pin_ConfigSet(18, HAL_PIN_TYPE_IO_18, HAL_PIN_DRIVING_IO_18);
    Hal_Pin_ConfigSet(19, HAL_PIN_TYPE_IO_19, HAL_PIN_DRIVING_IO_19);
    Hal_Pin_ConfigSet(20, HAL_PIN_TYPE_IO_20, HAL_PIN_DRIVING_IO_20);
    Hal_Pin_ConfigSet(21, HAL_PIN_TYPE_IO_21, HAL_PIN_DRIVING_IO_21);
    Hal_Pin_ConfigSet(22, HAL_PIN_TYPE_IO_22, HAL_PIN_DRIVING_IO_22);
    Hal_Pin_ConfigSet(23, HAL_PIN_TYPE_IO_23, HAL_PIN_DRIVING_IO_23);
    at_io01_uart_mode_set(HAL_PIN_0_1_UART_MODE);
}

/*************************************************************************
* FUNCTION:
*   Main_FlashLayoutUpdate
*
* DESCRIPTION:
*   update the flash layout
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_FlashLayoutUpdate(void)
{
    // update here
}
/*************************************************************************
* FUNCTION:
*   Main_MiscModulesInit
*
* DESCRIPTION:
*   the initial of driver part for cold and warm boot
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_MiscModulesInit(void)
{
	  //Hal_Wdt_Stop();   //disable watchdog here.
}

/*************************************************************************
* FUNCTION:
*   Main_AppInit_patch
*
* DESCRIPTION:
*   the initial of application
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_AppInit_patch(void)
{
    osThreadDef_t tThreadDef;
    osMessageQDef_t tMessageDef;
    osPoolDef_t tMemPoolDef;
    
    // do the gpio_int_test
    gpio_int_test();
    
    // create the thread for AppThread_1
    tThreadDef.name = "App_1";
    tThreadDef.pthread = Main_AppThread_1;
    tThreadDef.tpriority = OS_TASK_PRIORITY_APP;        // osPriorityNormal
    tThreadDef.instances = 0;                           // reserved, it is no used
    tThreadDef.stacksize = OS_TASK_STACK_SIZE_APP;      // (512), unit: 4-byte, the size is 512*4 bytes
    g_tAppThread_1 = osThreadCreate(&tThreadDef, NULL);
    if (g_tAppThread_1 == NULL)
    {
        printf("To create the thread for AppThread_1 is fail.\n");
    }
    
    // create the thread for AppThread_2
    tThreadDef.name = "App_2";
    tThreadDef.pthread = Main_AppThread_2;
    tThreadDef.tpriority = OS_TASK_PRIORITY_APP;        // osPriorityNormal
    tThreadDef.instances = 0;                           // reserved, it is no used
    tThreadDef.stacksize = OS_TASK_STACK_SIZE_APP;      // (512), unit: 4-byte, the size is 512*4 bytes
    g_tAppThread_2 = osThreadCreate(&tThreadDef, NULL);
    if (g_tAppThread_2 == NULL)
    {
        printf("To create the thread for AppThread_2 is fail.\n");
    }
    
    // create the message queue for AppMessageQ
    tMessageDef.queue_sz = APP_MESSAGE_Q_SIZE;          // number of elements in the queue
    tMessageDef.item_sz = sizeof(S_MessageQ);           // size of an item
    tMessageDef.pool = NULL;                            // reserved, it is no used
    g_tAppMessageQ = osMessageCreate(&tMessageDef, g_tAppThread_1);
    if (g_tAppMessageQ == NULL)
    {
        printf("To create the message queue for AppMessageQ is fail.\n");
    }
    
    // create the memory pool for AppMessageQ
    tMemPoolDef.pool_sz = APP_MESSAGE_Q_SIZE;           // number of items (elements) in the pool
    tMemPoolDef.item_sz = sizeof(S_MessageQ);           // size of an item
    tMemPoolDef.pool = NULL;                            // reserved, it is no used
    g_tAppMemPoolId = osPoolCreate(&tMemPoolDef);
    if (g_tAppMemPoolId == NULL)
    {
        printf("To create the memory pool for AppMessageQ is fail.\n");
    }
}

/*************************************************************************
* FUNCTION:
*   Main_AppThread_1
*
* DESCRIPTION:
*   the application thread 1
*
* PARAMETERS
*   1. argu     : [In] the input argument
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_AppThread_1(void *argu)
{
    osEvent tEvent;
    S_MessageQ *ptMsgPool;
    
    while (1)
    {
        // receive the message from AppMessageQ
        tEvent = osMessageGet(g_tAppMessageQ, osWaitForever);
        if (tEvent.status != osEventMessage)
        {
            printf("To receive the message from AppMessageQ is fail.\n");
            continue;
        }
        
        // get the content of message
        ptMsgPool = (S_MessageQ *)tEvent.value.p;
        
        // output the content of message
        printf("GPIO[%d] Level[%d]\n", ptMsgPool->ulGpioIdx, ptMsgPool->ulLevel);
        
        // free the memory pool
        osPoolFree(g_tAppMemPoolId, ptMsgPool);
    }
}

/*************************************************************************
* FUNCTION:
*   Main_AppThread_2
*
* DESCRIPTION:
*   the application thread 2
*
* PARAMETERS
*   1. argu     : [In] the input argument
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_AppThread_2(void *argu)
{
    uint32_t ulCount = 1;
    E_GpioLevel_t io_level;
    E_GpioIdx_t gpioIdx1,gpioIdx2,gpioIdx3;
    T_OPL1000_Gpio *gpioPtr;

    gpioPtr = &OPL1000_periph.gpio[0];
    gpioIdx1 = Hal_Pinmux_GetIO(gpioPtr->pin);
    gpioPtr = &OPL1000_periph.gpio[1];
    gpioIdx2 = Hal_Pinmux_GetIO(gpioPtr->pin);
    gpioPtr = &OPL1000_periph.gpio[4];
    gpioIdx3 = Hal_Pinmux_GetIO(gpioPtr->pin);
	
    while (1)
    {
        printf("Count = %d\n", ulCount);
        io_level = (E_GpioLevel_t)(ulCount % 2);
        if(io_level == GPIO_LEVEL_LOW)
            printf("GPIO6 is set to Low \n");
        else
            printf("GPIO6 is set to High \n");
        Hal_Vic_GpioOutput(gpioIdx1, io_level);
        
        io_level = (E_GpioLevel_t)!io_level;
        if(io_level == GPIO_LEVEL_LOW)
          printf("GPIO7 is set to Low \n");
        else
          printf("GPIO7 is set to High \n");
        Hal_Vic_GpioOutput(gpioIdx2, io_level);        			

        io_level = (E_GpioLevel_t)(ulCount % 2);
        if(io_level == GPIO_LEVEL_LOW)
          printf("D1 LED is turned off \n");
        else
          printf("D1 LED is light up \n");
        Hal_Vic_GpioOutput(gpioIdx3, io_level);   
				
        ulCount++;
        
        osDelay(1000);      // delay 1000 ms
    }
}

/*************************************************************************
* FUNCTION:
*   Main_AppMessageQSend
*
* DESCRIPTION:
*   send the message into AppMessageQ
*
* PARAMETERS
*   1. ptMsg    : [In] the pointer of message content
*
* RETURNS
*   osOK        : successful
*   osErrorOS   : fail
*
*************************************************************************/
static osStatus Main_AppMessageQSend(S_MessageQ *ptMsg)
{
    osStatus tRet = osErrorOS;
    S_MessageQ *ptMsgPool;
    
    // allocate the memory pool
    ptMsgPool = (S_MessageQ *)osPoolCAlloc(g_tAppMemPoolId);
    if (ptMsgPool == NULL)
    {
        printf("To allocate the memory pool for AppMessageQ is fail.\n");
        goto done;
    }
    
    // copy the message content
    memcpy(ptMsgPool, ptMsg, sizeof(S_MessageQ));
    
    // send the message
    if (osOK != osMessagePut(g_tAppMessageQ, (uint32_t)ptMsgPool, osWaitForever))
    {
        printf("To send the message for AppMessageQ is fail.\n");
        
        // free the memory pool
        osPoolFree(g_tAppMemPoolId, ptMsgPool);
        goto done;
    }
    
    tRet = osOK;

done:
    return tRet;
}

/*************************************************************************
* FUNCTION:
*   gpio_int_test
*
* DESCRIPTION:
*   an example that generate the output pulses from one gpio to another,
*   then the input side will be triggered the interrupt.
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void gpio_int_test(void)
{
    E_GpioIdx_t gpioIdx1,gpioIdx2;   
    T_OPL1000_Gpio *gpioPtr;
 
    // gpio[2] and gpio[3] are defined as input pin 
    gpioPtr = &OPL1000_periph.gpio[2];
    gpioIdx1 = Hal_Pinmux_GetIO(gpioPtr->pin);
    
    gpioPtr = &OPL1000_periph.gpio[3];
    gpioIdx2 = Hal_Pinmux_GetIO(gpioPtr->pin);
    
    // correspond to OPL1000_periph.gpio[2]
    Hal_Vic_GpioDirection(gpioIdx1, GPIO_INPUT);
    Hal_Vic_GpioCallBackFuncSet(gpioIdx1, gpio_int_callback);
    Hal_Vic_GpioIntTypeSel(gpioIdx1, INT_TYPE_FALLING_EDGE);
    Hal_Vic_GpioIntInv(gpioIdx1, 0);
    Hal_Vic_GpioIntMask(gpioIdx1, 0);
    Hal_Vic_GpioIntEn(gpioIdx1, 1);
    
    // GPIO5
    Hal_Vic_GpioDirection(gpioIdx2, GPIO_INPUT);
    Hal_Vic_GpioCallBackFuncSet(gpioIdx2, gpio_int_callback);
    Hal_Vic_GpioIntTypeSel(gpioIdx2, INT_TYPE_RISING_EDGE);
    Hal_Vic_GpioIntInv(gpioIdx2, 0);
    Hal_Vic_GpioIntMask(gpioIdx2, 0);
    Hal_Vic_GpioIntEn(gpioIdx2, 1);
}

/*************************************************************************
* FUNCTION:
*   gpio_int_callback
*
* DESCRIPTION:
*   an example that generate the output pulses from one gpio to another,
*   then the input side will be triggered the interrupt.
*
* PARAMETERS
*   tGpioIdx    : Index of call-back GPIO
*
* RETURNS
*   none
*
*************************************************************************/
static void gpio_int_callback(E_GpioIdx_t tGpioIdx)
{
    S_MessageQ tMsg;
    
    // send the result to AppThread_1
    tMsg.ulGpioIdx = tGpioIdx;
    tMsg.ulLevel = Hal_Vic_GpioInput(tGpioIdx);
    Main_AppMessageQSend(&tMsg);
}
