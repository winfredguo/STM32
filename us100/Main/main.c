#include "us100.h"
#include "usart.h"	
#include "timer.h"


/*********************************************************************************
**********************MCU启明 STM32F407应用开发板(高配版)*************************
**********************************************************************************
* 文件名称:  US100 超声波传感器使用主函数main.c                                        *
* 文件简述：US100                                                          *
* 创建日期：2016.10.23                                                           *
* 版    本：V1.0                                                                 *
* 作    者：wethty                                                               *
* 说    明：利用定时器捕获计算US100返回高电平时间，得到探测距离                  *                                     *
**********************************************************************************
*********************************************************************************/
int CalDistance(TIM_CAPTURE *tch );
 
 
extern TIM_CAPTURE TIM5CH1,TIM5CH2,TIM5CH3,TIM5CH4;
//extern TIM_CAPTURE TIM9CH1,TIM9CH2;
extern TIM_CAPTURE TIM2CH1,TIM2CH2,TIM2CH3,TIM2CH4;

int main(void)
{ 

	int distance =0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();          //初始化延时函数
	uart_init(9600);//初始化串口波特率为115200
	//初始化US100端口
	US100IO_Init();
	ChannelSet();// 通道类型结构体初始化配置,初始化时钟前配置
	TIM_Cap_Init(0XFFFFFFFF,84-1);// 以84M/84=1Mhz 频率计数
	
	while(1)
	{
 		delay_ms(500);
		
			// set trig 
		U1_TRIGPin =1;
		delay_us(20);
		U1_TRIGPin =0;
		distance = CalDistance( &TIM5CH1);
		printf("US100_1 distance=%d mm \n", distance); //打印总的高点平时间
//		
////			// set trig 
		U2_TRIGPin =1;
		delay_us(20);
		U2_TRIGPin =0;
		distance = CalDistance( &TIM5CH3);
		printf("US100_2 distance=%d mm \n", distance); //打印总的高点平时间
		
		U3_TRIGPin =1;
		delay_us(20);
		U3_TRIGPin =0;
		distance = CalDistance( &TIM2CH2);
		printf("US100_3 distance=%d mm \n", distance); //打印总的高点平时间
//		
		U4_TRIGPin =1;
		delay_us(20);
		U4_TRIGPin =0;
		distance = CalDistance( &TIM2CH3);
		printf("US100_4 distance=%d mm \n", distance); //打印总的高点平时间
		
		U5_TRIGPin =1;
		delay_us(20);
		U5_TRIGPin =0;
		distance = CalDistance( &TIM2CH4);
		printf("US100_5 distance=%d mm \n", distance); //打印总的高点平时间
		
//		U6_TRIGPin =1;
//		delay_us(20);
//		U6_TRIGPin =0;
//		distance = CalDistance( &TIM2CH4);
//		printf("US100_3 distance=%d mm \n", distance); //打印总的高点平时间
	}
}

int CalDistance(TIM_CAPTURE* tch )
{
		
	int Time_ECHO =0;
	long long temp=0;  
	int distance =0; 
	
 		if(tch->CAPTURE_STA&0X80)        //成功捕获到了一次高电平
		{
			temp=tch->CAPTURE_STA&0X3F;  //溢出次数

			Time_ECHO= (int)(temp * 0XFFFFFFFF);		 		         //溢出时间总和
			Time_ECHO+= tch->CAPTURE_VAL;		   //得到总的高电平时间

//			if( (temp < 6000) && ( temp >1) )
//			{
				distance = (Time_ECHO*34/100)/2; //距离= 高电平时间 * 0.34mm/us) / 2  mm		
//			}
			tch->CAPTURE_VAL=0;
			tch->CAPTURE_STA=0;	//开启下一次捕获
			
		}
	
	return distance;
}



