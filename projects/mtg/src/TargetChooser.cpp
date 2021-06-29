#include "PrecompiledHeader.h"

#include "TargetChooser.h"
#include "CardDescriptor.h"
#include "MTGGameZones.h"
#include "GameObserver.h"
#include "Subtypes.h"
#include "Counters.h"
#include "WEvent.h"
#include "AllAbilities.h"

#include <wge/log.hpp>

TargetChooser* TargetChooserFactory::createTargetChooser(string s, MTGCardInstance* card, MTGAbility* ability) {
    if (!s.size()) return nullptr;

    int zones[10];
    int nbzones = 0;
    size_t found;
    bool other = false;

    found = s.find("blockable");
    if (found != string::npos) {
        int maxtargets = 1;
        return NEW BlockableChooser(observer, card, maxtargets);
    }

    found = s.find("mytgt");
    if (found == 0) {
        MTGCardInstance* target = card->target;
        if (ability) target = (MTGCardInstance*)(ability->target);
        return NEW CardTargetChooser(observer, target, card);
    };

    found = s.find("targetedplayer");
    if (found == 0) {
        Player* pTarget = card->playerTarget;
        if (ability) pTarget = dynamic_cast<Player*>(ability->target);
        if (pTarget) return NEW PlayerTargetChooser(observer, card, 1, pTarget);
    };

    found = s.find("opponent");
    if (found == 0) {
        int maxtargets = 1;
        Player* opponent = card->controller()->opponent();
        return NEW PlayerTargetChooser(observer, card, maxtargets, opponent);
    };

    found = s.find("controller");
    if (found == 0) {
        int maxtargets = 1;
        Player* controller = card->controller();
        return NEW PlayerTargetChooser(observer, card, maxtargets, controller);
    };

    found = s.find("other ");
    if (found != string::npos) {
        other = true;
        s = s.erase(found, found + 6 - found);
    }

    found = s.find("trigger");
    if (found == 0) {
        if (s.length() > 7 && s.find("[") == 7) {
            string s1 = s.substr(7, s.find("]"));
            if (s1.find("to") != string::npos) return NEW TriggerTargetChooser(observer, WEvent::TARGET_TO);
            if (s1.find("from") != string::npos) return NEW TriggerTargetChooser(observer, WEvent::TARGET_FROM);
        }
        return NEW TriggerTargetChooser(observer, 1);
    }

    found = s.find("player");
    if (found != string::npos) {
        int maxtargets = 1;
        size_t several = s.find("<anyamount>");
        if (several != string::npos) maxtargets = TargetChooser::UNLITMITED_TARGETS;
        found = s.find("creature");
        if (found != string::npos)
            return NEW DamageableTargetChooser(observer, card, maxtargets,
                                               other);               // Any Damageable target (player, creature)
        return NEW PlayerTargetChooser(observer, card, maxtargets);  // Any player
    }

    found = s.find("mycurses");
    if (found != string::npos) {
        int maxtargets = TargetChooser::UNLITMITED_TARGETS;
        return NEW myCursesChooser(observer, card, maxtargets);
    }

    found = s.find("proliferation");
    if (found != string::npos) {
        int maxtargets = TargetChooser::UNLITMITED_TARGETS;
        return NEW ProliferateChooser(observer, card, maxtargets);
    }

    string s1;
    found = s.find("|");
    if (found != string::npos) {
        string s2;
        s1 = s.substr(0, found);
        s2 = s.substr(found + 1);
        while (s2.size()) {
            found = s2.find(",");
            string zoneName;
            if (found != string::npos) {
                zoneName = s2.substr(0, found);
                s2 = s2.substr(found + 1);
            } else {
                zoneName = s2;
                s2 = "";
            }
            zones[nbzones] = MTGGameZone::MY_BATTLEFIELD;

            if (zoneName == "*") {
                zones[nbzones++] = MTGGameZone::ALL_ZONES;
            } else if (zoneName == "graveyard") {
                zones[nbzones++] = MTGGameZone::MY_GRAVEYARD;
                zones[nbzones++] = MTGGameZone::OPPONENT_GRAVEYARD;
            } else if (zoneName == "battlefield" || zoneName == "inplay") {
                zones[nbzones++] = MTGGameZone::MY_BATTLEFIELD;
                zones[nbzones++] = MTGGameZone::OPPONENT_BATTLEFIELD;
            } else if (zoneName == "hand") {
                zones[nbzones++] = MTGGameZone::MY_HAND;
                zones[nbzones++] = MTGGameZone::OPPONENT_HAND;
            } else if (zoneName == "library") {
                zones[nbzones++] = MTGGameZone::MY_LIBRARY;
                zones[nbzones++] = MTGGameZone::OPPONENT_LIBRARY;
            } else if (zoneName == "nonbattlezone") {
                zones[nbzones++] = MTGGameZone::MY_GRAVEYARD;
                zones[nbzones++] = MTGGameZone::OPPONENT_GRAVEYARD;
                zones[nbzones++] = MTGGameZone::MY_LIBRARY;
                zones[nbzones++] = MTGGameZone::OPPONENT_LIBRARY;
                zones[nbzones++] = MTGGameZone::MY_HAND;
                zones[nbzones++] = MTGGameZone::OPPONENT_HAND;
                zones[nbzones++] = MTGGameZone::MY_EXILE;
                zones[nbzones++] = MTGGameZone::OPPONENT_EXILE;
            } else if (zoneName == "stack") {
                zones[nbzones++] = MTGGameZone::MY_STACK;
                zones[nbzones++] = MTGGameZone::OPPONENT_STACK;
            } else {
                int zone = MTGGameZone::zoneStringToId(zoneName);
                if (zone) zones[nbzones++] = zone;
            }
        }
    } else {
        s1 = s;
        nbzones = 2;
        zones[0] = MTGGameZone::MY_BATTLEFIELD;
        zones[1] = MTGGameZone::OPPONENT_BATTLEFIELD;
    }

    TargetChooser* tc  = nullptr;
    int maxtargets = 1;
    bool targetMin = false;
    CardDescriptor* cd = nullptr;
    // max targets allowed
    size_t limit = s1.find('<');
    if (limit != string::npos) {
        size_t end = s1.find(">", limit);
        string howmany;
        if (end != string::npos) {
            howmany = s1.substr(limit + 1, end - limit - 1);
            size_t uptoamount = howmany.find("upto:");

            if (uptoamount != string::npos) {
                howmany = s1.substr(uptoamount + 6, end - uptoamount - 6);
            } else {
                targetMin = true;  // if upto: is not found, then we need to have a minimum of the amount....
            }

            if (howmany.find("anyamount") != string::npos) {
                maxtargets = TargetChooser::UNLITMITED_TARGETS;
                targetMin = false;
            } else {
                auto* howmuch = NEW WParsedInt(howmany, nullptr, card);
                maxtargets = howmuch->getValue();
                delete howmuch;
            }

            s1 = s1.substr(end + 1);
        }
    }

    if (s1.find("children") != string::npos || s1.find("parents") != string::npos) {
        TargetChooser* deeperTc = nullptr;
        if (s1.find("[") != string::npos) {
            AbilityFactory af(observer);
            vector<string> deepTc = parseBetween(s1, "[", "]");
            deeperTc = createTargetChooser(deepTc[1], card);
        }
        return NEW ParentChildChooser(observer, card, maxtargets, deeperTc,
                                      s1.find("parents") != string::npos ? 2 : 1);
    }

    while (s1.size()) {
        found = s1.find(",");
        string typeName;
        if (found != string::npos) {
            typeName = s1.substr(0, found);
            s1 = s1.substr(found + 1);
        } else {
            typeName = s1;
            s1 = "";
        }

        // Advanced cards caracteristics ?
        found = typeName.find("[");
        if (found != string::npos) {
            int nbminuses = 0;
            int end = typeName.find("]");
            string attributes = typeName.substr(found + 1, end - found - 1);
            cd = NEW CardDescriptor();
            while (attributes.size()) {
                size_t found2 = attributes.find(";");
                size_t foundAnd = attributes.find("&");
                string attribute;
                if (found2 != string::npos) {
                    cd->mode = CD_OR;
                    attribute = attributes.substr(0, found2);
                    attributes = attributes.substr(found2 + 1);
                } else if (foundAnd != string::npos) {
                    cd->mode = CD_AND;
                    attribute = attributes.substr(0, foundAnd);
                    attributes = attributes.substr(foundAnd + 1);
                } else {
                    attribute = attributes;
                    attributes = "";
                }
                int minus = 0;
                if (attribute[0] == '-') {
                    minus = 1;
                    nbminuses++;
                    attribute = attribute.substr(1);
                }
                int comparisonMode = COMPARISON_NONE;
                int comparisonCriterion = 0;
                if (attribute.size() > 1) {
                    size_t operatorPosition = attribute.find("=", 1);
                    if (operatorPosition != string::npos) {
                        string numberCD =
                            attribute.substr(operatorPosition + 1, attribute.size() - operatorPosition - 1);
                        auto* val           = NEW WParsedInt(numberCD, nullptr, card);
                        comparisonCriterion = val->getValue();
                        delete val;
                        switch (attribute[operatorPosition - 1]) {
                        case '<':
                            if (minus) {
                                comparisonMode = COMPARISON_GREATER;
                            } else {
                                comparisonMode = COMPARISON_AT_MOST;
                            }
                            operatorPosition--;
                            break;
                        case '>':
                            if (minus) {
                                comparisonMode = COMPARISON_LESS;
                            } else {
                                comparisonMode = COMPARISON_AT_LEAST;
                            }
                            operatorPosition--;
                            break;
                        default:
                            if (minus) {
                                comparisonMode = COMPARISON_UNEQUAL;
                            } else {
                                comparisonMode = COMPARISON_EQUAL;
                            }
                        }
                        attribute = attribute.substr(0, operatorPosition);
                    }
                }

                // Attacker
                if (attribute.find("attacking") != string::npos) {
                    if (minus) {
                        cd->attacker = -1;
                    } else {
                        cd->attacker = 1;
                    }
                }
                // Blocker
                else if (attribute.find("blocking") != string::npos) {
                    if (minus) {
                        cd->defenser = &MTGCardInstance::NoCard;
                    } else {
                        cd->defenser = &MTGCardInstance::AnyCard;
                    }
                }
                // Tapped, untapped
                else if (attribute.find("tapped") != string::npos) {
                    if (minus) {
                        cd->unsecureSetTapped(-1);
                    } else {
                        cd->unsecureSetTapped(1);
                    }
                    // Token
                } else if (attribute.find("token") != string::npos) {
                    if (minus) {
                        cd->isToken = -1;
                    } else {
                        cd->isToken = 1;
                    }
                    // put in its zone this turn
                } else if (attribute.find("fresh") != string::npos) {
                    if (minus) {
                        cd->unsecuresetfresh(-1);
                    } else {
                        cd->unsecuresetfresh(1);
                    }
                }
                // creature is a level up creature
                else if (attribute.find("leveler") != string::npos) {
                    if (minus) {
                        cd->isLeveler = -1;
                    } else {
                        cd->isLeveler = 1;
                    }
                }
                // creature is enchanted
                else if (attribute.find("enchanted") != string::npos) {
                    if (minus) {
                        cd->CDenchanted = -1;
                    } else {
                        cd->CDenchanted = 1;
                    }
                }
                // creature was damaged
                else if (attribute.find("damaged") != string::npos) {
                    if (minus) {
                        cd->CDdamaged = -1;
                    } else {
                        cd->CDdamaged = 1;
                    }
                }
                // creature dealt damage to opponent
                else if (attribute.find("opponentdamager") != string::npos) {
                    if (minus) {
                        cd->CDopponentDamaged = -1;
                    } else {
                        cd->CDopponentDamaged = 1;
                    }
                }
                // creature dealt damage to controller
                else if (attribute.find("controllerdamager") != string::npos) {
                    if (minus) {
                        cd->CDcontrollerDamaged = -1;
                    } else {
                        cd->CDcontrollerDamaged = 1;
                    }
                } else if (attribute.find("multicolor") != string::npos) {
                    // card is multicolored?
                    if (minus) {
                        cd->setisMultiColored(-1);
                    } else {
                        cd->setisMultiColored(1);
                    }

                } else if (attribute.find("power") != string::npos) {
                    // Power restrictions
                    cd->setPower(comparisonCriterion);
                    cd->powerComparisonMode = comparisonMode;
                    // Toughness restrictions
                } else if (attribute.find("toughness") != string::npos) {
                    cd->setToughness(comparisonCriterion);
                    cd->toughnessComparisonMode = comparisonMode;
                    // Manacost restrictions
                } else if (attribute.find("manacost") != string::npos) {
                    cd->convertedManacost = comparisonCriterion;
                    cd->manacostComparisonMode = comparisonMode;
                    // Counter Restrictions
                } else if (attribute.find("share!") != string::npos) {
                    size_t start = attribute.find("share!");
                    size_t end = attribute.rfind("!");
                    string CDtype = attribute.substr(start + 6, end - start);

                    if (card && card->isSpell() && card->backupTargets.size()) {
                        if (auto* targetSpell = dynamic_cast<Spell*>(card->backupTargets[0])) {
                            // spells always store their targets in :targets[]
                            // however they are all erased as the spell resolves
                            // added a array to store these backups incase theyre needed
                            // again for effects such as these.
                            card->target = targetSpell->source;
                        }
                    }
                    if (CDtype.find("name") != string::npos) {
                        if (card->target)
                            cd->compareName = card->target->name;
                        else
                            cd->compareName = card->name;

                        cd->nameComparisonMode = COMPARISON_EQUAL;
                    } else if (CDtype.find("color") != string::npos) {
                        if (card->target)
                            cd->colors = card->target->colors;
                        else
                            cd->colors = card->colors;

                        cd->mode = CD_OR;
                    } else if (CDtype.find("types") != string::npos) {
                        if (card && card->target) {
                            cd->types = card->target->types;
                        } else {
                            cd->types = card->types;
                        }
                        // remove main types because we only care about subtypes here.
                        cd->removeType("artifact");
                        cd->removeType("land");
                        cd->removeType("enchantment");
                        cd->removeType("instant");
                        cd->removeType("sorcery");
                        cd->removeType("legendary");
                        cd->removeType("creature");
                        cd->removeType("planeswalker");
                        cd->removeType("tribal");
                        cd->mode = CD_OR;
                    }
                } else if (attribute.find("counter") != string::npos) {
                    if (attribute.find("{any}") != string::npos) {
                        cd->anyCounter = 1;
                    } else {
                        size_t start = attribute.find("{");
                        size_t end = attribute.find("}");
                        string counterString = attribute.substr(start + 1, end - start - 1);
                        AbilityFactory abf(observer);
                        Counter* counter = abf.parseCounter(counterString, card);
                        if (counter) {
                            cd->counterName = counter->name;
                            cd->counterNB = counter->nb;
                            cd->counterPower = counter->power;
                            cd->counterToughness = counter->toughness;
                            delete (counter);
                        }
                        if (minus) {
                            cd->counterComparisonMode = COMPARISON_LESS;
                        } else {
                            cd->counterComparisonMode = COMPARISON_AT_LEAST;
                        }
                    }
                } else {
                    int attributefound = 0;
                    // Colors - remove Artifact and Land from the loop
                    for (int cid = 1; cid < Constants::NB_Colors - 1; cid++) {
                        if (attribute.find(Constants::MTGColorStrings[cid]) != string::npos) {
                            attributefound = 1;
                            if (minus)
                                cd->SetExclusionColor(cid);
                            else
                                cd->setColor(cid);
                        }
                    }

                    if (attribute.find("chosencolor") != string::npos) {
                        attributefound = 1;
                        if (minus)
                            cd->SetExclusionColor(card->chooseacolor);
                        else
                            cd->setColor(card->chooseacolor);
                    }

                    if (attribute.find("chosentype") != string::npos) {
                        attributefound = 1;
                        if (minus) {
                            cd->setNegativeSubtype(card->chooseasubtype);
                        } else {
                            cd->setSubtype(card->chooseasubtype);
                        }
                    }

                    if (!attributefound) {
                        // Abilities
                        for (int j = 0; j < Constants::NB_BASIC_ABILITIES; j++) {
                            if (attribute.find(Constants::MTGBasicAbilities[j]) != string::npos) {
                                attributefound = 1;
                                if (minus)
                                    cd->mAbilityExclusions.set(j);
                                else
                                    cd->basicAbilities.set(j);
                            }
                        }
                    }

                    if (!attributefound) {
                        // Subtypes
                        if (minus) {
                            cd->setNegativeSubtype(attribute);
                        } else {
                            cd->setSubtype(attribute);
                        }
                    }
                }
            }
            if (nbminuses) cd->mode = CD_AND;
            typeName = typeName.substr(0, found);
        }
        if (cd) {
            if (!tc) {
                if (typeName != "*") cd->setSubtype(typeName);

                tc = NEW DescriptorTargetChooser(observer, cd, zones, nbzones, card, maxtargets, other, targetMin);
            } else {
                delete (cd);
                return nullptr;
            }
        } else {
            if (!tc) {
                if (typeName == "*") {
                    return NEW TargetZoneChooser(observer, zones, nbzones, card, maxtargets, other, targetMin);
                } else if (typeName == "this") {
                    return NEW CardTargetChooser(observer, card, card, zones, nbzones);
                } else if (typeName == "mystored") {
                    return NEW CardTargetChooser(observer, card->storedSourceCard, card, zones, nbzones);
                } else {
                    tc = NEW TypeTargetChooser(observer, typeName.c_str(), zones, nbzones, card, maxtargets, other,
                                               targetMin);
                }
            } else {
                ((TypeTargetChooser*)tc)->addType(typeName.c_str());
                tc->maxtargets = maxtargets;
            }
        }
    }
    return tc;
}

