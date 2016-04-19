/*
 * Bluetooth_RFCOMM.c
 *
 *  Created on: 31.3.2016
 *      Author: root
 */
#include "Bluetooth_RFCOMM.h"
#include "LCD.h"
#include "HRLVEZ0.h"
#include "SerializeDeserialize.h"

int bluetoothRFCOMM_Client(void)
{
	inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i, UserInput;
    char addr[18] = { 0 };
    char addr_array[NUMBER_OF_ADDRESSES][19];
    char name[248] = { 0 };

    /*
     * UUID:
     * 00001101-0000-1000-8000-00805F9B34FB
     */
    uint8_t svc_uuid_int[] = { 0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00,
      0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };

    /*************************************************************************/
    /* Retrieve the resource number of the first available Bluetooth adapter */
    /*************************************************************************/
    dev_id = hci_get_route(NULL);
    if (dev_id < 0) {
        perror("No Bluetooth Adapter Available");
        exit(1);
    }

    /*
     * Once the program has chosen which adapter to use in scanning for nearby devices,
     * it must allocate resources to use that adapter. This can be done with the
     * hci_open_dev function.
     */
    sock = hci_open_dev( dev_id );
    if (sock < 0) {
        perror("hci_open_dev error: \n");
        exit(1);
    }

    /**************************************************************************/
    /* If flags is set to IREQ_CACHE_FLUSH, then the cache of previously      */
    /* detected devices is flushed before performing the current inquiry.     */
    /* Otherwise, if flags is set to 0, then the results of previous          */
    /* inquiries may be returned, even if the devices aren't in range anymore.*/
    /*                                                                        */
    /* The inquiry lasts for at most 1.28 * len seconds, and at most max_rsp  */
    /* devices will be returned in the output parameter ii, which must be     */
    /* large enough to accommodate max_rsp results. We suggest using a        */
    /* max_rsp of 255 for a standard 10.24 second inquiry.                    */
    /**************************************************************************/
    len = 2;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    /*************************************************************************/
    /* Performs a Bluetooth device discovery and returns a list of detected  */
    /* devices and some basic information about them in the variable ii.     */
    /* On error,it returns -1 and sets errno accordingly.                    */
    /*************************************************************************/
    printf("Scanning.........\n");
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 )
    {
    	perror("hci_inquiry error! \n");
    	exit(1);
    }

    /******************************************************************************/
    /* Once a list of nearby Bluetooth devices and their addresses has been found,*/
    /* the program determines the user-friendly names associated with those       */
    /* addresses and presents them to the user. The hci_read_remote_name function */
    /* is used for this purpose.                                                  */
    /******************************************************************************/
    if(num_rsp > 0)
    {
    	for (i = 0; i < num_rsp; i++) {
    		ba2str(&(ii+i)->bdaddr, addr);
    		memset(name, 0, sizeof(name));

    		if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name),
    				name, 0) < 0)
    			strcpy(name, "[unknown]");

    		/***************************************/
    		/* Copy the address string to an array */
    		/* of strings of addresses             */
    		/***************************************/
    		strcpy(addr_array[i], addr);
    		printf("%d. Device: %s  %s \n", i+1, addr, name);
    	}

    	printf("\n*********************************************\n");
    	printf("Choose which Bluetooth device connect to:\n");
    	scanf("%d", &UserInput);
    	printf("\n");

        /*
         * Start the Bluetooth RFCOMM Client service and pass it the chosen Bluetooth device address
         */
        bluetoothRFCOMM_ClientConnect(addr_array[UserInput-1], svc_uuid_int);
    }
    else
    {
    	printf("No Bluetooth devices found\n");
    }

    free(ii);
    close(sock);
    return 0;
}

