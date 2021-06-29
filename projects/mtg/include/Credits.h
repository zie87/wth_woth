#ifndef _CREDITS_H_
#define _CREDITS_H_

#include <vector>
#include <string>
#include <JGE.h>
#include "WFont.h"
#include <time.h>
#include "Player.h"

class GameApp;
class DeckStats;

class Unlockable {
private:
    map<string, string> mValues;

public:
    Unlockable();
    void setValue(std::string, string);
    std::string getValue(std::string);
    bool isUnlocked();
    bool tryToUnlock(GameObserver* game);
    static void load();
    static map<string, Unlockable*> unlockables;
    static void Destroy();
};

class CreditBonus {
public:
    int value;
    std::string text;
    CreditBonus(int _value, std::string _text);
    void Render(float x, float y, WFont* font) const;
};

class Credits {
private:
    time_t gameLength;
    int isDifficultyUnlocked(DeckStats* stats);
    int isEvilTwinUnlocked() const;
    int isRandomDeckUnlocked() const;
    int IsMoreAIDecksUnlocked(DeckStats* stats);
    std::string unlockedTextureName;
    JQuadPtr GetUnlockedQuad(std::string texturename);

public:
    int value;
    Player *p1, *p2;
    GameObserver* observer;
    GameApp* app;
    int showMsg;
    int unlocked;
    std::string unlockedString;
    std::vector<CreditBonus*> bonus;
    Credits();
    ~Credits();
    void compute(GameObserver* observer, GameApp* _app);
    void Render();
    static int unlockRandomSet(bool force = false);
    static int unlockSetByName(std::string name);
    static int addCreditBonus(int value);
    static int addCardToCollection(int cardId, MTGDeck* collection);
    static int addCardToCollection(int cardId);
};

#endif
