//  ***************************************************************************
/// @file    pwm.h
/// @author  NeoProg
/// @brief   Leg move driver
//  ***************************************************************************
#ifndef PWM_H_
#define PWM_H_

#define PWM_DISABLE_CHANNEL_VALUE			(0x0000)

extern void pwm_init(void);
extern void pwm_enable(void);
extern void pwm_disable(void);
extern void pwm_lock_buffer(void);
extern void pwm_unlock_buffer(void);
extern void pwm_set_width(uint32_t ch, uint32_t width);

#endif // PWM_H_