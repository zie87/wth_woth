#ifndef PRECOMPILEDHEADER_H
#define PRECOMPILEDHEADER_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "config.h"
#include "DebugRoutines.h"

#include <assert.h>

#include "JGE.h"
#include "JFileSystem.h"

#include "GameOptions.h"

#include <wge/memory.hpp>

#if defined(QT_CONFIG) || defined(SDL_CONFIG)
    #define TOUCH_ENABLED
#endif

#endif  // PRECOMPILEDHEADER_H
