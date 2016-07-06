/*
 *  CameraPreview.java
 */
package my.project.MyRealTimeImageProcessing;

import java.io.*;
import java.util.List;
import java.util.ArrayList;

import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.ImageView;
import android.widget.SeekBar;

import android.graphics.Bitmap;

import android.hardware.Camera;
import android.hardware.Camera.Parameters;

import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.utils.Converters;
import org.opencv.android.Utils;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.Scalar;

//this class is instantiated my the main activity class (currently MyRealTimeImageProcessing - TODO to be renamed)
public class CameraPreview implements SurfaceHolder.Callback, Camera.PreviewCallback {

    // native libs loaded in main class caller
    public native boolean colourDetect ( int width, int height, byte[] NV21frame_data_bytes, int[] pixels, long mat_out_vec_vec_point, String root_folder_path, int[] hsv6 );

    static final String ROOT_FOLDER_PATH =  Environment.getExternalStorageDirectory().getAbsolutePath();
    static final String PATH_LOG = ROOT_FOLDER_PATH +"/tessdata/img/log.txt";
    static final Scalar COLOUR_OBJ_CONTOURS_SCALAR = new Scalar(164,240,64,255); /*green*/
    static final int LINE_WIDTH_PX_OBJ_CONTOURS = 5;

    SurfaceHolder mHolder;
    ImageView cam_preview_img_view = null;
    Bitmap bitmap = null;
    int[] pixels = null;
    byte[] frame_data_bytes = null;
    int imageFormat;

    int preview_size_width;
    int preview_size_height;

    Parameters parameters;
    int width;
    int height;

    Boolean is_processing = false;

    Handler mHandler = new Handler(Looper.getMainLooper());

    Camera mCamera = null;
    void set_mCamera ( Camera camera ) {
        //method to set a camera instance
        this.mCamera = camera;
    }
    // set by the main activity caller
    SeekBar seek_bar_h_low, seek_bar_h_high, seek_bar_s_low, seek_bar_s_high, seek_bar_v_low, seek_bar_v_high;

    CameraPreview self = this;

    public CameraPreview (
                         int preview_layout_width,
                         int preview_layout_height,
                         ImageView cam_preview_im_view_instance,
                         Camera camera,
                         SurfaceHolder holder,
                         SeekBar seek_bar_h_low, SeekBar seek_bar_h_high, SeekBar seek_bar_s_low, SeekBar seek_bar_s_high, SeekBar seek_bar_v_low, SeekBar seek_bar_v_high
                         ) {

        this.mCamera = camera;
        this.mHolder = holder;

        this.preview_size_width = preview_layout_width;
        this.preview_size_height = preview_layout_height;

        this.cam_preview_img_view = cam_preview_im_view_instance;

        self.seek_bar_h_low=seek_bar_h_low; self.seek_bar_h_high=seek_bar_h_high; self.seek_bar_s_low=seek_bar_s_low; self.seek_bar_s_high=seek_bar_s_high; self.seek_bar_v_low=seek_bar_v_low; self.seek_bar_v_high=seek_bar_v_high;
    }

    // Indian thanks and bows man - that's about all I needed
    @Override
    public void onPreviewFrame ( byte[] arg0, Camera arg1 ) {
        // At preview mode, the frame data will push to here.
        // if (imageFormat == ImageFormat.NV21) {
        //We only accept the NV21(YUV420) format.
        if ( !self.is_processing ) {
            self.frame_data_bytes = arg0;
//                this.mHandler.post(this.do_image_processing);
            new CameraPreview_objectDetect().execute("object detect and draw frame/lines on top of video preview" );
        }
        // }
    }

    public void onPause ( ) {
        this.mCamera.stopPreview ( );
    }

    @Override
    public void surfaceChanged ( SurfaceHolder arg0, int arg1, int arg2, int arg3 ) {

        this.parameters = this.mCamera.getParameters ( ) ;
        this.parameters.setPreviewSize ( this.preview_size_width, this.preview_size_height );

        this.imageFormat = this.parameters.getPreviewFormat();
        this.mCamera.setParameters(this.parameters);
        this.mCamera.startPreview();
    }

    @Override
    public void surfaceCreated ( SurfaceHolder arg0 ) {

        try {
            // If did not set the SurfaceHolder, the preview area will be black.
            this.mCamera.setPreviewDisplay(arg0);
            this.mCamera.setPreviewCallback(this);
        }
        catch ( IOException e ) {
            this.mCamera.stopPreview();
            this.mCamera.setPreviewCallback(null);
            this.mCamera.release();
            this.mCamera = null;
        }
    }

