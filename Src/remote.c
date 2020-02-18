#include "remote.h"
#include "stdint.h"
#include "tim.h"
#include "wired.h"



struct Command command;

uint16_t    u16gPPMCnt1=0;
uint16_t    u16gPPMCnt2=0;
uint16_t    u16gCHCnt=0;
int16_t    s16pCHDuty[9];

struct Remoter remoter;

/****************************************************************************
 *
 * Function Name:    timer_Start_IT
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      TIM2 TIM3接收中断使能
 * Note:
 *
 ***************************************************************************/
void timer_Start_IT()        // 开始遥控器接收  行走轮速度捕捉
{
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);

}

/****************************************************************************
 *
 * Function Name:    remoter_signal_process
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      遥控器信号处理
 * Note:
 *
 ***************************************************************************/
void remoter_signal_process(void)
{
    static uint8_t start_flag = 0;
    static int16_t pulse_width = 0;

    static int16_t temp;	//for test

    pulse_width = ((int16_t)remoter.negedge_counter) - remoter.posedge_counter;
    if(pulse_width > 3000)
    {
        start_flag = 1;
        remoter.channel_counter  = 0;
    }

    if(!start_flag)
    {
        return;
    }
    remoter.pulse_width[remoter.channel_counter] = pulse_width;
    switch(remoter.channel_counter)
    {
    case REMOTER_CHANNEL_NONE:

        break;

    case REMOTER_CHANNEL_LEFT_RIGHT:            //通道1：舵机方向
        temp = pulse_width;
        if(temp < 500)
        {
            temp = 500;
        }
        else if(temp > 1450)
        {
            temp = 1450;
        }
        if(temp < 700)
        {
            differential_speed = 700-temp;
            remoter.motor_turn = MOTOR_TURN_LEFT;
        }
        else if(temp > 1250)
        {
            differential_speed = (temp-1250);
            remoter.motor_turn = MOTOR_TURN_RIGHT;
        }
        else
        {
            differential_speed = 0;
            remoter.motor_turn = MOTOR_TURN_NONE;
        }
        break;

    case REMOTER_CHANNEL_FRONT_BACK:       //通道2：行走轮方向
        temp = pulse_width ;

        if(throttle_speed < 100)
        {
            remoter.motor_dir = MOTOR_DIR_STOP;
        }
        else
        {
            switch(remoter.motor_dir)
            {
            case MOTOR_DIR_STOP:
                if(temp < 600)
                {
                    remoter.motor_dir = MOTOR_DIR_FRONT;
                }
                if(temp > 1400)
                {
                    remoter.motor_dir = MOTOR_DIR_BACK;
                }
                break;

            case MOTOR_DIR_FRONT:
                if(temp > 1400)
                {
                    remoter.motor_dir = MOTOR_DIR_BACK;
                }
                break;

            case MOTOR_DIR_BACK:
                if(temp <600)
                {
                    remoter.motor_dir = MOTOR_DIR_FRONT;
                }
                break;

            default:
                break;
            }
        }
        break;

    case REMOTER_CHANNEL_THROTTLE:
        temp = pulse_width -500;   //通道3  油门

        if(temp < 100)
        {
            temp = 0;
        }
        throttle_speed = temp;
        break;

    case REMOTER_CHANNEL_RESERVED:      //通道4：
        temp = pulse_width;

        break;

    case REMOTER_CHANNEL_MODE:          //通道5：模式
        if(remoter.pulse_width[REMOTER_CHANNEL_THROTTLE] < 400)
        {
            break;
        }
        temp = pulse_width  ;
        if(temp < 600)
        {

        }
        else if(temp < 1100)
        {

        }
        else if(temp < 1600)
        {

        }
        break;

    case REMOTER_CHANNEL_VSM:           //通道6：吸水机
        temp = pulse_width;
        if(temp < 800)
        {

        }
        else if(temp < 1600)
        {

        }

        break;

    case REMOTER_CHANNEL_PUMP:         //通道7：水泵
        temp = pulse_width ;
        if(temp < 800)
        {

        }
        else if(temp < 1600)
        {

        }
        break;

    case REMOTER_CHANNEL_WASH_MOTOR:     //通道8：毛刷
        temp = pulse_width ;
        if(temp < 600)
        {

        }
        else if(temp>1400)
        {

        }
        else
        {

        }
        break;

    default:
        break;
    }
    remoter.channel_counter++;
    if(remoter.channel_counter >= 9)
    {
        start_flag = 0;
        remoter.channel_counter  = 0;
    }
}

