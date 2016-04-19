/*
 * lcd.cpp
 *
 *  Created on: 9.7.2015
 *      Author: root
 */
#include "LCD.h"

/* Global file descriptor variable */
int g_fd;

/* Open serial port and set the settings */
int setupLCD_Serial(void)
{
	int error;

	/* Open in write mode */
    g_fd = open("/dev/ttyAMA0", O_WRONLY | O_NOCTTY | O_NDELAY);

    if (g_fd == -1)
    {
            perror("Error - Unable to open UART!\n");
    }

    struct termios options;

    /* Get the current configuration of the serial interface */
    error = tcgetattr(g_fd, &options);
    if(error == -1)
    {
    	perror("tcgetattr error!\n");
    }

    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; //Baud rate 9600, Character size mask CS8, Ignore modem control lines, Enable receiver
    options.c_oflag = 0;
    options.c_lflag = 0;

    /*
     * Flushes (discards) not-send data (data still in the UART send buffer) and/or flushes (discards)
	 * received data (data already in the UART receive buffer).
     */
    error = tcflush(g_fd, TCOFLUSH);
    if(error == -1)
    {
    	perror("tcflush error!\n");
    }

    /* Apply the configuration */
    error = tcsetattr(g_fd, TCSANOW, &options);
    if(error == -1)
    {
    	perror("tcsetattr error!\n");
    }
    return 0;
}

/* Clears LCD display */
int clear_LCD(void)
{
	unsigned char data[2];
	data[0] = 4;
	data[1] = 0xFF;

	int g_n = write(g_fd, &data, sizeof(data));

	if (g_n < 0)
	{
		perror("Write failed - ");
	}
	return 0;
}

/* Define number of rows/columns on the display */
int setType_LCD(const unsigned char line, const unsigned char col)
{
	unsigned char data[4];
	data[0] = 0x05;
	data[1] = line;
	data[2] = col;
	data[3] = 0xFF;

    int g_n = write(g_fd, &data, sizeof(data));

    if (g_n < 0)
    {
            perror("Write failed - ");
    }
    return 0;
}

/* Backlight brightness */
int setBacklight_LCD(const unsigned char brightness)
{
	unsigned char data[3];
	data[0] = 0x07;
	data[1] = brightness;
	data[2] = 0xFF;

    int g_n =  write(g_fd, &data, sizeof(data));

    if (g_n < 0)
    {
    	perror("Write failed - ");
    }
    return 0;
}

/* Position the cursor */
int setCursor_LCD(const unsigned char line, const unsigned char col)
{
	unsigned char data[4];
	data[0] = 0x02;
	data[1] = line;
	data[2] = col;
	data[3] = 0xFF;

    int g_n = write(g_fd, &data, sizeof(data));

    if (g_n < 0)
    {
            perror("Write failed - ");
    }
    return 0;
}

/* Write a single character to the LCD display */
int writeChar_LCD(const unsigned char c)
{
	unsigned char data[3];
	data[0] = 0x0A;
	data[1] = c;
	data[2] = 0xFF;

	int g_n = write(g_fd, &data, sizeof(data));

	if (g_n < 0)
	{
		perror("Write failed -");
	}
	return 0;
}

/* Print long string to a lcd */
int printLongString_LCD(char *pstring, size_t len)
{
	int g_n;
	size_t i;

		/*
		 * If String is 16 or under characters long
		 */
		if(len <= 16){

			setCursor_LCD(1,1);

			unsigned char data[len+3];
			data[0] = 0x01;
			for(i = 1 ; i < (len+1) ; i++)
			{
				data[i] = pstring[i-1];
			}

			data[len+1] = '\0';
			data[len+2] = 0xFF;

			g_n = write(g_fd, &data, sizeof(data));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

		}
		/*
		 * If string is between 16 and 32 characters long
		 */
		else if (len > 16 && len <= 32){

			unsigned char data[19];
			unsigned char data2[len-13];

			setCursor_LCD(1,1);

			data[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data[i] = pstring[i-1];
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_n = write(g_fd, &data, sizeof(data));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < (len-15) ; i++)
			{
				data2[i] = pstring[(i+16)-1];
			}

			data2[(len-15)] = '\0';
			data2[(len-15)+1] = 0xFF;

			g_n = write(g_fd, &data2, sizeof(data2));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

		}
		/*
		 * If string is between 32-48 characters long
		 */
		else if (len > 32 && len <= 48){

			unsigned char data[19];
			unsigned char data2[19];
			unsigned char data3[len-29];

			setCursor_LCD(1,1);

			data[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data[i] = pstring[i-1];
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_n = write(g_fd, &data, sizeof(data));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data2[i] = pstring[(i+16)-1];
			}

			data2[17] = '\0';
			data2[18] = 0xFF;

			g_n = write(g_fd, &data2, sizeof(data2));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

			//Jump to next line
			setCursor_LCD(3,1);

			data3[0] = 0x01;
			for(i = 1 ; i < (len-31) ; i++)
			{
				data3[i] = pstring[(i+32)-1];
			}
			data3[len-31] = '\0';
			data3[len-31+1] = 0xFF;

			g_n = write(g_fd, &data3, sizeof(data3));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

		}
		/*
		 * If string is more than 48 and max 64 characters long
		 */
		else if(len > 48 && len <= 64){

			unsigned char data[19];
			unsigned char data2[19];
			unsigned char data3[19];
			unsigned char data4[len-45];

			setCursor_LCD(1,1);

			data[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data[i] = pstring[i-1];
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_n = write(g_fd, &data, sizeof(data));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data2[i] = pstring[(i+16)-1];
			}

			data2[17] = '\0';
			data2[18] = 0xFF;

			g_n = write(g_fd, &data2, sizeof(data2));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

			//Jump to next line
			setCursor_LCD(3,1);

			data3[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data3[i] = pstring[(i+32)-1];
			}
			data3[17] = '\0';
			data3[18] = 0xFF;

			g_n = write(g_fd, &data3, sizeof(data3));
			if (g_n < 0)
			{
				perror("Write failed -");
			}

			//Jump to next line
			setCursor_LCD(4,1);

			data4[0] = 0x01;
			for(i = 1 ; i < (len-47) ; i++)
			{
				data4[i] = pstring[(i+48)-1];
			}
			data4[(len-47)] = '\0';
			data4[(len-47)+1] = 0xFF;

			g_n = write(g_fd, &data4, sizeof(data4));
			if (g_n < 0)
			{
				perror("Write failed -");
			}
		} else {
			setCursor_LCD(2,1);
			char tooLongString[] = "Too long string!";
			size_t len = strlen(tooLongString);
			printString_LCD(tooLongString, len);
			printf("Too long string\n");
			sleep(2);
		}

	return 0;
}

