#include "Game.h"
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

bool running = true;

int main() {
    /* Game loop, nie tykac!! */
    system("resize -s 30 110");
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
