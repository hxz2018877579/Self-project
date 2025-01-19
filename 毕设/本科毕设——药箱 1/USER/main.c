#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h" 
#include "oled.h"
#include "usart4.h"
#include "as608.h"
#include "usart.h"
#include "beep.h"
#include "syn6288.h"
#include "usart2.h"
#include "DCS.h"
#include "usart3.h"
#include "uart5.h"
#include "sim900a.h"
extern int hour,min,sec;
 int main(void)
 {	
	delay_init();	    	 //延时函数初始化
	
	LED_Init();		  	//初始化与LED连接的硬件接口
	TIM3_Int_Init(9999,7199);//定时器1s
	UART4_init(57600);  //指纹
	 USART3_init(9600);//蓝牙
	OLED_Init();//显示屏
	OLED_Clear(); 
	BEEP_Init();//蜂鸣器
	delay_ms(1000);
	
	
	 
   	while(1)
	{
		
		if(PS_Sta){  //指纹按下
     press_FR();/* 指纹检测 */
   }
		
		blueteeth(); //蓝牙定时  

	}
}

