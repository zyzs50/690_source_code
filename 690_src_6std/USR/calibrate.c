#include "calibrate.h"
#include "ad.h"
#include "motor.h"
#include "math.h"					        //数学函数库
#include "usb_config.h"

u8 SelectCalib = 2;//选择哪种标准物质的靶值, 进行线性测试时，选用A是过不了的，要选用B

// u16 AD_Alllight[10][8]={0};		//保存光源直射时8通道AD值(10次走板共80个AD值),0:对应A ,7：对应F
// u16 AD_Halflight[10][16]={0}; //保存有检测物时16孔
float AD_Alllight[10][8]={0};		//保存光源直射时8通道AD值(10次走板共80个AD值),0:对应A ,7：对应F
float AD_Halflight[10][24]={0}; //保存有检测物时16孔: 对应6个标准物质

float ABS_Array[10][24]={0.0};		  //保存24孔(2,4,6,8,10,12列前4孔或1,2列全部孔)的ABS值（数组第一维表示走板次数，二维表示检测物孔数）
float ABS_Aver_Array[24]={0.0};   	//保存24孔(2,4,6,8,10,12列前4孔或1,2列全部孔)的ABS平均值

float ABS_Array2[10][24]={0.0};		  //修正后的值   ---这个是测试用的，后期如果程序空间不够，可以去掉
float ABS_Aver_Array2[24]={0.0};   	//修正后的值   ---这个是测试用的，后期如果程序空间不够，可以去掉

float CV[24];				  //保存液体样本的CV值
float SD[24];				  //保存液体样本的SD值
float Accuration[24];		  // 保存样本准确性
float Differ[6];			  //保存孔间均值
float Differ_betweenHoles[6];//保存孔间差值
float ABS_Send[146];		  //保存要发送的96个值

float Slope_405nm[8][6]={0};      //保存405nm各通道K（斜率）大小(数组第一维表示通道数，第二维表示每个通道K值个数)
float Slope_450nm[8][6]={0};	    //保存450nm各通道K（斜率）大小(数组第一维表示通道数，第二维表示每个通道K值个数)
float Slope_492nm[8][6]={0};	    //保存492nm各通道K（斜率）大小(数组第一维表示通道数，第二维表示每个通道K值个数)
float Slope_630nm[8][6]={0};	    //保存630nm各通道K（斜率）大小(数组第一维表示通道数，第二维表示每个通道K值个数)
float Slope_Ext01nm[8][6]={0};	    //扩展滤光片01
float Slope_Ext02nm[8][6]={0};	    //扩展滤光片02
float Slope_Ext03nm[8][6]={0};	    //扩展滤光片03
float Slope_Ext04nm[8][6]={0};	    //扩展滤光片04

float Intercept_405nm[8][6]={0};   //保存405nm各通道b值（截距）大小(数组第一维表示通道数，第二维表示每个通道b值个数)
float Intercept_450nm[8][6]={0};   //保存450nm各通道b值（截距）大小(数组第一维表示通道数，第二维表示每个通道b值个数)
float Intercept_492nm[8][6]={0};   //保存492nm各通道b值（截距）大小(数组第一维表示通道数，第二维表示每个通道b值个数)
float Intercept_630nm[8][6]={0};   //保存630nm各通道b值（截距）大小(数组第一维表示通道数，第二维表示每个通道b值个数)
float Intercept_Ext01nm[8][6]={0};	  //扩展滤光片01
float Intercept_Ext02nm[8][6]={0};	  //扩展滤光片02
float Intercept_Ext03nm[8][6]={0};	  //扩展滤光片03
float Intercept_Ext04nm[8][6]={0};	  //扩展滤光片04

float TestValue_405nm[8][5]={0};	 //保存405nm波长下0.2A,0.5A和1.0A,1.5A,2.0A,标准物质的8通道测量值
float TestValue_450nm[8][5]={0};	 //保存450nm波长下0.2A,0.5A和1.0A,1.5A,2.0A,标准物质的8通道测量值
float TestValue_492nm[8][5]={0};	 //保存492nm波长下0.2A,0.5A和1.0A,1.5A,2.0A,标准物质的8通道测量值
float TestValue_630nm[8][5]={0};	 //保存630nm波长下0.2A,0.5A和1.0A,1.5A,2.0A,标准物质的8通道测量值	
float TestValue_Ext01nm[8][5]={0};	  //扩展滤光片01
float TestValue_Ext02nm[8][5]={0};	  //扩展滤光片02
float TestValue_Ext03nm[8][5]={0};	  //扩展滤光片03
float TestValue_Ext04nm[8][5]={0};	  //扩展滤光片04

float StandardValue_405nm[7]  = {0,0.231,0.504,0.941,1.451,2.0,3.0};//{0,0.228,0.500,0.936,1.444};	   //405nm波长下光谱中性滤光片的靶值
float StandardValue_450nm[7]  = {0,0.218,0.472,0.867,1.341,2.0,3.0};//{0,0.217,0.471,0.864,1.336};		 //450nm波长下光谱中性滤光片的靶值
float StandardValue_492nm[7]  = {0,0.204,0.471,0.935,1.441,2.0,3.0};//{0,0.201,0.470,0.933,1.436};		 //4925nm波长下光谱中性滤光片的靶值
float StandardValue_630nm[7]  = {0,0.210,0.482,1.137,1.697,2.0,3.0};//{0,0.208,0.480,1.134,1.693};		 //630nm波长下光谱中性滤光片的靶值
float StandardValue_Ext01nm[7]= {0,0.210,0.482,1.137,1.697,2.0,3.0};	  //扩展滤光片01
float StandardValue_Ext02nm[7]= {0,0.210,0.482,1.137,1.697,2.0,3.0};	  //扩展滤光片02
float StandardValue_Ext03nm[7]= {0,0.210,0.482,1.137,1.697,2.0,3.0};	  //扩展滤光片03
float StandardValue_Ext04nm[7]= {0,0.210,0.482,1.137,1.697,2.0,3.0};	  //扩展滤光片04

/*证书编号：126300413*/
u8 targetValueA[96] = 
{
  0x02,0x28, 0x05,0x14, 0x09,0x41, 0x14,0x52, 0x20,0x00, 0x30,0x00, // 405
  0x02,0x13, 0x04,0x81, 0x09,0x67, 0x13,0x43, 0x20,0x00, 0x30,0x00, // 450
  0x01,0x99, 0x04,0x78, 0x09,0x34, 0x14,0x40, 0x20,0x00, 0x30,0x00, // 492
  0x02,0x06, 0x04,0x86, 0x11,0x36, 0x16,0x97, 0x20,0x00, 0x30,0x00,	// 630							   
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78, 0x20,0x00, 0x30,0x00,	// 扩展01
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78, 0x20,0x00, 0x30,0x00,	// 扩展02
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78, 0x20,0x00, 0x30,0x00,	// 扩展03
	0x02,0x07, 0x04,0x96, 0x11,0x11, 0x16,0x75, 0x20,0x00, 0x30,0x00  // 扩展04
};

/*证书编号：126300414*/
// u8 targetValueB[96] = 
// {														 
//   0x02,0x31, 0x05,0x14, 0x11,0x00, 0x14,0x46,  // 405
//   0x02,0x20, 0x04,0x86, 0x10,0x31, 0x13,0x39,  // 450
//   0x02,0x05, 0x04,0x99, 0x10,0x54, 0x14,0x37,  // 492
//   0x02,0x13, 0x05,0x67, 0x10,0x80, 0x16,0x95,	// 630							   
// 	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	// 扩展01
// 	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	// 扩展02
// 	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	// 扩展03
// 	0x02,0x07, 0x04,0x96, 0x11,0x11, 0x16,0x75   // 扩展04
// };
u8 targetValueB[96] = 
{														 
  0x02,0x28, 0x05,0x04, 0x10,0x91, 0x14,0x33, 0x20,0x00, 0x30,0x00,   // 405
  0x02,0x14, 0x04,0x74, 0x10,0x20, 0x13,0x28, 0x20,0x00, 0x30,0x00,  // 450
  0x01,0x99, 0x04,0x88, 0x10,0x42, 0x14,0x26, 0x20,0x00, 0x30,0x00,  // 492
  0x02,0x05, 0x05,0x56, 0x10,0x63, 0x16,0x83,	0x20,0x00, 0x30,0x00, // 630							   
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // 扩展01
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // 扩展02
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // 扩展03
	0x02,0x07, 0x04,0x96, 0x11,0x11, 0x16,0x75, 0x20,0x00, 0x30,0x00  // 扩展04
};

/*证书编号：126300413*/
u8 targetValueC[96] = 
{
  0x02,0x28, 0x05,0x14, 0x09,0x41, 0x14,0x52, 0x20,0x00, 0x30,0x00,  // 405
  0x02,0x13, 0x04,0x81, 0x09,0x67, 0x13,0x43, 0x20,0x00, 0x30,0x00,  // 450
  0x01,0x99, 0x04,0x78, 0x09,0x34, 0x14,0x40, 0x20,0x00, 0x30,0x00,  // 492
  0x02,0x06, 0x04,0x86, 0x11,0x36, 0x16,0x97,	0x20,0x00, 0x30,0x00,  // 630							   
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // 扩展01
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // 扩展02
	0x02,0x05, 0x04,0x87, 0x11,0x20, 0x16,0x78,	0x20,0x00, 0x30,0x00, // 扩展03
	0x02,0x07, 0x04,0x96, 0x11,0x11, 0x16,0x75, 0x20,0x00, 0x30,0x00   // 扩展04
};

u8 Wavelength_Flag = 0; //测样时，是否使用双波长， 1：使用双波长， 0：使用单波长

// u16 AD_ALLlight_Single[8]={0};          //保存单波长下光源直射时8通道AD值
// u16 AD_ALLlight_Double[8]={0};	        //保存次波长（双波长模式）光源直射时8通道AD值			  
float AD_ALLlight_Single[8]={0};          //保存单波长下光源直射时8通道AD值
float AD_ALLlight_Double[8]={0};	        //保存次波长（双波长模式）光源直射时8通道AD值			  

// u16 AD_Halflight_Single[12][8]={0};     //保存单波长下有检测物时96孔AD值（8行12列）
// u16 AD_Halflight_Double[12][8]={0};     //保存次波长（双波长模式）下有检测物时96孔AD值（8行12列）
float AD_Halflight_Single[12][8]={0};     //保存单波长下有检测物时96孔AD值（8行12列）
float AD_Halflight_Double[12][8]={0};     //保存次波长（双波长模式）下有检测物时96孔AD值（8行12列）

float  ABS_SingleWave0[12][8]={0.0};	//未修正过的ABS   
float  ABS_DoubleWave0[12][8]={0.0};	//未修正过的ABS

float  ABS_SingleWave[12][8]={0.0};		    //保存单波长下96孔的ABS值及显示值（8行12列）
float  ABS_DoubleWave[12][8]={0.0};		    //保存次波长（双波长模式）下96孔的ABS显示值（8行12列）


/****************************************************************************
* 名    称：void ADC0_Sample_ALLlight(u16 *address,u8 mode)
* 功    能：
* 入口参数：u16 *address  : 存放AD值的地址
* 入口参数：u8 mode  :采集模式 , 0:ABCD 列， 1：EFGH列， 2：ABCDEFGH 列
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void ADC0_Sample_Mode(float *address,u8 mode)
{
	u8 i=0;
  u8 count = 0;
	u8 channel = 0;
// 	u16 temp[8] = {0};
	//采集电压
	if(mode==0)//ABCD
	{
		channel = 7;
		count = 4;
	}
	else if(mode ==1)//EFGH
	{
		channel = 3;
		count = 4;
	}
	else if(mode ==2)//ABCDEFGH
	{
		channel = 7;
		count = 8;
	}

	for(i=0;i<count;i++)
	{		
		*address = Get_Adc_Average(channel,800);
// 		temp[i] = *address;
		address += 1; 
		channel -= 1;
// 		delay_ms(5);
	}
	
// 	for(i=0;i<count;i++)
// 	{
// 		printf("temp[%d]= %d\r\n",i,temp[i]);
// 	}
}

/****************************************************************************
* 名    称：u8 AD_Loop_Sample_K_B(u8 mode,u8 type)
* 功    能：
* 入口参数：u8 mode  :采集模式 , 0:ABCD 列， 1：EFGH列， 2：ABCDEFGH 列
* 入口参数：u8 type  :0：光路性能检验,  1：液体样本检验
* 出口参数：
* 说    明：次函数的走板测试用于 测试标准物质
* 调用方法：
****************************************************************************/
u8 AD_Loop_Sample_K_B(u8 mode,u8 type)
{
 	u8 Response_CMD[3]={0x68,0x00,0xff};
// 	u16 *address = 0;
	float *address = 0;
	u8 ret =0;
	u8 col =0;
	uint8_t loop = 0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint16_t k = 0;
	uint16_t pluse_Max = 600;
	uint8_t count = 100;
	
	//10次走板测通道AD值
	for(loop=0;loop<10;loop++)
	{
		pluse_Max = 600;
		count     = 100;
		//指向保存AD直射时的地址
		address = &AD_Alllight[loop][0];

		//酶标板复位
		ret = Plate_Motor_Reset(1);
		if(ret)//复位失败
		{
			for(k=0;k<200;k++)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_2);
				delay_us(250);
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);
				delay_us(250);
			}
			Response_CMD[1]=ret;
			USB_TxWrite(Response_CMD, 3);//发送响应
			return 1;
		}
		delay_ms(10);
		
		//采集8通道光源直射AD值
		ADC0_Sample_Mode(address,2);
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);//使能电机
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向进仓
		
		//让定位片进入光耦
		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//光耦挡住是1， 通过为0
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			pluse_Max = pluse_Max - 1;
			count = count - 1;
		}
		
		if(count==0)
		{
			//蜂鸣器鸣叫一声
			for(k=0;k<200;k++)
			{
				GPIO_SetBits(GPIOD,GPIO_Pin_2);
				delay_us(250);
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);
				delay_us(250);
			}
			
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			Response_CMD[1]=count;
			USB_TxWrite(Response_CMD, 3);//发送响应
			return 2;//同步带定位光耦有问题
		}	
			
		address = &AD_Halflight[loop][0];
		
		if(type == 0)
		{
// 			col = 8;//光路性能检验 测 8 列 
			col = 12;//光路性能检验 测 12 列, 1,3,5,7,9,11列 分别存放 0.2,0.5,1.0.1.5,2.0,3.0
		}
		else if(type == 1)
		{
			col = 2; //液体样本检验测2列
		}
		//正式开始定位
		for(i=0;i<col;i++)
		{
			//这里7个脉冲式为了让挡片移动一点距离，让定位槽偏离光耦，方便检测下一个定位槽
			for(j=0;j<7;j++)
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				pluse_Max = pluse_Max - 1;
			}
			
			while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//光耦挡住是1， 通过为0
			{
				GPIO_ResetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				GPIO_SetBits(GPIOB,GPIO_Pin_2);
				delay_ms(1);
				pluse_Max = pluse_Max - 1;
			}
			
			if(pluse_Max == 0)
			{
			  //蜂鸣器鸣叫一声
				for(k=0;k<200;k++)
				{
					GPIO_SetBits(GPIOD,GPIO_Pin_2);
					delay_us(250);
					GPIO_ResetBits(GPIOD,GPIO_Pin_2);
					delay_us(250);
				}
				
				GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
				Response_CMD[1]=pluse_Max;
				USB_TxWrite(Response_CMD, 3);//发送响应
				return 3;//定位失败
			}

			if(type==0)
			{
				//1,3,5,7列存放滤光片标准物质
				if((i==1)||(i==3)||(i==5)||(i==7)||(i==9)||(i==11))
				{
					delay_ms(500);					
					ADC0_Sample_Mode(address,mode); //4 通道 4列 为 16个数据
					address = address +4;
				}
			}
			else if(type==1)
			{
				//0,1列存放液体样本
				ADC0_Sample_Mode(address,2);//采集8通道 ，--8通道2列为 16个数据
				address = address +8;
			}
		}
	}
	ret = 0;
	//酶标板复位
	ret = Plate_Motor_Reset(1);
	if(ret)//复位失败
	{
		//蜂鸣器鸣叫一声
		for(k=0;k<200;k++)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			delay_us(250);
		}
		
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 4;
	}
	delay_ms(10);

	return 0;
}


