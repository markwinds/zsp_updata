#include "searchroad.h"
#include "common.h"
#include "include.h"

int8 state_line[5];
int8 is_rightcircul_flag = 0; // 0表示未检测到，1表示准备进圆，2表示已经进圆
int8 is_leftcircul_flag = 0;
enum road_condition road_mark = straightaway;

//基本上就是队友的代码，用matlab改了单边丟线情况
//<2018.5.5.yl>加入了出圆筛选中线
void Get_middle_line()
{
    int jh;
    int16 tem_val;
    jh = LINE_NUM - 1;
    float voff = 1.0*vaild_mark / 60 + 1.43;
    while (jh >= vaild_mark && left_black[jh] != -2 && right_black[jh] != -2)
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
                    if (is_rightcircul_flag == 2)
                    {
                        middleline[jh] = -2;
                    }
                    else
                        middleline[jh] = CAMERA_W / 2;
                }
                else
                {
                    //这里用的公式是matlab拟合出来的
                    if (-1 == right_black[jh])
                    {
                        tem_val = left_black[jh] + (int16) (36.82 + voff * abs(left_black[jh] - left_black[LINE_NUM - 1]) - 0.7718 * (LINE_NUM - jh));
                        middleline[jh] = ((tem_val >= CAMERA_W) ? (CAMERA_W - 1) : tem_val);
                    }
                    else
                    {
                        if (right_black[LINE_NUM - 1] < 0)
                        {
                            tem_val = right_black[jh] - (int16)(
                            (36.82 + voff * abs(right_black[jh] - CAMERA_W + 1) - 0.7718 * (LINE_NUM - jh)));
                        }
                        else
                        {
                            tem_val = right_black[jh] - (int16)(
                            (36.82 + voff * abs(right_black[jh] - right_black[LINE_NUM - 1]) - 0.7718 * (LINE_NUM - jh)));
                        }
                        middleline[jh] = ((tem_val < 0) ? (0) : tem_val);
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
        if(middleline[jh] >= 0 && middleline[jh] <= CAMERA_W){
            img[jh][middleline[jh]] = !img[jh][middleline[jh]];
        }
    }
    if (left_black[jh] == -2 || right_black[jh] == -2)
    {
        middleline[jh] = -2;
    }
}

