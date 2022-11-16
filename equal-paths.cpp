#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool pathLevel(Node * root, int level, int* leftest_level) {
    
    if (root == NULL) return true;

    //leaf node to be compared to original most left leaf node height
    if (root->left == NULL && root->right == NULL) {

        //on leftest most node right now
        if (*leftest_level == 0) {
            *leftest_level = level;
            return true;
        }

        else if ( level == *leftest_level) {  
            return true;
        }
        return false;
    }

    //node wasn't a leaf node
    return pathLevel(root->left, level + 1, leftest_level) && pathLevel(root->right, level + 1, leftest_level);

}

bool equalPaths(Node * root)
{
   int level = 0;
   //level of the most left node (to be compared with all other nodes)
   int leftest_level = 0;

   return pathLevel(root, level, &leftest_level);

}

