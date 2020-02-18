#ifndef __common_H
#define __common_H
#include "stm32f1xx_hal.h"

typedef enum {
    false =0,
    true=1,
} bool;

#define abs_value(x) ((x >=0)?x:-x)	//求x绝对值.

//判断digit的有效性，范围[min,max].
#define is_digit_valid(digit, min, max) (((((int32_t)digit)>=min)&&(((int32_t)digit)<=max))?true:false)

#define CHAR_TO_CMD_INDEX(a, b)  ((a-0x30)*10 + (b-0x30))

#define CLEAR_DATA_BIT(data, bit_num) do{data &=~(1<<bit_num);}while(0)
#define SET_DATA_BIT(data, bit_num) do{data |=(1<<bit_num);}while(0)
#define GET_DATA_BIT(data, bit_num) ((data & (1<<bit_num))!=0)


struct Window_Filter
{
    uint16_t max_counter;
    uint16_t counter;
    int32_t data[16];
    int32_t acc;
    int32_t result;
};

//用于实现类似迟滞比较器的功能
struct Hysteresis_Comparator
{
    uint8_t counter;
    uint8_t max_counter;

    int32_t high_value;
    int32_t low_value;

    uint8_t result;
};




int32_t asc_to_int32(char *data);
void convert_word_to_byte(uint32_t *word_data, uint8_t *byte_data, uint16_t word_size);
void convert_byte_to_word(uint8_t *byte_data, uint32_t *word_data, uint16_t word_size);
void combine_array_data_or(uint32_t *result_data, uint32_t *word_data1, uint32_t *word_data2, uint16_t word_size);
uint8_t check_array_data(uint32_t *word_data, uint16_t word_size);
int32_t window_filter(struct Window_Filter *filter_data, int32_t new_data);
uint8_t hysteresis_comparator(int32_t new_value, struct Hysteresis_Comparator *comparator);
bool check_data_ingtegrity(uint8_t *data, uint8_t len);
#endif
