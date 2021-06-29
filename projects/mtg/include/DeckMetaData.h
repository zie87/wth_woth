#ifndef _DECKMETADATA_H_
#define _DECKMETADATA_H_

#include <string>
#include <vector>
#include <map>
#include "DeckStats.h"

enum DECK_DIFFICULTY { HARD = -1, NORMAL = 0, EASY = 1 };

class DeckMetaData {
private:
    std::string mFilename;
    std::string mDescription;
    std::string mName;
    std::vector<int> mUnlockRequirements;
    int mDeckId;
    std::string mAvatarFilename;
    std::string mColorIndex;

    // statistical information
    int mGamesPlayed, mVictories, mPercentVictories, mDifficulty;
    int getAvatarId() const;

    DeckMetaData();

public:
    DeckMetaData(const std::string& filename, bool isAI = false);
    void LoadDeck();
    void LoadStats();

    // Accessors
    std::string getFilename();
    std::string getDescription();
    std::string getName();
    std::string getAvatarFilename();
    std::string getColorIndex();
    std::string getStatsSummary();
    std::vector<int> getUnlockRequirements();

    int getDeckId() const;
    int getGamesPlayed() const;
    int getVictories() const;
    int getVictoryPercentage() const;
    int getDifficulty() const;
    std::string getDifficultyString() const;

    // setters
    void setColorIndex(const std::string& colorIndex);
    void setDeckName(const std::string& newDeckTitle);
    void Invalidate();

    std::string mStatsFilename;
    std::string mPlayerDeck;
    bool mDeckLoaded;
    bool mStatsLoaded;
    bool mIsAI;
};

#endif
