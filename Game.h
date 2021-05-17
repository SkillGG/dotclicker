#ifndef header_name_h
#define header_name_h

#include <string>
#include <thread>
#include <vector>

/**
 * Stany, w jakich gra moze byc
 */
enum GameState {
    AUTORZY = 3,
    GAME = 2,
    MENU = 1,
    START = 0

};

// tzw. forward-reference, nie tykac
class Game;
class Player;

/**
 * Wszystkie ulepszenia musza miec:
 */
class Ulepszenie {
private:
    /** cena ulepszenia */
    int cost;
    /** id ulepszenia */
    int id;
    /** Czy jest wlaczone */
    bool equipped;

public:
    /** Daj ID */
    int getId();
    /** daj mi cene */
    int getCost();
    /** Czy jest zalozone */
    bool isEquipped() { return this->equipped; }
    /** zdejmij / zaloz ulepszenie */
    void toggleEquip() { this->equipped = !this->equipped; }
    /** co robi zaraz po kupieniu */
    virtual void buy(Game *p){};
    /** co robi przy uzyciu */
    virtual void use(Game *p){};
    /** Konstruktor */
    Ulepszenie(int id, int cost);
};

/**
 * Gracz
 */
class Player {
private:
    /** pieniadze gracza */
    int money;
    /** ulepszenia ktore posiada gracz */
    std::vector<Ulepszenie *> ulepszenia;
    /* dane calej gry */
    Game *Gra;

public:
    /** ile ma pieniedzy */
    int getMoney();
    /** Dodaj / Odejmin pieniedzy */
    int addMoney(int x);
    /** Sprawdz czy ma ulepszenie */
    bool maUlepszenie(int uid);
    /** Kup Ulepszenie */
    void kupUlepszenie(int uid);
    /** Uzyj ulepszenia */
    void useUlepszenie(int uid);
    /** Zaloz ulepszenie */
    void equipUlepszenie(int uid);
    /** Konstruktor */
    Player(Game *g);
};

// Nowe ulepszenia tu:
class PodwojnePieniadze1 : public Ulepszenie {
public:
    /** co robi tuz po kupnie */
    void buy(Game *p);
    /** co robi przy uzyciu */
    void use(Game *p);
    PodwojnePieniadze1(int cost);
};

class Game {
private:
    bool running;
    GameState state;

public:
    std::string lastInput;
    GameState getState();
    bool isRunning();
    void Logic();
    void Draw();
    std::vector<Ulepszenie *> mozliweUlepszenia;
    Player *player;
    void start();
    Game();
    void userInput(std::string s);
    void stop();
};

#endif
