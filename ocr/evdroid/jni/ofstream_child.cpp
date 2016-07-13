#include <jni.h>
#include <fstream>
#include <sstream>
#include "ofstream_child.hpp"

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
