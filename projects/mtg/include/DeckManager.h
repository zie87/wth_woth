#include <string>
#include <vector>

#include "DeckMetaData.h"

class DeckManager {
private:
    std::vector<DeckMetaData*> playerDeckOrderList;
    std::vector<DeckMetaData*> aiDeckOrderList;
    std::map<std::string, StatsWrapper*> playerDeckStatsMap;
    std::map<std::string, StatsWrapper*> aiDeckStatsMap;

    static DeckManager* mInstance;

public:
    DeckManager() {
        // private constructor
    }

    void updateMetaDataList(std::vector<DeckMetaData*>* refList, bool isAI);
    std::vector<DeckMetaData*>* getPlayerDeckOrderList();
    std::vector<DeckMetaData*>* getAIDeckOrderList();

    void AddMetaData(const std::string& filename, bool isAI);
    void DeleteMetaData(const std::string& filename, bool isAI);
    DeckMetaData* getDeckMetaDataById(int deckId, bool isAI);
    DeckMetaData* getDeckMetaDataByFilename(const std::string& filename, bool isAI);
    StatsWrapper* getExtendedStatsForDeckId(int deckId, MTGAllCards* collection, bool isAI);
    StatsWrapper* getExtendedDeckStats(DeckMetaData* selectedDeck, MTGAllCards* collection, bool isAI);

    static DeckManager* GetInstance();
    static void EndInstance();

    // convenience method to get the difficulty rating between two decks.  This should be refined a little more
    // since the eventual move of all deck meta data should be managed by this class

    int getDifficultyRating(Player* statsPlayer, Player* player);

    ~DeckManager();
};
