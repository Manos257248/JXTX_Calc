
#ifndef __JXTX_Calc_H
#define __JXTX_Calc_H

#include "main.h"

struct Gate
{
	unsigned short StartTime;	//��ʼʱ��
	unsigned short EndTime;		//����ʱ��
	unsigned short OpTime;		//�ֺ�բ����ʱ��
	float EndValue;
	float Speed;							//�ֺ�բ�ٶ�
	float InstantSpeed;				//�ֺ�բ˲ʱ�ٶ�
	float TotalDistance;			//���г�
	float OpenDistance;				//����
	float ReboundDistance;		//�����߶�
};

struct EnergyValue
{
	unsigned short StartTime;	//��ʼʱ��
	unsigned short EndTime;		//����ʱ��
	unsigned short Tc;		//�ֺ�բ����ʱ��
	unsigned short Ia_Time;
	unsigned short Im_Time;
	float Ia;
	float Ie;							//�ֺ�բ�ٶ�
	float Im;				//�ֺ�բ˲ʱ�ٶ�
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
