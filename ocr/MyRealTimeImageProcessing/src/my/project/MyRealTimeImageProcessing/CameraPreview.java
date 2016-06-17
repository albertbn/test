/*
 *  CameraPreview.java
 */
package my.project.MyRealTimeImageProcessing;

import java.io.*;
import java.util.List;
import java.util.ArrayList;

import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.ImageView;
import android.widget.TextView;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.YuvImage;
import android.graphics.Rect;
import android.graphics.Matrix;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;

import android.hardware.Camera;
import android.hardware.Camera.Parameters;

import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.utils.Converters;
import org.opencv.android.Utils;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.Scalar;

public class CameraPreview implements SurfaceHolder.Callback, Camera.PreviewCallback {

    static String root_folder_path =  Environment.getExternalStorageDirectory().getAbsolutePath();
    static String path_log = root_folder_path+"/tessdata/img/log.txt";
    static Scalar colour_obj_contours_scalar = new Scalar(164,240,64,255); /*green*/
    static final int line_width_px_obj_contours = 5;

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
    TextView h_low_text, h_high_text, s_low_text, s_high_text, v_low_text, v_high_text;

    CameraPreview self = this;

    public CameraPreview (
                         int preview_layout_width,
                         int preview_layout_height,
                         ImageView cam_preview_im_view_instance,
                         Camera camera,
                         SurfaceHolder holder,
                         TextView h_low_text, TextView h_high_text, TextView s_low_text, TextView s_high_text, TextView v_low_text, TextView v_high_text) {

        this.mCamera = camera;
        this.mHolder = holder;

        this.preview_size_width = preview_layout_width;
        this.preview_size_height = preview_layout_height;

        this.cam_preview_img_view = cam_preview_im_view_instance;

        self.h_low_text=h_low_text; self.h_high_text=h_high_text; self.s_low_text=s_low_text; self.s_high_text=s_high_text; self.v_low_text=v_low_text; self.v_high_text=v_high_text;
    }

    // Indian thanks and bows man - that's about all I needed
    @Override
    public void onPreviewFrame ( byte[] arg0, Camera arg1 ) {
        // At preview mode, the frame data will push to here.
        // if (imageFormat == ImageFormat.NV21) {
            //We only accept the NV21(YUV420) format.
            if ( !this.is_processing ) {
                this.frame_data_bytes = arg0;
                this.mHandler.post(this.do_image_processing);
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

    // Native JNI - load libraries
    static {
        System.loadLibrary("pngt");
        System.loadLibrary("lept");
        System.loadLibrary("tess");
        System.loadLibrary("ImageProcessing");
    }

    public native boolean colourDetect ( int width, int height, byte[] NV21frame_data_bytes, int[] pixels, long mat_out_vec_vec_point, String root_folder_path, int[] hsv6 );

    Runnable do_image_processing = new Runnable() {

            public void run() {
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
                hsv6[0] = Integer.parseInt ( h_low_text.getText().toString() ); hsv6[1] = Integer.parseInt ( s_low_text.getText().toString() ); hsv6[2] = Integer.parseInt ( v_low_text.getText().toString() );
                hsv6[3] = Integer.parseInt ( h_high_text.getText().toString() ); hsv6[4] = Integer.parseInt ( s_high_text.getText().toString() ); hsv6[5] = Integer.parseInt ( v_high_text.getText().toString() );
//                        h_low_text, h_high_text, s_low_text, s_high_text, v_low_text, v_high_text
                // call native JNI c++
                colourDetect ( width, height, self.frame_data_bytes, pixels,
                               mat_out_vec_vec_point.nativeObj, /*!*/
                               root_folder_path /*!*/, hsv6 );

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
                    Imgproc.drawContours(mat, contours_poly2, i, colour_obj_contours_scalar, line_width_px_obj_contours);
                }
                Utils.matToBitmap ( mat, self.bitmap );
                mat.release();

                cam_preview_img_view.setImageBitmap ( self.bitmap ) ;
                // canvas.drawCircle( height/2, width/2, height/2, paint);

                is_processing = false;
            }
        };

    static void log ( String s ) {

        BufferedWriter bw = null;

        try {
            bw = new BufferedWriter ( new FileWriter(path_log, "append mode? no time to check"!=null ) );
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
