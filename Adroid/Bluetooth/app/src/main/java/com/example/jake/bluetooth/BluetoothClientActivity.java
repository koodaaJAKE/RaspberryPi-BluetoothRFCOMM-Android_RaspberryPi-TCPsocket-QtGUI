package com.example.jake.bluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

public class BluetoothClientActivity extends Activity implements View.OnClickListener {

    //Bluetooth socket transfer commands
    public static final byte SOCKET_CLOSE_COMMAND     = 0x01;
    public static final byte HELLO_COMMAND            = 0x02;
    public static final byte READ_SENSOR_DATA_COMMAND = 0x03;
    public static final byte RANDOM_TEXT_COMMAND      = 0x04;
    public static final byte CLEAR_SCREEN             = 0x05;

    //Handler constants
    public static final int SUCCESS_CONNECT          = 0;
    public static final int TRANSFER_THREAD_CREATED  = 1;
    public static final int MESSAGE_READ             = 2;
    public static final int BYTES_AVAILABLE          = 3;

    //Bluetooth MAC address Intent extra returned from other activity
    public static String BLUETOOTH_DEVICE_ADDRESS = "device_address";

    private static final UUID bluetoothUUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");

    //public static final String raspberryPiBluetoothAddress = "5C:F3:70:0D:4C:3B";

    private BluetoothAdapter mBluetoothAdapter;
    private String bluetoothAddress;
    private ClientThread clientThread;
    private BluetoothTransferThread clientTransferThread;
    Button Connect, SendSomeText, Hello, CloseConnection, ReadSensorData, ClearText;
    TextView distanceView, velocityView;
    EditText randomEditText;
    private String randomText;

