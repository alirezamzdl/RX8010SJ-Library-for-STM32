#ifndef __KS0713_H__
#define __KS0713_H__

#include <stdint.h>
#include <stdbool.h>


extern uint8_t ANT_body;
extern uint8_t ANT_level;
extern uint8_t Falg_alarm;
extern uint8_t Falg_message;
extern uint8_t Flag_recive;
extern uint8_t Flag_send;
extern uint8_t Flag_bat_body;
extern uint8_t Bat_value;
extern uint8_t value1_7eg,value2_7eg,value3_7eg,value4_7eg;  //0-9:0-9   10:A  11:b  12:c  13:C  14:d  15:E  16:F  17:H  18:L  19:-  20:_   21:NULL
extern uint8_t Flag_7seg_dots;


void lcd_cls (void);
void lcd_cmd(unsigned char control);
void lcd_write(unsigned char data);
void lcd_init(void);
void LcdCharacter(char character);
void LcdString(char *characters);
void lcd_print(char * text);
void lcd_set_column( char cx,  char cy);
void lcd_set_cursor(unsigned char cx, unsigned char cy);
void LcdString_neg(char *characters, uint8_t neg);
void LcdCharacter_neg(char character);
void draw_progbar(int value,int row);
void LCD_write_string_Medium ( unsigned char X,unsigned char Y, char *string, char mode );
void LCD_icon(void);
#endif
