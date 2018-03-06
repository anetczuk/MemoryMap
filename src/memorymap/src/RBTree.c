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

#include "memorymap/MemoryArea.h"


typedef enum {
    RBTREE_BLACK = 0,
    RBTREE_RED
} NodeColor;


typedef struct RBTreeNode {
    struct RBTreeNode* parent;
    struct RBTreeNode* left;
    struct RBTreeNode* right;
    MemoryArea area;
    NodeColor color;                /// black by default
} RBTreeItem;


/// ===================================================


static const RBTreeItem* tree_getLeftDescendant(const RBTreeItem* node) {
    const RBTreeItem* curr = node;
    while (curr->left != NULL) {
        curr = curr->left;
    }
    return curr;
}

static const RBTreeItem* tree_getRightDescendant(const RBTreeItem* node) {
    const RBTreeItem* curr = node;
    while (curr->right != NULL) {
        curr = curr->right;
    }
    return curr;
}

/**
 * Returns ancestor on right side of current node.
 */
static const RBTreeItem* tree_getRightAncestor(const RBTreeItem* node) {
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

/**
 * Returns ancestor on left side of current node.
 */
static const RBTreeItem* tree_getLeftAncestor(const RBTreeItem* node) {
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


/// ===================================================


static size_t tree_sizeSubtree(const RBTreeItem* tree) {
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

static size_t tree_depthSubtree(const RBTreeItem* tree) {
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

    const RBTreeItem* node = tree_getLeftDescendant(tree->root);
    return node->area.start;
}

size_t tree_endAddress(const RBTree* tree) {
    if (tree==NULL)
        return 0;
    if (tree->root==NULL)
        return 0;

    const RBTreeItem* node = tree_getRightDescendant(tree->root);
    return node->area.end;
}

static int tree_isValid_checkPointers(const RBTreeItem* node, const RBTreeItem* parent) {
    if (node == NULL) {
        return 0;
    }
    if (node->parent != parent) {
        return -1;
    }
    if ((node->left == node->right) && (node->right != NULL)) {
        /// the same child
        return -1;
    }

    const int validLeft = tree_isValid_checkPointers(node->left, node);
    if (validLeft != 0) {
        return validLeft;
    }
    const int validRight = tree_isValid_checkPointers(node->right, node);
    if (validRight != 0) {
        return validRight;
    }
    return 0;
}

static int tree_isValid_checkMemory(const RBTreeItem* node) {
    if (node == NULL) {
        return 0;
    }
    const int validMem = memory_isValid( &(node->area) );
    if ( validMem != 0) {
        /// invalid memory segment
        return validMem;
    }

    const int validLeft = tree_isValid_checkMemory(node->left);
    if (validLeft != 0) {
        return validLeft;
    }
    const int validRight = tree_isValid_checkMemory(node->right);
    if (validRight != 0) {
        return validRight;
    }
    return 0;
}

static int tree_isValid_checkSorted(const RBTreeItem* node) {
    if (node == NULL) {
        return 0;
    }

    const RBTreeItem* prevTop = tree_getLeftAncestor(node);
    if (prevTop != NULL) {
        if ( prevTop->area.end > node->area.start) {
            return -1;
        }
    }
    const RBTreeItem* nextTop = tree_getRightAncestor(node);
    if (nextTop != NULL) {
        if ( nextTop->area.start < node->area.end) {
            return -3;
        }
    }

    if (node->left != NULL) {
        const RBTreeItem* prevBottom = tree_getRightDescendant(node->left);
        if (prevBottom != NULL) {
            if ( prevBottom->area.end > node->area.start) {
                return -2;
            }
        }
    }
    if (node->right != NULL) {
        const RBTreeItem* nextBottom = tree_getLeftDescendant(node->right);
        if (nextBottom != NULL) {
            if ( nextBottom->area.start < node->area.end) {
                return -4;
            }
        }
    }

    const int validLeft = tree_isValid_checkSorted(node->left);
    if (validLeft != 0) {
        return validLeft;
    }
    const int validRight = tree_isValid_checkSorted(node->right);
    if (validRight != 0) {
        return validRight;
    }
    return 0;
}

/**
 * Every path from a given node to any of its descendant NIL nodes
 * contains the same number of black nodes.
 */
static int tree_isValid_checkBlackPath(const RBTreeItem* node) {
    //TODO: implement
    ///if (node == NULL)
    return 0;
}

static int tree_isValidSubTree(const RBTreeItem* node) {
    if (node==NULL)
        return 0;

    /// check pointers
    const int validPointers = tree_isValid_checkPointers(node, node->parent);
    if (validPointers!=0) {
        return validPointers;
    }
    /// if pointers are valid, then there is no cycles

    /// check memory segments
    const int validMemory = tree_isValid_checkMemory(node);
    if (validMemory!=0) {
        return validMemory;
    }

    /// check is sorted
    const int validOrder = tree_isValid_checkSorted(node);
    if (validOrder!=0) {
        return validOrder;
    }

    /// checking red-black properties

//    /// root is black
//    if (node->parent == NULL) {
//        /// root
//        if (node->color != RBTREE_BLACK)
//            return -1;
//    }
//
//    /// if a node is red, then both its children are black
//    if (node->color == RBTREE_RED) {
//        if (node->left != NULL) {
//            if (node->left->color != RBTREE_BLACK) {
//                return -1;
//            }
//        }
//        if (node->right != NULL) {
//            if (node->right->color != RBTREE_BLACK) {
//                return -1;
//            }
//        }
//    }
//
//    const int validPath = tree_isValid_checkBlackPath(node);
//    if (validPath!=0) {
//        return validPath;
//    }

    return 0;
}

int tree_isValid(const RBTree* tree) {
    if (tree==NULL)
        return 0;
    if (tree->root == NULL)
        return 0;
    if (tree->root->parent != NULL)
        return -1;
    return tree_isValidSubTree( tree->root );
}

static RBTreeItem* tree_grandparent(RBTreeItem* node) {
	if (node->parent == NULL) {
		return NULL;
	}
	return node->parent->parent;
}

static RBTreeItem* tree_sibling(RBTreeItem* node) {
	if (node->parent == NULL) {
		return NULL;
	}
	if (node->parent->left == node)
		return node->parent->right;
	else
		return node->parent->left;
}

static inline RBTreeItem* tree_uncle(RBTreeItem* node) {
	return tree_sibling(node->parent);
}

static inline void tree_setLeftChild(RBTreeItem* node, RBTreeItem* child) {
    node->left = child;
    if (child != NULL) {
        child->parent = node;
    }
}

static inline void tree_setRightChild(RBTreeItem* node, RBTreeItem* child) {
    node->right = child;
    if (child != NULL) {
        child->parent = node;
    }
}

static void tree_rotate_left(RBTreeItem* node) {
    RBTreeItem* parent = node->parent;
    RBTreeItem* nnew = node->right;
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

static void tree_rotate_right(RBTreeItem* node) {
    RBTreeItem* parent = node->parent;
    RBTreeItem* nnew = node->left;
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

static void tree_repair(RBTreeItem* node) {
	RBTreeItem* nParent = node->parent;
	if ( nParent == NULL) {
		node->color = RBTREE_BLACK;
		return ;
	}
	if (nParent->color == RBTREE_BLACK) {
		/// do nothing
		return ;
	}
	RBTreeItem* uncle = tree_uncle(node);
	if (uncle != NULL) {
        if (uncle->color == RBTREE_RED) {
            nParent->color = RBTREE_BLACK;
            uncle->color = RBTREE_BLACK;
            RBTreeItem* grandpa = tree_grandparent(node);		/// never NULL here
            grandpa->color = RBTREE_RED;
            tree_repair(grandpa);
            return ;
        }
	}

    RBTreeItem* curr = node;
	{
        RBTreeItem* grandpa = tree_grandparent(curr);		/// never NULL here
        if ((grandpa->left != NULL) && (curr == grandpa->left->right)) {
            tree_rotate_left(curr->parent);
            curr = curr->left;
        } else if ((grandpa->right != NULL) && (curr == grandpa->right->left)) {
            tree_rotate_right(curr->parent);
            curr = curr->right;
        }
	}
	{
	    RBTreeItem* grandpa = tree_grandparent(curr);       /// never NULL here
        if (curr == curr->parent->left)
            tree_rotate_right(grandpa);
        else
            tree_rotate_left(grandpa);
        curr->parent->color = RBTREE_BLACK;
        grandpa->color = RBTREE_RED;
	}
}

static RBTreeItem* tree_insertLeftNode(RBTreeItem* node) {
	RBTreeItem* oldLeft = node->left;
	RBTreeItem* newNode = calloc( 1, sizeof(RBTreeItem) );
	tree_setLeftChild(node, newNode);
	newNode->color = RBTREE_RED;						/// default color of new node
	tree_setLeftChild(newNode, oldLeft);

	tree_repair(newNode);
	return newNode;
}

static RBTreeItem* tree_insertRightNode(RBTreeItem* node) {
	RBTreeItem* oldLeft = node->right;
	RBTreeItem* newNode = calloc( 1, sizeof(RBTreeItem) );
	tree_setRightChild(node, newNode);
	newNode->color = RBTREE_RED;						/// default color of new node
	newNode->right = oldLeft;
	tree_setRightChild(newNode, oldLeft);

	tree_repair(node->right);
	return newNode;
}

static void* tree_addMemoryToRight(RBTreeItem* node, MemoryArea* area);

static void* tree_addMemoryToLeft(RBTreeItem* node, MemoryArea* area) {
    if (area->end > node->area.start) {
        /// could not add on left side
    	return NULL;
    }

	const RBTreeItem* leftNode = NULL;
    if ( node->left == NULL ) {
    	const RBTreeItem* leftAncestor = tree_getLeftAncestor(node);
    	if (leftAncestor == NULL) {
    		/// smallest leaf case -- add node
    	    RBTreeItem* newNode = tree_insertLeftNode(node);
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
        RBTreeItem* newNode = tree_insertLeftNode(node);
        newNode->area = memory_create( minStart, areaSize );
		return (void*)newNode->area.start;
    }

	return tree_addMemoryToRight(node->left, area);
}

static void* tree_addMemoryToRight(RBTreeItem* node, MemoryArea* area) {
    if ( node->right == NULL ) {
    	const RBTreeItem* rightAncestor = tree_getRightAncestor(node);
    	if (rightAncestor == NULL) {
    		/// greatest leaf case -- add node
    		memory_fitAfter(&(node->area), area);
    		RBTreeItem* newNode = tree_insertRightNode(node);
    		newNode->area = *area;
			return (void*)newNode->area.start;
    	}
		/// leaf case -- check space
		const int doesFit = memory_fitBetween(&(node->area), &(rightAncestor->area), area);
		if (doesFit == 0) {
		    RBTreeItem* newNode = tree_insertRightNode(node);
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
	if (areaSize>spaceBetween) {
		/// no space -- go to next node
		return tree_addMemoryToRight(node->right, area);
	} else {
        return tree_addMemoryToLeft(node->right, area);
	}
}

static void tree_findRoot(RBTree* tree) {
    /// find the new root to return
    RBTreeItem* node = tree->root;
    while (node->parent != NULL)
        node = node->parent;
    tree->root = node;
}

static void* tree_addMemory(RBTree* tree, MemoryArea* area) {
    assert( tree != NULL );
    assert( area != NULL );

    if (tree->root == NULL) {
        tree->root = calloc( 1, sizeof(RBTreeItem) );
        ///tree->root->parent = NULL;
        ///tree->root->color = RBTREE_BLACK;
        tree->root->area = *area;
        assert( tree_isValid(tree) == 0 );
        tree_repair(tree->root);
        assert( tree_isValid(tree) == 0 );
        return (void*)tree->root->area.start;
    }

	void* newMemoryAddress = tree_addMemoryToLeft(tree->root, area);
	if (newMemoryAddress==NULL) {
	    /// ends inside or after current area
	    /// go to right, called on root should always work
	    newMemoryAddress = tree_addMemoryToRight(tree->root, area);
	}
    tree_findRoot(tree);
	assert( tree_isValid(tree) == 0 );
    return newMemoryAddress;
}

size_t tree_add(RBTree* tree, const size_t address, const size_t size) {
    if (tree == NULL) {
        return 0;
    }
    MemoryArea area = memory_create(address, size);
    void* retAddr = tree_addMemory(tree, &area);
    assert( tree_isValid(tree) == 0 );
    return (size_t)retAddr;
}

static void tree_printSubtree(const RBTreeItem* node) {
    if (node == NULL) {
        return ;
    }
    tree_printSubtree(node->left);
    printf("(%lu,%lu) ", node->area.start, node->area.end);
    tree_printSubtree(node->right);
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
    tree_printSubtree(tree->root);
    printf("%s", "\n");
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
    void* retAddr = tree_addMemory(tree, &area);
    assert( tree_isValid(tree) == 0 );
    return retAddr;
}

static RBTreeItem* tree_findNode(RBTreeItem* node, const size_t adress) {
	RBTreeItem* curr = node;
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

static inline void tree_reconnectToRight(RBTreeItem* node, RBTreeItem* subtree) {
    /// can return 'node', never NULL
	RBTreeItem* mostNode = (RBTreeItem*) tree_getRightDescendant(node);
	tree_setRightChild(mostNode, subtree);
}

void tree_delete(RBTree* tree, const size_t address) {
    RBTreeItem* node = tree_findNode( tree->root, address );
    if (node == NULL) {
        /// node not found -- nothing to remove
        return;
    }

    //TODO: repaint tree

    if (node->right == NULL) {
        /// simple case -- just remove
        if ( node->parent != NULL ) {
            if (node->parent->right == node) {
                tree_setRightChild(node->parent, node->left);
            } else {
                tree_setLeftChild(node->parent, node->left);
            }
        } else {
            /// removing root
            tree->root = node->left;
            if (node->left != NULL) {
                node->left->parent = NULL;
            }
        }
        free(node);
        return;
    }

    if (node->left == NULL) {
        /// simple case -- just reconnect
        if ( node->parent != NULL ) {
            if (node->parent->right == node) {
                tree_setRightChild(node->parent, NULL);
            } else {
                tree_setLeftChild(node->parent, NULL);
            }
        } else {
            /// removing root
            tree->root = node->right;
            if (node->right != NULL) {
                node->right->parent = NULL;
            }
        }
        free(node);
        return;
    }

    /// there is right subtree
    RBTreeItem* nextNode = (RBTreeItem*) tree_getLeftDescendant(node->right);     /// never NULL
    RBTreeItem* rightParent = nextNode->parent;                                   /// can be 'node'
    if (rightParent != node) {
        tree_setLeftChild(rightParent, NULL);
        tree_reconnectToRight(nextNode, rightParent);
    }
    tree_setLeftChild(nextNode, node->left);
    if (node->parent != NULL) {
        tree_setRightChild(node->parent, nextNode);
    } else {
        /// removing root
        tree->root = nextNode;
        nextNode->parent = NULL;
    }
    free(node);
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
    assert( tree_isValid(tree) == 0 );
}

int tree_init(RBTree* tree) {
    if (tree == NULL) {
        return -1;
    }

    tree->root = NULL;
    return 0;
}
