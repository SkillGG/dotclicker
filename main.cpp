#include <iostream>
#include <string>
#include <thread>
#include <stdio.h>
#include "Game.h"

using namespace std;

int lasttick = 0;
int thistick = 0;

bool running = true;

int main()
{
    Game *g = new Game();
    while (g->isRunning() == true)
    {
        string s;
        getline(cin, s);
        g->userInput(s);
    }
    cout << endl;
    return 0;
}
