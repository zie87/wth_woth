/*
 * GameStateDeckViewer.cpp
 * Class handling the Deck Editor
 */

#include "PrecompiledHeader.h"

#include <math.h>
#include <iomanip>

#include "GameStateDuel.h"
#include "GameStateDeckViewer.h"
#include "Translate.h"
#include "ManaCostHybrid.h"
#include "MTGCardInstance.h"
#include "WFilter.h"
#include "WDataSrc.h"
#include "DeckManager.h"
#include "DeckMetaData.h"
#include "DeckEditorMenu.h"
#include "SimpleMenu.h"
#include "utils.h"
#include "AIPlayer.h"

//!! helper function; this is probably handled somewhere in the code already.
// If not, should be placed in general library
void StringExplode(string str, string separator, vector<string>* results) {
    size_t found;
    found = str.find_first_of(separator);
    while (found != string::npos) {
        if (found > 0) results->push_back(str.substr(0, found));
        str = str.substr(found + 1);
        found = str.find_first_of(separator);
    }
    if (str.length() > 0) results->push_back(str);
}

GameStateDeckViewer::GameStateDeckViewer(GameApp* parent) : GameState(parent, "deckeditor") {
    bgMusic = NULL;
    useFilter = 0;
    isAIDeckSave = false;
    mSwitching = false;
    welcome_menu = NULL;
    myCollection = NULL;
    myDeck = NULL;
    filterMenu = NULL;
    source = NULL;
    hudAlpha = 0;
    subMenu = NULL;
    mRotation = 0;
    mSlide = 0;
    mAlpha = 255;
    menu = NULL;
    stw = NULL;

    statsPrevButton = NEW InteractiveButton(NULL, kPrevStatsButtonId, Fonts::MAIN_FONT, "Stats", SCREEN_WIDTH_F - 50,
                                            SCREEN_HEIGHT_F - 20, JGE_BTN_PREV);
    toggleDeckButton = NEW InteractiveButton(NULL, kToggleDeckActionId, Fonts::MAIN_FONT, "View Deck", 10,
                                             SCREEN_HEIGHT_F - 20, JGE_BTN_PRI);
    sellCardButton = NEW InteractiveButton(NULL, kSellCardActionId, Fonts::MAIN_FONT, "Sell Card",
                                           (SCREEN_WIDTH_F / 2) - 100, SCREEN_HEIGHT_F - 20, JGE_BTN_SEC);
    filterButton = NEW InteractiveButton(NULL, kFilterButtonId, Fonts::MAIN_FONT, "filter", (SCREEN_WIDTH_F - 110),
                                         SCREEN_HEIGHT_F - 20, JGE_BTN_CTRL);
}

GameStateDeckViewer::~GameStateDeckViewer() {
    SAFE_DELETE(bgMusic);
    SAFE_DELETE(toggleDeckButton);
    SAFE_DELETE(sellCardButton);
    SAFE_DELETE(statsPrevButton);
    SAFE_DELETE(filterButton);

    if (myDeck) {
        SAFE_DELETE(myDeck->parent);
        SAFE_DELETE(myDeck);
    }
    if (myCollection) {
        SAFE_DELETE(myCollection->parent);
        SAFE_DELETE(myCollection);
    }
    SAFE_DELETE(filterMenu);
}

void GameStateDeckViewer::rotateCards(int direction) {
    int left = direction;
    if (left)
        displayed_deck->next();
    else
        displayed_deck->prev();
    loadIndexes();

    int total = displayed_deck->Size();
    if (total) {
        lastPos = getCurrentPos();
        lastTotal = total;
    }
}

void GameStateDeckViewer::rebuildFilters() {
    if (!filterMenu) filterMenu = NEW WGuiFilters("Filter by...", NULL);
    if (source) SAFE_DELETE(source);
    source = NEW WSrcDeckViewer(myDeck, myCollection);
    filterMenu->setSrc(source);
    if (displayed_deck != myDeck) source->swapSrc();
    filterMenu->Finish(true);

    // no stats need updating if there isn't a deck to update
    if (stw && myDeck) stw->updateStats(myDeck);
    ;
}

void GameStateDeckViewer::updateFilters() {
    if (!displayed_deck) return;

    filterMenu->recolorFilter(useFilter - 1);
    filterMenu->Finish(true);
    int totalAfter = displayed_deck->Size();
    if (totalAfter && lastTotal) {
        // This part is a hack. I don't understand why in some cases "displayed_deck's" currentPos is not 0 at this
        // stage
        {
            while (int currentPos = displayed_deck->getOffset()) {
                if (currentPos > 0)
                    displayed_deck->prev();
                else
                    displayed_deck->next();
            }
        }

        int pos = (totalAfter * lastPos) / lastTotal;
        for (int i = 0; i < pos - 3; ++i) {  // "-3" because card "0" is displayed at position 3 initially
            displayed_deck->next();
        }
    }
    stw->updateStats(myDeck);
    ;
    return;
}

void GameStateDeckViewer::loadIndexes() {
    for (int i = 0; i < 7; i++) {
        cardIndex[i] = displayed_deck->getCard(i);
    }
}

void GameStateDeckViewer::switchDisplay() {
    if (displayed_deck == myCollection) {
        displayed_deck = myDeck;
        toggleDeckButton->setText("View Collection");
    } else {
        toggleDeckButton->setText("View Deck");
        displayed_deck = myCollection;
    }
    source->swapSrc();
    updateFilters();
    loadIndexes();
}

void GameStateDeckViewer::updateDecks() {
    SAFE_DELETE(welcome_menu);
    welcome_menu = NEW DeckEditorMenu(MENU_DECK_SELECTION, this, Fonts::OPTION_FONT, "Choose Deck To Edit");
    DeckManager* deckManager = DeckManager::GetInstance();
    vector<DeckMetaData*> playerDeckList = fillDeckMenu(welcome_menu, options.profileFile());

    newDeckname = "";
    welcome_menu->Add(MENU_ITEM_NEW_DECK, "--NEW--");
    if (options[Options::CHEATMODE].number &&
        (!myCollection || myCollection->getCount(WSrcDeck::UNFILTERED_MIN_COPIES) < 4))
        welcome_menu->Add(MENU_ITEM_CHEAT_MODE, "--UNLOCK CARDS--");
    welcome_menu->Add(MENU_ITEM_CANCEL, "Cancel");

    // update the deckmanager with the latest information
    deckManager->updateMetaDataList(&playerDeckList, false);
    // is this necessary to ensure no memory leaks?
    playerDeckList.clear();
}

void GameStateDeckViewer::buildEditorMenu() {
    std::ostringstream deckSummaryInformation;
    deckSummaryInformation << "All changes are final." << std::endl;

    if (menu) SAFE_DELETE(menu);
    // Build menu.
    JRenderer::GetInstance()->FillRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 100, ARGB(0, 0, 0, 0));
    menu = NEW DeckEditorMenu(MENU_DECK_BUILDER, this, Fonts::OPTION_FONT, "Deck Editor", myDeck, stw);

    menu->Add(MENU_ITEM_FILTER_BY, "Filter By...", "Narrow down the list of cards. ");
    menu->Add(MENU_ITEM_SWITCH_DECKS_NO_SAVE, "Switch Decks", "Do not make any changes.\nView another deck.");
    menu->Add(MENU_ITEM_SAVE_RENAME, "Rename Deck", "Change the name of the deck");
    menu->Add(MENU_ITEM_SAVE_RETURN_MAIN_MENU, "Save & Quit Editor", "Save changes.\nReturn to the main menu");
    menu->Add(MENU_ITEM_SAVE_AS_AI_DECK, "Save As AI Deck", deckSummaryInformation.str());
    menu->Add(MENU_ITEM_MAIN_MENU, "Quit Editor", "Do not make any changes to deck.\nReturn to the main menu.");
    menu->Add(MENU_ITEM_EDITOR_CANCEL, "Cancel", "Close menu.");
}

void GameStateDeckViewer::Start() {
    hudAlpha = 0;
    mSwitching = false;
    subMenu = NULL;
    myDeck = NULL;
    mStage = STAGE_WELCOME;
    mRotation = 0;
    mSlide = 0;
    mAlpha = 255;
    last_user_activity = NO_USER_ACTIVITY_HELP_DELAY + 1;
    onScreenTransition = 0;
    useFilter = 0;
    lastPos = 0;
    lastTotal = 0;

    pricelist = NEW PriceList("settings/prices.dat", MTGCollection());
    playerdata = NEW PlayerData(MTGCollection());
    myCollection = NEW DeckDataWrapper(playerdata->collection);
    myCollection->Sort(WSrcCards::SORT_ALPHA);
    displayed_deck = myCollection;

    // Icons
    mIcons = manaIcons;
    for (int i = 0; i < Constants::NB_Colors; i++) {
        mIcons[i]->SetHotSpot(16, 16);
    }

    // Grab a texture in VRAM.
    pspIconsTexture = WResourceManager::Instance()->RetrieveTexture("iconspsp.png", RETRIEVE_MANAGE);

    char buf[512];
    for (int i = 0; i < 8; i++) {
        sprintf(buf, "iconspsp%d", i);
        pspIcons[i] = WResourceManager::Instance()->RetrieveQuad("iconspsp.png", (float)i * 32, 0, 32, 32, buf);
        pspIcons[i]->SetHotSpot(16, 16);
    }
#ifndef TOUCH_ENABLED
    toggleDeckButton->setImage(pspIcons[6]);
    sellCardButton->setImage(pspIcons[7]);
#endif

    // init welcome menu
    updateDecks();

    GameApp::playMusic("Track1.mp3");

    loadIndexes();
    mEngine->ResetInput();
    JRenderer::GetInstance()->EnableVSync(true);
}

