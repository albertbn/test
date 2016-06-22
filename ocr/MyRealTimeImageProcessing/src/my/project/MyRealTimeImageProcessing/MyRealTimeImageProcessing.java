//
//
//
package my.project.MyRealTimeImageProcessing;

import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.PictureCallback;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Surface;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.Toast;

import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;

public class MyRealTimeImageProcessing extends LayoutsNControls {

    final static String PHOTO_PREFIX = "smc"; /*prefix of the high resolution photo/picture taken*/
    final static String ROOT_FOLDER_PATH =  Environment.getExternalStorageDirectory().getAbsolutePath();

    final int PREVIEW_SIZE_WIDTH = 480, PREVIEW_SIZE_HEIGHT = 640, PHOTO_WIDTH = 2048, PHOTO_HEIGHT = 1536;

    Camera mCamera;
    CameraPreview cam_preview;
    PictureCallback m_picture_callback;
    Context context;
    ImageView iv_cam_preview = null;

    MyRealTimeImageProcessing self = this;

    BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                } break;
                default: {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

    static void set_camera_display_orientation ( Activity activity,
                                               int cameraId,
                                               android.hardware.Camera camera
    ) {
        CameraInfo info = new CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }

        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360;
            result = (360 - result) % 360; // compensate the mirror
        } else { // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
        camera.setDisplayOrientation(result);
    }

    @Override
    public void onCreate ( Bundle savedInstanceState ) {

        super.onCreate(savedInstanceState);

        //Set this APK Full screen
        self.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        // from andrew cam example - other project
        self.context = this;

        //Set this APK no title
        self.requestWindowFeature(Window.FEATURE_NO_TITLE);
        self.setContentView(R.layout.main);

        // Create my camera preview
        self.iv_cam_preview = new ImageView(this);

        SurfaceView surface_cam_view = new SurfaceView(this);
        SurfaceHolder surface_cam_view_holder = surface_cam_view.getHolder();
        self.cam_preview = new CameraPreview ( self.PREVIEW_SIZE_HEIGHT, self.PREVIEW_SIZE_WIDTH, self.iv_cam_preview, self.mCamera, surface_cam_view_holder,
                                             self.bar_h_low, self.bar_h_high, self.bar_s_low, self.bar_s_high, self.bar_v_low, self.bar_v_high);

        surface_cam_view_holder.addCallback(self.cam_preview);
        surface_cam_view_holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        super.lay_video_preview_wrap.addView(surface_cam_view, new LayoutParams(self.PREVIEW_SIZE_WIDTH, self.PREVIEW_SIZE_HEIGHT));
        super.lay_video_preview_wrap.addView(self.iv_cam_preview, new LayoutParams(self.PREVIEW_SIZE_WIDTH, self.PREVIEW_SIZE_HEIGHT));

        super.btn_flash.setOnClickListener(self.flash_listener); /* uses camera, that's why not in base */
        super.btn_capture.setOnClickListener(self.capture_listener); /* uses camera, that's why not in base */
    }

    @Override
    protected void onPause ( ) {

        if ( self.cam_preview != null) self.cam_preview.onPause();
        super.onPause();

        //on Pause, release camera in order to be used from other applications
        self.release_camera();
    }

    @Override
    public void onResume ( ) {

        super.onResume();

        if ( !OpenCVLoader.initDebug() ) {
            // Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            // Log.d(TAG, "OpenCV library found inside package. Using it!");
            self.mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }

        if ( !self.has_camera() ) {
            Toast toast =
                Toast.makeText(self.context, "Sorry, your phone does not have a camera!", Toast.LENGTH_LONG);
            toast.show();
            self.finish();
        }
        if ( self.mCamera == null ) {

            self.mCamera = Camera.open(0);

            set_camera_display_orientation( this, CameraInfo.CAMERA_FACING_BACK, self.mCamera );

            //start focus
            Camera.Parameters params = mCamera.getParameters();
            Camera.Size size = self.get_best_preview_size( self.PHOTO_WIDTH, self.PHOTO_HEIGHT, params );

            params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            params.setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);

            params.setPictureSize(size.width, size.height);
            params.setJpegQuality(100);
            self.mCamera.setParameters(params);
            //end focus

            self.m_picture_callback = self.get_picture_callback();
            self.cam_preview.refreshCamera(self.mCamera);
        }
    }

    //set/used from onCreate
    final OnClickListener flash_listener = new OnClickListener() {
            @Override
            public void onClick ( View v ) {
                Camera.Parameters params = self.mCamera.getParameters();
                if ( self.is_torch_on ) {
                    self.is_torch_on=false;
                    params.setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);
                }
                else {
                    self.is_torch_on=true;
                    params.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
                }
                self.mCamera.setParameters(params);
            }
    };

    //set/used from onCreate
    final OnClickListener capture_listener = new OnClickListener() {
        @Override
        public void onClick ( View v ) {
            mCamera.takePicture ( null, null, m_picture_callback ) ;
        }
    };

    // credits: https://github.com/commonsguy/cw-advandroid/blob/master/Camera/Preview/src/com/commonsware/android/camera/PreviewDemo.java
    Camera.Size get_best_preview_size(
                                   int width,
                                   int height,
                                   Camera.Parameters parameters
                                   ) {
    Camera.Size result=null;

    for ( Camera.Size size : parameters.getSupportedPictureSizes() ) {

        if ( size.width<=width && size.height<=height ) {
        if (result==null) {
          result=size;
        }
        else {
          int resultArea=result.width*result.height;
          int newArea=size.width*size.height;

          if ( newArea>resultArea ) {
            result=size;
          }
        }
      }
    }

    return result;
  }

    //callback - trace - from captureListener > onClick > cam.takePicture
    final PictureCallback get_picture_callback ( ) {

        PictureCallback picture_callback = new PictureCallback ( ) {

            @Override
            public void onPictureTaken ( byte[] data, Camera camera ) {

                self.lay_main_wrap_preview.setVisibility(View.GONE); /*super*/
                self.lay_wrap_sliders.setVisibility(View.GONE); /*super*/
                self.lay_text_result.setVisibility(View.VISIBLE); /*super*/
                self.process_im_n_ocr(data);
            }
        };

        return picture_callback;
    }

    //called from on pause
    final void release_camera ( ) {
        // stop and release camera
        if ( self.mCamera != null ) {
            self.mCamera.stopPreview();
            self.mCamera.setPreviewCallback(null);
            self.mCamera.release();
            self.mCamera = null;
        }
    }

    //boolean has_camera ( Context context ) {
    final boolean has_camera ( ) {
        //check if the device has camera
        // return context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
        return true; /*fuck you*/
    }

    //========= virtual methods overridden by children =========//
    void process_im_n_ocr( byte[] data ) { /* virtual, overridden by child */ }
}
