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
	delay_init();	    	 //��ʱ������ʼ��
	
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	TIM3_Int_Init(9999,7199);//��ʱ��1s
	UART4_init(57600);  //ָ��
	 USART3_init(9600);//����
	OLED_Init();//��ʾ��
	OLED_Clear(); 
	BEEP_Init();//������
	delay_ms(1000);
	
	
	 
   	while(1)
	{
		
		if(PS_Sta){  //ָ�ư���
     press_FR();/* ָ�Ƽ�� */
   }
		
		blueteeth(); //������ʱ  

	}
}

