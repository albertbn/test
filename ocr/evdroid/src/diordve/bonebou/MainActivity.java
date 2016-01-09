package diordve.bonebou;

import java.io.File;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.FileOutputStream;
import java.util.ArrayList;
import java.util.Random;

import org.apache.http.NameValuePair;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageView;
import android.widget.Toast;

public class MainActivity extends Activity {

    final String UPLOAD_URL = "https://app.adcore.com/errmail/",
        IMG_TMP_PREF = "evdroid",
        IMG_TMP_EXT = ".jpg",
        IMG_PNG_EXT = ".png",
        IMG_DIR_PATH = "/tessdata/img/";

    static final int REQUEST_CODE_MAX = 65534;
    ImageView imgFavorite;
    WebView wv;
    Uri mImageUri;
    MainActivity tthis = this;

    File get_file_temp ( String part, String ext, int id ) throws Exception {

        File dir = null, ffile;
        try {
            dir = new File ( Environment.getExternalStorageDirectory().getAbsolutePath()+ IMG_DIR_PATH );

            if ( !dir.exists() ) dir.mkdir();

            ffile = new File ( dir.getAbsolutePath()+"/"+part+id+ext );

            return ffile;
        }
        catch ( Exception ex ) {
            tthis.post_error( "Camera, MainActivity.java, get_file_temp: " + err_str(ex) );
        }

        //just an error case return, same as return null
        return dir;
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //2 july 2015 - change to 1==1 to test imediate err...
        //http://www.acra.ch/
        if(1==0) throw new NullPointerException("ffds");

        //============

        this.imgFavorite = (ImageView)findViewById(R.id.imageView1);
        this.imgFavorite.setBackgroundColor(Color.WHITE);

        // calling:open()
        this.imgFavorite.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {

                    try {
                        open();
                    }
                    catch(Exception ex){

                        tthis.post_error( "Camera, MainActivity.java, imgFavorite.setOnClickListener: " + err_str(ex) );
                    }
                }
            });
    }

    // gets a path to a random image, opens existing camera and inits activity
    public void open ( ) {

        if ( 1==1 ) {
            preNocr.dodoit( "dummy testing hards..." );
            show_msg("DONE doing OCR - see how fast???");
            return;
        }

        Intent intent = new Intent ( android.provider.MediaStore.ACTION_IMAGE_CAPTURE);
        int img_id = 0;
        File photo;

        try {
            Random randomGenerator = new Random();
            // place where to store camera taken picture
            img_id = randomGenerator.nextInt(REQUEST_CODE_MAX);
            photo = this.get_file_temp( IMG_TMP_PREF, IMG_TMP_EXT, img_id );
        }
        catch ( Exception ex ) {
            show_msg("Can't create file to take picture! Please check SD card! Image shot is impossible!");
            tthis.post_error( "Camera, MainActivity.java, Can't create file to take picture! Please check SD card!, open(): " + err_str(ex) );
            return;
        }

        try {
            mImageUri = Uri.fromFile ( photo );
            intent.putExtra ( MediaStore.EXTRA_OUTPUT, mImageUri );
            startActivityForResult ( intent,  img_id );
        }
        catch ( Exception ex ) {
            tthis.post_error( "Camera, MainActivity.java, open() 2: "  + err_str(ex) );
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {

        try {
            if( resultCode==RESULT_OK){

                try {
                    // convert to png
                    File ff = tthis.get_file_temp( IMG_TMP_PREF, IMG_TMP_EXT, requestCode);
                    File ff_png = tthis.get_file_temp( IMG_TMP_PREF, IMG_PNG_EXT, requestCode);
                    convert2png ( ff.getAbsolutePath(), ff_png.getAbsolutePath() );
                    // call native c++ here
                    preNocr.dodoit( ff_png.getAbsolutePath() );


                    show_msg("TODO - pre and ocr... " + Uri.fromFile ( ff_png ) );
                    // ff.delete();
                }
                catch (Exception ex) {
                    tthis.post_error( "Camera, MainActivity.java, onActivityResult in: " + err_str(ex) );
                }
            }
            else {
                //nothing, take no action, no image came here...
            }

            super.onActivityResult(requestCode, resultCode, data);
        }
        catch ( Exception ex ) {
            tthis.post_error( "Camera, MainActivity.java, onActivityResult out: " + err_str(ex) );
        }
    }

    // ======================================
    // ===========dish and home utils=========
    // ======================================
    // =======================

    void convert2png ( String path_jpg, String path_png ) {

        try {
            Bitmap bmp = BitmapFactory.decodeFile(path_jpg);
            BitmapFactory.Options btmapOptions = new BitmapFactory.Options();
            btmapOptions.inPreferredConfig = Bitmap.Config.RGB_565; /*check*/
            bmp = BitmapFactory.decodeFile(path_jpg, btmapOptions);
            File img_png = new File ( path_png );
            FileOutputStream outStream;
            img_png.createNewFile(); // need to create file as empty ---
            outStream = new FileOutputStream(img_png);
            bmp.compress(Bitmap.CompressFormat.PNG, 100, outStream);

            outStream.flush();
            outStream.close();
            bmp.recycle();

        } catch ( Exception e ) {
            e.printStackTrace();
        }
    }

    void show_msg ( String msg ) {

        try {
            Context context = getApplicationContext();
            CharSequence text = msg;
            int duration = Toast.LENGTH_SHORT;

            Toast toast = Toast.makeText(context, text, duration);
            toast.show();
        }
        catch(Exception ex){
            tthis.post_error( "Camera, MainActivity.java, show_msg: " + err_str(ex) );
        }
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

    //=============defaults============
    @Override
    protected void onSaveInstanceState ( Bundle state ) {
        super.onSaveInstanceState ( state );
        state.putAll(state);
    }

    @Override
    public boolean onCreateOptionsMenu ( Menu menu ) {

        try {
            // Inflate the menu; this adds items to the action bar if it is present.
            // getMenuInflater().inflate(R.menu.main, menu);
        }
        catch(Exception ex){
            tthis.post_error( "Camera, MainActivity.java, imgFavorite.setOnClickListener: " + err_str(ex) );
        }
        return true;
    }
}
