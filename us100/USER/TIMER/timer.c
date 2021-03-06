#include "timer.h"
#include "us100.h"
#include "usart.h"	


/****************************************************************************
* 名    称: TIM_Cap_Init(u32 arr,u16 psc)
* 功    能 ：5路 捕获配置 
						TIM5 CH1~CH4对应引脚：PA0,PA1,PA2,PA3
						TIM2 CH1~Ch4对应引脚：PA15,PB3，PB10,PB11
						注：TIM5的CH2被占用，打开后频繁进入中断，故不用
						
						
* 入口参数：arr：自动重装值(TIM2,TIM5是32位的!!)
						psc：时钟预分频数
* 返回参数：无
* 说    明：定时器溢出时间计算方法:Tout=((auto_data+1)*(fractional+1))/Ft us  
* 配置：1# US100 ECHO：PA0，  TRIG:PG3
				2# US100 ECHO：PA2，  TRIG:PG4
				3# US100 ECHO：PB3，  TRIG:PG5
				4# US100 ECHO：PB10， TRIG:PD11
				5# US100 ECHO：PB11， TRIG:PD12
				
****************************************************************************/
void ChannelSet(void);
int status;


TIM_CAPTURE TIM5CH1,TIM5CH2,TIM5CH3,TIM5CH4;
//TIM_CAPTURE TIM9CH1,TIM9CH2;
TIM_CAPTURE TIM2CH1,TIM2CH2,TIM2CH3,TIM2CH4;

//extern void uart1SendChars(u8 *str, u16 strlen);

TIM_ICInitTypeDef  TIM5_ICInitStructure,TIM2_ICInitStructure;



//定时器5通道1输入捕获配置
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
void TIM_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
/*配置中断*/
	//TIM5
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	//TIM2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器�
//???????



	/*时钟设置*/
	//TIM5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIM时钟使能 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTA时钟	
	//TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//使能PORTE时钟	
 
	/* GPIO初始化*/
	//TIM5-GPIOA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_15|GPIO_Pin_2|GPIO_Pin_3; //GPIOA0~3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA0

	// TIM2-GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11; //GPIOB 3,10,11
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PB

//	
	/*引脚复用映射*/
	//PA0~3复用位定时器5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM5); //
	//PE5~6复用位定时器2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2); //
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2); //
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_TIM2); //

	
  /*TIM初始化  */
	//TIM5
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	//TIM2
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	/*捕获参数设置*/
	//初始化TIM5输入捕获参数
	//TIM5 的4个通道
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	//TIM2的3个通道
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
//	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; 
//	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_4; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	// 允许捕获中断 用到的通道允许中断
		TIM_ITConfig(TIM5, TIM_IT_CC1  | TIM_IT_CC3   |TIM_IT_Update,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	
		TIM_ITConfig(TIM2,TIM_IT_Update | TIM_IT_CC2| TIM_IT_CC3 | TIM_IT_CC4,ENABLE);

	
	/*使能定时器*/
  TIM_Cmd(TIM5,ENABLE ); 	//使能定时器5
	
	TIM_Cmd(TIM2,ENABLE );

	
}



//通道类型结构体初始化配置
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

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到高电平;1,已经捕获到高电平了.
//[5:0]:捕获高电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
//u8  TIM5CH1_CAPTURE_STA=0;	//输入捕获状态		    				
//u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)


