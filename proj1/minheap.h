// Name: Sawyer Bowerman

#ifndef MINHEAP_H
#define MINHEAP_H

struct Node {
        unsigned char inputs;                    // input character
        unsigned freq;                          // frequency of char
        struct Node *left, *right;              // pointers for the left and right children
};

struct MinHeap {
        unsigned size;
        unsigned cap;                           // capacity of minheap
        struct Node **array;                    // array of pointers
};

// function declarations
struct MinHeap* createMinHeap(unsigned cap);
struct Node* newNode(unsigned char inputs, unsigned freq);  // Fixed declaration
void insertMinHeap(struct MinHeap* minHeap, struct Node* node);
struct Node* extractMin(struct MinHeap* minHeap);
void minHeapify(struct MinHeap* minHeap, int idx);
void swapNode(struct Node** a, struct Node** b);

#endif