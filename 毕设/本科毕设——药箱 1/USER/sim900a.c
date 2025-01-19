#include "sim900a.h"
#include "uart5.h"
#include "usart.h"
#include "sys.h"
#include "delay.h"  
#include "string.h"
#include "math.h"
#include "timer.h"
#include "usart3.h"
char *phone_num = "ATD10086;"; //����绰���޸���������޸Ĳ���ĵ绰��
char *phone_num_message = "AT+CMGS=\"13978501332\""; //����绰���޸���������޸Ĳ���ĵ绰��
char *Chinese_text_message = "AT+CMGS=\"00310035003200300037003800350034003800360037\""; //����绰���޸���������޸Ĳ���ĵ绰��
//00310035003200300037003800350034003800360037
/*************  ���ر�������	**************/
char Uart5_Buf[Buf5_Max];//����2���ջ���
u8 First_Int = 0;
char error_result[20];

u8 phonebuf[20]; 		//���뻺��
u8 pohnenumlen=0;		//���볤��,���15���� 


void UART5_IRQHandler(void)                	
{
	u8 Res=0;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //���յ�����
	{	 
		Res = USART_ReceiveData(UART5);
		Uart5_Buf[First_Int] = Res;  	  //�����յ����ַ����浽������
		First_Int++;                	  //����ָ������ƶ�
		if(First_Int > Buf5_Max)       	  //���������,������ָ��ָ�򻺴���׵�ַ
		{
			First_Int = 0;
		} 
	} 
	if(USART_GetFlagStatus(UART5,USART_FLAG_ORE) != RESET) // ��� ORE ��־
  {
      USART_ClearFlag(UART5,USART_FLAG_ORE);
      USART_ReceiveData(UART5);
  }
} 



void CLR_Buf(void)
{
	u16 k;
	for(k=0;k<Buf5_Max;k++)      //��������������
	{
		Uart5_Buf[k] = 0x00;
	}
    First_Int = 0;              //�����ַ�������ʼ�洢λ��
}
/***************************************************************
ע����Ȼ����Է�������ֵ����ȷ����������һ������ָ�����ʧ���ˡ�
****************************************************************/
int send_text_message(char *content)
{
	u8 ret;
	char end_char[2];
	
	end_char[0] = 0x1A;//�����ַ�
	end_char[1] = '\0';
	

	
	ret = UART5_Send_AT_Command("AT+CMGF=1","OK",3,50);//����ΪTEXTģʽ
	if(ret == 0)
	{
		return AT_CMGF_ERROR;
	}
	
	ret = UART5_Send_AT_Command("AT+CSCS=\"GSM\"","OK",3,50);//�����ַ���ʽ
	if(ret == 0)
	{
		return AT_CSCS_ERROR;
	}
	
	ret = UART5_Send_AT_Command(phone_num_message,">",3,50);//���������˵ĵ绰����
	if(ret == 0)
	{
		return AT_CMGS_ERROR;
	}
	
	UART5_SendString(content); 
	ret = UART5_Send_AT_Command_End(end_char,"OK",1,350);//���ͽ��������ȴ�����ok,�ȴ�5S��һ�Σ���Ϊ���ųɹ����͵�״̬ʱ��Ƚϳ�
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}
//ģ�鼰��·ͨ�����
int simCard_test(void)
{
	int ret;
	ret = UART5_Send_AT_Command("AT","OK",3,50);//����ͨ���Ƿ�ɹ�
	  
	if(ret == 0)
	{
		
		return 0;
	}
	
//	ret = UART4_Send_AT_Command("AT+CPIN?","READY",2,50);//��ѯ���Ƿ����
//	if(ret == 0)
//	{
//		printf("where is your SIM Card?\r\n");
//		return 0;
//	}
	
//	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
//	if(ret == 0)
//	{
//		printf("I can't register the network\r\n");
//		return 0;
//	}
	delay_ms(1000);
	return 1;
}


//���Ͷ���
int send_message_num(char *message) 
{
	int ret;
	
	ret = UART5_Send_AT_Command("AT","OK",3,50);//����ͨ���Ƿ�ɹ�
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
	ret = UART5_Send_AT_Command("AT+CPIN?","READY",2,50);//��ѯ���Ƿ����
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	
	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
	if(ret == 0)
	{	
		return SIM_CARD_NO_REG_ERROR;
	}
	
	ret = send_text_message(message);//����TEXT����
	if(ret != 1)
	{		
		return MESSAGE_ERROR; 
	}
	
	return 1;
}

int call_phone_num(void) 
{
	int ret;
	
	ret = UART5_Send_AT_Command("AT","OK",3,50);//����ͨ���Ƿ�ɹ�
	  
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	/*
	ret = UART4_Send_AT_Command("AT+CPIN?","READY",2,50);//��ѯ���Ƿ����
	if(ret == 0)
	{
		return NO_SIM_CARD_ERROR;
	}
	*/
	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
	if(ret == 0)
	{
		return SIM_CARD_NO_REG_ERROR;
	}
	
	
	ret = UART5_Send_AT_Command(phone_num,"OK",2,50);//����
	if(ret == 0)
	{
		return CALL_ERROR;
	}
	
	return 1;
}

int redail_phone_num(void)
{
	int ret;
	int i;
	ret = UART5_Send_AT_Command("AT+DLST","OK",3,50);//�ز� �������OK ˵��ָ��ͳɹ� ���������ں���
	
	if(ret == 0)
	{
		return AT_DLST_OK_ERROR;
	}
	for(i = 0;i < 20;i++)   //�ٵȴ�2��  ��Ȼ���ŵ�ʱ��Ĵ�����Ϣû��������
	{
		delay_ms(100);
	} 
    if(Find("\"CALL\",1") == 0)
    {
        return AT_DLST_CALL_ERROR;
    }
    
    return 1;
}


