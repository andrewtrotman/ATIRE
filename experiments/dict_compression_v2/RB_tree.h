/*
 * Created: 14-July-2010
 *
 *  Author: Xiangfei Jia, xjianz@gmail.com, fei@cs.otago.ac.nz
 *
 *
 */

#ifndef RB_TREE_H
#define RB_TREE_H

#include <queue>
#include <iostream>
#include "primary_cmp.h"

using namespace std;

template <typename T, typename _Compare = Primary_cmp<T> > class RB_tree {
private:

	typedef enum {RED = 0, BLACK = 1} rbt_colour_t;

	class Node {
	public:
		T key;
		Node *left, *right, *parent;
		rbt_colour_t colour;
		Node() {left=right=parent=NULL; }
	};

	Node *root;
	_Compare compare;
	long long item_count;
	void left_rotate(Node *x);
	void right_rotate(Node *y);
	void binary_insert(Node *n);

	inline Node *the_successor(Node *x) {
		Node *y = x->right;

		if (y != NULL) {
			while (y->left) {
				y = y->left;
			}
			return y;
		} else {
			y = x->parent;
			while ((y != NULL) && (x == y->right)) {
				x = y;
				y = y->right;
			}
			if (y == root) {
				return NULL;
			}
			return y;
		}
	}

	void remove_fixup(Node *x);
	void freemem(Node *n);

	inline int search(T key, Node *n) {
		Node *x = root;
		while ((x != NULL) && (compare(x->key, key) != 0)) {
			if (compare(x->key, key) > 0) {
				x = x->left;
			} else {
				x = x->right;
			}
		}

		if (x != NULL) {
			n = x;
			return 1;
		} else {
			return 0;
		}
	}

public:
	RB_tree(const _Compare& __c = _Compare()) : compare(__c) {
		root = NULL; item_count = 0;
	}
	~RB_tree() { freemem(root); }

	void insert(T key);
	void remove(T key);
	void inorder_traverse(Node *x) {
		if (NULL == x) {
			return;
		}

		inorder_traverse(x->left);
		cout << "key: " << x->key << ", " << "colour: " << x->colour << endl;
		inorder_traverse(x->right);
	}
	void inorder() {
		inorder_traverse(root);
	}

	void breadth_first_traverse() {
		std::queue<Node *> q;
		Node *n;
		Node *nil = new Node();

		q.push(root);
		while(!q.empty()) {
			n = q.front();
			q.pop();
			if (n->left) {
				q.push(n->left);

			}
			if (n->right) {
				q.push(n->right);
			}
			cout << "key: " << n->key << ", " << "colour: " << n->colour << endl;
		}
	}

	inline int search(T key) {
		Node *x = root;
		while ((x != NULL) && (compare(x->key, key) != 0)) {
			if (compare(x->key, key) > 0) {
				x = x->left;
			} else {
				x = x->right;
			}
		}

		if (x != NULL) {
			return 1;
		} else {
			return 0;
		}
	}

};

template <typename T, typename _Compare> void RB_tree<T, _Compare>::freemem(Node *n) {
	if (NULL == n) {
		return;
	}
	freemem(n->left);
	freemem(n->right);
	delete n;
}

template <typename T, typename _Compare> void RB_tree<T, _Compare>::left_rotate(Node *x) {
	Node *y;

    y = x->right;
    x->right = y->left;
    if (y->left != NULL) {
        y->left->parent = x;
	}

    y->parent = x->parent;

    if (x->parent == NULL) {
		 root = y;
    } else {
        if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
		}
    }

    y->left = x;
    x->parent = y;
}

template <typename T, typename _Compare> void RB_tree<T, _Compare>::right_rotate(Node *y) {
	Node *x;
    x = y->left;

    y->left = x->right;
    if (x->right != NULL) {
        x->right->parent = y;
	}

    x->parent = y->parent;

    if (y->parent == NULL) {
		 root = x;
    } else {
        if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
		}
    }

    x->right = y;
    y->parent = x;

}

template <typename T, typename _Compare> void RB_tree<T, _Compare>::binary_insert(Node *n) {
	Node *y = NULL;
	Node *x = root;

	while(x != NULL) {
		y = x;
		if (compare(x->key, n->key) > 0) {
			x = x->left;
		} else {
			x = x->right;
		}
	}

	n->parent = y;

	if (y == NULL) {
		root = n;
	} else {
		if (compare(y->key, n->key) > 0) {
			y->left = n;
		} else {
			y->right = n;
		}
	}
}

