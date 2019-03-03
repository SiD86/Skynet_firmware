//  ***************************************************************************
/// @file    monitoring.h
/// @author  NeoProg
/// @brief   Monitoring subsystem
//  ***************************************************************************
#ifndef MONITORING_H_
#define MONITORING_H_

#include <stdint.h>


extern uint32_t wireless_voltage;
extern uint32_t periphery_voltage;
extern uint32_t battery_voltage;


extern void monitoring_init(void);
extern void monitoring_process(void);


#endif /* MONITORING_H_ */