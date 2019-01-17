//  ***************************************************************************
/// @file    veeprom_map.h
/// @author  NeoProg
/// @brief   VEEPROM mapping
//  ***************************************************************************
#ifndef VEEPROM_MAP_H_
#define VEEPROM_MAP_H_

// Base addresses for limbs configuration
#define LIMB_COXA_LENGTH_EE_ADDRESS					(0x0000)
#define LIMB_FEMUR_LENGTH_EE_ADDRESS				(0x0002)
#define LIMB_TIBIA_LENGTH_EE_ADDRESS				(0x0004)
#define LIMB_COXA_ZERO_ROTATE_EE_ADDRESS			(0x0006)
#define LIMB_FEMUR_ZERO_ROTATE_EE_ADDRESS			(0x0008)
#define LIMB_TIBIA_ZERO_ROTATE_EE_ADDRESS			(0x000A)
#define LIMB_COXA_MIN_ANGLE_EE_ADDRESS				(0x0010)
#define LIMB_COXA_MAX_ANGLE_EE_ADDRESS				(0x0011)
#define LIMB_FEMUR_MIN_ANGLE_EE_ADDRESS				(0x0012)
#define LIMB_FEMUR_MAX_ANGLE_EE_ADDRESS				(0x0013)
#define LIMB_TIBIA_MIN_ANGLE_EE_ADDRESS				(0x0014)
#define LIMB_TIBIA_MAX_ANGLE_EE_ADDRESS				(0x0015)
#define LIMB_START_POSITION_X_EE_ADDRESS			(0x0016)
#define LIMB_START_POSITION_Y_EE_ADDRESS			(0x0018)
#define LIMB_START_POSITION_Z_EE_ADDRESS			(0x001A)
#define LIMB_CONFIGURATION_SIZE						(32)


// Base addresses for servos configuration
#define SERVO_ZERO_OFFSET_EE_ADDRESS				(0x0100)	///< U8 Servo zero offset
#define SERVO_ROTATE_DIRECTION_EE_ADDRESS			(0x0101)	///< U8 Servo rotate direction
#define SERVO_TYPE_EE_ADDRESS                       (0x0102)    ///< U8 Servo type
#define SERVO_START_ANGLE_EE_ADDRESS				(0x0103)	///< U8 Servo start angle
#define SERVO_CONFIGURATION_SIZE					(8)

#endif /* VEEPROM_MAP_H_ */