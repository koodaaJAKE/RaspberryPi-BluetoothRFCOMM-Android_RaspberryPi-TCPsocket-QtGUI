package com.example.jake.bluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class BluetoothTransferActivity extends Activity implements View.OnClickListener {

    //Bluetooth MAC address Intent extra to be sent to other activity
    private static final String BLUETOOTH_DEVICE_ADDRESS = "device_address";
    private static final String raspberryPiBluetoothAddress = "5C:F3:70:0D:4C:3B";

    Button ServerConnection, ClientConnection;
    private String bluetoothAddress;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_transfer);

        setTitle(R.id.choose);
        ServerConnection = (Button)findViewById(R.id.ServerConnectionButton);
        ClientConnection = (Button)findViewById(R.id.ClientConnectionButton);

        ServerConnection.setOnClickListener(this);
        ClientConnection.setOnClickListener(this);

        //Get the Bluetooth device address from main activity for client connection
        Intent intent = getIntent();
        bluetoothAddress = intent.getStringExtra(BLUETOOTH_DEVICE_ADDRESS);

        Log.d("address", bluetoothAddress);

        bluetoothAddress = raspberryPiBluetoothAddress;
    }

    @Override
    public void onClick(View v) {

        switch(v.getId()){

            case R.id.ServerConnectionButton:

                //Get the BluetoothAdapter from singleton class
                BluetoothSingleton bSingleton = BluetoothSingleton.getInstance();
                BluetoothAdapter mBluetoothAdapter = bSingleton.getBluetoothAdapter();

                //Check if Bluetooth device is visible before starting Bluetooth Server activity
                if(mBluetoothAdapter.getScanMode() == mBluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {

                    // Start the server activity
                    Intent bluetoothServerIntent = new Intent(BluetoothTransferActivity.this, BluetoothServerActivity.class);

                    startActivity(bluetoothServerIntent);
                }
                else {
                    Toast.makeText(this, "Bluetooth device isn't visible", Toast.LENGTH_SHORT).show();
                    Intent getVisible = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
                    getVisible.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 120);
                    startActivityForResult(getVisible, 0);
                }

                break;

            case R.id.ClientConnectionButton:

                // Start the client activity and send the bluetooth MAC address
                Intent bluetoothClientIntent = new Intent(BluetoothTransferActivity.this, BluetoothClientActivity.class);
                bluetoothClientIntent.putExtra(BLUETOOTH_DEVICE_ADDRESS, bluetoothAddress);

                startActivity(bluetoothClientIntent);
                break;
        }
    }
}



