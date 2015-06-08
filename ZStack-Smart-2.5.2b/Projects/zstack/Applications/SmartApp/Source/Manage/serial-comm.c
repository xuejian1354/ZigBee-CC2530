/**************************************************************************************************
  Filename:       serial-com.c
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



