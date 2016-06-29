//
//
//
package my.project.MyRealTimeImageProcessing;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.util.concurrent.Executors;

/**
 * Created by Albert on 6/22/16.
 */
public class ImgProcessOcr extends MyRealTimeImageProcessing {

    // Native JNI - load libraries
    static {
        System.loadLibrary("pngt");
        System.loadLibrary("lept");
        System.loadLibrary("tess");
        System.loadLibrary("ImageProcessing");
    }

    // credits: http://stackoverflow.com/questions/9978011/android-ics-jni-error-attempt-to-use-stale-local-reference-0x1#12824591 - NOT Boolean
    public native boolean saveMiddleClass ( String root_folder_path, String img_unique_no_ext, long inputImage );  /*!not Boolean!!!*/

    final static String PHOTO_PREFIX = "smc"; /*prefix of the high resolution photo/picture taken*/
    final static String ROOT_FOLDER_PATH =  Environment.getExternalStorageDirectory().getAbsolutePath(); /* doesn't end with / */;
    final static String OCR_PATH =  ROOT_FOLDER_PATH + "/tessdata/ocr.txt";

    ImgProcessOcr self = this;

    @Override
    void process_im_n_ocr ( final byte[] data ) {

//      process the picture taken async
        Executors.newSingleThreadExecutor().execute( new Runnable() {
            @Override
            public void run() {

                Mat mat=new Mat();
                Bitmap bmp = BitmapFactory.decodeByteArray ( data, 0, data.length );
                Utils.bitmapToMat ( bmp, mat ); bmp.recycle();
                Imgproc.cvtColor ( mat, mat, Imgproc.COLOR_RGB2BGR );

                // JNI native call
                saveMiddleClass ( ROOT_FOLDER_PATH /*static*/, PHOTO_PREFIX, mat.getNativeObjAddr() ) ;
                mat.release();
            }
        } );

//        in the meantime, start streaming the text from the file appended gradually by the native dbscan and OCR
        Executors.newSingleThreadExecutor().execute( new Runnable() {
            @Override
            public void run() {
                self.stream_text_into_view();
            }
        });
    }

    void stream_text_into_view() {

        int rand_sleep;
        for ( int i=0; i< 100; ++i ) {

            rand_sleep = (int)( Math.random() * 1501 + 1);
            try {
                Thread.sleep ( rand_sleep );
            } catch ( InterruptedException e ) {
//            e.printStackTrace();
            }
            self.tv.append( "fuck u: "+i+','+rand_sleep+"\n" );
        }
    }

//    self.tv.setMovementMethod(new ScrollingMovementMethod()); /*scrolling for text view OCR results*/

}
