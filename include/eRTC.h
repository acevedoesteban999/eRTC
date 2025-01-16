#pragma once

#include "eI2C.h"
#include "esp_log.h"
#include <time.h>
#include <sys/time.h>


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
}ertc_data;

esp_err_t ertc_init();
void rtc_set_timedate_in_os();
bool ertc_set_time(ertc_data _ertc_data);
bool ertc_read(ertc_data*_ertc_data);