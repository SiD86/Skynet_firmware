//  ***************************************************************************
/// @file    usart1_pdc.h
/// @author  NeoProg
/// @brief   USART1 driver
//  ***************************************************************************
#ifndef USART1_PDC_H_
#define USART1_PDC_H_

#include <stdbool.h>


void     usart1_init();
void     usart1_reset(bool tx, bool rx);
bool     usart1_is_error();

void     usart1_start_tx(uint32_t size);
bool     usart1_is_tx_complete();
uint8_t* usart1_get_tx_buffer_address();

void     usart1_start_rx();
bool     usart1_is_frame_received();
uint32_t usart1_get_frame_size();
uint8_t* usart1_get_rx_buffer_address();


#endif // USART1_PDC_H_
