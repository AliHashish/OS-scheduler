#pragma once
#include "headers.h"

typedef struct priQ {
  int capacity;
  int size;

  process **queue;
} priQ;


//TODO - print

// 1 for success, 0 for failed (when no queue is passed)
bool create_priQ(priQ *all_process,int capacity) {
  if(!all_process) return 0;
  if(!capacity) capacity=50;

  all_process->size=0;
  all_process->capacity=capacity;
  all_process->queue = (process **)calloc(queue->capacity, sizeof(process *)); //initialize to zero
  return 1;
}

process* peek(priQ *all_process) {
  if(all_process->size==0) return NULL;
  return all_process->queue[0];
} 

void reheapDown(priQ* all_process, int i) {
  int *size = &all_process->size;
  bool left_ok = left(i) < *size && comparison(all_process->queue[left(i)],all_process->queue[i]);
  bool right_ok = right(i) < *size && comparison(all_process->queue[right(i)],all_process->queue[i]);

  while(i<*size && (left_ok || right_ok)) {
    if (left_ok && right_ok) {
      if (comparison(all_process->queue[left(i)],all_process->queue[right(i)])) {
        swap(&all_process->queue[i], &all_process->queue[left(i)]);
        i = left(i);
      }
      else {
       swap(&all_process->queue[i], &all_process->queue[right(i)]);
        i = right(i);
      }
    }
    else if (left_ok) {
        swap(&all_process->queue[i], &all_process->queue[left(i)]);
        i = left(i);
    }
    else {
        swap(&all_process->queue[i], &all_process->queue[right(i)]);
        i = right(i);
    }

    left_ok = left(i) < *size && comparison(all_process->queue[left(i)],all_process->queue[i]);
    right_ok = right(i) < *size && comparison(all_process->queue[right(i)],all_process->queue[i]);

  }
}

void reheapUp(priQ* all_process) {
  int *size = &all_process->size;

  int i = *size - 1;
  while (i != 0 && comparison(all_process->queue[i],all_process->queue[parent(i)])) {
   swap(&all_process->queue[i], &all_process->queue[parent(i)]);
    i = parent(i);
  }
}

process* dequeue(priQ *all_process) {
  if(!all_process || !all_process->queue) return NULL;
  int *size = &all_process->size;
  (*size--);
  process* removed = all_process->queue[0];
  all_process->queue[0] = all_process->queue[*size];
  // reheap down
  reheapDown(all_process,0);
  return removed;
}

bool enqueue(priQ* all_process, process* p) {
  if(all_process->capacity==all_process->size) return 0;
  int *size = &all_process->size;
  all_process->queue[*size] = p;
  (*size)++;
  // reheap up
  reheapUp(all_process);
  return 1;
}


bool remove(priQ* all_process, process* p) {
  int i;
  int *size = &all_process->size;

  for (i = 0; i < *size; i++) {
    if (all_process->queue[i] == process) {
      swap(&all_process->queue[i], &all_process->queue[*size - 1]);
      (*size)--;
      reheapDown(all_process, i);
      break;
      return 1;
    }
  }
  return 0;
}

bool freePri(priQ* all_process) {
  if(!all_process || !all_process->queue) return 0;
  free(all_process->queue);
  return 1;
}

/***** UTILS *****/
int parent(int i) {
  return (i-1)/2;
}

int left(int i) {
  return 2*i + 1;
}

int right(int i) {
  return 2*i + 2;
}

bool comparison(process *x, process* y) {
  if(x->priority > y->priorty) {
    return true;
  }
  return false;
}

void swap(process** x, process** y) {
  process* temp = *x;
  *x = *y;
  *y = temp;
}
