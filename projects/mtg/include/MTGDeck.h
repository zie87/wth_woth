#ifndef _MTGDECK_H_
#define _MTGDECK_H_

#define MTG_ERROR -1

#include "MTGDefinitions.h"
#include "GameApp.h"
#include "WResourceManager.h"
#include <dirent.h>
#include <Subtypes.h>
#include <string>

#include <wge/thread.hpp>

using std::string;
class GameApp;
class MTGCard;
class CardPrimitive;
class MTGPack;
class MTGSetInfo {
public:
    MTGSetInfo(std::string  _id);
    ~MTGSetInfo();
    std::string id;      // Short name: 10E, RAV, etc. Automatic from folder.
    std::string author;  // Author of set, for crediting mod makers, etc.
    std::string name;    // Long name: Tenth Edition
    int block;      // For future use by tournament mode, etc.
    int year;       // The year the set was released.
    // TODO Way to group cards by name, rather than mtgid.

    void count(MTGCard* c);

    int totalCards();
    std::string getName();
    std::string getBlock();
    void processConfLine(std::string  line);

    enum {
        // For memoized counts
        LAND = 0,
        COMMON = 1,
        UNCOMMON = 2,
        RARE = 3,
        MAX_RARITY = 4,  // For boosters, mythic is part of rare... always.
        MYTHIC = 4,
        TOTAL_CARDS = 5,
        MAX_COUNT = 6
    };

    MTGPack* mPack;     // Does it use a specialized booster pack?
    bool bZipped;       // Is this set's images present as a zip file?
    bool bThemeZipped;  //[...] in the theme?
    int counts[MTGSetInfo::MAX_COUNT];
};

class MTGSets {
public:
    // These values have to be < 0
    // A setID with a value >=0 will be looked into the sets table,
    // Negative values will be compared to these enums throughout the code (shop, filters...)
    enum {
        INTERNAL_SET = -1,
        ALL_SETS = -2,
    };

    friend class MTGSetInfo;
    MTGSets();
    ~MTGSets();

    int Add(const char* subtype);
    int findSet(std::string  value);
    int findBlock(std::string  s);
    int size();

    int getSetNum(MTGSetInfo* i);

    int operator[](std::string  id);  // Returns set id index, -1 for failure.
    std::string operator[](int id);  // Returns set id name, "" for failure.

    MTGSetInfo* getInfo(int setID);
    MTGSetInfo* randomSet(int blockId = -1, int atleast = -1);  // Tries to match, otherwise 100% random unlocked set

protected:
    std::vector<std::string> blocks;
    std::vector<MTGSetInfo*> setinfo;
};

extern MTGSets setlist;

class MTGAllCards {
private:
    MTGCard* tempCard;             // used by parser
    CardPrimitive* tempPrimitive;  // used by parser
    int currentGrade;  // used by Parser (we don't want an additional attribute for the primitives for that as it is
                       // only used at load time)
    static MTGAllCards* instance;

protected:
    int conf_read_mode;
    std::vector<int> colorsCount;
    int total_cards;
    void init();
    void initCounters();
    MTGAllCards();
    ~MTGAllCards();

public:
    enum {
        READ_ANYTHING = 0,
        READ_CARD = 1,
        READ_METADATA = 2,
    };
    std::vector<int> ids;
    std::map<int, MTGCard*> collection;
    std::map<std::string, CardPrimitive*> primitives;
    MTGCard* _(int id);
    MTGCard* getCardById(int id);

#ifdef TESTSUITE
    void prefetchCardNameCache();
#endif

    MTGCard* getCardByName(std::string  name);
    int load(const char* config_file, const char* setName = NULL, int autoload = 1);
    int countByType(const char* _type);
    int countByColor(int color);
    int countBySet(int setId);
    int totalCards();
    int randomCardId();

    static int findType(std::string  subtype, bool forceAdd = true) {
        std::lock_guard<wge::mutex> lock(instance->mMutex);
        return instance->subtypesList.find(subtype, forceAdd);
    };
    static int add(std::string  value, unsigned int parentType) {
        std::lock_guard<wge::mutex> lock(instance->mMutex);
        return instance->subtypesList.add(value, parentType);
    };
    static std::string findType(unsigned int id) { return instance->subtypesList.find(id); };
    static const std::vector<std::string>& getValuesById() { return instance->subtypesList.getValuesById(); };
    static const std::vector<std::string>& getCreatureValuesById() { return instance->subtypesList.getCreatureValuesById(); };
    static bool isSubtypeOfType(unsigned int subtype, unsigned int type) {
        return instance->subtypesList.isSubtypeOfType(subtype, type);
    };
    static bool isSuperType(unsigned int type) { return instance->subtypesList.isSuperType(type); };
    static bool isType(unsigned int type) { return instance->subtypesList.isType(type); };
    static bool isSubType(unsigned int type) { return instance->subtypesList.isSubType(type); };

    static void sortSubtypeList() {
        std::lock_guard<wge::mutex> lock(instance->mMutex);
        instance->subtypesList.sortSubTypes();
    }

    static int findSubtypeId(std::string  value) { return instance->subtypesList.find(value, false); }

    static void loadInstance();
    static void unloadAll();
    static inline MTGAllCards* getInstance() { return instance; };

private:
    wge::mutex mMutex;
    Subtypes subtypesList;
    std::map<std::string, MTGCard*> mtgCardByNameCache;
    int processConfLine(std::string & s, MTGCard* card, CardPrimitive* primitive);
    bool addCardToCollection(MTGCard* card, int setId);
    CardPrimitive* addPrimitive(CardPrimitive* primitive, MTGCard* card = NULL);
};

#define MTGCollection() MTGAllCards::getInstance()

class MTGDeck {
private:
    std::string getCardBlockText(const std::string& title, const std::string& textBlock);
    void printDetailedDeckText(std::ofstream& file);

protected:
    std::string filename;
    int total_cards;

public:
    MTGAllCards* database;
    std::map<int, int> cards;
    std::string meta_desc;
    std::string meta_name;
    std::vector<std::string> meta_AIHints;
    std::string meta_unlockRequirements;

    int meta_id;
    int totalCards();
    int totalPrice();
    MTGDeck(MTGAllCards* _allcards);
    MTGDeck(const char* config_file, MTGAllCards* _allcards, int meta_only = 0, int difficultySetting = 0);
    int addRandomCards(int howmany, int* setIds = NULL, int nbSets = 0, int rarity = -1, const char* subtype = NULL,
                       int* colors = NULL, int nbcolors = 0);
    int add(int cardid);
    int add(MTGDeck* deck);  // adds the contents of "deck" into myself
    int complete();
    int remove(int cardid);
    int removeAll();
    int add(MTGCard* card);
    int remove(MTGCard* card);
    std::string getFilename();
    int save();
    int save(const std::string& destFileName, bool useExpandedDescriptions, const std::string& deckTitle,
             const std::string& deckDesc);
    MTGCard* getCardById(int id);
};

#endif
