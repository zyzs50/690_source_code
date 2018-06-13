#ifndef __CALIBRATE_H__
#define __CALIBRATE_H__
#include "pbdata.h"

// extern u16  AD_Alllight[10][8];		   //保存光源直射时8通道AD值(10次走板共80个AD值)

extern u8 Wavelength_Flag; //测样时，是否使用双波长， 1：使用双波长， 0：使用单波长
extern float AD_ALLlight_Single[8];          //保存单波长下光源直射时8通道AD值
extern float AD_ALLlight_Double[8];	        //保存次波长（双波长模式）光源直射时8通道AD值			  
extern float AD_Halflight_Single[12][8];     //保存单波长下有检测物时96孔AD值（8行12列）
extern float AD_Halflight_Double[12][8];     //保存次波长（双波长模式）下有检测物时96孔AD值（8行12列）


extern float Slope_405nm[8][6];      //保存405nm各通道K（斜率）大小(数组第一维表示通道数，第二维表示每个通道K值个数)
extern float Slope_450nm[8][6];	    //保存450nm各通道K（斜率）大小(数组第一维表示通道数，第二维表示每个通道K值个数)
extern float Slope_492nm[8][6];	    //保存492nm各通道K（斜率）大小(数组第一维表示通道数，第二维表示每个通道K值个数)
extern float Slope_630nm[8][6];	    //保存630nm各通道K（斜率）大小(数组第一维表示通道数，第二维表示每个通道K值个数)
extern float Slope_Ext01nm[8][6];	    //扩展滤光片01
extern float Slope_Ext02nm[8][6];	    //扩展滤光片02
extern float Slope_Ext03nm[8][6];	    //扩展滤光片03
extern float Slope_Ext04nm[8][6];	    //扩展滤光片04

extern float Intercept_405nm[8][6];   //保存405nm各通道b值（截距）大小(数组第一维表示通道数，第二维表示每个通道b值个数)
extern float Intercept_450nm[8][6];   //保存450nm各通道b值（截距）大小(数组第一维表示通道数，第二维表示每个通道b值个数)
extern float Intercept_492nm[8][6];   //保存492nm各通道b值（截距）大小(数组第一维表示通道数，第二维表示每个通道b值个数)
extern float Intercept_630nm[8][6];   //保存630nm各通道b值（截距）大小(数组第一维表示通道数，第二维表示每个通道b值个数)
extern float Intercept_Ext01nm[8][6];	  //扩展滤光片01
extern float Intercept_Ext02nm[8][6];	  //扩展滤光片02
extern float Intercept_Ext03nm[8][6];	  //扩展滤光片03
extern float Intercept_Ext04nm[8][6];	  //扩展滤光片04

extern float TestValue_405nm[8][5];	 //保存405nm波长下0.2A,0.5A和1.0A标准物质的8通道测量值
extern float TestValue_450nm[8][5];	 //保存450nm波长下0.2A,0.5A和1.0A标准物质的8通道测量值
extern float TestValue_492nm[8][5];	 //保存492nm波长下0.2A,0.5A和1.0A标准物质的8通道测量值
extern float TestValue_630nm[8][5];	 //保存630nm波长下0.2A,0.5A和1.0A标准物质的8通道测量值	
extern float TestValue_Ext01nm[8][5];	  //扩展滤光片01
extern float TestValue_Ext02nm[8][5];	  //扩展滤光片02
extern float TestValue_Ext03nm[8][5];	  //扩展滤光片03
extern float TestValue_Ext04nm[8][5];	  //扩展滤光片04

extern u8 SelectCalib;//选择哪种标准物质的靶值
extern u8 targetValueA[96];
extern u8 targetValueB[96];
extern u8 targetValueC[96];


extern void ADC0_Sample_Mode(float *address,u8 mode);//通道 AD 采集
extern u8 AD_Loop_Sample_K_B(u8 mode,u8 type);//10次走板AD 采集
extern void Data_Handling_Optical(u8 mode,u8 type);//处理采集到的数据，计算ABS
extern void X_Y_Kb(u8 wave,u8 mode);//最小二乘法 求 K,B
extern void Data_Handling_LiquidSamples(u8 Wavelength_Main);
extern void Array_Combination_Optical(u8 Wavelength_Main,u8 Number_Channel);	  
extern void USB_Out_K_B_Abs(u8 mode,u8 wave);

extern u8 AD_Loop_Sample(float* addr);//一次12列走板AD采集
extern void Data_Handling(u8 Wavelength_Main,u8 Wavelength_Second);
extern void USB_Out_Sample_Abs(u8 wave,u8 out_mode);

#endif


