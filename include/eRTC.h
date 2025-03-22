#pragma once

#include "eI2C.h"
#include "esp_log.h"
#include <time.h>
#include <sys/time.h>
#define ERTC_DEFAULT_SLAVE_ADDR 0x68
#define DEFAULT_ERTC_REG_CONTROL DEFAULT_ERTC_REG_CONTROL_ACTIVE
#define DEFAULT_ERTC_REG_CONTROL_ACTIVE 0b00010011 //osc 32.768 khz
#define DEFAULT_ERTC_REG_CONTROL_INACTIVE 0b00000000 //osc 32.768 khz
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
bool ertc_configure_control(char control);
bool ertc_has_error();
void rtc_set_timedate_in_os();
bool ertc_set_time(ertc_data ertc_data, uint8_t control_reg);
bool ertc_read(ertc_data *_ertc_data);

uint32_t ertc_data_to_int(const ertc_data *data);
