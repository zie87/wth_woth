#include "PrecompiledHeader.h"

#include "ReplacementEffects.h"
#include "MTGCardInstance.h"
#include "TargetChooser.h"
#include "AllAbilities.h"

REDamagePrevention::REDamagePrevention(MTGAbility* source, TargetChooser* tcSource, TargetChooser* tcTarget,
                                       int damage, bool oneShot, int typeOfDamage)
    : source(source),
      tcSource(tcSource),
      tcTarget(tcTarget),
      damage(damage),
      oneShot(oneShot),
      typeOfDamage(typeOfDamage) {}

WEvent* REDamagePrevention::replace(WEvent* event) {
    if (!event) return event;
    if (!damage) return event;
    auto* e = dynamic_cast<WEventDamage*>(event);
    if (!e) return event;
    Damage* d = e->damage;
    if (d->typeOfDamage != typeOfDamage && typeOfDamage != DAMAGE_ALL_TYPES) return event;
    if ((!tcSource || tcSource->canTarget(d->source)) && (!tcTarget || tcTarget->canTarget(d->target))) {
        if (damage == -1) {
            d->damage = 0;
            delete event;
            if (oneShot) damage = 0;
            return nullptr;
        }
        if (damage >= d->damage) {
            damage -= d->damage;
            d->damage = 0;
            delete event;
            return nullptr;
        }
        d->damage -= damage;
        damage = 0;
        delete event;
        auto* newEvent = NEW WEventDamage(d);
        return newEvent;
    }
    return event;
}
REDamagePrevention::~REDamagePrevention() {
    SAFE_DELETE(tcSource);
    SAFE_DELETE(tcTarget);
}
// counters replacement effect///////////////////
RECountersPrevention::RECountersPrevention(MTGAbility* source, MTGCardInstance* cardSource,
                                           MTGCardInstance* cardTarget, TargetChooser* tc, Counter* counter)
    : source(source), cardSource(cardSource), cardTarget(cardTarget), TargetingCards(tc), counter(counter) {}

WEvent* RECountersPrevention::replace(WEvent* event) {
    if (!event) return event;
    auto* e = dynamic_cast<WEventCounters*>(event);
    if (!e) return event;
    if ((MTGCardInstance*)e->targetCard) {
        if ((MTGCardInstance*)e->targetCard == cardSource && counter) {
            if (e->power == counter->power && e->toughness == counter->toughness && e->name == counter->name)
                return event = nullptr;
        } else if ((MTGCardInstance*)e->targetCard == cardSource)
            return event = nullptr;
        else if (TargetingCards && TargetingCards->canTarget((MTGCardInstance*)e->targetCard))
            return event = nullptr;
    }
    return event;
}
RECountersPrevention::~RECountersPrevention() { SAFE_DELETE(TargetingCards); }
//////////////////////////////////////////////
ReplacementEffects::ReplacementEffects() {}

WEvent* ReplacementEffects::replace(WEvent* e) {
    for (auto it = modifiers.begin(); it != modifiers.end(); it++) {
        ReplacementEffect* re = *it;
        WEvent* newEvent = re->replace(e);
        if (!newEvent) return nullptr;
        if (newEvent != e) return replace(newEvent);
    }
    return e;
}

int ReplacementEffects::add(ReplacementEffect* re) {
    modifiers.push_back(re);
    return 1;
}

int ReplacementEffects::remove(ReplacementEffect* re) {
    modifiers.remove(re);
    return 1;
}

ReplacementEffects::~ReplacementEffects() {
    for (auto it = modifiers.begin(); it != modifiers.end(); it++) {
        ReplacementEffect* re = *it;
        delete (re);
    }
    modifiers.clear();
}
