/*	SCCS Id: @(#)role.c	3.4	2003/01/08	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985-1999. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


/*** Table of all roles ***/
/* According to AD&D, HD for some classes (ex. Wizard) should be smaller
 * (4-sided for wizards).  But this is not AD&D, and using the AD&D
 * rule here produces an unplayable character.  Thus I have used a minimum
 * of an 10-sided hit die for everything.  Another AD&D change: wizards get
 * a minimum strength of 4 since without one you can't teleport or cast
 * spells. --KAA
 *
 * As the wizard has been updated (wizard patch 5 jun '96) their HD can be
 * brought closer into line with AD&D. This forces wizards to use magic more
 * and distance themselves from their attackers. --LSZ
 *
 * With the introduction of races, some hit points and energy
 * has been reallocated for each race.  The values assigned
 * to the roles has been reduced by the amount allocated to
 * humans.  --KMH
 *
 * God names use a leading underscore to flag goddesses.
 */

/* thanks to CK for pointing out some stat distribution bugs --Amy */

const struct Role roles[] = {
{	{"Acid Mage", 0}, {
	{"Oozer",         0},
	{"Slimer",       0},
	{"Sludge Mage",       0},
	{"Spiller",     0},
	{"Corroder",       0},
	{"Acidsplasher",      0},
	{"Dissolver",   0},
	{"Dissolver", 0},
	{"Acid-Master",  0} },
	"Tartarica", "Acetica", "Hydrocloria", /* Special - Based on chemical acids */
	"Aci", "Slime Pit", "Electric Power Station",
	PM_ACID_MAGE, NON_PM, PM_ACID_HOUND_PUP,
	PM_ACIDSNORT, PM_ATTENDANT, PM_LIGHTNINGROD,
	PM_DROW, PM_TITAN, S_ANGEL, S_GIANT,
#if 0
	ART_ACIDFALL, ART_ACID_BRAND,
#endif
	ART_FLUTE_OF_SLIME,
	MRACE_HUMAN|MRACE_ELF|MRACE_MFLAY|MRACE_ORC | 
	ROLE_MALE|ROLE_FEMALE|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_ACID_STREAM,        -4 /* From old role.c */
},

{	{"Archeologist", 0}, {
	{"Digger",      0},
	{"Field Worker",0},
	{"Investigator",0},
	{"Exhumer",     0},
	{"Excavator",   0},
	{"Spelunker",   0},
	{"Speleologist",0},
	{"Collector",   0},
	{"Curator",     0} },
	"Quetzalcoatl", "Camaxtli", "Huhetotl", /* Central American */
	"Arc", "the College of Archeology", "the Tomb of the Toltec Kings",
	PM_ARCHEOLOGIST, NON_PM, NON_PM,
	PM_LORD_CARNARVON, PM_STUDENT, PM_MINION_OF_HUHETOTL,
	NON_PM, PM_HUMAN_MUMMY, S_SNAKE, S_MUMMY,
#if 0
	ART_WEREBANE, ART_GRAYSWANDIR,
#endif
	ART_ORB_OF_DETECTION,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_DWARF|MRACE_HOBBIT|MRACE_CLOCK | 
	  ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10, 10,  7,  7,  7 },
	{  20, 20, 20, 10, 20, 10 }, /* This must add up to 100 or it won't work right (attrib.c uses this code). Again, thanks to CK for pointing it out. --Amy */
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 10, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_MAGIC_MAPPING,   -4
},

{	{"Barbarian", 0}, {
	{"Plunderer",   "Plunderess"},
	{"Pillager",    0},
	{"Bandit",      0},
	{"Brigand",     0},
	{"Raider",      0},
	{"Reaver",      0},
	{"Slayer",      0},
	{"Chieftain",   "Chieftainess"},
	{"Conqueror",   "Conqueress"} },
	"Mitra", "Crom", "Set", /* Hyborian */
	"Bar", "the Camp of the Duali Tribe", "the Duali Oasis",
	PM_BARBARIAN, NON_PM, NON_PM,
	PM_PELIAS, PM_CHIEFTAIN, PM_THOTH_AMON,
	PM_OGRE, PM_TROLL, S_OGRE, S_TROLL,
#if 0
	ART_CLEAVER, ART_DEATHSWORD,
#endif
	ART_HEART_OF_AHRIMAN,
	MRACE_HUMAN|MRACE_ELF|MRACE_ORC|MRACE_WERE|MRACE_VAMPIRE|MRACE_GHOUL|MRACE_OGRE|MRACE_GIANT|MRACE_TROLL | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  16,  7,  7, 15, 16,  6 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 20, 0,  0,10,  2, 0 },	/* Hit points */
	{  5, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 14, 0, 0,  8, A_INT, SPE_HASTE_SELF,      -4
},

{	{"Bard", 0}, {
	{"Rhymer",      0},
	{"Lyrist",      0},
	{"Sonneteer",   0},
	{"Jongleur",    0},
	{"Troubadour",  0},
	{"Minstrel",    0},
	{"Lorist",      0},
	{"Well-known Bard",        0},
	{"Master Bard", 0} },
	"_Minerva", "Apollo", "Linus", /* classical gods of music */
	"Brd", "the Conservatorium", "the Island of Anthemoessa",
	PM_BARD, NON_PM, NON_PM,
	PM_PINDAR, PM_RHYMER, PM_AGLAOPE,
	PM_SNAKE, PM_WHITE_UNICORN, S_SNAKE, S_UNICORN,
#if 0
	ART_SHIMMERSTAFF, ART_SILERSHARD,
#endif
	ART_LYRE_OF_ORPHEUS,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_ORC|MRACE_CLOCK | 
	ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7, 10,  6, 10 },
	{  10, 10, 15, 25, 10, 30 },
	/* Init   Lower  Higher */
	{ 11, 0,  0, 8,  1, 0 },	/* Hit points */
	{  4, 3,  0, 1,  0, 2 },10,	/* Energy */
	10, 3,-3, 2, 9, A_CHA, SPE_SLEEP, -4
},

{	{"Caveman", "Cavewoman"}, {
	{"Troglodyte",  0},
	{"Aborigine",   0},
	{"Wanderer",    0},
	{"Vagrant",     0},
	{"Wayfarer",    0},
	{"Roamer",      0},
	{"Nomad",       0},
	{"Rover",       0},
	{"Pioneer",     0} },
	"Anu", "_Ishtar", "Anshar", /* Babylonian */
	"Cav", "the Deep Jungle", "the Dragon's Lair",
	PM_CAVEMAN, PM_CAVEWOMAN, PM_LITTLE_DOG,
	PM_SHAMAN_KARNOV, PM_NEANDERTHAL, PM_CHROMATIC_DRAGON,
	PM_TIGER, PM_APE, S_SNAKE, S_FELINE,
#if 0
	ART_GIANTKILLER, ART_SKULLCRUSHER,
#endif
	ART_SCEPTRE_OF_MIGHT,
	MRACE_HUMAN|MRACE_DWARF|MRACE_GNOME|MRACE_VAMPIRE|MRACE_KOBOLD|MRACE_GIANT | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7,  8,  6 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  2, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 12, 0, 1,  8, A_INT, SPE_DIG,             -4
},

