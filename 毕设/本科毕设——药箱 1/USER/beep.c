#include "beep.h"
#include "delay.h"
#include "usart3.h"
#include "timer.h" 
//��ʼ��PB8Ϊ�����.��ʹ������ڵ�ʱ��		    
//��������ʼ��
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //�ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);	 //
 
 GPIO_ResetBits(GPIOA,GPIO_Pin_4);//���0���رշ��������

}

void scan_QR_codes_tone(void) //ɨ����
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