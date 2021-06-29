#include "PrecompiledHeader.h"

#include "CardSelector.h"
#include "CardSelector.h"
#include "GameApp.h"
#include "Trash.h"
#include "GuiHand.h"
#include "OptionItem.h"

const float GuiHand::ClosedRowX = 459;
const float GuiHand::LeftRowX = 420;
const float GuiHand::RightRowX = 460;

const float GuiHand::OpenX = 394;
const float GuiHand::ClosedX = 494;
const float GuiHand::OpenY = SCREEN_HEIGHT - 50;
const float GuiHand::ClosedY = SCREEN_HEIGHT;

bool HandLimitor::select(Target* t) {
    if (auto* c = dynamic_cast<CardView*>(t))
        return hand->isInHand(c);
    return false;
}
bool HandLimitor::greyout(Target* t) { return true; }
HandLimitor::HandLimitor(GuiHand* hand) : hand(hand) {}

GuiHand::GuiHand(GameObserver* observer, MTGHand* hand) : GuiLayer(observer), hand(hand) {
    if (observer->getResourceManager()) {
        back = observer->getResourceManager()->RetrieveTempQuad("handback.png");
        if (back.get())
            back->SetTextureRect(1, 0, 100, 250);
        else
            GameApp::systemError = "Error loading hand texture : " __FILE__;
    }
}

GuiHand::~GuiHand() {
    for (auto& card : cards) delete card;
}

void GuiHand::Update(float dt) {
    for (auto& card : cards) card->Update(dt);
}

bool GuiHand::isInHand(CardView* card) {
    vector<CardView*>::iterator it;
    it = find(cards.begin(), cards.end(), card);
    return (it != cards.end());
}

GuiHandOpponent::GuiHandOpponent(GameObserver* observer, MTGHand* hand) : GuiHand(observer, hand) {
    vector<MTGCardInstance*>::iterator ite;
    for (ite = hand->cards.begin(); ite != hand->cards.end(); ite++) {
        WEventZoneChange event(*ite, nullptr, hand);
        receiveEventPlus(&event);
    }
}

void GuiHandOpponent::Render() {
    JQuadPtr quad = WResourceManager::Instance()->GetQuad(kGenericCardThumbnailID);

    float x = 45;
    for (auto& card : cards) {
        card->x    = x;
        card->y    = 2;
        card->zoom = 0.3f;
        card->Render(quad.get());
        x += 18;
    }
}

GuiHandSelf::GuiHandSelf(GameObserver* observer, MTGHand* hand)
    : GuiHand(observer, hand), state(Closed), backpos(ClosedX, SCREEN_HEIGHT - 250, 1.0, 0, 255) {
    limitor = NEW HandLimitor(this);
    if (OptionHandDirection::HORIZONTAL == options[Options::HANDDIRECTION].number) {
        backpos.t = M_PI / 2;
        backpos.y = ClosedY;
        backpos.x = SCREEN_WIDTH - 30 * 7 - 14;
        backpos.UpdateNow();
    }

    vector<MTGCardInstance*>::iterator ite;
    for (ite = hand->cards.begin(); ite != hand->cards.end(); ite++) {
        WEventZoneChange event(*ite, nullptr, hand);
        receiveEventPlus(&event);
    }
}

GuiHandSelf::~GuiHandSelf() { SAFE_DELETE(limitor); }

void GuiHandSelf::Repos() {
    float y = 48.0;
    if (Closed == state && OptionClosedHand::VISIBLE == options[Options::CLOSEDHAND].number) {
        float dist = 180.0f / cards.size();
        if (dist > 20)
            dist = 20.0;
        else
            y = 40.0;
        for (auto& card : cards) {
            card->x = ClosedRowX;
            card->y = y;
            y += dist;
        }
    } else {
        bool q = (Closed == state);
        if (OptionHandDirection::HORIZONTAL == options[Options::HANDDIRECTION].number) {
            y = SCREEN_WIDTH - 30;
            float dist = 240.0f / cards.size();
            if (dist > 30)
                dist = 30;
            else
                y = SCREEN_WIDTH - 15;
            for (auto it = cards.rbegin(); it != cards.rend(); ++it) {
                (*it)->x = y;
                (*it)->y = SCREEN_HEIGHT - 30;
                y -= dist;
                (*it)->alpha = static_cast<float>(q ? 0 : 255);
            }
            backpos.x = y + SCREEN_HEIGHT - 14;
        } else {
            float dist = 224.0f / ((cards.size() + 1) / 2);
            if (dist > 65) dist = 65;
            bool flip = false;
            for (auto& card : cards) {
                card->x = flip ? RightRowX : LeftRowX;
                card->y = y;
                if (flip) y += dist;
                flip = !flip;
                card->alpha = static_cast<float>(q ? 0 : 255);
            }
        }
    }
}

