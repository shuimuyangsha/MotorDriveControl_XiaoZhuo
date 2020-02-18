#include "wired.h"
#include "sfr.h"
#include "isr.h"
#include "common.h"

int left_rpm;	//��ߵ������ת��
int right_rpm;	//�ұߵ������ת��
int left_speed;
int right_speed;
int throttle_speed;
int differential_speed;

uint8_t sync_control_flag =0;//���������Ƿ�㲥ͨ�ţ�ֵΪ0�ǵ���ͨ�ţ�Ϊ1�ǹ㲥ͨ��
static uint8_t tx_index =0;//��־λָʾ���ݷ��͸��ĸ�ID��������


struct Uart_Buf com485_uart_buf;
struct Driver_Run_Status left_driver_status;//���������״̬
struct Driver_Run_Status right_driver_status;//�ұ�������״̬

/****************************************************************************
 *
 * Function Name:    com485_rx_data_parse
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      �������������ص����ݣ���ȡ��������ʵʱ״̬
 * Note:
 *
 ***************************************************************************/
void com485_rx_data_parse(void)
{
	uint8_t *buf = com485_uart_buf.rx_buf;
	uint8_t id_index = tx_index^0x01;//�������ݺ󽫻��tx_indexȡ�����������ݺ�Ż��յ����� �����ݣ����յ����ݺ󣬽�tx_indexȡ������õ���ǰ��������ʱ��tx_index
	struct Driver_Run_Status *driver_status;
	//����id_index�жϷ��ص�����������������������ұ�������
	if(!id_index)
	{
		driver_status = &left_driver_status;
		
	}
	else
	{
		driver_status = &right_driver_status;
	}
	
	driver_status->rt_speed = (int16_t)(buf[UART_UP_SPEED1]|(buf[UART_UP_SPEED2]<<8));//ʵʱ�ٶ�
	driver_status->error = (uint16_t)(buf[UART_UP_ERROR1]|(buf[UART_UP_ERROR2]<<8));//�쳣ֵ
	driver_status->voltage = buf[UART_UP_VOLTAGE];//�����ѹ
	driver_status->current = buf[UART_UP_CURRENT];//����
	driver_status->tempareture = buf[UART_UP_TEMPERATURE];//�¶�

}

/****************************************************************************
 *
 * Function Name:    com485_uart_pre_isr
 * Input:            None;
 * Output:           None;
 * Returns:          None;
 * Description:      ���ڽ����жϳ���
 * Note:
 *
 ***************************************************************************/
void com485_uart_pre_isr(void)
{
    uint64_t time = sys_time;
    if(COM485_UART->SR & UART_FLAG_RXNE)  //���ڽ����жϱ�־λ��λ
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
                if(check_data_ingtegrity(com485_uart_buf.rx_buf, UART_UPLINK_DATA_LENGTH))  //����У��ͨ������ת��־λ���Ա�����ѭ��debug_task�н�����λ�����͵�����
                {
                    //com485_uart_buf.rx_data_update_flag ^= 0x01;	//��ת��־λ
					com485_rx_data_parse();//�Խ��յ����ݽ��н���
                }

            }
        }
        else
        {
            com485_uart_buf.dump_data = COM485_UART->DR;
        }


    }
	//�����ݼĴ���д�����һ���ֽ����ݺ��ʹ�ܷ�������жϣ��ʽ��뷢������ж�ʱ����ʾ���������Ѿ�������ϣ����Թر�485���͹���
	if(COM485_UART->CR1 & UART_IT_TC)
	{
		COM485_UART->SR &= (~UART_FLAG_TC);
		COM485_UART->CR1 &= (~UART_IT_TC);
		Disable485TX();//
	}
	
    if(COM485_UART->CR1 & UART_IT_TXE)  //���ڷ����жϱ�־λ��λ
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
					//�����������ݺ�Ҫʹ�ܷ�������жϣ��ȴ��������ݷ�������˲��ܹر�485���͹��ܣ�����ᵼ�����һ���ֽڵ����ݷ��Ͳ�����
					COM485_UART->CR1 |= (UART_IT_TC);
				}
				
			}
        }

    }
	
	
    if(COM485_UART->SR & (UART_FLAG_NE |UART_FLAG_PE |UART_FLAG_FE | UART_FLAG_ORE))    //�����������־ | У����� | ֡���� | ���ش���
    {   //����֡��������ʱ����Ҫ�Ƚ����ݼĴ��������ݶ������������޷�����жϱ�־λ
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
 * Description:      ��ң�����ź�ת���ɶ����ҵ��������ת��ͨ��485���߷�����������������
 * Note:
 *
 ***************************************************************************/
void run_control(void)
{
	
	uint8_t *buf = com485_uart_buf.tx_buf;
	static uint32_t tx_time =0;
	if(sys_time -tx_time < 10)//ÿ��10���뷢��һ������
	{
		return;
	}
	tx_time = sys_time;
	Enable485TX() ;//ʹ��485����

	if(left_rpm == right_rpm)//���������������ٶ���ͬ����ʹ�ù㲥ͨ�ŵķ�ʽ��������
	{
		sync_control_flag = 1;
		
		buf[UART_DOWN_ID] = 0;						//Ϊ0��ʾ�����������������
		buf[UART_DOWN_CMD] = DRIVER_CMD_SPEED;
		buf[UART_DOWN_DATA1] = left_rpm&0xff;
		buf[UART_DOWN_DATA2] = (left_rpm>>8)&0xff;
		
		buf[UART_DOWN_SUM] = buf[UART_DOWN_ID] + buf[UART_DOWN_CMD] + buf[UART_DOWN_DATA1]+buf[UART_DOWN_DATA2];
	}
	else
	{
		sync_control_flag = 0;
		//���������������ٶȲ���ͬ�����Ⱥ�����������������ٶȿ�������
		if(!tx_index)
		{
			buf[UART_DOWN_ID] = 1;						//Ϊ1��ʾ���������
			buf[UART_DOWN_CMD] = DRIVER_CMD_SPEED;		//�ٶȿ�������
			buf[UART_DOWN_DATA1] = left_rpm&0xff;
			buf[UART_DOWN_DATA2] = (left_rpm>>8)&0xff;
			
			buf[UART_DOWN_SUM] = buf[UART_DOWN_ID] + buf[UART_DOWN_CMD] + buf[UART_DOWN_DATA1]+buf[UART_DOWN_DATA2];
		}
		else
		{
			buf[UART_DOWN_ID] = 2;						//Ϊ2��ʾ�ұ�������
			buf[UART_DOWN_CMD] = DRIVER_CMD_SPEED;		//�ٶȿ�������
			buf[UART_DOWN_DATA1] = right_rpm&0xff;
			buf[UART_DOWN_DATA2] = (right_rpm>>8)&0xff;
			
			buf[UART_DOWN_SUM] = buf[UART_DOWN_ID] + buf[UART_DOWN_CMD] + buf[UART_DOWN_DATA1]+buf[UART_DOWN_DATA2];
		}
		tx_index ^= 0x01;
	}
	com485_uart_buf.tx_counter =0;
	COM485_UART->CR1 |= UART_IT_TXE;//�����������ݷ���
}

