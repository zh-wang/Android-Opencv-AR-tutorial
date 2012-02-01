package org.opencv.samples.tutorial3;

import java.io.File;
import java.io.InputStream;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap.Config;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

public class Homography extends Activity{
	LinearLayout ll;
	public static int SELECT_IMAGE = 100;
	@Override
	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.homography);
		
		ll = (LinearLayout) findViewById(R.id.linearLayout1);
		
		Button button = (Button) findViewById(R.id.button1);
		button.setOnClickListener(new View.OnClickListener() {
			
			public void onClick(View v) {
				startActivityForResult(new Intent(Intent.ACTION_PICK, 
						android.provider.MediaStore.Images.Media.INTERNAL_CONTENT_URI), SELECT_IMAGE);
			}
		});
		
	}
	
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		if (requestCode == SELECT_IMAGE)
			if (resultCode == Activity.RESULT_OK) {
				Uri selectedImage = data.getData();
				ContentResolver cr = getContentResolver();  
	            String[] columns = {MediaStore.Images.Media.DATA };  
	            Cursor c = cr.query(selectedImage, columns, null, null, null);  
	              
	            c.moveToFirst();  
	            File picture = new File(c.getString(0));  
				
				doHomography(picture.getPath());

				ImageView iv = new ImageView(this);
				Bitmap bitmap = BitmapFactory.decodeFile("/sdcard/CameraNativeTest/marker_after.png");
				iv.setImageBitmap(bitmap);
				ll.addView(iv);
			}
	}

	private native void doHomography(String s);
}
