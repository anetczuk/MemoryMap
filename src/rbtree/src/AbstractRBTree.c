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

#include "../include/rbtree/AbstractRBTree.h"

#include <stdlib.h>                     /// free
#include <assert.h>
#include <string.h>



int rbtree_init(RBTree* tree) {
    if (tree == NULL) {
        return -1;
    }

    tree->root = NULL;
    return 0;
}

static const RBTreeNode* rbtree_getLeftmostNode(const RBTreeNode* node) {
    if (node == NULL) {
        return NULL;
    }
    const RBTreeNode* curr = node;
    while (curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}

static const RBTreeNode* rbtree_getRightmostNode(const RBTreeNode* node) {
    if (node == NULL) {
        return NULL;
    }
    const RBTreeNode* curr = node;
    while (curr->right != NULL) {
        curr = curr->right;
    }
    return curr;
}

static const RBTreeNode* rbtree_getLeftDescendant(const RBTreeNode* node) {
    return rbtree_getLeftmostNode(node->right);
}

static const RBTreeNode* rbtree_getRightDescendant(const RBTreeNode* node) {
    return rbtree_getRightmostNode(node->left);
}

/**
 * Returns ancestor on right side of current node.
 */
static const RBTreeNode* rbtree_getRightAncestor(const RBTreeNode* node) {
    const RBTreeNode* child = node;
    const RBTreeNode* curr = node->parent;
    while( curr != NULL ) {
        if (curr->left == child) {
            return curr;
        }
        child = curr;
        curr = curr->parent;
    }
    /// root found
    return NULL;
}

/**
 * Returns ancestor on left side of current node.
 */
static const RBTreeNode* rbtree_getLeftAncestor(const RBTreeNode* node) {
    const RBTreeNode* child = node;
    const RBTreeNode* curr = node->parent;
    while( curr != NULL ) {
        if (curr->right == child) {
            return curr;
        }
        child = curr;
        curr = curr->parent;
    }
    /// root found
    return NULL;
}


/// ===================================================


static size_t rbtree_sizeSubtree(const RBTreeNode* tree) {
    if (tree==NULL) {
        return 0;
    }

    const size_t leftSize = rbtree_sizeSubtree(tree->left);
    const size_t rightSize = rbtree_sizeSubtree(tree->right);
    return leftSize + rightSize + 1;
}

size_t rbtree_size(const RBTree* tree) {
    if (tree==NULL)
        return 0;
    return rbtree_sizeSubtree(tree->root);
}

static size_t rbtree_depthSubtree(const RBTreeNode* tree) {
	if (tree == NULL) {
		return 0;
	}
	const size_t dLeft = rbtree_depthSubtree(tree->left);
	const size_t dRight = rbtree_depthSubtree(tree->right);
	if (dLeft>dRight) {
		return dLeft+1;
	} else {
		return dRight+1;
	}
}

size_t rbtree_depth(const RBTree* tree) {
    if (tree==NULL)
        return 0;
    return rbtree_depthSubtree(tree->root);
}


/// ==================================================================================


static RBTreeValidationError rbtree_isValid_checkPointers(const RBTreeNode* node, const RBTreeNode* parent) {
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

    const RBTreeValidationError validLeft = rbtree_isValid_checkPointers(node->left, node);
    if (validLeft != ARBTREE_INVALID_OK) {
        return validLeft;
    }
    const RBTreeValidationError validRight = rbtree_isValid_checkPointers(node->right, node);
    if (validRight != ARBTREE_INVALID_OK) {
        return validRight;
    }

    return ARBTREE_INVALID_OK;
}

static RBTreeValidationError rbtree_isValid_checkValues(const RBTreeNode* node) {
    if (node == NULL) {
        return ARBTREE_INVALID_OK;
    }
    if ( rbtree_isValidValue( node->value ) == false ) {
        /// invalid memory segment
        return ARBTREE_INVALID_BAD_VALUE;
    }

    const RBTreeValidationError validLeft = rbtree_isValid_checkValues(node->left);
    if (validLeft != ARBTREE_INVALID_OK) {
        return validLeft;
    }
    const RBTreeValidationError validRight = rbtree_isValid_checkValues(node->right);
    if (validRight != ARBTREE_INVALID_OK) {
        return validRight;
    }

    return ARBTREE_INVALID_OK;
}

static RBTreeValidationError rbtree_isValid_checkSorted(const RBTreeNode* node) {
    if (node == NULL) {
        return ARBTREE_INVALID_OK;
    }

    const RBTreeNode* prevTop = rbtree_getLeftAncestor(node);
    if (prevTop != NULL) {
        if (rbtree_isValid_isOrder(prevTop, node) == false) {
        	return ARBTREE_INVALID_NOT_SORTED;
        }
    }
    const RBTreeNode* nextTop = rbtree_getRightAncestor(node);
    if (nextTop != NULL) {
        if (rbtree_isValid_isOrder(node, nextTop) == false) {
        	return ARBTREE_INVALID_NOT_SORTED;
        }
    }

    const RBTreeNode* prevBottom = rbtree_getRightDescendant(node);
    if (prevBottom != NULL) {
        if (rbtree_isValid_isOrder(prevBottom, node) == false) {
        	return ARBTREE_INVALID_NOT_SORTED;
        }
    }
    const RBTreeNode* nextBottom = rbtree_getLeftDescendant(node);
    if (nextBottom != NULL) {
        if (rbtree_isValid_isOrder(node, nextBottom) == false) {
        	return ARBTREE_INVALID_NOT_SORTED;
        }
    }

    const RBTreeValidationError validLeft = rbtree_isValid_checkSorted(node->left);
    if (validLeft != ARBTREE_INVALID_OK) {
        return validLeft;
    }
    const RBTreeValidationError validRight = rbtree_isValid_checkSorted(node->right);
    if (validRight != ARBTREE_INVALID_OK) {
        return validRight;
    }

    return ARBTREE_INVALID_OK;
}

static RBTreeValidationError rbtree_isValid_countBlackPaths(const RBTreeNode* node, size_t* counter) {
    if (node == NULL) {
        *counter = 0;
        return ARBTREE_INVALID_OK;
    }
    size_t leftCounter = 0;
    const RBTreeValidationError validLeft = rbtree_isValid_countBlackPaths(node->left, &leftCounter);
    if (validLeft != ARBTREE_INVALID_OK) {
        return validLeft;
    }
    size_t rightCounter = 0;
    const RBTreeValidationError validRight = rbtree_isValid_countBlackPaths(node->right, &rightCounter);
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
static RBTreeValidationError rbtree_isValid_checkBlackPath(const RBTreeNode* node) {
    if (node == NULL) {
        return ARBTREE_INVALID_OK;
    }
    size_t counter = 0;
    return rbtree_isValid_countBlackPaths(node, &counter);
}

static RBTreeValidationError rbtree_isValid_checkColor(const RBTreeNode* node) {
	if (node->left != NULL) {
		const RBTreeValidationError validLeft = rbtree_isValid_checkColor(node->left);
		if (validLeft != ARBTREE_INVALID_OK) {
			return validLeft;
		}
    }
	if (node->right != NULL) {
		const RBTreeValidationError validRight = rbtree_isValid_checkColor(node->right);
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

RBTreeValidationError rbtree_isValid(const RBTree* tree) {
    if (tree==NULL)
        return ARBTREE_INVALID_OK;
    if (tree->root == NULL)
        return ARBTREE_INVALID_OK;
    if (tree->root->parent != NULL)
        return ARBTREE_INVALID_ROOT_PARENT;

    const RBTreeNode* rootNode = tree->root;

    /// check pointers
    const RBTreeValidationError validPointers = rbtree_isValid_checkPointers(rootNode, rootNode->parent);
    if (validPointers != ARBTREE_INVALID_OK) {
        return validPointers;
    }
    /// if pointers are valid, then there is no cycles

    /// check memory segments
    const RBTreeValidationError validMemory = rbtree_isValid_checkValues(rootNode);
    if (validMemory != ARBTREE_INVALID_OK) {
        return validMemory;
    }

    /// check is sorted
    const RBTreeValidationError validOrder = rbtree_isValid_checkSorted(rootNode);
    if (validOrder != ARBTREE_INVALID_OK) {
        return validOrder;
    }

    /// checking red-black properties
    /// root is black
    if (rootNode->color != ARBTREE_COLOR_BLACK)
        return ARBTREE_INVALID_RED_ROOT;

    /// if a node is red, then both its children are black
    const RBTreeValidationError validColor = rbtree_isValid_checkColor(rootNode);
    if (validColor != ARBTREE_INVALID_OK) {
        return validColor;
    }

    const RBTreeValidationError validPath = rbtree_isValid_checkBlackPath(rootNode);
    if (validPath != ARBTREE_INVALID_OK) {
        return validPath;
    }

    return ARBTREE_INVALID_OK;
}


/// ==================================================================================


RBTreeNode* rbtree_findNode(const RBTree* tree, const RBTreeValue value) {
    if (tree == NULL) {
        return NULL;
    }
    RBTreeNode* curr = tree->root;
    while (curr != NULL) {
        if ( rbtree_checkOrder(value, curr->value) == true ) {
            /// value < curr->value
            curr = curr->left;
            continue ;
        }
        if ( rbtree_checkOrder(curr->value, value) == true ) {
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


/// ==================================================================================


static RBTreeNode* rbtree_grandparent(RBTreeNode* node) {
	if (node->parent == NULL) {
		return NULL;
	}
	return node->parent->parent;
}

static RBTreeNode* rbtree_sibling(RBTreeNode* node) {
	if (node->parent == NULL) {
		return NULL;
	}
	if (node->parent->left == node)
		return node->parent->right;
	else
		return node->parent->left;
}

static inline RBTreeNode* rbtree_uncle(RBTreeNode* node) {
	return rbtree_sibling(node->parent);
}

static inline void rbtree_setLeftChild(RBTreeNode* node, RBTreeNode* child) {
    node->left = child;
    if (child != NULL) {
        child->parent = node;
    }
}

static inline void rbtree_setRightChild(RBTreeNode* node, RBTreeNode* child) {
    node->right = child;
    if (child != NULL) {
        child->parent = node;
    }
}

static inline void rbtree_changeChild(RBTreeNode* node, RBTreeNode* from, RBTreeNode* to) {
    if (node->left == from) {
        rbtree_setLeftChild(node, to);
    } else {
        rbtree_setRightChild(node, to);
    }
}

static void rbtree_rotate_left(RBTreeNode* node) {
    RBTreeNode* parent = node->parent;
    RBTreeNode* nnew = node->right;
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

static void rbtree_rotate_right(RBTreeNode* node) {
    RBTreeNode* parent = node->parent;
    RBTreeNode* nnew = node->left;
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

static void rbtree_repair_insert(RBTreeNode* node) {
	RBTreeNode* nParent = node->parent;
	if ( nParent == NULL) {
		node->color = ARBTREE_COLOR_BLACK;
		return ;
	}
	if (nParent->color == ARBTREE_COLOR_BLACK) {
		/// do nothing
		return ;
	}
	RBTreeNode* uncle = rbtree_uncle(node);
	if (uncle != NULL) {
        if (uncle->color == ARBTREE_COLOR_RED) {
            nParent->color = ARBTREE_COLOR_BLACK;
            uncle->color = ARBTREE_COLOR_BLACK;
            RBTreeNode* grandpa = rbtree_grandparent(node);		/// never NULL here
            grandpa->color = ARBTREE_COLOR_RED;
            rbtree_repair_insert(grandpa);
            return ;
        }
	}

    RBTreeNode* curr = node;
	{
        RBTreeNode* grandpa = rbtree_grandparent(curr);		/// never NULL here
        if ((grandpa->left != NULL) && (curr == grandpa->left->right)) {
            rbtree_rotate_left(curr->parent);
            curr = curr->left;
        } else if ((grandpa->right != NULL) && (curr == grandpa->right->left)) {
            rbtree_rotate_right(curr->parent);
            curr = curr->right;
        }
	}
	{
	    RBTreeNode* grandpa = rbtree_grandparent(curr);       /// never NULL here
        if (curr == curr->parent->left)
            rbtree_rotate_right(grandpa);
        else
            rbtree_rotate_left(grandpa);
        curr->parent->color = ARBTREE_COLOR_BLACK;
        grandpa->color = ARBTREE_COLOR_RED;
	}
}

static RBTreeNode* rbtree_insertLeftNode(RBTreeNode* node) {
	RBTreeNode* oldLeft = node->left;
	RBTreeNode* newNode = rbtree_makeColoredNode(ARBTREE_COLOR_RED);         /// default color of new node
	rbtree_setLeftChild(node, newNode);
	rbtree_setLeftChild(newNode, oldLeft);

	rbtree_repair_insert(newNode);
	return newNode;
}

static RBTreeNode* rbtree_insertRightNode(RBTreeNode* node) {
	RBTreeNode* oldLeft = node->right;
	RBTreeNode* newNode = rbtree_makeColoredNode(ARBTREE_COLOR_RED);         /// default color of new node
	rbtree_setRightChild(node, newNode);
	newNode->right = oldLeft;
	rbtree_setRightChild(newNode, oldLeft);

	rbtree_repair_insert(node->right);
	return newNode;
}

static bool rbtree_addToRight(RBTreeNode* node, RBTreeValue value);

/**
 * Adding to left side: value should be smaller than 'node->value'
 */
static bool rbtree_addToLeft(RBTreeNode* node, RBTreeValue value) {
    if ( rbtree_checkOrder(value, node->value) == false ) {
        /// could not add on left side -- invalid order
    	return false;
    }

    if ( node->left == NULL ) {
    	/// leaf case -- can add
        RBTreeNode* newNode = rbtree_insertLeftNode(node);
        newNode->value = value;
		return true;
    }

	if (rbtree_addToLeft(node->left, value) == true) {
		return true;
	}

	return rbtree_addToRight(node->left, value);
}

/**
 * Adding to right side: value should be greater than 'node->value'
 */
static bool rbtree_addToRight(RBTreeNode* node, RBTreeValue value) {
    if ( rbtree_checkOrder(value, node->value) == true ) {
        /// could not add on right side -- invalid order
        return false;
    }

    if ( node->right == NULL ) {
        /// leaf case -- can add
        RBTreeNode* newNode = rbtree_insertRightNode(node);
        newNode->value = value;
        return true;
    }

    if (rbtree_addToLeft(node->right, value) == true) {
        return true;
    }

    return rbtree_addToRight(node->right, value);
}

static const RBTreeNode* rbtree_findRootFromNode(const RBTreeNode* node) {
    /// find the new root to return
    const RBTreeNode* curr = node;
    while (curr->parent != NULL)
        curr = curr->parent;
    return curr;
}

static void rbtree_findRoot(RBTree* tree) {
    /// find the new root to return
    tree->root = (RBTreeNode*) rbtree_findRootFromNode(tree->root);
}

static int rbtree_addValue(RBTree* tree, const RBTreeValue value) {
    assert( tree != NULL );
    if (rbtree_isValidValue(value) == false) {
        return -1;
    }

    if (tree->root == NULL) {
        tree->root = calloc( 1, sizeof(RBTreeNode) );
        ///tree->root->parent = NULL;
        ///tree->root->color = RBTREE_BLACK;
        tree->root->value = value;
        rbtree_repair_insert(tree->root);
        return 0;
    }

	bool addResult = rbtree_addToLeft(tree->root, value);
	if (addResult==true) {
	    rbtree_findRoot(tree);
	    return addResult;
	}
    /// go to right, called on root should always work
    addResult = rbtree_addToRight(tree->root, value);
    if (addResult==true) {
        rbtree_findRoot(tree);
    }
    return addResult;
}

int rbtree_add(RBTree* tree, const RBTreeValue value) {
    if (tree == NULL) {
        return -1;
    }
    return rbtree_addValue(tree, value);
}


/// ==============================================================================================


static void rbtree_printLevel(const RBTreeNode* node, const size_t level, int* previousNodeIndex) {
    if (node == NULL) {
        return ;
    }

    if (level > 0) {
        rbtree_printLevel(node->left, level-1, previousNodeIndex);
        rbtree_printLevel(node->right, level-1, previousNodeIndex);
        return ;
    }

    const size_t pos = rbtree_nodeIndex(node);
    const size_t diff = (pos==0) ? 0 : (pos - *previousNodeIndex) - 1;

    /// print empty space on left
    for(size_t i=0; i<diff; ++i) {
        printf("          ");
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
    rbtree_printValue(node->value);
    printf(")");

    *previousNodeIndex = pos;
}

static void rbtree_printSubtree(const RBTreeNode* node) {
    if (node == NULL) {
        printf("%s", "(NULL)");
        return ;
    }
    const size_t depth = rbtree_depthSubtree(node);
    for(size_t l = 0; l < depth; ++l) {
        int previousNodeIndex = -1;
        rbtree_printLevel(node, l, &previousNodeIndex);
        printf("%s", "\n");
    }
}

static inline void rbtree_printWhole(const RBTreeNode* node) {
    const RBTreeNode* root = rbtree_findRootFromNode(node);
    rbtree_printSubtree( root );
}

void rbtree_print(const RBTree* tree) {
    if (tree == NULL) {
        printf("%s", "[NULL]");
        return ;
    }
    rbtree_printWhole(tree->root);
}


/// ==============================================================================================


static int rbtree_releaseNodes(RBTreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    /**
     * Done in recursive manner. In case of very large structures consider
     * reimplementing it using while() and vector structure.
     */
    const int leftReleased = rbtree_releaseNodes(node->left);
    const int rightReleased = rbtree_releaseNodes(node->right);
    free(node);
    return leftReleased+rightReleased+1;
}

int rbtree_release(RBTree* tree) {
    if (tree==NULL) {
        return -1;
    }
    const int ret = rbtree_releaseNodes(tree->root);
    tree->root = NULL;
    return ret;
}


/// =========================================================================


static RBTreeNode* rbtree_repair_sibling(RBTreeNode* parent, RBTreeNode* node) {
    if (parent == NULL) {
        return NULL;
    }
    if (parent->left == node)
        return parent->right;
    else
        return parent->left;
}

static int rbtree_repair_isChildrenColors(const RBTreeNode* parent, const NodeColor leftChild, const NodeColor rightChild) {
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

static void rbtree_repair_case1(RBTreeNode* parent, RBTreeNode* node);

static void rbtree_repair_case6(RBTreeNode* parent, RBTreeNode* node) {
    RBTreeNode* sibling = rbtree_repair_sibling(parent, node);
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

static void rbtree_repair_case5(RBTreeNode* parent, RBTreeNode* node) {
    RBTreeNode* sibling = rbtree_repair_sibling(parent, node);
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

static void rbtree_repair_case4(RBTreeNode* parent, RBTreeNode* node) {
    if (parent->color != ARBTREE_COLOR_RED) {
        rbtree_repair_case5(parent, node);
        return ;
    }
    RBTreeNode* sibling = rbtree_repair_sibling(parent, node);
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

static void rbtree_repair_case3(RBTreeNode* parent, RBTreeNode* node) {
    if (parent->color != ARBTREE_COLOR_BLACK) {
        rbtree_repair_case4(parent, node);
        return ;
    }
    RBTreeNode* sibling = rbtree_repair_sibling(parent, node);
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

static void rbtree_repair_case2(RBTreeNode* parent, RBTreeNode* node) {
    RBTreeNode* sibling = rbtree_repair_sibling(parent, node);
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

static void rbtree_repair_case1(RBTreeNode* parent, RBTreeNode* node) {
    if (parent == NULL) {
        return ;
    }
    rbtree_repair_case2(parent, node);
}

static void rbtree_repair_delete(RBTreeNode* parent, RBTreeNode* node) {
    if (node != NULL) {
        if (node->color == ARBTREE_COLOR_RED) {
            node->color = ARBTREE_COLOR_BLACK;
            return ;
        }
    }

    /// restore -- cases
    rbtree_repair_case1(parent, node);
}

bool rbtree_delete(RBTree* tree, const RBTreeValue value) {
	(void) tree; /* unused */
	(void) value; /* unused */

    RBTreeNode* node = rbtree_findNode( tree, value );
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

        free(node);
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

        free(node);
        return true;
    }

    /// have both children
    /// there is right subtree

    RBTreeNode* nextNode = (RBTreeNode*) rbtree_getLeftDescendant(node);      /// never NULL
    node->value = nextNode->value;

    rbtree_changeChild(nextNode->parent, nextNode, nextNode->right);
    if (nextNode->color == ARBTREE_COLOR_BLACK) {
        rbtree_repair_delete(nextNode->parent, nextNode->right);
        /// can happpen than root changes due to rotations
        rbtree_findRoot(tree);
    }

    free(nextNode);
    return true;
}


/// =========================================================


RBTreeNode* rbtree_makeDefaultNode() {
    return calloc( 1, sizeof(RBTreeNode) );
}

RBTreeNode* rbtree_makeColoredNode(const NodeColor color) {
    RBTreeNode* node = rbtree_makeDefaultNode();
    node->color = color;
    return node;
}

size_t rbtree_nodeIndex(const RBTreeNode* node) {
    if (node == NULL) {
        return -1;
    }

    const size_t subIndex = rbtree_sizeSubtree(node->left);
    const RBTreeNode* leftNode = rbtree_getLeftAncestor(node);
    if (leftNode == NULL) {
        return subIndex;
    }
    return rbtree_nodeIndex(leftNode) + 1 + subIndex;
}

