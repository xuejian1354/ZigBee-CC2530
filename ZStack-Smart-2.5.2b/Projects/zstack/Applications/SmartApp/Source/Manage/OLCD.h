/**************************************************************************************************
  Filename:       OLCD.h
  Revised:        $Date: 2015-06-10 09:56:24 -0800 (Wed, 10 May 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains the OLCD Operation definitions.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-06-10
**************************************************************************************************/

#ifndef OLCD_H
#define OLCD_H
/*********************************************************************
 * INCLUDES
 */
#include <hal_board.h>


/*********************************************************************
 * MACROS
 */
#define LCD_SCL P1_5       //SCLK  时钟 D0（SCLK）
#define LCD_SDA P1_6       //SDA   D1（MOSI） 数据
#define LCD_RST P1_7       //_RES  hardware reset   复位 
#define LCD_DC  P0_0        //A0  H/L 命令数据选通端，H：数据，L:命令

#define XLevelL        0x00
#define XLevelH        0x10
#define XLevel         ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column     128
#define Max_Row        64
#define Brightness     0xCF 
#define X_WIDTH        128
#define Y_WIDTH        64 


/*********************************************************************
 * FUNCTIONS
 */
extern void DelayMS(uint32 msec);
extern void LCD_DLY_ms(uint32 ms);
extern void LCD_WrDat(uint8 dat) ;
extern void LCD_WrCmd(uint8 cmd);
extern void LCD_Set_Pos(uint8 x, uint8 y);
extern void LCD_Fill(uint8 bmp_dat);
extern void LCD_CLS(void);
extern void LCD_Init(void);
extern void LCD_P6x8Str(uint8 x, uint8 y,uint8 ch[]);
extern void LCD_P8x16Str(uint8 x, uint8 y,uint8 ch[]);
extern void LCD_P16x16Ch(uint8 x, uint8 y, uint8 N);
extern void Draw_BMP(uint8 x0, uint8 y0, uint8 x1, uint8 y1,uint8 BMP[]);
#endif