void GameStateDeckViewer::End() {
    JRenderer::GetInstance()->EnableVSync(false);

    SAFE_DELETE(welcome_menu);
    SAFE_DELETE(menu);
    SAFE_DELETE(subMenu);

    WResourceManager::Instance()->Release(pspIconsTexture);
    if (myCollection) {
        SAFE_DELETE(myCollection);
    }
    if (myDeck) {
        SAFE_DELETE(myDeck->parent);
        SAFE_DELETE(myDeck);
    }
    SAFE_DELETE(pricelist);
    SAFE_DELETE(playerdata);
    SAFE_DELETE(filterMenu);
    SAFE_DELETE(source);
}

void GameStateDeckViewer::addRemove(MTGCard* card) {
    if (!card) return;
    if (displayed_deck->Remove(card, 1, (displayed_deck == myDeck))) {
        if (displayed_deck == myCollection) {
            myDeck->Add(card);
            myDeck->Sort(WSrcCards::SORT_ALPHA);
        } else {
            myCollection->Add(card);
        }
    }
    myCollection->validate();
    myDeck->validate();
    stw->needUpdate = true;
    loadIndexes();
}

void GameStateDeckViewer::saveDeck() {
    // update the corresponding meta data object
    DeckMetaData* metaData = DeckManager::GetInstance()->getDeckMetaDataById(myDeck->parent->meta_id, false);
    if (newDeckname.length() > 0) metaData->setDeckName(newDeckname);
    mSwitching = true;
    myDeck->save();
    playerdata->save();
    pricelist->save();
}

/**
 save the deck in a readable format to allow people to edit the file offline
 */
void GameStateDeckViewer::saveAsAIDeck(string deckName) {
    int deckId = AIPlayer::getTotalAIDecks() + 1;

    std::ostringstream oss;
    oss << "deck" << deckId;
    string aiDeckName = oss.str();
    oss.str("");
    if (myDeck->parent->meta_desc == "")
        oss << std::endl << "Can you beat your own creations?" << std::endl << "User created AI Deck # " << deckId;
    else
        oss << myDeck->parent->meta_desc;
    string deckDesc = oss.str();
    string filepath = "ai/baka/";
    filepath.append(aiDeckName).append(".txt");
    DebugTrace("saving AI deck " << filepath);
    myDeck->save(filepath, true, deckName, deckDesc);
    AIPlayer::invalidateTotalAIDecks();  // We added one AI deck, so we need to invalidate the count cache
}

void GameStateDeckViewer::sellCard() {
    last_user_activity = 0;
    SAFE_DELETE(subMenu);
    char buffer[4096];
    {
        MTGCard* card = cardIndex[2];
        if (card && displayed_deck->count(card)) {
            price = pricelist->getSellPrice(card->getMTGId());
            sprintf(buffer, "%s : %i %s", _(card->data->getName()).c_str(), price, _("credits").c_str());
            const float menuXOffset = SCREEN_WIDTH_F - 300;
            const float menuYOffset = SCREEN_HEIGHT_F / 2;
            subMenu = NEW SimpleMenu(JGE::GetInstance(), MENU_CARD_PURCHASE, this, Fonts::MAIN_FONT, menuXOffset,
                                     menuYOffset, buffer);
            subMenu->Add(MENU_ITEM_YES, "Yes");
            subMenu->Add(MENU_ITEM_NO, "No", "", true);
        }
    }
    stw->needUpdate = true;
}

bool GameStateDeckViewer::userPressedButton() {
    return ((toggleDeckButton->ButtonPressed()) || (sellCardButton->ButtonPressed()) ||
            (statsPrevButton->ButtonPressed()) || (filterButton->ButtonPressed()));
}

void GameStateDeckViewer::setButtonState(bool state) {
    toggleDeckButton->setIsSelectionValid(state);
    sellCardButton->setIsSelectionValid(state);
    statsPrevButton->setIsSelectionValid(state);
    filterButton->setIsSelectionValid(state);
}

void GameStateDeckViewer::RenderButtons() {
    toggleDeckButton->Render();
    sellCardButton->Render();
    filterButton->Render();
    statsPrevButton->Render();
}

