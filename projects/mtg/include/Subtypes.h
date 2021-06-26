#ifndef _SUBTYPES_H_
#define _SUBTYPES_H_

#include <string>
#include <map>
#include <vector>

class Subtypes {
public:
    // A list of commonly used types
    enum {
        TYPE_CREATURE = 1,
        TYPE_ENCHANTMENT = 2,
        TYPE_SORCERY = 3,
        TYPE_INSTANT = 4,
        TYPE_LAND = 5,
        TYPE_ARTIFACT = 6,
        TYPE_LEGENDARY = 7,
        TYPE_SNOW = 8,
        TYPE_BASIC = 9,
        TYPE_WORLD = 10,
        TYPE_EQUIPMENT = 11,
        TYPE_AURA = 12,
        TYPE_PLANESWALKER = 13,
        TYPE_TRIBAL = 14,
        TYPE_PLANE = 15,
        TYPE_SCHEME = 16,
        TYPE_VANGUARD = 17,
        LAST_TYPE = TYPE_VANGUARD,
    };

protected:
    std::map<std::string, int> values;
    std::vector<std::string> valuesById;
    std::vector<unsigned int> subtypesToType;

public:
    std::vector<std::string> subtypesCreature;
    Subtypes();
    int find(std::string subtype, bool forceAdd = true);
    std::string find(unsigned int id);
    bool isSubtypeOfType(unsigned int subtype, unsigned int type);
    bool isSuperType(unsigned int type);
    bool isType(unsigned int type);
    bool isSubType(unsigned int type);
    void sortSubTypes();
    int add(std::string value, unsigned int parentType);
    const std::vector<std::string>& getValuesById();
    const std::vector<std::string>& getCreatureValuesById();
    const std::map<std::string, int>& getValuesByMap();
};

#endif
