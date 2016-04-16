/*
 * lcd.h
 *
 *  Created on: 9.7.2015
 *      Author: root
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdio.h>
#include <unistd.h> //Used for UART
#include <fcntl.h> //Used for UART
#include <termios.h> //Used for UART
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Global file descriptor variable */
extern int g_fd;

/* Function prototypes */
int setupLCD_Serial(void);
int clear_LCD(void);
int setType_LCD(const unsigned char line, const unsigned char col);
int setBacklight_LCD(const unsigned char brightness);
int setCursor_LCD(const unsigned char line, const unsigned char col);
int writeChar_LCD(const unsigned char c);
int printLongString_LCD(char *pstring, size_t len);
int printString_LCD(char *pstring, size_t len);
void uCharToChar(char *dst,unsigned char *src,size_t src_len);
int printDistance_LCD(char *pstring, size_t len);
void printDistance(const int distance);
int printSpeed_LCD(char* pstring, size_t len);
void printSpeed(const float speed);
int printConnect(void);
int printDisconnect(void);
int printAlreadyConnected(void);


#endif /* LCD_H_ */
