#include <iostream>
#include "Client.h"

int main(int nargs, char **argv) {
    std::cout << "Computer Networks Chat Client Starting..." << std::endl;
    Client client;
    std::cout << "Setting up" << std::endl;
    client.setup();
    std::cout << "Running application" << std::endl;
    while (client.tick() >= 0) {
        // Wait for 100 milliseconds before checking for new input and messages.
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
