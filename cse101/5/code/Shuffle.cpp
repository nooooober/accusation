#include <iostream>
#include <string>

#include "List.h"

void shuffle(List& Lt);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: ./" << argv[0] << "n" << std::endl;
    exit(1);
  }
  std::cout << "deck size       shuffle count" << std::endl;
  std::cout << "------------------------------" << std::endl;
  int n = atoi(argv[1]);
  List L;
  for (int i = 1; i <= n; i++) {
    L.insertBefore(i - 1);
    List Lt(L);
    int cnt = 0;
    do {
      shuffle(Lt);
      cnt++;
    } while (!Lt.equals(L));
    std::cout << i << "               " << cnt << std::endl;
  }

  return 0;
}

void shuffle(List& Lt) {
  List t;
  int mid = Lt.length() >> 1;
  Lt.moveFront();
  for (int i = 0; i < mid; i++) {
    int x = Lt.moveNext();
    Lt.eraseBefore();
    t.insertBefore(x);
  }
  t.moveFront();
  while (Lt.position() < Lt.length()) {
    Lt.moveNext();
    if (t.position() < t.length()) {
      Lt.insertAfter(t.moveNext());
      Lt.moveNext();
    }
  }
}