/****************************************************************************
* 名    称：void Data_Handling_Optical(u8 mode,u8 type)
* 功    能：计算10次走板得到的 16 孔 ABS 值
* 入口参数：mode   0：ABCD通道 ,     1：EFGH通道  
* 入口参数：type   0：光路性能检验,  1：液体样本检验
* 出口参数：
* 说    明： 光路性能检验，液体样本检验  都是得到16个数据， 前者是 4个通道 4列， 后者是8个通道2列
* 调用方法：
****************************************************************************/
void Data_Handling_Optical(u8 mode,u8 type)
{
	u8 i=0;
	u8 j=0;
	u8 col=0;
	
	u16 light_in = 0;  //直射输入
	u16 light_out = 0; //采集输入
	float A_BS = 0.0;
	
	if(type == 0) //0：光路性能检验,  
	{
		//求10次走板，每次 24孔的ABS
		if(mode == 0)//ABCD列
		{
			for(i=0;i<10;i++)//10次走板
			{
				for(col=0;col<6;col++)//6列
				{
					for(j=0;j<4;j++)//4孔
					{
						light_in  = AD_Alllight[i][j];
						light_out = AD_Halflight[i][j+col*4];
						A_BS      = (float)light_in / (float)light_out;     //必须强制转为float型，否则将导致计算对数不正确
						A_BS      = log10(A_BS);                 //计算吸光度
						ABS_Array[i][j+col*4] = A_BS;			    //保存ABS值到数组ABS_Array[]中
					}
				}
			}
		}
		else if(mode == 1)//EFGH列
		{
			for(i=0;i<10;i++)//10次走板
			{
				for(col=0;col<6;col++)//4列
				{
					for(j=0;j<4;j++)//4孔
					{
						light_in  = AD_Alllight[i][j+4];
						light_out = AD_Halflight[i][j+col*4];
						A_BS      = (float)light_in / (float)light_out;     //必须强制转为float型，否则将导致计算对数不正确
						A_BS      = log10(A_BS);                 //计算吸光度
						ABS_Array[i][j+col*4] = A_BS;			    //保存ABS值到数组ABS_Array[]中
					}
				}
			}
		}
	}
	else if(type == 1) //1：液体样本检验
	{
			for(i=0;i<10;i++)//10次走板
			{
				for(col=0;col<2;col++)//2列
				{
					for(j=0;j<8;j++)//8孔
					{
						light_in  = AD_Alllight[i][j];
						light_out = AD_Halflight[i][j+col*8];
						A_BS      = (float)light_in / (float)light_out;     //必须强制转为float型，否则将导致计算对数不正确
						A_BS      = log10(A_BS);                 //计算吸光度
						ABS_Array[i][j+col*8] = A_BS;			    //保存ABS值到数组ABS_Array[]中
					}
				}
			}
	}
	
	//求10次走板的平均ABS
	for(i=0;i<24;i++)
	{
		A_BS = 0.0;
		for(j=0;j<10;j++)
		{
			A_BS += ABS_Array[j][i];
		}
		A_BS = A_BS/10;
		ABS_Aver_Array[i] = A_BS;
	}
	
}


