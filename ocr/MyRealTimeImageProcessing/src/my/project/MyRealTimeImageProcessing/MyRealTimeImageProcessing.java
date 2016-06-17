//
//
//
package my.project.MyRealTimeImageProcessing;

//import java.io.FileNotFoundException;
//import java.io.FileOutputStream;
//import java.io.IOException;

import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.PictureCallback;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
//import org.opencv.core.Core;
import org.opencv.core.Mat;
//import org.opencv.imgcodecs.Imgcodecs;
//import org.opencv.core.CvType;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.imgproc.Imgproc;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Surface;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
//import android.widget.FrameLayout;
import android.widget.RelativeLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.text.method.ScrollingMovementMethod;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
//import android.hardware.Camera.PictureCallback;
//import android.content.pm.PackageManager;
import android.widget.Button;

public class MyRealTimeImageProcessing extends Activity {

    final int PREVIEW_SIZE_WIDTH = 480;
    final int PREVIEW_SIZE_HEIGHT = 640;
    final int PHOTO_WIDTH = 1536; /*2448*/
    final int PHOTO_HEIGHT =  2048; /*3264*/

    //HRS for white is (0,0,255)
    final int H_MIN=0, S_MIN=0, V_MIN=200;
    final int H_MAX=10, S_MAX=10, V_MAX=255;

    Camera mCamera;
    CameraPreview cam_preview;
    PictureCallback mPicture;
    Context context;
    ImageView iv_cam_preview = null;

    RelativeLayout rl_video_preview_wrap1;
    Button capture, flash;
    Boolean is_torch_on = false;
    TextView tv = null;

    SeekBar seek_bar_h_low, seek_bar_h_high, seek_bar_s_low, seek_bar_s_high, seek_bar_v_low, seek_bar_v_high;
    TextView h_low_text, h_high_text, s_low_text, s_high_text, v_low_text, v_high_text;

    MyRealTimeImageProcessing self = this;

    BaseLoaderCallback  mLoaderCallback = new BaseLoaderCallback(this) {
            @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS: {
                    //fuck
                } break;
                default: {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

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
        self.cam_preview = new CameraPreview(self.PREVIEW_SIZE_WIDTH, self.PREVIEW_SIZE_HEIGHT, self.iv_cam_preview, self.mCamera, surface_cam_view_holder,
                h_low_text, h_high_text, s_low_text, s_high_text, v_low_text, v_high_text );

        surface_cam_view_holder.addCallback(self.cam_preview);
        surface_cam_view_holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        self.rl_video_preview_wrap1 = (RelativeLayout) self.findViewById(R.id.rl_video_preview_wrap1);
        self.rl_video_preview_wrap1.addView(surface_cam_view, new LayoutParams(self.PREVIEW_SIZE_WIDTH, self.PREVIEW_SIZE_HEIGHT));
        self.rl_video_preview_wrap1.addView(self.iv_cam_preview, new LayoutParams(self.PREVIEW_SIZE_WIDTH, self.PREVIEW_SIZE_HEIGHT));
        self.capture = (Button) self.findViewById(R.id.button_capture);
        self.capture.setOnClickListener(self.captureListener);

        self.flash = (Button) self.findViewById(R.id.button_flash);
        self.flash.setOnClickListener(self.flash_listener);

        self.tv = (TextView) self.findViewById(R.id.tv_dump);
        self.tv.setMovementMethod(new ScrollingMovementMethod());

        self.set_sliders();
    }

    void set_sliders() {
        //slider displays
        self.h_low_text = (TextView) self.findViewById(R.id.h_low_text); self.h_high_text = (TextView) self.findViewById(R.id.h_high_text);
        self.s_low_text = (TextView) self.findViewById(R.id.s_low_text); self.s_high_text = (TextView) self.findViewById(R.id.s_high_text);
        self.v_low_text = (TextView) self.findViewById(R.id.v_low_text); self.v_high_text = (TextView) self.findViewById(R.id.v_high_text);

        self.seek_bar_h_low = (SeekBar) self.findViewById( R.id.seek_bar_h_low ); self.seek_bar_h_high = (SeekBar) self.findViewById( R.id.seek_bar_h_high );
        self.seek_bar_s_low = (SeekBar) self.findViewById( R.id.seek_bar_s_low ); self.seek_bar_s_high = (SeekBar) self.findViewById( R.id.seek_bar_s_high );
        self.seek_bar_v_low = (SeekBar) self.findViewById( R.id.seek_bar_v_low ); self.seek_bar_v_high = (SeekBar) self.findViewById( R.id.seek_bar_v_high );
        self.seek_bar_h_low.setOnSeekBarChangeListener(self.sb_listen); self.seek_bar_h_high.setOnSeekBarChangeListener(self.sb_listen);
        self.seek_bar_s_low.setOnSeekBarChangeListener(self.sb_listen); self.seek_bar_s_high.setOnSeekBarChangeListener(self.sb_listen);
        self.seek_bar_v_low.setOnSeekBarChangeListener(self.sb_listen); self.seek_bar_v_high.setOnSeekBarChangeListener(self.sb_listen);
        //set initial HSV stuff (HSV white is 0,0,255)
        self.seek_bar_h_low.setProgress(self.H_MIN); self.seek_bar_h_high.setProgress(self.H_MAX);
        self.seek_bar_s_low.setProgress(self.S_MIN); self.seek_bar_s_high.setProgress(self.S_MAX);
        self.seek_bar_v_low.setProgress(self.V_MIN); self.seek_bar_v_high.setProgress(self.V_MAX);
    }

    @Override
    protected void onPause ( ) {

        if ( self.cam_preview != null)
            self.cam_preview.onPause();
        super.onPause();

        //when on Pause, release camera in order to be used from other applications
        self.release_camera();
    }

    static void setCameraDisplayOrientation ( Activity activity,
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
    public void onResume ( ) {

        super.onResume();

        if ( !OpenCVLoader.initDebug() ) {
            // Log.d(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
            OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_3_0_0, this, mLoaderCallback);
        } else {
            // Log.d(TAG, "OpenCV library found inside package. Using it!");
            self.mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }

        if ( !self.has_camera(self.context) ) {
            Toast toast =
                Toast.makeText(self.context, "Sorry, your phone does not have a camera!", Toast.LENGTH_LONG);
            toast.show();
            self.finish();
        }
        if ( self.mCamera == null ) {

            self.mCamera = Camera.open(0);

            setCameraDisplayOrientation ( this, CameraInfo.CAMERA_FACING_BACK, self.mCamera );

            //start focus
            Camera.Parameters params = mCamera.getParameters();
            Camera.Size size = getBestPreviewSize ( self.PHOTO_WIDTH, self.PHOTO_HEIGHT, params );

            params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            params.setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);

            params.setPictureSize(size.width, size.height);
            params.setJpegQuality(100);
            self.mCamera.setParameters(params);
            //end focus

            self.mPicture = getPictureCallback();
            self.cam_preview.refreshCamera(self.mCamera);
        }
    }