{	{"Chef", 0}, { /* Level names based on Brigade de cuisine */
	{"Plongeur",  0}, /* Dishwasher (washes dishes) */
	{"Busser", 0}, /* Busboy (clears dishes and table, sets tables, etc)*/
	{"Expediter", 0}, /* Takes orders, puts finishing touches on dishes */
	{"Apprenti", 0}, /* Chef/cook in training */
	{"Commis",0}, /* Junior Cook */
	{"Cuisinier", 0}, /* Cook */
	{"Chef de Partie", 0}, /* Station Chef */
	{"Sous Chef", 0}, /* Sous-chef de cuisine - Second in Command */
	{"Chef de Cuisine", 0} }, /* Kitchen Chef - Head of all */
	"_Demeter", "_Hestia", "_Adephagia", /* Greek */
	"Chf", "The Iron Kitchen of Gourmet", "Hell's Kitchen", /* TODO: Quest stuff */
	PM_CHEF, NON_PM, NON_PM,
	PM_SUPREME_CHEF, PM_KITCHEN_PORTER, PM_THE_IMMORTAL_ROT, /* TODO: Quest leader/nemesis */
	PM_GIANT_BEETLE, PM_BLOODWORM, S_ANT, S_WORM,
#if 0
	ART_TENDERIZER, ART_THE_GAMBLER_S_SUIT,
#endif
	ART_DELUXE_YENDORIAN_KNIFE,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_KOBOLD|MRACE_NYMPH | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */  
	{   7, 10,  6,  7,  7, 10 },
	{  15, 10, 10, 15, 30, 20 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  0, 0 },	/* Hit points */
	{ 12, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_DETECT_FOOD,   -4
},


#ifdef CONVICT
{	{"Convict", 0}, {
	{"Detainee",     0},
	{"Inmate",   0},
	{"Jail-bird",0},
	{"Prisoner",0},
	{"Outlaw",    0},
	{"Crook",   0},
	{"Desperado",     0},
	{"Felon",    0},
	{"Fugitive",  0} },
	"Ilmater", "Grumbar", "_Tymora",	/* Faerunian */
	"Con", "Castle Waterdeep Dungeon", "the Warden's Level",
	PM_CONVICT, NON_PM, PM_SEWER_RAT,
	PM_ROBERT_THE_LIFER, PM_INMATE, PM_WARDEN_ARIANNA,
	PM_GIANT_BEETLE, PM_SOLDIER_ANT, S_RODENT, S_SPIDER,
#if 0
	ART_SHADOWBLADE, ART_SPINESEEKER,
#endif
	ART_IRON_BALL_OF_LIBERATION,
	/* absolutely everyone */
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|
	MRACE_VAMPIRE|MRACE_WERE|MRACE_ORC|MRACE_CLOCK|MRACE_GHOUL|
	MRACE_GIANT|MRACE_MFLAY|MRACE_KOBOLD|MRACE_NYMPH|MRACE_OGRE|
	MRACE_TROLL |
	ROLE_MALE|ROLE_FEMALE| ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7, 13,  6 },
	{  20, 20, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	-20, 5, 0, 2, 10, A_INT, SPE_TELEPORT_AWAY,   -4
},
#endif	/* CONVICT */

{	{"Corsair", 0}, {
	{"Lubber",  0},
	{"Swabby",        	0},
	{"Deckhand",   0},
	{"Bilge Rat",      		0},
	{"Helmsman",     "Helmswoman"},
	{"Navigator",      0},
	{"Bosun",			0},
	{"Mate",   	0},
	{"Commodore",  	0} },
	"Erzulie Freda", "Marassa Jumeaux", "Papa Legba",	/* Haitian Vodou */
	"Cor", "Queen Anne's Revenge", "Grotto of Souls",
	PM_CORSAIR, NON_PM, NON_PM,
	PM_BLACKBEARD, PM_PIRATE_BROTHER, PM_SPEARATHAN,
	PM_GIANT_EEL, PM_SKELETAL_PIRATE, S_EEL, S_ZOMBIE,
#if 0
	ART_QUEEN_S_GUARD, ART_SWORD_OF_BALANCE
#endif
	ART_PEARL_OF_WISDOM,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   8,  7, 7,  8,  8,  7 },
	{  20, 15, 10, 20, 25, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_CAUSE_FEAR,    -4
},

{	{"Diver", 0}, {
	{"Beachcomber",  0},
	{"Paddler",        	0},
	{"Snorkeler",   0},
	{"Swimmer",      		0},
	{"Wave-rider",     0},
	{"Cruiser",      0},
	{"Surfer",			0},
	{"Spearfisher",   	0},
	{"Torpedo",  	0} },
	"Tane", "Tangaroa", "Whiro",	/* Polynesian */
	"Div", "Scuba Dock", "Melville's Sea of Doom",
	PM_DIVER, NON_PM, NON_PM,
	PM_JACQUES_COUSTEAU, PM_ATTENDANT, PM_MOBY_DICK,
	PM_ELECTRIC_EEL, PM_GIANT_CRAB, S_EEL, S_EEL,
#if 0
	ART_POSEIDON_S_TREASURE, ART_FROSTBITE,
#endif
	ART_MAUI_S_FISHHOOK,
	MRACE_HUMAN|MRACE_ELF|MRACE_GIANT|MRACE_HOBBIT|MRACE_TROLL |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   8,  7, 7,  8,  8,  7 },
	{  20, 15, 10, 20, 25, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_DETECT_MONSTERS,    -4
},

{	{"Drunk", 0}, {
	{"Wino",     0},
	{"Boozer",   0},
	{"Drinker",0},
	{"Alcoholic",0},
	{"Slosh",    0},
	{"Partier",   0},
	{"Tastetester",     0},
	{"Winetaster",    0},
	{"Socialite",  0} },
	"Fufluns", "Dionysus", "Pan",	/* Classical gods of Wine */
	"Dru", "Brass Lantern Bar", "Moriarty's Saloon",
	PM_DRUNK, NON_PM, NON_PM,
	PM_ANDY_STAHL, PM_ATTENDANT, PM_COLIN_MORIARTY,
	PM_GNOME_WARRIOR, PM_DWARF_KING, S_HUMAN, S_GNOME,
#if 0
	ART_CROWN_ROYAL_CLOAK, ART_DIRK,
#endif
	ART_GOURD_OF_INFINITY,
	MRACE_HUMAN|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|
	MRACE_NYMPH|MRACE_KOBOLD | 
	ROLE_MALE|ROLE_FEMALE|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  6,  6, 14, 10 },
	{  15, 15,  7, 15, 30, 15 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 5, 0, 2, 10, A_INT, SPE_DETECT_FOOD,   -4
},

{	{"Electric Mage", 0}, {
	{"Charger",         0},
	{"Bolter",       0},
	{"Buzzer",       0},
	{"Hummer",     0},
	{"Energic",       0},
	{"Arclite",      0},
	{"Volt Technician",   0},
	{"Thundermage", 0},
	{"Shock-Master",  0} },
	"Static", "Magnet", "Lightning", /* Special */
	"Ele", "Bolttown", "Celebrity Graveyard",
	PM_ELECTRIC_MAGE, NON_PM, PM_SHOCK_HOUND_PUP,
	PM_GUITAR_HERO, PM_SHOCKER, PM_ELVIS_PRESLEY,
	PM_SHOGGOTH, PM_BURBLING_BLOB, S_PUDDING, S_BLOB,
#if 0
	ART_DEBUGGER, ART_SHOCK_BRAND,
#endif
	ART_HARP_OF_LIGHTNING,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_MFLAY|MRACE_CLOCK|MRACE_NYMPH | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_LIGHTNING,        -4 /* From old role.c */
},

{	{"Firefighter", 0}, {
	{"Water Boy",         "Water Girl"},
	{"Fire Cadet",       0},
	{"Rookie",       0},
	{"Extinguisher",     0},
	{"Smokeeater",       0},
	{"Fire Captain",      0},
	{"Platoon Chief",   0},
	{"Division Commander", 0},
	{"Fire Chief",  0} },
	"Prometheus", "Thor", "Arson", /* Fire */
	"Fir", "Fire-Brigade Base", "Smokey Forest",
	PM_FIREFIGHTER, NON_PM, PM_LITTLE_DOG,
	PM_SMOKEY, PM_ATTENDANT, PM_EDDIE_THE_PYRO,
	PM_FIRE_ELEMENTAL, PM_SALAMANDER, S_ELEMENTAL, S_LIZARD,
#if 0
	ART_FROSTBITE, ART_DIRK
#endif
	ART_FIRE_CHIEF_HELMET,
	MRACE_HUMAN|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|MRACE_GIANT | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	{  13,  7, 10,  8, 12, 12 },
	{  20, 15, 15, 10, 20, 20 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_TELEPORT_AWAY,        -4 /* From old role.c */
},

{	{"Flame Mage", 0}, {
	{"Spark",         0},   /* WAC was Igniter */
	{"Igniter",       0},
	{"Broiler",       0},   /* WAC was Igniter */
	{"Combuster",     0},   /* WAC was Torcher */
	{"Torcher",       0},
	{"Scorcher",      0},   /* WAC was Torcher */
	{"Incinerator",   0},
	{"Disintegrator", 0},   /* WAC was Incinerator */
	{"Flame-Master",  0} },
	"Earth", "Fire", "Ash", /* Special */
	"Fla", "the great Circle of Flame", "the Water Mage's Cave",
	PM_FLAME_MAGE, NON_PM, PM_HELL_HOUND_PUP,
	PM_HIGH_FLAME_MAGE, PM_IGNITER, PM_WATER_MAGE,
	PM_WATER_ELEMENTAL, PM_RUST_MONSTER, S_ELEMENTAL, S_RUSTMONST,
#if 0
	ART_FIREWALL, ART_FIRE_BRAND,
#endif
	ART_CANDLE_OF_ETERNAL_FLAME,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|MRACE_ORC|MRACE_GIANT|MRACE_MFLAY | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_FIREBALL,        -4 /* From old role.c */
},

{	{"Geek", 0}, {
	{"Dork",    0},
	{"Nerd",    0},
	{"Hobbyist",    0},
	{"Tester",    0},
	{"Hacker",    0},
	{"Programmer",    0},
	{"Project Leader",    0},
	{"Senior Developer",    0},
	{"Kernel Maintainer",    0} }, 
	"Jobs", "Wozniak", "Gates", /* geek */
	"Gee", "university campus", "development hell",
	PM_GEEK, NON_PM, NON_PM,
	PM_UPPER_GRADUATE, PM_HACKER, PM_STUDENT_LEADER,
	PM_BUG, PM_HEISENBUG, S_XAN, S_PUDDING,
#if 0
	ART_TESLA_S_COAT, ART_DIRK,
#endif
	ART_MASTER_BOOT_DISK,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT | ROLE_MALE|ROLE_FEMALE |
	ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 14,  6,  7,  7,  5 },
	{  10, 20, 10, 10, 30, 10 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_HALLUCINATION,   -4
},

{	{"Gladiator", 0}, {
	{"Velite",  0},
	{"Thraex",        	0},
	{"Dimachaerus",   0},
	{"Venator",      		0},
	{"Murmillo",     0},
	{"Hoplomachus",      0},
	{"Retiarius",			0},
	{"Secutor",   	0},
	{"Centurion",  	0} }, /* sorry Fyr, but Champion was just way too generic of a title. --Amy */
	"Sulla", "Cicero", "Catilina",	/* Ancient Rome */
	"Gla", "Arena Bloodworks", "Colosseum",
	PM_GLADIATOR, NON_PM, NON_PM,
	PM_TUTOR, PM_ATTENDANT, PM_THE_CHAMPION_OF_POMPEJI,
	PM_WEREPANTHER, PM_MINOTAUR, S_HUMAN, S_QUADRUPED,
#if 0
	ART_GLADIUS, ART_DIRK,
#endif
	ART_IMPERIAL_TOKEN,
	MRACE_HUMAN|MRACE_DWARF|MRACE_ORC|MRACE_OGRE|MRACE_GIANT |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   8,  7, 7,  8,  8,  7 },
	{  20, 15, 10, 20, 25, 10 },
	/* Init   Lower  Higher */
	{ 12, 0,  0, 8,  4, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_CAUSE_FEAR,    -4
},

{	{"Hacker", 0}, {
	{"Newbie",    0},
	{"BASIC Programmer",        "BASIC Programmeress"},
	{"C Programmer",        "C Programmeress"},
	{"Hacker",        "Hackeress"},
	{"NetHacker", "NetHackeress"},
	{"Nethack Programmer",      "Nethack Programmeress"},
	{"he who uses",       "she who uses"},
	{"he who knows",      "she who knows"},
	{"he who learns",     "she who learns"} },
	"UNIX", "the PDP-7", "VMS", /* Computerian */
	"Hac", "the Development Team's location", "the Microsoft headquarter",
	PM_HACKER, NON_PM, NON_PM,
	PM_THE_DEV_TEAM, PM_HACKER, PM_BILL_GATES,
	PM_BUG, PM_HEISENBUG, S_XAN, S_PUDDING,
#if 0
	ART_SHOCKER, ART_DEBUGGER,
#endif
	ART_NETHACK_SOURCES,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT | ROLE_MALE|ROLE_FEMALE |
	ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10, 10, 10,  8,  9, 7 },
	{  30, 15, 15, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_SICKNESS,   -4
},

{	{"Healer", 0}, {
	{"Rhizotomist",    0},
	{"Empiric",        0},
	{"Embalmer",       0},
	{"Dresser",        0},
	{"Medicus ossium", "Medica ossium"},
	{"Herbalist",      0},
	{"Magister",       "Magistra"},
	{"Physician",      0},
	{"Chirurgeon",     0} },
	"_Athena", "Hermes", "Poseidon", /* Greek */
	"Hea", "the Temple of Epidaurus", "the Temple of Coeus",
	PM_HEALER, NON_PM, NON_PM,
	PM_HIPPOCRATES, PM_ATTENDANT, PM_CYCLOPS,
	PM_GIANT_RAT, PM_SNAKE, S_RODENT, S_YETI,
#if 0
	ART_DELUDER, ART_MIRRORBRIGHT,
#endif
	ART_STAFF_OF_AESCULAPIUS,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_NYMPH |
	ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 13,  7, 11, 16 },
	{  15, 20, 20, 15, 25, 5 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 1,  0, 2 },20,	/* Energy */
	10, 3,-3, 2, 10, A_WIS, SPE_CURE_SICKNESS,   -4
},

{	{"Ice Mage", 0}, {
	{"Cooler",        0},   /* WAC was Chiller */
	{"Condenser",     0},   /* WAC was Chiller */
	{"Chiller",       0},
	{"Froster",       0},
	{"Permafroster",  0},   /* WAC was Froster */
	{"Icer",          0},   /* WAC was Froster */
	{"Freezer",       0},
	{"Sublimer",      0},   /* WAC was Freezer */
	{"Ice-Master",    0} },
	"Air", "Frost", "Smoke", /* Special */
	"Ice", "the great Ring of Ice", "the Earth Mage's Cave",
	PM_ICE_MAGE, NON_PM, PM_WINTER_WOLF_CUB,
	PM_HIGH_ICE_MAGE, PM_FROSTER, PM_EARTH_MAGE,
	PM_RUST_MONSTER, PM_XORN, S_RUSTMONST, S_XORN,
#if 0
	ART_DEEP_FREEZE, ART_FROST_BRAND,
#endif
	ART_STORM_WHISTLE,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_VAMPIRE|MRACE_ORC
	|MRACE_MFLAY|MRACE_GHOUL | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_CONE_OF_COLD,    -4 /* From old role.c */
},

#ifdef JEDI
{	{"Jedi", 0}, {
	{"Youngling",     0},
	//{"Padawan",       0},
	{"Padawan",       0},
	//{"Jedi Apprentice", 0},
	{"Jedi Apprentice", 0},
	//{"Jedi Knight",    0},
	{"Jedi Knight",    0},
	{"Jedi Hero",      0},
	{"Jedi Master",    0} },
	"the Light Side", "the Force", "the Dark Side",
	"Jed", "the Jedi Temple", "the Outer Rim",
	PM_JEDI, NON_PM, PM_LITTLE_DOG,
	PM_THE_JEDI_MASTER, PM_PADAWAN, PM_LORD_SIDIOUS,
	PM_STORMTROOPER, PM_STORMTROOPER, S_HUMAN, S_HUMAN,
#if 0
	ART_DELUDER, ART_SNAKESKIN
#endif
	ART_LIGHTSABER_PROTOTYPE,
	MRACE_HUMAN|MRACE_ELF|MRACE_HOBBIT | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7, 14, 12, 10, 14 },
	{  30, 15, 15, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 20, 4,  0, 8,  2, 4 },	/* Hit points */
	{  5, 4,  0, 1,  0, 4 },10,	/* Energy */
	0, 12 , -1, 
	2, 10, A_INT, SPE_CHARM_MONSTER,    -4
},
#endif

{	{"Knight", 0}, {
	{"Gallant",     0},
	{"Esquire",     0},
	{"Bachelor",    0},
	{"Sergeant",    0},
	{"Knight",      0},
	{"Banneret",    0},
	{"Cavalier",   "Cavaliere"},
	{"Seignieur",   "Dame"},
	{"Paladin",     0} },
	"Lugh", "_Brigit", "Manannan Mac Lir", /* Celtic */
	"Kni", "Camelot Castle", "the Isle of Glass",
	PM_KNIGHT, NON_PM, PM_PONY,
	PM_KING_ARTHUR, PM_PAGE, PM_IXOTH,
	PM_QUASIT, PM_OCHRE_JELLY, S_IMP, S_JELLY,
#if 0
	ART_CLARENT, ART_EXCALIBUR,
#endif
	ART_MAGIC_MIRROR_OF_MERLIN,
	MRACE_HUMAN|MRACE_DWARF|MRACE_CLOCK|MRACE_VAMPIRE |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  13,  7, 14,  8, 10, 17 },
	{  30, 15, 15, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  2, 0 },	/* Hit points */
	{ 10, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4
},


