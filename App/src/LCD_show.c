/******************************************
 *                
 *                   更改日志
 * <2018.5.1><yl>:
 * 	加入了goto_End优化了操作，更改了消抖，且效果良好。
 *
 *******************************************/
#include "include.h"
#include <string.h>

uint8 lcd_mode = IMG_MODE;
uint8 key_on = 0;
uint8 is_show_va = 0;
uint8 is_show_line = 0;
uint8 please_clear = 0;
float motor_go = 0;		//在显示状态下控制电机是否转动的变量
int colour[MAX_OPTION]; //0元素也保存有有效数据
Site_t tem_site_str[] = {0, 0, 0, 20, 0, 40, 0, 60, 0, 80, 0, 100};
Site_t tem_site_data[] = {60, 0, 60, 20, 60, 40, 60, 60, 60, 80, 60, 100};

int page = 1;		 //lcd当前所在页
int current_row = 0; //当前所在行
float flash_in = 0;  //是否写入flash

/*----------各种状态下对应的5个建的操作--------*/
Lcd_State wait_middle =
	{
		quit_Lcd,	  //中 退出lcd,显示图像
		goto_End,	  //上
		goto_Begin,	//下
		ignore_Oprate, //左
		ignore_Oprate  //右
};
Lcd_State wait_begin =
	{
		goto_Set,  //中
		goto_Wait, //上
		goto_next, //下
		data_Down, //左
		data_Up	//右
};
Lcd_State wait_end =
	{
		goto_Set,	//中
		goto_Before, //上
		goto_Wait,   //下
		data_Down,   //左
		data_Up		 //右
};
Lcd_State normal_page =
	{
		goto_Set,	//中
		goto_Before, //上
		goto_next,   //下
		data_Down,   //左
		data_Up		 //右
};
Lcd_State imgbuff_show =
	{
		quit_show,  //中
		open_va,	//上
		go_Picture, //下
		show_line,  //左
		ushow_line  //右
};
Lcd_State read_picture =
	{
		read_Picture, //中
		go_Back,	  //上
		read_Array,   //下
		read_Before,  //左
		read_After	//右
};

Lcd_State *p_current_state = &imgbuff_show;

void PORTD_IRQHandler()
{
	uint32 flag;

	while (!PORTD_ISFR)
		;
	flag = PORTD_ISFR;
	PORTD_ISFR = ~0; //清中断标志位

	DELAY_MS(20);

	if (gpio_get(KEY_PTxn[5]) == KEY_DOWN && flag & (1 << 13)) //中键按下
	{
		onpress_M();
	}
	else if (gpio_get(KEY_PTxn[0]) == KEY_DOWN && flag & (1 << 10))
	{
		onpress_U();
	}
	else if (gpio_get(KEY_PTxn[1]) == KEY_DOWN && flag & (1 << 14))
	{
		onpress_D();
	}
	else if (gpio_get(KEY_PTxn[2]) == KEY_DOWN && flag & (1 << 11))
	{
		onpress_L();
	}
	else if (gpio_get(KEY_PTxn[3]) == KEY_DOWN && flag & (1 << 12))
	{
		onpress_R();
	}
	else if ((gpio_get(KEY_PTxn[4]) == KEY_DOWN && flag & (1 << 7)) && IMG_MODE == lcd_mode) //如果是flash按键按下,且是在图像模式下
	{
		save_picture = 1; //如果flash写入图像信息的键按下，标志位置1
	}
	key_on = 1;				 //记录有按键按下
	disable_irq(PORTD_IRQn); //消抖
}

/*结构体的元素个数存放在colour[MAX_OPTION - 1]中 
消抖时间控制在最后几行*/
void Open_UI()
{
	int i = 0;
	int m = 0;
	int n = 0;

	for (n = 0;; n++)
	{
		if (0 == strcmp(screen_data[n].data_name, "end"))
			break;
		if (n > 200)
			break;
	}
	colour[MAX_OPTION - 1] = 300 * n; //记录要处理的数据个数
	if (1 == key_on)				  //有按键按下才刷新
	{
		m = 6 * (page - 1);
		LCD_clear(WHITE); //清屏防止上次留下残影
		for (i = 0; i < 6; i++)
		{
			if (m + i >= n)
			{
				break;
			}
			if (99 == screen_data[m + i].icrement)
			{
				LCD_str(tem_site_str[i], screen_data[m + i].data_name, BLACK, colour[m + i]); //记得回来改颜色
				LCD_str(tem_site_data[i], (((int)(*(screen_data[m + i].data_value))) % 2) ? "ON" : "OFF", BLACK, WHITE);
			}
			else
			{
				LCD_str(tem_site_str[i], screen_data[m + i].data_name, BLACK, colour[m + i]); //记得回来改颜色
				LCD_numf(tem_site_data[i], (float)(*(screen_data[m + i].data_value)), BLACK, WHITE);
			}
		}
		key_on = 0;
		DELAY_MS(200); //消抖
		enable_irq(PORTD_IRQn);
	}
}

