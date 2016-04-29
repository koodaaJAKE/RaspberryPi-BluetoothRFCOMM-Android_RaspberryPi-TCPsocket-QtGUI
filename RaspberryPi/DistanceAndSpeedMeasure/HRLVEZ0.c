/*
 * hrlvez0.cpp
 *
 * This library is for reading Serial interface HRLVEZ0 Ultrasonic sensor with Raspberry Pi.
 */

#include "HRLVEZ0.h"
#include "LCD.h"
#include "TCP_Socket.h"

/* Static global variable of serial file descriptor */
static int uart0_filestream;

int setupHRLVEZ0_Serial(void)
{
	int error;
	struct termios options;

	/* Open in non blocking read mode */
	uart0_filestream = open("/dev/ttyAMA0", O_RDONLY | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if (uart0_filestream == -1)
	{
		perror("Error - Unable to open UART!\n");
	}

	/* Get the current configuration of the serial interface */
	error = tcgetattr(uart0_filestream, &options);
	if(error == -1)
	{
		perror("tcgetattr error!\n");
	}

	//Set the configuration
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; 	//Baud rate 9600, Character size mask CS8, Ignore modem control lines, Enable receiver
	options.c_iflag = IGNPAR; //Ignore framing errors and parity errors.
	options.c_oflag = 0;
	options.c_lflag = 0;

	 /*
	 * Flushes (discards) not-send data (data still in the UART send buffer) and/or flushes (discards)
	 * received data (data already in the UART receive buffer).
	*/
	error = tcflush(uart0_filestream, TCIFLUSH);
	if(error == -1)
	{
		perror("tcflush error!\n");
	}

	/* Apply the configuration */
	error = tcsetattr(uart0_filestream, TCSANOW, &options);
	if(error == -1)
	{
		perror("tcflush error!\n");
	}
	return 0;
}

int serialHRLVEZ0_Close(void)
{
    int statusVal;
    statusVal = close(uart0_filestream);

    if(statusVal < 0)
    {
    	perror("Could not close serial device");
    	return -1;
    }
    return statusVal;
}

int measureHRLVEZ0_Data(HRLVEZ0_Data_t *HRLVEZ0_Data)
{
	int i = 0;

	while(i < 2)
	{
		//----- CHECK FOR ANY RX BYTES -----
		if (uart0_filestream != -1)
		{
			//Structure for serial data readings of the sensor
			data_frame_t serial_data;

			/* Read first byte and discard all characters until "R", start of the frame is read */
			unsigned char start;
			int rx_length = read(uart0_filestream, &start, sizeof(start));

			if (rx_length < 0)
			{
				//No bytes to receive
			}
			else if (rx_length == 0)
			{
				//No data waiting
			}
			else
			{
				//Bytes received
				if(start == 'R')
				{
					/* Read next characters one by one and check if it's a digit */
                    rx_length = read(uart0_filestream, &serial_data.data1, sizeof(serial_data.data1));
                    if(rx_length == -1)
                    {
                    	perror("Read failed\n");
                    }

                    if(serial_data.data1 >= '0' &&  serial_data.data1 <= '9')
                    {
                    	rx_length = read(uart0_filestream, &serial_data.data2, sizeof(serial_data.data2));
                        if(rx_length == -1)
                        {
                        	perror("Read failed2\n");
                        }

                        if(serial_data.data2 >= '0' && serial_data.data2 <= '9')
                        {
                        	rx_length = read(uart0_filestream, &serial_data.data3, sizeof(serial_data.data3));
                            if(rx_length == -1)
                            {
                            	perror("Read failed3\n");
                            }

                            if(serial_data.data3 >= '0' && serial_data.data3 <= '9')
                            {
                            	rx_length = read(uart0_filestream, &serial_data.data4, sizeof(serial_data.data4));
                                if(rx_length == -1)
                                {
                                	perror("Read failed4\n");
                                }

                                if(serial_data.data4 >= '0' && serial_data.data4 <= '9')
                                {
                                	/* Read characters until carriage return is read */
        	                        do
                	                {
        	                        	rx_length = read(uart0_filestream, &serial_data.end, sizeof(serial_data.end));
        	                            if(rx_length == -1)
        	                            {
        	                            	perror("Read failed5\n");
        	                            }

	                                }
        	                        while(serial_data.end != 13);
                                 }
                             }
                         }
                    }

					serial_data.end = '\0';

					/* Convert string distance to long int */
                    HRLVEZ0_Data->distance = strtol ((const char*)&serial_data, NULL, 10);
                    //printf("\n Integer distance: %ld mm \n", HRLVEZ0_Data->distance);

                    /*
					setCursor_LCD(2,3);
					char strng[] = "Distance is:";
					size_t l = strlen(strng);
					printString_LCD(strng, l);

					setCursor_LCD(3,6);
					printDistance(HRLVEZ0_Data->distance);
					*/

					//Take distances only between 300mm and 5000mm for speed calculation
					if(HRLVEZ0_Data->distance > 300 && HRLVEZ0_Data->distance < 5000)
					{
						long tempdistance[2];
						tempdistance[i] = HRLVEZ0_Data->distance;

						if(i != 0)
						{
							HRLVEZ0_Data->prevdistance = tempdistance[i-1];

							//Object moving towards the sensor
							if(HRLVEZ0_Data->prevdistance > tempdistance[i])
							{
								//printf("Moving towards the sensor\n");

								HRLVEZ0_Data->speed = (((float)HRLVEZ0_Data->prevdistance - (float)HRLVEZ0_Data->distance) / 100); //100ms measuring cycle
								HRLVEZ0_Data->speed *= 3.6; //conversion to km/h
								//printf("\n Speed is: %.2f km/h\n", HRLVEZ0_Data->speed);
/*
								if(HRLVEZ0_Data->speed > 20.00)
								{
									setCursor_LCD(2,4);
									char strng[] = "Speed is:";
									size_t l = strlen(strng);
									printString_LCD(strng, l);
									setCursor_LCD(3,4);
									printSpeed_LCD(HRLVEZ0_Data->speed);
								}
*/
							}

							//Object moving away from the sensor
							else
							{
								//printf("Moving away from the sensor\n");

								HRLVEZ0_Data->speed = (((float)HRLVEZ0_Data->distance - (float)HRLVEZ0_Data->prevdistance) / 100); //100ms measuring cycle
								HRLVEZ0_Data->speed *= 3.6; //conversion to km/h
								//printf("\n Speed is: %.2f km/h\n", HRLVEZ0_Data->speed);
/*
								if(HRLVEZ0_Data->speed > 20.00)
								{
									setCursor_LCD(2,4);
									char strng[] = "Speed is:";
									size_t l = strlen(strng);
									printString_LCD(strng, l);
									setCursor_LCD(3,4);
									printSpeed_LCD(HRLVEZ0_Data->speed);
								}
*/
							}

						}
					}
					i++;
				}
			}
		}
	}

	if(HRLVEZ0_Data->distance == 300)
		HRLVEZ0_Data->speed = 0;

	return 0;
}




