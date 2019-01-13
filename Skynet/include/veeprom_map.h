//  ***************************************************************************
/// @file    veeprom_map.h
/// @author  NeoProg
/// @brief   VEEPROM mapping
//  ***************************************************************************
#ifndef VEEPROM_MAP_H_
#define VEEPROM_MAP_H_

// Base addresses for servo configuration
#define SERVO_ZERO_OFFSET_EE_ADDRESS				(0x0100)	///< U8 Servo zero offset
#define SERVO_ROTATE_DIRECTION_EE_ADDRESS			(0x0101)	///< U8 Servo rotate direction
#define SERVO_TYPE_EE_ADDRESS                       (0x0102)    ///< U8 Servo type
#define SERVO_START_ANGLE_EE_ADDRESS				(0x0103)	///< U8 Servo start angle
#define SERVO_CONFIGURATION_SIZE					(8)

#define LIMB_CONTROL_EE_ADDRESS						(0x01C0)	///< U16 Limb control
#define LIMB_0_0_SERVO_EE_ADDRESS					(0x01D0)	///< U8  Limb_0 link_0 servo
#define LIMB_0_1_SERVO_EE_ADDRESS					(0x01D1)	///< U8  Limb_0 link_1 servo
#define LIMB_0_2_SERVO_EE_ADDRESS					(0x01D2)	///< U8  Limb_0 link_2 servo
#define LIMB_CONFIGURATION_SIZE						(4)


#endif /* VEEPROM_MAP_H_ */