package com.example.jake.bluetooth;

import android.bluetooth.BluetoothAdapter;

/**
 * Bluetooth singleton class for getting and setting the Bluetooth adapter
 * so that the mBluetooth adapter can be accessed from all activities
 */
public class BluetoothSingleton {
    private static BluetoothSingleton mInstance = new BluetoothSingleton();
    private BluetoothAdapter mBluetoothAdapter;

    private BluetoothSingleton(){
        // Private constructor to avoid new instances
    }

    public static BluetoothSingleton getInstance(){
        return mInstance;
    }

    public void setBluetoothAdapter(BluetoothAdapter adapter){
        mBluetoothAdapter = adapter;
    }

    public BluetoothAdapter getBluetoothAdapter(){
        return mBluetoothAdapter;
    }
}

