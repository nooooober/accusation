#include "Dictionary.h"

#include <iostream>
#include <string>

Dictionary::Node::Node(keyType k, valType v) {
  key = k;
  val = v;
  left = nullptr;
  right = nullptr;
  parent = nullptr;
}

void Dictionary::inOrderString(std::string& s, Node* r) const {
  if (r == nullptr) {
    return;
  }
  inOrderString(s, r->left);
  s += r->key + " : " + std::to_string(r->val) + "\n";
  inOrderString(s, r->right);
}

void Dictionary::preOrderString(std::string& s, Node* r) const {
  if (r == nullptr) {
    return;
  }
  s += r->key + "\n";
  preOrderString(s, r->left);
  preOrderString(s, r->right);
}

void Dictionary::preOrderCopy(Node* r, Node* node) {
  if (r == node || r == nil || r == nullptr) {
    return;
  }
  setValue(r->key, r->val);
  preOrderCopy(r->left, node);
  preOrderCopy(r->right, node);
}

void Dictionary::postOrderDelete(Node* r) {
  if (r == nullptr || r == nil) {
    return;
  }
  postOrderDelete(r->left);
  postOrderDelete(r->right);
  delete r;
  num_pairs--;
}

Dictionary::Node* Dictionary::search(Node* r, keyType k) const {
  if (r == nullptr) {
    return nil;
  }
  if (r->key == k) {
    return r;
  }
  if (r->key > k) {
    return search(r->left, k);
  } else {
    return search(r->right, k);
  }
}

Dictionary::Node* Dictionary::findMin(Node* r) {
  if (r == nullptr) {
    return nil;
  }
  Node* node = r;
  while (node->left != nullptr) {
    node = node->left;
  }
  return node;
}

Dictionary::Node* Dictionary::findMax(Node* r) {
  if (r == nullptr) {
    return nil;
  }
  Node* node = r;
  while (node->right != nullptr) {
    node = node->right;
  }
  return node;
}

Dictionary::Node* Dictionary::findNext(Node* node) {
  if (node == nullptr || node == nil) {
    return nil;
  }
  if (node->right != nullptr) {
    return findMin(node->right);
  }
  Node* cur = node->parent;
  while (cur != nullptr && node == cur->right) {
    node = cur;
    cur = cur->parent;
  }
  return cur == nullptr ? nil : cur;
}

Dictionary::Node* Dictionary::findPrev(Node* node) {
  if (node == nullptr || node == nil) {
    return nil;
  }
  if (node->left != nullptr) {
    return findMax(node->left);
  }
  Node* cur = node->parent;
  while (cur != nullptr && cur->left == node) {
    node = cur;
    cur = cur->parent;
  }
  return cur == nullptr ? nil : cur;
}

Dictionary::Dictionary() {
  nil = new Node("NIL", -1);
  root = nullptr;
  current = nil;
  num_pairs = 0;
}

Dictionary::Dictionary(const Dictionary& dict) {
  nil = new Node("NIL", -1);
  root = nullptr;
  num_pairs = 0;
  current = nil;
  preOrderCopy(dict.root, nullptr);
}

Dictionary::~Dictionary() {
  clear();
  delete nil;
  nil = nullptr;
  current = nullptr;
  root = nullptr;
}

int Dictionary::size() const { return num_pairs; }

bool containHelp(Dictionary::Node* root, keyType k) {
  if (root == nullptr) {
    return false;
  }
  return root->key == k || containHelp(root->left, k) ||
         containHelp(root->right, k);
}

bool Dictionary::contains(keyType k) const {
  if (num_pairs <= 0) {
    return false;
  }
  return containHelp(root, k);
}

valType& Dictionary::getValue(keyType k) const {
  if (!contains(k)) {
    throw std::logic_error("error");
  }
  Node* node = search(this->root, k);
  return node->val;
}

bool Dictionary::hasCurrent() const { return current == nil ? false : true; }

keyType Dictionary::currentKey() const {
  if (!hasCurrent()) {
    throw std::logic_error("error");
  }
  return current->key;
}

