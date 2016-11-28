#include "timer.h"
#include "us100.h"
#include "usart.h"	


/****************************************************************************
* ��    ��: TIM_Cap_Init(u32 arr,u16 psc)
* ��    �� ��5· �������� 
						TIM5 CH1~CH4��Ӧ���ţ�PA0,PA2,PA3
						TIM9 CH1~CH2��Ӧ����: PE5,PE6
						
						TIM2 CH1~Ch4��Ӧ���ţ�PA15,PB3��PB10,PB11
						ע��TIM5��CH2��ռ�ã��򿪺�Ƶ�������жϣ��ʲ���
						1#US100
						
* ��ڲ�����arr���Զ���װֵ(TIM2,TIM5��32λ��!!)
						psc��ʱ��Ԥ��Ƶ��
* ���ز�������
* ˵    ������ʱ�����ʱ����㷽��:Tout=((auto_data+1)*(fractional+1))/Ft us  
* ���ã�1# US100 ECHO��PA0�� TRIG:PG3

****************************************************************************/
void ChannelSet(void);
int status;


TIM_CAPTURE TIM5CH1,TIM5CH2,TIM5CH3,TIM5CH4;
//TIM_CAPTURE TIM9CH1,TIM9CH2;
TIM_CAPTURE TIM2CH1,TIM2CH2,TIM2CH3,TIM2CH4;

//extern void uart1SendChars(u8 *str, u16 strlen);

TIM_ICInitTypeDef  TIM5_ICInitStructure,TIM2_ICInitStructure;



//��ʱ��5ͨ��1���벶������
//arr���Զ���װֵ(TIM2,TIM5��32λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
/*�����ж�*/
	//TIM5
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	//TIM2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ����
//???????



	/*ʱ������*/
	//TIM5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIMʱ��ʹ�� 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTAʱ��	
	//TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//ʹ��PORTEʱ��	
 
	/* GPIO��ʼ��*/
	//TIM5-GPIOA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; //GPIOA0~3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA0

	// TIM2-GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11; //GPIOB 3,10,11
	GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��PB

//	
	/*���Ÿ���ӳ��*/
	//PA0~3����λ��ʱ��5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM5); //
	//PE5~6����λ��ʱ��9
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2); //
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2); //
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_TIM2); //
	//GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_TIM2); //
	
  /*TIM��ʼ��  */
	//TIM5
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	//TIM2
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	/*�����������*/
	//��ʼ��TIM5���벶�����
//	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
//  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
//  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
//  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
//  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
//	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
//	
//	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
//  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
//  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
//  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
//  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
//	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
//	
//	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
//  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
//  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
//  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
//  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
//	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
//	
//	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
//  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
//  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
//  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
//  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
//	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	//TIM5 ��4��ͨ��
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
	
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	//TIM2��3��ͨ��
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
//	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; 
//	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_4; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	// �������ж�
	//TIM5
	//TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������ж� ,����CC1IE�����ж�	
	//	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);//��������ж� ,����CC1IE�����ж�	
		TIM_ITConfig(TIM5, TIM_IT_CC1  | TIM_IT_CC3 | TIM_IT_CC4  |TIM_IT_Update,ENABLE);//��������ж� ,����CC1IE�����ж�	
	//	TIM_ITConfig(TIM5,TIM_IT_CC2,ENABLE);
//	TIM_ITConfig(TIM5,TIM_IT_CC3,ENABLE);
//	TIM_ITConfig(TIM5,TIM_IT_CC4,ENABLE);
	//TIM2
		TIM_ITConfig(TIM2,TIM_IT_Update | TIM_IT_CC2| TIM_IT_CC3 | TIM_IT_CC4,ENABLE);
//	TIM_ITConfig(TIM9,TIM_IT_Update|TIM_IT_CC2,ENABLE);
	
	/*ʹ�ܶ�ʱ��*/
  TIM_Cmd(TIM5,ENABLE ); 	//ʹ�ܶ�ʱ��5
	
	TIM_Cmd(TIM2,ENABLE );

	
}



//ͨ�����ͽṹ���ʼ������
void ChannelSet(void)
{
	
	TIM_CAPTURE TIMX;
	
	//
	TIMX.CAPTURE_VAL=0;
	TIMX.CAPTURE_STA=0;
	TIMX.timX = TIM5;
	TIMX.TIM_IT_CCX= TIM_IT_CC1;
	TIM5CH4 = TIM5CH3 = TIM5CH2= TIM5CH1=TIMX;
	//
	TIM5CH1.TIM_IT_CCX = TIM_IT_CC1;
	TIM5CH2.TIM_IT_CCX = TIM_IT_CC2;
	TIM5CH3.TIM_IT_CCX = TIM_IT_CC3;
	TIM5CH4.TIM_IT_CCX = TIM_IT_CC4;
	//
	TIMX.timX = TIM2;
	TIM2CH4=TIM2CH3=TIM2CH2=TIM2CH1=TIMX;
	TIM2CH1.TIM_IT_CCX = TIM_IT_CC1;
	TIM2CH2.TIM_IT_CCX = TIM_IT_CC2; 
	TIM2CH3.TIM_IT_CCX = TIM_IT_CC3;
	TIM2CH4.TIM_IT_CCX = TIM_IT_CC4; 
	

}

//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽸ߵ�ƽ;1,�Ѿ����񵽸ߵ�ƽ��.
//[5:0]:����ߵ�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
//u8  TIM5CH1_CAPTURE_STA=0;	//���벶��״̬		    				
//u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)


