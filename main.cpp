#include <stdio.h>

#include <iostream>
#include <string>

#include "Game.h"

using namespace std;

bool running = true;

int main() {
    /* Game loop, nie tykac!! */
    /*a co jak tykne?...*/
    system("resize -s 30 110");
    Game *g = new Game();
    while (g->isRunning() == true) {
        system("clear");
        g->Draw();
        string s;
        getline(cin, s);
        g->userInput(s);
    }
    cout << endl;
    delete g;
    return 0;
}
