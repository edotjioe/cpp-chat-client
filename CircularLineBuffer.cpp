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
    return (start + count) % bufferSize;
}

int CircularLineBuffer::findNewline() {
    for (int i = 1; i < bufferSize; ++i) {
        if(buffer[(start + i) % bufferSize] == '\n')
            return (start + i) % bufferSize;
    }
    return 40;
}

bool CircularLineBuffer::hasLine() {
    return findNewline() < 40 ? true : false;
}

bool CircularLineBuffer::_writeChars(const char *chars, size_t nchars) {
    if (isFull() || nchars > freeSpace())
        return false;

    for (int i = 0; i < nchars; ++i) {
        buffer[(nextFreeIndex() + i) % bufferSize] = chars[i];
    }

    count = count + nchars;

    return true;
}

std::string CircularLineBuffer::_readLine() {
    std::cout << "Hasline " << hasLine() << " find new line " << findNewline();
    if (hasLine()){
        int len;
        if ((start % bufferSize) > findNewline())
            len = findNewline() + (bufferSize - (start % bufferSize));
        else if (findNewline() > (start % bufferSize))
            len = findNewline() - (start % bufferSize);

        std::cout << "Start: " << start << " Count: " << count << " Length: " << len;
        char strl[len];

        memset(&strl, 0x00, sizeof(strl));
        for (int i = 0; i < len; ++i) {
            strl[i] = buffer[(i + start) % bufferSize];
        }

        strl[len] = '\0';
        start = start + len + 1;
        count = count - len - 1;

        std::string str(strl);

        return str;
    }
    return "shit";
}