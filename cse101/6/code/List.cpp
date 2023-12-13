#include "List.h"

#include <iostream>
#include <string>

using namespace std;

List::Node::Node(ListElement x) : data(x) {}
List::Node::Node(ListElement x, struct Node* next, struct Node* prev)
    : data(x), next(next), prev(prev) {}

void List::create_dummy() {
  frontDummy = new Node(0);
  backDummy = new Node(0);
  frontDummy->next = backDummy;
  backDummy->prev = frontDummy;
  beforeCursor = frontDummy;
  afterCursor = backDummy;
  num_elements = 0;
  pos_cursor = 0;
}

List::List() { create_dummy(); }

List::List(const List& L) {
  create_dummy();
  Node* cur = L.frontDummy->next;
  while (cur != L.backDummy) {
    this->insertBefore(cur->data);
    cur = cur->next;
  }
}

List::~List() {
  this->clear();
  delete this->frontDummy;
  delete this->backDummy;
}

int List::length() const { return this->num_elements; }

ListElement List::front() const {
  if (this->length() > 0) {
    return frontDummy->next->data;
  } else {
    abort();
  }
}

ListElement List::back() const {
  if (this->length() > 0) {
    return backDummy->prev->data;
  } else {
    abort();
  }
}

ListElement List::peekNext() const {
  if (this->position() < this->length()) {
    return afterCursor->data;
  } else {
    abort();
  }
}

ListElement List::peekPrev() const {
  if (this->position() > 0) {
    return beforeCursor->data;
  } else {
    abort();
  }
}

int List::position() const { return this->pos_cursor; }

void List::moveFront() {
  this->pos_cursor = 0;
  beforeCursor = frontDummy;
  afterCursor = frontDummy->next;
}

void List::moveBack() {
  this->pos_cursor = this->length();
  afterCursor = backDummy;
  beforeCursor = backDummy->prev;
}

int List::moveNext() {
  if (this->position() < this->length()) {
    beforeCursor = beforeCursor->next;
    afterCursor = afterCursor->next;
    this->pos_cursor++;
    return beforeCursor->data;
  } else {
    abort();
  }
}

int List::movePrev() {
  if (this->position() > 0) {
    beforeCursor = beforeCursor->prev;
    afterCursor = afterCursor->prev;
    this->pos_cursor--;
    return afterCursor->data;
  } else {
    abort();
  }
}

void List::insertAfter(ListElement x) {
  Node* cur = new Node(x, afterCursor, beforeCursor);
  beforeCursor->next = cur;
  afterCursor->prev = cur;
  afterCursor = cur;
  this->num_elements++;
}

void List::insertBefore(ListElement x) {
  Node* cur = new Node(x, afterCursor, beforeCursor);
  beforeCursor->next = cur;
  afterCursor->prev = cur;
  beforeCursor = cur;
  this->num_elements++;
  this->pos_cursor++;
}

void List::eraseAfter() {
  if (this->position() < this->length()) {
    Node* del = afterCursor;
    afterCursor = afterCursor->next;
    beforeCursor->next = afterCursor;
    afterCursor->prev = beforeCursor;
    this->num_elements--;
    delete del;
  } else {
    abort();
  }
}

void List::eraseBefore() {
  if (this->position() > 0) {
    Node* del = beforeCursor;
    beforeCursor = beforeCursor->prev;
    beforeCursor->next = afterCursor;
    afterCursor->prev = beforeCursor;
    this->pos_cursor--;
    this->num_elements--;
    delete del;
  } else {
    abort();
  }
}

int List::findNext(ListElement x) {
  while (position() < length()) {
    if (peekNext() == x) {
      moveNext();
      return position();
    }
    moveNext();
  }
  moveBack();
  return -1;
}

int List::findPrev(ListElement x) {
  while (position() > 0) {
    if (peekPrev() == x) {
      movePrev();
      return position();
    }
    movePrev();
  }
  moveFront();
  return -1;
}

void List::cleanup() {
  Node* cur = this->frontDummy->next;
  int outter_pos = 0;
  while (cur != this->backDummy) {
    int x = cur->data;
    Node* re = cur->next;
    int inner_pos = outter_pos + 1;
    while (re != this->backDummy) {
      int rex = re->data;
      Node* before = re->prev;
      Node* after = re->next;
      Node* del = re;
      re = after;
      if (x == rex) {
        before->next = after;
        after->prev = before;
        delete del;
        this->num_elements--;
        if (inner_pos <= pos_cursor) {
          pos_cursor--;
        }
      }
      inner_pos++;
    }
    outter_pos++;
    cur = cur->next;
  }
  int pos = this->pos_cursor;
  this->moveFront();
  while (this->position() != pos) {
    this->moveNext();
  }
}

void List::clear() {
  this->moveFront();
  while (this->length() > 0) {
    this->eraseAfter();
  }
}

List List::concat(const List& L) const {
  List ret;
  Node* cur;
  cur = this->frontDummy->next;
  while (cur != this->backDummy) {
    ret.insertBefore(cur->data);
    cur = cur->next;
  }
  cur = L.frontDummy->next;
  while (cur != L.backDummy) {
    ret.insertBefore(cur->data);
    cur = cur->next;
  }
  ret.beforeCursor = ret.frontDummy;
  ret.afterCursor = ret.beforeCursor->next;
  ret.pos_cursor = 0;
  return ret;
}

std::string List::to_string() const {
  string ret = "(";
  List::Node* cur = this->frontDummy->next;
  while (cur != this->backDummy) {
    ret += std::to_string(cur->data);
    if (cur->next != this->backDummy) {
      ret += ", ";
    }
    cur = cur->next;
  }
  ret += ")";
  return ret;
}

bool List::equals(const List& R) const {
  if (length() != R.length()) {
    return false;
  }
  Node* cur = this->frontDummy->next;
  Node* re = R.frontDummy->next;
  while (cur != this->backDummy && re != R.backDummy) {
    int x = cur->data;
    int y = re->data;
    if (x != y) {
      return false;
    }
    cur = cur->next;
    re = re->next;
  }
  return true;
}

std::ostream& operator<<(std::ostream& stream, const List& L) {
  stream << L.to_string();
  return stream;
}

bool operator==(const List& A, const List& B) { return A.equals(B); }

List& List::operator=(const List& L) {
  clear();
  delete this->frontDummy;
  delete this->backDummy;
  create_dummy();
  Node* cur = L.frontDummy->next;
  while (cur != L.backDummy) {
    this->insertBefore(cur->data);
    cur = cur->next;
  }
  moveFront();
  while (position() < length()) {
    if (position() == L.position()) break;
    moveNext();
  }
  return *this;
}

void List::setAfter(ListElement x) {
  if (this->position() < this->length()) {
    afterCursor->data = x;
  }
}

void List::setBefore(ListElement x) {
  if (this->position() > 0) {
    beforeCursor->data = x;
  }
}
