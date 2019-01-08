//  ***************************************************************************
/// @file    multimedia.h
/// @author  NeoProg
/// @brief   Multimedia subsystem
//  ***************************************************************************
#ifndef MULTIMEDIA_H_
#define MULTIMEDIA_H_

#include <stdbool.h>


extern uint8_t ram_multimedia_state;

void multimedia_init(void);
void multimedia_process(void);
bool multimedia_update_image(void);

uint16_t multimedia_get_image_size(void);
bool multimedia_read_image_data(uint32_t address, uint8_t* buffer, uint32_t bytes_count);


#endif /* MULTIMEDIA_H_ */