{	{"Locksmith", 0}, {
	{"Listener",     0},
	{"Lockpicker",    0},
	{"Lock Breaker",       0},
	{"Lock Trickster",    0},
	{"Safecracker",     0},
	{"Keymaster",     0},
	{"Slim Jim",     "Slim Jane"},
	{"Keymaker",     0},
	{"Supreme Locksmith",       0} },
	"Nuada", "Dagda", "Morrigan", /* Celtic */
	"Loc", "Emergency Base", "Locked Room",
	PM_LOCKSMITH, NON_PM, NON_PM,
	PM_MASTER_OF_LOCKS, PM_THUG, PM_BURGLAR_ED,
	PM_MUGGER, PM_LEPRECHAUN, S_HUMAN, S_HUMAN,
#if 0
	ART_OATHKEEPER, ART_DIRK,
#endif
	ART_LOCKPICK_OF_ARSENE_LUPIN,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_CLOCK | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  7, 10,  7,  6 },
	{  20, 15, 10, 30, 10, 20 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_KNOCK, -4
},

{	{"Lunatic", 0}, {
	{"Infected",     0},
	{"Summoner",   0},
	{"Moon Worshipper",0},
	{"Render",0},
	{"Fiend",    0},
	{"Nightmare",   0},
	{"Devourer",     0},
	{"Greater Fiend",    0},
	{"Master Shapeshifter",  0} },
	"Eluvian", "Moon", "Lycanthus",	/* from an old SLASH version without extended magic */
	"Lun", "the Calerin Forest", "the Nightmare Forest",
	PM_LUNATIC, NON_PM, PM_WOLF,
	PM_HIGH_LYCANTHROPE, PM_FIEND, PM_SIR_LORIMAR,
	PM_WOODLAND_ELF, PM_FOREST_CENTAUR, S_HUMAN, S_CENTAUR,
#if 0
	ART_DOOMBLADE, ART_HEARTSEEKER
#endif
	ART_STAFF_OF_WITHERING,
	MRACE_HUMAN|MRACE_ELF|MRACE_OGRE|MRACE_ORC|MRACE_GIANT|MRACE_TROLL | 
	ROLE_MALE|ROLE_FEMALE|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7, 13,  6 },
	{  20, 20, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{  18, 0,  0, 8,  6, 0 },	/* Hit points */
	{  5, 0,  0, 3,  0, 3 },10,	/* Energy */
	-3, 5, 0, 2, 10, A_INT, SPE_CAUSE_FEAR,   -4
},

{	{"Monk", 0}, {
	{"Candidate",         0},
	{"Novice",            0},
	{"Initiate",          0},
	{"Student of Stones", 0},
	{"Student of Waters", 0},
	{"Student of Metals", 0},
	{"Student of Winds",  0},
	{"Student of Fire",   0},
	{"Master",            0} },
	"Shan Lai Ching", "Chih Sung-tzu", "Huan Ti", /* Chinese */
	"Mon", "the Monastery of Chan-Sune",
	  "the Monastery of the Earth-Lord",
	PM_MONK, NON_PM, NON_PM,
	PM_GRAND_MASTER, PM_ABBOT, PM_MASTER_KAEN,
	PM_EARTH_ELEMENTAL, PM_XORN, S_ELEMENTAL, S_XORN,
#if 0
	ART_GAUNTLETS_OF_DEFENSE, ART_WHISPERFEET,
#endif
	ART_EYES_OF_THE_OVERWORLD,
	MRACE_HUMAN|MRACE_HOBBIT|MRACE_NYMPH|MRACE_CLOCK | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  8,  8,  7,  7 },
	{  25, 10, 20, 20, 15, 10 },
	/* Init   Lower  Higher */
	{ 17, 0,  0, 8,  1, 0 },	/* Hit points */
	{  8, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_RESTORE_ABILITY, -4
},

{	{"Musician", 0}, {
	{"Whistler", 0},
	{"Drummer", 0},
	{"Fiddler", 0},
	{"Singer",	0},
	{"Entertainer", 0},
	{"Concertist", 0},
	{"Meistersinger", "Primadonna"},
	{"Conductor", 0},
	{"Virtuoso", 0} },
	"Donblas", "Grome", "Arioch", /* Melnibonean */

	"Mus", "Wild West Bar", "High Noon Plains",
	PM_MUSICIAN, NON_PM, NON_PM,
	PM_HARRY_THE_COWBOY, PM_ATTENDANT, PM_DRACO_THE_SHARPSHOOTER,
	PM_WOODLAND_ELF, PM_HIGH_ELF, S_HUMAN, S_HUMAN,
#if 0
	ART_MUSA_SHIYA, ART_DIRK,
#endif
	ART_HARP_OF_HARMONY,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_NYMPH |
	ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  7, 10,  7, 10 },
	{  20, 15, 10, 30, 10, 20 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  8, 0,  0, 2,  0, 2 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_CAUSE_FEAR,   -4
},

{	{"Necromancer", 0}, {
	{"Gravedigger",  0},
	{"Embalmer", 0},
	{"Mortician", 0},
	{"Zombie Lord", 0},
	{"Ghoul Master",0},
	{"Necromancer", 0},
	{"Necromancer", 0},
	{"Undead Master", 0},
	{"Lich Lord", 0} },
	"Nharlotep", "Zugguthobal", "Gothuulbe", /* Assorted slimy things */
	"Nec", "the Tower of the Dark Lord", "the Lair of Maugneshaagar",
	PM_NECROMANCER, NON_PM, PM_GHOUL,
	PM_DARK_LORD, PM_EMBALMER, PM_MAUGNESHAAGAR,
	PM_NUPPERIBO, PM_MONGBAT, S_BAT, S_IMP,
#if 0
	ART_SERPENT_S_TONGUE, ART_GRIMTOOTH,
#endif
	ART_GREAT_DAGGER_OF_GLAURGNAA,
	MRACE_HUMAN|MRACE_ELF|MRACE_VAMPIRE|MRACE_MFLAY|MRACE_ORC|MRACE_GHOUL | 
	ROLE_MALE|ROLE_FEMALE|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */  /* Direct copy from Wizard */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 17, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 15, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 
	2, 10, A_INT, SPE_SUMMON_UNDEAD,   -4
},

{	{"Ninja", "Kunoichi"}, {
	{"Kukkyu",     0},
	{"Hakkyu",    0},
	{"Nanakyu",       0},
	{"Rokkyu",    0},
	{"Gokyu",      0},
	{"Yonkyu",     0},
	{"Sankyu",     0},
	{"Nikyu",     0},
	{"Ikkyu",       0} },
	"Jiraiya", "_Tsunade", "Orochimaru", /* from Jiraiya Goketsu Monogatari */
	"Nin", "Green Plains", "Demon Orb Lair",
	PM_NINJA, NON_PM, PM_LITTLE_DOG,
	PM_KEN_HAYABUSA, PM_ROSHI, PM_JAQUIO,
	PM_WOLF, PM_GIANT_BAT, S_DOG, S_BAT,
#if 0
	ART_NIGHTENGALE, ART_SPINESEEKER,
#endif
	ART_FUMA_ITTO_NO_KEN,
	MRACE_HUMAN|MRACE_ELF|MRACE_VAMPIRE|MRACE_KOBOLD|MRACE_ORC
	| ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7, 15,  7,  6 },
	{  15, 10, 10, 30, 10, 10 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  1, 0 },	/* Hit points */
	{  7, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_ENCHANT_WEAPON,    -4
},

{	{"Nobleman", "Noblewoman"}, {
	{"Pargar",       0},
	{"Cneaz",	     0},
	{"Ban",		     0},
	{"Jude",	     0},
	{"Boier",	     0},
	{"Cupar",	     0},
	{"Clucer",	     0},
	{"Domn",	     0},
	{"Domnitor",     0} },
	"God the Father", "_Mother Earth", "the Satan", /* Romanian, sorta */
	"Nob", "your ancestral home",
	  "the rebel village",
	PM_NOBLEMAN, PM_NOBLEWOMAN, PM_PONY,
	PM_OLD_GYPSY_WOMAN, PM_SERVANT, PM_REBEL_RINGLEADER,
	PM_SOLDIER, PM_PEASANT, S_HUMANOID, S_HUMAN,
#if 0
	ART_ROD_OF_LORDLY_MIGHT, ART_DIRK,
#endif
	ART_MANTLE_OF_HEAVEN,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_HOBBIT|MRACE_VAMPIRE | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   10,10,  7, 10,  7,  7 },
	{  20, 18, 10, 20, 15, 17 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 2,  0, 2,  0, 2 },10,	/* Energy */
	10, 4,-4, 4, 8, A_INT, SPE_PROTECTION, -24
},

{	{"Officer", 0}, {
	{"Coffee Boy", "Coffee Girl"},
	{"Cadet",   0},
	{"Meter Man", "Meter Maid"},
	{"Rookie",0},
	{"Beat Cop",    0},
	{"Sergeant",   0},
	{"Captain",     0},
	{"Police Chief",    0},
	{"Commissioner",  0} },
	"Magnum", "Smith", "Wesson", /* guns */
	"Off", "Central Cop Station", "Tank Breaker Hideout",
	PM_OFFICER, NON_PM, PM_LITTLE_DOG,
	PM_COMMISSIONER_HUNTER, PM_STUDENT, PM_GRANDPA_TANK_BREAKER,
	PM_MUGGER, PM_STREET_THUG, S_HUMAN, S_HUMAN,
#if 0
	ART_PEACEKEEPER, ART_DIRK,
#endif
	ART_WHISTLE_OF_THE_WARDEN,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF | 
	ROLE_MALE|ROLE_FEMALE|ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  12, 11, 10, 13, 10, 10 },
	{  20, 15, 15, 10, 20, 10 },
	/* Init   Lower  Higher */
	{  12, 0,  0, 8,  0, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_HASTE_SELF,   -4
},

{	{"Paladin", 0}, {
	{"Undefiler",    0},
	{"Faithful",     0},
	{"Religious One",       0},
	{"Returned One",      0},
	{"Sacred One",      0},
	{"Savior",       0},
	{"Celestial",        0},
	{"Decomposer",   0},
	{"Annihilator", 0} },
	0, 0, 0,	/* randomly chosen from other roles */
	"Pal", "Ancient Temple", "the World-Eater's domain",
	PM_PALADIN, NON_PM, PM_PONY,
	PM_EREC, PM_ACOLYTE, PM_ALDUIN,
	PM_HUMAN_ZOMBIE, PM_WRAITH, S_ZOMBIE, S_WRAITH,
#if 0
	ART_SILVERSTAR, ART_DISRUPTER,
#endif
	ART_CUDGEL_OF_CUTHBERT,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GIANT | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 10,  7,  7,  7 },
	{  20, 10, 30, 15, 20, 5 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 9,  1, 0 },	/* Hit points */
	{ 17, 3,  0, 2,  0, 2 },10,	/* Energy */
	0, 3,-2, 2, 10, A_WIS, SPE_REMOVE_CURSE,    -4
},

