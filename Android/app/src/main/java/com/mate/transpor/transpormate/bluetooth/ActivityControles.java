package com.mate.transpor.transpormate.bluetooth;

import android.app.Activity;
import android.os.Bundle;
import android.widget.SeekBar;
import android.widget.TextView;

public class ActivityControles extends Activity {


int p = 0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_controles);

        final SeekBar Leftseek = (SeekBar) findViewById(R.id.LeftSeekBar);
        final SeekBar Rightseek = (SeekBar) findViewById(R.id.RightSeekBar);
        final TextView textView = (TextView) findViewById(R.id.t1);
        final TextView textView2 = (TextView) findViewById(R.id.t2);

        Leftseek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                // TODO Auto-generated method stub
            }

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
                // TODO Auto-generated method stub
                // Enviar msj al bluetooth.
                textView.setText(String.valueOf(progress));
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

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
                // TODO Auto-generated method stub
                // Enviar msj al bluetooth.
                textView2.setText(String.valueOf(progress));

            }
        });

    }
}
