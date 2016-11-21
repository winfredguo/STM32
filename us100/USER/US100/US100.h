#ifndef __US100_H
#define __US100_H
#include "common.h"

////////////////////////////////////////////////////////////////////////////////////	

//LED�˿ڶ���
//#define LED0 PEout(3)	 
//#define LED1 PEout(4)	 
//#define LED2 PGout(9)	 

# define U1_TRIGPin PGout(3)
# define U2_TRIGPin PGout(4)
# define U3_TRIGPin PGout(5)
# define U4_TRIGPin PDout(11)
# define U5_TRIGPin PDout(12)
# define U6_TRIGPin PDout(13)



//# define ECHOPin PGin(4)

typedef  struct TIM_CH_CAPTURE 
{
	u8  CAPTURE_STA;		//���벶��״̬		    				
	u32 CAPTURE_VAL;	//���벶��ֵ 
	TIM_TypeDef* timX;  // ��ʱ�� TIM5��TIM9��etc
	uint16_t TIM_IT_CCX; // ��ʱ���е�ͨ��
} TIM_CAPTURE;




//��������
//void LED_Init(void);//��ʼ��	
void US100IO_Init(void);
void TIM_Cap_Init(u32 arr,u16 psc);
void ChannelSet(void);


#endif
