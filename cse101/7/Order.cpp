#include <fstream>
#include <iostream>
#include <string>

#include "Dictionary.h"

using namespace std;

int main(int argc, char* argv[]) {
  ifstream in(argv[1]);
  ofstream out(argv[2]);
  if (in.fail() || out.fail()) {
    cerr << "failed to read file " << endl;
    exit(1);
  }
  string x;
  Dictionary dict;
  for (int i = 1; getline(in, x); i++) {
    dict.setValue(x, i);
  }
  out << dict.to_string() << endl;
  out << dict.pre_string() << endl;

  in.close();
  out.close();
  return 1;
}