/* Print short string to a lcd */
int printString_LCD(char *pstring, size_t len)
{
	unsigned char data[len+3];
	size_t i;
	data[0] = 0x01;

	for(i = 1 ; i < (len+1) ; i++)
	{
		data[i] = pstring[i-1];
	}

	data[len+1] = '\0';
	data[len+2] = 0xFF;

	int g_n = write(g_fd, &data, sizeof(data));

	if (g_n < 0)
	{
		perror("Write failed -");
	}
	return 0;
}

/* Print distance to a lcd */
int printDistance_LCD(char *pstring, size_t len)
{
	unsigned char data[len+5];
	size_t i;
	data[0] = 0x01;

	for(i = 1 ; i < (len+1) ; i++)
	{
		data[i] = pstring[i-1];
	}

	data[len+1] = 'm';
	data[len+2] = 'm';
	data[len+3] = '\0';
	data[len+4] = 0xFF;

	int g_n = write(g_fd, &data, sizeof(data));

	if (g_n < 0)
	{
		perror("Write failed -");
	}
	return 0;
}

/* Converts unsigned char array to char array */
void uCharToChar(char *dst,unsigned char *src,size_t src_len)
{
        while (src_len--)
            dst += sprintf(dst,"%02x",*src++);
        *dst = '\0';
}

/* Convert int to string and print it to a lcd screen */
void printDistance(const int distance)
{
	char dBuf[5];
	snprintf(dBuf, sizeof(dBuf), "%d", distance);

	size_t l = strlen(dBuf);
	printDistance_LCD(dBuf, l);
}

/* Print float speed to a lcd */
int printSpeed_LCD(char *pstring, size_t len)
{
	unsigned char data[len+7];
	size_t i;
	data[0] = 0x01;

	for(i = 1 ; i < (len+1) ; i++)
	{
		data[i] = pstring[i-1];
	}

	data[len+1] = 'k';
	data[len+2] = 'm';
	data[len+3] = '/';
	data[len+4] = 'h';
	data[len+5] = '\0';
	data[len+6] = 0xFF;

	int g_n = write(g_fd, &data, sizeof(data));

	if (g_n < 0)
	{
		perror("Write failed -");
	}
	return 0;
}

/* Convert float to string and print it to a lcd screen */
void printSpeed(const float speed)
{
	if(speed >= 100)
	{
		char buf[7];
		snprintf(buf, sizeof(buf), "%.2f", speed);

		size_t l = strlen(buf);
		printSpeed_LCD(buf, l);
	}
	else if(speed < 100)
	{
		char buf[6];
		snprintf(buf, sizeof(buf), "%.2f", speed);

		size_t l = strlen(buf);
		printSpeed_LCD(buf, l);
	}
}

int printConnect(void)
{
	setCursor_LCD(2,3);
	char strng[] = "CONNECTED!!";
	size_t l = strlen(strng);
	printString_LCD(strng, l);

	return 0;
}

int printDisconnect(void)
{
	setCursor_LCD(2,2);
	char strng[] = "DISCONNECTED!!";
	size_t l = strlen(strng);
	printString_LCD(strng, l);

	return 0;
}

int printAlreadyConnected(void)
{
	clear_LCD();
	setCursor_LCD(2,5);
	char strng[] = "ALREADY";
	size_t l = strlen(strng);
	printString_LCD(strng, l);

	setCursor_LCD(3,4);
	char strng2[] = "CONNECTED!";
	l = strlen(strng2);
	printString_LCD(strng2, l);

	return 0;
}
