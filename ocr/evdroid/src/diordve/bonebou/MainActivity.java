package diordve.bonebou;

import java.io.File;

import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.widget.Toast;
import android.os.Environment;

public class MainActivity extends Activity {

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        // preNocr.main(null);
        // System.loadLibrary("preNocr");

        // Context context = getApplicationContext();
        // CharSequence text = Environment.getExternalStorageDirectory().getAbsolutePath();
        // int duration = Toast.LENGTH_SHORT;

        // File f = new File ( Environment.getExternalStorageDirectory() + "/tessdata" );
        // if ( f.isDirectory() ) {
        //     text = "yep, files is here man";
        // }
        // else{
        //     text = "fuck, nothing there...";
        // }

        // Toast toast = Toast.makeText(context, text, duration);
        // toast.show();
        preNocr.main( null );
    }
}
