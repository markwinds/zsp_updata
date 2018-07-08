#include "common.h"
#include "include.h"

//float temp_p=0;
long quad_val = 0;
long quad_last = 0;
long ac_quad = 0;
int motor_speed_ture=0;
/*!
 *  @brief      main函数
 *  @since      v5.0
 *  @note       FTM 正交解码 测试
 */
void Quad_Init(void)
{
    ftm_quad_init(FTM1);                                    //FTM1 正交解码初始化（所用的管脚可查 port_cfg.h ）切记要把引脚改成pta8，pta9
    pit_init_ms(PIT0, 23);                                   //初始化PIT0，定时时间为： 23ms
    set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);      //设置PIT0的中断服务函数为 PIT0_IRQHandler
    enable_irq (PIT0_IRQn);                                 //使能PIT0中断
}

/*!
 *  @brief      PIT0中断服务函数
 *  @since      v5.0
 */
void PIT0_IRQHandler(void)
{
    PIT_Flag_Clear(PIT0);       //清中断标志位
    quad_val -= ftm_quad_get(FTM1);   //获取FTM正交解码的脉冲数(负数表示反方向)
    ftm_quad_clean(FTM1);
    if(cmotor)
    {
        PID_Realize(cmotor,(quad_val - quad_last)/3, (int)(motor_speed_ture));       
    }
    if(csteer)
    {
        ftm_pwm_duty(FTM0, FTM_CH6, 425 + (int)Steer_Pid( csteer ));
        //ftm_pwm_duty(FTM0, FTM_CH6, 430 + temp_p);
    }
    quad_last = quad_val;    
}

