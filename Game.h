#ifndef header_name_h
#define header_name_h

#include <map>
#include <string>
#include <vector>

/**
 * Stany, w jakich gra moze byc
 */
enum GameState {
    ULEPSZENIA = 4,
    AUTORZY = 3,
    GAME = 2,
    START = 0
};

enum UlepszeniaState {
    MAIN = 1,
    EQ = 2,
    KUP = 3
};

class Game;
class Ulepszenie;

/**
 * Gracz
 */
class Player {
   private:
    /** pieniadze gracza */
    unsigned int money;
    /** ulepszenia ktore posiada gracz */
    std::vector<Ulepszenie *> ulepszenia;
    /* dane calej gry */
    Game *Gra;
    /** Znaki, ktore mozna uzywac */
    std::map<std::string, int> characters;

   public:
    /** ile ma pieniedzy */
    unsigned int getMoney();
    unsigned int calculateBaseMoney(std::string s);
    std::map<std::string, int> getCharacters();
    /** Dodaj / Odejmin pieniedzy */
    void addMoney(int x);
    std::vector<Ulepszenie *> getUlepszenia() { return this->ulepszenia; }
    /** Sprawdz czy ma ulepszenie */
    bool maUlepszenie(int uid);
    /**Usuwa ulepszenie*/
    void removeUlepszenie(int uid);
    /** Kup Ulepszenie */
    bool kupUlepszenie(int uid);
    /** Zaloz ulepszenie */
    bool equipUlepszenie(int uid);
    /** Usun string do wpisania */
    void removeFeedableCharacter(std::string c);
    /** Dodaj znak, do wpisania */
    void addFeedableCharacter(std::string c, int i);
    /** Daj playerowi string */
    void feedString(std::string s);
    /** Konstruktor */
    Player(Game *g);
};

class Game {
   private:
    bool running;
    GameState state;
    UlepszeniaState ulstate;

   public:
    int pienionzki;
    bool kupnoSkrzynki;
    int nrSkrzynki;
    bool commandNotFoundError;
    bool notEnoughMoneyError;
    bool outOfRangeError;
    bool playerAlreadyBought;
    bool notIntegerError;
    std::string lastInput;
    Ulepszenie *getUlepszenie(int uid);
    GameState getState();
    UlepszeniaState getUlState();
    bool isRunning();
    void Draw();
    std::vector<Ulepszenie *> mozliweUlepszenia;
    Player *player;
    void start();
    Game();
    ~Game();
    void userInput(std::string s);
    void stop();
    bool rutek;
};

/// ============================ ULEPSZENIA

/**
 * Wszystkie ulepszenia musza miec:
 */
class Ulepszenie {
   private:
    /** cena ulepszenia */
    unsigned int cost;
    /** id ulepszenia */
    int id;
    /** Czy jest wlaczone */
    bool equipped;

   public:
    /** Daj ID */
    int getId();
    /** daj mi cene */
    unsigned int getCost();
    /** Czy jest zalozone */
    bool isEquipped() { return this->equipped; }
    /** zdejmij / zaloz ulepszenie */
    void toggleEquip(Game *g) {
        this->equipped = !this->equipped;
        if (this->equipped)
            this->equip(g);
        else
            this->unequip(g);
    }
    /** opis getter */
    virtual std::string getOpis() { return "Ulepszenie#" + std::__cxx11::to_string(this->id); };
    /** co robi zaraz po kupieniu */
    virtual void buy(Game *p){};
    /** co robi przy uzyciu */
    virtual int use(Game *p, std::string s, unsigned int bm) { return 0; };
    virtual void equip(Game *p){};
    /** co robi po zdjeciu */
    virtual void unequip(Game *p){};
    /** Konstruktor */
    Ulepszenie(int id, unsigned int cost);
    virtual ~Ulepszenie() = default;
};

class PodwojnePieniadze1 : public Ulepszenie {
   public:
    std::string getOpis();
    /** co robi tuz po kupnie */
    void buy(Game *p);
    /** co robi przy uzyciu */
    int use(Game *p, std::string s, unsigned int bm);
    void equip(Game *p);
    /** co robi przy unequip */
    void unequip(Game *p);
    PodwojnePieniadze1(int cost);
};

class Przecinek : public Ulepszenie {
   public:
    std::string getOpis();
    void buy(Game *p);
    int use(Game *p, std::string s, unsigned int bm);
    void equip(Game *p);
    void unequip(Game *p);
    Przecinek(int cost);
};

class Nawiasy : public Ulepszenie {
   public:
    std::string getOpis();
    void buy(Game *p);
    int use(Game *p, std::string s, unsigned int bm);
    void equip(Game *p);
    void unequip(Game *p);
    Nawiasy(int cost);
};

class NaszaKlasa : public Ulepszenie {
   public:
    std::string getOpis();
    void buy(Game *p);
    int use(Game *p, std::string s, unsigned int bm);
    void equip(Game *p);
    void unequip(Game *p);
    NaszaKlasa(int cost);
};

class Skrzynka : public Ulepszenie {
   public:
    std::string getOpis();
    void buy(Game *p);
    int use(Game *p, std::string s, unsigned int bm);
    void equip(Game *p);
    void unequip(Game *p);
    Skrzynka(int cost);
};
#endif
