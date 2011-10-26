/*
 * Created: 05-July-2010
 * 
 *  Author: Xiangfei Jia, xjianz@gmail.com, fei@cs.otago.ac.nz
 * 
 * 
 * 
 */
#ifndef LINKED_LIST_H
#define LINKED_LSIT_H

#include <string.h>

template <typename T> class Linked_list {
private:
	class Node {
		public:
			T data;
			char is_active;
			Node *next;
			Node(T data) : data(data), is_active(1), next(NULL) {}
			Node() : data(NULL), is_active(1), next(NULL) {}
	};
	
	Node *head, *tail;
	int item_count;
	
public:
	Linked_list(); 
	Linked_list(const Linked_list &l);
	~Linked_list();
	void reinit();
	void append(T data);
	void print();
	int find(T data);
	int find_and_inactivate(T data);
	int find_and_remove(T data);
	T first();
	T next();
	T first_active();
	T next_active();

private:
	Node *current;
	void init() {head = tail = NULL; item_count = 0;};
	void freemem();
};


template <typename T> Linked_list<T>::Linked_list() {
	init();
}

template <typename T> void Linked_list<T>::freemem() {
	Node *current = head, *temp;
	while (current) {
		temp = current->next;
		delete current;
		current = temp;
	}
}

template <typename T> Linked_list<T>::~Linked_list() {
	freemem();
}

template <typename T> void Linked_list<T>::reinit() {
	freemem();
	init();
}

template <typename T> void Linked_list<T>::append(T data) {
	Node *new_node = new Node(data);
	if (head == NULL) {
		head = tail = new_node;
	} else {
		tail->next = new_node;
		tail = new_node;
	}
	item_count++;
}

template <typename T> void Linked_list<T>::print() {
	Node *current;
	for (current = head; current != NULL; current = current->next) {
		printf("data: %d, is_active: %d\n", current->data, current->is_active);
	}
}

template <typename T> int Linked_list<T>::find(T data) {
	Node *current;
	for (current = head; current != NULL; current = current->next) {
		//printf("data: %d\n", *(current->data));
		if (current->data == data) {
			return 1;
		}
	}
	return -1;
}

template <typename T> int Linked_list<T>::find_and_inactivate(T data) {
	Node *current = head;
	for (current = head; current != NULL; current = current->next) {
		//printf("data: %d\n", *(current->data));
		if (current->data == data) {
			current->is_active = 0;
			return 1;
		}
	}
	return -1;
}

template <typename T> int Linked_list<T>::find_and_remove(T data) {
	Node *current = head;
	Node *prev = NULL;
	for (current = head; current != NULL; prev = current, current = current->next) {
		//printf("data: %d\n", *(current->data));
		if (current->data == data) {
			if (current == head) {
				head = current->next;
				delete current;
			} else if (current == tail) {
				tail = prev;
				tail->next = NULL;
				delete current;
			} else {
				prev->next = current->next;
				delete current;
				
			}
			item_count--;
			return 1;
		}
	}
	return -1;
}


template <typename T> T Linked_list<T>::first() {
	current = head;
	return current->data;
}

template <typename T> T Linked_list<T>::next() {
	current = current->next;
	if (current) {
		return current->data;
	} else {
		return NULL;
	}
}

template <typename T> T Linked_list<T>::first_active() {
	for (current = head; current != NULL; current = current->next) {
		if (current->is_active) {
			return current->data;
		}
	}
	return NULL;
}

template <typename T> T Linked_list<T>::next_active() {
	for (current = current->next; current != NULL; current = current->next) {
		if (current->is_active) {
			return current->data;
		}
	}
	return NULL;
}

#endif /* LINKED_LIST_H */