void UI_INIT()
{
	int i = 0;

	for (i = 0; i < MAX_OPTION; i++)
	{
		colour[i] = WHITE;
	}
	/*配置各个按键的中断*/
	port_init(PTD14, ALT1 | IRQ_FALLING | PULLUP); //下
	port_init(PTD13, ALT1 | IRQ_FALLING | PULLUP); //中
	port_init(PTD12, ALT1 | IRQ_FALLING | PULLUP); //右
	port_init(PTD11, ALT1 | IRQ_FALLING | PULLUP); //左
	port_init(PTD10, ALT1 | IRQ_FALLING | PULLUP); //上
	port_init(PTD7, ALT1 | IRQ_FALLING | PULLUP);  //flash按键
	enable_irq(PORTD_IRQn);						   //使能d对应的端口也就是按键的port
}

/*-----------------新增功能的函数-----------------*/
Lcd_State *quit_Lcd(Lcd_State *pThis) //退出lcd模式
{
	page = 1;
	current_row = 0;
	lcd_mode = IMG_MODE;
	return &imgbuff_show;
}

Lcd_State *goto_Begin(Lcd_State *pThis) //从等待模式进入本页第一行
{
	current_row = 1;
	colour[6 * (page - 1) + current_row - 1] = GREEN; //选中的行变成绿色
	if (1 == page)
		return &wait_begin;
	else
		return &normal_page;
}

//new
Lcd_State *goto_End(Lcd_State *pThis) //从等待模式进入本页第一行
{
	if (1 == page)
	{
		current_row = 6;
	}
	else
	{
		current_row = 1;
	}
	colour[6 * (page - 1) + current_row - 1] = GREEN; //选中的行变成绿色
	return &normal_page;
}

Lcd_State *ignore_Oprate(Lcd_State *pThis)
{
	return pThis;
}

Lcd_State *goto_Set(Lcd_State *pThis)
{
	if (GREEN == colour[6 * (page - 1) + current_row - 1])
		colour[6 * (page - 1) + current_row - 1] = RED;
	else
		colour[6 * (page - 1) + current_row - 1] = GREEN;
	return pThis;
}

Lcd_State *goto_Wait(Lcd_State *pThis)
{
	if (GREEN == colour[6 * (page - 1) + current_row - 1]) //在未选中的情况下才运行
	{
		colour[6 * (page - 1) + current_row - 1] = WHITE;
		return &wait_middle;
	}
	else
		return pThis;
}

Lcd_State *goto_next(Lcd_State *pThis)
{
	int i = 0;

	if (GREEN == colour[6 * (page - 1) + current_row - 1])
	{
		colour[6 * (page - 1) + current_row - 1] = WHITE;
		if (1 == colour[MAX_OPTION - 1] / 300 || 6 * (page - 1) + current_row - 1 == ((colour[MAX_OPTION - 1] / 300) - 1)) //如果只有一个数据要处理或者当前页面只有一行
		{
			return &wait_middle;
		}
		if (6 * (page - 1) + current_row - 1 == ((colour[MAX_OPTION - 1] / 300) - 2)) //如果再往下一行就是最后一行
		{
			i = 1;
		}
		if (6 == current_row)
		{
			page++;
			current_row = 1;
		}
		else
			current_row++;
		colour[6 * (page - 1) + current_row - 1] = GREEN;
		if (1 == i)
			return &wait_end;
		else
			return &normal_page;
	}
	else
		return pThis;
}

Lcd_State *goto_Before(Lcd_State *pThis)
{
	int i = 0;

	if (GREEN == colour[6 * (page - 1) + current_row - 1])
	{
		colour[6 * (page - 1) + current_row - 1] = WHITE;
		if (1 == colour[MAX_OPTION - 1] / 300 || 6 * (page - 1) + current_row - 1 == 0)
		{
			return &wait_middle;
		}
		if (6 * (page - 1) + current_row - 1 == 1)
		{
			i = 1;
		}
		if (1 == current_row)
		{
			page--;
			current_row = 6;
		}
		else
			current_row--;
		colour[6 * (page - 1) + current_row - 1] = GREEN;
		if (1 == i)
			return &wait_begin;
		else
			return &normal_page;
	}
	else
		return pThis;
}

