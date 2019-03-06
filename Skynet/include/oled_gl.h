//  ***************************************************************************
/// @file    oled_gl.h
/// @author  NeoProg
/// @brief   OLED display graphic library
//  ***************************************************************************
#ifndef OLED_GL_H_
#define OLED_GL_H_

#include <stdint.h>
#include <stdbool.h>


bool oled_gl_init(void);

void oled_gl_clear_display(void);
void oled_gl_clear_row_fragment(uint32_t row, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void oled_gl_draw_float_number(uint32_t row, uint32_t x, float number);
void oled_gl_draw_dec_number(uint32_t row, uint32_t x, int32_t number);
void oled_gl_draw_hex_number(uint32_t row, uint32_t x, uint32_t number);
void oled_gl_draw_string(uint32_t row, uint32_t x, const char* str);

void oled_gl_draw_horizontal_line(uint32_t row, uint32_t x, uint32_t y, uint32_t width);
void oled_gl_draw_rect(uint32_t row, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void oled_gl_draw_bitmap(uint32_t row, uint32_t x, uint32_t bitmap_width, uint32_t bitmap_height, const uint8_t* bitmap);

void oled_gl_display_update(void);
void oled_gl_start_async_display_update(void);
void oled_gl_async_display_update_process(void);


#endif /* OLED_GL_H_ */