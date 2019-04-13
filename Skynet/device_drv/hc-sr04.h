//  ***************************************************************************
/// @file    hc-sr04.h
/// @author  NeoProg
/// @brief   HC-SR04 driver
//  ***************************************************************************
#ifndef HC_SR04_H_
#define HC_SR04_H_

#include <stdint.h>
#include <stdbool.h>


void hcsr04_init(void);
void hcsr04_start(void);
void hcsr04_process(void);
uint32_t hcsr04_get_distance(void);


#endif /* HC_SR04_H_ */