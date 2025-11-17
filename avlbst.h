#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;
    

protected:
    int8_t balance_;    // effectively a signed char


};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    int getHeight(AVLNode<Key, Value>* node);
    int findBalance(AVLNode<Key, Value>* node);
    void updateBalance(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rebalanceNode(AVLNode<Key, Value>* node);


    void rotateLeft(AVLNode<Key,Value>* x);
    void rotateRight(AVLNode<Key,Value>* x);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
  
    const Key& key = new_item.first;
    const Value& value = new_item.second;

    if(this->root_ == nullptr){
      this->root_ = new AVLNode<Key, Value>(key, value, nullptr);
      return;
    }

    Node<Key, Value>* curr = this->root_;
    Node<Key, Value>* par = nullptr;

    while(curr != nullptr){
      par = curr;
      if(key < curr->getKey()){
        curr = curr->getLeft();
      } 
      else if (key > curr->getKey()){
        curr = curr->getRight();
      } else {
        curr->setValue(value);
        return;
      }
    }

    AVLNode<Key, Value>* avlPar = static_cast<AVLNode<Key, Value>*>(par);
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(key, value, avlPar);

    if(key < par->getKey()){
      par->setLeft(newNode);
    } else {
      par->setRight(newNode);
    }

    AVLNode<Key,Value>* node = avlPar;
    while(node != nullptr){
      rebalanceNode(node);
      if(node->getBalance() == 0){
        break;
      }
      node = node->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* target = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    
    if (target == nullptr) {
        return;
    }

    // Two children case: swap with predecessor
    if (target->getLeft() != nullptr && target->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(target));
        nodeSwap(static_cast<AVLNode<Key, Value>*>(target), static_cast<AVLNode<Key, Value>*>(pred));
    }

    // Now target has at most one child
    AVLNode<Key, Value>* par = target->getParent();
    AVLNode<Key, Value>* child = target->getLeft();
    if (child == nullptr) {
        child = target->getRight();
    }

    if(child != nullptr){
      child->setParent(par);
    }

    if(par == nullptr){
      this->root_ = child;
    } 
    else if(target == par->getLeft()){
      par->setLeft(child);
    } else {
      par->setRight(child);
    }
    delete target;
   
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(par);
    while (node != nullptr) {
      AVLNode<Key, Value>* newSubtreeRoot = rebalanceNode(node);
      int newBalance = newSubtreeRoot->getBalance();
      if (newBalance == -1 || newBalance == 1) {
           break;
      }
        
      node = newSubtreeRoot->getParent();
    }  
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
int AVLTree<Key, Value>::getHeight(AVLNode<Key, Value>* node){
  if(node == nullptr){
    return -1;
  }

  int leftHeight = getHeight(node->getLeft());
  int rightHeight = getHeight(node->getRight());

  if(leftHeight > rightHeight){
    return 1+leftHeight;
  } else {
    return 1+ rightHeight;
  }
}

template<class Key, class Value>
int AVLTree<Key, Value>::findBalance(AVLNode<Key, Value>* node){
  if(node == nullptr){
    return 0;
  }

  return getHeight(node->getRight()) - getHeight(node->getLeft());
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalance(AVLNode<Key, Value>* node){
  if(node != nullptr){
    node->setBalance(findBalance(node));
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* pivot){
  AVLNode<Key, Value>* newRoot = pivot->getRight();
  AVLNode<Key, Value>* newSubtree = newRoot->getLeft();
  AVLNode<Key, Value>* par = pivot->getParent();

  newRoot->setParent(par);
  if(par == nullptr){
    this->root_ = newRoot;
  } else if(pivot == par->getLeft()){
    par->setLeft(newRoot);
  } else {
    par->setRight(newRoot);
  }
  
  newRoot->setLeft(pivot);
  pivot->setParent(newRoot);

  pivot->setRight(newSubtree);
  if(newSubtree != nullptr){
    newSubtree->setParent(pivot);
  }

  updateBalance(pivot);
  updateBalance(newRoot);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* pivot){
  AVLNode<Key, Value>* newRoot = pivot->getLeft();
  AVLNode<Key, Value>* newSubtree = newRoot->getRight();
  AVLNode<Key, Value>* par = pivot->getParent();

  newRoot->setParent(par);
  if(par == nullptr){
    this->root_ = newRoot;
  } else if(pivot == par->getLeft()){
    par->setLeft(newRoot);
  } else {
    par->setRight(newRoot);
  }
  
  newRoot->setRight(pivot);
  pivot->setParent(newRoot);

  pivot->setLeft(newSubtree);
  if(newSubtree != nullptr){
    newSubtree->setParent(pivot);
  }

  updateBalance(pivot);
  updateBalance(newRoot);
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key,Value>::rebalanceNode(AVLNode<Key, Value>* node){
  updateBalance(node);
  int balance = node->getBalance();

  if(balance > 1){
    AVLNode<Key, Value>* rightChild = node->getRight();
    if(rightChild->getBalance() < 0){
      rotateRight(rightChild);
    }
    rotateLeft(node);
    return node->getParent();
  }

  if(balance < -1){
    AVLNode<Key, Value>* leftChild = node->getLeft();
    if(leftChild->getBalance() > 0){
      rotateLeft(leftChild);
    }
    rotateRight(node);
    return node->getParent();
  }
  return node;
}




#endif