int bluetoothRFCOMM_ClientConnect(const char *target_addr, uint8_t svc_uuid_int[])
{
	int bytes_read, bytes_sent, s, channel, status;
    uuid_t svc_uuid;
    //bdaddr_t target;
    sdp_list_t *response_list = NULL, *search_list, *attrid_list;
    sdp_session_t *session = 0;
    bool socketCloseFlag = false;
	HRLVEZ0_Data_t bluetoothHRLVEZ0_Data;
	unsigned char *serializationPtr;
	struct sockaddr_rc addr = { 0 };

    str2ba( target_addr, &addr.rc_bdaddr );

    //Connect to the SDP server running on the remote machine
    session = sdp_connect( BDADDR_ANY, &addr.rc_bdaddr, SDP_RETRY_IF_BUSY );
    if (session == NULL) {
      perror("Failed to find target \n");
      return 1;
    }

    //Specify the UUID of the application we're searching for
    sdp_uuid128_create( &svc_uuid, svc_uuid_int );
    search_list = sdp_list_append( NULL, &svc_uuid );

    //Specify that we want a list of all the matching applications' attributes
    uint32_t range = 0x0000ffff;
    attrid_list = sdp_list_append( NULL, &range );

    //Get a list of service records that have UUID 0xabcd
    if(sdp_service_search_attr_req( session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &response_list) < 0){
    	perror("SDP service search failed \n");
    	sdp_close(session);
    	return 1;
    }

    sdp_list_t *r = response_list;

    //Go through each of the service records
    for (; r; r = r->next ) {
        sdp_record_t *rec = (sdp_record_t*) r->data;
        sdp_list_t *proto_list;

        //Get a list of the protocol sequences
        if( sdp_get_access_protos( rec, &proto_list ) == 0 ) {
        sdp_list_t *p = proto_list;

        //Go through each protocol sequence
        for( ; p ; p = p->next ) {
            sdp_list_t *pds = (sdp_list_t*)p->data;

            //Go through each protocol list of the protocol sequence
            for( ; pds ; pds = pds->next ) {

                //Check the protocol attributes
                sdp_data_t *d = (sdp_data_t*)pds->data;
                int proto = 0;
                for( ; d; d = d->next ) {

                    switch( d->dtd ) {

                        case SDP_UUID16:
                        case SDP_UUID32:
                        case SDP_UUID128:

                            proto = sdp_uuid_to_proto( &d->val.uuid );
                            break;

                        case SDP_UINT8:

                            if( proto == RFCOMM_UUID ) {
                                printf("RFCOMM channel: %d\n",d->val.int8);
                                channel = d->val.int8;
                            }
                            break;
                    }
                }
            }
            sdp_list_free( (sdp_list_t*)p->data, 0 );
        }
        sdp_list_free( proto_list, 0 );

        }

        printf("Found service record 0x%x\n", rec->handle);
        sdp_record_free( rec );
    }

    //Allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    //Set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) channel;

    //Connect to the server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if(status < 0){
    	perror("Couldn't connect\n");
    	return 1;
    }
    printf("Connected to socket: %d \n", s);

	/*
	 * Wait for data until the socketCloseFlag is set to end the transaction and close the socket connection
	 */
	while(socketCloseFlag == false)
	{
		unsigned char recvBuffer[128] = { 0 };
		unsigned char sendBuffer[128] = { 0 };

		printf("Waiting bytes to read.............\n");
		bytes_read = read(s, recvBuffer, sizeof(recvBuffer));
		if(bytes_read <= 0){
			perror("read() failed: \n");
			return 1;
		}
		else
			printf("Bytes received: %d	command: %X\n", bytes_read, recvBuffer[0]);

		switch(recvBuffer[0]){

			case BLUETOOTH_SOCKET_CLOSE_COMMAND:

				socketCloseFlag = true;
				printDisconnect();
				printf("Closing the socket...\n");
				sleep(1);
				break;

			case PRINT_HELLO:

				; //Empty statement needed here
				char strng[] = "Hello from the Samsung GalaxyS5!";
				size_t l = strlen(strng);
				printLongString_LCD(strng, l);
				sleep(1);
				clear_LCD();
				break;

			case READ_HRLVEZ0_DATA:

				//Read the HRLVEZ0 sensor data
				measureHRLVEZ0_Data(&bluetoothHRLVEZ0_Data);

				//Serialize data before sending it through socket
				serializationPtr = Serialize_Struct(sendBuffer, &bluetoothHRLVEZ0_Data);
				sendBuffer[12] = FRAME_END_CHAR;
				bytes_sent = write(s, sendBuffer, serializationPtr - sendBuffer + 1);
				if(bytes_sent <= 0)
				{
					perror("Write failed!\n");
					socketCloseFlag = true;
					break;
				}
				else{
					printf("Bytes sent: %d\n", bytes_sent);

					int k;
					for(k = 0 ; k <= 13 ; k++){
						printf("buf[%d]: %X \t", k, sendBuffer[k]);
					}
					printf("\n");
				}
				break;

			/*
			 * Read random text from the Android end and print it to the terminal
			 */
			case RANDOM_TEXT:

				bytes_read = read(s, recvBuffer, sizeof(recvBuffer));
				if(bytes_read <= 0){
					perror("read() failed: \n");
					break;
				}
				else {
					printf("Bytes received: %d \n", bytes_read);

					char randomTextArray[bytes_read+1];
					int i;

					/* Move received data to another array with proper size and data type */
					for(i = 0 ; i <= bytes_read ; i++){
						randomTextArray[i] = recvBuffer[i];
					}

					/* Print it to a LCD screen */
					size_t strLength = strlen(randomTextArray);
					printLongString_LCD(randomTextArray, strLength);
					sleep(1);
					clear_LCD();
				}
				break;

			case CLEAR_SCREEN:
				clear_LCD();
				break;
		}//Switch
	}//While

    sdp_close(session);
    close(s);
    return 0;
}

