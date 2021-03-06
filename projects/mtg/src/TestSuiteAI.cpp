#include "PrecompiledHeader.h"

#include "TestSuiteAI.h"
#include "MTGAbility.h"
#include "MTGRules.h"
#include "ActionLayer.h"
#include "GuiCombat.h"
#include "Rules.h"
#include "GameObserver.h"
#include "GameStateShop.h"

#include <wge/log.hpp>

#ifdef TESTSUITE

// NULL is sent in place of a MTGDeck since there is no way to create a MTGDeck without a proper deck file.
// TestSuiteAI will be responsible for managing its own deck state.
TestSuiteAI::TestSuiteAI(TestSuiteGame* tsGame, int playerId)
    : AIPlayerBaka(tsGame->observer, "testsuite", "testsuite", "baka.jpg", nullptr) {
    SAFE_DELETE(game);  // game might have been set in the parent with default values
    game = tsGame->buildDeck(this, playerId);
    game->setOwner(this);

    suite          = tsGame;
    timer          = 0;
    playMode       = MODE_TEST_SUITE;
    this->deckName = "Test Suite AI";
}

MTGCardInstance* TestSuiteAI::getCard(string action) {
    int mtgid = Rules::getMTGId(action);
    if (mtgid) return Rules::getCardByMTGId(observer, mtgid);

    // This mostly handles tokens
    std::transform(action.begin(), action.end(), action.begin(), ::tolower);
    for (int i = 0; i < 2; i++) {
        Player* p            = observer->players[i];
        MTGGameZone* zones[] = {p->game->library, p->game->hand, p->game->inPlay, p->game->graveyard};
        for (int j = 0; j < 4; j++) {
            MTGGameZone* zone = zones[j];
            for (int k = 0; k < zone->nb_cards; k++) {
                MTGCardInstance* card = zone->cards[k];
                if (!card) return nullptr;
                string name = card->getLCName();
                if (name == action) return card;
            }
        }
    }
    WGE_LOG_ERROR("Can't find card: {}", action);
    return nullptr;
}

Interruptible* TestSuiteGame::getActionByMTGId(int mtgid) {
    ActionStack* as       = observer->mLayers->stackLayer();
    Interruptible* action = nullptr;
    while ((action = as->getNext(action, 0, 0, 1))) {
        if (action->source && action->source->getMTGId() == mtgid) {
            return action;
        }
    }
    return nullptr;
}

int TestSuiteAI::displayStack() {
    if (playMode == MODE_AI) return 0;
    return 1;
}