void GameStateDeckViewer::Update(float dt) {
    int x, y;
    unsigned int distance2;
    unsigned int minDistance2 = -1;
    int n = 0;

    if (options.keypadActive()) {
        options.keypadUpdate(dt);

        if (newDeckname != "") {
            newDeckname = options.keypadFinish();

            if (newDeckname != "") {
                if (isAIDeckSave) {
                    saveAsAIDeck(newDeckname);
                    isAIDeckSave = false;
                } else if (myDeck && myDeck->parent) {
                    myDeck->parent->meta_name = newDeckname;
                    saveDeck();
                    updateDecks();
                }
                mStage = STAGE_WAITING;
            }
            newDeckname = "";
        }
        // Prevent screen from updating.
        return;
    }
    hudAlpha = 255 - ((int)last_user_activity * 500);
    if (hudAlpha < 0) hudAlpha = 0;
    if (subMenu) {
        subMenu->Update(dt);
        if (subMenu->isClosed()) {
            SAFE_DELETE(subMenu);
        }
        return;
    }
    if (mStage == STAGE_WAITING || mStage == STAGE_ONSCREEN_MENU) {
        switch (mEngine->ReadButton()) {
        case JGE_BTN_LEFT:
            last_user_activity = 0;
            mStage = STAGE_TRANSITION_LEFT;
            break;
        case JGE_BTN_RIGHT:
            last_user_activity = 0;
            mStage = STAGE_TRANSITION_RIGHT;
            break;
        case JGE_BTN_UP:
            last_user_activity = 0;
            mStage = STAGE_TRANSITION_UP;
            useFilter++;
            if (useFilter >= MAX_SAVED_FILTERS) useFilter = 0;
            break;
        case JGE_BTN_DOWN:
            last_user_activity = 0;
            mStage = STAGE_TRANSITION_DOWN;
            useFilter--;
            if (useFilter < 0) useFilter = MAX_SAVED_FILTERS - 1;
            break;
        case JGE_BTN_CANCEL:
            options[Options::DISABLECARDS].number = !options[Options::DISABLECARDS].number;
            break;
        case JGE_BTN_PRI:
            if (last_user_activity > 0.2) {
                last_user_activity = 0;
                switchDisplay();
            }
            break;
        case JGE_BTN_OK:
            if (mEngine->GetLeftClickCoordinates(x, y)) {
                // verify that none of the buttons fired
                if (userPressedButton()) {
                    Update(dt);
                    break;
                }

                for (int i = 0; i < CARDS_DISPLAYED; i++) {
                    distance2 =
                        static_cast<unsigned int>((cardsCoordinates[i].second - y) * (cardsCoordinates[i].second - y) +
                                                  (cardsCoordinates[i].first - x) * (cardsCoordinates[i].first - x));
                    if (distance2 < minDistance2) {
                        minDistance2 = distance2;
                        n = i;
                    }
                }

                if (n != 2) {
                    mSelected = n;
                    last_user_activity = 0;
                    mStage = STAGE_TRANSITION_SELECTED;
                }
                mEngine->LeftClickedProcessed();
            }

            if (mStage != STAGE_TRANSITION_SELECTED && last_user_activity > .05) {
                last_user_activity = 0;
                addRemove(cardIndex[2]);
            }

            break;
        case JGE_BTN_SEC:
            sellCard();
            break;

        case JGE_BTN_MENU:
            mStage = STAGE_MENU;
            buildEditorMenu();
            break;
        case JGE_BTN_CTRL:
            mStage = STAGE_FILTERS;
            if (!filterMenu) {
                filterMenu = NEW WGuiFilters("Filter by...", NULL);
                if (source) SAFE_DELETE(source);
                source = NEW WSrcDeckViewer(myDeck, myCollection);
                filterMenu->setSrc(source);
                if (displayed_deck != myDeck) source->swapSrc();
            }
            filterMenu->Entering(JGE_BTN_NONE);
            break;
        case JGE_BTN_PREV:
            if (last_user_activity < NO_USER_ACTIVITY_HELP_DELAY)
                last_user_activity = NO_USER_ACTIVITY_HELP_DELAY + 1;
            else if ((mStage == STAGE_ONSCREEN_MENU) && (--stw->currentPage < 0))
                stw->currentPage = stw->pageCount;
            break;
        case JGE_BTN_NEXT:
            if (last_user_activity < NO_USER_ACTIVITY_HELP_DELAY)
                last_user_activity = NO_USER_ACTIVITY_HELP_DELAY + 1;
            else if ((mStage == STAGE_ONSCREEN_MENU) && (++stw->currentPage > stw->pageCount))
                stw->currentPage = 0;
            break;
        default:  // no keypress
            if (last_user_activity > NO_USER_ACTIVITY_HELP_DELAY) {
                if (mStage != STAGE_ONSCREEN_MENU) {
                    mStage = STAGE_ONSCREEN_MENU;
                    onScreenTransition = 1;
                } else {
                    if (onScreenTransition > 0)
                        onScreenTransition -= 0.05f;
                    else
                        onScreenTransition = 0;
                }
            } else
                last_user_activity += dt;

            break;
        }
    }
    if (mStage == STAGE_TRANSITION_SELECTED) {
        if (mSelected < 2) {
            mRotation -= dt * MED_SPEED;
            if (mRotation < mSelected - 2) {
                do {
                    rotateCards(STAGE_TRANSITION_RIGHT);
                    mRotation += 1;
                } while (mRotation < -1.0f);
                mStage = STAGE_WAITING;
                mRotation = 0;
            }
        } else if (mSelected > 2) {
            mRotation += dt * MED_SPEED;
            if (mRotation > mSelected - 2) {
                do {
                    rotateCards(STAGE_TRANSITION_LEFT);
                    mRotation -= 1;
                } while (mRotation > 1.0f);
                mStage = STAGE_WAITING;
                mRotation = 0;
            }
        }
    }
    if (mStage == STAGE_TRANSITION_RIGHT || mStage == STAGE_TRANSITION_LEFT) {
        if (mStage == STAGE_TRANSITION_RIGHT) {
            mRotation -= dt * MED_SPEED;
            if (mRotation < -1.0f) {
                do {
                    rotateCards(mStage);
                    mRotation += 1;
                } while (mRotation < -1.0f);
                mStage = STAGE_WAITING;
                mRotation = 0;
            }
        } else if (mStage == STAGE_TRANSITION_LEFT) {
            mRotation += dt * MED_SPEED;
            if (mRotation > 1.0f) {
                do {
                    rotateCards(mStage);
                    mRotation -= 1;
                } while (mRotation > 1.0f);
                mStage = STAGE_WAITING;
                mRotation = 0;
            }
        }
    }
    if (mStage == STAGE_TRANSITION_DOWN || mStage == STAGE_TRANSITION_UP) {
        if (mStage == STAGE_TRANSITION_DOWN) {
            mSlide -= 0.05f;
            if (mSlide < -1.0f) {
                updateFilters();
                loadIndexes();
                mSlide = 1;
            } else if (mSlide > 0 && mSlide < 0.05) {
                mStage = STAGE_WAITING;
                mSlide = 0;
            }
        }
        if (mStage == STAGE_TRANSITION_UP) {
            mSlide += 0.05f;
            if (mSlide > 1.0f) {
                updateFilters();
                loadIndexes();
                mSlide = -1;
            } else if (mSlide < 0 && mSlide > -0.05) {
                mStage = STAGE_WAITING;
                mSlide = 0;
            }
        }

    } else if (mStage == STAGE_WELCOME)
        welcome_menu->Update(dt);
    else if (mStage == STAGE_MENU)
        menu->Update(dt);
    else if (mStage == STAGE_FILTERS) {
        JButton key = mEngine->ReadButton();
        if (filterMenu) {
            if (key == JGE_BTN_CTRL) {
                // useFilter = 0;
                filterMenu->Finish(true);
                filterMenu->Update(dt);
                loadIndexes();
                return;
            }
            if (!filterMenu->isFinished()) {
                filterMenu->CheckUserInput(key);
                filterMenu->Update(dt);
            } else {
                mStage = STAGE_WAITING;
                updateFilters();
                loadIndexes();
            }
        }
    }
}

void GameStateDeckViewer::renderOnScreenBasicInfo() {
    JRenderer* renderer = JRenderer::GetInstance();
    WFont* mFont = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);
    char buffer[256];

    float y = 0;
    int allCopies, nowCopies;
    nowCopies = displayed_deck->getCount(WSrcDeck::FILTERED_COPIES);
    allCopies = displayed_deck->getCount(WSrcDeck::UNFILTERED_COPIES);
    WCardFilter* wc = displayed_deck->getFiltersRoot();

    if (wc)
        sprintf(buffer, "%s %i of %i cards (%i unique)", (displayed_deck == myDeck) ? "DECK: " : " ", nowCopies,
                allCopies, displayed_deck->getCount(WSrcDeck::FILTERED_UNIQUE));
    else
        sprintf(buffer, "%s%i cards (%i unique)", (displayed_deck == myDeck) ? "DECK: " : " ", allCopies,
                displayed_deck->getCount(WSrcDeck::UNFILTERED_UNIQUE));

    float w = mFont->GetStringWidth(buffer);
    PIXEL_TYPE backupColor = mFont->GetColor();

    renderer->FillRoundRect(SCREEN_WIDTH - (w + 27), y + 5, w + 10, 15, 5, ARGB(hudAlpha / 2, 0, 0, 0));
    mFont->SetColor(ARGB(hudAlpha, 255, 255, 255));
    mFont->DrawString(buffer, SCREEN_WIDTH - 22, y + 15, JGETEXT_RIGHT);
    mFont->SetColor(backupColor);

    if (useFilter != 0) renderer->RenderQuad(mIcons[useFilter - 1].get(), SCREEN_WIDTH - 10, y + 15, 0.0f, 0.5, 0.5);
}

// returns position of the current card (cusor) in the currently viewed color/filter
int GameStateDeckViewer::getCurrentPos() {
    int total = displayed_deck->Size();

    int currentPos = displayed_deck->getOffset();
    currentPos += 2;  // we start by displaying card number 3
    currentPos = currentPos % total + 1;
    if (currentPos < 0) currentPos = (total + currentPos);
    if (!currentPos) currentPos = total;
    return currentPos;
}

void GameStateDeckViewer::renderSlideBar() {
    WFont* mFont = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);

    int total = displayed_deck->Size();
    if (total == 0) return;

    float filler = 15;
    float y = SCREEN_HEIGHT_F - 25;
    float bar_size = SCREEN_WIDTH_F - 2 * filler;
    JRenderer* r = JRenderer::GetInstance();
    int currentPos = getCurrentPos();

    float cursor_pos = bar_size * currentPos / total;

    r->FillRoundRect(filler + 5, y + 5, bar_size, 0, 3, ARGB(hudAlpha / 2, 0, 0, 0));
    r->DrawLine(filler + cursor_pos + 5, y + 5, filler + cursor_pos + 5, y + 10, ARGB(hudAlpha / 2, 0, 0, 0));

    r->FillRoundRect(filler, y, bar_size, 0, 3, ARGB(hudAlpha / 2, 128, 128, 128));
    r->DrawLine(filler + cursor_pos, y, filler + cursor_pos, y + 5, ARGB(hudAlpha, 255, 255, 255));
    char buffer[256];
    string deckname = _("Collection");
    if (displayed_deck == myDeck) {
        deckname = _("Deck");
    }
    sprintf(buffer, "%s - %i/%i", deckname.c_str(), currentPos, total);
    mFont->SetColor(ARGB(hudAlpha, 255, 255, 255));
    mFont->DrawString(buffer, SCREEN_WIDTH / 2, y, JGETEXT_CENTER);

    mFont->SetColor(ARGB(255, 255, 255, 255));
}

void GameStateDeckViewer::renderDeckBackground() {
    int max1 = 0;
    int maxC1 = 4;
    int max2 = 0;
    int maxC2 = 4;

    for (int i = 0; i < Constants::NB_Colors - 1; i++) {
        int value = myDeck->getCount(i);
        if (value > max1) {
            max2 = max1;
            maxC2 = maxC1;
            max1 = value;
            maxC1 = i;
        } else if (value > max2) {
            max2 = value;
            maxC2 = i;
        }
    }
    if (max2 < max1 / 2) {
        maxC2 = maxC1;
    }
    PIXEL_TYPE colors[] = {
        static_cast<PIXEL_TYPE>(ARGB(255, Constants::_r[maxC1], Constants::_g[maxC1], Constants::_b[maxC1])),
        static_cast<PIXEL_TYPE>(ARGB(255, Constants::_r[maxC1], Constants::_g[maxC1], Constants::_b[maxC1])),
        static_cast<PIXEL_TYPE>(ARGB(255, Constants::_r[maxC2], Constants::_g[maxC2], Constants::_b[maxC2])),
        static_cast<PIXEL_TYPE>(ARGB(255, Constants::_r[maxC2], Constants::_g[maxC2], Constants::_b[maxC2])),
    };

    JRenderer::GetInstance()->FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, colors);
}

