package com.mate.transpor.transpormate.bluetooth;

import java.io.Console;
import java.text.DecimalFormat;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.hardware.SensorManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Locale;
import java.util.UUID;

/*********************************************************************************************************
 * Activity que muestra realiza la comunicacion con Arduino
 **********************************************************************************************************/

//******************************************** Hilo principal del Activity**************************************
public class activity_comunicacion extends Activity implements SensorEventListener
{
    Button btnActivarSensores;
    Handler bluetoothIn;
    final int handlerState = 0; //used to identify handler message

    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private StringBuilder recDataString = new StringBuilder();

    private ConnectedThread mConnectedThread;
    private SensorManager mSensorManager;
    private boolean SensoresActivados = false;
    private boolean EstaAvanzando = false;

    // SPP UUID service  - Funciona en la mayoria de los dispositivos
    private static final UUID BTMODULEUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    // String for MAC address del Hc05
    private static String address = null;

    DecimalFormat dosdecimales = new DecimalFormat( "###.###");

    SeekBar Leftseek;
    SeekBar Rightseek;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_comunicacion);

        //Se definen los componentes del layout
        btnActivarSensores =(Button)findViewById(R.id.btnActivarSensores);
        Leftseek = (SeekBar) findViewById(R.id.LeftSeekBar);
        Rightseek = (SeekBar) findViewById(R.id.RightSeekBar);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);

        //obtengo el adaptador del bluethoot
        btAdapter = BluetoothAdapter.getDefaultAdapter();

        //defino el Handler de comunicacion entre el hilo Principal  el secundario.
        //El hilo secundario va a mostrar informacion al layout atraves utilizando indeirectamente a este handler
        bluetoothIn = Handler_Msg_Hilo_Principal();

        //defino los handlers para el boton Activar Sensor, y cambios en la barra de controles.
        btnActivarSensores.setOnClickListener(btnActivarSensoresListener);

        Leftseek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            // a - negativo maximo 0
            // b - negativo tres cuartos 1
            // c - negativo medio 2
            // e - detenido 4
            // g - positivo medio 6
            // h - positivo tres cuartos 7
            // i - positivo maximo 8


            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
                // TODO Auto-generated method stub
                // Enviar msj al bluetooth.
                //textView.setText(String.valueOf(progress));
                if(progress == 0)
                    mConnectedThread.write( String.valueOf("a"));
                if(progress == 1)
                    mConnectedThread.write( String.valueOf("b"));
                if(progress == 2)
                    mConnectedThread.write( String.valueOf("c"));
                if(progress == 3)
                    mConnectedThread.write( String.valueOf("e"));
                if(progress == 4)
                    mConnectedThread.write( String.valueOf("g"));
                if(progress == 5)
                    mConnectedThread.write( String.valueOf("h"));
                if(progress == 6)
                    mConnectedThread.write( String.valueOf("i"));
            }
        });

        Rightseek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            // j - negativo maximo 0
            // k - negativo tres cuartos 1
            // l - negativo medio 2
            // n - detenido 3
            // p - positivo medio 4
            // q - positivo tres cuartos 5
            // r - positivo maximo 6
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
                // TODO Auto-generated method stub
                // Enviar msj al bluetooth.
                //textView2.setText(String.valueOf(progress));
                if(progress == 0)
                    mConnectedThread.write( String.valueOf("j"));
                if(progress == 1)
                    mConnectedThread.write( String.valueOf("k"));
                if(progress == 2)
                    mConnectedThread.write( String.valueOf("l"));
                if(progress == 3)
                    mConnectedThread.write( String.valueOf("n"));
                if(progress == 4)
                    mConnectedThread.write( String.valueOf("p"));
                if(progress == 5)
                    mConnectedThread.write( String.valueOf("q"));
                if(progress == 6)
                    mConnectedThread.write( String.valueOf("r"));
            }
        });

    }

    // Metodo que escucha el cambio de sensibilidad de los sensores
    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy)
    {

    }

    // Metodo que escucha el cambio de los sensores
    @Override
    public void onSensorChanged(SensorEvent event) {

        // Cada sensor puede lanzar un thread que pase por aqui
        // Para asegurarnos ante los accesos simult�neos sincronizamos esto

        synchronized (this) {
            Log.d("sensor", event.sensor.getName());

            switch (event.sensor.getType()) {
                case Sensor.TYPE_ACCELEROMETER:
                    //if ((event.values[0] > 15) || (event.values[1] > 15) || (event.values[2] > 15)) {
                    //EnviarMensajeDetener();
                    //    mConnectedThread.write( String.valueOf("u"));
                    //}
                    if( event.values[0] > 7 ){
                        //EnviarMensajeRetroceder();
                        if(EstaAvanzando){
                            EstaAvanzando = false;
                            mConnectedThread.write( String.valueOf("v"));
                        }
                    }

                    if( event.values[0] < 3){
                        //EnviarMensajeAvanzar();
                        if(!EstaAvanzando){
                            EstaAvanzando = true;
                            mConnectedThread.write( String.valueOf("w"));
                        }
                    }
                    break;
                case Sensor.TYPE_PROXIMITY:
                    // Si detecta 0 lo represento
                    if (event.values[0] == 0) {
                        //EnviarMensajeTitilarLuz();
                        mConnectedThread.write( String.valueOf("x"));
                    }
                    else
                    {
                        //ApagarLed
                        mConnectedThread.write( String.valueOf("y"));
                    }
                    break;
                case Sensor.TYPE_LIGHT :
                    if(event.values[0] < 2 )
                    {
                        //Detener motores
                        mConnectedThread.write( String.valueOf("u"));
                    }
                    else {

                    }
                    break;

            }
        }
    }

    @Override
    protected void onStop()
    {

        Parar_Sensores();

        super.onStop();
    }

    @Override
    protected void onDestroy()
    {
        Parar_Sensores();

        super.onDestroy();
    }

    @Override
    protected void onRestart()
    {
        //Ini_Sensores();

        super.onRestart();
    }

    @Override
    //Cada vez que se detecta el evento OnResume se establece la comunicacion con el HC05, creando un
    //socketBluethoot
    public void onResume() {
        super.onResume();

        //Ini_Sensores();
        //Obtengo el parametro, aplicando un Bundle, que me indica la Mac Adress del HC05
        Intent intent=getIntent();
        Bundle extras=intent.getExtras();

        address= extras.getString("Direccion_Bluethoot");

        BluetoothDevice device = btAdapter.getRemoteDevice(address);

        //se realiza la conexion del Bluethoot crea y se conectandose a atraves de un socket
        try
        {
            btSocket = createBluetoothSocket(device);
        }
        catch (IOException e)
        {
            showToast( "La creacción del Socket fallo");
        }
        // Establish the Bluetooth socket connection.
        try
        {
            btSocket.connect();
        }
        catch (IOException e)
        {
            try
            {
                btSocket.close();
            }
            catch (IOException e2)
            {
                //insert code to deal with this
            }
        }

        //Una establecida la conexion con el Hc05 se crea el hilo secundario, el cual va a recibir
        // los datos de Arduino atraves del bluethoot
        mConnectedThread = new ConnectedThread(btSocket);
        mConnectedThread.start();

        //I send a character when resuming.beginning transmission to check device is connected
        //If it is not an exception will be thrown in the write method and finish() will be called
        mConnectedThread.write("x");
    }


    @Override
    //Cuando se ejecuta el evento onPause se cierra el socket Bluethoot, para no recibiendo datos
    public void onPause()
    {
        super.onPause();
        try
        {
            Parar_Sensores();
            //Don't leave Bluetooth sockets open when leaving activity
            btSocket.close();
        } catch (IOException e2) {
            //insert code to deal with this
        }
    }

    private View.OnClickListener btnActivarSensoresListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            try {
                if (SensoresActivados == false) {
                    Ini_Sensores();
                    SensoresActivados = true;
                    btnActivarSensores.setText("Desactivar Sensores");
                    Leftseek.setEnabled(false);
                    Rightseek.setEnabled(false);
                } else {
                    Parar_Sensores();
                    SensoresActivados = false;
                    btnActivarSensores.setText("Activar Sensores");
                    Leftseek.setEnabled(true);
                    Rightseek.setEnabled(true);
                }
            }
            catch (Exception e){
                Log.e( "Error botton", e.getMessage());
            }
        }
    };

    //Metodo que crea el socket bluethoot
    private BluetoothSocket createBluetoothSocket(BluetoothDevice device) throws IOException {

        return  device.createRfcommSocketToServiceRecord(BTMODULEUUID);
    }

    //Handler que permite mostrar datos en el Layout al hilo secundario
    private Handler Handler_Msg_Hilo_Principal ()
    {
        return new Handler() {
            public void handleMessage(android.os.Message msg)
            {
                //si se recibio un msj del hilo secundario
                if (msg.what == handlerState)
                {
                    //voy concatenando el msj
                    String readMessage = (String) msg.obj;
                    recDataString.append(readMessage);
                    int endOfLineIndex = recDataString.indexOf("\r\n");

                    if(readMessage.contains("D")){
                        Rightseek.setEnabled(false);
                        Leftseek.setEnabled(false);
                        Rightseek.setProgress(3);
                        Leftseek.setProgress(3);
                        if(SensoresActivados){
                            Parar_Sensores();
                            //Envio mensaje detener autito.
                            mConnectedThread.write( String.valueOf("u"));
                        }
                    }
                    if(readMessage.contains("H")){
                        if(SensoresActivados)
                            Ini_Sensores();
                        else{
                            Rightseek.setEnabled(true);
                            Leftseek.setEnabled(true);
                        }
                    }
                    if(readMessage.contains("T")){
                        showToast("Temperatura correcta");
                    }
                    //cuando recibo toda una linea la muestro en el layout
                    if (endOfLineIndex > 0)
                    {
                        //String dataInPrint = recDataString.substring(0, endOfLineIndex);
                        //txtPotenciometro.setText(dataInPrint);
                        recDataString.delete(0, recDataString.length());
                    }
                }
            }
        };

    }

    /*** Manejo de Sensores ***/
    protected void Ini_Sensores()
    {
        try {
            mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_NORMAL);
            //mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION),     SensorManager.SENSOR_DELAY_NORMAL);
            //mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR), SensorManager.SENSOR_DELAY_NORMAL);
            //mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD),  SensorManager.SENSOR_DELAY_NORMAL);
            mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY), SensorManager.SENSOR_DELAY_FASTEST);
            mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT), SensorManager.SENSOR_DELAY_FASTEST);
        }
        catch (Exception e){
            Log.e( "Error botton", e.getMessage());
        }
}

    private void Parar_Sensores()
    {
        mSensorManager.unregisterListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER));
        mSensorManager.unregisterListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY));
        mSensorManager.unregisterListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT));
    }

    private void showToast(String message) {
        Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
    }


    //******************************************** Hilo secundario del Activity**************************************
    //*************************************** recibe los datos enviados por el HC05**********************************

    private class ConnectedThread extends Thread
    {
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        //Constructor de la clase del hilo secundario
        public ConnectedThread(BluetoothSocket socket)
        {
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try
            {
                //Create I/O streams for connection
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            } catch (IOException e) { }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        //metodo run del hilo, que va a entrar en una espera activa para recibir los msjs del HC05
        public void run()
        {
            byte[] buffer = new byte[800000];
            int bytes;

            //el hilo secundario se queda esperando mensajes del HC05
            while (true)
            {
                try
                {
                    //se leen los datos del Bluethoot
                    bytes = mmInStream.read(buffer);
                    String readMessage = new String(buffer, 0, bytes);
                    //se muestran en el layout de la activity, utilizando el handler del hilo
                    // principal antes mencionado

                    bluetoothIn.obtainMessage(handlerState, bytes, -1, readMessage).sendToTarget();
                } catch (IOException e) {
                    e.printStackTrace();
                    break;
                }
            }
        }

        //write method
        public void write(String input) {
            byte[] msgBuffer = input.getBytes();           //converts entered String into bytes
            try {
                mmOutStream.write(msgBuffer);                //write bytes over BT connection via outstream
            } catch (IOException e) {
                //if you cannot write, close the application
                showToast("La conexion fallo");
                finish();

            }
        }
    }

}
