#ifndef _GAME_STATE_H_
#define _GAME_STATE_H_

#define FADING_SPEED 350.0f

class JGE;

#include <JSoundSystem.h>
#include <string>
#include <vector>

#include "DeckMetaData.h"
#include "DeckMenu.h"

enum ENUM_GAME_STATE {
    GAME_STATE_NONE = -1,
    GAME_STATE_MENU = 1,
    GAME_STATE_DUEL = 2,
    GAME_STATE_DECK_VIEWER = 3,
    GAME_STATE_SHOP = 4,
    GAME_STATE_OPTIONS = 5,
    GAME_STATE_AWARDS = 6,
    GAME_STATE_STORY = 7,
    GAME_STATE_TRANSITION = 8,
    GAME_STATE_MAX = 9,
};

enum ENUM_GS_TRANSITION { TRANSITION_FADE = 0, TRANSITION_FADE_IN = 1, MAX_TRANSITION };

class GameApp;
class SimpleMenu;
class Player;

class GameState {
protected:
    GameApp* mParent;
    JGE* mEngine;
    std::string mStringID;

public:
    GameState(GameApp* parent, std::string id);
    virtual ~GameState() {}

    virtual void Create() {}
    virtual void Destroy() {}

    virtual void Start() {}
    virtual void End() {}

    virtual void OnScroll(int inXVelocity, int inYVelocity) {}

    virtual void Update(float dt) = 0;
    virtual void Render() = 0;

    std::string getStringID() { return mStringID; };

    // deck manipulation methods
    // 2010/09/15:
    // this was originally one method to do everything.  That has been split up into two distinct
    // methods since the original was building a menu and returning a value.  The first
    // creates the std::vector containing the deck information.  The second will render that information
    // it makes it easier to manipulate the deck information menus.

    // generate the Deck Meta Data and build the menu items of the menu given
    static std::vector<DeckMetaData*> fillDeckMenu(SimpleMenu* _menu, const std::string& path,
                                              const std::string& smallDeckPrefix = "", Player* statsPlayer = NULL);

    // generate the Deck Meta Data and build the menu items of the menu given
    // Will display up to maxDecks if maxDecks is non 0,all decks in path otherwise
    static std::vector<DeckMetaData*> fillDeckMenu(DeckMenu* _menu, const std::string& path, const std::string& smallDeckPrefix = "",
                                              Player* statsPlayer = NULL, int maxDecks = 0);

    // build a std::vector of decks with the information passsed in.
    static std::vector<DeckMetaData*> BuildDeckList(const std::string& path, const std::string& smallDeckPrefix = "",
                                               Player* statsPlayer = NULL, int maxDecks = 0);

    // build menu items based on the std::vector<DeckMetaData *>
    static void renderDeckMenu(SimpleMenu* _menu, const std::vector<DeckMetaData*>& deckMetaDataList);

    // build menu items based on the std::vector<DeckMetaData *>
    static void renderDeckMenu(DeckMenu* _menu, const std::vector<DeckMetaData*>& deckMetaDataList);
};
bool sortByName(DeckMetaData* d1, DeckMetaData* d2);

#endif
