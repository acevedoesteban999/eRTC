#pragma once

#include "eI2C.h"
#include "esp_log.h"
#include <time.h>
#include <sys/time.h>
#define ERTC_DEFAULT_SLAVE_ADDR 0x68

typedef struct{
   int seconds;
   int minutes;
   int hours;
   int day_of_week;    //1 Sunday
   int day_of_month;   
   int month;
   int year;
}ertc_data;


extern unsigned char ERTC_SLAVE_ADDR;

esp_err_t ertc_init();
void ertc_set_slave(unsigned char slave_addr);
bool ertc_get_data_os(ertc_data *rtc_time);
bool ertc_has_error();
void rtc_set_timedate_in_os();
bool ertc_set_time(ertc_data _ertc_data);
bool ertc_read(ertc_data*_ertc_data);