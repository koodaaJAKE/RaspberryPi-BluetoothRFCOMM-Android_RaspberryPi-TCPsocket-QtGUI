package com.example.jake.bluetooth;

import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Handler;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * This thread runs during a connection with a remote device.
 * It handles all incoming and outgoing transmissions.
 */
public class BluetoothTransferThread extends Thread {

    private final BluetoothSocket connectedSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private Context context;
    private final Handler threadHandler;

    public BluetoothTransferThread(BluetoothSocket socket, Context context, Handler mHandler) {
        this.context = context;
        connectedSocket = socket;
        threadHandler = mHandler;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        // Get the BluetoothSocket input and output streams
        try {
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) {

        }

        mmInStream = tmpIn;
        mmOutStream = tmpOut;
    }

    public void run() {

        //Send a transfer thread created message to activity
        threadHandler.obtainMessage(BluetoothClientActivity.TRANSFER_THREAD_CREATED).sendToTarget();

        int[] buffer;
        int readByte;
        int i = 0, bytesAvailable;

        /**
         * Keep listening to the InputStream while connected
         */
        while (true) {

            try {

                //Check if there is bytes available to read in the InputStream
                bytesAvailable = mmInStream.available();
                if(bytesAvailable > 0) {
                    buffer = new int[bytesAvailable];
                    Log.d(getClass().getName(), String.format("value = %d", bytesAvailable));

                    /*
                     * Read the stream byte at a time and store it to a buffer until we have received the end of the frame char
                     */
                    do {
                        //readByte = dInputStream.readUnsignedByte();
                        readByte = mmInStream.read();
                        buffer[i] = readByte;
                        i++;
                    } while (readByte != 0xEE);

                    //Send the received data through handler back to activity
                    threadHandler.obtainMessage(BluetoothClientActivity.MESSAGE_READ, buffer).sendToTarget();
                }
                try {
                    currentThread().sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * Write to the connected OutStream.
     */
    public void write(byte[] buffer) {
        try {
            mmOutStream.write(buffer);
            Toast.makeText(this.context, "Wrote to the socket", Toast.LENGTH_SHORT).show();

        } catch (IOException e) {
            Toast.makeText(this.context, "Couldn't write to the socket", Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Close the transfer thread
     */
    public void cancel() {
        try {
            connectedSocket.close();
            Toast.makeText(this.context, "Transfer thread socket closed", Toast.LENGTH_SHORT).show();
        } catch (IOException e) {
            Toast.makeText(this.context, "Couldn't close the transfer thread socket", Toast.LENGTH_SHORT).show();
        }
    }
}