TargetChooser* TargetChooserFactory::createTargetChooser(MTGCardInstance* card) {
    if (!card) return nullptr;
    int id = card->getId();
    string s = card->spellTargetType;
    if (card->alias) {
        id = card->alias;
        // TODO load target as well... ?
    }
    TargetChooser* tc = createTargetChooser(s, card);
    if (tc) return tc;
    // Any target than cannot be defined automatically is determined by its id
    switch (id) {
    // Spell
    case 1224:  // Spell blast
    {
#if defined(WIN32) || defined(LINUX)
        WGE_LOG_TRACE("Counter Spell !");
#endif
        return NEW SpellTargetChooser(observer, card);
    }
        // Spell Or Permanent
    case 1282:  // ChaosLace
    case 1152:  // DeathLace
    case 1358:  // PureLace
    case 1227:  // ThoughLace
    case 1257:  // Lifelace
    {
        return NEW SpellOrPermanentTargetChooser(observer, card);
    }
        // Red Spell or Permanent
    case 1191:  // Blue Elemental Blast
    {
        return NEW SpellOrPermanentTargetChooser(observer, card, Constants::MTG_COLOR_RED);
    }
        // Blue Spell or Permanent
    case 1312:  // Red Elemental Blast
    {
        return NEW SpellOrPermanentTargetChooser(observer, card, Constants::MTG_COLOR_BLUE);
    }
        // Damage History
    case 1344:  // Eye for an Eye
    {
        return NEW DamageTargetChooser(observer, card, -1, 1, RESOLVED_OK);
    }
    default: {
        return nullptr;
    }
    }
}

