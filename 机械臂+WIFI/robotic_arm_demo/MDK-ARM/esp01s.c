
//��Ƭ��ͷ�ļ�
#include "main.h"

//�����豸����
#include "esp01s.h"
//Ӳ������
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>
#include <string.h>
 

#define ESP01S_WIFI_INFO		"AT+CWJAP=\"WIFI\",\"00000000\"\r\n"
#define ESP01S_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"

unsigned char ESP01S_buf[128];
unsigned short ESP01S_cnt = 0, ESP01S_cntPre = 0;

//uint8_t aRxBuffer;			//�����жϻ���
//==========================================================
//	�������ƣ�	ESP01S_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP01S_Clear(void)
{

	memset(ESP01S_buf, 0, sizeof(ESP01S_buf));
	ESP01S_cnt = 0;

}

//==========================================================
//	�������ƣ�	ESP01S_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool ESP01S_WaitRecive(void)
{

	if(ESP01S_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(ESP01S_cnt == ESP01S_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		ESP01S_cnt = 0;							//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	ESP01S_cntPre = ESP01S_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	ESP01S_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool ESP01S_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(huart1, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP01S_WaitRecive() == REV_OK)							//����յ�����
		{
			if(strstr((const char *)ESP01S_buf, res) != NULL)		//����������ؼ���
			{
				ESP01S_Clear();									//��ջ���
				
				return 0;
			}
		}
		
		HAL_Delay(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	ESP01S_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP01S_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP01S_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
	if(!ESP01S_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
	{
		Usart_SendString(huart1, data, len);		//�����豸������������
	}

}

//==========================================================
//	�������ƣ�	ESP01S_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP01S�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP01S_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	
	do
	{
		if(ESP01S_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)ESP01S_buf, "IPD,");				//������IPD��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
				
			}
		}
		
		HAL_Delay(5);
    timeOut--;		//��ʱ�ȴ�
	} while(timeOut>0);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	ESP01S_Init
//
//	�������ܣ�	��ʼ��ESP01S
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void ESP01S_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();			


//	//ESP8266��λ����
//	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_Initure.Pin = GPIO_PIN_1;					//GPIOB1-��λ
//	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
//	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

//	HAL_Delay(250);
//	
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
//	HAL_Delay(500);
	
	ESP01S_Clear();
	

	while(ESP01S_SendCmd("AT\r\n", "OK"))
		HAL_Delay(500);
	
	//while(ESP01S_SendCmd("AT+CWMODE=1\r\n", "OK"))
//	ESP01S_SendCmd("AT+RST\r\n", "");
//        HAL_Delay(500);	
//	
//    ESP01S_SendCmd("AT+CIPCLOSE\r\n", "");              //�ر�TCP����
//		HAL_Delay(500);	
	

	while(ESP01S_SendCmd("AT+CWMODE=1\r\n", "OK"))     //ģʽ1(Station),Ĭ�ϱ���Flash
		HAL_Delay(500);

      //����DHCP(��ȡ����IP),Ĭ�ϱ���Flash
	while(ESP01S_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))  
		HAL_Delay(500);
	
          //����WIFI
	while(ESP01S_SendCmd(ESP01S_WIFI_INFO, "GOT IP"))
		HAL_Delay(500);
	
     //����TCP����
	while(ESP01S_SendCmd(ESP01S_ONENET_INFO, "CONNECT"))
		HAL_Delay(500);
	


}

/* USER CODE BEGIN 4 */
 



/* USER CODE END 4 */

