#include "Game.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

using namespace std;

bool isInt(const string &s) {
    if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
        return false;
    char *p;
    strtol(s.c_str(), &p, 10);
    return (*p == 0);
}

int checkCommand(string s1, string s2) {
    //convert s1 and s2 into lower case strings
    transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
    if (s1.compare(s2) == 0)
        return true; //The strings are same
    return false;    //not matched
}

Player::Player(Game *g) {
    this->money = 0;
    this->Gra = g;
    this->ulepszenia = {};
}

int Ulepszenie::getId() { return this->id; }
unsigned int Ulepszenie::getCost() { return this->cost; }

Ulepszenie::Ulepszenie(int i, unsigned int c) {
    this->id = i;
    this->cost = c;
}

/** Ustaw domyslne wartosci */
Game::Game() {
    // lista mozliwych ulepszen
    this->mozliweUlepszenia = {
        (Ulepszenie *)(new PodwojnePieniadze1(100)),
        (Ulepszenie *)(new UzycieSlowaOwoc(1500))};
    this->commandNotFoundError = false;
    this->notEnoughMoneyError = false;
    this->state = GameState::START;
    this->running = true;
    this->player = new Player(this);
    this->player->addFeedableCharacter(".", 1);
    this->player->addFeedableCharacter("debug", 10000);
    this->ulstate=UlepszeniaState::MAIN;
}

/** Zatrzymaj gre */
void Game::stop() { this->running = false; }
/** Sprawdz czy dziala gra */
bool Game::isRunning() { return this->running; }

int countOccurences(const string &str, const string &findStr) {

    const int step = findStr.size();
    int count(0);
    size_t pos(0);
    while ((pos = str.find(findStr, pos)) != std::string::npos) {
        pos += step;
        ++count;
    }
    return count;
}

UlepszeniaState Game::getUlState() { return this->ulstate; }
GameState Game::getState() { return this->state; }

Ulepszenie *Game::getUlepszenie(int uid) {
    for (Ulepszenie *ch : this->mozliweUlepszenia) {
        if (ch->getId() == uid)
            return ch;
    }
    return nullptr;
}

unsigned int Player::calculateBaseMoney(string s) {
    int base = 0;
    for (const auto c : this->characters) {
        string fr(c.first);
        int ile(countOccurences(s, fr));
        base += c.second * ile;
    }
    return base;
}

void Player::feedString(string s) {
    int basemoney = calculateBaseMoney(s);
    for (const auto f : this->ulepszenia) {
        if (f->isEquipped())
            basemoney += f->use(this->Gra, s, basemoney);
    }
    this->addMoney(basemoney);
}

/**
 * To jest funkcja ktora czyta userInput i ustawia rozne rzeczy zgodnie z nim
 */
