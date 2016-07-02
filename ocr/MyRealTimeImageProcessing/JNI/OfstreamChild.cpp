
#include <jni.h>
#include <fstream>
#include "OfstreamChild.hpp"

template<typename T>
std::ostream& operator<<( OfstreamChild& ofsc, T op ) {

  ofsc.Get_ofs() << (std::string)op;
  ofsc.Get_ofs() << op;
  return ofsc.Get_ofs();
}

void OfstreamChild::Close(){
  if ( ofs_.is_open() ) ofs_.close();
}


