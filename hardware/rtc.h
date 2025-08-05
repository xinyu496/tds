#ifndef _RTC_H
#define _RTC_H

#include "stm32f10x.h"

//extern const uint8_t days_in_month[13];

// 日期时间结构体
typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t weekday;
} DateTime;





uint8_t calculate_weekday(uint16_t year, uint8_t month, uint8_t day);// 计算星期几（Zeller公式）
uint8_t is_valid_date(DateTime *dt);// 日期合法性验证
void unix_to_datetime(uint32_t timestamp, DateTime *dt) ;// 从Unix时间戳转换到日期时间

uint8_t is_leap_year(uint16_t year);// 闰年判断（公历规则）return 1:run  0:gong



void RTC_Init(void) ;// RTC初始化

void RTC_GetDateTime(DateTime *dt);// 获取当前日期时间


void RTC_SetDateTime(DateTime *dt);// 设置日期时间


















#endif





