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

size_t tree_depthSubtree(RBTreeItem* tree) {
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

size_t tree_depth(RBTree* tree) {
    if (tree==NULL)
        return 0;
    return tree_depthSubtree(tree->root);
}

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

RBTreeItem* tree_grandparent(RBTreeItem* node) {
	if (node->parent == NULL) {
		return NULL;
	}
	return node->parent->parent;
}

RBTreeItem* tree_sibling(RBTreeItem* node) {
	if (node->parent == NULL) {
		return NULL;
	}
	if (node->parent->left == node)
		return node->parent->right;
	else
		return node->parent->left;
}

RBTreeItem* tree_uncle(RBTreeItem* node) {
	return tree_sibling(node->parent);
}

void tree_rotate_left(RBTreeItem* node) {
    RBTreeItem* parent = node->parent;
    RBTreeItem* nnew = node->right;
    assert(nnew != NULL);                   /// since the leaves of a red-black tree are empty, they cannot become internal nodes
    node->right = nnew->left;
    nnew->left = node;
    nnew->parent = node->parent;
    node->parent = nnew;
    if (parent == NULL) {
        return;
    }
    if (parent->left == node) {
        parent->left = nnew;
    } else {
        parent->right = nnew;
    }
}

void tree_rotate_right(RBTreeItem* node) {
    RBTreeItem* parent = node->parent;
    RBTreeItem* nnew = node->left;
    assert(nnew != NULL);                   /// since the leaves of a red-black tree are empty, they cannot become internal nodes
    node->left = nnew->right;
    nnew->right = node;
    nnew->parent = node->parent;
    node->parent = nnew;
    if (parent == NULL) {
        return;
    }
    if (parent->left == node) {
        parent->left = nnew;
    } else {
        parent->right = nnew;
    }
}

void tree_repair(RBTreeItem* node) {
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

RBTreeItem* tree_insertLeftNode(RBTreeItem* node) {
	RBTreeItem* oldLeft = node->left;
	RBTreeItem* newNode = calloc( 1, sizeof(RBTreeItem) );
	node->left = newNode;
	newNode->parent = node;
	newNode->color = RBTREE_RED;						/// default color of new node
	newNode->left = oldLeft;

	tree_repair(newNode);

	return newNode;
}

RBTreeItem* tree_insertRightNode(RBTreeItem* node) {
	RBTreeItem* oldLeft = node->right;
	RBTreeItem* newNode = calloc( 1, sizeof(RBTreeItem) );
	node->right = newNode;
	newNode->parent = node;
	newNode->color = RBTREE_RED;						/// default color of new node
	newNode->right = oldLeft;

	tree_repair(node->right);

	return newNode;
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
    		/// smallest leaf case -- add node
    	    RBTreeItem* newNode = tree_insertLeftNode(node);
    	    newNode->area = *area;
			return (void*)newNode->area.start;
    	}
		leftArea = &(leftAncestor->area);
    } else {
        void* ret = tree_addMemoryToLeft(node->left, area);
        if (ret != NULL) {
            return ret;
        }
    	leftArea = &(node->left->area);
    }

    /// check free space between
    const size_t spaceBetween = node->area.start - leftArea->end;
    const size_t areaSize = memory_size( area );
    if (spaceBetween < areaSize) {
    	/// could not add to left side
    	return NULL;
    }

    /// fits between -- check if there are nodes between

    if ( node->left == NULL ) {
    	/// leaf case -- can add, no more between
        RBTreeItem* newNode = tree_insertLeftNode(node);
        newNode->area = *area;
		return (void*)newNode->area.start;
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

    const size_t minStart = tree_startAdderes(&(node->area), area );
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
        tree_repair(tree->root);
        return (void*)tree->root->area.start;
    }

	void* retLeft = tree_addMemoryToLeft(tree->root, area);
	if (retLeft!=NULL) {
	    tree_findRoot(tree);
		return retLeft;
	}
    /// else: ends inside or after current area
    /// go to right, called on root should always work
	void* retRight = tree_addMemoryToRight(tree->root, area);
	tree_findRoot(tree);
	return retRight;
}

size_t tree_add(RBTree* tree, const size_t address, const size_t size) {
    if (tree == NULL) {
        return 0;
    }
    MemoryArea area = memory_create(address, size);
    return (size_t)tree_addMemory(tree, &area);
}

void tree_printSubtree(RBTreeItem* node) {
    if (node == NULL) {
        return ;
    }
    tree_printSubtree(node->left);
    printf("(%lu,%lu) ", node->area.start, node->area.end);
    tree_printSubtree(node->right);
}

void tree_print(RBTree* tree) {
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
    return tree_addMemory(tree, &area);
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

static RBTreeItem* tree_getLeftDescendant(RBTreeItem* node) {
	RBTreeItem* curr = node;
	while (curr->left != NULL) {
		curr = curr->left;
	}
	return curr;
}

static RBTreeItem* tree_getRightDescendant(RBTreeItem* node) {
	RBTreeItem* curr = node;
	while (curr->right != NULL) {
		curr = curr->right;
	}
	return curr;
}

static inline void tree_reconnectToRight(RBTreeItem* node, RBTreeItem* subtree) {
	RBTreeItem* mostNode = tree_getRightDescendant(node);			/// can return 'node', never NULL
	mostNode->right = subtree;
	subtree->parent = mostNode;
}

static inline void tree_connectToLeft(RBTreeItem* node, RBTreeItem* subtree) {
	node->left = subtree;
	if (subtree != NULL) {
		subtree->parent = node;
	}
}

void tree_munmap(RBTree* tree, void *vaddr) {
    if (tree == NULL) {
        return ;
    }
    if (tree->root==NULL) {
        return ;
    }

    const size_t voffset = (size_t)vaddr;
    RBTreeItem* node = tree_findNode( tree->root, voffset );
    if (node == NULL) {
    	/// node not found -- nothing to remove
    	return;
    }

    if (node->right == NULL) {
    	/// just remove
    	if ( node->parent != NULL ) {
    		node->parent->right = NULL;
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

    /// there is right subtree
    RBTreeItem* nextNode = tree_getLeftDescendant(node->right);		/// never NULL
    RBTreeItem* rightParent = nextNode->parent;						/// can be 'node'
    if (rightParent != node) {
		rightParent->left = NULL;
		tree_reconnectToRight(nextNode, rightParent);
    }
	tree_connectToLeft(nextNode, node->left);
	if (node->parent != NULL) {
		node->parent->right = nextNode;
		nextNode->parent = node->parent;
	} else {
		/// removing root
		tree->root = nextNode;
		nextNode->parent = NULL;
	}
	free(node);
}

int tree_init(RBTree* tree) {
    if (tree == NULL) {
        return -1;
    }

    tree->root = NULL;
    return 0;
}
