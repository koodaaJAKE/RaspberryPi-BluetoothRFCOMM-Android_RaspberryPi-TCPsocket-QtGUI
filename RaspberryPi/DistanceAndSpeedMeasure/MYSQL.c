/*
 * MYSQL.c
 */
#include "MYSQL.h"

/* Static function declarations */
static void mysqlFinishWithError(void);
static void createDataBase(const char *dataBase);

/* Static local variable declarations */
static MYSQL *mysqlConnectionHandle;

static void mysqlFinishWithError(void)
{
	fprintf(stderr, "%s\n", mysql_error(mysqlConnectionHandle));
	mysql_close(mysqlConnectionHandle);
    exit(1);
}

void initializeMySQL(mySqlConstants *p_mySqlConstants)
{
	/* Check the current MySQL client version */
	printf("MySQL client version: %s\n", mysql_get_client_info());

	mysqlConnectionHandle = mysql_init(NULL);
	if(mysqlConnectionHandle != NULL)
		printf("MySQL initialized!\n");
	else
		mysqlFinishWithError();

	//createDataBase(p_mySqlConstants->DATABASE);

	/* Connect to the MySQL database */
	if(mysql_real_connect(mysqlConnectionHandle, p_mySqlConstants->HOST, p_mySqlConstants->USER, p_mySqlConstants->PASSWORD,
	        p_mySqlConstants->DATABASE, 0, NULL, 0) != NULL)
		printf("Connected to the MySQL database!\n");
	else
		mysqlFinishWithError();

	/* Create table */
	if(mysql_query(mysqlConnectionHandle, "DROP TABLE IF EXISTS DistanceAndSpeed") != 0)
		mysqlFinishWithError();

	if(mysql_query(mysqlConnectionHandle, "CREATE TABLE DistanceAndSpeed(Id INT, Distance INT, PreviousDistance INT, Speed FLOAT)") == 0)
		printf("Table created!\n");
	else
		mysqlFinishWithError();
}

static void createDataBase(const char *dataBase)
{
	char dataBaseName[128];
	snprintf(dataBaseName, 128, "CREATE DATABASE IF NOT EXISTS %s", dataBase);

	if(mysql_query(mysqlConnectionHandle, dataBaseName) == 0)
		printf("Database created!\n");
	else
		mysqlFinishWithError();
}

void insertDataToTable(int id, thread_data_t *sensorData)
{
    char buffer[BUFSIZE];

    if(snprintf(buffer, BUFSIZE, "INSERT INTO DistanceAndSpeed(Id, Distance, PreviousDistance, Speed) VALUES(%d, %ld, %ld, %f);",
    		id, sensorData->distance, sensorData->prevDistance, sensorData->speed) < BUFSIZE)
        mysql_query(mysqlConnectionHandle, buffer);
    else
    	mysqlFinishWithError();
}
