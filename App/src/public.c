#include "include.h"

Site_t line_site={0,0};




/*在while循环里面每次都要执行的非核心操作*/
void do_Sys()
{
    if (please_clear)
    {
        LCD_clear(WHITE);
        please_clear = 0;
    }
    enable_irq(PORTD_IRQn);
    if((delete_picture-0.3)>0.5)
    {
        delete_Picture();
        delete_picture=0;
    }

    if (IMG_MODE == lcd_mode)
    {
        if (is_show_va) //是能够在IMG_MODE模式下显示数据
        {
            // if(state_line[0] == 0)Judge_circul();
            LCD_numf(tem_site_str[4], (float)isiscircul_flag, GREEN, BLUE);
            // if(state_line[0] == 3) Goin_circul();
            // LCD_numf(tem_site_str[5], (float)state_line[2], GREEN, BLUE);
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
                if (left_black[i] >= 0)
                    LCD_point(line_site, RED);
            }
            for (i = 0; i < 60; i++)
            {
                line_site.x = right_black[i];
                line_site.y = i;
                if (right_black[i] >= 0)
                    LCD_point(line_site, BLUE);
            }
        }
        Control_core();
    }
    // LCD_numf(tem_site_str[5], motor_speed, GREEN, BLUE);
    // LCD_num(tem_site_str[4], speed_add, GREEN, BLUE);
}


/*初始化系统的函数，为了使main函数整洁*/
void init_Sys()
{
	LCD_init();
	camera_init(imgbuff);
	ftm_pwm_init(FTM2, FTM_CH0, 300, 0);
	ftm_pwm_init(FTM0, FTM_CH6, 300, 43);
	UI_INIT();
	led_init (LED0);

	set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler);   //设置 DMA0 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(PORTD_VECTORn, PORTD_IRQHandler); //ui所需中断的初始化
	Quad_Init();										 //编码器中断
	flash_init();   
	flash_Out();                                         //读取数据

    //init_B(img_bucket);
    init_P();
}



/*!
*  @brief      PORTA中断服务函数，与摄像头有关
*  @since      v5.0
*/
void PORTA_IRQHandler()
{
	uint8 n; //引脚号
	uint32 flag;
	while (!PORTA_ISFR);
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


