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

#include "memorymap/MemoryArea.h"


typedef enum {
    BLACK = 0,
    RED
} NodeColor;


typedef struct RBTreeNode {
    struct RBTreeNode* parent;
    struct RBTreeNode* left;
    struct RBTreeNode* right;
    MemoryArea area;
    NodeColor color;                /// black by default
} RBTreeItem;


/// ===================================================


static size_t tree_sizeSubtree(RBTreeItem* tree) {
    if (tree==NULL) {
        return 0;
    }

    const size_t leftSize = tree_sizeSubtree(tree->left);
    const size_t rightSize = tree_sizeSubtree(tree->right);
    return leftSize + rightSize + 1;
}

size_t tree_size(RBTree* tree) {
    if (tree==NULL)
        return 0;
    return tree_sizeSubtree(tree->root);
}

//static inline int tree_fitBetween(const InsertionState* state, MemoryArea* area) {
//    const MemoryArea* smallerArea = NULL;
//    if (state->smaller!=NULL) {
//        smallerArea = &(state->smaller->area);
//    }
//    const MemoryArea* greaterArea = NULL;
//    if (state->greater!=NULL) {
//        greaterArea = &(state->greater->area);
//    }
//    return memory_fitBetween(smallerArea, greaterArea, area);
//}

//static inline void tree_insertLeft(RBTreeItem* node, const MemoryArea* area) {
//    RBTreeItem* old = node->left;
//    node->left = calloc( 1, sizeof(RBTreeItem) );
//    RBTreeItem* newNode = node->left;
//    newNode->area = *area;
//    newNode->left = old;
//    /// right is NULL
//}
//
//static inline void tree_insertRight(RBTreeItem* node, const MemoryArea* area) {
//    RBTreeItem* old = node->right;
//    node->right = calloc( 1, sizeof(RBTreeItem) );
//    RBTreeItem* newNode = node->left;
//    newNode->area = *area;
//    newNode->right = old;
//    /// left is NULL
//}

//static RBTreeItem* tree_getPreviousSegment(RBTreeItem* node) {
//    assert(node!=NULL);
//    if (node->right==NULL) {
//        return NULL;
//    }
//    RBTreeItem* next = node->right;
//    while(next->left!=NULL) {
//        next = next->left;
//    }
//    return next;
//}
//
//static RBTreeItem* tree_getNextSegment(RBTreeItem* node) {
//    assert(node!=NULL);
//    if (node->left==NULL) {
//        return NULL;
//    }
//    RBTreeItem* prev = node->left;
//    while(prev->right!=NULL) {
//        prev = prev->right;
//    }
//    return prev;
//}

