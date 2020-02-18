#ifndef __remote_H
#define __remote_H

#include "stm32f1xx_hal.h"

#define servo_front_pwm_up   200
#define servo_front_pwm_down 350

extern uint32_t    u32gCntIRQ;                 //中断计数器

enum RUN_DIR
{
    RUN_DIR_STOP=0,
    RUN_DIR_FRONT,
    RUN_DIR_BACK,
    RUN_DIR_LEFT,
    RUN_DIR_RIGHT,
    RUN_DIR_FRONT_LEFT,
    RUN_DIR_FRONT_RIGHT,
    RUN_DIR_BACK_LEFT,
    RUN_DIR_BACK_RIGHT,
};

enum MOTOR_DIR
{
    MOTOR_DIR_STOP=0,
    MOTOR_DIR_FRONT,
    MOTOR_DIR_BACK,
};

enum MOTOR_TURN
{
    MOTOR_TURN_NONE=0,
    MOTOR_TURN_LEFT,
    MOTOR_TURN_RIGHT,
};

enum REMOTER_CHANNEL
{
    REMOTER_CHANNEL_NONE,
    REMOTER_CHANNEL_LEFT_RIGHT,
    REMOTER_CHANNEL_FRONT_BACK,
    REMOTER_CHANNEL_THROTTLE,

    REMOTER_CHANNEL_RESERVED,

    REMOTER_CHANNEL_MODE,
    REMOTER_CHANNEL_VSM,

    REMOTER_CHANNEL_PUMP,
    REMOTER_CHANNEL_WASH_MOTOR,
    REMOTER_CHANNEL_END,
};

enum  COMMAND
{
    remote_speed ,
    remote_dir ,
    vacuum ,
    brush ,
    remote_brush_dir ,
    pump_valve ,
    remote_angle ,
    states ,
    REMOTE_RX_END
};

#define  REMOTE_DATA_LENGTH   REMOTE_RX_END

struct Remoter
{
    enum MOTOR_DIR motor_dir;
    enum MOTOR_TURN motor_turn;
    enum RUN_DIR run_dir;
    uint8_t run_dir_last;
    uint8_t channel_counter;

    uint16_t posedge_counter;
    uint16_t negedge_counter;
    uint16_t throttle_pwm;		//油门
    uint16_t servo_motor_pwm;	//舵机
    uint16_t servo_front_motor_pwm;	//前舵机
    uint16_t pulse_width[REMOTER_CHANNEL_END];

};

struct Command
{
    uint8_t remote_rx_buf[REMOTE_DATA_LENGTH];		//遥控接收buf
};
extern struct Command command;

extern struct Remoter remoter;

//遥控器PPM解码
extern uint16_t    u16gPPMCnt1;    //PPM开始
extern uint16_t    u16gPPMCnt2;
extern uint16_t    u16gCHCnt;     //PPM通道
extern int16_t     s16pCHDuty[9];


//void parse_remote_data(void);
void timer_Start_IT(void) ;
void remote_val(void);
void remoter_signal_process(void);
void Computing_speed(void);
void Computing_dir(void);
void Computing_rpm(void);
#endif

