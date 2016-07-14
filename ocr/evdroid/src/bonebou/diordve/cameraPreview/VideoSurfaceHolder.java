package bonebou.diordve.cameraPreview;

import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.ImageView;
import android.widget.SeekBar;

import java.io.IOException;

/**
 * Created by albert on 7/14/16.
 */
public class VideoSurfaceHolder extends VideoFrameObjectDetect implements SurfaceHolder.Callback {

    int preview_size_width, preview_size_height;

    VideoSurfaceHolder self = this;
    //=========
    //CONSTRUCT
    //=========
    public VideoSurfaceHolder (Camera mCamera, ImageView cam_preview_img_view,
                               SeekBar[] bars /*comes low,high, low, high...*/,
                               int[] width_height ) {

        super(mCamera, cam_preview_img_view, bars);

        self.preview_size_width = width_height[0];
        self.preview_size_height = width_height[1];
    }

    @Override
    public void surfaceCreated ( SurfaceHolder surfaceHolder ) {

        try {

            // If SurfaceHolder not set, the preview area will be black.
            super.mCamera.setPreviewDisplay(surfaceHolder);
            super.mCamera.setPreviewCallback(this);

            Log.i(super.LOG_KEY, "VideoSurfaceHolder::surfaceCreated OK ");
        }
        catch ( IOException e ) {

            self.camera_release();

            Log.e(super.LOG_KEY, "VideoSurfaceHolder::surfaceCreated error: " + e.getMessage() );
        }
    }

    @Override
    public void surfaceChanged ( SurfaceHolder surfaceHolder, int i, int i1, int i2 ) {

        super.parameters = super.mCamera.getParameters ( ) ;
        super.parameters.setPreviewSize ( this.preview_size_width, this.preview_size_height );

        //this.imageFormat = this.parameters.getPreviewFormat();
        super.mCamera.setParameters (super.parameters);
        super.mCamera.startPreview();

        Log.i(super.LOG_KEY, "VideoSurfaceHolder::surfaceChanged... ");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

        self.camera_release();

        Log.i(super.LOG_KEY, "VideoSurfaceHolder::surfaceDestroyed ");
    }

    void camera_release ( ) {

        super.mCamera.stopPreview();
        super.mCamera.setPreviewCallback(null);
        super.mCamera.release();
        super.mCamera = null;
    }
}
