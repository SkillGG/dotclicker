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

void PodwojnePieniadze1::use(Game *g) {}

void PodwojnePieniadze1::buy(Game *g) {
    // zrob to po kupieniu
}

/** Ustaw domyslne wartosci */
Game::Game() {

    // lista mozliwych ulepszen
    this->mozliweUlepszenia = {(Ulepszenie *)(new PodwojnePieniadze1(100))};

    this->state = GameState::MENU;
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
    switch (this->getState()) {
    case GameState::START:
        // What to draw in START
        break;
    case GameState::MENU:
        // What to draw in MENU
        break;
    case GameState::GAME:
        // What to draw in GAME
        break;
    default:
        cout << "Something went wrong!";
        break;
    }
}

void DrawFromVector(vector<string> v) {
    for (size_t i = 0; i < v.size(); i++) {
        cout << v.at(i) << endl;
    }
}

/**
 * To jest funkcja do wypisywania tekstu na ekranie
 * Jest to funkcja, ktora ma wyswietlac tekst zaleznie od wybranego stanu
 * */
void Game::Draw() {
    vector<string> m;
    switch (this->getState()) {
    case START:
        // What to draw in START
        break;
    case MENU:
        // What to draw in MENU
        m = {
            "=======================================================",
            "",
            "              __  __   ___   _  _   _   _ ",
            "             |  \\/  | | __| | \\| | | | | |",
            "             | |\\/| | | _|  | .` | | |_| |",
            "             |_|  |_| |___| |_|\\_|  \\___/",
            "",
            "",
            "",
            "                  Zacznij zarabiac = 3*",
            "",
            "                  Ulepszenia = Ulepsz*",
            "",
            "                  Wyjscie = q",
            "",
            "======================================================="};
        DrawFromVector(m);
        break;
    case GAME:
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

void Player::kupUlepszenie(int uid) {
    try {
        Ulepszenie *u = this->Gra->mozliweUlepszenia.at(uid);
        if (this->getMoney() > u->getCost()) {
            this->money -= u->getCost();
            this->ulepszenia.push_back(u);
        }
    } catch (...) {
        cout << "Something went wrong";
    }
}

bool Player::maUlepszenie(int uid) {
    try {
        for (size_t i = 0; i < this->ulepszenia.size(); i++) {
            if (this->ulepszenia.at(i)->getId() == uid) {
                return true;
            }
        }
    } catch (...) {
        cout << "Something went wrong";
    }
    return false;
}

void Player::useUlepszenie(int id) {
    if (this->maUlepszenie(id)) {
        if (this->ulepszenia.at(id)->isEquipped())
            this->ulepszenia.at(id)->use(this->Gra);
    }
}

void Player::equipUlepszenie(int id) {
    if (this->maUlepszenie(id)) {
        this->ulepszenia.at(id)->toggleEquip();
    }
}