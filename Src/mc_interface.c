#include "mc_interface.h"




bool MCI_StartMotor(char ID)
{
	static uint32_t tx_time = 0;
	if (sys_time - tx_time < SPEED_SETTING_RESPONSE_WAITTIME)//每隔10毫秒发送一次数据
	{
		return 0;
	}
	tx_time = sys_time;

	PrepareAFrame_XiaoZhuo((uint8_t *)&com485_uart_buf.tx_buf, ID, DRIVER_CMD_MOTOR_ENABLE, (int)1);

	USART2_RS485_SendCommand((char *)&com485_uart_buf.tx_buf, sizeof(com485_uart_buf.tx_buf));

	return 0;
}


bool MCI_StopMotor(char ID)
{
	static uint32_t tx_time = 0;
	if (sys_time - tx_time < SPEED_SETTING_RESPONSE_WAITTIME)//每隔10毫秒发送一次数据
	{
		return 0;
	}
	tx_time = sys_time;

	PrepareAFrame_XiaoZhuo((uint8_t *)&com485_uart_buf.tx_buf, ID, DRIVER_CMD_MOTOR_ENABLE, (int)0);

	USART2_RS485_SendCommand((char *)&com485_uart_buf.tx_buf, sizeof(com485_uart_buf.tx_buf));
	return 0;
}