valType& Dictionary::currentVal() const {
  if (!hasCurrent()) {
    throw std::logic_error("error");
  }
  return current->val;
}

void Dictionary::clear() {
  postOrderDelete(root);
  current = nil;
}

void Dictionary::setValue(keyType k, valType v) {
  Node* needed = search(root, k);
  if (needed != nil) {
    needed->val = v;
    return;
  } else {
    needed = new Node(k, v);
    num_pairs++;
  }
  Node* cur = nullptr;
  Node* node = this->root;
  while (node != nullptr) {
    cur = node;
    if (needed->key < node->key) {
      node = node->left;
    } else {
      node = node->right;
    }
  }
  needed->parent = cur;
  if (cur == nullptr) {
    this->root = needed;
  } else if (needed->key < cur->key) {
    cur->left = needed;
  } else {
    cur->right = needed;
  }
}

void trans(Dictionary* dict, Dictionary::Node* from, Dictionary::Node* to) {
  if (from->parent == nullptr) {
    dict->root = to;
  } else if (from == from->parent->left) {
    from->parent->left = to;
  } else {
    from->parent->right = to;
  }
  if (to != nullptr) {
    to->parent = from->parent;
  }
}

void Dictionary::remove(keyType k) {
  Node* needed = search(root, k);
  if (needed == nil || needed == nullptr) {
    throw std::logic_error("error");
  }
  if (needed->left == nullptr) {
    trans(this, needed, needed->right);
  } else if (needed->right == nullptr) {
    trans(this, needed, needed->left);
  } else {
    Node* cur = findMin(needed->right);
    if (cur->parent != needed) {
      trans(this, cur, cur->right);
      cur->right = needed->right;
      cur->right->parent = cur;
    }
    trans(this, needed, cur);
    cur->left = needed->left;
    cur->left->parent = cur;
  }
  if (needed == current) {
    current = nil;
  }
  if (needed->parent != nullptr && needed->parent->right == needed) {
    needed->parent->right = nullptr;
  } else if (needed->parent != nullptr && needed->parent->left == needed) {
    needed->parent->left = nullptr;
  }
  delete needed;
  num_pairs--;
}

void Dictionary::begin() {
  if (num_pairs <= 0) {
    return;
  }
  current = root;
  while (current != nullptr && current->left != nullptr) {
    current = current->left;
  }
}

void Dictionary::end() {
  if (num_pairs <= 0) {
    return;
  }
  current = root;
  while (current != nullptr && current->right != nullptr) {
    current = current->right;
  }
}

bool isParent(Dictionary::Node* root, Dictionary::Node* _dict) {
  if (root == nullptr || _dict == nullptr) {
    return false;
  }
  Dictionary::Node* left = root->left;
  Dictionary::Node* right = root->right;
  if (left == _dict || right == _dict) {
    return true;
  }
  return isParent(left, _dict) || isParent(right, _dict);
}

void Dictionary::next() { current = findNext(current); }

void Dictionary::prev() { current = findPrev(current); }

std::string Dictionary::to_string() const {
  std::string s = "";
  Dictionary dict = *this;
  inOrderString(s, dict.root);
  return s;
}

std::string Dictionary::pre_string() const {
  Dictionary dict = *this;
  std::string s = "";
  preOrderString(s, root);
  return s;
}

bool Dictionary::equals(const Dictionary& dict) const {
  return to_string() == dict.to_string();
}

std::ostream& operator<<(std::ostream& stream, Dictionary& dict) {
  return stream << dict.to_string();
}

bool operator==(const Dictionary& root, const Dictionary& _dict) {
  return root.equals(_dict);
}

Dictionary& Dictionary::operator=(const Dictionary& dict) {
  if (this != &dict) {
    Dictionary tmp = dict;
    std::swap(root, tmp.root);
    std::swap(current, tmp.current);
    std::swap(num_pairs, tmp.num_pairs);
    std::swap(nil, tmp.nil);
  }
  return *this;
}