{	{"Pirate", 0}, {
	{"Landlubber",  0},
	{"Swabbie",        	0},
	{"Cutthroat",   0},
	{"Bosun",      		0},
	{"Second Mate",     0},
	{"First Mate",      0},
	{"Captain",			0},
	{"Pirate Lord",   	0},
	{"Dread Pirate",  	0} },
	"Nereus", "Neptune", "Proteus",	/* Classical sea gods */
	"Pir", "Tortuga", "Shipwreck Island",
	PM_PIRATE, NON_PM, NON_PM,
	PM_MAYOR_CUMMERBUND, PM_PIRATE_BROTHER, PM_BLACKBEARD_S_GHOST,
	PM_SKELETAL_PIRATE, PM_SOLDIER, S_RODENT, S_ELEMENTAL, /* Ghost pirates, soldiers, rats in the food stores, and the occasional storm*/
#if 0
	ART_MARAUDER_S_MAP, ART_REAVER,
#endif
	ART_TREASURY_OF_PROTEUS,
	MRACE_HUMAN|MRACE_ELF |
	ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   8,  7, 7,  8,  8,  7 },
	{  20, 15, 10, 20, 25, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  1, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 8, 0, 2,  9, A_INT, SPE_CAUSE_FEAR,    -4
},
#if 0 /* 5lo: Needs an entire redesign, deferred for now */
{	{"Pokemon", 0}, {
	{"CROC",    0},
	{"ALIGE",       0},
	{"BEIL",   0},
	{"MADAM", 0},
	{"GUAIL",         0},
	{"BANQ",    0},	/* One skilled at crossbows */
	{"PIJIA",        0},
	{"CLAU",  0},
	{"MIY",      0} },
	"A'En", "Dr. Oujide", "Team Missile Bomb", /* Pokemon Vietnamese Crystal */
	"Pok", "ELF GRANDFATHER RESEARCH LAB", "JOIN-CAVE",
	PM_POKEMON, NON_PM, PM_EEVEE,
	PM_DR__WUSIJI, PM_POKEMON_TRAINER, PM_HO_OH,
	PM_KOFFING, PM_RHYHORN, S_EYE, S_QUADRUPED,
	ART_GAUNTLETS_OF_ILLUSION,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|MRACE_VAMPIRE|MRACE_WERE|MRACE_ORC |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  13, 13, 13,  9, 13,  7 },
	{  30, 10, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  4, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4
},
#endif
{	{"Priest", "Priestess"}, {
	{"Aspirant",    0},
	{"Acolyte",     0},
	{"Adept",       0},
	{"Priest",      "Priestess"},
	{"Curate",      0},
	{"Canon",       "Canoness"},
	{"Lama",        0},
	{"Patriarch",   "Matriarch"},
	{"High Priest", "High Priestess"} },
	0, 0, 0,	/* chosen randomly from among the other roles */
	"Pri", "the Great Temple", "the Temple of Nalzok",
	PM_PRIEST, PM_PRIESTESS, NON_PM,
	PM_ARCH_PRIEST, PM_ACOLYTE, PM_NALZOK,
	PM_HUMAN_ZOMBIE, PM_WRAITH, S_ZOMBIE, S_WRAITH,
#if 0
	ART_DISRUPTER, ART_SUNSWORD,
#endif
	ART_MITRE_OF_HOLINESS,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|MRACE_ORC|MRACE_GIANT|MRACE_NYMPH|MRACE_CLOCK | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 10,  7,  7,  7 },
	{  15, 10, 30, 15, 20, 10 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 17, 3,  0, 2,  0, 2 },10,	/* Energy */
	0, 3,-2, 2, 10, A_WIS, SPE_REMOVE_CURSE,    -4
},
#if 0 /* 5lo: Needs an entire rewrite, deferred for now */
{	{"Psion", 0}, {
	{"Cantripper",         0},
	{"Spoonbender",            0},
	{"Kinetic",          0},
	{"Seer", 0},
	{"Psychic", 0},
	{"Oracle", 0},
	{"Levitator",  0},
	{"Visionary",   0},
	{"Master Psion",            0} },
	"Bickney", "Corridor", "Lockney", /* Egyptian */
	"Psi", "the Monastery of Psionics",
	  "the Monastery of the Swamp",
	PM_PSION, NON_PM, NON_PM,
	PM_PSI_MASTER, PM_ABBOT, PM_MASTER_SABRINA,
	PM_DOPPELGANGER, PM_MULTICOLOR_GRUE, S_GRUE, S_HUMAN,
#if 0
	ART_GAUNTLETS_OF_DEFENSE, ART_WHISPERFEET,
#endif
	ART_MANTLE_OF_KNOWLEDGE,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|MRACE_VAMPIRE|MRACE_WERE|MRACE_ORC | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  8,  8,  7,  7 },
	{   5, 30, 30,  5,  5, 25 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 20, 4,  0, 4,  0, 8 },10,	/* Energy */
	10, 8,-2, 2, 20, A_WIS, SPE_WIZARD_LOCK, -4
},
#endif
{	{"Ranger", 0}, {
#if 0	/* OBSOLETE */
	{"Edhel",       "Elleth"},
	{"Edhel",       "Elleth"},      /* elf-maid */
	{"Ohtar",       "Ohtie"},       /* warrior */
	{"Kano",			/* commander (Q.) ['a] */
			"Kanie"},	/* educated guess, until further research- SAC */
	{"Arandur",			/* king's servant, minister (Q.) - guess */
			"Aranduriel"},	/* educated guess */
	{"Hir",         "Hiril"},       /* lord, lady (S.) ['ir] */
	{"Aredhel",     "Arwen"},       /* noble elf, maiden (S.) */
	{"Ernil",       "Elentariel"},  /* prince (S.), elf-maiden (Q.) */
	{"Elentar",     "Elentari"},	/* Star-king, -queen (Q.) */
	"Solonor Thelandira", "Aerdrie Faenya", "Lolth", /* Elven */
#endif
	{"Tenderfoot",    0},
	{"Lookout",       0},
	{"Trailblazer",   0},
	{"Reconnoiterer", "Reconnoiteress"},
	{"Scout",         0},
	{"Arbalester",    0},	/* One skilled at crossbows */
	{"Archer",        0},
	{"Sharpshooter",  0},
	{"Marksman",      "Markswoman"} },
	"Mercury", "_Venus", "Mars", /* Roman/planets */
	"Ran", "Orion's camp", "the cave of the wumpus",
	PM_RANGER, NON_PM, PM_LITTLE_DOG /* Orion & canis major */,
	PM_ORION, PM_HUNTER, PM_SCORPIUS,
	PM_FOREST_CENTAUR, PM_SCORPION, S_CENTAUR, S_SPIDER,
#if 0
	ART_STRAIGHTSHOT, ART_DIRK,
#endif
	ART_LONGBOW_OF_DIANA,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_WERE|MRACE_ORC|MRACE_GIANT|MRACE_KOBOLD |
	ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{  13, 13, 13,  9, 13,  7 },
	{  30, 10, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 18, 0,  0, 6,  1, 0 },	/* Hit points */
	{  4, 0,  0, 1,  0, 1 },12,	/* Energy */
	10, 9, 2, 1, 10, A_INT, SPE_INVISIBILITY,   -4
},

{	{"Rogue", 0}, {
	{"Footpad",     0},
	{"Cutpurse",    0},
	{"Rogue",       0},
	{"Pilferer",    0},
	{"Robber",      0},
	{"Burglar",     0},
	{"Filcher",     0},
	{"Magsman",     "Magswoman"},
	{"Thief",       0} },
	"Issek", "Mog", "Kos", /* Nehwon */
	"Rog", "the Thieves' Guild Hall", "the Assassins' Guild Hall",
	PM_ROGUE, NON_PM, NON_PM,
	PM_MASTER_OF_THIEVES, PM_THUG, PM_MASTER_ASSASSIN,
	PM_LEPRECHAUN, PM_GUARDIAN_NAGA, S_NYMPH, S_NAGA,
#if 0
	ART_DOOMBLADE, ART_BAT_FROM_HELL,
#endif
	ART_MASTER_KEY_OF_THIEVERY,
	MRACE_HUMAN|MRACE_VAMPIRE|MRACE_WERE|MRACE_ORC|MRACE_KOBOLD
	| ROLE_MALE|ROLE_FEMALE |
	  ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7, 10,  7,  6 },
	{  20, 10, 10, 30, 20, 10 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{  3, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 8, 0, 1,  9, A_INT, SPE_DETECT_TREASURE, -4
},

{	{"Samurai", 0}, {
#if 0
	{"Hatamoto",    0},  /* Banner Knight */
	{"Ronin",       0},  /* no allegiance */
	{"Ninja Gaiden",       "Kunoichi"},  /* secret society */
	{"Joshu",       0},  /* heads a castle */
	{"Ryoshu",      0},  /* has a territory */
	{"Kokushu",     0},  /* heads a province */
	{"Daimyo",      0},  /* a samurai lord */
	{"Kuge",        0},  /* Noble of the Court */
	{"Shogun",      0} },/* supreme commander, warlord */
#endif
	{"Ashigaru",	0},	/* footman */
	{"Hatamoto",	0},	/* banner knight */
	{"Kumigashira",	0},	/* squad leader */
	{"Bangashira",	0},	/* leader of Kumigashira */
	{"Samurai-Daisho",	0},	/* lieutenant */
	{"Bushou",	0},	/* general */
	{"Ryoshu",	0},	/* has a territory */
	{"Joshu",	0},	/* heads a castle */
	{"Kokushu",	0} },	/* heads a province */
	"_Amaterasu Omikami", "Raijin", "Susanowo", /* Japanese */
	"Sam", "the Castle of the Taro Clan", "the Shogun's Castle",
	PM_SAMURAI, NON_PM, PM_LITTLE_DOG,
	PM_LORD_SATO, PM_ROSHI, PM_ASHIKAGA_TAKAUJI,
	PM_WOLF, PM_STALKER, S_DOG, S_ELEMENTAL,
#if 0
	ART_SNICKERSNEE, ART_KIKU_ICHIMONJI,
#endif
	ART_TSURUGI_OF_MURAMASA,
	MRACE_HUMAN|MRACE_CLOCK|MRACE_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  10,  8,  7, 10, 17,  6 },
	{  30, 10,  8, 30, 14,  8 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  1, 0 },	/* Hit points */
	{  7, 0,  0, 1,  0, 1 },11,	/* Energy */
	10, 10, 0, 0,  8, A_INT, SPE_CLAIRVOYANCE,    -4
},

#ifdef TOURIST
{	{"Tourist", 0}, {
	{"Rambler",     0},
	{"Sightseer",   0},
	{"Excursionist",0},
	{"Peregrinator","Peregrinatrix"},
	{"Traveler",    0},
	{"Journeyer",   0},
	{"Voyager",     0},
	{"Explorer",    0},
	{"Adventurer",  0} },
	"Blind Io", "_The Lady", "Offler", /* Discworld */
	"Tou", "Ankh-Morpork", "the Thieves' Guild Hall",
	PM_TOURIST, NON_PM, NON_PM,
	PM_TWOFLOWER, PM_GUIDE, PM_MASTER_OF_THIEVES,
	PM_GIANT_SPIDER, PM_FOREST_CENTAUR, S_SPIDER, S_CENTAUR,
#if 0
	ART_WHISPERFEET, ART_LUCKBLADE,
#endif
	ART_YENDORIAN_EXPRESS_CARD,
	MRACE_HUMAN|MRACE_HOBBIT | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  6,  7,  7, 10 },
	{  15, 10, 10, 15, 30, 20 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  0, 0 },	/* Hit points */
	{ 12, 0,  0, 1,  0, 1 },14,	/* Energy */
	0, 5, 1, 2, 10, A_INT, SPE_CHARM_MONSTER,   -4
},
#endif

