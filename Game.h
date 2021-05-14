#include <thread>
#include <string>

int gamethread(int n);

class Game
{
    private:
    bool running;
public:
    bool isRunning();
    void start();
    Game();
    void userInput(std::string s);
    void stop();
};