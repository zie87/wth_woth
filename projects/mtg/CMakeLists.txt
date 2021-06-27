
add_executable(wth_woth)

target_sources(wth_woth
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/InteractiveButton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AbilityParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ActionElement.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ActionLayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ActionStack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AIHints.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AIMomirPlayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AIPlayer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AIPlayerBaka.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AIStats.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AllAbilities.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CardGui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CardDescriptor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CardDisplay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CardEffect.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CardPrimitive.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CardSelector.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/CardSelectorSingleton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Counters.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Credits.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Damage.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DamagerDamaged.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DeckDataWrapper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DeckEditorMenu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DeckMenu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DeckMenuItem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DeckMetaData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DeckStats.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DuelLayers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Effects.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ExtraCost.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameApp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameLauncher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameObserver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameOptions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameState.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameStateAwards.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameStateDeckViewer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameStateDuel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/DeckManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameStateMenu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameStateOptions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameStateShop.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameStateStory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GameStateTransitions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiAvatars.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiBackground.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiCardsController.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiCombat.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiFrame.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiHand.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiLayers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiMana.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiPhaseBar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiPlay.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/GuiStatic.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/IconButton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ManaCost.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ManaCostHybrid.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MenuItem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ModRules.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGAbility.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGCardInstance.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGCard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGDeck.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGDefinitions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGGamePhase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGGameZones.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGPack.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/MTGRules.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Navigator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ObjectAnalytics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/OptionItem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PhaseRing.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Player.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PlayerData.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PlayGuiObjectController.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PlayGuiObject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PlayRestrictions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Pos.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PrecompiledHeader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PriceList.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ReplacementEffects.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Rules.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SimpleMenu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SimpleButton.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SimpleMenuItem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SimplePad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/SimplePopup.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StoryFlow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/StyleManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Subtypes.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TargetChooser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TargetsList.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TextScroller.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ThisDescriptor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Token.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Translate.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TranslateKeys.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Trash.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WEvent.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WResourceManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WCachedResource.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WDataSrc.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WGui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WFilter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/WFont.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AIPlayerBakaB.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/TestSuiteAI.cpp
    )

if(NOT DEFINED WIN32)
    target_compile_options(wth_woth PRIVATE "-DUSE_PRECOMPILED_HEADERS=1")
    target_compile_options(wth_woth PRIVATE "$<$<CONFIG:DEBUG>:-ggdb3>")
endif()

target_compile_options(wth_woth PRIVATE "$<$<CONFIG:DEBUG>:-D_DEBUG>")
target_compile_options(wth_woth PRIVATE "$<$<CONFIG:DEBUG>:-DDEBUG>")
target_compile_options(wth_woth PRIVATE "$<$<CONFIG:DEBUG>:-DDEBUG_CACHE>")

target_compile_options(wth_woth PRIVATE "$<$<CONFIG:DEBUG>:-DTESTSUITE>")
target_compile_options(wth_woth PRIVATE "$<$<CONFIG:DEBUG>:-DAI_CHANGE_TESTING>")


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/include/Version.hpp.in 
                ${PROJECT_BINARY_DIR}/include/generated/Version.hpp)

target_include_directories(wth_woth 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/include/generated/
)

target_link_libraries(wth_woth
    PUBLIC
        WotH::wge
        WotH::jge
    PRIVATE
        woth_options
)


if(DEFINED PSP)
create_pbp_file(TARGET wth_woth
                TITLE "Wagic, the Homebrew?!" 
                ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/icon.png
                BACKGROUND_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pic1.png
                PREVIEW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pic0.png
)
endif()