int bluetoothRFCOMM_Server(void)
{
	int port = 3, result, sock, client, bytes_read, bytes_sent;
	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	char buffer[1024] = { 0 };
	socklen_t opt = sizeof(rem_addr);
	bool socketCloseFlag = false;
	HRLVEZ0_Data_t bluetoothHRLVEZ0_Data;
	unsigned char *serializationPtr;

	//Local Bluetooth adapter
	loc_addr.rc_family = AF_BLUETOOTH;
	loc_addr.rc_bdaddr = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) port;

	//Register service
	sdp_session_t *session = registerService(port);

	//Allocate socket
	sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	printf("socket() returned %d\n", sock);

	//Bind socket to port 3 of the first available
	result = bind(sock, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
	printf("bind() on channel %d returned %d\n", port, result);

	//Put socket into listening mode
	result = listen(sock, 1);
	printf("listen() returned %d\n", result);

	//Accept one connection
	printf("Waiting for client to connect...\n");
	client = accept(sock, (struct sockaddr *)&rem_addr, &opt);
	printf("accept() returned %d\n", client);

	ba2str(&rem_addr.rc_bdaddr, buffer);
	fprintf(stderr, "Accepted connection from %s\n", buffer);
	memset(buffer, 0, sizeof(buffer));


	/*
	 * Wait for data until the socketCloseFlag is set to end the transaction and close the socket connection
	 */
	while(socketCloseFlag == false)
	{
		unsigned char recvBuffer[128] = { 0 };
		unsigned char sendBuffer[128] = { 0 };

		printf("Waiting bytes to read.............\n");
		bytes_read = read(client, recvBuffer, sizeof(recvBuffer));
		if(bytes_read <= 0){
			perror("read() failed: \n");
			break;
		}
		else
			printf("Bytes received: %d	command: %X\n", bytes_read, recvBuffer[0]);

		switch(recvBuffer[0]){

			case BLUETOOTH_SOCKET_CLOSE_COMMAND:

				socketCloseFlag = true;
				printDisconnect();
				printf("Closing the socket...\n");
				sleep(1);
				break;

			case PRINT_HELLO:

				; //Empty statement needed here
				char strng[] = "Hello from the Samsung GalaxyS5!";
				size_t l = strlen(strng);
				printLongString_LCD(strng, l);
				sleep(1);
				clear_LCD();
				break;

			case READ_HRLVEZ0_DATA:

				//Read the HRLVEZ0 sensor data
				measureHRLVEZ0_Data(&bluetoothHRLVEZ0_Data);

				//Serialize data before sending it through socket
				serializationPtr = Serialize_Struct(sendBuffer, &bluetoothHRLVEZ0_Data);
				sendBuffer[12] = FRAME_END_CHAR;
				bytes_sent = write(client, sendBuffer, serializationPtr - sendBuffer + 1);
				if(bytes_sent <= 0)
				{
					perror("Write failed!\n");
					socketCloseFlag = true;
					break;
				}
				else {
					printf("Bytes sent: %d\n", bytes_sent);
					int k;

					for(k = 0 ; k < 13 ; k++){
						printf("buf[%d]: %X \t",k, sendBuffer[k]);
					}
					printf("\n");
				}
				break;

			/*
			 * Read random text from the Android end and print it to the terminal
			*/
			case RANDOM_TEXT:

				bytes_read = read(client, recvBuffer, sizeof(recvBuffer));
				if(bytes_read <= 0){
					perror("read() failed: \n");
					exit(1);
				}
				else {
					printf("Bytes received: %d \n", bytes_read);

					char randomTextArray[bytes_read];
					int i;

					/* Move received data to another array with proper size and data type */
					for(i = 0 ; i <= bytes_read ; i++){
						randomTextArray[i] = recvBuffer[i];
					}

					/* Print it to a LCD screen */
					size_t strLength = strlen(randomTextArray);
					printLongString_LCD(randomTextArray, strLength);
					sleep(1);
					clear_LCD();
				}
				break;

			case CLEAR_SCREEN:
				clear_LCD();
				break;
		}//Switch
	}//While

	sdp_close(session);
	close(client);
	return 0;
}

