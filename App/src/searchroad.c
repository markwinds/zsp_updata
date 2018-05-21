/****************************************************************
 *	
 *				      赛道为1，黑线为0                                 
 *                            更改记录
 *<2018.4.30><yl>:
 *	把左右线数组和中线数组设置全局变量，避免每次函数重新开辟空间以节省时间，同时更改了函数以适配
 *<2018.5.19><yl>:
 *	加入了弯道因为圆环跑偏的补线。
 * 
****************************************************************/
#include "searchroad.h"
#include "common.h"
#include "include.h"

float average_offset[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //偏差度，为平均偏差度
uint8 imgbuff[CAMERA_SIZE];									  //定义存储接收图像的数组
uint8 img[CAMERA_H][CAMERA_W];
int8 left_black[CAMERA_H];		 //左边黑线数组
int8 right_black[CAMERA_H];		 //右边黑线数组
int8 middleline[CAMERA_H] = {0}; //存储中线位置的数组

int iscross_flag = 0;  //判断是否是十字道的标志 0表示没有进入1表示初步判断并开始计数距离
int iscross_count = 0; //十字道距离计数

int isisland_flag = 0; //是否是环岛的判断 0表示没有进入1表示初步判断并开始计数距离
int isisland_flag1 = 0;
int isisland_count = 0; //环岛距离计数

float temp_s[10] = {0};
int8 vaild_mark = 0;
int8 add_mark;

void Search_line_init()
{
	//send_init();
	//motorcontrol_int();
}
void Search_line()
{

	int left_black_before = CAMERA_W / 2; //上一次左边扫描到的黑线位置
	int left_next_line = 0;				  //判断扫描是否进入了新的一行
	int left_find_flag = 0;				  //左边是否找到黑线标志
	int jw_left;						  //向左搜索的当前列

	int right_black_before = CAMERA_W / 2;
	int right_next_line = 0;
	int right_find_flag = 0;
	int jw_right;

	int jh;			  //行参数
	float offset = 0; //偏差度，为整体偏差度
					  //int slope[CAMERA_H] = { 0 };                       //存放每行间黑线斜度的数组
	int count = 0;
	int i = 0;
	int j = 0;
	int m = 0;

	int cross_temp[2] = {-1, -1};
	vaild_mark = 0;

	left_black_before = CAMERA_W / 2;
	right_black_before = CAMERA_W / 2;
	jh = LINE_NUM - 1;

	/*-----------------------扫描像素点获取图像信息------------------------*/
	/*
		1.里面有直道进入圆环的判断
		2.主要更新的数据是left_black和right_black数组，其中-1表示此边未找到黑线，-2表示此行为黑线停止扫描（未必是全黑，只是近似黑线）
	*/
	while (jh >= 0) //行条件满足进入找线，将每一有图像的行都扫描一遍,LINE_NUM表示要扫描行数
	{
		/*-----------------左侧扫描--------------------*/
		if (0 == left_next_line) //left_black_next用来标志是否进入新的一行，若为0则是进入新的一行
		{
			jw_left = left_black_before; //若是新的一行开始则从上次找到黑线的位置开始找黑线
			left_next_line = 1;			 //标志参数归位
		}
		//左侧正常扫描

		if (jw_left > 0 && (0 == left_find_flag)) //如果扫描还没有到边缘且之前的扫描未找到黑点
		{
			if ((img[jh][jw_left]) < 1) //==0,扫描到黑点
			{
				if (left_black_before == jw_left && jh < 30) //如果第一次扫描就扫到黑点且当前行在屏幕的上半部分
				{
					while (0 == img[jh][jw_left] && jw_left < CAMERA_W - 1) //向右找赛道 此处不能<=因为这样的话jw_left++后就会使后面调用jw_left时数组越界
					{
						jw_left++;
					}
					if (CAMERA_W - 1 == jw_left) //如果整行是黑的就停止扫描
					{
						left_black[jh] = -2; //标记之后的行都为无效行
						right_black[jh] = -2;
						break;
					}
					else
					{
						left_find_flag = 1;						  //扫描到黑线，标志置1
						left_black[jh] = jw_left - 1;			  //黑线中心为jw_left
						left_black_before = jw_left + OFF_CENTER; //下一次扫描从上一次黑线的中心偏离5个像素的地方开始扫描
						if (left_black_before > CAMERA_W - 1)
							left_black_before = CAMERA_W - 1;
					}
				}
				else
				{
					left_find_flag = 1;						  //扫描到黑线，标志置1
					left_black[jh] = jw_left;				  //黑线中心为jw_left
					left_black_before = jw_left + OFF_CENTER; //下一次扫描从上一次黑线的中心偏离5个像素的地方开始扫描
					if (left_black_before > CAMERA_W - 1)
						left_black_before = CAMERA_W - 1;
				}
			}
			jw_left--; //若没有扫描到黑线就移动一个像素继续扫描
		}
		if ((0 == jw_left) && (0 == left_find_flag)) //左边找到最后一个像素任然未找到符合黑线条件的像素，则认为第一个为黑线
		{
			/*
			if (jh < 50 && left_black[jh + 1] != -1 )  //补扫，判断是否是圆环 如果前一行有线
			{
				if (left_black[jh + 1] - 2 >= 0 && left_black[jh + 1] + 3 <= CAMERA_W - 1)  //防止下面的img[jh + 1][left_black[jh + 1] - 2]数组越界
				{
					if (1 == img[jh + 1][left_black[jh + 1] - 2] && 1 == img[jh + 1][left_black[jh + 1] + 3])  //如果下面一行的突出尖符合条件
					{
						for (i = 2; i < 3; i++)  //向下扫到第5行看是否符合条件
						{
							j = left_black[jh + i] - 1; //用j记录下面第i行的边线列序号
							while (j > 0 && img[jh + i][j] != 1) //向左找0到1的跳变，赛道是1黑线是0
							{
								j--;
							}
							if (j <= 0) break; //只要下面的i行有一行不符合要求就直接跳出，这样的话i就不可能等于5，就不会增加标记
							if (right_black[jh + i] > CAMERA_W - 1 - 12 || -1 == right_black[jh + i] ||
								right_black[jh + i] - left_black[jh + i] < 20 || 
								right_black[jh + i] - left_black[jh + i] > 65) break;//右边不是直道就退出
						}
						if (3 == i)
						{
							judge_island();
						}
					}
				}
				else
				{
					for (i = 2; i < 3; i++)
					{
						j = left_black[jh + i] - 1;
						while (j > 0 && img[jh + i][j] != 1)
						{
							j--;
						}
						if (j <= 0) break;
						if (right_black[jh + i] > CAMERA_W - 1 - 12 || -1 == right_black[jh + i] ||
							right_black[jh + i] - left_black[jh + i] < 20 ||
							right_black[jh + i] - left_black[jh + i] > 65) break;//右边不是直道就退出
					}
					if (3 == i)
					{
						judge_island();
					}
				}
			}
			*/
			left_find_flag = 1;
			left_black[jh] = -1; //-1代表扫描到边线时还没找到黑线
			left_black_before = jw_left + OFF_CENTER;
			if (left_black_before > CAMERA_W - 1)
				left_black_before = CAMERA_W - 1;
		}

		//左侧丢线扫描

		/*-----------------右侧扫描--------------------*/
		if (0 == right_next_line)
		{
			jw_right = right_black_before;
			right_next_line = 1;
		}
		//右侧正常扫描
		if ((jw_right < (CAMERA_W - 1)) && (0 == right_find_flag)) //因为第0位也存有像素点，所以存有像素点的最后一位是CAMERA_W-1
		{
			if ((img[jh][jw_right]) < 1)
			{
				if (right_black_before == jw_right && jh < 30)
				{
					while (jw_right > 0 && 0 == img[jh][jw_right])
					{
						jw_right--;
					}
					if (0 == jw_right)
					{
						left_black[jh] = -2;
						right_black[jh] = -2;
						break;
					}
					else
					{
						right_find_flag = 1;
						right_black[jh] = jw_right + 1;
						right_black_before = jw_right - OFF_CENTER;
						if (right_black_before < 0)
							right_black_before = 0;
					}
				}
				else
				{
					right_find_flag = 1;
					right_black[jh] = jw_right;
					right_black_before = jw_right - OFF_CENTER;
					if (right_black_before < 0)
						right_black_before = 0;
				}
			}
			jw_right++;
		}
		//右侧丢线扫描
		if (jw_right == (CAMERA_W - 1) && (0 == right_find_flag)) //右边黑点未找到
		{
			/*
			if (jh < 50 && right_black[jh + 1] != -1)                        //补扫，判断是否是圆环
			{
				if (right_black[jh + 1] + 2 <= CAMERA_W - 1 && right_black[jh + 1] - 3 > 0)
				{
					if (1 == img[jh + 1][right_black[jh + 1] + 2] && 1 == img[jh + 1][right_black[jh + 1] - 3])
					{
						for (i = 2; i < 3; i++)
						{
							j = right_black[jh + i] + 1;
							while (j > 0 && j < CAMERA_W - 1 && img[jh + i][j] != 1)  //这里要比左边多出一个判断，因为当right_black[jh + i]值为-1时
							{
								j++;
							}
							if (j >= (CAMERA_W - 1) || j <= 0) break;
							if (left_black[jh + i] < 0 + 12 ||
								right_black[jh + i] - left_black[jh + i] < 20 ||
								right_black[jh + i] - left_black[jh + i] > 65) break;
						}
						if (3 == i)
						{
							judge_island();
						}
					}	
				}
				else
				{
					for (i = 2; i < 3; i++)
					{
						j = right_black[jh + i] + 1;
						while (j > 0 && j < CAMERA_W - 1 && img[jh + i][j] != 1)  //这里要比左边多出一个判断，因为当right_black[jh + i]值为-1时
						{
							j++;
						}
						if (j >= (CAMERA_W - 1) || j <= 0) break;
						if (left_black[jh + i] < 0 + 12 ||
							right_black[jh + i] - left_black[jh + i] < 20 ||
							right_black[jh + i] - left_black[jh + i] > 65) break;
					}
					if (3 == i)
					{
						judge_island();
					}
				}
			}
			*/
			right_find_flag = 1;
			right_black[jh] = -1;
			right_black_before = jw_right - OFF_CENTER;
			if (right_black_before < 0)
				right_black_before = 0;
		}

		if ((1 == left_find_flag) && (1 == right_find_flag)) //参数回归
		{
			if(0==img[jh][(real_L(left_black[jh])+real_R(right_black[jh]))>>1])	//中点为黑线
			{
				left_black[jh]=-2;
				right_black[jh]=-2;
				// if(jh > 50)
				// {
				// 	lcd_mode = STOP_MODE;
				// }
				vaild_mark = jh;
				break;
			}
			// else if (add_mark == -1 && jh < 50 && jh > 20 && left_black[jh] - left_black[jh + 1] < 0 &&
			//  left_black[jh + 2] - left_black[jh + 3] > 0)
			// {
			// 	add_mark = 1;
			// }
			// else if(add_mark == 1 && (right_black[jh]<0?CAMERA_W:right_black[jh]) - left_black[jh] > jh + 15)
			// {
			// 	left_black[jh] = right_black[jh]<0?CAMERA_W:right_black[jh] - jh - 5;
			// 	left_black_before = right_black[jh]<0?CAMERA_W:right_black[jh] - 10;
			// }
			left_next_line = 0;
			right_next_line = 0;
			left_find_flag = 0;
			right_find_flag = 0;
			jh--;
		}
	} //while

	/*----------------根据图像判断路况--------------------*/
	/*
		1.判断弯道圆环 用isisland_flag 第二个位置
		2.判断十字道 第一个位置
	*/
	// jh = LINE_NUM - 6;
	// while (jh >= 5 && left_black[jh] != -2 && right_black[jh] != -2)
	// {
	// 	/*if (left_black[jh + 3] - left_black[jh + 4] > 0 &&           //判断是否是圆环
	// 		left_black[jh + 2] - left_black[jh + 3] > 0 &&
	// 		left_black[jh + 1] - left_black[jh + 2] > 0 &&
	// 		left_black[jh + 0] - left_black[jh + 1] > 0 &&
	// 		left_black[jh - 1] - left_black[jh - 0] < 0 &&
	// 		left_black[jh - 2] - left_black[jh - 1] < 0 &&
	// 		left_black[jh - 3] - left_black[jh - 2] < 0 &&
	// 		left_black[jh - 4] - left_black[jh - 3] < 0
	// 		)
	// 	{
			
	// 		if (left_black[jh + 0] - left_black[jh + 4] > 6 && left_black[jh + 0] - left_black[jh - 4] > 6)//突出点够尖锐
	// 		{
	// 			isisland_flag++;
	// 			//temp_s[0] = left_black[jh];
	// 			//temp_s[1] = left_black[jh + 4];
	// 			//temp_s[2] = left_black[jh - 4];
	// 			//LCD_numf(tem_site_str[2], jh, GREEN, BLUE);
	// 		}
	// 	}
	// 	*/
	// 	j = 0;
	// 	m = 0;
	// 	if (-1 == left_black[jh] && -1 == right_black[jh] && iscross_flag != 2) //判断是否是十字 iscross_flag为2时就不用识别了
	// 	{
	// 		for (i = 1; i < 20; i++)
	// 		{
	// 			if (jh + i > LINE_NUM - 1)
	// 				break;
	// 			if (left_black[jh + i] != -1)
	// 			{
	// 				if (left_black[jh + i] > 1)
	// 				{
	// 					j = 1;
	// 				}
	// 				break;
	// 			}
	// 		}
	// 		for (i = 1; i < 20; i++)
	// 		{
	// 			if (jh + i > LINE_NUM - 1)
	// 				break;
	// 			if (right_black[jh + i] != -1)
	// 			{
	// 				if (right_black[jh + i] < CAMERA_W - 2)
	// 				{
	// 					m = 1;
	// 				}
	// 				break;
	// 			}
	// 		}
	// 		if (1 == j && 1 == m)
	// 		{
	// 			if (0 == iscross_flag) //如果第一次扫描到符合条件就初判为1
	// 			{
	// 				iscross_flag = 1;
	// 			}
	// 			else if (1 == iscross_flag && cross_distance_count > 4 && cross_distance_count < 8) //如果已经初判成功是十字且在4到8厘米后的复判中成功判断是十字那就认为是十字
	// 			{
	// 				iscross_flag = 2;
	// 			}
	// 		}
	// 	}
	// 	jh--;
	// }

	/*---------------------中心点的计算------------------------*/
	/*
		1.根据左右边界确定中心点的位置
		2.标记无效中心点
	*/
	if (2 == iscross_flag) //在十字道内计算中心的方法
	{
		jh = LINE_NUM - 1;
		if (20 < cross_distance_count && cross_distance_count < 130) //在这个范围内补线
		{
			while (jh >= 0 && left_black[jh] != -2 && right_black[jh] != -2)
			{
				if (jh == (LINE_NUM - 1)) //第一次进入循环
				{
					if (-1 == right_black[jh])
					{
						middleline[jh] = (left_black[jh] + CAMERA_W) / 2;
						cross_temp[1] = LINE_NUM - 1; //起始行全丢线就算找到下界
					}
					else if (-1 == left_black[jh])
					{
						middleline[jh] = (left_black[jh] + right_black[jh]) / 2;
						cross_temp[1] = LINE_NUM - 1; //起始行全丢线就算找到下界
					}
					else //如果起始行不丢线
						middleline[jh] = (left_black[jh] + right_black[jh]) / 2;
				}

				else //除了起始行，其他的行都执行这里面的代码
				{
					/*从最下面开始扫描当出现一边丢线时就一直进入这个if找十字上界 */
					/*进入这个if要满足上界未找到（找到时cross_temp[0]标记为-2）且（第一次开始丢线或者已经有过丢线）*/
					if ((-1 == left_black[jh] || -1 == right_black[jh] || cross_temp[1] != -1) && cross_temp[0] != -2)
					{
						if (-1 == cross_temp[1]) //如果是第一次丢线
						{
							cross_temp[1] = jh + 1; //记录补线下界
													//temp_s[5] = cross_temp[1];
													//temp_s[7] = middleline[cross_temp[1]];
						}
						if (jh <= 0) //未找到上界
						{
							for (i = 0; i < cross_temp[1]; i++)
							{
								middleline[i] = middleline[cross_temp[1]] + (CAMERA_W / 2 - middleline[cross_temp[1]]) * (cross_temp[1] - i) / (cross_temp[1]);
								//middleline[i] = (CAMERA_W / 2 + middleline[cross_temp[1]]) / 2;
							}
						}
						else if (left_black[jh] != -1 && right_black[jh] != -1) //两边都不丢线即找到补线上界
						{
							cross_temp[0] = jh; //记录补线上界
							//temp_s[4] = cross_temp[0];

							middleline[jh] = (left_black[jh] + right_black[jh]) / 2;
							middleline[jh] += ((CAMERA_W / 2 - middleline[jh]) / 2);
							//temp_s[6] = middleline[cross_temp[0]];
							for (i = cross_temp[0] + 1; i < cross_temp[1]; i++)
							{
								middleline[i] = middleline[cross_temp[1]] + (middleline[cross_temp[0]] - middleline[cross_temp[1]]) * (i - cross_temp[1]) / (cross_temp[1] - cross_temp[0]); //下界列+上下界列差*当前行差/上下界行差
																																															 //middleline[i] = (middleline[cross_temp[0]] + middleline[cross_temp[1]]) / 2;
																																															 //temp_s[9] = 10;
							}
							cross_temp[0] = -2; //不在进入这个if
						}
					}
					else //还未丢线时直接计算中点
					{
						if (-1 == right_black[jh])
						{
							middleline[jh] = (left_black[jh] + CAMERA_W) / 2;
						}
						else
						{
							middleline[jh] = (left_black[jh] + right_black[jh]) / 2;
						}
					}
				}
				jh--;
			}
			if (left_black[jh] == -2 || right_black[jh] != -2)
			{
				middleline[jh] = -2;
			}
		}
		else
		{
			nomal_middle(left_black, right_black, middleline);
		}

		if (326 < cross_distance_count && cross_distance_count < 430) //如果到了准备出十字
		{
			jh = LINE_NUM - 1;
			while (jh >= 0)
			{
				middleline[jh] = middleline[jh] + 0.5 * (middleline[jh] - CAMERA_W / 2);
				if (middleline[jh] > CAMERA_W - 1 - 6 || middleline[jh] < 0 + 6)
				{
					middleline[jh] = -2;
					break;
				}
				jh--;
			}
		}
	}
	else
	{
		//如果没有找到圆，看看有没有圆;如果在里面了，看看出去了没有
		// if ((!is_rightcircul_flag && !is_leftcircul_flag) || is_rightcircul_flag == 2 || is_leftcircul_flag == 2)
		// 	Judge_circul(); //找到圆了，准备进去了
		// if (is_rightcircul_flag == 1)
		// 	Goin_rightcircul();
		// if (is_leftcircul_flag == 1)
		// 	Goin_leftcircul();
		//普通中线获得的适配版
		Get_middle_line();
		//nomal_middle();
	}

	/*------------------------偏差度计算及中线展现-----------------------------*/
	/*
		1.用中心点算出偏差度
	*/
	// for (i = LINE_NUM - 1; i >= 1; i--)
	// {
	// 	if (-2 == middleline[i])
	// 		break;
	// 	else if (-1 == middleline[i]) {}
	// 	else
	// 	{
	// 		offset = offset + ((float)(middleline[i] - CAMERA_W / 2)*(1 + (60 - i)*TRAPEZOID_CORRECT / 40));          //offset是补偿，用来描述整体赛道的偏向,<0偏左
	// 		count++;
	// 		if (middleline[i] > CAMERA_W - 1)middleline[i] = CAMERA_W - 1;
	// 		if (middleline[i] < 0)middleline[i] = 0;
	// 		img[i][middleline[i]] = !img[i][middleline[i]];
	// 	}
	// }
	//为了补线重新加了函数
	Get_error_cal(&offset, &count);

	/*---------------更新偏差度队列---------------------*/
	for (i = 10; i > 1; i--)
	{
		average_offset[i] = average_offset[i - 1];
	}
	average_offset[1] = ((float)offset / (float)(count + 1));
	average_offset[1] -= CAMERA_HARDWARE_ERROR;
	cor_sp = (double)average_offset[1] / 10;
	cor_sp *= cor_sp;
}

float Find_slope() //找到梯形的斜率
{
	int j = 79; //当前列
	int h[3] = {2, 3, 4};

	while (0 == img[50][j] && j >= 0)
	{
		j--;
	}
	h[1] = j;

	j = 79;
	while (0 == img[10][j])
	{
		j--;
	}
	h[2] = j;

	LCD_num(tem_site_str[3], h[1], GREEN, BLUE);
	LCD_num(tem_site_str[4], h[2], GREEN, BLUE);
	//LCD_numf(tem_site_data[4], h[1]-h[2], GREEN, BLUE);

	return (float)(abs(h[1] - h[2])) / 40.0;
}

//中线翻转显示函数
void Negation()
{
	int i = 0;
	int j = 0;

	for (i = 0; i < CAMERA_H; i++)
	{
		for (j = 0; j < CAMERA_W; j++)
		{
			img[i][j] = !img[i][j];
		}
	}
}

void nomal_middle()
{
	int jh;

	jh = LINE_NUM - 1;
	while (jh >= 0 && left_black[jh] != -2 && right_black[jh] != -2)
	{
		if (jh == (LINE_NUM - 1))
		{
			if (-1 == right_black[jh])
				middleline[jh] = (left_black[jh] + CAMERA_W) / 2;
			else
				middleline[jh] = (left_black[jh] + right_black[jh]) / 2;
		}
		else
		{
			if (-1 == left_black[jh] || -1 == right_black[jh])
			{
				if (-1 == left_black[jh] && -1 == right_black[jh])
					middleline[jh] = CAMERA_W / 2;
				else
				{
					if (-1 == right_black[jh])
						middleline[jh] = left_black[jh] + ROAD_WIDE / (1 + (60 - jh) * TRAPEZOID_CORRECT / 40);
					else
						middleline[jh] = right_black[jh] - ROAD_WIDE / (1 + (60 - jh) * TRAPEZOID_CORRECT / 40);
				}
			}
			else
			{
				middleline[jh] = (left_black[jh] + right_black[jh]) / 2;
			}
		}
		jh--;
	}
	if (left_black[jh] == -2 || right_black[jh] == -2)
	{
		middleline[jh] = -2;
	}
}

void judge_island()
{
	if (0 == isisland_flag1) //如果第一次扫描到符合条件就初判为1
	{
		isisland_flag1 = 1;
	}
	else if (1 == isisland_flag1 && land_distance_count1 > 10 && land_distance_count1 < 15) //如果已经初判成功是十字且在4到8厘米后的复判中成功判断是十字那就认为是十字
	{
		isisland_flag1 = 2;
	}
	if (iscross_flag != 0)
	{
		isisland_flag1 = 0;
	}
}

int8 real_L(int8 left)
{
	if(-1==left)
	{
		return 0; 
	}
	else return left;
}

int8 real_R(int8 right)
{
	if(-1==right)
	{
		return LINE_NUM - 1; 
	}
	else return right;
}
