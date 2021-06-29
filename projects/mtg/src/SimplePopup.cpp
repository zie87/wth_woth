/*
 * SimplePopup.cpp
 *
 *  Created on: Nov 18, 2010
 *      Author: Michael
 *  The popups all have a default button created to act as the dismiss button, mapped to JGE_BTN_CANCEL (triangle) with
 * "Detailed Info" to use for the button width.
 */

#include "PrecompiledHeader.h"
#include "SimplePopup.h"
#include "JTypes.h"
#include "GameApp.h"
#include "DeckStats.h"
#include "DeckManager.h"

#include <wge/log.hpp>

#include <iomanip>

SimplePopup::SimplePopup(int id, JGuiListener* listener, const int fontId, const char* _title,
                         DeckMetaData* deckMetaData, MTGAllCards* collection, float cancelX, float cancelY)
    : JGuiController(JGE::GetInstance(), id, listener), mFontId(fontId), mCollection(collection) {
    mX = 19;
    mY = 66;
    mWidth = 180.0f;
    mTitle = _title;
    mMaxLines = 12;

    mTextFont = WResourceManager::Instance()->GetWFont(fontId);
    this->mCount = 1;  // a hack to ensure the menus do book keeping correctly.  Since we aren't adding items to the
                       // menu, this is required
    mStatsWrapper = nullptr;

    JGuiController::Add(NEW InteractiveButton(this, kDismissButtonId, Fonts::MAIN_FONT, "Detailed Info", cancelX,
                                              cancelY, JGE_BTN_CANCEL),
                        true);
    Update(deckMetaData);
}

void SimplePopup::Render() {
    mClosed = false;

    JRenderer* r = JRenderer::GetInstance();
    string detailedInformation = getDetailedInformation(mDeckInformation->getFilename());

    const float textHeight = mTextFont->GetHeight() * mMaxLines;
    r->FillRoundRect(mX, mY + 2, mWidth + 11, textHeight - 12, 2.0f, ARGB(255, 0, 0, 0));

    // currently causes a crash on the PSP when drawing the corners.
    // TODO: clean up the image ot make it loook cleaner. Find solution to load gfx to not crash PSP
#if 0
    r->DrawRoundRect(mX, mY + 2, mWidth + 11, textHeight - 12, 2.0f, ARGB( 255, 125, 255, 0) );
	drawBoundingBox( mX-3, mY, mWidth + 3, textHeight );
#endif
    mTextFont->DrawString(detailedInformation.c_str(), mX + 9, mY + 10);
}

// draws a bounding box around the popup.
void SimplePopup::drawBoundingBox(float x, float y, float width, float height) {
    // draw the corners
    string topCornerImageName = "top_corner.png";
    string bottomCornerImageName = "bottom_corner.png";
    string verticalBarImageName = "vert_bar.png";
    string horizontalBarImageName = "top_bar.png";

    const float boxWidth = (width + 15) / 3.0f;
    const float boxHeight = (height + 15) / 3.0f;

    drawHorzPole(horizontalBarImageName, false, false, x, y, boxWidth);
    drawHorzPole(horizontalBarImageName, false, true, x, y + height, boxWidth);

    drawVertPole(verticalBarImageName, false, false, x, y, boxHeight);
    drawVertPole(verticalBarImageName, true, false, x + width, y, boxHeight);

    drawCorner(topCornerImageName, false, false, x, y);
    drawCorner(topCornerImageName, true, false, x + width, y);
    drawCorner(bottomCornerImageName, false, false, x, y + height);
    drawCorner(bottomCornerImageName, true, false, x + width, y + height);
}

void SimplePopup::Update(DeckMetaData* selectedDeck) {
    mDeckInformation = selectedDeck;

    // get the information from the cache, if it doesn't exist create an entry
    mStatsWrapper = DeckManager::GetInstance()->getExtendedDeckStats(
        mDeckInformation, mCollection, (mDeckInformation->getFilename().find("baka") != string::npos));
}

