#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

int lasttick = 0;
int thistick = 0;

bool running = true;

int main() {
    /* Game loop, nie tykac!! */
    Game *g = new Game();
    while (g->isRunning() == true) {
        system("cls");
        //system("clear");
        g->Draw();
        string s;
        getline(cin, s);
        g->userInput(s);
    }
    cout << endl;
    delete g;
    return 0;
}
