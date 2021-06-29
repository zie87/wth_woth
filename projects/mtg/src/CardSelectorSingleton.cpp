#include "PrecompiledHeader.h"

#include "CardSelectorSingleton.h"

#include "DuelLayers.h"
#include "Navigator.h"

/*
 **
 */
namespace CardSelectorSingleton {
static CardSelectorBase* sCardSelectorInstance = nullptr;

CardSelectorBase* Create(GameObserver* observer, DuelLayers* inDuelLayers) {
    if (sCardSelectorInstance == nullptr) sCardSelectorInstance = NEW CardSelector(observer, inDuelLayers);

    return sCardSelectorInstance;
}

CardSelectorBase* Instance() {
    assert(sCardSelectorInstance);
    return sCardSelectorInstance;
}

void Terminate() {
    SAFE_DELETE(sCardSelectorInstance);
    sCardSelectorInstance = nullptr;
}
}  // namespace CardSelectorSingleton
