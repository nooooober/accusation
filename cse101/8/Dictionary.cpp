#include "Dictionary.h"

#include <iostream>
#include <string>
#define BLACK -1
#define RED 1

Dictionary::Node::Node(keyType kt, valType v) {
  key = kt;
  color = RED;
  val = v;
  right = nullptr;
  left = nullptr;
  parent = nullptr;
}

void Dictionary::inOrderString(std::string& str, Node* node) const {
  if (node->key == "NIL") {
    return;
  }
  inOrderString(str, node->left);
  str += node->key + " : " + std::to_string(node->val) + "\n";
  inOrderString(str, node->right);
}

void Dictionary::preOrderString(std::string& str, Node* node) const {
  if (node->key == "NIL") {
    return;
  }
  str += node->key + "\n";
  preOrderString(str, node->left);
  preOrderString(str, node->right);
}

void Dictionary::preOrderCopy(Node* node, Node* curNode) {
  if (node->key == "NIL") {
    return;
  }
  setValue(node->key, node->val);
  preOrderCopy(node->left, curNode);
  preOrderCopy(node->right, curNode);
}

void Dictionary::postOrderDelete(Node* node) {
  if (node->key == "NIL") {
    return;
  }
  postOrderDelete(node->left);
  postOrderDelete(node->right);
  delete node;
  num_pairs--;
}

Dictionary::Node* Dictionary::search(Node* node, keyType kt) const {
  if (node == nullptr || node->key == "NIL" || node == nil) {
    return nil;
  }
  if (node->key == kt) {
    return node;
  }
  if (node->key > kt) {
    return search(node->left, kt);
  } else {
    return search(node->right, kt);
  }
}

Dictionary::Node* Dictionary::findMin(Node* node) {
  if (node->key == "NIL" || node == nil) {
    return nil;
  }
  Node* curNode = node;
  while (curNode->left != nil) {
    curNode = curNode->left;
  }
  return curNode;
}

Dictionary::Node* Dictionary::findMax(Node* node) {
  if (node->key == "NIL" || node == nil) {
    return nil;
  }
  Node* curNode = node;
  while (curNode->right != nil) {
    curNode = curNode->right;
  }
  return curNode;
}

Dictionary::Node* Dictionary::findNext(Node* curNode) {
  if (curNode == nullptr || curNode == nil || curNode->key == "NIL") {
    return nil;
  }
  if (curNode->right != nil) {
    return findMin(curNode->right);
  }
  Node* M = curNode->parent;
  while (M != nil && curNode == M->right) {
    curNode = M;
    M = M->parent;
  }
  return M == nil ? nil : M;
}

Dictionary::Node* Dictionary::findPrev(Node* curNode) {
  if (curNode == nullptr || curNode == nil || curNode->key == "NIL") {
    return nil;
  }
  if (curNode->left != nil) {
    return findMax(curNode->left);
  }
  Node* M = curNode->parent;
  while (M != nil && M->left == curNode) {
    curNode = M;
    M = M->parent;
  }
  return M == nil ? nil : M;
}

