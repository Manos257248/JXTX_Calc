//#include <string.h>
#include "Calculate.h"
//#include <stdio.h>
//#include <math.h>

//float FilterData[2000];
//float MmtSpeed[2000];
//float FilterSpeed[2000];
//float Input[2000];
float Contain1[2000];		//�м�����
float Contain2[2000];		//�м�����
unsigned char Open = 0;
unsigned char Close = 1;



/**********����
*���������г�ʱ��Ҫ�˲�
*������բ˲���ٶȵ��ٶ���Ҫ�˲�
*��������բ˲ʱ�ٶ��˲���31
*
****************/

/******����
1�����к�����Ϊvoid
2����������Input���ܻ�����ĳ�ͻ����
3���ÿ������������memcpy������
4�淶����������
5���յı���Ҫ�ŵ��ṹ����
******/

//Ѱ���±�
//@parm	  ��һ��Valueֵ���±�
void GetX(const float *InputData, unsigned int *OutputData, unsigned int FromData, unsigned int ToData, float Value)
{
//	memcpy(Input, InputData, 8000);
	unsigned short i;
	for(i = FromData; i < ToData; i ++)
	{
		if(InputData[i] == Value)
		{
			*OutputData = i;
			break;
		}
	}
}

//Deepthλ������ֵ�˲�����NUM�������
void MovingAvgFilter(const float *InputData,float *OutputData,unsigned int Num, unsigned short Deepth)
{
	unsigned short i = 0;
	unsigned short j = 0;
	float sum = 0;
//	memcpy(Input, InputData, 8000);
	for(i = 0; i < Deepth; i ++)
	{
		OutputData[i] = InputData[i];
	}
//	for(i=Deepth;i<Num-Deepth;i++)
//	{
//		OutData[i] = GetAvr(&InputData[i],Deepth);
//	}
	for(i = Deepth; i < Num-Deepth; i ++)
	{
		for(j = i; j < i + Deepth; j ++)
		{
			sum = sum + InputData[j - (Deepth / 2)];
		}
		OutputData[i] = sum / Deepth;
		sum = 0;
	}
	for(i = Num - Deepth; i < Num; i ++)
	{
		OutputData[i] = InputData[i];
	}
}

//��FromNum��ToNum�����ݵ�ƽ������������ToNum��
void GetAvg(const float *InputData, float *OutputData, unsigned int FromNum, unsigned int ToNum)
{
	unsigned short i = 0;
	float Sum = 0.0;
	for(i = FromNum; i <= ToNum; i ++)
	{
		Sum = Sum + InputData[i];
	}
	*OutputData = Sum / (ToNum - FromNum);
}

//��Num��������У����˲ʱ�ٶ�
void GetMmtSpeed(const float *InputData, float *OutputData, unsigned int Num)
{
	unsigned short i;
//	memcpy(Input, InputData, 8000);
	OutputData[0] = 0;
	for(i = 0; i < Num; i ++)
	{
		OutputData[i + 1] = (InputData[i + 1] - InputData[i]) / 1.0;
//		if(OutputData[i + 1] < 0)		//ȡ����ֵ
//		{
//			OutputData[i + 1] = 0 - OutputData[i + 1];
//		}
	}
}

//��FromData��ToData��InputData��Ѱ�����ֵmax
void GetMax(const float *InputData, float *OutputData, unsigned int FromNum, unsigned int ToNum)
{
//	memcpy(Input, InputData, 8000);
	unsigned short i;
	float Data = InputData[0];
	for(i = FromNum; i < ToNum; i ++)
	{
		if(Data < InputData[i])
		{
			Data = InputData[i];
		}
	}
	*OutputData = Data;
}

//��FromData��ToData��InputData��Ѱ����Сֵmin
void GetMin(const float *InputData, float *OutputData, unsigned int FromNum, unsigned int ToNum)
{
//	memcpy(Input, InputData, 8000);
	unsigned short i;
	float Data = InputData[0];
	for(i = FromNum; i < ToNum; i ++)
	{
		if(Data > InputData[i])
		{
			Data = InputData[i];
		}
	}
	*OutputData = Data;

}