void GameStateDeckViewer::renderOnScreenMenu() {
    WFont* font = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);
    font->SetColor(ARGB(255, 255, 255, 255));
    JRenderer* r = JRenderer::GetInstance();
    float pspIconsSize = 0.5;
    float fH = font->GetHeight() + 1;

    float leftTransition = onScreenTransition * 84;
    float rightTransition = onScreenTransition * 204;
    float leftPspX = 40 - leftTransition;
    float leftPspY = SCREEN_HEIGHT / 2 - 20;
    float rightPspX = SCREEN_WIDTH - 100 + rightTransition;
    float rightPspY = SCREEN_HEIGHT / 2 - 20;

#ifdef TOUCH_ENABLED
    bool renderPSPIcons = false;
#else
    bool renderPSPIcons = true;
#endif

    if (stw->currentPage == 0) {
        // FillRects
        r->FillRect(0 - (onScreenTransition * 84), 0, 84, SCREEN_HEIGHT, ARGB(128, 0, 0, 0));
        r->FillRect(SCREEN_WIDTH - 204 + (onScreenTransition * 204), 0, 204, SCREEN_HEIGHT, ARGB(128, 0, 0, 0));
        if (renderPSPIcons) {
            // LEFT PSP CIRCLE render
            r->FillCircle(leftPspX, leftPspY, 40, ARGB(128, 50, 50, 50));

            r->RenderQuad(pspIcons[0].get(), leftPspX, leftPspY - 20, 0, pspIconsSize, pspIconsSize);
            r->RenderQuad(pspIcons[1].get(), leftPspX, leftPspY + 20, 0, pspIconsSize, pspIconsSize);
            r->RenderQuad(pspIcons[2].get(), leftPspX - 20, leftPspY, 0, pspIconsSize, pspIconsSize);
            r->RenderQuad(pspIcons[3].get(), leftPspX + 20, leftPspY, 0, pspIconsSize, pspIconsSize);

            font->DrawString(_("Prev."), leftPspX - 35, leftPspY - 15);
            font->DrawString(_("Next"), leftPspX + 15, leftPspY - 15);
            font->DrawString(_("card"), leftPspX - 35, leftPspY);
            font->DrawString(_("card"), leftPspX + 15, leftPspY);
            font->DrawString(_("Next edition"), leftPspX - 33, leftPspY - 35);
            font->DrawString(_("Prev. edition"), leftPspX - 33, leftPspY + 25);

            // RIGHT PSP CIRCLE render
            r->FillCircle(rightPspX + (onScreenTransition * 204), rightPspY, 40, ARGB(128, 50, 50, 50));
            r->RenderQuad(pspIcons[4].get(), rightPspX + 20, rightPspY, 0, pspIconsSize, pspIconsSize);
            r->RenderQuad(pspIcons[5].get(), rightPspX, rightPspY - 20, 0, pspIconsSize, pspIconsSize);
            r->RenderQuad(pspIcons[6].get(), rightPspX - 20, rightPspY, 0, pspIconsSize, pspIconsSize);
            r->RenderQuad(pspIcons[7].get(), rightPspX, rightPspY + 20, 0, pspIconsSize, pspIconsSize);

            font->DrawString(_("Toggle Images"), rightPspX - 35, rightPspY - 40);

            if (displayed_deck == myCollection) {
                font->DrawString(_("Add card"), rightPspX + 20, rightPspY - 15);
                font->DrawString(_("View Deck"), rightPspX - 20, rightPspY - 15, JGETEXT_RIGHT);
            } else {
                font->DrawString(_("Remove card"), rightPspX + 20, rightPspY - 15);
                font->DrawString(_("View Collection"), rightPspX - 20, rightPspY - 15, JGETEXT_RIGHT);
            }
            font->DrawString(_("Sell card"), rightPspX - 30, rightPspY + 20);
            // Bottom menus
            font->DrawString(_("menu"), SCREEN_WIDTH - 35 + rightTransition, SCREEN_HEIGHT - 15);
            font->DrawString(_("filter"), SCREEN_WIDTH - 95 + rightTransition, SCREEN_HEIGHT - 15);

            if (displayed_deck == myCollection) {
                font->DrawString(_("in: collection"), 5 - leftTransition, 5);
                font->DrawString(_("Use SQUARE to view your deck,"), SCREEN_WIDTH - 200 + rightTransition, 5);
            } else {
                font->DrawString(_("in: deck"), 5 - leftTransition, 5);
                font->DrawString(_("Use SQUARE to view collection,"), SCREEN_WIDTH - 200 + rightTransition, 5);
            }

            font->DrawString(_("Press L/R to cycle through"), SCREEN_WIDTH - 200 + rightTransition, 5 + fH);
            font->DrawString(_("deck statistics."), SCREEN_WIDTH - 200 + rightTransition, 5 + fH * 2);
        } else {
            // print stuff here about the editor commands
            float textYOffset = SCREEN_HEIGHT_F / 2;
            font->DrawString(_("Click on the card image"), SCREEN_WIDTH - 200 + rightTransition,
                             textYOffset - (2 * fH));
            if (displayed_deck == myCollection)
                font->DrawString(_("to add card to deck."), SCREEN_WIDTH - 200 + rightTransition, textYOffset - fH);
            else
                font->DrawString(_("to remove card from deck."), SCREEN_WIDTH - 200 + rightTransition,
                                 textYOffset - fH);
        }
        // Your Deck Information
        char buffer[300];
        int nb_letters = 0;
        int value = myDeck->getCount(WSrcDeck::UNFILTERED_COPIES);

        sprintf(buffer, _("Your Deck: %i cards").c_str(), value);
        font->DrawString(buffer, SCREEN_WIDTH - 200 + rightTransition, SCREEN_HEIGHT / 2 + 25);

        for (int j = 0; j < Constants::NB_Colors; j++) {
            int value = myDeck->getCount(j);
            if (value > 0) {
                sprintf(buffer, "%i", value);
                font->DrawString(buffer, SCREEN_WIDTH - 190 + rightTransition + nb_letters * 13,
                                 SCREEN_HEIGHT / 2 + 40);
                r->RenderQuad(mIcons[j].get(), SCREEN_WIDTH - 197 + rightTransition + nb_letters * 13,
                              SCREEN_HEIGHT / 2 + 46, 0, 0.5, 0.5);
                if (value > 9) {
                    nb_letters += 3;
                } else {
                    nb_letters += 2;
                }
            }
        }

    } else {
        stw->updateStats(myDeck);
        ;

        char buffer[300];

        leftTransition = -(onScreenTransition / 2) * SCREEN_WIDTH;
        rightTransition = -leftTransition;

        r->FillRect(0 + leftTransition, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, ARGB(128, 0, 0, 0));
        r->FillRect(SCREEN_WIDTH / 2 + rightTransition, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, ARGB(128, 0, 0, 0));
        r->FillRect(10 + leftTransition, 10, SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT - 20, ARGB(128, 0, 0, 0));
        r->FillRect(SCREEN_WIDTH / 2 + rightTransition, 10, SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT - 20,
                    ARGB(128, 0, 0, 0));
#ifndef TOUCH_ENABLED
        font->DrawString(_("menu"), SCREEN_WIDTH - 35 + rightTransition, SCREEN_HEIGHT - 15);
        font->DrawString(_("filter"), SCREEN_WIDTH - 95 + rightTransition, SCREEN_HEIGHT - 15);
#endif
        int nb_letters = 0;
        float posX, posY;
        DWORD graphColor;

        graphColor = ARGB(200, 155, 155, 155);
        string STATS_TITLE_FORMAT = _("%i: %s");

        switch (stw->currentPage) {
        case 1:  // Counts, price
            // Title
            sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage, _("Statistics Summary").c_str());
            font->DrawString(buffer, 10 + leftTransition, 10);

            posY = 30;
            posX = 180;
            sprintf(buffer, _("Your Deck: %i cards").c_str(), stw->cardCount);
            font->DrawString(buffer, 20 + leftTransition, posY);
            posY += 10;

            // Counts by color
            for (int j = 0; j < Constants::NB_Colors; j++) {
                int value = myDeck->getCount(j);
                if (value > 0) {
                    sprintf(buffer, "%i", value);
                    font->DrawString(buffer, 38 + nb_letters * 13 + leftTransition, posY + 5);
                    r->RenderQuad(mIcons[j].get(), 30 + nb_letters * 13 + leftTransition, posY + 11, 0, 0.5, 0.5);
                    if (value > 9) {
                        nb_letters += 3;
                    } else {
                        nb_letters += 2;
                    }
                }
            }
            posY += 25;

            r->DrawLine(posX - 4 + leftTransition, posY - 1, posX - 4 + leftTransition, posY + 177,
                        ARGB(128, 255, 255, 255));
            r->DrawLine(19 + leftTransition, posY - 1, 19 + leftTransition, posY + 177, ARGB(128, 255, 255, 255));
            r->DrawLine(posX + 40 + leftTransition, posY - 1, posX + 40 + leftTransition, posY + 177,
                        ARGB(128, 255, 255, 255));

            r->DrawLine(20 + leftTransition, posY - 1, posX + 40 + leftTransition, posY - 1, ARGB(128, 255, 255, 255));

            font->DrawString(_("Lands"), 20 + leftTransition, posY);
            sprintf(buffer, _("%i").c_str(), stw->countLands);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 14;
            r->DrawLine(20 + leftTransition, posY - 1, posX + 40 + leftTransition, posY - 1, ARGB(128, 255, 255, 255));
            font->DrawString(_("Creatures"), 20 + leftTransition, posY);
            sprintf(buffer, _("%i").c_str(), stw->countCreatures);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 14;
            r->DrawLine(20 + leftTransition, posY - 1, posX + 40 + leftTransition, posY - 1, ARGB(128, 255, 255, 255));
            font->DrawString(_("Spells"), 20 + leftTransition, posY);
            sprintf(buffer, _("%i").c_str(), stw->countSpells);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 10;
            font->DrawString(_("Instants"), 30 + leftTransition, posY);
            sprintf(buffer, _("%i").c_str(), stw->countInstants);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 10;
            font->DrawString(_("Enchantments"), 30 + leftTransition, posY);
            sprintf(buffer, _("%i").c_str(), stw->countEnchantments);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 10;
            font->DrawString(_("Sorceries"), 30 + leftTransition, posY);
            sprintf(buffer, _("%i").c_str(), stw->countSorceries);
            font->DrawString(buffer, posX + leftTransition, posY);
            // sprintf(buffer, "Artifacts: %i", stw->countArtifacts);
            // mFont->DrawString(buffer, 20, 123);

            posY += 14;
            r->DrawLine(20 + leftTransition, posY - 1, posX + 40 + leftTransition, posY - 1, ARGB(128, 255, 255, 255));

            font->DrawString(_("Average converted mana cost"), 20 + leftTransition, posY);
            sprintf(buffer, _("%2.2f").c_str(), stw->avgManaCost);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 14;
            r->DrawLine(20 + leftTransition, posY - 1, posX + 40 + leftTransition, posY - 1, ARGB(128, 255, 255, 255));
            font->DrawString(_("Probabilities"), 20 + leftTransition, posY);

            posY += 10;
            font->DrawString(_("No land in 1st hand"), 30 + leftTransition, posY);
            sprintf(buffer, _("%2.2f%%").c_str(), stw->noLandsProbInTurn[0]);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 10;
            font->DrawString(_("No land in 9 cards"), 30 + leftTransition, posY);
            sprintf(buffer, _("%2.2f%%").c_str(), stw->noLandsProbInTurn[2]);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 10;
            font->DrawString(_("No creatures in 1st hand"), 30 + leftTransition, posY);
            sprintf(buffer, _("%2.2f%%").c_str(), stw->noCreaturesProbInTurn[0]);
            font->DrawString(buffer, posX + leftTransition, posY);

            // Playgame Statistics
            posY += 14;
            r->DrawLine(20 + leftTransition, posY - 1, posX + 40 + leftTransition, posY - 1, ARGB(128, 255, 255, 255));
            font->DrawString(_("Playgame statistics"), 20 + leftTransition, posY);

            posY += 10;
            font->DrawString(_("Games played"), 30 + leftTransition, posY);
            sprintf(buffer, _("%i").c_str(), stw->gamesPlayed);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 10;
            font->DrawString(_("Victory ratio"), 30 + leftTransition, posY);
            sprintf(buffer, _("%i%%").c_str(), stw->percentVictories);
            font->DrawString(buffer, posX + leftTransition, posY);

            posY += 15;
            r->DrawLine(20 + leftTransition, posY - 1, posX + 40 + leftTransition, posY - 1, ARGB(128, 255, 255, 255));
            font->DrawString(_("Total price (credits)"), 20 + leftTransition, posY);
            sprintf(buffer, _("%i ").c_str(), stw->totalPrice);
            font->DrawString(buffer, posX + leftTransition, posY);
            r->DrawLine(20 + leftTransition, posY + 13, posX + 40 + leftTransition, posY + 13,
                        ARGB(128, 255, 255, 255));

            break;

        case 5:  // Land statistics
            sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage, _("Mana production").c_str());
            font->DrawString(buffer, 10 + leftTransition, 10);

            font->DrawString(_("Counts of manasources per type and color:"), 20 + leftTransition, 30);

            posY = 70;

            // Column titles
            for (int j = 0; j < Constants::NB_Colors - 1; j++) {
                r->RenderQuad(mIcons[j].get(), 52 + j * 15 + leftTransition, posY - 10, 0, 0.5, 0.5);
            }

            // font->DrawString(_("C"), 30 + leftTransition, posY-16);
            // font->DrawString(_("Ty"), 27 + leftTransition, posY-16);

            // Horizontal table lines
            r->DrawLine(27 + leftTransition, posY - 20, 60 + (Constants::NB_Colors - 2) * 15 + leftTransition,
                        posY - 20, ARGB(128, 255, 255, 255));
            r->DrawLine(27 + leftTransition, posY - 1, 60 + (Constants::NB_Colors - 2) * 15 + leftTransition, posY - 1,
                        ARGB(128, 255, 255, 255));
            r->DrawLine(27 + leftTransition, 2 * 10 + posY + 12, 60 + (Constants::NB_Colors - 2) * 15 + leftTransition,
                        2 * 10 + posY + 12, ARGB(128, 255, 255, 255));
            r->DrawLine(27 + leftTransition, 3 * 10 + posY + 14, 60 + (Constants::NB_Colors - 2) * 15 + leftTransition,
                        3 * 10 + posY + 14, ARGB(128, 255, 255, 255));

            // Vertical table lines
            r->DrawLine(26 + leftTransition, posY - 20, 26 + leftTransition, 3 * 10 + posY + 14,
                        ARGB(128, 255, 255, 255));
            r->DrawLine(43 + leftTransition, posY - 20, 43 + leftTransition, 3 * 10 + posY + 14,
                        ARGB(128, 255, 255, 255));
            r->DrawLine(60 + leftTransition + (Constants::NB_Colors - 2) * 15, posY - 20,
                        60 + leftTransition + (Constants::NB_Colors - 2) * 15, 3 * 10 + posY + 14,
                        ARGB(128, 255, 255, 255));

            font->DrawString(_("BL"), 27 + leftTransition, posY);
            font->DrawString(_("NB"), 27 + leftTransition, posY + 10);
            font->DrawString(_("O"), 30 + leftTransition, posY + 20);
            font->DrawString(_("T"), 30 + leftTransition, posY + 33);

            int curCount;

            for (int j = 0; j < Constants::NB_Colors - 1; j++) {
                curCount = stw->countBasicLandsPerColor[j];
                sprintf(buffer, (curCount == 0 ? "." : "%i"), curCount);
                font->DrawString(buffer, 49 + leftTransition + j * 15, posY);

                curCount = stw->countLandsPerColor[j];
                sprintf(buffer, (curCount == 0 ? "." : "%i"), curCount);
                font->DrawString(buffer, 49 + leftTransition + j * 15, posY + 10);

                curCount = stw->countNonLandProducersPerColor[j];
                sprintf(buffer, (curCount == 0 ? "." : "%i"), curCount);
                font->DrawString(buffer, 49 + leftTransition + j * 15, posY + 20);

                curCount = stw->countLandsPerColor[j] + stw->countBasicLandsPerColor[j] +
                           stw->countNonLandProducersPerColor[j];
                sprintf(buffer, (curCount == 0 ? "." : "%i"), curCount);
                font->DrawString(buffer, 49 + leftTransition + j * 15, posY + 33);
            }

            posY += 55;
            font->DrawString(_("BL - Basic lands"), 20 + leftTransition, posY);
            posY += 10;
            font->DrawString(_("NB - Non-basic lands"), 20 + leftTransition, posY);
            posY += 10;
            font->DrawString(_("O - Other (non-land) manasources"), 26 + leftTransition, posY);
            posY += 10;
            font->DrawString(_("T - Totals"), 26 + leftTransition, posY);

            break;

        case 6:  // Land statistics - in symbols
            sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage,
                    _("Mana production - in mana symbols").c_str());
            font->DrawString(buffer, 10 + leftTransition, 10);
            font->DrawString(_("Total colored manasymbols in lands' production:"), 20 + leftTransition, 30);

            int totalProducedSymbols;
            totalProducedSymbols = 0;
            for (int i = 1; i < Constants::NB_Colors - 1; i++) {
                totalProducedSymbols +=
                    stw->countLandsPerColor[i] + stw->countBasicLandsPerColor[i];  //!! Move to updatestats!
            }

            posY = 50;
            for (int i = 1; i < Constants::NB_Colors - 1; i++) {
                if (stw->countLandsPerColor[i] + stw->countBasicLandsPerColor[i] > 0) {
                    sprintf(buffer, _("%i").c_str(), stw->countLandsPerColor[i] + stw->countBasicLandsPerColor[i]);
                    font->DrawString(buffer, 20 + leftTransition, posY);
                    sprintf(buffer, _("(%i%%)").c_str(),
                            (int)(100 * (float)(stw->countLandsPerColor[i] + stw->countBasicLandsPerColor[i]) /
                                  totalProducedSymbols));
                    font->DrawString(buffer, 33 + leftTransition, posY);
                    posX = 72;
                    for (int j = 0; j < stw->countLandsPerColor[i] + stw->countBasicLandsPerColor[i]; j++) {
                        r->RenderQuad(mIcons[i].get(), posX + leftTransition, posY + 6, 0, 0.5, 0.5);
                        posX += ((j + 1) % 10 == 0) ? 17 : 13;
                        if ((((j + 1) % 30) == 0) &&
                            (j < stw->countLandsPerColor[i] + stw->countBasicLandsPerColor[i] - 1)) {
                            posX = 72;
                            posY += 15;
                        }
                    }
                    posY += 17;
                }
            }

            break;

        case 2:  // Mana cost detail
        case 3:
        case 4:
            int(*countPerCost)[Constants::STATS_MAX_MANA_COST + 1];
            int(*countPerCostAndColor)[Constants::STATS_MAX_MANA_COST + 1][Constants::MTG_NB_COLORS + 1];
            float avgCost;

            switch (stw->currentPage) {  // Nested switch on the same variable. Oh yes.
            case 2:                      // Total counts
                // Title
                sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage, _("Mana cost detail").c_str());
                font->DrawString(buffer, 10 + leftTransition, 10);
                font->DrawString(_("Card counts per mana cost:"), 20 + leftTransition, 30);
                avgCost = stw->avgManaCost;
                countPerCost = &stw->countCardsPerCost;
                countPerCostAndColor = &stw->countCardsPerCostAndColor;
                break;
            case 3:  // Creature counts
                // Title
                sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage,
                        _("Mana cost detail - Creatures").c_str());
                font->DrawString(buffer, 10 + leftTransition, 10);
                font->DrawString(_("Creature counts per mana cost:"), 20 + leftTransition, 30);
                avgCost = stw->avgCreatureCost;
                countPerCost = &stw->countCreaturesPerCost;
                countPerCostAndColor = &stw->countCreaturesPerCostAndColor;
                break;
            case 4:  // Spell counts
                // Title
                sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage, _("Mana cost detail - Spells").c_str());
                font->DrawString(buffer, 10 + leftTransition, 10);
                font->DrawString(_("Non-creature spell counts per mana cost:"), 20 + leftTransition, 30);
                avgCost = stw->avgSpellCost;
                countPerCost = &stw->countSpellsPerCost;
                countPerCostAndColor = &stw->countSpellsPerCostAndColor;
                break;
            default:
                countPerCost = NULL;
                countPerCostAndColor = NULL;
                avgCost = 0;
                break;
            }

            posY = 70;

            // Column titles
            for (int j = 0; j < Constants::NB_Colors - 1; j++) {
                r->RenderQuad(mIcons[j].get(), 67 + j * 15 + leftTransition, posY - 10, 0, 0.5, 0.5);
            }

            font->DrawString(_("C"), 30 + leftTransition, posY - 16);
            font->DrawString(_("#"), 45 + leftTransition, posY - 16);

            // Horizontal table lines
            r->DrawLine(27 + leftTransition, posY - 20, 75 + (Constants::NB_Colors - 2) * 15 + leftTransition,
                        posY - 20, ARGB(128, 255, 255, 255));
            r->DrawLine(27 + leftTransition, posY - 1, 75 + (Constants::NB_Colors - 2) * 15 + leftTransition, posY - 1,
                        ARGB(128, 255, 255, 255));
            r->DrawLine(27 + leftTransition, Constants::STATS_MAX_MANA_COST * 10 + posY + 12,
                        75 + (Constants::NB_Colors - 2) * 15 + leftTransition,
                        Constants::STATS_MAX_MANA_COST * 10 + posY + 12, ARGB(128, 255, 255, 255));

            // Vertical table lines
            r->DrawLine(26 + leftTransition, posY - 20, 26 + leftTransition,
                        Constants::STATS_MAX_MANA_COST * 10 + posY + 12, ARGB(128, 255, 255, 255));
            r->DrawLine(41 + leftTransition, posY - 20, 41 + leftTransition,
                        Constants::STATS_MAX_MANA_COST * 10 + posY + 12, ARGB(128, 255, 255, 255));
            r->DrawLine(58 + leftTransition, posY - 20, 58 + leftTransition,
                        Constants::STATS_MAX_MANA_COST * 10 + posY + 12, ARGB(128, 255, 255, 255));
            r->DrawLine(75 + leftTransition + (Constants::NB_Colors - 2) * 15, posY - 20,
                        75 + leftTransition + (Constants::NB_Colors - 2) * 15,
                        Constants::STATS_MAX_MANA_COST * 10 + posY + 12, ARGB(128, 255, 255, 255));

            for (int i = 0; i <= Constants::STATS_MAX_MANA_COST; i++) {
                sprintf(buffer, _("%i").c_str(), i);
                font->DrawString(buffer, 30 + leftTransition, posY);
                sprintf(buffer, ((*countPerCost)[i] > 0) ? _("%i").c_str() : ".", (*countPerCost)[i]);
                font->DrawString(buffer, 45 + leftTransition, posY);
                for (int j = 0; j < Constants::NB_Colors - 1; j++) {
                    sprintf(buffer, ((*countPerCostAndColor)[i][j] > 0) ? _("%i").c_str() : ".",
                            (*countPerCostAndColor)[i][j]);
                    font->DrawString(buffer, 64 + leftTransition + j * 15, posY);
                }
                r->FillRect(77.f + leftTransition + (Constants::NB_Colors - 2) * 15.0f, posY + 2.0f,
                            (*countPerCost)[i] * 5.0f, 8.0f, graphColor);
                posY += 10;
            }

            posY += 10;
            sprintf(buffer, _("Average converted mana cost: %2.2f").c_str(), avgCost);
            font->DrawString(buffer, 20 + leftTransition, posY);
            posY += 15;
            sprintf(buffer, _("C - Converted mana cost. Cards with cost>%i are included in the last row.").c_str(),
                    Constants::STATS_MAX_MANA_COST);
            font->DrawString(buffer, 20 + leftTransition, posY);
            posY += 10;
            font->DrawString(_("# - Total number of cards with given cost"), 20 + leftTransition, posY);

            break;

        case 8:
            // Title
            sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage, _("Probabilities").c_str());
            font->DrawString(buffer, 10 + leftTransition, 10);

            // No lands detail
            float graphScale, graphWidth;
            graphWidth = 100;
            graphScale = (stw->noLandsProbInTurn[0] == 0) ? 0 : (graphWidth / stw->noLandsProbInTurn[0]);
            font->DrawString(_("No lands in first n cards:"), 20 + leftTransition, 30);

            posY = 50;
            for (int i = 0; i < Constants::STATS_FOR_TURNS; i++) {
                sprintf(buffer, _("%i:").c_str(), i + 7);
                font->DrawString(buffer, 30 + leftTransition, posY);
                sprintf(buffer, _("%2.2f%%").c_str(), stw->noLandsProbInTurn[i]);
                font->DrawString(buffer, 45 + leftTransition, posY);
                r->FillRect(84 + leftTransition, posY + 2, graphScale * stw->noLandsProbInTurn[i], 8, graphColor);
                posY += 10;
            }

            // No creatures probability detail
            posY += 10;
            font->DrawString(_("No creatures in first n cards:"), 20 + leftTransition, posY);
            posY += 20;
            graphScale = (stw->noCreaturesProbInTurn[0] == 0) ? 0 : (graphWidth / stw->noCreaturesProbInTurn[0]);

            for (int i = 0; i < Constants::STATS_FOR_TURNS; i++) {
                sprintf(buffer, _("%i:").c_str(), i + 7);
                font->DrawString(buffer, 30 + leftTransition, posY);
                sprintf(buffer, _("%2.2f%%").c_str(), stw->noCreaturesProbInTurn[i]);
                font->DrawString(buffer, 45 + leftTransition, posY);
                r->FillRect(84 + leftTransition, posY + 2, graphScale * stw->noCreaturesProbInTurn[i], 8, graphColor);
                posY += 10;
            }

            break;

        case 7:  // Total mana cost per color
            // Title
            sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage, _("Mana cost per color").c_str());
            font->DrawString(buffer, 10 + leftTransition, 10);

            font->DrawString(_("Total colored manasymbols in cards' casting costs:"), 20 + leftTransition, 30);

            posY = 50;
            for (int i = 1; i < Constants::NB_Colors - 1; i++) {
                if (stw->totalCostPerColor[i] > 0) {
                    sprintf(buffer, _("%i").c_str(), stw->totalCostPerColor[i]);
                    font->DrawString(buffer, 20 + leftTransition, posY);
                    sprintf(buffer, _("(%i%%)").c_str(),
                            (int)(100 * (float)stw->totalCostPerColor[i] / stw->totalColoredSymbols));
                    font->DrawString(buffer, 33 + leftTransition, posY);
                    posX = 72;
                    for (int j = 0; j < stw->totalCostPerColor[i]; j++) {
                        r->RenderQuad(mIcons[i].get(), posX + leftTransition, posY + 6, 0, 0.5, 0.5);
                        posX += ((j + 1) % 10 == 0) ? 17 : 13;
                        if ((((j + 1) % 30) == 0) && (j < stw->totalCostPerColor[i] - 1)) {
                            posX = 72;
                            posY += 15;
                        }
                    }
                    posY += 17;
                }
            }
            break;

        case 9:  // Victory statistics
            // Title
            sprintf(buffer, STATS_TITLE_FORMAT.c_str(), stw->currentPage, _("Victory statistics").c_str());
            font->DrawString(buffer, 10 + leftTransition, 10);

            font->DrawString(_("Victories against AI:"), 20 + leftTransition, 30);

            sprintf(buffer, _("Games played: %i").c_str(), stw->gamesPlayed);
            font->DrawString(buffer, 20 + leftTransition, 45);
            sprintf(buffer, _("Victory ratio: %i%%").c_str(), stw->percentVictories);
            font->DrawString(buffer, 20 + leftTransition, 55);

            int AIsPerColumn = 19;
            posY = 70;
            posX = 20;

            // ToDo: Multiple pages when too many AI decks are present
            for (int i = 0; i < (int)stw->aiDeckStats.size(); i++) {
                sprintf(buffer, _("%.14s").c_str(), stw->aiDeckNames.at(i).c_str());
                font->DrawString(buffer, posX + (i < 2 * AIsPerColumn ? leftTransition : rightTransition), posY);
                sprintf(buffer, _("%i/%i").c_str(), stw->aiDeckStats.at(i)->victories,
                        stw->aiDeckStats.at(i)->nbgames);
                font->DrawString(buffer, posX + (i < AIsPerColumn ? leftTransition : rightTransition) + 80, posY);
                sprintf(buffer, _("%i%%").c_str(), stw->aiDeckStats.at(i)->percentVictories());
                font->DrawString(buffer, posX + (i < AIsPerColumn ? leftTransition : rightTransition) + 110, posY);
                posY += 10;
                if (((i + 1) % AIsPerColumn) == 0) {
                    posY = 70;
                    posX += 155;
                }
            }
            break;
        }
    }
}

