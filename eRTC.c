#include "eRTC.h"



uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

uint8_t bcd_to_decimal(uint8_t bcd) {
     return (bcd & 0x0F) + ((bcd >> 4) * 10);
}


esp_err_t rtc_init()
{
    return i2c_master_init(I2C_SDA_PIN,I2C_SCL_PIN,I2C_PORT);
}

void rtc_set_time() {
    uint8_t data[7];

    data[0] = decimal_to_bcd(0);    // s 
    data[1] = decimal_to_bcd(0);    // m 
    data[2] = decimal_to_bcd(21);   // h(24h)
    data[3] = decimal_to_bcd(2);    // day (1 Sunday)
    data[4] = decimal_to_bcd(21);   // Month day (1)
    data[5] = decimal_to_bcd(10);   // M 
    data[6] = decimal_to_bcd(24);   // Y

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLAVE_ADDRESS_RTC << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);             // Apuntar al registro de segundos
    i2c_master_write(cmd, data, sizeof(data), true);    // Escribir los datos
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
}

rtc_data rtc_read(){
    uint8_t data[7];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Comenzar una transmisión I2C
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLAVE_ADDRESS_RTC << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);  // Apuntar al registro de segundos
    i2c_master_start(cmd);  // Repetir el comienzo para leer
    i2c_master_write_byte(cmd, (SLAVE_ADDRESS_RTC << 1) | I2C_MASTER_READ, true);

    // Leer los 7 bytes que contienen la hora, minuto, segundo, día, fecha, mes y año
    i2c_master_read(cmd, data, 6, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, data + 6, I2C_MASTER_NACK);  // Leer el último byte
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    // Convertir los valores de BCD a decimal
    rtc_data _rtc_data;
    _rtc_data.seconds = bcd_to_decimal(data[0] & 0x7F);
    _rtc_data.minutes = bcd_to_decimal(data[1]);
    _rtc_data.hours = bcd_to_decimal(data[2] & 0x3F);  // Para formato 24 horas
    _rtc_data.day_of_week = bcd_to_decimal(data[3]);
    _rtc_data.day_of_month = bcd_to_decimal(data[4]);
    _rtc_data.month = bcd_to_decimal(data[5] & 0x1F);
    _rtc_data.year = bcd_to_decimal(data[6]); 

    return _rtc_data;
}

