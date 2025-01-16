#include "eRTC.h"

uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

uint8_t bcd_to_decimal(uint8_t bcd) {
     return (bcd & 0x0F) + ((bcd >> 4) * 10);
}


esp_err_t ertc_init()
{
    esp_err_t err =  ei2c_master_init(I2C_SDA_PIN,I2C_SCL_PIN,I2C_PORT);
    // is_slave_active(SLAVE_ADDRESS_RTC);
    return err;
}
void rtc_set_timedate_in_os() {
    ertc_data rtc_time;
    ertc_read(&rtc_time);

    struct tm timeinfo = {
        .tm_sec = rtc_time.seconds,        
        .tm_min = rtc_time.minutes,       
        .tm_hour = rtc_time.hours,         
        .tm_mday = rtc_time.day_of_month, 
        .tm_mon = rtc_time.month - 1,      
        .tm_year = rtc_time.year + 100,    
        .tm_wday = rtc_time.day_of_week    
    };

    time_t now = mktime(&timeinfo);

    if (now == -1) {
        ESP_LOGE("","Can not set RTC data into OS: mktime ");
        return;
    }

    struct timeval tv = {.tv_sec = now, .tv_usec = 0};
    if (settimeofday(&tv, NULL) != 0)
        ESP_LOGE("","Can not set RTC data into OS: settimeofday ");
    else 
        ESP_LOGI("","Set RTC data into OS: %s ",asctime(&timeinfo));
}

//TODO is_slave_active in "ertc_init_master"
// bool is_slave_active(uint8_t slave_addr) {
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, true);
//     i2c_master_stop(cmd);
    
//     esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(1000));
//     i2c_cmd_link_delete(cmd);

//     return (ret == ESP_OK);
// }

bool ertc_set_time(ertc_data _ertc_data) {
    uint8_t data[7];

    // Convertir los valores decimales a BCD
    data[0] = decimal_to_bcd(_ertc_data.seconds);    // s 
    data[1] = decimal_to_bcd(_ertc_data.minutes);    // m 
    data[2] = decimal_to_bcd(_ertc_data.hours);      // h(24h)
    data[3] = decimal_to_bcd(_ertc_data.day_of_week);    // day (1 Sunday)
    data[4] = decimal_to_bcd(_ertc_data.day_of_month);   // Month day (1)
    data[5] = decimal_to_bcd(_ertc_data.month);      // M 
    data[6] = decimal_to_bcd(_ertc_data.year);       // Y

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == NULL) {
        return false;  // Fallo al crear el comando I2C
    }

    i2c_master_start(cmd);
    if (i2c_master_write_byte(cmd, (SLAVE_ADDRESS_RTC << 1) | I2C_MASTER_WRITE, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;  // Fallo al escribir la dirección del RTC
    }

    if (i2c_master_write_byte(cmd, 0x00, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;  // Fallo al apuntar al registro de segundos
    }

    if (i2c_master_write(cmd, data, sizeof(data), true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;  // Fallo al escribir los datos
    }

    i2c_master_stop(cmd);
    if (i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000)) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;  // Fallo en la ejecución del comando I2C
    }

    i2c_cmd_link_delete(cmd);  // Liberar recursos

    return true;  // Todo salió bien
}


bool ertc_read(ertc_data*_ertc_data){
    uint8_t data[7];
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    if(cmd == NULL)
        return false;
    
    i2c_master_start(cmd);
    if (i2c_master_write_byte(cmd, (SLAVE_ADDRESS_RTC << 1) | I2C_MASTER_WRITE, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;
    }

    if (i2c_master_write_byte(cmd, 0x00, true) != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;
    }

    i2c_master_start(cmd);
    if (i2c_master_write_byte(cmd, (SLAVE_ADDRESS_RTC << 1) | I2C_MASTER_READ, true) != ESP_OK) {
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
    if (i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000)) != ESP_OK) {
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

