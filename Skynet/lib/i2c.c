//  ***************************************************************************
/// @file    i2c.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include <string.h>
#include "i2c.h"
#include "systimer.h"
#define IS_BIT_SET(_status, _bit)			(((_status) & (_bit)) == (_bit))
#define SDA_PIN								(PIO_PB12)
#define SCK_PIN								(PIO_PB13)
#define MAX_BUFFER_SIZE						(64)
#define I2C_TIMEOUT_MS						(5)


static uint8_t tx_buffer[MAX_BUFFER_SIZE] = { 0 };
static uint8_t rx_buffer[MAX_BUFFER_SIZE] = { 0 };

static uint32_t internal_address_length = 1;
static uint32_t start_operation_time = 0;
static volatile i2c_status_t driver_status = I2C_DRIVER_NO_ERROR;


static void stop_communication(void);


//  ***************************************************************************
/// @brief	Initialize GPIO and I2C registers in Master Mode
/// @param	clock_speed: I2C_SPEED_100KHZ or I2C_SPEED_400KHZ constants
/// @return	none
//  ***************************************************************************
void i2c_init(i2c_speed_t speed) {
	
	// Enable PDC clock
	REG_PMC_PCER1 = PMC_PCER1_PID39;
	while ((REG_PMC_PCSR1 & PMC_PCER1_PID39) == 0);

	// Enable I2C clock
	REG_PMC_PCER0 = PMC_PCER0_PID23;
	while ((REG_PMC_PCSR0 & PMC_PCER0_PID23) == 0);

	// Disable and reset I2C controller
	REG_TWI1_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS; // Disable PDC
	REG_TWI1_CR = TWI_CR_SWRST | TWI_CR_SVDIS | TWI_CR_MSDIS;
	
	// Send 9 pulses on SCL
	REG_PIOB_PER = SCK_PIN;
	REG_PIOB_OER = SCK_PIN;
	for (uint32_t i = 0; i < 9; ++i) {
		
		REG_PIOB_CODR = SCK_PIN;
		delay_ms(1);
		REG_PIOB_SODR = SCK_PIN;
		delay_ms(1);
	}

	// Configure SCK and SDA as A peripheral function
	REG_PIOB_PDR = SDA_PIN | SCK_PIN;					// Disable PIO

	// Configure I2C controller
	REG_TWI1_RHR; // Reset holding register
	REG_TWI1_CWGR = speed;
	REG_TWI1_CR = TWI_CR_MSEN | TWI_CR_SVDIS; 
	
	REG_TWI1_IDR = 0xFFFFFFFF;
	NVIC_EnableIRQ(TWI1_IRQn);
}

//  ***************************************************************************
/// @brief	Set internal address length
/// @param	length: 0 - no internal address, [1,2,3] byte address length
/// @return	none
//  ***************************************************************************
void i2c_set_internal_address_length(uint32_t length) {
	internal_address_length = length;
}

//  ***************************************************************************
/// @brief	Get current driver state
/// @note	This function update driver status
/// @return	Current driver status
//  ***************************************************************************
i2c_status_t i2c_get_status() {
	
	if (driver_status == I2C_DRIVER_BUSY) {
		
		if (get_time_ms() - start_operation_time > I2C_TIMEOUT_MS) {
			stop_communication();
			driver_status = I2C_DRIVER_ERROR;
		}
	}
	
	return driver_status;
}

//  ***************************************************************************
/// @brief	Reset error state
/// @return	none
//  ***************************************************************************
void i2c_force_reset_error_status() {

	if (driver_status == I2C_DRIVER_ERROR) {
		driver_status = I2C_DRIVER_NO_ERROR;
	}
}



//  ***************************************************************************
/// @brief	Synchronous write byte
/// @note	This wrappers for asynchronous mode functions
/// @param	dev_addr: I2C device address
/// @param	internal_addr: internal register address on I2C device
/// @param	data: byte for write
/// @return	true - no error, false - error
//  ***************************************************************************
bool i2c_write_byte(uint8_t dev_addr, uint32_t internal_addr, uint8_t data) {
	
	return i2c_write_bytes(dev_addr, internal_addr, &data, 1);
}

