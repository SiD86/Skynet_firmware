//  ***************************************************************************
/// @file    wireless.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include <string.h>
#include "wireless.h"
#include "usart3_pdc.h"
#include "systimer.h"
#include "error_handling.h"

#define SEND_STATE_DATA_PERIOD			(100)	// ms
#define WIRELESS_FRAME_SIZE				(sizeof(wireless_protocol_frame_t))
#define WIRELESS_FRAME_DATA_SIZE		(sizeof(wireless_protocol_frame_t::data))


static wireless_protocol_control_data_t wireless_control_data = {0};
static wireless_protocol_state_data_t   wireless_state_data = {0};
	
	
static void async_process_rx();
static void async_process_tx();
static bool is_wireless_frame_detected(uint8_t* data, uint32_t data_size);
static uint32_t calculate_CRC(const uint8_t* data);


//  ***************************************************************************
/// @brief	Wireless driver initialization
/// @return	none
//  ***************************************************************************
void wireless_init() {
	usart3_init();
	usart3_start_rx();
}

//  ***************************************************************************
/// @brief	Wireless driver process
/// @return	none
//  ***************************************************************************
void wireless_process() {

	if (usart3_is_error() == true) {
		usart3_reset(true, true);
		usart3_start_rx();
	}

	async_process_tx();
	async_process_rx();
}



//  ***************************************************************************
/// @brief	Process asynchronous transmit
/// @return	none
//  ***************************************************************************
static void async_process_tx() {

	static uint16_t frame_number = 0;
	static uint32_t prev_transmit_time = 0;

	// Check transmit interval
	uint32_t current_time = get_time_ms();
	if (current_time - prev_transmit_time < SEND_STATE_DATA_PERIOD) {
		return;
	}

	// Check complete transmit previous data
	if (usart3_is_tx_complete() == false) {
		prev_transmit_time = current_time;
		return;
	}

	// Build new wireless frame
	wireless_protocol_frame_t* frame = (wireless_protocol_frame_t*)usart3_get_tx_buffer_address();
	frame->number = frame_number;
	memcpy(frame->data, &wireless_state_data, WIRELESS_FRAME_DATA_SIZE);
	frame->CRC = calculate_CRC(frame->data);

	// Start transmit
	usart3_start_tx(WIRELESS_FRAME_SIZE);

	// Update variables
	prev_transmit_time = current_time;
	++frame_number;
}

//  ***************************************************************************
/// @brief	Process asynchronous receive
/// @return	none
//  ***************************************************************************
static void async_process_rx() {

	// Check frame received
	if (usart3_is_frame_received() == false) {
		return;	
	}

	// Check received data
	uint8_t* recv_data = usart3_get_rx_buffer_address();
	uint32_t data_size = usart3_get_frame_size();
	if (is_wireless_frame_detected(recv_data, data_size) == false) {
		return;
	}

	// Copy frame data
	wireless_protocol_frame_t* frame = (wireless_protocol_frame_t*)recv_data;
	memcpy(&wireless_control_data, frame->data, WIRELESS_FRAME_DATA_SIZE);
	
	// Start receive next frame
	usart3_start_rx();
}

//  ***************************************************************************
/// @brief	Check received data
/// @return	true - wireless frame detected, false - any data
//  ***************************************************************************
static bool is_wireless_frame_detected(uint8_t* data, uint32_t data_size) {

	wireless_protocol_frame_t* frame = (wireless_protocol_frame_t*)data;

	// Check packet size
	if (data_size != WIRELESS_FRAME_SIZE) {
		return false;
	}

	// Check CRC
	uint32_t CRC = calculate_CRC(frame->data);
	if (frame->CRC != CRC) {
		return false;
	}

	return true;
}

//  ***************************************************************************
/// @brief	Calculate wireless frame CRC
/// @return	CRC
//  ***************************************************************************
static uint32_t calculate_CRC(const uint8_t* data) {

	uint32_t CRC = 0;

	for (uint32_t i = 0; i < 32; i += 2) {

		uint16_t value = (data[i] << 8) | data[i + 1];
		if (value % 3 == 0)
			CRC |= 1 << (i + 0);

		if (value % 2 == 0)
			CRC |= 1 << (i + 1);
	}

	return CRC;
}
