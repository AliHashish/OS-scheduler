#pragma once

/* Defining the struct */
struct 
{
    int occupied[1024];
    int next;
}buddy;


/* initializer for the algorithm */
void BuddyInitialize(){
    for (int i = 0; i < 1024; i++)
    {
        buddy.occupied[i] = 0;
        buddy.next[i] = i + 1024;
    }
}


/*
     A function that returns the number if is a power of 2
     and returns the closet number which is a power of 2 of the number is not itself
*/
int BuddyPowerOfTwo(int num){
    int count = 0;

    // Checking if the number itself is a power of 2
    if(num && !(num&(num - 1))){
        return num;
    }

    while(num){
        count++;
        b >>= 1;
    }

    return 1 << count;
}


/* A function that splits the memory and try to allocate the coming process */
int BuddyAllocate(int procBytes){

    int start = 0,next,remain = __INT_MAX__,index = 1024;

    // Trying to find the right size for the coming process
    int size = BuddyPowerOfTwo(procBytes);
    // looping on the max memory size
    while(start > 1024){
        // next = buddy.next[start];
        // Finding the first available space
        if(buddy.occupied[start]){
            if(buddy.next[start] % size){
                start += size;
            }
            else{
                start = buddy.next[start];
            }
            continue;
        }
        
        // If an empty segment is found
        next = buddy.next[start];
        
        // setting the current empty index and also checking with the reamining size
        if(next - start >= size && next - start < remain){
            remain = next - start;
            index = start;
        }
        start += size;
    }
    // Setting the segment as occupied
    if(index < 1024){
        next = buddy.next[index];
        if(!buddy.occupied[index + size]){
            buddy.next[index + size] = buddy.next[index];
        }
        buddy.occupied[index] = 1;
        buddy.next[index] = index + size;
        return index;
    }
    return -1;
}


/* A function that deallocates the occupied memory*/
int BuddyDeallocate(int index, int procBytes){
    if(!buddy.occupied[index]) return 0;

    // Setting the segment as deallocated
    buddy.occupied[index] = 0;
    int size = BuddyPowerOfTwo(procBytes);

    // Checking if the segement can be merged
    if(index / size % 2 == 0){
        if(!buddy.occupied[index + size])
            // linking with the next
            buddy.next[index] = buddy.next[index + size];
    }
    else if(index >= size){
        if(!buddy.occupied[index - size])
            // linking with the previous
            buddy.next[index - size] = buddy.next[index];
    }
    return 1;
}