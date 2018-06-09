#include "include.h"

Site_t line_site={0,0};

Site_t site = {0, 0};
Size_t size = {80, 60};
Size_t imgsize = {CAMERA_W, CAMERA_H};
char stree_y[10000];
char offset_y[10000];
int count_x=0;
float out_xy=2;



/*!
*  @brief      PORTA中断服务函数，与摄像头有关
*  @since      v5.0
*/
void PORTA_IRQHandler()
{
	uint8 n; //引脚号
	uint32 flag;
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
 * @brief	  在while循环里面每次都要执行的非核心操作
 * 
 ***/
void do_Sys()
{
	int i=0;

    /*-----------清屏---------*/
	if (please_clear)
	{
		LCD_clear(WHITE);
		please_clear = 0;
	}
    
	/*-----------处理琐事---------*/
	if ((delete_picture - 0.3) > 0.5)
	{
		delete_Picture(); //flash!
		delete_picture = 0;
	}
	//enable_irq(PORTD_IRQn); //使能按键中断

	if(count_x<10000 && lcd_mode == IMG_MODE)
	{
		stree_y[count_x]=average_offset[0];
		offset_y[count_x]=average_offset[1];
		count_x++;
	}
	

	if(out_xy<1)
	{
		i=count_x-1;
		printf("offset\n");
		while(i>=0)
		{
			printf(" %d",offset_y[count_x-i-1]);	//输出道路的偏差度
			if(i>0) printf(",");
			i--;
		}
		printf("\n");
		out_xy=2;
	}
	else if(out_xy>3)
	{
		i=count_x-1;
		printf("stree\n");
		while(i>=0)
		{
			printf(" %d",stree_y[count_x-i-1]);	//输出舵机的偏转角
			if(i>0) printf(",");
			i--;
		}
		printf("\n");
		out_xy=2;
	}
}


/***
 * @brief	  初始化系统的函数，为了使main函数整洁
 * 
 ***/
void init_Sys()
{

	NVIC_SetPriorityGrouping(3);	 //设置优先级分组,4bit 抢占优先级,没有亚优先级
	NVIC_SetPriority(PORTA_IRQn, 0); //配置优先级 摄像头
	NVIC_SetPriority(DMA0_IRQn, 1);  //配置优先级 摄像头
	NVIC_SetPriority(PIT0_IRQn, 2);  //配置优先级 基本硬件和基本控制
	NVIC_SetPriority(PORTD_IRQn, 2); //配置优先级 摄像头	

	LCD_init();
	camera_init(imgbuff);
	UI_INIT();
	led_init(LED0);
	Quad_Init(); //编码器中断
	flash_init();
	flash_Out();	  //读取数据	
	ftm_pwm_init(FTM2, FTM_CH0, 300, 0);
	ftm_pwm_init(FTM2, FTM_CH1, 300, 0);
	ftm_pwm_init(FTM0, FTM_CH6, 300, 43);

	set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler);   //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(PORTD_VECTORn, PORTD_IRQHandler); //ui所需中断的初始化
        
	Co_Steer[0].P = 2;
	Co_Steer[0].D = 5;
	Co_Steer[2].P = Co_Steer[3].P = 7;
	Co_Steer[2].D = Co_Steer[3].D = 5;

	middleline[59] = middleline[58] = middleline[57] = 40;
	
	camera_get_img(); //相机获取第一帧图像
	enable_irq(PORTD_IRQn);
}




/***
 * @brief		显示模式下处理图像和搜线
 * 
 ***/
void DcdMode()
{
	// img_extract(img, imgbuff, CAMERA_SIZE); //解压图像
	//temp_s[6] = Find_slope();
	
	//yl_Search_line();
	Search_line(); //找线
	// img_compress(img, imgbuff, CAMERA_SIZE);		//图像压缩
	// LCD_Img_Binary_Z(site, size, imgbuff, imgsize); //lcd显示图像
	if (is_show_line != 4)
	{
		Negation();		
		LCD_Img_Binary_G(site, size, img);		
	}


	/*------------在图像下面显示数据-----------*/
	if (!is_show_va)
	{
		// if(state_line[0] == 0)Judge_circul();
		LCD_numf(tem_site_str[4], (double)average_offset[0], GREEN, BLUE);
		// if(state_line[0] == 3) Goin_circul();
		LCD_numf(tem_site_str[5], (double)ac_quad, GREEN, BLUE);
		//LCD_numf(tem_site_data[4], (float)average_offset[1], GREEN, BLUE);
		LCD_numf(tem_site_data[5], (float)cor_sp, GREEN, BLUE);
		// LCD_numf(tem_site_str[5], (double)Ma_Mark, GREEN, BLUE);
		// LCD_numf(tem_site_data[5], (float)cor_sp, GREEN, BLUE);		
		LCD_numf(tem_site_data[4], (float)Ma_Offset, GREEN, BLUE);

	}

	/*----------彩色显示边线,还有显示网格-----------*/
	if (is_show_line == 1 || is_show_line == 3) //网格
	{
		LCD_grid();
	}
	if (is_show_line == 2 || is_show_line == 3) //边线
	{
		int i;
		for (i = vaild_mark; i < 60; i++)
		{
			line_site.x = left_virtual[i];
			line_site.y = i;
			if (left_virtual[i] == -3)
				break;
			if (left_virtual[i] >= 0 && left_virtual[i] <= CAMERA_W - 1)
				LCD_point(line_site, RED);
		}
		for (i = vaild_mark; i < 60; i++)
		{
			line_site.x = right_virtual[i];
			line_site.y = i;
			if (right_virtual[i] == -3)
				break;
			if (right_virtual[i] >= 0 && right_virtual[i] <= CAMERA_W - 1)
				LCD_point(line_site, BLUE);
		}
	}

	//Control_core(); //控制舵机
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
			ClearPid();
			Con_Motor(0);
		}
		csteer = NULL;
		ftm_pwm_duty(FTM0, FTM_CH6, 430); //舵机回中
	}
	else if (lcd_mode == PICTURE_MODE)
	{
		cmotor = NULL;
		ClearPid();
		Con_Motor(0);
		csteer = NULL;
		ftm_pwm_duty(FTM0, FTM_CH6, 430);
	}
	else if(lcd_mode == IMG_MODE)
	{
		if (total_distance < 1000 && abs(motor_speed) > 7)
		{
			cmotor = &motor_pid; //电机
			//Con_Motor(100);
		}
		else
		{
			cmotor = NULL;
			ClearPid();
			Con_Motor(0);
		}
		//ftm_pwm_duty(FTM0, FTM_CH6, 380 + (int)steer_engine_degree); //舵机
		csteer = &steer_pid;
	}
	else if(lcd_mode == STOP_MODE)
	{
		cmotor = NULL;
		csteer = NULL;
		ftm_pwm_duty(FTM2, FTM_CH0, 0);
		ftm_pwm_duty(FTM2, FTM_CH1, 0);
		ftm_pwm_duty(FTM0, FTM_CH6, 430);
		Ma_Mark = 1;
		middleline[59] = middleline[58] = middleline[57] = 40;
		if(stop_save_motor < 7) stop_save_motor = motor_speed;
		motor_speed = 0;
	}
}

void ClearPid()
{
	Increase = 0;	
	LastError = 0;
	iError = 0;
	PrevError = 0;
}











