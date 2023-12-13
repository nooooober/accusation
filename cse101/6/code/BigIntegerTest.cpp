#include "BigInteger.h"
#include <iostream>

using namespace std;

int main() {
  BigInteger a("9999");
  BigInteger b("9999");
  cout << a << "  " << b  << endl;
  cout << (a == b) << endl;
  cout << a * a << endl;
  cout << a * b << endl;
  cout <<(a <= b) << endl;
}