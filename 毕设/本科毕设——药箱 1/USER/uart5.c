#include "uart5.h"
#include "sys.h"
 #include "delay.h"  

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif


void uart5_init(u32 bound)
{
	//GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
   
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //ʹ��UART5ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
//UART5_TX   GPIOC.12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC12��RXD
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.12
    
  //UART5_RX   GPIOD.2��ʼ��0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PD2��TXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOD2  
  //Uart5 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn ;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //IRQͨ��ʹ��
   NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
   //UART ��ʼ������
   USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
   USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
   USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
   USART_Init(UART5, &USART_InitStructure); //��ʼ������4
   USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
   USART_Cmd(UART5, ENABLE); 
//  //GPIO�˿�����
//    GPIO_InitTypeDef GPIO_InitStructure;
//    USART_InitTypeDef USART_InitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;
//   
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //ʹ��UART4ʱ��
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
////UART4_TX   GPIOC.10
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10��RXD
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
//    GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.10
//    
//  //UART4_RX   GPIOC.11��ʼ��0
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11��TXD
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//    GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC.11  
//  //Uart4 NVIC ����
//    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn ;
//   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
//   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3
//   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   //IRQͨ��ʹ��
//   NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���
//   
//   //UART ��ʼ������
//   USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
//   USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
//   USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//   USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
//   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
//   USART_Init(UART4, &USART_InitStructure); //��ʼ������4
//   USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
//   USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���4
}

void UART5_SendString(char* s)
{
	while(*s)//����ַ���������
	{
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC)==RESET); 
		USART_SendData(UART5 ,*s++);//���͵�ǰ�ַ�
	}
}

extern void CLR_Buf(void);
void UART5_Send_Command(char* s)
{
	CLR_Buf(); //��ս������ݵ�buffer
	UART5_SendString(s); //�����ַ���
	UART5_SendString("\r\n"); //���Զ����� \r\n�����ַ�
}

/*******************************************************************************
* ������ : Second_AT_Command
* ����   : ����ATָ���
* ����   : �������ݵ�ָ�롢ϣ���յ���Ӧ�𡢷��͵ȴ�ʱ��(��λ��20ms)
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
extern u8 Find(char *a);

u8 UART5_Send_AT_Command(char *b,char *a,u8 wait_time,u16 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //���û���ҵ� �ͼ����ٷ�һ��ָ�� �ٽ��в���Ŀ���ַ���                
	{
		UART5_Send_Command(b);//����4���� b �ַ��� �����Զ�����\r\n  �൱�ڷ�����һ��ָ��
		for(i = 0;i < 20;i++)   
		{
			delay_ms(interval_time); //�ȴ�һ��ʱ�� ��50�Ļ����� 50*20ms = 1��
		}
		
		if(Find(a))            //������ҪӦ����ַ��� a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}

void UART5_Send_Command_END(char* s)
{
	CLR_Buf(); //��ս������ݵ�buffer
	UART5_SendString(s); //�����ַ���
}

u8 UART5_Send_AT_Command_End(char *b,char *a,u8 wait_time,u32 interval_time)         
{
	u8 i;
	i = 0;
	while(i < wait_time)    //���û���ҵ� �ͼ����ٷ�һ��ָ�� �ٽ��в���Ŀ���ַ���                
	{
		UART5_Send_Command_END(b);//����4���� b �ַ��� ���ﲻ����\r\n 
		delay_ms(interval_time); //�ȴ�һ��ʱ�� ��50�Ļ����� 50*20ms = 1��
		if(Find(a))            //������ҪӦ����ַ��� a
		{
			return 1;
		}
		i++;
	}
	
	return 0;
}


