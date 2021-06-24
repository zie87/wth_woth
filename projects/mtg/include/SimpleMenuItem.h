#ifndef _SIMPLEMENU_ITEM_H
#define _SIMPLEMENU_ITEM_H

#include <string>
#include <JLBFont.h>
#include <JGui.h>
#include "SimpleButton.h"
#include "SimpleMenu.h"

#include <ostream>

using std::string;

#define SCALE_SELECTED 1.2f
#define SCALE_NORMAL 1.0f

class SimpleMenuItem : public SimpleButton {
private:
    std::string mDescription;

public:
    SimpleMenuItem(int id);
    SimpleMenuItem(SimpleMenu* _parent, int id, int fontId, string text, float x, float y, bool hasFocus = false,
                   bool autoTranslate = false);

    virtual void Entering();
    virtual void setDescription(const string& desc);
    virtual std::string getDescription() const;
    virtual std::ostream& toString(std::ostream& out) const;
};

#endif
