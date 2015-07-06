/**************************************************************************************************
  Filename:       IRRelay_DeviceDataCtrl.c
  Revised:        $Date: 2014-12-01 14:27:34 -0800 (Thu, 01 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of infrared relay device data operations
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-05-20
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 

/*********************************************************************
 * MACROS
 */
#define IRRELAY_LEARN_METHOD	"LEA"
#define IRRELAY_SEND_METHOD		"SEN"

#define IRRELAY_LEARN_CMD	0x88
#define IRRELAY_SEND_CMD	0x86

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern uint8 *optData;
extern uint8 optDataLen;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
void IRRelayApp_TxHandler(uint8 txBuf[], uint8 txLen);

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
	SerialTx_Handler(SERIAL_COM_PORT, IRRelayApp_TxHandler);
}

void HalStatesInit(devStates_t status)
{}

void IRRelayApp_TxHandler(uint8 txBuf[], uint8 txLen)
{}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	uint8 ctrlData[5] = {0};
	if(osal_memcmp(IRRELAY_LEARN_METHOD, data, 3))
	{
		ctrlData[0] = IRRELAY_LEARN_CMD;
	}
	else if(osal_memcmp(IRRELAY_SEND_METHOD, data, 3))
	{
		ctrlData[0] = IRRELAY_SEND_CMD;
	}
	else
	{
		return -1;
	}

	ctrlData[1] = atox((uint8 *)(data+3), 2);
	ctrlData[4] = ctrlData[0] ^ ctrlData[1];
	HalUARTWrite(SERIAL_COM_PORT, ctrlData, 5);

	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	*dataLen = 0;
	return 0;
}

