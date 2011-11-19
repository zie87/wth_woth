#ifndef _TARGETSLIST_H_
#define _TARGETSLIST_H_

class Targetable;
class MTGCardInstance;
class Player;
class Damageable;
class Spell;
class Interruptible;
class Damage;

#include <vector>
using std::vector;

class TargetsList
{
private:
    size_t iterateTarget(Targetable * previous);
public:
    TargetsList();
    TargetsList(Targetable * _targets[], int nbtargets);
    vector<Targetable*> targets;
    int alreadyHasTarget(Targetable * target);
    int removeTarget(Targetable * _card);
    int toggleTarget(Targetable * _card);
    virtual int addTarget(Targetable * _target);
    MTGCardInstance * getNextCardTarget(MTGCardInstance * previous = 0);
    Player * getNextPlayerTarget(Player * previous = 0);
    Damageable * getNextDamageableTarget(Damageable * previous = 0);
    Interruptible * getNextInterruptible(Interruptible * previous, int type);
    Spell * getNextSpellTarget(Spell * previous = 0);
    Damage * getNextDamageTarget(Damage * previous = 0);
    Targetable * getNextTarget(Targetable * previous = 0);
    void initTargets()
    {
        targets.clear();
    }
    ;
};

#endif