{	{"Undead Slayer", 0}, {
	{"Assistant",    0},
	{"Eliminator",   0},
	{"Eliminator",   0},
	{"Exterminator", 0},
	{"Exterminator", 0},
	{"Destroyer",   0},
	{"Vindicator",  0},
	{"Vindicator",  0},
	{"Undead Slayer", 0} },
	"Seeker", "_Osiris", "Seth", /* Egyptian */
	"Und", "the Temple of Light", "the Crypt of Dracula",
	PM_UNDEAD_SLAYER, NON_PM, NON_PM,
	PM_VAN_HELSING, PM_EXTERMINATOR, PM_COUNT_DRACULA,
	PM_HUMAN_MUMMY, PM_VAMPIRE, S_MUMMY, S_VAMPIRE,
#if 0
	ART_HOLY_SPEAR_OF_LIGHT, ART_SUNSWORD,
#endif
	ART_STAKE_OF_VAN_HELSING,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|MRACE_WERE|MRACE_ORC|MRACE_NYMPH|MRACE_MFLAY|MRACE_OGRE|MRACE_GIANT|MRACE_TROLL|MRACE_KOBOLD |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */ /* Modified from Knight */
	{  13,  7, 14,  8, 10, 10 },
	{  20, 15, 15, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 22, 0,  0, 8,  2, 0 },	/* Hit points */
	{ 16, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4
},

{	{"Undertaker", 0}, {
	{"Hearse Driver",    0},
	{"Disposer",   0},
	{"Grave Creator",   0},
	{"Open Casketeer", 0},
	{"Closed Casketeer", 0},
	{"Dumper",   0},
	{"Shoveler",  0},
	{"Crematorist",  0},
	{"Corpse Burner", 0} },
	"_Hel", "Pluto", "Orcus", /* Death */
	"Unt", "New Graveyard", "Underground Demon Lair",
	PM_UNDERTAKER, NON_PM, PM_DEATH_DOG,
	PM_AJELA, PM_EXTERMINATOR, PM_SEDUCER_SAINT,
	PM_NALFESHNEE, PM_PIT_FIEND, S_DEMON, S_DEMON,
#if 0
	ART_BLACKSHROUD, ART_DIRK
#endif
	ART_PICK_OF_THE_GRAVE,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|MRACE_NYMPH|MRACE_KOBOLD |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7,  7, 13, 10, 13,  6 },
	{  15, 20, 20, 15, 25, 10 },
	/* Init   Lower  Higher */
	{ 22, 0,  0, 8,  2, 0 },	/* Hit points */
	{ 16, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4
},

{	{"Valkyrie", 0}, {
	{"Stripling",   0},
	{"Skirmisher",  0},
	{"Fighter",     0},
	{"Man-at-arms", "Woman-at-arms"},
	{"Great Warrior",     0},
	{"Swashbuckler",0},
	{"Hero",        "Heroine"},
	{"Champion",    0},
	{"Lord",        "Lady"} },
	"Tyr", "Odin", "Loki", /* Norse */
	"Val", "the Shrine of Destiny", "the cave of Surtur",
	PM_VALKYRIE, NON_PM, NON_PM /*PM_WINTER_WOLF_CUB*/,
	PM_NORN, PM_SUPER_WARRIOR, PM_LORD_SURTUR,
	PM_FIRE_ANT, PM_FIRE_GIANT, S_ANT, S_GIANT,
#if 0
	ART_MJOLLNIR, ART_FROST_BRAND,
#endif
	ART_ORB_OF_FATE,
	MRACE_HUMAN|MRACE_DWARF|MRACE_GIANT|MRACE_NYMPH | ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  10,  7,  7,  7, 12,  7 },
	{  30,  6,  7, 20, 30,  7 },
	/* Init   Lower  Higher */
	{ 20, 0,  0, 8,  2, 0 },	/* Hit points */
	{  2, 0,  0, 1,  0, 1 },10,	/* Energy */
	0, 10,-2, 0,  9, A_WIS, SPE_LIGHTNING,    -4
},

{	{"Warrior", 0}, {
	{"Swordsman",     "Swordswoman"},
	{"Longswordsman",     "Longswordswoman"},
	{"Two-Handed Swordsman",    "Two-Handed Swordswoman"},
	{"Legionnaire",    0},
	{"Crusader",      0},
	{"Baron",    "Baroness"},
	{"Count",   "Countess"},
	{"Duke",   "Duchess"},
	{"Patriarch",     "Matriarch"} },
	"Talos", "_Meridia", "Clavicus Vile", /* The Elder Scrolls */
	"War", "Acro Castle", "the Isle of the Damned",
	PM_WARRIOR, NON_PM, PM_PONY,
	PM_SIR_LANCELOT, PM_PAGE, PM_ARCHNEMESIS,
	PM_RED_DRAGON, PM_BARBARIAN, S_DRAGON, S_HUMAN,
#if 0
	ART_HRUNTING, ART_DRAGONBANE,
#endif
	ART_SWORD_OF_SVYATOGOR,
	MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_GNOME|MRACE_HOBBIT|MRACE_KOBOLD|MRACE_CLOCK|MRACE_GIANT | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{  18,  7, 11, 10, 12, 14 },
	{  35, 10, 14, 13, 22, 6 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 15,  5, 0 },	/* Hit points */
	{ 10, 0,  0, 1,  0, 1 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4
},

{	{"Wizard", 0}, {
	{"Evoker",      0},
	{"Conjurer",    0},
	{"Thaumaturge", 0},
	{"Magician",    0},
	{"Warlock",     "Witch"},
	{"Enchanter",   "Enchantress"},
	{"Sorcerer",    "Sorceress"},
	{"Wizard",      0},
	{"Mage",        0} },
	"Ptah", "Thoth", "Anhur", /* Egyptian */
	"Wiz", "the Lonely Tower", "the Tower of Darkness",
	PM_WIZARD, NON_PM, PM_KITTEN,
	PM_NEFERET_THE_GREEN, PM_APPRENTICE, PM_DARK_ONE,
	PM_VAMPIRE_BAT, PM_XORN, S_BAT, S_WRAITH,
#if 0
	ART_MAGICBANE, ART_DELUDER,
#endif
	ART_EYE_OF_THE_AETHIOPICA,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_VAMPIRE|MRACE_ORC|MRACE_GHOUL|MRACE_MFLAY | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* Str Int Wis Dex Con Cha */
	{   7, 10,  7,  7,  7,  7 },
	{  10, 30, 10, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 15, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 25, 3,  0, 2,  0, 3 },12,	/* Energy */
	0, 1, 0, 3, 10, A_INT, SPE_MAGIC_MISSILE,   -4
},

#ifdef YEOMAN
{	{"Yeoman", 0}, {
	/* Landowner titles, naval ranks and positions */
	/* We intentionally avoid Lieutenant and Captain */
	{"Usher",          0},
	{"Steward",        "Stewardess"},
	{"Keeper",         0},
	{"Marshal",        0},
	{"Master Steward", "Master Stewardess"},
	{"Chamberlain",    0},
	{"Constable",      0},
	{"Chancellor",     0},
	{"Regent",         0} },
	"His Majesty", "His Holiness", "The Commons", /* The three estates */
	"Yeo", "London", "the inner ward",
	PM_YEOMAN, NON_PM, PM_PONY,
	PM_CHIEF_YEOMAN_WARDER, PM_YEOMAN_WARDER, PM_COLONEL_BLOOD,
	PM_RAVEN, PM_WEREWOLF, S_RODENT, S_DOG,
#if 0
	ART_REAPER, ART_SWORD_OF_JUSTICE,
#endif
	ART_CROWN_OF_SAINT_EDWARD,
	MRACE_HUMAN|MRACE_ELF|MRACE_HOBBIT | ROLE_MALE|ROLE_LAWFUL,
	/* Str Int Wis Dex Con Cha */
	{  12,  7, 10, 12, 12,  7 },
	{  20, 15, 15, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 19, 0,  0, 8,  2, 0 },	/* Hit points */
	{  3, 4,  0, 1,  0, 2 },10,	/* Energy */
	10, 8,-2, 0,  9, A_WIS, SPE_KNOCK,     -4
},
#endif

{	{"Zookeeper", 0}, {
	{"Feeder",      0},
	{"Feeder",0},
	{"Tamer",0},
	{"Tamer",     0},
	{"Trainer",   0},
	{"Chief Tamer",   0},
	{"Vet",0},
	{"Zookeeper",   0},
	{"Curator",     0} },
	"Balder", "Edda", "Hagen", /* Norse mythology */
	"Zoo", "Yellowstone Park", "Jurassic Park",
	PM_ZOOKEEPER, NON_PM, PM_DINGO_PUPPY,
	PM_MASTER_ZOOKEEPER, PM_STUDENT, PM_FEARFUL_TRICERATOPS,
	PM_CARNIVOROUS_APE, PM_JAGUAR, S_FELINE, S_ZOUTHERN,
#if 0
	ART_LIONTAMER, ART_DIRK,
#endif
	ART_GOLDEN_WHISTLE_OF_NORA,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_KOBOLD|MRACE_GIANT | 
	  ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	/* Str Int Wis Dex Con Cha */
	{   7, 10, 10,  7,  7,  7 },
	{  20, 20, 20, 10, 20, 10 },
	/* Init   Lower  Higher */
	{ 16, 0,  0, 8,  1, 0 },	/* Hit points */
	{ 10, 0,  0, 1,  0, 1 },14,	/* Energy */
	10, 5, 0, 2, 10, A_INT, SPE_CHARM_MONSTER,   -4
},

/* Array terminator */
{{0, 0}}
};

/* 
 * Realistically we don't need to build another structure array just for this
 * but this does leave us a framework to do more crossaligned roles
 * sometimes in the future, if we feel like it
 *
 * ideally we'd just properly expand roles[] but the hack is easier for a patch
 *
 * Potential concepts for alternate alignment roles:
 * Dark Paladin: Chaotic version of Paladin, perhaps gets a bonus for using cursed equipment
 * Sith: Chaotic counterpart to Jedi, always start with a red lightsaber, quest artifact being double-sided lightsaber instead?
 */

const struct Role align_roles[] = {
{      {"Dark Knight", 0}, {
       {"Sniveler", 0},
       {"Pawn", 0},
       {"Brute", 0},
       {"Mercenary", 0},
       {"Blackguard", 0},
       {"Turncoat", 0},
       {"Knave", 0},
       {"Dark Baron", 0}, 
       {"Dark Paladin", 0} }, /* This should probably be changed */
       "Lugh", "_Brigit", "Manannan Mac Lir", /* Celtic */
       "Kni", "Camelot Castle", "the Isle of Glass",
       PM_KNIGHT, NON_PM, PM_PONY,
       PM_KING_ARTHUR, PM_PAGE, PM_IXOTH,
       PM_QUASIT, PM_OCHRE_JELLY, S_IMP, S_JELLY,
#if 0
	ART_DIRGE, ART_SOULTHIEF,
#endif
       ART_MAGIC_MIRROR_OF_MERLIN,
       MRACE_HUMAN|MRACE_DWARF|MRACE_CLOCK|MRACE_VAMPIRE |
	ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_CHAOTIC,
       /* Str Int Wis Dex Con Cha */
       {  13,  7, 14,  8, 10, 17 },
       {  30, 15, 15, 10, 20, 10 },
       /* Init   Lower  Higher */
       { 14, 0,  0, 8,  2, 0 },        /* Hit points */
       {  1, 4,  0, 1,  0, 2 },10,     /* Energy */
       10, 8,-2, 0,  9, A_WIS, SPE_TURN_UNDEAD,     -4
},
/* new terminator, though we don't need it here */
{{0, 0}}
};


/* The player's role, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Role urole =
{	{"Undefined", 0}, { {0, 0}, {0, 0}, {0, 0},
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	"L", "N", "C", "Xxx", "home", "locate",
	NON_PM, NON_PM, NON_PM, NON_PM, NON_PM, NON_PM,
	NON_PM, NON_PM, 0, 0, 
#if 0
	0, 0,
#endif
	0, 0,
	/* Str Int Wis Dex Con Cha */
	{   7,  7,  7,  7,  7,  7 },
	{  20, 15, 15, 20, 20, 10 },
	/* Init   Lower  Higher */
	{ 10, 0,  0, 8,  1, 0 },	/* Hit points */
	{  2, 0,  0, 2,  0, 3 },14,	/* Energy */
	0, 10, 0, 0,  4, A_INT, 0, -3
};

/* Table of all races */
const struct Race races[] = {

{	"clockwork automaton", "automatic", "clockwork-kind", "Clk",
	{0, 0},
	PM_CLOCKWORK_AUTOMATON, NON_PM, NON_PM, NON_PM,
	MRACE_CLOCK | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	MRACE_CLOCK, MRACE_DWARF, MRACE_OGRE|MRACE_GNOME,
	/*    Str     Int Wis Dex Con Cha */
	{   3,         3,  3,  3,  3,  3 },
	{STR18(100),  18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  3, 0,  3, 0,  3, 0 },	/* Hit points */
	{  3, 0,  3, 0,  3, 0 },	/* Energy */
},

{	"doppelganger", "doppelganger", "doppelganger-kind", "Dop",
	{0, 0},
	PM_DOPPELGANGER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MRACE_HUMAN | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL|ROLE_CHAOTIC,
	MRACE_HUMAN, MRACE_WERE, MRACE_ELF|MRACE_GNOME|MRACE_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      1,      3,  3,  1,  1,  1 },
	{ STR18(100), 20, 20, 20, 20, 15 },
	/* Init   Lower  Higher */
	{  0, 0,  0, 1,  1, 0 },	/* Hit points */
	{  7, 0,  5, 0,  5, 0 }		/* Energy */
},

{	"drow", "droven", "drovenkind", "Dro",
	{0, 0},
	PM_DROW, NON_PM, PM_DROW_MUMMY, PM_DROW_ZOMBIE,
	MRACE_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MRACE_ELF, MRACE_ELF, MRACE_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   18,	   20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  7, 0,  3, 0,  3, 0 }		/* Energy */
},

