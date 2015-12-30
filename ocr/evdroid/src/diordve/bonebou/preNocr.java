package diordve.bonebou;

// javac HelloJNI.java
// javah HelloJNI --> creates HelloJNI.h

// javac HelloJNI.java && java HelloJNI

// before that:
// gcc  -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" -shared -o hello.so HelloJNI.c
// g++  -I"$JAVA_HOME/include" -I"$JAVA_HOME/include/darwin" -shared -o hello.so HelloJNI.cc -llept -ltesserac

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

import java.lang.*;
import java.io.File;

public class preNocr {

    static {

        // System.out.println(System.getProperty("java.library.path"));
        File f;
        f = new File("preNocr.so");
        // System.loadLibrary("hello");
        System.load(f.getAbsolutePath());
    }

    private native String sayHello ( String tessdata_path );

    public static void main ( String[] args ) {
        // andrew
        // String tessdata_path = Environment.getExternalStorageDirectory().getAbsolutePath();
        // mac
        // String tessdata_path = "/usr/local/Cellar/tesseract/3.04.00/share/";
        String tessdata_path = "./";
        String native_reply = new HelloJNI().sayHello(tessdata_path);
    }
}
