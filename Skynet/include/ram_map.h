//  ***************************************************************************
/// @file    ram_map.h
/// @author  NeoProg
/// @brief   RAM mapping functions
//  ***************************************************************************
#ifndef RAM_MAP_H_
#define RAM_MAP_H_


#define RAM_MAP_SIZE				(512)
#define RAM_MAP_BEGIN_ADDRESS		(0x0000)
#define RAM_MAP_END_ADDRESS			(RAM_MAP_BEGIN_ADDRESS + RAM_MAP_SIZE)


void ram_map_get_data(uint32_t address, uint8_t* buffer, uint32_t size);
void ram_map_put_data(uint32_t address, const uint8_t* buffer, uint32_t size);


#endif /* RAM_MAP_H_ */