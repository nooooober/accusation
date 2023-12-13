#include "BigInteger.h"

#include <cstring>
#include <functional>
#include <sstream>
#include <string>

using namespace std;

const long ex = 1e9;
const int exp = 9;

void serde(BigInteger& N) {
  while (N.digits.length() > 0 && N.digits.front() == 0) {
    N.digits.moveFront();
    N.digits.eraseAfter();
  }
  if (N.digits.length() == 0) {
    N.signum = 0;
    return;
  }

  N.digits.moveBack();
  while (N.digits.position() > 0) {
    long carry = 0;
    long e = N.digits.movePrev();
    if (e < 0) {
      if (N.digits.position() == 0) {
        N.digits.setAfter(-e);
        N.signum *= -1;
        break;
      }
      for (carry = -1; e + -carry * ex < 0; carry--)
        ;
      N.digits.setAfter(e + -carry * ex);
    } else if (e >= ex) {
      carry = 1;
      for (carry = 1; e + -carry * ex >= ex; carry++)
        ;
      N.digits.setAfter(e + -carry * ex);
    }
    if (N.digits.position() > 0) {
      long prev_elem = N.digits.movePrev();
      N.digits.setAfter(prev_elem + carry);
      N.digits.moveNext();
    } else if (carry <= -1) {
      N.digits.setAfter(-N.digits.front());
      N.signum = -1;
      if (N.digits.front() < 0) {
        N.digits.setAfter(ex + N.digits.front());
        N.digits.moveNext();
        while (N.digits.position() < N.digits.length()) {
          long n = N.digits.moveNext();
          N.digits.setBefore(ex - n);
        }
        break;
      }

    } else if (carry >= 1) {
      N.digits.insertAfter(carry);
    }
  }
  N.digits.moveFront();
}

BigInteger::BigInteger() {
  signum = 0;
  digits = List();
  serde(*this);
}

BigInteger::BigInteger(std::string s) {
  if (s.length() < 0) {
    exit(1);
  }
  if (s.substr(0, 1) == "-" || s.substr(0, 1) == "+") {
    if (s.substr(0, 1) == "-") {
      this->signum = -1;
    } else {
      this->signum = 1;
    }
    s = s.substr(1);
  } else {
    this->signum = 1;
  }
  int length = s.length();
  for (int i = 1; length - i * exp > -exp; i++) {
    int pos = length - i * exp;
    int cnt = exp;
    std::string num_str;
    if (pos > 0) {
      num_str = s.substr(pos, cnt);
    } else {
      num_str = s.substr(0, pos + exp);
    }
    long x = std::stol(num_str);
    this->digits.insertAfter(x);
  }
  serde(*this);
}

BigInteger::BigInteger(const BigInteger& N) {
  this->signum = N.signum;
  this->digits = List(N.digits);
  serde(*this);
}

int BigInteger::sign() const { return signum; }

int BigInteger::compare(const BigInteger& N) const {
  BigInteger ret = this->sub(N);
  return ret.sign();
}

void BigInteger::makeZero() {
  this->signum = 0;
  this->digits.clear();
}

void BigInteger::negate() { this->signum *= -1; }

BigInteger BigInteger::add(const BigInteger& N) const {
  BigInteger a(*this);
  BigInteger b(N);
  if (a.sign() != b.sign()) {
    if (b.sign() > 0) {
      return b.add(a);
    }

    b.signum *= -1;
    return a.sub(b);
  }
  BigInteger A;
  A.signum = signum;
  List da = a.digits;
  List db = b.digits;
  List dc;
  da.moveBack();
  db.moveBack();
  while (da.position() > 0 && db.position() > 0) {
    long m = da.movePrev();
    long n = db.movePrev();
    dc.insertAfter(m + n);
  }

  while (da.position() > 0) {
    long m = da.movePrev();
    dc.insertAfter(m);
  }
  while (db.position() > 0) {
    long n = db.movePrev();
    dc.insertAfter(n);
  }
  A.digits = dc;
  serde(A);
  return A;
}

BigInteger BigInteger::sub(const BigInteger& N) const {
  BigInteger a(*this);
  BigInteger b(N);
  if (a.sign() != b.sign()) {
    b.negate();
    return a.add(b);
  }

  if (b.digits.length() > a.digits.length()) {
    BigInteger temp = b - a;
    temp.signum *= -1;
    return temp;
  }

  if (b.digits.length() == a.digits.length()) {
    if (b.digits.length() > 0) {
      if (b.digits.front() > a.digits.front()) {
        BigInteger temp = b - a;
        temp.signum *= -1;
        return temp;
      }
    }
  }

  BigInteger A;
  A.signum = a.signum;
  List da = a.digits;
  List db = b.digits;
  List dc;
  da.moveBack();
  db.moveBack();
  while (da.position() > 0 && db.position() > 0) {
    long m = da.movePrev();
    long n = db.movePrev();
    dc.insertAfter(m - n);
  }

  while (da.position() > 0) {
    long m = da.movePrev();
    dc.insertAfter(m);
  }
  while (db.position() > 0) {
    long n = db.movePrev();
    dc.insertAfter(-n);
  }
  A.digits = dc;
  serde(A);
  return A;
}


