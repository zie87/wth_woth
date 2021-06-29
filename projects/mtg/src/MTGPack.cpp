#include "PrecompiledHeader.h"

#include <JGui.h>
#include <dirent.h>

#include "GameApp.h"
#include "Translate.h"
#include "WDataSrc.h"
#include "WFilter.h"
#include "DeckDataWrapper.h"
#include "MTGPack.h"

#include "../../../JGE/src/tinyxml/tinyxml.h"

#include <wge/algorithms.hpp>

MTGPack MTGPacks::defaultBooster;

int MTGPackEntryRandom::addCard(WSrcCards* pool, MTGDeck* to) {
    int fails = 0;
    if (!pool) return 1;
    WCFilterFactory* ff = WCFilterFactory::GetInstance();
    WCardFilter* oldf = pool->unhookFilters();
    pool->addFilter(ff->Construct(filter));
    fails = pool->addRandomCards(to, copies);
    pool->clearFilters();
    pool->addFilter(oldf);
    return fails;
}
int MTGPackEntrySpecific::addCard(WSrcCards* pool, MTGDeck* to) {
    if (!card) return copies;
    for (int i = 0; i < copies; i++) to->add(card);
    return 0;
}

int MTGPackSlot::add(WSrcCards* ocean, MTGDeck* to, int carryover) {
    if (!entries.size()) return copies;
    int fails = 0;
    int amt = copies + carryover;
    WSrcCards* myPool = nullptr;
    if (pool.size()) myPool = MTGPack::getPool(pool);
    if (!myPool) myPool = ocean;

    for (int i = 0; i < amt; i++) {
        wge::shuffle(entries.begin(), entries.end());
        size_t pos = 0;
        while (pos < entries.size() && entries[pos]->addCard(myPool, to)) pos++;
        if (pos == entries.size()) fails++;
    }
    if (myPool != ocean) SAFE_DELETE(myPool);
    return fails;
}

WSrcCards* MTGPack::getPool(string poolstr) {
    WSrcCards* mySrc    = nullptr;
    size_t s = poolstr.find("all");
    WCFilterFactory* ff = WCFilterFactory::GetInstance();

    if (s == string::npos) {  // Default to just unlocked cards
        mySrc = NEW WSrcUnlockedCards();
        s = poolstr.find("unlocked");
        string sub = poolstr;
        if (s != string::npos) sub = trim(poolstr.substr(s + 8));
        if (sub.size()) {
            mySrc->addFilter(ff->Construct(sub));
            mySrc->bakeFilters();
        }
    } else {  // Use everything.
        mySrc = NEW WSrcCards();
        string sub = poolstr.substr(s + 3);
        if (sub.size()) {
            mySrc->addFilter(ff->Construct(sub));
            mySrc->loadMatches(MTGCollection());
            mySrc->bakeFilters();
        } else
            mySrc->loadMatches(MTGCollection());
    }
    mySrc->Shuffle();
    return mySrc;
}

