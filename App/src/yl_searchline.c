#include "searchroad.h"
#include "common.h"
#include "include.h"

int8 state_line[5];
int8 isiscircul_flag = 0; // 0表示未检测到，1表示准备进圆，2表示已经进圆
enum road_condition road_mark = straightaway;



//基本上就是队友的代码，用matlab改了单边丟线情况
//<2018.5.5.yl>加入了出圆筛选中线
void Get_middle_line()
{
    int jh;
    int16 tem_val;
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
                //这里加入了在圆环内时，加入了出圆环判断，主要利用屏蔽两边丢线带飞车子
				if (-1 == left_black[jh] && -1 == right_black[jh])
                { 
                    if(isiscircul_flag == 2){
                        middleline[jh] == -2;
                    }
                    else
                    middleline[jh] = CAMERA_W / 2;
                }
				else
				{
                    //这里用的公式是matlab拟合出来的
					if (-1 == right_black[jh])
                    {
                        tem_val = left_black[jh] + (int16)(36.82 + 0.977*abs(left_black[jh] - left_black[LINE_NUM - 1]) - 0.7718*(LINE_NUM - jh));
                        middleline[jh] = ( (tem_val >= CAMERA_W) ? (CAMERA_W - 1) : tem_val);
                    } 
					else
                    {
                        if(right_black[LINE_NUM - 1] < 0)
                        {
                            tem_val = right_black[jh] - (int16)(36.82 + 0.977*abs(right_black[jh] - CAMERA_W + 1) - 0.7718*(LINE_NUM - jh));
                        }
                        else
                        {
                            tem_val = right_black[jh] - (int16)(36.82 + 0.977*abs(right_black[jh] - right_black[LINE_NUM - 1]) - 0.7718*(LINE_NUM - jh));
                        }
                        middleline[jh] = ( (tem_val < 0) ? (0) : tem_val);
                    } 
				}
			}
			else
			{
				middleline[jh] = (left_black[jh] + right_black[jh]) / 2;
			}
		}

        //if(middleline[jh] >=0 && middleline[jh] < LINE_NUM - 1)
        //{
        //    img[i][middleline[i]] = !img[i][middleline[i]];
        //}
        jh--;
	}
    if (left_black[jh] == -2 || right_black[jh] == -2)
    {
        middleline[jh] = -2;
    }
}



//这里是整个圆环判断主体
//<2018.5.4.yl>创建了函数，用于判断圆环
//<2018.5.5.yl>加入了出圆，丢线补线
void Judge_circul()
{
    int8 jh;
    int8 t_jh;
    if(isiscircul_flag == 0)
    {
        //找一找圆环在不在
        if(left_black[25] > 0 && right_black[25] < 0) //从25行开始搜丟线宽度
        {
            for(jh = 25; jh >= 10; jh--) //上面找丢线宽度
            {
                if(right_black[jh] > 0 || left_black[jh] < 0)
                {
                    break;
                }
            }
            for(t_jh = 25; t_jh <= 35; t_jh++) //下面找丢线宽度
            {
                if(right_black[t_jh] > 0 || left_black[t_jh] < 0)
                {
                    break;
                }
            }
            //减一减得到单边丢线总宽度大于十
            //追加判断突出角要在(40, 60)的范围里，且突出角形状较平(应该是一个 |_ 的形状这里判了 | 这个）
            //然后左边应该是直道，这里取三点算斜率
            if(t_jh - jh >= 10 && right_black[jh - 4] > 40 && right_black[jh - 4] < 60 &&
                abs(right_black[jh - 6] - right_black[jh - 2]) < 3 &&
                abs(left_black[50] + left_black[30] - (left_black[40] << 1)) < 3)// && img[jh - 4][CAMERA_W - 1] == 0)
            {
                    isiscircul_flag = 1;
                    //找到了
            }
        }
    }
    //如果在圆里了，这里判出圆,这里补的是出圆的线
    else if(isiscircul_flag == 2)
    {
        //int8 tem_i;
        //如果两边都是直道了代表出圆成功，重置标志位于0
        if(right_black[40] > left_black[40] && left_black[40] >0 && right_black[40] - left_black[40] < 55 &&
            right_black[30] > left_black[30] && left_black[30] >0 && right_black[30] - left_black[30] < 45 &&
           right_black[20] > left_black[20] && left_black[20] >0 && right_black[20] - left_black[20] < 35 ){
            //出圆了已经
            isiscircul_flag = 0;
            return ;
        }
        //tem_i = 30;
        //出圆时会遇见两边丢线的情况，所以这里补线
        if(left_black[30] < 0 && right_black[30] < 0)
        {
            for(t_jh = 30; t_jh <= 55; t_jh++) //下面找
            {
                if(left_black[t_jh] > 0 && left_black[t_jh - 1] < 0)
                {
                    break;
                }
            }
            //这里在寻找补线用的突出角，具体可以看图像
            if(t_jh != 56)
            {
                for(;left_black[t_jh + 1] - left_black[t_jh] >= 0 && t_jh < LINE_NUM - 1; t_jh++);
                for(;left_black[t_jh] + 2 < CAMERA_W - 1 && t_jh > 0; t_jh --)
                {
                    left_black[t_jh - 1] = left_black[t_jh] + 2;
                }
            }
        }
    }
}