bool GuiHandSelf::CheckUserInput(JButton key) {
    JButton trigger = (options[Options::REVERSETRIGGERS].number ? JGE_BTN_PREV : JGE_BTN_NEXT);
    if (trigger == key) {
        state = (Open == state ? Closed : Open);
        if (Open == state) observer->getCardSelector()->Push();
        observer->getCardSelector()->Limit(Open == state ? limitor : nullptr, CardView::handZone);
        if (Closed == state) observer->getCardSelector()->Pop();
        if (OptionHandDirection::HORIZONTAL == options[Options::HANDDIRECTION].number)
            backpos.y = Open == state ? OpenY : ClosedY;
        else
            backpos.x = Open == state ? OpenX : ClosedX;
        if (Open == state && OptionClosedHand::INVISIBLE == options[Options::CLOSEDHAND].number) {
            if (OptionHandDirection::HORIZONTAL == options[Options::HANDDIRECTION].number)
                for (auto& card : cards) {
                    card->y = SCREEN_HEIGHT + 30;
                    card->UpdateNow();
                }
            else
                for (auto& card : cards) {
                    card->x = SCREEN_WIDTH + 30;
                    card->UpdateNow();
                }
        }
        Repos();
        return true;
    }
    return false;
}

void GuiHandSelf::Update(float dt) {
    backpos.Update(dt);
    GuiHand::Update(dt);
}

void GuiHandSelf::Render() {
    // Empty hand
    if (state == Open && cards.size() == 0) {
        WFont* mFont = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);
        mFont->SetColor(ARGB(255, 255, 0, 0));
        if (OptionHandDirection::HORIZONTAL == options[Options::HANDDIRECTION].number) {
            back->SetColor(ARGB(255, 255, 0, 0));
            JRenderer::GetInstance()->RenderQuad(back.get(), backpos.actX, backpos.actY, backpos.actT, backpos.actZ,
                                                 backpos.actZ);
            back->SetColor(ARGB(255, 255, 255, 255));
            mFont->DrawString("0", SCREEN_WIDTH - 10, backpos.actY);
        } else
            backpos.Render(back.get());
        return;
    }

    backpos.Render(back.get());
    if (OptionClosedHand::VISIBLE == options[Options::CLOSEDHAND].number || state == Open)
        for (auto& card : cards) card->Render();
}

float GuiHandSelf::LeftBoundary() {
    float min = SCREEN_WIDTH + 10;
    if (OptionClosedHand::VISIBLE == options[Options::CLOSEDHAND].number || state == Open)
        for (auto& card : cards)
            if (card->x - CardGui::Width / 2 < min) min = card->x - CardGui::Width / 2;
    return min;
}

int GuiHandSelf::receiveEventPlus(WEvent* e) {
    if (auto* ev = dynamic_cast<WEventZoneChange*>(e))
        if (hand == ev->to) {
            CardView* card;
            if (ev->card->view) {
                // fix for http://code.google.com/p/wagic/issues/detail?id=462.
                // We don't want a card in the hand to have an alpha of 0
                ev->card->view->alpha = 255;

                card = NEW CardView(CardView::handZone, ev->card, *(ev->card->view));
            } else
                card = NEW CardView(CardView::handZone, ev->card, ClosedRowX, 0);
            card->t = 6 * M_PI;
            cards.push_back(card);
            observer->getCardSelector()->Add(card);
            Repos();
            return 1;
        }
    return 0;
}
int GuiHandSelf::receiveEventMinus(WEvent* e) {
    if (auto* event = dynamic_cast<WEventZoneChange*>(e)) {
        if (hand == event->from)
            for (auto it = cards.begin(); it != cards.end(); ++it)
                if (event->card->previous == (*it)->card) {
                    CardView* cv = *it;
                    observer->getCardSelector()->Remove(cv);
                    cards.erase(it);
                    Repos();
                    observer->mTrash->trash(cv);
                    return 1;
                }
        return 1;
    }
    return 0;
}

int GuiHandOpponent::receiveEventPlus(WEvent* e) {
    if (auto* event = dynamic_cast<WEventZoneChange*>(e))
        if (hand == event->to) {
            CardView* card;
            if (event->card->view)
                card = NEW CardView(CardView::handZone, event->card, *(event->card->view));
            else
                card = NEW CardView(CardView::handZone, event->card, ClosedRowX, 0);
            card->alpha = 255;
            card->t = -4 * M_PI;
            cards.push_back(card);
            return 1;
        }
    return 0;
}
int GuiHandOpponent::receiveEventMinus(WEvent* e) {
    if (auto* event = dynamic_cast<WEventZoneChange*>(e)) {
        if (hand == event->from)
            for (auto it = cards.begin(); it != cards.end(); ++it)
                if (event->card->previous == (*it)->card) {
                    CardView* cv = *it;
                    cards.erase(it);
                    observer->mTrash->trash(cv);
                    return 1;
                }
        return 0;
    }
    return 0;
}

// I wanna write it like that. GCC doesn't want me to without -O.
// I'm submitting a bug report.
//	  it->x = (it->x + (flip ? RightRowX : LeftRowX)) / 2;
