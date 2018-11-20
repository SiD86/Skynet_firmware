//  ***************************************************************************
/// @file    uart.h
/// @author  NeoProg
/// @brief   UART driver (main USB)
//  ***************************************************************************
#ifndef UART_H_
#define UART_H_

extern void uart_init();
extern void uart_write(uint8_t* data, uint32_t size);
extern void uart_print_debug_message(char* msg);
extern void uart_print_number(double number);

#endif /* UART_H_ */