static const RBTreeItem* tree_getRightAncestor(RBTreeItem* node) {
	const RBTreeItem* child = node;
	const RBTreeItem* curr = node->parent;
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

static const RBTreeItem* tree_getLeftAncestor(RBTreeItem* node) {
	const RBTreeItem* child = node;
	const RBTreeItem* curr = node->parent;
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

void tree_insertLeftNode(RBTreeItem* node) {
	RBTreeItem* oldLeft = node->left;
	node->left = calloc( 1, sizeof(RBTreeItem) );
	node->left->parent = node;
	node->left->left = oldLeft;
}

void tree_insertRightNode(RBTreeItem* node) {
	RBTreeItem* oldLeft = node->right;
	node->right = calloc( 1, sizeof(RBTreeItem) );
	node->right->parent = node;
	node->right->right = oldLeft;
}

static void* tree_addMemoryToRight(RBTreeItem* node, MemoryArea* area);

static void* tree_addMemoryToLeft(RBTreeItem* node, MemoryArea* area) {
    if (area->end > node->area.start) {
        /// could not add on left side
    	return NULL;
    }

	const MemoryArea* leftArea = NULL;
    if ( node->left == NULL ) {
    	const RBTreeItem* leftAncestor = tree_getLeftAncestor(node);
    	if (leftAncestor == NULL) {
    		/// leaf case -- add node
			tree_insertLeftNode(node);
			node->left->area = *area;
			return (void*)node->left->area.start;
    	}
		leftArea = &(leftAncestor->area);
    } else {
    	leftArea = &(node->left->area);
    }

    if (area->start < leftArea->end) {
    	/// could not add on left side
    	return NULL;
    }

    /// check free space
    const size_t spaceBetween = node->area.start - leftArea->end;
    const size_t areaSize = memory_size( area );
    if (spaceBetween < areaSize) {
    	/// could not add to left side
    	return NULL;
    }

    /// fits between -- check if there are nodes between

    if ( node->left == NULL ) {
    	/// leaf case -- can add, no more between
    	tree_insertLeftNode(node);
		node->left->area = *area;
		return (void*)node->left->area.start;
    }

	/// regular case -- check subtree
    if (node->left->right == NULL) {
    	/// no subtree -- add between
		tree_insertLeftNode(node);
		node->left->area = *area;
		return (void*)node->left->area.start;
    }

	return tree_addMemoryToRight(node->left, area);
}

static inline size_t tree_startAdderes(const MemoryArea* leftArea, const MemoryArea* check) {
	if (check->start < leftArea->end) {
		return leftArea->end;
	} else {
		return check->start;
	}
}

static void* tree_addMemoryToRight(RBTreeItem* node, MemoryArea* area) {
    if ( node->right == NULL ) {
    	const RBTreeItem* rightAncestor = tree_getRightAncestor(node);
    	if (rightAncestor == NULL) {
    		/// leaf case -- add node
    		memory_fitAfter(&(node->area), area);
			tree_insertRightNode(node);
			node->right->area = *area;
			return (void*)node->right->area.start;
    	}
		/// leaf case -- check space
		const int doesFit = memory_fitBetween(&(node->area), &(rightAncestor->area), area);
		if (doesFit == 0) {
			tree_insertRightNode(node);
			node->right->area = *area;
			return (void*)node->right->area.start;
		}
		/// no space -- return
		return NULL;
    }

    const MemoryArea* rightArea = &(node->right->area);
    const size_t minStart = tree_startAdderes(&(node->area), area );
    const size_t spaceBetween = rightArea->start - minStart;
	const size_t areaSize = memory_size( area );
	if (areaSize>spaceBetween) {
		/// no space -- go to next node
		return tree_addMemoryToRight(node->right, area);
	}
	if ( node->right->left == NULL ) {
		// no subtree -- add between
		area->start = minStart;
		area->end = minStart + areaSize;

		tree_insertRightNode(node);
		node->right->area = *area;
		return (void*)node->right->area.start;
	}
	return tree_addMemoryToLeft(node->right, area);
}

static void* tree_addMemory(RBTree* tree, MemoryArea* area) {
    assert( tree != NULL );
    assert( area != NULL );

    if (tree->root == NULL) {
        tree->root = calloc( 1, sizeof(RBTreeItem) );
        ///tree->root->parent = NULL;
        tree->root->area = *area;
        return (void*)tree->root->area.start;
    }

	void* ret = tree_addMemoryToLeft(tree->root, area);
	if (ret!=NULL) {
		return ret;
	}
    /// else: ends inside or after current area
    /// go to right, called on root should always work
    return tree_addMemoryToRight(tree->root, area);
}

size_t tree_add(RBTree* tree, const size_t address, const size_t size) {
    if (tree == NULL) {
        return 0;
    }
    MemoryArea area = memory_create(address, size);
    return (size_t)tree_addMemory(tree, &area);
}

static int tree_releaseNodes(RBTreeItem* tree) {
    if (tree == NULL) {
        return 0;
    }
    /**
     * Done in recursive manner. In case of very large structures consider
     * reimplementing it using while() and vector structure.
     */
    const int leftReleased = tree_releaseNodes(tree->left);
    const int rightReleased = tree_releaseNodes(tree->right);
    free(tree);
    return leftReleased+rightReleased+1;
}

int tree_release(RBTree* tree) {
    if (tree==NULL) {
        return -1;
    }
    return tree_releaseNodes(tree->root);
}


/// ===================================================


void* tree_mmap(RBTree* tree, void *vaddr, unsigned int size) {
    if (tree == NULL) {
        return NULL;
    }

    MemoryArea area = memory_create( (size_t)vaddr, size );
    return tree_addMemory(tree, &area);
}

void tree_munmap(RBTree* tree, void *vaddr) {
    //TODO: implement
}

int tree_init(RBTree* tree) {
    if (tree == NULL) {
        return -1;
    }

    tree->root = NULL;
    return 0;
}
