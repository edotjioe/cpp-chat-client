//
// Created by Jesse on 2019-01-09.
//

#ifndef CPP_CHAT_CLIENT_CLIENT_H
#define CPP_CHAT_CLIENT_CLIENT_H

#ifdef _WIN32
#else
#include <pthread.h>
#endif

#include <thread>
#include <string.h>
#include "Application.h"
#include "vusocket.h"
#include "CircularLineBuffer.h"

#define BUFFER_LENGTH 400

struct Message {
    char in[BUFFER_LENGTH];
    char out[BUFFER_LENGTH];
};

enum class ConnStatus {
    IN_PROGRESS,
    SUCCESS,
    BUSY,
    FAILED,
    QUIT
};

class Client : public Application {

private:
    struct Message message;
    ConnStatus loginStatus;

    bool SendUserName();
    ConnStatus ReceiveResponseFromServer();

    /**
     * You are free to add new member variables and methods here if needed.
     * Please do not remove the ones that are already here.
     */
    SOCKET sock;
    std::thread socketThread, stdinThread;
    CircularLineBuffer socketBuffer, stdinBuffer;



    /**
     * Assignment 2
     *
     * See the lab manual for the assignment description.
     */
    void tick() override;

    /**
     * Assignment 4
     *
     * See the lab manual for the assignment description.
     */
    int readFromStdin();

    /**
     * Assignment 4
     *
     * See the lab manual for the assignment description.
     */
    int readFromSocket();

    inline void threadReadFromStdin() {
        while (!isStopped()) {
            auto res = readFromStdin();
            if (res < 0) {
                stopApplication();
            }
        }
    }

    inline void threadReadFromSocket() {
        while (!isStopped()) {
            auto res = readFromSocket();
            if (res < 0) {
                stopApplication();
            }
        }
    }

    void createSocketAndLogIn();

    void closeSocket();

    inline void startThreads() {
        socketThread = std::thread(&Client::threadReadFromSocket, this);
        stdinThread = std::thread(&Client::threadReadFromStdin, this);
    }

    inline void stopThreads() {
        this->stopApplication();
        socketThread.join();
        stdinThread.join();
    }

public:
    inline ~Client() override {
        closeSocket();
        stopThreads();
    }

    /**
     * Assignment 1
     *
     * See the lab manual for the assignment description.
     */
    inline void setup() override {
        loginStatus = ConnStatus::IN_PROGRESS;
        createSocketAndLogIn();
        startThreads();
    }
};


#endif //CPP_CHAT_CLIENT_CLIENT_H
