# Create your own cards!
*source: [Wagic/CardCode from Miki](http://web.archive.org/web/20120129123445/http://wololo.net/miki/index.php/Wagic/CardCode)*

**his is an explanation on how to add your own cards to Wagic. Please try to create your own sets and share them with everyone! If you have questions, feel free to ask them in our [Cards/Sets creation forum](http://web.archive.org/web/20120129123445/http://wololo.net/forum/viewforum.php?f=8).**

Information is current as of 02/20/2011.

This is a work in progress. Please report any errors, ambiguities, or incomprehensible passages to our forum. Some statements in this document still require checking, these have been marked with (??).

# File structure

The cards used in Wagic can be found in the *Res/sets/* folder. This folder contains several subfolders, one for every set.

Each of these subfolders contains a *"_cards.dat"* file. As long as this file is included in a subdirectory of the "sets" folder, the cards it describes will be automatically loaded into the game.

For example: Having a file ***Res/sets/MYSet/_cards.dat*** will automatically create a set called MYSet, which contains all cards specified in this _cards.dat file, whenever you start the game.

So what does this _cards.dat file look like? Well, you can open one of those with any text editor (Notepad, for example), and you'll see that it is a list of:

```
[card]
  (content)
[/card]
```
So what can be put in contents? Contents is a list of keys and values spearated by a "=" sign. the keys can be the following:

* [name](#name)
* [primitive](#primitive)
* [grade](#grade)
* [id](#id)
* [rarity](#rarity)
* [mana](#mana)
* [kicker](#kicker)
* [color](#color)
* [type](#type)
* [subtype](#subtype)
* [power](#power)
* [toughness](#toughness)
* [text](#text)
* [target](#target)
* [abilities](#abilities)
* [auto](#auto)
* [autohand](#autohand)
* [autograveyard](#autograveyard)
* [alias](#alias)

**name, id, rarity, and type are compulsory.** The other ones, in some circumstances, are needed as well (such as toughness and power for "regular" creatures). Text is compulsory if you do not plan to add pictures (or plan to add pictures that do not have that text) to your set.

# name

**Description:** name denotes the name of the card

**Example:** `name=Grizzly Bears`

**Further information:** The engine currently adds names to the list of types, to make cards like "Plague Rats" work. This shouldn't affect anything else, but if you're giving a card a name that is also a type (for example, naming a card "Goblin"), we advise thorough testing.

# primitive

**Description:** primitive tells the engine to read all information for that card from a list of "primitives", which already contains an entry of this name.

**Example:** `primitive=Grizzly Bears`

**Usage:** The idea of this feature is to allow you to add "reprints" of cards into your set very easily. For example, if you want to add a reprint of *Royal Assassin* to your set, you only have to specify its id, its rarity, and the line `primitive=Royal Assassin`. Since *Royal Assassin* has already been coded (and inlcuded in the primitives file), all other data for the card will be taken from there.

This also means that you don't need to update your set when wechange the coding of Royal Assassin - your set will always load the information from the primitives file, which in this case will contain the updated version of Royal Assassin.

# grade

**Description:** gives a hint on the quality of the code of the card, how well the card respects the actual rules printed on it

**Example:** `grade=Borderline`

**Usage:** Currently 5 values are accepted for the grade: **Supported, Borderline, Unofficial, Crappy, Unsupported, Dangerous**. When this line is not specified, the default grade is "Supported". Here are the meanings of these values:

* **Supported:** Wagic handles this card 100% correctly, or the issues encountered with it are minor. This card can go into an AI Deck
* **Borderline:** Wagic handles this card correctly in most cases. In some edge cases, the card will not work as expected but this shouldn't be too much of an issue (example: Drain Life can use non black mana). Please think about it twice before putting such a card in an AI Deck
* **Unofficial:** The person who coded this card claims that you can use it, and according to this person, this card works as advertised. The only difference with such a card and a "Borderline" card is that the Wagic team hasn't had the time to review this card yet and mark it as "Supported" or "Borderline".
* **Crappy:** Wagic handles this card, but there are some side effects that can make it completely unbalanced. For example, an effect supposed to end at the end of the turn will last as long as the card is in play... DO NOT add a card marked as "Crappy" into an AI Deck
* **Unsupported:** The card doesn't work, mostly. For example for a creature, it can mean that its special abilities are not supported at all, and it will just be an expensive Vanilla creature. This is just for people who want to play with the card because they like the art, for example. Do not put these cards in an AI Deck
* **Dangerous:** This card can potentially crash Wagic. Even if it works 99% of the time, that's not our concern here. Of course, do not put those into an AI Deck!

Additionally, Wagic players have an option to choose which cards they want to have in their database. By default, this option is set to "Borderline", which means that both Supported and Borderline cards will be loaded at startup. Players who want to experiment with less safe cards can change this value. Next time the game is restarted, it will load the additional cards.

It is the role of the cards creators and developers to determine if a card goes to "Supported", "Borderline", or "Crappy". The line between these 3 categories is extremely thin, and the decision can take into account the popularity of the card, the inconvenience caused by the bugs, the difficulty involved with solving the bugs, etc... As a policy, the Wagic team will not accept bug reports for cards that are marked as "Crappy", "Unsupported", or "Dangerous". This can also be used as a factor in the decision to rank a card (especially between crappy and Borderline).

**Note:** it is possible to assign a grade to an entire primitive file by setting the grade at the very top of your file, BEFORE the first [card] tag.

# id
**Description:** id is a unique integer identifying the card. This id cannot be the same as any other id used in this set or in any other set.

**Example:** `id=174908`

Usage: To keep ids unique, it's best (for Magic cards) to use the id referenced at the Gatherer site. For example: [https://gatherer.wizards.com/Pages/Card/Details.aspx?multiverseid=49](https://gatherer.wizards.com/Pages/Card/Details.aspx?multiverseid=49) (Bad Moon alpha), the id is 49, as you can see from the number at the end of the URL.

For your own sets, try to choose an id range that no other sets use. Official sets use ids between 1 and about 250,000, so for your own sets, we suggest using values between 1,000,000 (one million) and 2,000,000,000 (two billion).

The engine stores collections and decks as lists of card ids. If a card is part of a collection, and its id gets changed, then the engine will notice that it does not have a card of that id in its database, and remove it from the collection.

The id will also be used to retrieve the picture for the card. For the card with the id 1,234,567 in set MySet, the engine will look for a card picture with the path and filename *Res/sets/MySet/1234567.jpg*, and a thumbnail *picture Res/sets/MySet/thumbnails/1234567.jpg*

Thumbnails must have a size of 45x64 pixels, and pictures must be 200x285.

# rarity

**Description:** rarity denotes the probability of a card to be found in a booster pack. Valid values are C, U, R, M, L, T and respectively mean Common, Uncommon, Rare, Mythic Rare, Land, Token.

**Example:** `rarity=U` (for an Uncommon card)

**Notes:** Currently, a booster pack will have 1 card with rarity L, 10 cards with rarity C, 3 cards with rarity U, one card with rarity R (which has a 1/8 chance to be of of rarity M instead), and no cards of rarity T.

The "T" rarity can be used to create tokens with complex rules (see below).

# mana

**Description:** mana denotes the cost needed to play the card from the hand. Each part of the cost is enclosed in braces {}. Valid values are {W}, {U}, {B}, {R}, and {G} (meaning (W)hite, bl(U)e, (B)lack, (R)ed, and (G)reen respectively), and also integers like {3}, {16}, or {0}, which denote costs that can be paid with mana of any color.

To represent hybrid mana, you put two mana symbols inside the same braces.

Additional valid values:

* `{D}` (Discarding a random card)
* `{E}/{E(target|zone)}` (Exiles something)
* `{H}/{H(target|zone)}` (Returns something to your hand)
* `{L}` (life loss)
* `{M}` (Puts a card from the top of you library)
* `{s2l}` (put *target|whereever* on top of your library)
* `{X}` or `{X}{X}` (now 2 {X} are possible! (Card example:Decree of Justice))
* `{l2e}` (Exiles the top card of you library )

**Examples:**

* `mana={W}{R}{R}` (one white and two red mana)
* `mana={3}{G}` (1 green mana, and 3 mana of any color)
* `mana={U}{WB}` (1 blue mana, and one mana which can be either white or black)
* `mana={2R}{2R}{2R}` (3 red mana, but each one of those could also be paid by 2 mana of any other colors)

**Limitation:** While it is possible in kicker costs (see kicker (cost) for these), alternate costs, buyback or flashback costs, it is currently not possible to specify any additional or alternative costs (like sacrificing or tapping cards) in the mana= line. Some kinds of additional costs can be snuck in by other means, see cost of activated abilities.

# kicker

**Description:** kicker denotes an additional cost that may be paid when playing the card. It uses the same format as [cost of activated abilities](#auto-cost). You can specify effects for a card which apply only if its kicker cost was paid (see [kicker condition](#kicker-condition) for these).

**Example:** A card that gives you 1 life if you pay its normal cost of {W}, but an additional 4 life if you pay an additional cost of `{1}{W}`, could be coded like this:

```
mana={W}
kicker={1}{W}
auto=life:1 controller
auto=kicker life:4 controller
```

# other 

**Description:** other denotes an alternate cost that may be paid instead of the mana cost, to play the card. It uses the same format as [cost of activated abilities](#auto-cost). You can specify effects for a card which apply only if its "other" cost was paid (see [alternative (condition)](#alternative_condition) for these).

## flashback

**Description:** flashback denotes an alternate cost that may be paid to play the card from a graveyard instead of playing it from your hand.It uses the same format as [cost of activated abilities](#auto-cost). Such a card will be moved to your "exile" zone after being played through it flashback cost. Therefore this cost makes sense only for sorceries and instants

## buyback

**Description:** buyback denotes an alternate cost that may be paid when playing the card. It uses the same format as [cost of activated abilities](#auto-cost). If the buyback cost was paid, the card will go back to your hand instead of going to the graveyard once it resolves. Therefore buyback only makes sense for sorceries and instants. Note that buyback is an additional cost in MTG, but is described as an alternate cost in Wagic. So you need to include the original mana cost as well

**Example:** A card that gives you 1 life if you pay its normal cost of `{W}`, but goes back to your hand if you pay an additional cost of `{1}{W}`, could be coded like this:

```
mana={W}
buyback={1}{W}{W}
auto=life:1 controller
```

# color

**Description:** color denotes the color of the card. Usually colors get determined automatically from the [mana cost](#mana), but some cards have special rules which determine their color(s). For these, the color key is provided. Valid values are *white, blue, black, red, green, artifact*

**Example:** `color=red`

**Limitation:** You can only set one color this way. It is not possible to specify several different colors.

# type

**Description:** type denotes the type(s) and supertype(s) of the card, space separated. Although anything's fine in there, most used values are: *Sorcery, Instant, Creature, Artifact, Artifact Creature, Enchantment, Land*. These can be prefixed by the following: *"Legendary", "Snow", "Basic",...*

**Examples:**

```
type=Enchantment
type=Legendary Creature
type=Basic Land
```

**Limitations:** While you can specify types and subtypes on two separate lines, the engine currently treats only a few specific hardcoded values as "types" or "supertypes", the rest is treated as subtypes.

# subtype

**Description:** subtype denotes all subtypes of the card, separated from each other by a space character.

**Examples:**
```
subtype=goblin
subtype=human wizard ally
subtype=aura
```
**Usage:** Just as [types](#type), you can specify a list of several subtypes.

# power

**Description:** power is an integer describing the power of the card. It is used for creatures only.

**Example:** `power=5`

**Usage:** Some cards have a power of `*`, or `*+1`. In this case, specify the value that the term would have if the `*` was a zero - e.g. `power=0` and `power=1` for the two examples above. The functionality of the `*` can be coded by a later [auto](#auto) rule.

# toughness

**Description:** toughness is an integer describing the toughness of the card. It is used for creatures only.

**Example:** `toughness=5`

**Usage:** Some cards have a toughness of `*`, or `*+1`. In this case, specify the value that the term would have if the `*` was a zero - e.g. `toughness=0` and `toughness=1` for the two examples above. The functionality of the `*` can be coded by a later [auto](#auto) rule.

# text

**Description:** text is a text describing the effects of the card.

**Example:** `text=Flying. First Strike. Whenever this creature attacks, draw a card.`
# abilities

**Description:** abilities denotes those special abilities of a card that can be defined with a single keyword, i.e which don't need any values specified for them.

You can specify several abilities by separating them with commas.

The following abilities are supported:
* absorb
* affinityartifacts / affinityforests / affinitygreencreatures / affinityislands / * affinitymountains / affinityplains / affinityswamps
* cantattack
* cantblock
* cantlifelose *(controller can't lose by getting to 0 life or below)*
* cantlose *(controller can't lose - Card example:Platinum Angel)*
* cantmillose *(controller can't lose by getting milled)*
* cantregen *(creature can't regnerate)*
* cantwin *(controller cant win the game / opponent cant lose)*
* changeling
* cloud *(can only block creatures with flying)*
* controllershroud *(Card example:True Believer)*
* deathtouch
* defender
* doesnotuntap *(... during it's controller's untap phase)*
* double strike
* exalted
* exiledeath *(will send the card to exile instead of to graveyard opon death. Treated * as a replacement effect.)*

* fear
* first strike
* flash
* flanking
* flying
* foresthome / islandhome / mountainhome / plainshome / swamphome
* forestwalk / islandwalk / mountainwalk / plainswalk / swampwalk
* frozen *(card/target will not untap on its controllers next untap phase. Card example:Tangle)*
* haste
* horsemanship
* indestructible
* infect
* intimidate
* leyline *Enables the coding of the Leyline cycles*
* lifelink
* mustattack
* nofizzle *(cannot be countered)*
* nomaxhand *(controller has no maximum max hand)*
* opponentshroud *(cannot be targeted by spells or abilities that the opponent controls)*
* persist
* phantom *(prevents damage and remove one +1/+1 counter. Card examples:Phantom Centaur,Phantom Nishoba)*
* playershroud *???*
* poisontoxic / posiontwotoxic / poisonthreetoxis *(card adds 1,2 or 3 poison counters to the player if damaged)*
* protection from white / protection from blue / protection from black / protection from red / protection from green *(note that there's also "protection from(target_type)" available as an auto rule)*
* rampage (??) *(note that there's also "rampage(p/t,n)" available as an auto rule)*
* reach
* reachshadow *(can block creatures with shadow)*
* retrace
* shadow
* shroud
* storm
* sunburst
* trample
* treason *(card with abilities=treason will be sacrificed at the end of the turn. Card example:Sneak Attack)*
* unblockable *(note that there's also "cantbeblockedby(target_type)" available as an auto rule)*
* vigilance
* vigor *(instead of taking damage the source gains +1/+1 counters. Card examples:Phytohydra)*
* wilting *(source takes damage in the form of -1/-1 counters)*
* wither

* legendarylandwalk
* desertlandwalk
* snowforestlandwalk
* snowplainslandwalk
* snowmountainlandwalk
* snowislandlandwalk
* snowswamplandwalk
* snowlandwalk
* nonbasiclandwalk
* strong *(cant be blocked by creature with less power)*
* weak *(cant block creatures with more power)*
* phasing
* split second


Note that despite being supported, some of the abilities don't work like you would expect them to, due to some limitations in the engine.

**Example:** `abilities=flying,first strike,shadow`

## deprecated abilities

As wagic evolves, we simplify some of the abilities, make them more flexible, or get rid of abilities that don't work correctly. We try to stay backwards compatible, but sometimes there's no choice...

* bothnocreature (no player can cast creatures) (deprecated since version 0.15, use maxCast instead)
* bothcantcast (no player can cast spell) (deprecated since version 0.15, use maxCast instead)
* bothonecast (no player can cast more than one spell per turn) (deprecated since version 0.15, use maxCast instead)
* cantcreaturecast (card controller cant cast creature) (deprecated since version 0.15, use maxCast instead)
* cantspellcast (card controller can't cast spells) (deprecated since version 0.15, use maxCast instead)

The following abilities were once available, but never worked right and have been removed for the time being: *banding*

# target

**Description:** target defines the potential targets for the card.

There is an important distinction between instants/sorceries and other cards. **Only** instants/sorceries can use the `target=` key. All other cards specify targets in their [auto](#auto) rules.

The syntax for the target specification is the same though, no matter whether a target is specified with a `target=` key or inside an `auto=` rule. Therefore, the complete syntax is explained here.

**The syntax is the following:**

`target=target_type_1[s][,target_type_2[s]][|zone1[,zone2]]`

## target types
Valid keywords for target_type are:

* any word that can be found in the [type](#type) or [subtype](subtype) key of any card
* player (meaning "a player")
* the * symbol (asterisk), meaning "a card of any type"
* this (meaning: the one card that has this rule)
* mytgt (meaning: the target of the card that has this rule. Mytgt uses a very broad definition of "target". For example, an aura can always use mytgt to reference the permanent it enchants as "mytgt", even though - according to the rules - it isn't technically targeting it anymore.)

Adding a "s" to a target means "1 target or more". Otherwise, it is 1 target. Note: This may not work as expected, depending on the effect applied to the targets. (??)

**Examples:**

|               |                                                                                                                       |
|---------------|-----------------------------------------------------------------------------------------------------------------------|
| creature	    | *targets one creature*                                                                                                |
| goblin,player	| *targets one goblin OR one player*                                                                                    |
| wall,land,cat	| *targets one wall or land or cat*                                                                                     |
| `*`	        | *targets one card of any type*                                                                                        |
| `elfs`	    | *targets one or more elves - note that the parser doesn't understand irregular plurals, so "elves" would not work*    |
| `*s`	        | *targets one or more card of any type (??)*                                                                           |

**Limitations:** Although you can use "player" as a target, you cannot specify anything about this player. For example, you cannot specify "opponent" (which would be useful for rules like "Whenever this card deals damage to an opponent ...").

## target restrictions

A target can be followed by a string inside brackets (`[]`) to give more specific information on it. Keywords allowed inside the brackets are:

* attacking
* blocking
* tapped
* one or more colors *(white, blue, black, red, green, artifact)*
* one or more abilities, such as *flying*.
* [power](#power), [toughness](#toughness), and/or [manacost](#mana)
* gear *looks if a card is equipped* `["this(gear !=0) effect"]` *or how many artifacts are on a card* `["thisforeach(equip) effect"]`
* fresh *[fresh]|zone donates a card that was placed into the determined zone this turn. [fresh] works for the zones `battlefield` and `graveyard`.*
* multicolor *denotes multicolored target*
* leveler *checks if the card is a level up card*
* enchanted *checks if the card is enchanted*

the following checks if both the colors are present on the target -

* blackandgreen
* blackandwhite
* redandblue
* blueandgreen
* redandwhite

Keywords inside brackets can be prefixed by a minus (`-`) sign that means "not". For example, `-tapped` means "not tapped", i.e. untapped.

Keywords inside brackets are separated by a semicolon (`;`). Currently, the relation between these restrictions is "or" if there is no minus sign inside the brackets and if the restrictions are of the same kind (for example, both are colors, or both are abilities), and "and" otherwise. (??) This has led to confusion and will probably change in the future.

Each of the three keywords `power`, `toughness`, and `manacost`, can be followed by one of three comparison operators: = (equal to), <= (less than or equal), >= (greater than or equal); and then by an integer, to which the value will be compared.

**Examples:**
|                               |                                                                           |
|-------------------------------|---------------------------------------------------------------------------|
| `creature[blue]`              | *targets one blue creature*                                               |
| `artifact[-tapped]`           | *targets one untapped artifact*                                           |
| `creature[-black;-artifact]`  | *targets one creature that is not black AND is not an artifact*           |
| `creature[elf;warrior]`       | *targets one creature that is an elf OR a warrior OR both*                |
| `cleric[toughness<=4]`        | *targets one cleric whose toughness is 4 or less*                         |
| `aura[manacost>=3]`           | *targets one aura which has a converted manacost of 3 or more*            |
| `creature[fresh]`             | *targets a creature which was a put into the determined zone this turn*   |

**Limitations:** Target resrictions are currently not available for targets using the keywords `player`, `this` or `mytgt`. So, unfortunately, it is not yet possible to code cards with rules like "When this card attacks ...", `because aslongas(this[attacking])` cannot be parsed.

## target zones

You can limit the targeting of your card to specific zones. The following zones are available: `battlefield`, `graveyard`, `library`, `hand`, `stack`, `exile`, `*`

The asterisk (`*`) means "all zones". You can also use the term `inplay` synonymous for `battlefield`, and `removedfromgame` synonymous for `exile`.

The default value for zones is `inplay`.

'Battlefield' encompasses the whole battlefield, 'graveyard' encompasses all graveyards, 'library' encompasses all libraries, etc. This is often not desired, so you can limit the zone further by preceding its name with one of the following terms: `my`, `opponent`, `owner`, `targetcontroller`, `targetowner`

The prefix and the zone name form a single word, e.g. `mybattlefield` (for: anything on the battlefield under my control) or `opponentgraveyard`.

These prefixes are always seen from the perspective of the player who controls the card that uses them in its code. Let's say I'm playing against *Sheila*, and I control a creature called *Painter*, which lets me target a graveyard and change the color of all cards inside to green. The following table shows which graveyard would be affected by this ability, depending on the prefix used before 'graveyard':

| Prefix	        | Meaning	                                                                | "Painter" affects ...                                             |
|-------------------|---------------------------------------------------------------------------|-------------------------------------------------------------------|
| (none)	        | *the whole zone*	                                                        | ... all graveyards of all players                                 |
| `my`	            | *zone/cards controlled by the controller of the targeting card*           | ... my graveyard, because I'm controlling Painter                 |
| `opponent`	    | *zone/cards controlled by the opponent of the targeting card's controller*| ... Sheila's graveyard, because she's the opponent of Painter     |
| `owner`	        | *zone/cards controlled by the owner of the targeting card*                | ... usually my graveyard, assuming that Painter was originally part of my deck. If Painter was originally part of Sheila's deck, and I merely took control of it during play, then Sheila is Painter's owner, and her graveyard would be targeted.                 |
| `targetcontroller`| *zone/cards controlled by the controller of the targeted card*            | If target is a player, targetcontroller will be that player. If target is a card, targetcontroller will be that card's controller |
| `targetowner`	    | *zone/cards controlled by the owner of the targeted card*                 | If target is a player, targetcontroller will be that player. If target is a card, targetcontroller will be that card's owner |

**Limitations:**

* Anything that allows a player to look into a library will cause the library to be shuffled after the effect has resolved.
* Some zones are not supported by the interface (although the engine can handle them). For example, the `exile` zone cannot be displayed at all, although the engine can address it and move cards to it.
* `mybattlefield` really means *"the zone formed by all permanents on the battlefield which I control"*. It does not mean *"my half of the duel screen"*. This is important for auras cast on opponent creatures: these are displayed in their opponent's half of the screen, but nevertheless belong to their controller's battlefield.

## example targets

All this sounds pretty complex, but it's not. Let's see some working examples:

|                                   |                                                                                       |
|-----------------------------------|---------------------------------------------------------------------------------------|
| `creature`	                    | *any creature in play*                                                                |
| `artifact,enchantment`	        | *any artifact or enchantment in play*                                                 |
| `*\|graveyard`	                    | *any card in any graveyard*                                                           |
| `creature,player`	                | *any creature or player in play*                                                      |
| `mountains`	                    | *one or more mountains in play*                                                       |
| `artifact,enchantment\|mygraveyard`| *any artifact or enchantment in your graveyard*                                       |
| `creature[-black;-artifact]`	    | *any non black, non artifact creature in play (that's what actually used for Terror)* |
| `*\|stack`                         | *any spell on the stack*                                                              |

If you cannot describe the target with the available keywords, then you're doomed, you cannot add the card to the game without coding.

# auto

auto is the most complex key, as it allows to program the behavior of the cards to some extent. If you cannot add what you want with auto, it means you cannot add the card to the game without recompiling it.

Unlike other parameters, auto can appear several times in one card description.

Please note that the `auto` parameter is changing very often, and we cannot guarantee that future releases will be backward compatible!

## auto syntax

Values for `auto` often consist of several parts. An (approximated) general syntax for auto values is this (values inside brackets `[]` are optional):

`[cost:|trigger:][conditions]effect [&& further effects] [target|playertarget][restrictions]`

The individual parts and their meaning:

* [cost](#auto-cost) - the cost for activating this effect
* [trigger](#auto-trigger) - an event that triggers this effect
* [conditions](#auto-conditions) - special conditions which need to be fulfilled for this effect to take place
* [effect](#auto-effects) - the effect itself
* [further effects](#auto-effects) - additional effects
* [target](#auto-target)|[playertarget](#target) - the target(s) of the effect(s)
* [restrictions](#auto-restrictions) - restrictions that apply to this rule, e.g. when and how often it can be activated

The order of elements if often interchangeable, but not always. The order used in this reference will give you maximal safety that your code can be parsed correctly. This can be important because the parser is a bit stingy with error messages (you'll only get any if you compile the game in debug mode, and even then they are sparse).

Spaces are necessary in some places, forbidden in some, and optional in some other places. The easiest rule to prevent problems is to never put a space in front of a "(" character.

Colons are important signposts for the parser, make sure that you use them correctly (look at the examples provided here).

## auto cost

For most effects, cost is an optional parameter. If an auto rule has no cost, then it will be executed whenever the card is played or enters the battlefield. For example, a creature coded with `auto=life:2` would give its controller 2 life whenever it enters the battlefield. The effects of most instants and sorceries are coded without a cost as well, since their cost is being paid when casting the spell.

If an auto rule has a cost, then this denotes an activated ability of this card. In an auto rule, cost may contain any of the following elements:

* any kind and number of mana costs, specified with the same syntax as for [mana](#mana) keys
* one `{T}` symbol, for "tap this permanent", or `{T(target)}`, where target is anything that could be represented in a [target](#target) key.
* one sacrifice, which can be specified either as `{S}` (for "sacrifice this permanent"), or `{S(target)}`, where target is anything that could be represented in a [target](#target)  key.
* `{E}/{E(target|zone)}` *Exiles something as additional casting/activation cost*
* `{H}/{H(target|zone)}` *Returns something to your hand as additional casting/activation cost*
* `{L}` *life loss as additional casting/activation cost*
* `{D}` *Discarding a random card as additional casting/activation cost*
* `{M}` *Puts a card from the top of you library as additional casting/activation cost*
* `{N}` *cost of returning an unblocked attacker to your hand*
* `{discard(target|zone)}` *targeted discard cost*

**Note:** You have to type in one `{L}` for each single point of life to pay for the autoline's activation (see "Examples"). The same is true for `{D}` and `{M}`.

* `{s2l}` *put *target|whereever* on top of your library as additional casting/activation cost*
* `{l2e}` *Exiles the top card of you library as additional casting/activation cost.Card example:Arc-Slogger*

**Examples:**

|                                                   |                                                                                                                               |
|---------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------|
| `{T}`                                             | *tap this card*                                                                                                               |
| `{G}{T}`                                          | *pay one green mana and tap this card*                                                                                        |
| `{1}{S(goblin\|mybattlefield)}`                   | *pay 1 mana and sacrifice a goblin you control*                                                                               |
| `{S(*\|myhand)}`                                  | *choose and discard a card - discarding is technically very similar to "sacrificing from the hand"* (??)                      |
| `{0}`                                             | *This ability can be activated without any cost, but it needs to be activated, it does not fire automatically or continuously*|
| `{L}{L}{L}{B}{S(creature\|mybattlefield)}`        | *pay 3 life, 1 black mana and sacrifice a creature you control*                                                               |
| `{E(*\|mygraveyard)}{T}`                          | *exile a card from your graveyard and tap this card*                                                                          |
| `{T(elf\|mybattlefield)}{T(elf\|mybattlefield)}`  | *tap 2 elves you control*                                                                                                     |

As you can see, there are no limitations in combining the different auto costs.

**Limitations:**

* previously only one targeted cost was possible, this is no longer the case. it is now possible to denote as many targeted cost in a single cost line as you wish.
* Note that for targeted sacrifice costs, you always have to add a target zone (usually `|myinplay`), otherwise the player will be able to sacrifice cards controlled by the opponent.

## auto triggers
Triggers are effects that are executed not immediately, but when a certain event occurs. Each trigger defines the event that triggers it.

Triggers are declared with the keyword @, and end with a colon :. The most basic trigger syntax is: `@[trigger]:[effect]`

The following triggers are available in Wagic:

* `(various names)`: trigger on phase start
* `@movedto`: trigger on a card having changed zones
* `@attacking`: trigger on a card declared as an attacker
* `@blocking`: trigger on a card declared as an blocker
* `@blocked`: trigger on an attacking card being blocked
* `@notblocked`: trigger on an unblocked attacking card
* `@damaged`: trigger on a card or player being damaged
* `@combatdamaged`: trigger on a card or player being combat damaged
* `@noncombatdamaged`: trigger on a card or player being damaged by a noncombat source.
* `@tapped`: trigger on a card becoming tapped
* `@tappedformana`: trigger on a card becoming tapped for mana
* `@untapped`: trigger on a card becoming untapped
* `@drawn`: trigger related to each card draw
* `@vampired`: triggers when a card is put in graveyard this turn that was damaged by source
* `@lifed`: trigger related to life gain
* `@lifeloss`: trigger related to lifeloss
* `@targeted`: trigger related to a card being targeted
* `@discarded`: trigger related to cards being discarded
* `@sacrificed`: trigger related to cards being sacrificed
* `@attackedalone`: trigger related to when a card attacked alone

**Limitation:**

It is currently not possible to pay a [cost](#auto-cost) during a triggered effect.

### **phase-based triggers**

`@[next|each] [my|opponent|targetcontroller] (beginofturn|untap|upkeep|draw|firstmain|combatbegins|attackers|blockers|combatdamage|combatends|secondmain|endofturn|cleanup|beforenextturn): effect`

The effect will trigger at the beginning of the respective phase or step.

The keywords `next` and `each` determine whether the effect will trigger only the `next` time that the given phase begins, or `each` time that a phase of this name begins (as long as the rule is active).

The keywords `my`, `opponent`, and `targetcontroller`(??) determine whether only phases of the card's controller are counted (`my´), or only phases of the card controller's opponent (`opponent`). If neither of the three is specified, then both players' turns can fire the trigger.

**Examples:**

|                                           |                                                                   |
|-------------------------------------------|-------------------------------------------------------------------|
| `@each my upkeep:life:2 controller`	    | *gives the card's controller 2 life at each of his upkeep phases* |
| `@each combatbegins:life:2 controller`	| *gives the card's controller 2 life at each of his upkeep phases* |
| `@next opponent end:draw:2 opponent`      | *lets the opponent draw 2 cards at the end of his next turn*      |

**Limitations:**

It is curently not possible to trigger an effect at the end of a phase or step. A workaround is to let the effect trigger at the beginning of the following step (for example, instead of trying to trigger at the end of upkeep, let the effect trigger at the begin of the draw phase phase). This will work as long as Wagic has no cards which skip steps. (In the above example, skipping the draw phase would skip the trigger, which wouldn't be the desired outcome).

Some phases are currently "messy", and it's not guaranteed that triggers upon them will work. Specifically, the phase `beginofturn` may be over before all triggers had a chance to fire. The phases from `draw` to `secondmain` (including the two themselves) should be safe; for all others, thorough testing is advised.

#### `@movedto`
`@movedTo(target) [from(zone)]: effect`

The effect will be executed whenever a card matching the target specifications comes from "zone". The "from" parameter is optional and defaults to "anywhere".

Note that the first parameter is a target specification, not just a zone name. This means you can code rules like "Whenever a goblin comes into play ..." with a trigger `@movedTo(goblin|battlefield)`.

#### `@damaged`
`@damaged(target1) [from(target2)]: effect`

The effect will be executed whenever a card (or player) matching the target1 specifications is damaged by a card matching the target2 specifications. The "from" part is optional and defaults to "any source".

This trigger will fire on any kind of damage being dealt, not only combat damage. An activated ability "Tap this card to deal 1 damage to target player" would cause the trigger to fire as well.

#### `@tapped`
`@tapped(target): effect`

The effect will be executed whenever a card matching the target specifications get tapped.

### **Special case trigger restrictions**

note that not all of the triggers can use all of the restrictions, these were added on a "needed" basis.

|                               |                                                                           |
|-------------------------------|---------------------------------------------------------------------------|
| `[trigger] once`              | *this trigger will only fire once in its life time then become inactive*  |
| `[trigger] sourcenottap`      | *this trigger will only fire if the source is not tapped.*                |
| `[trigger] sourcetap`         | *this trigger will only fire if the source is tapped.*                    |
| `[trigger] opponentpoisoned`  | *this trigger will only fire if the opponent is poisoned.*                |
| `[trigger] turnlimited`       | *this trigger will only fire once per turn.*                              |

## auto conditions

The conditions in this section govern whether or not an effect will be executed. Currently Wagic knows two special conditions, [kicker](#kicker-condition) and [may](#may).

### **kicker condition**

An effect that uses the term `kicker` in its `auto=` rule will only be executed if this card's kicker cost was paid. Usually `kicker` is the first word in the `auto=` rule. See [kicker cost](#kicker) for an example.

**Limitations:** Kicker currently only allows to specify *additional* effects. If a card's kicker effect *replaces* its usual effect, then the card cannot be coded. To do this, you would need to tell the *regular* effect to only execute if the kicker cost was not paid, and we curently don't have a *nokicker* condition. *(Note to wololo: I may have a way of solving this, and enabling cards with more than one kicker as well, but this will require more thought.)*

### **may**

The `may` keyword is used to represent the text *"When ... comes into play, you may ... "*. When this rule is executed, then player will see a menu pop up, which will allow him to choose the rule's effect or cancel. The effect following the `may` keyword will only be executed if the player chose to do so.

**Example:**

|                                                           |                                                           |
|-----------------------------------------------------------|-----------------------------------------------------------|
| `auto=may life:2`                                         | *When this comes into play, you may gain 2 life.          |
| `auto=may bury target(creature)`                          | *When this comes into play, you may bury target creature. |
| `auto=@movedTo(*[white]\|stack): may life:1 controller`   | *Whenever a white spell is cast, you may gain 1 life.*    |


**Limitations:**

* `May` was designed for "comes into play" effects. It also works for triggers like the one in the last example. It may not work in other circumstances (e.g. in activated abilities), so thorough testing is advised for these cases.
* If a card has more than one `may` rule active in a given situation, then these rules will compete with each other (see [competing effects](#competing-effects)). The player may choose one of them, but not more.

## auto effects

A wide (and ever-increasing) range of effects is available for `auto=` rules. The following effects are currently supported:

* `[cost:]bury [target]` - destroys target, it cannot be regenerated
* `[cost:]destroy [target]` - destroys target
* `[cost:]damage:n [target|playertarget]` - does n damage to target. n may be a [variable](#auto-parsed-integers).
* `[cost:]regenerate [target]` - regenerates target creature
* `[cost:]tap [target]` - taps a target. If no target is available, taps the card itself
* `[cost:]untap [target]` - untaps a target. If no target is available, untaps the card itself
* `[cost:]moveTo(zone) [target]` - moves target card into specified zone. Allowed keywords for zone are those listed under [target zones](#target-zones).
* `[cost:][-]ability [target]` - gives ability to target. "ability" can be any of the keywords described in the ability section previously. For auras, instants and sorceries, cost is usually empty. For instants, sorceries, and activated abilities, the effect lasts until end of turn. For auras, it lasts as long as the aura is present. If a minus sign is used, this removes the ability from the target.
* `[cost:]n/m [target]` - gives `+n/+m` (`+n` power and `+m` toughness) to the target creature. n and m can be negative, or zero, or [variables](#auto-parsed-integers). For auras, instants and sorceries, cost is usually empty. For instants, sorceries, and activated abilities, the effect lasts until end of turn. For auras, it lasts as long as the aura is present.
* `[cost:]counter(p/t,n,name) [target]` - put n p/t counters on target creature. If n is negative, remove -n p/t counters from target creature. n defaults to 1. Name can be any name you can imagine acommon example counters with the name "Charge": counter(0/0,1,Charge).
* `[cost:]add{mana}` - Mana producer. Adds mana if the user pays cost. Cost can be emtpy, or contain only {t}, which is the case for basic lands.
* `[cost:]token(name,types, p/t,abilities colors)[*n]` - puts n tokens with name "name", types "types", power p, toughness t, abilities "abilities" and colors "colors" into play. Default value for n is 1. n may also be a [variable](#auto-parsed-integers). An example (Dragon Roost (10E)): `auto={5}{R}{R}:token(Dragon,creature dragon, 5/5,flying red)`. **Note:** If you have specified the token in any `_cards.dat` file as its own card (with [rarity](#rarity) T and [id](#id) i), then you can simplify the command to: `[cost:]token(id)[*n]`. It is recommended to use the negative id of the token-creating card as the id for the token, to prevent incompatibilities.
* `[cost:]cycling` - sends the card to the graveyard and draws a new one from the library
* `[cost:]fizzle [target]` - cancel a spell on the stack. The target zone defaults to stack.
* `[cost:]copy [target]` - become a copy of target (used for clone)
* `[cost:]becomes(types, p/t,abilities(??) colors(??))[target]` - target becomes a card of types "types", power p, toughness t, abilities "abilities" and colors "colors". p and t may be variables. Cannot be used for activated abilities yet. Note: This feature is still experimental and needs thorough testing.
* `[cost:]transforms(types or subtypes,color,ability)` - similar to becomes. The main difference is that you can change single parameters of a permanent like only color or type. -- (Card examples:Memnarch,Wild Mongrel)
* `[cost:]equip [target(??)]` - equips target with the card. target defaults to creature|myBattlefield. Can only be activated in the controller's main phases.
* `[cost:]attach [target(??)]` - same as equip, but can be used anytime its controller has priority.
* `protection from(target)` - gives the card protection from all cards that match the criteria specified in target. Cannot be used in activated abilities or sorceries/instants. Can be used as continuous effect (as a permanent ability of the card, or transferred via aura / lord / etc.)
* `cantbeblockedby(target)` - makes the card unblockable by creatures that match the criteria specified in target. Cannot be used in activated abilities or sorceries/instants. Can be used as continuous effect (as a permanent ability of the card, or transferred via aura / lord / etc.)
* `[cost:]maxCast(target)n` - sets the Maximum number of spells (defined by target) a player can cast (move to the stack) per turn
* `[cost:]maxPlay(target)n` - sets the Maximum number of cards (defined by target) a player can put into play (move to the battlefield) per turn. The difference with maxCast is that maxPlay can be used on cards that don't use the stack, such as lands.
* `[cost:]preventallcombatdamage [to(target1)] [from(target2)]` - no combat damage can be dealt from creatures matching the target2 specification to targets matching the target1 specification, for the duration of this effect. from() and to() are both optional and default to "anyone". This is a fairly recent feature, thorough testing is advised.
* `[cost:]fog` - This is similar to "preventallcombatdamage", which only worked for instants and sorceries. "Fog" works only for permanents and is used in combination with oneshot
* `[cost:]preventallnoncombatdamage [to(target1)] [from(target2)]` - Extension of preventallcombatdamage, prevents all noncombat damage [from a target to a target].
* `[cost:]preventalldamage [to(target1)] [from(target2)]` - Extension of preventallcombatdamage, prevents all damage (=combat damage + noncombat damage) [from a target to a target].
* `[cost:]prevent:x` - damage prevention to single creatures and players. (Card examples:Healing Salve,Battlefield Medic)
* `[cost:]rampage(p/t,n) [target]` - Whenever target creature gets blocked, it gets +p/+t for each blocker beyond the nth. n defaults to 1.
* `[cost:]draw:n [playertarget]` - draw n cards. n may be a variable.
* `[cost:]life:n [playertarget]` - gain n life. If n is negative, lose -n life. n may be a variable.
* `[cost:]deplete:n [playertarget]` - move n cards from the top of the library to the graveyard.
* `[cost:]discard:n [playertarget]` - randomly discard n cards
* `[cost:]shuffle [playertarget]` - shuffles the library of playertarget. Usually used in conjunction with other effect that affect the library without shuffling it by themselves. For example, "Pay {U}: Shuffle this card back into your library" can be coded as {U}:moveto(this|mylibrary) && shuffle
* `[cost:]lord(targets) effect [other]` - Continuously applies effect to all cards matching the target desciption. effect is any effect that would work in an "auto=" line. other is used if the ability should not be applied the lord card itself. (Example: Lord of Atlantis is lord(merfolk) islandwalk.)
* `[cost:]all(targets) effect [other]` - all() works identical to lord(), but guarantees that the action will occur only once. Whenever you have problems with lord(), try to use all() instead. Example: bury all(creature)
* `[cost:]foreach(targets) effect [other] [criterion]`
  - Applies effect as many times as the amount of cards matching the target description.
  - effect is any effect that would work in an "auto=" line.
  - other is used if the ability should not be applied the lord card itself.
  - criterion is an optional element consisting of a space character, then a "<" or ">" sign, and then a number n. If a "<n" criterion is specified, then effect is only applied if the number of matching targets m is smaller than n, and only n-m times. If a ">n" criterion is specified, then effect is only applied if the number of matching targets m is greater than n, and only m-n times. This is a fairly new feature, testing is advised.
  - **Example:** Keldon Warlord is `foreach([creature[-wall]|myinplay) 1/1`
  - **Example:** Black Vise uses `foreach(*|opponenthand) damage:1 opponent >4`
* `[cost:]aslongas(targets) effect [other] [criterion]`
  - Applies effect once, but only if there is at least one card matching the target description.
  - effect is any effect that would work in an "auto=" line.
  - other is used if the card itslf should not be counted as a match.
  - criterion is an optional element consisting of a space character, then a "<" or ">" sign, and then a number n. If a "<n" criterion is specified, then effect is only applied if the number of matching targets m is smaller than n. If a ">n" criterion is specified, then effect is only applied if the number of matching targets m is greater than n. This is a fairly new feature, testing is advised.
  - **Example:** Kird Ape uses `aslongas(forest|myinplay) 1/2`
* `[cost:]bloodthirst:n` - if an opponent was dealt damage this turn, this creature enters the battlefield with n +1/+1 counters on it.
* `[cost:]bushido(n/n)` - When this blocks or becomes blocked, it gets +n/+n until end of turn.
* `[cost:]land:n` - target player can play n additional lands this turn.
  - **Note:** Land:n does only work on instants and sorceries right now!
* `[{N}cost:]ninjutsu` - Return an unblocked attacker you control to hand: Put the card with ninjutsu onto the battlefield from your hand tapped and attacking.
  - **Note:** For a card with Ninjutsu, you basically need 2 things: {N} as cost which means return an unblocked attacker as a cost + the keyword ninjutsu.
  - **Example:** Higure, the Still Wind uses `{N}{2}{U}{U}:ninjutsu`
* `[cost:]twist [target]` - switches a creatures current power and toughness with each other.
* `removemana([*][cost]` - Removes mana from target player's manapool. if "*" is set, all mana matching the "cost" is removed. If * is set alone the manapool is emptied. If cost is set alone, this exact cost gets removed from the manapool
* `resetDamage` - Removes all damage received so far by a creature
* `loseAbilities` - Target Card loses all the abilities printed on it (*BETA, needs testing*)
* `loseSubtypesOf([#type|type])` - Target Card loses its subtypes of a given type. Valid values for the type are currently hardcoded. e.g: `loseSubtypesOf(land)`

## cost reduction

`altercost( [colorless | red | white | black | blue | green] , -x )`
* reduces the target card casting cost by x. Cannot be used in activated abilities or sorceries/instants. Can be used as continuous effect (as a permanent ability of the card, or transferred via aura / lord / etc.)

**Example:** Edgewalker
```
auto=lord(cleric|myhand) altercost( white, -1)
auto=lord(cleric|myhand) altercost( black, -1)
```

The follow 4 lines have to be added in order to reset the cost if the coded permanent leaves play.

* `autoexile=all(*|myhand) resetcost`
* `autograveyard=all(*|myhand) resetcost`
* `autohand=all(*|myhand) resetcost`
* `autolibrary=all(*|myhand) resetcost`

## cost increase

`altercost( [colorless | red | white | black | blue | green] , +x )`
* increases the target card casting cost by x. Cannot be used in activated abilities or sorceries/instants. Can be used as continuous effect (as a permanent ability of the card, or transferred via aura / lord / etc.).

**Example: chill** 
```
auto=lord(*[red]|myhand) altercost( colorless, +2 )"
auto=lord(*[red]|opponenthand) altercost( colorless, +2 ) )
```

Likewise, the follow 4 lines have to be added in order to reset the cost if the coded permanent leaves play.

* `autoexile=all(*|myhand) resetcost`
* `autograveyard=all(*|myhand) resetcost`
* `autohand=all(*|myhand) resetcost`
* `autolibrary=all(*|myhand) resetcost`

## Misc
* `name(...) && effect` - When used in an autoline it replaces an autoline's "ability" text with a custom ability name.
* deprecated keywords:
  - `[cost:]nocreatures` - target player cant play creatures this turn. (Please use maxCast instead)
  - `[cost:]nospells` - target player cant play spells this turn. (Please use maxCast instead) 
  - `[cost:]onlyonecast` - target player can only play 1 spell this turn (Please use maxCast instead)

## multiple effects

You can (and often need to) specify multiple effects for a card. You can code alternative [competing effects](#competing-effects), or you can code [multiple effects for one cost](#multiple-effects-for-one-cost).

### **competing effects**
If several `auto` effects compete with each other (i.e. if the game cannot determine automatically which action the player wants to take), then the game will automatically create a "choice" menu.

Effects compete with each other if they have a cost and if that cost can be paid in the given situation. Having more than one effect with a [may condition](#may) active will cause those to compete as well(??). Effects without a cost never compete, instead the engine simply executes all of them.

**Examples:**

The following two effects (from a dual land) will compete. When the land is tapped for mana, the game will open a menu, which lets the player choose between the two effects:
```
auto={T}:add{B}
auto={T}:add{G}
```

The following two effects would also compete, since the cost for both could be paid, so the game cannot know which effect the player wants to activate:
```
auto={G}:damage:1 target(goblin)
auto={G}:untap target(elf)
```

The following two effects would compete if the player has green mana in his manapool when selecting the card (because then both effects could be paid for, but *not* if he has no green mana available (in that case there's only one effect available, and the game will execute this one automatically):
```
auto={1}:tap target(artifact)
auto={G}:untap target(elf)
```

The following two effects would *not* compete because they have no cost - both of them would be executed (although this would make the card a bit silly). The effects contradict each other, but that does not cause them to compete. Effects only compete under the conditions described above.
```
auto=1/1
auto=-1/-1
```

### **multiple effects for one cost**

You may want one cost to activate several actions. This is done by putting the keyword `&&` between two effects.

**Examples:**
|                                                           |                                                               |
|-----------------------------------------------------------|---------------------------------------------------------------|
|`auto={T}:add{W} && damage:1 controller`                   | *Tap this card: You gain 1 white mana and receive 1 damage.*  |
|`auto={2}:bury target(ooze) && regenerate all(treefolk)`   | *Pay 2 mana: bury target ooze, and regenerate all treefolk.*  |

**Limitations:**

* The `&&` keyword is only available for activated abilities, i.e. for `auto=` rules that actually do have a [cost](#auto-cost) segment.
* You can only specify one explicit target (i.e. a target that requires a choice by the player) per auto= line. So, a rule like "Tap: Bury target elf, then bury target goblin" cannot be coded yet - you would need code like bury target(elf) && bury target(goblin), and you cannot have two "target()" specifications in the same auto= line.
* The engine cannot easily handle auto= lines which combine an effect on a target with an effect on the card itself. This happens because the usual way to tell an engine to apply an effect to the card itself is to simply not specify a target, like this: `{2}:flying`. If you add a targeted effect to this line, like this: `{2}:flying && -1/-1 target(wall)`, then the "no target specified" condition isn't fulfilled anymore, and the game will give the `flying` ability to the wall. You can get around this problem by specifying the card itself as a target for the first effect (but without using a [target()](#auto-target) command), like this: `{2}:flying all(this) && -1/-1 target(wall)`.
* The above workaround might work with all(mytgt) too if required, but this has not been tested yet. (There's a risk that "mytgt" and target() confuse each other.)
* The sequence of actions is not always from left to right. The game requires you to specify your target before any effects are executed, even when the target specification is the last element in your auto= line. This is according to the rules - the target needs to be specified when the ability is put on the stack, not when it resolves.

## auto target

**target** defines a group of cards that share some characteristics (for example, all black or artifact creatures on the battlefield). Wagic uses such targets in four ways:

1. If the target is part of another command, then it defines a group of cards that this command operates on. For example, in lord(elf|mybattlefield) 1/1, the target elf|mybattlefield defines a group of cards (all elves on the player's battlefield) that the command operates upon (i.e. every card belonging to this group will receive a 1/1 bonus). In the syntax definitions of this reference, such targets are always enclosed in parantheses and attached directly to the command to which they belong. For example, the target for lord commands is written like this: `lord(targets) effect`
2. If the target is not part of another command, then it defines a group of cards from which the player needs to choose one (or more). For example, in bury target(goblin|mybattlefield), the target goblin|mybattlefield defines a group of cards (goblins on the player's battlefield), and the player must then choose one of those, which will be buried. In the syntax definitions of this reference, such targets are never enclosed in parantheses and are preceded by a space. For example, the target for bury commands is written like this: `bury [target]`. In card code, such targets explicitly need the term "target()" written out, they are coded like this: `target(goblin|mybattlefield)`
3. As an alternative to the notation in the paragraph above, Wagic also allows you to define targets with the keyword "notatarget()", e.g. `copy notatarget(goblin|mybattlefield)`. The only difference between target() and notatarget() is that target() excludes cards which cannot be legally targeted (e.b. because they have the "shroud" ability), while notatarget() includes those. This is useful for cards like Clone, which allow the player to choose a card without technically targeting it.
4. A fourth usage of "target" is the way how targets appear in a `target=` line. This line always defines targets of the second kind, but does not need the term "target" or parantheses around it. Example: `target=zombie|graveyard`.

Which values a target can have and how they are specified is explained in the [target](#target) section.

If an `auto=` line requires a target, but none is specified, and a `target=` line is present, then the value of that target= line will be used as target for the `auto=` line. This is how instants and sorceries define their targets and effects.

## auto playertarget

For some effects, you can specify a special kind of target: a playertarget. This is an optional value for effects that indirectly target a player. For example, if an effect specifies to draw cards, then the playertarget defines *which* player does that.

Valid values for playertarget are:

* *controller* - the player who controls the card that has this rule
* *opponent* - the player who does not control the card that has this rule
* *targetcontroller* - the player who controls the target of this auto= rule

When `playertarget` is not specified, Wagic tries to use the most logical value for it: if a `target=` line is specified for an effect, Wagic will use this target if it's a player, or the target's controller if it is a card.

## auto restrictions
For activated abilities (i.e. auto rules with a cost segment) you can restrict when or how often they can be activated or how long they last. The following keywords are available:

phase restrictions - allow the effect to be activated only at certain times
limit - limits how often the ability can be activated per turn
ueot - "until end of turn", forces the effect to end when the turn ends

### **phase restrictions**

**Description:** phase restrictions allow the effect to be activated only at certain times. The following keywords are available:

* `myturnonly` - only usable in the card controller's turn
* `assorcery` - only usable when a sorcery could be played, i.e. in the card controller's first and second main phase
* `my[phasename]only` - only usable in the respective phase in the card controller's turn. `[phasename]` is written without the square brackets and can be any phase name. You can see a list of available phase names in the section for [phase-based triggers](#phase-based-triggers)

**Examples:**

|                           |                                                                                           |
|---------------------------|-------------------------------------------------------------------------------------------|
| `{T}:add{B} asSorcery`    | *Tap this card: Add one black mana. Use this ability only when you could play a sorcery.* |
| `{3}:untap myupkeeponly`  | *In your upkeep phase, you may pay 3 mana to untap this card.*                            |

**Limitations:**

* These restrictions were designed for activated abilities so they probably won't work for continuous effects. For a creature that gets +2/+2 in its controller's turn, `2/2 myturnonly` will probably not work. However, `@each my draw:2/2 ueot` might. (??)
* You can't (yet) restrict activation to other combinations of phases, like *"only during combat"* or *"only before attachers have been declared"*.

### **limit**

**Description:** limit limits how often the ability can be activated per turn

**Example:**
|                      |                                                                                                                |
|----------------------|----------------------------------------------------------------------------------------------------------------|
| `{B}:add{W} limit:2` | *activating this card lets the player turn 1 black mana into 1 white mana, but no more than 2 times per turn*  |

### **ueot**

**Description:** ueot (meaning *"until end of turn"*), forces the effect to end when the turn ends.

Usually, Wagic guesses whether an effect is meant to last until end of turn, or indefinitely. For example, it assumes that any activated abilities only last until end of turn, and that the effects of enchantments last as long as the enchantment is in play. This guess is correct in the vast majority of cases.

Sometimes, however, the guess may be wrong, and you may want to explicitly tell Wagic that an effect is only meant to least until end of turn. In this case, use the `ueot` keyword.

**Example:**

An Enchantment that gives all cats +1/+1 (as long as it is in play), and also makes those cats unblockable for the turn in which it entered the battlefield, can be coded like this:
```
auto=lord(cat) 1/1
auto=lord(cat) unblockable ueot
```

## auto parsed integers

Many [effects](#auto_effects) in [auto](#auto) rules have numbers as parameters: how many cards to draw, how much life to give, how much power to add, etc. Usually, you simply write a fixed number into the rule. In many places, you can also use a variable instead of a fixed number. The following variables are available:

|                       |                                                                       |
|-----------------------|-----------------------------------------------------------------------|
| `X`                   | *the amount of mana paid for the "X" of the spell cost of this card*  |
| `p`                   | *the power of this card's target*                                     |
| `t`                   | *the toughness of this card's target*                                 |
| `manacost`            | *the converted manacost of this card's target*                        |
| `lifetotal`           |                                                                       |
| `opponentlifetotal`   |                                                                       |


**Examples:**

|                                                       |                                                                                               |
|-------------------------------------------------------|-----------------------------------------------------------------------------------------------|
| `auto=draw:X controller`                              | *draw one card for each mana paid for the "X" cost of this spell*                             |
| `auto=bury target(goblin) && life:p targetcontroller` | *bury target goblin, and give its controller an amount of life equal to that goblin's power*  |
| `auto={T}:0/manacost target(elf)`                     | *target elf gains an amount of toughness equal to his converted manacost*                     |


**Limitations:**

* Some [effects](#auto-effects) can use this functionality, others can't. Thorough testing is advised.
* For at least the following effects, this feature has been implemented: becomes, damage, draw, life, power/toughness modifiers (excluding counters), token (for the number of tokens)

# auto macros

With time, the Wagic syntax for auto lines has evolved from a very simple series of keywords to a convoluted system, close to a programming language, with lots of exceptions and tribal knowledge. In an attempt to simplify this, we introduced auto macros, a system that allows to "summarize" a complex auto line into a simple keyword, actually a function accepting parameters. This is useful especially if you plan to code dozens of cards that share a complex ability.

Let's for example look at the code for cycling (as of 2011/11/12): `autohand={2}{cycle}:name(cycling) draw:1`. It is not super long to type, but ideally we would prefer to type `cycling({2})` or something similar. The macros system allows to simplify things by writing the complex ability once, and associating it to a keyword.

Here's and example: `#AUTO_DEFINE __CYCLING__($cost) $cost{cycle}:name(cycling) draw:1`

The syntax is simple: `#AUTO_DEFINE [function name and parameters] [actual ability code]`.

In our example, we define the keyword` __CYCLING__` which takes a variable `$cost`. We then tell the engine to "rewrite" that keyword and parameter into `$cost{cycle}:name(cycling) draw:1` (note the `$cost` variable in there).

This then allows card coders to write:

`autohand=__CYCLING__({2})`

instead of

`autohand={2}{cycle}:name(cycling) draw:1`

**Important Notes**

* The keyword used for a macro cannot contain any space
it is recommended to prefix parameters for the macros with the symbol "$", to avoid any weird replacement
* Macros are global. Wherever you define them in your card files, they will apply everywhere
  - We suggest to write all the macros in a single text file, inside the sets/primitives folder
* Macros are simply "replaced" with the code they describe, through a simple text replacement algorithm.
  - For this reason, it is important to use unique macro names that cannot "conflict". For example, having a macro named "MACRO" and another one named "MACRO2" is a bad idea, because the string "MACRO" exists in the string "MACRO2". As a convention, we start and end all our macro names by `__`, and we guarantee that we don't use `__` inside of the macro name anyplace else.

## autohand

**Description:** `autohand` is a recently introduced key that allows you to specify effects which can be activated while the card is in your hand.

Regular `auto=` rules are only active when the card is on the battlefield, they are ignored if the card is in any other zone. However, you may want to code effects which can be activated from the hand (without actually playing them), for example to code Cycling effects. For such effects, `autohand=` has been introduced.

**Example:** `autohand={2}:cycling`

**Limitations:** Currently, `autohand=` only works for activated abilities (i.e. rules with a cost segment). Continuous effects or triggers cannot be coded in `autohand=` lines.

***This is a new feature, thorough testing is advised.***

## autograveyard

**Description:** `autograveyard` is a key that allows you to specify effects which can be activated while the card is in your graveyard.

**Example:** `autograveyard={B}{B}:moveTo(myBattlefield)`

**Limitations:** Currently, `autograveyard=` only works for activated abilities (i.e. rules with a cost segment). Continuous effects or triggers cannot be coded in `autohand=` lines.

## alias

**Description:** `alias` is used to access existing hardcoded routines which were originally coded for other cards. Its main use is to create a new card that has the same hardcoded effects as an existing one.

For example, the effect of Control Magic* (id 1194) currently cannot be coded with any `auto=` rule. Therefore, the card's functionality was hardcoded, i.e. directly programmed into the engine. The engine knows that whenever a card with the id 1194 is played, it needs to execute this hardcoded effect.

There's another card called *Persuasion* which has exactly the same effect as *Control Magic*. So we want to make *Persuasion* behave just like *Control Magic*. Usually we could just copy any rules and effects over from *Control Magic's* card data to make *Persuasion* work, but in this case there's nothing to copy, since the code is stored in the engine, not in the card's data. So, to make *Persuasion* work, we have to tell it that it needs to access the hardcoded functions that were meant for the card with id 1194 (*Control Magic*).

This is exactly what the `alias=` key does. So to make *Persuasion* work, we would add the line `alias=1194` to its code. Note that we still need to specify all other data for Persuasion (type, mana, even the target) - the `alias` *only* makes the *Persuasion* access a hardcoded function, it copies no information over from the aliased card.

**Limitations:** Use alias with care. The only time you can be 100% sure that it's correct to use alias is when the card is a reprint of an existing card, or functionally identical.

The `alias=` key has led to confusion and may be removed from the parser in the future.

# Final Words
Now, the best piece of advice we can give you is to look at the existing _cards.dat files and get inspiration from there. If you have question or run into problems, ask for help in our [Cards/Sets creation forum](http://web.archive.org/web/20120129123445/http://wololo.net/forum/viewforum.php?f=8).

If you're planning to code many cards, then you will appreciate a comfortable way of testing their functions. Check the [Test Suite Reference](http://web.archive.org/web/20120129123445/http://wololo.net/miki/index.php/Wagic/TestSuite) for that.

If you want to dig deeper, you can have a look at TargetChooser.cpp and MTGAbility.cpp in the source code. See our [Compilation Guide](http://web.archive.org/web/20120129123445/http://wololo.net/miki/index.php/Wagic/Compiling) for help on obtaining Wagic's source code and compiling it.

Good luck!

Retrieved from [http://wololo.net/miki/index.php/Wagic/CardCode](http://web.archive.org/web/20120129123445/http://wololo.net/miki/index.php/Wagic/CardCode)
