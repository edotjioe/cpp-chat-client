//
// Created by Edo on 2/11/2019.
//
#include <thread>
#include <string.h>
#include "Application.h"
#include "vusocket.h"
#include "CircularLineBuffer.h"
#include "Client.h"


#define PORT 8080

int Client::readFromSocket() {
    return 0;
}

int Client::readFromStdin() {
    return 0;
}

void Client::createSocketAndLogIn() {
    std::cout << "Creating socket and log in" << std::endl;

    while (true) {
        std::string str;
        std::cout << "\nEnter a name : ";
        std::cin >> str;

        if (str.compare("!quit") == 0) {
            std::cout << "Quiting" << std::endl;

            this -> closeSocket();

            break;
        }

        std::cout<<"\nYou entered "<< str << std::endl;


    }
}

void Client::closeSocket() {
    std::cout << "Closing socket" << std::endl;
}

void Client::tick() {

}