{	"dwarf", "dwarven", "dwarvenkind", "Dwa",
	{0, 0},
	PM_DWARF, NON_PM, PM_DWARF_MUMMY, PM_DWARF_ZOMBIE,
	MRACE_DWARF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL,
	MRACE_DWARF, MRACE_DWARF|MRACE_GNOME, MRACE_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 16, 16, 20, 20, 16 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 3,  2, 0 },	/* Hit points */
	{  0, 0,  0, 0,  0, 0 }		/* Energy */
},

{	"elf", "elven", "elvenkind", "Elf",
	{0, 0},
	PM_ELF, NON_PM, PM_ELF_MUMMY, PM_ELF_ZOMBIE,
	MRACE_ELF | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	MRACE_ELF, MRACE_ELF, MRACE_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   18,	   20, 20, 18, 16, 18 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  1, 0 },	/* Hit points */
	{  8, 0,  3, 0,  3, 0 }		/* Energy */
},

{	"ghoul", "ghoulish", "ghoulhood", "Gul",
	{0, 0},
	PM_GHOUL, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MRACE_GHOUL | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MRACE_GHOUL, MRACE_VAMPIRE|MRACE_GHOUL, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(19),  17, 17, 15, 18, 15 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},

{	"giant", "giant", "giantkind", "Gia",
	{0, 0},
	PM_GIANT, NON_PM, PM_GIANT_MUMMY, PM_GIANT_ZOMBIE,
	MRACE_GIANT | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MRACE_GIANT, 0, MRACE_GNOME|MRACE_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(25),  17, 17, 15, 20, 16 },
	/* Init   Lower  Higher */
	{  4, 4,  0, 4,  0, 3 },	/* Hit points */
	{  1, 1,  0, 2,  0, 2 }		/* Energy */
},

{	"gnome", "gnomish", "gnomehood", "Gno",
	{0, 0},
	PM_GNOME, NON_PM, PM_GNOME_MUMMY, PM_GNOME_ZOMBIE,
	MRACE_GNOME | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL,
	MRACE_GNOME, MRACE_DWARF|MRACE_GNOME, MRACE_HUMAN|MRACE_HOBBIT,
	/*  Str    Int Wis Dex Con Cha */
	{    3,     3,  3,  3,  3,  3 },
	{   18,    16, 18, 18, 20, 20 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  2, 0,  2, 0,  2, 0 }		/* Energy */
},

{	"hobbit", "hobbit", "hobbit-kind", "Hob",
	{0, 0},
	PM_HOBBIT, NON_PM, NON_PM, NON_PM,
	MRACE_HOBBIT | ROLE_MALE|ROLE_FEMALE | ROLE_LAWFUL|ROLE_NEUTRAL,
	MRACE_HOBBIT, MRACE_HOBBIT, MRACE_GNOME|MRACE_ORC,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{  18,     16, 18, 18, 20, 20 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  4, 0,  2, 1,  2, 0 }		/* Energy */
},
{	"human", "human", "humanity", "Hum",
	{"man", "woman"},
	PM_HUMAN, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MRACE_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_LAWFUL|ROLE_NEUTRAL|ROLE_CHAOTIC,
	MRACE_HUMAN, 0, MRACE_GNOME|MRACE_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
},

{	"illithid", "illithid", "mindflayerhood", "Ill",
	{0, 0},
	PM_ILLITHID, NON_PM, NON_PM, NON_PM,
	MRACE_MFLAY | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MRACE_MFLAY, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{     18,     25, 23, 18, 17, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 2,  2, 0 },	/* Hit points */
	{  9, 0,  6, 0,  6, 0 }		/* Energy */
},

{	"incantifier", "incantifier", "wanterkind", "Inc",
	{0, 0},
	PM_INCANTIFIER, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MRACE_HUMAN | ROLE_MALE|ROLE_FEMALE |
	  ROLE_NEUTRAL|ROLE_CHAOTIC,
	MRACE_HUMAN, 0, MRACE_GNOME|MRACE_ORC|MRACE_ELF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  0, 2 },	/* Hit points */
	{  0, 0,  10, 0, 10, 0 }		/* Energy */
},

{	"kobold", "kobold", "koboldkind", "Kob",
	{0, 0},
	PM_KOBOLD, NON_PM, PM_KOBOLD_MUMMY, PM_KOBOLD_ZOMBIE,
	MRACE_KOBOLD | ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL|ROLE_CHAOTIC,
	/* "Cutebolds" from DF are typically seen as neutral */
	MRACE_KOBOLD, MRACE_KOBOLD,
	MRACE_HUMAN|MRACE_ELF|MRACE_GNOME|MRACE_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{     17,     17, 16, 20, 18, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  1, 0,  0, 2,  0, 2 }		/* Energy */
},

{	"lycanthrope", "lycanthropic", "lycanthropehood", "Lyc",
	{0, 0},
	PM_HUMAN_WEREWOLF, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MRACE_WERE | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MRACE_WERE, 0, MRACE_ELF|MRACE_GNOME|MRACE_DWARF,
	/*    Str     Int Wis Dex Con Cha */
	{      4,      1,  1,  4,  4,  2 },
	{ STR19(19),  15, 15, 20, 19, 15 },
	/* Init   Lower  Higher */
	{  4, 0,  0, 2,  2, 0 },	/* Hit points */
	{  5, 0,  4, 0,  4, 0 }		/* Energy */
},

{	"nymph", "nymphian", "nymphhood", "Nym",
	/* suggestion by BellisColdwine */
	{0, 0},
	PM_NYMPH, NON_PM, NON_PM, NON_PM,
	MRACE_NYMPH| ROLE_MALE|ROLE_FEMALE | ROLE_NEUTRAL|ROLE_CHAOTIC,
	MRACE_NYMPH, 0, MRACE_GNOME|MRACE_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{     16,     18, 18, 25, 15, 25 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  4, 0,  3, 2,  3, 2 }		/* Energy */
},

{	"ogre", "ogre", "ogrekind", "Ogr",
	{0, 0},
	PM_OGRE, NON_PM, PM_OGRE_MUMMY, PM_OGRE_ZOMBIE,
	MRACE_OGRE| ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MRACE_OGRE, 0, 
	MRACE_HUMAN|MRACE_DWARF|MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(23),  15, 15, 15, 20, 15 },
	/* Init   Lower  Higher */
	{  4, 2,  0, 3,  0, 2 },	/* Hit points */
	{  0, 0,  0, 1,  0, 1 }		/* Energy */
},

{	"orc", "orcish", "orcdom", "Orc",
	{0, 0},
	PM_ORC, NON_PM, PM_ORC_MUMMY, PM_ORC_ZOMBIE,
	MRACE_ORC | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MRACE_ORC, 0, MRACE_HUMAN|MRACE_ELF|MRACE_DWARF|MRACE_HOBBIT,
	/*  Str    Int Wis Dex Con Cha */
	{   3,      3,  3,  3,  3,  3 },
	{STR18(50), 16, 16, 18, 18, 16 },
	/* Init   Lower  Higher */
	{  1, 0,  0, 1,  0, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},

{	"troll", "troll", "trollkind", "Tro",
	{0, 0},
	PM_TROLL, NON_PM, PM_TROLL_MUMMY, PM_TROLL_ZOMBIE,
	MRACE_TROLL | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MRACE_TROLL, 0, MRACE_ELF|MRACE_HUMAN|MRACE_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR19(23),  17, 15, 16, 25, 15 },
	/* Init   Lower  Higher */
	{  6, 0,  0, 3,  3, 0 },	/* Hit points */
	{  1, 0,  1, 0,  1, 0 }		/* Energy */
},

{	"vampire", "vampiric", "vampirehood", "Vam",
	{0, 0},
	PM_VAMPIRE, NON_PM, PM_HUMAN_MUMMY, PM_HUMAN_ZOMBIE,
	MRACE_VAMPIRE | ROLE_MALE|ROLE_FEMALE | ROLE_CHAOTIC,
	MRACE_VAMPIRE, 0, MRACE_ELF|MRACE_GNOME|MRACE_HOBBIT|MRACE_DWARF|MRACE_ORC,
	/*    Str     Int Wis Dex Con Cha */
	{      4,      0,  0,  4,  3,  4 },
	{ STR19(19),  18, 18, 20, 20, 20 },
	/* Init   Lower  Higher */
	{  3, 0,  0, 3,  2, 0 },	/* Hit points */
	{  5, 0,  4, 0,  4, 0 }		/* Energy */
},
/* Array terminator */
{ 0, 0, 0, 0 }};


/* The player's race, created at runtime from initial
 * choices.  This may be munged in role_init().
 */
struct Race urace =
{	"something", "undefined", "something", "Xxx",
	{0, 0},
	NON_PM, NON_PM, NON_PM, NON_PM,
	0, 0, 0, 0,
	/*    Str     Int Wis Dex Con Cha */
	{      3,      3,  3,  3,  3,  3 },
	{ STR18(100), 18, 18, 18, 18, 18 },
	/* Init   Lower  Higher */
	{  2, 0,  0, 2,  1, 0 },	/* Hit points */
	{  1, 0,  2, 0,  2, 0 }		/* Energy */
};

/* Table of all genders */
const struct Gender genders[] = {
	{"male",	"he",	"him",	"his",	"Mal",	ROLE_MALE},
	{"female",	"she",	"her",	"her",	"Fem",	ROLE_FEMALE},
	{"neuter",	"it",	"it",	"its",	"Ntr",	ROLE_NEUTER}
};

#ifdef MAC_MPW
const size_t maxGender = sizeof genders/sizeof genders[0];
#endif /* MAC_MPW */

/* Table of all alignments */
const struct Align aligns[] = {
	{"law",		"lawful",	"Law",	ROLE_LAWFUL,	A_LAWFUL},
	{"balance",	"neutral",	"Neu",	ROLE_NEUTRAL,	A_NEUTRAL},
	{"chaos",	"chaotic",	"Cha",	ROLE_CHAOTIC,	A_CHAOTIC},
	{"evil",	"unaligned",	"Una",	0,		A_NONE}
};

STATIC_DCL char * FDECL(promptsep, (char *, int));
STATIC_DCL int FDECL(role_gendercount, (int));
STATIC_DCL int FDECL(race_alignmentcount, (int));

/* used by str2XXX() */
static char NEARDATA randomstr[] = "random";

#ifdef MAC_MPW
const size_t maxAlign = sizeof aligns/sizeof aligns[0];
#endif /* MAC_MPW */

boolean
validrole(rolenum)
	int rolenum;
{
	return (rolenum >= 0 && rolenum < SIZE(roles)-1);
}


int
randrole()
{
	return (rn2(SIZE(roles)-1));
}


