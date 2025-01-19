#pragma once

#include "eI2C.h"
#include "esp_log.h"
#include <time.h>
#include <sys/time.h>
#define ERTC_DEFAULT_SLAVE_ADDR 0x68

typedef struct{
   unsigned seconds;
   unsigned minutes;
   unsigned hours;
   unsigned day_of_week;    //1 Sunday
   unsigned day_of_month;   
   unsigned month;
   unsigned year;
}ertc_data;


extern unsigned char ERTC_SLAVE_ADDR;

esp_err_t ertc_init();
void ertc_set_slave(unsigned char slave_addr);
void rtc_set_timedate_in_os();
bool ertc_set_time(ertc_data _ertc_data);
bool ertc_read(ertc_data*_ertc_data);