/***********************************************************************
 *
 * (c) Copyright 2001-2018, XZBOTS, All Rights Reserved.
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF XZBOTS, INC.
 * The copyright notice above does not evidence any actual or intended
 * publication of such source code.
 *
 *  Subsystem:   ����ģ��
 *  File:        common.c
 *  Author:      ��ҫ��
 *  Description: �����õı�������ʽת������
 *  Others:
 *  History:
 *
 *     1. Date:
 *        Author:
 *        Modification:
 *     2. ...
 *
 *********************************************************************/

#include "common.h"
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
//#include "sensor.h"
#include "wired.h"
#include "crc_check.h"

//��Ӧ10^0 ~ 10^9
const int32_t digit32[10] =
{
    1,
    1e+1,
    1e+2,
    1e+3,
    1e+4,
    1e+5,
    1e+6,
    1e+7,
    1e+8,
    1e+9,
};

/****************************************************************************
 *
 * Function Name:    asc_to_int32
 * Input:            data - ��ת������
 * Output:           None;
 * Returns:          void;
 * Description:      ��ASCII��ת����32λ�޷�����
 * Note:
 *
 ***************************************************************************/
int32_t asc_to_int32(char *data)
{
    uint8_t i = 0,k = 0;
    uint8_t neg_flag = 0;
    int32_t result = 0;

    //�ж���������
    if(data[i] == '-') //����Ϊ��
    {
        neg_flag = 1;
        i++;
    }

    //�õ�ʮ��������λ��
    do
    {
        if((data[i] >= '0') && (data[i] <= '9')) //Ϊ0~9����
        {
            i++;
        }
        else
        {
            break;
        }
    }
    while(i < 10);

    //����������Ϊ��ʱ��ʾ0
    if((!i) || (((i == 1) && neg_flag)))
    {
        return 0;
    }

    //��ʮ���Ƶ�ASCII��ת��32λ�޷�����
    for(k = neg_flag; k < i; ++k)
    {
        result += (data[k] - 0x30) * digit32[i - k - 1];
    }
    if(neg_flag)   //����Ϊ��
    {
        result = -result;
    }

    return result;
}




/****************************************************************************
 *
 * Function Name:    convert_byte_to_word
 * Input:            *byte_data - �ֽ�����;
 *					 *word_data - ˫������;
 *                   word_size - ˫�ֳ���.
 * Output:           None;
 * Returns:          None;
 * Description:      ��˫��ת�����ֽ�.
 * Note:
 *
 ***************************************************************************/
void convert_word_to_byte(uint32_t *word_data, uint8_t *byte_data, uint16_t word_size)
{
    uint16_t i;

    for(i = 0; i < word_size; ++i)
    {
        byte_data[i*4] = word_data[i]&0xff;
        byte_data[i*4 + 1] = (word_data[i] >> 8) & 0xff;
        byte_data[i*4 + 2] = (word_data[i] >> 16) & 0xff;
        byte_data[i*4 + 3] = (word_data[i] >> 24) & 0xff;
    }
}

/****************************************************************************
 *
 * Function Name:    convert_byte_to_word
 * Input:            *byte_data - �ֽ�����;
 *					 *word_data - ˫������;
 *                   word_size - ˫�ֳ���.
 * Output:           None;
 * Returns:          None;
 * Description:      ���ֽ�ת����˫��.
 * Note:
 *
 ***************************************************************************/
void convert_byte_to_word(uint8_t *byte_data, uint32_t *word_data,  uint16_t word_size)
{
    uint16_t i;

    for(i = 0; i < word_size; ++i)
    {
        word_data[i] = byte_data[i*4] + (byte_data[i*4 + 1] << 8) \
                       + (byte_data[i*4 + 2] << 16) + (byte_data[i*4 + 3] << 24);
    }
}

/****************************************************************************
 *
 * Function Name:    combine_array_data_or
 * Input:
 * Output:           result_data
 * Returns:          None;
 * Description:      ���ֽ�ת��Ϊ��
 * Note:
 *
 ***************************************************************************/
void combine_array_data_or(uint32_t *result_data, uint32_t *word_data1, uint32_t *word_data2, uint16_t word_size)
{
    uint16_t i;

    for(i = 0; i < word_size; ++i)
    {
        result_data[i] = word_data1[i]|word_data2[i];
    }
}

//
uint8_t check_array_data(uint32_t *word_data, uint16_t word_size)
{
    uint16_t i;

    for(i = 0; i < word_size; ++i)
    {
        if(word_data[i])
        {
            return 1;
        }
    }
    return 0;
}

/****************************************************************************
 *
 * Function Name:    window_filter
 * Input:            filter_data - ����Ϊ16������;
 *                   new_data - �������ݣ�
 * Output:           None;
 * Returns:          None;
 * Description:      ���ھ�ֵ�˲���Ĭ�ϴ��ڳ���Ϊ16.
 * Note:
 *
 ***************************************************************************/
int32_t window_filter(struct Window_Filter *filter_data, int32_t new_data)
{
    filter_data->acc = filter_data->acc + new_data - filter_data->data[filter_data->counter];
    filter_data->data[filter_data->counter++] = new_data;
    if(filter_data->max_counter)
    {
        filter_data->result = filter_data->acc/filter_data->max_counter;
        if(filter_data->counter >= filter_data->max_counter)
        {
            filter_data->counter = 0;
        }
    }
    else
    {
        filter_data->result = filter_data->acc>>4;
        filter_data->counter &= 0x0f;
    }
    return filter_data->result;
}


/****************************************************************************
 *
 * Function Name:    hysteresis_comparator
 * Input:            comparator - ���ͱȽ����ṹ��ָ��
 *                   new_data - �������ݣ�
 * Output:           None;
 * Returns:          None;
 * Description:      ����ʵ�ֳ��ͱȽ����Ĺ��ܣ�ʹ��ǰ���max_counter��high_value��low_value���г�ʼ��
 * Note:
 *
 ***************************************************************************/
uint8_t hysteresis_comparator(int32_t new_value, struct Hysteresis_Comparator *comparator)
{
    if(new_value >= comparator->high_value) {
        if(comparator->counter < comparator->max_counter)
        {
            comparator->counter++;

        }
        else
        {
            comparator->result = 1;
        }
    }
    else if(new_value <= comparator->low_value)
    {
        if(comparator->counter)
        {
            comparator->counter--;
        }
        else {
            comparator->result = 0;
        }

    }
    return comparator->result;
}

/****************************************************************************
 *
 * Function Name:    check_data_ingtegrity
 * Input:            data:��Ҫ���к�У������ݣ����һ���ֽ�����Ϊ��У����;length:���ݳ���
 * Output:           None;
 * Returns:          None;
 * Description:      ��У��
 * Note:
 *
 ***************************************************************************/
bool check_data_ingtegrity(uint8_t *data, uint8_t len)
{

    uint16_t result=0;

    result = crc16(data, len-2);
    if(result != ((data[len-1]<<8)|data[len-2]))
    {
        return false;
    }

    return true;
}

