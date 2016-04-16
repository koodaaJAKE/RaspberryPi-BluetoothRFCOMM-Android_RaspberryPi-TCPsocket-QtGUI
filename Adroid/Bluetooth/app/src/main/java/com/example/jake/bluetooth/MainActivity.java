package com.example.jake.bluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Set;

public class MainActivity extends Activity implements View.OnClickListener {

    private static final int REQUEST_ENABLE_BT = 1;

    //Bluetooth MAC address Intent extra to be sent to other activity
    private static String BLUETOOTH_DEVICE_ADDRESS = "device_address";

    Button SetOnBluetooth, SetOffBluetooth, QueryDevices, DiscoverDevices, Visible, Exit;
    private BluetoothAdapter mBluetoothAdapter;
    ListView pairedBluetoothDeviceList, discoveredBluetoothDeviceList;
    private ArrayAdapter<String> discoveredBluetoothAdapter, pairedBluetoothAdapter;
    private ArrayList<String> discoveredBluetoothDevices, pairedBluetoothDevices;
    Set<BluetoothDevice> pairedDevices;
    IntentFilter filter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Initialization method
        initialize();

        //When ExitButton is clicked finish the application
        if (getIntent().getBooleanExtra("EXIT", false))
        {
            finish();
            return;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        // Register for broadcasts when a device is discovered
        filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        this.registerReceiver(Receiver, filter);

        // Register for broadcasts when discovery has finished
        filter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        this.registerReceiver(Receiver, filter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(Receiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        // Make sure we're not doing discovery anymore
        if (mBluetoothAdapter != null) {

            mBluetoothAdapter.cancelDiscovery();
        }

    }

    private void initialize() {

        //Set and get the Bluetooth adapter to and from the Singleton Class
        BluetoothSingleton bSingleton = BluetoothSingleton.getInstance();
        bSingleton.setBluetoothAdapter(BluetoothAdapter.getDefaultAdapter());
        mBluetoothAdapter = bSingleton.getBluetoothAdapter();

        SetOnBluetooth = (Button)findViewById(R.id.SetOnBluetoothButton);
        SetOffBluetooth = (Button)findViewById(R.id.SetOffBluetoothButton);
        QueryDevices = (Button)findViewById(R.id.QueryButton);
        DiscoverDevices = (Button)findViewById(R.id.DiscoverDevicesButton);
        Visible = (Button)findViewById(R.id.VisibleButton);
        Exit = (Button)findViewById(R.id.ExitButton);

        SetOnBluetooth.setOnClickListener(this);
        SetOffBluetooth.setOnClickListener(this);
        QueryDevices.setOnClickListener(this);
        DiscoverDevices.setOnClickListener(this);
        Visible.setOnClickListener(this);
        Exit.setOnClickListener(this);

        //Set up the ListView, ArrayList and ArrayAdapter for paired and discovered bluetooth devices
        pairedBluetoothDevices = new ArrayList();
        pairedBluetoothAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, pairedBluetoothDevices);
        pairedBluetoothDeviceList = (ListView) findViewById(R.id.paired_device_list);
        pairedBluetoothDeviceList.setAdapter(pairedBluetoothAdapter);
        pairedBluetoothDeviceList.setOnItemClickListener(pairedDeviceClickListener);

        discoveredBluetoothDevices = new ArrayList();
        discoveredBluetoothAdapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, discoveredBluetoothDevices);
        discoveredBluetoothDeviceList = (ListView) findViewById(R.id.discovered_device_list);
        discoveredBluetoothDeviceList.setAdapter(discoveredBluetoothAdapter);
        discoveredBluetoothDeviceList.setOnItemClickListener(discoveredDeviceClickListener);
    }

    @Override
    public void onClick(View v) {

        switch(v.getId()){

            case R.id.SetOnBluetoothButton:
                setOnBluetooth();
                break;

            case R.id.SetOffBluetoothButton:
                setOffBluetooth();
                break;

            case R.id.QueryButton:
                queryDevices();
                break;

            case R.id.DiscoverDevicesButton:
                discoverDevices();
                break;

            case R.id.VisibleButton:
                setVisible();
                break;

            case R.id.ExitButton:
                exitButtonClicked();
                break;
        }
    }

    private void setOnBluetooth() {

        if(mBluetoothAdapter == null){
            Toast.makeText(getApplicationContext(), "Device doesn't support bluetooth", Toast.LENGTH_SHORT).show();
        }
        if(!mBluetoothAdapter.isEnabled()){
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            Toast.makeText(getApplicationContext(), "Bluetooth turned ON!", Toast.LENGTH_SHORT).show();
        }
        else{
            Toast.makeText(getApplicationContext(), "Bluetooth is already ON!", Toast.LENGTH_SHORT).show();
        }
    }

