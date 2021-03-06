#include "PrecompiledHeader.h"

#include "Translate.h"
#include "WResourceManager.h"
#include "TranslateKeys.h"
#ifdef SDL_CONFIG
#include <SDL.h>
#endif
using std::map;
using std::string;

static std::map<const LocalKeySym, KeyRep> fattable;
static std::map<const JButton, KeyRep> slimtable;

#if defined(LINUX) || defined(SDL_CONFIG)
const KeyRep& translateKey(LocalKeySym key) {
    {
        std::map<const LocalKeySym, KeyRep>::iterator res;
        if ((res = fattable.find(key)) != fattable.end()) return res->second;
    }

    char* str = nullptr;

#if defined(SDL_CONFIG)
    str = (char*)SDL_GetKeyName(key);
#endif
    if (!str) {
        str = NEW char[11];
        sprintf(str, "%lu",
                (long unsigned int)key);  // TODO: Wagic is not supposed to know that a key actually is an unsingned
                                          // long, so this part should probably be platform specific (move to JGE ?)
    }
    const KeyRep k = std::make_pair(str, static_cast<JQuad*>(nullptr));
    fattable[key]  = k;
    return fattable[key];
}
#elif defined(WIN32)
const KeyRep& translateKey(LocalKeySym key) {
    {
        auto res = fattable.find(key);
        if (res != fattable.end()) return (res->second);
    }
    unsigned int sc = MapVirtualKey(key, 0);

    switch (key) {
    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:  // arrow keys
    case VK_PRIOR:
    case VK_NEXT:  // page up and page down
    case VK_END:
    case VK_HOME:
    case VK_INSERT:
    case VK_DELETE:
    case VK_DIVIDE:  // numpad slash
    case VK_NUMLOCK: {
        sc |= 0x100;  // set extended bit
        break;
    }
    }

    char buf[256];
    memset(buf, 0, 256);

    string s;
    // Convert to ANSI string
    if (GetKeyNameTextA(sc << 16, buf, 256) > 0) s = buf;

    KeyRep k;
    if (0 == s.length()) {
        char* str = NEW char[11];
        sprintf(str, "%d", key);
        k = std::make_pair(str, static_cast<JQuad*>(NULL));
    } else
        k = std::make_pair(s, static_cast<JQuad*>(NULL));
    fattable[key] = k;
    return fattable[key];
}
#else  // PSP

const KeyRep& translateKey(LocalKeySym key) {
    std::map<const LocalKeySym, KeyRep>::iterator res;
    if ((res = fattable.find(key)) == fattable.end()) {
        if (fattable.end() == fattable.find(PSP_CTRL_SELECT)) {
            fattable[PSP_CTRL_SELECT]   = make_pair(_("Select"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_START]    = make_pair(_("Start"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_UP]       = make_pair(_("Up"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_RIGHT]    = make_pair(_("Right"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_DOWN]     = make_pair(_("Down"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_LEFT]     = make_pair(_("Left"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_LTRIGGER] = make_pair(_("Left trigger"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_RTRIGGER] = make_pair(_("Right trigger"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_TRIANGLE] = make_pair(_("Triangle"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_CIRCLE]   = make_pair(_("Circle"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_CROSS]    = make_pair(_("Cross"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_SQUARE]   = make_pair(_("Square"), static_cast<JQuad*>(NULL));
            fattable[PSP_CTRL_HOLD]     = make_pair(_("Hold"), static_cast<JQuad*>(NULL));
        } else {
            char* str = NEW char[11];
            sprintf(str, "%d", (int)key);
            fattable[key] = std::make_pair(str, static_cast<JQuad*>(static_cast<JQuad*>(NULL)));
        }
        res = fattable.find(key);
    }
    KeyRep& k = res->second;
    switch (key) {
    case PSP_CTRL_SELECT:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)2 * 32, 32, 64, 32, "PSP_CTRL_SELECT", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_START:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)0 * 32, 32, 64, 32, "PSP_CTRL_START", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_UP:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)0 * 32, 0, 32, 32, "PSP_CTRL_UP", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_RIGHT:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)3 * 32, 0, 32, 32, "PSP_CTRL_RIGHT", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_DOWN:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)1 * 32, 0, 32, 32, "PSP_CTRL_DOWN", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_LEFT:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)2 * 32, 0, 32, 32, "PSP_CTRL_LEFT", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_LTRIGGER:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)6 * 32, 32, 64, 32, "PSP_CTRL_LTRIGGER", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_RTRIGGER:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)8 * 32, 32, 64, 32, "PSP_CTRL_RTRIGGER", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_TRIANGLE:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)5 * 32, 0, 32, 32, "PSP_CTRL_TRIANGLE", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_CIRCLE:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)4 * 32, 0, 32, 32, "PSP_CTRL_CIRCLE", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_CROSS:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)7 * 32, 0, 32, 32, "PSP_CTRL_CROSS", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_SQUARE:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)6 * 32, 0, 32, 32, "PSP_CTRL_SQUARE", RETRIEVE_NORMAL)
                       .get();
        break;
    case PSP_CTRL_HOLD:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)4 * 32, 0, 32, 32, "PSP_CTRL_HOLD", RETRIEVE_NORMAL)
                       .get();
        break;
    default: /* Unknown key : no icon */;
    }
    return k;
}
#endif

