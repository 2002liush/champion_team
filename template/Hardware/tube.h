#ifndef __TUBE_H
#define __TUBE_H

#include "main.h"

#define u8  unsigned char 
#define u16 unsigned int 

#define SEG_A_Pin GPIO_PIN_7
#define SEG_A_GPIO_Port GPIOD
#define SEG_B_Pin GPIO_PIN_6
#define SEG_B_GPIO_Port GPIOD
#define SEG_C_Pin GPIO_PIN_5
#define SEG_C_GPIO_Port GPIOD
#define SEG_D_Pin GPIO_PIN_4
#define SEG_D_GPIO_Port GPIOD
#define SEG_E_Pin GPIO_PIN_3
#define SEG_E_GPIO_Port GPIOD
#define SEG_F_Pin GPIO_PIN_2
#define SEG_F_GPIO_Port GPIOD
#define SEG_G_Pin GPIO_PIN_1
#define SEG_G_GPIO_Port GPIOD
#define SEG_DP_Pin GPIO_PIN_0
#define SEG_DP_GPIO_Port GPIOD
#define SEG_CTRL4_Pin GPIO_PIN_12
#define SEG_CTRL4_GPIO_Port GPIOC
#define SEG_CTRL3_Pin GPIO_PIN_11
#define SEG_CTRL3_GPIO_Port GPIOC
#define SEG_CTRL2_Pin GPIO_PIN_10
#define SEG_CTRL2_GPIO_Port GPIOC
#define SEG_CTRL1_Pin GPIO_PIN_10
#define SEG_CTRL1_GPIO_Port GPIOA

void DisplayReset(void);
void DisplayFullReset(void);
void LedDisplay(int NumToDisplay);
void TimeDots(void);
void LED_WriteNumber(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int number);
void FourLed(int a,int b,int c,int d);
void FourLed_with_dots(int a,int b,int c,int d);
	


#endif
