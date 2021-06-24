#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "JGE.h"
#include "MTGGameZones.h"
#include "Damage.h"
#include "Targetable.h"

class MTGDeck;
class MTGPlayerCards;
class MTGInPlay;
class ManaPool;

class Player : public Damageable {
protected:
    ManaPool* manaPool;
    JTexture* mAvatarTex;
    JQuadPtr mAvatar;
    bool loadAvatar(string file, string resName = "playerAvatar");
    bool premade;

public:
    enum Mode { MODE_TEST_SUITE, MODE_HUMAN, MODE_AI };

    int deckId;
    std::string mAvatarName;
    Mode playMode;
    bool nomaxhandsize;
    MTGPlayerCards* game;
    MTGDeck* mDeck;
    std::string deckFile;
    std::string deckFileSmall;
    std::string deckName;
    std::string phaseRing;
    int offerInterruptOnPhase;
    int skippingTurn;
    int extraTurn;
    std::vector<MTGCardInstance*> curses;
    Player(GameObserver* observer, string deckFile, string deckFileSmall, MTGDeck* deck = NULL);
    virtual ~Player();
    virtual void setObserver(GameObserver* g);
    virtual void End();
    virtual int displayStack() { return 1; }
    const std::string getDisplayName() const;

    int afterDamage();

    int gainLife(int value);
    int loseLife(int value);
    int gainOrLoseLife(int value);

    bool isPoisoned() { return (poisonCount > 0); }
    int poisoned();
    int damaged();
    int prevented();
    void unTapPhase();
    MTGInPlay* inPlay();
    ManaPool* getManaPool();
    void takeMulligan();

    void cleanupPhase();
    virtual int Act(float dt) { return 0; }

    virtual int isAI() { return 0; }

    bool isHuman() { return (playMode == MODE_HUMAN); }

    Player* opponent();
    int getId();
    JQuadPtr getIcon();

    virtual int receiveEvent(WEvent* event) { return 0; }

    virtual void Render() {}

    /**
    ** Returns the path to the stats file of currently selected deck.
    */
    std::string GetCurrentDeckStatsFile();
    virtual bool parseLine(const string& s);
    friend std::ostream& operator<<(std::ostream&, const Player&);
    bool operator<(Player& aPlayer);
    bool isDead();
};

class HumanPlayer : public Player {
public:
    HumanPlayer(GameObserver* observer, string deckFile, string deckFileSmall, bool premade = false,
                MTGDeck* deck = NULL);
    void End();
    friend std::ostream& operator<<(std::ostream&, const HumanPlayer&);
};

#endif