TargetChooser::TargetChooser(GameObserver* observer, MTGCardInstance* card, int _maxtargets, bool _other,
                             bool _targetMin)
    : TargetsList(), observer(observer) {
    forceTargetListReady = 0;
    source = card;
    targetter = card;
    maxtargets = _maxtargets;
    other = _other;
    targetMin = _targetMin;
    done = false;
    attemptsToFill = 0;
    if (source)
        Owner = source->controller();
    else
        Owner = nullptr;
}

// Default targetter : every card can be targetted, unless it is protected from the targetter card
// For spells that do not "target" a specific card, set targetter to NULL
bool TargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (!target) return false;
    if (auto* card = dynamic_cast<MTGCardInstance*>(target)) {
        if (other) {
            MTGCardInstance* tempcard = card;
            while (tempcard) {
                if (tempcard == source) return false;
                tempcard = tempcard->previous;
            }
        }

        if (source &&
            ((source->hasSubtype(Subtypes::TYPE_AURA) || source->hasSubtype(Subtypes::TYPE_EQUIPMENT)) &&
             source->target && source->target == card && source->target->isPhased && targetter->target == card)) {
            return true;
        }
        // this is kinda cheating but by default we let auras and equipments always continue to target a phased
        // creature.
        else if (card && card->isPhased) {
            return false;
        }

        if (source && targetter && card->isInPlay(observer) && !withoutProtections) {
            if (card->has(Constants::SHROUD)) return false;
            if (card->protectedAgainst(targetter)) return false;
            if (card->CantBeTargetby(targetter)) return false;
            if ((targetter->controller() != card->controller()) && card->has(Constants::OPPONENTSHROUD)) return false;
        }
        return true;
    } else if (dynamic_cast<Interruptible*>(target))
        return true;

    return false;
}

