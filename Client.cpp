//
// Created by Edo on 2/11/2019.
//
#include <thread>
#include <string.h>
#include "vusocket.h"
#include "Client.h"
#include <iostream>
#include <Ws2tcpip.h>

using namespace std;

void Client::createSocketAndLogIn() {
    OutputDebugStringW(L"Creating socket.");
    std::cout << "Creating socket and log in" << std::endl;

    loginStatus = ConnStatus::IN_PROGRESS;
    struct addrinfo hints = {0}, *addrs, *adr;

    const char *host = "52.58.97.202";
    const char *port = "5382";

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    sock_init();

    const int addrinfo = getaddrinfo(host, port, &hints, &addrs);

    if (addrinfo != 0) {
        std::cout << "Host not found." << std::endl;
        abort();
    }

    for (adr = addrs; adr != nullptr; adr = adr -> ai_next) {
        if ((sock = socket(adr -> ai_family, adr -> ai_socktype, adr -> ai_protocol)) == -1) {
            sock_error_code();
            continue;
        }

        std::cout << "Successfully connected to the server" << std::endl;
        break;
    }

    char string[16];
    strcpy(string, "HELLO-FROM Edo\n");

    if (sendto(sock, string, 16, 0, adr->ai_addr, adr->ai_addrlen) == -1)
        cout << "Failed to send" << endl;


    char buffer[4096];
    int n = 0, len;

    if(n = recvfrom(sock, (char *)buffer, 4096, MSG_WAITALL, adr->ai_addr, &len) == -1)
        cout << "Failed to receive" << endl;

    buffer[n] = '\0';
    printf("Server: %s\n", buffer);

//    while (loginStatus == ConnStatus::IN_PROGRESS) {
//        if (sendUserName()) {
//            loginStatus = receiveResponseFromServer();
//        }
//    }
}

int Client::tick() {
    if(loginStatus == ConnStatus::SUCCESS){
        if(stdinBuffer.hasLine()){
            std::string output;
            output = stdinBuffer.readLine();

            char msg[output.size()];
            strcpy(msg, output.c_str());

            command(msg);
        }

        if(socketBuffer.hasLine()){
            std::string input;
            input = socketBuffer.readLine();

            std::cout << "SERVER: " << input << std::endl;
        }
        return 0;
    }
    return -1;
}

void Client::command(char msg[]) {
    if (msg[0] == '@') {
        char newMsg[1024] = "";
        std::copy(&msg[1], &msg[strlen(msg)], newMsg);
        strcpy(msg, "SEND ");
        strcat(msg, newMsg);
    } else if (strcmp(msg, "!who\n") == 0) {
        strcpy(msg, "WHO\n");
    } else {
        std::cout << "Client: Command not recognised" << std::endl;
        return;
    }

    send(sock, msg, strlen(msg), 0);
}

bool Client::quit(char msg[]) {
    return strcmp(msg, "!exit\n") == 0;
}

int Client::readFromSocket() {
    memset(&message.in, 0x00, sizeof(message.in));

    int length = recv(sock, message.in, 1024, 0);

    if(length != -1) {
        std::string output;
        output = message.in;
        char msg[length];
        strcpy(msg, output.c_str());
        msg[length] = '\0';
        socketBuffer.writeChars(msg, length);
        return length;
    }
    return length;
}

int Client::readFromStdin() {
    char input[4096];

    std::cin.getline(input,sizeof(input));

    int length = strlen(input) + 1;
    char msg[length];
    strcpy(msg, input);
    msg[length - 1] = '\n';
    msg[length] = '\0';

    if(quit(msg)){
        std::cout << "Exiting client" << std::endl;
        loginStatus = ConnStatus::QUIT;
        return -1;
    }

    stdinBuffer.writeChars(msg, length);
    return length;
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

