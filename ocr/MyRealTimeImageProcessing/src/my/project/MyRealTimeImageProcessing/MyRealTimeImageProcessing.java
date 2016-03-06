package my.project.MyRealTimeImageProcessing;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;

import android.app.Activity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Toast;

import android.graphics.Bitmap;
import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
import android.hardware.Camera.PictureCallback;
import android.content.pm.PackageManager;
import android.widget.Button;

public class MyRealTimeImageProcessing extends Activity {

    Camera mCamera;
    CameraPreview camPreview;
    PictureCallback mPicture;
    Button capture;
    Context myContext;
    ImageView MyCameraPreview = null;
    FrameLayout mainLayout;
    int PreviewSizeWidth = 640;
    int PreviewSizeHeight= 480;

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

        mainLayout = (FrameLayout) findViewById(R.id.frameLayout1);
        mainLayout.addView(camView, new LayoutParams(PreviewSizeWidth, PreviewSizeHeight));
        mainLayout.addView(MyCameraPreview, new LayoutParams(PreviewSizeWidth, PreviewSizeHeight));
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

    @Override
    public void onResume ( ) {

        super.onResume();
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
            mPicture = getPictureCallback();
            camPreview.refreshCamera(mCamera);
        }
    }

    //set form onCreate
    OnClickListener captrureListener = new OnClickListener() {
            @Override
            public void onClick ( View v ) {

                Camera.Parameters params = mCamera.getParameters();
                Camera.Size size = getBestPreviewSize ( 2048,1536, params );
                params.setPictureSize(size.width, size.height);
                params.setJpegQuality(100);
                mCamera.setParameters(params);
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

          if (newArea>resultArea) {
            result=size;
          }
        }
      }
    }

    return(result);
  }

        public native boolean saveMiddleClass ( int width, int height, byte[] NV21FrameData, int[] pixels );
    //callback - trace - from captureListener > onClick > cam.takePicture
    PictureCallback getPictureCallback ( ) {
        PictureCallback picture = new PictureCallback ( ) {

                @Override
                public void onPictureTaken(byte[] data, Camera camera) {

                    Bitmap bitmap = Bitmap.createBitmap(2048,1536, Bitmap.Config.ARGB_8888);
                    int[] pixels = new int[2048*1536];

                    // error prone - don't know if this comes yuv
                    saveMiddleClass ( 2048, 1536, data, pixels );
                    bitmap.setPixels(pixels, 0, 2048, 0, 0, 2048, 1536);

                    //make a new picture file
                    File pictureFile = getOutputMediaFile();

                    if ( pictureFile == null ) {
                        return;
                    }
                    try {
                        //write the file
                        FileOutputStream fos = new FileOutputStream(pictureFile);
                        // fos.write(data);
                        bitmap.compress(Bitmap.CompressFormat.JPEG, 100, fos);
                        fos.close();
                        Toast toast =
                            Toast.makeText(
                                           myContext,
                                           "Picture saved: " + pictureFile.getName(),
                                           Toast.LENGTH_LONG);
                        toast.show();

                    } catch (FileNotFoundException e) {
                    } catch (IOException e) {
                    }

                    //refresh camera to continue preview
                    camPreview.refreshCamera ( mCamera );
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
