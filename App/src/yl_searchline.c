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
        //找一找左圆环在不在
        if (left_black[25] < 0 && right_black[25] > 0) //从25行开始搜丟线宽度
        {
            for (jh = 25; jh >= 10; jh--) //上面找丢线宽度
            {
                if (right_black[jh] < 0 || left_black[jh] > 0)
                {
                    break;
                }
            }
            for (t_jh = 25; t_jh <= 45; t_jh++) //下面找丢线宽度
            {
                if (right_black[t_jh] < 0 || left_black[t_jh] > 0)
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
            if (t_jh - jh >= 10 && left_black[t_jh + 1] > 0)
            {
                int8 i;
                for (i = 3; i < 7; i++)
                {
                    if (left_black[t_jh + i] < left_black[t_jh + i - 1] ||
                        (left_black[t_jh + i] - left_black[t_jh + i - 1]) > (left_black[t_jh + i - 1] - left_black[t_jh + i - 2]))
                    {
                        break;
                    }
                }
                if (i == 7)
                    is_leftcircul_flag = 1;
            }
        }
    }
    //如果在圆里了，这里判出圆,这里补的是出圆的线,left
    else if (is_leftcircul_flag == 2)
    {
        //int8 tem_i;
        //如果两边都是直道了代表出圆成功，重置标志位于0
        if (right_black[40] > left_black[40] && left_black[40] > 0 && right_black[40] - left_black[40] < 55 &&
            right_black[30] > left_black[30] && left_black[30] > 0 && right_black[30] - left_black[30] < 45 &&
            right_black[20] > left_black[20] && left_black[20] > 0 && right_black[20] - left_black[20] < 35)
        {
            //出圆了已经
            is_leftcircul_flag = 0;
            return;
        }
        //tem_i = 30;
        //出圆时会遇见两边丢线的情况，所以这里补线
            for (t_jh = 30; t_jh <= 55; t_jh++) //下面找
            {
                if (right_black[t_jh] < 0 && left_black[t_jh] < 0)
                {
                    right_black[t_jh] = t_jh >> 1;
                }
            }
        if (left_black[30] < 0 && right_black[30] < 0)
        {

            // //这里在寻找补线用的突出角，具体可以看图像
            // if (t_jh != 56)
            // {
            //     for (; right_black[t_jh + 1] - right_black[t_jh] < 0 && t_jh < LINE_NUM - 1; t_jh++)
            //         ;
            //     for (; right_black[t_jh] - 2 >= 0 && img[t_jh][right_black[t_jh] - 2] && t_jh > 0; t_jh--)
            //     {
            //         right_black[t_jh - 1] = right_black[t_jh] - 2;
            //     }
            // }
        }
    }
}