void Dictionary::LeftRotate(Node* curNode) {
  Node* M = curNode->right;
  curNode->right = M->left;
  if (M->left != nil) {
    M->left->parent = curNode;
  }
  M->parent = curNode->parent;
  if (curNode->parent == nil) {
    root = M;
  } else if (curNode == curNode->parent->left) {
    curNode->parent->left = M;
  } else {
    curNode->parent->right = M;
  }
  M->left = curNode;
  curNode->parent = M;
}
void Dictionary::RightRotate(Node* curNode) {
  Node* M = curNode->left;
  curNode->left = M->right;
  if (M->right != nil) {
    M->right->parent = curNode;
  }
  M->parent = curNode->parent;
  if (curNode->parent == nil) {
    root = M;
  } else if (curNode == curNode->parent->right) {
    curNode->parent->right = M;
  } else {
    curNode->parent->left = M;
  }
  M->right = curNode;
  curNode->parent = M;
}
void Dictionary::RB_InsertFixUp(Node* curNode) {
  while (curNode->parent->color == RED) {
    if (curNode->parent == curNode->parent->parent->left) {
      Node* M = curNode->parent->parent->right;
      if (M->color == RED) {
        curNode->parent->color = BLACK;
        M->color = BLACK;
        curNode->parent->parent->color = RED;
        curNode = curNode->parent->parent;
      } else {
        if (curNode == curNode->parent->right) {
          curNode = curNode->parent;
          LeftRotate(curNode);
        }
        curNode->parent->color = BLACK;
        curNode->parent->parent->color = RED;
        RightRotate(curNode->parent->parent);
      }
    } else {
      Node* M = curNode->parent->parent->left;
      if (M->color == RED) {
        curNode->parent->color = BLACK;
        M->color = BLACK;
        curNode->parent->parent->color = RED;
        curNode = curNode->parent->parent;
      } else {
        if (curNode == curNode->parent->left) {
          curNode = curNode->parent;
          RightRotate(curNode);
        }
        curNode->parent->color = BLACK;
        curNode->parent->parent->color = RED;
        LeftRotate(curNode->parent->parent);
      }
    }
  }
  root->color = BLACK;
}
void Dictionary::RB_Transplant(Node* u, Node* v) {
  if (u->parent == nil) {
    root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}
void Dictionary::RB_DeleteFixUp(Node* curNode) {
  while (curNode != root && curNode->color == BLACK) {
    if (curNode == curNode->parent->left) {
      Node* M = curNode->parent->right;
      if (M->color == RED) {
        M->color = BLACK;
        curNode->parent->color = RED;
        LeftRotate(curNode->parent);
        M = curNode->parent->right;
      }
      if (M->left->color == BLACK && curNode->right->color == BLACK) {
        M->color = RED;
        curNode = curNode->parent;
      } else {
        if (M->right->color == BLACK) {
          M->left->color = BLACK;
          M->color = RED;
          RightRotate(M);
          M = curNode->parent->right;
        }
        M->color = curNode->parent->color;
        curNode->parent->color = BLACK;
        M->right->color = BLACK;
        LeftRotate(curNode->parent);
        curNode = root;
      }
    } else {
      Node* M = curNode->parent->left;
      if (M->color == RED) {
        M->color = BLACK;
        curNode->parent->color = RED;
        RightRotate(curNode->parent);
        M = curNode->parent->left;
      }
      if (M->right->color == BLACK && curNode->left->color == BLACK) {
        M->color = RED;
        curNode = curNode->parent;
      } else {
        if (M->left->color == BLACK) {
          M->right->color = BLACK;
          M->color = RED;
          LeftRotate(M);
          M = curNode->parent->left;
        }
        M->color = curNode->parent->color;
        curNode->parent->color = BLACK;
        M->left->color = BLACK;
        RightRotate(curNode->parent);
        curNode = root;
      }
    }
  }
  curNode->color = BLACK;
}
void Dictionary::RB_Delete(Node* curNode) {
  Node* Y = curNode;
  Node* X = nil;
  int Y_ORIGIN_COLOR = Y->color;
  if (Y->left == nil) {
    X = curNode->right;
    RB_Transplant(curNode, curNode->right);
  } else if (curNode->right == nil) {
    X = curNode->left;
    RB_Transplant(curNode, curNode->left);
  } else {
    Y = findMin(curNode->right);
    Y_ORIGIN_COLOR = Y->color;
    X = Y->right;
    if (Y->parent == curNode) {
      X->parent = Y;
    } else {
      RB_Transplant(Y, Y->right);
      Y->right = curNode->right;
      Y->right->parent = Y;
    }
    RB_Transplant(curNode, Y);
    Y->left = curNode->left;
    Y->left->parent = Y;
    Y->color = curNode->color;
  }
  if (Y_ORIGIN_COLOR == BLACK) {
    RB_DeleteFixUp(X);
  }
}

Dictionary::Dictionary() {
  nil = new Node("NIL", -1);
  nil->parent = nil;
  nil->left = nil;
  nil->right = nil;
  nil->color = BLACK;
  root = nil;
  current = nil;
  num_pairs = 0;
}

Dictionary::Dictionary(const Dictionary& D) {
  nil = new Node("NIL", -1);
  nil->parent = nil;
  nil->left = nil;
  nil->right = nil;
  nil->color = BLACK;
  root = nil;
  current = nil;
  num_pairs = 0;
  preOrderCopy(D.root, D.nil);
}

Dictionary::~Dictionary() {
  clear();
  delete nil;
  nil = nullptr;
  current = nullptr;
  root = nullptr;
}

int Dictionary::size() const { return num_pairs; }
bool containsKey(Dictionary::Node* A, Dictionary::Node* nil, keyType kt) {
  if (A == nil) {
    return false;
  }
  return A->key == kt || containsKey(A->left, nil, kt) ||
         containsKey(A->right, nil, kt);
}

bool Dictionary::contains(keyType kt) const {
  if (num_pairs <= 0) {
    return false;
  }
  return containsKey(root, nil, kt);
}

valType& Dictionary::getValue(keyType kt) const {
  if (!contains(kt)) {
    throw std::logic_error("err");
  }
  Node* curNode = search(this->root, kt);
  return curNode->val;
}

bool Dictionary::hasCurrent() const { return current == nil ? false : true; }

keyType Dictionary::currentKey() const {
  if (!hasCurrent()) {
    throw std::logic_error("err");
  }
  return current->key;
}

valType& Dictionary::currentVal() const {
  if (!hasCurrent()) {
    throw std::logic_error("err");
  }
  return current->val;
}

void Dictionary::clear() {
  postOrderDelete(root);
  root = nil;
  current = nil;
}
void Dictionary::setValue(keyType kt, valType v) {
  Node* tmpNode = search(root, kt);
  if (tmpNode != nil) {
    tmpNode->val = v;
    return;
  } else {
    tmpNode = new Node(kt, v);
    if (kt != "NIL") {
      tmpNode->left = nil;
      tmpNode->right = nil;
      tmpNode->parent = nil;
    }
    num_pairs++;
  }
  Node* M = nil;
  Node* curNode = this->root;
  while (curNode != nil) {
    M = curNode;
    if (tmpNode->key < curNode->key) {
      curNode = curNode->left;
    } else {
      curNode = curNode->right;
    }
  }
  tmpNode->parent = M;
  if (M == nil) {
    this->root = tmpNode;
  } else if (tmpNode->key < M->key) {
    M->left = tmpNode;
  } else {
    M->right = tmpNode;
  }
  tmpNode->left = nil;
  tmpNode->right = nil;
  tmpNode->color = RED;
  RB_InsertFixUp(tmpNode);
}

void transplant(Dictionary* D, Dictionary::Node* U, Dictionary::Node* V) {
  if (U->parent == D->nil) {
    D->root = V;
  } else if (U == U->parent->left) {
    U->parent->left = V;
  } else {
    U->parent->right = V;
  }
  if (V != D->nil) {
    V->parent = U->parent;
  }
}
void Dictionary::remove(keyType kt) {
  Node* tmpNode = search(root, kt);
  if (tmpNode == nil || tmpNode == nullptr) {
    throw std::logic_error("err");
  }
  RB_Delete(tmpNode);
  if (tmpNode == current) {
    current = nil;
  }
  if (tmpNode->parent != nil && tmpNode->parent->right == tmpNode) {
    tmpNode->parent->right = nil;
  } else if (tmpNode->parent != nil && tmpNode->parent->left == tmpNode) {
    tmpNode->parent->left = nil;
  }
  delete tmpNode;
  num_pairs--;
}

void Dictionary::begin() {
  if (num_pairs <= 0) {
    return;
  }
  current = root;
  while (current != nil && current->left != nil) {
    current = current->left;
  }
}

void Dictionary::end() {
  if (num_pairs <= 0) {
    return;
  }
  current = root;
  while (current != nil && current->right != nil) {
    current = current->right;
  }
}
void Dictionary::next() {
  if (!hasCurrent()) {
    throw std::logic_error("err");
  }
  current = findNext(current);
}

void Dictionary::prev() {
  if (!hasCurrent()) {
    throw std::logic_error("err");
  }
  current = findPrev(current);
}

std::string Dictionary::to_string() const {
  std::string str = "";
  Dictionary D = *this;
  inOrderString(str, D.root);
  return str;
}

std::string Dictionary::pre_string() const {
  Dictionary D = *this;
  std::string str = "";
  preOrderString(str, root);
  return str;
}

bool Dictionary::equals(const Dictionary& D) const {
  return to_string() == D.to_string();
}

std::ostream& operator<<(std::ostream& stream, Dictionary& D) {
  return stream << D.to_string();
}

bool operator==(const Dictionary& A, const Dictionary& B) {
  return A.equals(B);
}

Dictionary& Dictionary::operator=(const Dictionary& D) {
  if (this != &D) {
    Dictionary temp = D;
    std::swap(nil, temp.nil);
    std::swap(root, temp.root);
    std::swap(current, temp.current);
    std::swap(num_pairs, temp.num_pairs);
  }
  return *this;
}