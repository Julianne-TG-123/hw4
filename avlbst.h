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
    void rotateLeft(AVLNode<Key,Value>* x);
    void rotateRight(AVLNode<Key,Value>* x);
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* child);
    void removeFix(AVLNode<Key,Value>* node);


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

    // Empty tree 
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(key, value, nullptr);
        return;
    }

    // BST insertion
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (current != nullptr) {
        parent = current;
        if (key < current->getKey()) {
            current = current->getLeft();
        } else if (current->getKey() < key) {
            current = current->getRight();
        } else {
            current->setValue(value);
            return;
        }
    }

    // create and link new 
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(key, value, parent);
    if (key < parent->getKey()) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);
    }

    // Propagate balance updates upward
    AVLNode<Key, Value>* child = newNode;
    AVLNode<Key, Value>* ancestor = child->getParent();

    while (ancestor != nullptr) {
        if (child == ancestor->getLeft()) {
            ancestor->updateBalance(-1);
        } else {
            ancestor->updateBalance(1);
        }

        if (ancestor->getBalance() == 0) {
            break;
        }

        if (ancestor->getBalance() == 1 || ancestor->getBalance() == -1) {
            child = ancestor;
            ancestor = ancestor->getParent();
        } else {
            insertFix(ancestor, child);
            break;
        }
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
        nodeSwap(target, pred);
    }

    // Now target has at most one child
    AVLNode<Key, Value>* parent = target->getParent();
    AVLNode<Key, Value>* child = target->getLeft();
    if (child == nullptr) {
        child = target->getRight();
    }

    // Determine which side of parent is being affected
    int8_t diff = 0;
    if (parent != nullptr) {
        if (target == parent->getLeft()) {
            diff = 1;
        } else {
            diff = -1;
        }
    }

    // Perform the actual deletion
    if (child != nullptr) {
        child->setParent(parent);
    }

    if (parent == nullptr) {
        this->root_ = child;
    } else {
        if (target == parent->getLeft()) {
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
    }

    delete target;

    // Rebalance the tree
    if (parent != nullptr) {
        parent->updateBalance(diff);
        int8_t parentBalance = parent->getBalance();

        if (parentBalance == 0) {
            removeFix(parent);
        } else if (parentBalance == 2 || parentBalance == -2) {
            // Need to fix imbalance at parent
            AVLNode<Key, Value>* sibling;
            if (diff == 1) {
                sibling = parent->getRight();
            } else {
                sibling = parent->getLeft();
            }
            
            int8_t siblingBalance = sibling->getBalance();

            if (siblingBalance == 0) {
                if (diff == 1) {
                    rotateLeft(parent);
                } else {
                    rotateRight(parent);
                }
                sibling->setBalance(-diff);
                parent->setBalance(diff);
            } else if (siblingBalance == diff) {
                if (diff == 1) {
                    rotateLeft(parent);
                } else {
                    rotateRight(parent);
                }
                sibling->setBalance(0);
                parent->setBalance(0);
                removeFix(sibling);
            } else {
                AVLNode<Key, Value>* nephew;
                if (diff == 1) {
                    nephew = sibling->getLeft();
                } else {
                    nephew = sibling->getRight();
                }

                if (diff == 1) {
                    rotateRight(sibling);
                    rotateLeft(parent);
                } else {
                    rotateLeft(sibling);
                    rotateRight(parent);
                }

                int8_t nephewBalance = nephew->getBalance();
                if (nephewBalance == diff) {
                    sibling->setBalance(-diff);
                    parent->setBalance(0);
                } else if (nephewBalance == 0) {
                    sibling->setBalance(0);
                    parent->setBalance(0);
                } else {
                    sibling->setBalance(0);
                    parent->setBalance(diff);
                }
                nephew->setBalance(0);
                removeFix(nephew);
            }
        }
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
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightChild = node->getRight();
    AVLNode<Key, Value>* leftGrandchild = rightChild->getLeft();

    // Update parent connection
    rightChild->setParent(node->getParent());
    if (node->getParent() == nullptr) {
        this->root_ = rightChild;
    } else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(rightChild);
    } else {
        node->getParent()->setRight(rightChild);
    }

    // Perform rotation
    rightChild->setLeft(node);
    node->setParent(rightChild);

    // Reattach orphaned subtree
    node->setRight(leftGrandchild);
    if (leftGrandchild != nullptr) {
        leftGrandchild->setParent(node);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* leftChild = node->getLeft();
    AVLNode<Key, Value>* rightGrandchild = leftChild->getRight();

    // Update parent connection
    leftChild->setParent(node->getParent());
    if (node->getParent() == nullptr) {
        this->root_ = leftChild;
    } else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(leftChild);
    } else {
        node->getParent()->setRight(leftChild);
    }

    // Perform rotation
    leftChild->setRight(node);
    node->setParent(leftChild);

    // Reattach orphaned subtree
    node->setLeft(rightGrandchild);
    if (rightGrandchild != nullptr) {
        rightGrandchild->setParent(node);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child)
{
    AVLNode<Key, Value>* grandparent = parent->getParent();
    bool parentIsLeftChild = (parent == grandparent->getLeft());

    if (parentIsLeftChild) {
        bool childIsLeftChild = (child == parent->getLeft());
        
        if (childIsLeftChild) {
            // Left-Left (single)
            rotateRight(grandparent);
            parent->setBalance(0);
            grandparent->setBalance(0);
        } else {
            // Left-Right (double)
            rotateLeft(parent);
            rotateRight(grandparent);

            int8_t childBalance = child->getBalance();
            if (childBalance == -1) {
                parent->setBalance(0);
                grandparent->setBalance(1);
            } else if (childBalance == 0) {
                parent->setBalance(0);
                grandparent->setBalance(0);
            } else {
                parent->setBalance(-1);
                grandparent->setBalance(0);
            }
            child->setBalance(0);
        }
    } else {
        bool childIsRightChild = (child == parent->getRight());
        
        if (childIsRightChild) {
            // Right-Right (single left)
            rotateLeft(grandparent);
            parent->setBalance(0);
            grandparent->setBalance(0);
        } else {
            // Right-Left (double)
            rotateRight(parent);
            rotateLeft(grandparent);

            int8_t childBalance = child->getBalance();
            if (childBalance == 1) {
                parent->setBalance(0);
                grandparent->setBalance(-1);
            } else if (childBalance == 0) {
                parent->setBalance(0);
                grandparent->setBalance(0);
            } else {
                parent->setBalance(1);
                grandparent->setBalance(0);
            }
            child->setBalance(0);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* parent = node->getParent();
    if (parent == nullptr) {
        return;
    }

    // balance based on which child was affected
    int8_t diff;
    if (node == parent->getLeft()) {
        diff = 1;
    } else {
        diff = -1;
    }
    
    parent->updateBalance(diff);
    int8_t parentBalance = parent->getBalance();

    // parent becomes balanced (continue)
    if (parentBalance == 0) {
        removeFix(parent);
        return;
    }

    // parent becomes unbalanced (stop) 
    if (parentBalance == 1 || parentBalance == -1) {
        return;
    }

    // parent becomes very unbalanced (rotate)
    AVLNode<Key, Value>* sibling;
    if (diff == 1) {
        sibling = parent->getRight();
    } else {
        sibling = parent->getLeft();
    }
    
    int8_t siblingBalance = sibling->getBalance();

    if (siblingBalance == 0) {
        // sibling is balanced (single rotation, height same)
        if (diff == 1) {
            rotateLeft(parent);
        } else {
            rotateRight(parent);
        }
        sibling->setBalance(-diff);
        parent->setBalance(diff);
        
        
    } else if (siblingBalance == diff) {
        // sibling same direction (single rotation, height reduced)
        if (diff == 1) {
            rotateLeft(parent);
        } else {
            rotateRight(parent);
        }
        sibling->setBalance(0);
        parent->setBalance(0);
        removeFix(sibling);
        
    } else {
        // sibling opposite direction (double rotation)
        AVLNode<Key, Value>* nephew;
        if (diff == 1) {
            nephew = sibling->getLeft();
        } else {
            nephew = sibling->getRight();
        }

        if (diff == 1) {
            rotateRight(sibling);
            rotateLeft(parent);
        } else {
            rotateLeft(sibling);
            rotateRight(parent);
        }

        int8_t nephewBalance = nephew->getBalance();
        if (nephewBalance == diff) {
            sibling->setBalance(-diff);
            parent->setBalance(0);
        } else if (nephewBalance == 0) {
            sibling->setBalance(0);
            parent->setBalance(0);
        } else {
            sibling->setBalance(0);
            parent->setBalance(diff);
        }
        nephew->setBalance(0);
        removeFix(nephew);
    }
}



#endif
