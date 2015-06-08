/**************************************************************************************************
  Filename:       serial-com.h
  Revised:        $Date: 2014-04-25 17:21:26 -0800 (Wed, 25 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Setting serial port.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2014-04-25
**************************************************************************************************/

#ifndef SERIAL_COM_H
#define SERIAL_COM_H


#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "string.h"
#include "hal_uart.h"

/*********************************************************************
 * CONSTANTS
 */
#ifndef SERIAL_COM_PORT
#if HAL_UART_DMA==2 || HAL_UART_ISR==2
#define SERIAL_COM_PORT  1
#else
#define SERIAL_COM_PORT  0
#endif
#endif

#if !defined( SERIAL_COM_BAUD )
//#define SERIAL_COM_BAUD  HAL_UART_BR_38400
#define SERIAL_COM_BAUD  HAL_UART_BR_115200
#endif

// When the Rx buf space is less than this threshold, invoke the Rx callback.
#if !defined( SERIAL_COM_THRESH )
#define SERIAL_COM_THRESH  64
#endif

#if !defined( SERIAL_COM_RX_SZ )
#define SERIAL_COM_RX_SZ  128
#endif

#if !defined( SERIAL_COM_TX_SZ )
#define SERIAL_COM_TX_SZ  128
#endif

// Millisecs of idle time after a byte is received before invoking Rx callback.
#if !defined( SERIAL_COM_IDLE )
#define SERIAL_COM_IDLE  6
#endif

// This is the max byte count per OTA message.
#if !defined( SERIAL_COM_TX_MAX )
#define SERIAL_COM_TX_MAX  80
#endif

#define SERIAL_COM_RSP_CNT  4

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */
extern void Serial_Init(halUARTCBack_t callBackFunc);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_COM_H */

