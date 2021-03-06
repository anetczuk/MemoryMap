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

/**
 *  node
 *     \
 *       \
 *         n
 *        /
 *      ret
 */
static const RBTreeNode* tree_getRightDescendant(const RBTreeNode* node) {
    return tree_getLeftmostNode(node->right);
}

/**
 *      node
 *      /
 *    /
 *  n
 *   \
 *   ret
 */
static const RBTreeNode* tree_getLeftDescendant(const RBTreeNode* node) {
    return tree_getRightmostNode(node->left);
}

/**
 * Returns ancestor on right side of current node.
 *         ret
 *         /
 *       /
 *     n
 *      \
 *      node
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
 *    ret
 *      \
 *        \
 *          n
 *         /
 *       node
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

MemoryArea tree_area(const RBTree* tree) {
    if (tree==NULL)
        return memory_create(0, 0);
    if (tree->root==NULL)
        return memory_create(0, 0);

    const size_t startAddress = tree_startAddress(tree);
    const size_t endAddress = tree_endAddress(tree);
    const size_t addressSpace = endAddress - startAddress;
    return memory_create(startAddress, addressSpace);
}

static const RBTreeNode* tree_leftNode(const RBTreeNode* node) {
    const RBTreeNode* desc = tree_getLeftDescendant(node);
    if (desc != NULL) {
        return desc;
    }
    return tree_getLeftAncestor(node);
}

static const RBTreeNode* tree_rightNode(const RBTreeNode* node) {
    const RBTreeNode* desc = tree_getRightDescendant(node);
    if (desc != NULL) {
        return desc;
    }
    return tree_getRightAncestor(node);
}

static const RBTreeNode* tree_nodeByIndex(const RBTreeNode* node, const size_t index) {
    if (node==NULL) {
        return NULL;
    }
    const size_t currIndex = tree_nodeIndex(node);
    if (index < currIndex) {
        const RBTreeNode* left = tree_leftNode(node);
        return tree_nodeByIndex(left, index);
    }
    if (index > currIndex) {
        const RBTreeNode* right = tree_rightNode(node);
        return tree_nodeByIndex(right, index);
    }
    /// index equals
    return node;
}

MemoryArea tree_valueByIndex(const RBTree* tree, const size_t index) {
    if (tree == NULL) {
        return memory_create(0, 0);
    }
    const RBTreeNode* node = tree_nodeByIndex( tree->root, index );
    if (node == NULL) {
        return memory_create(0, 0);
    }
    return node->area;
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

    const RBTreeNode* prevBottom = tree_getLeftDescendant(node);
    if (prevBottom != NULL) {
        if ( prevBottom->area.end > node->area.start) {
            return RBTREE_INVALID_NOT_SORTED;
        }
    }
    const RBTreeNode* nextBottom = tree_getRightDescendant(node);
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

static RBTreeValidationError tree_isValid_checkColor(const RBTreeNode* node) {
	if (node->left != NULL) {
		const RBTreeValidationError validLeft = tree_isValid_checkColor(node->left);
		if (validLeft != RBTREE_INVALID_OK) {
			return validLeft;
		}
    }
	if (node->right != NULL) {
		const RBTreeValidationError validRight = tree_isValid_checkColor(node->right);
		if (validRight != RBTREE_INVALID_OK) {
			return validRight;
		}
	}

    if (node->color != RBTREE_COLOR_RED) {
    	return RBTREE_INVALID_OK;
    }
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

	return RBTREE_INVALID_OK;
}

RBTreeValidationError tree_isValid(const RBTree* tree) {
    if (tree==NULL)
        return RBTREE_INVALID_OK;
    if (tree->root == NULL)
        return RBTREE_INVALID_OK;
    if (tree->root->parent != NULL)
        return RBTREE_INVALID_ROOT_PARENT;

    const RBTreeNode* rootNode = tree->root;

    /// check pointers
    const RBTreeValidationError validPointers = tree_isValid_checkPointers(rootNode, rootNode->parent);
    if (validPointers != RBTREE_INVALID_OK) {
        return validPointers;
    }
    /// if pointers are valid, then there is no cycles

    /// check memory segments
    const RBTreeValidationError validMemory = tree_isValid_checkMemory(rootNode);
    if (validMemory != RBTREE_INVALID_OK) {
        return validMemory;
    }

    /// check is sorted
    const RBTreeValidationError validOrder = tree_isValid_checkSorted(rootNode);
    if (validOrder != RBTREE_INVALID_OK) {
        return validOrder;
    }

    /// checking red-black properties
    /// root is black
    if (rootNode->color != RBTREE_COLOR_BLACK)
        return RBTREE_INVALID_RED_ROOT;

    /// if a node is red, then both its children are black
    const RBTreeValidationError validColor = tree_isValid_checkColor(rootNode);
    if (validColor != RBTREE_INVALID_OK) {
        return validColor;
    }

    const RBTreeValidationError validPath = tree_isValid_checkBlackPath(rootNode);
    if (validPath != RBTREE_INVALID_OK) {
        return validPath;
    }

    return RBTREE_INVALID_OK;
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
	RBTreeNode* newNode = tree_makeColoredNode(RBTREE_COLOR_RED);         /// default color of new node
	tree_setLeftChild(node, newNode);
	tree_setLeftChild(newNode, oldLeft);

	tree_repair_insert(newNode);
	return newNode;
}

static RBTreeNode* tree_insertRightNode(RBTreeNode* node) {
	RBTreeNode* oldLeft = node->right;
	RBTreeNode* newNode = tree_makeColoredNode(RBTREE_COLOR_RED);         /// default color of new node
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

static const RBTreeNode* tree_findRootFromNode(const RBTreeNode* node) {
    /// find the new root to return
    const RBTreeNode* curr = node;
    while (curr->parent != NULL)
        curr = curr->parent;
    return curr;
}

static void tree_findRoot(RBTree* tree) {
    /// find the new root to return
    tree->root = (RBTreeNode*) tree_findRootFromNode(tree->root);
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


/// ==============================================================================================


static void tree_printLevel(const RBTreeNode* node, const size_t level, int* previousNodeIndex) {
    if (node == NULL) {
        return ;
    }

    if (level > 0) {
        tree_printLevel(node->left, level-1, previousNodeIndex);
        tree_printLevel(node->right, level-1, previousNodeIndex);
        return ;
    }

    const size_t pos = tree_nodeIndex(node);
    const size_t diff = (pos==0) ? 0 : (pos - *previousNodeIndex) - 1;

    /// print empty space on left
    for(size_t i=0; i<diff; ++i) {
        printf("          ");
    }

    char color = 'X';
    switch(node->color) {
    case RBTREE_COLOR_BLACK: {
        color = 'B';
        break;
    }
    case RBTREE_COLOR_RED: {
        color = 'R';
        break;
    }
    }

    const size_t blockSize = memory_size(&(node->area));
    printf("(%c,%03lx,%02lx)", color, node->area.start, blockSize);

    *previousNodeIndex = pos;
}

static void tree_printSubtree(const RBTreeNode* node) {
    if (node == NULL) {
        printf("%s", "(NULL)");
        return ;
    }
    const size_t depth = tree_depthSubtree(node);
    for(size_t l = 0; l < depth; ++l) {
        int previousNodeIndex = -1;
        tree_printLevel(node, l, &previousNodeIndex);
        printf("%s", "\n");
    }
}

static inline void tree_printWhole(const RBTreeNode* node) {
    const RBTreeNode* root = tree_findRootFromNode(node);
    tree_printSubtree( root );
}

void tree_print(const RBTree* tree) {
    if (tree == NULL) {
        printf("%s", "[NULL]");
        return ;
    }
    tree_printWhole(tree->root);
}


/// ==============================================================================================


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


/// =========================================================================


static RBTreeNode* tree_repair_sibling(RBTreeNode* parent, RBTreeNode* node) {
    if (parent == NULL) {
        return NULL;
    }
    if (parent->left == node)
        return parent->right;
    else
        return parent->left;
}

static int tree_repair_isChildrenColors(const RBTreeNode* parent, const NodeColor leftChild, const NodeColor rightChild) {
    if ( parent->left != NULL ) {
        if ( parent->left->color != leftChild ) {
            return 1;
        }
    } else {
        if ( leftChild != RBTREE_COLOR_BLACK) {
            return 1;
        }
    }
    if ( parent->right != NULL ) {
        if ( parent->right->color != rightChild ) {
            return 1;
        }
    } else {
        if ( rightChild != RBTREE_COLOR_BLACK) {
            return 1;
        }
    }
    return 0;
}

static void tree_repair_case1(RBTreeNode* parent, RBTreeNode* node);

static void tree_repair_case6(RBTreeNode* parent, RBTreeNode* node) {
    RBTreeNode* sibling = tree_repair_sibling(parent, node);
    sibling->color = parent->color;
    parent->color = RBTREE_COLOR_BLACK;

    if (parent->left == node) {
        if (sibling->right!=NULL)
            sibling->right->color = RBTREE_COLOR_BLACK;
        tree_rotate_left( parent );
    } else {
        if (sibling->left!=NULL)
            sibling->left->color = RBTREE_COLOR_BLACK;
        tree_rotate_right( parent );
    }
}

static void tree_repair_case5(RBTreeNode* parent, RBTreeNode* node) {
    RBTreeNode* sibling = tree_repair_sibling(parent, node);
    if (sibling->color != RBTREE_COLOR_BLACK) {
        tree_repair_case6(parent, node);
        return ;
    }
//    if (node == NULL) {
//        tree_repair_case6(parent, node);
//        return ;
//    }

    if (parent->left == node) {
        /// is left child
        if (tree_repair_isChildrenColors(sibling, RBTREE_COLOR_RED, RBTREE_COLOR_BLACK) == 0) {
            sibling->color = RBTREE_COLOR_RED;
            if (sibling->left != NULL)
                sibling->left->color = RBTREE_COLOR_BLACK;
            tree_rotate_right( sibling );
            tree_repair_case6(parent, node);
            return ;
        }
        tree_repair_case6(parent, node);
        return ;
    }

    if (parent->right == node) {
        /// is right child
        if (tree_repair_isChildrenColors(sibling, RBTREE_COLOR_BLACK, RBTREE_COLOR_RED) == 0) {
            sibling->color = RBTREE_COLOR_RED;
            if (sibling->right != NULL)
                sibling->right->color = RBTREE_COLOR_BLACK;
            tree_rotate_left( sibling );
            tree_repair_case6(parent, node);
            return ;
        }
        tree_repair_case6(parent, node);
        return ;
    }
}

static void tree_repair_case4(RBTreeNode* parent, RBTreeNode* node) {
    if (parent->color != RBTREE_COLOR_RED) {
        tree_repair_case5(parent, node);
        return ;
    }
    RBTreeNode* sibling = tree_repair_sibling(parent, node);
    if (sibling->color != RBTREE_COLOR_BLACK) {
        tree_repair_case5(parent, node);
        return ;
    }
    if (tree_repair_isChildrenColors(sibling, RBTREE_COLOR_BLACK, RBTREE_COLOR_BLACK) != 0) {
        tree_repair_case5(parent, node);
        return ;
    }
    sibling->color = RBTREE_COLOR_RED;
    parent->color = RBTREE_COLOR_BLACK;
}

static void tree_repair_case3(RBTreeNode* parent, RBTreeNode* node) {
    if (parent->color != RBTREE_COLOR_BLACK) {
        tree_repair_case4(parent, node);
        return ;
    }
    RBTreeNode* sibling = tree_repair_sibling(parent, node);
    if (sibling->color != RBTREE_COLOR_BLACK) {
        tree_repair_case4(parent, node);
        return ;
    }

    if (tree_repair_isChildrenColors(sibling, RBTREE_COLOR_BLACK, RBTREE_COLOR_BLACK) != 0) {
        tree_repair_case4(parent, node);
        return ;
    }

    sibling->color = RBTREE_COLOR_RED;
    tree_repair_case1(parent->parent, parent);
}

static void tree_repair_case2(RBTreeNode* parent, RBTreeNode* node) {
    RBTreeNode* sibling = tree_repair_sibling(parent, node);
    if (sibling == NULL) {
        /// case of root
        return ;
    }
    if (sibling->color == RBTREE_COLOR_RED) {
        parent->color = RBTREE_COLOR_RED;
        sibling->color = RBTREE_COLOR_BLACK;
        if (parent->left == node)
            tree_rotate_left( parent );
        else
            tree_rotate_right( parent );
    }
    tree_repair_case3(parent, node);
}

static void tree_repair_case1(RBTreeNode* parent, RBTreeNode* node) {
    if (parent == NULL) {
        return ;
    }
    tree_repair_case2(parent, node);
}

static void tree_repair_delete(RBTreeNode* parent, RBTreeNode* node) {
    if (node != NULL) {
        if (node->color == RBTREE_COLOR_RED) {
            node->color = RBTREE_COLOR_BLACK;
            return ;
        }
    }

    /// restore -- cases
    tree_repair_case1(parent, node);
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
            tree_repair_delete(node->parent, node->left);
            /// can happpen than root changes due to rotations
            tree_findRoot(tree);
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
            tree_repair_delete(node->parent, node->right);
            /// can happpen than root changes due to rotations
            tree_findRoot(tree);
        }

        free(node);
        return;
    }

    /// have both children
    /// there is right subtree

    RBTreeNode* nextNode = (RBTreeNode*) tree_getRightDescendant(node);      /// never NULL
    node->area = nextNode->area;

    tree_changeChild(nextNode->parent, nextNode, nextNode->right);
    if (nextNode->color == RBTREE_COLOR_BLACK) {
        tree_repair_delete(nextNode->parent, nextNode->right);
        /// can happpen than root changes due to rotations
        tree_findRoot(tree);
    }

    free(nextNode);
}


/// =========================================================================


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


RBTreeNode* tree_makeDefaultNode() {
    return calloc( 1, sizeof(RBTreeNode) );
}

RBTreeNode* tree_makeColoredNode(const NodeColor color) {
    RBTreeNode* node = tree_makeDefaultNode();
    node->color = color;
    return node;
}

//RBTreeNode* node_makeLeaf(const NodeColor color, MemoryArea area) {
//    RBTreeNode* node = tree_makeColoredNode(color);
//    node->area  = area;
//    return node;
//}
//
//RBTreeNode* node_makeFull(RBTreeNode* left, RBTreeNode* right, MemoryArea area, const NodeColor color) {
//    RBTreeNode* node = tree_makeColoredNode(color);
//    node_connectLeft(node, left);
//    node_connectRight(node, right);
//    node->area  = area;
//    return node;
//}
//
//void node_init(RBTreeNode* node) {
//    if (node==NULL) {
//        return ;
//    }
//    memset(node, 0x0, sizeof(RBTreeNode));
//}
//
//void node_release(RBTreeNode* node) {
//    if (node==NULL) {
//        return ;
//    }
//    tree_releaseNodes( node->left );
//    tree_releaseNodes( node->right );
//    node->left = NULL;
//    node->right = NULL;
//}

size_t tree_nodeIndex(const RBTreeNode* node) {
    if (node == NULL) {
        return -1;
    }

    const size_t subIndex = tree_sizeSubtree(node->left);
    const RBTreeNode* leftNode = tree_getLeftAncestor(node);
    if (leftNode == NULL) {
        return subIndex;
    }
    return tree_nodeIndex(leftNode) + 1 + subIndex;
}

//void node_setArea(RBTreeNode* node, const MemoryArea* area) {
//    if (node==NULL) {
//        return ;
//    }
//    if (area==NULL) {
//        return ;
//    }
//    node->area = *area;
//}
//
//void node_setColor(RBTreeNode* node, const NodeColor color) {
//    if (node==NULL) {
//        return ;
//    }
//    node->color = color;
//}
//
//RBTreeNode* node_getLeft(RBTreeNode* node) {
//    if (node==NULL) {
//        return NULL;
//    }
//    return node->left;
//}
//
//RBTreeNode* node_getRight(RBTreeNode* node) {
//    if (node==NULL) {
//        return NULL;
//    }
//    return node->right;
//}
//
//void node_connectLeft(RBTreeNode* node, RBTreeNode* child) {
//    tree_setLeftChild(node, child);
//}
//
//void node_connectRight(RBTreeNode* node, RBTreeNode* child) {
//    tree_setRightChild(node, child);
//}

