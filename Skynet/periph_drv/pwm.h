//  ***************************************************************************
/// @file    pwm.h
/// @author  NeoProg
/// @brief   Leg move driver
//  ***************************************************************************
#ifndef PWM_H_
#define PWM_H_


#define PWM_DISABLE_CHANNEL_VALUE			(0x0000)

typedef enum {
	PWM_BUFFERS_LOCK,
	PWM_BUFFERS_UNLOCK
} pwm_buffer_state_t;


extern void pwm_init(void);
extern void pwm_enable(void);
extern void pwm_disable(void);
extern void pwm_set_buffers_state(pwm_buffer_state_t state);
extern void pwm_set_width(uint32_t ch, uint32_t width);
extern uint32_t pwm_get_counter();


#endif // PWM_H_