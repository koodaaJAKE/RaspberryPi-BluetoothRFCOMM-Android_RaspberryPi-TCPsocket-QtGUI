/*
 * LCD.c
 *
 * This library is for writing to 16x4 LCD display with Serial interface. The 16x4 LCD display is
 * attached to I2C/Serial LCD Backpack and it's controlled with Raspberry Pi.
 */
#include "LCD.h"

/* Static function declarations */
static int createCharacter(const unsigned char memoryLocation, const unsigned char *characterMap);
static int createAwithDots(void);
static int createOwithDots(void);
static int createCapitalAwithDots(void);
static int createCapitalOwithDots(void);
static int printSpeed_LCD(const char* pstring, const size_t len);
static int printDistance_LCD(const char *pstring, const size_t len);

/* Static local variable of serial file descriptor */
static int g_fd;

/* Static local variable of bytes written */
static int g_bytesWritten;

/* Static local variable of length of string */
static size_t stringLength = 0;

/* Custom made characters */
static unsigned char aWithDots[8] = {
	0b01010,
	0b00000,
	0b01110,
	0b00011,
	0b01111,
	0b11011,
	0b01111,
	0b0000
};

static unsigned char oWithDots[8] = {
	0b01010,
	0b00000,
	0b01110,
	0b10001,
	0b10001,
	0b10001,
	0b01110,
	0b00000
};

static unsigned char capitalAwithDots[8] = {
	0b01010,
	0b00000,
	0b01110,
	0b10001,
	0b10001,
	0b11111,
	0b10001,
	0b10001
};

static unsigned char capitalOwithDots[8] = {
	0b01010,
	0b00000,
	0b01110,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b01110
};

/* Open serial port and set the settings */
int setupLCD_Serial(void)
{
	int error;

	/* Open in write mode */
    g_fd = open("/dev/ttyAMA0", O_WRONLY | O_NOCTTY | O_NDELAY);

    if (g_fd == -1)
    {
    	perror("Error - Unable to open UART!\n");
        return -1;
    }

    struct termios options;

    /* Get the current configuration of the serial interface */
    error = tcgetattr(g_fd, &options);
    if(error == -1)
    {
    	perror("tcgetattr error!\n");
    	return -1;
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
    	return -1;
    }

    /* Apply the configuration */
    error = tcsetattr(g_fd, TCSANOW, &options);
    if(error == -1)
    {
    	perror("tcsetattr error!\n");
    	return -1;
    }

	/* Create custom made characters */
	createAwithDots();
	createOwithDots();
	createCapitalAwithDots();
	createCapitalOwithDots();

    return 0;
}

/* Close the serial */
int serialLCD_Close(void)
{
    int statusVal;
    statusVal = close(g_fd);

    if(statusVal < 0)
    {
    	perror("Could not close serial device");
    	return -1;
    }
    return statusVal;
}

/* Creates custom made character */
static int createCharacter(const unsigned char memoryLocation, const unsigned char *characterMap)
{
	/* Memory location for custom made character can be between 0-7 */
	if(memoryLocation <= 7 && memoryLocation >= 0)
	{
		unsigned char characterMapData[8];
		int i;

		/* Populate the character map data array */
		for( i = 0 ; i < 8 ; i++){
			characterMapData[i] = characterMap[i];
		}

		unsigned char data[2], dataEnd[1] = { 0xFF };
		data[0] = 0x40;
		data[1] = memoryLocation;

		g_bytesWritten = write(g_fd, &data, sizeof(data));

		if (g_bytesWritten < 0)
		{
			perror("Write failed - ");
			return -1;
		}

		g_bytesWritten = write(g_fd, &characterMapData, sizeof(characterMapData));

		if (g_bytesWritten < 0)
		{
			perror("Write failed - ");
			return -1;
		}

		g_bytesWritten = write(g_fd, &dataEnd, sizeof(dataEnd));

		if (g_bytesWritten < 0)
		{
			perror("Write failed - ");
			return -1;
		}
		return 0;
	}
	else
		return -1;
}

static int createAwithDots(void)
{
	if(createCharacter(0x00, aWithDots) < 0){
		return -1;
	}
	else
		return 0;
}

static int createOwithDots(void)
{
	if(createCharacter(0x01, oWithDots) < 0){
		return -1;
	}
	else
		return 0;
}

static int createCapitalAwithDots(void)
{
	if(createCharacter(0x02, capitalAwithDots) < 0){
		return -1;
	}
	else
		return 0;
}

static int createCapitalOwithDots(void)
{
	if(createCharacter(0x03, capitalOwithDots) < 0){
		return -1;
	}
	else
		return 0;
}

