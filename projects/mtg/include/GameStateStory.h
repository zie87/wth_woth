#ifndef _GAME_STATE_STORY_H_
#define _GAME_STATE_STORY_H_

#include "GameState.h"
#include <JGui.h>

#include <vector>
#include <string>

class StoryFlow;
class SimpleMenu;

class GameStateStory : public GameState, public JGuiListener {
private:
    StoryFlow* flow;
    SimpleMenu* menu;
    std::vector<std::string> stories;
    void loadStoriesMenu(const char* root);

public:
    GameStateStory(GameApp* parent);
    ~GameStateStory();
    void Start();
    void End();
    void Update(float dt);
    void Render();
    void ButtonPressed(int controllerId, int controlId);
};

#endif
