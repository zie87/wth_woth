#ifndef _UTILS_H_
#define _UTILS_H_

#include <JGE.h>

#if defined(PSP)
    #include <pspkernel.h>
    #include <pspdisplay.h>
    #include <pspctrl.h>
    #include <pspiofilemgr.h>
    #include <pspdebug.h>
    #include <psputility.h>
    #include <pspgu.h>
    #include <psprtc.h>

    #include <psptypes.h>
    #include <malloc.h>

#endif

#include <math.h>
#include <stdio.h>
#include <string>

#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdlib.h>

#include "DebugRoutines.h"

#include <list>
#include <vector>
#include <string>

using std::string;

// std::string manipulation methods
std::string& trim(std::string& str);
std::string& ltrim(std::string& str);
std::string& rtrim(std::string& str);

inline std::string trim(const std::string& str) {
    std::string value(str);
    return trim(value);
}

std::string join(std::vector<std::string>& v, std::string delim = " ");

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems);
std::vector<std::string> split(
    const std::string& s,
    char delim);  // splits a std::string with "delim" and returns a std::vector of std::strings.

// A simple parsing function
// splits std::string s by finding the first occurence of start, and the first occurence of stop, and returning
// a std::vector of 3 std::strings. The first std::string is everything before the first occurence of start, the second
// std::string is everything between start and stop the third std::string is everything after stop. for example,
// parseBetween ("this is a function(foo) call", "function(", ")") will return: ["this is a ", "foo", " call"]; If an
// error occurs, returns an empty std::vector. if "stopRequired" is set to false, the function will return a
// std::vector of 3 std::strings even if "stop" is not found in the std::string.
std::vector<std::string>& parseBetween(const std::string& s, std::string start, std::string stop, bool stopRequired,
                                       std::vector<std::string>& elems);
std::vector<std::string> parseBetween(const std::string& s, std::string start, std::string stop,
                                      bool stopRequired = true);

std::string wordWrap(const std::string& s, float width, int fontId);

// basic hash function
unsigned long hash_djb2(const char* str);

// This class wraps random generation and the pre-loading/saving of randoms
// The idea is to make it instantiable to be able to handle randoms differently per class of group of classes.
// In particular, to be able to control the AI randoms independently of the other game randoms so that we can actually
// test AI
class RandomGenerator {
protected:
    std::list<int> loadedRandomValues;
    std::list<int> usedRandomValues;
    bool log;

public:
    RandomGenerator(bool doLog = false) : log(doLog){};
    void loadRandValues(std::string s);
    std::ostream& saveUsedRandValues(std::ostream& out) const;
    std::ostream& saveLoadedRandValues(std::ostream& out);
    int random();
    template <typename Iter>
    void random_shuffle(Iter first, Iter last) {
        ptrdiff_t i, n;
        n = (last - first);
        for (i = n - 1; i > 0; --i) swap(first[i], first[random() % (i + 1)]);
    };
};

int WRand(bool log = false);

#ifdef LINUX
void dumpStack();
#endif

/* RAM simple check functions header */

// *** DEFINES ***

#if defined(WIN32) || defined(LINUX)
    #define RAM_BLOCK (100 * 1024 * 1024)
#else
    #define RAM_BLOCK (1024 * 1024)
#endif

// *** FUNCTIONS DECLARATIONS ***

u32 ramAvailableLineareMax(void);
u32 ramAvailable(void);

/*
#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
*/

inline void ReplaceString(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

bool fileExists(const char* filename);
bool FileExists(const std::string& filename);
std::string buildFilePath(const std::vector<string>& folders, const std::string& filename);
std::string ensureFolder(const std::string& folderName);
/*
template <class T> istream& operator>>(istream& in, T& p)
{
    std::string s;

    while(std::getline(in, s))
    {
        if(!p.parseLine(s))
        {
            break;
        }
    }

    return in;
}
*/
#endif
