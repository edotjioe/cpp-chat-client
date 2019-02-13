#include <iostream>
#include "Client.h"

int main(int nargs, char **argv) {
    std::cout << "Computer Networks Chat Client Starting..." << std::endl;
    Application* app = new Client();
    std::cout << "Setting up" << std::endl;
    app->setup();
    std::cout << "Running application" << std::endl;
    app->run();
    app->stopApplication();
    std::cout << app->isStopped() << std::endl;
    delete app;
    return 0;
}