int TargetChooser::addTarget(Targetable* target) {
    if (canTarget(target)) {
        TargetsList::addTarget(target);
    }

    return targetsReadyCheck();
}

int TargetChooser::ForceTargetListReady() {
    int state = targetsReadyCheck();
    if (state == TARGET_OK) {
        forceTargetListReady = 1;
    }
    return forceTargetListReady;
}

int TargetChooser::targetsReadyCheck() {
    if (!targets.size()) {
        return TARGET_NOK;
    }
    if (full()) {
        return TARGET_OK_FULL;
    }
    if (!ready()) {
        return TARGET_OK_NOT_READY;
    }
    return TARGET_OK;
}

int TargetChooser::targetListSet() {
    int state = targetsReadyCheck();
    if (state == TARGET_OK_FULL || forceTargetListReady) {
        return 1;
    }
    return 0;
}

bool TargetChooser::validTargetsExist(int maxTargets) {
    for (int i = 0; i < 2; ++i) {
        int maxAmount = 0;
        Player* p = observer->players[i];
        if (canTarget(p)) return true;
        MTGGameZone* zones[] = {p->game->inPlay,  p->game->graveyard, p->game->hand,
                                p->game->library, p->game->exile,     p->game->stack};
        for (int k = 0; k < 6; k++) {
            MTGGameZone* z = zones[k];
            if (targetsZone(z)) {
                for (int j = 0; j < z->nb_cards; j++) {
                    if (canTarget(z->cards[j])) maxAmount++;
                }
            }
        }
        if (maxAmount >= maxTargets) return true;
    }
    return false;
}