//  ***************************************************************************
/// @brief	Synchronous write data
/// @note	This wrappers for asynchronous mode functions
/// @param	dev_addr: I2C device address
/// @param	internal_addr: internal register address on I2C device
/// @param	data: data for write
/// @return	true - no error, false - error
//  ***************************************************************************
bool i2c_write_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t* data, uint32_t size) {

	// Disable all current I2C communications
	stop_communication();
	driver_status = I2C_DRIVER_NO_ERROR;

	// Copy data to transmit buffer
	memcpy(tx_buffer, data, size);

	// Start send data and wait complete communication
	i2c_async_write_bytes(dev_addr, internal_addr, size);
	while (i2c_get_status() == I2C_DRIVER_BUSY);

	return (driver_status == I2C_DRIVER_NO_ERROR);
}

//  ***************************************************************************
/// @brief	Start asynchronous write data
/// @note	This functions use PDC
/// @param	dev_addr: I2C device address
/// @param	internal_addr: internal register address on I2C device
/// @param	size: byte for write
/// @retval	true - operation start success, false - operation start fail
//  ***************************************************************************
bool i2c_async_write_bytes(uint8_t dev_addr, uint32_t internal_addr, uint32_t size) {

	// Check driver state and timeout operation
	if (driver_status == I2C_DRIVER_BUSY) {
		return false;
	}

	// Disable all I2C interrupts
	REG_TWI1_IDR = 0xFFFFFFFF;

	// Configure TX PDC channel
	REG_TWI1_TPR = (uint32_t)tx_buffer;
	REG_TWI1_TCR = size;

	// Configure Master mode (DADR | write mode | internal address length)
	REG_TWI1_MMR = (dev_addr << 16) | (0 << 12) | (internal_address_length << 8);
	REG_TWI1_IADR = internal_addr;

	// Enable transmitter
	REG_TWI1_PTCR = TWI_PTCR_TXTEN | TWI_PTCR_RXTDIS;

	// Configure interrupts
	REG_TWI1_IER = TWI_IER_ENDTX | TWI_IER_NACK;

	driver_status = I2C_DRIVER_BUSY;
	start_operation_time = get_time_ms();
	return true;
}

//  ***************************************************************************
/// @brief	Write bits to register
/// @param	dev_addr: I2C device address
/// @param	reg_addr: internal register address on I2C device
/// @param	mask: bits mask
/// @param	bits: bits for written
/// @param	timeout: timeout [ms]
/// @retval	true - no error, false - error
//  ***************************************************************************
bool i2c_write_bits(uint8_t dev_addr, uint8_t internal_addr, uint8_t mask, uint8_t bits) {

	uint8_t reg_data = 0;
	if (i2c_read_byte(dev_addr, internal_addr, &reg_data) == false) {
		return false;
	}

	reg_data &= ~mask;	// Clear bits
	reg_data |= bits;	// Write new bits

	return i2c_write_byte(dev_addr, internal_addr, reg_data);
}

//  ***************************************************************************
/// @brief  Get TX buffer address
/// @return	Buffer address
//  ***************************************************************************
uint8_t* i2c_async_get_tx_buffer_address(void) {
	
	return tx_buffer;
}



//
// RX
//
//  ***************************************************************************
/// @brief	Synchronous read byte
/// @note	This wrappers for asynchronous mode functions
/// @param	dev_addr: I2C device address
/// @param	internal_addr: internal register address on I2C device
/// @param	data: pointer to receive buffer
/// @retval	Byte value
//  ***************************************************************************
bool i2c_read_byte(uint8_t dev_addr, uint32_t internal_addr, uint8_t* data) {
	
	return i2c_read_bytes(dev_addr, internal_addr, data, 1);
}

//  ***************************************************************************
/// @brief	Synchronous read data
/// @note	This wrappers for asynchronous mode functions
/// @param	dev_addr: I2C device address
/// @param	internal_addr: internal register address on I2C device
/// @param	data: pointer to receive buffer
/// @size	data: bytes count for read
/// @retval	true - no error, false - error
//  ***************************************************************************
bool i2c_read_bytes(uint8_t dev_addr, uint32_t internal_addr, uint8_t* data, uint32_t size) {

	// Disable all current I2C communications
	stop_communication();
	driver_status = I2C_DRIVER_NO_ERROR;

	i2c_async_read_bytes(dev_addr, internal_addr, size);
	while (i2c_get_status() == I2C_DRIVER_BUSY);

	// Check driver status
	if (driver_status == I2C_DRIVER_ERROR) {
		return false;
	}

	// Copy received data
	memcpy(data, rx_buffer, size);
	return true;
}

