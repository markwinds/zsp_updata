#include "include.h"
#include <string.h>

uint8 lcd_mode = IMG_MODE;
uint8 key_on = 0;
int colour[MAX_OPTION]; //0元素也保存有有效数据
Site_t tem_site_str[] = { 0, 0, 0, 20, 0, 40, 0, 60, 0, 80, 0, 100 };
Site_t tem_site_data[] = { 60, 0, 60, 20, 60, 40, 60, 60, 60, 80, 60, 100};

int page = 1;  //lcd当前所在页
int current_row = 0; //当前所在行

/*----------各种状态下对应的5个建的操作--------*/
Lcd_State wait_middle = {
	quit_Lcd,        //中 退出lcd,显示图像
	goto_Begin,      //上
	goto_Begin,      //下
	ignore_Oprate,   //左
	ignore_Oprate    //右
};
Lcd_State wait_begin = {
	goto_Set,        //中 
	goto_Wait,       //上
	goto_next,       //下
	data_Down,       //左
	data_Up          //右
};
Lcd_State wait_end = {
	goto_Set,        //中 退出lcd,显示图像
	goto_Before,     //上
	goto_Wait,       //下
	data_Down,       //左
	data_Up          //右
};
Lcd_State normal_page = {
	goto_Set,        //中 退出lcd,显示图像
	goto_Before,     //上
	goto_next,       //下
	data_Down,       //左
	data_Up          //右
};

Lcd_State *p_current_state = &wait_middle;


void PORTD_IRQHandler()
{
	uint32 flag;

	while (!PORTD_ISFR);
	flag = PORTD_ISFR;
	PORTD_ISFR = ~0;                                   //清中断标志位


	if (IMG_MODE == lcd_mode)
	{
		if (flag & (1 << 13))  lcd_mode = UI_MODE;         //如果中键按下，则进入ui模式 //LCD_clear(WHITE);												  
	}
	else
	{
		if (flag & (1 << 13))   //中键按下
		{
			onpress_M();
		}
		else if (flag & (1 << 10))
		{
			onpress_U();
		}
		else if (flag & (1 << 14))
		{
			onpress_D();
		}
		else if (flag & (1 << 11))
		{
			onpress_L();
		}
		else if (flag & (1 << 12))
		{
			onpress_R();
		}
	}
	key_on = 1; //记录有按键按下
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
		if (0==strcmp(screen_data[n].data_name, "end")) break;
		if (n > 200)break;
	}
	colour[MAX_OPTION - 1] = 300*n; //记录要处理的数据个数
	if (1 == key_on)//有按键按下才刷新
	{
		m = 6 * (page - 1);
		LCD_clear(WHITE);//清屏防止上次留下残影
		for (i = 0; i < 6; i++)
		{
			if (m + i >= n)
			{
				break;
			}
			if (99 == screen_data[m + i].icrement)
			{
				LCD_str(tem_site_str[i], screen_data[m + i].data_name, BLACK, colour[m + i]);  //记得回来改颜色
				LCD_str(tem_site_data[i], (((int)(*(screen_data[m + i].data_value))) % 2 )? "ON" : "OFF", BLACK, WHITE);
			}
			else
			{
				LCD_str(tem_site_str[i], screen_data[m + i].data_name, BLACK, colour[m + i]);//记得回来改颜色
				LCD_numf(tem_site_data[i], (float)(*(screen_data[m + i].data_value)), BLACK, WHITE);
			}
		}
		key_on = 0;
		for (i = 0; i < 1000; i++)
			for (m = 0; m < 5000; m++);//消抖
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
	port_init(PTD14, ALT1 | IRQ_FALLING | PULLUP);  //下
	port_init(PTD13, ALT1 | IRQ_FALLING | PULLUP);  //中
	port_init(PTD12, ALT1 | IRQ_FALLING | PULLUP);      //右
	port_init(PTD11, ALT1 | IRQ_FALLING | PULLUP);      //左
	port_init(PTD10, ALT1 | IRQ_FALLING | PULLUP);      //上
	enable_irq(PORTD_IRQn);                //使能d对应的端口也就是按键的port
}





/*-----------------新增功能的函数-----------------*/
Lcd_State *quit_Lcd(Lcd_State *pThis) //退出lcd模式
{
	page = 1;
	current_row = 0;
	lcd_mode = IMG_MODE;
	return &wait_middle;
}

Lcd_State *goto_Begin(Lcd_State *pThis) //从等待模式进入本页第一行
{
	current_row = 1;
	colour[6 * (page - 1) + current_row - 1] = GREEN; //选中的行变成绿色
	if (1 == page) return &wait_begin;
	else return &normal_page;
	
}

Lcd_State *ignore_Oprate(Lcd_State *pThis)
{
	return pThis;
}

Lcd_State *goto_Set(Lcd_State *pThis)
{
	if (GREEN == colour[6 * (page - 1) + current_row - 1]) colour[6 * (page - 1) + current_row - 1] = RED;
	else colour[6 * (page - 1) + current_row - 1] = GREEN;
	return pThis;
}

Lcd_State *goto_Wait(Lcd_State *pThis)
{
	if (GREEN == colour[6 * (page - 1) + current_row - 1]) //在未选中的情况下才运行
	{
		colour[6 * (page - 1) + current_row - 1] = WHITE;
		return &wait_middle;
	}
	else return pThis;	
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
		else current_row++;
		colour[6 * (page - 1) + current_row - 1] = GREEN;
		if(1==i) return &wait_end;
		else return &normal_page;
	}
	else return pThis;
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
		else current_row--;
		colour[6 * (page - 1) + current_row - 1] = GREEN;
		if (1 == i) return &wait_begin;
		else return &normal_page;
	}
	else return pThis;
}

Lcd_State *data_Down(Lcd_State *pThis)
{
	if (RED == colour[6 * (page - 1) + current_row - 1])
	{
		*(screen_data[6 * (page - 1) + current_row - 1].data_value) -= screen_data[6 * (page - 1) + current_row - 1].icrement;
	}
	return pThis;
}

Lcd_State *data_Up(Lcd_State *pThis)
{
	if (RED == colour[6 * (page - 1) + current_row - 1])
	{
		*(screen_data[6 * (page - 1) + current_row - 1].data_value) += screen_data[6 * (page - 1) + current_row - 1].icrement;
	}
	return pThis;
}


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


