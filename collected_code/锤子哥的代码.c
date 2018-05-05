/*****************************************
 *         这是一份来自锤子哥的代码
 *         他的注释由yl来补充
 *         希望大家喜欢
******************************************/
if (i == 22)
{
  //这里主要判断第一个丟线，
  //          对   cur_R_ready_flag   判断，成功后标志位置1    
  //这里是搜线到第22行之后，分别判断22行和24行是否是左边不丟线，右边丟线的情况(这里数组前的 非号 是没丢)。
  //而且还追加了赛道必须在(45,75)之间的判定
  //street_len不出意外是赛道长度
  if (!Left_Add_Flag[i] && !Left_Add_Flag[i + 2] && Right_Add_Flag[i] && Right_Add_Flag[i + 2] && street_len > 47 && Width_Real[i + 2] > 45 && Width_Real[i + 2] < 75) //
  {
    cur_R_ready_flag = 1; //右圆环预识别       左不丢线 右丢线
  }
  else
    cur_R_ready_flag = 0; //不清零  只要过了第一段丢线 清了后面进行不下去


  if (cur_R_ready_flag || cur_R_ready_delay_flag)
  {
    //置1延时标志位
    cur_R_ready_delay_flag = 1; //第一段丢线判断后置 "1"  作为内圆判断的前提条件
    
    //就是判断黑圆在22，24的位置
    //此处判断22与24行左右均 没有 丟线，且赛道长度小于40
    //成功后内圆标志置1
    if ((!Right_Add_Flag[i] && !Right_Add_Flag[i + 2]) && !Left_Add_Flag[i] && !Left_Add_Flag[i + 2] && cur_R_ready_time_flag > 10 && street_len > 40) //
    {
      cur_R_ready_rest_flag = 1; //内圆判断标志
      aa = 1;
    }
    else
      ;

    //第一段丢线0.5秒后 第一段标志和内圆标志清零
    //左边丟线，这个丟线意义不明
    if (cur_R_ready_time_flag > 200 || Left_Add_Flag[i] || Left_Add_Flag[i + 2]) /*此处时间需要调试*/
    {
      cur_R_ready_delay_flag = 0; //第一段丢线标志清零
      cur_R_ready_rest_flag = 0;  //内圆标志清零
    }

    //第一段丢线0.5秒内  找到第二段丢线  内圆和第一段标志清零
    else if (cur_R_ready_rest_flag && Right_Add_Flag[i] && Right_Add_Flag[i + 2] && !Left_Add_Flag[i] && !Left_Add_Flag[i + 2] && street_len > 40)
    {
      cur_R_ready_delay_flag = 0; //第一段丢线标志清零
      cur_R_ready_rest_flag = 0;  //内圆标志清零
      cur_R_real_flag = 1;        //第二段丢线标志
      bb = 1;
    }
    else
      ;
  }
  else //否则标志都清零
  {
    cur_R_ready_delay_flag = 0;
    cur_R_ready_time_flag = 0;
    cur_R_ready_rest_flag = 0;
  }
}
/*右准确识别*/
if (cur_R_real_flag || cur_R_real_delay_flag) //第二段标志下
{
  cur_R_real_flag = 0;       //第二段标志清零
  cur_R_real_delay_flag = 1; //重新置 "1" 第二段标志
  led(LED1, LED_ON);
  //这里应该转弯了，即刚进圆环，所以本来前面的直道变成丟线了
  if (i == 32 && cur_R_real_time_flag > 80 && (Left_Add_Flag[i + 2] && !Right_Add_Flag[i + 2])) //在第二段丢线0.5秒后 且左边22 24丢线
  {
    cur_R_real_rest_flag = 1; //复位标志
    xx = 1;
  }

  if (cur_R_real_rest_flag)
  {
    cur_R_real_time_flag1++;
  }

  if (cur_R_real_time_flag1 > 200)
  {
    cur_R_real_time_flag1 = 200;
  }

  if (i == 22 && cur_R_real_time_flag1 > 60 && !Left_Add_Flag[i] && !Left_Add_Flag[i + 2] && !Right_Add_Flag[i] && !Right_Add_Flag[i + 2])
  {
    cur_R_real_delay_flag = 0; //清零第二段丢线标志   （此时赋值为第二段丢线标志）
  }

}
else
{
  cur_R_real_delay_flag = 0;
  cur_R_real_time_flag = 0;
  cur_R_real_time_flag1 = 0;
  cur_R_real_rest_flag = 0;
}
//增量式PID 电机 控制
int32 PID_Realize(PID *sptr, int32 ActualSpeed, int32 SetSpeed)
{
  //当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
  int32 iError, //当前误差
      Increase; //最后得出的实际增量

  iError = SetSpeed - ActualSpeed; //计算当前误差

  Increase = (int)(sptr->KP * (iError - sptr->LastError) + sptr->KI * iError + sptr->KD * (iError - 2 * sptr->LastError + sptr->PrevError)); //加速度 ********************强制装换数据类型 防止数据出错*********************
  sptr->PrevError = sptr->LastError;                                                                                                         //更新前次误差
  sptr->LastError = iError;                                                                                                                  //更新上次误差

  return Increase;
}