const KeyRep& translateKey(JButton key) {
    auto res = slimtable.find(key);
    if (res == slimtable.end()) {
        if (slimtable.end() == slimtable.find(JGE_BTN_CTRL)) {
            slimtable[JGE_BTN_NONE]       = make_pair(_("Delete this binding"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_CTRL]       = make_pair(_("Select"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_MENU]       = make_pair(_("Start"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_UP]         = make_pair(_("Up"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_RIGHT]      = make_pair(_("Right"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_DOWN]       = make_pair(_("Down"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_LEFT]       = make_pair(_("Left"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_PREV]       = make_pair(_("Left trigger"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_NEXT]       = make_pair(_("Right trigger"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_CANCEL]     = make_pair(_("Triangle"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_OK]         = make_pair(_("Circle"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_SEC]        = make_pair(_("Cross"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_PRI]        = make_pair(_("Square"), static_cast<JQuad*>(nullptr));
            slimtable[JGE_BTN_FULLSCREEN] = make_pair(_("Fullscreen"), static_cast<JQuad*>(nullptr));
        } else {
            char* str = NEW char[11];
            sprintf(str, "%d", key);
            slimtable[key] = std::make_pair(str, static_cast<JQuad*>(nullptr));
        }
        res = slimtable.find(key);
    }
    KeyRep& k = res->second;
    switch (key) {
    case JGE_BTN_CTRL:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)2 * 32, 32, 64, 32, "PSP_CTRL_SELECT", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_MENU:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)0 * 32, 32, 64, 32, "PSP_CTRL_START", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_UP:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)0 * 32, 0, 32, 32, "PSP_CTRL_UP", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_RIGHT:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)3 * 32, 0, 32, 32, "PSP_CTRL_RIGHT", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_DOWN:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)1 * 32, 0, 32, 32, "PSP_CTRL_DOWN", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_LEFT:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)2 * 32, 0, 32, 32, "PSP_CTRL_LEFT", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_PREV:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)6 * 32, 32, 64, 32, "PSP_CTRL_LTRIGGER", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_NEXT:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)8 * 32, 32, 64, 32, "PSP_CTRL_RTRIGGER", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_CANCEL:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)5 * 32, 0, 32, 32, "PSP_CTRL_TRIANGLE", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_OK:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)4 * 32, 0, 32, 32, "PSP_CTRL_CIRCLE", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_SEC:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)7 * 32, 0, 32, 32, "PSP_CTRL_CROSS", RETRIEVE_NORMAL)
                       .get();
        break;
    case JGE_BTN_PRI:
        k.second = WResourceManager::Instance()
                       ->RetrieveQuad("iconspsp.png", (float)6 * 32, 0, 32, 32, "PSP_CTRL_SQUARE", RETRIEVE_NORMAL)
                       .get();
        break;
    default: /* Unknown key : no icon */;
    }
    return k;
}
