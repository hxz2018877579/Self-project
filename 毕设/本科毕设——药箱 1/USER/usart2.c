#include "sys.h"
#include "usart2.h"
#include "timer.h"



void USART2_SendData(u8 data)
{
	while((USART2->SR & 0X40) == 0);
	USART2->DR = data;
}

void USART2_SendString(u8 *DAT, u8 len)
{
	u8 i;
	for(i = 0; i < len; i++)
	{
		USART2_SendData(*DAT++);
	}
}

#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
u16 USART2_RX_STA = 0;     //����״̬���

void USART2_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO

	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO

	//USART2 NVIC ����
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2

	TIM7_Int_Init(99, 7199);		//10ms�ж�
	USART2_RX_STA = 0;		//����
	TIM_Cmd(TIM7, DISABLE);			//�رն�ʱ��7
}

void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Res = USART_ReceiveData(USART2);	//��ȡ���յ�������
		if((USART2_RX_STA & 0x8000) == 0) //����δ���
		{
			if(USART2_RX_STA < USART2_REC_LEN)	//�����Խ�������
			{
				TIM_SetCounter(TIM7, 0); //���������          				//���������
				if(USART2_RX_STA == 0) 				//ʹ�ܶ�ʱ��3���ж�
				{
					TIM_Cmd(TIM7, ENABLE); //ʹ�ܶ�ʱ��7
				}
				USART2_RX_BUF[USART2_RX_STA++] = Res;	//��¼���յ���ֵ
			}
			else
			{
				USART2_RX_STA |= 1 << 15;				//ǿ�Ʊ�ǽ������
			}
		}
	}
}
#endif