void GameStateDeckViewer::renderCard(int id, float rotation) {
    WFont* mFont = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);
    MTGCard* card = cardIndex[id];

    float max_scale = 0.96f;
    float x_center_0 = 180;
    float right_border = SCREEN_WIDTH + 180;

    float x_center = x_center_0 + cos((rotation + 8 - id) * M_PI / 12) * (right_border - x_center_0);
    float scale = max_scale / 1.12f * cos((x_center - x_center_0) * 1.5f / (right_border - x_center_0)) +
                  0.2f * max_scale * cos(cos((x_center - x_center_0) * 0.15f / (right_border - x_center_0)));
    float x = x_center;  // ;

    float y = (SCREEN_HEIGHT_F) / 2.0f + SCREEN_HEIGHT_F * mSlide * (scale + 0.2f);

    cardsCoordinates[id] = std::pair<float, float>(x, y);

    int alpha = (int)(255 * (scale + 1.0 - max_scale));

    if (!card) return;

    if (!WResourceManager::Instance()->IsThreaded()) {
        JQuadPtr backQuad = WResourceManager::Instance()->GetQuad(kGenericCardID);
        JQuadPtr quad;

        int cacheError = CACHE_ERROR_NONE;

        if (!options[Options::DISABLECARDS].number) {
            quad = WResourceManager::Instance()->RetrieveCard(card, RETRIEVE_EXISTING);
            cacheError = WResourceManager::Instance()->RetrieveError();
            if (!quad.get() && cacheError != CACHE_ERROR_404) {
                if (last_user_activity > (abs(2 - id) + 1) * NO_USER_ACTIVITY_SHOWCARD_DELAY)
                    quad = WResourceManager::Instance()->RetrieveCard(card);
                else {
                    quad = backQuad;
                }
            }
        }

        if (quad.get()) {
            if (quad == backQuad) {
                quad->SetColor(ARGB(255, 255, 255, 255));
                float _scale = scale * (285 / quad->mHeight);
                JRenderer::GetInstance()->RenderQuad(quad.get(), x, y, 0.0f, _scale, _scale);
            } else {
                Pos pos = Pos(x, y, scale * 285 / 250, 0.0, 255);
                CardGui::DrawCard(card, pos);
            }
        } else {
            Pos pos = Pos(x, y, scale * 285 / 250, 0.0, 255);
            CardGui::DrawCard(card, pos, DrawMode::kText);
        }
    } else {
        int mode = !options[Options::DISABLECARDS].number ? DrawMode::kNormal : DrawMode::kText;

        Pos pos = Pos(x, y, scale * 285 / 250, 0.0, 255);
        CardGui::DrawCard(card, pos, mode);
    }

    int quadAlpha = alpha;
    if (!displayed_deck->count(card)) quadAlpha /= 2;
    quadAlpha = 255 - quadAlpha;
    if (quadAlpha > 0) {
        JRenderer::GetInstance()->FillRect(x - scale * 100.0f, y - scale * 142.5f, scale * 200.0f, scale * 285.0f,
                                           ARGB(quadAlpha, 0, 0, 0));
    }
    if (last_user_activity < 3) {
        int fontAlpha = alpha;
        float qtY = y - 135 * scale;
        float qtX = x + 40 * scale;
        char buffer[4096];
        sprintf(buffer, "x%i", displayed_deck->count(card));
        WFont* font = mFont;
        font->SetColor(ARGB(fontAlpha / 2, 0, 0, 0));
        JRenderer::GetInstance()->FillRect(qtX, qtY, font->GetStringWidth(buffer) + 6, 16,
                                           ARGB(fontAlpha / 2, 0, 0, 0));
        font->DrawString(buffer, qtX + 4, qtY + 4);
        font->SetColor(ARGB(fontAlpha, 255, 255, 255));
        font->DrawString(buffer, qtX + 2, qtY + 2);
        font->SetColor(ARGB(255, 255, 255, 255));
    }
}