//这里是整个圆环判断主体
//<2018.5.4.yl>创建了函数，用于判断圆环
//<2018.5.5.yl>加入了出圆，丢线补线
//<2018.5.18.yl>更改了圆环判定，新方法条件宽松，且正常跑车时,尚未误判,加入了左圆判断。
void Judge_circul()
{
    int8 jh;
    int8 t_jh;
    if (is_rightcircul_flag == 0  && abs(left_black[50] + left_black[20] - (left_black[35] << 1)) < 3)
    {
        //找一找右圆环在不在
        if (left_black[25] > 0 && right_black[25] < 0) //从25行开始搜丟线宽度
        {
            for (jh = 25; jh >= 10; jh--) //上面找丢线宽度
            {
                if (right_black[jh] > 0 || left_black[jh] < 0)
                {
                    break;
                }
            }
            for (t_jh = 25; t_jh <= 45; t_jh++) //下面找丢线宽度
            {
                if (right_black[t_jh] > 0 || left_black[t_jh] < 0)
                {
                    break;
                }
            }
            //减一减得到单边丢线总宽度大于十
            //追加判断突出角要在(40, 60)的范围里，且突出角形状较平(应该是一个 |_ 的形状这里判了 | 这个）
            //然后左边应该是直道，这里取三点算斜率
            /*if(t_jh - jh >= 10 && right_black[jh - 4] > 40 && right_black[jh - 4] < 60 &&
                abs(right_black[jh - 6] - right_black[jh - 2]) < 3 &&
                abs(left_black[50] + left_black[30] - (left_black[40] << 1)) < 3)// && img[jh - 4][CAMERA_W - 1] == 0)
            {
                    is_rightcircul_flag = 1;
                    //找到了
            }*/
            if (t_jh - jh >= 10 && right_black[t_jh + 1] > 0)
            {
                int8 i;
                for (i = 3; i < 7; i++)
                {
                    if (right_black[t_jh + i] > right_black[t_jh + i - 1] ||
                        (right_black[t_jh + i] - right_black[t_jh + i - 1]) > (right_black[t_jh + i - 1] - right_black[t_jh + i - 2]))
                    {
                        break;
                    }
                }
                if (i == 7)
                    is_rightcircul_flag = 1;
            }
        }
    }
    //如果在圆里了，这里判出圆,这里补的是出圆的线,right
    else if (is_rightcircul_flag == 2)
    {
        //int8 tem_i;
        //如果两边都是直道了代表出圆成功，重置标志位于0
        if (right_black[40] > left_black[40] && left_black[40] > 0 && right_black[40] - left_black[40] < 55 &&
            right_black[30] > left_black[30] && left_black[30] > 0 && right_black[30] - left_black[30] < 45 &&
            right_black[20] > left_black[20] && left_black[20] > 0 && right_black[20] - left_black[20] < 35)
        {
            //出圆了已经
            is_rightcircul_flag = 0;
            return;
        }
        //tem_i = 30;
        //出圆时会遇见两边丢线的情况，所以这里补线
        if (left_black[30] < 0 && right_black[30] < 0)
        {
            for (t_jh = 30; t_jh <= 55; t_jh++) //下面找
            {
                if (left_black[t_jh] > 0 && left_black[t_jh - 1] < 0)
                {
                    break;
                }
            }
            //这里在寻找补线用的突出角，具体可以看图像
            if (t_jh != 56)
            {
                for (; left_black[t_jh + 1] - left_black[t_jh] >= 0 && t_jh < LINE_NUM - 1; t_jh++)
                    ;
                for (; left_black[t_jh] + 2 < CAMERA_W - 1 && t_jh > 0; t_jh--)
                {
                    left_black[t_jh - 1] = left_black[t_jh] + 2;
                }
            }
        }
    }

    if (is_leftcircul_flag == 0  && abs(right_black[50] + right_black[20] - (right_black[35] << 1)) < 3)
    {
    //     //找一找左圆环在不在
    //     if (left_black[25] < 0 && right_black[25] > 0) //从25行开始搜丟线宽度
    //     {
    //         for (jh = 25; jh >= 10; jh--) //上面找丢线宽度
    //         {
    //             if (right_black[jh] < 0 || left_black[jh] > 0)
    //             {
    //                 break;
    //             }
    //         }
    //         for (t_jh = 25; t_jh <= 45; t_jh++) //下面找丢线宽度
    //         {
    //             if (right_black[t_jh] < 0 || left_black[t_jh] > 0)
    //             {
    //                 break;
    //             }
    //         }
    //         //减一减得到单边丢线总宽度大于十
    //         //追加判断突出角要在(40, 60)的范围里，且突出角形状较平(应该是一个 |_ 的形状这里判了 | 这个）
    //         //然后左边应该是直道，这里取三点算斜率
    //         /*if(t_jh - jh >= 10 && right_black[jh - 4] > 40 && right_black[jh - 4] < 60 &&
    //             abs(right_black[jh - 6] - right_black[jh - 2]) < 3 &&
    //             abs(left_black[50] + left_black[30] - (left_black[40] << 1)) < 3)// && img[jh - 4][CAMERA_W - 1] == 0)
    //         {
    //                 is_rightcircul_flag = 1;
    //                 //找到了
    //         }*/
    //         if (t_jh - jh >= 10 && left_black[t_jh + 1] > 0)
    //         {
    //             int8 i;
    //             for (i = 3; i < 7; i++)
    //             {
    //                 if (left_black[t_jh + i] < left_black[t_jh + i - 1] ||
    //                     (left_black[t_jh + i] - left_black[t_jh + i - 1]) > (left_black[t_jh + i - 1] - left_black[t_jh + i - 2]))
    //                 {
    //                     break;
    //                 }
    //             }
    //             if (i == 7)
    //                 is_leftcircul_flag = 1;
    //         }
    //     }
    // }
    // //如果在圆里了，这里判出圆,这里补的是出圆的线,left
    // else if (is_leftcircul_flag == 2)
    // {
    //     //int8 tem_i;
    //     //如果两边都是直道了代表出圆成功，重置标志位于0
    //     if (right_black[40] > left_black[40] && left_black[40] > 0 && right_black[40] - left_black[40] < 55 &&
    //         right_black[30] > left_black[30] && left_black[30] > 0 && right_black[30] - left_black[30] < 45 &&
    //         right_black[20] > left_black[20] && left_black[20] > 0 && right_black[20] - left_black[20] < 35)
    //     {
    //         //出圆了已经
    //         is_leftcircul_flag = 0;
    //         return;
    //     }
    //     //tem_i = 30;
    //     //出圆时会遇见两边丢线的情况，所以这里补线
    //         for (t_jh = 30; t_jh <= 55; t_jh++) //下面找
    //         {
    //             if (right_black[t_jh] < 0 && left_black[t_jh] < 0)
    //             {
    //                 right_black[t_jh] = t_jh >> 1;
    //             }
    //         }
    //     if (left_black[30] < 0 && right_black[30] < 0)
    //     {

    //         // //这里在寻找补线用的突出角，具体可以看图像
    //         // if (t_jh != 56)
    //         // {
    //         //     for (; right_black[t_jh + 1] - right_black[t_jh] < 0 && t_jh < LINE_NUM - 1; t_jh++)
    //         //         ;
    //         //     for (; right_black[t_jh] - 2 >= 0 && img[t_jh][right_black[t_jh] - 2] && t_jh > 0; t_jh--)
    //         //     {
    //         //         right_black[t_jh - 1] = right_black[t_jh] - 2;
    //         //     }
    //         // }
    //     }
    }
}

