//  ***************************************************************************
/// @file    gui.c
/// @author  NeoProg
//  ***************************************************************************
#include "gui.h"

#include <sam.h>
#include <stdint.h>
#include <stdbool.h>
#include "oled_gl.h"
#include "monitoring.h"
#include "systimer.h"
#include "error_handling.h"
#include "version.h"

#define DISPLAY_UPDATE_PERIOD					(500)


#define SKYNET_LOGO_BITMAP_WIDTH				(128)
#define SKYNET_LOGO_BITMAP_HEIGHT				(64)
static const uint8_t skynet_logo_bitmap[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x50,
	0x50, 0x54, 0x54, 0x55, 0x55, 0x54, 0x54, 0x50, 0x50, 0x40,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x50,
	0x50, 0x54, 0x54, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x54, 0x54, 0x50, 0x50, 0x40, 0x40, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x50,
	0x50, 0x54, 0x54, 0x50, 0x50, 0x40, 0x41, 0x01, 0x05, 0x05,
	0x15, 0x15, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x15, 0x15,
	0x05, 0x05, 0x01, 0x41, 0x40, 0x50, 0x50, 0x54, 0x54, 0x50,
	0x50, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x50,
	0x50, 0x54, 0x54, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x54, 0x54, 0x50,
	0x50, 0x40, 0x41, 0x01, 0x05, 0x05, 0x15, 0x55, 0x55, 0x15,
	0x05, 0x05, 0x01, 0x41, 0x40, 0x50, 0x50, 0x54, 0x54, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x54, 0x54, 0x50, 0x50, 0x40, 0x40, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x50,
	0x50, 0x54, 0x54, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x54, 0x00, 0x00, 0x00, 0x00, 0x54, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x54,
	0x54, 0x50, 0x50, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x50,
	0x50, 0x54, 0x54, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55, 0x55, 0x54, 0x54, 0x50, 0x50, 0x40,
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0,
	0xF8, 0xF8, 0xB8, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9,
	0xB9, 0xB9, 0xB9, 0xB9, 0x11, 0x01, 0x01, 0x01, 0xF9, 0xF9,
	0xF9, 0xC1, 0xC1, 0xE1, 0xE1, 0xF1, 0x79, 0x79, 0x39, 0x19,
	0x19, 0x09, 0x01, 0x01, 0x01, 0xF9, 0xF9, 0xF9, 0xC1, 0xC1,
	0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0xF9, 0xF9, 0xF9,
	0x00, 0x00, 0x00, 0xF0, 0xF9, 0xF9, 0x79, 0xF1, 0xF1, 0xE1,
	0xC1, 0x81, 0x01, 0x01, 0x01, 0xF9, 0xF9, 0xF9, 0x01, 0x01,
	0xC1, 0xF1, 0xF9, 0xF9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9,
	0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0x01, 0x01, 0x01, 0x39, 0x39,
	0x39, 0x39, 0x39, 0x39, 0xF9, 0xF9, 0xF9, 0x39, 0x39, 0x39,
	0x39, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3B, 0x3B,
	0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B,
	0x3F, 0x3F, 0x1F, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x07,
	0x07, 0x0F, 0x0F, 0x1E, 0x1C, 0x3C, 0x38, 0x38, 0x30, 0x20,
	0x20, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x3B, 0x3B, 0x3B,
	0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3F, 0x1F, 0x1F, 0x00, 0x00,
	0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F,
	0x1F, 0x1E, 0x3C, 0x3F, 0x1F, 0x0F, 0x00, 0x00, 0x07, 0x1F,
	0x3F, 0x3F, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B, 0x3B,
	0x3B, 0x3B, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x3F, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

#define BATTERY_BITMAP_WIDTH					(13)
#define BATTERY_BITMAP_HEIGHT					(8)
static const uint8_t battery_bitmap[] = {
	0xFE, 0x82, 0x93, 0xBB, 0x92, 0x82, 0x82, 0x82, 0x92, 0x93, 
	0x93, 0x82, 0xFE
};

#define CHIP_BITMAP_WIDTH						(13)
#define CHIP_BITMAP_HEIGHT						(8)
static const uint8_t chip_bitmap[] = {
	0x7E, 0xC3, 0x42, 0xC3, 0x42, 0xC3, 0x42, 0xC3, 0x42, 0xC3, 
	0x42, 0xC3, 0x7E
};

#define COMM_BITMAP_WIDTH						(10)
#define COMM_BITMAP_HEIGHT						(8)
static const uint8_t comm_bitmap[] = {
	0x20, 0x40, 0xFF, 0x40, 0x20, 0x04, 0x02, 0xFF, 0x02, 0x04
};


typedef enum {
	STATE_NOINIT,
	STATE_UPDATE_BATTERY_VOLTAGE,
	STATE_UPDATE_PERIPHERY_VOLTAGE,
	STATE_UPDATE_WIRELESS_VOLTAGE,
	STATE_UPDATE_ERROR_STATUS,
	STATE_UPDATE_SYSTEM_MODE,
	STATE_UPDATE_DISPLAY
} state_t;

static state_t module_state = STATE_NOINIT;


//  ***************************************************************************
/// @brief  GUI initialization
/// @param  none
/// @return none
//  ***************************************************************************
void gui_init(void) {
	
	if (oled_gl_init() == false) {
		return;
	}
	
	// Show logo
	oled_gl_draw_bitmap(0, 0, SKYNET_LOGO_BITMAP_WIDTH, SKYNET_LOGO_BITMAP_HEIGHT, skynet_logo_bitmap);
	oled_gl_display_update();
	delay_ms(1000);
	oled_gl_clear_display();
	oled_gl_display_update();
	

	// Draw battery voltage
	oled_gl_draw_bitmap(0, 0, BATTERY_BITMAP_WIDTH, BATTERY_BITMAP_HEIGHT, battery_bitmap);
	oled_gl_draw_float_number(0, 20, 0.0);
	oled_gl_draw_string(0, 45, "V");
	
	// Draw periphery voltage
	oled_gl_draw_bitmap(2, 0, CHIP_BITMAP_WIDTH, CHIP_BITMAP_HEIGHT, chip_bitmap);
	oled_gl_draw_float_number(2, 20, 0.0);
	oled_gl_draw_string(2, 45, "V");
	
	// Draw wireless voltage
	oled_gl_draw_bitmap(4, 3, COMM_BITMAP_WIDTH, COMM_BITMAP_HEIGHT, comm_bitmap);
	oled_gl_draw_float_number(4, 20, 0.0);
	oled_gl_draw_string(4, 45, "V");
	
	// Draw horizontal separator
	oled_gl_draw_horizontal_line(5, 0, 7, 128);
	
	// Draw error status
	oled_gl_draw_hex_number(7, 0, 0x00000000);
	
	// Draw FW version number
	oled_gl_draw_string(7, 72, VERSION_STR);
	
	// Draw vertical separator
	oled_gl_draw_string(0, 56, "|");
	oled_gl_draw_string(1, 56, "|");
	oled_gl_draw_string(2, 56, "|");
	oled_gl_draw_string(3, 56, "|");
	oled_gl_draw_string(4, 56, "|");
	
	// Draw system mode
	oled_gl_draw_string(0, 67, "SYSTEM");
	oled_gl_draw_string(2, 67, "INITIALIZE");
	oled_gl_draw_string(4, 67, "MODE");
	
	oled_gl_display_update();
	
	
	module_state = STATE_UPDATE_BATTERY_VOLTAGE;
}

//  ***************************************************************************
/// @brief  GUI process
/// @param  none
/// @return none
//  ***************************************************************************
void gui_process(void) {
	
	if (callback_is_gui_error_set() == true) return;
	
	
	static uint32_t prev_update_time = 0;
	
	switch (module_state) {
		
		case STATE_UPDATE_BATTERY_VOLTAGE:
			oled_gl_draw_float_number(0, 20, battery_voltage / 10.0);
			module_state = STATE_UPDATE_PERIPHERY_VOLTAGE;
			break;
		
		case STATE_UPDATE_PERIPHERY_VOLTAGE:
			oled_gl_draw_float_number(2, 20, periphery_voltage / 10.0);
			module_state = STATE_UPDATE_WIRELESS_VOLTAGE;
			break;
			
		case STATE_UPDATE_WIRELESS_VOLTAGE:
			oled_gl_draw_float_number(4, 20, wireless_voltage / 10.0);
			module_state = STATE_UPDATE_ERROR_STATUS;
			break;
			
		case STATE_UPDATE_ERROR_STATUS:
			oled_gl_draw_hex_number(7, 0, error_status);
			module_state = STATE_UPDATE_SYSTEM_MODE;
			break;
			
		case STATE_UPDATE_SYSTEM_MODE:
			if (callback_is_emergency_mode_active() == true) {
				oled_gl_clear_row_fragment(2, 67, 0, 67, 8);
				oled_gl_draw_string(2, 67, "EMERGENCY");
			}
			else if (callback_is_voltage_error_set() == true) {
				oled_gl_draw_string(0, 67, "BATTERY");
				oled_gl_draw_string(2, 67, "LOW");
				oled_gl_draw_string(4, 67, "VOLTAGE");
			}
			else {
				oled_gl_clear_row_fragment(2, 67, 0, 67, 8);
				oled_gl_draw_string(2, 67, "STANDART");
			}
			module_state = STATE_UPDATE_DISPLAY;
			break;
			
		case STATE_UPDATE_DISPLAY:
			if (get_time_ms() - prev_update_time >= DISPLAY_UPDATE_PERIOD) {
				
				static bool is_rect_visible = false;
				if (is_rect_visible == true) {
					oled_gl_clear_row_fragment(0, 120, 0, 8, 8);
				}
				else {
					oled_gl_draw_rect(0, 120, 0, 8, 8);
				}
				is_rect_visible = !is_rect_visible;
				
				oled_gl_start_async_display_update();
				prev_update_time = get_time_ms();
				
				module_state = STATE_UPDATE_BATTERY_VOLTAGE;
			}
			break;
		
		case STATE_NOINIT:
		default:
			callback_set_internal_error(ERROR_MODULE_GUI);
			break;
	}
	
	
	oled_gl_async_display_update_process();
}