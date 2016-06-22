//
//
//
package my.project.MyRealTimeImageProcessing;

import android.app.Activity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

/**
 * Created by Albert on 6/21/16.
 */
public class LayoutsNControls extends Activity {

    LayoutsNControls self = this;

    //HSV for white is (0,0,255)
    final int H_MIN=1, S_MIN=1, V_MIN=0;
    final int H_MAX=179, S_MAX=100, V_MAX=255;

    SeekBar bar_h_low, bar_h_high, bar_s_low, bar_s_high, bar_v_low, bar_v_high;
    TextView h_low_text, h_high_text, s_low_text, s_high_text, v_low_text, v_high_text;

    RelativeLayout lay_main_wrap_preview, lay_video_preview_wrap, lay_wrap_sliders, lay_text_result;
    Button btn_capture, btn_flash, btn_calibrate, btn_text_good, btn_text_again;
    boolean is_torch_on=false, is_calibrate=true; /*!DON'T use Boolean, forget the fucking class!*/
    TextView tv;

    @Override
    public void onCreate ( Bundle savedInstanceState ) {

        super.onCreate(savedInstanceState);

        self.init_controls();
        self.bind_controls();
    }

    void init_controls() {

        self.lay_main_wrap_preview = (RelativeLayout) self.findViewById(R.id.lay_main_wrap_preview);
        self.lay_video_preview_wrap = (RelativeLayout) self.findViewById(R.id.lay_video_preview_wrap);
        self.lay_wrap_sliders = (RelativeLayout) self.findViewById(R.id.lay_wrap_sliders);
        self.lay_text_result = (RelativeLayout) self.findViewById(R.id.lay_text_result);

        self.btn_capture = (Button) self.findViewById(R.id.btn_capture); /*events set in child*/
        self.btn_flash = (Button) self.findViewById(R.id.btn_flash); /*events set in child*/
        self.btn_calibrate = (Button) self.findViewById(R.id.btn_calibrate);
        self.btn_text_good = (Button) self.findViewById(R.id.btn_text_good);
        self.btn_text_again = (Button) self.findViewById(R.id.btn_text_again);

        self.tv = (TextView) self.findViewById(R.id.tv_dump); /*text view for OCR result*/

        //slider text displays
        self.h_low_text = (TextView) self.findViewById(R.id.h_low_text); self.h_high_text = (TextView) self.findViewById(R.id.h_high_text);
        self.s_low_text = (TextView) self.findViewById(R.id.s_low_text); self.s_high_text = (TextView) self.findViewById(R.id.s_high_text);
        self.v_low_text = (TextView) self.findViewById(R.id.v_low_text); self.v_high_text = (TextView) self.findViewById(R.id.v_high_text);

        self.bar_h_low = (SeekBar) self.findViewById( R.id.bar_h_low); self.bar_h_high = (SeekBar) self.findViewById( R.id.bar_h_high);
        self.bar_s_low = (SeekBar) self.findViewById( R.id.bar_s_low); self.bar_s_high = (SeekBar) self.findViewById( R.id.bar_s_high);
        self.bar_v_low = (SeekBar) self.findViewById( R.id.bar_v_low); self.bar_v_high = (SeekBar) self.findViewById( R.id.bar_v_high);
    }

    void bind_controls ( ) {

        self.btn_calibrate.setOnClickListener (

            new View.OnClickListener() {
                @Override
                public void onClick ( View v ) {
                    if(self.is_calibrate){
                        self.is_calibrate=false; self.lay_wrap_sliders.setVisibility(View.GONE);
                    }
                    else {
                        self.is_calibrate=true; self.lay_wrap_sliders.setVisibility(View.VISIBLE);
                    }
                }
            }
        );

//      TODO - make different cases for text good and text again...
        View.OnClickListener listen_text = new View.OnClickListener( ) {
            @Override
            public void onClick ( View v ) {
                self.lay_main_wrap_preview.setVisibility(View.VISIBLE);
                if(self.is_calibrate) self.lay_wrap_sliders.setVisibility(View.VISIBLE);
                self.lay_text_result.setVisibility(View.GONE);
            }
        };
        self.btn_text_good.setOnClickListener(listen_text);
        self.btn_text_again.setOnClickListener(listen_text);

        self.tv.setMovementMethod(new ScrollingMovementMethod()); /*scrolling for text view OCR results*/

        self.bind_sliders();
    }

    void bind_sliders() {

        self.bar_h_low.setOnSeekBarChangeListener(self.sb_listen); self.bar_h_high.setOnSeekBarChangeListener(self.sb_listen);
        self.bar_s_low.setOnSeekBarChangeListener(self.sb_listen); self.bar_s_high.setOnSeekBarChangeListener(self.sb_listen);
        self.bar_v_low.setOnSeekBarChangeListener(self.sb_listen); self.bar_v_high.setOnSeekBarChangeListener(self.sb_listen);
        //set initial HSV stuff (HSV white is 0,0,255)
        self.bar_h_low.setProgress(self.H_MIN); self.bar_h_high.setProgress(self.H_MAX);
        self.bar_s_low.setProgress(self.S_MIN); self.bar_s_high.setProgress(self.S_MAX);
        self.bar_v_low.setProgress(self.V_MIN); self.bar_v_high.setProgress(self.V_MAX);
    }

    //seek bar generic listeners
    SeekBar.OnSeekBarChangeListener sb_listen = new SeekBar.OnSeekBarChangeListener() {

        int progress = 0;

        @Override
        public void onProgressChanged(SeekBar seekBar, int progressValue, boolean fromUser) {

            progress = progressValue;
            String s_progress = Integer.toString(progress);

            switch ( seekBar.getId() ){
                case R.id.bar_h_low:
                    self.h_low_text.setText("h_low: "+s_progress); break;
                case R.id.bar_h_high:
                    self.h_high_text.setText("h_high: "+s_progress); break;
                case R.id.bar_s_low:
                    self.s_low_text.setText("s_low: "+s_progress); break;
                case R.id.bar_s_high:
                    self.s_high_text.setText("s_high: "+s_progress); break;
                case R.id.bar_v_low:
                    self.v_low_text.setText("v_low: "+s_progress); break;
                case R.id.bar_v_high:
                    self.v_high_text.setText("v_high: "+s_progress); break;
                default:
                    self.h_low_text.setText(Integer.toString(progress)+','+ Integer.toString(seekBar.getId())+','+ Integer.toString(seekBar.getId())); break;
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
}