void Game::userInput(string s) {
    this->lastInput = s;
    if (checkCommand("quit", s) || checkCommand("q", s)) {
        this->stop();
    }
    switch (this->getState()) {

    case GameState::START:
        // What to draw in START
        if (checkCommand("menu", s) || checkCommand("m", s)) {
            this->state = GameState::MENU;
        } else if (checkCommand("autorzy", s) || checkCommand("a", s)) {
            this->state = GameState::AUTORZY;
        } else
            this->commandNotFoundError = true;
        break;

    case GameState::MENU:
        // What to draw in MENU
        if (checkCommand("powrot", s) || checkCommand("p", s)) {
            this->state = GameState::START;
        } else if (checkCommand("start", s)) {
            this->state = GameState::GAME;
        } else
            this->commandNotFoundError = true;
        break;

    case GameState::GAME:
        // What to draw in GAME
        if (checkCommand("menu", s)) {
            this->state = GameState::MENU;
        } else if (checkCommand("ulepszenia", s) || checkCommand("u", s)) {
            this->state = GameState::ULEPSZENIA;
        } else {
            this->player->feedString(s);
        }
        break;
    case GameState::AUTORZY:
        //// What to draw in AUTORZY
        if (checkCommand("powrot", s) || checkCommand("p", s)) {
            this->state = GameState::START;
        } else {
            this->commandNotFoundError = true;
        }
        break;
    case GameState::ULEPSZENIA:
        //// What to draw in ULEPSZENIA
        switch (this->ulstate) {
        case KUP:
            if (checkCommand("p", s) || checkCommand("powrot", s)) {
                this->ulstate = UlepszeniaState::MAIN;
            } else if (isInt(s)) {
                int sint = atoi(s.c_str());
                bool jestUl = false;
                for (Ulepszenie *u : this->mozliweUlepszenia) {
                    if (u->getId() == sint) {
                        jestUl = true;
                        break;
                    }
                }
                if (jestUl) {
                    if (!this->player->kupUlepszenie(atoi(s.c_str()))) {
                        this->notEnoughMoneyError = true;
                    }
                } else {
                    this->outOfRangeError = true;
                }
            } else {
                this->notIntegerError = true;
            }
            break;
        case EQ:
            if (checkCommand("p", s) || checkCommand("powrot", s)) {
                this->ulstate = UlepszeniaState::MAIN;
            } else if (isInt(s)) {
                int sint = atoi(s.c_str());
                bool jestUl = false;
                for (Ulepszenie *u : this->player->getUlepszenia()) {
                    if (u->getId() == sint) {
                        u->toggleEquip(this);
                        jestUl = true;
                        break;
                    }
                }
                if (!jestUl) {
                    this->outOfRangeError = true;
                }
            } else
                this->notIntegerError = true;
            break;
        case MAIN:
            if (checkCommand("p", s) || checkCommand("powrot", s)) {
                this->state = GameState::GAME;
            } else if (checkCommand("b", s) || checkCommand("kup", s)) {
                this->ulstate = UlepszeniaState::KUP;
            } else if (checkCommand("e", s) || checkCommand("eq", s) || checkCommand("ekwipunek", s)) {
                this->ulstate = UlepszeniaState::EQ;
            }
            break;
        default:
            break;
        }
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

    vector<string> m, s, g, u, a;
    string lits = "\n";
    string ulepszeniaString = "";
    pair<char, int> chr;
    const string separator = "==============================================================================================================";
    switch (this->getState()) {
    case START:
        // What to draw in START
        s = {
            separator,
            "",
            "",
            "                             ____   ___ _____    ____ _     ___ ____ _  _______ ____",
            "                            |  _ \\ / _ \\_   _|  / ___| |   |_ _/ ___| |/ / ____|  _ \\",
            "                            | | | | | | || |   | |   | |    | | |   | ' /|  _| | |_) |",
            "                            | |_| | |_| || |   | |___| |___ | | |___| . \\| |___|  _ <",
            "                            |____/ \\___/ |_|    \\____|_____|___\\____|_|\\_\\_____|_| \\_\\",
            "",
            "",
            "",
            "",
            "",
            "                                             < Menu      [ M ] >",
            "",
            "                                             < Autorzy   [ A ] >",
            "",
            "                                             < Wyjscie   [ Q ] >",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            separator};
        if (this->commandNotFoundError) {
            this->commandNotFoundError = false;
            u.insert(u.end(), {"", "Nie znaleziono komendy!", "", separator});
        }
        DrawFromVector(s);
        break;
    case MENU:
        // What to draw in MENU
        m = {
            separator,
            "",
            "",
            "                                          __  __   ___   _  _   _   _ ",
            "                                         |  \\/  | | __| | \\| | | | | |",
            "                                         | |\\/| | | _|  | .` | | |_| |",
            "                                         |_|  |_| |___| |_|\\_|  \\___/",
            "",
            "",
            "",
            "",
            "",
            "                                         < Start Game [ S ] >",
            "",
            "                                         < Powrot [ P ] >",
            "",
            "                                         < Wyjscie [ Q ] >",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            separator};
        if (this->commandNotFoundError) {
            this->commandNotFoundError = false;
            u.insert(u.end(), {"", "Nie znaleziono komendy!", "", separator});
        }
        DrawFromVector(m);
        break;
    case AUTORZY:
        a = {
            separator,
            "",
            "",
            "                                    ___   __  ____________  ____  _______  __",
            "                                   /   | / / / /_  __/ __ \\/ __ \\/__  /\\ \\/ /",
            "                                  / /| |/ / / / / / / / / / /_/ /  / /  \\  /",
            "                                 / ___ / /_/ / / / / /_/ / _, _/  / /__ / /",
            "                                /_/  |_\\____/ /_/  \\____/_/ |_|  /____//_/",
            "",
            "",
            "",
            "",
            "",
            "                                   Marcin Majewski = Glowny Programista",
            "",
            "                                   Karol Salacinski = Ulepszenia i Dodatki do Gry",
            "",
            "                                   Maciej Gawin = Ulepszenia i Dodatki do Gry",
            "",
            "                                   Krystian Sokolowski = Beater i Poprawa bledow",
            "",
            "                                   Szymon Sloniowski = Interfejs Gry",
            "",
            "",
            "",
            "",
            " < Powrot [ P ] >                                                                          < Wyjscie  [ Q ] >",
            separator};
        DrawFromVector(a);
        break;
    case GAME:
        // What to draw in GAME
        for (pair<string, int> chr : this->player->getCharacters()) {
            lits += "'" + chr.first + "'" + " warte: " + to_string(chr.second) + "$\n";
        }
        g = {
            separator,
            "Pieniadze: " + to_string(this->player->getMoney()) + "$",
            "Ulepszenia: " + ulepszeniaString + "(wpisz 'ulepszenia' by otoworzyc menu ulepszen)",
            "Literki:" + lits,
            separator};
        DrawFromVector(g);
        break;
    case ULEPSZENIA:
        u = {
            separator,
            "USTAWIENIA ULEPSZEN",
            separator};
        switch (this->ulstate) {
        case KUP:
            u.insert(u.end(), {"SKLEP:", "(wpisz ID ulepszenia aby kupic)", separator});
            for (const auto ulep : this->mozliweUlepszenia) {
                if (!this->player->maUlepszenie(ulep->getId())) {
                    string xx(to_string(ulep->getId()) + " ($" + to_string(ulep->getCost()) + ") - " + ulep->getOpis());
                    u.insert(u.end(), xx);
                }
            }
            u.insert(u.end(), separator);
            if (this->notEnoughMoneyError) {
                this->notEnoughMoneyError = false;
                u.insert(u.end(), {separator, "ZA MALO PIENIEDZY!", separator});
            }
            if (this->notIntegerError) {
                this->notIntegerError = false;
                u.insert(u.end(), {separator, "ID ULEPSZENIA POWINNO BYC NUMEREM!", separator});
            }
            if (this->outOfRangeError) {
                this->outOfRangeError = false;
                u.insert(u.end(), {separator, "ULEPSZENIE O TAKIM ID NIE ISTNIEJE!", separator});
            }
            break;
        case EQ:
            u.insert(u.end(), {"KUPIONE ULEPSZENIA:", "Wpisz ID ulepszenia aby wylaczyc/wlaczyc", separator});
            for (const auto ulep : this->player->getUlepszenia()) {
                u.insert(u.end(), to_string(ulep->getId()) + " (" + (ulep->isEquipped() ? "ON" : "OFF") + ") - " + ulep->getOpis());
            }
            u.insert(u.end(), separator);
            if (this->notIntegerError) {
                this->notIntegerError = false;
                u.insert(u.end(), {separator, "ID ULEPSZENIA POWINNO BYC NUMEREM!", separator});
            }
            if (this->outOfRangeError) {
                this->outOfRangeError = false;
                u.insert(u.end(), {separator, "NIE MASZ KUPIONEGO ULEPSZENIA O TAKIM ID!", separator});
            }
            break;
        case MAIN:
            u.insert(u.end(), {"KUP (b) - wejdz do sklepu", "EQ (e) - zarzadzaj ulepszeniami", separator});
            break;
        default:
            cout << (int)this->ulstate;
        }
        u.insert(u.end(), {"Wpisz powrot(p) aby wyjsc", separator});
        DrawFromVector(u);
        break;
    default:
        // grrrr, error
        cout << "Something went wrong!";
        this->stop();
        break;
    }
}

unsigned int Player::getMoney() { return this->money; }

void Player::addMoney(int i) { this->money += i; }

bool Player::kupUlepszenie(int uid) {
    Ulepszenie *u = this->Gra->getUlepszenie(uid);
    if (this->getMoney() > u->getCost()) {
        this->money -= u->getCost();
        this->ulepszenia.insert(this->ulepszenia.end(), u);
        u->buy(this->Gra);
        u->equip(this->Gra);
        return true;
    } else
        return false;
    return false;
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

void Player::equipUlepszenie(int id) {
    if (this->maUlepszenie(id)) {
        this->ulepszenia.at(id)->toggleEquip(this->Gra);
    }
}

void Player::removeFeedableCharacter(string s) {
    auto it = this->characters.find(s);
    this->characters.erase(it);
}
void Player::addFeedableCharacter(string c, int i) { this->characters.insert(pair<string, int>(c, i)); }
map<string, int> Player::getCharacters() { return this->characters; }

// =========================== ULEPSZENIA

PodwojnePieniadze1::PodwojnePieniadze1(int cost) : Ulepszenie::Ulepszenie(1, cost) {}

std::string PodwojnePieniadze1::getOpis() { return "Podwaja ilosc pieniedzy"; }
int PodwojnePieniadze1::use(Game *g, string s, unsigned int bm) {
    // dodaj drugie tyle pieniedzy
    return bm;
}
void PodwojnePieniadze1::buy(Game *g) {}
void PodwojnePieniadze1::equip(Game *g) {}
void PodwojnePieniadze1::unequip(Game *g) {}

UzycieSlowaOwoc::UzycieSlowaOwoc(int cost) : Ulepszenie::Ulepszenie(2, cost) {}
std::string UzycieSlowaOwoc::getOpis() { return "Pozwala na uzycie slowa 'owoc'"; }
int UzycieSlowaOwoc::use(Game *g, string s, unsigned int bm) { return 0; /* nie dodawaj zadnej kasy */ }
void UzycieSlowaOwoc::buy(Game *g) {}
void UzycieSlowaOwoc::equip(Game *g) {
    g->player->addFeedableCharacter("owoc", 15);
}
void UzycieSlowaOwoc::unequip(Game *g) {
    g->player->removeFeedableCharacter("owoc");
}
