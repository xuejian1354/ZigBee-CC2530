/**************************************************************************************************
  Filename:       incode.c
  Revised:        $Date: 2014-12-03 09:38:12 -0800 (Wed, 03 dec 2014) $
  Revision:       $Revision: 29217 $

  Description:    This file contains incode application interface definitions.
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2014-12-01
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "incode.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static uint8 ctox(uint8 src);
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/***************************************************************
                     二进制转化为十六进制函数
*****************************************************************/
void incode_2_to_16(uint8 *dest ,uint8 *src,uint8 len)
{
     if(len <= 0 || dest==NULL || src == NULL)
       return ;
     uint8* xad;   
     uint8 i=0;
     uint8 ch; 
     xad=src+len-1;  
     for(i=0;i<len;i++,xad--)  
     {      //高位
            ch= (*xad >> 4) & 0x0F;     
            dest[i<<1]=ch+((ch<10)?'0':'7');
            //低位
            ch= *xad & 0x0F;  
            dest[(i<<1)+1]=ch+((ch<10)?'0':'7');  
            
      } 
}


//16 进制字符转化为十进制
uint8 ctox(uint8 src)
{
	uint8 temp = 0;

	if(src>='0' && src<='9')
		temp = src-'0';
	else if(src>='a' && src<='f')
		temp = src-'a'+10;
	else if(src>='A' && src<='F')
		temp = src-'A'+10;

	return temp;
}


//16 进制短地址字符串转化为短地址
void incode_16_to_2(uint16 *dest , uint8 *src,uint8 len)
{
    if(len <= 0 || src == NULL)
       return ;
    uint8 i  = 0;
    uint8 temp = 0;
    for(i = 0;i< len;i++){
        temp = src[i]-'0';
        temp = (temp<=9)?temp:(temp-7);
        *dest += (temp << ((len-1-i)<<2));
    }
}


uint8 atox(uint8 *src, uint8 len)
{
	uint8 temp=0, i=0, length;
	length = len;

	while(length--)
	{
		temp = ctox(*(src+i)) + temp*0x10;
		i++;
	}

	return temp;
}
 
