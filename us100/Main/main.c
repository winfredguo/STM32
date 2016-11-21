#include "us100.h"
#include "usart.h"	
#include "timer.h"


/*********************************************************************************
**********************MCU���� STM32F407Ӧ�ÿ�����(�����)*************************
**********************************************************************************
* �ļ�����:  US100 ������������ʹ��������main.c                                        *
* �ļ�������US100                                                          *
* �������ڣ�2016.10.23                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�wethty                                                               *
* ˵    �������ö�ʱ���������US100���ظߵ�ƽʱ�䣬�õ�̽�����                  *                                     *
**********************************************************************************
*********************************************************************************/
int CalDistance(TIM_CAPTURE *tch );
 
 
extern TIM_CAPTURE TIM5CH1,TIM5CH2,TIM5CH3,TIM5CH4;
extern TIM_CAPTURE TIM9CH1,TIM9CH2;

int main(void)
{ 
//		int Time_ECHO =0;
//	long long temp=0;  
	int distance =0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init();          //��ʼ����ʱ����

	uart_init(9600);//��ʼ�����ڲ�����Ϊ115200

	//��ʼ��US100�˿�
	US100IO_Init();

	ChannelSet();// ͨ�����ͽṹ���ʼ������,��ʼ��ʱ��ǰ����

	TIM_Cap_Init(0XFFFFFFFF,84-1);// ��84M/84=1Mhz Ƶ�ʼ���
	//printf("after TIM_Cap_Init"); //��ӡ�ܵĸߵ�ƽʱ��	
	

	

	
	while(1)
	{
 		delay_ms(500);
		
			// set trig 
		U1_TRIGPin =1;
		delay_us(20);
		U1_TRIGPin =0;
		distance = CalDistance( &TIM5CH1);
		printf("US100_1 distance=%d mm \n", distance); //��ӡ�ܵĸߵ�ƽʱ��
		
//			// set trig 
		U2_TRIGPin =1;
		delay_us(20);
		U2_TRIGPin =0;
		distance = CalDistance( &TIM5CH3);
		printf("US100_2 distance=%d mm \n", distance); //��ӡ�ܵĸߵ�ƽʱ��
				
		

		
	
	
	}
}

int CalDistance(TIM_CAPTURE* tch )
{
		
	int Time_ECHO =0;
	long long temp=0;  
	int distance =0; 
	
	
	
	
	
	
	
		//printf("tch->CAPTURE_STA=%X",tch->CAPTURE_STA); //��ӡ�ܵĸߵ�ƽʱ��
 		if(tch->CAPTURE_STA&0X80)        //�ɹ�������һ�θߵ�ƽ
		{
			temp=tch->CAPTURE_STA&0X3F;  //�������
			
			Time_ECHO= (int)(temp * 0XFFFFFFFF);		 		         //���ʱ���ܺ�
			Time_ECHO+= tch->CAPTURE_VAL;		   //�õ��ܵĸߵ�ƽʱ��

//			if( (temp < 6000) && ( temp >1) )
//			{
				distance = (Time_ECHO*34/100)/2; //����= �ߵ�ƽʱ�� * 0.34mm/us) / 2  mm
				//printf("distance=%d mm  overflow=%lld TIM5CH1_CAPTURE_VAL=%d \n",distance,temp,TIM5CH1_CAPTURE_VAL); //��ӡ�ܵĸߵ�ƽʱ��
				//	printf("distance=%d mm \n",distance); //��ӡ�ܵĸߵ�ƽʱ��
//			}
			tch->CAPTURE_VAL=0;
			tch->CAPTURE_STA=0;	//������һ�β���
			
		}
	
	return distance;
}