//Ѱ��ADֵ��ʼ�±�
void GetADStartTime(const float *ADInputData, unsigned int *OutputData, unsigned int Num, unsigned char OpenOrClose)
{
	float ImSpeed[2000];
	float Avg = 0.0;
	unsigned short i, Cnt = 0;
	unsigned int Index;
	GetMmtSpeed(ADInputData, ImSpeed, Num);
	if(OpenOrClose == Open)			//Open��բ
	{
		for(i = 100; i < 1000; i ++)
		{
			if(ImSpeed[i] > 0)
			{
				Cnt ++;
				if(Cnt == 30)					//AD��ʼ����1������30�����ݶ�����0
				{
					GetAvg(ImSpeed, &Avg, i - 29, i);
					if(Avg >= 8)				//AD��ʼ����2����բ30����ֵ > 8
					{
						Index = i - 29 + 1;	//�±� + 1
						*OutputData = Index;
						break;
					}
					else Cnt = 0;
				}
			}
			else Cnt = 0;

		}
	}
	else							// Close��բ
	{
		for(i = 0; i < Num; i ++)
		{
			if(ImSpeed[i] > 0)
			{
				Cnt ++;
				if(Cnt == 30)					//AD��ʼ����1������30�����ݶ�����0
				{
					GetAvg(ImSpeed, &Avg, i - 29, i);
					if(Avg >= 7)				//AD��ʼ����2����բ30����ֵ > 7
					{
						Index = i - 29 + 1;	//�±� + 1
						*OutputData = Index;
						break;
					}
					else Cnt = 0;
				}
				else Cnt = 0;
			}
			else Cnt = 0;
		}
	}
}

//�г̻���Ϊ�Ƕ�
void CalcAngle(const float *DistanceInputData, float *OutputData, unsigned int Num, unsigned char OpenOrClose)
{
	unsigned short i;
	if(OpenOrClose == Open)
	{
		MovingAvgFilter(DistanceInputData, Contain1, 2000, 31);
		for(i = 0; i < Num; i ++)
			OutputData[i] = (Contain1[i] - 10000) / 4 / 2048 * 360;		//��բÿʱ�̽Ƕ�
	}
	else
	{
		MovingAvgFilter(DistanceInputData, Contain1, 2000, 61);
		for(i = 0; i < Num; i ++)
			OutputData[i] = (Contain1[i] - 10000) / 4 / 2048 * 360;		//��բÿʱ�̽Ƕ�
	}
}

//�ǶȻ���Ϊ�г�
void CalcDistance(const float *AngleInputData, float *OutputData, unsigned int Num)
{
	unsigned short i;
	for(i = 0; i < Num; i ++)
		OutputData[i] = AngleInputData[i] * 4 * 2048 / 360 + 10000;		//ÿʱ���г�
}

//ADֵ����Ϊ����
void CalcIm(const float *ADInputData, float *OutputData, unsigned int Num, float HuoerImax)
{
	float Avg;
	unsigned short i = 0;
	GetAvg(ADInputData, &Avg, 0, 19);	//ǰ20�����ݾ�ֵ
	for(i = 0; i < Num; i ++)
		OutputData[i] = (ADInputData[i] - Avg) * 5 / 32768 * HuoerImax;
}


/*************************
*��դ�߽���ʱ���±�(�պϡ��շ�)
*
*
*************************/
void GetDistanceEndTime(const float *DistanceInputData, unsigned int *OutputData, unsigned int Num, unsigned char OpenOrClose)
{
	float SpeedValue;
	unsigned int Index;
	if(OpenOrClose == Open)		//Open��
	{
		MovingAvgFilter(DistanceInputData, Contain1, Num, 31);
		GetMmtSpeed(Contain1, Contain2, Num);
		MovingAvgFilter(Contain2, Contain1, Num, 31);
		GetMin(Contain1, &SpeedValue, 0, Num);
		GetX(Contain1, &Index, 0, Num, SpeedValue);
//		if((Index > 700) | (Index < 300))
//		{
//			*EndTime = 0;	
//		}
//		else
//		{
			*OutputData = Index + 1;					//�շ�ʱ��			
//		}
	}
	else						//Close��
	{
		MovingAvgFilter(DistanceInputData, Contain1, Num, 61);
		GetMmtSpeed(Contain1, Contain2, Num);
		MovingAvgFilter(Contain2, Contain1, Num, 61);
		GetMax(Contain1, &SpeedValue, 0, Num);
		GetX(Contain1, &Index, 0, Num, SpeedValue);
//		if((Index > 900) | (Index < 500))
//		{
//			*EndTime = 0;
//		}
//		else
//		{
			*OutputData = (short)((Index + 1) * 0.9565 - 18.55 + 0.5);		//�պ�ʱ�̣���������
//		}
	}
}

