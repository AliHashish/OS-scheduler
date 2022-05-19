#pragma once
#include "headers.h"
#include <string.h>
unsigned int** emptySlots;
unsigned int* numberEmptySlots;

/*
     A function that returns the number if is a power of 2
     and returns the closet number which is a power of 2 of the number is not itself
*/
 int __nextPowerOf2(int num) {
  int count = 0;

    // Checking if the number itself is a power of 2
    // example of how it works: supposed num = 8 ----> 1000
    // the condition now equates to:
    // 1000 && !(1000 & 0111)
    // 1000 && !(0000)
    // True && ! False ----> True
    // only an exact power of 2 would make the right hand side ! False
    if(num && !(num&(num - 1))){
        return num;
    }

    // Having reached this point means that num is not a power of 2
    // So we allocate the smallest power of 2 greater than num.
    // example of how it works: supposed num = 9 ----> 1001
    // 1001 is true, so we add count (count = 1 now), then right shift
    // 100 is true, so we add count (count = 2 now), then right shift
    // 10 is true, so we add count (count = 3 now), then right shift
    // 1 is true, so we add count (count = 4 now), then right shift
    // 0 is false, exit while loop
    while(num){
        count++;
        num >>= 1;        // num = num >> 1;
    }

    // now count = 4, so 1 << 4 = 10000 ----> 16, which is the smallest
    // power of 2 greater than 9
    return 1 << count;
    // Notice that 1 << 0 = 1, so it handles this corner case well.
}

unsigned int __logOfPower2(unsigned int n) {
  unsigned count = 0;
  while (n != 0) {
    n >>= 1;
    count += 1;
  }

  return count - 1;
}

/* Slots */

int __init_slots(int size, unsigned int*** _emptySlots, unsigned int** _numberEmptySlots) {
  unsigned int slotsNum = __logOfPower2(size) + 1;

  // allocate slots of slotsNum size (first dimension is 11 elements (0 (2^0 = 1) -> 10 (2^10 = 1024)) )
  unsigned int** emptySlots = (unsigned int**)malloc((slotsNum) * sizeof(unsigned int*));

  for (int i = 0; i < slotsNum; i++) {
    /*
      each index corresponds to possible [size] locations
      i.e: at 0 : 2^0 = 1 correponds to 1024/1 possible locations
           at 8 : 2^8 = 256 corresponds to 1024/256 = 4 possible location
      size is divided by 2 for each iteration, at i = 10, size = 1
      meaning that for 2^10 = 1024 there's only one possible partition.
    */
    emptySlots[i] = (unsigned int*)malloc(size * sizeof(unsigned int));

    // if (emptySlots[i] == NULL) {
    //   for (int j = i - 1; j >= 0; j--)
    //     free(emptySlots[j]);

    //   free(emptySlots);

    //   return -1;
    // }

    size /= 2;
  }

  *_emptySlots = emptySlots;

  // allocate numberEmptySlots, an array of slotsNum (previously calculated) entries
  unsigned int* numberEmptySlots = (unsigned int*)malloc((slotsNum) * sizeof(unsigned int));

  // if (numberEmptySlots == NULL) {
  //   for (int j = slotsNum; j >= 0; j--)
  //     free(emptySlots[j]);

  //   free(emptySlots);

  //   return -1;
  // }

  memset(numberEmptySlots, 0, (slotsNum) * sizeof(unsigned int));
  fflush(stdout);

  *_numberEmptySlots = numberEmptySlots;

  return 1;
}

int __slot_insert(unsigned int slotIdx, int position) {
  if (slotIdx < 0)
    return -1;

  //looping index on empty slots
  int i = 0;

  /*should insert slot at correct index (comparing positions)
    empty slots are sorted according to their position in memory
    should increment as long as my position is greater than current position
    and my index is less than number of empty slots of given size
  */
  while (i < numberEmptySlots[slotIdx] && emptySlots[slotIdx][i] < position) {
    i++;
  }

  /*
    shifting current slots in order to insert in correct position
  */
  for (int j = numberEmptySlots[slotIdx] - 1; j >= i; j--) {
    emptySlots[slotIdx][j + 1] = emptySlots[slotIdx][j];
  }

  emptySlots[slotIdx][i] = position;

  numberEmptySlots[slotIdx]++;

  return i;
}

