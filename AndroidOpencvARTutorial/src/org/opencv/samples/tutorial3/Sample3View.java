package org.opencv.samples.tutorial3;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

class Sample3View extends SampleViewBase {
	
	String tag = this.toString();
	
    public Sample3View(Context context) {
        super(context);
    }

    @Override
    protected Bitmap processFrame(byte[] data) {
        Log.d("TAG", "processFrame");
        int frameSize = getFrameWidth() * getFrameHeight();
        int[] rgba = new int[frameSize];
        
        FindFeatures(getFrameWidth(), getFrameHeight(), data, rgba);

        Bitmap bmp = Bitmap.createBitmap(getFrameWidth(), getFrameHeight(), Bitmap.Config.ARGB_8888);
        bmp.setPixels(rgba, 0/* offset */, getFrameWidth() /* stride */, 0, 0, getFrameWidth(), getFrameHeight());
        return bmp;
    }

    public native void FindFeatures(int width, int height, byte yuv[], int[] rgba);

}
