#include <stdio.h> 
#include <stdlib.h> 

#include "Places.h"
/////////////////////////////

//make new queue
struct Queue* NewQueue(int capacity);

//check if Queue is full.
// Queue is full when size becomes
// equal to the capacity
int isFull(struct Queue* queue);

// Queue is empty when size is 0
int isEmpty(struct Queue* queue);

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, PlaceId p);

// Function to remove an item from queue.
// It changes front and size
PlaceId dequeue(struct Queue* queue);

// Check if place is already in to-visit queue.
int isNotInQueue (struct Queue* queue, PlaceId p);

// drop the queue
void dropHunterQueue(struct Queue* queue);
