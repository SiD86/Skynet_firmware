//  ***************************************************************************
/// @file    multimedia.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include <string.h>
#include "multimedia.h"
#include "vc0706.h"
#include "systimer.h"
#include "error_handling.h"

#define IMAGE_BUFFER_SIZE			(50 * 1024)	// 50kB = 51200 bytes


typedef enum {
    STATE_NOINIT,
	STATE_IDLE,
    STATE_SEND_CAPTURE_IMAGE_CMD,
    STATE_SEND_READ_IMAGE_SIZE_CMD,
    STATE_SEND_READ_RAW_IMAGE_DATA_CMD,
	STATE_PARSE_IMAGE_DATA,
    STATE_WAIT
} module_state_t;


uint8_t ram_multimedia_state = STATE_NOINIT;

static module_state_t module_state = STATE_NOINIT;
static uint8_t image_data[IMAGE_BUFFER_SIZE] = {0};  // Buffer for store JPEG image (640x480)
static uint16_t image_size = 33333;


void multimedia_init(void) {
    
    if (vc0706_init(true) == false) {
        callback_set_hardware_error(ERROR_MODULE_MULTIMEDIA);
        return;
    }

	// Clear image data
	memset(image_data, 0x00, IMAGE_BUFFER_SIZE);
    
    module_state = STATE_IDLE;
    ram_multimedia_state = module_state;
}

//  ***************************************************************************
/// @brief  Multimedia process
/// @note   Call from main loop
//  ***************************************************************************
void multimedia_process(void) {
    
    if (callback_is_multimedia_error_set() == true) {
        return; // Module disabled
    }
    
    static uint32_t start_wait_time = 0;
    static uint32_t timeout_value = 0;
    static module_state_t success_state = STATE_NOINIT;
    static module_state_t fail_state = STATE_NOINIT;
    
    switch (module_state) {
		
		case STATE_IDLE:
			break;
     
        case STATE_SEND_CAPTURE_IMAGE_CMD:
			vc0706_async_capture_image();
            
            start_wait_time = get_time_ms();
            timeout_value = 200;
            module_state = STATE_WAIT;
            success_state = STATE_SEND_READ_IMAGE_SIZE_CMD;
            fail_state = STATE_SEND_CAPTURE_IMAGE_CMD;
            break;
        
        case STATE_SEND_READ_IMAGE_SIZE_CMD:
			vc0706_async_read_image_size();
            
            start_wait_time = get_time_ms();
            timeout_value = 200;
            module_state = STATE_WAIT;
            success_state = STATE_SEND_READ_RAW_IMAGE_DATA_CMD;
            fail_state = STATE_SEND_READ_IMAGE_SIZE_CMD;
            break;
        
        case STATE_SEND_READ_RAW_IMAGE_DATA_CMD:
			image_size = vc0706_parse_image_size();
            if (vc0706_async_read_raw_image_data(image_data, IMAGE_BUFFER_SIZE, image_size) == false) {
				callback_set_memory_error(ERROR_MODULE_MULTIMEDIA); // Image buffer have little size
				break;
			}
        
            start_wait_time = get_time_ms();
            timeout_value = 5000;
            module_state = STATE_WAIT;
            success_state = STATE_PARSE_IMAGE_DATA;
            fail_state = STATE_SEND_READ_RAW_IMAGE_DATA_CMD;
            break;
			
		case STATE_PARSE_IMAGE_DATA:
			vc0706_parse_image_data(image_data, image_size);
			module_state = STATE_IDLE;
			break;
        
        case STATE_WAIT:
            if (vc0706_is_async_operation_complete() == false) {
                
                if (get_time_ms() - start_wait_time > timeout_value) {
                    callback_set_hardware_error(ERROR_MODULE_MULTIMEDIA);
                    break;
                }
            }
            else {
                
                module_state = (vc0706_is_operation_status_success() == true) ? success_state : fail_state;
            }
            break;
            
        case STATE_NOINIT:
        default:
            callback_set_selfdiag_error(ERROR_MODULE_MULTIMEDIA);
            break;
    }
    
    ram_multimedia_state = module_state;
}

//  ***************************************************************************
/// @brief  Start receive image from VC0706
/// @return true - start success, false - error
//  ***************************************************************************
bool multimedia_update_image(void) {

	if (module_state == STATE_IDLE) {
		module_state = STATE_SEND_CAPTURE_IMAGE_CMD;
		return true;
	}	
	return false;
}

//  ***************************************************************************
/// @brief  Get image size
/// @return Image size
//  ***************************************************************************
uint16_t multimedia_get_image_size(void) {
	return image_size;
}

//  ***************************************************************************
/// @brief  Read image data
/// @param  address: multimedia address [0x0000; image_size - 1]
/// @param  buffer: pointer to buffer
/// @param  bytes_count: bytes count for read
/// @return true - read success, false - error
//  ***************************************************************************
bool multimedia_read_image_data(uint32_t address, uint8_t* buffer, uint32_t bytes_count) {
	
	if (address + bytes_count > image_size) {
		return false;
	}
	
	memcpy(buffer, &image_data[address], bytes_count);
	return true;
}