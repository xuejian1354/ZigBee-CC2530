/**************************************************************************************************
  Filename:       serial-comm.c
  Revised:        $Date: 2014-04-25 17:25:52 -0800 (Wed, 25 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Analysis frame format.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2014-04-25
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "serial-comm.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

#ifndef HAL_UART01_BOTH
void Serial_Init(halUARTCBack_t callBackFunc)
{
	halUARTCfg_t uartConfig;

	uartConfig.configured           = TRUE;
	uartConfig.baudRate             = SERIAL_COM_BAUD;
	uartConfig.flowControl          = HAL_UART_FLOW_OFF;
	uartConfig.flowControlThreshold = SERIAL_COM_THRESH;
	uartConfig.rx.maxBufSize        = SERIAL_COM_RX_SZ;
	uartConfig.tx.maxBufSize        = SERIAL_COM_TX_SZ;
	uartConfig.idleTimeout          = SERIAL_COM_IDLE;
	uartConfig.intEnable            = TRUE;
	uartConfig.callBackFunc         = callBackFunc;
	
	HalUARTOpen (SERIAL_COM_PORT, &uartConfig);
}
#else
void Serial_Init(halUARTCBack_t callBackFunc)
{
	halUARTCfg_t uartConfig0, uartConfig1;

	uartConfig0.configured           = TRUE;
	uartConfig0.baudRate             = SERIAL_COM_BAUD0;
	uartConfig0.flowControl          = HAL_UART_FLOW_OFF;
	uartConfig0.flowControlThreshold = SERIAL_COM_THRESH;
	uartConfig0.rx.maxBufSize        = SERIAL_COM_RX_SZ;
	uartConfig0.tx.maxBufSize        = SERIAL_COM_TX_SZ;
	uartConfig0.idleTimeout          = SERIAL_COM_IDLE;
	uartConfig0.intEnable            = TRUE;
	uartConfig0.callBackFunc         = callBackFunc;

	uartConfig1.configured           = TRUE;
	uartConfig1.baudRate             = SERIAL_COM_BAUD1;
	uartConfig1.flowControl          = HAL_UART_FLOW_OFF;
	uartConfig1.flowControlThreshold = SERIAL_COM_THRESH;
	uartConfig1.rx.maxBufSize        = SERIAL_COM_RX_SZ;
	uartConfig1.tx.maxBufSize        = SERIAL_COM_TX_SZ;
	uartConfig1.idleTimeout          = SERIAL_COM_IDLE;
	uartConfig1.intEnable            = TRUE;
	uartConfig1.callBackFunc         = callBackFunc;
	
	HalUARTOpen (SERIAL_COM_PORT0, &uartConfig0);
	HalUARTOpen (SERIAL_COM_PORT1, &uartConfig1);
}
#endif