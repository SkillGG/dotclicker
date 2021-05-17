#ifndef header_name_h
#define header_name_h

#include <string>
#include <thread>
#include <vector>

int gamethread(int n);

enum GameState {
    GAME = 2,
    MENU = 1,
    START = 0
};

class Game;

class Player;

class Ulepszenie {
private:
    int cost;
    int id;

public:
    int getId();
    int getCost();
    virtual void buy(Game *p){};
    virtual void use(Game *p){};
    ~Ulepszenie(){};
    Ulepszenie(int id, int cost);
};

class Player {
private:
    int money;
    std::vector<Ulepszenie*> ulepszenia;
    Game *Gra;

public:
    int getMoney();
    int addMoney(int x);
    void kupUlepszenie(Ulepszenie *u);
    void useUlepszenie(Ulepszenie *u);
    bool maUlepszenie(Ulepszenie *u);
    Player(Game *g);
};

// Nowe ulepszenia tu:
class PodwojnePieniadze1 : public Ulepszenie {
public:
    void buy(Game *p);
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
    std::vector<Ulepszenie*> mozliweUlepszenia;
    Player *player;
    void start();
    Game();
    void userInput(std::string s);
    void stop();
};

#endif