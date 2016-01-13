
#include <iostream>
#include <sys/stat.h>

using namespace std;

bool file_exists ( const string& name ) {

  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}