/****************************************************************************
* 名    称：void Bcdvert(u8 sour[], float dest[], u8 wavel)
* 功    能：
* 入口参数：计算标准物质的靶值
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
u8 Bcdvert(u8 sour[], float dest[], u8 wavel)			   //将收到的数据转化为float型
{	  
  int tempdata,i;
	for(i=0; i<12; i++)
	{
		tempdata = sour[wavel*12 + i++];
		tempdata = (tempdata<<8) + sour[wavel*12 +i];
		dest[i/2 + 1] = (float)((tempdata>>12)&0x0f) + (float)((tempdata >> 8)&0x0f)/10 + (float)((tempdata >> 4)&0x0f)/100 + (float)((tempdata&0x0f))/1000;  
	}	
	return 0;
}


/****************************************************************************
* 名    称：void CV_SD_Function(float X[10][16],float x[16],u8 n)
* 功    能：
* 入口参数：求CV、SD值（CV=SD/x，SD为标准差，x为均值）
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void CV_SD_Function(float X[10][24],float x[24],u8 n)
{
	 u8 i,j;
	 u8 m = 0;
	 //float CV,SD;
	 float a = 0.0;
	 for(i = 0;i < 24;i ++)
	 {
	   for(j=0;j<n;j++)
	 	 {
		   a = a + (X[j][i] - x[i] ) * (X[j][i] - x[i] );			//求累加和
		 }
		 SD[m] = sqrt(a / (n - 1));								    //求SD值
	   CV[m] = SD[m] / x[i];										 //求CV值
		 m ++;
		 a = 0.0;	  
	 }
	 m = 0;
}

	
/****************************************************************************
* 名    称：void CV_SD_Function1(float X[16],float x[4],u8 n)
* 功    能：
* 入口参数：求孔间差
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void CV_SD_Function1(float X[24],float x[6],u8 n)
{
	 u8  i,j;
	 u8  m = 0;
	 //float CV,SD;
	 float a = 0.0;
	 float std;
	 i = 0;
	 for(j=0;j<24;j++)
	 {	     
	   a = a + (X[j] - x[i] ) * (X[j] - x[i] );
	 	 if(j == 3 || j == 7 || j == 11 || j == 15 || j == 19 || j == 23)
		 {
		 	 std = sqrt(a / (n - 1));
			 Differ_betweenHoles[m]	= std / x[i];				 //求孔间差CV值
			 m ++;
			 i ++;
			 a = 0.0;
		 }
	 }
}

/****************************************************************************
* 名    称：void X_Y_Kb(u8 Wavelength_Main,u8 Number_Channel)
* 功    能：用最小二乘法求K、b值（每通道4个K、b值）及进行ABS显示值的修正、ABCD或EFGH通道的准确度、重复性、通道差的计算等
* 入口参数：Wavelength_Main ：使用的波长
* 入口参数：Number_Channel  ：0:ABCD 列， 1：EFGH列，
* 出口参数：
* 说    明：光路性能检验 用来计算出 K,B值， 而液体样本检验 是用 K,B 值来修正吸光度
* 调用方法：
****************************************************************************/
void X_Y_Kb(u8 Wavelength_Main,u8 Number_Channel)
{
	u8 y = 0;
	u8 i = 0;		 										//临时变量
	u8 j = 0;
	u8 k = 0;
	u8 m = 0;		 
	u8 n = 2;		 
	u8 h = 0;		 
	float a1 = 0;												 //定义变量保存ABS平均值累加和
	float a2 = 0;												 //定义变量保存ABS平方值累加和
	float c1 = 0;												 //定义变量保存各波长下光谱中性滤光片标准值的累加和
	float c2 = 0;												 //定义变量保存ABS平均值和光谱中性滤光片标准值乘积的累加和
	float result = 0;											 //累加和变量
	u8 Num_Test = 0;											 //走板次数
	u8 Num_Glass;												 //光谱中性滤光片的种类数（共4种：0.2A,0.5A,1.0A,1.5A
	u8 Num_PerGlass;											 //每种中性滤光片占孔位数（共4孔）

	if(SelectCalib == 0x01)											 //靶值A
	{
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_405nm, 0);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_450nm, 1);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_492nm, 2);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_630nm, 3);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_Ext01nm, 4);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_Ext02nm, 5);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_Ext03nm, 6);
	 Bcdvert((u8 *)targetValueA, (float *)StandardValue_Ext04nm, 7);
	}
	else if(SelectCalib == 0x02)										 //靶值B
	{
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_405nm, 0);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_450nm, 1);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_492nm, 2);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_630nm, 3);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_Ext01nm, 4);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_Ext02nm, 5);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_Ext03nm, 6);
	 Bcdvert((u8 *)targetValueB, (float *)StandardValue_Ext04nm, 7);
	}
	else																 //靶值C
	{
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_405nm, 0);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_450nm, 1);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_492nm, 2);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_630nm, 3);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_Ext01nm, 4);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_Ext02nm, 5);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_Ext03nm, 6);
	 Bcdvert((u8 *)targetValueC, (float *)StandardValue_Ext04nm, 7);
	}
	
	for(Num_Glass=0;Num_Glass<6;Num_Glass++)						 	 //计算4种滤光片0.2 ，0.5，1.0，1.5, 2.0， 3.0
	{
		for(Num_PerGlass=0;Num_PerGlass<8;Num_PerGlass++)			  //计算每个通道的6个K、b值
		{
			if(Num_PerGlass != 0)										  //求吸光度不为0的累加和
			{
					a1 += ABS_Aver_Array[k];								  //累加ABS平均值
					a2 += ABS_Aver_Array[k] * ABS_Aver_Array[k];			  //求ABS平方值累加和
			}
			else
			{
				 a1 = 0;												  //求吸光度为0的累加和
				 a2 = 0;
			}
			if(Wavelength_Main == 1)                     // 405nm波长 
			{ 														  //求405nm波长下光谱中性滤光片标准值的累加和
					c1 +=	StandardValue_405nm[i];
					c2 += ABS_Aver_Array[k] * StandardValue_405nm[i];	 //求405nm波长下ABS平均值和光谱中性滤光片标准值乘积的累加和
			}
			else if(Wavelength_Main == 2)			                  //450nm波长
			{ 
					c1 +=	StandardValue_450nm[i];									  //求450nm波长下光谱中性滤光片标准值的累加和
					c2 += ABS_Aver_Array[k] * StandardValue_450nm[i];				  //求450nm波长下ABS平均值和光谱中性滤光片标准值乘积的累加和
			}
			else if(Wavelength_Main == 3)                // 492nm波长 
			{ 
					c1 +=	StandardValue_492nm[i];									  //求492nm波长下光谱中性滤光片标准值的累加和
					c2 += ABS_Aver_Array[k] * StandardValue_492nm[i];				  //求492nm波长下ABS平均值和光谱中性滤光片标准值乘积的累加和
			}
			else if(Wavelength_Main == 4)				  //630nm波长
			{ 
					c1 +=	StandardValue_630nm[i];									  //求630nm波长下光谱中性滤光片标准值的累加和
					c2 += ABS_Aver_Array[k] * StandardValue_630nm[i];				  //求630nm波长下ABS平均值和光谱中性滤光片标准值乘积的累加和
			}
			else if(Wavelength_Main == 5)				  //扩展波长01
			{ 
					c1 +=	StandardValue_Ext01nm[i];									  //求扩展波长01波长下光谱中性滤光片标准值的累加和
					c2 += ABS_Aver_Array[k] * StandardValue_Ext01nm[i];				  //求扩展波长01波长下ABS平均值和光谱中性滤光片标准值乘积的累加和
			}
			else if(Wavelength_Main == 6)				  //扩展波长02
			{ 
					c1 +=	StandardValue_Ext02nm[i];									  //求扩展波长02波长下光谱中性滤光片标准值的累加和
					c2 += ABS_Aver_Array[k] * StandardValue_Ext02nm[i];				  //求扩展波长02波长下ABS平均值和光谱中性滤光片标准值乘积的累加和
			}
			else if(Wavelength_Main == 7)				  //扩展波长03
			{ 
					c1 +=	StandardValue_Ext03nm[i];									  //求扩展波长03波长下光谱中性滤光片标准值的累加和
					c2 += ABS_Aver_Array[k] * StandardValue_Ext03nm[i];				  //求扩展波长03波长下ABS平均值和光谱中性滤光片标准值乘积的累加和
			}
			else if(Wavelength_Main == 8)				  //扩展波长04
			{ 
					c1 +=	StandardValue_Ext04nm[i];									  //求扩展波长04波长下光谱中性滤光片标准值的累加和
					c2 += ABS_Aver_Array[k] * StandardValue_Ext04nm[i];				  //求扩展波长04波长下ABS平均值和光谱中性滤光片标准值乘积的累加和
			}
			
			
			
			if(Num_PerGlass != 0)									  //标准物质吸光度不为0
			{
					 k += 4;											  
			}
			i++;
			m++;
			if( m == 2 || m == 4 || m == 6 || m == 8 || m == 10 || m == 12 )				  //求各个波长的4个K、B值
			{														  
				if(Wavelength_Main == 1)                                       //405nm波长
				{
					if(Number_Channel == 0x01)									//EFGH通道
					{   
							Slope_405nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_405nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
// 							if(h != 3)
							if(h != 5)
							{
								TestValue_405nm[j+4][h] = ABS_Aver_Array[y];		//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
							}
					 }
					 else if(Number_Channel == 0x00)														//ABCD通道
					 {
							Slope_405nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_405nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
							if(h!= 5)
							{
								TestValue_405nm[j][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A,1.5,2.0测量值存入数组TestValue_405nm[][]中
							}
					 }
					 y += 4;																				//选择0.5A或1.0A的测量平均值
					 if(h == 5)
					 {																					
// 							y = y - 15;
							y = y - 23;
					 }	                                     	//一通道0.2A,0.5A,1.0A测量值存完后进入下一通道0.2A,0.5A,1.0A测量值的存取
				}
				else if(Wavelength_Main == 2)			                  //450nm波长
				{
					if(Number_Channel == 0x01)									//EFGH通道
					{   
							Slope_450nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_450nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
							if(h != 5)
							{
								TestValue_450nm[j+4][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中														
							}
					 }
					 else	if(Number_Channel == 0x00)																	//ABCE通道
					 {
							Slope_450nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_450nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
							if(h != 5)
							{
								TestValue_450nm[j][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 								y = y - 15;
								y = y - 23;
					 }																					//准备存0.5A,1.0A测试
				}
				else if(Wavelength_Main == 3)                               //492nm波长
				{
					if(Number_Channel == 0x01)									//EFGH通道
					{   
						Slope_492nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
						Intercept_492nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
						if(h != 5)
						{
							TestValue_492nm[j+4][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
						}
					}
					else if(Number_Channel == 0x00)														//ABCD通道
					{
						Slope_492nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
						Intercept_492nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
						if(h != 5)
						{
							TestValue_492nm[j][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
						}

					 }
					 y += 4;
					 if(h == 5)
					 {
// 								y = y - 15;
								y = y - 23;
					 }																					//准备存0.5A,1.0A测试
				} 
				else if(Wavelength_Main == 4)              //630nm
				{
					if(Number_Channel == 0x01)									//EFGH通道
					{   
						Slope_630nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
						Intercept_630nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
						if(h != 5)
						{
							TestValue_630nm[j+4][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCE通道
					 {
							Slope_630nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_630nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
							if(h != 5)
							{
								TestValue_630nm[j][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//准备存0.5A,1.0A测试
				}
				else if(Wavelength_Main == 5)              //扩展滤光片01
				{
					if(Number_Channel == 0x01)									//EFGH通道
					{   
						Slope_Ext01nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
						Intercept_Ext01nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
						if(h != 5)
						{
							TestValue_Ext01nm[j+4][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCE通道
					 {
							Slope_Ext01nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_Ext01nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
							if(h != 5)
							{
								TestValue_Ext01nm[j][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//准备存0.5A,1.0A测试
				}
				else if(Wavelength_Main == 6)              //扩展滤光片02
				{
					if(Number_Channel == 0x01)									//EFGH通道
					{   
						Slope_Ext02nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
						Intercept_Ext02nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
						if(h != 5)
						{
							TestValue_Ext02nm[j+4][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCE通道
					 {
							Slope_Ext02nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_Ext02nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
							if(h != 5)
							{
								TestValue_Ext02nm[j][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//准备存0.5A,1.0A测试
				}
				else if(Wavelength_Main == 7)              //扩展滤光片03
				{
					if(Number_Channel == 0x01)									//EFGH通道
					{   
						Slope_Ext03nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
						Intercept_Ext03nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
						if(h != 5)
						{
							TestValue_Ext03nm[j+4][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCE通道
					 {
							Slope_Ext03nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_Ext03nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
							if(h != 5)
							{
								TestValue_Ext03nm[j][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//准备存0.5A,1.0A测试
				}
				else if(Wavelength_Main == 8)              //扩展滤光片04
				{
					if(Number_Channel == 0x01)									//EFGH通道
					{   
						Slope_Ext04nm[j+4][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
						Intercept_Ext04nm[j+4][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
						if(h != 5)
						{
							TestValue_Ext04nm[j+4][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
						}
					 }
					 else if(Number_Channel == 0x00)															//ABCE通道
					 {
							Slope_Ext04nm[j][h]     = (n * c2 - a1 * c1) / (n * a2 - a1 * a1);		//求各个通道的K值(斜率)
							Intercept_Ext04nm[j][h] = (a2 * c1 - a1 * c2) / (n * a2 - a1 * a1);		//求各个通道的b值（截距）
							if(h != 5)
							{
								TestValue_Ext04nm[j][h] = ABS_Aver_Array[y];								//将0.2A,0.5A,1.0A测量值存入数组TestValue_405nm[][]中
							}
					 }
					 y += 4;
					 if(h == 5)
					 {
// 						 y = y - 15;
						 y = y - 23;
					 }																					//准备存0.5A,1.0A测试
				}
				k -= 4;															//取回上一次的值
				i --;
				h ++;															//取回上一次的值
				a1 = 0;															//清0
				a2 = 0;
				c1 = 0;
				c2 = 0;
			}												 
		}
		m = 0;						 			
		i = 0;
		h = 0;
		j ++;
		k = j;
	}
	
	j = 0;
	m = 0;
	n = 0;
	y = 0;
	
	
	
  //上面计算出了K ,B
  //此时用上面计算出来的K,B来修正测试得到的ABS	
	m = 0;
	n = 0;
	if(Wavelength_Main == 1)       //405nm波长
	{
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGH通道
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][1] + Intercept_405nm[m+4][1];	//修正0.2A标准物质ABS均值
		       }
					 else														//ABCD通道
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][1] + Intercept_405nm[m][1];	//修正0.2A标准物质ABS均值
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //修正0.5A标准物质ABS均值
		     {  
					 if(Number_Channel == 0x01)									//EFGH通道
		   	   {   
						 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m+4][1])										  //小于设定测量值
			   		 {
					  	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][1] + Intercept_405nm[m+4][1];			 //选用0.2-0.5A段的K，B值						  
					   }	
					   else																			                   //大于设定测量值
					   {
					  	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][2] + Intercept_405nm[m+4][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  else														   	//ABCD通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m][1])										  //小于设定测量值
			   		   {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][1] + Intercept_405nm[m][1];			 //选用0.2-0.5A段的K，B值						  
					      }	
					      else																	                   //大于设定测量值
					      {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][2] + Intercept_405nm[m][2];			 //选用0.5-1.0A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m+4][2])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][2] + Intercept_405nm[m+4][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][3] + Intercept_405nm[m+4][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m][2])										 //小于设定测量值
			   		   {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][2] + Intercept_405nm[m][2];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][3] + Intercept_405nm[m][3];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//修正1.5A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m+4][3])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][3] + Intercept_405nm[m+4][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][4] + Intercept_405nm[m+4][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m][3])										 //小于设定测量值
			   		   {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][3] + Intercept_405nm[m][3];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][4] + Intercept_405nm[m][4];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//修正2.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m+4][4])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][4] + Intercept_405nm[m+4][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][5] + Intercept_405nm[m+4][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_405nm[m][4])										 //小于设定测量值
			   		   {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][4] + Intercept_405nm[m][4];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][5] + Intercept_405nm[m][5];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else																					//修正3.0A标准物质ABS均值
				 {
				 	 if(Number_Channel == 0x01)									//EFGH通道
				 	 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m+4][5] + Intercept_405nm[m+4][5];				 //修正1.5A标准物质ABS均值
		       }
					 else														 //ABCD通道
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_405nm[m][5] + Intercept_405nm[m][5];				 //修正1.5A标准物质ABS均值
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	     }	         
	   }
		 m = 0;
	}
	else if(Wavelength_Main == 2)			                 //450nm波长
	{ 
		  for(Num_Test=0;Num_Test<10;Num_Test++)
	    {
		    for(i=0;i<24;i++)
		    {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGH通道
					 {
					    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][1] + Intercept_450nm[m+4][1];	//修正0.2A标准物质ABS均值
		       }
					 else														//ABCD通道
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][1] + Intercept_450nm[m][1];	//修正0.2A标准物质ABS均值
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //修正0.5A标准物质ABS均值
		     {  
					 if(Number_Channel == 0x01)									//EFGH通道
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m+4][1])										  //小于设定测量值
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][1] + Intercept_450nm[m+4][1];			 //选用0.2-0.5A段的K，B值						  
					    }	
					    else																			                   //大于设定测量值
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][2] + Intercept_450nm[m+4][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  else														   	//ABCD通道
					  {
					  	if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m][1])										  //小于设定测量值
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][1] + Intercept_450nm[m][1];			 //选用0.2-0.5A段的K，B值						  
					    }	
					    else																	                   //大于设定测量值
					    {
					  	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][2] + Intercept_450nm[m][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)						//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m+4][2])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][2] + Intercept_450nm[m+4][2];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][3] + Intercept_450nm[m+4][3];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  else															 //ABCF通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m][2])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][2] + Intercept_450nm[m][2];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][3] + Intercept_450nm[m][3];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)						//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m+4][3])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][3] + Intercept_450nm[m+4][3];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][4] + Intercept_450nm[m+4][4];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  else															 //ABCF通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m][3])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][3] + Intercept_450nm[m][3];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][4] + Intercept_450nm[m][4];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)						//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					     if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m+4][4])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][4] + Intercept_450nm[m+4][4];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][5] + Intercept_450nm[m+4][5];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  else															 //ABCF通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_450nm[m][4])										 //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][4] + Intercept_450nm[m][4];			 //选用0.5-1.0A段的K，B值						  
					     }	
					     else																				 //大于设定测量值
					     {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][5] + Intercept_450nm[m][5];			 //选用1.0-1.5A段的K，B值
					     }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	m = 0;
// 						  n = 0;
					  }
				 }
				 else																					//修正1.5A标准物质ABS均值
				 {
				 	 if(Number_Channel == 0x01)									//EFGH通道
				 	 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m+4][5] + Intercept_450nm[m+4][5];				 //修正1.5A标准物质ABS均值
		       }
					 else														 //ABCD通道
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_450nm[m][5] + Intercept_450nm[m][5];				 //修正1.5A标准物质ABS均值
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	     }	         
	   }
		 m = 0;
	 }
   else if(Wavelength_Main == 3)		                 //492nm波长 
   { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGH通道
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][1] + Intercept_492nm[m+4][1];	//修正0.2A标准物质ABS均值
		       }
					 else														//ABCD通道
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][1] + Intercept_492nm[m][1];	//修正0.2A标准物质ABS均值
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //修正0.5A标准物质ABS均值
		     {  
					 if(Number_Channel == 0x01)									//EFGH通道
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m+4][1])										  //小于设定测量值
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][1] + Intercept_492nm[m+4][1];			 //选用0.2-0.5A段的K，B值						  
					    }	
					    else																			                   //大于设定测量值
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][2] + Intercept_492nm[m+4][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  else														   	//ABCD通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m][1])										  //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][1] + Intercept_492nm[m][1];			 //选用0.2-0.5A段的K，B值						  
					      }	
					      else																	                   //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][2] + Intercept_492nm[m][2];			 //选用0.5-1.0A段的K，B值
					      }
					  }

					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	  m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m+4][2])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][2] + Intercept_492nm[m+4][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][3] + Intercept_492nm[m+4][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m][2])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][2] + Intercept_492nm[m][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][3] + Intercept_492nm[m][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					   m = 0;
						 n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m+4][3])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][3] + Intercept_492nm[m+4][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][4] + Intercept_492nm[m+4][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m][3])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][3] + Intercept_492nm[m][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][4] + Intercept_492nm[m][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					   m = 0;
						 n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m+4][4])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][4] + Intercept_492nm[m+4][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][5] + Intercept_492nm[m+4][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_492nm[m][4])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][4] + Intercept_492nm[m][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][5] + Intercept_492nm[m][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					   m = 0;
						 n = 0;
					  }
				 }
				 else																					//修正1.5A标准物质ABS均值
				 {
				 	 if(Number_Channel == 0x01)									//EFGH通道
				 	 {
					    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m+4][5] + Intercept_492nm[m+4][5];				 //修正1.5A标准物质ABS均值
		       }
					 else														 //ABCD通道
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_492nm[m][5] + Intercept_492nm[m][5];				 //修正1.5A标准物质ABS均值
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	     }	         
	   }
		 m = 0;
	 }
	 else if(Wavelength_Main == 4)				//630nm波长
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGH通道
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][1] + Intercept_630nm[m+4][1];	//修正0.2A标准物质ABS均值
		       }
					 else														//ABCD通道
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][1] + Intercept_630nm[m][1];	//修正0.2A标准物质ABS均值
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //修正0.5A标准物质ABS均值
		     {  
					 if(Number_Channel == 0x01)									//EFGH通道
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m+4][1])										  //小于设定测量值
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][1] + Intercept_630nm[m+4][1];			 //选用0.2-0.5A段的K，B值						  
					    }	
					    else																			                   //大于设定测量值
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][2] + Intercept_630nm[m+4][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  else														   	//ABCD通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m][1])										  //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][1] + Intercept_630nm[m][1];			 //选用0.2-0.5A段的K，B值						  
					      }	
					      else																	                   //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][2] + Intercept_630nm[m][2];			 //选用0.5-1.0A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m+4][2])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][2] + Intercept_630nm[m+4][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][3] + Intercept_630nm[m+4][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m][2])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][2] + Intercept_630nm[m][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][3] + Intercept_630nm[m][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m+4][3])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][3] + Intercept_630nm[m+4][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][4] + Intercept_630nm[m+4][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m][3])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][3] + Intercept_630nm[m][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][4] + Intercept_630nm[m][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m+4][4])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][4] + Intercept_630nm[m+4][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][5] + Intercept_630nm[m+4][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_630nm[m][4])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][4] + Intercept_630nm[m][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][5] + Intercept_630nm[m][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//修正1.5A标准物质ABS均值
				 {
				 	 if(Number_Channel == 0x01)									//EFGH通道
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m+4][5] + Intercept_630nm[m+4][5];				 //修正1.5A标准物质ABS均值
		       }
					 else														 //ABCD通道
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_630nm[m][5] + Intercept_630nm[m][5];				 //修正1.5A标准物质ABS均值
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 else if(Wavelength_Main == 5)				//扩展波长01
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGH通道
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][1] + Intercept_Ext01nm[m+4][1];	//修正0.2A标准物质ABS均值
		       }
					 else														//ABCD通道
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][1] + Intercept_Ext01nm[m][1];	//修正0.2A标准物质ABS均值
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //修正0.5A标准物质ABS均值
		     {  
					 if(Number_Channel == 0x01)									//EFGH通道
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m+4][1])										  //小于设定测量值
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][1] + Intercept_Ext01nm[m+4][1];			 //选用0.2-0.5A段的K，B值						  
					    }	
					    else																			                   //大于设定测量值
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][2] + Intercept_Ext01nm[m+4][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  else														   	//ABCD通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m][1])										  //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][1] + Intercept_Ext01nm[m][1];			 //选用0.2-0.5A段的K，B值						  
					      }	
					      else																	                   //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][2] + Intercept_Ext01nm[m][2];			 //选用0.5-1.0A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m+4][2])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][2] + Intercept_Ext01nm[m+4][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][3] + Intercept_Ext01nm[m+4][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m][2])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][2] + Intercept_Ext01nm[m][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][3] + Intercept_Ext01nm[m][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m+4][3])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][3] + Intercept_Ext01nm[m+4][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][4] + Intercept_Ext01nm[m+4][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m][3])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][3] + Intercept_Ext01nm[m][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][4] + Intercept_Ext01nm[m][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m+4][4])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][4] + Intercept_Ext01nm[m+4][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][5] + Intercept_Ext01nm[m+4][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext01nm[m][4])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][4] + Intercept_Ext01nm[m][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][5] + Intercept_Ext01nm[m][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//修正1.5A标准物质ABS均值
				 {
				 	 if(Number_Channel == 0x01)									//EFGH通道
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m+4][5] + Intercept_Ext01nm[m+4][5];				 //修正1.5A标准物质ABS均值
		       }
					 else														 //ABCD通道
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext01nm[m][5] + Intercept_Ext01nm[m][5];				 //修正1.5A标准物质ABS均值
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 else if(Wavelength_Main == 6)				//扩展波长02
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGH通道
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][1] + Intercept_Ext02nm[m+4][1];	//修正0.2A标准物质ABS均值
		       }
					 else														//ABCD通道
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][1] + Intercept_Ext02nm[m][1];	//修正0.2A标准物质ABS均值
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //修正0.5A标准物质ABS均值
		     {  
					 if(Number_Channel == 0x01)									//EFGH通道
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m+4][1])										  //小于设定测量值
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][1] + Intercept_Ext02nm[m+4][1];			 //选用0.2-0.5A段的K，B值						  
					    }	
					    else																			                   //大于设定测量值
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][2] + Intercept_Ext02nm[m+4][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  else														   	//ABCD通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m][1])										  //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][1] + Intercept_Ext02nm[m][1];			 //选用0.2-0.5A段的K，B值						  
					      }	
					      else																	                   //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][2] + Intercept_Ext02nm[m][2];			 //选用0.5-1.0A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m+4][2])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][2] + Intercept_Ext02nm[m+4][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][3] + Intercept_Ext02nm[m+4][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m][2])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][2] + Intercept_Ext02nm[m][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][3] + Intercept_Ext02nm[m][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m+4][3])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][3] + Intercept_Ext02nm[m+4][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][4] + Intercept_Ext02nm[m+4][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m][3])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][3] + Intercept_Ext02nm[m][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][4] + Intercept_Ext02nm[m][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m+4][4])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][4] + Intercept_Ext02nm[m+4][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][5] + Intercept_Ext02nm[m+4][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext02nm[m][4])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][4] + Intercept_Ext02nm[m][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][5] + Intercept_Ext02nm[m][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//修正1.5A标准物质ABS均值
				 {
				 	 if(Number_Channel == 0x01)									//EFGH通道
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m+4][5] + Intercept_Ext02nm[m+4][5];				 //修正1.5A标准物质ABS均值
		       }
					 else														 //ABCD通道
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext02nm[m][5] + Intercept_Ext02nm[m][5];				 //修正1.5A标准物质ABS均值
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 else if(Wavelength_Main == 7)				//扩展波长03
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGH通道
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][1] + Intercept_Ext03nm[m+4][1];	//修正0.2A标准物质ABS均值
		       }
					 else														//ABCD通道
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][1] + Intercept_Ext03nm[m][1];	//修正0.2A标准物质ABS均值
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //修正0.5A标准物质ABS均值
		     {  
					 if(Number_Channel == 0x01)									//EFGH通道
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m+4][1])										  //小于设定测量值
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][1] + Intercept_Ext03nm[m+4][1];			 //选用0.2-0.5A段的K，B值						  
					    }	
					    else																			                   //大于设定测量值
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][2] + Intercept_Ext03nm[m+4][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  else														   	//ABCD通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m][1])										  //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][1] + Intercept_Ext03nm[m][1];			 //选用0.2-0.5A段的K，B值						  
					      }	
					      else																	                   //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][2] + Intercept_Ext03nm[m][2];			 //选用0.5-1.0A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m+4][2])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][2] + Intercept_Ext03nm[m+4][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][3] + Intercept_Ext03nm[m+4][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m][2])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][2] + Intercept_Ext03nm[m][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][3] + Intercept_Ext03nm[m][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m+4][3])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][3] + Intercept_Ext03nm[m+4][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][4] + Intercept_Ext03nm[m+4][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m][3])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][3] + Intercept_Ext03nm[m][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][4] + Intercept_Ext03nm[m][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m+4][4])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][4] + Intercept_Ext03nm[m+4][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][5] + Intercept_Ext03nm[m+4][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext03nm[m][4])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][4] + Intercept_Ext03nm[m][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][5] + Intercept_Ext03nm[m][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//修正1.5A标准物质ABS均值
				 {
				 	 if(Number_Channel == 0x01)									//EFGH通道
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m+4][5] + Intercept_Ext03nm[m+4][5];				 //修正1.5A标准物质ABS均值
		       }
					 else														 //ABCD通道
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext03nm[m][5] + Intercept_Ext03nm[m][5];				 //修正1.5A标准物质ABS均值
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 else if(Wavelength_Main == 8)				//扩展波长04
	 { 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
		   for(i=0;i<24;i++)
		   {
				 if(i < 4)
	   	   {   
				   if(Number_Channel == 0x01)									//EFGH通道
					 {
					   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][1] + Intercept_Ext04nm[m+4][1];	//修正0.2A标准物质ABS均值
		       }
					 else														//ABCD通道
					 {
					 	  ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][1] + Intercept_Ext04nm[m][1];	//修正0.2A标准物质ABS均值
					 }
					 m ++;
					 if(i == 3)
					 {
					 	 m = 0;
					 }
				 } 
		     else if(i >= 4 && i < 8)																  //修正0.5A标准物质ABS均值
		     {  
					 if(Number_Channel == 0x01)									//EFGH通道
		   	   {   
						  if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m+4][1])										  //小于设定测量值
			   		  {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][1] + Intercept_Ext04nm[m+4][1];			 //选用0.2-0.5A段的K，B值						  
					    }	
					    else																			                   //大于设定测量值
					    {
					  	   ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][2] + Intercept_Ext04nm[m+4][2];			 //选用0.5-1.0A段的K，B值
					    }
					  }
					  else														   	//ABCD通道
					  {
					  	 if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m][1])										  //小于设定测量值
			   		   {
					  	    ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][1] + Intercept_Ext04nm[m][1];			 //选用0.2-0.5A段的K，B值						  
					      }	
					      else																	                   //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][2] + Intercept_Ext04nm[m][2];			 //选用0.5-1.0A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 7)
					  {
					  	 m = 0;
					  }
		     }
				 else if(i >= 8 && i < 12)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m+4][2])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][2] + Intercept_Ext04nm[m+4][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][3] + Intercept_Ext04nm[m+4][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m][2])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][2] + Intercept_Ext04nm[m][2];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][3] + Intercept_Ext04nm[m][3];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 11)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 12 && i < 16)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m+4][3])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][3] + Intercept_Ext04nm[m+4][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][4] + Intercept_Ext04nm[m+4][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m][3])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][3] + Intercept_Ext04nm[m][3];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][4] + Intercept_Ext04nm[m][4];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 15)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else if(i >= 16 && i < 20)																//修正1.0A标准物质ABS均值
				 {
					  if(Number_Channel == 0x01)									//EFGH通道
				 	  {
					      if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m+4][4])										 //小于设定测量值
			   		    {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][4] + Intercept_Ext04nm[m+4][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	       ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][5] + Intercept_Ext04nm[m+4][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  else															 //ABCF通道
					  {
					  	  if(ABS_Array[Num_Test][ i ] < TestValue_Ext04nm[m][4])										 //小于设定测量值
			   		    {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][4] + Intercept_Ext04nm[m][4];			 //选用0.5-1.0A段的K，B值						  
					      }	
					      else																				 //大于设定测量值
					      {
					  	     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][5] + Intercept_Ext04nm[m][5];			 //选用1.0-1.5A段的K，B值
					      }
					  }
					  m ++;
					  //n ++;
					  if(i == 19)
					  {
					  	  m = 0;
						  n = 0;
					  }
				 }
				 else																					//修正1.5A标准物质ABS均值
				 {
				 	 if(Number_Channel == 0x01)									//EFGH通道
				 	 {
					     ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m+4][5] + Intercept_Ext04nm[m+4][5];				 //修正1.5A标准物质ABS均值
		       }
					 else														 //ABCD通道
					 {				 
					 	 ABS_Array2[Num_Test][ i ] = ABS_Array[Num_Test][ i ] * Slope_Ext04nm[m][5] + Intercept_Ext04nm[m][5];				 //修正1.5A标准物质ABS均值
					 }
					 m ++;
					 if(i == 23)
					 {
					 	 m = 0;
					 }
				 }
	      }	         
	   }
		 m = 0;
   }
	 
  //求10次走板的ABS修正值平均值
	result = 0;														 
	for(n=0;n<24;n++)													
	{
		for(Num_Test=0;Num_Test<10;Num_Test++)
		{
			result += ABS_Array2[Num_Test][ n ];						 //求ABS显示值的累加和
		}
		ABS_Aver_Array2[n] = result / 10;									 //保存ABS修正值的平均值
		result = 0;														 //清累加变量
	}
	
	
	CV_SD_Function(ABS_Array2,ABS_Aver_Array2,10);						 //调用函数求样本CV值、SD值
	 
	//求各个波长下的准确度
	if(Wavelength_Main == 1)//||(Wavelength_Main == 5))	20110421		                     //405nm波长 or 412
	{ 	
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_405nm[n]);			 //求准确度
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //选择标准物的靶值
			{
			    n ++;
			}
		}
		n = 0;
	}
	else if(Wavelength_Main == 2)			                 //450nm波长
	{
	    n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_450nm[n]);			 //求准确度
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //选择标准物的靶值
			{
			    n ++;
			}
		}
	    n = 0;
	}
	else if(Wavelength_Main == 3)//||(Wavelength_Main == 6))	20110421		                 //492nm波长
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_492nm[n]);			 //求准确度
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //选择标准物的靶值
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 4)						//630nm波长
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_630nm[n]);			 //求准确度
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //选择标准物的靶值
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 5)						//扩展波长01
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_Ext01nm[n]);			 //求准确度
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //选择标准物的靶值
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 6)						//扩展波长02
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_Ext02nm[n]);			 //求准确度
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //选择标准物的靶值
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 7)						//扩展波长03
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_Ext03nm[n]);			 //求准确度
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //选择标准物的靶值
			{
			    n ++;
			}
		}
		n = 0;
	}
	else	if(Wavelength_Main == 8)						//扩展波长04
	{
		n = 1;
		for(m=0;m<24;m++)
		{		
			Accuration[m] = fabs(ABS_Aver_Array2[m] - StandardValue_Ext04nm[n]);			 //求准确度
			if(m == 3 || m == 7 || m == 11 || m == 15 || m == 19)										 //选择标准物的靶值
			{
			    n ++;
			}
		}
		n = 0;
	}
	 
	m = 0;
	result = 0;									 
	for(n=0;n<24;n++)									
  {																			  //求通道均值
		 result += ABS_Aver_Array2[n] ;
		 if(n == 3 || n == 7 || n == 11 || n == 15 || n == 19 || n == 23)
		 {
			 result    /= 4;
			 Differ[m] = result;
			 result    = 0;
			 m ++;
		 }
	}

	CV_SD_Function1(ABS_Aver_Array2,Differ,6);									//求通道差
	
	//保存 K,B,T
}


