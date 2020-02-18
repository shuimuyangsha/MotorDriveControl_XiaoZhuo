#ifndef __isr_H
#define __isr_H

#include "stm32f1xx_hal.h"


//extern uint16_t move_pwm;
//extern uint16_t temp;


extern uint32_t temp_throttle_pwm;
extern uint32_t  temp;
extern uint8_t   TIM2CH1_CAPTURE_STA,ppm_rx_sta,ppm_rx_num;	//输入捕获状态
extern uint16_t	TIM2CH1_CAPTURE_VAL;	//输入捕获值
extern uint16_t  ppm_rx[10];//ppm_rx[0]   1   接收到ppm数据


extern uint8_t flag_4ms;
extern uint8_t control_flag_4ms;
extern __IO uint32_t sys_time;
extern __IO uint32_t sys_time_1s;
extern uint8_t sys_time_counter;

extern uint32_t FL_CaptureNumber;    // 输入捕获数
extern uint32_t BL_CaptureNumber;
extern uint32_t FR_CaptureNumber;
extern uint32_t BR_CaptureNumber;



void sys_tick_post_isr(void);

void BACK_MOTOR_EXTI0(void);
void FRONT_MOTOR_EXTI4(void);
void PUMP_EXTI6(void);
void VAC_EXTI7(void);
//void HAL_TIM_IC_CaptureCallback(htim2);
//void HAL_TIM_IC_CaptureCallback(htim3);
void remote_input_capture_isr(TIM_HandleTypeDef *htim);
void speed_capture_isr(void);
void uart1_pre_isr(void);
void uart2_pre_isr(void);
#endif


