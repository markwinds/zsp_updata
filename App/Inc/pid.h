#ifndef __PID_H__
#define __PID_H__

#include "include.h"
#include "common.h"

#define	DEGREE_MAX    110     //舵机最大偏角
#define	R_DEGREE_MAX  100     //舵机最大偏角
#define L_DEGREE_MAX -100
#define M_QUITE_AREA  7     //中速偏差的静态区，将对数据使用静态处理
#define M_BACK1       3.5      //脱离中线的速度为1挡时
#define M_BACK1_SPEED 3      //回中速度为1挡
#define M_BACK2       2
#define M_BACK2_SPEED 2
#define M_BACK_SPEED  1
#define M_BUFF_AREA   12
#define M_BUFF_SPEED  2

//#define STEER_KP     3.0 
//#define STEER_KI     3.0
//#define STEER_KD     3.0

extern float STEER_KP;
extern float STEER_KI;
extern float STEER_KD;
extern float MOTOR_KP;
extern float MOTOR_KI;
extern float MOTOR_KD;
extern int Increase;
extern int32 iError; //当前误差
extern int32 LastError; //当前误差
extern int32 PrevError;
extern float arcsin[99];


typedef enum Speed_mode
{
	STOP_SPEED,
	LOW_SPEED,
	MIDDLE_SPEED,
	HIGH_SPEED,
	CHECH,
	OTHER
}Speed_mode;

typedef struct PID_t
{
	float P;
	float I;
	float D;
}PID;

extern Speed_mode car_mode;
extern float steer_engine_degree;        //舵机转动的角度
extern float motor_speed;                //电机转速
extern double cor_sp;

extern PID *cmotor, *csteer;
extern PID motor_pid,steer_pid;
extern PID Co_Steer[10];

int Steer_Pid(PID*);
void PID_Realize(PID*,int32, int32);
void Control_core();





#endif  