#include "rtc.h"

// ���·�����������0��ʹ�ã�
const uint8_t days_in_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// RTC��ʼ��
void RTC_Init(void) {
   // ����PWR��BKPʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    
    // ������ʺ�����
    PWR_BackupAccessCmd(ENABLE);
    
    // ����Ƿ�Ϊ�״�����
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A9) {
        // 1. ��λ��������
        BKP_DeInit();
        
        // 2. ����LSE�ⲿ���پ���
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
        
        // 3. ѡ��RTCʱ��Դ
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        
        // 4. �ȴ�RTC�Ĵ���ͬ��
        RTC_WaitForSynchro();
        
        // 5. ����Ԥ��Ƶ����32768/32768 = 1Hz
        RTC_SetPrescaler(32767);
        RTC_WaitForLastTask();
        
//        // 6. ���ó�ʼ����ʱ�䣨ʾ����2023-10-1 12:00:00 ������
//        RTC_SetCounter(946684800); // 2000-01-01 00:00:00��׼������
			

			DateTime init_dt = {0, 1, 10, 2, 8, 2025, 7}; // 2023-01-01 12:00:00 ����
			RTC_SetDateTime(&init_dt);
			
        RTC_WaitForLastTask();
        
        // ����ѳ�ʼ��
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A9);
    } else {
        // �ȴ�ͬ��
        RTC_WaitForSynchro();
    }
}

u32 iii;
// ��ȡ��ǰ����ʱ��
void RTC_GetDateTime(DateTime *dt) {
    uint32_t counter = RTC_GetCounter();
		iii = RTC_GetCounter();
    unix_to_datetime(counter, dt);
}

// ��������ʱ�� 1970��Ϊ��׼
void RTC_SetDateTime(DateTime *dt) {
    if (!is_valid_date(dt)) return;
    
    // 1970-01-01����ǰ���ڵ�����
    uint32_t days = 0;
    
    // ����1970�굽Ŀ����ݵ�����
    for (uint16_t y = 1970; y < dt->year; y++) {
        days += is_leap_year(y) ? 366 : 365;
    }
    
    // ����Ŀ�������1��1�յ�Ŀ���·ݵ�����
    for (uint8_t m = 1; m < dt->month; m++) {
        days += days_in_month[m];
        // ����2�¶��1��
        if (m == 2 && is_leap_year(dt->year)) {
            days++;
        }
    }
    
    // ���ϵ�������
    days += dt->day  ;
    
    // ���㵱��ʱ�䲿�ֵ�����
    uint32_t time_in_seconds = dt->seconds + 
                              dt->minutes * 60 + 
                              dt->hours * 3600;
    
    // ����������
    uint32_t total_seconds = time_in_seconds + days * 86400 ;
    
    // ����RTC������
    RTC_SetCounter(total_seconds);
    RTC_WaitForLastTask();
}



/******************
�������㷨�ļ�

********************/
 


// �����жϣ��������� return 1:run  0:gong
uint8_t is_leap_year(uint16_t year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

// �������ڼ���Zeller��ʽ��
uint8_t calculate_weekday(uint16_t year, uint8_t month, uint8_t day) {
    if (month < 3) {
        month += 12;
        year -= 1;
    }
    uint16_t c = year / 100;
    uint16_t y = year % 100;
    uint8_t weekday = (day + (13 * (month + 1)) / 5 + y + y / 4 + c / 4 + 5 * c) % 7;
    return (weekday + 5) % 7 + 1; // ת�����Ϊ1=��һ ... 7=����
}

// ���ںϷ�����֤
uint8_t is_valid_date(DateTime *dt) {
    if (dt->month > 12 || dt->month == 0) return 0;
    if (dt->day == 0) return 0;
    
    uint8_t days = days_in_month[dt->month];
    if (dt->month == 2 && is_leap_year(dt->year)) 
        days = 29;
    
    return dt->day <= days;
}


// ��Unixʱ���ת��������ʱ��
void unix_to_datetime(uint32_t timestamp, DateTime *dt) {
    // ����ʱ�䲿��
    dt->seconds = timestamp % 60;
    timestamp /= 60;
    dt->minutes = timestamp % 60;
    timestamp /= 60;
    dt->hours = timestamp % 24;
    timestamp /= 24;
    
    // �������ڲ��֣�����1970-01-01��
    uint16_t days = (uint16_t)timestamp;
    uint16_t year = 1970;
    uint8_t month;
    
    while (days > (is_leap_year(year) ? 366 : 365)) {
        days -= is_leap_year(year) ? 366 : 365;
        year++;
    }
    
    dt->year = year;
    uint8_t leap = is_leap_year(year);
    
    for (month = 1; month <= 12; month++) {
        uint8_t dim = days_in_month[month];
        if (month == 2 && leap) dim++;
        
        if (days <= dim) break;
        days -= dim;
    }
    
    dt->month = month;
    dt->day = days;
    dt->weekday = calculate_weekday(year, month, days);
}

