/*******************************************************************************
* ������ : Wait_CREG
* ����   : �ȴ�ģ��ע��ɹ�
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
u8 Wait_CREG(u8 query_times)
{
	u8 i;
	u8 k;
	u8 j;
	i = 0;
	CLR_Buf();
	while(i == 0)        			
	{

		UART5_Send_Command("AT+CREG?");  //���Ͳ�ѯ�Ƿ�ע������ĺ���
		for(i = 0;i < 20;i++)   
		{
			delay_ms(100);
		}

        //������ҵ�" +CREG: 0,1"˵�����ɹ�ע�ᣬ���ҷ������������forѭ��
		for(k=0;k<Buf5_Max;k++)      			
		{
			if((Uart5_Buf[k] == '+')&&(Uart5_Buf[k+1] == 'C')&&(Uart5_Buf[k+2] == 'R')&&(Uart5_Buf[k+3] == 'E')&&(Uart5_Buf[k+4] == 'G')&&(Uart5_Buf[k+5] == ':'))
			{
					 
				if((Uart5_Buf[k+7] == '0')&&((Uart5_Buf[k+9] == '1')||(Uart5_Buf[k+9] == '5')))
				{
					i = 1;
					return 1;  //����鵽��ֱ�ӷ��� ��ѯ�����Ļ� �����ٲ�ѯ �ܹ���ѯ query_times ���
				}
				
			}
		}
		j++;
		if(j > query_times)
		{
			return 0;
		}
		
	}
	return 0;
}

/*******************************************************************************
* ������ : Find
* ����   : �жϻ������Ƿ���ָ�����ַ���
* ����   : 
* ���   : 
* ����   : unsigned char:1 �ҵ�ָ���ַ���0 δ�ҵ�ָ���ַ� 
* ע��   : 
*******************************************************************************/

u8 Find(char *a)
{ 
	if(strstr(Uart5_Buf, a)!=NULL) //Uart4_Buf���շ������ݵ�buffer strstr�����ǿ⺯����������������ж�Uart4_Buf�����Ƿ���a�ַ���
	{
		return 1;
	}	
	else
	{
		return 0;
	}
		
}

//��ֲ�������Ķ���-st
int send_message_chinese(char *message)
{
	int ret;
	
	ret = UART5_Send_AT_Command("AT","OK",3,50);//����ͨ���Ƿ�ɹ�
	if(ret == 0)
	{
		return COMMUNITE_ERROR;
	}
	
//	ret = UART4_Send_AT_Command("AT+CPIN?","READY",2,50);//��ѯ���Ƿ����
//	if(ret == 0)
//	{
//		return NO_SIM_CARD_ERROR;
//	}
	
//	ret = Wait_CREG(3);//��ѯ���Ƿ�ע�ᵽ����
//	if(ret == 0)
//	{	
//		return SIM_CARD_NO_REG_ERROR;
//	}
	
	ret = UART5_Send_AT_Command("AT+CMGF=1","OK",2,50);//�����ı�ģʽ
	if(ret == 0)
	{
		return MESSAGE_MODE_SET_ERROR;
	}

	ret = UART5_Send_AT_Command("AT+CSMP=17,167,1,8","OK",2,50);//���ö���Ϣ�ı�ģʽ���� 
	//ret = UART4_Send_AT_Command("AT+CSMP=17,0,2,25","OK",2,50);//���ö���Ϣ�ı�ģʽ���� 
	if(ret == 0)
	{
		return SET_MESSAGE_TEXT_MODE_ERROR;
	}	

	ret = UART5_Send_AT_Command("AT+CSCS=\"UCS2\"","OK",2,50);//����TE�ַ���ΪUCS2 
	if(ret == 0)
	{
		return MESSAGE_SET_UCS2_ERROR;
	}
	
	ret = send_chinese_text_message(message);//����TEXT����
	if(ret == 1)
	{		
		return 1; 
	}
	
	return CHINESE_MESSAGE_ERROR;
}


int send_chinese_text_message(char *content)
{
	u8 ret;
	char end_char[2];
	
	end_char[0] = 0x1A;//�����ַ�
	end_char[1] = '\0';
	
	ret = UART5_Send_AT_Command(Chinese_text_message,">",3,50);//���������˵ĵ绰����
	if(ret == 0)
	{
		return AT_CMGS_ERROR;
	}
	
	UART5_SendString(content); 
	ret = UART5_Send_AT_Command_End(end_char,"+CMGS",1,350);//���ͽ��������ȴ�����ok,�ȴ�5S��һ�Σ���Ϊ���ųɹ����͵�״̬ʱ��Ƚϳ�
	if(ret == 0)
	{
		return END_CHAR_ERROR;
	}
	
	return 1;
}

void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode)
{
	
	src++;
}


void last_warn()
{
	
  if((hour==t1)&&(min==t2+15)&&flag1)
  {
  send_message_chinese("4E3A4E864FDD8BC160A876848EAB4F5350655EB7FF0C8BF7630965F6670D836F");
	  flag1=0;
  
  }
  else if ((hour==t11)&&(min==t22+15)&&flag2)
  {
  send_message_chinese("4E3A4E864FDD8BC160A876848EAB4F5350655EB7FF0C8BF7630965F6670D836F");
	  flag2=0;
  
  } 
  else if ((hour==t111)&&(min==t222+15)&&flag3)
  {
  send_message_chinese("4E3A4E864FDD8BC160A876848EAB4F5350655EB7FF0C8BF7630965F6670D836F");
	  flag3=0;
  
  } 
  else ;


}