void GameStateDeckViewer::renderCard(int id) { renderCard(id, 0); }

void GameStateDeckViewer::Render() {
    setButtonState(false);
    WFont* mFont = WResourceManager::Instance()->GetWFont(Fonts::MAIN_FONT);
    JRenderer::GetInstance()->ClearScreen(ARGB(0, 0, 0, 0));
    if (displayed_deck == myDeck && mStage != STAGE_MENU) renderDeckBackground();
    int order[3] = {1, 2, 3};
    if (mRotation < 0.5 && mRotation > -0.5) {
        order[1] = 3;
        order[2] = 2;
    } else if (mRotation < -0.5) {
        order[0] = 3;
        order[2] = 1;
    }

    // even though we want to draw the cards in a particular z order for layering, we want to prefetch them
    // in a different order, ie the center card should appear first, then the adjacent ones
    if (WResourceManager::Instance()->IsThreaded()) {
        WResourceManager::Instance()->RetrieveCard(cardIndex[0]);
        WResourceManager::Instance()->RetrieveCard(cardIndex[3]);
        WResourceManager::Instance()->RetrieveCard(cardIndex[4]);
        WResourceManager::Instance()->RetrieveCard(cardIndex[2]);
        WResourceManager::Instance()->RetrieveCard(cardIndex[5]);
        WResourceManager::Instance()->RetrieveCard(cardIndex[1]);
        WResourceManager::Instance()->RetrieveCard(cardIndex[6]);
    }

    renderCard(6, mRotation);
    renderCard(5, mRotation);
    renderCard(4, mRotation);
    renderCard(0, mRotation);

    for (int i = 0; i < 3; i++) {
        renderCard(order[i], mRotation);
    }

    if (displayed_deck->Size() > 0) {
        setButtonState(true);
        renderSlideBar();
    } else {
        mFont->DrawString(_("No Card"), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, JGETEXT_CENTER);
    }
    if (mStage == STAGE_ONSCREEN_MENU) {
        renderOnScreenMenu();
    } else if (mStage == STAGE_WELCOME) {
        setButtonState(false);
        welcome_menu->Render();
    } else {
        setButtonState(true);
        renderOnScreenBasicInfo();
    }

    if (mStage == STAGE_MENU) {
        setButtonState(false);
        menu->Render();
    }

    if (subMenu) subMenu->Render();

    if (filterMenu && !filterMenu->isFinished()) {
        setButtonState(false);
        filterMenu->Render();
    }

    if (options.keypadActive()) options.keypadRender();

    RenderButtons();
}

