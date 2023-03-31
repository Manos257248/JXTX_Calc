#include <stdio.h>
#include "JXTX_Calc.h"
//***********合闸计算************//
//技算开始时刻：获取电流的微分集合，
//				连续20数值大于0的时刻
//计算结束时刻：对行程数据做41深度的滑动均值滤波，
//				获取滤波后的微分集合，
//				找到最大速度时刻点maxindex
//				结束时刻endtime=maxindex+1-56
//合闸总行程计算：转换角度（value-10000）/2048/4*360
//				总行程=结束20平均值-开始20平均值
//合闸开距计算：结束时刻点的角度-开始20个角度平均值
//合闸速度计算：合闸前3.5°这段行程的速度
//合闸瞬时速度：获取角度的微分集合，找到Endtime时刻的速度即合闸瞬时速度

//***********分闸计算************//
//技算开始时刻：获取电流的微分集合，
//				连续20数值大于0的时刻
//计算结束时刻：对行程数据做21深度的滑动均值滤波，
//				获取滤波后的微分集合，
//				找到最小速度时刻点minindex
//				结束时刻endtime=minindex+1-4
//合闸速度计算：刚分后6.5°这段行程的速度
//分闸反弹高度：转换角度（value-10000）/2048/4*360
//				获取后20个的角度平均值EndAvg
//				反弹高度=EndAvg-角度最小值
//分闸瞬时速度：获取角度的微分集合，找到Endtime时刻的速度即分闸瞬时速度

float Calc_Buf[4000];
float Calc_speed[2000];
float Calc_FilterBuf[2000];
float Calc_Angle[2000];
unsigned short xx;

struct Gate GateOpen,GateClose;
struct EnergyValue Energy;

void U16toFloat(unsigned short *Input,float *Output,unsigned short Num)
{
	unsigned short i;
	for(i=0;i<Num;i++)
	{
		Output[i] = Input[i];
	}
}

void GetCalcData(unsigned short *Input,float *Output,unsigned short Num,unsigned char Channel)
{
	unsigned short i;
	for(i=0;i<Num;i++)
	{
		Output[i] = Input[6+i*6+Channel];
	}
}
float GetAvr(float *p,unsigned int Num)
{
	unsigned int i = 0;
	float Sum = 0;
	for(i=0;i<Num;i++)
	{
		Sum += p[i];
	}
	return (Sum/Num);
}

void MovingAvrFilter(float *InputData,float *OutData,unsigned int Num,unsigned short Deepth)
{
	float t[2000];
	unsigned int i = 0;
	unsigned int j = 0;
	float sum = 0;
	memcpy(t,InputData,8000);
	for(i=0;i<Deepth;i++)
	{
		OutData[i] = InputData[i];
	}
//	for(i=Deepth;i<Num-Deepth;i++)
//	{
//		OutData[i] = GetAvr(&InputData[i],Deepth);
//	}
	for(i=Deepth;i<Num-Deepth;i++)
	{
		for(j=i;j<i+Deepth;j++)
		{
			sum = sum + t[j-(Deepth/2)];
		}
		OutData[i] = sum/Deepth;
		sum = 0;
	}
	for(i=Num-Deepth;i<Num;i++)
	{
		OutData[i] = InputData[i];
	}
}

void GetMax(float *p,float *MaxData,unsigned short *Index,unsigned int Num)
{
	unsigned short i = 0;
	float data = p[0];
	for(i=1;i<Num;i++)
	{
		if(data<p[i])
		{
			data = p[i];
			*Index = i;
		}
	}
	*MaxData = data;
}

void GetMin(float *p,float *MinData,unsigned short *Index,unsigned int Num)
{
	unsigned short i = 0;
	float data = p[0];
	for(i=1;i<Num;i++)
	{
		if(data>p[i])
		{
			data = p[i];
			*Index = i;
		}
	}
	*MinData = data;
}

void GetSpeed(float *p,float *speed,unsigned int Num)
{
	unsigned int i = 0;
	speed[0] = 0;
	for(i=1;i<Num;i++)
	{
		speed[i] = p[i] - p[i-1];
	}
}

unsigned short Get_I(float *p,float AD,unsigned short liangcheng)
{
	unsigned short i=0;
	float avr = 0;
	float value = 0;
	avr = GetAvr(p,20);
	value = (AD-avr)*5/32768*liangcheng*100;
	return (unsigned short)(value);
}