/******************
*�ֺ�բʱ�䣨�±꣩
*
*
******************/
void GetOpenOrCloseTime(const float *ADInputData, const float *DistanceInputData, unsigned int *OpenCloseTime, unsigned int Num, unsigned char OpenOrClose)
{
	unsigned int GangheTime = 0;
	unsigned int GangfenTime = 0;
	unsigned int OpenTime = 0;
	unsigned int CloseTime = 0;
	unsigned int ADTime = 0;
	if(OpenOrClose == Open)	//��բʱ��
	{
		GetDistanceEndTime(DistanceInputData, &GangfenTime, Num, Open);
		GetADStartTime(ADInputData, &ADTime, Num, Open);
		OpenTime = GangfenTime - ADTime;
	}
	else			//
	{
		GetDistanceEndTime(DistanceInputData, &GangheTime, Num, Close);
		GetADStartTime(ADInputData, &ADTime, Num, Close);
		CloseTime = GangheTime - ADTime;
	}
	if(OpenOrClose == Open)
		*OpenCloseTime = OpenTime;
	else
		*OpenCloseTime = CloseTime;

}

/****************************
*��բ���г� 
*ÿʱ���г� = (Data - 10000) / 2048 /4 * 360
*
****************************/
void GetTotalDistance(const float *DistanceInputData, float *OutputData, unsigned int Num)
{
	float AvgFrist20 = 0.0;
	float AvgLast20 = 0.0;
	MovingAvgFilter(DistanceInputData, Contain2, Num, 61);
	CalcAngle(Contain2, Contain1, Num, Close);
//	for(i = 0; i < 20; i ++)
//	{
//		Contain2[i] = Contain1[i];
//	}
	GetAvg(Contain1, &AvgFrist20, 0, 19);					//[0] ~ [19]
//	for(i = 0; i < 20; i ++)
//	{
//		Contain2[i] = Contain1[Num - 20 + i];
//	}
	GetAvg(Contain1, &AvgLast20, Num - 20, Num - 1);	//[1980] ~ [1999]
	*OutputData = (AvgLast20 - AvgFrist20);
}

/************************
*����
*
*
*************************/
void GetKaiDistance(const float *DistanceInputData, float *OutputData, unsigned int Num, unsigned char OpenOrClose)
{
	unsigned int gangheTime = 0;
	float gangheData = 0.0;
	float AvgFrist20 = 0.0;
	unsigned short i;
	for(i = 0; i < Num; i ++)
	{
		Contain1[i] = (DistanceInputData[i] - 10000) / 2048 /4 * 360;	//ÿʱ�̽Ƕ�
	}
	GetDistanceEndTime(DistanceInputData, &gangheTime, Num, OpenOrClose);
	gangheData = Contain1[gangheTime];
//	for(i = 0; i < 20; i ++)
//	{
//		Contain2[i] = Contain1[i];
//	}
	GetAvg(Contain1, &AvgFrist20, 0, 19);	//[0] ~ [19]
	*OutputData = gangheData - AvgFrist20;
	
}

/*********************
*�ֺ�բ�ٶ�
*
*
**********************/
void OpenOrCloseSpeed(const float *DistanceInputData, float *OutputData, unsigned int Num, unsigned char OpenOrClose)
{
	unsigned int Time = 0;
	float gangheData = 0.0;
	float Angle;
	unsigned int Index;
	unsigned short i;
//	MovingAvgFilter(DistanceInputData, Contain2, Num, 61);				//�˲�
	for(i = 0; i < Num; i ++)
		Contain1[i] = (DistanceInputData[i] - 10000) / 2048 /4 * 360;	//ÿʱ�̽Ƕ�
	if(OpenOrClose == Open)
	{
		GetDistanceEndTime(DistanceInputData, &Time, Num, Close);		//�պ�ʱ��
//		gangheData = Contain1[Time];									//�պ�����
		CalcAngle(Contain1, Contain2, Num, Close);						//ÿʱ������ת��Ϊÿʱ�̽Ƕ�
		Angle = Contain2[Time] - 3.5;
		GetX(Contain2, &Index, 0, Num, Angle);
		*OutputData = 3.5 / (Time - Index);								//��բ�ٶ�
	}
	else
	{
		GetDistanceEndTime(DistanceInputData, &Time, Num, Open);		//�շ�ʱ��
//		gangheData = Contain1[Time];									//�շ�����
		CalcAngle(Contain1, Contain2, Num, Open);						//ÿʱ�̵�����ת��Ϊÿʱ�̵ĽǶ�
		Angle = Contain2[Time] + 6.5;
		GetX(Contain2, &Index, 0, Num, Angle);
		*OutputData = 6.5 / (Index - Time);								//��բ�ٶ�

	}
}

