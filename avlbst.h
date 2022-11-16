#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"
#include <cassert>

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
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
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

    //helper functions
    void rotateLeft(AVLNode<Key,Value>* n);
    void rotateRight(AVLNode<Key,Value>* n);
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key, Value>* p, int diff);
    AVLNode<Key,Value>* findKey(AVLNode<Key,Value>* n, const Key& key);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    if(this->root_ == NULL){ 
      this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
      static_cast<AVLNode<Key, Value>*>(this->root_)->setBalance(0);
      return;
     } 
    else if(findKey(static_cast<AVLNode<Key, Value>*>(this->root_), new_item.first) != NULL) {
      AVLNode<Key,Value> * result = findKey(static_cast<AVLNode<Key, Value>*>(this->root_), new_item.first);
      //just update the value
      result->setValue(new_item.second);
      return;
    }
    AVLNode<Key,Value> * traverse = static_cast<AVLNode<Key, Value>*>(this->root_);
    //pointer that maintains the previous position of traverse
    AVLNode<Key, Value>* previous = NULL;
    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    n->setBalance(0);
    while ( traverse != NULL ) {
      previous = traverse;
      if( new_item.first < traverse->getKey() ) {
        traverse = traverse->getLeft();
      }
      else {
        traverse = traverse->getRight();
      }
    }
    //Set parent node
    n->setParent(previous);
    //Insert the node
    if(n->getKey() < previous->getKey()) {
      previous->setLeft(n);
    }
    else {
      previous->setRight(n);
    }
    if((int) previous->getBalance() == -1 || (int) previous->getBalance() == 1 ) {
      previous->setBalance(0);
      return;
    }
    if(previous->getLeft() == n) {
      previous->updateBalance(-1);
    }
    else if (previous->getRight() == n) {
      previous->updateBalance(1);
    }
    insertFix(previous, n);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
  int diff = 0;
  //Check if key is in tree
  AVLNode<Key,Value> *n = findKey(static_cast<AVLNode<Key, Value>*>(this->root_),key);
  if (!n) {
    return;
  }

  if(n->getRight() && n->getLeft()){
    AVLNode<Key,Value> * previous = static_cast<AVLNode<Key, Value>*>(this->predecessor(n));
    nodeSwap(n, previous);
  }

  AVLNode<Key,Value>* p = n->getParent();
  if(p != NULL ) {
    if( p->getLeft() == n ) {
      diff = 1;
    }
    if( p->getRight() == n ) {
      diff = -1;
    }
  }

    //Check how many children
  if( n->getLeft() == NULL || n->getRight() == NULL) {
      AVLNode<Key,Value>* tmp;
      if( n->getRight() == NULL ) {
        tmp = n->getLeft();
      }
      else {
        tmp = n->getRight();
      }
      //check if node that needs to be deleted is the root
      if(p == NULL) {
        if(tmp != NULL) {
          tmp->setParent(p);
        }
        this->root_ = tmp;
        n = NULL;
        delete n;
      }
      else {
        if( n == p->getLeft() ) {
          p->setLeft(tmp);
        }
        else {
          p->setRight(tmp);
        }
        if(tmp != NULL) {
          tmp->setParent(p);
        }
        n = NULL;
        delete n;

      }
      removeFix(p, diff);
      
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
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n) {
  if( p == NULL || p->getParent() == NULL ) {
    return;
  }

  AVLNode<Key,Value>* g = p->getParent();
  if( g->getLeft() == p) {
    g->updateBalance(-1);
    //Case 1
    if ( (int) g->getBalance() == 0 ) {
      return;
    }
    else if ( (int) g->getBalance() == -1 ) {
       insertFix(g, p);
    }
    else if ( (int) g->getBalance() == -2 ) {
      //zig zig case check if n is the left child of parent
      if( n == p->getLeft() ) {
        rotateRight(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      //zig zag case
      else {
        rotateLeft(p);
        rotateRight(g);
        //Case 3a 
        if( (int) n->getBalance() == -1) {
          p->setBalance(0);
          g->setBalance(1);
          n->setBalance(0);
        }
         //Case 3b
        else if( (int) n->getBalance() == 0) {
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
         //Case 3c 
        else if( (int) n->getBalance() == 1) {
          p->setBalance(-1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
  }
  else if( g->getRight() == p) {
    g->updateBalance(1);
    //Case 1
    if ( (int) g->getBalance() == 0 ) {
      return;
    }
    else if ( (int) g->getBalance() == 1 ) {
       insertFix(g, p);
    }
    else if ( (int) g->getBalance() == 2 ) {
      //zig zig case
      if( n == p->getRight() ) {
        rotateLeft(g);
        p->setBalance(0);
        g->setBalance(0);
      }
      //zig zag case
      else {
        rotateRight(p);
        rotateLeft(g);
        //Case 3a 
        if( (int) n->getBalance() == 1) {
          p->setBalance(0);
          g->setBalance(-1);
          n->setBalance(0);
        }
         //Case 3b
        else if( (int) n->getBalance() == 0) {
          p->setBalance(0);
          g->setBalance(0);
          n->setBalance(0);
        }
         //Case 3c 
        else if( (int) n->getBalance() == -1) {
          p->setBalance(1);
          g->setBalance(0);
          n->setBalance(0);
        }
      }
    }
}
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff) {
  int ndiff;
  if( n == NULL ) {
    return;
  }
  AVLNode<Key,Value>* p = n->getParent();
  if( p != NULL ) {
    if(p->getLeft() == n) {
      ndiff = 1;
    }
    else {
      ndiff = -1;
    }
  }
  if(diff == -1) {
    //Case 1: balance(n) + diff == -2
    if( (int) n->getBalance() + diff == -2 ) {
      AVLNode<Key,Value>* c = n->getLeft();
      //case 1a: b(c) == -1
      if((int) c->getBalance() == -1) {
        rotateRight(n);
        n->setBalance(0);
        c->setBalance(0);
        removeFix(p, ndiff);
      }
      //case 1b: b(c) == 0
      else if((int) c->getBalance() == 0) {
        rotateRight(n);
        n->setBalance(-1);
        c->setBalance(1);
        return;
      }
      else if ( (int) c->getBalance() == 1 ) {
        //case 1c b(c) = +1
        AVLNode<Key,Value>* g = c->getRight();
        rotateLeft(c);
        rotateRight(n);
        if((int) g->getBalance() == 1) {
          n->setBalance(0);
          c->setBalance(-1);
          g->setBalance(0);
        }
        else if((int) g->getBalance() == 0) {
          n->setBalance(0);
          c->setBalance(0);
          g->setBalance(0);
        }
        else if((int) g->getBalance() == -1) {
          n->setBalance(1);
          c->setBalance(0);
          g->setBalance(0);
        }
        removeFix(p, ndiff);
      }
    }
    //Case 2
    else if((int) n->getBalance() + diff == -1) {
      n->setBalance(-1);
      return;
    }
    //Case 3
    else if( (int) n->getBalance() + diff == 0 ) {
      n->setBalance(0);
      removeFix(p, ndiff);
    }
  }
  if(diff == 1) {
    //Case 1: balance(n) + diff == 2
    if( (int) n->getBalance() + diff == 2 ) {
      AVLNode<Key,Value>* c = n->getRight();
      //case 1a: b(c) == 1
      if((int) c->getBalance() == 1) {
        rotateLeft(n);
        n->setBalance(0);
        c->setBalance(0);
        removeFix(p, ndiff);
      }
      //case 1b: b(c) == 0
      else if((int) c->getBalance() == 0) {
        rotateLeft(n);
        n->setBalance(1);
        c->setBalance(-1);
        return;
      }
      //case 1c b(c) = +1
      else if( (int) c->getBalance() == -1) {
        AVLNode<Key,Value>* g = c->getLeft();
        rotateRight(c);
        rotateLeft(n);
        if((int) g->getBalance() == -1) {
          n->setBalance(0);
          c->setBalance(1);
          g->setBalance(0);
        }
        else if((int) g->getBalance() == 0) {
          n->setBalance(0);
          c->setBalance(0);
          g->setBalance(0);
        }
        else if((int) g->getBalance() == 1) {
        n->setBalance(-1);
        c->setBalance(0);
        g->setBalance(0);
        }
      removeFix(p, ndiff);
      }
      
    }
    //Case 2
    else if((int) n->getBalance() + diff == 1) {
      n->setBalance(1);
      return;
    }
    //Case 3
    else if( (int) n->getBalance() + diff == 0 ) {
      n->setBalance(0);
      removeFix(p, ndiff);
    }
  }
  


}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* x){
   AVLNode<Key,Value> *y = x->getRight(); // x  
  //  AVLNode<Key,Value> *tmp = n->getRight(); // z 
   AVLNode<Key,Value> *b = y->getLeft(); // b Switches
   AVLNode<Key,Value> *p = x->getParent();

  // Link p and y
  y->setParent(p);
  if (p) {
    if (x == p->getLeft()) {
      p->setLeft(y);
    } else {
      p->setRight(y);
    }
  } else {
    this->root_ = y;
  }

  // Link x and y
  x->setParent(y);
  y->setLeft(x); // Switches

  // Link x and b
  x->setRight(b); // Switches
  if (b) {
    b->setParent(x);
  }

  return;
          
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* x){
   AVLNode<Key,Value> *y = x->getLeft(); // x  
  //  AVLNode<Key,Value> *tmp = n->getRight(); // z 
   AVLNode<Key,Value> *b = y->getRight(); // b Switches
   AVLNode<Key,Value> *p = x->getParent();

  // Link p and y
  y->setParent(p);
  if (p) {
    if (x == p->getLeft()) {
      p->setLeft(y);
    } else {
      p->setRight(y);
    }
  } else {
    this->root_ = y;
  }

  // Link x and y
  x->setParent(y);
  y->setRight(x); // Switches

  // Link x and b
  x->setLeft(b); // Switches
  if (b) {
    b->setParent(x);
  }
  return;  
}


template<class Key, class Value>
AVLNode<Key,Value>* AVLTree<Key, Value>::findKey(AVLNode<Key,Value>* n, const Key& key) {
  if(n == NULL) {
    return NULL;
  }
  else if(n->getKey() == key) {
    return n;
  }
  //look through the left subtree
  else if (n->getKey() > key) {
    
    n = findKey(n->getLeft(), key);
    return n;
  }
  //look through the right subtree
  else {
    n = findKey(n->getRight(), key);
    return n;
  }
  return NULL;
}


#endif
