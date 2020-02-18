#include "isr.h"
#include "tim.h"
#include "stm32f1xx_hal.h"
#include "remote.h"
#include "stm32f1xx_it.h"
#include "sfr.h"
#include "wired.h"
#include "common.h"

uint8_t flag_4ms=0;
uint8_t control_flag_4ms=0;
__IO uint32_t sys_time;
__IO uint32_t sys_time_1s;
uint8_t sys_time_counter=0;

uint32_t temp;
uint32_t temp_throttle_pwm;

uint32_t LEFT_MOTOR_CaptureNumber=0;    // 输入捕获数
uint32_t LEFT_MID_CaptureNumber=0;
uint32_t RIGHT_MID_CaptureNumber=0;
uint32_t RIGHT_MOTOR_CaptureNumber=0;

/****************************************************************************
 *
 * Function Name:    sys_tick_post_isr
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      系统定时器
 * Note:
 *
 ***************************************************************************/
void sys_tick_post_isr(void)   // 1ms 中断一次
{

    sys_time = HAL_GetTick();     //获取tick值（毫秒）

    if((sys_time & 0x03) == 0x03 )    //sys_time & 00000011
    {
        flag_4ms ^= 0x01;
    }
}



/****************************************************************************
 *
 * Function Name:    HAL_TIM_IC_CaptureCallback
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      定时器输入捕获中断
 * Note:
 *
 ***************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == htim2.Instance)   //TIM2
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)              //通道1上升沿
        {
            remoter.posedge_counter= HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_1);   //捕获TIM2CH1上升沿来临时的CNT值
        }

        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)        //通道2下降沿
        {
            remoter.negedge_counter = HAL_TIM_ReadCapturedValue(&htim2,TIM_CHANNEL_2);   //捕获TIM2CH2下降沿来临时的CNT值
            remoter_signal_process();   //遥控器信号处理
            __HAL_TIM_SET_COUNTER(&htim2, 0);     //清除CNT值

        }
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
        {
            LEFT_MOTOR_CaptureNumber++;
        }
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
        {
            LEFT_MID_CaptureNumber++;
        }

    }



}



