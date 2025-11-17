#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int treeHeight (Node* node);

int treeHeight (Node* node){
  if(node == nullptr || (node->left == nullptr && node->right == nullptr)){
    return 0;
  }


  int leftHeight = 0;
  int rightHeight = 0;

  if(node->left != nullptr){
    leftHeight = treeHeight(node->left);
  }

  if(node->right != nullptr){
    rightHeight = treeHeight(node->right);
  }

  if(leftHeight > rightHeight){
    return leftHeight + 1;
  } else {
    return rightHeight + 1;
  }

}

bool equalPaths(Node * root)
{ 
  // Add your code below
  if(root == nullptr){
    return true;
  }

  if(root->left == nullptr && root->right == nullptr){
      return true;
  }
  
  if (root->left != nullptr && root->right != nullptr){
    int leftHeight = treeHeight(root->left);
    int rightHeight = treeHeight(root->right);
    
    if(leftHeight != rightHeight){
      return false;
    }

    return equalPaths(root->left) && equalPaths(root->right);
  }

  if(root->left){
    return equalPaths(root->left);
  }

  return equalPaths(root->right);

}