int TargetChooser::countValidTargets() {
    int result = 0;

    // Some TargetChooser objects are created at a point where no observer is available
    // see  ManaCost::parseManaCost which sets observer to NULL in some cases (namely: when the cards database is
    // loaded at game start) This is a workaround for this situation
    if (!observer && source) observer = source->getObserver();

    for (int i = 0; i < 2; ++i) {
        assert(observer);
        Player* p = observer->players[i];
        if (canTarget(p)) result++;
        MTGGameZone* zones[] = {p->game->inPlay, p->game->graveyard, p->game->hand, p->game->library, p->game->exile};
        for (int k = 0; k < 5; k++) {
            MTGGameZone* z = zones[k];
            if (targetsZone(z)) {
                for (int j = 0; j < z->nb_cards; j++) {
                    if (canTarget(z->cards[j])) result++;
                }
            }
        }
    }
    return result;
}

bool TargetChooser::equals(TargetChooser* tc) {
    // This function always return 1 for now, since the default TargetChooser targets everything
    // In the future we might need to check some of "targetter" settings to take protection into account...
    return true;
}

/**
 a specific Card
 **/
CardTargetChooser::CardTargetChooser(GameObserver* observer, MTGCardInstance* _card, MTGCardInstance* source,
                                     int* _zones, int _nbzones)
    : TargetZoneChooser(observer, _zones, _nbzones, source) {
    validTarget = _card;
}

bool CardTargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (!target) return false;

    auto* card = dynamic_cast<MTGCardInstance*>(target);
    if (!card) return false;
    if (!nbzones && !TargetChooser::canTarget(target, withoutProtections)) return false;
    if (nbzones && !TargetZoneChooser::canTarget(target, withoutProtections)) return false;

    while (card) {
        if (card == validTarget) return true;
        card = card->previous;
    }
    return false;
}

CardTargetChooser* CardTargetChooser::clone() const {
    auto* a = NEW CardTargetChooser(*this);
    return a;
}

bool CardTargetChooser::equals(TargetChooser* tc) {
    auto* ctc = dynamic_cast<CardTargetChooser*>(tc);
    if (!ctc) return false;

    if (validTarget != ctc->validTarget)  // todo, check also previous cards, see "cantarget"...
        return false;

    return TargetZoneChooser::equals(tc);
}

/**
 Choose anything that has a given list of types
 **/
TypeTargetChooser::TypeTargetChooser(GameObserver* observer, const char* _type, MTGCardInstance* card, int _maxtargets,
                                     bool other, bool targetMin)
    : TargetZoneChooser(observer, card, _maxtargets, other, targetMin) {
    int id = MTGAllCards::findType(_type);
    nbtypes = 0;
    addType(id);
    int default_zones[] = {MTGGameZone::MY_BATTLEFIELD, MTGGameZone::OPPONENT_BATTLEFIELD};
    init(default_zones, 2);
}

TypeTargetChooser::TypeTargetChooser(GameObserver* observer, const char* _type, int* _zones, int nbzones,
                                     MTGCardInstance* card, int _maxtargets, bool other, bool targetMin)
    : TargetZoneChooser(observer, card, _maxtargets, other, targetMin) {
    int id = MTGAllCards::findType(_type);
    nbtypes = 0;
    addType(id);
    if (nbzones == 0) {
        int default_zones[] = {MTGGameZone::MY_BATTLEFIELD, MTGGameZone::OPPONENT_BATTLEFIELD};
        init(default_zones, 2);
    } else {
        init(_zones, nbzones);
    }
}

void TypeTargetChooser::addType(const char* _type) {
    int id = MTGAllCards::findType(_type);
    addType(id);
}

void TypeTargetChooser::addType(int type) {
    types[nbtypes] = type;
    nbtypes++;
}

bool TypeTargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (!TargetZoneChooser::canTarget(target, withoutProtections)) return false;
    if (auto* card = dynamic_cast<MTGCardInstance*>(target)) {
        for (int i = 0; i < nbtypes; i++) {
            if (card->hasSubtype(types[i])) return true;
            if (card->getLCName().size()) {
                if (MTGAllCards::findType(card->getLCName()) == types[i]) return true;
            }
        }
        return false;
    } else if (auto* action = dynamic_cast<Interruptible*>(target)) {
        if (action->type == ACTION_SPELL && action->state == NOT_RESOLVED) {
            auto* spell           = (Spell*)action;
            MTGCardInstance* card = spell->source;
            for (int i = 0; i < nbtypes; i++) {
                if (card->hasSubtype(types[i])) return true;
                if (MTGAllCards::findType(card->name) == types[i]) return true;
            }
            return false;
        }
    }
    return false;
}

TypeTargetChooser* TypeTargetChooser::clone() const {
    auto* a = NEW TypeTargetChooser(*this);
    return a;
}

