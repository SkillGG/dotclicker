#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>

using namespace std;

int lasttick = 0;
int thistick = 0;

bool running = true;

int main() {
    Game *g = new Game();
    while (g->isRunning() == true) {
        g->Draw();
        system("cls");
        system("clear");
        string s;
        getline(cin, s);
        g->userInput(s);
    }
    cout << endl;
    delete g;
    return 0;
}
