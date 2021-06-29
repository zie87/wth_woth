#include "PrecompiledHeader.h"

#include "CardSelector.h"
#include "GameApp.h"
#include "GuiAvatars.h"
#include "GameObserver.h"

#define LIB_GRAVE_OFFSET 230

GuiAvatars::GuiAvatars(GameObserver* observer) : GuiLayer(observer), active(nullptr) {
    Add(self = NEW GuiAvatar(SCREEN_WIDTH, SCREEN_HEIGHT, false, observer->players[0], GuiAvatar::BOTTOM_RIGHT, this));
    self->zoom = 0.9f;
    Add(selfGraveyard = NEW GuiGraveyard(SCREEN_WIDTH - GuiAvatar::Width - GuiGameZone::Width / 2 - 11,
                                         SCREEN_HEIGHT - GuiAvatar::Height - 1, false, observer->players[0], this));
    Add(selfLibrary = NEW GuiLibrary(SCREEN_WIDTH - GuiAvatar::Width - GuiGameZone::Width / 2 - 11,
                                     SCREEN_HEIGHT - GuiAvatar::Height - 5 + GuiGameZone::Height + 5, false,
                                     observer->players[0], this));

    Add(opponent = NEW GuiAvatar(0, 0, false, observer->players[1], GuiAvatar::TOP_LEFT, this));
    opponent->zoom = 0.9f;
    // opponenthandveiw button
    Add(opponentHand = NEW GuiOpponentHand(-30 + GuiAvatar::Width * 1.2 - GuiGameZone::Width / 2,
                                           35 + GuiGameZone::Height - 10, false, observer->players[1], this));
    // opponenthandveiwends
    Add(opponentGraveyard = NEW GuiGraveyard(5 + GuiAvatar::Width * 1.4 - GuiGameZone::Width / 2, 5, false,
                                             observer->players[1], this));
    Add(opponentLibrary = NEW GuiLibrary(5 + GuiAvatar::Width * 1.4 - GuiGameZone::Width / 2,
                                         5 + GuiGameZone::Height + 5, false, observer->players[1], this));

    observer->getCardSelector()->Add(self);
    observer->getCardSelector()->Add(selfGraveyard);
    observer->getCardSelector()->Add(selfLibrary);
    observer->getCardSelector()->Add(opponent);
    observer->getCardSelector()->Add(opponentGraveyard);
    observer->getCardSelector()->Add(opponentLibrary);
    observer->getCardSelector()->Add(opponentHand);
    selfGraveyard->alpha = selfLibrary->alpha = opponentGraveyard->alpha = opponentLibrary->alpha =
        opponentHand->alpha = 0;
}

float GuiAvatars::LeftBoundarySelf() { return SCREEN_WIDTH - 10; }

GuiAvatars::~GuiAvatars() {}

void GuiAvatars::Activate(PlayGuiObject* c) {
    c->zoom = 1.2f;
    c->mHasFocus = true;

    if ((opponentGraveyard == c) || (opponentLibrary == c) || (opponent == c) || (opponentHand == c)) {
        opponentGraveyard->alpha = opponentLibrary->alpha = opponentHand->alpha = 128.0f;
        active = opponent;
        opponent->zoom = 1.2f;
    } else if ((selfGraveyard == c) || (selfLibrary == c) || (self == c)) {
        selfGraveyard->alpha = selfLibrary->alpha = 128.0f;
        self->zoom = 1.0f;
        active = self;
    }
    if (opponent != c && self != c) c->alpha = 255.0f;
}
void GuiAvatars::Deactivate(PlayGuiObject* c) {
    c->zoom = 1.0;
    c->mHasFocus = false;
    if ((opponentGraveyard == c) || (opponentLibrary == c) || (opponentHand == c) || (opponent == c)) {
        opponentGraveyard->alpha = opponentLibrary->alpha = opponentHand->alpha = 0;
        opponent->zoom = 0.9f;
        active                                                                  = nullptr;
    } else if ((selfGraveyard == c) || (selfLibrary == c) || (self == c)) {
        selfGraveyard->alpha = selfLibrary->alpha = 0;
        self->zoom = 0.5f;
        active                                    = nullptr;
    }
}

int GuiAvatars::receiveEventPlus(WEvent* e) {
    return selfGraveyard->receiveEventPlus(e) | opponentGraveyard->receiveEventPlus(e) |
           opponentHand->receiveEventPlus(e);
}

int GuiAvatars::receiveEventMinus(WEvent* e) {
    selfGraveyard->receiveEventMinus(e);
    opponentGraveyard->receiveEventMinus(e);
    opponentHand->receiveEventMinus(e);
    return 1;
}

bool GuiAvatars::CheckUserInput(JButton key) {
    if (self->CheckUserInput(key)) return true;
    if (opponent->CheckUserInput(key)) return true;
    if (selfGraveyard->CheckUserInput(key)) return true;
    if (opponentGraveyard->CheckUserInput(key)) return true;
    if (opponentHand->CheckUserInput(key)) return true;
    if (selfLibrary->CheckUserInput(key)) return true;
    if (opponentLibrary->CheckUserInput(key)) return true;
    return false;
}

void GuiAvatars::Update(float dt) {
    self->Update(dt);
    opponent->Update(dt);
    selfGraveyard->Update(dt);
    opponentHand->Update(dt);
    opponentGraveyard->Update(dt);
    selfLibrary->Update(dt);
    opponentLibrary->Update(dt);
}

void GuiAvatars::Render() {
    JRenderer* r = JRenderer::GetInstance();
    float w = 54;
    float h = 54;
    if (opponent == active) {
        r->FillRect(opponent->actX, opponent->actY, w * opponent->actZ, h * opponent->actZ, ARGB(200, 0, 0, 0));
    } else if (self == active) {
        r->FillRect(self->actX - w * self->actZ - 4.5f, self->actY - h * self->actZ, w * self->actZ, h * self->actZ,
                    ARGB(200, 0, 0, 0));
    }
    GuiLayer::Render();
}

GuiAvatar* GuiAvatars::GetSelf() { return self; }

GuiAvatar* GuiAvatars::GetOpponent() { return opponent; }