    //=============

    OnSeekBarChangeListener sb_listen = new OnSeekBarChangeListener() {

        int progress = 0;

        @Override
        public void onProgressChanged(SeekBar seekBar, int progressValue, boolean fromUser) {

            progress = progressValue;

            switch ( seekBar.getId() ){
                case R.id.h_low_text:
                    self.h_low_text.setText(progress); break;
                case R.id.h_high_text:
                    self.h_high_text.setText(progress); break;
                case R.id.s_low_text:
                    self.s_low_text.setText(progress); break;
                case R.id.s_high_text:
                    self.s_high_text.setText(progress); break;
                case R.id.v_low_text:
                    self.v_low_text.setText(progress); break;
                case R.id.v_high_text:
                    self.v_high_text.setText(progress); break;
            }

            // Toast.makeText(getApplicationContext(), "Changing seekbar progress: "+progress, Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            // Toast.makeText(getApplicationContext(), "Started tracking seekbar", Toast.LENGTH_SHORT).show();
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            // textView.setText("Covered: " + progress + "/" + seekBar.getMax());
            // Toast.makeText(getApplicationContext(), "Stopped tracking seekbar", Toast.LENGTH_SHORT).show();
        }
    };
    //=============

    //set from onCreate
    OnClickListener flash_listener = new OnClickListener() {
            @Override
            public void onClick ( View v ) {
                Camera.Parameters params = self.mCamera.getParameters();
                if(self.is_torch_on){
                    self.is_torch_on=false;
                    params.setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);
                }
                else{
                    self.is_torch_on=true;
                    params.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
                }
                self.mCamera.setParameters(params);
            }
    };

    //set from onCreate
    OnClickListener captureListener = new OnClickListener() {
        @Override
        public void onClick ( View v ) {
            mCamera.takePicture ( null, null, mPicture ) ;
        }
    };

    // credits: https://github.com/commonsguy/cw-advandroid/blob/master/Camera/Preview/src/com/commonsware/android/camera/PreviewDemo.java
    Camera.Size getBestPreviewSize (
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

    public native Boolean saveMiddleClass ( String root_folder_path, String img_unique_no_ext, long inputImage );
    void tweak_bytes ( byte[] data ) {

        Mat mat=new Mat();
        Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
        Utils.bitmapToMat(bmp, mat);   //converting a mat to bitmap
        bmp=null;
        Imgproc.cvtColor ( mat, mat, Imgproc.COLOR_RGB2BGR );
        String root_folder =  Environment.getExternalStorageDirectory().getAbsolutePath();
        saveMiddleClass ( root_folder, "smc", mat.getNativeObjAddr() ) ;
    }

    //callback - trace - from captureListener > onClick > cam.takePicture
    PictureCallback getPictureCallback ( ) {

        PictureCallback picture = new PictureCallback ( ) {

            @Override
            public void onPictureTaken ( byte[] data, Camera camera ) {

                self.tweak_bytes(data) ; return;
            }
        };

        return picture;
    }

    void release_camera() {
        // stop and release camera
        if (self.mCamera != null) {
            self.mCamera.stopPreview();
            self.mCamera.setPreviewCallback(null);
            self.mCamera.release();
            self.mCamera = null;
        }
    }

    Boolean has_camera ( Context context ) {
        //check if the device has camera
        // return context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
        return true; /*fuck you*/
    }
}