void MTGPackSlot::addEntry(MTGPackEntry* item) {
    if (item) entries.push_back(item);
}
int MTGPack::assemblePack(MTGDeck* to) {
    int carryover = 0;
    WSrcCards* p = getPool(pool);
    if (!p) return -1;
    p->Shuffle();

    for (auto& slots : slotss) {
        carryover = slots->add(p, to, carryover);
        if (carryover > 0) carryover = carryover;  // This means we're failing.
    }
    SAFE_DELETE(p);
    return carryover;
}
void MTGPack::countCards() {
    minCards = 0;
    maxCards = 0;
    for (auto ps : slotss) {
        int top = 0;
        int bot = 999999999;
        for (size_t y = 0; y < ps->entries.size(); y++) {
            int test = ps->entries[y]->copies * ps->copies;
            if (test > top) top = test;
            if (test < bot) bot = test;
        }
        maxCards += top;
        minCards += bot;
    }
}
void MTGPack::load(string filename) {
    // TODO Placeholder until XML format available.
    TiXmlDocument packfile(filename.c_str());
    if (!packfile.LoadFile()) return;
    TiXmlHandle hDoc(&packfile);
    TiXmlElement* pPack;
    pPack = hDoc.FirstChildElement().Element();
    if (!pPack) {
        return;
    }
    // root should be "pack"
    string tag = pPack->Value();
    std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
    if (tag != "pack") return;
    // After validating, handle actual loading.
    TiXmlElement* pSlot;
    const char* holder = nullptr;
    holder = pPack->Attribute("price");
    if (holder)
        price = atoi(holder);
    else
        price = Constants::PRICE_BOOSTER;
    holder = pPack->Attribute("pool");
    if (holder)
        pool = holder;
    else
        pool = "";
    holder = pPack->Attribute("type");
    if (holder) {
        type = holder;
    } else
        type = "Booster";
    holder = pPack->Attribute("name");
    if (holder)
        name = holder;
    else
        name = "Special";
    holder = pPack->Attribute("requires");
    if (holder) check = holder;
    holder = pPack->Attribute("sort");
    if (holder)
        sort = holder;
    else
        sort = "";
    std::transform(sort.begin(), sort.end(), sort.begin(), ::tolower);

    for (pSlot = pPack->FirstChildElement(); pSlot != nullptr; pSlot = pSlot->NextSiblingElement()) {
        TiXmlElement* pEntry;
        // Load slot.
        tag = pSlot->Value();
        std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
        if (tag != "slot") continue;
        auto* s = NEW MTGPackSlot();
        slotss.push_back(s);
        holder = pSlot->Attribute("copies");
        if (holder)
            s->copies = atoi(holder);
        else
            s->copies = 1;
        holder = pSlot->Attribute("pool");
        if (holder) s->pool = holder;

        for (pEntry = pSlot->FirstChildElement(); pEntry != nullptr; pEntry = pEntry->NextSiblingElement()) {
            tag = pEntry->Value();
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            if (tag == "card") {  // Load specific card
                auto* es = NEW MTGPackEntrySpecific();
                holder = pEntry->Attribute("copies");
                if (holder)
                    es->copies = atoi(holder);
                else
                    es->copies = 1;
                es->card = MTGCollection()->getCardByName(pEntry->Value());
                s->addEntry(es);
            } else if (tag == "random_card") {  // Load random card
                auto* er = NEW MTGPackEntryRandom();
                holder = pEntry->Attribute("copies");
                if (holder)
                    er->copies = atoi(holder);
                else
                    er->copies = 1;
                const char* text = pEntry->GetText();
                if (text) er->filter = text;
                s->addEntry(er);
            } else if (tag == "nothing") {
                auto* nt = NEW MTGPackEntryNothing();
                s->addEntry(nt);
            }
        }
    }
    bValid = true;
    countCards();
    return;
}
MTGPackSlot::~MTGPackSlot() {
    for (auto& entrie : entries) {
        SAFE_DELETE(entrie);
    }
    entries.clear();
}
MTGPack::~MTGPack() {
    for (auto& slots : slotss) {
        SAFE_DELETE(slots);
    }
    slotss.clear();
}
MTGPacks::~MTGPacks() {
    for (auto& pack : packs) {
        SAFE_DELETE(pack);
    }
    packs.clear();
}
MTGPack* MTGPacks::randomPack(int key) {
    if (!key) key = rand();
    size_t s = packs.size();
    if (!s) return nullptr;
    return packs[key % s];
}
void MTGPacks::loadAll() {
    vector<string> packFiles = JFileSystem::GetInstance()->scanfolder("packs/");
    for (auto relative : packFiles) {
        char myFilename[4096];
        sprintf(myFilename, "packs/%s", relative.c_str());
        if (relative[0] == '.') continue;
        if (!strcmp(relative.c_str(), "default_booster.txt")) continue;
        auto* p = NEW MTGPack(myFilename);
        if (!p->isValid()) {
            SAFE_DELETE(p);
            continue;
        }
        packs.push_back(p);
    }
}
string MTGPack::getName() {
    string n = _(name);
    string t = _(type);
    char buf[1024];
    if (minCards != maxCards)
        sprintf(buf, "%s %s (%i-%i cards)", n.c_str(), t.c_str(), minCards, maxCards);
    else
        sprintf(buf, "%s %s (%i cards)", n.c_str(), t.c_str(), maxCards);
    return buf;
}

bool MTGPack::meetsRequirements() {
    bool unlocked = true;
    WCFilterFactory* ff = WCFilterFactory::GetInstance();
    WSrcCards* myC = getPool(pool);
    if (!myC || myC->Size() < maxCards) unlocked = false;  // Top pool lacks cards.
    SAFE_DELETE(myC);
    if (!check.size() || !unlocked) return unlocked;
    myC = NEW WSrcUnlockedCards();  // Requirements are independent of pool;
    WCardFilter* cf = ff->Construct(check);
    unlocked = !myC->isEmptySet(cf);  // Quick check for empty set status.
    SAFE_DELETE(cf);                  // delete requirement filter
    SAFE_DELETE(myC);                 // delete pool.
    return unlocked;
}

bool MTGPack::isUnlocked() {
    if (unlockStatus == 0) {
        if (meetsRequirements())
            unlockStatus = 1;
        else
            unlockStatus = -1;
    }
    return (unlockStatus > 0);
}

MTGPack* MTGPacks::getDefault() {
    if (!defaultBooster.isValid()) {
        defaultBooster.load("packs/default_booster.txt");
        defaultBooster.unlockStatus = 1;
        if (!defaultBooster.isValid()) {
            auto* ps   = NEW MTGPackSlot();
            ps->copies = 1;
            ps->addEntry(NEW MTGPackEntryRandom("rarity:mythic;"));
            for (int i = 0; i < 7; i++) ps->addEntry(NEW MTGPackEntryRandom("rarity:rare;"));
            defaultBooster.slotss.push_back(ps);
            ps = NEW MTGPackSlot();
            ps->copies = 3;
            ps->addEntry(NEW MTGPackEntryRandom("rarity:uncommon;"));
            defaultBooster.slotss.push_back(ps);
            ps = NEW MTGPackSlot();
            ps->copies = 1;
            ps->addEntry(NEW MTGPackEntryRandom("rarity:land;&type:basic;"));
            defaultBooster.slotss.push_back(ps);
            ps = NEW MTGPackSlot();
            ps->copies = 10;
            ps->addEntry(NEW MTGPackEntryRandom("rarity:common;"));
            defaultBooster.slotss.push_back(ps);
            defaultBooster.bValid = true;
            defaultBooster.unlockStatus = 1;
            for (auto& slots : defaultBooster.slotss) {
                slots->addEntry(NEW MTGPackEntryRandom("rarity:special;"));
            }
        }
    }
    return &defaultBooster;
}

void MTGPacks::refreshUnlocked() {
    for (auto& pack : packs) {
        if (pack->unlockStatus < 0) pack->unlockStatus = 0;
    }
}
