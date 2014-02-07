package zh.wang.android.opencv.ar;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;

public class OpenCVARActivity extends Activity {
    private static final String TAG = "OpenCVARActivity";

    public OpenCVARActivity() {
        Log.i(TAG, "Instantiated new " + this.getClass());

    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(new OpenCVARView(getApplicationContext()));
    }
}

