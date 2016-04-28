package com.example.jake.bluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
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

import static java.lang.Float.intBitsToFloat;

public class BluetoothServerActivity extends Activity implements View.OnClickListener {

    private static final UUID bluetoothUUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
    private static final String NAME = "BLUETOOTH_APPLICATION";

    Button Connect, SendSomeText, Hello, CloseConnection, ReadSensorData, ClearText;
    TextView distanceView, velocityView;
    EditText randomEditText;
    private String randomText;
    private ServerThread serverThread;
    private BluetoothTransferThread serverTransferThread;
    private BluetoothAdapter mBluetoothAdapter;

    //Flags to check the state of the threads
    private boolean isConnectThreadRunning = false, isTransferThreadRunning = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_server);

        //Set title to "Bluetooth Server"
        setTitle(R.id.server);

        initialize();

        Log.d("uuid", bluetoothUUID.toString());
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

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){

            case R.id.ConnectButton:

                if(!isConnectThreadRunning && !isTransferThreadRunning) {

                    //Create serverThread instance from a AcceptThread class
                    serverThread = new ServerThread(mBluetoothAdapter);
                    //Start the server thread and if success start the transfer thread also
                    serverThread.start();
                    break;
                }
                Toast.makeText(this, "Threads already running", Toast.LENGTH_SHORT).show();
                break;

            case R.id.SendSomeTextButton:

                if(isConnectThreadRunning && isTransferThreadRunning) {

                    //Get the random text from the EditText field
                    randomText = randomEditText.getText().toString();
                    if(!randomText.matches("")) {
                        sendSomeRandomText();
                    }
                    else Toast.makeText(this, "Type some text first", Toast.LENGTH_SHORT).show();
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
                if(isTransferThreadRunning && isConnectThreadRunning) {
                    clearScreen();
                }
                break;
        }
    }

    /**
     * Handler for handling the messages of the transfer thread
     */
    private final Handler transferThreadHandler = new Handler(){

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what){

                case BluetoothClientActivity.SUCCESS_CONNECT:

                    isConnectThreadRunning = true;
                    isTransferThreadRunning = true;
                    Toast.makeText(BluetoothServerActivity.this, "Socket connection created", Toast.LENGTH_SHORT).show();
                    //Start the transfer thread
                    serverTransferThread = new BluetoothTransferThread((BluetoothSocket)msg.obj, BluetoothServerActivity.this, transferThreadHandler);
                    serverTransferThread.start();

                    break;

                case BluetoothClientActivity.TRANSFER_THREAD_CREATED:
                    Toast.makeText(BluetoothServerActivity.this, "Transfer thread created", Toast.LENGTH_SHORT).show();
                    break;

                case BluetoothClientActivity.MESSAGE_READ:

                    Toast.makeText(BluetoothServerActivity.this,"Received data from the socket", Toast.LENGTH_SHORT).show();
                    int[] receivedData = (int[]) msg.obj;
                    int distance, intVelocity;
                    float velocity;

                    //Parse the received data
                    distance = fromBytesToInt(receivedData, false);
                    intVelocity = fromBytesToInt(receivedData, true);

                    //Integer to IEEE 754 float
                    velocity = intBitsToFloat(intVelocity);

                    //Convert to string and set precision to 2
                    String velocityToString = String.format("%.02f", velocity);

                    String intToString = Integer.toString(distance);

                    velocityView.setText(velocityToString);
                    distanceView.setText(intToString);

                    break;
            }
        }
    };

    private void readSensorData(){
        byte[] readSensorDataByte = new byte[1];
        readSensorDataByte[0] = BluetoothClientActivity.READ_SENSOR_DATA_COMMAND;
        serverTransferThread.write(readSensorDataByte);
    }

    private void printHello(){
        byte[] helloByte = new byte[1];
        helloByte[0] = BluetoothClientActivity.HELLO_COMMAND;
        serverTransferThread.write(helloByte);
    }

    private void closeTheConnection(){
        byte[] closeByte = new byte[1];
        closeByte[0] = BluetoothClientActivity.SOCKET_CLOSE_COMMAND;
        serverTransferThread.write(closeByte);
        serverTransferThread.cancel();
        serverThread.cancel();

        //And jump back to the main activity
        Intent goBackToMainActivity = new Intent(this, MainActivity.class);
        goBackToMainActivity.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(goBackToMainActivity);
    }

    private void sendSomeRandomText(){

        byte[] sendSomeRandomTextByte = new  byte[1];
        sendSomeRandomTextByte[0] = BluetoothClientActivity.RANDOM_TEXT_COMMAND;
        serverTransferThread.write(sendSomeRandomTextByte);

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
        serverTransferThread.write(randomTextArray);

        //Clear the EditText field
        randomEditText.setText("");
    }

    private void clearScreen(){
        byte[] clearScreenByte = new byte[1];
        clearScreenByte[0] = BluetoothClientActivity.CLEAR_SCREEN;
        serverTransferThread.write(clearScreenByte);
    }

    //Method to parse the data
    private int fromBytesToInt(int[] bytes, boolean tf){
        int bytesToInt;
        if(tf) {
            return bytesToInt = bytes[4] << 24 | bytes[5] << 16 | bytes[6] << 8 | bytes[7];
        }else{
            return bytesToInt = bytes[8] << 24 | bytes[9] << 16 | bytes[10] << 8 | bytes[11];
        }
    }

    private class ServerThread extends Thread {
        private final BluetoothServerSocket bluetoothServerSocket;

        public ServerThread(BluetoothAdapter pBluetoothAdapter) {

            // Always cancel discovery because it will slow down a connection
            pBluetoothAdapter.cancelDiscovery();

            // Use a temporary object that is later assigned to mmServerSocket,
            // because bluetoothServerSocket is final
            BluetoothServerSocket tmp = null;
            try {
                // MY_UUID is the app's UUID string, also used by the client code
                tmp = pBluetoothAdapter.listenUsingRfcommWithServiceRecord(NAME, bluetoothUUID);
                Toast.makeText(BluetoothServerActivity.this, "Server socket created", Toast.LENGTH_SHORT).show();
            } catch (IOException e) {
                Toast.makeText(BluetoothServerActivity.this, "Couldn't create socket", Toast.LENGTH_SHORT).show();
            }
            bluetoothServerSocket = tmp;
        }

        public void run() {
            BluetoothSocket mServerSocket = null;

            // Keep listening until exception occurs or a socket is returned
            while (true) {
                try {
                    mServerSocket = bluetoothServerSocket.accept();

                } catch (IOException e) {

                    break;
                }
                // If a connection was accepted
                if (mServerSocket != null) {
                    // Do work to manage the connection (in a separate thread)
                    //manageConnectedSocket(socket);
                    try {
                        bluetoothServerSocket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    break;
                }
            }

            //Send success connect message to handler and start transfer thread
            transferThreadHandler.obtainMessage(BluetoothClientActivity.SUCCESS_CONNECT, mServerSocket).sendToTarget();
        }

        /**
         * Will cancel the listening socket, and cause the thread to finish
         */
        public void cancel() {
            try {
                bluetoothServerSocket.close();
                Toast.makeText(BluetoothServerActivity.this, "Socket closed", Toast.LENGTH_SHORT).show();
            } catch (IOException e) {
                Toast.makeText(BluetoothServerActivity.this, "Couldn't close socket", Toast.LENGTH_SHORT).show();
            }
        }
    }
}
