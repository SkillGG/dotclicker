#include "Game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

// ============================= HELPER FUNCTIONS

bool isInt(const string &s) {
    if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
        return false;
    char *p;
    strtol(s.c_str(), &p, 10);
    return (*p == 0);
}

string centerOnScreen(string s) {
    size_t l = s.length();
    string r = "";
    for (size_t p = 0; p < (110 - l) / 2; p++)
        r += " ";
    return r + s;
}

string putOnRight(string s) {
    while (s.length() != 110) {
        s = " " + s;
    }
    return s;
}

int checkCommand(string s1, string s2) {
    //convert s1 and s2 into lower case strings
    transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
    transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
    if (s1.compare(s2) == 0)
        return true;  //The strings are same
    return false;     //not matched
}

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

void DrawFromVector(vector<string> v) {
    for (size_t i = 0; i < v.size(); i++) {
        cout << v.at(i) << endl;
    }
}

// =========================== GAME

/** Ustaw domyslne wartosci */
Game::Game() {
    // lista mozliwych ulepszen
    this->mozliweUlepszenia = {
        (Ulepszenie *)(new Skrzynka(1000)),
        (Ulepszenie *)(new Przecinek(200))};
    this->kupnoSkrzynki = false;
    this->pienionzki = 0;
    this->nrSkrzynki = 0;
    this->commandNotFoundError = false;
    this->notIntegerError = false;
    this->outOfRangeError = false;
    this->notEnoughMoneyError = false;
    this->playerAlreadyBought = false;
    this->lastInput = "";
    this->state = GameState::START;
    this->running = true;
    this->player = new Player(this);
    this->player->addFeedableCharacter(".", 1);
    this->ulstate = UlepszeniaState::MAIN;
    this->rutek = false;
    srand(time(NULL));
}

Game::~Game() {
    delete this->player;
    for (auto n : this->mozliweUlepszenia)
        delete n;
}

/** Zatrzymaj gre */
void Game::stop() { this->running = false; }
/** Sprawdz czy dziala gra */
bool Game::isRunning() { return this->running; }

UlepszeniaState Game::getUlState() { return this->ulstate; }
GameState Game::getState() { return this->state; }

Ulepszenie *Game::getUlepszenie(int uid) {
    for (Ulepszenie *ch : this->mozliweUlepszenia) {
        if (ch->getId() == uid)
            return ch;
    }
    return nullptr;
}

/**
 * To jest funkcja ktora czyta userInput i ustawia rozne rzeczy zgodnie z nim
 */
