package zh.wang.android.opencv.ar;

import java.io.IOException;
import java.util.List;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.os.Build;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class OpenCVARView extends SurfaceView implements SurfaceHolder.Callback, Runnable {
    private static final String TAG = "OpenCVARView";

    private static int MAGIC_TEXTURE_ID = 10;

    private Camera mCamera;
    private SurfaceHolder mHolder;
    private SurfaceTexture mSurfaceTexture;
    private int mFrameWidth;
    private int mFrameHeight;
    private byte[] mFrame;
    private boolean mThreadRun;
    
    public OpenCVARView(Context context) {
        super(context);
        getHolder().addCallback(this);
        Log.i(TAG, "Instantiated new " + this.getClass());
        mSurfaceTexture = new SurfaceTexture(10);
    }

    public int getFrameWidth() {
        return mFrameWidth;
    }

    public int getFrameHeight() {
        return mFrameHeight;
    }

    public void surfaceChanged(SurfaceHolder _holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged");
        if (mCamera != null) {
            Camera.Parameters params = mCamera.getParameters();
            List<Camera.Size> sizes = params.getSupportedPreviewSizes();
            
            mFrameWidth = width / 2;
            mFrameHeight = height / 2;

            // selecting optimal camera preview size
            {
                double minDiff = Double.MAX_VALUE;
                for (Camera.Size size : sizes) {
                    Log.d(TAG, "size : " + size.width + ", " + size.height);
                    if (Math.abs(size.height - height) < minDiff) {
                        mFrameWidth = size.width;
                        mFrameHeight = size.height;
                        minDiff = Math.abs(size.height - height);
                    }
                }
            }

            Log.d(TAG, "change preview size to : " + mFrameWidth + ", " + mFrameHeight);
            
            mHolder = _holder;

            params.setPreviewSize(getFrameWidth(), getFrameHeight());
            mCamera.setParameters(params);
            
            try {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
                    mSurfaceTexture = new SurfaceTexture(MAGIC_TEXTURE_ID);
                    mCamera.setPreviewTexture(mSurfaceTexture);
                    // TODO Auto-generated catch block
                } else {
                    mCamera.setPreviewDisplay(null);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            mCamera.startPreview();
        }
    }

    public void surfaceCreated(final SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        mCamera = Camera.open();

        mCamera.setPreviewCallback(new PreviewCallback() {
            public void onPreviewFrame(byte[] data, Camera camera) {
                synchronized (OpenCVARView.this) {
                    mFrame = data;
                    OpenCVARView.this.notify();
                }
            }
        });
        
        (new Thread(this)).start();

    }
    
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");
        mThreadRun = false;
        if (mCamera != null) {
            synchronized (this) {
                mCamera.stopPreview();
                mCamera.setPreviewCallback(null);
                mCamera.release();
                mCamera = null;
            }
        }
    }

    public void run() {
        mThreadRun = true;
        Log.d(TAG, "Starting processing thread");
        while (mThreadRun) {
            Bitmap bmp = null;

            synchronized (this) {
                try {
                    this.wait();
                    bmp = processFrame(mFrame);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            if (bmp != null) {
                Log.d(TAG, "bitmap: " + bmp.getWidth() + ", " + bmp.getHeight());
                if (mHolder == null) {
                    Log.d(TAG, "mHolder is null");
                }
                if (!mHolder.getSurface().isValid()) {
                    Log.d(TAG, "surface is not valid");
                }
                Canvas canvas = mHolder.lockCanvas();
                if (canvas == null) {
                    Log.d(TAG, "canvas is null");
                }
                Paint paint = new Paint();
                paint.setColor(0xFFFF0000);
                if (canvas != null) {
                    canvas.drawBitmap(bmp, (canvas.getWidth() - getFrameWidth()) / 2, (canvas.getHeight() - getFrameHeight()) / 2, null);
                    mHolder.unlockCanvasAndPost(canvas);
                }
                bmp.recycle();
            }
            
        }
    }
    
    protected Bitmap processFrame(byte[] data) {
        int frameSize = getFrameWidth() * getFrameHeight();
        int[] rgba = new int[frameSize];
        
        FindFeatures(getFrameWidth(), getFrameHeight(), data, rgba);

        Bitmap bmp = Bitmap.createBitmap(getFrameWidth(), getFrameHeight(), Bitmap.Config.ARGB_8888);
        bmp.setPixels(rgba, 0/* offset */, getFrameWidth() /* stride */, 0, 0, getFrameWidth(), getFrameHeight());
        return bmp;
    }

    public native void FindFeatures(int width, int height, byte yuv[], int[] rgba);
}