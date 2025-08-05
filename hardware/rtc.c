#include "rtc.h"

// 各月份天数表（索引0不使用）
const uint8_t days_in_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// RTC初始化
void RTC_Init(void) {
   // 启用PWR和BKP时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    
    // 允许访问后备区域
    PWR_BackupAccessCmd(ENABLE);
    
    // 检查是否为首次配置
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A9) {
        // 1. 复位备份区域
        BKP_DeInit();
        
        // 2. 启用LSE外部低速晶振
        RCC_LSEConfig(RCC_LSE_ON);
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
        
        // 3. 选择RTC时钟源
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        
        // 4. 等待RTC寄存器同步
        RTC_WaitForSynchro();
        
        // 5. 设置预分频器：32768/32768 = 1Hz
        RTC_SetPrescaler(32767);
        RTC_WaitForLastTask();
        
//        // 6. 设置初始日期时间（示例：2023-10-1 12:00:00 周六）
//        RTC_SetCounter(946684800); // 2000-01-01 00:00:00基准的秒数
			

			DateTime init_dt = {0, 1, 10, 2, 8, 2025, 7}; // 2023-01-01 12:00:00 周日
			RTC_SetDateTime(&init_dt);
			
        RTC_WaitForLastTask();
        
        // 标记已初始化
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A9);
    } else {
        // 等待同步
        RTC_WaitForSynchro();
    }
}

u32 iii;
// 获取当前日期时间
void RTC_GetDateTime(DateTime *dt) {
    uint32_t counter = RTC_GetCounter();
		iii = RTC_GetCounter();
    unix_to_datetime(counter, dt);
}

// 设置日期时间 1970作为基准
void RTC_SetDateTime(DateTime *dt) {
    if (!is_valid_date(dt)) return;
    
    // 1970-01-01到当前日期的天数
    uint32_t days = 0;
    
    // 计算1970年到目标年份的天数
    for (uint16_t y = 1970; y < dt->year; y++) {
        days += is_leap_year(y) ? 366 : 365;
    }
    
    // 计算目标年份中1月1日到目标月份的天数
    for (uint8_t m = 1; m < dt->month; m++) {
        days += days_in_month[m];
        // 闰年2月多加1天
        if (m == 2 && is_leap_year(dt->year)) {
            days++;
        }
    }
    
    // 加上当月天数
    days += dt->day  ;
    
    // 计算当天时间部分的秒数
    uint32_t time_in_seconds = dt->seconds + 
                              dt->minutes * 60 + 
                              dt->hours * 3600;
    
    // 总秒数计算
    uint32_t total_seconds = time_in_seconds + days * 86400 ;
    
    // 更新RTC计数器
    RTC_SetCounter(total_seconds);
    RTC_WaitForLastTask();
}



/******************
万年历算法文件

********************/
 


// 闰年判断（公历规则） return 1:run  0:gong
uint8_t is_leap_year(uint16_t year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

// 计算星期几（Zeller公式）
uint8_t calculate_weekday(uint16_t year, uint8_t month, uint8_t day) {
    if (month < 3) {
        month += 12;
        year -= 1;
    }
    uint16_t c = year / 100;
    uint16_t y = year % 100;
    uint8_t weekday = (day + (13 * (month + 1)) / 5 + y + y / 4 + c / 4 + 5 * c) % 7;
    return (weekday + 5) % 7 + 1; // 转换结果为1=周一 ... 7=周日
}

// 日期合法性验证
uint8_t is_valid_date(DateTime *dt) {
    if (dt->month > 12 || dt->month == 0) return 0;
    if (dt->day == 0) return 0;
    
    uint8_t days = days_in_month[dt->month];
    if (dt->month == 2 && is_leap_year(dt->year)) 
        days = 29;
    
    return dt->day <= days;
}


// 从Unix时间戳转换到日期时间
void unix_to_datetime(uint32_t timestamp, DateTime *dt) {
    // 计算时间部分
    dt->seconds = timestamp % 60;
    timestamp /= 60;
    dt->minutes = timestamp % 60;
    timestamp /= 60;
    dt->hours = timestamp % 24;
    timestamp /= 24;
    
    // 计算日期部分（基于1970-01-01）
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

























