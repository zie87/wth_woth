#ifndef _AIMOMIRPLAYER_H_
#define _AIMOMIRPLAYER_H_

#include "AIPlayerBaka.h"

class AIMomirPlayer : public AIPlayerBaka {
public:
    AIMomirPlayer(GameObserver* observer, string file, string fileSmall, string avatarFile, MTGDeck* deck = NULL);
    int getEfficiency(OrderedAIAction* action);
    int momir();
    int computeActions();
    MTGAbility* momirAbility;
    MTGAbility* getMomirAbility();
};

#endif
