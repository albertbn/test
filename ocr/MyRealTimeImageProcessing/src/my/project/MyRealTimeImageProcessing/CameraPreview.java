/*
 *  CameraPreview.java
 */
package my.project.MyRealTimeImageProcessing;

import java.io.IOException;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.ImageView;

import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;

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
    boolean bProcessing = false;

    Handler mHandler = new Handler(Looper.getMainLooper());

    public CameraPreview (
                         int PreviewlayoutWidth,
                         int PreviewlayoutHeight,
                         ImageView CameraPreview,
                         Camera camera,
                         SurfaceHolder holder) {
        mCamera = camera;
        mHolder = holder;

        PreviewSizeWidth = PreviewlayoutWidth;
        PreviewSizeHeight = PreviewlayoutHeight;
        MyCameraPreview = CameraPreview;
        bitmap = Bitmap.createBitmap(PreviewSizeWidth, PreviewSizeHeight, Bitmap.Config.ARGB_8888);
        pixels = new int[PreviewSizeWidth * PreviewSizeHeight];
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

        Parameters parameters = mCamera.getParameters();
        // Set the camera preview size
        parameters.setPreviewSize(PreviewSizeWidth, PreviewSizeHeight);
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
    public native boolean ImageProcessing ( int width, int height, byte[] NV21FrameData, int [] pixels );
    public native boolean colourDetect ( int width, int height, byte[] NV21FrameData );

    static {
        System.loadLibrary("pngt");
        System.loadLibrary("lept");
        System.loadLibrary("tess");
        System.loadLibrary("ImageProcessing");
    }

    Runnable DoImageProcessing = new Runnable() {

            public void run() {
                Log.i("MyRealTimeImageProcessing", "DoImageProcessing():");
                bProcessing = true;
                // ImageProcessing(PreviewSizeWidth, PreviewSizeHeight, FrameData, pixels);
                // try to modify the FrameData mat
                colourDetect ( PreviewSizeWidth, PreviewSizeHeight, FrameData );

                // bitmap.setPixels(pixels, 0, PreviewSizeWidth, 0, 0, PreviewSizeWidth, PreviewSizeHeight);
                // MyCameraPreview.setImageBitmap(bitmap);
                bProcessing = false;
            }
        };
}