//void GetStartTime(float *SourceDat,float *Speed,unsigned short *Index,unsigned short TotalNum,unsigned short Num)
//{
//	float sum=0;
//	float avr=0;
//	unsigned short i=0;
//	unsigned short j=0;
//	unsigned short t=0;
//	for(i=0;i<100;i++)
//		avr+=SourceDat[i];
//	avr = avr/100;
//	for(i=0;i<TotalNum-Num;i++)
//	{
//		for(j=i;j<i+Num;j++)
//		{
//			if(Speed[j]>0)
//				t++;
//			else
//			{
//				if(SourceDat[j]-avr>30)
//					t++;
//			}
//		}
//		if(t==Num)
//		{
//			*Index = i;
//			return;
//		}
//		else
//			t=0;
//	}
//}

short GetStartTime(float *SourceDat,unsigned short TotalNum,unsigned char OpenOrClose)
{
	float t[2000];
	float avr=0;
	unsigned int i=0;
	unsigned int j=0;
	unsigned int cnt=0;
	if(OpenOrClose==0)//fenzha
	{	
		GetSpeed(SourceDat,Calc_speed,TotalNum);
		//MovingAvrFilter(Calc_speed,t,2000,21);
		for(i=100;i<1000;i++)
		{
			for(j=i;j<i+30;j++)
			{
				if(Calc_speed[j]>0)
					cnt++;
				else
					break;
			}
			if(cnt==30)
			{
				if(GetAvr(&Calc_speed[i],30)>=8)
					return i+1;
			}
			cnt = 0;
		}
		return -1;
	}
	else
	{
		GetSpeed(SourceDat,Calc_speed,TotalNum);
		//MovingAvrFilter(Calc_speed,t,2000,21);
		for(i=100;i<1000;i++)
		{
			for(j=i;j<i+60;j++)
			{
				if(Calc_speed[j]>=0)
					cnt++;
				else
					break;
			}
			if(cnt==60)
			{
				if(GetAvr(&Calc_speed[i],30)>=7)
					return i+1;
			}
			cnt = 0;
		}
		return -1;		
	}
}

unsigned short GetEndTime(float *SourceDat,unsigned short TotalNum,unsigned char OpenOrClose)
{
	float t[2000];
	float value;
	unsigned short index=0;
	
	if(OpenOrClose==0)//open
	{
		MovingAvrFilter(SourceDat,t,2000,31);
		GetSpeed(t,Calc_speed,2000);
		MovingAvrFilter(Calc_speed,t,2000,31);
		GetMin(t,&value,&index,2000);
		if((index>700)|(index<300))
			return 0;
		else
			return index+1;
	}
	else//close
	{
		MovingAvrFilter(SourceDat,t,2000,61);
		GetSpeed(t,Calc_speed,2000);
		MovingAvrFilter(Calc_speed,t,2000,61);
		GetMax(t,&value,&index,2000);
		if((index>900)|(index<500))
			return 0;
		else
		{
			value = (index+1)*0.9565-18.55;
			return (short)(value);
		}
	}
}

void CalcAngle(float *p,float *angle,unsigned short Num,unsigned char OpenOrClose)
{
	float t[2000];
	unsigned short i;
	
	if(OpenOrClose==0)
	{
		MovingAvrFilter(p,t,2000,31);
		for(i=0;i<Num;i++)
			angle[i] = (10000-t[i])/4/2048*360;
	}
	else
	{
		MovingAvrFilter(p,t,2000,61);
		for(i=0;i<Num;i++)
			angle[i] = (t[i]-10000)/4/2048*360;
	}
}

void GetCloseData(float *SourceDat,unsigned short *Index,float TargetDat,unsigned short Num)
{
	unsigned short i = 0;
	float delta = 0;
	delta = fabs(SourceDat[0]-TargetDat);
	for(i=1;i<Num;i++)
	{
		if(fabs(SourceDat[i]-TargetDat)<delta)
		{
			delta = fabs(SourceDat[i]-TargetDat);
			*Index = i;
		}
	}
}

void GetReboundDistance(float *SourceDat,float *ReboundDistance,unsigned short Num)
{
	unsigned short i;
	unsigned short Index;
	float avr;
	avr = GetAvr(&SourceDat[1979],20);
	for(i=0;i<Num;i++)
	{
		if(SourceDat[i]>=avr)
		{
			Index = i;
			break;
		}
	}
	GetMin(&SourceDat[Index],ReboundDistance,&i,Num-Index);
	*ReboundDistance = avr-*ReboundDistance;
}

void Get_T1(float *p,unsigned short *Index,unsigned short Num)
{
	unsigned short i;
	if(Num<200)
	{
		*Index = 0;
		return;
	}
	else
	{
		for(i=200;i<Num;i++)
		{
			if(p[i+1]<p[i])
			{
				*Index = i;
				return;
			}
		}
	}
}

