package org.opencv.samples.tutorial3;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class MainScreen extends Activity {
	@Override
	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		ListView lv = new ListView(this);
		setContentView(lv);
		
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
		adapter.add("camera");
		adapter.add("bitmap");
		adapter.add("homography");
		
		lv.setAdapter(adapter);
		lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {

			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3) {
				if(arg2 == 0){
					Intent intent = new Intent();
					intent.setClass(getApplicationContext(), Sample3Native.class);
					startActivity(intent);
				} 
				if(arg2 == 1){
					Intent intent = new Intent();
					intent.setClass(getApplicationContext(), BitmapProcessing.class);
					startActivity(intent);
				}
				if(arg2 == 2){
					Intent intent = new Intent();
					intent.setClass(getApplicationContext(), Homography.class);
					startActivity(intent);
				}
			}
		});

	}
	

    static {
        System.loadLibrary("native_sample");
    }
}