//  ***************************************************************************
/// @brief	Start asynchronous read data
/// @note	This functions using PDC
/// @param	dev_addr: I2C device address
/// @param	internal_addr: internal register address on I2C device
/// @param	data: pointer to receive buffer
/// @size	data: bytes count for read
/// @retval	true - operation start success, false - operation start fail
//  ***************************************************************************
bool i2c_async_read_bytes(uint8_t dev_addr, uint32_t internal_addr, uint32_t size) {

	// Check driver state
	if (driver_status == I2C_DRIVER_BUSY) {
		return false;
	}

	// Disable all I2C interrupts
	REG_TWI1_IDR = 0xFFFFFFFF;

	// Configure RX PDC channel
	REG_TWI1_RPR = (uint32_t)rx_buffer;
	REG_TWI1_RCR = size - 1; // Without last byte (for send STOP)

	// Configure Master mode (DADR | read mode | internal address length)
	REG_TWI1_MMR = (dev_addr << 16) | TWI_MMR_MREAD | (internal_address_length << 8);
	REG_TWI1_IADR = internal_addr;

	// Enable receiver
	REG_TWI1_PTCR = TWI_PTCR_TXTDIS | TWI_PTCR_RXTEN;

	// Send START
	REG_TWI1_CR = TWI_CR_START;

	// Configure interrupts
	REG_TWI1_IER = TWI_IER_ENDRX | TWI_IER_NACK;

	driver_status = I2C_DRIVER_BUSY;
	start_operation_time = get_time_ms();
	return true;
}

//  ***************************************************************************
/// @brief  Get RX buffer address
/// @return	Buffer address
//  ***************************************************************************
uint8_t* i2c_async_get_rx_buffer_address(void) {
	return rx_buffer;
}



//  ***************************************************************************
/// @brief  Stop communication
/// @param	None
/// @return	None
//  ***************************************************************************
static void stop_communication(void) {

	// Disable PDC
	REG_TWI1_PTCR = TWI_PTCR_TXTDIS | TWI_PTCR_RXTDIS;
	REG_TWI1_RCR = 0;
	REG_TWI1_TCR = 0;

	// Disable all I2C interrupts
	REG_TWI1_IDR = 0xFFFFFFFF;

	// Reset status register
	REG_TWI1_SR;
}


//  ***************************************************************************
/// @brief  I2C bus ISR
/// @param	None
/// @return	None
//  ***************************************************************************
void TWI1_Handler(void) {

	uint32_t status = REG_TWI1_SR;
	uint32_t irq_mask = REG_TWI1_IMR;

	// Errors
	if (IS_BIT_SET(status, TWI_SR_NACK) && IS_BIT_SET(irq_mask, TWI_IMR_NACK)) {
		stop_communication();
		driver_status = I2C_DRIVER_ERROR;
	}
	
	// Send STOP complete
	else if (IS_BIT_SET(status, TWI_SR_TXCOMP) && IS_BIT_SET(irq_mask, TWI_IMR_TXCOMP)) {
		stop_communication();
		driver_status = I2C_DRIVER_NO_ERROR;
	}

	// TX handler
	else if (IS_BIT_SET(status, TWI_SR_ENDTX) && IS_BIT_SET(irq_mask, TWI_IMR_ENDTX)) {

		// Disable TX PDC channel
		REG_TWI1_PTCR = TWI_PTCR_TXTDIS;

		// Send STOP
		REG_TWI1_CR = TWI_CR_STOP;

		// Configure interrupts
		REG_TWI1_IDR = 0xFFFFFFFF;
		REG_TWI1_IER = TWI_IER_TXCOMP;
	}

	// RX handler
	else if (IS_BIT_SET(status, TWI_SR_ENDRX) && IS_BIT_SET(irq_mask, TWI_IMR_ENDRX)) {

		// Disable RX PDC channel
		REG_TWI1_PTCR = TWI_PTCR_RXTDIS;

		// Send STOP
		REG_TWI1_CR = TWI_CR_STOP;

		// Configure interrupts
		REG_TWI1_IDR = 0xFFFFFFFF;
		REG_TWI1_IER = TWI_IER_RXRDY;
	}

	// Receive last byte
	else if (IS_BIT_SET(status, TWI_SR_RXRDY) && IS_BIT_SET(irq_mask, TWI_IMR_RXRDY)) {

		// Read last byte
		uint8_t* buffer_addr = (uint8_t*)REG_TWI1_RPR;
		*buffer_addr = REG_TWI1_RHR;

		// Configure interrupts
		REG_TWI1_IDR = 0xFFFFFFFF;
		REG_TWI1_IER = TWI_IER_TXCOMP;
	}
}