int TestSuiteAI::Act(float dt) {
    observer->setLoser(nullptr);  // Prevent draw rule from losing the game

    // Last bits of initialization require to be done here, after the first "update" call of the game

    if (suite->currentAction == 0) {
        for (int i = 0; i < 2; ++i)
            observer->players[i]->getManaPool()->copy(suite->initState.players[i]->getManaPool());
    }

    if (playMode == MODE_AI && suite->aiMaxCalls) {
        float static counter = 1.0f;
        suite->aiMaxCalls--;
        suite->timerLimit = 40;        // TODO Remove this limitation when AI is not using a stupid timer anymore...
        AIPlayerBaka::Act(counter++);  // dt);
    }
    if (playMode == MODE_HUMAN) {
        observer->mLayers->CheckUserInput(0);
        suite->currentAction++;  // hack to avoid repeating the initialization of manapool
        return 1;
    }

    timer += 1;
    if (timer < suite->timerLimit) return 1;
    timer = 0;

    string action = suite->getNextAction();
    WGE_LOG_TRACE("command: {}", action);

    if (observer->mLayers->stackLayer()->askIfWishesToInterrupt == this) {
        if (action != "no" && action != "yes") {
            observer->mLayers->stackLayer()->cancelInterruptOffer();
            suite->currentAction--;
            return 1;
        }
    }

    if (action == "") {
        // end of game
        suite->assertGame();
        observer->setLoser(observer->players[0]);
        WGE_LOG_TRACE("================================    END OF TEST   =======================\n");
        return 1;
    }

    if (action == "eot") {
        if (observer->getCurrentGamePhase() != MTG_PHASE_CLEANUP) suite->currentAction--;
        observer->userRequestNextGamePhase();
    } else if (action == "human") {
        WGE_LOG_TRACE("You have control");
        playMode = MODE_HUMAN;
        return 1;
    } else if (action == "ai") {
        WGE_LOG_TRACE("Switching to AI");
        playMode = MODE_AI;
        return 1;
    } else if (action == "next" || action.find("goto") != string::npos) {
        if (action.find("goto ") != string::npos) {
            size_t found  = action.find("goto ");
            string phase  = action.substr(found + 5);
            int phaseToGo = 0;
            for (int i = 0; i < NB_MTG_PHASES; i++) {
                if (phase.find(Constants::MTGPhaseCodeNames[i]) != string::npos) {
                    phaseToGo = i;
                }
            }
            if (observer->getCurrentGamePhase() != phaseToGo)
                suite->currentAction--;
            else {
                return 1;
            }
            GuiCombat* gc = observer->mLayers->combatLayer();
            if (ORDER == observer->combatStep || DAMAGE == observer->combatStep) {
                gc->clickOK();
            } else {
                observer->userRequestNextGamePhase();
            }
        } else {
            GuiCombat* gc = observer->mLayers->combatLayer();
            if (ORDER == observer->combatStep || DAMAGE == observer->combatStep)
                gc->clickOK();
            else
                observer->userRequestNextGamePhase();
        }
    } else if (action == "yes")
        observer->mLayers->stackLayer()->setIsInterrupting(this);
    else if (action == "endinterruption")
        observer->mLayers->stackLayer()->endOfInterruption();
    else if (action == "no") {
        if (observer->mLayers->stackLayer()->askIfWishesToInterrupt == this)
            observer->mLayers->stackLayer()->cancelInterruptOffer();
    } else if (action.find("choice ") != string::npos) {
        WGE_LOG_TRACE("choice !!!");
        int choice = atoi(action.substr(action.find("choice ") + 7).c_str());
        observer->mLayers->actionLayer()->doReactTo(choice);
    } else if (action.find(" -momir- ") != string::npos) {
        int start       = action.find(" -momir- ");
        int cardId      = Rules::getMTGId(action.substr(start + 9));
        int cardIdHand  = Rules::getMTGId(action.substr(0, start));
        MTGMomirRule* a = ((MTGMomirRule*)observer->mLayers->actionLayer()->getAbility(MTGAbility::MOMIR));
        a->reactToClick(Rules::getCardByMTGId(observer, cardIdHand), cardId);
        observer->mLayers->actionLayer()->stuffHappened = 1;
    } else if (action.find("p1") != string::npos || action.find("p2") != string::npos) {
        Player* p    = observer->players[1];
        size_t start = action.find("p1");
        if (start != string::npos) p = observer->players[0];
        observer->cardClick(nullptr, p);
    } else {
        int mtgid                  = Rules::getMTGId(action);
        Interruptible* toInterrupt = nullptr;
        if (mtgid) {
            WGE_LOG_TRACE("TESTSUITE CARD ID: {}", mtgid);
            toInterrupt = suite->getActionByMTGId(mtgid);
        }

        if (toInterrupt) {
            observer->stackObjectClicked(toInterrupt);
            return 1;
        }

        MTGCardInstance* card = getCard(action);
        if (card) {
            WGE_LOG_TRACE("TESTSUITE Clicking ON: {}", card->name);
            card->currentZone->needShuffle = true;  // mimic library shuffle
            observer->cardClick(card, card);
            observer->forceShuffleLibraries();  // mimic library shuffle
            return 1;
        }
    }
    return 0;
}

TestSuiteActions::TestSuiteActions() { nbitems = 0; }

void TestSuiteActions::add(string s) {
    actions[nbitems] = s;
    nbitems++;
}

TestSuiteState::TestSuiteState() { players.clear(); }

TestSuiteState::~TestSuiteState() {
    if (players.size()) {
        if (players[0]) SAFE_DELETE(players[0]);
        if (players[1]) SAFE_DELETE(players[1]);
    }
};

void TestSuiteState::parsePlayerState(int playerId, string s) { players[playerId]->parseLine(s); }

string TestSuiteGame::getNextAction() {
    currentAction++;
    if (actions.nbitems && currentAction <= actions.nbitems) {
        return actions.actions[currentAction - 1];
    }
    return "";
}

