#pragma once

#include "headers.h"

struct pcb{
    int size;       // actual used sized
    int capacity;   // maximum size available for usage
    process **array;
} pcb;

void pcbInitialize(int inputSize) {

    if(!inputSize || inputSize<1) inputSize = 200;
    if(inputSize<200) inputSize = 200;

    pcb.array = malloc(inputSize*sizeof(process));
    pcb.size = 0;
    pcb.capacity = inputSize;

}

process* pcbInsert(process* p) {

    if(pcb.size == pcb.capacity) {
        pcb.capacity = pcb.capacity * 2;
        pcb.array = realloc(pcb.array,pcb.capacity*sizeof(process));
    }
    process* to_insert = (process*) malloc(sizeof(process));
    *to_insert = *p;

    pcb.array[pcb.size++] = to_insert;
    return to_insert;

}

bool pcbRemove(process* p) {
    int id = p->id;
    process* remove = NULL;

    for(int i=0;i<pcb.size;i++) {
        if(pcb.array[i]->id == id) {
            remove = pcb.array[i];
            pcb.array[i] = pcb.array[pcb.size-1];
            pcb.size--;
            break;
        }
    }
    if(remove) {
        free(remove);
        return 1;
    }
    return 0;
}

void pcbRefresh() {

    for(int i=0;i<pcb.size;i++) {
        if(pcb.array[i]->status == 1 || pcb.array[i]->status==2) { //running process
            pcb.array[i]->remainingtime--;
        }
        else if(pcb.array[i]->status == 3) { //waiting process
            pcb.array[i]->waitingtime++;
        }
        else if(pcb.array[i]->status == 4) {
            pcbRemove(pcb.array[i]);
        }
    }

}

process* pcbGetProcess(int id) {
    process *p = NULL;
    
    for(int i=0;i<pcb.size;i++) {
        if(pcb.array[i]->pid == id) {
            p = pcb.array[i];
            break;
        }
    }

    // if no process with such ID exists, NULL is returned (must check on NULL)
    return p;
}

bool pcbFree() {

    for(int i=0;i<pcb.size;i++) free(pcb.array[i]);

    free(pcb.array);

    return 1;
}

// void pcbFilePrint