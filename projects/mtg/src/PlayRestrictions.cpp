#include "PrecompiledHeader.h"

#include "PlayRestrictions.h"
#include "TargetChooser.h"
#include "MTGCardInstance.h"

PlayRestriction::PlayRestriction(TargetChooser* tc) : tc(tc) {
    tc->setAllZones();  // This is to allow targetting cards without caring about the actual zone
    tc->targetter = nullptr;
};

PlayRestriction::~PlayRestriction() { SAFE_DELETE(tc); };

MaxPerTurnRestriction::MaxPerTurnRestriction(TargetChooser* tc, int maxPerTurn, MTGGameZone* zone)
    : PlayRestriction(tc), maxPerTurn(maxPerTurn), zone(zone) {}

int MaxPerTurnRestriction::canPutIntoZone(MTGCardInstance* card, MTGGameZone* destZone) {
    if (destZone != zone) return PlayRestriction::NO_OPINION;

    if (!tc->canTarget(card)) return PlayRestriction::NO_OPINION;

    if (maxPerTurn == NO_MAX) return PlayRestriction::CAN_PLAY;

    if (zone->seenThisTurn(tc, Constants::CAST_ALL) >= maxPerTurn) return PlayRestriction::CANT_PLAY;

    return PlayRestriction::CAN_PLAY;
};

MaxPerTurnRestriction* PlayRestrictions::getMaxPerTurnRestrictionByTargetChooser(TargetChooser* tc) {
    TargetChooser* _tc = tc->clone();
    _tc->setAllZones();  // we don't care about the actual zone for the "equals" check

    for (auto iter = restrictions.begin(); iter != restrictions.end(); ++iter) {
        auto* mptr = dynamic_cast<MaxPerTurnRestriction*>(*iter);
        if (mptr && mptr->tc->equals(_tc)) {
            delete _tc;
            return mptr;
        }
    }

    delete _tc;
    return nullptr;
}

void PlayRestrictions::addRestriction(PlayRestriction* restriction) {
    // TODO control that the id does not already exist?
    restrictions.push_back(restriction);
}

void PlayRestrictions::removeRestriction(PlayRestriction* restriction) {
    for (auto iter = restrictions.begin(); iter != restrictions.end(); ++iter) {
        if (*iter == restriction) {
            restrictions.erase(iter);
            return;
        }
    }
}

int PlayRestrictions::canPutIntoZone(MTGCardInstance* card, MTGGameZone* destZone) {
    if (!card) return PlayRestriction::CANT_PLAY;

    for (auto iter = restrictions.begin(); iter != restrictions.end(); ++iter) {
        if ((*iter)->canPutIntoZone(card, destZone) == PlayRestriction::CANT_PLAY) return PlayRestriction::CANT_PLAY;
    }

    return PlayRestriction::CAN_PLAY;
}

PlayRestrictions::~PlayRestrictions() {
    for (auto iter = restrictions.begin(); iter != restrictions.end(); ++iter) {
        SAFE_DELETE(*iter);
    }
    restrictions.clear();
}
