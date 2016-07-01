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

import android.os.FileObserver;
import android.os.Handler;
import android.os.Looper;
import android.os.AsyncTask;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

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
    // final static String OCR_PATH =  ROOT_FOLDER_PATH + "/tessdata/ocr.txt";
    final static String OCR_PATH =  ROOT_FOLDER_PATH + "/tessdata/dump.txt";

    Handler mHandler = new Handler(Looper.getMainLooper());

    ImgProcessOcr self = this;

    FileObserver fileObserver;
    boolean is_observing=false;

    @Override
    void process_im_n_ocr ( final byte[] data ) {

        // pic
        new ImgProcessOCR_processPic().execute ( data );

        self.tv.setText ( "" );
        // new ImgProcessOCR_streamText().execute ( "do it" );

        if( !self.is_observing ) {
            self.is_observing = true;
            self.fileObserver = new FileObserver ( OCR_PATH ) {
                    @Override
                    public void onEvent ( int eevent, String s ) {

                        self.append_txt ( s );
                        switch (eevent){
                        case FileObserver.ACCESS:
                            self.append_txt ( "ACCESS" );
                            break;
                        case FileObserver.ALL_EVENTS:
                            self.append_txt ( "ALL_EVENTS" );
                            break;
                        case FileObserver.ATTRIB:
                            self.append_txt ( "ATTRIB" );
                            break;
                        case FileObserver.CLOSE_NOWRITE:
                            self.append_txt ( "CLOSE_NOWRITE" );
                            break;
                        case FileObserver.CLOSE_WRITE:
                            self.append_txt ( "CLOSE_WRITE" );
                            break;
                        case FileObserver.CREATE:
                            self.append_txt ( "CREATE" );
                            break;
                        case FileObserver.DELETE:
                            self.append_txt ( "DELETE" );
                            break;
                        case FileObserver.DELETE_SELF:
                            self.append_txt ( "DELETE_SELF" );
                            break;
                        case FileObserver.MODIFY:
                            self.append_txt ( "MODIFY" );
                            break;
                        case FileObserver.MOVE_SELF:
                            self.append_txt ( "MOVE_SELF" );
                            break;
                        case FileObserver.MOVED_FROM:
                            self.append_txt ( "MOVED_FROM" );
                            break;
                        case FileObserver.MOVED_TO:
                            self.append_txt ( "MOVED_TO" );
                            break;
                        case FileObserver.OPEN:
                            self.append_txt ( "OPEN" );
                            break;
                        default:
                            self.append_txt ( Integer.toString(eevent) );
                            break;
                        }
                    }
                };
            self.fileObserver.startWatching(); //START OBSERVING
        }
    }

    // Runnable used by the ImgProcessOCR_streamText
    void append_txt ( final String txt ) {

        self.mHandler.post ( new Runnable() {
            @Override
            public void run ( ) {

                self.tv.append ( txt + "\n" );
                // self.tv.setText ( txt );
            }
        } );
    }

    //==================

    // this chap internally saves a pic from the native/c++
    class ImgProcessOCR_processPic extends AsyncTask <byte[], Integer, String> {

        @Override
        protected String doInBackground ( byte[]... params ) {

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

    // OCR result text
    class ImgProcessOCR_streamText extends AsyncTask <String, Integer, String> {

        @Override
        protected String doInBackground ( String... params ) {

            self.append_txt ( "do in background" );
            FileObserver fileObserver = new FileObserver ( OCR_PATH ) {
                    @Override
                    public void onEvent ( int eevent, String s ) {

                        switch (eevent){
                        case FileObserver.CLOSE_NOWRITE:
                        case FileObserver.CLOSE_WRITE:
                            // this.stopWatching();
                            self.append_txt ( "close write or not" );
                            break;
                        }

                        RandomAccessFile raf=null;
                        try {
                            raf = new RandomAccessFile( OCR_PATH, "r" );
                            String txt = raf.readUTF();
                            self.append_txt ( txt );

                        } catch (FileNotFoundException e) {
                            self.append_txt ( "file not found ex" );
                            // e.printStackTrace();
                        } catch (IOException e) {
                            self.append_txt ( "io ex" );
                            // e.printStackTrace();
                        }
                        finally {
                            if(raf!=null) try {
                                    raf.close();
                                } catch (IOException e) {
                                    // e.printStackTrace();
                                }
                        }
                    }
                };
            fileObserver.startWatching(); //START OBSERVING

            return null;
        }
    }
}
