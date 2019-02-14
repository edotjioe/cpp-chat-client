//
// Created by Edo on 2/11/2019.
//
#include <thread>
#include <string.h>
#include "Application.h"
#include "vusocket.h"
#include "CircularLineBuffer.h"
#include "Client.h"
#include <iostream>

#define HOST "52.58.97.202"
#define PORT "5378"

void Client::tick() {

}

int Client::readFromSocket() {
    return 0;
}

int Client::readFromStdin() {
    return 0;
}

void Client::createSocketAndLogIn() {
    std::cout << "Creating socket and log in" << std::endl;

    int bufsize = 1024;
    char buffer[bufsize];

    struct addrinfo hints = {0}, *addrs;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    sock_init();

    const int status = getaddrinfo(HOST, PORT, &hints, &addrs);

    if (status != 0) {
        fprintf(stderr, "%s: %s\n", HOST, gai_strerror(status));
        abort();
    }

    sock = socket(status, SOCK_STREAM, IPPROTO_TCP);

    if (sock_valid(sock) <= 0) {
        std::cout << "Invalid socket" << std::endl;
        abort();
    }

    while (true) {
        std::string name;
        std::cout << "\nEnter a name : ";
        std::cin >> name;

        if (name.compare("!quit") == 0) {
            std::cout << "Quiting" << std::endl;

            break;
        }

        std::string message = "HELLO-FROM " + name + "\\n";

        send(sock, message.c_str(), (int) message.length(), 0);

        std::cout << recv(sock, buffer, bufsize, 0) << std::endl;

        std::cout << sock_error_code() << std::endl;

        std::cout<<"\nYou entered "<< name << std::endl;
    }

    sock_close(sock);

    sock_quit();
}

void Client::closeSocket() {
    std::cout << "Closing socket" << std::endl;
}