int
str2role(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; roles[i].name.m; i++) {
	    /* Does it match the male name? */
	    if (!strncmpi(str, roles[i].name.m, len))
		return i;
	    /* Or the female name? */
	    if (roles[i].name.f && !strncmpi(str, roles[i].name.f, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, roles[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validrace(rolenum, racenum)
	int rolenum, racenum;
{
	/* Assumes validrole */
	/* WAC -- checks ROLE_GENDMASK and ROLE_ALIGNMASK as well (otherwise, there 
	 * might not be an allowed gender or alignment for that role
	 */
	return (racenum >= 0 && racenum < SIZE(races)-1 &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_RACEMASK) &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_GENDMASK) &&
		(roles[rolenum].allow & races[racenum].allow & ROLE_ALIGNMASK));
}


int
randrace(rolenum)
	int rolenum;
{
	int i, n = 0;

	/* Count the number of valid races */
	for (i = 0; races[i].noun; i++)
/*	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK)*/
	    if (validrace(rolenum,i))
	    	n++;

	/* Pick a random race */
	/* Use a factor of 100 in case of bad random number generators */
	if (n) n = rn2(n*100)/100;
	for (i = 0; races[i].noun; i++)
/*	    if (roles[rolenum].allow & races[i].allow & ROLE_RACEMASK) {*/
	    if (validrace(rolenum,i)) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role has no permitted races? */
	return (rn2(SIZE(races)-1));
}

/*
 * [ALI] Find the player equivalent race for a monster from its M2 flags.
 */

int
mrace2race(mflagsr)
	int mflagsr;
{
	int i;

	/* Look for a race with the correct selfmask */
	for (i = 0; races[i].noun; i++)
	    if (mflagsr & races[i].selfmask) {
		/* Where more than one player race has the same monster race,
		 * return the base race.
		 */
		if (mflagsr & MRACE_HUMAN && races[i].malenum != PM_HUMAN)
		    continue;
		if (mflagsr & MRACE_ELF && races[i].malenum != PM_ELF)
		    continue;
		return i;
	    }
	return ROLE_NONE;
}

int
str2race(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; races[i].noun; i++) {
	    /* Does it match the noun? */
	    if (!strncmpi(str, races[i].noun, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, races[i].filecode))
		return i;
	}

	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validgend(rolenum, racenum, gendnum)
	int rolenum, racenum, gendnum;
{
	/* Assumes validrole and validrace */
	return (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		(roles[rolenum].allow & races[racenum].allow &
		 genders[gendnum].allow & ROLE_GENDMASK));
}


int
randgend(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid genders */
	for (i = 0; i < ROLE_GENDERS; i++)
/*	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) */
	    if (validgend(rolenum, racenum, i))
	    	n++;

	/* Pick a random gender */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_GENDERS; i++)
/*	    if (roles[rolenum].allow & races[racenum].allow &
	    		genders[i].allow & ROLE_GENDMASK) {*/
	    if (validgend(rolenum, racenum, i)) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted genders? */
	return (rn2(ROLE_GENDERS));
}


int
str2gend(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_GENDERS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, genders[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, genders[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}


boolean
validalign(rolenum, racenum, alignnum)
	int rolenum, racenum, alignnum;
{
	/* Assumes validrole and validrace */
	return (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		(roles[rolenum].allow & races[racenum].allow &
		 aligns[alignnum].allow & ROLE_ALIGNMASK));
}


int
randalign(rolenum, racenum)
	int rolenum, racenum;
{
	int i, n = 0;

	/* Count the number of valid alignments */
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK)
	    	n++;

	/* Pick a random alignment */
	if (n) n = rn2(n);
	for (i = 0; i < ROLE_ALIGNS; i++)
	    if (roles[rolenum].allow & races[racenum].allow &
	    		aligns[i].allow & ROLE_ALIGNMASK) {
	    	if (n) n--;
	    	else return (i);
	    }

	/* This role/race has no permitted alignments? */
	return (rn2(ROLE_ALIGNS));
}


int
str2align(str)
	char *str;
{
	int i, len;

	/* Is str valid? */
	if (!str || !str[0])
	    return ROLE_NONE;

	/* Match as much of str as is provided */
	len = strlen(str);
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    /* Does it match the adjective? */
	    if (!strncmpi(str, aligns[i].adj, len))
		return i;
	    /* Or the filecode? */
	    if (!strcmpi(str, aligns[i].filecode))
		return i;
	}
	if ((len == 1 && (*str == '*' || *str == '@')) ||
		!strncmpi(str, randomstr, len))
	    return ROLE_RANDOM;

	/* Couldn't find anything appropriate */
	return ROLE_NONE;
}

/* is rolenum compatible with any racenum/gendnum/alignnum constraints? */
boolean
ok_role(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    long allow;

    if (rolenum >= 0 && rolenum < SIZE(roles)-1) {
	allow = roles[rolenum].allow;

	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;

	if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(roles)-1; i++) {
	    allow = roles[i].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
		continue;
	    if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		    !(allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random role subject to any racenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a role is returned only if there is  */
/* a single possibility */
int
pick_role(racenum, gendnum, alignnum, pickhow)
int racenum, gendnum, alignnum, pickhow;
{
    int i;
    int roles_ok = 0;

    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum))
	    roles_ok++;
    }
    if (roles_ok == 0 || (roles_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    roles_ok = rn2(roles_ok);
    for (i = 0; i < SIZE(roles)-1; i++) {
	if (ok_role(i, racenum, gendnum, alignnum)) {
	    if (roles_ok == 0)
		return i;
	    else
		roles_ok--;
	}
    }
    return ROLE_NONE;
}

/* is racenum compatible with any rolenum/gendnum/alignnum constraints? */
boolean
ok_race(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    long allow;

    if (racenum >= 0 && racenum < SIZE(races)-1) {
	allow = races[racenum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		!(allow & genders[gendnum].allow & ROLE_GENDMASK))
	    return FALSE;
	if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		!(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
	    return FALSE;

	if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < SIZE(races)-1; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (gendnum >= 0 && gendnum < ROLE_GENDERS &&
		    !(allow & genders[gendnum].allow & ROLE_GENDMASK))
		continue;
	    if (alignnum >= 0 && alignnum < ROLE_ALIGNS &&
		    !(allow & aligns[alignnum].allow & ROLE_ALIGNMASK))
		continue;
	    if (!(allow & ROLE_RACEMASK) || !(allow & ROLE_GENDMASK) ||
		    !(allow & ROLE_ALIGNMASK))
		continue;
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random race subject to any rolenum/gendnum/alignnum constraints */
/* If pickhow == PICK_RIGID a race is returned only if there is  */
/* a single possibility */
int
pick_race(rolenum, gendnum, alignnum, pickhow)
int rolenum, gendnum, alignnum, pickhow;
{
    int i;
    int races_ok = 0;

    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum))
	    races_ok++;
    }
    if (races_ok == 0 || (races_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    races_ok = rn2(races_ok);
    for (i = 0; i < SIZE(races)-1; i++) {
	if (ok_race(rolenum, i, gendnum, alignnum)) {
	    if (races_ok == 0)
		return i;
	    else
		races_ok--;
	}
    }
    return ROLE_NONE;
}

/* is gendnum compatible with any rolenum/racenum/alignnum constraints? */
/* gender and alignment are not comparable (and also not constrainable) */
boolean
ok_gend(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    long allow;

    if (gendnum >= 0 && gendnum < ROLE_GENDERS) {
	allow = genders[gendnum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;
		    
	if (!(allow & ROLE_GENDMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_GENDERS; i++) {
	    allow = genders[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (allow & ROLE_GENDMASK)
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random gender subject to any rolenum/racenum/alignnum constraints */
/* gender and alignment are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID a gender is returned only if there is  */
/* a single possibility */
int
pick_gend(rolenum, racenum, alignnum, pickhow)
int rolenum, racenum, alignnum, pickhow;
{
    int i;
    int gends_ok = 0;

    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum))
	    gends_ok++;
    }
    if (gends_ok == 0 || (gends_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    gends_ok = rn2(gends_ok);
    for (i = 0; i < ROLE_GENDERS; i++) {
	if (ok_gend(rolenum, racenum, i, alignnum)) {
	    if (gends_ok == 0)
		return i;
	    else
		gends_ok--;
	}
    }
    return ROLE_NONE;
}

/* is alignnum compatible with any rolenum/racenum/gendnum constraints? */
/* alignment and gender are not comparable (and also not constrainable) */
boolean
ok_align(rolenum, racenum, gendnum, alignnum)
int rolenum, racenum, gendnum, alignnum;
{
    int i;
    long allow;

    if (alignnum >= 0 && alignnum < ROLE_ALIGNS) {
	allow = aligns[alignnum].allow;

	if (rolenum >= 0 && rolenum < SIZE(roles)-1)
	    allow &= roles[rolenum].allow;
	if (racenum >= 0 && racenum < SIZE(races)-1)
	    allow &= races[racenum].allow;
		    
	if (!(allow & ROLE_ALIGNMASK))
	    return FALSE;
	return TRUE;
    } else {
	for (i = 0; i < ROLE_ALIGNS; i++) {
	    allow = races[i].allow;
	    if (rolenum >= 0 && rolenum < SIZE(roles)-1)
		allow &= roles[rolenum].allow;
	    if (racenum >= 0 && racenum < SIZE(races)-1)
		allow &= races[racenum].allow;
	    if (allow & ROLE_ALIGNMASK)
	    return TRUE;
	}
	return FALSE;
    }
}

/* pick a random alignment subject to any rolenum/racenum/gendnum constraints */
/* alignment and gender are not comparable (and also not constrainable) */
/* If pickhow == PICK_RIGID an alignment is returned only if there is  */
/* a single possibility */
int
pick_align(rolenum, racenum, gendnum, pickhow)
int rolenum, racenum, gendnum, pickhow;
{
    int i;
    int aligns_ok = 0;

    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i))
	    aligns_ok++;
    }
    if (aligns_ok == 0 || (aligns_ok > 1 && pickhow == PICK_RIGID))
	return ROLE_NONE;
    aligns_ok = rn2(aligns_ok);
    for (i = 0; i < ROLE_ALIGNS; i++) {
	if (ok_align(rolenum, racenum, gendnum, i)) {
	    if (aligns_ok == 0)
		return i;
	    else
		aligns_ok--;
	}
    }
    return ROLE_NONE;
}

void
rigid_role_checks()
{
    /* Some roles are limited to a single race, alignment, or gender and
     * calling this routine prior to XXX_player_selection() will help
     * prevent an extraneous prompt that actually doesn't allow
     * you to choose anything further. Note the use of PICK_RIGID which
     * causes the pick_XX() routine to return a value only if there is one
     * single possible selection, otherwise it returns ROLE_NONE.
     *
     */
    if (flags.initrole == ROLE_RANDOM) {
	/* If the role was explicitly specified as ROLE_RANDOM
	 * via -uXXXX-@ then choose the role in here to narrow down
	 * later choices. Pick a random role in this case.
	 */
	flags.initrole = pick_role(flags.initrace, flags.initgend,
					flags.initalign, PICK_RANDOM);
	if (flags.initrole < 0)
	    flags.initrole = randrole();
    }
    if (flags.initrole != ROLE_NONE) {
	if (flags.initrace == ROLE_NONE)
	     flags.initrace = pick_race(flags.initrole, flags.initgend,
						flags.initalign, PICK_RIGID);
	if (flags.initalign == ROLE_NONE)
	     flags.initalign = pick_align(flags.initrole, flags.initrace,
						flags.initgend, PICK_RIGID);
	if (flags.initgend == ROLE_NONE)
	     flags.initgend = pick_gend(flags.initrole, flags.initrace,
						flags.initalign, PICK_RIGID);
    }
}

#define BP_ALIGN	0
#define BP_GEND		1
#define BP_RACE		2
#define BP_ROLE		3
#define NUM_BP		4

STATIC_VAR char pa[NUM_BP], post_attribs;

STATIC_OVL char *
promptsep(buf, num_post_attribs)
char *buf;
int num_post_attribs;
{
	const char *conj = "and ";
	if (num_post_attribs > 1
	    && post_attribs < num_post_attribs && post_attribs > 1)
	 	Strcat(buf, ","); 
	Strcat(buf, " ");
	--post_attribs;
	if (!post_attribs && num_post_attribs > 1) Strcat(buf, conj);
	return buf;
}

STATIC_OVL int
role_gendercount(rolenum)
int rolenum;
{
	int gendcount = 0;
	if (validrole(rolenum)) {
		if (roles[rolenum].allow & ROLE_MALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_FEMALE) ++gendcount;
		if (roles[rolenum].allow & ROLE_NEUTER) ++gendcount;
	}
	return gendcount;
}

STATIC_OVL int
race_alignmentcount(racenum)
int racenum;
{
	int aligncount = 0;
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (races[racenum].allow & ROLE_CHAOTIC) ++aligncount;
		if (races[racenum].allow & ROLE_LAWFUL) ++aligncount;
		if (races[racenum].allow & ROLE_NEUTRAL) ++aligncount;
	}
	return aligncount;
}

char *
root_plselection_prompt(suppliedbuf, buflen, rolenum, racenum, gendnum, alignnum)
char *suppliedbuf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	int k, gendercount = 0, aligncount = 0;
	char buf[BUFSZ];
	static char err_ret[] = " character's";
	boolean donefirst = FALSE;

	if (!suppliedbuf || buflen < 1) return err_ret;

	/* initialize these static variables each time this is called */
	post_attribs = 0;
	for (k=0; k < NUM_BP; ++k)
		pa[k] = 0;
	buf[0] = '\0';
	*suppliedbuf = '\0';
	
	/* How many alignments are allowed for the desired race? */
	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM)
		aligncount = race_alignmentcount(racenum);

	if (alignnum != ROLE_NONE && alignnum != ROLE_RANDOM) {
		/* if race specified, and multiple choice of alignments for it */
		if ((racenum >= 0) && (aligncount > 1)) {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
			donefirst = TRUE;
		} else {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, aligns[alignnum].adj);
			donefirst = TRUE;
		}
	} else {
		/* if alignment not specified, but race is specified
			and only one choice of alignment for that race then
			don't include it in the later list */
		if ((((racenum != ROLE_NONE && racenum != ROLE_RANDOM) &&
			ok_race(rolenum, racenum, gendnum, alignnum))
		      && (aligncount > 1))
		     || (racenum == ROLE_NONE || racenum == ROLE_RANDOM)) {
			pa[BP_ALIGN] = 1;
			post_attribs++;
		}
	}
	/* <your lawful> */

	/* How many genders are allowed for the desired role? */
	if (validrole(rolenum))
		gendercount = role_gendercount(rolenum);

	if (gendnum != ROLE_NONE  && gendnum != ROLE_RANDOM) {
		if (validrole(rolenum)) {
		     /* if role specified, and multiple choice of genders for it,
			and name of role itself does not distinguish gender */
			if ((rolenum != ROLE_NONE) && (gendercount > 1)
						&& !roles[rolenum].name.f) {
				if (donefirst) Strcat(buf, " ");
				Strcat(buf, genders[gendnum].adj);
				donefirst = TRUE;
			}
	        } else {
			if (donefirst) Strcat(buf, " ");
	        	Strcat(buf, genders[gendnum].adj);
			donefirst = TRUE;
	        }
	} else {
		/* if gender not specified, but role is specified
			and only one choice of gender then
			don't include it in the later list */
		if ((validrole(rolenum) && (gendercount > 1)) || !validrole(rolenum)) {
			pa[BP_GEND] = 1;
			post_attribs++;
		}
	}
	/* <your lawful female> */

	if (racenum != ROLE_NONE && racenum != ROLE_RANDOM) {
		if (validrole(rolenum) && ok_race(rolenum, racenum, gendnum, alignnum)) {
			if (donefirst) Strcat(buf, " "); 
			Strcat(buf, (rolenum == ROLE_NONE) ?
				races[racenum].noun :
				races[racenum].adj);
			donefirst = TRUE;
		} else if (!validrole(rolenum)) {
			if (donefirst) Strcat(buf, " ");
			Strcat(buf, races[racenum].noun);
			donefirst = TRUE;
		} else {
			pa[BP_RACE] = 1;
			post_attribs++;
		}
	} else {
		pa[BP_RACE] = 1;
		post_attribs++;
	}
	/* <your lawful female gnomish> || <your lawful female gnome> */

	if (validrole(rolenum)) {
		if (donefirst) Strcat(buf, " ");
		if (gendnum != ROLE_NONE) {
		    if (gendnum == 1  && roles[rolenum].name.f)
			Strcat(buf, roles[rolenum].name.f);
		    else
  			Strcat(buf, roles[rolenum].name.m);
		} else {
			if (roles[rolenum].name.f) {
				Strcat(buf, roles[rolenum].name.m);
				Strcat(buf, "/");
				Strcat(buf, roles[rolenum].name.f);
			} else 
				Strcat(buf, roles[rolenum].name.m);
		}
		donefirst = TRUE;
	} else if (rolenum == ROLE_NONE) {
		pa[BP_ROLE] = 1;
		post_attribs++;
	}
	
	if ((racenum == ROLE_NONE || racenum == ROLE_RANDOM) && !validrole(rolenum)) {
		if (donefirst) Strcat(buf, " ");
		Strcat(buf, "character");
		donefirst = TRUE;
	}
	/* <your lawful female gnomish cavewoman> || <your lawful female gnome>
	 *    || <your lawful female character>
	 */
	if (buflen > (int) (strlen(buf) + 1)) {
		Strcpy(suppliedbuf, buf);
		return suppliedbuf;
	} else
		return err_ret;
}

char *
build_plselection_prompt(buf, buflen, rolenum, racenum, gendnum, alignnum)
char *buf;
int buflen, rolenum, racenum, gendnum, alignnum;
{
	const char *defprompt = "Shall I pick a character for you? [ynq] ";
	int num_post_attribs = 0;
	char tmpbuf[BUFSZ];
	
	if (buflen < QBUFSZ)
		return (char *)defprompt;

	Strcpy(tmpbuf, "Shall I pick ");
	if (racenum != ROLE_NONE || validrole(rolenum))
		Strcat(tmpbuf, "your ");
	else {
		Strcat(tmpbuf, "a ");
	}
	/* <your> */

	(void)  root_plselection_prompt(eos(tmpbuf), buflen - strlen(tmpbuf),
					rolenum, racenum, gendnum, alignnum);
	Sprintf(buf, "%s", s_suffix(tmpbuf));

	/* buf should now be:
	 * < your lawful female gnomish cavewoman's> || <your lawful female gnome's>
	 *    || <your lawful female character's>
	 *
         * Now append the post attributes to it
	 */

	num_post_attribs = post_attribs;
	if (post_attribs) {
		if (pa[BP_RACE]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, "race");
		}
		if (pa[BP_ROLE]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, "role");
		}
		if (pa[BP_GEND]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, "gender");
		}
		if (pa[BP_ALIGN]) {
			(void) promptsep(eos(buf), num_post_attribs);
			Strcat(buf, "alignment");
		}
	}
	Strcat(buf, " for you? [ynq] ");
	return buf;
}

