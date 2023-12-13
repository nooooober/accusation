#include <iostream>
#include <string>

#include "Dictionary.h"

using namespace std;

int main() {
  Dictionary A, B;

  A.setValue("a", 111);
  A.setValue("b", 65);
  A.setValue("e", 4554564);
  A.setValue("h", 43434);
  A.setValue("f", 23);
  A.setValue("i", 4343);

  cout << A.pre_string();
  cout << B.pre_string();

  if (A.pre_string() == B.pre_string()) {
    cout << "greater" << endl;
    ;
  }

  return 0;
}
