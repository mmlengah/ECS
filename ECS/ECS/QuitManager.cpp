#include "QuitManager.h"

QuitManager& QuitManager::getInstance() {
    static QuitManager instance; 
    return instance;
}

QuitManager::QuitManager() : quit(false) {}

bool QuitManager::isQuit() const {
    return quit;
}

void QuitManager::setQuit(bool value) {
    quit = value;
}