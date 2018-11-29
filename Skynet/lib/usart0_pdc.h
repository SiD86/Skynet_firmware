//  ***************************************************************************
/// @file    usart0_pdc.h
/// @author  NeoProg
/// @brief   USART0 driver
//  ***************************************************************************
#ifndef USART0_PDC_H_
#define USART0_PDC_H_

#include <stdbool.h>


void     usart0_init();
void     usart0_reset(bool tx, bool rx);
bool     usart0_is_error();

void     usart0_start_tx(uint32_t size);
bool     usart0_is_tx_complete();
uint8_t* usart0_get_tx_buffer_address();

void     usart0_start_rx();
bool     usart0_is_frame_received();
uint32_t usart0_get_frame_size();
uint8_t* usart0_get_rx_buffer_address();


#endif // USART0_PDC_H_