#ifndef _ONENET_H_
#define _ONENET_H_

_Bool OneNet_DevLink(void);
void OneNet_SendData(void);
void OneNet_RevPro(unsigned char *cmd);
extern int counters;
#endif

