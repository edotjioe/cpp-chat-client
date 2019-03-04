//
// Created by Edo on 2/11/2019.
//

#include <iostream>
#include <string.h>
#include "CircularLineBuffer.h"

int CircularLineBuffer::freeSpace() {
    return bufferSize - count;
}

bool CircularLineBuffer::isFull() {
    return bufferSize - count <= 0 ? true: false;
}

bool CircularLineBuffer::isEmpty() {
    return count == 0 ? true: false;
}

int CircularLineBuffer::nextFreeIndex() {
    return start + count % bufferSize;
}

int CircularLineBuffer::findNewline() {
    for (int i = 0; i < bufferSize; ++i) {
        if(buffer[start + i % bufferSize] == '-')
            return start + i % bufferSize;
    }
    return 0;
}

bool CircularLineBuffer::hasLine() {
    return findNewline() > 0 ? true : false;
}

bool CircularLineBuffer::_writeChars(const char *chars, size_t nchars) {
    if (isFull() || nchars > freeSpace())
        return false;

    for (int i = 0; i < nchars; ++i) {
        buffer[nextFreeIndex() + i % bufferSize] = chars[i];
    }

    count = count + nchars;

    std::cout << "Start: " << start << ", " << "Count: " << count << ", ";
    std::cout << "Buffer: ";
    for (int j = start; j < count; ++j) {
        std::cout << buffer[j] << "";
    }
    std::cout << std::endl;
    return true;
}

std::string CircularLineBuffer::_readLine() {
    if (hasLine()){
        const int len = findNewline() - start;
        char strl[len];
        for (int i = 0; i < len; ++i) {
            strl[i] = buffer[i + start % bufferSize];
        }

        for (int j = 0; j < count; ++j) {
            std::cout << strl[j + start];
        }

        start = findNewline() + 1;
        count = count - len;

        std::string str(strl);
        return str;
    }
    return "";
}