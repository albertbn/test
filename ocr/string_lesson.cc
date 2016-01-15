
// g++ string_lesson.cc -o string_lesson && ./string_lesson

#include <iostream>

static const char* path_sd_card;

using namespace std;

string get_path_absolute ( const string& relative ) {

  string path_sd_card_str ( path_sd_card );
  return path_sd_card_str + relative;
}

int main() {
  path_sd_card = "fock u";
  string s = get_path_absolute ( "/screw u" );

  cout << s << endl;
}
