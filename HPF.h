#pragma once

#include "priQ.h"

priQ* queue;

bool HPFaddProcess(void *type, process* p);
bool HPFpreempt(void *type);
process* HPFgetNextProcess(void *type);
bool HPFremoveProcess(void *type, process* p);
bool HPFfree(void *type);

void cast(void *type) {
    queue = (priQ *) type;
}

bool HPFInitialize(scheduling_algo* current_algorithm) {
    if(!current_algorithm) return 0;

    priQ* queue = (priQ*)malloc(sizeof(priQ));
    priQcreate(queue, 50);

    *current_algorithm = (scheduling_algo){
        queue,
        HPFaddProcess,
        HPFpreempt,
        HPFgetNextProcess,
        HPFremoveProcess,
        HPFfree,
    };

    return 1;
}

bool HPFaddProcess(void *type, process* p) {
    cast(type);
    bool success = priQenqueue(queue,p);
    return success;
}

process* HPFgetNextProcess(void *type) {
    cast(type);
    process* nextProcess = priQpeek(queue);
    return nextProcess;
}

bool HPFpreempt(void *type) {
    cast(type);
    return current_running_process != priQpeek(queue);
}

bool HPFremoveProcess(void *type, process* p) {
    cast(type);
    bool success = priQremove(queue,p);
    return success;
}

bool HPFfree(void *type) {
    cast(type);
    bool succes = priQfree(queue);
    return succes;
}