void TestSuiteGame::handleResults(bool wasAI, int error) { testsuite->handleResults(wasAI, error); };

void TestSuite::initGame(GameObserver* g) {
    observer = g;
    // The first test runs slowly, the other ones run faster.
    // This way a human can see what happens when testing a specific file,
    // or go faster when it comes to the whole test suite.
    // Warning, putting this value too low (< 3) will give unexpected results
    if (!timerLimit) {
        timerLimit = 40;
    } else {
        timerLimit = 3;
    }

    TestSuiteGame::initGame();
}

int TestSuiteGame::Log(const char* text) {
    std::ofstream file;
    if (JFileSystem::GetInstance()->openForWrite(file, "test/results.html", std::ios_base::app)) {
        file << text;
        file << "\n";
        file.close();
    }

    WGE_LOG_DEBUG("test result: {}", text);
    return 1;
}

void TestSuiteGame::assertGame() {
    mMutex.lock();
    // compare the game state with the results
    char result[4096];
    sprintf(result, "<h3>%s</h3>", filename.c_str());
    Log(result);

    int error  = 0;
    bool wasAI = false;

    if (observer->getCurrentGamePhase() != endState.phase) {
        sprintf(result, "<span class=\"error\">==phase problem. Expected [ %s ](%i), got [ %s ](%i)==</span><br />",
                Constants::MTGPhaseNames[endState.phase], endState.phase,
                Constants::MTGPhaseNames[observer->currentGamePhase], observer->currentGamePhase);
        Log(result);
        error++;
    }
    for (int i = 0; i < 2; i++) {
        auto* p = (TestSuiteAI*)(observer->players[i]);
        if (p->playMode == Player::MODE_AI) wasAI = true;

        if (p->life != endState.players[i]->life) {
            sprintf(result, "<span class=\"error\">==life problem for player %i. Expected %i, got %i==</span><br />", i,
                    endState.players[i]->life, p->life);
            Log(result);
            error++;
        }
        if (p->poisonCount != endState.players[i]->poisonCount) {
            sprintf(result,
                    "<span class=\"error\">==poison counter problem for player %i. Expected %i, got %i==</span><br />",
                    i, endState.players[i]->poisonCount, p->poisonCount);
            Log(result);
            error++;
        }
        if (!p->getManaPool()->canAfford(endState.players[i]->getManaPool())) {
            sprintf(result,
                    "<span class=\"error\">==Mana problem. Was expecting %i but got %i for player %i==</span><br />",
                    endState.players[i]->getManaPool()->getConvertedCost(), p->getManaPool()->getConvertedCost(), i);
            Log(result);
            error++;
        }
        if (!endState.players[i]->getManaPool()->canAfford(p->getManaPool())) {
            sprintf(result,
                    "<span class=\"error\">==Mana problem. Was expecting %i but got %i for player %i==</span><br />",
                    endState.players[i]->getManaPool()->getConvertedCost(), p->getManaPool()->getConvertedCost(), i);
            Log(result);

            if (endState.players[i]->getManaPool()->getConvertedCost() == p->getManaPool()->getConvertedCost()) {
                sprintf(result,
                        "<span class=\"error\">====(Apparently Mana Color issues since converted cost is the "
                        "same)==</span><br />");
                Log(result);
            }
            error++;
        }
        MTGGameZone* playerZones[]   = {p->game->graveyard, p->game->library, p->game->hand, p->game->inPlay};
        MTGGameZone* endstateZones[] = {endState.players[i]->game->graveyard, endState.players[i]->game->library,
                                        endState.players[i]->game->hand, endState.players[i]->game->inPlay};
        for (int j = 0; j < 4; j++) {
            MTGGameZone* zone = playerZones[j];
            if (zone->nb_cards != endstateZones[j]->nb_cards) {
                sprintf(result,
                        "<span class=\"error\">==Card number not the same in player %i's %s==, expected %i, got "
                        "%i</span><br />",
                        i, zone->getName(), endstateZones[j]->nb_cards, zone->nb_cards);
                Log(result);
                error++;
            }
            for (size_t k = 0; k < (size_t)endstateZones[j]->nb_cards; k++) {
                MTGCardInstance* cardToCheck =
                    (k < endstateZones[j]->cards.size()) ? endstateZones[j]->cards[k] : nullptr;
                if (cardToCheck) {  // Can be NULL if used "*" in the testcase.
                    MTGCardInstance* card = Rules::getCardByMTGId(observer, cardToCheck->getId());
                    if (card != nullptr && !zone->hasCard(card)) {
                        sprintf(result, "<span class=\"error\">==Card ID not the same. Didn't find %i</span><br />",
                                card->getId());
                        Log(result);
                        error++;
                    }
                }
            }
        }
    }
    handleResults(wasAI, error);

    if (!error)
        Log("<span class=\"success\">==Test Succesful !==</span>");
    else
        Log("<span class=\"error\">==Test Failed !==</span>");
    mMutex.unlock();
}

