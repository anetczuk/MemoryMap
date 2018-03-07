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

#include "memorymap/RBTree.h"

#include <stdlib.h>                     /// free
#include <assert.h>
#include <stdio.h>                      /// printf
#include <string.h>



static const RBTreeNode* tree_getLeftmostNode(const RBTreeNode* node) {
    if (node == NULL) {
        return NULL;
    }
    const RBTreeNode* curr = node;
    while (curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}

static const RBTreeNode* tree_getRightmostNode(const RBTreeNode* node) {
    if (node == NULL) {
        return NULL;
    }
    const RBTreeNode* curr = node;
    while (curr->right != NULL) {
        curr = curr->right;
    }
    return curr;
}

static const RBTreeNode* tree_getLeftDescendant(const RBTreeNode* node) {
    return tree_getLeftmostNode(node->right);
}

static const RBTreeNode* tree_getRightDescendant(const RBTreeNode* node) {
    return tree_getRightmostNode(node->left);
}

/**
 * Returns ancestor on right side of current node.
 */
static const RBTreeNode* tree_getRightAncestor(const RBTreeNode* node) {
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
static const RBTreeNode* tree_getLeftAncestor(const RBTreeNode* node) {
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


static size_t tree_sizeSubtree(const RBTreeNode* tree) {
    if (tree==NULL) {
        return 0;
    }

    const size_t leftSize = tree_sizeSubtree(tree->left);
    const size_t rightSize = tree_sizeSubtree(tree->right);
    return leftSize + rightSize + 1;
}

size_t tree_size(const RBTree* tree) {
    if (tree==NULL)
        return 0;
    return tree_sizeSubtree(tree->root);
}

static size_t tree_depthSubtree(const RBTreeNode* tree) {
	if (tree == NULL) {
		return 0;
	}
	const size_t dLeft = tree_depthSubtree(tree->left);
	const size_t dRight = tree_depthSubtree(tree->right);
	if (dLeft>dRight) {
		return dLeft+1;
	} else {
		return dRight+1;
	}
}

size_t tree_depth(const RBTree* tree) {
    if (tree==NULL)
        return 0;
    return tree_depthSubtree(tree->root);
}

size_t tree_startAddress(const RBTree* tree) {
    if (tree==NULL)
        return 0;
    if (tree->root==NULL)
        return 0;

    const RBTreeNode* node = tree_getLeftmostNode(tree->root);
    return node->area.start;
}

size_t tree_endAddress(const RBTree* tree) {
    if (tree==NULL)
        return 0;
    if (tree->root==NULL)
        return 0;

    const RBTreeNode* node = tree_getRightmostNode(tree->root);
    return node->area.end;
}


/// ==================================================================================


static RBTreeValidationError tree_isValid_checkPointers(const RBTreeNode* node, const RBTreeNode* parent) {
    if (node == NULL) {
        return RBTREE_INVALID_OK;
    }
    if (node->parent != parent) {
        return RBTREE_INVALID_NODE_PARENT;
    }
    if ((node->left == node->right) && (node->right != NULL)) {
        /// the same child
        return RBTREE_INVALID_SAME_CHILD;
    }

    const RBTreeValidationError validLeft = tree_isValid_checkPointers(node->left, node);
    if (validLeft != RBTREE_INVALID_OK) {
        return validLeft;
    }
    const RBTreeValidationError validRight = tree_isValid_checkPointers(node->right, node);
    if (validRight != RBTREE_INVALID_OK) {
        return validRight;
    }

    return RBTREE_INVALID_OK;
}

static RBTreeValidationError tree_isValid_checkMemory(const RBTreeNode* node) {
    if (node == NULL) {
        return RBTREE_INVALID_OK;
    }
    const int validMem = memory_isValid( &(node->area) );
    if ( validMem != 0) {
        /// invalid memory segment
        return RBTREE_INVALID_BAD_MEMORY_SEGMENT;
    }

    const RBTreeValidationError validLeft = tree_isValid_checkMemory(node->left);
    if (validLeft != RBTREE_INVALID_OK) {
        return validLeft;
    }
    const RBTreeValidationError validRight = tree_isValid_checkMemory(node->right);
    if (validRight != RBTREE_INVALID_OK) {
        return validRight;
    }

    return RBTREE_INVALID_OK;
}

static RBTreeValidationError tree_isValid_checkSorted(const RBTreeNode* node) {
    if (node == NULL) {
        return RBTREE_INVALID_OK;
    }

    const RBTreeNode* prevTop = tree_getLeftAncestor(node);
    if (prevTop != NULL) {
        if ( prevTop->area.end > node->area.start) {
            return RBTREE_INVALID_NOT_SORTED;
        }
    }
    const RBTreeNode* nextTop = tree_getRightAncestor(node);
    if (nextTop != NULL) {
        if ( nextTop->area.start < node->area.end) {
            return RBTREE_INVALID_NOT_SORTED;
        }
    }

    const RBTreeNode* prevBottom = tree_getRightDescendant(node);
    if (prevBottom != NULL) {
        if ( prevBottom->area.end > node->area.start) {
            return RBTREE_INVALID_NOT_SORTED;
        }
    }
    const RBTreeNode* nextBottom = tree_getLeftDescendant(node);
    if (nextBottom != NULL) {
        if ( nextBottom->area.start < node->area.end) {
            return RBTREE_INVALID_NOT_SORTED;
        }
    }

    const RBTreeValidationError validLeft = tree_isValid_checkSorted(node->left);
    if (validLeft != RBTREE_INVALID_OK) {
        return validLeft;
    }
    const RBTreeValidationError validRight = tree_isValid_checkSorted(node->right);
    if (validRight != RBTREE_INVALID_OK) {
        return validRight;
    }

    return RBTREE_INVALID_OK;
}

static RBTreeValidationError tree_isValid_countBlackPaths(const RBTreeNode* node, size_t* counter) {
    if (node == NULL) {
        *counter = 0;
        return RBTREE_INVALID_OK;
    }
    size_t leftCounter = 0;
    const RBTreeValidationError validLeft = tree_isValid_countBlackPaths(node->left, &leftCounter);
    if (validLeft != RBTREE_INVALID_OK) {
        return validLeft;
    }
    size_t rightCounter = 0;
    const RBTreeValidationError validRight = tree_isValid_countBlackPaths(node->right, &rightCounter);
    if (validRight != RBTREE_INVALID_OK) {
        return validRight;
    }

    if (leftCounter != rightCounter) {
        return RBTREE_INVALID_BLACK_PATH;
    }

    if (node->color == RBTREE_COLOR_BLACK) {
        *counter = leftCounter+1;
    } else {
        *counter = leftCounter;
    }

    return RBTREE_INVALID_OK;
}

/**
 * Every path from a given node to any of its descendant NULL nodes
 * contains the same number of black nodes.
 */
static RBTreeValidationError tree_isValid_checkBlackPath(const RBTreeNode* node) {
    if (node == NULL) {
        return RBTREE_INVALID_OK;
    }
    size_t counter = 0;
    return tree_isValid_countBlackPaths(node, &counter);
}

static RBTreeValidationError tree_isValidSubTree(const RBTreeNode* node) {
    if (node==NULL)
        return RBTREE_INVALID_OK;

    /// check pointers
    const RBTreeValidationError validPointers = tree_isValid_checkPointers(node, node->parent);
    if (validPointers != RBTREE_INVALID_OK) {
        return validPointers;
    }
    /// if pointers are valid, then there is no cycles

    /// check memory segments
    const RBTreeValidationError validMemory = tree_isValid_checkMemory(node);
    if (validMemory != RBTREE_INVALID_OK) {
        return validMemory;
    }

    /// check is sorted
    const RBTreeValidationError validOrder = tree_isValid_checkSorted(node);
    if (validOrder != RBTREE_INVALID_OK) {
        return validOrder;
    }

    /// checking red-black properties
    /// root is black
    if (node->parent == NULL) {
        /// root
        if (node->color != RBTREE_COLOR_BLACK)
            return RBTREE_INVALID_RED_ROOT;
    }

    /// if a node is red, then both its children are black
    if (node->color == RBTREE_COLOR_RED) {
        if (node->left != NULL) {
            if (node->left->color != RBTREE_COLOR_BLACK) {
                return RBTREE_INVALID_BLACK_CHILDREN;
            }
        }
        if (node->right != NULL) {
            if (node->right->color != RBTREE_COLOR_BLACK) {
                return RBTREE_INVALID_BLACK_CHILDREN;
            }
        }
    }

    const RBTreeValidationError validPath = tree_isValid_checkBlackPath(node);
    if (validPath != RBTREE_INVALID_OK) {
        return validPath;
    }

    return RBTREE_INVALID_OK;
}

RBTreeValidationError tree_isValid(const RBTree* tree) {
    if (tree==NULL)
        return RBTREE_INVALID_OK;
    if (tree->root == NULL)
        return RBTREE_INVALID_OK;
    if (tree->root->parent != NULL)
        return RBTREE_INVALID_ROOT_PARENT;
    return tree_isValidSubTree( tree->root );
}


/// ==================================================================================


static RBTreeNode* tree_grandparent(RBTreeNode* node) {
	if (node->parent == NULL) {
		return NULL;
	}
	return node->parent->parent;
}

static RBTreeNode* tree_sibling(RBTreeNode* node) {
	if (node->parent == NULL) {
		return NULL;
	}
	if (node->parent->left == node)
		return node->parent->right;
	else
		return node->parent->left;
}

static inline RBTreeNode* tree_uncle(RBTreeNode* node) {
	return tree_sibling(node->parent);
}

static inline void tree_setLeftChild(RBTreeNode* node, RBTreeNode* child) {
    node->left = child;
    if (child != NULL) {
        child->parent = node;
    }
}

static inline void tree_setRightChild(RBTreeNode* node, RBTreeNode* child) {
    node->right = child;
    if (child != NULL) {
        child->parent = node;
    }
}

static inline void tree_changeChild(RBTreeNode* node, RBTreeNode* from, RBTreeNode* to) {
    if (node->left == from) {
        tree_setLeftChild(node, to);
    } else {
        tree_setRightChild(node, to);
    }
}

static void tree_rotate_left(RBTreeNode* node) {
    RBTreeNode* parent = node->parent;
    RBTreeNode* nnew = node->right;
    assert(nnew != NULL);                   /// since the leaves of a red-black tree are empty, they cannot become internal nodes
    tree_setRightChild(node, nnew->left);
    tree_setLeftChild(nnew, node);
    if (parent == NULL) {
        nnew->parent = NULL;
        return;
    }
    if (parent->left == node) {
        tree_setLeftChild(parent, nnew);
    } else {
        tree_setRightChild(parent, nnew);
    }
}

static void tree_rotate_right(RBTreeNode* node) {
    RBTreeNode* parent = node->parent;
    RBTreeNode* nnew = node->left;
    assert(nnew != NULL);                   /// since the leaves of a red-black tree are empty, they cannot become internal nodes
    tree_setLeftChild(node, nnew->right);
    tree_setRightChild(nnew, node);
    if (parent == NULL) {
        nnew->parent = NULL;
        return;
    }
    if (parent->left == node) {
        tree_setLeftChild(parent, nnew);
    } else {
        tree_setRightChild(parent, nnew);
    }
}

static void tree_repair_insert(RBTreeNode* node) {
	RBTreeNode* nParent = node->parent;
	if ( nParent == NULL) {
		node->color = RBTREE_COLOR_BLACK;
		return ;
	}
	if (nParent->color == RBTREE_COLOR_BLACK) {
		/// do nothing
		return ;
	}
	RBTreeNode* uncle = tree_uncle(node);
	if (uncle != NULL) {
        if (uncle->color == RBTREE_COLOR_RED) {
            nParent->color = RBTREE_COLOR_BLACK;
            uncle->color = RBTREE_COLOR_BLACK;
            RBTreeNode* grandpa = tree_grandparent(node);		/// never NULL here
            grandpa->color = RBTREE_COLOR_RED;
            tree_repair_insert(grandpa);
            return ;
        }
	}

    RBTreeNode* curr = node;
	{
        RBTreeNode* grandpa = tree_grandparent(curr);		/// never NULL here
        if ((grandpa->left != NULL) && (curr == grandpa->left->right)) {
            tree_rotate_left(curr->parent);
            curr = curr->left;
        } else if ((grandpa->right != NULL) && (curr == grandpa->right->left)) {
            tree_rotate_right(curr->parent);
            curr = curr->right;
        }
	}
	{
	    RBTreeNode* grandpa = tree_grandparent(curr);       /// never NULL here
        if (curr == curr->parent->left)
            tree_rotate_right(grandpa);
        else
            tree_rotate_left(grandpa);
        curr->parent->color = RBTREE_COLOR_BLACK;
        grandpa->color = RBTREE_COLOR_RED;
	}
}

static RBTreeNode* tree_insertLeftNode(RBTreeNode* node) {
	RBTreeNode* oldLeft = node->left;
	RBTreeNode* newNode = node_makeColored(RBTREE_COLOR_RED);         /// default color of new node
	tree_setLeftChild(node, newNode);
	tree_setLeftChild(newNode, oldLeft);

	tree_repair_insert(newNode);
	return newNode;
}

static RBTreeNode* tree_insertRightNode(RBTreeNode* node) {
	RBTreeNode* oldLeft = node->right;
	RBTreeNode* newNode = node_makeColored(RBTREE_COLOR_RED);         /// default color of new node
	tree_setRightChild(node, newNode);
	newNode->right = oldLeft;
	tree_setRightChild(newNode, oldLeft);

	tree_repair_insert(node->right);
	return newNode;
}

static void* tree_addMemoryToRight(RBTreeNode* node, MemoryArea* area);

static void* tree_addMemoryToLeft(RBTreeNode* node, MemoryArea* area) {
    if (area->end > node->area.start) {
        /// could not add on left side
    	return NULL;
    }

	const RBTreeNode* leftNode = NULL;
    if ( node->left == NULL ) {
    	const RBTreeNode* leftAncestor = tree_getLeftAncestor(node);
    	if (leftAncestor == NULL) {
    		/// smallest leaf case -- add node
    	    RBTreeNode* newNode = tree_insertLeftNode(node);
    	    newNode->area = *area;
			return (void*)newNode->area.start;
    	}
    	leftNode = leftAncestor;
    } else {
        void* ret = tree_addMemoryToLeft(node->left, area);
        if (ret != NULL) {
            return ret;
        }
        leftNode = node->left;
    }

    /// check free space between
    const size_t minStart = memory_startAddress(&(leftNode->area), area );
    const size_t spaceBetween = node->area.start - minStart;
    const size_t areaSize = memory_size( area );
    if (spaceBetween < areaSize) {
    	/// could not add to left side
    	return NULL;
    }

    /// fits between -- check if there are nodes between

    if ( node->left == NULL ) {
    	/// leaf case -- can add, no more between
        RBTreeNode* newNode = tree_insertLeftNode(node);
        newNode->area = memory_create( minStart, areaSize );
		return (void*)newNode->area.start;
    }

	return tree_addMemoryToRight(node->left, area);
}

static void* tree_addMemoryToRight(RBTreeNode* node, MemoryArea* area) {
    if ( node->right == NULL ) {
    	const RBTreeNode* rightAncestor = tree_getRightAncestor(node);
    	if (rightAncestor == NULL) {
    		/// greatest leaf case -- add node
    		memory_fitAfter(&(node->area), area);
    		RBTreeNode* newNode = tree_insertRightNode(node);
    		newNode->area = *area;
			return (void*)newNode->area.start;
    	}
		/// leaf case -- check space
		const int doesFit = memory_fitBetween(&(node->area), &(rightAncestor->area), area);
		if (doesFit == 0) {
		    RBTreeNode* newNode = tree_insertRightNode(node);
		    newNode->area = *area;
			return (void*)newNode->area.start;
		}
		/// no space -- return
		return NULL;
    }

    const MemoryArea* rightArea = &(node->right->area);
    if (area->start > rightArea->start) {
        return tree_addMemoryToRight(node->right, area);
    }

    const size_t minStart = memory_startAddress(&(node->area), area );
    const size_t spaceBetween = rightArea->start - minStart;
	const size_t areaSize = memory_size( area );
	if (areaSize<=spaceBetween) {
	    void* retAddr = tree_addMemoryToLeft(node->right, area);
	    if (retAddr != NULL) {
	        return retAddr;
	    }
	}
    /// no space -- go to next node
    return tree_addMemoryToRight(node->right, area);
}

static void tree_findRoot(RBTree* tree) {
    /// find the new root to return
    RBTreeNode* node = tree->root;
    while (node->parent != NULL)
        node = node->parent;
    tree->root = node;
}

static void* tree_addMemory(RBTree* tree, MemoryArea* area) {
    assert( tree != NULL );
    assert( area != NULL );
    if (area->start == 0) {
        return NULL;
    }

    if (tree->root == NULL) {
        tree->root = calloc( 1, sizeof(RBTreeNode) );
        ///tree->root->parent = NULL;
        ///tree->root->color = RBTREE_BLACK;
        tree->root->area = *area;
        tree_repair_insert(tree->root);
        return (void*)tree->root->area.start;
    }

	void* newMemoryAddress = tree_addMemoryToLeft(tree->root, area);
	if (newMemoryAddress==NULL) {
	    /// ends inside or after current area
	    /// go to right, called on root should always work
	    newMemoryAddress = tree_addMemoryToRight(tree->root, area);
	}
    tree_findRoot(tree);
    return newMemoryAddress;
}

size_t tree_add(RBTree* tree, const size_t address, const size_t size) {
    if (tree == NULL) {
        return 0;
    }
    MemoryArea area = memory_create(address, size);
    void* retAddr = tree_addMemory(tree, &area);
    /// assert( tree_isValid(tree) == 0 );
    return (size_t)retAddr;
}

size_t node_index(const RBTreeNode* node) {
    if (node == NULL) {
        return -1;
    }

    const size_t subIndex = tree_sizeSubtree(node->left);
    const RBTreeNode* leftNode = tree_getLeftAncestor(node);
    if (leftNode == NULL) {
        return subIndex;
    }
    return node_index(leftNode) + 1 + subIndex;
}

static void tree_printLevel(const RBTreeNode* node, const size_t level, int* previousNodeIndex) {
    if (node == NULL) {
        return ;
    }

    if (level > 0) {
        tree_printLevel(node->left, level-1, previousNodeIndex);
        tree_printLevel(node->right, level-1, previousNodeIndex);
        return ;
    }

    const size_t pos = node_index(node);
    const size_t diff = (pos==0) ? 0 : (pos - *previousNodeIndex) - 1;

    /// print empty space on left
    for(size_t i=0; i<diff; ++i) {
        printf("           ");
    }

    switch(node->color) {
    case RBTREE_COLOR_BLACK: {
        printf("(B,%3lu,%3lu)", node->area.start, node->area.end);
        break;
    }
    case RBTREE_COLOR_RED: {
        printf("(R,%3lu,%3lu)", node->area.start, node->area.end);
        break;
    }
    }

    *previousNodeIndex = pos;
}

void tree_print(const RBTree* tree) {
    if (tree == NULL) {
        printf("%s", "[NULL]");
        return ;
    }
    if (tree->root == NULL) {
        printf("%s", "(NULL)");
        return ;
    }
    const size_t depth = tree_depth(tree);
    for(size_t l = 0; l < depth; ++l) {
        int previousNodeIndex = -1;
        tree_printLevel(tree->root, l, &previousNodeIndex);
        printf("%s", "\n");
    }
}

static int tree_releaseNodes(RBTreeNode* node) {
    if (node == NULL) {
        return 0;
    }
    /**
     * Done in recursive manner. In case of very large structures consider
     * reimplementing it using while() and vector structure.
     */
    const int leftReleased = tree_releaseNodes(node->left);
    const int rightReleased = tree_releaseNodes(node->right);
    free(node);
    return leftReleased+rightReleased+1;
}

int tree_release(RBTree* tree) {
    if (tree==NULL) {
        return -1;
    }
    const int ret = tree_releaseNodes(tree->root);
    tree->root = NULL;
    return ret;
}


/// ===================================================


void* tree_mmap(RBTree* tree, void *vaddr, unsigned int size) {
    if (tree == NULL) {
        return NULL;
    }

    MemoryArea area = memory_create( (size_t)vaddr, size );
    void* retAddr = tree_addMemory(tree, &area);
    assert( tree_isValid(tree) == RBTREE_INVALID_OK );
    return retAddr;
}

RBTreeNode* tree_findNode(const RBTree* tree, const size_t adress) {
    if (tree == NULL) {
        return NULL;
    }
	RBTreeNode* curr = tree->root;
	while (curr != NULL) {
		if (adress < curr->area.start) {
			curr = curr->left;
		} else if (adress < curr->area.end) {
			return curr;
		} else {
			curr = curr->right;
		}
	}
	/// not found
	return NULL;
}

//static void tree_recolor(RBTreeNode* node) {
//    if (node->color == RBTREE_BLACK) {
//        node->color = RBTREE_RED;
//    } else {
//        node->color = RBTREE_BLACK;
//    }
//}

static int tree_repair_childrenColors(const RBTreeNode* parent, const NodeColor leftChild, const NodeColor rightChild) {
    if ( parent->left->color != leftChild ) {
        return 1;
    }
    if ( parent->right->color != rightChild ) {
        return 1;
    }
    return 0;
}

static void tree_repair_case1(RBTreeNode* node);

static void tree_repair_case6(RBTreeNode* node) {
    RBTreeNode* sibling = tree_sibling(node);
    sibling->color = node->parent->color;
    node->parent->color = RBTREE_COLOR_BLACK;

    if (node->parent->left == node) {
        sibling->right->color = RBTREE_COLOR_BLACK;
        tree_rotate_left( node->parent );
    } else {
        sibling->left->color = RBTREE_COLOR_BLACK;
        tree_rotate_right( node->parent );
    }
}

static void tree_repair_case5(RBTreeNode* node) {
    RBTreeNode* sibling = tree_sibling(node);
    if (sibling->color != RBTREE_COLOR_BLACK) {
        tree_repair_case6(node);
        return ;
    }

    if (node->parent->left == node) {
        if (tree_repair_childrenColors(sibling, RBTREE_COLOR_RED, RBTREE_COLOR_BLACK) != 0) {
            sibling->color = RBTREE_COLOR_RED;
            sibling->left->color = RBTREE_COLOR_BLACK;
            tree_rotate_right( sibling );
            tree_repair_case6(node);
            return ;
        }
    }

    if (node->parent->right == node) {
        if (tree_repair_childrenColors(sibling, RBTREE_COLOR_BLACK, RBTREE_COLOR_RED) != 0) {
            sibling->color = RBTREE_COLOR_RED;
            sibling->right->color = RBTREE_COLOR_BLACK;
            tree_rotate_left( sibling );
            tree_repair_case6(node);
            return ;
        }
    }
}

static void tree_repair_case4(RBTreeNode* node) {
    if (node->parent->color != RBTREE_COLOR_RED) {
        tree_repair_case5(node);
        return ;
    }
    RBTreeNode* sibling = tree_sibling(node);
    if (sibling->color != RBTREE_COLOR_BLACK) {
        tree_repair_case5(node);
        return ;
    }
    if (tree_repair_childrenColors(sibling, RBTREE_COLOR_BLACK, RBTREE_COLOR_BLACK) != 0) {
        tree_repair_case5(node);
        return ;
    }
    sibling->color = RBTREE_COLOR_RED;
    node->parent->color = RBTREE_COLOR_BLACK;
}

static void tree_repair_case3(RBTreeNode* node) {
    if (node->parent->color != RBTREE_COLOR_BLACK) {
        tree_repair_case4(node);
        return ;
    }
    RBTreeNode* sibling = tree_sibling(node);
    if (sibling->color != RBTREE_COLOR_BLACK) {
        tree_repair_case4(node);
        return ;
    }

    if (tree_repair_childrenColors(sibling, RBTREE_COLOR_BLACK, RBTREE_COLOR_BLACK) != 0) {
        tree_repair_case4(node);
        return ;
    }

    sibling->color = RBTREE_COLOR_RED;
    tree_repair_case1(node->parent);
}

static void tree_repair_case2(RBTreeNode* node) {
    RBTreeNode* sibling = tree_sibling(node);
    if (sibling == NULL) {
        /// case of root
        return ;
    }
    if (sibling->color == RBTREE_COLOR_RED) {
        node->parent->color = RBTREE_COLOR_RED;
        sibling->color = RBTREE_COLOR_BLACK;
        if (node->parent->left == node)
            tree_rotate_left( node->parent );
        else
            tree_rotate_right( node->parent );
    }
    tree_repair_case3(node);
}

static void tree_repair_case1(RBTreeNode* node) {
    if (node->parent == NULL) {
        return ;
    }
    tree_repair_case2(node);
}

static void tree_repair_delete(RBTreeNode* node) {
    if (node == NULL) {
        return ;
    }

    if (node->color == RBTREE_COLOR_RED) {
        node->color = RBTREE_COLOR_BLACK;
        return ;
    }

    /// restore -- cases
    tree_repair_case1(node);
}

void tree_delete(RBTree* tree, const size_t address) {
    RBTreeNode* node = tree_findNode( tree, address );
    if (node == NULL) {
        /// node not found -- nothing to remove
        return;
    }

    if (node->right == NULL) {
        /// simple case -- just remove
        if ( node->parent != NULL ) {
            /// non-root case
            tree_changeChild(node->parent, node, node->left);
        } else {
            /// removing root
            tree->root = node->left;
            if (node->left != NULL) {
                node->left->parent = NULL;
            }
        }

        if (node->color == RBTREE_COLOR_BLACK) {
            tree_repair_delete(node->left);
        }

        free(node);
        return;
    }

    if (node->left == NULL) {
        /// simple case -- just reconnect
        if ( node->parent != NULL ) {
            /// non-root case
            tree_changeChild(node->parent, node, node->right);
        } else {
            /// removing root
            tree->root = node->right;
            if (node->right != NULL) {
                node->right->parent = NULL;
            }
        }

        if (node->color == RBTREE_COLOR_BLACK) {
            tree_repair_delete(node->right);
        }

        free(node);
        return;
    }

    /// have both children
    /// there is right subtree

    RBTreeNode* nextNode = (RBTreeNode*) tree_getLeftDescendant(node);      /// never NULL
    node->area = nextNode->area;

    tree_changeChild(nextNode->parent, nextNode, nextNode->right);
    if (node->color == RBTREE_COLOR_BLACK) {
        tree_repair_delete(nextNode->right);
    }

    free(nextNode);
}

void tree_munmap(RBTree* tree, void *vaddr) {
    if (tree == NULL) {
        return ;
    }
    if (tree->root==NULL) {
        return ;
    }

    const size_t voffset = (size_t)vaddr;
    tree_delete(tree, voffset);
    assert( tree_isValid(tree) == RBTREE_INVALID_OK );
}

int tree_init(RBTree* tree) {
    if (tree == NULL) {
        return -1;
    }

    tree->root = NULL;
    return 0;
}


/// =========================================================


RBTreeNode* node_makeDefault() {
    return calloc( 1, sizeof(RBTreeNode) );
}

RBTreeNode* node_makeColored(const NodeColor color) {
    RBTreeNode* node = node_makeDefault();
    node->color = color;
    return node;
}

RBTreeNode* node_makeLeaf(const NodeColor color, MemoryArea area) {
    RBTreeNode* node = node_makeColored(color);
    node->area  = area;
    return node;
}

RBTreeNode* node_makeFull(RBTreeNode* left, RBTreeNode* right, MemoryArea area, const NodeColor color) {
    RBTreeNode* node = node_makeColored(color);
    node_connectLeft(node, left);
    node_connectRight(node, right);
    node->area  = area;
    return node;
}

void node_init(RBTreeNode* node) {
    if (node==NULL) {
        return ;
    }
    memset(node, 0x0, sizeof(RBTreeNode));
}

void node_release(RBTreeNode* node) {
    if (node==NULL) {
        return ;
    }
    tree_releaseNodes( node->left );
    tree_releaseNodes( node->right );
    node->left = NULL;
    node->right = NULL;
}

void node_setArea(RBTreeNode* node, const MemoryArea* area) {
    if (node==NULL) {
        return ;
    }
    if (area==NULL) {
        return ;
    }
    node->area = *area;
}

void node_setColor(RBTreeNode* node, const NodeColor color) {
    if (node==NULL) {
        return ;
    }
    node->color = color;
}

RBTreeNode* node_getLeft(RBTreeNode* node) {
    if (node==NULL) {
        return NULL;
    }
    return node->left;
}

RBTreeNode* node_getRight(RBTreeNode* node) {
    if (node==NULL) {
        return NULL;
    }
    return node->right;
}

void node_connectLeft(RBTreeNode* node, RBTreeNode* child) {
    tree_setLeftChild(node, child);
}

void node_connectRight(RBTreeNode* node, RBTreeNode* child) {
    tree_setRightChild(node, child);
}

