#ifndef _MANACOST_H_
#define _MANACOST_H_

#include "utils.h"
#include "MTGDefinitions.h"
#include "ObjectAnalytics.h"

class ManaCostHybrid;
class ExtraCosts;
class ExtraCost;
class MTGAbility;
class MTGCardInstance;
class Player;

class ManaCost
#ifdef TRACK_OBJECT_USAGE
    : public InstanceCounter<ManaCost>
#endif
{

    friend std::ostream& operator<<(std::ostream& out, ManaCost& m);
    friend std::ostream& operator<<(std::ostream& out, ManaCost* m);
    friend std::ostream& operator<<(std::ostream& out, ManaCost m);

protected:
    std::vector<int8_t> cost;
    std::vector<ManaCostHybrid> hybrids;

    virtual void init();

public:
    enum {
        MANA_UNPAID = 0,
        MANA_PAID = 1,
        MANA_PAID_WITH_KICKER = 2,
        MANA_PAID_WITH_ALTERNATIVE = 3,
        MANA_PAID_WITH_BUYBACK = 4,
        MANA_PAID_WITH_FLASHBACK = 5,
        MANA_PAID_WITH_RETRACE = 6,
        MANA_PAID_WITH_MORPH = 7,
        MANA_PAID_WITH_SUSPEND = 8
    };

    ExtraCosts* extraCosts;
    ManaCost* kicker;
    ManaCost* alternative;
    ManaCost* BuyBack;
    ManaCost* FlashBack;
    ManaCost* Retrace;
    ManaCost* morph;
    ManaCost* suspend;
    string alternativeName;
    bool isMulti;
    static ManaCost* parseManaCost(string value, ManaCost* _manacost = NULL, MTGCardInstance* c = NULL);
    virtual void resetCosts();
    void x();
    int hasX();
    void specificX(int color = 0);
    int hasSpecificX();
    int xColor;
    int hasAnotherCost();
    ManaCost(std::vector<int8_t>& _cost, int nb_elems = 1);
    ManaCost();
    ~ManaCost();
    ManaCost(ManaCost* _manaCost);
    ManaCost(const ManaCost& manaCost);
    ManaCost& operator=(const ManaCost& manaCost);
    void copy(ManaCost* _manaCost);
    int isNull();
    int getConvertedCost();
    string toString();
    int getCost(int color);
    // Returns NULL if i is greater than nbhybrids
    ManaCostHybrid* getHybridCost(unsigned int i);
    int hasColor(int color);
    int remove(int color, int value);
    int add(int color, int value);

    //
    // Extra Costs (sacrifice,counters...)
    //
    int addExtraCost(ExtraCost* _cost);
    int addExtraCosts(ExtraCosts* _cost);
    int setExtraCostsAction(MTGAbility* action, MTGCardInstance* card) const;
    int isExtraPaymentSet() const;
    int canPayExtra() const;
    int doPayExtra() const;
    ExtraCost* getExtraCost(unsigned int i) const;

    int addHybrid(int c1, int v1, int c2, int v2);
    int tryToPayHybrids(std::vector<ManaCostHybrid>& _hybrids, int _nbhybrids, std::vector<int8_t>& diff);
    void randomDiffHybrids(ManaCost* _cost, std::vector<int8_t>& diff);
    int add(ManaCost* _cost);
    int remove(ManaCost* _cost);
    int removeAll(int color);
    int pay(ManaCost* _cost);

    // return 1 if _cost can be paid with current data, 0 otherwise
    int canAfford(ManaCost* _cost);

    int isPositive();
    ManaCost* Diff(ManaCost* _cost);
#ifdef WIN32
    void Dump();
#endif
};

class ManaPool : public ManaCost {
protected:
    Player* player;

public:
    void Empty();
    ManaPool(Player* player);
    ManaPool(ManaCost* _manaCost, Player* player);
    int remove(int color, int value);
    int add(int color, int value, MTGCardInstance* source = NULL);
    int add(ManaCost* _cost, MTGCardInstance* source = NULL);
    int pay(ManaCost* _cost);
};

#endif
