//  ***************************************************************************
/// @file    i2c.h
/// @author  NeoProg
/// @brief   I2C bus driver
//  ***************************************************************************
#ifndef I2C_H_
#define I2C_H_


typedef enum {
	I2C_SPEED_100KHZ = 0x1D0D0,
	I2C_SPEED_400KHZ = 0x6565
} i2c_speed_t;

typedef enum {
	I2C_DRIVER_NO_ERROR = 0,
	I2C_DRIVER_BUSY,
	I2C_DRIVER_ERROR
} i2c_status_t;


void         i2c_init(i2c_speed_t speed);
void         i2c_set_internal_address_length(uint32_t length);
i2c_status_t i2c_get_status(void);
void         i2c_force_reset_error_status(void);

bool         i2c_write_byte(uint8_t dev_addr, uint32_t internal_addr, uint8_t data);
bool         i2c_write_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t* data, uint32_t size);
bool         i2c_async_write_bytes(uint8_t dev_addr, uint32_t internal_addr, uint32_t size);
bool		 i2c_write_bits(uint8_t dev_addr, uint8_t internal_addr, uint8_t mask, uint8_t bits);
uint8_t*     i2c_async_get_tx_buffer_address(void);

bool         i2c_read_byte(uint8_t dev_addr, uint32_t internal_addr, uint8_t* data);
bool         i2c_read_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t* data, uint32_t size);
bool         i2c_async_read_bytes(uint8_t dev_addr, uint32_t internal_addr, uint32_t size);
uint8_t*     i2c_async_get_rx_buffer_address(void);


#endif // I2C_H_