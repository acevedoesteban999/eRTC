#pragma once

#include "eI2C.h"
#include "esp_log.h"


#define I2C_PORT I2C_NUM_0              
#define I2C_SCL_PIN 22                  
#define I2C_SDA_PIN 21                     
#define SLAVE_ADDRESS_RTC 0x68 

typedef struct{
   unsigned seconds;
   unsigned minutes;
   unsigned hours;
   unsigned day_of_week;    //1 Sunday
   unsigned day_of_month;   
   unsigned month;
   unsigned year;
}rtc_data;

extern char BufferRtcI2C[25];

esp_err_t rtc_init_master();
void rtc_set_time(rtc_data _rtc_data);
rtc_data rtc_read();