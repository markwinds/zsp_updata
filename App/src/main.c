/*摄像头60*80，屏幕128*128*/
#include "common.h"
#include "include.h"

//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void DcdMode();
void Controll();
void RunMode();
//变量声明
int _temp = 0;
Site_t line_site;
float velocity = 0;
int16 motor_velocity = 0;
Screen_Data screen_data[] = {
	
	{"M_KP", &(motor_pid.P), 0.01, 1},
	{"M_KI", &(motor_pid.I), 0.01, 2},
	{"M_KD", &(motor_pid.D), 0.01, 3},
	
	{"enM", &(motor_go), 99, 0}, //使能电机
	{"speed", &(motor_speed), 10, 4},

	{"length", &(total_distance), 500, 0},

//	{"Aspeed", &(velocity), 0, 0},	

	{"flash", &(flash_in), 1, -1},
	{"de_pic", &(delete_picture), 1, 0},
	{"se_pic", &(read_all_picture), 1, 0},


	{"end", &(temp_s[9]), 1202, 0}
	
};

/*
图像不压缩直接显示
*/
Site_t site = {0, 0};
Size_t size;
Size_t imgsize = {CAMERA_W, CAMERA_H};

void main(void)
{
	//size.H = LCD_H;
	//size.W = LCD_W;
	//size.H = LCD_H/2;
	//size.W = LCD_W/2;
	size.H = 60;
	size.W = 80;

	LCD_init();
	camera_init(imgbuff);
	UI_INIT();		  
	led_init(LED0);
	Quad_Init();	  //编码器中断
	flash_init();
	flash_Out();	  //读取数据	
	ftm_pwm_init(FTM2, FTM_CH0, 300, 0);
	ftm_pwm_init(FTM2, FTM_CH1, 300, 0);
	ftm_pwm_init(FTM0, FTM_CH6, 300, 43);

	set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler);   //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(PORTD_VECTORn, PORTD_IRQHandler); //ui所需中断的初始化

	camera_get_img(); //相机获取第一帧图像

	while (1)
	{
		/*----------使能赛道采集,再去处理图像---------*/
		ov7725_eagle_img_flag = IMG_START; //开始采集图像
		PORTA_ISFR = ~0;				   //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
		enable_irq(PORTA_IRQn);			   //允许PTA的中断

		/*-----------清屏---------*/
		if (please_clear)
		{
			LCD_clear(WHITE);
			please_clear = 0;
		}

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
			RunMode();
		}

		/*-----------处理琐事---------*/
		if ((delete_picture - 0.3) > 0.5)
		{
			delete_Picture(); //flash!
			delete_picture = 0;
		}
		enable_irq(PORTD_IRQn); //使能按键中断

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

	} //while
}

/*!
*  @brief      PORTA中断服务函数，与摄像头有关
*  @since      v5.0
*/
void PORTA_IRQHandler()
{
	uint8 n; //引脚号
	uint32 flag;
	_temp++;
	while (!PORTA_ISFR)
		;
	flag = PORTA_ISFR;
	PORTA_ISFR = ~0; //清中断标志位

	n = 29;				 //场中断
	if (flag & (1 << n)) //PTA29触发中断
	{
		camera_vsync();
	}
#if (CAMERA_USE_HREF == 1) //使用行中断
	n = 28;
	if (flag & (1 << n)) //PTA28触发中断
	{
		camera_href();
	}
#endif
}

/*!
*  @brief      DMA0中断服务函数，与摄像头有关
*  @since      v5.0
*/
void DMA0_IRQHandler()
{
	camera_dma();
}

/***
 * @brief		显示模式下处理图像和搜线
 * 
***/
void DcdMode()
{
	img_extract(img, imgbuff, CAMERA_SIZE); //解压图像
	//temp_s[6] = Find_slope();
	Search_line(); //找线
	Negation();
	// img_compress(img, imgbuff, CAMERA_SIZE);		//图像压缩
	// LCD_Img_Binary_Z(site, size, imgbuff, imgsize); //lcd显示图像
	LCD_Img_Binary_G(site, size, img);

	/*------------在图像下面显示数据-----------*/
	if (is_show_va)
	{
		// if(state_line[0] == 0)Judge_circul();
		LCD_numf(tem_site_str[4], (double)ac_quad, GREEN, BLUE);
		// if(state_line[0] == 3) Goin_circul();
		LCD_numf(tem_site_str[5], (double)Increase, GREEN, BLUE);
		// LCD_numf(tem_site_data[4], (float)state_line[1], GREEN, BLUE);
		// LCD_numf(tem_site_data[5], (float)state_line[3], GREEN, BLUE);
	}

	/*----------彩色显示边线,还有显示网格-----------*/
	if (is_show_line == 1 || is_show_line == 3) //网格
	{
		LCD_grid();
	}
	if (is_show_line == 2 || is_show_line == 3) //边线
	{
		int i;
		for (i = 0; i < 60; i++)
		{
			line_site.x = left_black[i];
			line_site.y = i;
			if (left_black[i] == -3)
				break;
			if (left_black[i] >= 0)
				LCD_point(line_site, RED);
		}
		for (i = 0; i < 60; i++)
		{
			line_site.x = right_black[i];
			line_site.y = i;
			if (right_black[i] == -3)
				break;
			if (right_black[i] >= 0)
				LCD_point(line_site, BLUE);
		}
	}

	Control_core(); //控制舵机

	if (1 == key_on)
		enable_irq(PORTD_IRQn); //激活按键中断

	save_Picture(); //检测是否需要将图片写入flash
}

/***
 * @brief		不同模式下控制电机和舵机的方案选择
 * 
***/
void Controll()
{
	Update_Motor();

	if (lcd_mode == UI_MODE)
	{
		if (motor_go < 0 && total_distance < 1000)
			cmotor = &motor_pid;
		else
		{
			cmotor = NULL;
			Increase = 0;
			Con_Motor(0);
		}
		ftm_pwm_duty(FTM0, FTM_CH6, 380); //舵机回中
	}
	else if (lcd_mode == PICTURE_MODE)
	{
		cmotor = NULL;
		Increase = 0;
		Con_Motor(0);
		ftm_pwm_duty(FTM0, FTM_CH6, 380);
	}
	else if(lcd_mode == IMG_MODE)
	{
		if (total_distance < 1000)
		{
			cmotor = &motor_pid; //电机
		}
		else
		{
			cmotor = NULL;
			Increase = 0;
			Con_Motor(0);
		}
		//ftm_pwm_duty(FTM0, FTM_CH6, 380 + (int)steer_engine_degree); //舵机
	}
}

/***
 * @brief		全速前进
 * 
***/
void RunMode()
{

}