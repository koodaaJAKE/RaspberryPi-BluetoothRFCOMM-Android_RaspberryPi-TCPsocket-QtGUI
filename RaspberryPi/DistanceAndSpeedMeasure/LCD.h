/*
 * LCD.h
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdio.h>
#include <unistd.h> //Used for UART
#include <fcntl.h> //Used for UART
#include <termios.h> //Used for UART
#include <stdlib.h>
#include <string.h>

/* Function prototypes */
int setupLCD_Serial(void);
int serialLCD_Close(void);
int clear_LCD(void);
int setType_LCD(const unsigned char line, const unsigned char col);
int setBacklight_LCD(const unsigned char brightness);
int setCursor_LCD(const unsigned char line, const unsigned char col);
int writeChar_LCD(const unsigned char c);
int printLongString_LCD(const char *pstring, const size_t len);
int printString_LCD(const char *pstring, const size_t len);
int printDistance(const int distance);
int printSpeed(const float speed);
int printConnect(void);
int printDisconnect(void);
int printAlreadyConnected(void);


#endif /* LCD_H_ */