//这个是进圆的函数
//仅仅在标志位是1的时候调用，主要就是找 |_ 这个突出角，然后往左边补线，用来进圆
//<2018.5.4.yl>创建来进圆
//<2018.5.18.yl>加入了对应的左圆,进圆判断加了条件。
void Goin_rightcircul()
{
    int8 jh;
    for (jh = 55; jh > 35; jh--)
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
    }
    for (; jh > 10; jh--)
    {
        //这里是找尖角
        if (right_black[jh] < 0 && right_black[jh - 1] > 40)
        {
            break;
        }
        if (left_black[jh] < 0 && right_black[jh] < 0)
        {
            jh = 10;
            break;
        }
    }

    if (jh == 10 && left_black[30] > 10)
    {
        //这里是找 |_ 突出角失败，代表已经进圆了，这里不严谨，还要改
        //找不到了，那么就进圆了
        is_rightcircul_flag = 2;
    }
    else if (jh != 10)
    {
        //找到突出角了，往左边-2补线，直到碰到左边线
        jh -= 4;
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
void Get_error_cal(float *offset, int *count)
{
    int i;
    float qoff = 1;
    if (is_rightcircul_flag == 1)
    {
        //准备进圆，补线
        *offset += ((float)(left_black[state_line[1]] - left_black[state_line[2]]) * (state_line[2] - state_line[1]) / (LINE_NUM - state_line[1]));
    }
    else if (is_leftcircul_flag == 1)
    {
        *offset += ((float)(right_black[state_line[1]] - right_black[state_line[2]]) * (state_line[2] - state_line[1]) / (LINE_NUM - state_line[1]));
    }
    else
        for (i = LINE_NUM - 1; i >= 1; i--)
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
                img[i][middleline[i]] = !img[i][middleline[i]];
            }
        }
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
endif
int8 Ma_Mark;
// 0:全丟线 1:直道 2:弯道 4:十字 6:左圆环 9:右圆环
int8 As_Mark;
// 0:正常状态 1:右丟线一判 2:左丟线一判 3:左圆环判 4: 5:弯道初判 6:全丢一判
int16 Co_Mark;
void yl_Search_line()
{   
    //数据初始化
	{
        int16 left_black_before = CAMERA_W / 2;              //上一次左边扫描到的黑线位置
        int16 left_next_line = 0;                            //判断扫描是否进入了新的一行
        int16 left_find_flag = 0;                            //左边是否找到黑线标志												                
        int16 jw_left;                                       //向左搜索的当前列


        int16 right_black_before = CAMERA_W / 2;
        int16 right_next_line = 0;
        int16 right_find_flag = 0;
        int16 jw_right;

        int16 jh;                                            //行参数
        float offset = 0;                                   //偏差度，为整体偏差度
                                                        //int slope[CAMERA_H] = { 0 };                       //存放每行间黑线斜度的数组
        int16 count = 0;
        int16 i = 0;
        int16 j = 0;
        int16 m = 0;

        int16 cross_temp[2] = { -1,-1 };

        int8 Curve = 0; //弯道行
        int8 End_s = 0; //搜索中止行

        left_black_before = CAMERA_W / 2;
        right_black_before = CAMERA_W / 2;

        Ma_Mark = 1;
        As_Mark = 1;
        Co_Mark = 0;

        jh = LINE_NUM-1;
    }
    //


    //先扫5行
    for(;jh > LINE_NUM - 4; jh--)
    {
        if(!img[jh][40])
        {   //出赛道,停下
            lcd_mode = STOP_MODE;
            return ;
        }

        for(jw_left = 40; jw_left >= 0; jw_left--)
        {
            if(!img[jh][jw_left] && img[jh][jw_left + 1]){
                break;
            }
        }

        for(jw_right = 40; jw_right < CAMERA_W; jw_right++)
        {
            if(!img[jh][jw_right] && img[jh][jw_right - 1]){
                break;
            }
        }

        if(jw_left >= 0 && jw_right < CAMERA_W && jw_right - jw_left > 55)
        {   //正确扫描到赛道
            left_black[jh] = jw_left;
            right_black[jh] = jw_right;
            middleline[jh] = (jw_left + jw_right)>>1;
        }
        else if(jw_left == -1 && jw_right == CAMERA_W)
        {   //两边丟线

        }
        else if(jw_right - jw_left > 70)
        {
            
        }
        else if(jw_right - jw_left > 35)
        {   //单边丟线
            if(jw_left < 0)
            {
                Ma_Mark = 6;
            }
            else if(jw_right >= CAMERA_W)
            {
                Ma_Mark = 9;
            }
        }
        else
        {   //扫不到
            Ma_Mark = 0;
            return 0;           
        }
    }

    //正常扫描
    for(; jh>=0; jh --)
    {
        if(!img[jh][middleline[jh + 1]])
        {
            vaild_mark = jh; //停止搜索
            break;
        }

        //搜左边
        for(jw_left = middleline[jh + 1]; jw_left >= 0; jw_left--)
        {
            if(!img[jh][jw_left] && img[jh][jw_left + 1])
            {
                break;
            }
        }

        //搜右边
        for(jw_right = middleline[jh + 1]; jw_right < CAMERA_W; jw_right++)
        {
            if(!img[jh][jw_right] && img[jh][jw_right - 1])
            {
                break;
            }
        }

        if(jw_left == -1 && jw_right == CAMERA_W)
        {   //两行丟线
            if(Ma_Mark == 6 || Ma_Mask == 9)
            {   //直接用上一场的数据
                break;
            }
            As_Mark = 6;
        }
        else if(jw_left == -1 && jh > 10)
        {   //左丟线
            As_Mark = 2; //一判
        }
        else if(jw_right == CAMERA_W && jh > 10)
        {   //右丟线
            As_Mark = 1; //一判
        }
        else if(jw_left >= 0 && jw_right < CAMERA_W )
        {   //正常搜到
            left_black = jw_left; 
            right_black = jw_right;
            middleline = (jw_left + jw_right) >> 1;
        }
        else
        {
            vaild_mark = jh; //停止搜索
            break;           
        }
    }

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