//
// Created by Edo on 2/11/2019.
//

#include "CircularLineBuffer.h"

int CircularLineBuffer::freeSpace() {
    return 0;
}

bool CircularLineBuffer::isFull() {
    return false;
}

bool CircularLineBuffer::isEmpty() {
    return false;
}

int CircularLineBuffer::nextFreeIndex() {
    return 0;
}

int CircularLineBuffer::findNewline() {
    return 0;
}

bool CircularLineBuffer::hasLine() {
    return false;
}

bool CircularLineBuffer::writeChars(const char *chars, size_t nchars) {
    return false;
}