//����ͨ����״�����¼�����
void SingleChannelHandler(TIM_CAPTURE * tch )
{
	

	if((tch->CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{
		if(TIM_GetITStatus(tch->timX, TIM_IT_Update) != RESET)//���
		{	     
			if(tch->CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((tch->CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					tch->CAPTURE_STA|=0X80;		//��ǳɹ�������һ��
					tch->CAPTURE_VAL=0XFFFFFFFF;
				}else tch->CAPTURE_VAL++;
			}	 
		}
		if(TIM_GetITStatus(tch->timX, tch->TIM_IT_CCX) != RESET)//����1���������¼�
		{	
			if(tch->CAPTURE_STA&0X40)		//�Ѿ����񵽸ߵ�ƽ�����β����½���		
			{	  			
				tch->CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
			  //tch->CAPTURE_VAL=TIM_GetCapture1(tch->timX);//��ȡ��ǰ�Ĳ���ֵ.
				
				switch( tch->TIM_IT_CCX ) // ���ݲ�ͬͨ��ѡ����
				{
					case TIM_IT_CC1:						
						tch->CAPTURE_VAL=TIM_GetCapture1(tch->timX);//��ȡ��ǰ�Ĳ���ֵ.
						TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
						break;
					case TIM_IT_CC2:
						tch->CAPTURE_VAL=TIM_GetCapture2(tch->timX);//��ȡ��ǰ�Ĳ���ֵ.
						TIM_OC2PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
						break;
					case TIM_IT_CC3:
						tch->CAPTURE_VAL=TIM_GetCapture3(tch->timX);//��ȡ��ǰ�Ĳ���ֵ.
						TIM_OC3PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
						break;
					case TIM_IT_CC4:
						tch->CAPTURE_VAL=TIM_GetCapture4(tch->timX);//��ȡ��ǰ�Ĳ���ֵ.
						TIM_OC4PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
						break;
					default:
						break;
				}
				//printf(" tch->TIM_IT_CCX = %d ",tch->TIM_IT_CCX); //��ӡ�ܵĸߵ�ƽʱ��
	 			//TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���

			}else  								//��δ��ʼ,��һ�β���������
			{
				tch->CAPTURE_STA=0;			//���
				tch->CAPTURE_VAL=0;
				tch->CAPTURE_STA|= 0X40;		//��ǲ�����������
				TIM_Cmd(tch->timX,DISABLE ); 	//�رն�ʱ��
	 			TIM_SetCounter(tch->timX,0);
				
//				printf(" tch->CAPTURE_STA = %d ",tch->CAPTURE_STA); //��ӡ�ܵĸߵ�ƽʱ��
	 			//TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
				switch( tch->TIM_IT_CCX )
				{
					case TIM_IT_CC1:
						TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 ����Ϊ�����ز���
						break;
					case TIM_IT_CC2:
						TIM_OC2PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 ����Ϊ�����ز���
						break;
					case TIM_IT_CC3:
						TIM_OC3PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 ����Ϊ�����ز���
						break;
					case TIM_IT_CC4:
						TIM_OC4PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 ����Ϊ�����ز���
						break;
					default:
						break;
				}
			
				TIM_Cmd(tch->timX,ENABLE ); 	//ʹ�ܶ�ʱ��s
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(tch->timX, tch->TIM_IT_CCX | TIM_IT_Update); //����жϱ�־λ
}



//��ʱ��5�жϷ������	 
void TIM5_IRQHandler(void)
{ 


//	status = TIM_GetITStatus(TIM5, TIM_IT_CC1 );
//	printf(" TIM_GetITStatus1 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC2 );
//	printf(" TIM_GetITStatus2 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC3 );
//	printf(" TIM_GetITStatus3 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC4 );
//	printf(" TIM_GetITStatus4 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//	status = TIM_GetITStatus(TIM5, TIM_IT_Update );
//	printf(" TIM_GetITStatus5 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//////	
//	
//	
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update); //����жϱ�־λ
	if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) 
   {								  
     
		 /* Clear TIM3 Capture compare interrupt pending bit */
     
		  SingleChannelHandler( &TIM5CH1);
		 
	 }
	 	if(TIM_GetITStatus(TIM5, TIM_IT_CC3) !=RESET) 
   {								  
     
		  SingleChannelHandler( &TIM5CH3);
		 
	 }
	 	if(TIM_GetITStatus(TIM5, TIM_IT_CC4) !=RESET) 
   {								  
     
		  SingleChannelHandler( &TIM5CH4);
		 
	 }

}

void TIM2_IRQHandler(void)
{
	//	status = TIM_GetITStatus(TIM5, TIM_IT_CC1 );
//	printf(" TIM_GetITStatus1 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC2 );
//	printf(" TIM2_GetITStatus2 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC3 );
//	printf(" TIM2_GetITStatus3 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC4 );
//	printf(" TIM2_GetITStatus4 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
//	status = TIM_GetITStatus(TIM2, TIM_IT_Update );
//	printf(" TIM2_GetITStatus5 = %d ",status); //��ӡ�ܵĸߵ�ƽʱ��
	
	 TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //����жϱ�־λ
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) 
   {								  
     
	     
		  SingleChannelHandler( &TIM2CH2);
		 
	 }
	 	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) !=RESET) 
   {								  
     
		  SingleChannelHandler( &TIM2CH3);
		 
	 }
	 	if(TIM_GetITStatus(TIM2, TIM_IT_CC4) !=RESET) 
   {								  
     
		  SingleChannelHandler( &TIM2CH4);
		 
	 }

	
}

//void TIM1_BRK_TIM9_IRQHandler(void)
//{
//	
//	 SingleChannelHandler(TIM9CH1);
//	 SingleChannelHandler(TIM9CH2);
//	
//}
