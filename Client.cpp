//
// Created by Edo on 2/11/2019.
//
#include <thread>
#include <string.h>
#include "vusocket.h"
#include "Client.h"
#include <iostream>

void Client::createSocketAndLogIn() {
    OutputDebugStringW(L"Creating socket.");
    std::cout << "Creating socket and log in" << std::endl;

    loginStatus == ConnStatus::IN_PROGRESS;
    struct addrinfo hints = {0}, *addrs;

    const char *host = "52.58.97.202";
    const char *port = "5378";

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    sock_init();

    const int addrinfo = getaddrinfo(host, port, &hints, &addrs);

    if (addrinfo != 0) {
        std::cout << "Host not found." << std::endl;
        abort();
    }

    for (struct addrinfo *adr = addrs; adr != nullptr; adr = adr -> ai_next) {
        if ((sock = socket(adr -> ai_family, adr -> ai_socktype, adr -> ai_protocol)) == -1) {
            sock_error_code();
            continue;
        }
        if (connect((SOCKET) sock, adr -> ai_addr, adr -> ai_addrlen) == -1) {
            sock_error_code();
            sock_close(sock);
            continue;
        }
        std::cout << "Successfully connected to the server" << std::endl;
        break;
    }

    loginStatus = ConnStatus::SUCCESS;
    while (loginStatus == ConnStatus::IN_PROGRESS) {
        std::cout << "Text";
        if (sendUserName()) {
            loginStatus = receiveResponseFromServer();
        }
    }
}

int Client::tick() {
    if(loginStatus == ConnStatus::SUCCESS){
        memset(&message.in, 0x00, sizeof(message.in));
        memset(&message.out, 0x00, sizeof(message.out));

        char msg[1024] = "";
        std::cout << "Please enter your message:";

        fgets(message.out, 1024, stdin);
        strcat(msg, message.out);

        command(msg);

        char test[11] = {'h', 'e', 'l', 'l', 'o', 'h', 'e', 'l', 'l', 'o', '\n'};
        char test2[8] = {'g', 'o', 'o', 'd', 'b', 'y', 'e', '\n'};
        socketBuffer.writeChars(test, 11);
        socketBuffer.writeChars(test2, 8);
        std::string txt = socketBuffer.readLine();
        std::cout << "txt: " << txt << std::endl;
        socketBuffer.writeChars(test2, 8);
        socketBuffer.writeChars(test2, 8);
//        std::string txt = socketBuffer.readLine();
//        std::cout << std::endl << "txt: " << txt << std::endl;
        txt = socketBuffer.readLine();
        std::cout << std::endl << "txt: " << txt << std::endl;
        txt = socketBuffer.readLine();
        std::cout << std::endl << "txt: " << txt << std::endl;
        txt = socketBuffer.readLine();
        std::cout << std::endl << "txt: " << txt << std::endl;
        return 1;
    }

    return -1;
}

void Client::command(char msg[]) {
    if (quit(msg)){
        loginStatus = ConnStatus::QUIT;
        std::cout << "Quiting chat client." << std::endl;
        return;
    } else if (msg[0] == '@') {
        char newMsg[1024] = "";
        std::copy(&msg[1], &msg[strlen(msg)], newMsg);
        strcpy(msg, "SEND ");
        strcat(msg, newMsg);
    } else if (strcmp(msg, "!who\n") == 0) {
        std::cout << "Requesting user list." << std::endl;
        strcpy(msg, "WHO\n");
    } else {
        std::cout << "Client: Command not recognised" << std::endl;
        return;
    }

    send(sock, msg, strlen(msg), 0);
    recv(sock, message.in, 1024, 0);

    std::cout << "SERVER: " << message.in << std::endl;

    if (strcmp(message.in, "SEND-OK\n") == 0) {
        recv(sock, message.in, 1024, 0);

        std::cout << "SERVER: " << message.in << std::endl;
    }
}

bool Client::quit(char msg[]) {
    return strcmp(msg, "!quit\n") ? false : true;
}

int Client::readFromSocket() {
    return 0;
}

int Client::readFromStdin() {
    return 0;
}

// Send username to server
bool Client::sendUserName() {
    memset(&message.out, 0x00, sizeof(message.out));

    char username[] = "", send_message[] = "HELLO-FROM ";
    std::cout << "Please enter your user name: ";

    fgets(message.out, BUFFER_LENGTH, stdin);
    strcat(username, message.out);

    if (quit(username)){
        loginStatus = ConnStatus::QUIT;
        std::cout << "Quiting chat client." << std::endl;
        return false;
    }

    strcat(send_message, username);
    puts(send_message);

    int send_len = send(sock, send_message, strlen(send_message), 0);

    if (send_len) {
        OutputDebugStringW(L"Send Success! SIZE: " + send_len);
        return true;
    }else {
        OutputDebugStringW(L"Error sending username.");
        return false;
    }
}

// Receive response of server
ConnStatus Client::receiveResponseFromServer() {
    memset(&message.in, 0x00, sizeof(message.in));

    int recv_length = recv(sock, message.in, 1024, 0);

    if (recv_length != -1) {
        OutputDebugStringW(L"Send Success! SIZE: " + recv_length);
        std::cout << "SERVER: " << message.in << std::endl;

        if (!strncmp("IN-USE", message.in, 6)) {
            OutputDebugStringW(L"Username in-use, ask for new username.");
            printf("Username in-use, try another username.");
            return ConnStatus::IN_PROGRESS;
        }
        else if (!strncmp("BUSY", message.in, 4)) {
            std::cout << ("Server is busy. Try again later.") << std::endl;
            return ConnStatus::BUSY;
        }
        else {
            OutputDebugStringW(L"Successfully established connection.");
            return ConnStatus::SUCCESS;
        }
    }else {
        std::cout << "Failed to establish connection with server";
        return ConnStatus::FAILED;
    }
}

void Client::closeSocket() {
    sock_close(sock);
    sock_quit();
    std::cout << "Closing socket" << std::endl;
}