    @Override
    public void surfaceDestroyed ( SurfaceHolder arg0 ) {

        try {
            this.mCamera.stopPreview();
            this.mCamera.setPreviewCallback(null);
            this.mCamera.release();
            this.mCamera = null;
        }
        catch ( Exception ex ) { }
    }

    public void refreshCamera ( Camera camera ) {

        if ( this.mHolder.getSurface()==null ) {
            // preview surface does not exist
            return;
        }
        // stop preview before making changes
        try {
            this.mCamera.stopPreview();
        } catch ( Exception e ) {
            // ignore: tried to stop a non-existent preview
        }
        // set preview size and make any resize, rotate or
        // reformatting changes here
        // start preview with new settings
        this.set_mCamera ( camera );

        try {
            this.mCamera.setPreviewDisplay ( this.mHolder );
            this.mCamera.startPreview();
        } catch ( Exception e ) {
            Log.d("evdroid", "Error starting camera preview: " + e.getMessage());
        }
    }

    void set_bitmap ( ) {
        this.mHandler.post(this.run_set_bitmap);
    }

    Runnable run_set_bitmap = new Runnable() {

        public void run() {
            self.cam_preview_img_view.setImageBitmap ( self.bitmap ) ;
//                self.bitmap.recycle();
        }
    };

    //=======================
    class CameraPreview_objectDetect extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground ( String... params ) {

            Log.i("evdroid", "do_image_processing():");
            self.is_processing = true;

            if ( self.bitmap==null || self.pixels==null ) {

                self.parameters = self.mCamera.getParameters();
                self.width = self.parameters.getPreviewSize().width;
                self.height = self.parameters.getPreviewSize().height;
                self.pixels = new int [ self.width * self.height ];
            }

            Mat mat_out_vec_vec_point = new Mat();

            int[] hsv6 = new int[6];
            hsv6[0] = self.seek_bar_h_low.getProgress(); hsv6[1] = self.seek_bar_s_low.getProgress(); hsv6[2] = self.seek_bar_v_low.getProgress();
            hsv6[3] = self.seek_bar_h_high.getProgress(); hsv6[4] = self.seek_bar_s_high.getProgress(); hsv6[5] = self.seek_bar_v_high.getProgress();

            // call native JNI c++
            colourDetect ( self.width, self.height, self.frame_data_bytes, pixels,
                    mat_out_vec_vec_point.nativeObj, /*!*/
                    ROOT_FOLDER_PATH /*!*/, hsv6 );

            List<MatOfPoint> contours_poly2 = new ArrayList<MatOfPoint> ( ); /*will have the points for the object outlines, they will be drawn by drawContours*/
            Converters.Mat_to_vector_vector_Point ( mat_out_vec_vec_point, contours_poly2 );

            mat_out_vec_vec_point.release();

            self.bitmap = Bitmap.createBitmap ( self.height, self.width, Bitmap.Config.ARGB_8888 ) ; /*mind the height and width reversed - PORTRAIT mode*/

            // signature:
            // setPixels(int[] pixels, int offset, int stride, int x, int y, int width, int height)
            // bitmap.setPixels ( pixels, 0, width, 0, 0, width, height ); /*ORIG*/

            Mat mat = new Mat();
            Utils.bitmapToMat ( self.bitmap, mat );
            // log ("size of c_poly2 is" + contours_poly2.size());
            for  ( int i = 0; i < contours_poly2.size(); ++i ) {
                Imgproc.drawContours(mat, contours_poly2, i, COLOUR_OBJ_CONTOURS_SCALAR, LINE_WIDTH_PX_OBJ_CONTOURS);
            }
            Utils.matToBitmap ( mat, self.bitmap );
            mat.release();

//            self.cam_preview_img_view.setImageBitmap ( self.bitmap ) ;
            // canvas.drawCircle( height/2, width/2, height/2, paint);
            self.set_bitmap();

            self.is_processing = false;

            //=========
            return null;
        }
    }

    static void log ( String s ) {

        BufferedWriter bw = null;

        try {
            bw = new BufferedWriter ( new FileWriter(PATH_LOG, "append mode? no time to check"!=null ) );
            bw.write(s);
            bw.newLine();
            bw.flush();
        } catch (IOException ex) {
            // throw ex;
        } finally {
            if (bw != null) try {
                    bw.close();
                } catch (IOException ex2) { }
        }
    }
}
