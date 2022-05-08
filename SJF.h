#pragma once

#include "priQ.h"

priQ* queue;

bool SJFaddProcess(void *type, process* p);
bool SJFpreempt(void *type);
process* SJFgetNextProcess(void *type);
bool SJFremoveProcess(void *type, process* p);
bool SJFfree(void *type);

void cast(void *type) {
    queue = (priQ *) type;
}

bool SJFInitialize(scheduling_algo* current_algorithm) {
    if(!current_algorithm) return 0;

    priQ* queue = (priQ*)malloc(sizeof(priQ));
    priQcreate(queue, 50);

    *current_algorithm = (scheduling_algo){
        queue,
        SJFaddProcess,
        SJFgetNextProcess,
        SJFremoveProcess,
        SJFfree,
    };

    return 1;
}

bool SJFaddProcess(void *type, process* p) {
    cast(type);
    bool success = priQenqueue(queue,p);
    return success;
}

process* SJFgetNextProcess(void *type) {
    cast(type);
    process* nextProcess = priQpeek(queue);
    return nextProcess;
}

bool SJFpreempt(void *type) {
}

bool SJFremoveProcess(void *type, process* p) {
    cast(type);
    bool success = priQremove(queue,p);
    return success;
}

bool SJFfree(void *type) {
    cast(type);
    bool succes = priQfree(queue);
    return succes;
}
