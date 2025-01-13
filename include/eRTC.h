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
}ertc_data;

esp_err_t ertc_init();
void ertc_set_time(ertc_data _ertc_data);
ertc_data ertc_read();