/// MIT License
///
/// Copyright (c) 2017 Arkadiusz Netczuk <dev.arnet@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include "rbtree/AbstractRBTree.h"

#include <stdlib.h>                     /// free
#include <assert.h>
#include <string.h>



void rbtree_init(ARBTree* tree) {
    assert( tree != NULL );

    tree->root = NULL;

    tree->fIsLessOrder = NULL;

    tree->fTryFitRight = NULL;
    tree->fTryFitLeft = NULL;

    tree->fPrintValue = NULL;
    tree->fDeleteValue = NULL;
}

static const ARBTreeNode* rbtree_getLeftmostNode(const ARBTreeNode* node) {
    if (node == NULL) {
        return NULL;
    }
    const ARBTreeNode* curr = node;
    while (curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}

static const ARBTreeNode* rbtree_getRightmostNode(const ARBTreeNode* node) {
    if (node == NULL) {
        return NULL;
    }
    const ARBTreeNode* curr = node;
    while (curr->right != NULL) {
        curr = curr->right;
    }
    return curr;
}

/**
 *  node
 *     \
 *       \
 *         n
 *        /
 *      ret
 */
static const ARBTreeNode* rbtree_getRightDescendant(const ARBTreeNode* node) {
    return rbtree_getLeftmostNode(node->right);
}

/**
 *      node
 *      /
 *    /
 *  n
 *   \
 *   ret
 */
static const ARBTreeNode* rbtree_getLeftDescendant(const ARBTreeNode* node) {
    return rbtree_getRightmostNode(node->left);
}


/// ===================================================


static size_t rbtree_sizeSubtree(const ARBTreeNode* node) {
    if (node==NULL) {
        return 0;
    }
    const size_t leftSize = rbtree_sizeSubtree(node->left);
    const size_t rightSize = rbtree_sizeSubtree(node->right);
    return leftSize + rightSize + 1;
}

size_t rbtree_size(const ARBTree* tree) {
    assert( tree != NULL );
    return rbtree_sizeSubtree(tree->root);
}

static size_t rbtree_depthSubtree(const ARBTreeNode* node) {
	if (node == NULL) {
		return 0;
	}
	const size_t dLeft = rbtree_depthSubtree(node->left);
	const size_t dRight = rbtree_depthSubtree(node->right);
	if (dLeft>dRight) {
		return dLeft+1;
	} else {
		return dRight+1;
	}
}

size_t rbtree_depth(const ARBTree* tree) {
    assert( tree != NULL );
    return rbtree_depthSubtree(tree->root);
}

static const ARBTreeNode* rbtree_leftNode(const ARBTreeNode* node) {
    const ARBTreeNode* desc = rbtree_getLeftDescendant(node);
    if (desc != NULL) {
        return desc;
    }
    return rbtree_getLeftAncestor(node);
}

static const ARBTreeNode* rbtree_rightNode(const ARBTreeNode* node) {
    const ARBTreeNode* desc = rbtree_getRightDescendant(node);
    if (desc != NULL) {
        return desc;
    }
    return rbtree_getRightAncestor(node);
}

static const ARBTreeNode* rbtree_nodeByIndex(const ARBTreeNode* node, const size_t index) {
    if (node==NULL) {
        return NULL;
    }
    const size_t currIndex = rbtree_nodeIndex(node);
    if (index < currIndex) {
        const ARBTreeNode* left = rbtree_leftNode(node);
        return rbtree_nodeByIndex(left, index);
    }
    if (index > currIndex) {
        const ARBTreeNode* right = rbtree_rightNode(node);
        return rbtree_nodeByIndex(right, index);
    }
    /// index equals
    return node;
}

ARBTreeValue rbtree_valueByIndex(const ARBTree* tree, const size_t index) {
    if (tree == NULL) {
        return NULL;
    }
    const ARBTreeNode* node = rbtree_nodeByIndex( tree->root, index );
    if (node == NULL) {
        return NULL;
    }
    return node->value;
}


/// ==================================================================================


static ARBTreeValidationError rbtree_isValid_checkConnections(const ARBTreeNode* node, const ARBTreeNode* parent) {
    if (node == NULL) {
        return ARBTREE_INVALID_OK;
    }
    if (node->parent != parent) {
        return ARBTREE_INVALID_NODE_PARENT;
    }
    if ((node->left == node->right) && (node->right != NULL)) {
        /// the same child
        return ARBTREE_INVALID_SAME_CHILD;
    }

    const ARBTreeValidationError validLeft = rbtree_isValid_checkConnections(node->left, node);
    if (validLeft != ARBTREE_INVALID_OK) {
        return validLeft;
    }
    const ARBTreeValidationError validRight = rbtree_isValid_checkConnections(node->right, node);
    if (validRight != ARBTREE_INVALID_OK) {
        return validRight;
    }

    return ARBTREE_INVALID_OK;
}

static ARBTreeValidationError rbtree_isValid_checkSorted(const ARBTree* tree, const ARBTreeNode* node) {
    if (node == NULL) {
        return ARBTREE_INVALID_OK;
    }

    const ARBTreeNode* prevTop = rbtree_getLeftAncestor(node);
    if (prevTop != NULL) {
        if (tree->fIsLessOrder(node->value, prevTop->value) == true) {
        	return ARBTREE_INVALID_NOT_SORTED;
        }
    }
    const ARBTreeNode* nextTop = rbtree_getRightAncestor(node);
    if (nextTop != NULL) {
        if (tree->fIsLessOrder(nextTop->value, node->value) == true) {
        	return ARBTREE_INVALID_NOT_SORTED;
        }
    }

    const ARBTreeNode* prevBottom = rbtree_getLeftDescendant(node);
    if (prevBottom != NULL) {
        if (tree->fIsLessOrder(node->value, prevBottom->value) == true) {
        	return ARBTREE_INVALID_NOT_SORTED;
        }
    }
    const ARBTreeNode* nextBottom = rbtree_getRightDescendant(node);
    if (nextBottom != NULL) {
        if (tree->fIsLessOrder(nextBottom->value, node->value) == true) {
        	return ARBTREE_INVALID_NOT_SORTED;
        }
    }

    const ARBTreeValidationError validLeft = rbtree_isValid_checkSorted(tree, node->left);
    if (validLeft != ARBTREE_INVALID_OK) {
        return validLeft;
    }
    const ARBTreeValidationError validRight = rbtree_isValid_checkSorted(tree, node->right);
    if (validRight != ARBTREE_INVALID_OK) {
        return validRight;
    }

    return ARBTREE_INVALID_OK;
}

static ARBTreeValidationError rbtree_isValid_countBlackPaths(const ARBTreeNode* node, size_t* counter) {
    if (node == NULL) {
        *counter = 0;
        return ARBTREE_INVALID_OK;
    }
    size_t leftCounter = 0;
    const ARBTreeValidationError validLeft = rbtree_isValid_countBlackPaths(node->left, &leftCounter);
    if (validLeft != ARBTREE_INVALID_OK) {
        return validLeft;
    }
    size_t rightCounter = 0;
    const ARBTreeValidationError validRight = rbtree_isValid_countBlackPaths(node->right, &rightCounter);
    if (validRight != ARBTREE_INVALID_OK) {
        return validRight;
    }

    if (leftCounter != rightCounter) {
        return ARBTREE_INVALID_BLACK_PATH;
    }

    if (node->color == ARBTREE_COLOR_BLACK) {
        *counter = leftCounter+1;
    } else {
        *counter = leftCounter;
    }

    return ARBTREE_INVALID_OK;
}

/**
 * Every path from a given node to any of its descendant NULL nodes
 * contains the same number of black nodes.
 */
static ARBTreeValidationError rbtree_isValid_checkBlackPath(const ARBTreeNode* node) {
    if (node == NULL) {
        return ARBTREE_INVALID_OK;
    }
    size_t counter = 0;
    return rbtree_isValid_countBlackPaths(node, &counter);
}

static ARBTreeValidationError rbtree_isValid_checkColor(const ARBTreeNode* node) {
	if (node->left != NULL) {
		const ARBTreeValidationError validLeft = rbtree_isValid_checkColor(node->left);
		if (validLeft != ARBTREE_INVALID_OK) {
			return validLeft;
		}
    }
	if (node->right != NULL) {
		const ARBTreeValidationError validRight = rbtree_isValid_checkColor(node->right);
		if (validRight != ARBTREE_INVALID_OK) {
			return validRight;
		}
	}

    if (node->color != ARBTREE_COLOR_RED) {
    	return ARBTREE_INVALID_OK;
    }
	if (node->left != NULL) {
		if (node->left->color != ARBTREE_COLOR_BLACK) {
			return ARBTREE_INVALID_BLACK_CHILDREN;
		}
	}
	if (node->right != NULL) {
		if (node->right->color != ARBTREE_COLOR_BLACK) {
			return ARBTREE_INVALID_BLACK_CHILDREN;
		}
	}

	return ARBTREE_INVALID_OK;
}

ARBTreeValidationError rbtree_isValid(const ARBTree* tree) {
    assert( tree != NULL );

    if (tree->root == NULL)
        return ARBTREE_INVALID_OK;
    if (tree->root->parent != NULL)
        return ARBTREE_INVALID_ROOT_PARENT;

    const ARBTreeNode* rootNode = tree->root;

    /// check pointers
    const ARBTreeValidationError validPointers = rbtree_isValid_checkConnections(rootNode, rootNode->parent);
    if (validPointers != ARBTREE_INVALID_OK) {
        return validPointers;
    }
    /// if pointers are valid, then there is no cycles

    /// check is sorted
    const ARBTreeValidationError validOrder = rbtree_isValid_checkSorted(tree, rootNode);
    if (validOrder != ARBTREE_INVALID_OK) {
        return validOrder;
    }

    /// checking red-black properties
    /// root is black
    if (rootNode->color != ARBTREE_COLOR_BLACK)
        return ARBTREE_INVALID_RED_ROOT;

    /// if a node is red, then both its children are black
    const ARBTreeValidationError validColor = rbtree_isValid_checkColor(rootNode);
    if (validColor != ARBTREE_INVALID_OK) {
        return validColor;
    }

    const ARBTreeValidationError validPath = rbtree_isValid_checkBlackPath(rootNode);
    if (validPath != ARBTREE_INVALID_OK) {
        return validPath;
    }

    return ARBTREE_INVALID_OK;
}


/// ==================================================================================


ARBTreeNode* rbtree_findNode(const ARBTree* tree, const ARBTreeValue value) {
    assert( tree != NULL );

    ARBTreeNode* curr = tree->root;
    while (curr != NULL) {
        if ( tree->fIsLessOrder(value, curr->value) == true ) {
            /// value < curr->value
            curr = curr->left;
            continue ;
        }
        if ( tree->fIsLessOrder(curr->value, value) == true ) {
            /// value > curr->value
            curr = curr->right;
            continue ;
        }

        /// equal
        return curr;
    }
    /// not found
    return NULL;
}

static const ARBTreeNode* rbtree_findRootFromNode(const ARBTreeNode* node) {
    /// find the new root to return
    const ARBTreeNode* curr = node;
    while (curr->parent != NULL)
        curr = curr->parent;
    return curr;
}

static void rbtree_findRoot(ARBTree* tree) {
    /// find the new root to return
    tree->root = (ARBTreeNode*) rbtree_findRootFromNode(tree->root);
}


/// ==================================================================================


static ARBTreeNode* rbtree_grandparent(ARBTreeNode* node) {
	assert( node->parent != NULL );
	return node->parent->parent;
}

static ARBTreeNode* rbtree_sibling(ARBTreeNode* node) {
	assert( node->parent != NULL );
	if (node->parent->left == node)
		return node->parent->right;
	else
		return node->parent->left;
}

static inline ARBTreeNode* rbtree_uncle(ARBTreeNode* node) {
	return rbtree_sibling(node->parent);
}

static inline void rbtree_setLeftChild(ARBTreeNode* node, ARBTreeNode* child) {
    node->left = child;
    if (child != NULL) {
        child->parent = node;
    }
}

static inline void rbtree_setRightChild(ARBTreeNode* node, ARBTreeNode* child) {
    node->right = child;
    if (child != NULL) {
        child->parent = node;
    }
}

static inline void rbtree_changeChild(ARBTreeNode* node, ARBTreeNode* from, ARBTreeNode* to) {
    if (node->left == from) {
        rbtree_setLeftChild(node, to);
    } else {
        rbtree_setRightChild(node, to);
    }
}

static void rbtree_rotate_left(ARBTreeNode* node) {
    ARBTreeNode* parent = node->parent;
    ARBTreeNode* nnew = node->right;
    assert(nnew != NULL);                   /// since the leaves of a red-black tree are empty, they cannot become internal nodes
    rbtree_setRightChild(node, nnew->left);
    rbtree_setLeftChild(nnew, node);
    if (parent == NULL) {
        nnew->parent = NULL;
        return;
    }
    if (parent->left == node) {
        rbtree_setLeftChild(parent, nnew);
    } else {
        rbtree_setRightChild(parent, nnew);
    }
}

static void rbtree_rotate_right(ARBTreeNode* node) {
    ARBTreeNode* parent = node->parent;
    ARBTreeNode* nnew = node->left;
    assert(nnew != NULL);                   /// since the leaves of a red-black tree are empty, they cannot become internal nodes
    rbtree_setLeftChild(node, nnew->right);
    rbtree_setRightChild(nnew, node);
    if (parent == NULL) {
        nnew->parent = NULL;
        return;
    }
    if (parent->left == node) {
        rbtree_setLeftChild(parent, nnew);
    } else {
        rbtree_setRightChild(parent, nnew);
    }
}

static void rbtree_repair_insert(ARBTreeNode* node) {
	ARBTreeNode* nParent = node->parent;
	if ( nParent == NULL) {
		node->color = ARBTREE_COLOR_BLACK;
		return ;
	}
	if (nParent->color == ARBTREE_COLOR_BLACK) {
		/// do nothing
		return ;
	}
	ARBTreeNode* uncle = rbtree_uncle(node);
	if (uncle != NULL) {
        if (uncle->color == ARBTREE_COLOR_RED) {
            nParent->color = ARBTREE_COLOR_BLACK;
            uncle->color = ARBTREE_COLOR_BLACK;
            ARBTreeNode* grandpa = rbtree_grandparent(node);		/// never NULL here
            grandpa->color = ARBTREE_COLOR_RED;
            rbtree_repair_insert(grandpa);
            return ;
        }
	}

    ARBTreeNode* curr = node;
	{
        ARBTreeNode* grandpa = rbtree_grandparent(curr);		/// never NULL here
        if ((grandpa->left != NULL) && (curr == grandpa->left->right)) {
            rbtree_rotate_left(curr->parent);
            curr = curr->left;
        } else if ((grandpa->right != NULL) && (curr == grandpa->right->left)) {
            rbtree_rotate_right(curr->parent);
            curr = curr->right;
        }
	}
	{
	    ARBTreeNode* grandpa = rbtree_grandparent(curr);       /// never NULL here
        if (curr == curr->parent->left)
            rbtree_rotate_right(grandpa);
        else
            rbtree_rotate_left(grandpa);
        curr->parent->color = ARBTREE_COLOR_BLACK;
        grandpa->color = ARBTREE_COLOR_RED;
	}
}

static ARBTreeNode* rbtree_insertLeftNode(ARBTreeNode* node) {
	ARBTreeNode* oldLeft = node->left;
	ARBTreeNode* newNode = rbtree_makeColoredNode(ARBTREE_COLOR_RED);         /// default color of new node
	rbtree_setLeftChild(node, newNode);
	rbtree_setLeftChild(newNode, oldLeft);

	rbtree_repair_insert(newNode);
	return newNode;
}

static ARBTreeNode* rbtree_insertRightNode(ARBTreeNode* node) {
	ARBTreeNode* oldLeft = node->right;
	ARBTreeNode* newNode = rbtree_makeColoredNode(ARBTREE_COLOR_RED);         /// default color of new node
	rbtree_setRightChild(node, newNode);
	newNode->right = oldLeft;
	rbtree_setRightChild(newNode, oldLeft);

	rbtree_repair_insert(node->right);
	return newNode;
}


/// ======================================================================================


/**
 * Adding to left side: value should be smaller than 'node->value'
 * Returns:
 *      -1 - if cannot add because of sub-node
 *       0 - if cannot fit: bad order or cannot fit
 *       1 - if added
 */
static int rbtree_addToLeft(const ARBTree* tree, ARBTreeNode* node, ARBTreeValue value) {
    if ( tree->fIsLessOrder(value, node->value) == false ) {
        /// could not add on left side  -- 'value' is equal or greater than node
        return 0;               /// go to right
    }

    if ( node->left != NULL ) {
        /// leaf exists -- go to left
        return -1;
    }

    /// leaf case -- can add
    if ( tree->fTryFitLeft != NULL ) {
        if ( tree->fTryFitLeft(node, value) == false ) {
            return 0;       /// go to right
        }
    }
    ARBTreeNode* newNode = rbtree_insertLeftNode(node);
    newNode->value = value;
    return 1;
}

/**
 * Adding to right side: value should be greater than 'node->value'
 * Returns:
 *      -1 - if cannot add because of sub-node
 *       0 - if cannot fit
 *       1 - if added
 */
static int rbtree_addToRight(const ARBTree* tree, ARBTreeNode* node, ARBTreeValue value) {
    /// if is not less than left, then it goes to right

    if ( node->right != NULL ) {
        /// leaf exists -- go to right
        return -1;
    }

    /// leaf case -- can add
    if ( tree->fTryFitRight != NULL ) {
        if ( tree->fTryFitRight(node, value) == false ) {
            return 0;
        }
    }
    ARBTreeNode* newNode = rbtree_insertRightNode(node);
    newNode->value = value;
    return 1;
}

/**
 * If no smaller node exists, then returns greater node.
 */
static ARBTreeNode* rbtree_findSmallerNode(const ARBTree* tree, ARBTreeNode* currNode, ARBTreeValue value) {
    ARBTreeNode* tmpNode = currNode;
    ARBTreeNode* bestNode = tmpNode;
    bool valid = false;
    while(tmpNode!=NULL) {
        if ( tree->fIsLessOrder(tmpNode->value, value)) {
            /// in order
            bestNode = tmpNode;
            valid = true;
            if (tmpNode->right == NULL) {
                break;
            }
            /// check for more accurate results
            tmpNode = tmpNode->right;
        } else {
            /// 'value' is equal or less than node -- check right subtree
            if (valid == false) {
                bestNode = tmpNode;
            }
            if (tmpNode->left == NULL) {
                /// no 'smaller' node exists
                break;
            }
            tmpNode = tmpNode->left;
        }
    }
    return bestNode;
}

/**
 * Returns node that has at least one NULLed leaf.
 */
static const ARBTreeNode* rbtree_getRightLeaf(const ARBTreeNode* node) {
    const ARBTreeNode* bellow = rbtree_getRightDescendant(node);        /// NULL when no right node
    if (bellow!=NULL) {
        /// left node is NULL
        return bellow;
    }
    const ARBTreeNode* ancestor = rbtree_getRightAncestor(node);        /// NULL when no parent
    if (ancestor==NULL) {
        return NULL;
    }
    const ARBTreeNode* right = rbtree_getRightDescendant(ancestor);     /// NULL when no right node
    if (right!=NULL) {
        /// left node is NULL
        return right;
    }
    /// right node is NULL
    return ancestor;
}

static bool rbtree_addToNode(const ARBTree* tree, ARBTreeNode* currNode, ARBTreeValue value) {
    ARBTreeNode* tmpNode = rbtree_findSmallerNode(tree, currNode, value);       /// never NULL
    while(tmpNode!=NULL) {
        const int leftState = rbtree_addToLeft(tree, tmpNode, value);
        if (leftState > 0) {
            return true;
        }
        const int rightState = rbtree_addToRight(tree, tmpNode, value);
        if (rightState > 0) {
            return true;
        }

        tmpNode = (ARBTreeNode*) rbtree_getRightLeaf(tmpNode);
        assert(tmpNode != NULL);
    }
    return false;
}

bool rbtree_add(ARBTree* tree, const ARBTreeValue value) {
    assert( tree != NULL );

    if (tree->root == NULL) {
        tree->root = rbtree_makeDefaultNode();
        ///tree->root->parent = NULL;
        ///tree->root->color = RBTREE_BLACK;
        tree->root->value = value;
        rbtree_repair_insert(tree->root);
        return true;
    }

    if (rbtree_addToNode(tree, tree->root, value) == false) {
        return false;
    }
    rbtree_findRoot(tree);
    return true;
}


/// ==============================================================================================


static void rbtree_printLevel(const ARBTree* tree, const ARBTreeNode* node, const size_t level, int* previousNodeIndex) {
    if (node == NULL) {
        return ;
    }

    if (level > 0) {
        rbtree_printLevel(tree, node->left, level-1, previousNodeIndex);
        rbtree_printLevel(tree, node->right, level-1, previousNodeIndex);
        return ;
    }

    const size_t pos = rbtree_nodeIndex(node);
    const size_t diff = (pos==0) ? 0 : (pos - *previousNodeIndex) - 1;

    /// print empty space on left
    for(size_t i=0; i<diff; ++i) {
//        printf("          ");
        printf("      ");
    }

    char color = 'X';
    switch(node->color) {
    case ARBTREE_COLOR_BLACK: {
        color = 'B';
        break;
    }
    case ARBTREE_COLOR_RED: {
        color = 'R';
        break;
    }
    }

    printf("(%c,", color);
    tree->fPrintValue(node->value);
    printf(")");

    *previousNodeIndex = pos;
}

void rbtree_print(const ARBTree* tree) {
    if (tree == NULL) {
        printf("%s", "[NULL]");
        return ;
    }
    const ARBTreeNode* root = tree->root;
    if (root == NULL) {
        printf("%s", "(NULL)");
        return ;
    }
    const size_t depth = rbtree_depthSubtree(root);
    for(size_t l = 0; l < depth; ++l) {
        int previousNodeIndex = -1;
        rbtree_printLevel(tree, root, l, &previousNodeIndex);
        printf("%s", "\n");
    }
}


/// ==============================================================================================


static void rbtree_releaseNode(ARBTree* tree, ARBTreeNode* node) {
    tree->fDeleteValue(node->value);
    free(node);
}

static int rbtree_releaseSubtree(ARBTree* tree, ARBTreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    /**
     * Done in recursive manner. In case of very large structures consider
     * reimplementing it using while() and vector structure.
     */
    const int leftReleased = rbtree_releaseSubtree(tree, node->left);
    const int rightReleased = rbtree_releaseSubtree(tree, node->right);

    rbtree_releaseNode(tree, node);

    return leftReleased+rightReleased+1;
}

bool rbtree_release(ARBTree* tree) {
    assert( tree != NULL );
    if (tree->root==NULL) {
        /// empty is valid, so releasing is successful
        return true;
    }
    rbtree_releaseSubtree(tree, tree->root);
    tree->root = NULL;
    return true;
}


/// =========================================================================


static ARBTreeNode* rbtree_repair_sibling(ARBTreeNode* parent, ARBTreeNode* node) {
    if (parent == NULL) {
        return NULL;
    }
    if (parent->left == node)
        return parent->right;
    else
        return parent->left;
}

static int rbtree_repair_isChildrenColors(const ARBTreeNode* parent, const ARBTreeNodeColor leftChild, const ARBTreeNodeColor rightChild) {
    if ( parent->left != NULL ) {
        if ( parent->left->color != leftChild ) {
            return 1;
        }
    } else {
        if ( leftChild != ARBTREE_COLOR_BLACK) {
            return 1;
        }
    }
    if ( parent->right != NULL ) {
        if ( parent->right->color != rightChild ) {
            return 1;
        }
    } else {
        if ( rightChild != ARBTREE_COLOR_BLACK) {
            return 1;
        }
    }
    return 0;
}

static void rbtree_repair_case1(ARBTreeNode* parent, ARBTreeNode* node);

static void rbtree_repair_case6(ARBTreeNode* parent, ARBTreeNode* node) {
    ARBTreeNode* sibling = rbtree_repair_sibling(parent, node);
    sibling->color = parent->color;
    parent->color = ARBTREE_COLOR_BLACK;

    if (parent->left == node) {
        if (sibling->right!=NULL)
            sibling->right->color = ARBTREE_COLOR_BLACK;
        rbtree_rotate_left( parent );
    } else {
        if (sibling->left!=NULL)
            sibling->left->color = ARBTREE_COLOR_BLACK;
        rbtree_rotate_right( parent );
    }
}

static void rbtree_repair_case5(ARBTreeNode* parent, ARBTreeNode* node) {
    ARBTreeNode* sibling = rbtree_repair_sibling(parent, node);
    if (sibling->color != ARBTREE_COLOR_BLACK) {
        rbtree_repair_case6(parent, node);
        return ;
    }
//    if (node == NULL) {
//        rbtree_repair_case6(parent, node);
//        return ;
//    }

    if (parent->left == node) {
        /// is left child
        if (rbtree_repair_isChildrenColors(sibling, ARBTREE_COLOR_RED, ARBTREE_COLOR_BLACK) == 0) {
            sibling->color = ARBTREE_COLOR_RED;
            if (sibling->left != NULL)
                sibling->left->color = ARBTREE_COLOR_BLACK;
            rbtree_rotate_right( sibling );
            rbtree_repair_case6(parent, node);
            return ;
        }
        rbtree_repair_case6(parent, node);
        return ;
    }

    if (parent->right == node) {
        /// is right child
        if (rbtree_repair_isChildrenColors(sibling, ARBTREE_COLOR_BLACK, ARBTREE_COLOR_RED) == 0) {
            sibling->color = ARBTREE_COLOR_RED;
            if (sibling->right != NULL)
                sibling->right->color = ARBTREE_COLOR_BLACK;
            rbtree_rotate_left( sibling );
            rbtree_repair_case6(parent, node);
            return ;
        }
        rbtree_repair_case6(parent, node);
        return ;
    }
}

static void rbtree_repair_case4(ARBTreeNode* parent, ARBTreeNode* node) {
    if (parent->color != ARBTREE_COLOR_RED) {
        rbtree_repair_case5(parent, node);
        return ;
    }
    ARBTreeNode* sibling = rbtree_repair_sibling(parent, node);
    if (sibling->color != ARBTREE_COLOR_BLACK) {
        rbtree_repair_case5(parent, node);
        return ;
    }
    if (rbtree_repair_isChildrenColors(sibling, ARBTREE_COLOR_BLACK, ARBTREE_COLOR_BLACK) != 0) {
        rbtree_repair_case5(parent, node);
        return ;
    }
    sibling->color = ARBTREE_COLOR_RED;
    parent->color = ARBTREE_COLOR_BLACK;
}

static void rbtree_repair_case3(ARBTreeNode* parent, ARBTreeNode* node) {
    if (parent->color != ARBTREE_COLOR_BLACK) {
        rbtree_repair_case4(parent, node);
        return ;
    }
    ARBTreeNode* sibling = rbtree_repair_sibling(parent, node);
    if (sibling->color != ARBTREE_COLOR_BLACK) {
        rbtree_repair_case4(parent, node);
        return ;
    }

    if (rbtree_repair_isChildrenColors(sibling, ARBTREE_COLOR_BLACK, ARBTREE_COLOR_BLACK) != 0) {
        rbtree_repair_case4(parent, node);
        return ;
    }

    sibling->color = ARBTREE_COLOR_RED;
    rbtree_repair_case1(parent->parent, parent);
}

static void rbtree_repair_case2(ARBTreeNode* parent, ARBTreeNode* node) {
    ARBTreeNode* sibling = rbtree_repair_sibling(parent, node);
    if (sibling == NULL) {
        /// case of root
        return ;
    }
    if (sibling->color == ARBTREE_COLOR_RED) {
        parent->color = ARBTREE_COLOR_RED;
        sibling->color = ARBTREE_COLOR_BLACK;
        if (parent->left == node)
            rbtree_rotate_left( parent );
        else
            rbtree_rotate_right( parent );
    }
    rbtree_repair_case3(parent, node);
}

static void rbtree_repair_case1(ARBTreeNode* parent, ARBTreeNode* node) {
    if (parent == NULL) {
        return ;
    }
    rbtree_repair_case2(parent, node);
}

static void rbtree_repair_delete(ARBTreeNode* parent, ARBTreeNode* node) {
    if (node != NULL) {
        if (node->color == ARBTREE_COLOR_RED) {
            node->color = ARBTREE_COLOR_BLACK;
            return ;
        }
    }

    /// restore -- cases
    rbtree_repair_case1(parent, node);
}

bool rbtree_delete(ARBTree* tree, const ARBTreeValue value) {
    ARBTreeNode* node = rbtree_findNode( tree, value );
    if (node == NULL) {
        /// node not found -- nothing to remove
        return false;
    }

    if (node->right == NULL) {
        /// simple case -- just remove
        if ( node->parent != NULL ) {
            /// non-root case
            rbtree_changeChild(node->parent, node, node->left);
        } else {
            /// removing root
            tree->root = node->left;
            if (node->left != NULL) {
                node->left->parent = NULL;
            }
        }

        if (node->color == ARBTREE_COLOR_BLACK) {
            rbtree_repair_delete(node->parent, node->left);
            /// can happpen than root changes due to rotations
            rbtree_findRoot(tree);
        }

        rbtree_releaseNode(tree, node);
        return true;
    }

    if (node->left == NULL) {
        /// simple case -- just reconnect
        if ( node->parent != NULL ) {
            /// non-root case
            rbtree_changeChild(node->parent, node, node->right);
        } else {
            /// removing root
            tree->root = node->right;
            if (node->right != NULL) {
                node->right->parent = NULL;
            }
        }

        if (node->color == ARBTREE_COLOR_BLACK) {
            rbtree_repair_delete(node->parent, node->right);
            /// can happpen than root changes due to rotations
            rbtree_findRoot(tree);
        }

        rbtree_releaseNode(tree, node);
        return true;
    }

    /// have both children
    /// there is right subtree

    ARBTreeNode* nextNode = (ARBTreeNode*) rbtree_getRightDescendant(node);      /// never NULL

    /// swap pointer values (it's important -- it causes to release proper pointer)
    ARBTreeValue tmpVal = node->value;
    node->value = nextNode->value;
    nextNode->value = tmpVal;

    rbtree_changeChild(nextNode->parent, nextNode, nextNode->right);
    if (nextNode->color == ARBTREE_COLOR_BLACK) {
        rbtree_repair_delete(nextNode->parent, nextNode->right);
        /// can happpen than root changes due to rotations
        rbtree_findRoot(tree);
    }

    rbtree_releaseNode(tree, nextNode);
    return true;
}


/// =========================================================


ARBTreeNode* rbtree_makeDefaultNode() {
    return calloc( 1, sizeof(ARBTreeNode) );
}

ARBTreeNode* rbtree_makeColoredNode(const ARBTreeNodeColor color) {
    ARBTreeNode* node = rbtree_makeDefaultNode();
    node->color = color;
    return node;
}

size_t rbtree_nodeIndex(const ARBTreeNode* node) {
    if (node == NULL) {
        return -1;
    }

    const size_t subIndex = rbtree_sizeSubtree(node->left);
    const ARBTreeNode* leftNode = rbtree_getLeftAncestor(node);
    if (leftNode == NULL) {
        return subIndex;
    }
    return rbtree_nodeIndex(leftNode) + 1 + subIndex;
}

