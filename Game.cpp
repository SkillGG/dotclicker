#include "Game.h"
#include <thread>
#include <string>

using namespace std;

void Game::stop()
{
}

Game::Game()
{
}

bool Game::isRunning(){
    return this->running;
}

void Game::userInput(string s)
{
    if (s == "q")
    {
        this->running = false;
    }
}