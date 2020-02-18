#include "wired.h"
#include "sfr.h"
#include "isr.h"
#include "common.h"

int left_rpm;	//左边电机期望转速
int right_rpm;	//右边电机期望转速
int left_speed;
int right_speed;
int throttle_speed;
int differential_speed;

uint8_t sync_control_flag =0;//用于区分是否广播通信，值为0是单点通信，为1是广播通信
static uint8_t tx_index =0;//标志位指示数据发送给哪个ID的驱动器


struct Uart_Buf com485_uart_buf;
struct Driver_Run_Status left_driver_status;//左边驱动器状态
struct Driver_Run_Status right_driver_status;//右边驱动器状态

/****************************************************************************
 *
 * Function Name:    com485_rx_data_parse
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      解析驱动器返回的数据，获取驱动器的实时状态
 * Note:
 *
 ***************************************************************************/
void com485_rx_data_parse(void)
{
	uint8_t *buf = com485_uart_buf.rx_buf;
	uint8_t id_index = tx_index^0x01;//发送数据后将会把tx_index取反，发送数据后才会收到返回 的数据，故收到数据后，将tx_index取反将会得到此前发送数据时的tx_index
	struct Driver_Run_Status *driver_status;
	//根据id_index判断返回的数据来自左边驱动器或者右边驱动器
	if(!id_index)
	{
		driver_status = &left_driver_status;
		
	}
	else
	{
		driver_status = &right_driver_status;
	}
	
	driver_status->rt_speed = (int16_t)(buf[UART_UP_SPEED1]|(buf[UART_UP_SPEED2]<<8));//实时速度
	driver_status->error = (uint16_t)(buf[UART_UP_ERROR1]|(buf[UART_UP_ERROR2]<<8));//异常值
	driver_status->voltage = buf[UART_UP_VOLTAGE];//输入电压
	driver_status->current = buf[UART_UP_CURRENT];//电流
	driver_status->tempareture = buf[UART_UP_TEMPERATURE];//温度

}

/****************************************************************************
 *
 * Function Name:    com485_uart_pre_isr
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      串口接收中断程序
 * Note:
 *
 ***************************************************************************/
void com485_uart_pre_isr(void)
{
    uint64_t time = sys_time;
    if(COM485_UART->SR & UART_FLAG_RXNE)  //串口接收中断标志位置位
    {
        if(time - com485_uart_buf.rx_time > UART_RX_BYTE_TIMEOUT)
        {
            com485_uart_buf.rx_counter =0;
        }

        com485_uart_buf.rx_time = time;

        if(com485_uart_buf.rx_counter < UART_UPLINK_DATA_LENGTH)
        {
            com485_uart_buf.rx_buf[com485_uart_buf.rx_counter++] = COM485_UART->DR;
            if(com485_uart_buf.rx_counter == UART_UPLINK_DATA_LENGTH)
            {
                if(check_data_ingtegrity(com485_uart_buf.rx_buf, UART_UPLINK_DATA_LENGTH))  //若和校验通过，则反转标志位，以便在主循环debug_task中解析上位机发送的数据
                {
                    //com485_uart_buf.rx_data_update_flag ^= 0x01;	//反转标志位
					com485_rx_data_parse();//对接收的数据进行解析
                }

            }
        }
        else
        {
            com485_uart_buf.dump_data = COM485_UART->DR;
        }


    }
	//往数据寄存器写入最后一个字节数据后才使能发送完成中断，故进入发送完成中断时，表示所有数据已经发送完毕，可以关闭485发送功能
	if(COM485_UART->CR1 & UART_IT_TC)
	{
		COM485_UART->SR &= (~UART_FLAG_TC);
		COM485_UART->CR1 &= (~UART_IT_TC);
		Disable485TX();//
	}
	
    if(COM485_UART->CR1 & UART_IT_TXE)  //串口发送中断标志位置位
    {
        if(com485_uart_buf.tx_counter < UART_DOWNLINK_DATA_LENGTH)
        {
            if(COM485_UART->SR & UART_FLAG_TXE)
            {

                COM485_UART->DR = com485_uart_buf.tx_buf[com485_uart_buf.tx_counter++];

            }
			if(com485_uart_buf.tx_counter == UART_DOWNLINK_DATA_LENGTH)
			{
				COM485_UART->CR1 &= (~UART_IT_TXE);
				if(!sync_control_flag)
				{
					COM485_UART->SR &= (~UART_FLAG_TC);
					//发送所有数据后，要使能发送完成中断，等待所有数据发送完成了才能关闭485发送功能，否则会导致最后一个字节的数据发送不完整
					COM485_UART->CR1 |= (UART_IT_TC);
				}
				
			}
        }

    }
	
	
    if(COM485_UART->SR & (UART_FLAG_NE |UART_FLAG_PE |UART_FLAG_FE | UART_FLAG_ORE))    //（噪声错误标志 | 校验错误 | 帧错误 | 过载错误）
    {   //遇到帧错误等情况时，需要先将数据寄存器的数据读出来，否则无法清除中断标志位
        com485_uart_buf.dump_data = COM485_UART->SR;
        com485_uart_buf.dump_data = COM485_UART->DR;
    }

}

