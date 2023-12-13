#include <iostream>
#include <string>
#include <vector>
#include "Dictionary.h"

using namespace std;

int main(int argc, char* argv[]) {
 
  vector<string> x = {"dsadsa", "das", "abc","abc","bca"};
  Dictionary dict;
  for (int i = 1; i <= x.size() ; i++) {
    dict.setValue(x[i - 1], i);
  }
  cout << dict.to_string() << endl;

  return 1;
}