/*
 * Registers SDP service for Bluetooth RFCOMM server connection
 */
sdp_session_t *registerService(uint8_t rfcomm_channel)
{

	/* A 128-bit number used to identify this service. The words are ordered from most to least
	* significant, but within each word, the octets are ordered from least to most significant.
	* For example, the UUID represneted by this array is 00001101-0000-1000-8000-00805F9B34FB. (The
	* hyphenation is a convention specified by the Service Discovery Protocol of the Bluetooth Core
	* Specification, but is not particularly important for this program.)
	*
	* This UUID is the Bluetooth Base UUID and is commonly used for simple Bluetooth applications.
	* Regardless of the UUID used, it must match the one that the Jake's Android app is searching
	* for.
	*/
	uint32_t svc_uuid_int[] = { 0x01110000, 0x00100000, 0x80000080, 0xFB349B5F };
	const char *service_name = "Jake's RFCOMM Bluetooth server";
	const char *svc_dsc = "A RFCOMM Bluetooth server that interfaces with the Jake's Android app";
	const char *service_prov = "Jake";

	uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid,
	       svc_class_uuid;
	sdp_list_t *l2cap_list = 0,
	            *rfcomm_list = 0,
	             *root_list = 0,
	              *proto_list = 0,
	               *access_proto_list = 0,
	                *svc_class_list = 0,
	                 *profile_list = 0;
	sdp_data_t *channel = 0;
	sdp_profile_desc_t profile;
	sdp_record_t record = { 0 };
	sdp_session_t *session = 0;

	//Set the general service ID
	sdp_uuid128_create(&svc_uuid, &svc_uuid_int);
	sdp_set_service_id(&record, svc_uuid);

	char str[256] = "";
	sdp_uuid2strn(&svc_uuid, str, 256);
	printf("Registering UUID %s\n", str);

	//Set the service class
	sdp_uuid16_create(&svc_class_uuid, SERIAL_PORT_SVCLASS_ID);
	svc_class_list = sdp_list_append(0, &svc_class_uuid);
	sdp_set_service_classes(&record, svc_class_list);

	//Set the Bluetooth profile information
	sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
	profile.version = 0x0100;
	profile_list = sdp_list_append(0, &profile);
	sdp_set_profile_descs(&record, profile_list);

	//Make the service record publicly browsable
	sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
	root_list = sdp_list_append(0, &root_uuid);
	sdp_set_browse_groups(&record, root_list);

	//Set l2cap information
	sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
	l2cap_list = sdp_list_append(0, &l2cap_uuid);
	proto_list = sdp_list_append(0, l2cap_list);

	//Register the RFCOMM channel for RFCOMM sockets
	sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
	channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
	rfcomm_list = sdp_list_append(0, &rfcomm_uuid);
	sdp_list_append(rfcomm_list, channel);
	sdp_list_append(proto_list, rfcomm_list);

	access_proto_list = sdp_list_append(0, proto_list);
	sdp_set_access_protos(&record, access_proto_list);

	//Set the name, provider, and description
	sdp_set_info_attr(&record, service_name, service_prov, svc_dsc);

	//Connect to the local SDP server, register the service record,
	//and disconnect
	session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
	sdp_record_register(session, &record, 0);

	//Cleanup
	sdp_data_free(channel);
	sdp_list_free(l2cap_list, 0);
	sdp_list_free(rfcomm_list, 0);
	sdp_list_free(root_list, 0);
	sdp_list_free(access_proto_list, 0);
	sdp_list_free(svc_class_list, 0);
	sdp_list_free(profile_list, 0);

	return session;
}