void Get_T2(float *p,unsigned short *Index,unsigned short StartNum,unsigned short EndNum)
{
	unsigned short i;
	float t=0;
	t = p[StartNum];
	for(i=StartNum;i<EndNum;i++)
	{
		if(t>p[i])
		{
			t=p[i];
			*Index = i;
		}
	}
}

void Get_T3(float *p,unsigned short *Index,float TargetDat,unsigned short StartNum,unsigned short EndNum)
{
	unsigned short i;
	if(StartNum<0)
	{
		*Index = i;
		return;
	}
	else
	{
		for(i=StartNum;i>EndNum;i--)
		{
			if(p[i]<TargetDat)
			{
				*Index = i;
				return;
			}
		}
	}
}

void Get_T4(float *p,unsigned short *Index,unsigned short StartNum,unsigned short EndNum)
{
	unsigned short i;
	float t=0;
	t = p[StartNum];
	if(StartNum<0)
	{
		*Index = i;
		return;
	}
	else
	{
		for(i=StartNum;i<EndNum;i++)
		{
			if(t<p[i])
			{
				t=p[i];
				*Index = i;
			}
		}
	}
}

void Get_T5(float *p,unsigned short *Index,unsigned short Num,unsigned short StartNum)
{
	unsigned short i;
	//unsigned int avr;
	float avr=0;
	for(i=Num-200;i<Num;i++)
		avr = avr + p[i];
	avr = avr/200;
	if(StartNum<0)
	{
		*Index = i;
		return;
	}
	else
	{
		for(i=StartNum;i<Num;i++)
		{
			if(p[i]<avr)
			{
				*Index = i;
				return;
			}
		}
	}
}

/*
	GetSpeed(OpenData,Calc_speed,2000);
	GetStartTime(OpenData,Calc_speed,&GateOpen.StartTime,2000,20);
	printf("分闸起始时刻 = %d, \n",GateOpen.StartTime);
	GetEndTime(OpenDistanceData,&GateOpen.EndTime,2000,0);
	GateOpen.EndTime = GateOpen.EndTime +1 -4;
	printf("分闸结束时刻 = %d, \n",GateOpen.EndTime);
	printf("分闸时间 = %d, \n",GateOpen.EndTime-GateOpen.StartTime);
	CalcAngle(OpenDistanceData,Calc_Angle,2000,0);
	GetCloseData(Calc_Angle,&xx,(Calc_Angle[GateOpen.EndTime]+6.5),2000);
	GateOpen.Speed = 65.0/(xx-GateOpen.EndTime);
	printf("分闸速度 = %f, \n",GateOpen.Speed);
	GetSpeed(Calc_Angle,Calc_speed,2000);
	printf("分闸瞬时速度 = %f, \n",Calc_speed[GateOpen.EndTime]*10);
	GetReboundDistance(Calc_Angle,&GateOpen.ReboundDistance,2000);
	printf("反弹高度 = %f, \n",GateOpen.ReboundDistance);

	GetSpeed(CloseData,Calc_speed,2000);
	GetStartTime(CloseData,Calc_speed,&GateClose.StartTime,2000,20);
	printf("合闸起始时刻 = %d, \n",GateClose.StartTime);
	GetEndTime(CloseDistanceData,&GateClose.EndTime,2000,1);
	GateClose.EndTime = GateClose.EndTime +1 -56;
	printf("合闸结束时刻 = %d, \n",GateClose.EndTime);
	printf("合闸时间 = %d, \n",GateClose.EndTime-GateClose.StartTime);
	CalcAngle(CloseDistanceData,Calc_Angle,2000,1);
	GateClose.TotalDistance = Calc_Angle[1999];
	printf("合闸总行程 = %f, \n",GateClose.TotalDistance);
	GateClose.OpenDistance = Calc_Angle[GateClose.EndTime];
	printf("合闸开距 = %f, \n",GateClose.OpenDistance);
	GetCloseData(Calc_Angle,&xx,(Calc_Angle[GateClose.EndTime]-3.5),2000);
	//printf("xx = %d, \n",xx);
	GateClose.Speed = 35.0/(GateClose.EndTime-xx);
	printf("合闸速度 = %f, \n",GateClose.Speed);
	GetSpeed(Calc_Angle,Calc_speed,2000);
	printf("合闸瞬时速度 = %f, \n",Calc_speed[GateClose.EndTime]*10);
*/