#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"

#include "led.h"
extern unsigned char esp8266_buf[256];
extern unsigned short esp8266_cnt, esp8266_cntPre;

//���ڽ��ջ����� 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
u8 len1;	
vu16 USART3_RX_STA=0;   	
      u8 chuank[7];
u8 tt,t1=0,t2=0,t3=0,t0=0,t11=0,t22=0,t33=0,t111=0,t222=0,t333=0;
//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART3_init(u32 bound)
{  

	GPIO_InitTypeDef gpio_initstruct;
	USART_InitTypeDef usart_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	//PB11  TXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_10;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_initstruct);
	
	//PB10	 RXD
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_11;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio_initstruct);
	
	usart_initstruct.USART_BaudRate = bound;
	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usart_initstruct.USART_Parity = USART_Parity_No;									//��У��
	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART3, &usart_initstruct);
	
	USART_Cmd(USART3, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	nvic_initstruct.NVIC_IRQChannel = USART3_IRQn;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_initstruct);

}

//==========================================================
//	�������ƣ�	USART3_IRQHandler
//
//	�������ܣ�	����3�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //�����ж�
	{
		u8 Res;
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x21 0x23��β,��!#)
		{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
			
		if((USART3_RX_STA&0x8000)==0)//����δ���
			{
			if(USART3_RX_STA&0x4000)//���յ���0x21
				{
				if(Res!=0x23)USART3_RX_STA=0;//���մ���,���¿�ʼ
				else USART3_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X21
				{	
				if(Res==0x21)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_MAX_RECV_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}  	
		/*
		if((USART3_RX_STA&0x8000)==0)//����δ���
			{
			if(USART3_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
				else USART3_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_MAX_RECV_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}  
*/			
		//USART_ClearFlag(USART3, USART_FLAG_RXNE);
	}

}
}	
void blueteeth(void)
{

if(USART3_RX_STA&0x8000)
		{					   
			len1=USART3_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			
			for(tt=0;tt<len1;tt++)
			{
				chuank[tt]=USART3_RX_BUF[tt];
				
			}
			
			USART3_RX_STA=0;
		
		t0=chuank[6]-48;
		switch(t0){
			case 1:
		          t1=(chuank[0]-48)*10+(chuank[1]-48);
		          t2=(chuank[2]-48)*10+(chuank[3]-48);
		          t3=(chuank[4]-48)*10+(chuank[5]-48);
			break;
			
			case 2:
		          t11=(chuank[0]-48)*10+(chuank[1]-48);
		          t22=(chuank[2]-48)*10+(chuank[3]-48);
		          t33=(chuank[4]-48)*10+(chuank[5]-48);
			break;
			
			case 3:
		          t111=(chuank[0]-48)*10+(chuank[1]-48);
		          t222=(chuank[2]-48)*10+(chuank[3]-48);
		          t333=(chuank[4]-48)*10+(chuank[5]-48);
			break;
			
			case 4:
		          hour=(chuank[0]-48)*10+(chuank[1]-48);
		          min=(chuank[2]-48)*10+(chuank[3]-48);
		          sec=(chuank[4]-48)*10+(chuank[5]-48);
			break;
			
		
		}
			
			
		}
		 memset(chuank, 0, sizeof chuank); 


}


////����3,printf ����
////ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
//void u3_printf(char* fmt,...)  
//{  
//	u16 i,j; 
//	va_list ap; 
//	va_start(ap,fmt);
//	vsprintf((char*)USART3_TX_BUF,fmt,ap);
//	va_end(ap);
//	i=strlen((const char*)USART3_TX_BUF);		//�˴η������ݵĳ���
//	for(j=0;j<i;j++)							//ѭ����������
//	{
//	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
//	  USART_SendData(USART3,USART3_TX_BUF[j]); 
//	} 
//}

 