bool TypeTargetChooser ::equals(TargetChooser* tc) {
    auto* ttc = dynamic_cast<TypeTargetChooser*>(tc);
    if (!ttc) return false;

    if (nbtypes != ttc->nbtypes) return false;

    map<int, int> counts;

    for (int i = 0; i < nbtypes; ++i) {
        counts[types[i]] += 1;
        counts[ttc->types[i]] -= 1;
    }

    for (int i = 0; i < nbtypes; ++i) {
        if (counts[types[i]] || counts[ttc->types[i]]) return false;
    }

    return TargetZoneChooser::equals(tc);
}

/**
 A Target Chooser associated to a Card Descriptor object, for fine tuning of targets description
 **/
DescriptorTargetChooser::DescriptorTargetChooser(GameObserver* observer, CardDescriptor* _cd, MTGCardInstance* card,
                                                 int _maxtargets, bool other, bool targetMin)
    : TargetZoneChooser(observer, card, _maxtargets, other, targetMin) {
    int default_zones[] = {MTGGameZone::MY_BATTLEFIELD, MTGGameZone::OPPONENT_BATTLEFIELD};
    init(default_zones, 2);
    cd = _cd;
}

DescriptorTargetChooser::DescriptorTargetChooser(GameObserver* observer, CardDescriptor* _cd, int* _zones, int nbzones,
                                                 MTGCardInstance* card, int _maxtargets, bool other, bool targetMin)
    : TargetZoneChooser(observer, card, _maxtargets, other, targetMin) {
    if (nbzones == 0) {
        int default_zones[] = {MTGGameZone::MY_BATTLEFIELD, MTGGameZone::OPPONENT_BATTLEFIELD};
        init(default_zones, 2);
    } else {
        init(_zones, nbzones);
    }
    cd = _cd;
}

bool DescriptorTargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (!TargetZoneChooser::canTarget(target, withoutProtections)) return false;
    if (auto* _target = dynamic_cast<MTGCardInstance*>(target)) {
        if (cd->match(_target)) return true;
    } else if (auto* action = dynamic_cast<Interruptible*>(target)) {
        if (action->type == ACTION_SPELL && action->state == NOT_RESOLVED) {
            auto* spell           = (Spell*)action;
            MTGCardInstance* card = spell->source;
            if (cd->match(card)) return true;
        }
    }
    return false;
}

DescriptorTargetChooser::~DescriptorTargetChooser() { SAFE_DELETE(cd); }

DescriptorTargetChooser* DescriptorTargetChooser::clone() const {
    auto* a = NEW DescriptorTargetChooser(*this);
    a->cd = NEW CardDescriptor(*cd);
    return a;
}

bool DescriptorTargetChooser::equals(TargetChooser* tc) {
    auto* dtc = dynamic_cast<DescriptorTargetChooser*>(tc);
    if (!dtc) return false;

    // TODO Descriptors need to have an "equals" method too -_-

    return TargetZoneChooser::equals(tc);
}

/* TargetzoneChooser targets everything in a given zone */
TargetZoneChooser::TargetZoneChooser(GameObserver* observer, MTGCardInstance* card, int _maxtargets, bool other,
                                     bool targetMin)
    : TargetChooser(observer, card, _maxtargets, other, targetMin) {
    init(nullptr, 0);
}

TargetZoneChooser::TargetZoneChooser(GameObserver* observer, int* _zones, int _nbzones, MTGCardInstance* card,
                                     int _maxtargets, bool other, bool targetMin)
    : TargetChooser(observer, card, _maxtargets, other, targetMin) {
    init(_zones, _nbzones);
}

int TargetZoneChooser::init(int* _zones, int _nbzones) {
    for (int i = 0; i < _nbzones; i++) {
        zones[i] = _zones[i];
    }
    nbzones = _nbzones;
    return nbzones;
}

int TargetZoneChooser::setAllZones() {
    int zones[] = {
        MTGGameZone::ALL_ZONES,
    };

    init(zones, 1);
    return 1;
}

bool TargetZoneChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (!TargetChooser::canTarget(target, withoutProtections)) return false;
    if (auto* card = dynamic_cast<MTGCardInstance*>(target)) {
        for (int i = 0; i < nbzones; i++) {
            if (zones[i] == MTGGameZone::ALL_ZONES) return true;
            if (MTGGameZone::intToZone(observer, zones[i], source, card)->hasCard(card)) return true;
        }
    } else if (auto* spell = dynamic_cast<Spell*>(target)) {
        WGE_LOG_TRACE("CHECKING Spell");
        if (spell->state == NOT_RESOLVED) {
            MTGCardInstance* card = spell->source;
            for (int i = 0; i < nbzones; i++)
                if (MTGGameZone::intToZone(Owner->getObserver(), zones[i], source, card)->hasCard(card)) return true;
        }
    }
    return false;
}

bool TargetZoneChooser::targetsZone(MTGGameZone* z) {
    for (int i = 0; i < nbzones; i++)
        if (MTGGameZone::intToZone(Owner->getObserver(), zones[i], source) == z) return true;
    return false;
}
bool TargetZoneChooser::targetsZone(MTGGameZone* z, MTGCardInstance* mSource) {
    if (mSource) source = mSource;
    for (int i = 0; i < nbzones; i++)
        if (MTGGameZone::intToZone(source->owner->getObserver(), zones[i], source) == z) return true;
    return false;
}

TargetZoneChooser* TargetZoneChooser::clone() const {
    auto* a = NEW TargetZoneChooser(*this);
    return a;
}