/* Clears LCD display */
int clear_LCD(void)
{
	unsigned char data[2];
	data[0] = 4;
	data[1] = 0xFF;

	g_bytesWritten = write(g_fd, &data, sizeof(data));

	if (g_bytesWritten < 0)
	{
		perror("Write failed - ");
		return -1;
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

	g_bytesWritten = write(g_fd, &data, sizeof(data));

    if (g_bytesWritten < 0)
    {
    	perror("Write failed - ");
        return -1;
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

	g_bytesWritten =  write(g_fd, &data, sizeof(data));

    if (g_bytesWritten < 0)
    {
    	perror("Write failed - ");
    	return -1;
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

	g_bytesWritten = write(g_fd, &data, sizeof(data));

    if (g_bytesWritten < 0)
    {
    	perror("Write failed - ");
        return -1;
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

	g_bytesWritten = write(g_fd, &data, sizeof(data));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

/* Print long string to a lcd */
int printLongString_LCD(const char *pstring, const size_t len)
{
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
				data[i] = *pstring++;
			}

			data[len+1] = '\0';
			data[len+2] = 0xFF;

			g_bytesWritten = write(g_fd, &data, sizeof(data));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
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
				data[i] = *pstring++;
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_bytesWritten = write(g_fd, &data, sizeof(data));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < (len-15) ; i++)
			{
				data2[i] = *pstring++;
			}

			data2[(len-15)] = '\0';
			data2[(len-15)+1] = 0xFF;

			g_bytesWritten = write(g_fd, &data2, sizeof(data2));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
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
				data[i] = *pstring++;
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_bytesWritten = write(g_fd, &data, sizeof(data));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data2[i] = *pstring++;
			}

			data2[17] = '\0';
			data2[18] = 0xFF;

			g_bytesWritten = write(g_fd, &data2, sizeof(data2));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(3,1);

			data3[0] = 0x01;
			for(i = 1 ; i < (len-31) ; i++)
			{
				data3[i] = *pstring++;
			}
			data3[len-31] = '\0';
			data3[len-31+1] = 0xFF;

			g_bytesWritten = write(g_fd, &data3, sizeof(data3));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
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
				data[i] = *pstring++;
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_bytesWritten = write(g_fd, &data, sizeof(data));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data2[i] = *pstring++;
			}

			data2[17] = '\0';
			data2[18] = 0xFF;

			g_bytesWritten = write(g_fd, &data2, sizeof(data2));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(3,1);

			data3[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data3[i] = *pstring++;
			}
			data3[17] = '\0';
			data3[18] = 0xFF;

			g_bytesWritten = write(g_fd, &data3, sizeof(data3));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(4,1);

			data4[0] = 0x01;
			for(i = 1 ; i < (len-47) ; i++)
			{
				data4[i] = *pstring++;
			}
			data4[(len-47)] = '\0';
			data4[(len-47)+1] = 0xFF;

			g_bytesWritten = write(g_fd, &data4, sizeof(data4));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}
		} else {
			setCursor_LCD(2,1);
			char tooLongString[] = "Too long string!";
			stringLength = strlen(tooLongString);
			printString_LCD(tooLongString, stringLength);
			printf("Too long string\n");
			sleep(2);
		}

	return 0;
}

/* Print short string to a lcd */
int printString_LCD(const char *pstring, const size_t len)
{
	unsigned char data[len+3];
	size_t i;
	data[0] = 0x01;

	for(i = 1 ; i < (len+1) ; i++)
	{
		data[i] = *pstring++;
	}

	data[len+1] = '\0';
	data[len+2] = 0xFF;

	g_bytesWritten = write(g_fd, &data, sizeof(data));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

/* Print distance to a lcd */
static int printDistance_LCD(const char *pstring, const size_t len)
{
	unsigned char data[len+5];
	size_t i;
	data[0] = 0x01;

	for(i = 1 ; i < (len+1) ; i++)
	{
		data[i] = *pstring++;
	}

	data[len+1] = 'm';
	data[len+2] = 'm';
	data[len+3] = '\0';
	data[len+4] = 0xFF;

	g_bytesWritten = write(g_fd, &data, sizeof(data));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

/* Convert int to string and print it to a lcd screen */
int printDistance(const int distance)
{
	char dBuf[5];
	snprintf(dBuf, sizeof(dBuf), "%d", distance);

	stringLength = strlen(dBuf);
	printDistance_LCD(dBuf, stringLength);

	return 0;
}

/* Print float speed to a lcd */
static int printSpeed_LCD(const char *pstring, const size_t len)
{
	unsigned char data[len+7];
	size_t i;
	data[0] = 0x01;

	for(i = 1 ; i < (len+1) ; i++)
	{
		data[i] = *pstring++;
	}

	data[len+1] = 'k';
	data[len+2] = 'm';
	data[len+3] = '/';
	data[len+4] = 'h';
	data[len+5] = '\0';
	data[len+6] = 0xFF;

	g_bytesWritten = write(g_fd, &data, sizeof(data));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

/* Convert float to string and print it to a lcd screen */
int printSpeed(const float speed)
{
	if(speed >= 100)
	{
		char buf[7];
		snprintf(buf, sizeof(buf), "%.2f", speed);

		stringLength = strlen(buf);
		printSpeed_LCD(buf, stringLength);

		return 0;
	}
	else if(speed < 100)
	{
		char buf[6];
		snprintf(buf, sizeof(buf), "%.2f", speed);

		stringLength = strlen(buf);
		printSpeed_LCD(buf, stringLength);

		return 0;
	}

	return 0;
}

int printConnect(void)
{
	setCursor_LCD(2,3);
	const char *strng = "CONNECTED!!";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);
	sleep(2);
	clear_LCD();
	return 0;
}

int printDisconnect(void)
{
	setCursor_LCD(2,2);
	const char *strng = "DISCONNECTED!!";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);
	sleep(2);
	clear_LCD();
	return 0;
}

int printAlreadyConnected(void)
{
	clear_LCD();
	setCursor_LCD(2,5);
	const char *strng = "ALREADY";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);

	setCursor_LCD(3,4);
	const char *strng2 = "CONNECTED!";
	stringLength = strlen(strng2);
	printString_LCD(strng2, stringLength);
	sleep(2);
	clear_LCD();

	return 0;
}
