#ifndef __CALIBRATE_H__
#define __CALIBRATE_H__
#include "pbdata.h"

// extern u16  AD_Alllight[10][8];		   //�����Դֱ��ʱ8ͨ��ADֵ(10���߰干80��ADֵ)

extern u8 Wavelength_Flag; //����ʱ���Ƿ�ʹ��˫������ 1��ʹ��˫������ 0��ʹ�õ�����
extern float AD_ALLlight_Single[8];          //���浥�����¹�Դֱ��ʱ8ͨ��ADֵ
extern float AD_ALLlight_Double[8];	        //����β�����˫����ģʽ����Դֱ��ʱ8ͨ��ADֵ			  
extern float AD_Halflight_Single[12][8];     //���浥�������м����ʱ96��ADֵ��8��12�У�
extern float AD_Halflight_Double[12][8];     //����β�����˫����ģʽ�����м����ʱ96��ADֵ��8��12�У�


extern float Slope_405nm[8][6];      //����405nm��ͨ��K��б�ʣ���С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��Kֵ����)
extern float Slope_450nm[8][6];	    //����450nm��ͨ��K��б�ʣ���С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��Kֵ����)
extern float Slope_492nm[8][6];	    //����492nm��ͨ��K��б�ʣ���С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��Kֵ����)
extern float Slope_630nm[8][6];	    //����630nm��ͨ��K��б�ʣ���С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��Kֵ����)
extern float Slope_Ext01nm[8][6];	    //��չ�˹�Ƭ01
extern float Slope_Ext02nm[8][6];	    //��չ�˹�Ƭ02
extern float Slope_Ext03nm[8][6];	    //��չ�˹�Ƭ03
extern float Slope_Ext04nm[8][6];	    //��չ�˹�Ƭ04

extern float Intercept_405nm[8][6];   //����405nm��ͨ��bֵ���ؾࣩ��С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��bֵ����)
extern float Intercept_450nm[8][6];   //����450nm��ͨ��bֵ���ؾࣩ��С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��bֵ����)
extern float Intercept_492nm[8][6];   //����492nm��ͨ��bֵ���ؾࣩ��С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��bֵ����)
extern float Intercept_630nm[8][6];   //����630nm��ͨ��bֵ���ؾࣩ��С(�����һά��ʾͨ�������ڶ�ά��ʾÿ��ͨ��bֵ����)
extern float Intercept_Ext01nm[8][6];	  //��չ�˹�Ƭ01
extern float Intercept_Ext02nm[8][6];	  //��չ�˹�Ƭ02
extern float Intercept_Ext03nm[8][6];	  //��չ�˹�Ƭ03
extern float Intercept_Ext04nm[8][6];	  //��չ�˹�Ƭ04

extern float TestValue_405nm[8][5];	 //����405nm������0.2A,0.5A��1.0A��׼���ʵ�8ͨ������ֵ
extern float TestValue_450nm[8][5];	 //����450nm������0.2A,0.5A��1.0A��׼���ʵ�8ͨ������ֵ
extern float TestValue_492nm[8][5];	 //����492nm������0.2A,0.5A��1.0A��׼���ʵ�8ͨ������ֵ
extern float TestValue_630nm[8][5];	 //����630nm������0.2A,0.5A��1.0A��׼���ʵ�8ͨ������ֵ	
extern float TestValue_Ext01nm[8][5];	  //��չ�˹�Ƭ01
extern float TestValue_Ext02nm[8][5];	  //��չ�˹�Ƭ02
extern float TestValue_Ext03nm[8][5];	  //��չ�˹�Ƭ03
extern float TestValue_Ext04nm[8][5];	  //��չ�˹�Ƭ04

extern u8 SelectCalib;//ѡ�����ֱ�׼���ʵİ�ֵ
extern u8 targetValueA[96];
extern u8 targetValueB[96];
extern u8 targetValueC[96];


extern void ADC0_Sample_Mode(float *address,u8 mode);//ͨ�� AD �ɼ�
extern u8 AD_Loop_Sample_K_B(u8 mode,u8 type);//10���߰�AD �ɼ�
extern void Data_Handling_Optical(u8 mode,u8 type);//����ɼ��������ݣ�����ABS
extern void X_Y_Kb(u8 wave,u8 mode);//��С���˷� �� K,B
extern void Data_Handling_LiquidSamples(u8 Wavelength_Main);
extern void Array_Combination_Optical(u8 Wavelength_Main,u8 Number_Channel);	  
extern void USB_Out_K_B_Abs(u8 mode,u8 wave);

extern u8 AD_Loop_Sample(float* addr);//һ��12���߰�AD�ɼ�
extern void Data_Handling(u8 Wavelength_Main,u8 Wavelength_Second);
extern void USB_Out_Sample_Abs(u8 wave,u8 out_mode);

#endif