bool TargetZoneChooser::equals(TargetChooser* tc) {
    auto* tzc = dynamic_cast<TargetZoneChooser*>(tc);
    if (!tzc) return false;

    if (nbzones != tzc->nbzones) return false;

    map<int, int> counts;

    for (int i = 0; i < nbzones; ++i) {
        counts[zones[i]] += 1;
        counts[tzc->zones[i]] -= 1;
    }

    for (int i = 0; i < nbzones; ++i) {
        if (counts[zones[i]] || counts[tzc->zones[i]]) return false;
    }

    // TODO: ALL_ZONES should be equivalent to something actually targetting all zones...

    return TargetChooser::equals(tc);
}

/* Player Target */
PlayerTargetChooser::PlayerTargetChooser(GameObserver* observer, MTGCardInstance* card, int _maxtargets, Player* p)
    : TargetChooser(observer, card, _maxtargets), p(p) {}

bool PlayerTargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (source && targetter && (targetter->controller() != targetter->controller()->opponent()) &&
        (targetter->controller()->opponent()->game->inPlay->hasAbility(Constants::CONTROLLERSHROUD)) &&
        targetter->controller() != target)
        return false;
    if (source && targetter && (targetter->controller() == targetter->controller()) &&
        (targetter->controller()->opponent()->game->inPlay->hasAbility(Constants::PLAYERSHROUD)) &&
        targetter->controller()->opponent() == target)
        return false;
    if (source && targetter && (targetter->controller() == targetter->controller()) &&
        (targetter->controller()->game->inPlay->hasAbility(Constants::PLAYERSHROUD)) &&
        targetter->controller() == target)
        return false;

    auto* pTarget = dynamic_cast<Player*>(target);
    return (pTarget && (!p || p == pTarget));
}

PlayerTargetChooser* PlayerTargetChooser::clone() const {
    auto* a = NEW PlayerTargetChooser(*this);
    return a;
}

bool PlayerTargetChooser::equals(TargetChooser* tc) {
    auto* ptc = dynamic_cast<PlayerTargetChooser*>(tc);
    if (!ptc) return false;

    if (p != ptc->p) return false;

    return TargetChooser::equals(tc);
}

/*Damageable Target */
bool DamageableTargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (source && targetter && (targetter->controller() != targetter->controller()->opponent()) &&
        (targetter->controller()->opponent()->game->inPlay->hasAbility(Constants::CONTROLLERSHROUD)) &&
        targetter->controller() != target)
        return false;
    if (source && targetter && (targetter->controller() == targetter->controller()) &&
        (targetter->controller()->opponent()->game->inPlay->hasAbility(Constants::PLAYERSHROUD)) &&
        targetter->controller()->opponent() == target)
        return false;
    if (source && targetter && (targetter->controller() == targetter->controller()) &&
        (targetter->controller()->game->inPlay->hasAbility(Constants::PLAYERSHROUD)) &&
        targetter->controller() == target)
        return false;
    if (dynamic_cast<Player*>(target)) {
        return true;
    }
    return TypeTargetChooser::canTarget(target, withoutProtections);
}

DamageableTargetChooser* DamageableTargetChooser::clone() const {
    auto* a = NEW DamageableTargetChooser(*this);
    return a;
}

bool DamageableTargetChooser::equals(TargetChooser* tc) {
    auto* dtc = dynamic_cast<DamageableTargetChooser*>(tc);
    if (!dtc) return false;

    return TypeTargetChooser::equals(tc);
}

/*Spell */

SpellTargetChooser::SpellTargetChooser(GameObserver* observer, MTGCardInstance* card, int _color, int _maxtargets,
                                       bool other, bool targetMin)
    : TargetChooser(observer, card, _maxtargets, other, targetMin) {
    color = _color;
}

bool SpellTargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    auto* spell = dynamic_cast<Spell*>(target);
    if (!spell) return false;

    if (spell->state == NOT_RESOLVED) {
        MTGCardInstance* card = spell->source;
        if (card && (color == -1 || card->hasColor(color))) return true;
    }

    return false;
}

SpellTargetChooser* SpellTargetChooser::clone() const {
    auto* a = NEW SpellTargetChooser(*this);
    return a;
}

bool SpellTargetChooser::equals(TargetChooser* tc) {
    auto* stc = dynamic_cast<SpellTargetChooser*>(tc);
    if (!stc) return false;

    if (color != stc->color) return false;

    return TargetChooser::equals(tc);
}

/*Spell or Permanent */
SpellOrPermanentTargetChooser::SpellOrPermanentTargetChooser(GameObserver* observer, MTGCardInstance* card, int _color,
                                                             int _maxtargets, bool other, bool targetMin)
    : TargetZoneChooser(observer, card, _maxtargets, other, targetMin) {
    int default_zones[] = {MTGGameZone::MY_BATTLEFIELD, MTGGameZone::OPPONENT_BATTLEFIELD};
    init(default_zones, 2);
    color = _color;
}

bool SpellOrPermanentTargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (auto* card = dynamic_cast<MTGCardInstance*>(target)) {
        if (color == -1 || card->hasColor(color)) return TargetZoneChooser::canTarget(target, withoutProtections);
    } else if (auto* spell = dynamic_cast<Spell*>(target)) {
        if (spell->state == NOT_RESOLVED) {
            MTGCardInstance* card = spell->source;
            if (card && (color == -1 || card->hasColor(color))) return true;
        }
    }
    return false;
}

SpellOrPermanentTargetChooser* SpellOrPermanentTargetChooser::clone() const {
    auto* a = NEW SpellOrPermanentTargetChooser(*this);
    return a;
}

