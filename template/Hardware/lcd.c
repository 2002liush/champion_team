#include "lcd.h"
#include "spi.h"
#include "gpio.h"
#include "main.h"

/****************************************************************************
* ��    �ƣ�void  SPIv_WriteData(u8 Data)
* ��    �ܣ�STM32_ģ��SPIдһ���ֽ����ݵײ㺯��
* ��ڲ�����Data
* ���ڲ�������
* ˵    ����STM32_ģ��SPI��дһ���ֽ����ݵײ㺯��
****************************************************************************/
//void  SPIv_WriteData(u8 Data)
//{
//	unsigned char i=0;
//	for(i=8;i>0;i--)
//	{
//	  if(Data&0x80)	
//		{
//			LCD_SDA_SET; //????
//		}
//    else 
//		{
//			LCD_SDA_CLR;
//		}	   
//      LCD_SCL_CLR;       
//      LCD_SCL_SET;
//      Data<<=1; 
//	}
//}

uint8_t SPI_WriteByte(uint8_t *TxData, uint16_t size)
{
	return HAL_SPI_Transmit(&hspi2,TxData,size,1000);
}

void  SPIv_WriteData(uint8_t Data)
{
	SPI_WriteByte(&Data, 1);
}


/****************************************************************************
* ��    �ƣ�Lcd_WriteIndex(u8 Index)
* ��    �ܣ���Һ����дһ��8λָ��
* ��ڲ�����Index   �Ĵ�����ַ
* ���ڲ�������
* ˵    ��������ǰ����ѡ�п��������ڲ�����
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
   LCD_CS_CLR;
   LCD_RS_CLR;

   SPIv_WriteData(Index);

   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�Lcd_WriteData(u8 Data)
* ��    �ܣ���Һ����дһ��8λ����
* ��ڲ�����dat     �Ĵ�������
* ���ڲ�������
* ˵    �����������ָ����ַд�����ݣ��ڲ�����
****************************************************************************/
void Lcd_WriteData(u8 Data)
{
   LCD_CS_CLR;
   LCD_RS_SET;

   SPIv_WriteData(Data);

   LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�void LCD_WriteReg(u8 Index,u16 Data)
* ��    �ܣ�д�Ĵ�������
* ��ڲ�����Index,Data
* ���ڲ�������
* ˵    ����������Ϊ��Ϻ�������Index��ַ�ļĴ���д��Dataֵ
****************************************************************************/
void LCD_WriteReg(u8 Index,u16 Data)
{
	Lcd_WriteIndex(Index);
  	Lcd_WriteData_16Bit(Data);
}

/****************************************************************************
* ��    �ƣ�void Lcd_WriteData_16Bit(u16 Data)
* ��    �ܣ���Һ����дһ��16λ����
* ��ڲ�����Data
* ���ڲ�������
* ˵    �����������ָ����ַд��һ��16λ����
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{	
	LCD_CS_CLR;
  LCD_RS_SET;
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);
	LCD_CS_SET;
}

/****************************************************************************
* ��    �ƣ�void Lcd_Reset(void)
* ��    �ܣ�Һ��Ӳ��λ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����Һ����ʼ��ǰ��ִ��һ�θ�λ����
****************************************************************************/
void Lcd_Reset(void)
{
	LCD_RST_CLR;
	HAL_Delay(100);
	LCD_RST_SET;
	HAL_Delay(50);
}

void Lcd_Init(void)
{	
	
	Lcd_Reset(); //Reset before LCD Init.

	//2.2inch TM2.2-G2.2 Init 20171020 
	Lcd_WriteIndex(0x11);  
	Lcd_WriteData(0x00); 

	Lcd_WriteIndex(0xCF);  
	Lcd_WriteData(0X00); 
	Lcd_WriteData(0XC1); 
	Lcd_WriteData(0X30);

	Lcd_WriteIndex(0xED);  
	Lcd_WriteData(0X64); 
	Lcd_WriteData(0X03); 
	Lcd_WriteData(0X12);
	Lcd_WriteData(0X81);

	Lcd_WriteIndex(0xE8);  
	Lcd_WriteData(0X85); 
	Lcd_WriteData(0X11); 
	Lcd_WriteData(0X78);

	Lcd_WriteIndex(0xF6);  
	Lcd_WriteData(0X01); 
	Lcd_WriteData(0X30); 
	Lcd_WriteData(0X00);

	Lcd_WriteIndex(0xCB);  
	Lcd_WriteData(0X39); 
	Lcd_WriteData(0X2C); 
	Lcd_WriteData(0X00);
	Lcd_WriteData(0X34);
	Lcd_WriteData(0X05);

	Lcd_WriteIndex(0xF7);  
	Lcd_WriteData(0X20); 

	Lcd_WriteIndex(0xEA);  
	Lcd_WriteData(0X00); 
	Lcd_WriteData(0X00); 

	Lcd_WriteIndex(0xC0);  
	Lcd_WriteData(0X20); 

	Lcd_WriteIndex(0xC1);  
	Lcd_WriteData(0X11); 

	Lcd_WriteIndex(0xC5);  
	Lcd_WriteData(0X31); 
	Lcd_WriteData(0X3C); 

	Lcd_WriteIndex(0xC7);  
	Lcd_WriteData(0XA9); 

	Lcd_WriteIndex(0x3A);  
	Lcd_WriteData(0X55); 
	
  Lcd_WriteIndex(0x36);  
	#if USE_HORIZONTAL
		 Lcd_WriteData(0xE8);//��������
	#else
		 Lcd_WriteData(0x48);//�������� 
	#endif

	Lcd_WriteIndex(0xB1);  
	Lcd_WriteData(0X00); 
	Lcd_WriteData(0X18); 

	Lcd_WriteIndex(0xB4);  
	Lcd_WriteData(0X00); 
	Lcd_WriteData(0X00); 

	Lcd_WriteIndex(0xF2);  
	Lcd_WriteData(0X00); 

	Lcd_WriteIndex(0x26);  
	Lcd_WriteData(0X01); 

	Lcd_WriteIndex(0xE0);  
	Lcd_WriteData(0X0F); 
	Lcd_WriteData(0X17); 
	Lcd_WriteData(0X14); 
	Lcd_WriteData(0X09); 
	Lcd_WriteData(0X0C); 
	Lcd_WriteData(0X06); 
	Lcd_WriteData(0X43); 
	Lcd_WriteData(0X75); 
	Lcd_WriteData(0X36); 
	Lcd_WriteData(0X08); 
	Lcd_WriteData(0X13); 
	Lcd_WriteData(0X05); 
	Lcd_WriteData(0X10); 
	Lcd_WriteData(0X0B); 
	Lcd_WriteData(0X08); 


	Lcd_WriteIndex(0xE1);  
	Lcd_WriteData(0X00); 
	Lcd_WriteData(0X1F); 
	Lcd_WriteData(0X23); 
	Lcd_WriteData(0X03); 
	Lcd_WriteData(0X0E); 
	Lcd_WriteData(0X04); 
	Lcd_WriteData(0X39); 
	Lcd_WriteData(0X25); 
	Lcd_WriteData(0X4D); 
	Lcd_WriteData(0X06); 
	Lcd_WriteData(0X0D); 
	Lcd_WriteData(0X0B); 
	Lcd_WriteData(0X33); 
	Lcd_WriteData(0X37); 
	Lcd_WriteData(0X0F); 

	Lcd_WriteIndex(0x29);  	
}



/*************************************************
��������LCD_Set_XY
���ܣ�����lcd��ʾ��ʼ��
��ڲ�����xy����
����ֵ����
*************************************************/
void Lcd_SetXY(u16 Xpos, u16 Ypos)
{	
	Lcd_WriteIndex(0x2A);
	Lcd_WriteData_16Bit(Xpos);
	Lcd_WriteIndex(0x2B);
	Lcd_WriteData_16Bit(Ypos);
	Lcd_WriteIndex(0x2c);	
} 
/*************************************************
��������LCD_Set_Region
���ܣ�����lcd��ʾ�����ڴ�����д�������Զ�����
��ڲ�����xy�����յ�
����ֵ����
*************************************************/
//������ʾ����
void Lcd_SetRegion(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{
	Lcd_WriteIndex(0x2A);
	Lcd_WriteData_16Bit(xStar);
	Lcd_WriteData_16Bit(xEnd);
	Lcd_WriteIndex(0x2B);
	Lcd_WriteData_16Bit(yStar);
	Lcd_WriteData_16Bit(yEnd);
	Lcd_WriteIndex(0x2c);
}

	
/*************************************************
��������LCD_DrawPoint
���ܣ���һ����
��ڲ�����xy�������ɫ����
����ֵ����
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetXY(x,y);
	Lcd_WriteData_16Bit(Data);

}    

/*************************************************
��������Lcd_Clear
���ܣ�ȫ����������
��ڲ����������ɫCOLOR
����ֵ����
*************************************************/
void Lcd_Clear(u16 Color)               
{	
   unsigned int i;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	 LCD_CS_CLR;
   LCD_RS_SET;	
   for(i=0;i<X_MAX_PIXEL*Y_MAX_PIXEL;i++)
   {	
//	  	Lcd_WriteData_16Bit(Color);

		SPIv_WriteData(Color>>8);
		SPIv_WriteData(Color);

   }   
		LCD_CS_SET;
}

