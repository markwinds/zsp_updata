/*摄像头60*80，屏幕128*128*/
#include "common.h"
#include "include.h"


Screen_Data screen_data[] = {
	
	{"M_KP", &(motor_pid.P), 0.1, 1},
	{"M_KI", &(motor_pid.I), 0.01, 2},
	{"M_KD", &(motor_pid.D), 0.01, 3},
	
	{"enM", &(motor_go), 99, 0}, //使能电机
	{"speed", &(motor_speed), 10, 4},

	{"length", &(total_distance), 500, 0},

//	{"Aspeed", &(velocity), 0, 0},	

	{"flash", &(flash_in), 1, -1},
	{"de_pic", &(delete_picture), 1, 0},
	{"se_pic", &(read_all_picture), 1, 0},

	{"S_KP", &(steer_pid.P), 0.01, 5},
	{"S_KD", &(steer_pid.D), 0.1, 6},

	{"end", &(temp_s[9]), 1202, 0}
	
};

void main(void)
{
	init_Sys();
	//steer_pid.P = 0;
	while (1)
	{
		/*----------使能赛道采集,再去处理图像---------*/
		ov7725_eagle_img_flag = IMG_START; //开始采集图像
		PORTA_ISFR = ~0;				   //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
		enable_irq(PORTA_IRQn);			   //允许PTA的中断

		/*-----------根据模式进行处理----------*/
		if (lcd_mode == IMG_MODE)
		{
			DcdMode(); //显示模式下，显示赛道
		}
		else if (lcd_mode == PICTURE_MODE)
		{
			read_Picture_Array(); //falsh!
			LCD_num(tem_site_data[3], picture_count, GREEN, BLUE);
		}
		else if (lcd_mode == UI_MODE)
		{
			Open_UI(); //ui界面用于调控参数
		}
		else if (lcd_mode == RUN_MODE)
		{
			if(stop_save_motor < 7) stop_save_motor = motor_speed;
			motor_speed = 0;
		}


		/*-----------速度和距离的一些更新---------*/
		Controll();

		/*---------图像处理结束,检验采集结束---------*/
		while (ov7725_eagle_img_flag != IMG_FINISH) //等待下一帧图像采集完毕
		{
			if (ov7725_eagle_img_flag == IMG_FAIL) //假如图像采集错误，则重新开始采集
			{
				ov7725_eagle_img_flag = IMG_START; //开始采集图像
				PORTA_ISFR = ~0;				   //写1清中断标志位(必须的，不然会导致一开中断就马上触发中断)
				enable_irq(PORTA_IRQn);			   //允许PTA的中断
			}
		}
		do_Sys();
	} //while
}
