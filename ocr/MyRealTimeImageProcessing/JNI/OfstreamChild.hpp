/*! \file OfstreamChild.hpp */

#ifndef OFSTREAM_CHILD_HPP
#define OFSTREAM_CHILD_HPP

#ifdef __cplusplus

#ifndef SKIP_INCLUDES
// includes here
#include <jni.h>
#include <fstream>
#endif // SKIP_INCLUDES

class OfstreamChild {
public:
  OfstreamChild ( std::ofstream& ofs,
                  jclass& _clazz, jmethodID& _void_method,
                  jobject& _jobj
                  )
    : ofs_(ofs), clazz(_clazz), void_method(_void_method), jobj(_jobj)
  { }

  void Close();
  std::ofstream& Get_ofs(){ return ofs_; }

  ~OfstreamChild() {
    if ( ofs_.is_open() ) ofs_.close();
  }

private:
  template<typename T> friend std::ofstream& operator<<(OfstreamChild&, T);
  std::ofstream& ofs_;
  jclass& clazz;
  jmethodID& void_method;
  jobject& jobj;
};

#endif // __cplusplus
#endif /*OFSTREAM_CHILD_HPP*/
