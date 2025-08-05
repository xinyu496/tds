#ifndef _RTC_H
#define _RTC_H

#include "stm32f10x.h"

//extern const uint8_t days_in_month[13];

// ����ʱ��ṹ��
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t weekday;
} DateTime;





uint8_t calculate_weekday(uint16_t year, uint8_t month, uint8_t day);// �������ڼ���Zeller��ʽ��
uint8_t is_valid_date(DateTime *dt);// ���ںϷ�����֤
void unix_to_datetime(uint32_t timestamp, DateTime *dt) ;// ��Unixʱ���ת��������ʱ��

uint8_t is_leap_year(uint16_t year);// �����жϣ���������return 1:run  0:gong



void RTC_Init(void) ;// RTC��ʼ��

void RTC_GetDateTime(DateTime *dt);// ��ȡ��ǰ����ʱ��


void RTC_SetDateTime(DateTime *dt);// ��������ʱ��


















#endif





