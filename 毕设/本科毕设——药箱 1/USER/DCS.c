#include "DCS.h"



void DCS_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
					
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//PB1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOb.1
	DCS=1;
}
 