#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include "headers.h"


typedef struct circularQueue {
    int front;
    int back;
    int size;
    int occupied;
    process **entries;
} circularQueue;

//Default intializer
circularQueue circularQueueDefault = {0, 0, 0, 0, NULL};\

//A function the returns the next index
int cqueueForward(circularQueue *queue, int i) {
  return (i + queue->size - 1) % queue->size;
}


//A function the returns the previous index
int cqueueBackward(circularQueue *queue, int i) {
  return (i + 1) % queue->size;
}


//A function that creats a queue for the entries
//-1 for faild attempt and 0 for success
int cqueueInit(circularQueue *queue, int size) {
  if (!queue || !size) return -1;
  if (queue->entries) return -1;

  queue->size = size;
  //calloc for dynamic allocation and intializing all entries with (0)
  queue->entries = (process **)calloc(queue->size, sizeof(process *));
  return 0;
}


//A function that frees the dynamically allocated memory
//-1 for faild attempt and 0 for success
int cqueueFree(circularQueue *queue) {
  if (!queue)
    return -1;
  if (!queue->entries)
    return -1;
  //free() is to free the memory allocation
  free(queue->entries);
  return 0;
}


//A function that returns a pointer to the front entry
//returns NULL on failure
process *cqueueFront(circularQueue *queue) {
  if (!queue) return NULL;
  if (!queue->entries) return NULL;

  // Queue is empty
  if (!queue->occupied) return NULL;

  return queue->entries[queue->front];
}


//A function that enqueues and returns a pointer to the new front entry
//returns NULL on failure
int cqueueEnqueue(circularQueue *queue, process *p) {
  if (!queue) return -1;
  if (!queue->entries) return -1;

  // Queue is full
  if (queue->occupied == queue->size) return -1;

  queue->entries[queue->back] = p;
  // Move back circularly
  queue->back = cqueueBackward(queue, queue->back);
  queue->occupied = queue->occupied < queue->size ? queue->occupied + 1 : queue->size;

  return 0;
}


//A function that dequeues and returns a pointer to the new front entry
//returns NULL on failure
process *cqueueDequeue(circularQueue *queue) {
  if (!queue) return NULL;
  if (!queue->entries) return NULL;

  // Queue is empty
  if (!queue->occupied)
    return NULL;

  // Remove front element
  process *p = queue->entries[queue->front];
  // Move back circularly
  queue->front = cqueueBackward(queue, queue->front);
  queue->occupied = queue->occupied ? queue->occupied - 1 : 0;

  return p;
}


//A function that removes a process entry
//returns NULL on failure
process *cqueueRemove(circularQueue *queue, process *p) {
  if (!queue) return NULL;
  if (!queue->entries) return NULL;

  // Queue is empty
  if (!queue->occupied) return NULL;

  // Searching for P on step before back
  int i = cqueueForward(queue, queue->back);
  for (; i != queue->back; i = cqueueForward(queue, i)) {
    if (queue->entries[i] == p) break;
  }
  if (i == queue->back) return NULL;

  for (; i != queue->back; i = cqueueBackward(queue, i)) {
    queue->entries[i] = queue->entries[cqueueBackward(queue, i)];
  }

  // Move forward circularly
  queue->back = cqueueForward(queue, queue->back);  
  queue->occupied = queue->occupied ? queue->occupied - 1 : 0;
  return p;
}


#endif