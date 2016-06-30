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

// import java.util.concurrent.Executors;
import android.os.FileObserver;
import android.os.Handler;
import android.os.Looper;
import android.os.AsyncTask;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

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

    Handler mHandler = new Handler(Looper.getMainLooper());

    ImgProcessOcr self = this;

    @Override
    void process_im_n_ocr ( final byte[] data ) {

//      process the picture taken async
        // self.mHandler.post( new Runnable() {
        //     @Override
        //     public void run() {

        //         Mat mat=new Mat();
        //         Bitmap bmp = BitmapFactory.decodeByteArray ( data, 0, data.length );
        //         Utils.bitmapToMat ( bmp, mat ); bmp.recycle();
        //         Imgproc.cvtColor ( mat, mat, Imgproc.COLOR_RGB2BGR );

        //         // JNI native call
        //         saveMiddleClass ( ROOT_FOLDER_PATH /*static*/, PHOTO_PREFIX, mat.getNativeObjAddr() ) ;
        //         mat.release();
        //     }
        // } );

        // pic
        new ImgProcessOCR_processPic().execute ( data );

        FileObserver fileObserver = new FileObserver ( OCR_PATH ) {
            @Override
            public void onEvent ( int i, String s ) {

//                FileObserver.CREATE;
//                FileObserver.DELETE;
//                FileObserver.MODIFY;
//                FileObserver.ACCESS;
//                FileObserver.OPEN;
                  this.stopWatching();
                try {
                    FileInputStream fs = new FileInputStream ( OCR_PATH );
                    fs.read ( new byte[10] );
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        };
        fileObserver.startWatching(); //START OBSERVING

        // OCR result text
        self.tv.setText ( "" );
        new ImgProcessOCR_streamText().execute ( "fuck ass" );
        new ImgProcessOCR_streamText().execute ( "screw u" );
        new ImgProcessOCR_streamText().execute ( "tits" );

        // self.tv.append( "\nfuck u ass hole\n" );
//        in the meantime, start streaming the text from the file appended gradually by the native dbscan and OCR
        // self.mHandler.post ( new Runnable() {
        //     @Override
        //     public void run() {
        //         self.tv.setText("");
        //         self.stream_text_into_view();
        //     }
        // } );
    }

    // Runnable used by the ImgProcessOCR_streamText
    void append_txt ( final String txt ) {

        self.mHandler.post ( new Runnable() {
            @Override
            public void run ( ) {
                self.tv.append ( txt );
            }
        } );
    }

//     void stream_text_into_view() {

//         int rand_sleep;
//         for ( int i=0; i< 50; ++i ) {

//             rand_sleep = (int)( Math.random() * 1501 + 1);
//            //  try {
// //                 Thread.sleep ( rand_sleep );
// //             } catch ( InterruptedException e ) {
// // //            e.printStackTrace();
// //             }
//             self.tv.append( "fuck u: "+i+','+rand_sleep+"\n" );
//         }
//     }

//    self.tv.setMovementMethod(new ScrollingMovementMethod()); /*scrolling for text view OCR results*/

    //==================

    // this chap internally saves a pic from the native/c++
    class ImgProcessOCR_processPic extends AsyncTask <byte[], Integer, String> {

        //        @Override
        //        protected void onPreExecute() {}

        @Override
        protected String doInBackground ( byte[]... params ) {

            //            this.publishProgress(1);
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

        //        @Override
        //        protected void onProgressUpdate(Integer... values) {}
        //
        //        @Override
        //        protected void onPostExecute(String result) {}
    }

    //==================

    class ImgProcessOCR_streamText extends AsyncTask <String, Integer, String> {

        //        @Override
        //        protected void onPreExecute() {}

        @Override
        protected String doInBackground ( String... params ) {

            //            this.publishProgress(1);
            int rand_sleep;
            String prm = params[0];
            for ( int i=0; i< 50; ++i ) {

                rand_sleep = (int)( Math.random() * 701 + 1);
                try {
                    Thread.sleep ( rand_sleep );
                } catch ( InterruptedException e ) {
                    //            e.printStackTrace();
                }
                self.append_txt ( prm + " §§§±§§§ " + i+ ", " +rand_sleep+"\n" );
            }
            return null;
        }

        //        @Override
        //        protected void onProgressUpdate(Integer... values) {}
        //
        //        @Override
        //        protected void onPostExecute(String result) {}
    }
}
