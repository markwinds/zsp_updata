/*
用于ui的一些函数、变量、结构体
*/
#ifndef _LCD_SHOW_H_
#define _LCD_SHOW_H_


/*------------------------------------------------include----------------------------------------------*/
#include "common.h"
#include "include.h"


/*------------------------------------------------define------------------------------------------------*/
#define IMG_MODE 1
#define UI_MODE  2
#define ITEM_NUM 3

#define MAX_OPTION 30                        //lcd能使用的最大的参数个数
#define SECTOR_NUM  (FLASH_SECTOR_NUM-1)     //最后一个扇区


/*---------------------------------------------状态模式的一些结构体---------------------------------------*/
typedef struct Screen_Data      //传参结构体,ui显示数据的结构体
{
	char *data_name;  			//参数名称
	float *data_value; 			//参数值
	float icrement;   			//参数累加数 如果是100就是on\off
	int ip;  					//是否存放入flash的标志位
}Screen_Data;

typedef struct Lcd_State //状态结构体
{
	struct Lcd_State *(*press_M)(struct Lcd_State *pThis); //函数指针，press_M指向一个返回值是Lcd_State指针类型，参数是Lcd_State指针类型的函数
	struct Lcd_State *(*press_U)(struct Lcd_State *pThis); //指向上建按下时的处理函数
	struct Lcd_State *(*press_D)(struct Lcd_State *pThis);
	struct Lcd_State *(*press_L)(struct Lcd_State *pThis);
	struct Lcd_State *(*press_R)(struct Lcd_State *pThis);
}Lcd_State;


/*------------------------------------------------全局变量---------------------------------------------*/
extern Lcd_State wait_middle, wait_begin, wait_end, normal_page, imgbuff_show; //lcd等待按键时的4种状态
extern Lcd_State *p_current_state;	//指向当前状态的指针

extern int page;					//ui当前所在页
extern int current_row;				//ui当前所在行
extern Screen_Data screen_data[];	//ui显示的参数


extern float flash_in;				//是否将参数写进flash的标志，改变一次写入一次
extern float motor_go;  			//在显示状态下控制电机是否转动的变量
extern uint8 key_on;
extern uint8 lcd_mode;
extern uint8 is_show_va;
extern uint8 is_show_line;
extern uint8 please_clear;
extern int colour[];				//存放各个参数背景色的数组
extern Site_t tem_site_str[];
extern Site_t tem_site_data[];


/*--------------------------------------------------函数--------------------------------------------------*/
void onpress_M(); //中间按下时的处理函数
void onpress_U();
void onpress_D();
void onpress_L();
void onpress_R();

/*对应按键的动作函数*/
Lcd_State *quit_Lcd(Lcd_State *pThis);
Lcd_State *goto_Begin(Lcd_State *pThis);
Lcd_State *goto_End(Lcd_State *pThis);
Lcd_State *ignore_Oprate(Lcd_State *pThis);
Lcd_State *goto_Set(Lcd_State *pThis);
Lcd_State *goto_Wait(Lcd_State *pThis);
Lcd_State *goto_next(Lcd_State *pThis);
Lcd_State *data_Down(Lcd_State *pThis);
Lcd_State *data_Up(Lcd_State *pThis);
Lcd_State *goto_Before(Lcd_State *pThis);

Lcd_State *quit_show(Lcd_State *pThis);
Lcd_State *open_va(Lcd_State *pThis);
Lcd_State *close_va(Lcd_State *pThis);
Lcd_State *ushow_line(Lcd_State *pThis);
Lcd_State *show_line(Lcd_State *pThis);
Lcd_State *do_nothing(Lcd_State *pThis);

/*普通函数*/
void PORTD_IRQHandler();
void UI_INIT();
void Open_UI();
extern void flash_In();
extern void flash_Out();
extern void next_Write_Location();




#endif