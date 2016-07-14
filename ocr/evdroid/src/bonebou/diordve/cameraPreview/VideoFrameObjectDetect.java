package bonebou.diordve.cameraPreview;

import android.graphics.Bitmap;
import android.hardware.Camera;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.ImageView;
import android.widget.SeekBar;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;
import org.opencv.utils.Converters;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by albert on 7/14/16.
 * this chap just gets the video preview frame bytes, makes object detection in native,
 * gets back a bunch of points, which are used to draw lines over the actual preview frame image (a green border surrounding the receipt)
 */
public class VideoFrameObjectDetect implements Camera.PreviewCallback {

    // native libs loaded in main class caller - does object detection - white rectangle/paper invoice
    public native boolean colourDetect ( int width, int height, byte[] NV21frame_data_bytes, int[] pixels, long mat_out_vec_vec_point, String root_folder_path, int[] hsv6 );

    static final String LOG_KEY = "evdroid::cameraPreview";
    static final String ROOT_FOLDER_PATH =  Environment.getExternalStorageDirectory().getAbsolutePath();
    static final Scalar COLOUR_OBJ_CONTOURS_SCALAR = new Scalar(164,240,64,255); /*green*/
    static final int LINE_WIDTH_PX_OBJ_CONTOURS = 5;

    boolean is_processing = false;
    byte[] frame_data_bytes = null;
    int width, height;
    int[] pixels = null;
    Bitmap bitmap = null;
    Camera.Parameters parameters;

    // set by the main activity caller
    Camera mCamera = null;
    ImageView cam_preview_img_view = null;
    SeekBar seek_bar_h_low, seek_bar_h_high, seek_bar_s_low, seek_bar_s_high, seek_bar_v_low, seek_bar_v_high;

    //===
    Handler mHandler = new Handler(Looper.getMainLooper());

    VideoFrameObjectDetect self = this;
    //=========
    //CONSTRUCT
    //=========
    public VideoFrameObjectDetect ( Camera mCamera, ImageView cam_preview_img_view, SeekBar[] bars /*comes low,high, low, high...*/ ) {

        this.mCamera = mCamera;
        this.cam_preview_img_view = cam_preview_img_view;
        self.seek_bar_h_low=bars[0]; self.seek_bar_h_high=bars[1]; self.seek_bar_s_low=bars[2];
        self.seek_bar_s_high=bars[3]; self.seek_bar_v_low=bars[4]; self.seek_bar_v_high=bars[5];
    }

    @Override
    public void onPreviewFrame ( byte[] bytes, Camera camera ) {

        //We only accept the NV21(YUV420) format. if (imageFormat == ImageFormat.NV21) {
        if ( !self.is_processing ) {
            self.frame_data_bytes = bytes;
            new ObjectDetect().execute (
                    self.seek_bar_h_low.getProgress(),
                    self.seek_bar_s_low.getProgress(),
                    self.seek_bar_v_low.getProgress(),
                    self.seek_bar_h_high.getProgress(),
                    self.seek_bar_s_high.getProgress(),
                    self.seek_bar_v_high.getProgress()
            );
        }
    }
    //=======================
    void set_bitmap ( ) {
        self.mHandler.post(self.run_set_bitmap);
    }

    Runnable run_set_bitmap = new Runnable ( ) {

        public void run() {
            self.cam_preview_img_view.setImageBitmap ( self.bitmap ) ;
            // self.bitmap.recycle(); /* TODO - check if you can release the bitmap after set/render? */
        }
    };

    void set_fields_as_needed(){

        if ( self.bitmap!=null && self.pixels!=null ) return;

        self.parameters = self.mCamera.getParameters();
        self.width = self.parameters.getPreviewSize().width;
        self.height = self.parameters.getPreviewSize().height;
        self.pixels = new int[0]; /* TODO dummy - try to ignore it in cpp*/
    }

    void draw_lines_from_native_points ( Mat mat_out_vec_vec_point ) {

        List<MatOfPoint> contours_poly2 = new ArrayList<MatOfPoint>( ); /* will have the points for the object outlines, they will be drawn by drawContours */
        Converters.Mat_to_vector_vector_Point ( mat_out_vec_vec_point, contours_poly2 );
        mat_out_vec_vec_point.release ( );

        self.bitmap = Bitmap.createBitmap ( self.height, self.width, Bitmap.Config.ARGB_8888 ) ; /* mind the height and width reversed - PORTRAIT mode */
        Mat mat = new Mat();
        Utils.bitmapToMat ( self.bitmap, mat );
        // log ("size of c_poly2 is" + contours_poly2.size());
        for  ( int i = 0; i < contours_poly2.size(); ++i ) {
            Imgproc.drawContours ( mat, contours_poly2, i, COLOUR_OBJ_CONTOURS_SCALAR, LINE_WIDTH_PX_OBJ_CONTOURS );
        }
        contours_poly2.clear();

        Utils.matToBitmap ( mat, self.bitmap );
        mat.release();
    }
    //=======================
    // calls native to execute object detection based on hsv colour detection
    class ObjectDetect extends AsyncTask<Integer, Integer, String> {

        @Override
        protected String doInBackground ( Integer... params ) {
            Log.i( LOG_KEY, "VideoFrameObjectDetect::ObjectDetect : AsyncTask");

            //=========
            self.is_processing = true;
            //=========

            self.set_fields_as_needed();

            Mat mat_out_vec_vec_point = new Mat();

            int[] hsv6 = new int[6];
            hsv6[0] = params[0]; hsv6[1] = params[1]; hsv6[2] = params[2];
            hsv6[3] = params[3]; hsv6[4] = params[4]; hsv6[5] = params[5];

            // call native JNI c++
            colourDetect ( self.width, self.height, self.frame_data_bytes, pixels,
                    mat_out_vec_vec_point.nativeObj, /*!*/
                    ROOT_FOLDER_PATH /*!*/, hsv6 ); self.pixels=new int[0]; self.frame_data_bytes = new byte[0];

            self.draw_lines_from_native_points(mat_out_vec_vec_point);

            self.set_bitmap();

            //=========
            self.is_processing = false;
            //=========

            return null;
        }
    }
}
