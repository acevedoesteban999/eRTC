#include "eRTC.h"


unsigned char ERTC_SLAVE_ADDR = ERTC_DEFAULT_SLAVE_ADDR;
int ertc_error = -1;
uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

uint8_t bcd_to_decimal(uint8_t bcd) {
     return (bcd & 0x0F) + ((bcd >> 4) * 10);
}


esp_err_t ertc_init()
{
    esp_err_t err =  ei2c_master_init();
    return err;
}


void ertc_set_slave(unsigned char slave_addr){
    ERTC_SLAVE_ADDR = slave_addr;
}

bool ertc_get_data_os(ertc_data*rtc_time) {
    
    struct timeval tv;
    struct tm *timeinfo;

    gettimeofday(&tv, NULL);
    timeinfo = localtime(&tv.tv_sec);

    rtc_time->year = timeinfo->tm_year;
    rtc_time->month = timeinfo->tm_mon;
    rtc_time->day_of_month = timeinfo->tm_mday;
    rtc_time->hours = timeinfo->tm_hour;
    rtc_time->minutes = timeinfo->tm_min;
    rtc_time->seconds = timeinfo->tm_sec;
    
    return !ertc_has_error();
}

bool ertc_has_error(){
    return ertc_error != 0;
}

 
void _rtc_set_timedate_in_os(ertc_data rtc_time) {
    struct timeval tv;
    struct tm tm = {
        .tm_year = rtc_time.year - 1900, 
        .tm_mon = rtc_time.month,           
        .tm_mday = rtc_time.day_of_month,
        .tm_hour = rtc_time.hours,          
        .tm_min = rtc_time.minutes,           
        .tm_sec = rtc_time.seconds             
    };
    tv.tv_sec = mktime(&tm); 
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);

}

void rtc_set_timedate_in_os() {
    ertc_data rtc_time;
    if(ertc_read(&rtc_time)){
        _rtc_set_timedate_in_os(rtc_time);
        ertc_error = 0;
    }else{
        ertc_error = 1;
    }

}



bool ertc_set_time(ertc_data _ertc_data) {
    uint8_t data[7];

    
    data[0] = decimal_to_bcd(_ertc_data.seconds);    // s 
    data[1] = decimal_to_bcd(_ertc_data.minutes);    // m 
    data[2] = decimal_to_bcd(_ertc_data.hours);      // h(24h)
    data[3] = decimal_to_bcd(_ertc_data.day_of_week);    // day (1 Sunday)
    data[4] = decimal_to_bcd(_ertc_data.day_of_month);   // Month day (1)
    data[5] = decimal_to_bcd(_ertc_data.month);      // M 
    data[6] = decimal_to_bcd(_ertc_data.year);       // Y

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == NULL) {
        return false; 
    }

    i2c_master_start(cmd);
    if (i2c_master_write_byte(cmd, (ERTC_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;  
    }

    if (i2c_master_write_byte(cmd, 0x00, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;  
    }

    if (i2c_master_write(cmd, data, sizeof(data), true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;  
    }

    i2c_master_stop(cmd);
    if (i2c_master_cmd_begin(EI2C_GPIO.PORT, cmd, pdMS_TO_TICKS(1000)) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false; 
    }

    i2c_cmd_link_delete(cmd); 
    _rtc_set_timedate_in_os(_ertc_data);
    return true;  
}

bool ertc_read(ertc_data*_ertc_data){
    uint8_t data[7];
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    if(cmd == NULL)
        return false;
    
    i2c_master_start(cmd);
    if (i2c_master_write_byte(cmd, (ERTC_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;
    }

    if (i2c_master_write_byte(cmd, 0x00, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;
    }

    i2c_master_start(cmd);
    if (i2c_master_write_byte(cmd, (ERTC_SLAVE_ADDR << 1) | I2C_MASTER_READ, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;
    }

    // Read  7 bytes
    if (i2c_master_read(cmd, data, 6, I2C_MASTER_ACK) != ESP_OK ||
        i2c_master_read_byte(cmd, data + 6, I2C_MASTER_NACK) != ESP_OK) {
        i2c_master_stop(cmd);
        i2c_cmd_link_delete(cmd);
        return false;
    }

    i2c_master_stop(cmd);
    if (i2c_master_cmd_begin(EI2C_GPIO.PORT, cmd, pdMS_TO_TICKS(1000)) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;
    }

    i2c_cmd_link_delete(cmd);

    _ertc_data->seconds = bcd_to_decimal(data[0] & 0x7F);
    _ertc_data->minutes = bcd_to_decimal(data[1]);
    _ertc_data->hours = bcd_to_decimal(data[2] & 0x3F);  // Para formato 24 horas
    _ertc_data->day_of_week = bcd_to_decimal(data[3]);
    _ertc_data->day_of_month = bcd_to_decimal(data[4]);
    _ertc_data->month = bcd_to_decimal(data[5] & 0x1F);
    _ertc_data->year = bcd_to_decimal(data[6]);


    return true;
}