std::string SimplePopup::getDetailedInformation(std::string filename) {
    std::ostringstream oss;
    oss << "------- Deck Summary -----" << std::endl
        << "Cards: " << mStatsWrapper->cardCount << std::endl
        << "Creatures: " << std::setw(2) << mStatsWrapper->countCreatures
        << "  Enchantments: " << mStatsWrapper->countEnchantments << std::endl
        << "Instants: " << std::setw(4) << mStatsWrapper->countInstants << "   Sorceries:      " << std::setw(2)
        << mStatsWrapper->countSorceries << std::endl
        << "Lands: "
        << "A: " << std::setw(2) << std::left
        << mStatsWrapper->countLandsPerColor[Constants::MTG_COLOR_ARTIFACT] +
               mStatsWrapper->countBasicLandsPerColor[Constants::MTG_COLOR_ARTIFACT]
        << " "
        << "G: " << std::setw(2) << std::left
        << mStatsWrapper->countLandsPerColor[Constants::MTG_COLOR_GREEN] +
               mStatsWrapper->countLandsPerColor[Constants::MTG_COLOR_GREEN]
        << " "
        << "R: " << std::setw(2) << std::left
        << mStatsWrapper->countLandsPerColor[Constants::MTG_COLOR_RED] +
               mStatsWrapper->countBasicLandsPerColor[Constants::MTG_COLOR_RED]
        << " "
        << "U: " << std::setw(2) << std::left
        << mStatsWrapper->countLandsPerColor[Constants::MTG_COLOR_BLUE] +
               mStatsWrapper->countBasicLandsPerColor[Constants::MTG_COLOR_BLUE]
        << " "
        << "B: " << std::setw(2) << std::left
        << mStatsWrapper->countLandsPerColor[Constants::MTG_COLOR_BLACK] +
               mStatsWrapper->countBasicLandsPerColor[Constants::MTG_COLOR_BLACK]
        << " "
        << "W: " << std::setw(2) << std::left
        << mStatsWrapper->countLandsPerColor[Constants::MTG_COLOR_WHITE] +
               mStatsWrapper->countBasicLandsPerColor[Constants::MTG_COLOR_WHITE]
        << std::endl
        << "  --- Mana Curve ---  " << std::endl;

    for (int costIdx = 0; costIdx < Constants::STATS_MAX_MANA_COST + 1; ++costIdx)
        if (mStatsWrapper->countCardsPerCost[costIdx] > 0)
            oss << costIdx << ": " << std::setw(2) << std::left << mStatsWrapper->countCardsPerCost[costIdx] << "  ";

    oss << std::endl;

    oss << " --- Average Cost --- " << std::endl
        << "Creature: " << std::setprecision(2) << mStatsWrapper->avgCreatureCost << std::endl
        << "Mana: " << std::setprecision(2) << mStatsWrapper->avgManaCost << "   "
        << "Spell: " << std::setprecision(2) << mStatsWrapper->avgSpellCost << std::endl;

    return oss.str();
}

void SimplePopup::Update(float dt) {
    JButton key = mEngine->ReadButton();
    CheckUserInput(key);
}

// drawing routines
void SimplePopup::drawCorner(string imageName, bool flipX, bool flipY, float x, float y) {
    WGE_LOG_TRACE("Drawing a Corner! ");
    JRenderer* r = JRenderer::GetInstance();
    JQuadPtr horizontalBarImage = WResourceManager::Instance()->RetrieveTempQuad(imageName, TEXTURE_SUB_5551);
    horizontalBarImage->SetHFlip(flipX);
    horizontalBarImage->SetVFlip(flipY);

    r->RenderQuad(horizontalBarImage.get(), x, y);
    WGE_LOG_TRACE("Done Drawing a Corner!");
}

void SimplePopup::drawHorzPole(string imageName, bool flipX = false, bool flipY = false, float x = 0, float y = 0,
                               float width = SCREEN_WIDTH_F) {
    WGE_LOG_TRACE("Drawing a horizontal border!");
    JRenderer* r = JRenderer::GetInstance();
    JQuadPtr horizontalBarImage = WResourceManager::Instance()->RetrieveTempQuad(imageName, TEXTURE_SUB_5551);
    if (horizontalBarImage != nullptr) {
        horizontalBarImage->SetHFlip(flipX);
        horizontalBarImage->SetVFlip(flipY);

        r->RenderQuad(horizontalBarImage.get(), x, y, 0, width);
    } else {
        WGE_LOG_ERROR("Error trying to render horizontal edge!");
    }
    WGE_LOG_TRACE("Done Drawing a horizontal border!");
}

void SimplePopup::drawVertPole(string imageName, bool flipX = false, bool flipY = false, float x = 0, float y = 0,
                               float height = SCREEN_HEIGHT_F) {
    WGE_LOG_TRACE("Drawing a Vertical border!");
    JRenderer* r = JRenderer::GetInstance();
    JQuadPtr verticalBarImage = WResourceManager::Instance()->RetrieveTempQuad(imageName, TEXTURE_SUB_5551);
    if (verticalBarImage != nullptr) {
        verticalBarImage->SetHFlip(flipX);
        verticalBarImage->SetVFlip(flipY);

        r->RenderQuad(verticalBarImage.get(), x, y, 0, 1.0f, height);
    } else {
        WGE_LOG_ERROR("Error trying to render vertical edge!");
    }
    WGE_LOG_TRACE("DONE Drawing a horizontal border!");
}

void SimplePopup::Close() {
    mClosed = true;
    mCount = 0;
}

SimplePopup::~SimplePopup(void) {
    mTextFont        = nullptr;
    mDeckInformation = nullptr;
}
