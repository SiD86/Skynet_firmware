//  ***************************************************************************
/// @file    VC0706.h
/// @author  NeoProg
/// @brief   VC0706 camera driver
//  ***************************************************************************
#include <sam.h>
#include <stdlib.h>
#include "vc0706.h"
#include "usart1_pdc.h"
#include "systimer.h"

#define DEFAULT_SPEED_MODE_USART_BAUD_RATE        (115200)
#define FAST_SPEED_MODE_USART_BAUD_RATE           (460800)


static bool wait_async_operation_complete_complete(uint32_t timeout);
static bool set_fast_baud_rate(void);
static bool set_single_shot_mode(void);


//  ***************************************************************************
/// @brief	VC0706 initialization
/// @param  is_fast_speed_mode: true - initialize VC0706 in fast speed mode
/// @return	true - success, false - error
//  ***************************************************************************
bool vc0706_init(bool is_fast_speed_mode) {
    
    usart1_init(DEFAULT_SPEED_MODE_USART_BAUD_RATE);
	
	if (is_fast_speed_mode == true) {
		
		// Switch to fast speed mode
		set_fast_baud_rate();
		
		// Change USART baud rate
		usart1_set_baud_rate(FAST_SPEED_MODE_USART_BAUD_RATE);
		
		delay_ms(50);
	}
    
    // Set single shot mode
    return set_single_shot_mode();
}

//  ***************************************************************************
/// @brief  Async set image compression rate
/// @param  compression_rate: compression rate
//  ***************************************************************************
void vc0706_async_set_compression_rate(uint32_t compression_rate) {
	
	uint8_t* tx_buffer = usart1_get_internal_tx_buffer_address();
	tx_buffer[0] = 0x56;
	tx_buffer[1] = 0x00;
	tx_buffer[2] = 0x31;
	tx_buffer[3] = 0x05;
	tx_buffer[4] = 0x01;
	tx_buffer[5] = 0x01;
	tx_buffer[6] = 0x12;
	tx_buffer[7] = 0x04;
	tx_buffer[8] = compression_rate;
	
	usart1_reset(true, true);
	usart1_start_rx(USART1_USE_INTERNAL_BUFFER, 0);
	usart1_start_tx(9);
}

//  ***************************************************************************
/// @brief  Async capture image
//  ***************************************************************************
void vc0706_async_capture_image(void) {
	
	uint8_t* tx_buffer = usart1_get_internal_tx_buffer_address();
	tx_buffer[0] = 0x56;
    tx_buffer[1] = 0x00;
    tx_buffer[2] = 0x36;
    tx_buffer[3] = 0x01;
    tx_buffer[4] = 0x02;
	
	usart1_reset(true, true);
	usart1_start_rx(USART1_USE_INTERNAL_BUFFER, 0);
	usart1_start_tx(5);
}

//  ***************************************************************************
/// @brief  Async read image size
//  ***************************************************************************
void vc0706_async_read_image_size(void) {
	
	uint8_t* tx_buffer = usart1_get_internal_tx_buffer_address();
	tx_buffer[0] = 0x56;
	tx_buffer[1] = 0x00;
	tx_buffer[2] = 0x34;
	tx_buffer[3] = 0x01;
	tx_buffer[4] = 0x00;
	
	usart1_reset(true, true);
	usart1_start_rx(USART1_USE_INTERNAL_BUFFER, 0);
	usart1_start_tx(5);
}

//  ***************************************************************************
/// @brief  Async read raw image data
/// @note   After this operation complete need call vc0706_prepare_image_data()
/// @param  buffer: pointer to buffer for receive image data
/// @param  buffer_size: buffer size
/// @param  image_size: image size for reading
/// @return true - operation started, false - buffer have little size
//  ***************************************************************************
bool vc0706_async_read_raw_image_data(uint8_t* buffer, uint32_t buffer_size, uint32_t image_size) {
	
	if (image_size + 10 > buffer_size) {
		return false;
	}
	
 	uint8_t* tx_buffer = usart1_get_internal_tx_buffer_address();
	tx_buffer[0] = 0x56;
	tx_buffer[1] = 0x00;
	tx_buffer[2] = 0x32;
	tx_buffer[3] = 0x0C;
	tx_buffer[4] = 0x00;
	tx_buffer[5] = 0x0C;
	tx_buffer[6] = 0x00;
	tx_buffer[7] = 0x00;
	tx_buffer[8] = 0x00;
	tx_buffer[9] = 0x00;
	tx_buffer[10] = (image_size & 0xFF000000) >> 24;
	tx_buffer[11] = (image_size & 0x00FF0000) >> 16;
	tx_buffer[12] = (image_size & 0x0000FF00) >> 8;
	tx_buffer[13] = (image_size & 0x000000FF) >> 0;
	tx_buffer[14] = 0x00;
	tx_buffer[15] = 0x00;
	
	usart1_reset(true, true);
	usart1_start_rx(buffer, buffer_size);
	usart1_start_tx(16);
	
	return true;
}

