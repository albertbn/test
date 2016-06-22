//
//
//
package my.project.MyRealTimeImageProcessing;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

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

    // TODO - maybe change the native signatures according to top child?
    // credits: http://stackoverflow.com/questions/9978011/android-ics-jni-error-attempt-to-use-stale-local-reference-0x1#12824591 - NOT Boolean
    public native boolean saveMiddleClass ( String root_folder_path, String img_unique_no_ext, long inputImage );  /*!not Boolean!!!*/

    @Override
    void process_im_n_ocr ( byte[] data ) {

        Mat mat=new Mat();
        Bitmap bmp = BitmapFactory.decodeByteArray ( data, 0, data.length );
        Utils.bitmapToMat ( bmp, mat ); bmp.recycle();
        Imgproc.cvtColor ( mat, mat, Imgproc.COLOR_RGB2BGR );

        // JNI native call
        saveMiddleClass ( ROOT_FOLDER_PATH /*static*/, PHOTO_PREFIX, mat.getNativeObjAddr() ) ;
        mat.release();
    }
}