    //Flags to check the state of the threads
    private boolean isConnectThreadRunning = false, isTransferThreadRunning = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_client);

        //Set title to "Bluetooth Client"
        setTitle(R.id.client);

        initialize();

    }

    private void initialize() {

        Connect = (Button)findViewById(R.id.ConnectButton);
        SendSomeText = (Button)findViewById(R.id.SendSomeTextButton);
        Hello = (Button)findViewById(R.id.HelloButton);
        CloseConnection = (Button)findViewById(R.id.CloseConnectionButton);
        ReadSensorData = (Button)findViewById(R.id.ReadSensorDataButton);
        ClearText = (Button)findViewById(R.id.ClearTextButton);

        distanceView = (TextView)findViewById(R.id.distanceValueView);
        velocityView = (TextView)findViewById(R.id.velocityValueView);

        randomEditText = (EditText)findViewById(R.id.editText);

        Connect.setOnClickListener(this);
        SendSomeText.setOnClickListener(this);
        Hello.setOnClickListener(this);
        CloseConnection.setOnClickListener(this);
        ReadSensorData.setOnClickListener(this);
        ClearText.setOnClickListener(this);

        //Get the BluetoothAdapter from singleton class
        BluetoothSingleton bSingleton = BluetoothSingleton.getInstance();
        mBluetoothAdapter = bSingleton.getBluetoothAdapter();

        //Get the Bluetooth device address from main activity for client connection
        Intent intent = getIntent();
        bluetoothAddress = intent.getStringExtra(BLUETOOTH_DEVICE_ADDRESS);

        Log.d("address", bluetoothAddress);
    }

    @Override
    public void onClick(View v) {

        switch (v.getId()) {

            case R.id.ConnectButton:

                if(!isConnectThreadRunning && !isTransferThreadRunning) {

                    // Get the BluetoothDevice object
                    BluetoothDevice clientDevice = mBluetoothAdapter.getRemoteDevice(bluetoothAddress);

                    //Create clientThread instance from a ConnectThread class
                    clientThread = new ClientThread(clientDevice);
                    //Start the client thread and if success, the transfer thread starts also
                    clientThread.start();
                    break;
                }
                Toast.makeText(this, "Threads already running", Toast.LENGTH_SHORT).show();
                break;

            case R.id.SendSomeTextButton:
                if (isConnectThreadRunning && isTransferThreadRunning){

                    //Get the random text from the EditText field
                    randomText = randomEditText.getText().toString();
                    if(!randomText.matches("")) {
                        sendSomeRandomText();
                    }
                    else
                        Toast.makeText(this, "Type some text first", Toast.LENGTH_SHORT).show();
                }
                break;

            case R.id.ReadSensorDataButton:

                //Clear the TextViews
                distanceView.setText("");
                velocityView.setText("");

                if (isConnectThreadRunning && isTransferThreadRunning){
                    readSensorData();
                }
                break;

            case R.id.HelloButton:

                if(isTransferThreadRunning && isConnectThreadRunning){
                    printHello();
                }
                break;

            case R.id.CloseConnectionButton:

                if(isTransferThreadRunning && isConnectThreadRunning){
                    closeTheConnection();
                }
                break;

            case R.id.ClearTextButton:
                randomEditText.setText("");
                clearScreen();
                break;
        }
    }

    /**
     * Handler for handling the messages of the transfer thread
     */
    private final Handler threadHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);

            switch (msg.what){

                case SUCCESS_CONNECT:
                    isConnectThreadRunning = true;
                    isTransferThreadRunning = true;
                    Toast.makeText(BluetoothClientActivity.this, "Socket connection created", Toast.LENGTH_SHORT).show();
                    //Start the transfer thread
                    clientTransferThread = new BluetoothTransferThread((BluetoothSocket)msg.obj, BluetoothClientActivity.this, threadHandler);
                    clientTransferThread.start();
                    break;

                case TRANSFER_THREAD_CREATED:
                    Toast.makeText(BluetoothClientActivity.this, "Transfer thread created", Toast.LENGTH_SHORT).show();
                    break;

                case MESSAGE_READ:

                    Toast.makeText(BluetoothClientActivity.this,"Received data from the socket", Toast.LENGTH_SHORT).show();

                    int[] receivedData = (int[])msg.obj;
                    int distance, intVelocity;
                    double velocity;

                    //Parse the received data
                    distance = fromBytesToInt(receivedData, false);
                    intVelocity = fromBytesToInt(receivedData, true);
                    velocity = (double)intVelocity;

                    //Set the precision of double to be 1 decimal
                    Double truncatedVelocity = new Double(velocity);
                    Double truncatedDouble = new BigDecimal(truncatedVelocity)
                            .setScale(1, BigDecimal.ROUND_HALF_UP)
                            .doubleValue();

                    String doubleToString = Double.toString(truncatedDouble);
                    String intToString = Integer.toString(distance);

                    velocityView.setText(doubleToString);
                    distanceView.setText(intToString);

                    break;

            }
        }
    };

    private void printHello(){
        byte[] helloByte = new byte[1];
        helloByte[0] = HELLO_COMMAND;
        clientTransferThread.write(helloByte);
    }

    private void closeTheConnection(){
        byte[] closeByte = new byte[1];
        closeByte[0] = SOCKET_CLOSE_COMMAND;
        clientTransferThread.write(closeByte);
        clientTransferThread.cancel();
        clientThread.cancel();

        //And jump back to the main activity
        Intent goBackToMainActivity = new Intent(this, MainActivity.class);
        goBackToMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(goBackToMainActivity);
    }

    private void readSensorData(){
        byte[] readSensorDataByte = new byte[1];
        readSensorDataByte[0] = READ_SENSOR_DATA_COMMAND;
        clientTransferThread.write(readSensorDataByte);

    }

    private void sendSomeRandomText(){

        byte[] sendSomeRandomTextByte = new  byte[1];
        sendSomeRandomTextByte[0] = RANDOM_TEXT_COMMAND;
        clientTransferThread.write(sendSomeRandomTextByte);

        //Create a ArrayList and populate it with the random string of the EditText field
        List<Byte> b1 = new ArrayList<>();
        for(byte b : randomText.getBytes()) {
            b1.add(b);
        }

        //Convert the ArrayList to byte array
        byte[] randomTextArray = new byte[b1.size()];
        for(int i = 0 ; i < b1.size(); i++) {
            randomTextArray[i] = b1.get(i);
        }

        //Send the text through Bluetooth socket
        clientTransferThread.write(randomTextArray);

        //Clear the EditText field
        randomEditText.setText("");
    }

    private void clearScreen(){
        byte[] clearScreenByte = new byte[1];
        clearScreenByte[0] = CLEAR_SCREEN;
        clientTransferThread.write(clearScreenByte);
    }

    //Method to parse the data
    private int fromBytesToInt(int[] bytes, boolean tf){
        int bytesToInt;
        if(tf) {
            //Gets the float velocity from the buffer
            return bytesToInt = bytes[4] << 24 | bytes[5] << 16 | bytes[6] << 8 | bytes[7];
        }else{
            //Gets the int distance from the buffer
            return bytesToInt = bytes[8] << 24 | bytes[9] << 16 | bytes[10] << 8 | bytes[11];
        }
    }

    private class ClientThread extends Thread {
        private final BluetoothSocket mmSocket;

        public ClientThread(BluetoothDevice device) {
            // Use a temporary object that is later assigned to mmSocket,
            // because mmSocket is final
            BluetoothSocket tmp = null;

            // Get a BluetoothSocket to connect with the given BluetoothDevice
            try {
                // MY_UUID is the app's UUID string, also used by the server code
                //Toast.makeText(BluetoothClientActivity.this, "Getting the Bluetooth socket", Toast.LENGTH_SHORT).show();
                tmp = device.createRfcommSocketToServiceRecord(bluetoothUUID);
            } catch (IOException e) {
                //Toast.makeText(BluetoothClientActivity.this, "Couldn't get the Bluetooth socket", Toast.LENGTH_SHORT).show();
            }
            mmSocket = tmp;
        }

        public void run() {

            // Cancel discovery because it will slow down the connection
            mBluetoothAdapter.cancelDiscovery();

            try {
                // Connect the device through the socket. This will block
                // until it succeeds or throws an exception
                //Toast.makeText(BluetoothClientActivity.this, "Connecting to the Bluetooth socket", Toast.LENGTH_SHORT).show();
                mmSocket.connect();
            } catch (IOException connectException) {
                // Unable to connect; close the socket and get out
                //Toast.makeText(BluetoothClientActivity.this, "Couldn't connect to the Bluetooth socket", Toast.LENGTH_SHORT).show();
                try {
                    mmSocket.close();
                } catch (IOException closeException) {
                    //Toast.makeText(BluetoothClientActivity.this, "Couldn't close the Bluetooth socket", Toast.LENGTH_SHORT).show();
                }

                return;
            }

            //Send success connect message to handler and start transfer thread
            threadHandler.obtainMessage(SUCCESS_CONNECT, mmSocket).sendToTarget();
        }

        /**
         * Will cancel the listening socket, and cause the thread to finish
         */
        public void cancel() {
            try {
                mmSocket.close();
                Toast.makeText(BluetoothClientActivity.this, "Connect thread socket closed", Toast.LENGTH_SHORT).show();
            } catch (IOException e) {
                Toast.makeText(BluetoothClientActivity.this, "Couldn't close the Bluetooth socket connection", Toast.LENGTH_SHORT).show();
            }
        }
    }
}
