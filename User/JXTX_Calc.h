
#ifndef __JXTX_Calc_H
#define __JXTX_Calc_H

#include "main.h"

struct Gate
{
	unsigned short StartTime;	//起始时刻
	unsigned short EndTime;		//结束时刻
	unsigned short OpTime;		//分合闸动作时间
	float EndValue;
	float Speed;							//分合闸速度
	float InstantSpeed;				//分合闸瞬时速度
	float TotalDistance;			//总行程
	float OpenDistance;				//开距
	float ReboundDistance;		//反弹高度
};

struct EnergyValue
{
	unsigned short StartTime;	//起始时刻
	unsigned short EndTime;		//结束时刻
	unsigned short Tc;		//分合闸动作时间
	unsigned short Ia_Time;
	unsigned short Im_Time;
	float Ia;
	float Ie;							//分合闸速度
	float Im;				//分合闸瞬时速度
};

extern float Calc_Buf[2000];
extern float Calc_speed[2000];
extern float Calc_FilterBuf[2000];
extern struct Gate GateOpen,GateClose;
extern struct EnergyValue Energy;

void GetCalcData(float *Input,unsigned short *Output,unsigned short Num,unsigned char Channel);
void GetAvr(float *p,float *Avr,unsigned int Num);
void MovingAvrFilter(float *InputData,float *OutData,unsigned int Num,unsigned short Deepth);
void GetMax(float *p,float *MaxData,unsigned short *Index,unsigned int Num);
void GetMin(float *p,float *MinData,unsigned short *Index,unsigned int Num);
void GetSpeed(float *p,float *speed,unsigned int Num);
void GetStartTime(float *SourceDat,float *Speed,unsigned short *Index,unsigned short TotalNum,unsigned short Num);
void GetEndTime(float *SourceDat,unsigned short *Index,unsigned short TotalNum,unsigned char OpenOrClose);
void CalcAngle(float *p,float *angle,unsigned short Num,unsigned char OpenOrClose);
void GetCloseData(float *SourceDat,unsigned short *Index,float TargetDat,unsigned short Num);
void GetReboundDistance(float *SourceDat,float *ReboundDistance,unsigned short Num);
void Get_T1(float *p,unsigned short *Index,unsigned short Num);
void Get_T2(float *p,unsigned short *Index,unsigned short StartNum,unsigned short EndNum);
void Get_T3(float *p,unsigned short *Index,float TargetDat,unsigned short StartNum,unsigned short EndNum);
void Get_T4(float *p,unsigned short *Index,unsigned short StartNum,unsigned short EndNum);
void Get_T5(float *p,unsigned short *Index,unsigned short Num,unsigned short StartNum);

#endif
