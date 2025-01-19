

//��Ƭ��ͷ�ļ�
#include "main.h"

//�����豸
#include "esp01s.h"

//Э���ļ�
#include "onenet.h"
#include "MQTT.h"

//Ӳ������
#include "usart.h"
//C��
#include <string.h>
#include <stdio.h>



#define PROID		"570399"//��ƷID

#define AUTH_INFO	"123456"//��Ȩ��Ϣ

#define DEVID		"1041794845"//�豸ID

	int counters=0;
extern unsigned char ESP01S_buf[128];


//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP01S_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP01S_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:;status = 0;break;					
					case 1:;break;
					case 2:;break;
					case 3:;break;
					case 4:;break;
					case 5:;break;
					
					default:;break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		;
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[32];
	float Temp;
    float Humidity;
	

	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, ",;");
	
	memset(text, 0, sizeof(text));
	//sprintf(text, "Temp,%.2f;Humidity,%.2f;",Temp,Humidity);
	sprintf(text, "counters,%d;", counters); 
	strcat(buf, text);
	
	
	return strlen(buf);

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//���
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP01S_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
			;
			
			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
		}
		else
			;
	}
	
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
			
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
				{
				
					
					ESP01S_SendData(mqttPacket._data, mqttPacket._len);			//�ظ�����
					MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
				}
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
		
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP01S_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//����'}'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}


