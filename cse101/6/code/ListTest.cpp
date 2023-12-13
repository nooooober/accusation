#include <iostream>
#include "List.h"
using namespace std;

int main() {
  List l;
  l.insertAfter(1);
  l.insertAfter(2);
  l.insertAfter(4);
  l.insertBefore(1);
  l.insertBefore(2);
  l.insertBefore(4);
  cout << l.length() << endl;
  cout << l << endl;
  l.moveFront();
  l.eraseAfter();
  List ll = l;
  ll.moveBack();
  ll.movePrev();
  ll.eraseBefore();
  cout << ll << endl;
  cout << l <<endl;
  cout << ll.equals(l) << endl;

}