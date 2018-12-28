#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "ff.h"
#include "./mjpegplayer/avifile.h"
#include "./mjpegplayer/vidoplayer.h"
#include "./Bsp/lcd/bsp_lcd.h"
#include "./Bsp/wm8978/bsp_wm8978.h"  
#include "emXGUI.h"
#include "emXGUI_JPEG.h"
#include "rtthread.h"
#include "GUI_MusicList_DIALOG.h"
#include "x_libc.h"
#include "GUI_MUSICPLAYER_DIALOG.h"
FIL       fileR;
UINT      BytesRD;
__align(4) uint8_t   Frame_buf[1024*30];

static volatile uint8_t audiobufflag=0;
__align(4) uint8_t   Sound_buf[4][1024*5]={0};

uint8_t   *pbuffer;

uint32_t  mid;
uint32_t  Strsize;
uint16_t  Strtype;



static volatile uint8_t timeout;
extern WAVEFORMAT*   wavinfo;
extern avih_TypeDef* avihChunk;
extern HWND wnd_time;
extern int avi_chl;
void MUSIC_I2S_DMA_TX_Callback(void);
extern void mjpegdraw(uint8_t *mjpegbuffer,uint32_t size);
static void TIM3_Config(uint16_t period,uint16_t prescaler);
extern void App_DecodeMusic(HWND hwnd, const void *dat, int cbSize, JPG_DEC *dec);
extern char tiimg[];
extern unsigned int timgsize(void);
extern HDC hdc_AVI;
extern HWND hwnd_AVI;
extern volatile int win_fps;
void JPEG_Out(HDC hdc,int x,int y,u8 *mjpegbuffer,s32 size);

static volatile int frame=0;
static volatile int t0=0;
volatile int avi_fps=0;

static  JPG_DEC *dec=NULL;

