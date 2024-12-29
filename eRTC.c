#include "eRTC.h"

uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

uint8_t bcd_to_decimal(uint8_t bcd) {
     return (bcd & 0x0F) + ((bcd >> 4) * 10);
}


esp_err_t ertc_init_master()
{
    esp_err_t err =  ei2c_master_init(I2C_SDA_PIN,I2C_SCL_PIN,I2C_PORT);
    // is_slave_active(SLAVE_ADDRESS_RTC);
    return err;
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

void ertc_set_time(ertc_data _ertc_data) {
    uint8_t data[7];

    data[0] = decimal_to_bcd(_ertc_data.seconds);    // s 
    data[1] = decimal_to_bcd(_ertc_data.minutes);    // m 
    data[2] = decimal_to_bcd(_ertc_data.hours);   // h(24h)
    data[3] = decimal_to_bcd(_ertc_data.day_of_week);    // day (1 Sunday)
    data[4] = decimal_to_bcd(_ertc_data.day_of_month);   // Month day (1)
    data[5] = decimal_to_bcd(_ertc_data.month);   // M 
    data[6] = decimal_to_bcd(_ertc_data.year);   // Y

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLAVE_ADDRESS_RTC << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true);             // Apuntar al registro de segundos
    i2c_master_write(cmd, data, sizeof(data), true);    // Escribir los datos
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
}

ertc_data ertc_read(){
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
    ertc_data _ertc_data;
    _ertc_data.seconds = bcd_to_decimal(data[0] & 0x7F);
    _ertc_data.minutes = bcd_to_decimal(data[1]);
    _ertc_data.hours = bcd_to_decimal(data[2] & 0x3F);  // Para formato 24 horas
    _ertc_data.day_of_week = bcd_to_decimal(data[3]);
    _ertc_data.day_of_month = bcd_to_decimal(data[4]);
    _ertc_data.month = bcd_to_decimal(data[5] & 0x1F);
    _ertc_data.year = bcd_to_decimal(data[6]); 

    return _ertc_data;
}

