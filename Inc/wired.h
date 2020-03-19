#ifndef __wired_H
#define __wired_H

#include "stm32f1xx.h"

//驱动器返回给上位机的数据
enum UART_UPLINK_DATA
{
    UART_UP_ID,				//驱动器ID
    UART_UP_SPEED1,			//速度低8位
    UART_UP_SPEED2,			//速度高8位
    UART_UP_ERROR1,			//错误状态低8位
    UART_UP_ERROR2,			//错误状态高8位
    UART_UP_VOLTAGE,		//电压，为实际值的10倍，最大25.5V，高于此电压均显示25.5V
    UART_UP_CURRENT,		//电流，为实际值的10倍，最大25.5A，高于此电压均显示25.5A
    UART_UP_TEMPERATURE,	//温度
    UART_UP_CRC_L,			//CRC16校验结果低8位
    UART_UP_CRC_H,			//CRC16校验结果高8位
    UART_UP_END,
};

#define UART_UPLINK_DATA_LENGTH	UART_UP_END
#define UART_UPLINK_TX_TIME		(87*UART_UPLINK_DATA_LENGTH)


enum UART_DOWNLINK_DATA
{
    UART_DOWN_ID,				//	驱动器编号
    UART_DOWN_CMD,				//命令字
    UART_DOWN_DATA1,			//数据低8位
    UART_DOWN_DATA2,			//数据高8位
    UART_DOWN_CRC_L,			//CRC16校验结果低8位
    UART_DOWN_CRC_H,			//CRC16校验结果高8位
    UART_DOWN_END,
};
#define UART_DOWNLINK_DATA_LENGTH	UART_DOWN_END


enum DRIVER_CMD							//命令
{
    DRIVER_CMD_NULL =0,													//空命令
    DRIVER_CMD_SPEED,													//速度控制
    DRIVER_CMD_MODE,												//速度控制方式：0：自适应；1：PWM；2：串口；3：电位器
    DRIVER_CMD_ID,												//设备ID

    DRIVER_CMD_HALL_TYPE,											//霍尔类型
    DRIVER_CMD_MOTOR_PALARITY,									//5电机极对数
    DRIVER_CMD_MAX_RMP,											//额定转速
    DRIVER_CMD_LOOP,												//7 开闭环控制：0：开环；1：速度闭环；2：位置环
    DRIVER_CMD_SPEED_TIME,										//加速时间
    DRIVER_CMD_MAX_CURRENT,										//最大电流
    DRIVER_CMD_DECEL_TIME,											//10减速时间

    DRIVER_CMD_ERR_RESTART_ENABLE,										//发生错误后自动重启，0：不重启，1：重启
    DRIVER_CMD_ERR_DELAY_TIME,									//发生错误后的等待时间
    DRIVER_CMD_STALL_TIME,				//							//触发堵转保护的时间

    DRIVER_CMD_ERROR_MASK,												//DATA0: 0:清除，1：设置； DATA1:异常号
    DRIVER_CMD_DIR_REVERSE,											//控制方向反相，0：不反相，1：反相
    DRIVER_CMD_BAUDRATE,												//16波特率设置，见枚举
    DRIVER_CMD_MAX_TORQUE,												//最大力矩

    DRIVER_CMD_CW_PHASE_OFFSET,										//正转相位偏置
    DRIVER_CMD_CCW_PHASE_OFFSET,	//								//反转相位偏置


    DRIVER_CMD_PC_CONFIG,												//保存配置程序

    DRIVER_CMD_PC_ERROR,													//flash保存异常记录
    DRIVER_CMD_PC_CONNECT,												//电脑串口连接
    DRIVER_CMD_PC_RESPONSE,												//响应

    DRIVER_CMD_KP,												//速度环PID参数P
    DRIVER_CMD_KI,												//速度环PID参数I
    DRIVER_CMD_KD,	//										//速度环PID参数D

    DRIVER_CMD_IQ_KP,												//写IQ PID的KP值	 ：
    DRIVER_CMD_IQ_KI,												//写IQ PID的KI值	 ：
    DRIVER_CMD_IQ_KD,												//写IQ PID的KD值	 ：


    DRIVER_CMD_ID_KP,												//写ID PID的KP值	 ：
    DRIVER_CMD_ID_KI,												//写ID PID的KI值	 ：
    DRIVER_CMD_ID_KD,												//32写ID PID的KD值	 ：

    DRIVER_CMD_POS_KP,												//写位置环 PID的KP值	 ：
    DRIVER_CMD_POS_KI,												//写位置环 PID的KI值	 ：
    DRIVER_CMD_POS_KD,												//写 PID的KD值	 ：

    DRIVER_CMD_INSTANT_ACCEL_TIME,									//瞬时加速时间
    DRIVER_CMD_INSTANT_DECEL_TIME,									//瞬时减速时间

    DRIVER_CMD_PC_FAST_READ_CONFIG,										//快速读取配置参数
    DRIVER_CMD_PC_FAST_SET_CONFIG,										//快速设置配置参数
    DRIVER_CMD_PC_READ_CONFIG,											//读取配置参数
    DRIVER_CMD_PC_SET_CONFIG,											//设置配置参数
    DRIVER_CMD_PC_REAL_TIME_DATA,
    DRIVER_CMD_PC_CONTROL,

    DRIVER_CMD_ERROR_TRIG_CURRENT,								//最小异常触发电流，小于该值，短路异常，反转异常将不会触发
    DRIVER_CMD_SERIAL_MODE,											//设置串口模式

    DRIVER_CMD_FAST_SHUTDOWN_TIME,									//快速关机时间，与加减速时间类似，专门用于快速关机的减速过程
    DRIVER_CMD_SLOW_SHUTDOWN_TIME,									//慢速关机时间，与加减速时间类似，专门用于慢速关机的减速过程

	DRIVER_CMD_MOTOR_ENABLE,										// 电机使能，0：电机不使能，电机处于松轴状态；1：电机使能
	
    DRIVER_CMD_DATA_END,
};



#define MAX_DISTANCE 765
#define MAX_UART_TX_SIZE UART_DOWNLINK_DATA_LENGTH
#define MAX_UART_RX_SIZE UART_UPLINK_DATA_LENGTH
#define UART_RX_BYTE_TIMEOUT 2

struct Uart_Buf {
    uint8_t rx_buf[MAX_UART_RX_SIZE];		  	//串口接收buf
    uint8_t tx_buf[MAX_UART_TX_SIZE];			//串口发送buf

    uint8_t dump_data;

    uint8_t rx_data_update_flag;					//数据更新标志位，收到新的数据包后，该标志位将会反转
    uint8_t rx_data_process_flag;					//数据处理标志位，处理完一个数据包后该标志位将会反转

    uint32_t tx_counter;							//发送计数器
    uint32_t tx_size;								//发送字节总数
    uint32_t rx_counter;							//接收计数器
    uint32_t error_counter;
    uint32_t rx_time;								//接收时间，用于检测数据包是否超时

};

struct Driver_Run_Status
{
	int16_t rt_speed;
	uint16_t error;
	uint8_t voltage;
	uint8_t current;
	uint8_t tempareture;
	int32_t shift;
	int16_t shift_error;
};



extern int left_rpm;
extern int right_rpm;
extern int left_speed;
extern int right_speed;
extern int throttle_speed;
extern int differential_speed;
extern struct Uart_Buf left_uart_buf;
extern struct Uart_Buf right_uart_buf;
extern struct Uart_Buf com485_uart_buf;

void com485_uart_pre_isr(void);
void run_control(void);
#endif
