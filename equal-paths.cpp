#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int depth (Node* root);

int depth (Node* root){
  if(root == NULL){
    return 0; // empty subtree
  }

  if (root->left == NULL && root->right == NULL){
    return 0; // no leafs
  }

  int leftDepth = 0;
  int rightDepth =0;

  if(root->left != NULL){
    leftDepth = depth(root->left);
  }

  if(root->right != NULL){
    rightDepth = depth(root->right);
  }

  if(leftDepth >rightDepth){
    return leftDepth+1;
  } else {
    return rightDepth+1;
  }

}

bool equalPaths(Node * root)
{ 
  // Add your code below
  if(root == NULL){
    return true;
  }

  if(root->left == NULL && root->right == NULL){
      return true;
  }
  
  if (root->left != NULL && root->right != NULL){
    int leftDepth = depth(root->left);
    int rightDepth = depth(root->right);
    
    if(leftDepth != rightDepth){
      return false;
    }

    return equalPaths(root->left) && equalPaths(root->right);
  }

  if(root->left != NULL){
    return equalPaths(root->left);
  }

  return equalPaths(root->right);

}

