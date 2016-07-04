/*! \file OfstreamChild.hpp */

#ifndef OFSTREAM_CHILD_HPP
#define OFSTREAM_CHILD_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <jni.h>
#include <fstream>
#endif // SKIP_INCLUDES

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

class ofstream_child {

  // template<typename T> friend std::ofstream& operator<<(ofstream_child& ofsc, const T& op){

  //   if ( ofsc.is_jni_inited() ) {

  //     // jstring jstr = ofsc.get_env()->NewStringUTF ( ((std::string)op).c_str() );
  //     jstring jstr = ofsc.get_env()->NewStringUTF ( SSTR(op).c_str() );

  //     // send text message to java
  //     ofsc.get_env()->CallVoidMethod ( *(ofsc.get_jobj()), *(ofsc.get_void_method()), jstr );
  //   }

  //   ofsc.get_ofs() << op;
  //   return ofsc.get_ofs();
  // }

  // std::ofstream& operator<<( std::ofstream& (*pf) (std::ofstream&) ){

  //   if ( this->is_jni_inited() ) {

  //     // jstring jstr = ofsc.get_env()->NewStringUTF ( ((std::string)op).c_str() );
  //     jstring jstr = this->get_env()->NewStringUTF ( SSTR(pf).c_str() );

  //     // send text message to java
  //     this->get_env()->CallVoidMethod ( *(this->get_jobj()), *(this->get_void_method()), jstr );
  //   }

  //   this->get_ofs() << pf;
  //   return this->get_ofs();
  // }

public:
  ofstream_child ( ) : is_jni_inited_bit(false) { }

  template <typename T>
  ofstream_child& operator<<(const T& a) {

    if ( this->is_jni_inited() ) {

      // jstring jstr = this->get_env()->NewStringUTF ( ((std::string)op).c_str() );
      jstring jstr = this->get_env()->NewStringUTF ( SSTR(a).c_str() );

      // send text message to java
      this->get_env()->CallVoidMethod ( *(this->get_jobj()), *(this->get_void_method()), jstr );
    }
    this->ofs<<a;
    return *this;
  }

  ofstream_child& operator<<(std::ostream& (*pf) (std::ostream&)){

    if ( this->is_jni_inited() ) {

      // jstring jstr = this->get_env()->NewStringUTF ( ((std::string)op).c_str() );
      jstring jstr = this->get_env()->NewStringUTF ( SSTR(pf).c_str() );

      // send text message to java
      this->get_env()->CallVoidMethod ( *(this->get_jobj()), *(this->get_void_method()), jstr );
    }
    this->ofs<<pf;
    return *this;
  }

  // the ofstream->open signature: http://www.cplusplus.com/reference/fstream/ofstream/open/
  void open ( const char* filename,  std::ios_base::openmode mode = std::ios_base::out );
  void close ( );

  void init_jni ( JNIEnv* env, jmethodID void_method, jobject jobj );

  // getters/setters
  std::ofstream& get_ofs(){ return this->ofs; }

  bool is_jni_inited() { return this->is_jni_inited_bit; }
  JNIEnv* get_env () { return this->env; };
  jobject* get_jobj () { return &(this->jobj); };
  jmethodID* get_void_method () { return &(this->void_method); }

  // destruct or surrender
  ~ofstream_child ( ) {
    if ( this->ofs.is_open() ) this->ofs.close();
  }


private:
  std::ofstream ofs;

  bool is_jni_inited_bit;
  JNIEnv* env;
  jmethodID void_method;
  jobject jobj;
};

#endif // __cplusplus
#endif /*OFSTREAM_CHILD_HPP*/
