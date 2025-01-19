#include "beep.h"
#include "delay.h"
#include "usart3.h"
#include "timer.h" 
//初始化PB8为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);	 //
 
 GPIO_ResetBits(GPIOA,GPIO_Pin_4);//输出0，关闭蜂鸣器输出

}

void scan_QR_codes_tone(void) //扫码音
{    
    BEEP=1;  
    delay_ms(200);
    BEEP=0;
}

void alert(void)
{
if((hour==t1)&&(min==t2)&&flag1)
{
	scan_QR_codes_tone();
}
else if((hour==t11)&&(min==t22)&&flag2)
{
	scan_QR_codes_tone();
}
else if((hour==t111)&&(min==t222)&&flag3)
{
	scan_QR_codes_tone();
}
else ;

}