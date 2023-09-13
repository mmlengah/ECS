#pragma once

class QuitManager {
public:
    static QuitManager& getInstance();

    bool isQuit() const;

    void setQuit(bool value);

private:
    QuitManager();
    bool quit;
};

