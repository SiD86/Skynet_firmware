//  ***************************************************************************
/// @file    pwm.h
/// @author  NeoProg
/// @brief   Leg move driver
//  ***************************************************************************
#ifndef PWM_H_
#define PWM_H_

#define PWM_DISABLE_CHANNEL_VALUE			(0x0000)

extern void pwm_init(void);
extern void pwm_start(void);
extern void pwm_stop(void);
extern uint32_t pwm_get_counter(void);
extern void pwm_set_width(uint32_t ch, uint32_t width);

#endif // PWM_H_