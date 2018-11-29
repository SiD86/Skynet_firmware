//  ***************************************************************************
/// @file    usart3_pdc.h
/// @author  NeoProg
/// @brief   USART3 driver
//  ***************************************************************************
#ifndef USART3_PDC_H_
#define USART3_PDC_H_

#include <stdbool.h>


void     usart3_init();
void     usart3_reset(bool tx, bool rx);
bool     usart3_is_error();

void     usart3_start_tx(uint32_t size);
bool     usart3_is_tx_complete();
uint8_t* usart3_get_tx_buffer_address();

void     usart3_start_rx();
bool     usart3_is_frame_received();
uint32_t usart3_get_frame_size();
uint8_t* usart3_get_rx_buffer_address();


#endif // USART3_PDC_H_