int GameStateDeckViewer::loadDeck(int deckid) {
    if (!stw) {
        DeckManager* deckManager = DeckManager::GetInstance();
        stw = deckManager->getExtendedStatsForDeckId(deckid, MTGCollection(), false);
    }

    stw->currentPage = 0;
    stw->pageCount = 9;
    stw->needUpdate = true;

    if (!playerdata) playerdata = NEW PlayerData(MTGCollection());
    SAFE_DELETE(myCollection);
    myCollection = NEW DeckDataWrapper(playerdata->collection);
    myCollection->Sort(WSrcCards::SORT_ALPHA);
    displayed_deck = myCollection;

    char deckname[256];
    sprintf(deckname, "deck%i.txt", deckid);
    if (myDeck) {
        SAFE_DELETE(myDeck->parent);
        SAFE_DELETE(myDeck);
    }
    myDeck = NEW DeckDataWrapper(NEW MTGDeck(options.profileFile(deckname, "", false).c_str(), MTGCollection()));

    // Check whether the cards in the deck are actually available in the player's collection:
    int cheatmode = options[Options::CHEATMODE].number;
    bool bPure = true;
    for (int i = 0; i < myDeck->Size(true); i++) {
        MTGCard* current = myDeck->getCard(i, true);
        int howmanyinDeck = myDeck->count(current);
        for (int i = myCollection->count(current); i < howmanyinDeck; i++) {
            bPure = false;
            if (cheatmode) {                           // Are we cheating?
                playerdata->collection->add(current);  // Yup, add it to collection permanently.
                myCollection->Add(current);
            } else {
                myDeck->Remove(current, howmanyinDeck - i);  // Nope. Remove it from deck.
                break;
            }
        }

        myCollection->Remove(current, myDeck->count(current));
    }
    if (!bPure) {
        myDeck->validate();
        myCollection->validate();
    }

    myDeck->Sort(WSrcCards::SORT_ALPHA);
    SAFE_DELETE(filterMenu);
    rebuildFilters();
    loadIndexes();
    return 1;
}

