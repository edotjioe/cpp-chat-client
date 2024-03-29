//
// Created by Jesse on 2019-01-09.
//

#include "Application.h"

#define PORT 8080

void Application::run() {
    while (!this->stop) {
        this->tick();
        this->stopApplication();
    }
}

void Application::stopApplication() {
    this->stop = true;
}

bool Application::isStopped() {
    return this->stop;
}
