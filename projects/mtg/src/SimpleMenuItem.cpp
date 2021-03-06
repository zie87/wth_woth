#include "PrecompiledHeader.h"

#include "SimpleMenuItem.h"
#include "Translate.h"
#include "WResourceManager.h"

SimpleMenuItem::SimpleMenuItem(int id) : SimpleButton(id) {}

SimpleMenuItem::SimpleMenuItem(SimpleMenu* _parent, int id, int fontId, std::string text, float x, float y,
                               bool hasFocus, bool autoTranslate)
    : SimpleButton(_parent, id, fontId, text, x, y, hasFocus, autoTranslate) {
    parent = (SimpleMenu*)_parent;
    mDescription = "";
}

void SimpleMenuItem::Entering() {
    checkUserClick();
    setFocus(true);
    if (getParent() != nullptr) {
        auto* menu             = (SimpleMenu*)parent;
        menu->selectionTargetY = getY();
    }
}

/* Accessors */
string SimpleMenuItem::getDescription() const { return mDescription; }

void SimpleMenuItem::setDescription(const std::string& desc) { mDescription = desc; }

std::ostream& SimpleMenuItem::toString(std::ostream& out) const {
    return out << "SimpleMenuItem ::: mHasFocus : " << hasFocus() << " ; parent : " << getParent()
               << " ; mText : " << getText() << " ; mScale : " << getScale()
               << " ; mTargetScale : " << getTargetScale() << " ; mX,mY : " << getX() << "," << getY();
}
