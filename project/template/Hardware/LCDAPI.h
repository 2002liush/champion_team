#ifndef __LCDAPI_H
#define __LCDAPI_H
#include "main.h"

#define u8 unsigned char
#define u16 unsigned int

void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);  
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);
void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s);
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num);

void LCD_DrawPoint(u16 x,u16 y);
unsigned long oled_pow(u8 m,u8 n);
void LCD_Showdecimal(u8 x,u8 y,float num,u8 z_len,u8 f_len,u8 size2);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode);
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
void showhanzi(unsigned int x,unsigned int y,unsigned char index);
void showimage(const unsigned char *p);

void LCD_ShowNum(u16 x,u16 y,unsigned long num,u8 len);
void show_str(u16 x, u16 y, u8 str[],u8 len,u8 dist);
//void picture();

#endif