/************************
*�ֺ�բ˲���ٶ�
*
*
*************************/
void OpenOrCloseMmtSpeed(const float *DistanceInputData, float *OutputData, unsigned int Num, unsigned char OpenOrClose)
{
	unsigned int Time;
	if(OpenOrClose == Open)			//��բ
	{
		GetDistanceEndTime(DistanceInputData, &Time, Num, Open);
		MovingAvgFilter(DistanceInputData, Contain1, Num, 31);
		GetMmtSpeed(Contain1, Contain2, Num);
		MovingAvgFilter(Contain2, Contain1, Num, 31);	//�˲����˲ʱ�ٶ�
		*OutputData = Contain1[Time];

	}
	else							//��բ
	{
		GetDistanceEndTime(DistanceInputData, &Time, Num, Close);
		MovingAvgFilter(DistanceInputData, Contain1, Num, 61);
		GetMmtSpeed(Contain1, Contain2, Num);
		MovingAvgFilter(Contain2, Contain1, Num, 61);	//�˲����˲ʱ�ٶ�
		*OutputData = Contain1[Time];
	}
}

/*************************
*��բ����
*
*
*
**************************/
void CloseOverrush(const float *DistanceInputData, float *OutputData, unsigned int Num)
{
	float Max;
	float Avglast20;
	GetMax(DistanceInputData, &Max, 0, Num);
	GetAvg(DistanceInputData, &Avglast20, Num - 20, Num - 1);	//[1980] ~ [1999]
	*OutputData = Max - Avglast20;
}

/*************************
*��բ�����߶�
*
*
*
***************************/
void ReboundHeight(const float *DistanceInputData, float *OutputData, unsigned int Num)
{
	float Avglast20 = 0.0;
	float Max;
	unsigned int ReboundTime;
	CalcAngle(DistanceInputData, Contain1, Num, Open);
	GetAvg(Contain1, &Avglast20, Num - 20, Num - 1);	//[1980] ~ [1999]
	GetX(Contain1, &ReboundTime, 0, Num, Avglast20);
	GetMax(Contain1, &Max, ReboundTime, Num);
	*OutputData = Max - Avglast20; 
}

/*************************
*��T0 = Im��ʼ��
*
*
**************************/
void Get_T0(const float *ADInputData, unsigned int *OutputData, unsigned int Num, unsigned char OpenOrClose)
{	
	GetADStartTime(ADInputData, OutputData, Num, OpenOrClose);
}


/*************************
*��T1 = 200���һ���½���
*
*
*
**************************/
void Get_T1(const float *ImInputData, unsigned int *OutputData, unsigned int Num)
{
	unsigned short i;
	for(i = 200; i < Num; i ++)
	{
		if(ImInputData[i] > ImInputData[i + 1])
		{
			*OutputData = i;
			break;
		}
	}
}

/*************************
*��T2 = Min(T1 ~ T3)
*
*
**************************/
void Get_T2(const float *ImInputData, unsigned int *OutputData, unsigned int Num)
{
	unsigned int T1, T3;
	float Min;
	Get_T1(ImInputData, &T1, Num);
	Get_T3(ImInputData, &T3, Num);
	GetMin(ImInputData, &Min, T1, T3);
	GetX(ImInputData, OutputData, T1, T3, Min);
}

/*************************
*��T3 = T4 - 30
*
*
**************************/
void Get_T3(const float *ImInputData, unsigned int *OutputData, unsigned int Num)
{
	unsigned int T4;
	Get_T4(ImInputData, &T4, Num);
	*OutputData = T4 - 30;
}

/*************************
*��T4 = Im���ֵ�±�
*
*
**************************/
void Get_T4(const float *ImInputData, unsigned int *OutputData, unsigned int Num)
{
	float Max = 0.0;
	GetMax(ImInputData, &Max, 0, Num);
	GetX(ImInputData, OutputData, 0, Num, Max);
}

/*************************
*��T5 = (��T4��ʼ��Im = Avg���600)
*
*
**************************/
void Get_T5(const float *ImInputData, unsigned int *OutputData, unsigned int Num)
{
	unsigned int T4;
	float Avglast600;										//��600��Im���ݵ�ƽ��ֵ
	Get_T4(ImInputData, &T4, Num);
//	for(i = 0; i < 600; i ++)
//		Contain1[i] = ImInputData[i + Num - 600];			//��600��Im����ת��Contain1
	GetAvg(ImInputData, &Avglast600, Num - 600, Num - 1);	//��600��Im����[1400] ~ [1999]��ƽ��ֵ
	GetX(ImInputData, OutputData, T4, Num, Avglast600);
}

/**************************
*T1ʱ��Imֵ
*
*
***************************/
void Get_ImT1(const float *ImInputData, float *OutputData, unsigned int Num)
{
	unsigned int T1;
	Get_T1(ImInputData, &T1, Num);
	*OutputData = ImInputData[T1];
}

