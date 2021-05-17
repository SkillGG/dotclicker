#include "Game.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;

Player::Player(Game *g) {
    this->money = 0;
    this->Gra = g;
    this->ulepszenia = {};
}

int Ulepszenie::getId() { return this->id; }
int Ulepszenie::getCost() { return this->cost; }

Ulepszenie::Ulepszenie(int i, int c) {
    this->id = i;
    this->cost = c;
}

PodwojnePieniadze1::PodwojnePieniadze1(int cost) : Ulepszenie::Ulepszenie(1, cost) {}

void PodwojnePieniadze1::use(Game *g) {
    // kup to po uzyciu
}

void PodwojnePieniadze1::buy(Game *g) {
    // zrob to po kupieniu
}

/** Ustaw domyslne wartosci */
Game::Game() {

    // lista mozliwych ulepszen
    this->mozliweUlepszenia = {(Ulepszenie *)(new PodwojnePieniadze1(100))};

    this->state = MENU;
    this->running = true;
    this->player = new Player(this);
}

/** Zatrzymaj gre */
void Game::stop() { this->running = false; }
/** Sprawdz czy dziala gra */
bool Game::isRunning() { return this->running; }

GameState Game::getState() { return this->state; }

/**
 * To jest funkcja ktora czyta userInput i ustawia rozne rzeczy zgodnie z nim
 */
void Game::userInput(string s) {
    this->lastInput = s;
    if (s == "q" || s == "Q") {
        this->stop();
    }
}
/**
 * To jest funkcja do wypisywania tekstu na ekranie
 * Jest to funkcja, ktora ma wyswietlac tekst zaleznie od wybranego stanu
 * */
void Game::Draw() {
    switch (this->getState()) {
    case 0:
        // What to draw in START
        break;
    case 1:
        // What to draw in MENU
        break;
    case 2:
        // What to draw in GAME
        break;
    default:
        // grrrr, error
        cout << "Something went wrong!";
        this->stop();
        break;
    }
}

int Player::getMoney() { return this->money; }

void Player::kupUlepszenie(Ulepszenie *u) {
    if (this->getMoney() > u->getCost()) {
        this->money -= u->getCost();
        u->buy(this->Gra);
    }
}

bool Player::maUlepszenie(Ulepszenie *u) {
    for (size_t i = 0; i < this->ulepszenia.size(); i++) {
        if (this->ulepszenia.at(i)->getId() == u->getId()) {
            return true;
        }
    }
    return false;
}

void Player::useUlepszenie(Ulepszenie *u) {
    if (this->maUlepszenie(u)) {
        u->use(this->Gra);
    }
}