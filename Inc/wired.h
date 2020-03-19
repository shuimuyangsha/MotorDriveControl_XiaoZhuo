#ifndef __wired_H
#define __wired_H

#include "stm32f1xx.h"

//���������ظ���λ��������
enum UART_UPLINK_DATA
{
    UART_UP_ID,				//������ID
    UART_UP_SPEED1,			//�ٶȵ�8λ
    UART_UP_SPEED2,			//�ٶȸ�8λ
    UART_UP_ERROR1,			//����״̬��8λ
    UART_UP_ERROR2,			//����״̬��8λ
    UART_UP_VOLTAGE,		//��ѹ��Ϊʵ��ֵ��10�������25.5V�����ڴ˵�ѹ����ʾ25.5V
    UART_UP_CURRENT,		//������Ϊʵ��ֵ��10�������25.5A�����ڴ˵�ѹ����ʾ25.5A
    UART_UP_TEMPERATURE,	//�¶�
    UART_UP_CRC_L,			//CRC16У������8λ
    UART_UP_CRC_H,			//CRC16У������8λ
    UART_UP_END,
};

#define UART_UPLINK_DATA_LENGTH	UART_UP_END
#define UART_UPLINK_TX_TIME		(87*UART_UPLINK_DATA_LENGTH)


enum UART_DOWNLINK_DATA
{
    UART_DOWN_ID,				//	���������
    UART_DOWN_CMD,				//������
    UART_DOWN_DATA1,			//���ݵ�8λ
    UART_DOWN_DATA2,			//���ݸ�8λ
    UART_DOWN_CRC_L,			//CRC16У������8λ
    UART_DOWN_CRC_H,			//CRC16У������8λ
    UART_DOWN_END,
};
#define UART_DOWNLINK_DATA_LENGTH	UART_DOWN_END


enum DRIVER_CMD							//����
{
    DRIVER_CMD_NULL =0,													//������
    DRIVER_CMD_SPEED,													//�ٶȿ���
    DRIVER_CMD_MODE,												//�ٶȿ��Ʒ�ʽ��0������Ӧ��1��PWM��2�����ڣ�3����λ��
    DRIVER_CMD_ID,												//�豸ID

    DRIVER_CMD_HALL_TYPE,											//��������
    DRIVER_CMD_MOTOR_PALARITY,									//5���������
    DRIVER_CMD_MAX_RMP,											//�ת��
    DRIVER_CMD_LOOP,												//7 ���ջ����ƣ�0��������1���ٶȱջ���2��λ�û�
    DRIVER_CMD_SPEED_TIME,										//����ʱ��
    DRIVER_CMD_MAX_CURRENT,										//������
    DRIVER_CMD_DECEL_TIME,											//10����ʱ��

    DRIVER_CMD_ERR_RESTART_ENABLE,										//����������Զ�������0����������1������
    DRIVER_CMD_ERR_DELAY_TIME,									//���������ĵȴ�ʱ��
    DRIVER_CMD_STALL_TIME,				//							//������ת������ʱ��

    DRIVER_CMD_ERROR_MASK,												//DATA0: 0:�����1�����ã� DATA1:�쳣��
    DRIVER_CMD_DIR_REVERSE,											//���Ʒ����࣬0�������࣬1������
    DRIVER_CMD_BAUDRATE,												//16���������ã���ö��
    DRIVER_CMD_MAX_TORQUE,												//�������

    DRIVER_CMD_CW_PHASE_OFFSET,										//��ת��λƫ��
    DRIVER_CMD_CCW_PHASE_OFFSET,	//								//��ת��λƫ��


    DRIVER_CMD_PC_CONFIG,												//�������ó���

    DRIVER_CMD_PC_ERROR,													//flash�����쳣��¼
    DRIVER_CMD_PC_CONNECT,												//���Դ�������
    DRIVER_CMD_PC_RESPONSE,												//��Ӧ

    DRIVER_CMD_KP,												//�ٶȻ�PID����P
    DRIVER_CMD_KI,												//�ٶȻ�PID����I
    DRIVER_CMD_KD,	//										//�ٶȻ�PID����D

    DRIVER_CMD_IQ_KP,												//дIQ PID��KPֵ	 ��
    DRIVER_CMD_IQ_KI,												//дIQ PID��KIֵ	 ��
    DRIVER_CMD_IQ_KD,												//дIQ PID��KDֵ	 ��


    DRIVER_CMD_ID_KP,												//дID PID��KPֵ	 ��
    DRIVER_CMD_ID_KI,												//дID PID��KIֵ	 ��
    DRIVER_CMD_ID_KD,												//32дID PID��KDֵ	 ��

    DRIVER_CMD_POS_KP,												//дλ�û� PID��KPֵ	 ��
    DRIVER_CMD_POS_KI,												//дλ�û� PID��KIֵ	 ��
    DRIVER_CMD_POS_KD,												//д PID��KDֵ	 ��

    DRIVER_CMD_INSTANT_ACCEL_TIME,									//˲ʱ����ʱ��
    DRIVER_CMD_INSTANT_DECEL_TIME,									//˲ʱ����ʱ��

    DRIVER_CMD_PC_FAST_READ_CONFIG,										//���ٶ�ȡ���ò���
    DRIVER_CMD_PC_FAST_SET_CONFIG,										//�����������ò���
    DRIVER_CMD_PC_READ_CONFIG,											//��ȡ���ò���
    DRIVER_CMD_PC_SET_CONFIG,											//�������ò���
    DRIVER_CMD_PC_REAL_TIME_DATA,
    DRIVER_CMD_PC_CONTROL,

    DRIVER_CMD_ERROR_TRIG_CURRENT,								//��С�쳣����������С�ڸ�ֵ����·�쳣����ת�쳣�����ᴥ��
    DRIVER_CMD_SERIAL_MODE,											//���ô���ģʽ

    DRIVER_CMD_FAST_SHUTDOWN_TIME,									//���ٹػ�ʱ�䣬��Ӽ���ʱ�����ƣ�ר�����ڿ��ٹػ��ļ��ٹ���
    DRIVER_CMD_SLOW_SHUTDOWN_TIME,									//���ٹػ�ʱ�䣬��Ӽ���ʱ�����ƣ�ר���������ٹػ��ļ��ٹ���

	DRIVER_CMD_MOTOR_ENABLE,										// ���ʹ�ܣ�0�������ʹ�ܣ������������״̬��1�����ʹ��
	
    DRIVER_CMD_DATA_END,
};



#define MAX_DISTANCE 765
#define MAX_UART_TX_SIZE UART_DOWNLINK_DATA_LENGTH
#define MAX_UART_RX_SIZE UART_UPLINK_DATA_LENGTH
#define UART_RX_BYTE_TIMEOUT 2

struct Uart_Buf {
    uint8_t rx_buf[MAX_UART_RX_SIZE];		  	//���ڽ���buf
    uint8_t tx_buf[MAX_UART_TX_SIZE];			//���ڷ���buf

    uint8_t dump_data;

    uint8_t rx_data_update_flag;					//���ݸ��±�־λ���յ��µ����ݰ��󣬸ñ�־λ���ᷴת
    uint8_t rx_data_process_flag;					//���ݴ����־λ��������һ�����ݰ���ñ�־λ���ᷴת

    uint32_t tx_counter;							//���ͼ�����
    uint32_t tx_size;								//�����ֽ�����
    uint32_t rx_counter;							//���ռ�����
    uint32_t error_counter;
    uint32_t rx_time;								//����ʱ�䣬���ڼ�����ݰ��Ƿ�ʱ

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