//这个是进圆的函数
//仅仅在标志位是1的时候调用，主要就是找 |_ 这个突出角，然后往左边补线，用来进圆
//<2018.5.4.yl>创建来进圆
//<2018.5.18.yl>加入了对应的左圆,进圆判断加了条件。
void Goin_rightcircul()
{
    int8 jh;
    for (jh = 55; jh > 20; jh--)
    {
        //这里是防止，因为搜线缺陷会出现线全丢的情况进圆，这个时候给个固定offset来把车骗到圆里面
        if (right_black[jh] < 0 && left_black[jh] < 0)
        {
            state_line[1] = 30;
            state_line[2] = 59;
            left_black[30] = 20;
            left_black[59] = 1;
            return;
        }
        if (right_black[jh] < 0 && right_black[jh - 1] > 40)
        {
            break;
        }
    }
    // for (; jh > 10; jh--)
    // {
    //     //这里是找尖角

    //     if (left_black[jh] < 0 && right_black[jh] < 0)
    //     {
    //         jh = 10;
    //         break;
    //     }
    // }

    if (jh == 10 && left_black[30] > 10)
    {
        //这里是找 |_ 突出角失败，代表已经进圆了，这里不严谨，还要改
        //找不到了，那么就进圆了
        is_rightcircul_flag = 2;
    }
    else if (jh != 10)
    {
        //找到突出角了，往左边-2补线，直到碰到左边线
        jh -= 2;
        while(jh > 10 && right_black[jh] > 60)
        {
            jh --;
        }
        vaild_mark = jh;
        left_black[jh] = right_black[jh - 1] - 2;
        state_line[1] = jh;
        for (; jh < LINE_NUM - 1; jh++)
        {
            if (left_black[jh] - 3 < 0 || left_black[jh] - 3 < left_black[jh + 1])
            {
                state_line[2] = jh;
                break;
            }
            left_black[jh + 1] = left_black[jh] - ((left_black[jh] < 40) ? 2 : 3);
        }
    }
}

void Goin_leftcircul()
{
    int8 jh;
    for (jh = 55; jh > 35; jh--)
    {
        //这里是防止，因为搜线缺陷会出现线全丢的情况进圆，这个时候给个固定offset来把车骗到圆里面
        if (right_black[jh] < 0 && left_black[jh] < 0)
        {
            state_line[1] = 30;
            state_line[2] = 59;
            right_black[30] = 1;
            right_black[59] = 16;
            return;
        }
    }
    for (; jh > 10; jh--)
    {
        //这里是找尖角
        if (left_black[jh] < 0 && left_black[jh - 1] > 0)
        {
            break;
        }
        if (left_black[jh] < 0 && right_black[jh] < 0)
        {
            jh = 10;
            break;
        }
    }

    if (jh == 10 && right_black[30] < CAMERA_W - 10 && right_black[30] > 0)
    {
        //这里是找 |_ 突出角失败，代表已经进圆了，这里不严谨，还要改
        //找不到了，那么就进圆了
        is_leftcircul_flag = 2;
    }
    else if (jh != 10)
    {
        //找到突出角了，往右边+2补线，直到碰到右边线
        jh -= 4;
        vaild_mark = jh;
        right_black[jh] = left_black[jh - 1] + 2;
        state_line[1] = jh;
        for (; jh < LINE_NUM - 1; jh++)
        {
            if (right_black[jh] + 3 >= CAMERA_W || right_black[jh] + 3 >= right_black[jh + 1])
            {
                state_line[2] = jh;
                break;
            }
            right_black[jh + 1] = right_black[jh] + ((right_black[jh] < 40) ? 2 : 3);
        }
    }
}