//单个通道的状捕获事件处理
void SingleChannelHandler(TIM_CAPTURE * tch )
{
	

	if((tch->CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		if(TIM_GetITStatus(tch->timX, TIM_IT_Update) != RESET)//溢出
		{	     
			if(tch->CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((tch->CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					tch->CAPTURE_STA|=0X80;		//标记成功捕获了一次
					tch->CAPTURE_VAL=0XFFFFFFFF;
				}else tch->CAPTURE_VAL++;
			}	 
		}
		if(TIM_GetITStatus(tch->timX, tch->TIM_IT_CCX) != RESET)//捕获1发生捕获事件
		{	
			if(tch->CAPTURE_STA&0X40)		//已经捕获到高电平，本次捕获到下降沿		
			{	  			
				tch->CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
			  //tch->CAPTURE_VAL=TIM_GetCapture1(tch->timX);//获取当前的捕获值.
				
				switch( tch->TIM_IT_CCX ) // 根据不同通道选择函数
				{
					case TIM_IT_CC1:						
						tch->CAPTURE_VAL=TIM_GetCapture1(tch->timX);//获取当前的捕获值.
						TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
						break;
					case TIM_IT_CC2:
						tch->CAPTURE_VAL=TIM_GetCapture2(tch->timX);//获取当前的捕获值.
						TIM_OC2PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
						break;
					case TIM_IT_CC3:
						tch->CAPTURE_VAL=TIM_GetCapture3(tch->timX);//获取当前的捕获值.
						TIM_OC3PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
						break;
					case TIM_IT_CC4:
						tch->CAPTURE_VAL=TIM_GetCapture4(tch->timX);//获取当前的捕获值.
						TIM_OC4PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获
						break;
					default:
						break;
				}
				//printf(" tch->TIM_IT_CCX = %d ",tch->TIM_IT_CCX); //打印总的高点平时间
	 			//TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 设置为上升沿捕获

			}else  								//还未开始,第一次捕获上升沿
			{
				tch->CAPTURE_STA=0;			//清空
				tch->CAPTURE_VAL=0;
				tch->CAPTURE_STA|= 0X40;		//标记捕获到了上升沿
				TIM_Cmd(tch->timX,DISABLE ); 	//关闭定时器
	 			TIM_SetCounter(tch->timX,0);
				
//				printf(" tch->CAPTURE_STA = %d ",tch->CAPTURE_STA); //打印总的高点平时间
	 			//TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Falling);		//CC1P=1 设置为下降沿捕获
				switch( tch->TIM_IT_CCX )
				{
					case TIM_IT_CC1:
						TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 设置为下降沿捕获
						break;
					case TIM_IT_CC2:
						TIM_OC2PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 设置为下降沿捕获
						break;
					case TIM_IT_CC3:
						TIM_OC3PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 设置为下降沿捕获
						break;
					case TIM_IT_CC4:
						TIM_OC4PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 设置为下降沿捕获
						break;
					default:
						break;
				}
			
				TIM_Cmd(tch->timX,ENABLE ); 	//使能定时器s
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(tch->timX, tch->TIM_IT_CCX | TIM_IT_Update); //清除中断标志位
}



//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{ 


//	status = TIM_GetITStatus(TIM5, TIM_IT_CC1 );
//	printf(" TIM_GetITStatus1 = %d ",status); //打印总的高点平时间
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC2 );
//	printf(" TIM_GetITStatus2 = %d ",status); //打印总的高点平时间
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC3 );
//	printf(" TIM_GetITStatus3 = %d ",status); //打印总的高点平时间
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC4 );
//	printf(" TIM_GetITStatus4 = %d ",status); //打印总的高点平时间
//	status = TIM_GetITStatus(TIM5, TIM_IT_Update );
//	printf(" TIM_GetITStatus5 = %d ",status); //打印总的高点平时间
//////	
//	
//	
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update); //清除中断标志位
	if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) 
   {								       
		  SingleChannelHandler( &TIM5CH1);
	 }
		if(TIM_GetITStatus(TIM5, TIM_IT_CC3) !=RESET) 
	 {								   
			SingleChannelHandler( &TIM5CH3); 
	 }
}

void TIM2_IRQHandler(void)
{
	//	status = TIM_GetITStatus(TIM5, TIM_IT_CC1 );
//	printf(" TIM_GetITStatus1 = %d ",status); //打印总的高点平时间
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC2 );
//	printf(" TIM2_GetITStatus2 = %d ",status); //打印总的高点平时间
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC3 );
//	printf(" TIM2_GetITStatus3 = %d ",status); //打印总的高点平时间
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC4 );
//	printf(" TIM2_GetITStatus4 = %d ",status); //打印总的高点平时间
//	status = TIM_GetITStatus(TIM2, TIM_IT_Update );
//	printf(" TIM2_GetITStatus5 = %d ",status); //打印总的高点平时间
	
	 TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除中断标志位
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