    private void setOffBluetooth() {

        if(mBluetoothAdapter.isEnabled()){

            if (mBluetoothAdapter.isDiscovering()){
                mBluetoothAdapter.cancelDiscovery();
            }

            mBluetoothAdapter.disable();
            Toast.makeText(getApplicationContext(), "Bluetooth turned OFF", Toast.LENGTH_SHORT).show();
        }
        else {
            Toast.makeText(getApplicationContext(), "Bluetooth is already OFF", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * This method shows the already paired devices
     */
    private void queryDevices() {

        if(mBluetoothAdapter.isEnabled()){

            if (mBluetoothAdapter.isDiscovering()){
                mBluetoothAdapter.cancelDiscovery();
            }

            //Clear the List View adapters
            pairedBluetoothAdapter.clear();
            discoveredBluetoothAdapter.clear();
            pairedDevices = mBluetoothAdapter.getBondedDevices();

            // If there are paired devices
            if (pairedDevices.size() > 0){
                // Loop through paired devices
                for (BluetoothDevice device : pairedDevices){
                    // Add the name and address to an array adapter to show in a ListView
                    Toast.makeText(getApplicationContext(), "Showing Paired Devices", Toast.LENGTH_SHORT).show();
                    pairedBluetoothDevices.add(device.getName() + "  " + device.getAddress());
                    pairedBluetoothAdapter.notifyDataSetChanged();
                }
            }
        }
        else{
            Toast.makeText(getApplicationContext(), "Bluetooth isn't ON", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * This method starts discovery for devices which aren't paired
     */
    private void discoverDevices() {

        if(mBluetoothAdapter.isEnabled()) {

            //Clear the List View adapters
            pairedBluetoothAdapter.clear();
            discoveredBluetoothAdapter.clear();

            if (!mBluetoothAdapter.isDiscovering()) {

                // Indicate scanning in the title
                setTitle(R.id.scanning);

                mBluetoothAdapter.startDiscovery();
                Toast.makeText(getApplicationContext(), "Discovery started", Toast.LENGTH_SHORT).show();

            }
            else{
                mBluetoothAdapter.cancelDiscovery();
                Toast.makeText(getApplicationContext(), "Discovery cancelled", Toast.LENGTH_SHORT).show();
            }
        }
        else{
            Toast.makeText(getApplicationContext(), "Bluetooth isn't ON", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * The BroadcastReceiver that listens for discovered devices
     */
    private final BroadcastReceiver Receiver = new BroadcastReceiver(){

        public void onReceive(Context context, Intent intent){

            String action = intent.getAction();

            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {

                // Get the BluetoothDevice object from the Intent
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                // Add the name and address to an array adapter to show in a ListView, check first
                // it isn't already paired device
                if(device.getBondState() != BluetoothDevice.BOND_BONDED) {

                    discoveredBluetoothDevices.add(device.getName() + "  " + device.getAddress());
                    discoveredBluetoothAdapter.notifyDataSetChanged();

                }
            //Check when the discovery has finished
            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {

                //Set the activity title
                setTitle(R.id.bluetooth);

                //If nothing found
                if(discoveredBluetoothAdapter.getCount() == 0){

                    Toast.makeText(getApplicationContext(), "No devices found", Toast.LENGTH_SHORT).show();
                }
            }
        }
    };

    /**
     * The on-click listener for paired devices in the ListView
     */
    private AdapterView.OnItemClickListener pairedDeviceClickListener
            = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

            // Cancel discovery because it's costly and we're about to connect
            mBluetoothAdapter.cancelDiscovery();

            // Get the device MAC address, which is the last 17 chars in the View
            String info = ((TextView) view).getText().toString();
            String address = info.substring(info.length() - 17);

            // Start new activity and send the MAC address
            Intent bluetoothActivityIntent = new Intent(MainActivity.this, BluetoothTransferActivity.class);
            bluetoothActivityIntent.putExtra(BLUETOOTH_DEVICE_ADDRESS, address);

            //Clear the ListView
            pairedBluetoothAdapter.clear();

            startActivity(bluetoothActivityIntent);
        }
    };

    /**
     * The on-click listener for discovered devices in the ListView
     */
    private AdapterView.OnItemClickListener discoveredDeviceClickListener
            = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                // Cancel discovery because it's costly and we're about to connect
                mBluetoothAdapter.cancelDiscovery();

                // Get the device MAC address, which is the last 17 chars in the View
                String info = ((TextView) view).getText().toString();
                String address = info.substring(info.length() - 17);

                // Start new activity and send the MAC address
                Intent bluetoothActivityIntent = new Intent(MainActivity.this, BluetoothTransferActivity.class);
                bluetoothActivityIntent.putExtra(BLUETOOTH_DEVICE_ADDRESS, address);

                //Clear the ListView
                discoveredBluetoothAdapter.clear();

                startActivity(bluetoothActivityIntent);

        }
    };

    /**
     * This method sets the Bluetooth device discoverable to other Bluetooth devices
     */
    private void setVisible() {

        if(mBluetoothAdapter.isEnabled()){
            Intent getVisible = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            getVisible.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 120);
            startActivityForResult(getVisible, 0);
            Toast.makeText(getApplicationContext(), "Device is now visible for other devices", Toast.LENGTH_SHORT).show();
        }
        else{
            Toast.makeText(getApplicationContext(), "Bluetooth isn't ON", Toast.LENGTH_SHORT).show();
        }
    }

    private void exitButtonClicked(){
        Intent intent = new Intent(this, MainActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        intent.putExtra("EXIT", true);
        startActivity(intent);
        finish();
    }
}