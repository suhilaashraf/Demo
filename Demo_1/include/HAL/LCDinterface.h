#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include "Errors.h"
#include "LCD_cfg.h"

#define FIRST_LINE	0
#define SECOND_LINE	1
enum
{
	init_done,
	write_done,
	clear_done,
	setcursor_done
};


typedef struct{
	void* LCD_Port;
	uint8_t LCD_Pin;
}LCD_cfg_t;

extern uint8_t flag [4];
ErrorStatus_t LCD_initAsyn();
ErrorStatus_t LCD_writeStringAsyn(char*string,uint8_t length);
ErrorStatus_t LCD_clearScreenAsyn();
ErrorStatus_t LCD_setCursorPosAsyn(uint8_t posX,uint8_t posY);
ErrorStatus_t LCD_getState(uint8_t * state);


#endif
