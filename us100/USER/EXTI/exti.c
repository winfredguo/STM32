#include "exti.h"
#include "hs0038.h"
#include "lcd.h"

/*********************************************************************************
*************************MCU���� STM32F407Ӧ�ÿ�����******************************
**********************************************************************************
* �ļ�����: exti.c                                                               *
* �ļ��������ⲿ�жϳ�ʼ��                                                       *
* �������ڣ�2015.03.18                                                           *
* ��    ����V1.0                                                                 *
* ��    �ߣ�Clever                                                               *
* ˵    �����жϷ��������������չܽ��յ��ĸ���ƽ��ʱ��                       * 
**********************************************************************************
*********************************************************************************/	 


//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
	static u8  i=0;             //���պ����źŴ���
	static u8 Start_flag;       //�Ƿ�ʼ�����־λ
	
  TIM_Cmd(TIM2,ENABLE);   
	
	if(Start_flag)                         
	{
		if(Ir_Time >34)//������ TC9012��ͷ�룬9ms+4.5ms
		{		
			i = 0;	
		}
		
		Ir_TimeData[i] = Ir_Time;//�洢ÿ����ƽ�ĳ���ʱ�䣬�����Ժ��ж���0����1
		Ir_Time = 0;
		i++;
		
		if(i ==33)
		{
			 Recive_OK = 1;
			 TIM_Cmd(TIM2,DISABLE);    
			 i = 0;
			 IR_flag++;
		}
		if(IR_flag==2)     //������һ�ΰ����ⰴ���������ڶ���ʼ������ֵ
			IR_OK = 1; 
	}
	else
	{
		TIM_Cmd(TIM2,DISABLE);    
		IR_OK = 0;
		Ir_Time = 0;
		Start_flag = 1;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line2);  //���LINE2�ϵ��жϱ�־λ 
}

   
//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE2
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//PE2 ���ӵ��ж���2
	
	/* ����EXTI_Line2 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	   
}












