#include "Menu.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

static void sleepMilliseconds(unsigned int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

int main() {
    // Display welcome message
    cout << "\n" << BOLD << UNDERLINE << COLOR_GREEN 
         << " BIBLIOTECH v2.0 - Systeme de Gestion Multimedia" << ANSI_RESET << endl;
    cout << "==============================================================\n";
    
    cout << COLOR_GREEN << "[OK] Bibliotech v2.0 - Systeme de gestion initialise!" << ANSI_RESET << endl;
    
    // Cross-platform delay
    sleepMilliseconds(1500);
    
    Menu menu;
    menu.executer();
    
    return 0;
}