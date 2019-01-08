//  ***************************************************************************
/// @file    VC0706.h
/// @author  NeoProg
/// @brief   VC0706 camera driver
//  ***************************************************************************
#ifndef VC0706_H_
#define VC0706_H_

#include <stdbool.h>


extern bool vc0706_init(bool is_fast_speed_mode);
extern void vc0706_async_set_compression_rate(uint32_t compression_rate);
extern void vc0706_async_capture_image(void);
extern void vc0706_async_read_image_size(void);
extern bool vc0706_async_read_raw_image_data(uint8_t* buffer, uint32_t buffer_size, uint32_t image_size);

extern bool vc0706_is_async_operation_complete(void);
extern bool vc0706_is_operation_status_success(void);
extern uint32_t vc0706_parse_image_size(void);
extern void vc0706_parse_image_data(uint8_t* raw_image_data, uint32_t image_size);


#endif /* VC0706_H_ */