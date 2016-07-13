//
//
//
package bonebou.diordve;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import android.os.Bundle;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.os.AsyncTask;

/**
 * Created by Albert on 6/22/16.
 */
public class ImgProcessOcr extends Evdroid {

    // Native JNI - load libraries
    static {
        System.loadLibrary("pngt");
        System.loadLibrary("lept");
        System.loadLibrary("tess");
        System.loadLibrary("jni_java_native_bridge");
    }

    // credits: http://stackoverflow.com/questions/9978011/android-ics-jni-error-attempt-to-use-stale-local-reference-0x1#12824591 - NOT Boolean
    public native boolean initOcr ( String root_folder_path );
    public native boolean saveMiddleClass ( String root_folder_path, String img_unique_no_ext, long inputImage );  /*!not Boolean!!!*/

    Handler mHandler = new Handler(Looper.getMainLooper());

    ImgProcessOcr self = this;

    @Override
    public void onCreate ( Bundle savedInstanceState ) {
        super.onCreate ( savedInstanceState );
        new ImgProcessOCR_initOcr().execute ( "initOcr" ); /*init the tess once in native JNI via async thread to boost performance*/
    }

    @Override
    void process_im_n_ocr ( final byte[] data ) {
        // pic
        new ImgProcessOCR_processPic().execute ( data );
        self.tv.setText ( "" );
    }

    void append_txt ( final String txt ) {

        self.mHandler.post ( new Runnable() {
            @Override
            public void run ( ) {

                if(txt==null) return;

                if ( txt.contains("RESET_CLEAR_IMG")  )
                    self.img_capture_preview.setImageURI(Uri.parse(IMG_CAPTURE_PATH+"?time=fuck"));
                else if ( txt.contains("DISPLAY_IMG") )
                    self.img_capture_preview.setImageURI(Uri.parse(IMG_CAPTURE_PATH));
                else if ( txt.contains("CCLLEEAARR") ) {
                    self.tv.setText ( "" );
                }
                else
                    // self.tv.append ( txt + "\n" );
                    self.tv.append ( txt );
            }

        } );
    }

    // called by the c++ JNI
    public void messageMe ( String text ) {
        self.append_txt ( text );
    }

    //==================

    // this chap internally saves a pic from the native/c++
    class ImgProcessOCR_processPic extends AsyncTask <byte[], Integer, String> {

        @Override
        protected String doInBackground ( byte[]... params ) {

            self.messageMe("RESET_CLEAR_IMG");
            // this.publishProgress(1);
            byte[] data = params[0];

            Mat mat=new Mat();
            Bitmap bmp = BitmapFactory.decodeByteArray ( data, 0, data.length );
            Utils.bitmapToMat ( bmp, mat ); bmp.recycle();
            Imgproc.cvtColor ( mat, mat, Imgproc.COLOR_RGB2BGR );

            // JNI native call
            saveMiddleClass ( ROOT_FOLDER_PATH /*static*/, PHOTO_PREFIX, mat.getNativeObjAddr() ) ;
            mat.release();

            return null;
        }
    }

    //==================
    // init the tess once in native JNI via async thread to boost performance
    class ImgProcessOCR_initOcr extends AsyncTask <String, Integer, String> {

        @Override
        protected String doInBackground ( String... params ) {

            // JNI native call
            initOcr( ROOT_FOLDER_PATH );
            return null;
        }
    }
}