//****4个标准物质***将ABCD通道或EFGH通道要发送的16个ABS值、16个准确度、16个CV值、4个孔间差值保存在ABS_Send[]数组中*******//

//****6个标准物质***将ABCD通道或EFGH通道要发送的24个ABS值、24个准确度、24个CV值、6个孔间差值保存在ABS_Send[]数组中*******//
void Array_Combination_Optical(u8 Wavelength_Main,u8 Number_Channel)	  
{	
  u8 i,j,m,n;
	m = 0;
	n = 0;
	j = 0;
	if(Wavelength_Main == 1)			 //405nm波长 
	{			 
		if(Number_Channel ==1)			 //EFGH通道
	  {
			 for(i=0;i<146;i++)										     //循环控制发送96个数据
	     {
		      if(i<24)
	        {
		         ABS_Send[i] = ABS_Aver_Array2[i];			         //保存标准物质24个ABS显示均值
		      }
		      else if(i>= 24 && i<48)
	        {
		         ABS_Send[i] = Accuration[i - 24];                   //保存标准物质24个准确度值   
		      }
		      else if(i>= 48 && i<72)
		      {
			       ABS_Send[i] = CV[i - 48];							 //保存标准物质24个CV值
		      }
		      else if(i>= 72 && i<78)
		      {
			       ABS_Send[i] = Differ_betweenHoles[i - 72];			 //保存标准物质6个通道差值
		      }
		      else if(i>=78 && i<102)									 //保存4个通道（ABCD或EFGH）的24个K值（每个通道6个K值）
		      {
			       ABS_Send[i] = Slope_405nm[m+4][n];			 //存取K值					
				     n ++;
				     if(n == 6)									 //一个通道的4个K值存取完毕
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 101)										  //4通道K值存取完毕M清0
			       {
			 	       m = 0;
			       }	  
			    }
		      else if(i>=102 && i<126)									 //保存4个通道（ABCD或EFGH）的24个b值（每个通道4个b值）
		      {
			      ABS_Send[i] = Intercept_405nm[m+4][n];			 //存取b值					
				    n ++;
				    if(n == 6)									         //一个通道的4个b值存取完毕
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 125)										  //4通道b值存取完毕M清0
			      {
			 	       m = 0;
			      }	  
			    }
					else if(i>=126 && i<146)				
			    {
			       ABS_Send[i] = TestValue_405nm[m+4][n];			 //存取T值					
				     n ++;
				     if(n == 5)									 //一个通道的4个K值存取完毕
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 145)										  //4通道K值存取完毕M清0
			       {
			 	       m = 0;
			       }	  
			    }
		    }            
		}
		else if(Number_Channel == 0)														//ABCD通道
		{
			 for(i=0;i<146;i++)										     //循环控制发送96个数据
	     {
		     if(i<24)
	       {
						ABS_Send[i] = ABS_Aver_Array2[i];			         //保存标准物质24个ABS显示均值
				 }
				 else if(i>= 24 && i<48)
				 {
						ABS_Send[i] = Accuration[i - 24];                   //保存标准物质24个准确度值   
				 }
				 else if(i>= 48 && i<72)
				 {
						ABS_Send[i] = CV[i - 48];							 //保存标准物质24个CV值
				 }
				 else if(i>= 72 && i<78)
				 {
						ABS_Send[i] = Differ_betweenHoles[i - 72];			 //保存标准物质6个通道差值
				 }
				 else if(i>=78 && i<102)									 //保存4个通道（ABCD或EFGH）的24个K值（每个通道4个K值）
				 {
			      ABS_Send[i] = Slope_405nm[m][n];			 //存取K值					
				    n ++;
				    if(n == 6)									 //一个通道的4个K值存取完毕
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 101)										  //4通道K值存取完毕M清0
			      {
			 	      m = 0;
			      }	  
			   }
				 else if(i>=102 && i<126)									 //保存4个通道（ABCD或EFGH）的16个b值（每个通道4个b值）
				 {
						ABS_Send[i] = Intercept_405nm[m][n];			 //存取b值					
						n ++;
						if(n == 6)									         //一个通道的4个b值存取完毕
						{
							 n = 0;
						 m ++;
						}
						if(i == 125)										  //4通道b值存取完毕M清0
						{
							m = 0;
						}	  
			  }
				else if(i>=126 && i<146)				
				{
					 ABS_Send[i] = TestValue_405nm[m][n];			 //存取T值					
					 n ++;
					 if(n == 5)									 //一个通道的4个K值存取完毕
					 {
						 n = 0;
						 m ++;
					 }
					 if(i == 145)										  //4通道K值存取完毕M清0
					 {
						 m = 0;
					 }	  
				}
		 }    
		}
	}
  else if(Wavelength_Main == 2)							 //450nm波长
	{			 
		if(Number_Channel ==1)			 //EFGH通道
	  {
			 for(i=0;i<146;i++)										     //循环控制发送96个数据
	     {
		      if(i<24)
	        {
		         ABS_Send[i] = ABS_Aver_Array2[i];			         //保存标准物质24个ABS显示均值
		      }
		      else if(i>= 24 && i<48)
	        {
		         ABS_Send[i] = Accuration[i - 24];                   //保存标准物质24个准确度值   
		      }
		      else if(i>= 48 && i<72)
		      {
			       ABS_Send[i] = CV[i - 48];							 //保存标准物质24个CV值
		      }
		      else if(i>= 72 && i<78)
		      {
			       ABS_Send[i] = Differ_betweenHoles[i - 72];			 //保存标准物质6个通道差值
		      }
		      else if(i>=78 && i<102)									 //保存4个通道（ABCD或EFGH）的24个K值（每个通道6个K值）
		      {
			       ABS_Send[i] = Slope_450nm[m+4][n];			 //存取K值					
				     n ++;
				     if(n == 6)									 //一个通道的4个K值存取完毕
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 101)										  //4通道K值存取完毕M清0
			       {
			 	       m = 0;
			       }	  
			    }
		      else if(i>=102 && i<126)									 //保存4个通道（ABCD或EFGH）的24个b值（每个通道4个b值）
		      {
			      ABS_Send[i] = Intercept_450nm[m+4][n];			 //存取b值					
				    n ++;
				    if(n == 6)									         //一个通道的4个b值存取完毕
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 125)										  //4通道b值存取完毕M清0
			      {
			 	       m = 0;
			      }	  
			    }
					else if(i>=126 && i<146)				
			    {
			       ABS_Send[i] = TestValue_450nm[m+4][n];			 //存取T值					
				     n ++;
				     if(n == 5)									 //一个通道的4个K值存取完毕
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 145)										  //4通道K值存取完毕M清0
			       {
			 	       m = 0;
			       }	  
			    }
		    }            
		}
		else if(Number_Channel == 0)														//ABCD通道
		{
			 for(i=0;i<146;i++)										     //循环控制发送96个数据
	     {
		     if(i<24)
	       {
						ABS_Send[i] = ABS_Aver_Array2[i];			         //保存标准物质24个ABS显示均值
				 }
				 else if(i>= 24 && i<48)
				 {
						ABS_Send[i] = Accuration[i - 24];                   //保存标准物质24个准确度值   
				 }
				 else if(i>= 48 && i<72)
				 {
						ABS_Send[i] = CV[i - 48];							 //保存标准物质24个CV值
				 }
				 else if(i>= 72 && i<78)
				 {
						ABS_Send[i] = Differ_betweenHoles[i - 72];			 //保存标准物质6个通道差值
				 }
				 else if(i>=78 && i<102)									 //保存4个通道（ABCD或EFGH）的24个K值（每个通道4个K值）
				 {
			      ABS_Send[i] = Slope_450nm[m][n];			 //存取K值					
				    n ++;
				    if(n == 6)									 //一个通道的4个K值存取完毕
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 101)										  //4通道K值存取完毕M清0
			      {
			 	      m = 0;
			      }	  
			   }
				 else if(i>=102 && i<126)									 //保存4个通道（ABCD或EFGH）的16个b值（每个通道4个b值）
				 {
						ABS_Send[i] = Intercept_450nm[m][n];			 //存取b值					
						n ++;
						if(n == 6)									         //一个通道的4个b值存取完毕
						{
							 n = 0;
						 m ++;
						}
						if(i == 125)										  //4通道b值存取完毕M清0
						{
							m = 0;
						}	  
			  }
				else if(i>=126 && i<146)				
				{
					 ABS_Send[i] = TestValue_450nm[m][n];			 //存取T值					
					 n ++;
					 if(n == 5)									 //一个通道的4个K值存取完毕
					 {
						 n = 0;
						 m ++;
					 }
					 if(i == 145)										  //4通道K值存取完毕M清0
					 {
						 m = 0;
					 }	  
				}
		 }    
		}
	}
	else if(Wavelength_Main == 3)                           //492nm波长
	{			 
		if(Number_Channel ==1)			 //EFGH通道
	  {
			 for(i=0;i<146;i++)										     //循环控制发送96个数据
	     {
		      if(i<24)
	        {
		         ABS_Send[i] = ABS_Aver_Array2[i];			         //保存标准物质24个ABS显示均值
		      }
		      else if(i>= 24 && i<48)
	        {
		         ABS_Send[i] = Accuration[i - 24];                   //保存标准物质24个准确度值   
		      }
		      else if(i>= 48 && i<72)
		      {
			       ABS_Send[i] = CV[i - 48];							 //保存标准物质24个CV值
		      }
		      else if(i>= 72 && i<78)
		      {
			       ABS_Send[i] = Differ_betweenHoles[i - 72];			 //保存标准物质6个通道差值
		      }
		      else if(i>=78 && i<102)									 //保存4个通道（ABCD或EFGH）的24个K值（每个通道6个K值）
		      {
			       ABS_Send[i] = Slope_492nm[m+4][n];			 //存取K值					
				     n ++;
				     if(n == 6)									 //一个通道的4个K值存取完毕
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 101)										  //4通道K值存取完毕M清0
			       {
			 	       m = 0;
			       }	  
			    }
		      else if(i>=102 && i<126)									 //保存4个通道（ABCD或EFGH）的24个b值（每个通道4个b值）
		      {
			      ABS_Send[i] = Intercept_492nm[m+4][n];			 //存取b值					
				    n ++;
				    if(n == 6)									         //一个通道的4个b值存取完毕
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 125)										  //4通道b值存取完毕M清0
			      {
			 	       m = 0;
			      }	  
			    }
					else if(i>=126 && i<146)				
			    {
			       ABS_Send[i] = TestValue_492nm[m+4][n];			 //存取T值					
				     n ++;
				     if(n == 5)									 //一个通道的4个K值存取完毕
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 145)										  //4通道K值存取完毕M清0
			       {
			 	       m = 0;
			       }	  
			    }
		    }            
		}
		else if(Number_Channel == 0)														//ABCD通道
		{
			 for(i=0;i<146;i++)										     //循环控制发送96个数据
	     {
		     if(i<24)
	       {
						ABS_Send[i] = ABS_Aver_Array2[i];			         //保存标准物质24个ABS显示均值
				 }
				 else if(i>= 24 && i<48)
				 {
						ABS_Send[i] = Accuration[i - 24];                   //保存标准物质24个准确度值   
				 }
				 else if(i>= 48 && i<72)
				 {
						ABS_Send[i] = CV[i - 48];							 //保存标准物质24个CV值
				 }
				 else if(i>= 72 && i<78)
				 {
						ABS_Send[i] = Differ_betweenHoles[i - 72];			 //保存标准物质6个通道差值
				 }
				 else if(i>=78 && i<102)									 //保存4个通道（ABCD或EFGH）的24个K值（每个通道4个K值）
				 {
			      ABS_Send[i] = Slope_492nm[m][n];			 //存取K值					
				    n ++;
				    if(n == 6)									 //一个通道的4个K值存取完毕
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 101)										  //4通道K值存取完毕M清0
			      {
			 	      m = 0;
			      }	  
			   }
				 else if(i>=102 && i<126)									 //保存4个通道（ABCD或EFGH）的16个b值（每个通道4个b值）
				 {
						ABS_Send[i] = Intercept_492nm[m][n];			 //存取b值					
						n ++;
						if(n == 6)									         //一个通道的4个b值存取完毕
						{
							 n = 0;
						 m ++;
						}
						if(i == 125)										  //4通道b值存取完毕M清0
						{
							m = 0;
						}	  
			  }
				else if(i>=126 && i<146)				
				{
					 ABS_Send[i] = TestValue_492nm[m][n];			 //存取T值					
					 n ++;
					 if(n == 5)									 //一个通道的4个K值存取完毕
					 {
						 n = 0;
						 m ++;
					 }
					 if(i == 145)										  //4通道K值存取完毕M清0
					 {
						 m = 0;
					 }	  
				}
		 }    
		}
	}
	else if(Wavelength_Main == 4)			                 //630nm波长
	{			 
		if(Number_Channel ==1)			 //EFGH通道
	  {
			 for(i=0;i<146;i++)										     //循环控制发送96个数据
	     {
		      if(i<24)
	        {
		         ABS_Send[i] = ABS_Aver_Array2[i];			         //保存标准物质24个ABS显示均值
		      }
		      else if(i>= 24 && i<48)
	        {
		         ABS_Send[i] = Accuration[i - 24];                   //保存标准物质24个准确度值   
		      }
		      else if(i>= 48 && i<72)
		      {
			       ABS_Send[i] = CV[i - 48];							 //保存标准物质24个CV值
		      }
		      else if(i>= 72 && i<78)
		      {
			       ABS_Send[i] = Differ_betweenHoles[i - 72];			 //保存标准物质6个通道差值
		      }
		      else if(i>=78 && i<102)									 //保存4个通道（ABCD或EFGH）的24个K值（每个通道6个K值）
		      {
			       ABS_Send[i] = Slope_630nm[m+4][n];			 //存取K值					
				     n ++;
				     if(n == 6)									 //一个通道的4个K值存取完毕
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 101)										  //4通道K值存取完毕M清0
			       {
			 	       m = 0;
			       }	  
			    }
		      else if(i>=102 && i<126)									 //保存4个通道（ABCD或EFGH）的24个b值（每个通道4个b值）
		      {
			      ABS_Send[i] = Intercept_630nm[m+4][n];			 //存取b值					
				    n ++;
				    if(n == 6)									         //一个通道的4个b值存取完毕
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 125)										  //4通道b值存取完毕M清0
			      {
			 	       m = 0;
			      }	  
			    }
					else if(i>=126 && i<146)				
			    {
			       ABS_Send[i] = TestValue_630nm[m+4][n];			 //存取T值					
				     n ++;
				     if(n == 5)									 //一个通道的4个K值存取完毕
				     {
				  	   n = 0;
					     m ++;
				     }
					   if(i == 145)										  //4通道K值存取完毕M清0
			       {
			 	       m = 0;
			       }	  
			    }
		    }            
		}
		else if(Number_Channel == 0)														//ABCD通道
		{
			 for(i=0;i<146;i++)										     //循环控制发送96个数据
	     {
		     if(i<24)
	       {
						ABS_Send[i] = ABS_Aver_Array2[i];			         //保存标准物质24个ABS显示均值
				 }
				 else if(i>= 24 && i<48)
				 {
						ABS_Send[i] = Accuration[i - 24];                   //保存标准物质24个准确度值   
				 }
				 else if(i>= 48 && i<72)
				 {
						ABS_Send[i] = CV[i - 48];							 //保存标准物质24个CV值
				 }
				 else if(i>= 72 && i<78)
				 {
						ABS_Send[i] = Differ_betweenHoles[i - 72];			 //保存标准物质6个通道差值
				 }
				 else if(i>=78 && i<102)									 //保存4个通道（ABCD或EFGH）的24个K值（每个通道4个K值）
				 {
			      ABS_Send[i] = Slope_630nm[m][n];			 //存取K值					
				    n ++;
				    if(n == 6)									 //一个通道的4个K值存取完毕
				    {
				  	  n = 0;
					    m ++;
				    }
					  if(i == 101)										  //4通道K值存取完毕M清0
			      {
			 	      m = 0;
			      }	  
			   }
				 else if(i>=102 && i<126)									 //保存4个通道（ABCD或EFGH）的16个b值（每个通道4个b值）
				 {
						ABS_Send[i] = Intercept_630nm[m][n];			 //存取b值					
						n ++;
						if(n == 6)									         //一个通道的4个b值存取完毕
						{
							 n = 0;
						 m ++;
						}
						if(i == 125)										  //4通道b值存取完毕M清0
						{
							m = 0;
						}	  
			  }
				else if(i>=126 && i<146)				
				{
					 ABS_Send[i] = TestValue_630nm[m][n];			 //存取T值					
					 n ++;
					 if(n == 5)									 //一个通道的4个K值存取完毕
					 {
						 n = 0;
						 m ++;
					 }
					 if(i == 145)										  //4通道K值存取完毕M清0
					 {
						 m = 0;
					 }	  
				}
		 }    
		}	
	}
}

