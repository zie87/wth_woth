#ifndef _TRANSLATEKEYS_H_
#define _TRANSLATEKEYS_H_

#include <utility>
#include <string>
#include "JGE.h"

typedef std::pair<std::string, JQuad*> KeyRep;
const KeyRep& translateKey(LocalKeySym);
const KeyRep& translateKey(JButton);

#endif
