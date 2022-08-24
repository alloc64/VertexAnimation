#ifndef _CONSOLEWINDOW_H
#define _CONSOLEWINDOW_H

#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <windows.h>

using namespace std;

class ConsoleWindow {
public:
    ConsoleWindow();

    ~ConsoleWindow();

    void Open();

    void Close();

    bool Status() { if (hStdHandle == INVALID_HANDLE_VALUE) return FALSE; else return TRUE; }

protected:
    const WORD MAX_CONSOLE_ROWS;
    const WORD MAX_CONSOLE_COLUMNS;
    FILE *fp;
    HANDLE hStdHandle;
    int hConHandle;
};

#endif