BigInteger BigInteger::mult(const BigInteger& N) const {
  if (this->signum == 0 || N.signum == 0) {
    BigInteger ret;
    serde(ret);
    return ret;
  }
  std::function<std::string(BigInteger)> stringFunc =
      [](BigInteger x) -> std::string {
    if (x.signum == 0) {
      return "0";
    }
    std::string s = "";
    if (x.signum == -1) {
      s += '-';
    }
    x.digits.moveFront();
    while (x.digits.position() < x.digits.length()) {
      long e = x.digits.moveNext();
      std::string es = std::to_string(e);
      while (x.digits.position() != 1 && es.length() < exp) {
        es = '0' + es;
      }
      s += es;
    }
    return s;
  };

  function<int(char)> ctoi = [](char c) -> int { return c - '0'; };
  function<string(string&, string&)> mul = [&](string& a, string& b) -> string {
    if (a == "0" || b == "0") return "0";
    string ret = "";
    bool ne = false;
    if ((a[0] == '-' && b[0] != '-') || (a[0] != '-' && b[0] == '-')) {
      ne = true;
    }
    if (a[0] == '-') a.erase(a.begin());
    if (b[0] == '-') b.erase(b.begin());
    int la = a.length();
    int lb = b.length();
    int* t = (int*)calloc(1, sizeof(int) * (la + lb - 1));
    for (int i = 0; i < la; i++) {
      for (int j = 0; j < lb; j++) {
        t[i + j] += (ctoi(a[i]) * ctoi(b[j]));
      }
    }
    int carry = 0;
    for (int i = la + lb - 2; i >= 0; i--) {
      int now = t[i] + carry;
      ret = std::to_string(now % 10) + ret;
      carry = now / 10;
    }
    while (carry > 0) {
      ret = std::to_string(carry % 10) + ret;
      carry = carry / 10;
    }
    if (ne) ret = "-" + ret;
    free(t);
    return ret;
  };

  std::string a = stringFunc(*this);
  std::string b = stringFunc(N);
  BigInteger ret(mul(a, b));
  serde(ret);

  return ret;
}

std::string BigInteger::to_string() {
  if (this->signum == 0) {
    return "0";
  }
  std::string s = "";
  if (this->signum == -1) {
    s += '-';
  }
  digits.moveFront();
  while (digits.position() < digits.length()) {
    long e = digits.moveNext();
    std::string es = std::to_string(e);
    while (digits.position() != 1 && es.length() < exp) {
      es = '0' + es;
    }
    s += es;
  }
  return s;
}

std::ostream& operator<<(std::ostream& stream, BigInteger N) {
  return stream << N.to_string();
}

bool operator==(const BigInteger& A, const BigInteger& B) {
  BigInteger ret = A.sub(B);
  return A.sub(B).sign() == 0 ? true : false;
}

bool operator<(const BigInteger& A, const BigInteger& B) {
  BigInteger ret = A.sub(B);
  return ret.sign() < 0 ? true : false;
}

bool operator<=(const BigInteger& A, const BigInteger& B) {
  BigInteger ret = A.sub(B);
  return ret.sign() <= 0 ? true : false;
}

bool operator>(const BigInteger& A, const BigInteger& B) {
  BigInteger ret = A.sub(B);
  return ret.sign() > 0 ? true : false;
}

bool operator>=(const BigInteger& A, const BigInteger& B) {
  BigInteger ret = A.sub(B);
  return ret.sign() >= 0 ? true : false;
}

BigInteger operator+(const BigInteger& A, const BigInteger& B) {
  return A.add(B);
}

BigInteger operator+=(BigInteger& A, const BigInteger& B) {
  A = A.add(B);
  return A;
}

BigInteger operator-(const BigInteger& A, const BigInteger& B) {
  return A.sub(B);
}

BigInteger operator-=(BigInteger& A, const BigInteger& B) {
  A = A.sub(B);
  return A;
}

BigInteger operator*(const BigInteger& A, const BigInteger& B) {
  return A.mult(B);
}

BigInteger operator*=(BigInteger& A, const BigInteger& B) {
  A = A.mult(B);
  return A.mult(B);
}