/****************************************************************************
 *
 * Function Name:    run_control
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      将遥控器信号转换成对左右电机的期望转速通过485总线发给左右两个驱动器
 * Note:
 *
 ***************************************************************************/
void run_control(void)
{
	
	uint8_t *buf = com485_uart_buf.tx_buf;
	static uint32_t tx_time =0;
	if(sys_time -tx_time < 10)//每隔10毫秒发送一次数据
	{
		return;
	}
	tx_time = sys_time;
	Enable485TX() ;//使能485发送

	if(left_rpm == right_rpm)//如果左右两个电机速度相同，可使用广播通信的方式发送命令
	{
		sync_control_flag = 1;
		
		buf[UART_DOWN_ID] = 0;						//为0表示两个驱动器都会接收
		buf[UART_DOWN_CMD] = DRIVER_CMD_SPEED;
		buf[UART_DOWN_DATA1] = left_rpm&0xff;
		buf[UART_DOWN_DATA2] = (left_rpm>>8)&0xff;
		
		buf[UART_DOWN_SUM] = buf[UART_DOWN_ID] + buf[UART_DOWN_CMD] + buf[UART_DOWN_DATA1]+buf[UART_DOWN_DATA2];
	}
	else
	{
		sync_control_flag = 0;
		//如果左右两个电机速度不相同，则先后给两个驱动器发送速度控制命令
		if(!tx_index)
		{
			buf[UART_DOWN_ID] = 1;						//为1表示左边驱动器
			buf[UART_DOWN_CMD] = DRIVER_CMD_SPEED;		//速度控制命令
			buf[UART_DOWN_DATA1] = left_rpm&0xff;
			buf[UART_DOWN_DATA2] = (left_rpm>>8)&0xff;
			
			buf[UART_DOWN_SUM] = buf[UART_DOWN_ID] + buf[UART_DOWN_CMD] + buf[UART_DOWN_DATA1]+buf[UART_DOWN_DATA2];
		}
		else
		{
			buf[UART_DOWN_ID] = 2;						//为2表示右边驱动器
			buf[UART_DOWN_CMD] = DRIVER_CMD_SPEED;		//速度控制命令
			buf[UART_DOWN_DATA1] = right_rpm&0xff;
			buf[UART_DOWN_DATA2] = (right_rpm>>8)&0xff;
			
			buf[UART_DOWN_SUM] = buf[UART_DOWN_ID] + buf[UART_DOWN_CMD] + buf[UART_DOWN_DATA1]+buf[UART_DOWN_DATA2];
		}
		tx_index ^= 0x01;
	}
	com485_uart_buf.tx_counter =0;
	COM485_UART->CR1 |= UART_IT_TXE;//启动串口数据发送
}

