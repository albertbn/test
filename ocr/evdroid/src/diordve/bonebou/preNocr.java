
// from src
// javac diordve/bonebou/preNocr.java
// javah preNocr --> creates preNocr.h //this is done different from outside

// from src
// javac diordve/bonebou/preNocr.java && java diordve/bonebou/preNocr

// before that: see jni/diordve_bonebou_preNocr.cc

package diordve.bonebou;

import java.lang.*;
import java.io.File;

// REMARK for mac
import android.os.Environment;

public class preNocr {

    static {

        // credits for loading explicitly pngt, lept and tess... not clear how it worked on the galaxy (didn't work on nexus):
        // http://stackoverflow.com/questions/31833308/tess-two-cant-find-libpng-so
        // andrew
        System.loadLibrary("pngt");
        System.loadLibrary("lept");
        System.loadLibrary("tess");
        System.loadLibrary("preNocr");

        // TEST
        // File f = new File("~/test/ocr/evdroid/jni/preNocr.so");
        // System.load(f.getAbsolutePath());
    }

    private native void doit ( String tessdata_path, String img_path );

    public static String dodoit ( String img_png_path ) {

        // andrew - REMARK for mac
        String tessdata_path = Environment.getExternalStorageDirectory().getAbsolutePath();
        // mac - UNMARK for mac
        // String tessdata_path = "/usr/local/Cellar/tesseract/3.04.00/share/";
        // String tessdata_path = "./";
        new preNocr().doit ( tessdata_path, img_png_path );
        return "skywalker";
    }
    public static void main ( String[] args ) {

        // andrew - REMARK for mac
        // String tessdata_path = Environment.getExternalStorageDirectory().getAbsolutePath();
        // mac - UNMARK for mac
        // String tessdata_path = "/usr/local/Cellar/tesseract/3.04.00/share/";
        // String tessdata_path = "./";
        // new preNocr().doit(tessdata_path);
    }
}

// =================

// set #JAVA_HOME
// http://www.mkyong.com/java/how-to-set-java_home-environment-variable-on-mac-os-x/

// vim ~/.bash_profile
// export JAVA_HOME=$(/usr/libexec/java_home)
// source .bash_profile

// JNI tute
// https://www3.ntu.edu.sg/home/ehchua/programming/java/JavaNativeInterface.html

// http://stackoverflow.com/questions/1403788/java-lang-unsatisfiedlinkerror-no-dll-in-java-library-path
// Rick C. Hodgin - thanks man, load the focking file

//yep!