bool SpellOrPermanentTargetChooser::equals(TargetChooser* tc) {
    auto* sptc = dynamic_cast<SpellOrPermanentTargetChooser*>(tc);
    if (!sptc) return false;

    if (color != sptc->color) return false;

    return TargetZoneChooser::equals(tc);
}

/*Damage */
DamageTargetChooser::DamageTargetChooser(GameObserver* observer, MTGCardInstance* card, int _color, int _maxtargets,
                                         int _state)
    : TargetChooser(observer, card, _maxtargets) {
    color = _color;
    state = _state;
}

bool DamageTargetChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (auto* damage = dynamic_cast<Damage*>(target)) {
        if (damage->state == state || state == -1) {
            MTGCardInstance* card = damage->source;
            if (card && (color == -1 || card->hasColor(color))) return true;
        }
    }
    return false;
}

DamageTargetChooser* DamageTargetChooser::clone() const {
    auto* a = NEW DamageTargetChooser(*this);
    return a;
}

bool DamageTargetChooser::equals(TargetChooser* tc) {
    auto* dtc = dynamic_cast<DamageTargetChooser*>(tc);
    if (!dtc) return false;

    if (color != dtc->color || state != dtc->state) return false;

    return TargetChooser::equals(tc);
}

TriggerTargetChooser::TriggerTargetChooser(GameObserver* observer, int _triggerTarget) : TargetChooser(observer) {
    triggerTarget = _triggerTarget;
    target        = nullptr;
}

bool TriggerTargetChooser::targetsZone(MTGGameZone* z) { return true; }

bool TriggerTargetChooser::canTarget(Targetable* _target, bool withoutProtections) { return _target == target; }

TriggerTargetChooser* TriggerTargetChooser::clone() const {
    auto* a = NEW TriggerTargetChooser(*this);
    return a;
}

bool TriggerTargetChooser::equals(TargetChooser* tc) {
    auto* ttc = dynamic_cast<TriggerTargetChooser*>(tc);
    if (!ttc) return false;

    if (triggerTarget != ttc->triggerTarget) return false;

    return TargetChooser::equals(tc);
}

/*my curses */
bool myCursesChooser::canTarget(Targetable* target, bool withoutProtections) {
    for (unsigned int i = 0; i < source->controller()->curses.size(); ++i) {
        MTGCardInstance* compare = source->controller()->curses[i];
        if (compare == dynamic_cast<MTGCardInstance*>(target)) return true;
    }
    return false;
}

myCursesChooser* myCursesChooser::clone() const {
    auto* a = NEW myCursesChooser(*this);
    return a;
}

bool myCursesChooser::equals(TargetChooser* tc) {
    auto* dtc = dynamic_cast<myCursesChooser*>(tc);
    if (!dtc) return false;

    return TypeTargetChooser::equals(tc);
}

/*display cards blockable by source */
bool BlockableChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (auto* card = dynamic_cast<MTGCardInstance*>(target)) {
        return !(!card->isAttacker() || !source->canBlock(card));
    }
    return TypeTargetChooser::canTarget(target, withoutProtections);
}

BlockableChooser* BlockableChooser::clone() const {
    auto* a = NEW BlockableChooser(*this);
    return a;
}

bool BlockableChooser::equals(TargetChooser* tc) {
    auto* dtc = dynamic_cast<BlockableChooser*>(tc);
    if (!dtc) return false;

    return TypeTargetChooser::equals(tc);
}

//-----------
/*Proliferate Target */
bool ProliferateChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (auto* card = dynamic_cast<MTGCardInstance*>(target)) {
        return !(card->counters && card->counters->counters.empty());
    } else if (auto* p = dynamic_cast<Player*>(target)) {
        return p->poisonCount != 0;
    }
    return TypeTargetChooser::canTarget(target, withoutProtections);
}

ProliferateChooser* ProliferateChooser::clone() const {
    auto* a = NEW ProliferateChooser(*this);
    return a;
}

bool ProliferateChooser::equals(TargetChooser* tc) {
    auto* dtc = dynamic_cast<ProliferateChooser*>(tc);
    if (!dtc) return false;

    return TypeTargetChooser::equals(tc);
}

/*parents or children Target */
bool ParentChildChooser::canTarget(Targetable* target, bool withoutProtections) {
    if (auto* card = dynamic_cast<MTGCardInstance*>(target)) {
        if (type == 1) {
            if (!source->childrenCards.size()) return false;
            for (unsigned int w = 0; w < source->childrenCards.size(); w++) {
                MTGCardInstance* child = source->childrenCards[w];
                if (child == card) {
                    if (deeperTargeting) {
                        if (!deeperTargeting->canTarget(child)) return false;
                    }
                    return true;
                }
            }
        } else {
            if (!source->parentCards.size()) return false;
            for (unsigned int w = 0; w < source->parentCards.size(); w++) {
                MTGCardInstance* parent = source->parentCards[w];
                if (parent == card) {
                    if (deeperTargeting) {
                        if (!deeperTargeting->canTarget(parent)) return false;
                    }
                    return true;
                }
            }
        }
        return false;
    }
    return TypeTargetChooser::canTarget(target, withoutProtections);
}

ParentChildChooser* ParentChildChooser::clone() const {
    auto* a = NEW ParentChildChooser(*this);
    return a;
}

bool ParentChildChooser::equals(TargetChooser* tc) {
    auto* dtc = dynamic_cast<ParentChildChooser*>(tc);
    if (!dtc) return false;

    return TypeTargetChooser::equals(tc);
}

ParentChildChooser::~ParentChildChooser() { SAFE_DELETE(deeperTargeting); }