void TestSuite::handleResults(bool wasAI, int error) {
    if (wasAI) {
        nbAITests++;
        if (error) {
            nbAIFailed++;
        }
    } else {
        nbTests++;
        if (error) {
            nbFailed++;
        }
    }
}

TestSuite::~TestSuite() {
    mProcessing = false;
    while (mWorkerThread.size()) {
        mWorkerThread.back()->join();
        SAFE_DELETE(mWorkerThread.back());
        mWorkerThread.pop_back();
    }

    observer = nullptr;
}

TestSuite::TestSuite(const char* filename) : TestSuiteGame(nullptr), mRules(nullptr), mProcessing(false) {
    timerLimit = 0;
    testsuite  = this;

    std::string s;
    nbfiles      = 0;
    currentfile  = 0;
    nbFailed     = 0;
    nbTests      = 0;
    nbAIFailed   = 0;
    nbAITests    = 0;
    int comment  = 0;
    seed         = 0;
    forceAbility = false;
    aiMaxCalls   = -1;
    startTime    = JGEGetTime();
    endTime      = startTime;
    std::string contents;
    if (JFileSystem::GetInstance()->readIntoString(filename, contents)) {
        std::stringstream stream(contents);
        while (std::getline(stream, s)) {
            if (!s.size()) continue;
            if (s[s.size() - 1] == '\r') s.erase(s.size() - 1);  // Handle DOS files
            if (!s.size()) continue;
            if (s[0] == '/' && s[1] == '*') comment = 1;
            if (s[0] && s[0] != '#' && !comment) {
                files[nbfiles] = s;
                nbfiles++;
            }
            if (s[0] == '*' && s[1] == '/') comment = 0;
        }
    }

    // If more than 1 test, prefecth names to make the suite run faster
    if (nbfiles > 1) MTGCollection()->prefetchCardNameCache();

    std::ofstream file2;
    if (JFileSystem::GetInstance()->openForWrite(file2, "/test/results.html")) {
        file2 << "<html><head>";
#ifdef WIN32
        file2 << "<meta http-equiv=\"refresh\" content=\"10\" >";
#endif
        file2 << "<STYLE type='text/css'>";
        file2 << ".success {color:green}\n";
        file2 << ".error {color:red}\n";
        file2 << "</STYLE></head><body>\n";
        file2.close();
    }

    SAFE_DELETE(observer);
}

int TestSuite::loadNext() {
    endTime           = JGEGetTime();
    summoningSickness = 0;
    seed              = 0;
    aiMaxCalls        = -1;
    if (!nbfiles) return 0;

    filename = getNextFile();
    if (filename == "") {
        // we let GameStateDuel delete the latest gameObserver.
        mProcessing = false;
        observer    = nullptr;
        return 0;
    }

    if (filename == "+pregametests") {
        pregameTests();
        return loadNext();
    }

    if (!mProcessing) {  // "I don't like to wait" mode
        mProcessing = true;
        while (mWorkerThread.size()) {
            mWorkerThread.back()->join();
            SAFE_DELETE(mWorkerThread.back());
            mWorkerThread.pop_back();
        }

        //FIXME: more threads creating a lot of false positives!
        constexpr unsigned int max_number_of_threads = 2U;
        const auto thread_count = std::min(max_number_of_threads, wge::thread::hardware_concurrency());
        for (size_t i = 0; i < (thread_count - 1); i++) mWorkerThread.push_back(new wge::thread(ThreadProc, this));
    }

    cleanup();
    if (!load())
        return loadNext();
    else
        WGE_LOG_INFO("Starting test : {}", files[currentfile - 1]);
    return currentfile;
}