void Computing_dir(void)
{
    switch(remoter.motor_dir)
    {
    case MOTOR_DIR_STOP:
        switch(remoter.motor_turn)
        {
        case MOTOR_TURN_NONE:
            remoter.run_dir = RUN_DIR_STOP;
            break;

        case MOTOR_TURN_LEFT:
            remoter.run_dir = RUN_DIR_LEFT;
            break;

        case MOTOR_TURN_RIGHT:
            remoter.run_dir = RUN_DIR_RIGHT;
            break;

        default:
            remoter.run_dir = RUN_DIR_STOP;
            break;
        }
        break;

    case MOTOR_DIR_FRONT:
        switch(remoter.motor_turn)
        {
        case MOTOR_TURN_NONE:
            remoter.run_dir = RUN_DIR_FRONT;
            break;

        case MOTOR_TURN_LEFT:
            remoter.run_dir = RUN_DIR_FRONT_LEFT;
            break;

        case MOTOR_TURN_RIGHT:
            remoter.run_dir = RUN_DIR_FRONT_RIGHT;
            break;

        default:
            remoter.run_dir = RUN_DIR_STOP;
            break;
        }
        break;

    case MOTOR_DIR_BACK:
        switch(remoter.motor_turn)
        {
        case MOTOR_TURN_NONE:
            remoter.run_dir = RUN_DIR_BACK;
            break;

        case MOTOR_TURN_LEFT:
            remoter.run_dir = RUN_DIR_BACK_LEFT;
            break;

        case MOTOR_TURN_RIGHT:
            remoter.run_dir = RUN_DIR_BACK_RIGHT;
            break;

        default:
            remoter.run_dir = RUN_DIR_STOP;
            break;
        }
        break;
    default:
        remoter.run_dir = RUN_DIR_STOP;
        break;
    }
}


void Computing_speed(void)
{
    switch(remoter.run_dir)
    {
    case RUN_DIR_STOP:
        left_speed = 0;
        right_speed = 0;
        break;

    case RUN_DIR_FRONT:
        left_speed = throttle_speed;
        right_speed = throttle_speed;
        break;

    case RUN_DIR_BACK:
        left_speed = -throttle_speed;
        right_speed = -throttle_speed;
        break;

    case RUN_DIR_LEFT:
        left_speed = -differential_speed;
        right_speed = differential_speed;
        break;

    case RUN_DIR_RIGHT:
        left_speed = differential_speed;
        right_speed = -differential_speed;
        break;

    case RUN_DIR_FRONT_LEFT:
        left_speed = throttle_speed-differential_speed;
        right_speed = throttle_speed+differential_speed;
        break;

    case RUN_DIR_FRONT_RIGHT:
        left_speed = throttle_speed+differential_speed;
        right_speed = throttle_speed-differential_speed;
        break;

    case RUN_DIR_BACK_LEFT:
        left_speed = -throttle_speed+differential_speed;
        right_speed = -throttle_speed-differential_speed;
        break;

    case RUN_DIR_BACK_RIGHT:
        left_speed = -throttle_speed-differential_speed;
        right_speed = -throttle_speed+differential_speed;
        break;

    default:
        left_speed = 0;
        right_speed = 0;
        break;
    }
    if(left_speed >= 1000)
    {
        left_speed = 1000;
    }

    if(right_speed >= 1000)
    {
        right_speed = 1000;
    }

    if(left_speed <= -1000)
    {
        left_speed = -1000;
    }

    if(right_speed <= -1000)
    {
        right_speed = -1000;
    }
}
void Computing_rpm(void)
{
    left_rpm = left_speed*2/5;
    right_rpm =	right_speed*2/5;

    if(left_rpm >= 400)
    {
        left_rpm = 400;
    }

    if(right_rpm >= 400)
    {
        right_rpm = 400;
    }

    if(left_rpm <= -400)
    {
        left_rpm = -400;
    }

    if(right_rpm <= -400)
    {
        right_rpm = -400;
    }

}



