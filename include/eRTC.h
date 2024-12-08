#pragma once

#include "eI2C.h"
#include "esp_log.h"


#define I2C_PORT I2C_NUM_0              
#define I2C_SCL_PIN 22                  
#define I2C_SDA_PIN 21                     
#define SLAVE_ADDRESS_RTC 0x68 

typedef struct{
   uint8_t seconds;
   uint8_t minutes;
   uint8_t hours;
   uint8_t day_of_week;
   uint8_t day_of_month;
   uint8_t month;
   uint8_t year;
}rtc_data;

extern char BufferRtcI2C[25];

esp_err_t rtc_init();
void rtc_set_time();
rtc_data rtc_read();