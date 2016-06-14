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

    SurfaceHolder mHolder;
    Camera mCamera = null;
    ImageView MyCameraPreview = null;
    Bitmap bitmap = null;
    int[] pixels = null;
    byte[] FrameData = null;
    int imageFormat;

    int PreviewSizeWidth;
    int PreviewSizeHeight;

    Parameters parameters;
    int width;
    int height;
    // Matrix matrix;

    boolean bProcessing = false;

    Handler mHandler = new Handler(Looper.getMainLooper());

    static String root_folder_path =  Environment.getExternalStorageDirectory().getAbsolutePath();
    static String path_log = root_folder_path+"/tessdata/img/log.txt";
    // char ccount='A';

    public CameraPreview (
                         int PreviewlayoutWidth,
                         int PreviewlayoutHeight,
                         ImageView cameraPreviewInstance,
                         Camera camera,
                         SurfaceHolder holder) {

        mCamera = camera;
        mHolder = holder;

        // matrix = new Matrix();
        // matrix.postRotate(90);

        PreviewSizeWidth = PreviewlayoutWidth;
        PreviewSizeHeight = PreviewlayoutHeight;

        MyCameraPreview = cameraPreviewInstance;
    }

    // Indian thanks and bows man - that's about all I needed
    @Override
    public void onPreviewFrame ( byte[] arg0, Camera arg1 ) {
        // At preview mode, the frame data will push to here.
        if (imageFormat == ImageFormat.NV21) {
            //We only accept the NV21(YUV420) format.
            if ( !bProcessing ) {
                FrameData = arg0;
                mHandler.post(DoImageProcessing);
            }
        }
    }

    public void onPause() {
        mCamera.stopPreview();
    }

    @Override
    public void surfaceChanged ( SurfaceHolder arg0, int arg1, int arg2, int arg3 ) {

        parameters = mCamera.getParameters ( ) ;
        // Set the camera preview size
        parameters.setPreviewSize ( PreviewSizeWidth, PreviewSizeHeight );

        imageFormat = parameters.getPreviewFormat();
        mCamera.setParameters(parameters);
        mCamera.startPreview();
    }

    @Override
    public void surfaceCreated ( SurfaceHolder arg0 ) {
        // mCamera = Camera.open();
        // if(mCamera==null) mCamera = Camera.open(0);
        try {
            // If did not set the SurfaceHolder, the preview area will be black.
            mCamera.setPreviewDisplay(arg0);
            mCamera.setPreviewCallback(this);
        }
        catch (IOException e) {
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }
    }

    @Override
    public void surfaceDestroyed ( SurfaceHolder arg0 ) {
        try {
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }
        catch(Exception ex){}
    }

    public void refreshCamera ( Camera camera ) {

        if (mHolder.getSurface() == null) {
            // preview surface does not exist
            return;
        }
        // stop preview before making changes
        try {
            mCamera.stopPreview();
        } catch ( Exception e ) {
            // ignore: tried to stop a non-existent preview
        }
        // set preview size and make any resize, rotate or
        // reformatting changes here
        // start preview with new settings
        setCamera(camera);
        try {
            mCamera.setPreviewDisplay(mHolder);
            mCamera.startPreview();
        } catch (Exception e) {
            Log.d("MyRealTimeImageProcessing", "Error starting camera preview: " + e.getMessage());
        }
    }

    public void setCamera ( Camera camera ) {
        //method to set a camera instance
        mCamera = camera;
    }

    // Native JNI
    static {
        System.loadLibrary("pngt");
        System.loadLibrary("lept");
        System.loadLibrary("tess");
        System.loadLibrary("ImageProcessing");
    }

    public native boolean ImageProcessing ( int width, int height, byte[] NV21FrameData, int[] pixels );
    public native boolean colourDetect ( int width, int height, byte[] NV21FrameData, int[] pixels, long mat_out_vec_vec_point, String root_folder_path );

    Runnable DoImageProcessing = new Runnable() {

            public void run() {
                Log.i("MyRealTimeImageProcessing", "DoImageProcessing():");
                bProcessing = true;
                byte[] data = FrameData;

                if ( bitmap==null || pixels==null ) {

                    parameters = mCamera.getParameters();
                    width = parameters.getPreviewSize().width;
                    height = parameters.getPreviewSize().height;
                    pixels = new int [ width * height ];
                }

                Mat mat_out_vec_vec_point = new Mat();


                // call native JNI c++
                colourDetect ( width, height, data, pixels,
                               mat_out_vec_vec_point.nativeObj, /*!*/
                               root_folder_path /*!*/ );

                List<MatOfPoint> contours_poly2 = new ArrayList<MatOfPoint> ( );
                Converters.Mat_to_vector_vector_Point ( mat_out_vec_vec_point, contours_poly2 );

                mat_out_vec_vec_point.release();

                bitmap = Bitmap.createBitmap ( height, width, Bitmap.Config.ARGB_8888 ) ;

                // signature:
                // setPixels(int[] pixels, int offset, int stride, int x, int y, int width, int height)
                // bitmap.setPixels ( pixels, 0, width, 0, 0, width, height ); /*ORIG*/

                Mat mat = new Mat();
                Utils.bitmapToMat ( bitmap, mat );
                // log ("size of c_poly2 is" + contours_poly2.size());
                for  ( int i = 0; i < contours_poly2.size(); ++i ) {
                    Imgproc.drawContours(mat, contours_poly2, i, new Scalar(164,240,64,255), 5);
                }
                Utils.matToBitmap ( mat, bitmap );
                mat.release();

                MyCameraPreview.setImageBitmap ( bitmap ) ;
                // canvas.drawCircle( height/2, width/2, height/2, paint);

                bProcessing = false;
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
