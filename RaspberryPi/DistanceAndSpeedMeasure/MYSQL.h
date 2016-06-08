/*
 * MYSQL.h
 */

#ifndef MYSQL_H_
#define MYSQL_H_

#include <mysql/my_global.h>
#include <mysql/mysql.h>
#include "thread.h"

#define BUFSIZE 128

typedef struct mySqlDetails
{
	const char *HOST;
	const char *USER;
	const char *PASSWORD;
	const char *DATABASE;
}mySqlConstants;

/* Function prototypes */
void initializeMySQL(mySqlConstants *p_mySqlConstants);
void insertDataToTable(int id, thread_data_t *sensorData);

#endif /* MYSQL_H_ */
