#ifndef _GUI_LAYERS_H_
#define _GUI_LAYERS_H_

#define DIR_DOWN 1
#define DIR_UP 2
#define DIR_LEFT 3
#define DIR_RIGHT 4

#include <JGui.h>
#include "WEvent.h"

class GameObserver;
class Player;

class GuiLayer {
protected:
    JButton mActionButton;
    GameObserver* observer;

public:
    int mCurr;
    vector<JGuiObject*> mObjects;
    vector<JGuiObject*> manaObjects;
    void Add(JGuiObject* object);
    int Remove(JGuiObject* object);
    int modal;
    bool hasFocus;
    virtual void resetObjects();
    int getMaxId();
    GuiLayer(GameObserver* observer);
    virtual ~GuiLayer();
    virtual void Update(float dt);
    virtual bool CheckUserInput(JButton key) { return false; }

    int getIndexOf(JGuiObject* object);
    JGuiObject* getByIndex(int index);
    virtual void Render();
    int empty() {
        if (mObjects.size()) return 0;
        return 1;
    }

    virtual int receiveEventPlus(WEvent* e) { return 0; }

    virtual int receiveEventMinus(WEvent* e) { return 0; }
};

#endif
