#ifndef __LED_H
#define __LED_H
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////	

//LED�˿ڶ���
//#define LED0 PEout(3)	 
//#define LED1 PEout(4)	 
//#define LED2 PGout(9)	 

# define TRIGPin PGout(3)
//# define ECHOPin PGin(4)


//��������
//void LED_Init(void);//��ʼ��	
void US100IO_Init(void);

#endif
