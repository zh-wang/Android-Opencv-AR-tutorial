package zh.wang.android.opencv.ar;


import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;


public class BitmapProcessing extends Activity{

	TextView tv;
	
	@Override
	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.bitmap_processing_layout);
		tv = (TextView) findViewById(R.id.textView1);
		String s = null;
		GetCameraIntrisicParams();
	}

	private native void GetCameraIntrisicParams();
}