#pragma once

/* Defining the struct */
struct 
{
    int occupied[1024];
    int next[1024];
}buddy;


/* initializer for the algorithm */
void BuddyInitialize(){
    for (int i = 0; i < 1024; i++)
    {
        buddy.occupied[i] = 0;      // initially, nothing is occupied
        buddy.next[i] = i + 1024;   // initially, the whole memory is one segment
    }
}


/*
     A function that returns the number if is a power of 2
     and returns the closet number which is a power of 2 of the number is not itself
*/
int BuddyPowerOfTwo(int num){
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


/* A function that splits the memory and try to allocate the coming process */
int BuddyAllocate(int procBytes){

    int start = 0,next,remain = __INT_MAX__,index = 1024;

    // Trying to find the right size for the coming process
    int size = BuddyPowerOfTwo(procBytes);
    // looping on the max memory size
    while(start < 1024){
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
    return -1;      // msh el mfrood n5aleeha 0 3lshan tb2a zy el ba2y, 0 failed, 1 succeeded?
}


/* A function that deallocates the occupied memory*/
int BuddyDeallocate(int index, int procBytes){
    if(!buddy.occupied[index]) return 0;

    // Setting the segment as deallocated
    buddy.occupied[index] = 0;
    int size = BuddyPowerOfTwo(procBytes);

    // Checking if the segement can be merged by checking if it
    // has an index that is an even multiple of size
    if(index / size % 2 == 0){
        if(!buddy.occupied[index + size])   // if the following segment is not occupied, then merge
            // linking with the next
            buddy.next[index] = buddy.next[index + size];
    }
    else if(index >= size){
        if(!buddy.occupied[index - size])   // if the preceding segment is not occupied, then merge
            // linking with the previous
            buddy.next[index - size] = buddy.next[index];
    }
    return 1;
}