/**************************
*T2ʱ��Imֵ
*
*
***************************/
void Get_ImT2(const float *ImInputData, float *OutputData, unsigned int Num)
{
	unsigned int T2;
	Get_T2(ImInputData, &T2, Num);
	*OutputData = ImInputData[T2];
}

/**************************
*T3ʱ��Imֵ
*
*
***************************/
void Get_ImT3(const float *ImInputData, float *OutputData, unsigned int Num)
{
	unsigned int T3;
	Get_T3(ImInputData, &T3, Num);
	*OutputData = ImInputData[T3];
}

/**************************
*T4ʱ��Imֵ
*
*
***************************/
void Get_ImT4(const float *ImInputData, float *OutputData, unsigned int Num)
{
	unsigned int T4;
	Get_T4(ImInputData, &T4, Num);
	*OutputData = ImInputData[T4];
}


/**************************
*��բ��������չʾ
*
*
***************************/
void OpenDataShow(const float *DistanceInputData, const float *ADInputData, unsigned int Num, float HuoerImax, OpenGateTypedef *OpenGate)
{
//	CalcDistance(AngleInputData, Contain1, Num);
	CalcIm(ADInputData, Contain2, Num, HuoerImax);
	
	GetDistanceEndTime(DistanceInputData, &(OpenGate->DistanceEndTime), Num, Open);
	GetOpenOrCloseTime(ADInputData, DistanceInputData, &(OpenGate->OpenTime), Num, Open);
	OpenOrCloseSpeed(DistanceInputData, &(OpenGate->OpenSpeed), Num, Open);
	ReboundHeight(DistanceInputData, &(OpenGate->ReboundHeight), Num);
	OpenOrCloseMmtSpeed(DistanceInputData, &(OpenGate->OpenMmtSpeed), Num, Open);
	Get_T0(Contain2, &(OpenGate->T0), Num, Open);
	Get_T1(Contain2, &(OpenGate->T1), Num);
	Get_T2(Contain2, &(OpenGate->T2), Num);
	Get_T3(Contain2, &(OpenGate->T3), Num);
	Get_T4(Contain2, &(OpenGate->T4), Num);
	Get_T5(Contain2, &(OpenGate->T5), Num);
	Get_ImT1(Contain2, &(OpenGate->Get_ImT1), Num);
	Get_ImT2(Contain2, &(OpenGate->Get_ImT2), Num);
	Get_ImT3(Contain2, &(OpenGate->Get_ImT3), Num);
	Get_ImT4(Contain2, &(OpenGate->Get_ImT4), Num);
}

/**************************
*��բ��������չʾ
*
*
***************************/
void CloseDataShow(const float *DistanceInputData, const float *ADInputData, unsigned int Num, float HuoerImax, CloseGateTypedef *CloseGate)
{
//	CalcDistance(AngleInputData, Contain1, Num);
	CalcIm(ADInputData, Contain2, Num, HuoerImax);
	
	GetDistanceEndTime(DistanceInputData, &(CloseGate->DistanceEndTime), Num, Close);
	GetOpenOrCloseTime(ADInputData, DistanceInputData, &(CloseGate->CloseTime), Num, Close);
	GetTotalDistance(DistanceInputData, &(CloseGate->CloseTotalDistance), Num);
	GetKaiDistance(DistanceInputData, &(CloseGate->CloseKaiDistance), Num, Close);
	OpenOrCloseSpeed(DistanceInputData, &(CloseGate->CloseSpeed), Num, Close);
	OpenOrCloseMmtSpeed(DistanceInputData, &(CloseGate->CloseMmtSpeed), Num, Close);
	CloseOverrush(DistanceInputData, &(CloseGate->CloseOverrush), Num);
	Get_T0(ADInputData, &(CloseGate->T0), Num, Close);
	Get_T1(Contain2, &(CloseGate->T1), Num);
	Get_T2(Contain2, &(CloseGate->T2), Num);
	Get_T3(Contain2, &(CloseGate->T3), Num);
	Get_T4(Contain2, &(CloseGate->T4), Num);
	Get_T5(Contain2, &(CloseGate->T5), Num);
	Get_ImT1(Contain2, &(CloseGate->Get_ImT1), Num);
	Get_ImT2(Contain2, &(CloseGate->Get_ImT2), Num);
	Get_ImT3(Contain2, &(CloseGate->Get_ImT3), Num);
	Get_ImT4(Contain2, &(CloseGate->Get_ImT4), Num);
}