void USB_Out_K_B_Abs(u8 mode,u8 wave)
{
	u8 i=0;
	u8 j=0;
	float *addr_array_k;
	float *addr_array_b;
	float *addr_array_t;
	float *std;
	switch(wave)
	{
		case 1:
			addr_array_k = (float*)&Slope_405nm[0][0];
			addr_array_b = (float*)&Intercept_405nm[0][0];
			addr_array_t = (float*)&TestValue_405nm[0][0];
		  std          = (float*)&StandardValue_405nm[0];
			break;
		case 2:
			addr_array_k = (float*)&Slope_450nm[0][0];
			addr_array_b = (float*)&Intercept_450nm[0][0];
			addr_array_t = (float*)&TestValue_450nm[0][0];
		  std          = (float*)&StandardValue_450nm[0];
			break;
		case 3:
			addr_array_k = (float*)&Slope_492nm[0][0];
			addr_array_b = (float*)&Intercept_492nm[0][0];
			addr_array_t = (float*)&TestValue_492nm[0][0];
		  std          = (float*)&StandardValue_492nm[0];
			break;
		case 4:
			addr_array_k = (float*)&Slope_630nm[0][0];
			addr_array_b = (float*)&Intercept_630nm[0][0];
			addr_array_t = (float*)&TestValue_630nm[0][0];
		  std          = (float*)&StandardValue_630nm[0];
			break;
		default:
			break;
	}
	
	if(mode == 1)//输出原始AD值
	{
		for(i=0;i<10;i++)
		{
			for(j=0;j<8;j++)
			{
				printf("AD_Alllight[%d][%d]=%.4f\r\n",i,j,AD_Alllight[i][j]);
			}
		}
		
		for(i=0;i<10;i++)
		{
			for(j=0;j<24;j++)
			{
				printf("AD_Halflight[%d][%d]=%.4f\r\n",i,j,AD_Halflight[i][j]);
			}
		}
	}
	else if(mode == 2)//输出原始ABS
	{
		for(i=0;i<10;i++)
		{
			for(j=0;j<24;j++)
			{
				printf("ABS_Array[%d][%d]=%0.4f\r\n",i,j,ABS_Array[i][j]);
			}
		}
		
		for(i=0;i<24;i++)
		{
			printf("ABS_Aver_Array[%d]=%0.4f\r\n",i,ABS_Aver_Array[i]);
		}
	}
	else if(mode == 3)//输出修正后的ABS
	{
		
		for(i=0;i<10;i++)
		{
			for(j=0;j<24;j++)
			{
				printf("ABS_Array2[%d][%d]=%0.4f\r\n",i,j,ABS_Array2[i][j]);
			}
		}
		
		for(i=0;i<24;i++)
		{
			printf("ABS_Aver_Array2[%d]=%0.4f\r\n",i,ABS_Aver_Array2[i]);
		}
		
		
		//输出  Slope_450nm K,B,T
		for(i=0;i<8;i++)
		{
			for(j=0;j<6;j++)
			{
				printf("Slope_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_k);
				addr_array_k = addr_array_k+1;
			}
		}
		
		for(i=0;i<8;i++)
		{
			for(j=0;j<6;j++)
			{
				printf("Intercept_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_b);
				addr_array_b = addr_array_b+1;
			}
		}

		for(i=0;i<8;i++)
		{
			for(j=0;j<5;j++)
			{
				printf("TestValue_%d[%d][%d]=%0.4f\r\n",wave,i,j,*addr_array_t);
				addr_array_t = addr_array_t+1;
			}
		}
		
	}
	else if(mode == 4)//输出有效数据--光性能校正
	{
		for(i=0;i<146;i++)
		{
				printf(":%0.4f",ABS_Send[i]);
		}
		
		for(i=1;i<7;i++)
		{
				printf(":%0.4f",std[i]);
		}
	}
 }


/****************************************************************************
* 名    称：Data_Handling_LiquidSamples(u8 Wavelength_Main)
* 功    能：输出双波长测得的 AD 值 及 计算的到的ABS
* 入口参数：u8 Wavelength_Main :
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void Data_Handling_LiquidSamples(u8 Wavelength_Main)
{
	//uchar data k = 0;
	u8 n = 0;
	u8 m = 0;
	u8 Num_Test = 0;								//走板测试次数
// 	u8 Num_Glass = 0;
// 	u8 Num_PerGlass = 0;
// 	float A_BS;	
	float result = 0;							//累加和变量		    
// 	uint  Max_AD,Meas_AD;					            //吸光度变量
// 	u8 Num_Queue = 0;
// 	Num_Channel = 8;										//置通道数

	//进行吸光度ABS显示值的修正
	Num_Test = 0;													 
	if(Wavelength_Main == 1)                    //405nm波长,变量m用来控制8个通道的选择
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
	   {
			 for(n=0;n<16;n++)
			 {
				 if(ABS_Array[Num_Test][n] < TestValue_405nm[m][1])											               //当小于0.5A测量值
				 {
			   	   if(ABS_Array[Num_Test][n] < TestValue_405nm[m][0])									                   //当小于0.2A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][0] + Intercept_405nm[m][0];	   //修正0-0.2A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][1] + Intercept_405nm[m][1];	   //修正0.2-0.5A的ABS值
				     }
				 }
				 else	if(ABS_Array[Num_Test][n] < TestValue_405nm[m][3])																								   ///当小于1.5A测量值
				 {
			   	   if(ABS_Array[Num_Test][n]  < TestValue_405nm[m][2])													   //当小于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][2] + Intercept_405nm[m][2];	   //修正0.5-1.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][3] + Intercept_405nm[m][3];   //修正1.0-1.5A的ABS值
				     }
			   }
				 else																									   ///当大于等于1.5A测量值
				 {
			   	   if(ABS_Array[Num_Test][n]  < TestValue_405nm[m][4])													   //当小于2.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][4] + Intercept_405nm[m][4];	   //修正1.5-2.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_405nm[m][5] + Intercept_405nm[m][5];   //修正2.0-3.0A或大于3.0A的ABS值
				     }
			   }
				 
				m ++;
				if(n == 7)																						            //已是第1列最后一个通道（n=7）
				{
					m = 0;																						            //当已是第一列最后一个通道时m须置0使k、b值再从第1通道取起
				}
		 }	         
		 m = 0;
	 }
	}
	else if(Wavelength_Main == 2)			                 //450nm波长
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_450nm[m][1])											               //当小于0.5A测量值
			     {
						 if(ABS_Array[Num_Test][n] < TestValue_450nm[m][0])									                   //当小于0.2A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][0] + Intercept_450nm[m][0];	   //修正0-0.2A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][1] + Intercept_450nm[m][1];	   //修正0.2-0.5A的ABS值
				     }
			     }
					 else if(ABS_Array[Num_Test][n] < TestValue_450nm[m][3])											               //当小于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n] < TestValue_450nm[m][2])									                   //当小于1.0A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][2] + Intercept_450nm[m][2];	   //修正0.5-1.0A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][3] + Intercept_450nm[m][3];	   //修正1.0-1.5A的ABS值
				     }
			     }
			     else																									   ///当大于等于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_450nm[m][4])													   //当小于2.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][4] + Intercept_450nm[m][4];	   //修正1.5-2.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_450nm[m][5] + Intercept_450nm[m][5];   //修正2.0-3.0A或大于3.0A的ABS值
				     }
			     }
				m ++;
				if(n == 7)																						            //已是第1列最后一个通道（n=7）
				{
				 	m = 0;																						            //当已是第一列最后一个通道时m须置0使k、b值再从第1通道取起
				}
			 }	         
		 m = 0;
	 }
	}
	else if(Wavelength_Main == 3)     //492nm波长
	{ 
		for(Num_Test=0;Num_Test<10;Num_Test++)
	  {
			 for(n=0;n<16;n++)
			 {
			     if(ABS_Array[Num_Test][n] < TestValue_492nm[m][1])											               //当小于0.5A测量值
			     {
						 if(ABS_Array[Num_Test][n] < TestValue_492nm[m][0])									                   //当小于0.2A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][0] + Intercept_492nm[m][0];	   //修正0-0.2A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][1] + Intercept_492nm[m][1];	   //修正0.2-0.5A的ABS值
				     }
			     }
					 else	if(ABS_Array[Num_Test][n] < TestValue_492nm[m][3])								//当小于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_492nm[m][2])													   //当小于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][2] + Intercept_492nm[m][2];	   //修正0.5-1.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][3] + Intercept_492nm[m][3];   //修正1.0-1.5A 的ABS值
				     }
			     }
			     else																									   ///当大于等于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_492nm[m][4])													   //当小于2.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][4] + Intercept_492nm[m][4];	   //修正1.5-2.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_492nm[m][5] + Intercept_492nm[m][5];   //修正2.0-3.0A或大于3.0A的ABS值
				     }
			     }
				m ++;
				if(n == 7)																						            //已是第1列最后一个通道（n=7）
				{
				 	m = 0;																						            //当已是第一列最后一个通道时m须置0使k、b值再从第1通道取起
				}
		 }	         
		 m = 0;
	 }
	}
	else if(Wavelength_Main == 4)		     //630nm波长
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_630nm[m][1])											               //当小于0.5A测量值
			     {
			   	     if(ABS_Array[Num_Test][n] < TestValue_630nm[m][0])									                   //当小于0.2A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][0] + Intercept_630nm[m][0];	   //修正0-0.2A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][1] + Intercept_630nm[m][1];	   //修正0.2-0.5A的ABS值
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_630nm[m][3])				   //当小于  1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_630nm[m][2])													   //当小于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][2] + Intercept_630nm[m][2];	   //修正0.5-1.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][3] + Intercept_630nm[m][3];   //修正1.0-1.5A或大于1.5A的ABS值
				     }
			     }
			     else																									   ///当大于等于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_630nm[m][4])													   //当小于2.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][4] + Intercept_630nm[m][4];	   //修正1.5-2.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_630nm[m][5] + Intercept_630nm[m][5];   //修正2.0-3.0A或大于3.0A的ABS值
				     }
			     }
				m ++;
				if(n == 7)																						            //已是第1列最后一个通道（n=7）
				{
				 	m = 0;																						            //当已是第一列最后一个通道时m须置0使k、b值再从第1通道取起
				}
			 }	         
		 m = 0;
	 }
	}														 
	else if(Wavelength_Main == 5)		     //扩展波长01
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_Ext01nm[m][1])											               //当小于0.5A测量值
			     {
			   	   if(ABS_Array[Num_Test][n] < TestValue_Ext01nm[m][0])									                   //当小于0.2A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][0] + Intercept_Ext01nm[m][0];	   //修正0-0.2A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][1] + Intercept_Ext01nm[m][1];	   //修正0.2-0.5A的ABS值
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_Ext01nm[m][3])				   //当小于  1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext01nm[m][2])													   //当小于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][2] + Intercept_Ext01nm[m][2];	   //修正0.5-1.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][3] + Intercept_Ext01nm[m][3];   //修正1.0-1.5A或大于1.5A的ABS值
				     }
			     }
			     else																									   ///当大于等于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext01nm[m][4])													   //当小于2.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][4] + Intercept_Ext01nm[m][4];	   //修正1.5-2.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext01nm[m][5] + Intercept_Ext01nm[m][5];   //修正2.0-3.0A或大于3.0A的ABS值
				     }
			     }
				m ++;
				if(n == 7)																						            //已是第1列最后一个通道（n=7）
				{
				 	m = 0;																						            //当已是第一列最后一个通道时m须置0使k、b值再从第1通道取起
				}
			 }	         
		 m = 0;
	 }
	}						    
	else if(Wavelength_Main == 6)		     //扩展波长02
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_Ext02nm[m][1])											               //当小于0.5A测量值
			     {
			   	   if(ABS_Array[Num_Test][n] < TestValue_Ext02nm[m][0])									                   //当小于0.2A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][0] + Intercept_Ext02nm[m][0];	   //修正0-0.2A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][1] + Intercept_Ext02nm[m][1];	   //修正0.2-0.5A的ABS值
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_Ext02nm[m][3])				   //当小于  1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext02nm[m][2])													   //当小于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][2] + Intercept_Ext02nm[m][2];	   //修正0.5-1.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][3] + Intercept_Ext02nm[m][3];   //修正1.0-1.5A或大于1.5A的ABS值
				     }
			     }
			     else																									   ///当大于等于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext02nm[m][4])													   //当小于2.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][4] + Intercept_Ext02nm[m][4];	   //修正1.5-2.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext02nm[m][5] + Intercept_Ext02nm[m][5];   //修正2.0-3.0A或大于3.0A的ABS值
				     }
			     }
				m ++;
				if(n == 7)											                        //已是第1列最后一个通道（n=7）
				{
				 	m = 0;																	//当已是第一列最后一个通道时m须置0使k、b值再从第1通道取起
				}
			 }	         
		 m = 0;
	 }
	}//XX			
	else if(Wavelength_Main == 7)		     //扩展波长03
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_Ext03nm[m][1])											               //当小于0.5A测量值
			     {
			   	   if(ABS_Array[Num_Test][n] < TestValue_Ext03nm[m][0])									                   //当小于0.2A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][0] + Intercept_Ext03nm[m][0];	   //修正0-0.2A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][1] + Intercept_Ext03nm[m][1];	   //修正0.2-0.5A的ABS值
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_Ext03nm[m][3])				   //当小于  1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext03nm[m][2])													   //当小于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][2] + Intercept_Ext03nm[m][2];	   //修正0.5-1.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][3] + Intercept_Ext03nm[m][3];   //修正1.0-1.5A或大于1.5A的ABS值
				     }
			     }
			     else																									   ///当大于等于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext03nm[m][4])													   //当小于2.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][4] + Intercept_Ext03nm[m][4];	   //修正1.5-2.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext03nm[m][5] + Intercept_Ext03nm[m][5];   //修正2.0-3.0A或大于3.0A的ABS值
				     }
			     }
				m ++;
				if(n == 7)											                        //已是第1列最后一个通道（n=7）
				{
				 	m = 0;																	//当已是第一列最后一个通道时m须置0使k、b值再从第1通道取起
				}
			 }	         
		 m = 0;
	 }
	}//XX			
	else if(Wavelength_Main == 8)		     //扩展波长04
	{ 
		 for(Num_Test=0;Num_Test<10;Num_Test++)
		 {
		     for(n=0;n<16;n++)
		     {
			     if(ABS_Array[Num_Test][n] < TestValue_Ext04nm[m][1])											               //当小于0.5A测量值
			     {
			   	   if(ABS_Array[Num_Test][n] < TestValue_Ext04nm[m][0])									                   //当小于0.2A测量值
				     { 
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][0] + Intercept_Ext04nm[m][0];	   //修正0-0.2A的ABS值
				     }
				     else																					               //当大于等于0.2小于0.5A
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][1] + Intercept_Ext04nm[m][1];	   //修正0.2-0.5A的ABS值
				     }
			     }
			     else	if(ABS_Array[Num_Test][n] < TestValue_Ext04nm[m][3])				   //当小于  1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext04nm[m][2])													   //当小于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][2] + Intercept_Ext04nm[m][2];	   //修正0.5-1.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][3] + Intercept_Ext04nm[m][3];   //修正1.0-1.5A或大于1.5A的ABS值
				     }
			     }
			     else																									   ///当大于等于1.5A测量值
			     {
						 if(ABS_Array[Num_Test][n]  < TestValue_Ext04nm[m][4])													   //当小于2.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][4] + Intercept_Ext04nm[m][4];	   //修正1.5-2.0A的ABS值
				     }
				     else																								   //当大于等于1.0A测量值
				     {
				   	      ABS_Array2[Num_Test][n] = ABS_Array[Num_Test][n] * Slope_Ext04nm[m][5] + Intercept_Ext04nm[m][5];   //修正2.0-3.0A或大于3.0A的ABS值
				     }
			     }
				m ++;
				if(n == 7)											                        //已是第1列最后一个通道（n=7）
				{
				 	m = 0;																	//当已是第一列最后一个通道时m须置0使k、b值再从第1通道取起
				}
			 }	         
		 m = 0;
	 }
	}//XX			
	
	for(n=0;n<16;n++)												 //求10次走板中前8次走板的ABS显示平均值
	{
		for(Num_Test=0;Num_Test<10;Num_Test++)
		{
			result += ABS_Array2[Num_Test][ n ];					     //求每个孔前10次走板的ABS显示值累加和
		}
		ABS_Aver_Array2[n] = result / 10;								//求每个孔前10次走板的ABS显示值平均值
		result = 0;													//累加和变量清0准备求下一孔前10次走板的ABS显示值累加和
	}																	

	CV_SD_Function(ABS_Array2,ABS_Aver_Array2,10);						//调用函数求液体样本CV值、SD值
	
}


/****************************************************************************
* 名    称：USB_Out_Sample_Abs(u8 wave_flag,u8 out_mode)
* 功    能：输出双波长测得的 AD 值 及 计算的到的ABS
* 入口参数：u8 wave_flag :0 :单波长，  1：双波长
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void USB_Out_Sample_Abs(u8 wave_flag,u8 out_mode)
{
	u8 i=0;
	u8 j=0;
	
	if (out_mode == 0)
	{
		//单波长 AD值 （主波长）
		for(i=0;i<8;i++)
		{
			printf("AD_ALLlight_Single[%d]=%.4f\r\n",i,AD_ALLlight_Single[i]);
		}
		
		for(i=0;i<12;i++)
		{
			for(j=0;j<8;j++)
			{
				printf("AD_Halflight_Single[%d][%d]=%.4f\r\n",i,j,AD_Halflight_Single[i][j]);
			}
		}
		
		//输出单波长的 原始ABS
		for(i=0;i<12;i++)
		{
			for(j=0;j<8;j++)
			{
				printf("ABS_SingleWave0[%d][%d]=%0.4f\r\n",i,j,ABS_SingleWave0[i][j]);
			}
		}
		
		//输出单波长的 修正后的ABS	
		for(i=0;i<12;i++)
		{
			for(j=0;j<8;j++)
			{
				printf("ABS_SingleWave[%d][%d]=%0.4f\r\n",i,j,ABS_SingleWave[i][j]);
			}
		}
		
		if(wave_flag == 1)//双波长 AD 值 (次波长)
		{
			for(i=0;i<8;i++)
			{
				printf("AD_ALLlight_Double[%d]=%.4f\r\n",i,AD_ALLlight_Double[i]);
			}
			
			for(i=0;i<12;i++)
			{
				for(j=0;j<8;j++)
				{
					printf("AD_Halflight_Double[%d][%d]=%.4f\r\n",i,j,AD_Halflight_Double[i][j]);
				}
			}
			
		//输出次波长的 原始ABS
			for(i=0;i<12;i++)
			{
				for(j=0;j<8;j++)
				{
					printf("ABS_DoubleWave0[%d][%d]=%0.4f\r\n",i,j,ABS_DoubleWave0[i][j]);
				}
			}
			
		//输出次波长的 修正后的ABS	
			for(i=0;i<12;i++)
			{
				for(j=0;j<8;j++)
				{
					printf("ABS_DoubleWave[%d][%d]=%0.4f\r\n",i,j,ABS_DoubleWave[i][j]);
				}
			}
		}
	}
	else if(out_mode == 1)
	{
		//输出单波长的 修正后的ABS	
		for(i=0;i<12;i++)
		{
			for(j=0;j<8;j++)
			{
				printf(":%0.4f",ABS_SingleWave[i][j]);
			}
		}
	}
}

/****************************************************************************
* 名    称：u8 AD_Loop_Sample(u16* addr)
* 功    能：
* 入口参数：u16* addr : AD采集值存放的地址
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
u8 AD_Loop_Sample(float* addr)
{
 	u8 Response_CMD[3]={0x68,0x00,0xff};
	float *address = 0;
	u8 ret =0;
	uint8_t i = 0;
	uint8_t j = 0;
	uint16_t pluse_Max = 600;
	uint8_t count = 100;

	address = addr;
	pluse_Max = 600;
	count     = 100;
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_0);//使能电机
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);//方向进仓
	
	//让定位片进入光耦
	while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0)&&(count > 0))//光耦挡住是1， 通过为0
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_2);
		delay_ms(1);
		GPIO_SetBits(GPIOB,GPIO_Pin_2);
		delay_ms(1);
		pluse_Max = pluse_Max - 1;
		count = count - 1;
	}
	
	if(count==0)//同步带定位光耦有问题
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
		Response_CMD[1]=count;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 1;
	}	
		
	//正式开始定位12列
	for(i=0;i<12;i++)
	{
		//这里7个脉冲式为了让挡片移动一点距离，让定位槽偏离光耦，方便检测下一个定位槽
		for(j=0;j<7;j++)
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			pluse_Max = pluse_Max - 1;
		}
		
		while((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) != 0)&&(pluse_Max > 0))//光耦挡住是1， 通过为0
		{
			GPIO_ResetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			GPIO_SetBits(GPIOB,GPIO_Pin_2);
			delay_ms(1);
			pluse_Max = pluse_Max - 1;
		}
		
		if(pluse_Max == 0)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_0);//禁止电机
			Response_CMD[1]=pluse_Max;
			USB_TxWrite(Response_CMD, 3);//发送响应
			return 2;//定位失败
		}

		delay_ms(200);
		ADC0_Sample_Mode(address,2);//采集8通道
		address = address +8;
	}
	
	ret = 0;
	//酶标板复位
	ret = Plate_Motor_Reset(1);
	if(ret)//复位失败
	{
		Response_CMD[1]=ret;
		USB_TxWrite(Response_CMD, 3);//发送响应
		return 3;
	}
	delay_ms(10);

	return 0;
}


/****************************************************************************
* 名    称：u8 Data_Handling(u8 Wavelength_Main,u8 Wavelength_Second)
* 功    能：数据处理函数（计算单/双波长下样本吸光度ABS实测值，修正ABS显示值）
* 入口参数：u16* addr : AD采集值存放的地址
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void Data_Handling(u8 Wavelength_Main,u8 Wavelength_Second)
{
  u8 i = 2;
	u8 k = 1;
	u8 m,n;
	float A_BS;										    //吸光度变量
	u8 Wave_Flag =0;   
	u8 Num_Queue =0;   //列数
	u8 Num_Channel =8; //每列的通道数
// 	u16  Max_AD,Meas_AD;					  //吸光度变量
// 	u16  *Adress_result96;					//地址变量
// 	u16  *Adress_Fulllight;			    //全光源时的AD值地址指针
	float  Max_AD,Meas_AD;					  //吸光度变量
	float  *Adress_result96;					//地址变量
	float  *Adress_Fulllight;			    //全光源时的AD值地址指针
	float *Adress_ABS;							//ABS值地址变量

	Adress_result96  = &AD_Halflight_Single[0][0];//单波长 12列 8 通道AD
	Adress_Fulllight = AD_ALLlight_Single;  //单波长直射通道AD
	Adress_ABS       = &ABS_SingleWave[0][0];					    //ABS值地址变量
	
	if(Wavelength_Second)
	{
		Wave_Flag = 1;//双波长
	}
	else
	{
		Wave_Flag = 0;//单波长
	}
	
	while(i !=0)
	{
		 while( Num_Queue != 12 )	                    //12列数据未计算完成
		 {
			  while(Num_Channel != 0)
			  {
			      Max_AD   = *Adress_Fulllight;										//取光源直射下的AD值
			      Meas_AD  = *Adress_result96;										  //取有检测物时的AD值
			      A_BS     = (float)Max_AD / Meas_AD;                                 //必须强制转为float型，否则将导致计算对数不正确
			      A_BS     = log10(A_BS);                                            //计算吸光度
			      *Adress_ABS = A_BS;					                            //保存ABS值
			      Adress_Fulllight ++;
			      Adress_result96  ++;
			      Adress_ABS ++;
			      Num_Channel --;
		 	  }

			  Num_Channel = 8;
			  Num_Queue ++ ;		                   //采集完1列则列变量自加1
			  if(k == 1)
			  {   
				   Adress_Fulllight = AD_ALLlight_Single;	   //重置单波长下光源直射时8通道地址
			  }
			  else
			  {
				   Adress_Fulllight = AD_ALLlight_Double;	   //重置双波长下光源直射时8通道地址
			  }
	   }
		 Num_Queue = 0;
		 if( Wave_Flag == 1) //双波长法
		 {
				i --;
				k --;
				Adress_Fulllight = AD_ALLlight_Double;				   //重置光源直射时8通道地址
				Adress_result96  = &AD_Halflight_Double[0][0];				   //双波长96孔数据地址
				Adress_ABS       = &ABS_DoubleWave[0][0];				   //双波长96孔ABS值地址
		 }
		 else//单波长
		 {
		   i = i - 2;
		 }
  }

//-------------------------测试数据----------
	for(i=0;i<12;i++)
	{
		for(k=0;k<8;k++)
		{
			ABS_SingleWave0[i][k] = ABS_SingleWave[i][k];
			ABS_DoubleWave0[i][k] = ABS_DoubleWave[i][k];
		}
	}
//-------------------
		

		
// 	ReadFlash(1);            //从flash中读取主波长的定标系数（K、b值及测试值）
	//修正单波长模式下ABS显示值
	if(Wavelength_Main == 1)                        //||(Wavelength_Main == 5))	405nm波长 
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_405nm[n][1])											               //当小于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_405nm[n][0])									                   //当小于0.2A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][0] + Intercept_405nm[n][0];	   //修正0-0.2A的ABS值
				   }
				   else																					               //当大于等于0.2小于0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][1] + Intercept_405nm[n][1];	   //修正0.2-0.5A的ABS值
				   }
			   }
			   else if(ABS_SingleWave[m][n] < TestValue_405nm[n][3])																									   ///当大于等于0.5A测量值
			   {
			   	   if(ABS_SingleWave[m][n] < TestValue_405nm[n][2])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][2] + Intercept_405nm[n][2];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][3] + Intercept_405nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }				 
			   else																									   ///当大于等于0.5A测量值
			   {
			   	   if(ABS_SingleWave[m][n] < TestValue_405nm[n][4])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][4] + Intercept_405nm[n][4];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_405nm[n][5] + Intercept_405nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
		   }
	    }
	}
	else if(Wavelength_Main == 2)			                     //450nm波长
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_450nm[n][1])											               //当小于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_450nm[n][0])									                   //当小于0.2A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][0] + Intercept_450nm[n][0];	   //修正0-0.2A的ABS值
				   }
				   else																					               //当大于等于0.2小于0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][1] + Intercept_450nm[n][1];	   //修正0.2-0.5A的ABS值
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_450nm[n][3])																								   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_450nm[n][2])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][2] + Intercept_450nm[n][2];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][3] + Intercept_450nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
			   else																									   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_450nm[n][4])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][4] + Intercept_450nm[n][4];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_450nm[n][5] + Intercept_450nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
		   }
	    }
	}
	else if(Wavelength_Main == 3)                //||(Wavelength_Main == 6))	492nm波长 
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_492nm[n][1])											               //当小于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_492nm[n][0])									                   //当小于0.2A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][0] + Intercept_492nm[n][0];	   //修正0-0.2A的ABS值
				   }
				   else																					               //当大于等于0.2小于0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][1] + Intercept_492nm[n][1];	   //修正0.2-0.5A的ABS值
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_492nm[n][3])																								   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_492nm[n][2])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][2] + Intercept_492nm[n][2];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][3] + Intercept_492nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
			   else																									   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_492nm[n][4])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][4] + Intercept_492nm[n][4];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_492nm[n][5] + Intercept_492nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
		   }
	    }
	}
	else	if(Wavelength_Main == 4)		                                            //630nm波长
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_630nm[n][1])											               //当小于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_630nm[n][0])									                   //当小于0.2A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][0] + Intercept_630nm[n][0];	   //修正0-0.2A的ABS值
				   }
				   else																					               //当大于等于0.2小于0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][1] + Intercept_630nm[n][1];	   //修正0.2-0.5A的ABS值
				   }
			   }
			   else if(ABS_SingleWave[m][n] < TestValue_630nm[n][3])																										   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_630nm[n][2])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][2] + Intercept_630nm[n][2];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][3] + Intercept_630nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
			   else																									   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_630nm[n][4])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][4] + Intercept_630nm[n][4];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_630nm[n][5] + Intercept_630nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
		   }
	  }
	}
	else	if(Wavelength_Main == 5)		                                            //扩展波长01
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][1])											               //当小于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][0])									                   //当小于0.2A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext01nm[n][0] + Intercept_Ext01nm[n][0];	   //修正0-0.2A的ABS值
				   }
				   else																					               //当大于等于0.2小于0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext01nm[n][1] + Intercept_Ext01nm[n][1];	   //修正0.2-0.5A的ABS值
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][3])																								   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][2])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext01nm[n][2] + Intercept_Ext01nm[n][2];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = TestValue_Ext01nm[m][n] * Slope_Ext01nm[n][3] + Intercept_Ext01nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
			   else																									   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext01nm[n][4])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext01nm[n][4] + Intercept_Ext01nm[n][4];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = TestValue_Ext01nm[m][n] * Slope_Ext01nm[n][5] + Intercept_Ext01nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
		   }
	  }
	}
	else	if(Wavelength_Main == 6)		                                            //扩展波长02
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][1])											               //当小于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][0])									                   //当小于0.2A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][0] + Intercept_Ext02nm[n][0];	   //修正0-0.2A的ABS值
				   }
				   else																					               //当大于等于0.2小于0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][1] + Intercept_Ext02nm[n][1];	   //修正0.2-0.5A的ABS值
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][3])																									   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][2])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][2] + Intercept_Ext02nm[n][2];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][3] + Intercept_Ext02nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
			   else																									   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext02nm[n][4])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][4] + Intercept_Ext02nm[n][4];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext02nm[n][5] + Intercept_Ext02nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
		   }
	  }
	}
	else	if(Wavelength_Main == 7)		                                            //扩展波长03
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][1])											               //当小于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][0])									                   //当小于0.2A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][0] + Intercept_Ext03nm[n][0];	   //修正0-0.2A的ABS值
				   }
				   else																					               //当大于等于0.2小于0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][1] + Intercept_Ext03nm[n][1];	   //修正0.2-0.5A的ABS值
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][3])																								   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][2])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][2] + Intercept_Ext03nm[n][2];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][3] + Intercept_Ext03nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
			   else																									   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext03nm[n][4])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][4] + Intercept_Ext03nm[n][4];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext03nm[n][5] + Intercept_Ext03nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
		   }
	  }
	}
	else	if(Wavelength_Main == 8)		                                            //扩展波长04
	{   
	    for(m=0;m<12;m++)
	    {
		   for(n=0;n<8;n++)
		   {   
		     if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][1])											               //当小于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][0])									                   //当小于0.2A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][0] + Intercept_Ext04nm[n][0];	   //修正0-0.2A的ABS值
				   }
				   else																					               //当大于等于0.2小于0.5A
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][1] + Intercept_Ext04nm[n][1];	   //修正0.2-0.5A的ABS值
				   }
			   }
			   else	if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][3])																								   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][2])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][2] + Intercept_Ext04nm[n][2];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][3] + Intercept_Ext04nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
			   else																									   ///当大于等于0.5A测量值
			   {
			   	 if(ABS_SingleWave[m][n] < TestValue_Ext04nm[n][4])													   //当小于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][4] + Intercept_Ext04nm[n][4];	   //修正0.5-1.0A的ABS值
				   }
				   else																								   //当大于等于1.0A测量值
				   {
				   	   ABS_SingleWave[m][n] = ABS_SingleWave[m][n] * Slope_Ext04nm[n][5] + Intercept_Ext04nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
				   }
			   }
		   }
	  }
	}

	
	//修正双波长模式下ABS显示值														   
	if(Wave_Flag == 1)							 //双波长法下计算ABS值
	{
		 Wavelength_Main = Wavelength_Second;			 //次波长值赋给主波长
// 		 ReadFlash(1);									 //读取次波长的定标系数
		 if(Wavelength_Second == 1)                        //||(Wavelength_Second == 5))	405nm次波长  
	   {   
	     for(m=0;m<12;m++)
	     {
		     for(n=0;n<8;n++)
		     {   
					 if(ABS_DoubleWave[m][n] < TestValue_405nm[n][1])											               //当小于0.5A测量值
			     {
							if(ABS_DoubleWave[m][n] < TestValue_405nm[n][0])									                   //当小于0.2A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][0] + Intercept_405nm[n][0];	   //修正0-0.2A的ABS值
							}
							else																					               //当大于等于0.2小于0.5A
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][1] + Intercept_405nm[n][1];	   //修正0.2-0.5A的ABS值
							}
					 }
					 else	if(ABS_DoubleWave[m][n] < TestValue_405nm[n][3])																								   ///当大于等于0.5A测量值
					 {
							if(ABS_DoubleWave[m][n] < TestValue_405nm[n][2])													   //当小于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][2] + Intercept_405nm[n][2];	   //修正0.5-1.0A的ABS值
							}
							else																								   //当大于等于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][3] + Intercept_405nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
							}
					 }
					 else																									   ///当大于等于0.5A测量值
					 {
							if(ABS_DoubleWave[m][n] < TestValue_405nm[n][4])													   //当小于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][4] + Intercept_405nm[n][4];	   //修正0.5-1.0A的ABS值
							}
							else																								   //当大于等于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_405nm[n][5] + Intercept_405nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
							}
					 }
					 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//双波长法下计算ABS = 主波长ABS - 次波长ABS
		     }
	     }
	   }
	   else if(Wavelength_Second == 2)			                     //450nm波长
	   {   
	     for(m=0;m<12;m++)
	     {
		     for(n=0;n<8;n++)
		     {   
					 if(ABS_DoubleWave[m][n] < TestValue_450nm[n][1])											               //当小于0.5A测量值
					 {
							if(ABS_DoubleWave[m][n] < TestValue_450nm[n][0])									                   //当小于0.2A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][0] + Intercept_450nm[n][0];	   //修正0-0.2A的ABS值
							}
							else																					               //当大于等于0.2小于0.5A
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][1] + Intercept_450nm[n][1];	   //修正0.2-0.5A的ABS值
							}
					 }
					 else if(ABS_DoubleWave[m][n] < TestValue_450nm[n][3])																									   ///当大于等于0.5A测量值
					 {
							if(ABS_DoubleWave[m][n] < TestValue_450nm[n][2])													   //当小于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][2] + Intercept_450nm[n][2];	   //修正0.5-1.0A的ABS值
							}
							else																								   //当大于等于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][3] + Intercept_450nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
							}
					 }
					 else																									   ///当大于等于0.5A测量值
					 {
							if(ABS_DoubleWave[m][n] < TestValue_450nm[n][4])													   //当小于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][4] + Intercept_450nm[n][4];	   //修正0.5-1.0A的ABS值
							}
							else																								   //当大于等于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_450nm[n][5] + Intercept_450nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
							}
					 }
					 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//双波长法下计算ABS = 主波长ABS - 次波长ABS
				 }
			 }
		 }
		 else if(Wavelength_Second == 3)                         //(Wavelength_Second == 6))			  492nm波长 
		 {   
			 for(m=0;m<12;m++)
			 {
				 for(n=0;n<8;n++)
				 {   
					 if(ABS_DoubleWave[m][n] < TestValue_492nm[n][1])											               //当小于0.5A测量值
					 {
							if(ABS_DoubleWave[m][n] < TestValue_492nm[n][0])									                   //当小于0.2A测量值
							{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][0] + Intercept_492nm[n][0];	   //修正0-0.2A的ABS值
							}
							else																					               //当大于等于0.2小于0.5A
							{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][1] + Intercept_492nm[n][1];	   //修正0.2-0.5A的ABS值
							}
					 }
					 else	if(ABS_DoubleWave[m][n] < TestValue_492nm[n][3])																								   ///当大于等于0.5A测量值
					 {
							if(ABS_DoubleWave[m][n] < TestValue_492nm[n][2])													   //当小于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][2] + Intercept_492nm[n][2];	   //修正0.5-1.0A的ABS值
							}
							else																								   //当大于等于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][3] + Intercept_492nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
							}
					 }
					 else																									   ///当大于等于0.5A测量值
					 {
							if(ABS_DoubleWave[m][n] < TestValue_492nm[n][4])													   //当小于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][4] + Intercept_492nm[n][4];	   //修正0.5-1.0A的ABS值
							}
							else																								   //当大于等于1.0A测量值
							{
									ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_492nm[n][5] + Intercept_492nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
							}
					 }
					 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//双波长法下计算ABS = 主波长ABS - 次波长ABS
				 }
			 }
		}
		else if(Wavelength_Second == 4)		                                            //630nm波长
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_630nm[n][1])											               //当小于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_630nm[n][0])									                   //当小于0.2A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][0] + Intercept_630nm[n][0];	   //修正0-0.2A的ABS值
						}
						else																					               //当大于等于0.2小于0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][1] + Intercept_630nm[n][1];	   //修正0.2-0.5A的ABS值
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_630nm[n][3])																									   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_630nm[n][2])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][2] + Intercept_630nm[n][2];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][3] + Intercept_630nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 else																									   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_630nm[n][4])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][4] + Intercept_630nm[n][4];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_630nm[n][5] + Intercept_630nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//双波长法下计算ABS = 主波长ABS - 次波长ABS
			 }
		 }
	  }	
		else if(Wavelength_Second == 5)		                                            //扩展波长1
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][1])											               //当小于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][0])									                   //当小于0.2A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][0] + Intercept_Ext01nm[n][0];	   //修正0-0.2A的ABS值
						}
						else																					               //当大于等于0.2小于0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][1] + Intercept_Ext01nm[n][1];	   //修正0.2-0.5A的ABS值
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][3])																								   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][2])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][2] + Intercept_Ext01nm[n][2];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][3] + Intercept_Ext01nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 else																									   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext01nm[n][4])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][4] + Intercept_Ext01nm[n][4];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext01nm[n][5] + Intercept_Ext01nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//双波长法下计算ABS = 主波长ABS - 次波长ABS
			 }
		 }
	  }	
		else if(Wavelength_Second == 6)		                                            //扩展波长02
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][1])											               //当小于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][0])									                   //当小于0.2A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][0] + Intercept_Ext02nm[n][0];	   //修正0-0.2A的ABS值
						}
						else																					               //当大于等于0.2小于0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][1] + Intercept_Ext02nm[n][1];	   //修正0.2-0.5A的ABS值
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][3])																									   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][2])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][2] + Intercept_Ext02nm[n][2];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][3] + Intercept_Ext02nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 else																									   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext02nm[n][4])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][4] + Intercept_Ext02nm[n][4];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext02nm[n][5] + Intercept_Ext02nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//双波长法下计算ABS = 主波长ABS - 次波长ABS
			 }
		 }
	  }	
		else if(Wavelength_Second == 7)		                                            //扩展波长03
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][1])											               //当小于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][0])									                   //当小于0.2A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][0] + Intercept_Ext03nm[n][0];	   //修正0-0.2A的ABS值
						}
						else																					               //当大于等于0.2小于0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][1] + Intercept_Ext03nm[n][1];	   //修正0.2-0.5A的ABS值
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][3])																								   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][2])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][2] + Intercept_Ext03nm[n][2];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][3] + Intercept_Ext03nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 else																									   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext03nm[n][4])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][4] + Intercept_Ext03nm[n][4];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext03nm[n][5] + Intercept_Ext03nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//双波长法下计算ABS = 主波长ABS - 次波长ABS
			 }
		 }
	  }	
		else if(Wavelength_Second == 8)		                                            //扩展波长04
		{   
			for(m=0;m<12;m++)
		  {
			 for(n=0;n<8;n++)
			 {   
				 if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][1])											               //当小于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][0])									                   //当小于0.2A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][0] + Intercept_Ext04nm[n][0];	   //修正0-0.2A的ABS值
						}
						else																					               //当大于等于0.2小于0.5A
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][1] + Intercept_Ext04nm[n][1];	   //修正0.2-0.5A的ABS值
						}
				 }
				 else	if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][3])																								   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][2])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][2] + Intercept_Ext04nm[n][2];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][3] + Intercept_Ext04nm[n][3];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 else																									   ///当大于等于0.5A测量值
				 {
						if(ABS_DoubleWave[m][n] < TestValue_Ext04nm[n][4])													   //当小于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][4] + Intercept_Ext04nm[n][4];	   //修正0.5-1.0A的ABS值
						}
						else																								   //当大于等于1.0A测量值
						{
								ABS_DoubleWave[m][n] = ABS_DoubleWave[m][n] * Slope_Ext04nm[n][5] + Intercept_Ext04nm[n][5];	   //修正1.0-1.5A或大于1.5A的ABS值
						}
				 }
				 ABS_SingleWave[m][n] -= ABS_DoubleWave[m][n];									//双波长法下计算ABS = 主波长ABS - 次波长ABS
			 }
		 }
	  }			
	}

	Wave_Flag = 0;
}																									//清双波长标志	

