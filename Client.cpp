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
    createSocket();

    while (loginStatus == ConnStatus::IN_PROGRESS) {
        if (sendUserName()) {
            memset(&message.in, 0x00, sizeof(message.in));
            if (recvfrom(sock, message.in, sizeof(message.in), 0, adr->ai_addr, &len) < 0) {
                cout << "Failed to establish connection with server" << endl;
                loginStatus = ConnStatus::FAILED;
                return;
            }
            loginStatus = receiveResponseFromServer(message.in);
        }
    }
}

/*Creating socket with UDP protocol*/
void Client::createSocket() {
    OutputDebugStringW(L"Creating socket.");
    std::cout << "Creating socket and log in" << std::endl;

    loginStatus = ConnStatus::IN_PROGRESS;
    struct addrinfo hints = {0}, *addrs;

    const char *host = "52.58.97.202";
    const char *port = "5382";
    len = sizeof(sockaddr_in);

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
        break;
    }
}

void Client::closeSocket() {
    sock_close(sock);
    sock_quit();
    std::cout << "Closing socket" << std::endl;

    return;
}

/*Handling the different commands of the client*/
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

    if (sendto(sock, msg, strlen(msg), 0, adr -> ai_addr, adr -> ai_addrlen) == -1) {
        cout << "Failed to send" << endl;
        return;
    }
    //send(sock, msg, strlen(msg), 0);
}

/*Handling exit functionality*/
bool Client::exit(char *msg) {
    return strcmp(msg, "!exit\n") == 0;
}

/*Send username to server*/
bool Client::sendUserName() {
    memset(&message.out, 0x00, sizeof(message.out));

    char username[] = "", send_message[] = "HELLO-FROM ";
    std::cout << "Please enter your user name: ";

    fgets(message.out, BUFFER_LENGTH, stdin);
    strcat(username, message.out);

    if (exit(username)){
        loginStatus = ConnStatus::QUIT;
        std::cout << "Quiting chat client." << std::endl;
        return false;
    }

    strcat(send_message, username);
    puts(send_message);

    if (sendto(sock, send_message, strlen(send_message), 0, adr -> ai_addr, adr -> ai_addrlen) == -1) {
        cout << "Failed to send" << endl;
        return false;
    }

    return true;
}

/*Receive response of server*/
ConnStatus Client::receiveResponseFromServer(char message[]) {
    OutputDebugStringW(L"Send Success! SIZE: " + strlen(message));
    std::cout << "SERVER: " << message << std::endl;

    if (!strncmp("IN-USE", message, 6)) {
        OutputDebugStringW(L"Username in-use, ask for new username.");
        printf("Username in-use, try another username.");
        return ConnStatus::IN_PROGRESS;
    }
    else if (!strncmp("BUSY", message, 4)) {
        std::cout << ("Server is busy. Try again later.") << std::endl;
        return ConnStatus::BUSY;
    }
    else {
        OutputDebugStringW(L"Successfully established connection.");
        return ConnStatus::SUCCESS;
    }
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

int Client::readFromSocket() {
    memset(&message.in, 0x00, sizeof(message.in));

    int length = recvfrom(sock, message.in, sizeof(message.in), 0, adr->ai_addr, &len);

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

    if(exit(msg)){
        std::cout << "Exiting client" << std::endl;
        loginStatus = ConnStatus::QUIT;
        return -1;
    }

    stdinBuffer.writeChars(msg, length);
    return length;
}