void GameStateDeckViewer::ButtonPressed(int controllerId, int controlId) {
    int deckIdNumber = controlId;
    int deckListSize = 0;
    string defaultAiName;
    DeckManager* deckManager = DeckManager::GetInstance();
    vector<DeckMetaData*>* deckList;
    switch (controllerId) {
    case MENU_DECK_SELECTION:  // Deck menu
        if (controlId == MENU_ITEM_CANCEL) {
            if (!mSwitching)
                mParent->DoTransition(TRANSITION_FADE, GAME_STATE_MENU);
            else
                mStage = STAGE_WAITING;

            mSwitching = false;
            break;
        } else if (controlId == MENUITEM_MORE_INFO) {
            break;
        } else if (controlId == MENU_ITEM_CHEAT_MODE) {  // (PSY) Cheatmode: Complete the collection
            playerdata->collection->complete();          // Add the cards
            playerdata->collection->save();              // Save the new collection
            for (int i = 0; i < setlist.size(); i++) {   // Update unlocked sets
                GameOptionAward* goa = dynamic_cast<GameOptionAward*>(&options[Options::optionSet(i)]);
                if (goa) goa->giveAward();
            }
            options.save();
            SAFE_DELETE(myCollection);
            myCollection = NEW DeckDataWrapper(playerdata->collection);
            myCollection->Sort(WSrcCards::SORT_ALPHA);
            displayed_deck = myCollection;
            rebuildFilters();
            loadIndexes();
            mStage = STAGE_WELCOME;
            break;
        }
        mStage = STAGE_WAITING;
        deckList = deckManager->getPlayerDeckOrderList();
        deckListSize = deckList->size();

        if (controlId == MENU_ITEM_NEW_DECK)  // new deck option selected
            deckIdNumber = deckList->size() + 1;
        else if (deckListSize > 0 && controlId <= deckListSize)
            deckIdNumber = deckList->at(controlId - 1)->getDeckId();
        else
            deckIdNumber = controlId;

        loadDeck(deckIdNumber);
        mStage = STAGE_WAITING;
        break;

    case MENU_DECK_BUILDER:  // Save / exit menu
        switch (controlId) {
        case MENU_ITEM_SAVE_RETURN_MAIN_MENU:
            saveDeck();
            mParent->DoTransition(TRANSITION_FADE, GAME_STATE_MENU);
            break;

        case MENU_ITEM_SAVE_RENAME:
            if (myDeck && myDeck->parent) {
                options.keypadStart(myDeck->parent->meta_name, &newDeckname);
                options.keypadTitle("Rename deck");
            }
            break;

        case MENU_ITEM_SAVE_AS_AI_DECK:
            // find the next unused ai deck number
            // warn user that once saved, no edits can be made
            // save entire collection to ai as spelled out card with count
            // bring user to main deck editor menu.
            isAIDeckSave = true;
            defaultAiName = myDeck && myDeck->parent ? myDeck->parent->meta_name : "Custom AI Deck";
            options.keypadStart(defaultAiName, &newDeckname);
            options.keypadTitle("Name Custom AI Deck");
            updateDecks();
            mStage = STAGE_WELCOME;
            mSwitching = true;
            break;

        case MENU_ITEM_SWITCH_DECKS_NO_SAVE:
            mStage = STAGE_WELCOME;
            mSwitching = true;
            break;
        case MENU_ITEM_MAIN_MENU:
            mParent->DoTransition(TRANSITION_FADE, GAME_STATE_MENU);
            break;
        case MENU_ITEM_EDITOR_CANCEL:
            mStage = STAGE_WAITING;
            break;
        case MENU_ITEM_FILTER_BY:
            mStage = STAGE_FILTERS;
            if (!filterMenu) rebuildFilters();
            filterMenu->Entering(JGE_BTN_NONE);
            break;
        }
        break;

    case MENU_CARD_PURCHASE:  // Yes/ No sub menu.
        switch (controlId) {
        case MENU_ITEM_YES: {
            MTGCard* card = cardIndex[2];
            if (card) {
                int rnd = (rand() % 25);
                playerdata->credits += price;
                price = price - (rnd * price) / 100;
                pricelist->setPrice(card->getMTGId(), price);
                playerdata->collection->remove(card->getMTGId());
                displayed_deck->Remove(card, 1);
                displayed_deck->validate();
                stw->needUpdate = true;
                loadIndexes();
            }
        }
        case MENU_ITEM_NO:
            subMenu->Close();
            break;
        }
    }
}

/*
  to keep actions consistent across the different platforms, we need to branch the way swipes are interpreted.  iOS5
 changes the way a swipe moves a document on the page.  swipe down is to simulate dragging the page down instead of
 moving down on a page.
 */
void GameStateDeckViewer::OnScroll(int inXVelocity, int inYVelocity) {
    int magnitude = static_cast<int>(sqrtf((float)((inXVelocity * inXVelocity) + (inYVelocity * inYVelocity))));

    bool flickHorizontal = (abs(inXVelocity) > abs(inYVelocity));
    bool flickUp = !flickHorizontal && (inYVelocity > 0) ? true : false;
    bool flickRight = flickHorizontal && (inXVelocity < 0) ? true : false;

    if (flickHorizontal) {
        if (abs(inXVelocity) > 300) {
            // determine how many cards to move, the faster the velocity the more cards to move.
            // the display is setup so that there is a max of 2 cards to the left and 7 cards to the right
            // of the current card.
            int numCards = (magnitude / 500) % 8;
            int offset = 0;
            if ((numCards == 0) && magnitude) numCards = 7;
            if (!flickRight) {
                if (numCards > 1) offset = 0;
            } else
                offset = 2 + numCards;

            mEngine->LeftClickedProcessed();
            mEngine->LeftClicked(static_cast<int>(cardsCoordinates[offset].first),
                                 static_cast<int>(cardsCoordinates[offset].second));
            mEngine->HoldKey_NoRepeat(JGE_BTN_OK);
        }
    } else
        mEngine->HoldKey_NoRepeat(flickUp ? JGE_BTN_UP : JGE_BTN_DOWN);
}
