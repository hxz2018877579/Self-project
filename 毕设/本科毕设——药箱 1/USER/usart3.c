#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"

#include "led.h"
extern unsigned char esp8266_buf[256];
extern unsigned short esp8266_cnt, esp8266_cntPre;

//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//发送缓冲,最大USART3_MAX_SEND_LEN字节
u8 len1;	
vu16 USART3_RX_STA=0;   	
      u8 chuank[7];
u8 tt,t1=0,t2=0,t3=0,t0=0,t11=0,t22=0,t33=0,t111=0,t222=0,t333=0;
//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
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
	usart_initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	usart_initstruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
	usart_initstruct.USART_Parity = USART_Parity_No;									//无校验
	usart_initstruct.USART_StopBits = USART_StopBits_1;								//1位停止位
	usart_initstruct.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(USART3, &usart_initstruct);
	
	USART_Cmd(USART3, ENABLE);														//使能串口
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);									//使能接收中断
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	nvic_initstruct.NVIC_IRQChannel = USART3_IRQn;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic_initstruct);

}

//==========================================================
//	函数名称：	USART3_IRQHandler
//
//	函数功能：	串口3收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收中断
	{
		u8 Res;
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x21 0x23结尾,即!#)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
			
		if((USART3_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART3_RX_STA&0x4000)//接收到了0x21
				{
				if(Res!=0x23)USART3_RX_STA=0;//接收错误,重新开始
				else USART3_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X21
				{	
				if(Res==0x21)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_MAX_RECV_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}  	
		/*
		if((USART3_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
				else USART3_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_MAX_RECV_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
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
			len1=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
			
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


////串口3,printf 函数
////确保一次发送数据不超过USART3_MAX_SEND_LEN字节
//void u3_printf(char* fmt,...)  
//{  
//	u16 i,j; 
//	va_list ap; 
//	va_start(ap,fmt);
//	vsprintf((char*)USART3_TX_BUF,fmt,ap);
//	va_end(ap);
//	i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
//	for(j=0;j<i;j++)							//循环发送数据
//	{
//	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
//	  USART_SendData(USART3,USART3_TX_BUF[j]); 
//	} 
//}

 
