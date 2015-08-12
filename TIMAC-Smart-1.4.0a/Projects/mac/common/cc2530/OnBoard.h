/**************************************************************************************************
  Filename:       OnBoard.h
  Revised:        $Date: 2011-09-29 14:34:28 -0700 (Thu, 29 Sep 2011) $
  Revision:       $Revision: 27760 $

  Description:    Defines stuff for EVALuation boards
                  This file targets the Texas Instruments CC2530EB


  Copyright 2008-2011 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef ONBOARD_H
#define ONBOARD_H

#include "hal_mcu.h"
#include "hal_sleep.h"

/*********************************************************************
 */
// Internal (MCU) RAM addresses
#define MCU_RAM_BEG 0x0100
#define MCU_RAM_END RAMEND
#define MCU_RAM_LEN (MCU_RAM_END - MCU_RAM_BEG + 1)

// Internal (MCU) heap size
#if !defined( INT_HEAP_LEN )
  #define INT_HEAP_LEN  2048  // 1.00K
#endif

// Memory Allocation Heap
#if defined( EXTERNAL_RAM )
  #define MAXMEMHEAP EXT_RAM_LEN   // Typically, 32K
#else
  #define MAXMEMHEAP INT_HEAP_LEN  // Typically, 0.70-1.50K
#endif

// Timer clock and power-saving definitions
#define TIMER_DECR_TIME    1  // 1ms - has to be matched with TC_OCC
#define RETUNE_THRESHOLD   1  // Threshold for power saving algorithm

/* OSAL timer defines */
#define TICK_TIME   1000   /* Timer per tick - in micro-sec */
#define TICK_COUNT  1

#ifndef _WIN32
extern void _itoa(uint16 num, uint8 *buf, uint8 radix);
#endif

#ifndef RAMEND
#define RAMEND 0x1000
#endif

/* Tx and Rx buffer size defines used by SPIMgr.c */
#define MT_UART_THRESHOLD    5
#define MT_UART_TX_BUFF_MAX  180
#define MT_UART_RX_BUFF_MAX  128
#define MT_UART_IDLE_TIMEOUT 5

/* system restart and boot loader used from MT.c */
// Restart system from absolute beginning
// Disables interrupts, forces WatchDog reset
#define SystemReset()       \
{                           \
  HAL_DISABLE_INTERRUPTS(); \
  HAL_SYSTEM_RESET();       \
}

#define SystemResetSoft()  Onboard_soft_reset()

/* Reset reason for reset indication */
#define ResetReason() ((SLEEPSTA >> 3) & 0x03)

/* port definition stuff used by MT */
#if defined (ZAPP_P1)
  #define ZAPP_PORT HAL_UART_PORT_0 //SERIAL_PORT1
#elif defined (ZAPP_P2)
  #define ZAPP_PORT HAL_UART_PORT_1 //SERIAL_PORT2
#else
  #undef ZAPP_PORT
#endif
#if defined (ZTOOL_P1)
  #define ZTOOL_PORT HAL_UART_PORT_0 //SERIAL_PORT1
#elif defined (ZTOOL_P2)
  #define ZTOOL_PORT HAL_UART_PORT_1 //SERIAL_PORT2
#else
  #undef ZTOOL_PORT
#endif

/* sleep macros required by OSAL_PwrMgr.c */
#define SLEEP_DEEP                  0             /* value not used */
#define SLEEP_LITE                  0             /* value not used */
#define MIN_SLEEP_TIME              14            /* minimum time to sleep */
#define OSAL_SET_CPU_INTO_SLEEP(m)  halSleep(m)   /* interface to HAL sleep */


#if defined CC253X_MACNP
#define ZNP_CFG1_UART                       0
#define ZNP_CFG1_SPI                        1
extern uint8 znpCfg1;
#endif

/* used by MT.c */
uint8 OnBoard_SendKeys( uint8 keys, uint8 state );

/*
 * Board specific random number generator
 */
extern uint16 Onboard_rand( void );

/*
 * Get elapsed timer clock counts
 *   reset: reset count register if TRUE
 */
extern uint32 TimerElapsed( void );

/*
* Board specific soft reset.
*/
extern __near_func void Onboard_soft_reset( void );

/*********************************************************************
 */

#endif
