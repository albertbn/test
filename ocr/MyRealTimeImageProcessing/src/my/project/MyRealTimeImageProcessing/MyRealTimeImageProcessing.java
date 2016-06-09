package my.project.MyRealTimeImageProcessing;

import java.io.File;
//import java.io.FileNotFoundException;
//import java.io.FileOutputStream;
//import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

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
import android.widget.Toast;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
//import android.hardware.Camera.PictureCallback;
//import android.content.pm.PackageManager;
import android.widget.Button;

public class MyRealTimeImageProcessing extends Activity {

    int PreviewSizeWidth = 480;
    int PreviewSizeHeight = 640;
    int PHOTO_WIDTH = 1536; /*2448*/
    int PHOTO_HEIGHT =  2048; /*3264*/

    Camera mCamera;
    CameraPreview camPreview;
    PictureCallback mPicture;
    Button capture;
    Context myContext;
    ImageView MyCameraPreview = null;
    // FrameLayout mainLayout;
    RelativeLayout rl_video_preview_wrap1;

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
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                             WindowManager.LayoutParams.FLAG_FULLSCREEN);

        // from andrew cam example - other project
        myContext = this;

        //Set this APK no title
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.main);

        // Create my camera preview
        MyCameraPreview = new ImageView(this);

        SurfaceView camView = new SurfaceView(this);
        SurfaceHolder camHolder = camView.getHolder();
        camPreview = new CameraPreview(PreviewSizeWidth, PreviewSizeHeight, MyCameraPreview, mCamera, camHolder);

        camHolder.addCallback(camPreview);
        camHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        rl_video_preview_wrap1 = (RelativeLayout) findViewById(R.id.rl_video_preview_wrap1);
        rl_video_preview_wrap1.addView(camView, new LayoutParams(PreviewSizeWidth, PreviewSizeHeight));
        rl_video_preview_wrap1.addView(MyCameraPreview, new LayoutParams(PreviewSizeWidth, PreviewSizeHeight));
        capture = (Button) findViewById(R.id.button_capture);
        capture.setOnClickListener(captrureListener);
    }

    @Override
    protected void onPause ( ) {

        if ( camPreview != null)
            camPreview.onPause();
        super.onPause();

        // from andrew cam example - other project
        //when on Pause, release camera in order to be used from other applications
        releaseCamera();
    }

        public static void setCameraDisplayOrientation ( Activity activity,
                                                     int cameraId,
                                                     android.hardware.Camera camera
                                                         ) {
            android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
            android.hardware.Camera.getCameraInfo(cameraId, info);
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
            mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
        }

        if ( !hasCamera(myContext) ) {
            Toast toast =
                Toast.makeText(myContext,
                               "Sorry, your phone does not have a camera!",
                               Toast.LENGTH_LONG);
            toast.show();
            finish();
        }
        if ( mCamera == null ) {

            mCamera = Camera.open(0);

            setCameraDisplayOrientation ( this, CameraInfo.CAMERA_FACING_BACK, mCamera );

            //start focus
            Camera.Parameters params = mCamera.getParameters();
            Camera.Size size = getBestPreviewSize ( PHOTO_WIDTH, PHOTO_HEIGHT, params );

            params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            params.setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);
            // params.setFlashMode(Camera.Parameters.FLASH_MODE_ON);
            // params.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);

            params.setPictureSize(size.width, size.height);
            params.setJpegQuality(100);
            mCamera.setParameters(params);
            //end focus

            mPicture = getPictureCallback();
            camPreview.refreshCamera(mCamera);
        }
    }

    //set form onCreate
    OnClickListener captrureListener = new OnClickListener() {
            @Override
            public void onClick ( View v ) {

                // Camera.Parameters params = mCamera.getParameters();
                // Camera.Size size = getBestPreviewSize ( 3264,2448, params );

                // params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
                // params.setFlashMode(Camera.Parameters.FLASH_MODE_AUTO);
                // // params.setFlashMode(Camera.Parameters.FLASH_MODE_ON);

                // params.setPictureSize(size.width, size.height);
                // params.setJpegQuality(100);
                // mCamera.setParameters(params);
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

    return(result);
  }

    public native boolean saveMiddleClass ( String root_folder_path, String img_unique_no_ext, long inputImage );
    void tweak_bytes ( byte[] data ) {

        Mat mat=new Mat();
        Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
        Utils.bitmapToMat(bmp, mat);   //converting a mat to bitmap
        bmp=null;
        Imgproc.cvtColor(mat,mat,Imgproc.COLOR_RGB2BGR);
        String root_folder =  Environment.getExternalStorageDirectory().getAbsolutePath();
        saveMiddleClass ( root_folder, "smc", mat.getNativeObjAddr() ) ;

        //here - send the mat as in the fd app
        // File pictureFile = getOutputMediaFile();
        // Imgcodecs.imwrite ( pictureFile.toString(), mat );
    }

    //callback - trace - from captureListener > onClick > cam.takePicture
    PictureCallback getPictureCallback ( ) {
        PictureCallback picture = new PictureCallback ( ) {

                @Override
                public void onPictureTaken ( byte[] data, Camera camera ) {

                    tweak_bytes ( data ) ; return;
                }
            };
        return picture;
    }

    //create file path where the .jpg will be saved
    static File getOutputMediaFile() {

        //make a new file directory inside the "sdcard" folder
        File mediaStorageDir = new File("/sdcard/", "JCG Camera");

        //if this "JCGCamera folder does not exist
        if (!mediaStorageDir.exists()) {
            //if you cannot make this folder return
            if (!mediaStorageDir.mkdirs()) {
                return null;
            }
        }

        //take the current timeStamp
        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
        File mediaFile;
        //and make a media file:
        mediaFile = new File(mediaStorageDir.getPath() + File.separator + "IMG_" + timeStamp + ".jpg");

        return mediaFile;
    }

    void releaseCamera ( ) {
        // stop and release camera
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }
    }

    boolean hasCamera ( Context context ) {
        //check if the device has camera
        // return context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA);
        return true; /*fuck you*/
    }
}
