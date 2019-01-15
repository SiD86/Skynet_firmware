//  ***************************************************************************
/// @file    led.h
/// @author  NeoProg
/// @brief   Led driver
//  ***************************************************************************
#ifndef LED_H_
#define LED_H_


typedef enum {
	GREEN_LED,
    YELLOW_LED,
    RED_LED
} led_id;


void led_init(void);
void led_enable(led_id id);


#endif /* LED_H_ */