void TestSuiteActions::cleanup() { nbitems = 0; }

void TestSuiteState::cleanup(TestSuiteGame* tsGame) {
    for (size_t i = 0; i < players.size(); i++) {
        SAFE_DELETE(players[i]);
    }
    players.clear();

    players.push_back(new TestSuiteAI(tsGame, 0));
    players.push_back(new TestSuiteAI(tsGame, 1));
}

void TestSuite::cleanup() {
    currentAction = 0;
    observer      = nullptr;
    initState.cleanup(this);
    endState.cleanup(this);
    actions.cleanup();
}

bool TestSuiteGame::load() {
    summoningSickness = 0;
    forceAbility      = false;
    gameType          = GAME_TYPE_CLASSIC;

    std::string s;

    int state = -1;

    std::string contents;
    if (JFileSystem::GetInstance()->readIntoString("test/" + filename, contents)) {
        std::stringstream stream(contents);
        while (std::getline(stream, s)) {
            if (!s.size()) continue;
            if (s[s.size() - 1] == '\r') s.erase(s.size() - 1);  // Handle DOS files
            if (!s.size()) continue;
            if (s[0] == '#') continue;
            std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            if (s == "summoningsickness") {
                summoningSickness = 1;
                continue;
            }
            if (s == "forceability") {
                forceAbility = true;
                continue;
            }
            if (s.find("seed ") == 0) {
                seed = atoi(s.substr(5).c_str());
                continue;
            }
            if (s.find("aicalls ") == 0) {
                aiMaxCalls = atoi(s.substr(8).c_str());
                continue;
            }
            if (s == "momir") {
                gameType = GAME_TYPE_MOMIR;
                continue;
            }
            switch (state) {
            case -1:
                if (s == "[init]") state++;
                break;
            case 0:
                if (s == "[player1]") {
                    state++;
                } else {
                    initState.phase = PhaseRing::phaseStrToInt(s);
                }
                break;
            case 1:
                if (s == "[player2]") {
                    state++;
                } else {
                    initState.parsePlayerState(0, s);
                }
                break;
            case 2:
                if (s == "[do]") {
                    state++;
                } else {
                    initState.parsePlayerState(1, s);
                }
                break;
            case 3:
                if (s == "[assert]") {
                    state++;
                } else {
                    actions.add(s);
                }
                break;
            case 4:
                if (s == "[player1]") {
                    state++;
                } else {
                    endState.phase = PhaseRing::phaseStrToInt(s);
                }
                break;
            case 5:
                if (s == "[player2]") {
                    state++;
                } else {
                    endState.parsePlayerState(0, s);
                }
                break;
            case 6:
                if (s == "[end]") {
                    state++;
                } else {
                    endState.parsePlayerState(1, s);
                }
                break;
            }
        }
    } else {
        return false;
    }
    return true;
}

void TestSuite::pregameTests() {
    // Test Booster Generation
    srand(1024);
    char result[1024];
    ShopBooster sb;
    for (int i = 0; i < 5; i++) {
        nbTests++;
        sprintf(result, "<h3>pregame/BoosterTest#%i</h3>", i);
        Log(result);
        if (!sb.unitTest()) nbFailed++;
    }
}

void TestSuite::ThreadProc(void* inParam) {
    WGE_LOG_TRACE("Entering");
    auto* instance = reinterpret_cast<TestSuite*>(inParam);
    if (instance) {
        string filename;
        float counter = 1.0f;
        while (instance->mProcessing && (filename = instance->getNextFile()) != "") {
            TestSuiteGame theGame(instance, filename);
            if (theGame.isOK) {
                theGame.observer->loadTestSuitePlayer(0, &theGame);
                theGame.observer->loadTestSuitePlayer(1, &theGame);

                theGame.observer->startGame(theGame.gameType, Rules::getRulesByFilename("testsuite.txt"));
                theGame.initGame();

                while (!theGame.observer->didWin()) {
                    theGame.observer->Update(counter++);
                }
            }
        }
    }
    WGE_LOG_TRACE("Leaving");
}

wge::size_t TestSuite::run() {
    mProcessing = true;
    ThreadProc(this);

    return nbFailed + nbAIFailed;
}

wge::mutex TestSuiteGame::mMutex;

TestSuiteGame::~TestSuiteGame() { SAFE_DELETE(observer); }