u32 alltime = 0;		//总时长 
u32 cur_time; 		//当前播放时间 
uint8_t temp11=0;	
u32 pos;
s32 time_sum = 0;
void AVI_play(char *filename, HWND hwnd)
{
  FRESULT  res;
  uint32_t offset;
  uint16_t audiosize;
  uint8_t avires=0;
  uint8_t audiosavebuf;

  pbuffer=Frame_buf;
  res=f_open(&fileR,filename,FA_READ);
  if(res!=FR_OK)
  {
    return;    
  }
  AVI_DEBUG("S\n");
  res=f_read(&fileR,pbuffer,20480,&BytesRD);
  AVI_DEBUG("E\n");  
  avires=AVI_Parser(pbuffer);//解析AVI文件格式
  if(avires)
  {
    return;    
  }
  
  avires=Avih_Parser(pbuffer+32);//解析avih数据块
  if(avires)
  {
    return;    
  }
  //strl列表
  avires=Strl_Parser(pbuffer+88);//解析strh数据块
  if(avires)
  {
    return;    
  }
  
  avires=Strf_Parser(pbuffer+164);//解析strf数据块
  if(res!=FR_OK)
  {
    return;    
  }
  
  mid=Search_Movi(pbuffer);//寻找movi ID	（数据块）	
  if(mid==0)
  {
    return;    
  }
  
  Strtype=MAKEWORD(pbuffer+mid+6);//流类型（movi后面有两个字符）
  Strsize=MAKEDWORD(pbuffer+mid+8);//流大小
  if(Strsize%2)Strsize++;//奇数加1
  f_lseek(&fileR,mid+12);//跳过标志ID  
  
  offset=Search_Auds(pbuffer);
  if(offset==0)
  {
    return;    
  }  
  audiosize=*(uint8_t *)(pbuffer+offset+4)+256*(*(uint8_t *)(pbuffer+offset+5));
  if(audiosize==0)
  {
    offset=(uint32_t)pbuffer+offset+4;
    mid=Search_Auds((uint8_t *)offset);
    if(mid==0)
    {
      return;    
    }
    audiosize=*(uint8_t *)(mid+offset+4)+256*(*(uint8_t *)(mid+offset+5));
  }
  
   I2S_Stop();			/* 停止I2S录音和放音 */
	wm8978_Reset();		/* 复位WM8978到复位状态 */	
  	/* 配置WM8978芯片，输入为DAC，输出为耳机 */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* 调节音量，左右相同音量 */
	wm8978_SetOUT1Volume(15);

	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
  I2S_GPIO_Config();
  I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16b, wavinfo->SampleRate);
  I2S_DMA_TX_Callback=MUSIC_I2S_DMA_TX_Callback;			//回调函数指wav_i2s_dma_callback
  I2S_Play_Stop();
  I2Sx_TX_DMA_Init((uint16_t *)Sound_buf[1],(uint16_t *)Sound_buf[2],audiosize/2);
  audiobufflag=0;	    
  timeout=0;
  audiosavebuf=0;
  audiobufflag=0;
  TIM3_Config((avihChunk->SecPerFrame/100)-1,9000-1);
  I2S_Play_Start();  
	
	t0= GUI_GetTickCount();

   //歌曲总长度=每一帧需要的时间（s）*帧总数
   alltime=(avihChunk->SecPerFrame/1000)*avihChunk->TotalFrame;
   alltime/=1000;//单位是秒
  WCHAR buff[128];
  //char *str = NULL;
  RECT rc0 = {0, 370,120,30};//当前时间
  RECT rc1 = {680,370,120,30};//总时间
  RECT rc2 = {0,0,800,40};//歌曲名称
  RECT rc3 = {0,40,380,40};//分辨率
  RECT rc4 = {440,40,360,40};//歌曲名称
  
  while(1&&!sw_flag)//播放循环
  {					
		int t1;
     if(!avi_chl){
   //fptr存放着文件指针的位置，fsize是文件的总大小，两者之间的比例和当前时间与总时长的比例相同（fptr/fsize = cur/all）     
   cur_time=((double)fileR.fptr/fileR.fsize)*alltime;
   //更新进度条
   InvalidateRect(wnd_time, NULL, FALSE);   
   SendMessage(wnd_time, SBM_SETVALUE, TRUE, cur_time*255/alltime);     
	x_wsprintf(buff, L"%02d:%02d:%02d",///%02d:%02d:%02d alltime/3600,(alltime%3600)/60,alltime%60
             cur_time/3600,(cur_time%3600)/60,cur_time%60); 		
	 if(Strtype==T_vids)//显示帧
    {    	
			frame++;
			t1 =GUI_GetTickCount();
			if((t1 - t0) >= 1000)
			{
				
				avi_fps =frame;
				t0 =t1;
				frame =0;
			}

      //HDC hdc_mem,hdc;
      pbuffer=Frame_buf;
      AVI_DEBUG("S\n"); 
      f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息
      AVI_DEBUG("E\n");   
			timeout=0;
		
			if(frame&1)
			{	
	
	#if 1		//直接写到窗口方式.	
				HDC hdc;
				
				hdc =GetDC(hwnd_AVI);
				JPEG_Out(hdc,160,89,Frame_buf,BytesRD);
            ClrDisplay(hdc, &rc0, MapRGB(hdc, 0,0,0));
            SetTextColor(hdc, MapRGB(hdc,255,255,255));
            DrawText(hdc, buff,-1,&rc0,DT_VCENTER|DT_CENTER);
            
           x_wsprintf(buff, L"%02d:%02d:%02d",
                     alltime/3600,(alltime%3600)/60,alltime%60);
           ClrDisplay(hdc, &rc1, MapRGB(hdc, 0,0,0));
           SetTextColor(hdc, MapRGB(hdc,255,255,255));
           DrawText(hdc, buff,-1,&rc1,DT_VCENTER|DT_CENTER);
           
           char *ss;
           int length1=strlen(filename);
           int length2=strlen("0:/srcdata/");
           if(strncpy(filename,"0:/srcdata/",length2))//比较前n个字符串，类似strcpy
           {
             ss = filename + length2;
           }
           ClrDisplay(hdc, &rc2, MapRGB(hdc, 0,0,0));
           x_mbstowcs_cp936(buff, ss, 200);
           DrawText(hdc, buff,-1,&rc2,DT_VCENTER|DT_CENTER); 
           
           x_wsprintf(buff, L"帧率：%dFPS/s", avi_fps);
           ClrDisplay(hdc, &rc4, MapRGB(hdc, 0,0,0));
           DrawText(hdc, buff,-1,&rc4,DT_VCENTER|DT_LEFT);            
           ClrDisplay(hdc, &rc3, MapRGB(hdc, 0,0,0));
           x_wsprintf(buff, L"分辨率： %d*%d ", img_w, img_h);
           DrawText(hdc, buff,-1,&rc3,DT_VCENTER|DT_RIGHT); 
           
			  ReleaseDC(hwnd_AVI,hdc);
	#endif
			}
			
      while(timeout==0)
      {   
				rt_thread_delay(1); //不要死等，最好用信号量.				
        //GUI_msleep(5);
      }      
      
      //DeleteDC(hdc_mem);
      //ReleaseDC(hwnd, hdc);
      timeout=0;
    }//显示帧
    else if(Strtype==T_auds)//音频输出
    { 
      uint8_t i;
      audiosavebuf++;
      if(audiosavebuf>3)
			{
				audiosavebuf=0;
			}	
      do
      {
				//rt_thread_delay(1); 
        i=audiobufflag;
        if(i)
					i--;
        else 
					i=3; 

      }while(audiobufflag==i);
      AVI_DEBUG("S\n");
      f_read(&fileR,Sound_buf[audiosavebuf],Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息
      AVI_DEBUG("E\n");
      pbuffer=Sound_buf[audiosavebuf];      
    }
    else break;
					   	
  }
     else{
         pos = fileR.fptr;
//         //根据进度条调整播放位置				
         temp11=SendMessage(wnd_time, SBM_GETVALUE, NULL, NULL); 
         time_sum = fileR.fsize/alltime*(temp11*alltime/255-cur_time);//跳过多少数据
         //如果当前文件指针未到最后
        	if(pos<fileR.fsize)pos+=time_sum; 
         //如果文件指针到了最后30K内容
			if(pos>(fileR.fsize-1024*30))
			{
				pos=fileR.fsize-1024*30;
			}
         f_lseek(&fileR,pos);
         AVI_DEBUG("S\n");
         f_read(&fileR,Frame_buf,1024*30,&BytesRD);
         AVI_DEBUG("E\n");
         if(pos == 0)
            mid=Search_Movi(Frame_buf);//寻找movi ID
         else 
            mid = 0;
         mid += Search_Fram(Frame_buf);
         Strtype=MAKEWORD(pbuffer+mid+2);//流类型
         Strsize=MAKEDWORD(pbuffer+mid+4);//流大小
         if(Strsize%2)Strsize++;//奇数加1
         f_lseek(&fileR,pos+mid+8);//跳过标志ID  
         AVI_DEBUG("S\n");
         f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息   
         AVI_DEBUG("E\n");
         avi_chl = 0;    
     }
     
    
    //判断下一帧的帧内容 
    Strtype=MAKEWORD(pbuffer+Strsize+2);//流类型
    Strsize=MAKEDWORD(pbuffer+Strsize+4);//流大小									
    if(Strsize%2)Strsize++;//奇数加1		  
  
     }
  
 

  sw_flag = 0;
  I2S_Play_Stop();
  I2S_Stop();		/* 停止I2S录音和放音 */
	wm8978_Reset();	/* 复位WM8978到复位状态 */
  TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,DISABLE); //使能定时器3
  f_close(&fileR);
}

void MUSIC_I2S_DMA_TX_Callback(void)
{
  audiobufflag++;
  if(audiobufflag>3)
	{
		audiobufflag=0;
	}
	
	if(DMA1_Stream4->CR&(1<<19)) //当前读取Memory1数据
	{
		DMA_MemoryTargetConfig(DMA1_Stream4,(uint32_t)Sound_buf[audiobufflag], DMA_Memory_0);
	}
	else
	{
		DMA_MemoryTargetConfig(DMA1_Stream4,(uint32_t)Sound_buf[audiobufflag], DMA_Memory_1); 
	}
}

/**
  * @brief  通用定时器3中断初始化
  * @param  period : 自动重装值。
  * @param  prescaler : 时钟预分频数
  * @retval 无
  * @note   定时器溢出时间计算方法:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=定时器工作频率,为SystemCoreClock/2=90,单位:Mhz
  */
static void TIM3_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=period;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  定时器3中断服务函数
  * @param  无
  * @retval 无
  */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{    
    timeout=1;
	}
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位	
}