void Game::userInput(string s) {
    this->lastInput = s;
    if (checkCommand("quit", s) || checkCommand("q", s))
        this->stop();
    switch (this->getState()) {
        case GameState::START:
            // What to draw in START
            if (checkCommand("autorzy", s) || checkCommand("a", s))
                this->state = GameState::AUTORZY;
            else if (checkCommand("start", s) || (checkCommand("s", s)))
                this->state = GameState::GAME;
            else
                this->commandNotFoundError = true;
            break;
        case GameState::GAME:
            // What to draw in GAME
            if (checkCommand("menu", s))
                this->state = GameState::START;
            else if (checkCommand("ulepszenia", s) || checkCommand("u", s))
                this->state = GameState::ULEPSZENIA;
            else if (checkCommand("powrot", s) || checkCommand("p", s))
                this->state = GameState::START;
            else
                this->player->feedString(s);

            break;
        case GameState::AUTORZY:
            //// What to draw in AUTORZY
            if (checkCommand("powrot", s) || checkCommand("p", s))
                this->state = GameState::START;
            else
                this->commandNotFoundError = true;

            break;
        case GameState::ULEPSZENIA:
            //// What to draw in ULEPSZENIA
            switch (this->ulstate) {
                case KUP:
                    if (checkCommand("p", s) || checkCommand("powrot", s))
                        this->ulstate = UlepszeniaState::MAIN;
                    else if (isInt(s)) {
                        int sint = atoi(s.c_str());
                        bool jestUl = false;
                        for (Ulepszenie *u : this->mozliweUlepszenia) {
                            if (u->getId() == sint) {
                                jestUl = true;
                                break;
                            }
                        }
                        if (jestUl) {
                            if (this->player->maUlepszenie(atoi(s.c_str())))
                                this->playerAlreadyBought = true;
                            else if (!this->player->kupUlepszenie(atoi(s.c_str())))
                                this->notEnoughMoneyError = true;
                        } else
                            this->outOfRangeError = true;
                    } else
                        this->notIntegerError = true;
                    break;
                case EQ:
                    if (checkCommand("p", s) || checkCommand("powrot", s)) {
                        this->ulstate = UlepszeniaState::MAIN;
                    } else if (isInt(s)) {
                        int sint = atoi(s.c_str());
                        if (!this->player->equipUlepszenie(sint))
                            this->outOfRangeError = true;
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
                    } else
                        this->commandNotFoundError = true;
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

/**
 * To jest funkcja do wypisywania tekstu na ekranie
 * Jest to funkcja, ktora ma wyswietlac tekst zaleznie od wybranego stanu
 * */
void Game::Draw() {
    vector<string> m, s, g, u, a;
    string lits = "\n";
    const string controls = centerOnScreen("< Powrot [ P ] >                                                                        < Wyjscie  [ Q ] >");
    string ulepszeniaString = "";
    pair<char, int> chr;
    size_t i = 0;
    const string separator = "==============================================================================================================";
    const string nieZnaleziono = centerOnScreen("NIE MA TAKIEJ KOMENDY!");
    switch (this->getState()) {
        case START:
            // What to draw in START
            s = {
                separator,
                putOnRight("Ver. 1.0.2b"),
                "",
                centerOnScreen(" /$$$$$$$              /$$                     /$$ /$$           /$$                          "),
                centerOnScreen("| $$__  $$            | $$                    | $$|__/          | $$                          "),
                centerOnScreen("| $$  \\ $$  /$$$$$$  /$$$$$$          /$$$$$$$| $$ /$$  /$$$$$$$| $$   /$$  /$$$$$$   /$$$$$$ "),
                centerOnScreen("| $$  | $$ /$$__  $$|_  $$_/         /$$_____/| $$| $$ /$$_____/| $$  /$$/ /$$__  $$ /$$__  $$"),
                centerOnScreen("| $$  | $$| $$  \\ $$  | $$          | $$      | $$| $$| $$      | $$$$$$/ | $$$$$$$$| $$  \\__/"),
                centerOnScreen("| $$  | $$| $$  | $$  | $$ /$$      | $$      | $$| $$| $$      | $$_  $$ | $$_____/| $$      "),
                centerOnScreen("| $$$$$$$/|  $$$$$$/  |  $$$$/      |  $$$$$$$| $$| $$|  $$$$$$$| $$ \\  $$|  $$$$$$$| $$      "),
                centerOnScreen("|_______/  \\______/    \\___/         \\_______/|__/|__/ \\_______/|__/  \\__/ \\_______/|__/      "),
                "",
                "",
                "",
                centerOnScreen("< Start Game     [ S ] >"),
                "",
                centerOnScreen("< Autorzy        [ A ] >"),
                "",
                centerOnScreen("< Wyjscie        [ Q ] >"),
                "",
                "",
                "",
                "",
                "",
                "",
                separator};
            if (this->commandNotFoundError) {
                this->commandNotFoundError = false;
                s.insert(s.end(), {nieZnaleziono, separator});
            } else {
                s.insert(s.end() - 2, {"", ""});
            }
            DrawFromVector(s);
            break;
        case AUTORZY:
            a = {
                separator,
                "",
                "",
                centerOnScreen("  /$$$$$$              /$$                                            "),
                centerOnScreen(" /$$__  $$            | $$                                            "),
                centerOnScreen("| $$  \\ $$ /$$   /$$ /$$$$$$    /$$$$$$   /$$$$$$  /$$$$$$$$ /$$   /$$"),
                centerOnScreen("| $$$$$$$$| $$  | $$|_  $$_/   /$$__  $$ /$$__  $$|____ /$$/| $$  | $$"),
                centerOnScreen("| $$__  $$| $$  | $$  | $$    | $$  \\ $$| $$  \\__/   /$$$$/ | $$  | $$"),
                centerOnScreen("| $$  | $$| $$  | $$  | $$ /$$| $$  | $$| $$        /$$__/  | $$  | $$"),
                centerOnScreen("| $$  | $$|  $$$$$$/  |  $$$$/|  $$$$$$/| $$       /$$$$$$$$|  $$$$$$$"),
                centerOnScreen("|__/  |__/ \\______/    \\___/   \\______/ |__/      |________/ \\____  $$"),
                centerOnScreen("                                                             /$$  | $$"),
                centerOnScreen("                                                            |  $$$$$$/"),
                centerOnScreen("                                                             \\______/ "),
                "                                Marcin Majewski = Glowny Programista",
                "",
                "                                Karol Salacinski = Ulepszenia i Dodatki do Gry",
                "",
                "                                Maciej Gawin = Ulepszenia i Dodatki do Gry",
                "",
                "                                Krystian Sokolowski = Beater i Poprawa bledow",
                "",
                "                                Szymon Sloniowski = Interfejs Gry",
                "",
                controls,
                separator};
            if (this->commandNotFoundError) {
                this->commandNotFoundError = false;
                a.insert(a.end(), {nieZnaleziono, separator});
            } else {
                a.insert(a.end() - 2, {"", ""});
            }
            DrawFromVector(a);
            break;
        case GAME:
            // What to draw in GAME
            for (pair<string, int> chr : this->player->getCharacters()) {
                lits += "'" + chr.first + "'" + " warte: " + to_string(chr.second) + "$\n";
                i++;
            }
            g = {
                separator,
                "",
                centerOnScreen("Pieniadze: " + to_string(this->player->getMoney()) + "$"),
                "",
                centerOnScreen("< Ulepszenia     [ U ] >"),
                "",
                separator,
                centerOnScreen("// Dostepne Slowa \\\\"),
                lits};
            while (g.size() < 25 - i)
                g.insert(g.end(), "");
            g.insert(g.end(), {controls, separator});
            DrawFromVector(g);
            break;
        case ULEPSZENIA:
            u = {
                separator + "\n",
                "",
                centerOnScreen(" /$$   /$$ /$$                                                             /$$          "),
                centerOnScreen("| $$  | $$| $$                                                            |__/          "),
                centerOnScreen("| $$  | $$| $$  /$$$$$$   /$$$$$$   /$$$$$$$ /$$$$$$$$  /$$$$$$  /$$$$$$$  /$$  /$$$$$$ "),
                centerOnScreen("| $$  | $$| $$ /$$__  $$ /$$__  $$ /$$_____/|____ /$$/ /$$__  $$| $$__  $$| $$ |____  $$"),
                centerOnScreen("| $$  | $$| $$| $$$$$$$$| $$  \\ $$|  $$$$$$    /$$$$/ | $$$$$$$$| $$  \\ $$| $$  /$$$$$$$"),
                centerOnScreen("| $$  | $$| $$| $$_____/| $$  | $$ \\____  $$  /$$__/  | $$_____/| $$  | $$| $$ /$$__  $$"),
                centerOnScreen("|  $$$$$$/| $$|  $$$$$$$| $$$$$$$/ /$$$$$$$/ /$$$$$$$$|  $$$$$$$| $$  | $$| $$|  $$$$$$$"),
                centerOnScreen(" \\______/ |__/ \\_______/| $$____/ |_______/ |________/ \\_______/|__/  |__/|__/ \\_______/"),
                centerOnScreen("                        | $$                                                            "),
                centerOnScreen("                        |__/                                                            "),
                "",
                ""};
            switch (this->ulstate) {
                case KUP:
                    u.insert(u.end(), {
                                          centerOnScreen("/\\/SKLEP\\/\\"),
                                          "",
                                      });
                    for (const auto ulep : this->mozliweUlepszenia) {
                        if (!this->player->maUlepszenie(ulep->getId())) {
                            string xx(to_string(ulep->getId()) + "  [ $" + to_string(ulep->getCost()) + " ] - " + ulep->getOpis());
                            u.insert(u.end(), xx);
                        }
                    }
                    while (u.size() < 24 - ((this->playerAlreadyBought || this->kupnoSkrzynki || this->commandNotFoundError || this->notEnoughMoneyError || this->notIntegerError || this->outOfRangeError) ? 2 : 0))
                        u.insert(u.end(), "");
                    u.insert(u.end(), centerOnScreen("Wpisz numer Ulepszenia aby Kupic"));
                    break;
                case EQ:
                    u.insert(u.end(), {centerOnScreen("/\\/EKWIPUNEK\\/\\"), ""});
                    for (const auto ulep : this->player->getUlepszenia()) {
                        u.insert(u.end(), to_string(ulep->getId()) + " (" + (ulep->isEquipped() ? "ON" : "OFF") + ") - " + ulep->getOpis());
                    }
                    while (u.size() < 24 - ((this->playerAlreadyBought || this->kupnoSkrzynki || this->commandNotFoundError || this->notIntegerError || this->outOfRangeError) ? 2 : 0))
                        u.insert(u.end(), "");
                    u.insert(u.end(), centerOnScreen("Wpisz numer Ulepszenia aby Wlaczyc lub Wylaczyc"));
                    break;
                case MAIN:
                    u.insert(u.end(), {"",
                                       centerOnScreen("< Kup Ulepszenia [ B ] >"),
                                       "",
                                       centerOnScreen("< Ekwipunek      [ E ] >"),
                                       "",
                                       "",
                                       "",
                                       "",
                                       ""});
                    if (!this->commandNotFoundError) {
                        u.insert(u.end(), {"", ""});
                    }
                    break;
                default:
                    cout << (int)this->ulstate;
            }
            u.insert(u.end(), {controls, separator});
            if (this->notEnoughMoneyError) {
                this->notEnoughMoneyError = false;
                u.insert(u.end(), {centerOnScreen("ZA MALO PIENIEDZY!"), separator});
            }
            if (this->notIntegerError && this->ulstate == UlepszeniaState::KUP) {
                this->notIntegerError = false;
                u.insert(u.end(), {centerOnScreen("ID ULEPSZENIA POWINNO BYC CYFRA!"), separator});
            }
            if (this->notIntegerError && this->ulstate == UlepszeniaState::EQ) {
                this->notIntegerError = false;
                u.insert(u.end(), {centerOnScreen("ID ULEPSZENIA POWINNO BYC NUMEREM!"), separator});
            }
            if (this->outOfRangeError && this->ulstate == UlepszeniaState::KUP) {
                this->outOfRangeError = false;
                u.insert(u.end(), {centerOnScreen("ULEPSZENIE O TAKIM ID NIE ISTNIEJE!"), separator});
            }
            if (this->outOfRangeError && this->ulstate == UlepszeniaState::EQ) {
                this->outOfRangeError = false;
                u.insert(u.end(), {centerOnScreen("NIE MASZ KUPIONEGO ULEPSZENIA O TAKIM ID!"), separator});
            }
            if (this->playerAlreadyBought) {
                this->playerAlreadyBought = false;
                u.insert(u.end(), {centerOnScreen("JUZ KUPIONO ULEPSZENIE O TAKIM ID!"), separator});
            }
            if (this->commandNotFoundError) {
                this->commandNotFoundError = false;
                u.insert(u.end(), {nieZnaleziono, separator});
            }
            if (this->kupnoSkrzynki) {
                this->kupnoSkrzynki = false;
                u.insert(u.end(), {centerOnScreen("DROP #" + to_string(this->nrSkrzynki) + ": " + to_string(this->pienionzki) + "$"), separator});
            }
            DrawFromVector(u);
            break;
        default:
            // grrrr, error
            cout << "Something went wrong!";
            this->stop();
            break;
    }
}

// =================== PLAYER

Player::Player(Game *g) {
    this->money = 0;
    this->Gra = g;
    this->ulepszenia = {};
}

void Player::removeUlepszenie(int uid) {
    for (size_t i = 0; i < this->ulepszenia.size(); i++) {
        if (this->ulepszenia.at(i)->getId() == uid)
            this->ulepszenia.erase(this->ulepszenia.begin() + i);
    }
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
    if (checkCommand("rutek", s) && !this->Gra->rutek) {
        this->addMoney(213742069);
        this->Gra->rutek = true;
        return;
    }
    int basemoney = calculateBaseMoney(s);
    for (const auto f : this->ulepszenia) {
        if (f->isEquipped())
            basemoney += f->use(this->Gra, s, basemoney);
    }
    this->addMoney(basemoney);
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

bool Player::equipUlepszenie(int id) {
    if (this->maUlepszenie(id)) {
        this->Gra->getUlepszenie(id)->toggleEquip(this->Gra);
        return true;
    }
    return false;
}

void Player::removeFeedableCharacter(string s) {
    auto it = this->characters.find(s);
    this->characters.erase(it);
}
void Player::addFeedableCharacter(string c, int i) { this->characters.insert(pair<string, int>(c, i)); }
map<string, int> Player::getCharacters() { return this->characters; }

// =========================== ULEPSZENIA

int Ulepszenie::getId() { return this->id; }
unsigned int Ulepszenie::getCost() { return this->cost; }

Ulepszenie::Ulepszenie(int i, unsigned int c) {
    this->id = i;
    this->cost = c;
    this->equipped = false;
}

PodwojnePieniadze1::PodwojnePieniadze1(int cost) : Ulepszenie::Ulepszenie(5, cost) {}

std::string PodwojnePieniadze1::getOpis() { return "Podwaja ilosc pieniedzy"; }
int PodwojnePieniadze1::use(Game *g, string s, unsigned int bm) {
    // dodaj drugie tyle pieniedzy
    return bm;
}
void PodwojnePieniadze1::buy(Game *g) {
    this->toggleEquip(g);
}
void PodwojnePieniadze1::equip(Game *g) {}
void PodwojnePieniadze1::unequip(Game *g) {}

Przecinek::Przecinek(int cost) : Ulepszenie::Ulepszenie(2, cost) {}
std::string Przecinek::getOpis() { return "Pozwala na uzycie przecinka"; }
int Przecinek::use(Game *g, string s, unsigned int bm) { return 0; }
void Przecinek::buy(Game *g) {
    this->toggleEquip(g);
    g->mozliweUlepszenia.insert(g->mozliweUlepszenia.begin() + this->getId() - 1, (Ulepszenie *)new Nawiasy{1000});
}
void Przecinek::equip(Game *g) {
    g->player->addFeedableCharacter(",", 5);
}
void Przecinek::unequip(Game *g) {
    g->player->removeFeedableCharacter(",");
}

Nawiasy::Nawiasy(int cost) : Ulepszenie::Ulepszenie(3, cost) {}
std::string Nawiasy::getOpis() { return "Pozwala na uzycie Nawiasow()"; }
int Nawiasy::use(Game *g, string s, unsigned int bm) { return 0; }
void Nawiasy::buy(Game *g) {
    this->toggleEquip(g);
    g->mozliweUlepszenia.insert(g->mozliweUlepszenia.begin() + this->getId() - 1, (Ulepszenie *)new NaszaKlasa{2000});
}
void Nawiasy::equip(Game *g) {
    g->player->addFeedableCharacter("()", 20);
}
void Nawiasy::unequip(Game *g) {
    g->player->removeFeedableCharacter("()");
}

NaszaKlasa::NaszaKlasa(int cost) : Ulepszenie::Ulepszenie(4, cost) {}
std::string NaszaKlasa::getOpis() { return "Pozwala na uzycie 2tb"; }
int NaszaKlasa::use(Game *g, string s, unsigned int bm) { return 0; }
void NaszaKlasa::buy(Game *g) {
    this->toggleEquip(g);
    g->mozliweUlepszenia.insert(g->mozliweUlepszenia.begin() + this->getId() - 1, (Ulepszenie *)new PodwojnePieniadze1{50000});
}
void NaszaKlasa::equip(Game *g) {
    g->player->addFeedableCharacter("2tb", 50);
}
void NaszaKlasa::unequip(Game *g) {
    g->player->removeFeedableCharacter("2tb");
}

Skrzynka::Skrzynka(int cost) : Ulepszenie::Ulepszenie(1, cost) {}
std::string Skrzynka::getOpis() { return "Skrzynka z losowym dropem (20$, 500$, 1000$, a moze i wiecej...)"; }
int Skrzynka::use(Game *g, string s, unsigned int bm) { return 0; }
void Skrzynka::buy(Game *g) {
    float drop = (rand() % 10000 + 1) / 100.0;
    g->kupnoSkrzynki = true;
    int p = 0;
    cout << drop << endl;
    if (drop < 35) {
        p = 20;

    } else if (drop < 65 && drop >= 35) {
        p = 500;

    } else if (drop < 85 && drop >= 65) {
        p = 1000;

    } else if (drop < 95 && drop >= 85) {
        p = 2000;

    } else if (drop < 99.9 && drop >= 85) {
        p = 4000;

    } else {
        p = 2000000;
    }
    g->player->addMoney(p);
    g->pienionzki = p;
    g->nrSkrzynki++;
    g->player->removeUlepszenie(this->getId());
}

void Skrzynka::equip(Game *g) {}
void Skrzynka::unequip(Game *g) {}
