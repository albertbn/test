
package diordve.bonebou;

import java.io.File;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Random;
import org.apache.http.NameValuePair;

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

        //============
    //main fn to send errors
    //============
    void post_error ( final String text ) {

        try {
            //=================
            Thread t = new Thread( new Runnable() {

                    @Override
                    public void run() {
                        try{
                            ArrayList<NameValuePair> nameValuePairs = new  ArrayList<NameValuePair>();
                            nameValuePairs.add(new BasicNameValuePair("text",text));

                            HttpClient httpclient = new DefaultHttpClient();
                            HttpPost httppost = new HttpPost(UPLOAD_URL);
                            httppost.setEntity(new UrlEncodedFormEntity(nameValuePairs, "UTF-8"));
                            //HttpResponse response =
                            httpclient.execute(httppost);

                        } catch( Exception e ){

                            //check what to do with error
                            //show_msg(e.toString());
                        }
                    }
                });

            t.start();
            //=================
        } catch ( Exception ex ) {
            //e.printStackTrace();
        }
    }

    public String err_str ( Exception ex ) {

        StringWriter writer = new StringWriter();
        PrintWriter printWriter = new PrintWriter( writer );
        ex.printStackTrace( printWriter );
        printWriter.flush();

        return writer.toString();
    }
}
