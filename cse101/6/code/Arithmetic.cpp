#include <fstream>
#include <iostream>
#include <string>

#include "BigInteger.h"

using namespace std;

int main(int argc, char **argv) {
  if (argc != 3) {
    cerr << "usage: " << argv[0] << "inputFile  outputFile" << endl;
    exit(1);
  }
  ifstream in(argv[1]);
  ofstream out(argv[2]);
  if (in.fail()) {
    cerr << "failed to open file" << argv[1] << endl;
    exit(1);
  }
  if (out.fail()) {
    cerr << "failed to open file" << argv[2] << endl;
    exit(1);
  }

  string num;
  in >> num;
  BigInteger A(num);
  in >> num;
  BigInteger B(num);

  BigInteger three("3");
  BigInteger two("2");
  BigInteger nine("9");
  BigInteger st("16");

  BigInteger da(A * A);
  BigInteger db(B * B);
  out << A << endl << endl;
  out << B << endl << endl;
  out << (A + B) << endl << endl;
  out << (A - B) << endl << endl;
  out << (A - A) << endl << endl;
  out << (three * A - two * B) << endl << endl;
  out << (A * B) << endl << endl;
  out << da << endl << endl;
  out << db << endl << endl;
  out << (nine * da * da + st * db * db * B) << endl << endl;

  in.close();
  out.close();
}