Lcd_State *data_Down(Lcd_State *pThis)
{
	if (RED == colour[6 * (page - 1) + current_row - 1])
	{
		*(screen_data[6 * (page - 1) + current_row - 1].data_value) -= screen_data[6 * (page - 1) + current_row - 1].icrement;
		if (screen_data[6 * (page - 1) + current_row - 1].ip == -1) //写flash操作
		{
			flash_In();
		}
	}
	else
	{
		colour[6 * (page - 1) + current_row - 1] = WHITE;
		current_row = 0;
		return &wait_middle;
	}
	return pThis;
}

Lcd_State *data_Up(Lcd_State *pThis)
{
	if (RED == colour[6 * (page - 1) + current_row - 1])
	{
		*(screen_data[6 * (page - 1) + current_row - 1].data_value) += screen_data[6 * (page - 1) + current_row - 1].icrement;
		if (screen_data[6 * (page - 1) + current_row - 1].ip == -1) //写flash操作
		{
			flash_In();
		}
	}
	else
	{
		colour[6 * (page - 1) + current_row - 1] = WHITE;
		current_row = 0;
		return &wait_middle;
	}
	return pThis;
}

Lcd_State *quit_show(Lcd_State *pThis)
{
	please_clear = 1;
	page = 1;
	current_row = 0;
	lcd_mode = UI_MODE;
	return &wait_middle;
}

Lcd_State *open_va(Lcd_State *pThis)
{
	is_show_va = !is_show_va;
	if (!is_show_va)
		LCD_clear(WHITE);
	return pThis;
}
Lcd_State *show_line(Lcd_State *pThis)
{
	is_show_line++;
	if (is_show_line > 3)
	{
		is_show_line = 0;
	}
	return pThis;
}
Lcd_State *ushow_line(Lcd_State *pThis)
{
	is_show_line = 0;
	return pThis;
}
Lcd_State *do_nothing(Lcd_State *pThis)
{
	return pThis;
}

Lcd_State *go_Picture(Lcd_State *pThis)
{
	lcd_mode = PICTURE_MODE;
	flash_picture[1] = 0;
	flash_picture[0] = SECTOR_NUM - 1;
	LCD_clear(WHITE);
	return &read_picture;
}

Lcd_State *read_Picture(Lcd_State *pThis)
{
	if(10==picture_choose_before) picture_choose=2;
	else picture_choose = 10;
	return pThis;
}

Lcd_State *go_Back(Lcd_State *pThis)
{
	lcd_mode = IMG_MODE;
	picture_choose = 0;
	picture_count = 1;
	flash_picture[1] = 0;
	flash_picture[0] = SECTOR_NUM - 1;
	return &imgbuff_show;
}

Lcd_State *read_Array(Lcd_State *pThis) //读数组
{
	picture_choose = 4;
	return pThis;
}

Lcd_State *read_Before(Lcd_State *pThis) //前一幅图片
{
	picture_choose = 1;
	return pThis;
}

Lcd_State *read_After(Lcd_State *pThis)
{
	picture_choose = 3;
	return pThis;
}

/*中断调用的函数*/
void onpress_M()
{
	p_current_state = p_current_state->press_M(p_current_state);
}

void onpress_U()
{
	p_current_state = p_current_state->press_U(p_current_state);
}

void onpress_D()
{
	p_current_state = p_current_state->press_D(p_current_state);
}

void onpress_L()
{
	p_current_state = p_current_state->press_L(p_current_state);
}

void onpress_R()
{
	p_current_state = p_current_state->press_R(p_current_state);
}

/*flash操作函数*/
void flash_In() //将数据写入flash
{
	int i = 0;

	flash_erase_sector(SECTOR_NUM); //擦除扇区,擦一次只能写一次
	while (strcmp(screen_data[i].data_name, "end") != 0)
	{
		if (screen_data[i].ip > 0)
		{
			flash_write(SECTOR_NUM, screen_data[i].ip * 4, (uint32)((*(screen_data[i].data_value)) * 100.0 + 0.5)); //四舍五入写入，防止float精度不够
		}
		i++;
	}
}

void flash_Out()
{
	int i = 0;
	uint32 data = 0;

	while (strcmp(screen_data[i].data_name, "end") != 0)
	{
		if (screen_data[i].ip > 0)
		{
			data = flash_read(SECTOR_NUM, screen_data[i].ip * 4, uint32);
			*(screen_data[i].data_value) = (float)((double)data / 100.0);
		}
		i++;
	}
}