//  ***************************************************************************
/// @brief	Check execute async operation complete
/// @return true - execute complete, false - in progress
//  ***************************************************************************
bool vc0706_is_async_operation_complete(void) {
	return usart1_is_frame_received();
}

//  ***************************************************************************
/// @brief	Check current operation status
/// @return true - success, false - error
//  ***************************************************************************
bool vc0706_is_operation_status_success(void) {
	
	const uint8_t* rx = usart1_get_internal_rx_buffer_address();
	return rx[3] == 0x00;
}

//  ***************************************************************************
/// @brief	Get image size
/// @note	Call after execute vc0706_async_read_image_size() cmd
/// @return image size
//  ***************************************************************************
uint32_t vc0706_parse_image_size(void) {
	
	const uint8_t* rx = usart1_get_internal_rx_buffer_address();
	return (rx[5] << 24) | (rx[6] << 16) | (rx[7] << 8) | (rx[8] << 0); 
}

//  ***************************************************************************
/// @brief  Parse image data from raw image data
/// @param  raw_image_data: pointer to buffer with raw image data
/// @param  image_size: image size
//  ***************************************************************************
void vc0706_parse_image_data(uint8_t* raw_image_data, uint32_t image_size) {
	
	// Remove service info from begin
	for (uint32_t i = 0; i < image_size; ++i) {
		raw_image_data[i] = raw_image_data[i + 5];
	}
	
	// Remove service info from end
	for (uint32_t i = image_size; i < image_size + 10; ++i) {
		raw_image_data[i] = 0x00;
	} 
}


//  ***************************************************************************
/// @brief	Start send set fast baud rate command
/// @note   set 420800 baud rate
//  ***************************************************************************
static bool set_fast_baud_rate(void) {
    
    uint8_t* tx = usart1_get_internal_tx_buffer_address();
    tx[0] = 0x56;
    tx[1] = 0x00;
    tx[2] = 0x24;
    tx[3] = 0x05;
    tx[4] = 0x02;
    tx[5] = 0x02;
    tx[6] = 0xA6;
    tx[7] = 0x00;
    tx[8] = 0x03;
    
	usart1_reset(true, true);
    usart1_start_rx(USART1_USE_INTERNAL_BUFFER, 0);
    usart1_start_tx(9);
    
    return wait_async_operation_complete_complete(500);
}

//  ***************************************************************************
/// @brief	Set single shot mode command
/// @return true - success, false - error
//  ***************************************************************************
static bool set_single_shot_mode(void) {
    
    uint8_t* tx = usart1_get_internal_tx_buffer_address();
    tx[0] = 0x56;
    tx[1] = 0x00;
    tx[2] = 0x36;
    tx[3] = 0x01;
    tx[4] = 0x00;
    
	usart1_reset(true, true);
    usart1_start_rx(USART1_USE_INTERNAL_BUFFER, 0);
    usart1_start_tx(5);
    
    return wait_async_operation_complete_complete(500);
}

//  ***************************************************************************
/// @brief	Wait async operation execute complete
/// @param	timeout_ms: operation timeout value
/// @return true - operation complete, false - timeout
//  ***************************************************************************
static bool wait_async_operation_complete_complete(uint32_t timeout_ms) {
    
    uint32_t start_time = get_time_ms();
    
    while (vc0706_is_async_operation_complete() == false) {
        
        if (get_time_ms() - start_time > timeout_ms) {
            return false;
        }
    }
    
    return true;
}