template <typename T, typename _Compare> void RB_tree<T, _Compare>::insert(T key) {
	Node *y, *x = new Node();
	x->key = key;

	x->colour = RED;
	x->left = x->right = x->parent = NULL;
	binary_insert(x);

	while ((x != root) && (x->parent->colour == RED)) {
		if (x->parent == x->parent->parent->left) {
			y = x->parent->parent->right;
			if ((y != NULL) && (y->colour == RED)) {
				x->parent->colour = BLACK;
				y->colour = BLACK;
				x->parent->parent->colour = RED;
				x = x->parent->parent;
			} else {
				if (x == x->parent->right) {
					x = x->parent;
					left_rotate(x);
				}
				x->parent->colour = BLACK;
				x->parent->parent->colour = RED;
				right_rotate(x->parent->parent);
			}

		} else { // case for x->parent == x->parent->parent->right
			y = x->parent->parent->left;
			if ((y != NULL) && (y->colour == RED)) {
				x->parent->colour = BLACK;
				y->colour = BLACK;
				x->parent->parent->colour = RED;
				x = x->parent->parent;
			} else {
				if (x == x->parent->left) {
					x = x->parent;
					right_rotate(x);
				}
				x->parent->colour = BLACK;
				x->parent->parent->colour = RED;
				left_rotate(x->parent->parent);
			}

		}

	}

	root->colour = BLACK;
}

template <typename T, typename _Compare> void RB_tree<T, _Compare>::remove_fixup(Node *x) {
	Node *w;

	while ((x != root) && (x->colour == BLACK)) {
		if (x == x->parent->left) {
			w = x->parent->right;
			if (w->colour == RED) {
				w->colour = BLACK;
				x->parent->colour = RED;
				left_rotate(x->parent);
				w = x->parent->right;
			}
			if ((w->left->colour == BLACK) && (w->right->colour == BLACK)) {
				w->colour = RED;
				x = x->parent;
			} else {
				if (w->right->colour == BLACK) {
					w->left->colour = BLACK;
					w->colour = RED;
					right_rotate(w);
					w = x->parent->right;
				}
				w->colour = x->parent->colour;
				x->parent->colour = BLACK;
				w->right->colour = BLACK;
				left_rotate(x->parent);
				x = root; // to exit while loop
			}
		} else {
			w = x->parent->left;
			if (w->colour == RED) {
				w->colour = BLACK;
				x->parent->colour = RED;
				right_rotate(x->parent);
				w = x->parent->left;
			}
			if ((w->right->colour == BLACK) && (w->left->colour == BLACK)) {
				w->colour = RED;
				x = x->parent;
			} else {
				if (w->left->colour == BLACK) {
					w->right->colour = BLACK;
					w->colour = RED;
					left_rotate(w);
					w = x->parent->left;
				}
				w->colour = x->parent->colour;
				x->parent->colour = BLACK;
				w->left->colour = BLACK;
				right_rotate(x->parent);
				x = root; // to exit while loop
			}
		}
	}

	x->colour = BLACK;
}

template <typename T, typename _Compare> void RB_tree<T, _Compare>::remove(T key) {
	Node *z = NULL;
	Node *y = NULL,  *x = NULL;

	if (search(key, z)) {
		if ((z->left == NULL) || (z->right == NULL)) {
			y = z;
			//printf("deleting a leaf\n");
		} else {
			y = the_successor(z);
			//printf("deleting a inter-node, successor: %d\n", y->key);
		}

		if (y->left != NULL) {
			x = y->left;
		} else {
			x = y->right;
		}

		// x could be NULL
		if (x != NULL) {
			x->parent = y->parent;
		}

		if (y->parent == NULL) {
			root = x;
		} else {
			if (y == y->parent->left) {
				y->parent->left = x;
			} else {
				y->parent->right = x;
			}
		}

		if (y != z) {
			z->key = y->key;
		}

		// x could be NULL
		if ((y->colour == BLACK) && (x)) {
			//printf("fixup x: %d\n", x->key);
			remove_fixup(x);
		}

		delete y;
	}

}

#endif // RB_TREE_H
