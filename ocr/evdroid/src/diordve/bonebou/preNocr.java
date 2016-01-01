
// javac preNocr.java
// javah preNocr --> creates preNocr.h //this is done different from outside

// javac preNocr.java && java preNocr

// before that:
// g++  -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" -shared -o preNocr.so preNocr.cc -llept -ltesserac

package diordve.bonebou;

import java.lang.*;
import java.io.File;

import android.os.Environment;

public class preNocr {

    static {

        // andrew
        System.loadLibrary("preNocr");

        // TEST
        // File f = new File("/Users/albert/test/ocr/evdroid/jni/preNocr.so");
        // System.load(f.getAbsolutePath());
    }

    private native void doit ( String tessdata_path );

    public static void main ( String[] args ) {

        // andrew
        String tessdata_path = Environment.getExternalStorageDirectory().getAbsolutePath();
        // mac
        // String tessdata_path = "/usr/local/Cellar/tesseract/3.04.00/share/";
        // String tessdata_path = "./";
        new preNocr().doit(tessdata_path);
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
