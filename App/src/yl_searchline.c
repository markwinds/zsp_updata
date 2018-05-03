#include "searchroad.h"
#include "common.h"
#include "include.h"

int8 state_line[5];
enum road_condition road_mark = rest;

//这个暂时还没用到
void Judge_body()
{
    uint8 state_line_num[5];  
    uint8 cnt;
    int16 jh;
    for(cnt = 0, jh = LINE_NUM - 1; cnt < 5; cnt++){
        if(jh > 10){
            state_line[cnt] = judge_block(&jh, &state_line_num[cnt]);
        }
        if(jh <= 10){
            break;
        }
    }
    
    if(jh>30){
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
        
        //judge_island();
    }
    else if(state_line[0] == 0){
        road_mark = rest; //电机停止工作
        // quit();
    }
    else{
        //判块失败，运行队友代码
    }
}

int8 Judge_block(int16 *jh, uint8 *s_num)
{  
    int8 tem_store[2] = {3};
    s_num = 0;
    for(; *jh <= 10; *jh--) //
    {
        if(left_black[*jh] == -2 || right_black[*jh] == -2 || left_black[*jh] >= right_black[*jh]){
            tem_store[1] = 0;
        } 
        else if((left_black[*jh] == -1 && right_black[*jh] != -1)){
            tem_store[1] = -1;
        }
        else if((left_black[*jh] != -1 && right_black[*jh] == -1)){
            tem_store[1] = 1;
        }
        else if(left_black[*jh] >= 0 && right_black[*jh] >= 0){
            if(abs(left_black[*jh] - right_black[*jh]) < (*jh + 20) ){
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

        if(tem_store[0] == 3){
            tem_store[0] = tem_store[1];
            *s_num ++;
        }
        else if(tem_store[1] == tem_store[0]){
            *s_num ++;
        }
        else if(*s_num < 3){
            tem_store[0] = tem_store[1];
            *s_num = 0;
        }
        else return tem_store[0];
    }
    return 0;
}

//基本上就是队友的代码，改了单边丟线情况
void Get_middle_line(){
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
				if (-1 == left_black[jh] && -1 == right_black[jh]) middleline[jh] = CAMERA_W / 2;
				else
				{
                    //这里用的公式是matlab拟合出来的
					if (-1 == right_black[jh])
                    {
                        tem_val = left_black[jh] + (int16)(33.82 + 0.977*abs(left_black[jh] - left_black[LINE_NUM - 1]) - 0.7718*(LINE_NUM - jh));
                        middleline[jh] = ( (tem_val >= CAMERA_W) ? (CAMERA_W - 1) : tem_val);
                    } 
					else
                    {
                        tem_val = right_black[jh] - (int16)(33.82 + 0.977*abs(right_black[jh] - right_black[LINE_NUM - 1]) - 0.7718*(LINE_NUM - jh));
                        middleline[jh] = ( (tem_val < 0) ? (0) : tem_val);
                    } 
				}
			}
			else
			{
				middleline[jh] = (left_black[jh] + right_black[jh]) / 2;
			}
		}
        if (left_black[jh] == -2 || right_black[jh] != -2)
        {
            middleline[jh] = -2;
        }        
		jh--;
	}
    if (left_black[jh] == -2 || right_black[jh] != -2)
    {
        middleline[jh] = -2;
    }  
}