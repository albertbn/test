#include <jni.h>
#include <fstream>
#include <sstream>
#include "ofstream_child.hpp"

// #define SSTR( x ) static_cast< std::ostringstream & >( \
//         ( std::ostringstream() << std::dec << x ) ).str()

// template<typename T>
// std::ofstream& operator<<(ofstream_child& ofsc, T op) {

//   // ofsc.Get_ofs() << (std::string)op;
//   if ( ofsc.is_jni_inited() ) {

//     // jstring jstr = ofsc.get_env()->NewStringUTF ( ((std::string)op).c_str() );
//     jstring jstr = ofsc.get_env()->NewStringUTF ( SSTR(op).c_str() );

//     // send text message to java
//     ofsc.get_env()->CallVoidMethod ( *(ofsc.get_jobj()), *(ofsc.get_void_method()), jstr );
//   }

//   ofsc.get_ofs() << op;
//   return ofsc.get_ofs();
// }

void ofstream_child::open ( const char* filename,  std::ios_base::openmode mode ) {

  if ( !this->ofs.is_open() ) this->ofs.open(filename, mode);
}

void ofstream_child::close(){
  if ( this->ofs.is_open() ) this->ofs.close();
}

void ofstream_child::init_jni ( JNIEnv* env, jmethodID void_method, jobject jobj ) {

  this->is_jni_inited_bit = true;

  this->env = env;
  this->void_method = void_method;
  this->jobj = jobj;
}