//这个是原来偏差函数的基础上，添加了补线后偏差算法
//<2018.5.18.yl>加入了左圆补线
#ifdef OLD
void Get_error_cal(float *offset, int *count)
{
    int i;
    float qoff = 1;
    if (is_rightcircul_flag == 1)
    {
        //准备进圆，补线
        *offset += ((float)(left_black[state_line[1]] - left_black[state_line[2]]) * (state_line[2] - state_line[1]) / (LINE_NUM - state_line[1]));
        *offset *= 1.6;
    }
    else if (is_leftcircul_flag == 1)
    {
        *offset += ((float)(right_black[state_line[1]] - right_black[state_line[2]]) * (state_line[2] - state_line[1]) / (LINE_NUM - state_line[1]));
    }
    else
        for (i = LINE_NUM - 1; i >= vaild_mark; i--)
        {
            qoff += 0.035;
            if (-2 == middleline[i])
                break;
            else if (-1 == middleline[i])
            {
            }
            else if (is_rightcircul_flag != 2 || i >= 20)
            {
                *offset = *offset + qoff * (float)(middleline[i] - CAMERA_W / 2); //offset是补偿，用来描述整体赛道的偏向,<0偏左
                (*count)++;
                if (middleline[i] > CAMERA_W - 1)
                    middleline[i] = CAMERA_W - 1;
                if (middleline[i] < 0)
                    middleline[i] = 0;
            }
        }
#elif 1
void Get_error_cal(float *offset)
{
    float qoff = 0;
    *offset = 0;
    for (uint8 i = LINE_NUM - 1; i >= vaild_mark; i--)
    {
        qoff += 0.035;
        *offset = *offset + qoff * (float)(middleline[i] - CAMERA_W / 2); //offset是补偿，用来描述整体赛道的偏向,<0偏左
        img[i][middleline[i]] = !img[i][middleline[i]];
    }
    *offset /= (LINE_NUM - vaild_mark);
    if(Ma_Mark == 2)
    {
        *offset -= (Be_Offset / 2);
    }
    else if(Ma_Mark == 3)
    {
        *offset += (Be_Offset / 2);
    }
#endif
}

/*****************************
 * 
 *         以下暂时还没用到
 * 
 ****************************/
#if 0
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
        //isis_rightcircul_flag = 0;
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
#elif 1

/*!
 *  @brief      搜线主体
 *  @since      v5.0
 *  @introducation  
 *  @update
 *      <2018.5.25.yl> 初步完成弯道补线算法
 */
int8 Ma_Mark = 1;
// 描述这场图像状态
// 0:全丟线 1:直道 2:左弯道 3.右弯道 4:十字 6:左圆环 9:右圆环
int8 As_Mark;
// 描述扫描暂态
// 0:正常状态 1:直道 2:左丟线 3:右丟线 4:全丢 5:弯道初判 6:全丢一判
int8 Bo_Mark;
//描述暂态长度
int16 Co_Mark;
//新的暂态位置
int8 Ne_Mark;
//左右圆环标志

float Ma_Offset = 0;
float Be_Offset = 0;
float La_Offset = 0;

int8 right_land_flag = 0;
int8 left_land_flag = 0;

PID Co_Steer[10];

int8 left_virtual[CAMERA_H];
int8 right_virtual[CAMERA_H];
void yl_Search_line()
{   
    //数据初始化

        int16 jw_left;                                       //向左搜索的当前列  
        int16 jw_right;  
        int16 jh;                                            //行参数        
        int8 skew = 0;

        //Ma_Mark = 1;
        As_Mark = 1;
        Ne_Mark = 1;
        Co_Mark = 0;
        int8 Lo_Mark = 0;

        int8 Lo_Start = 0;
        int8 Lo_End = 0;

        int8 begin_s = 0;
        int8 begin_d = 0;
        int8 end_i = 0;
        int8 tem_l = 0;
        int8 tem_r = 0;
        int8 tem_m = 0;
        //int8 vaild_mark = 4;

        jh = LINE_NUM-1;

        int8 min_road = CAMERA_W - 1;
        
        
        // int16 left_black_before = CAMERA_W / 2;              //上一次左边扫描到的黑线位置
        // int16 left_next_line = 0;                            //判断扫描是否进入了新的一行
        // int16 left_find_flag = 0;                            //左边是否找到黑线标志												                



        // int16 right_black_before = CAMERA_W / 2;
        // int16 right_next_line = 0;
        // int16 right_find_flag = 0;



        // float offset = 0;                                   //偏差度，为整体偏差度
        // int slope[CAMERA_H] = { 0 };                       //存放每行间黑线斜度的数组
        // int16 count = 0;
        // int16 i = 0;
        // int16 j = 0;
        // int16 m = 0;

        // int16 cross_temp[2] = { -1,-1 };

        // int8 Curve = 0; //弯道行
        // int8 End_s = 0; //搜索中止行

    //

    LCD_numf(tem_site_str[5], (double)2, GREEN, BLUE);
    //先扫3行______从59到57
    for(;jh > LINE_NUM - 4; jh--)
    {
        if(!img[jh][middleline[jh]])
        {   //从中间开始,如果本来就是黑的,判为出赛道,停下
            lcd_mode = STOP_MODE;
            return ;
        }

        //从中心往左扫
        for(jw_left = middleline[jh]; jw_left > 0; jw_left--)
        {   //寻找白黑跳变点
            if(!img[jh][jw_left] && img[jh][jw_left + 1]){
                break;
            }
        }

        //从中心往右扫
        for(jw_right = middleline[jh]; jw_right < CAMERA_W - 1; jw_right++)
        {   //寻找白黑跳变点
            if(!img[jh][jw_right] && img[jh][jw_right - 1]){
                break;
            }
        }

        if(jw_left >= 0 && jw_right < CAMERA_W && jw_right - jw_left > 50)
        {   //正确扫描到赛道,记录下赛道长度,记录左右点,中线为平均值
            if(As_Mark == 1){
                Co_Mark ++;}
            else{
                As_Mark = 1;
                Co_Mark = 0;}
            middleline[jh] = (jw_left + jw_right)>>1;
        }
        else if(jw_left == -1 && jw_right == CAMERA_W)
        {   //两边丟线,十字复判,丟线行用上一场数据
            middleline[jh] = 40;
        }
        else if(jw_right == CAMERA_W || jw_left == -1)
        {   //单边丟线,直接赛宽补线

            if(jw_left < 0)
            {   //记录当前左边丟线几行
                if(As_Mark == 2){
                    Co_Mark ++;}
                else{
                    As_Mark = 2;
                    Co_Mark = 0;}                
                middleline[jh] = jw_right - 29;
                if(middleline[jh] < 5) middleline[jh] = 5;
            }
            else if(jw_right >= CAMERA_W)
            {   //记录当前右边丟线几行
                if(As_Mark == 3){
                    Co_Mark ++;}
                else{
                    As_Mark = 3;
                    Co_Mark = 0;}
                middleline[jh] = jw_left + 29;
                if(middleline[jh] > CAMERA_W - 6) middleline[jh] = CAMERA_W - 6;
            }
        }
        else
        {   //判不出来,重新扫
            Ma_Mark = 0;
            return;           
        }
        left_virtual[jh] = left_black[jh] = jw_left;
        right_virtual[jh] = right_black[jh] = jw_right;

        tem_m = jw_right - jw_left;
        if(tem_m < min_road)
        {
            min_road = tem_m;
        }
    }

    LCD_numf(tem_site_str[5], (double)3, GREEN, BLUE);
    //********************************************************************************
    //正常扫描______从56行开始,隔两行扫往0扫
    for(; jh>=2; jh -=2)
    {
        if(!img[jh][middleline[jh + 2]])
        {   //从上一行的中点开始扫,如果是黑点,则扫完退出
            // if(jh < 50 && left_black[jh + 6] == 0 && Ma_Mark != 4 &&
            //  (skew == -1 || (left_black[59] < 5 && left_black[50] < 5))){   //&& begin_s
            // if(jh < 50 && left_black[jh + 6] == 0 && right_black[jh + 2] < 40)
            // {
            //     //提早较多扫完，且之前单边丟线,弯道补线
            //     vaild_mark = jh;
            //     Ma_Mark = 2;
            //     Be_Offset = BlockOffset(jh,-1);
            //     //FullBend(jh, -1, begin_s);
            // }
            // // else if(jh < 50 && right_black[jh + 6] == CAMERA_W - 1 && Ma_Mark != 4 &&
            // //  (skew == 1 || (right_black[59] > 75 && right_black[50] > 75))){  //&& begin_s
            // else if(jh < 50 && right_black[jh + 6] == CAMERA_W - 1 && left_black[jh + 2] > 40)
            // {
            //     //提早较多扫完，且之前单边丟线,弯道补线
            //     vaild_mark = jh;
            //     Ma_Mark = 3;
            //     Be_Offset = BlockOffset(jh,1);
            //     //FullBend(jh, 1, begin_s);
            // }
            // else
            vaild_mark = jh; //停止搜索,记录终止行
            break;
        }

        //________________________________________________________________________

        //搜左边
        for(jw_left = middleline[jh + 2]; jw_left >= 0; jw_left--)
        {
            if(!img[jh][jw_left] && img[jh][jw_left + 1])
            {
                break;
            }
        }

        //搜右边
        for(jw_right = middleline[jh + 2]; jw_right < CAMERA_W; jw_right++)
        {
            if(!img[jh][jw_right] && img[jh][jw_right - 1])
            {
                break;
            }
        }

        //_________________________________________________________________________

        // 处理一下
        if((jw_right - jw_left > 75))
        {   //两边丟线
            Lo_Mark ++;
            Ne_Mark = 4;
            if(Ma_Mark == 6 || Ma_Mark == 9)
            {   //圆环出现,全部直接用上一场的数据
                break;
            }
        }
        else
        {   //其他情况
            tem_m = jw_right - jw_left;

            if(jw_left < 0)
            {   //左丢线,记录丟线宽度
                if(Ma_Mark == 4)
                {
                    jw_right = CAMERA_W - 1;
                }
                Lo_Mark ++;
                Ne_Mark = 2;
            }
            else if(jw_right == CAMERA_W)
            {   //右丟线,记录丟线宽度
                if(Ma_Mark == 4)
                {
                    jw_left = -1;
                }
                Lo_Mark ++;
                Ne_Mark = 3;
            }
            //路宽判丢
            else if(tem_m > min_road)
            {   
                Lo_Mark ++;
            }
            else if(tem_m < min_road)
            {
                Lo_Mark = 0;
                min_road = tem_m;
            }      


        }

        //判丢标记
        if(Lo_Mark && !Lo_Start)
        {
            Lo_Start = jh + 1;
        }
        else if(!Lo_Mark && Lo_Start && !Lo_End)
        {
            Lo_End = jh;
        }
        else if(Lo_Mark && Lo_End)
        {
            vaild_mark = jh;
            break;
        }
        else if(Lo_Mark == 4 && min_road > 30)
        {
            min_road --;
            Lo_Mark = 1;
        }

        //算中线
        left_black[jh] = (jw_left < 0)? 0:jw_left;      
        right_black[jh] = (jw_right > (CAMERA_W - 1))? (CAMERA_W - 1):jw_right; 
             


        if(!Lo_Mark)
        {
            left_virtual[jh] = left_black[jh];
            right_virtual[jh] = right_black[jh];
            middleline[jh] = (left_black[jh] + right_black[jh]) >> 1;
            if(middleline[jh] < 0) middleline[jh] = 0;
            else if(middleline[jh] > CAMERA_W - 1) middleline[jh] = CAMERA_W - 1;               
        }
        else{
            left_virtual[jh] = left_virtual[jh + 2] > left_black[jh]? left_virtual[jh + 2] : left_black[jh];
            right_virtual[jh] = right_virtual[jh + 2] > right_black[jh]? right_black[jh] : right_virtual[jh + 2];
            middleline[jh] = (left_virtual[jh] + right_virtual[jh]) >> 1;
            if(middleline[jh] < 0) middleline[jh] = 0;
            else if(middleline[jh] > CAMERA_W - 1) middleline[jh] = CAMERA_W - 1;               
        }

        if(right_virtual[jh] - left_virtual[jh] <= 5)
        {
            vaild_mark = jh + 2;
            break;
        }

        //补下一条中线
        left_black[jh - 1] = left_black[jh];   
        right_black[jh - 1] = right_black[jh];
        middleline[jh - 1] = (middleline[jh]);          
        left_virtual[jh - 1] = left_virtual[jh];
        right_virtual[jh - 1] = right_virtual[jh];

        //Who are you?
        /*
        if(As_Mark == Ne_Mark){
            Co_Mark ++;
        }  
        else
        {   //换路检查
            switch(Ne_Mark) //换来的
            {
                case 1:        
                    break;
                case 2: 
                    if(!left_land_flag)
                    {
                        left_land_flag = 1;
                    }
                    else if(left_land_flag == 2)
                    {
                        left_land_flag = 3;
                    }
                    begin_s = jh - 2;
                    break;
                case 3:
                    if(!right_land_flag)
                    {
                        right_land_flag = 1;
                    }      
                    else if(right_land_flag == 2)
                    {
                        right_land_flag = 3;
                    }  
                    begin_s = jh - 2;
                    break;   
                case 4:
                    //begin_d = jh + 2;
                    break;         
            }
            switch(As_Mark) //被换的
            {
                case 1:
                    if(jh < 50)
                    {
                        if(left_black[jh + 2] > left_black[jh + 4] && right_black[jh + 2] >= right_black[jh + 4])
                        {
                            skew = 1;
                        }
                        else if(left_black[jh + 2] <= left_black[jh + 4] && right_black[jh + 2] < right_black[jh + 4])
                        {
                            skew = -1;
                        }
                    }
                    break;
                case 4:
                    if(Co_Mark >= 2 && jh < 50)
                    {
                        Ma_Mark = 4;
                    }
                    break;
            }
            As_Mark = Ne_Mark;
            Co_Mark = 0;
        }
        */
              
    }

    //补一下丟线结束标志
    if(Lo_End <= vaild_mark && Lo_Start)
    {   
        Lo_End = vaild_mark + 2;
    }    
    LCD_numf(tem_site_str[5], 4, GREEN, BLUE);
    //补线
    if(Lo_Start > Lo_End)
    {
        Site_t begin = {left_virtual[Lo_Start], Lo_Start},end = {left_virtual[Lo_End], Lo_End};

        FullLine(end, begin, left_virtual);
        begin.x = right_virtual[Lo_Start];
        end.x = right_virtual[Lo_End];
        FullLine(end, begin, right_virtual);
        for(jh = Lo_Start; jh <= Lo_End; jh--)
        {
            middleline[jh] = (left_virtual[jh] + right_virtual[jh]) >> 1;
            if(middleline[jh] < 0) middleline[jh] = 0;
            else if(middleline[jh] > CAMERA_W - 1) middleline[jh] = CAMERA_W - 1;                
        }
    }
    LCD_numf(tem_site_str[5], (double)5, GREEN, BLUE);
    tem_m = Lo_End - vaild_mark;
    // if(vaild_mark < 20)
    // {
    //     Ma_Mark = 1;
    // }
    // else if(Lo_End > 5 && tem_m < 5 && Lo_Start - Lo_End > 8)
    // {
    //     if(left_black[Lo_Start - 2] == 0 && left_black[Lo_Start - 4] == 0 && 
    //         left_black[Lo_End + 2] == 0 && left_black[Lo_End + 4] == 0)
    //     {
    //         Ma_Mark = 2;
    //         Be_Offset = BlockOffset(Lo_End,-1);
    //     }
    //     else if(right_black[Lo_Start - 2] == CAMERA_W - 1 && right_black[Lo_Start - 4] == CAMERA_W - 1 && 
    //         right_black[Lo_End + 2] == CAMERA_W - 1 && right_black[Lo_End + 4] == CAMERA_W - 1)
    //     {
    //         Ma_Mark = 3;
    //         Be_Offset = BlockOffset(Lo_End,1);
    //     }
    // }
    Get_error_cal(&Ma_Offset);
}

/*!
 *  @brief      两点斜率补线函数
 *  @since      v5.0
 *  @introducation  begin是高点,end是低点,line是要补线的数组.
 *                  begin,end都是坐标类型
 */
void FullLine(Site_t begin, Site_t end, int8 line[])
{ 
    LCD_point(begin,GREEN);  
    LCD_point(end,GREEN);
    float f_step;
    float base = (float)end.x;  
    int8 step = (end.y - begin.y);
    if(step <= 1) return ;
    f_step = 1.0*(begin.x - end.x)/step;
    for(int16 tem_i = end.y - 1; tem_i > begin.y; tem_i--  ){
        base += f_step;
        if(base > CAMERA_W - 2 || base < 1)
        {
            break;
        }        
        line[tem_i] = (uint8)base;
    }
}

/*!
 *  @brief      检查一条线是否是直线
 *  @since      v5.0
 *  @introducation  检查是否时在[20,50]范围内是直线
 */
uint8 IsStraight(int8 line[])
{
    return ((line[50] + line[35] - line[20] << 1)  < 3);
}

/*!
 *  @brief      弯道布线
 *  @since      v5.0
 *  @introducation  jh_ma是丟线后的一点,direction为-1表示左弯,1表示右弯
 *                  jh_begin是丟线起始点
 */
uint8 FullBend(int8 jh_ma, int8 direction, int8 jh_begin)
{
    uint8 jw, jh, jh_left, jh_right;
    if(direction == 1) //right
    {
        for(jh = jh_ma + 1; !img[jh][CAMERA_W - 1] && jh < CAMERA_H - 1; jh ++);
        if(img[jh][CAMERA_W - 1])
        {
            for(jw = CAMERA_W - 1; jw >  middleline[jh_begin] && jw > 35 && img[jh][jw]; jw --)
            {
                for(jh_left = jh; jh_left > 1; jh_left --)
                {
                    if(img[jh_left][jw] && !img[jh_left - 1][jw]) break;
                }

                for(jh_right = jh; jh_right < CAMERA_H - 2; jh_right ++)
                {
                    if(!img[jh_right][jw] && img[jh_right - 1][jw]) break;
                }

                if(jh_right > jh_begin + 5)//(jh_left + jh_right) >> 1 < jh_ma)
                {
                    break;
                }
                else 
                {
                    jh = (jh_left + jh_right) >> 1;
                    img[jh][jw] = !img[jh][jw];
                }
            }
        }
        Site_t begin = {jw + 1, jh}, end = {middleline[jh_begin], jh_begin};
        FullLine(begin, end, middleline);        
    }
    else if(direction == -1) //left
    {
        for(jh = jh_ma + 1; !img[jh][0] && jh < CAMERA_H - 1; jh ++);
        if(img[jh][0])
        {
            for(jw = 0; jw < middleline[jh_begin] && jw < 45 && img[jh][jw]; jw ++)
            {
                for(jh_left = jh; jh_left > 1; jh_left --)
                {
                    if(img[jh_left][jw] && !img[jh_left - 1][jw]) break;
                }

                for(jh_right = jh; jh_right < CAMERA_H - 2; jh_right ++)
                {
                    if(!img[jh_right][jw] && img[jh_right - 1][jw]) break;
                }

                if(jh_right > jh_begin + 5)//(jh_left + jh_right) >> 1 < jh_ma)
                {

                    break;
                }
                else 
                {
                    jh = (jh_left + jh_right) >> 1;
                    img[jh][jw] = !img[jh][jw];
                }
            } 
            Site_t begin = {jw + 1, jh}, end = {middleline[jh_begin], jh_begin};
            FullLine(begin, end, middleline);
        }
    }
}

int8 BlockOffset(int8 jh_ma,int8 direction)
{
    int8 white = 0;
    int8 black = 0;
    if(direction == 1)
    {
        for(int8 jh = jh_ma + 1;jh < CAMERA_H - 1; jh ++)
        {
            if(!white)
            {
                if(img[jh][CAMERA_W - 1]){
                    white = jh;
                }
            }
            else
            {
                if(!img[jh][CAMERA_W - 1]){
                    black = jh;
                    break;
                }
            }
        }        
    }
    else if(direction == -1)
    {
        for(int8 jh = jh_ma + 1;jh < CAMERA_H - 1; jh ++)
        {
            if(!white)
            {
                if(img[jh][0]){
                    white = jh;
                }
            }
            else
            {
                if(!img[jh][0]){
                    black = jh;
                    break;
                }
            }
        }        
    }
    if(black)return black - white;
    else return 30;
}
// void Search_line_left()
// {
//     int8 before = Curve - 3;

//     for(jh = 0; jh < right_black[Curve]; jh++ )
//     {

//         if(!img[before][jh])
//         {
//             return ;
//         }

//         for(jw = before; jw >= 0; jw--)
//         {
//             if(!img[jw][jh] && img[jw + 1][jh]){
//                 break;
//             }
//         }

//         if(jh > 3 && jw > right_black[jh - 2] + 1){

//         }
//     }


// }
#endif