TestSuiteGame::TestSuiteGame(TestSuite* testsuite)
    : summoningSickness(0),
      forceAbility(false),
      gameType(GAME_TYPE_CLASSIC),
      timerLimit(0),
      currentAction(0),
      observer(nullptr),
      testsuite(testsuite) {}

TestSuiteGame::TestSuiteGame(TestSuite* testsuite, string _filename)
    : summoningSickness(0),
      forceAbility(false),
      gameType(GAME_TYPE_CLASSIC),
      timerLimit(3),
      currentAction(0),
      observer(nullptr),
      testsuite(testsuite) {
    filename = _filename;
    observer = new GameObserver();

    initState.cleanup(this);
    endState.cleanup(this);

    isOK = load();
}

void TestSuiteGame::ResetManapools() {
    for (int i = 0; i < 2; ++i) observer->players[i]->getManaPool()->copy(initState.players[i]->getManaPool());
}

void TestSuiteGame::initGame() {
    WGE_LOG_TRACE("Init Game");
    observer->phaseRing->goToPhase(initState.phase, observer->players[0], false);
    observer->currentGamePhase = initState.phase;

    observer->resetStartupGame();

    for (int i = 0; i < 2; i++) {
        auto* p                = (AIPlayerBaka*)(observer->players[i]);
        p->forceBestAbilityUse = forceAbility;
        p->life                = initState.players[i]->life;
        p->poisonCount         = initState.players[i]->poisonCount;
        std::stringstream stream;
        initState.players[i]->getRandomGenerator()->saveLoadedRandValues(stream);
        p->getRandomGenerator()->loadRandValues(stream.str());
        MTGGameZone* playerZones[]       = {p->game->graveyard, p->game->library, p->game->hand, p->game->inPlay};
        MTGGameZone* loadedPlayerZones[] = {initState.players[i]->game->graveyard, initState.players[i]->game->library,
                                            initState.players[i]->game->hand, initState.players[i]->game->inPlay};
        for (int j = 0; j < 4; j++) {
            MTGGameZone* zone = playerZones[j];
            for (size_t k = 0; k < loadedPlayerZones[j]->cards.size(); k++) {
                MTGCardInstance* card = Rules::getCardByMTGId(observer, loadedPlayerZones[j]->cards[k]->getId());
                if (card && zone != p->game->library) {
                    if (zone == p->game->inPlay) {
                        MTGCardInstance* copy = p->game->putInZone(card, p->game->library, p->game->stack);
                        auto* spell           = NEW Spell(observer, copy);
                        spell->resolve();
                        if (!summoningSickness && (size_t)p->game->inPlay->nb_cards > k)
                            p->game->inPlay->cards[k]->summoningSickness = 0;
                        delete spell;
                    } else {
                        if (!p->game->library->hasCard(card)) {
                            WGE_LOG_ERROR("card not found in library!");
                        }
                        p->game->putInZone(card, p->game->library, zone);
                    }
                } else {
                    if (!card) {
                        WGE_LOG_ERROR("card is NULL!");
                    }
                }
            }
            zone->cardsSeenThisTurn
                .clear();  // don't consider those cards as having moved in this area during this turn
        }
        p->game->stack->cardsSeenThisTurn
            .clear();  // don't consider those cards as having moved in this area during this turn
    }
    WGE_LOG_TRACE("Init Game Done !");
}

MTGPlayerCards* TestSuiteGame::buildDeck(Player* player, int playerId) {
    int list[100];
    int nbcards          = 0;
    MTGPlayerCards* deck = nullptr;

    if (initState.players.size() > (size_t)playerId) {
        MTGGameZone* loadedPlayerZones[] = {
            initState.players[playerId]->game->graveyard, initState.players[playerId]->game->library,
            initState.players[playerId]->game->hand, initState.players[playerId]->game->inPlay};

        for (int j = 0; j < 4; j++) {
            for (size_t k = 0; k < loadedPlayerZones[j]->cards.size(); k++) {
                int cardid    = loadedPlayerZones[j]->cards[k]->getId();
                list[nbcards] = cardid;
                nbcards++;
            }
        }
        deck = NEW MTGPlayerCards(player, list, nbcards);
    } else {
        deck = NEW MTGPlayerCards();
    }

    return deck;
}

#endif
