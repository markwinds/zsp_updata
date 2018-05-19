/*摄像头60*80，屏幕128*128*/
#include "common.h"
#include "include.h"


int _temp = 0;
Screen_Data screen_data[] = {

	{"P", &(STEER_KP), 0.1, 1},
	{"I", &(STEER_KI), 0.1, 2},
	{"D", &(STEER_KD), 0.1, 3},

	{"enM", &(motor_go), 99, 0}, //使能电机
	{"speed", &(target_speed), 10, 4},

	{"length", &(total_distance), 500, 0},

	{"flash", &(flash_in), 1, -1},
	{"de_pic", &(delete_picture),1,0},
	{"se_pic", &(read_all_picture),1,0},

	{"end", &(temp_s[9]), 1202, 0}};

/*
图像不压缩直接显示
*/
void main(void)
{
	Site_t site = {0, 0};
	Size_t size;
	Size_t imgsize = {CAMERA_W, CAMERA_H};
	Size_t imgsize1 = {P_col, P_row};
	size.H = LCD_H;
	size.W = LCD_W;
	//size.H = LCD_H/2;
	//size.W = LCD_W/2;
	// size.H = 60;
	// size.W = 80;

	PTB18;
	init_Sys();
    camera_get_img();    //相机获取第一帧图像
	while (1)
	{
		ov7725_eagle_img_flag = IMG_START;      //开始采集图像
		PORTA_ISFR = ~0;                        //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
		enable_irq(PORTA_IRQn);                 //允许PTA的中断
		//img_compress(img, imgbuff, CAMERA_SIZE);        
		if (IMG_MODE == lcd_mode)
		{				
			img_extract(img, imgbuff, CAMERA_SIZE); //解压图像
			tran_O_P();
			img_compress(img_P,P_buff,P_row*P_col/8);	//压缩P图
			//vcan_sendimg(P_buff,P_row*P_col/8);
			//show_P();
			//Search_line(); //找线
			//Negation();
			LCD_Img_Binary_Z(site, size, P_buff,imgsize1);
			//LCD_Img_Binary_G(site, size, img);
		}
		else if (PICTURE_MODE == lcd_mode)
		{
			read_Picture_Array();
			LCD_num(tem_site_data[3], picture_count, GREEN, BLUE);
		}
		else
			Open_UI();

		
		/*-----------速度和距离的一些更新---------*/
		Update_Motor();
		motor_speed=0;
		if (UI_MODE == lcd_mode)
		{
			if (1 == ((int)motor_go) % 2 && total_distance < 1000)
				ftm_pwm_duty(FTM0, FTM_CH5, (int)motor_speed);
			else
				ftm_pwm_duty(FTM0, FTM_CH5, 0);
			ftm_pwm_duty(FTM0, FTM_CH6, 380); //舵机回中
		}
		else if (PICTURE_MODE == lcd_mode)
		{
			ftm_pwm_duty(FTM0, FTM_CH5, 0);
			ftm_pwm_duty(FTM0, FTM_CH6, 380);
		}
		else
		{
			if (total_distance < 1000)
			{
				ftm_pwm_duty(FTM0, FTM_CH5, (int)motor_speed); //电机
			}
			else
				ftm_pwm_duty(FTM0, FTM_CH5, 0);

			ftm_pwm_duty(FTM0, FTM_CH6, 380 + (int)steer_engine_degree); //舵机
		}
		while(ov7725_eagle_img_flag != IMG_FINISH)           //等待下一帧图像采集完毕
		{
			if(ov7725_eagle_img_flag == IMG_FAIL)            //假如图像采集错误，则重新开始采集
			{
				ov7725_eagle_img_flag = IMG_START;           //开始采集图像
				PORTA_ISFR = ~0;                //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
				enable_irq(PORTA_IRQn);                 //允许PTA的中断
			}
		}
	    do_Sys();
	} //while
}