//这个是进圆的函数
//仅仅在标志位是1的时候调用，主要就是找 |_ 这个突出角，然后往左边补线，用来进圆
//<2018.5.4.yl>创建来进圆
void Goin_circul()
{
    int8 jh;
    for(jh = 55; jh > 35; jh --)
    {  
        //这里是防止，因为搜线缺陷会出现线全丢的情况进圆，这个时候给个固定offset来把车骗到圆里面
        if(right_black[jh] < 0 && left_black[jh] < 0){
            state_line[1] = 30;
            state_line[2] = 59;
            left_black[30] = 20;
            left_black[59] = 1;
            return ;
        }
    }    
    for(; jh > 10; jh --)
    {  
        //这里是找尖角
        if(right_black[jh] < 0 && right_black[jh - 1] > 40)
        {
            break;
        }
    }

    if(jh == 10)
    {
        //这里是找 |_ 突出角失败，代表已经进圆了，这里不严谨，还要改
        //找不到了，那么就进圆了
        isiscircul_flag = 2;
    }
    else
    {
        //找到突出角了，往左边-2补线，直到碰到左边线
        jh -= 4;
        left_black[jh] = right_black[jh - 1] - 2;
        state_line[1] = jh;
        for(; jh < LINE_NUM - 1; jh++)
        {
            if(left_black[jh] - 3 < 0 || left_black[jh] - 3 < left_black[jh + 1])
            {
                state_line[2] = jh;
                break;
            }
            left_black[jh + 1] = left_black[jh] - ((left_black[jh] < 40)?2:3);
        }
    }    
}


//这个是原来偏差函数的基础上，添加了补线后偏差算法
void Get_error_cal(float *offset, int *count)
{
    int i;
    if(isiscircul_flag == 1){
        //准备进圆，补线
        *offset += ((float)(left_black[state_line[1]] - left_black[state_line[2]])*(state_line[2] - state_line[1])/(LINE_NUM - state_line[1]));
    }
    else
	for (i = LINE_NUM - 1; i >= 1; i--)
	{
		if (-2 == middleline[i])
			break;
		else if (-1 == middleline[i]) {}
		else if(isiscircul_flag != 2 || i >= 35)
		{
			*offset = *offset + ((float)(middleline[i] - CAMERA_W / 2)*(1 + (60 - i)*TRAPEZOID_CORRECT / 40));          //offset是补偿，用来描述整体赛道的偏向,<0偏左
			(*count)++;
			if (middleline[i] > CAMERA_W - 1)middleline[i] = CAMERA_W - 1;
			if (middleline[i] < 0)middleline[i] = 0;
			img[i][middleline[i]] = !img[i][middleline[i]];
		}
	}
}


/*****************************
 * 
 *         以下暂时还没用到
 * 
 ****************************/
void Judge_body()
{
    uint8 state_line_num[5];  
    uint8 cnt;
    int16 jh;
    for(cnt = 0, jh = LINE_NUM - 1; cnt < 5; cnt++){
        if(jh > 10){
            state_line[cnt] = Judge_block(&jh, &state_line_num[cnt]);
        }
        else if(jh <= 10){
            state_line[cnt] = 0;
        }
    }
    
    if(jh>30){
        state_line_num[0] = state_line_num[1] = 0;
        //这.我猜是图像花了
    }

    if(state_line[0] == 2 && state_line[1] == 2){
        //直道
    } 
    else if((abs(state_line[0]) == 1 && state_line[1] == 0) || (abs(state_line[1]) == 1 && state_line[2] == 0)){
        //弯道
    }
    else if(abs(state_line[0]) == 1 && state_line[1] == 2 && abs(state_line[2]) == 1 && state_line[3] == 2 && state_line[0] == state_line[2]){
        //圆环预判
        //isisiscircul_flag = 0;
        //judge_island();
    }
    else if(state_line[0] == 0 || (state_line[0] == -2 && state_line[1] != 2)){
        road_mark = rest; //电机停止工作
        // quit();
    }
    else{
        //判块失败，运行队友代码
    }
}

int8 Judge_block(int16 *jh, uint8 *s_num)
{  
    int8 tem_store[2] = {0};
    *s_num = 0;
    for(; *jh >= 10; (*jh) --) //
    {
        if(left_black[*jh] == -2 || right_black[*jh] == -2 || (left_black[*jh] >= right_black[*jh] && right_black[*jh] >= 0)){
            *jh = 1;
            return 0;
        } 
        else if((left_black[*jh] == -1 && right_black[*jh] != -1)){
            tem_store[1] = -1;
        }
        else if((left_black[*jh] != -1 && right_black[*jh] == -1)){
            tem_store[1] = 1;
        }
        else if(left_black[*jh] >= 0 && right_black[*jh] >= 0){
            if(abs(left_black[*jh] - right_black[*jh]) ){
                tem_store[1] = 2;
            }
            else if(abs(left_black[*jh] - left_black[*jh - 1]) > abs(right_black[*jh] - right_black[*jh - 1]) + 2){
                tem_store[1] = -1;
            } 
            else if(abs(left_black[*jh] - left_black[*jh - 1]) + 2 < abs(right_black[*jh] - right_black[*jh - 1])){
                tem_store[1] = 1;
            }            
        }
        else{
            tem_store[1] = -2;
        }

        if(!tem_store[0]){
            tem_store[0] = tem_store[1];
            *s_num ++;
        }
        else if(tem_store[1] == tem_store[0]){
            *s_num ++;
        }
        else if(*s_num < 6){
            tem_store[0] = tem_store[1];
            *s_num = 0;
        }
        else return tem_store[0];
    }
    if(*s_num < 6) return 0;
    return tem_store[0];
}


int8 Count_black(int16 jh, int8 start, int8 end, int8 extent)
{
    for(; start < end; end += extent){
        if(img[jh][end] == 0)
        {
            break;
        }
    }
    return end - start + 1;
}