bool __slot_remove(unsigned int slotIdx, unsigned int procIdx) {
  if (procIdx >= numberEmptySlots[slotIdx])  
    return 0;
  
  //shifting other slots to remove current slot
  for (int i = procIdx; i < numberEmptySlots[slotIdx] - 1; i++) {
    emptySlots[slotIdx][i] = emptySlots[slotIdx][i + 1];
  }

  //decrease number of empty slots
  numberEmptySlots[slotIdx]--;

  return 1;
}


bool __buddy_split(unsigned int slotIdx) {
  if (slotIdx == 0)
    return 0;

  /* while there are no empty slots of current size, recursively split on next greater size (next power of 2)
    initially, memory has one entry of size = 1024, so recursion base condition is
    next size with available empty slots (will never TLE since there's always an empty slot of size 1024)
  */

  if (numberEmptySlots[slotIdx] == 0)
    __buddy_split(slotIdx + 1);
  
  //get position of first empty slot of given size
  unsigned int position = emptySlots[slotIdx][0];

  //remove slot of this size, and then split into two strictly smaller-sized slots
  __slot_remove(slotIdx, 0);

  /*insert two slots instead of removed one, each of size (prevSize/2)
    first slot starts at start of prevSlot and second starts at start_of_prevSlot+(prevSize/2)
  */
  __slot_insert(slotIdx - 1, position);
  __slot_insert(slotIdx - 1, position + (1 << (slotIdx - 1)));

  return 1;
}


void __buddy_check_merge(unsigned int slotIdx, unsigned int elemIdx) {
  unsigned int elemPos = emptySlots[slotIdx][elemIdx];
  unsigned int isOddIdx = ((elemPos / (1 << slotIdx)) % 2);

  if (isOddIdx)  // oddIdx -> check if left of me is free
  {
    if (elemIdx > 0 && elemPos - emptySlots[slotIdx][elemIdx - 1] == (1 << slotIdx)) {
      // I have a left and it is the one right before me
      unsigned int slotStart = emptySlots[slotIdx][elemIdx - 1];

      __slot_remove(slotIdx, elemIdx);
      __slot_remove(slotIdx, elemIdx - 1);

      unsigned int insertedAt = __slot_insert(slotIdx + 1, slotStart);

      __buddy_check_merge(slotIdx + 1, insertedAt);
    }
  }
  else {  // evenIdx -> check if right of me is free
    if (elemIdx < ((int)numberEmptySlots[slotIdx] - 1) && emptySlots[slotIdx][elemIdx + 1] - elemPos == (1 << slotIdx)) {
      // I have a right and it is the one right after me
      unsigned int slotStart = emptySlots[slotIdx][elemIdx];

      __slot_remove(slotIdx, elemIdx + 1);
      __slot_remove(slotIdx, elemIdx);

      unsigned int insertedAt = __slot_insert(slotIdx + 1, slotStart);

      __buddy_check_merge(slotIdx + 1, insertedAt);
    }
  }
}

int buddy_allocate(unsigned int size) {
  // get index and size occupied by process that will be be allocated
  unsigned int nearestSize = __nextPowerOf2(size);
  unsigned int index = __logOfPower2(nearestSize);

  // if there is no empty slot of required size, recursively split until you reach required size
  if (numberEmptySlots[index] == 0) {
    if (__buddy_split(index + 1) == 0)
      return 0;
  }

  //position at which to allocate process
  unsigned int position = emptySlots[index][0];

  //remove first empty slot of 2^index size (slot is occupied by process now)  
  __slot_remove(index, 0);  

  return position;
}

bool buddy_free(int position, int size) {
  // get index and size occupied by process that will be removed
  unsigned int nearestSize = __nextPowerOf2(size);
  unsigned int index = __logOfPower2(nearestSize);

  // insert an empty slot with calculated size and index
  int emptySlot = __slot_insert(index, position);

  // check if we could merge after each removal of process
  __buddy_check_merge(index, emptySlot);

  return 1;
}

void buddy_init() {
  int size = 1024;
  __init_slots(size, &emptySlots, &numberEmptySlots);

  unsigned int logSize = __logOfPower2(size);
  emptySlots[logSize][0] = 0;
  numberEmptySlots[logSize] = 1;
}