#undef BP_ALIGN
#undef BP_GEND
#undef BP_RACE
#undef BP_ROLE
#undef NUM_BP

void
plnamesuffix()
{
	char *sptr, *eptr;
	int i;

	/* Look for tokens delimited by '-' */
	if ((eptr = index(plname, '-')) != (char *) 0)
	    *eptr++ = '\0';
	while (eptr) {
	    /* Isolate the next token */
	    sptr = eptr;
	    if ((eptr = index(sptr, '-')) != (char *)0)
		*eptr++ = '\0';

	    /* Try to match it to something */
	    if ((i = str2role(sptr)) != ROLE_NONE)
		flags.initrole = i;
	    else if ((i = str2race(sptr)) != ROLE_NONE)
		flags.initrace = i;
	    else if ((i = str2gend(sptr)) != ROLE_NONE)
		flags.initgend = i;
	    else if ((i = str2align(sptr)) != ROLE_NONE)
		flags.initalign = i;
	}
	if(!plname[0]) {
	    askname();
	    plnamesuffix();
	}

	/* commas in the plname confuse the record file, convert to spaces */
	for (sptr = plname; *sptr; sptr++) {
		if (*sptr == ',') *sptr = ' ';
	}
}


/*
 *	Special setup modifications here:
 *
 *	Unfortunately, this is going to have to be done
 *	on each newgame or restore, because you lose the permonst mods
 *	across a save/restore.  :-)
 *
 *	1 - The Rogue Leader is the Tourist Nemesis.
 *	2 - Priests start with a random alignment - convert the leader and
 *	    guardians here.
 *	3 - Elves can have one of two different leaders, but can't work it
 *	    out here because it requires hacking the level file data (see
 *	    sp_lev.c).
 *
 * This code also replaces quest_init().
 */
void
role_init()
{
	int alignmnt;

	/* Strip the role letter out of the player name.
	 * This is included for backwards compatibility.
	 */
	plnamesuffix();

	/* Check for a valid role.  Try flags.initrole first. */
	if (!validrole(flags.initrole)) {
	    /* Try the player letter second */
	    if ((flags.initrole = str2role(pl_character)) < 0)
	    	/* None specified; pick a random role */
	    	flags.initrole = randrole();
	}

	/* We now have a valid role index.  Copy the role name back. */
	/* This should become OBSOLETE */
	Strcpy(pl_character, roles[flags.initrole].name.m);
	pl_character[PL_CSIZ-1] = '\0';

	/* Check for a valid race */
	if (!validrace(flags.initrole, flags.initrace))
	    flags.initrace = randrace(flags.initrole);

	/* Check for a valid gender.  If new game, check both initgend
	 * and female.  On restore, assume flags.female is correct. */
	if (flags.pantheon == -1) {	/* new game */
	    if (!validgend(flags.initrole, flags.initrace, flags.female))
		flags.female = !flags.female;
	}
	if (!validgend(flags.initrole, flags.initrace, flags.initgend))
	    /* Note that there is no way to check for an unspecified gender. */
	    flags.initgend = flags.female;

	/* Check for a valid alignment */
	if (!validalign(flags.initrole, flags.initrace, flags.initalign))
	    /* Pick a random alignment */
	    flags.initalign = randalign(flags.initrole, flags.initrace);
	alignmnt = aligns[flags.initalign].value;

	/* Initialize urole and urace */
	urole = roles[flags.initrole];
	urace = races[flags.initrace];

	/* kick it over to alternate-alignment role */
	if (alignmnt == A_CHAOTIC && Role_if(PM_KNIGHT)) {
		urole = align_roles[0];
	}

	/* Fix up the quest leader */
	if (urole.ldrnum != NON_PM) {
	    mons[urole.ldrnum].msound = MS_LEADER;
	    mons[urole.ldrnum].mflags2 |= (M2_PEACEFUL);
	    mons[urole.ldrnum].mflags3 |= M3_CLOSE;
	    mons[urole.ldrnum].maligntyp = alignmnt * 3;
	}

	/* Fix up the quest guardians */
	if (urole.guardnum != NON_PM) {
	    mons[urole.guardnum].mflags2 |= (M2_PEACEFUL);
	    mons[urole.guardnum].maligntyp = alignmnt * 3;
	}

	/* Fix up the quest nemesis */
	if (urole.neminum != NON_PM) {
	    mons[urole.neminum].msound = MS_NEMESIS;
	    mons[urole.neminum].mflags2 &= ~(M2_PEACEFUL);
	    mons[urole.neminum].mflags2 |= (M2_NASTY|M2_STALK|M2_HOSTILE);
	    mons[urole.neminum].mflags3 |= M3_WANTSARTI | M3_WAITFORU;
	}

	/* Fix up the god names */
	if (flags.pantheon == -1) {		/* new game */

		    flags.pantheon = flags.initrole;	/* use own gods */
		    while (!roles[flags.pantheon].lgod)	/* unless they're missing */
			flags.pantheon = randrole();
	}
	if (!urole.lgod) {
	    urole.lgod = roles[flags.pantheon].lgod;
	    urole.ngod = roles[flags.pantheon].ngod;
	    urole.cgod = roles[flags.pantheon].cgod;
	}

#if 0 /* Now in polyself.c, init_uasmon() */
	/* Fix up infravision */
	if (mons[urace.malenum].mflags3 & M3_INFRAVISION) {
	    /* although an infravision intrinsic is possible, infravision
	     * is purely a property of the physical race.  This means that we
	     * must put the infravision flag in the player's current race
	     * (either that or have separate permonst entries for
	     * elven/non-elven members of each class).  The side effect is that
	     * all NPCs of that class will have (probably bogus) infravision,
	     * but since infravision has no effect for NPCs anyway we can
	     * ignore this.
	     */
	    mons[urole.malenum].mflags3 |= M3_INFRAVISION;
	    if (urole.femalenum != NON_PM)
	    	mons[urole.femalenum].mflags3 |= M3_INFRAVISION;
	}
#endif

	/* Artifacts are fixed in hack_artifacts() */

	/* Success! */
	return;
}

const char *
Hello(mtmp)
struct monst *mtmp;
{
	switch (Role_switch) {
	case PM_KNIGHT:
	case PM_PALADIN:
	    return ("Salutations"); /* Olde English */
	case PM_MONK:
	    return ("Namaste");	/* Sanskrit */
	case PM_SAMURAI:
	case PM_NINJA:
	    return (mtmp && mtmp->data == &mons[PM_SHOPKEEPER] ?
	    		"Irasshaimase" : "Konnichi wa"); /* Japanese */
	case PM_PIRATE:
	case PM_CORSAIR:
		return ("Ahoy");
#ifdef TOURIST
	case PM_TOURIST:
	    return ("Aloha");       /* Hawaiian */
#endif
	case PM_VALKYRIE:
	    return (
#ifdef MAIL
	    		mtmp && mtmp->data == &mons[PM_MAIL_DAEMON] ? "Hallo" :
#endif
	    		"Velkommen");   /* Norse */
	default:
	    return ("Hello");
	}
}

const char *
Goodbye()
{
	switch (Role_switch) {
	case PM_KNIGHT:
	case PM_PALADIN:
	    return ("Fare thee well");  /* Olde English */
	case PM_MONK:
	    return ("Punardarsanaya"); /* Sanskrit */
	case PM_SAMURAI:
	case PM_NINJA:
	    return ("Sayonara");        /* Japanese */
#ifdef TOURIST
	case PM_TOURIST:
	    return ("Aloha");           /* Hawaiian */
#endif
	case PM_VALKYRIE:
	    return ("Farvel");          /* Norse */
	default:
	    return ("Goodbye");
	}
}

/* role.c */
