//    Copyright 2014 Zhenghong Wang
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

package zh.wang.android.opencv.ar;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class MainActivity extends Activity {
	@Override
	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		ListView lv = new ListView(this);
		setContentView(lv);
		
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
		adapter.add("OpenCV AR Sample");
//		adapter.add("bitmap");
//		adapter.add("homography");
		
		lv.setAdapter(adapter);
		lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {

			public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
					long arg3) {
				if(arg2 == 0){
					Intent intent = new Intent();
					intent.setClass(getApplicationContext(), OpenCVARActivity.class);
					startActivity(intent);
				} 
//				if(arg2 == 1){
//					Intent intent = new Intent();
//					intent.setClass(getApplicationContext(), BitmapProcessing.class);
//					startActivity(intent);
//				}
//				if(arg2 == 2){
//					Intent intent = new Intent();
//					intent.setClass(getApplicationContext(), Homography.class);
//					startActivity(intent);
//				}
			}
		});

	}
	

    static {
        System.loadLibrary("native_sample");
    }
}
