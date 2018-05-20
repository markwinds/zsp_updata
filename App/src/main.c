/*摄像头60*80，屏幕128*128*/
#include "common.h"
#include "include.h"

//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void DcdMode();
void Controll();

int _temp = 0;
Site_t line_site;
float velocity = 0;
int16 motor_velocity = 0;
Screen_Data screen_data[] = {

	{"P", &(STEER_KP), 0.1, 1},
	{"D", &(STEER_KD), 0.1, 3},

	{"enM", &(motor_go), 99, 0}, //使能电机
	{"speed", &(motor_speed), 10, 4},

	{"length", &(total_distance), 500, 0},

	{"Aspeed", &(velocity), 0, 0},	

	{"flash", &(flash_in), 1, -1},
	{"de_pic", &(delete_picture), 1, 0},	//
	{"se_pic", &(read_all_picture), 1, 0},
	{"M_KP", &(MOTOR_KP), 0.1, 3},
	{"M_KI", &(MOTOR_KI), 0.1, 0},
	{"M_KD", &(MOTOR_KD), 0.1, 0},

	{"end", &(temp_s[9]), 1202, 0}};

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
	ftm_pwm_init(FTM2, FTM_CH0, 300, 0);
	ftm_pwm_init(FTM2, FTM_CH1, 300, 0);
	ftm_pwm_init(FTM0, FTM_CH6, 300, 43);
	UI_INIT();
	led_init(LED0);

	set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler);   //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(PORTD_VECTORn, PORTD_IRQHandler); //ui所需中断的初始化
	Quad_Init();										 //编码器中断
	flash_init();
	flash_Out();	  //读取数据
	camera_get_img(); //相机获取第一帧图像

	while (1)
	{
		ov7725_eagle_img_flag = IMG_START; //开始采集图像
		PORTA_ISFR = ~0;				   //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
		enable_irq(PORTA_IRQn);			   //允许PTA的中断

		//清屏专用
		if (please_clear)
		{
			LCD_clear(WHITE);
			please_clear = 0;
		}
		
		if (IMG_MODE == lcd_mode)
		{
			DcdMode(); //显示模式下，显示赛道
		}
		else if (PICTURE_MODE == lcd_mode)
		{
			read_Picture_Array(); //falsh!
			LCD_num(tem_site_data[3], picture_count, GREEN, BLUE);
		}
		else
			Open_UI(); //ui界面用于调控参数

		enable_irq(PORTD_IRQn); //使能按键中断

		if ((delete_picture - 0.3) > 0.5)
		{
			delete_Picture(); //flash!
			delete_picture = 0;
		}

		/*-----------速度和距离的一些更新---------*/
		Controll();

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

void DcdMode()
{
	img_extract(img, imgbuff, CAMERA_SIZE); //解压图像
	//temp_s[6] = Find_slope();
	Search_line(); //找线
	Negation();
	// img_compress(img, imgbuff, CAMERA_SIZE);		//图像压缩
	// LCD_Img_Binary_Z(site, size, imgbuff, imgsize); //lcd显示图像
	LCD_Img_Binary_G(site, size, img);

	if (is_show_va) //是能够在IMG_MODE模式下显示数据
	{
		// if(state_line[0] == 0)Judge_circul();
		LCD_numf(tem_site_str[4], (float)is_leftcircul_flag, GREEN, BLUE);
		// if(state_line[0] == 3) Goin_circul();
		LCD_numf(tem_site_str[5], (float)is_rightcircul_flag, GREEN, BLUE);
		// LCD_numf(tem_site_data[4], (float)state_line[1], GREEN, BLUE);
		// LCD_numf(tem_site_data[5], (float)state_line[3], GREEN, BLUE);
	}
	/*彩色显示边线*/
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
	Control_core();

	if (1 == key_on)
		enable_irq(PORTD_IRQn); //激活按键中断

	save_Picture(); //检测是否需要将图片写入flash
}

void Controll()
{
	Update_Motor();
	if(quad_time >= 25){
		quad_time = 0;
		velocity = (temp_velocity << 2);
		temp_velocity = 0;
		motor_velocity = (int)motor_speed;
	}

	if (UI_MODE == lcd_mode)
	{
		if (1 == ((int)motor_go) % 2 && total_distance < 1000)
			Con_Motor(motor_velocity);
		else
			Con_Motor(0);
		ftm_pwm_duty(FTM0, FTM_CH6, 380); //舵机回中
	}
	else if (PICTURE_MODE == lcd_mode)
	{
		Con_Motor(0);
		ftm_pwm_duty(FTM0, FTM_CH6, 380);
	}
	else
	{
		if (total_distance < 1000)
		{
			Con_Motor(motor_velocity); //电机
		}
		else
			Con_Motor(0);

		ftm_pwm_duty(FTM0, FTM_CH6, 380 + (int)steer_engine_degree); //舵机
	}
	while (ov7725_eagle_img_flag != IMG_FINISH) //等待下一帧图像采集完毕
	{
		if (ov7725_eagle_img_flag == IMG_FAIL) //假如图像采集错误，则重新开始采集
		{
			ov7725_eagle_img_flag = IMG_START; //开始采集图像
			PORTA_ISFR = ~0;				   //写1清中断标志位(必须的，不然会导致一开中断就马上触发中断)
			enable_irq(PORTA_IRQn);			   //允许PTA的中断
		}
	}
}