/*	SCCS Id: @(#)u_init.c	3.4	2002/10/22	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

struct trobj {
	short trotyp;
	schar trspe;
	char trclass;
	Bitfield(trquan,6);
	Bitfield(trbless,2);
};

STATIC_DCL void FDECL(ini_inv, (struct trobj *));
STATIC_DCL void FDECL(knows_object,(int));
STATIC_DCL void FDECL(knows_class,(CHAR_P));
STATIC_DCL void FDECL(augment_skill_cap,(int, int, int, int));
STATIC_DCL boolean FDECL(restricted_spell_discipline, (int));

#define UNDEF_TYP	0
#define UNDEF_SPE	'\177'
#define UNDEF_BLESS	2

/*
 *	Initial inventory for the various roles.
 */

static struct trobj Archeologist[] = {
#define A_BOOK          4
	/* if adventure has a name...  idea from tan@uvm-gen */
	{ BULLWHIP, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ LEATHER_JACKET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FEDORA, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },        
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 2, UNDEF_BLESS },
	{ PICK_AXE, 0, TOOL_CLASS, 1, UNDEF_BLESS },
	{ TINNING_KIT, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ TOUCHSTONE, 0, GEM_CLASS, 1, 0 },
	{ SACK, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Barbarian[] = {
#define B_MAJOR	0	/* two-handed sword or battle-axe  */
#define B_MINOR	1	/* matched with axe or short sword */
	{ TWO_HANDED_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ AXE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ RING_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Bard[] = {
#define BARD_INSTR 1
#define BARD_BOOZE 5
#define BARD_WHISTLE 6
	{ SILVER_DAGGER, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ WOODEN_HARP, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ LEATHER_CLOAK, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ APPLE, 0, FOOD_CLASS, 3, 0 },
	{ ORANGE, 0, FOOD_CLASS, 3, 0 },
	{ POT_BOOZE, 0, POTION_CLASS, 1, UNDEF_BLESS },
	{ TIN_WHISTLE, 0, TOOL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Cave_man[] = {
#define C_AMMO	2
	{ CLUB, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ SLING, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ FLINT, 0, GEM_CLASS, 15, UNDEF_BLESS },	/* quan is variable */
	{ ROCK, 0, GEM_CLASS, 3, 0 },			/* yields 18..33 */
	{ LEATHER_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Chef[] = { /* 5lo: New role */
	{ KNIFE, 1, WEAPON_CLASS, 1, 1 },
	{ LEATHER_ARMOR, 1, ARMOR_CLASS, 1, 1 },
	{ DENTED_POT, 2, ARMOR_CLASS, 1, 1 },
	{ FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
	/* Give him some herbs and spices... */
	{ SPRIG_OF_WOLFSBANE, 0, FOOD_CLASS, 1, 0 },
	{ CLOVE_OF_GARLIC, 0, FOOD_CLASS, 1, 0 },
	{ POT_WATER, UNDEF_SPE, POTION_CLASS, 2, 0 },
	{ POT_BOOZE, UNDEF_SPE, POTION_CLASS, 2, UNDEF_BLESS },
	{ CAN_OF_GREASE, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ TOWEL, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ TINNING_KIT, UNDEF_SPE, TOOL_CLASS, 1, 1 },
	{ TIN_OPENER, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Electric_Mage[] = {
#define E_BOOK		9
	{ QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },        /* for dealing with ghosts */
	{ STUDDED_LEATHER_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
	{ WAN_LIGHTNING, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ SPE_SHOCKING_SPHERE, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ SPE_LIGHTNING, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
  	{ 0, 0, 0, 0, 0 }
};

static struct trobj Acid_Mage[] = {
#define ACID_BOOK	9
	{ QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },        /* for dealing with ghosts */
	{ STUDDED_LEATHER_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
	{ WAN_ACID, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ SPE_ACID_SPHERE, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ SPE_ACID_STREAM, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
  	{ 0, 0, 0, 0, 0 }
};

static struct trobj Flame_Mage[] = {
#define F_BOOK          9
	{ QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },        /* for dealing with ghosts */
	{ STUDDED_LEATHER_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
	{ WAN_FIRE, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ SPE_FLAME_SPHERE, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ SPE_FIREBALL, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
  	{ 0, 0, 0, 0, 0 }
};
#ifdef CONVICT
static struct trobj Convict[] = {
	{ ROCK, 0, GEM_CLASS, 1, 0 },
	{ STRIPED_SHIRT, 0, ARMOR_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif  /* CONVICT */

static struct trobj Lunatic[] = { /* 5lo: Changing to starting objects from Slash 6 Lycanthrope role */
	{ ORCISH_SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ POT_SICKNESS, 0, POTION_CLASS, 2, 0 },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 2, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Drunk[] = {
	{ RUBBER_HOSE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ POT_BOOZE, UNDEF_SPE, POTION_CLASS, 5, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 5, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 5, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 5, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0, }
};

static struct trobj Officer[] = {
	{ CLUB, 0, WEAPON_SYM, 1, UNDEF_BLESS },
	{ PISTOL, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
#define COP_BULLET 2
	{ BULLET, 0, WEAPON_CLASS, 40, 0 },
	{ LEATHER_ARMOR, 0, ARMOR_SYM, 1, UNDEF_BLESS },
#if 0 /* 5lo: Removed as these were to simulate firearms in Nethack-- */
	{ WAN_MAGIC_MISSILE, UNDEF_SPE, WAND_SYM, 1, UNDEF_BLESS },
	{ WAN_STRIKING, UNDEF_SPE, WAND_SYM, 1, UNDEF_BLESS },
#endif
	{ DOUGHNUT, 0, FOOD_SYM, 6, 0 },
	{ 0, 0, 0, 0, 0, }
};

static struct trobj Musician[] = {
	{ FEDORA, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ HAWAIIAN_SHIRT, 2, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ MAGIC_FLUTE, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ MAGIC_HARP, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ DRUM_OF_EARTHQUAKE, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ FROST_HORN, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ FIRE_HORN, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0, }
};

static struct trobj Undertaker[] = {
	{ SCALPEL, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ EXPENSIVE_SUIT, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ WAN_UNDEAD_TURNING, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
//	{ WAN_UNDEAD_TURNING, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ TINNING_KIT, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ BRASS_LANTERN, 1, TOOL_CLASS, 1, 0 }, /* Graveyards are dark places */
	{ PICK_AXE, 0, TOOL_CLASS, 1, UNDEF_BLESS }, /* They dig graves after all */
	{ 0, 0, 0, 0, 0, }
};

static struct trobj Zookeeper[] = {
	{ LONG_BULLWHIP, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ TRIPE_RATION, 0, FOOD_CLASS, 5, UNDEF_BLESS },
	{ ASIAN_PEAR, 0, FOOD_CLASS, 3, 0 },
	{ EUCALYPTUS_LEAF, 0, FOOD_CLASS, 1, 0 },
	{ SCR_TAMING, UNDEF_SPE, SCROLL_CLASS, 3, 1 },
	{ LEASH, 0, TOOL_CLASS, 3, 0 },
	{ 0, 0, 0, 0, 0, }
};
static struct trobj Firefighter[] = {
	{ AXE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ FIRE_HELMET, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ ASBESTOS_JACKET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ POT_WATER, 0, POTION_CLASS, 5, 0 },
	{ SCR_FIRE, UNDEF_SPE, SCROLL_CLASS, 4, UNDEF_BLESS },
	{ WAN_FIRE, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0, }
};

static struct trobj Locksmith[] = {
	{ DAGGER, 0, WEAPON_CLASS, 5, UNDEF_BLESS },
	{ WAN_OPENING, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ WAN_LOCKING, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ SKELETON_KEY, 0, TOOL_CLASS, 1, 0 },
	{ LOCK_PICK, 9, TOOL_CLASS, 1, 0 },
	{ SACK, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0, }
};

static struct trobj Ninja[] = {
#define NIN_SHURIKEN 1
	{ SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },      /* the wakizashi */
	{ SHURIKEN, 0, WEAPON_CLASS, 9, UNDEF_BLESS },        /* quan is variable */
	{ LEATHER_CLOAK, 0, ARMOR_CLASS, 1, 0 },
	{ LOW_BOOTS, 0, ARMOR_CLASS, 1, 0 },
	{ POT_SICKNESS, 0, POTION_CLASS, 1, 0 },
	{ POT_INVISIBILITY, UNDEF_SPE, POTION_CLASS, 1, 1 },
	{ LOCK_PICK, 9, TOOL_CLASS, 1, 0 },
	{ GRAPPLING_HOOK, 0, TOOL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0, }
};

#if 0 /* Deferred */
static struct trobj Pokemon[] = {
	{ UNDEF_TYP, UNDEF_SPE, FOOD_CLASS, 5, 0 },
	{ WAN_ENLIGHTENMENT, UNDEF_SPE, WAND_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif

static struct trobj Hacker[] = {
	{ ELECTRIC_SWORD, 1, WEAPON_CLASS, 1, 1},
#define H_IC 1
	{ IC, 0, TOOL_CLASS, 7, UNDEF_BLESS },	/* quan is var. */
	{ GAUNTLETS_OF_TYPING, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ POT_JOLT_COLA, 0, POTION_CLASS, 4, UNDEF_BLESS },
	{ HACKER_S_FOOD, 0, FOOD_CLASS, 2, UNDEF_BLESS },
	{ PACK_OF_FLOPPIES, 0, TOOL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Geek[] = {
	{ SHARPENED_PENCIL, 3, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ CANDY_BAR, 0, FOOD_CLASS, 5, 0 },
	{ EXPENSIVE_CAMERA, UNDEF_SPE, TOOL_CLASS, 1, 0 },
#ifdef NEPHI_PHOTOGRAPHY
	{ SPE_PHOTO_ALBUM, 0, SPBOOK_CLASS, 1, 0 },
#endif
	{ UGLY_BACKPACK, 0, TOOL_CLASS, 1, 0 },
	{ LENSES, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Healer[] = {
	{ SCALPEL, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ RUBBER_GLOVES, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ STETHOSCOPE, 0, TOOL_CLASS, 1, 0 },
	{ MEDICAL_KIT, 0, TOOL_CLASS, 1, 0 },        
	{ POT_HEALING, 0, POTION_CLASS, 4, UNDEF_BLESS },
	{ POT_EXTRA_HEALING, 0, POTION_CLASS, 4, UNDEF_BLESS },
	{ WAN_SLEEP, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	/* [Tom] they might as well have a wand of healing, too */
	{ WAN_HEALING, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	/* always blessed, so it's guaranteed readable */
	{ SPE_HEALING, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ SPE_STONE_TO_FLESH, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ APPLE, 0, FOOD_CLASS, 10, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Ice_Mage[] = {
#define I_BOOK          9
	{ QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },        /* for dealing with ghosts */
	{ STUDDED_LEATHER_ARMOR, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 2, 0 },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
	{ WAN_COLD, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ SPE_FREEZE_SPHERE, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ SPE_CONE_OF_COLD, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};
#ifdef JEDI
static struct trobj Jedi[] = {
	{ ROBE, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ LEATHER_GLOVES, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj GreenSaber[] = {
	{ GREEN_LIGHTSABER, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0, }
};
#ifdef D_SABER
static struct trobj BlueSaber[] = {
	{ BLUE_LIGHTSABER, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0, }
};
#endif
static struct trobj RedSaber[] = {
	{ RED_LIGHTSABER, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0, }
};
#endif
static struct trobj Knight[] = {
	{ LONG_SWORD, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ LANCE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ PLATE_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ HELMET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ LARGE_SHIELD, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ LEATHER_GLOVES, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ APPLE, 0, FOOD_CLASS, 10, 0 },
	{ CARROT, 0, FOOD_CLASS, 10, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Warrior[] = {
	{ BROADSWORD, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ CROSSBOW, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
#define WAR_BOLT 2
	{ CROSSBOW_BOLT, 0, WEAPON_CLASS, 25, 1 },
	{ SCALE_MAIL, 0, ARMOR_CLASS, 1, 0 },
	{ LOW_BOOTS, 0, ARMOR_CLASS, 1, 0},
	{ SMALL_SHIELD, 0, ARMOR_CLASS, 1, 0 },
	{ APPLE, 0, FOOD_CLASS, 7, 0 },
	{ CARROT, 0, FOOD_CLASS, 7, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Monk[] = {
#define M_BOOK		2
	{ LEATHER_GLOVES, 2, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ ROBE, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
	{ POT_HEALING, 0, POTION_CLASS, 3, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
	{ APPLE, 0, FOOD_CLASS, 5, UNDEF_BLESS },
	{ ORANGE, 0, FOOD_CLASS, 5, UNDEF_BLESS },
	/* Yes, we know fortune cookies aren't really from China.  They were
	 * invented by George Jung in Los Angeles, California, USA in 1916.
	 */
	{ FORTUNE_COOKIE, 0, FOOD_CLASS, 3, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
#if 0 /* Deferred */
static struct trobj Psion[] = {
#define PS_BOOK		2
	{ LEATHER_GLOVES, 2, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ ROBE, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
	{ POT_MONSTER_DETECTION, 0, POTION_CLASS, 3, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
	{ APPLE, 0, FOOD_CLASS, 5, UNDEF_BLESS },
	{ ORANGE, 0, FOOD_CLASS, 5, UNDEF_BLESS },
	/* Yes, we know fortune cookies aren't really from China.  They were
	 * invented by George Jung in Los Angeles, California, USA in 1916.
	 */
	{ FORTUNE_COOKIE, 0, FOOD_CLASS, 3, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
#endif
static struct trobj Noble[] = {
#define NOB_SHIRT	1
	{ RAPIER, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ RUFFLED_SHIRT, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ LEATHER_GLOVES, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ LEATHER_CLOAK, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ APPLE, 0, FOOD_CLASS, 10, 0 },
	{ FOOD_RATION, 0, FOOD_CLASS, 3, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Pirate[] = {
#define PIR_KNIVES	1
#define PIR_SNACK 7
#define PIR_JEWELRY 9
#define PIR_TOOL 10
#define PIR_BULLET 3
	{ SCIMITAR, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ KNIFE, 1, WEAPON_CLASS, 2, 0 },
	{ PISTOL, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ BULLET, 0, WEAPON_CLASS, 25, 0 },
	{ LEATHER_JACKET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ HIGH_BOOTS, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ CRAM_RATION, 0, FOOD_CLASS, 2, UNDEF_BLESS },
	{ BANANA, 0, FOOD_CLASS, 3, 0 },
	{ POT_BOOZE, 0, POTION_CLASS, 3, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ OILSKIN_SACK, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Corsair[] = {
	{ SCIMITAR, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ DAGGER, 1, WEAPON_CLASS, 7, 0 },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ OILSKIN_SACK, 0, TOOL_CLASS, 1, 0 },
	{ POT_BOOZE, 0, POTION_CLASS, 2, UNDEF_BLESS },
	{ TIN, 0, FOOD_CLASS, 1, 0 },
	{ CRAM_RATION, 0, FOOD_CLASS, 1, 1 },
	{ FIGURINE, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Gladiator[] = {
	{ SILVER_SPEAR, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ SHIELD_OF_REFLECTION, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ CRAM_RATION, 0, FOOD_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Diver[] = {
	{ KNIFE, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ SPEAR, 0, WEAPON_CLASS, 3, UNDEF_BLESS },
	{ TRIDENT, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ OILSKIN_CLOAK, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ CAN_OF_GREASE, UNDEF_SPE, TOOL_CLASS, 1, 1 },
	{ TALLOW_CANDLE, 0, TOOL_CLASS, 3, 1 },
	{ TOWEL, 0, TOOL_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Necromancer[] = {
#define N_BOOK          8
/* pretty much like Wizard, except with pick-axe instead of magic resist. */
	{ QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },        /* for dealing with ghosts */
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 2, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 3, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 3, UNDEF_BLESS },
	{ SPE_SUMMON_UNDEAD, 0, SPBOOK_CLASS, 1, 1 },
	{ SPE_COMMAND_UNDEAD, 0, SPBOOK_CLASS, 1, 1 },
	{ SPE_DRAIN_LIFE, 0, SPBOOK_CLASS, 1, 1 },
		/* WAC -- gave him drain life rather than turn undead */
	{ WAN_DRAINING, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
		/* KMH -- ...and the matching wand for the inexperienced */
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ PICK_AXE, 0, TOOL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Paladin[] = {
	{ SILVER_SHORT_SWORD, 1, WEAPON_CLASS, 1, 1 },
	{ RING_MAIL, 1, ARMOR_CLASS, 1, 1 },
	{ HELMET, 0, ARMOR_CLASS, 1, 1 },
	{ SMALL_SHIELD, 0, ARMOR_CLASS, 1, 1 },
	{ LEATHER_GLOVES, 0, ARMOR_CLASS, 1, 1 },
	{ POT_WATER, 0, POTION_CLASS, 2, 1 },	/* holy water */
	{ CLOVE_OF_GARLIC, 0, FOOD_CLASS, 2, 1 },
	{ SPRIG_OF_WOLFSBANE, 0, FOOD_CLASS, 2, 1 },
	{ SPE_HEALING, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Priest[] = {
#define P_BOOK          7
	{ MACE, 0, WEAPON_CLASS, 1, 1 },
	{ ROBE, 0, ARMOR_CLASS, 1, 1 },
	{ SMALL_SHIELD, 2, ARMOR_CLASS, 1, 1 },
	{ POT_WATER, 0, POTION_CLASS, 4, 1 },	/* holy water */
	{ CLOVE_OF_GARLIC, 0, FOOD_CLASS, 1, 1 },
	{ SPRIG_OF_WOLFSBANE, 0, FOOD_CLASS, 1, 1 },
	{ SPE_HEALING, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Ranger[] = {
#define RAN_BOW			1
#define RAN_TWO_ARROWS	2
#define RAN_ZERO_ARROWS	3
	{ DAGGER, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ BOW, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ ARROW, 2, WEAPON_CLASS, 50, UNDEF_BLESS },
	{ ARROW, 0, WEAPON_CLASS, 30, UNDEF_BLESS },
	{ CLOAK_OF_DISPLACEMENT, 2, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ CRAM_RATION, 0, FOOD_CLASS, 4, 0 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Rogue[] = {
#define R_DAGGERS	1
#define R_DARTS         2
	{ SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ DAGGER, 0, WEAPON_CLASS, 10, 0 },	/* quan is variable */
	{ DART, 0, WEAPON_CLASS, 25, UNDEF_BLESS },
	{ LEATHER_ARMOR, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ POT_SICKNESS, 0, POTION_CLASS, 1, 0 },
	{ SCR_GOLD_DETECTION, 0, SCROLL_CLASS, 4, 1 },
	{ SCR_TELEPORTATION, 0, SCROLL_CLASS, 4, 1 },
	{ LOCK_PICK, 9, TOOL_CLASS, 1, 0 },
	{ OILSKIN_SACK, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Samurai[] = {
#define S_ARROWS	3
	{ KATANA, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ SHORT_SWORD, 0, WEAPON_CLASS, 1, UNDEF_BLESS }, /* wakizashi */
	{ YUMI, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ YA, 0, WEAPON_CLASS, 25, UNDEF_BLESS }, /* variable quan */
	{ SPLINT_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};

#ifdef TOURIST
static struct trobj Tourist[] = {
#define T_DARTS		0
	{ DART, 2, WEAPON_CLASS, 25, UNDEF_BLESS },	/* quan is variable */
	{ UNDEF_TYP, UNDEF_SPE, FOOD_CLASS, 12, 0 },
	{ POT_EXTRA_HEALING, 0, POTION_CLASS, 2, UNDEF_BLESS },
	{ SCR_MAGIC_MAPPING, 0, SCROLL_CLASS, 6, UNDEF_BLESS },
	{ HAWAIIAN_SHIRT, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ EXPENSIVE_CAMERA, UNDEF_SPE, TOOL_CLASS, 1, 0 },
#ifdef NEPHI_PHOTOGRAPHY
	{ SPE_PHOTO_ALBUM, 0, SPBOOK_CLASS, 1, 0 },
#endif
	{ CREDIT_CARD, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif
static struct trobj UndeadSlayer[] = {
#define U_MINOR 1       /* silver spear or whip [Castlevania] 25/25% */
                        /* crossbow 50% [Buffy] */
#define U_RANGE 2       /* silver daggers or crossbow bolts */
#define U_MISC  3       /* +1 boots [Buffy can kick] or helmet */
#define U_ARMOR 4       /* Tshirt/leather +1 or chain mail */
	{ WOODEN_STAKE, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ SILVER_SPEAR, 0, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ SILVER_DAGGER, 0, WEAPON_CLASS, 5, UNDEF_BLESS },
	{ HELMET, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ CHAIN_MAIL, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ CLOVE_OF_GARLIC, 0, FOOD_CLASS, 5, 1 },
	{ SPRIG_OF_WOLFSBANE, 0, FOOD_CLASS, 5, 1 },
	{ HOLY_WAFER, 0, FOOD_CLASS, 4, 0 },
	{ POT_WATER, 0, POTION_CLASS, 4, 1 },        /* holy water */
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Valkyrie[] = {
/*        { LONG_SWORD, 1, WEAPON_CLASS, 1, UNDEF_BLESS }, */
	{ SPEAR, 1, WEAPON_CLASS, 1, UNDEF_BLESS }, /* Traditional Weapon - WAC*/
	{ DAGGER, 0, WEAPON_CLASS, 5, UNDEF_BLESS },
	{ SMALL_SHIELD, 3, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ FOOD_RATION, 0, FOOD_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Wizard[] = {
#define W_MULTSTART	2
#define W_MULTEND	6
#define W_BOOK1         6
#define W_BOOK2         7
#define W_BOOK3         8
#define W_BOOK4         9
	{ QUARTERSTAFF, 1, WEAPON_CLASS, 1, 1 },
	{ CLOAK_OF_MAGIC_RESISTANCE, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 2, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 3, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 3, UNDEF_BLESS },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};
#ifdef YEOMAN
static struct trobj Yeoman[] = {
	{ SHORT_SWORD, 2, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ PARTISAN, 1, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ LEATHER_ARMOR, 1, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ HIGH_BOOTS, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ LEATHER_GLOVES, 0, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ APPLE, 0, FOOD_CLASS, 10, 0 },
	{ CARROT, 0, FOOD_CLASS, 10, 0 },
	{ POT_WATER, 0, POTION_CLASS, 3, 0 },
	{ FISHING_POLE, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif

/*
 *	Optional extra inventory items.
 */

static struct trobj HealingBook[] = {
	{ SPE_HEALING, 0, SPBOOK_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj ForceBook[] = {
	{ SPE_FORCE_BOLT, 0, SPBOOK_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj ExtraBook[] = {
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, 1 },
	{ 0, 0, 0, 0, 0 }
};


static struct trobj KoboldItem[] = {
	{ DART, 0, WEAPON_CLASS, 10, 0 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj KoboldItemB[] = {
	{ ORCISH_DAGGER, 0, WEAPON_CLASS, 3, 0 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj KoboldItemC[] = {
	{ ORCISH_SPEAR, 0, WEAPON_CLASS, 2, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj AutomatonItem[] = {
	{ POT_OIL, 0, POTION_CLASS, 2, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj TrollItemA[] = {
	{ RANSEUR, 0, WEAPON_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj TrollItemB[] = {
	{ PARTISAN, 0, WEAPON_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj TrollItemC[] = {
	{ GLAIVE, 0, WEAPON_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj TrollItemD[] = {
	{ SPETUM, 0, WEAPON_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj LostSoulItem[] = {
	{ FOOD_RATION, 0, FOOD_CLASS, 5, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj LostSoulItemX[] = {
	{ SCR_IDENTIFY, 0, SCROLL_CLASS, 15, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj LostSoulItemY[] = {
	{ TORCH, 0, TOOL_CLASS, 3, 0 },
	{ 0, 0, 0, 0, 0 }
};
#ifdef EASY_MODE
static struct trobj AlwaysStartItem[] = {
	{ POT_HEALING, 0, POTION_CLASS, 5, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj AlwaysStartItemB[] = {
	{ SCR_IDENTIFY, 0, SCROLL_CLASS, 2, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif /* EASY_MODE */
static struct trobj UberLostSoulItemA[] = {
	{ TALLOW_CANDLE, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemB[] = {
	{ POT_FRUIT_JUICE, 0, POTION_CLASS, 5, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemC[] = {
	{ UNDEF_TYP, UNDEF_SPE, POTION_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemD[] = {
	{ UNDEF_TYP, UNDEF_SPE, SCROLL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemE[] = {
	{ UNDEF_TYP, UNDEF_SPE, WAND_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemF[] = {
	{ UNDEF_TYP, UNDEF_SPE, ARMOR_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemG[] = {
	{ UNDEF_TYP, UNDEF_SPE, WEAPON_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemH[] = {
	{ UNDEF_TYP, UNDEF_SPE, RING_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemI[] = {
	{ UNDEF_TYP, UNDEF_SPE, AMULET_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemJ[] = {
	{ UNDEF_TYP, UNDEF_SPE, TOOL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemK[] = {
	{ UNDEF_TYP, UNDEF_SPE, FOOD_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemL[] = {
	{ UNDEF_TYP, UNDEF_SPE, SPBOOK_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemM[] = {
	{ UNDEF_TYP, UNDEF_SPE, GEM_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemN[] = {
	{ UNDEF_TYP, UNDEF_SPE, ROCK_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemO[] = {
	{ UNDEF_TYP, UNDEF_SPE, BALL_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemP[] = {
	{ UNDEF_TYP, UNDEF_SPE, CHAIN_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemQ[] = {
	{ UNDEF_TYP, UNDEF_SPE, VENOM_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj UberLostSoulItemR[] = {
	{ UNDEF_TYP, UNDEF_SPE, RANDOM_CLASS, 1, UNDEF_BLESS },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj OgreItem[] = {
	{ CLUB, 0, WEAPON_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};

static struct trobj Tinopener[] = {
	{ TIN_OPENER, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj XtraRing[] = {
	{ UNDEF_TYP, 0, RING_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Magicmarker[] = {
	{ MAGIC_MARKER, UNDEF_SPE, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Lamp[] = {
	{ OIL_LAMP, 1, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Torch[] = {
	{ TORCH, 0, TOOL_CLASS, 2, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Blindfold[] = {
	{ BLINDFOLD, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Instrument[] = {
	{ WOODEN_FLUTE, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Xtra_food[] = {
	{ UNDEF_TYP, UNDEF_SPE, FOOD_CLASS, 2, 0 },
	{ 0, 0, 0, 0, 0 }
};
#ifdef TOURIST
static struct trobj Leash[] = {
	{ LEASH, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
static struct trobj Towel[] = {
	{ TOWEL, 0, TOOL_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif	/* TOURIST */
static struct trobj Wishing[] = {
	{ WAN_WISHING, 3, WAND_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#ifdef GOLDOBJ
static struct trobj Money[] = {
	{ GOLD_PIECE, 0 , COIN_CLASS, 1, 0 },
	{ 0, 0, 0, 0, 0 }
};
#endif

/* race-based substitutions for initial inventory;
   the weaker cloak for elven rangers is intentional--they shoot better */
static struct inv_sub { short race_pm, item_otyp, subs_otyp; } inv_subs[] = {
    { PM_ELF,	DAGGER,			ELVEN_DAGGER	      },
    { PM_ELF,	SPEAR,			ELVEN_SPEAR	      },
    { PM_ELF,	SHORT_SWORD,		ELVEN_SHORT_SWORD     },
    { PM_ELF,	BOW,			ELVEN_BOW	      },
    { PM_ELF,	ARROW,			ELVEN_ARROW	      },
    { PM_ELF,	HELMET,			ELVEN_LEATHER_HELM    },
    { PM_ELF,	SMALL_SHIELD,		ELVEN_SHIELD	      },
    { PM_ELF,	CLOAK_OF_DISPLACEMENT,	ELVEN_CLOAK	      },
    { PM_ELF,	CRAM_RATION,		LEMBAS_WAFER	      },
    { PM_ORC,	DAGGER,			ORCISH_DAGGER	      },
    { PM_ORC,	SPEAR,			ORCISH_SPEAR	      },
    { PM_ORC,	SHORT_SWORD,		ORCISH_SHORT_SWORD    },
    { PM_ORC,	BOW,			ORCISH_BOW	      },
    { PM_ORC,	ARROW,			ORCISH_ARROW	      },
    { PM_ORC,	HELMET,			ORCISH_HELM	      },
    { PM_ORC,	SMALL_SHIELD,		ORCISH_SHIELD	      },
    { PM_ORC,	RING_MAIL,		ORCISH_RING_MAIL      },
    { PM_ORC,	CHAIN_MAIL,		ORCISH_CHAIN_MAIL     },
    { PM_DWARF, SPEAR,			DWARVISH_SPEAR	      },
    { PM_DWARF, SHORT_SWORD,		DWARVISH_SHORT_SWORD  },
    { PM_DWARF, HELMET,			DWARVISH_IRON_HELM    },
    { PM_DWARF, SMALL_SHIELD,		DWARVISH_ROUNDSHIELD  },
    { PM_DWARF, PICK_AXE,		DWARVISH_MATTOCK      },
    { PM_GNOME, BOW,			CROSSBOW	      },
    { PM_GNOME, ARROW,			CROSSBOW_BOLT	      },
    { PM_GNOME, HELMET, 		GNOMISH_HELM },
    { PM_GNOME, LOW_BOOTS, 		GNOMISH_BOOTS },
    { PM_GNOME, HIGH_BOOTS, 		GNOMISH_BOOTS },
    { PM_GNOME, LEATHER_ARMOR, 		GNOMISH_SUIT },
    { PM_HUMAN_WEREWOLF,FOOD_RATION,		TRIPE_RATION	      },
    { PM_HUMAN_WEREWOLF,SILVER_SPEAR,		SPEAR		      },
    { PM_HUMAN_WEREWOLF,SILVER_DAGGER,		DAGGER		      },
#ifdef FIREARMS
    { PM_HUMAN_WEREWOLF,SILVER_BULLET,		BULLET		      },
#endif
    { PM_DROW,		DAGGER,			DARK_ELVEN_DAGGER     },
    { PM_DROW,		SHORT_SWORD,		DARK_ELVEN_SHORT_SWORD},
    { PM_DROW,		BOW,			DARK_ELVEN_BOW	      },
    { PM_DROW,		ARROW,			DARK_ELVEN_ARROW      },
    { PM_VAMPIRE,	POT_FRUIT_JUICE,	POT_BLOOD	      },
    { PM_VAMPIRE,	FOOD_RATION,		POT_VAMPIRE_BLOOD     },
    { PM_INCANTIFIER,CLOAK_OF_MAGIC_RESISTANCE,	ROBE  },
    { PM_INCANTIFIER,CLOAK_OF_DISPLACEMENT,	ROBE  },
    { PM_INCANTIFIER,	LEATHER_ARMOR,		ROBE  },
    { PM_INCANTIFIER,	LEATHER_JACKET,		ROBE  },
    { PM_INCANTIFIER,	RING_MAIL,		ROBE  },
    { PM_INCANTIFIER,	SPLINT_MAIL,		ROBE  },
    { PM_INCANTIFIER,	FOOD_RATION,	SCR_FOOD_DETECTION	},
    { PM_INCANTIFIER,	CRAM_RATION, 	SCR_FOOD_DETECTION	},
    { PM_INCANTIFIER,	HACKER_S_FOOD,	SCR_FOOD_DETECTION	},
    { PM_INCANTIFIER,	POT_FRUIT_JUICE,SCR_FOOD_DETECTION	},
    { PM_INCANTIFIER,	BANANA,		SCR_FOOD_DETECTION	},
    { PM_INCANTIFIER,	ORANGE,		SCR_FOOD_DETECTION	},
    { PM_INCANTIFIER,	POT_BOOZE,	SCR_FOOD_DETECTION	},
    { PM_GHOUL,		FOOD_RATION,		POT_BLOOD     	      },
    { PM_GHOUL,		POT_FRUIT_JUICE,	POT_BLOOD	      },
    { PM_CLOCKWORK_AUTOMATON,	FOOD_RATION,	POT_OIL    	      },
    { PM_CLOCKWORK_AUTOMATON,	CRAM_RATION, 	POT_OIL 	      },
    { PM_CLOCKWORK_AUTOMATON,	POT_FRUIT_JUICE,POT_OIL		      },
    { PM_CLOCKWORK_AUTOMATON,	TRIPE_RATION,	POT_OIL    	      },
    { PM_CLOCKWORK_AUTOMATON,	BANANA,		POT_OIL    	      },
    { PM_CLOCKWORK_AUTOMATON,	ORANGE,		POT_OIL    	      },
    { PM_CLOCKWORK_AUTOMATON,	POT_BOOZE,	POT_OIL    	      },
    { NON_PM,		STRANGE_OBJECT,		STRANGE_OBJECT	      }
};

/* align-based substitutions for initial inventory */
struct inv_asub { aligntyp align; short item_otyp, subs_otyp; };
static struct inv_asub inv_asubs[] = {
    { A_CHAOTIC,	HOLY_WAFER,		LEMBAS_WAFER	      },
    { A_NONE,		STRANGE_OBJECT,		STRANGE_OBJECT	      }
};

/* KMH -- Expectations for skills.
 *	1.	Starting equipment will start as basic, and should have a maximum
 *		of at least skilled.  If you enter the dungeon with it, you should
 *		know how to use it.  See weapon.c, skill_init().
 *	2.	First and second artifact gifts should have a maximum of at least
 *		skilled.  See artifact.c, init_artifacts().
 *	3.	Quest artifacts should have a maximum of expert.  See artifact.c,
 *		init_artifacts().
 *	4.	Races that construct weapons (elf, dwarf, orc) should have
 *		a maximum of expert in those weapons.
 *	5.	Spellcasters should get a maximum of at least basic in all spell
 *		categories.
 */

/* 5lo: Reverted skills back to what they were in vanilla, patches, and variants
 * that included skills.  For the rest, I either copy-pasted skillsets from other
 * roles that I felt were similar/close enough, or edited the existing skillset to
 * meet a more balanced level */
static const struct def_skill Skill_A[] = {
    { P_DAGGER, P_BASIC },		{ P_KNIFE,  P_BASIC },
    { P_PICK_AXE, P_EXPERT },		{ P_SHORT_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },		{ P_SABER, P_EXPERT },
    { P_CLUB, P_SKILLED },              { P_PADDLE, P_BASIC },
    { P_QUARTERSTAFF, P_SKILLED },
#ifdef FIREARMS
    { P_FIREARM, P_SKILLED },
#endif
    { P_SLING, P_SKILLED },		{ P_DART, P_BASIC },
    { P_BOOMERANG, P_EXPERT },		{ P_WHIP, P_EXPERT },
    { P_UNICORN_HORN, P_SKILLED },
    { P_ATTACK_SPELL, P_BASIC },	{ P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },   { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_PROTECTION_SPELL, P_BASIC },    { P_BODY_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_SKILLED },
    /*WAC- 'A' knows of all spells from studying,  but only really good
    at divination class*/
    { P_TWO_WEAPON_COMBAT, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_B[] = {
    { P_DAGGER, P_BASIC },		{ P_AXE, P_EXPERT },
    { P_PICK_AXE, P_SKILLED },	{ P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_SKILLED },	{ P_LONG_SWORD, P_SKILLED },
    { P_TWO_HANDED_SWORD, P_EXPERT },	{ P_SCIMITAR, P_SKILLED },
    { P_SABER, P_BASIC },		{ P_CLUB, P_SKILLED },
    { P_PADDLE, P_SKILLED },
    { P_MACE, P_SKILLED },		{ P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_BASIC },		{ P_HAMMER, P_EXPERT },
    { P_QUARTERSTAFF, P_BASIC },	{ P_SPEAR, P_SKILLED },
    { P_TRIDENT, P_SKILLED },		{ P_BOW, P_BASIC },
    { P_ATTACK_SPELL, P_SKILLED },	{ P_BODY_SPELL, P_SKILLED },
/*WAC - 'B' is good at attack spells and body manipulation spells
        -both combat oriented */
#ifdef STEED
    { P_RIDING, P_BASIC },
#endif
    { P_TWO_WEAPON_COMBAT, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_MASTER },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Bard[] = {
    /* 5lo: Comes straight from the Bard patch. */
    { P_DAGGER, P_SKILLED },		{ P_KNIFE,  P_BASIC },
    { P_SHORT_SWORD, P_BASIC },		{ P_SCIMITAR, P_BASIC },
    { P_CLUB, P_SKILLED },		{ P_FLAIL, P_BASIC },
    { P_QUARTERSTAFF, P_SKILLED },	{ P_POLEARMS, P_BASIC },
    { P_JAVELIN, P_SKILLED },		{ P_SPEAR, P_BASIC },
    { P_SLING, P_SKILLED },		{ P_DART, P_EXPERT },	 
    { P_ENCHANTMENT_SPELL, P_SKILLED },	{ P_BODY_SPELL, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_MUSICALIZE, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_C[] = {
    { P_KNIFE,  P_SKILLED },
    { P_AXE, P_SKILLED },		{ P_PICK_AXE, P_BASIC },
    { P_CLUB, P_EXPERT },               { P_PADDLE, P_EXPERT },
    { P_MACE, P_EXPERT },
    { P_MORNING_STAR, P_BASIC },	{ P_FLAIL, P_SKILLED },
    { P_HAMMER, P_SKILLED },		{ P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },		{ P_SPEAR, P_EXPERT },
    { P_JAVELIN, P_SKILLED },		{ P_TRIDENT, P_SKILLED },
    { P_BOW, P_EXPERT },                { P_SLING, P_SKILLED },
    { P_BOOMERANG, P_EXPERT },
    { P_MATTER_SPELL, P_SKILLED },      { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_BODY_SPELL, P_SKILLED },

/*WAC - 'C' is at one with the elements - matter spells, as well as
        basic enchantments - removed attack spell basic skill as
        it is now "dark" spells*/
    { P_BARE_HANDED_COMBAT, P_MASTER },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Chef[] = {
    /* 5lo: This'll be organized better later...*/
    { P_DAGGER, P_SKILLED },		{ P_KNIFE,  P_EXPERT },
    { P_AXE, P_EXPERT },		{ P_DART, P_EXPERT },
    { P_CLUB, P_SKILLED },              { P_PADDLE, P_SKILLED },
    { P_LONG_SWORD, P_BASIC },		{ P_FLAIL, P_BASIC },
    { P_SHORT_SWORD, P_SKILLED },
    { P_HAMMER, P_EXPERT },		{ P_QUARTERSTAFF, P_SKILLED },
    { P_SABER, P_BASIC},		{ P_SLING, P_SKILLED },
    { P_SLING, P_BASIC},		{ P_UNICORN_HORN, P_BASIC },
    { P_DIVINATION_SPELL, P_SKILLED },  { P_PROTECTION_SPELL, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_SKILLED }, {P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_F[] = {
/*Style: small-med edged weapons, blunt weapons*/
    { P_DAGGER, P_EXPERT },             { P_KNIFE,  P_SKILLED },
    { P_AXE, P_BASIC },                 /*{ P_PICK_AXE, P_BASIC },*/
    { P_SHORT_SWORD, P_EXPERT },        /*{ P_BROAD_SWORD, P_BASIC },*/
    { P_LONG_SWORD, P_SKILLED },        /*{ P_TWO_HANDED_SWORD, P_BASIC },*/
    { P_SCIMITAR, P_SKILLED },          { P_SABER, P_SKILLED },
    { P_PADDLE, P_BASIC },
    { P_MACE, P_BASIC },                /*{ P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },*/               { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_SKILLED },       /* { P_POLEARMS, P_BASIC },*/
/* Relies on spells for ranged attack
    { P_SPEAR, P_BASIC },               { P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },             { P_LANCE, P_BASIC },
    { P_BOW, P_BASIC },                 { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },            { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },            { P_BOOMERANG, P_BASIC },
*/
    { P_WHIP, P_BASIC },                { P_UNICORN_HORN, P_SKILLED },

    { P_ATTACK_SPELL, P_SKILLED },      { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },   { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_PROTECTION_SPELL, P_BASIC },    { P_BODY_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_EXPERT },
/*  Added expert matter spell (elements), skilled in attack, basic in rest
        He is a mage,  so knows the types.*/
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED }, { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Fir[] = {
    /* 5lo: Still a bit of a work in progress */
    { P_AXE, P_EXPERT },                 { P_PICK_AXE, P_EXPERT },
    { P_SHORT_SWORD, P_EXPERT },         { P_BROAD_SWORD, P_EXPERT },
    { P_SCIMITAR, P_BASIC },             { P_SABER, P_SKILLED },
    { P_PADDLE, P_BASIC },
    { P_MACE, P_SKILLED },               { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },               { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },        { P_POLEARMS, P_BASIC },
    { P_BOW, P_SKILLED },                 { P_SLING, P_SKILLED },
    { P_CROSSBOW, P_SKILLED },            { P_DART, P_SKILLED },
    { P_WHIP, P_BASIC },                { P_UNICORN_HORN, P_BASIC },

    { P_PROTECTION_SPELL, P_SKILLED },   { P_HEALING_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_SKILLED },
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_BASIC }, { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Lun[] = {
    /* Directly taken from Slash'EM 0.0.6 sources - Lycanthrope role */
    { P_DAGGER, P_BASIC },              { P_KNIFE,  P_SKILLED },
    { P_AXE, P_SKILLED },               { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_EXPERT },        { P_CLUB, P_EXPERT },
    { P_PADDLE, P_BASIC },
    { P_MACE, P_EXPERT },		{ P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_SKILLED },		{ P_HAMMER, P_SKILLED },
    { P_QUARTERSTAFF, P_EXPERT },	{ P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_EXPERT },		{ P_JAVELIN, P_SKILLED },
    { P_TRIDENT, P_SKILLED },		{ P_BOW, P_EXPERT },
    { P_SLING, P_SKILLED },		{ P_BOOMERANG, P_EXPERT },
    { P_UNICORN_HORN, P_BASIC },

    { P_HEALING_SPELL, P_BASIC },     { P_PROTECTION_SPELL, P_BASIC },    
/*WAC - added skilled body spells (he polymorphs), expert heal
        - regeneration and create familiar*/
    { P_BARE_HANDED_COMBAT, P_GRAND_MASTER },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Ele[] = {
/* 5lo: Copy-paste of Flame Mage */
/*Style: small-med edged weapons, blunt weapons*/
    { P_DAGGER, P_EXPERT },             { P_KNIFE,  P_SKILLED },
    { P_AXE, P_BASIC },                 /*{ P_PICK_AXE, P_BASIC },*/
    { P_SHORT_SWORD, P_EXPERT },        /*{ P_BROAD_SWORD, P_BASIC },*/
    { P_LONG_SWORD, P_SKILLED },        /*{ P_TWO_HANDED_SWORD, P_BASIC },*/
    { P_SCIMITAR, P_SKILLED },          { P_SABER, P_SKILLED },
    { P_PADDLE, P_BASIC },
    { P_MACE, P_BASIC },                /*{ P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },*/               { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_SKILLED },       /* { P_POLEARMS, P_BASIC },*/
/* Relies on spells for ranged attack
    { P_SPEAR, P_BASIC },               { P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },             { P_LANCE, P_BASIC },
    { P_BOW, P_BASIC },                 { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },            { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },            { P_BOOMERANG, P_BASIC },
*/
    { P_WHIP, P_BASIC },                { P_UNICORN_HORN, P_SKILLED },

    { P_ATTACK_SPELL, P_SKILLED },      { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },   { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_PROTECTION_SPELL, P_BASIC },    { P_BODY_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_EXPERT },
/*  Added expert matter spell (elements), skilled in attack, basic in rest
        He is a mage,  so knows the types.*/
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED }, { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Aci[] = {
    /* 5lo: Copy-paste of Ice Mage */
/*Resorts mostly to stabbing weapons*/
    { P_DAGGER, P_EXPERT },             { P_KNIFE,  P_SKILLED },
/*  { P_AXE, P_BASIC },                 { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_EXPERT },        { P_BROAD_SWORD, P_BASIC },*/
    { P_LONG_SWORD, P_SKILLED },      /*{ P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },          { P_SABER, P_SKILLED },
    { P_MACE, P_BASIC },                { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },               { P_HAMMER, P_BASIC },*/
    { P_QUARTERSTAFF, P_SKILLED },      { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },               { P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },             { P_LANCE, P_BASIC },
/*  { P_BOW, P_BASIC },                 { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },            { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },            { P_BOOMERANG, P_BASIC },*/
    { P_WHIP, P_BASIC },                { P_UNICORN_HORN, P_SKILLED },

    { P_ATTACK_SPELL, P_BASIC },        { P_HEALING_SPELL, P_SKILLED },
    { P_DIVINATION_SPELL, P_BASIC },    { P_ENCHANTMENT_SPELL, P_EXPERT },
    { P_PROTECTION_SPELL, P_SKILLED },  { P_BODY_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_EXPERT },
    /*WAC - same as Flame Mage*/
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED }, { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

#ifdef CONVICT
static const struct def_skill Skill_Con[] = {
    /* 5lo: Straight from Convict patch */
    { P_DAGGER, P_SKILLED },		{ P_KNIFE,  P_EXPERT },
    { P_HAMMER, P_SKILLED },		{ P_PICK_AXE, P_EXPERT },
    { P_CLUB, P_EXPERT },		{ P_MACE, P_BASIC },
    { P_DART, P_SKILLED },		{ P_FLAIL, P_EXPERT },
    { P_SHORT_SWORD, P_BASIC },		{ P_SLING, P_SKILLED },
    { P_ATTACK_SPELL, P_BASIC },	{ P_BODY_SPELL, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};
#endif  /* CONVICT */

static const struct def_skill Skill_Dru[] = {
    /* 5lo: A focus on short, one handed bladed weapons */
    { P_DAGGER, P_EXPERT },		{ P_KNIFE,  P_EXPERT },
    { P_WHIP, P_EXPERT },		{ P_CLUB, P_SKILLED },    
    { P_SHORT_SWORD, P_SKILLED },
    { P_DART, P_SKILLED },		{ P_FLAIL, P_BASIC },
    { P_SLING, P_SKILLED },    		{ P_UNICORN_HORN, P_BASIC },
#ifdef FIREARMS
    { P_FIREARM, P_BASIC },
#endif
    { P_DIVINATION_SPELL, P_BASIC },	{ P_BODY_SPELL, P_BASIC },
    { P_MUSICALIZE, P_BASIC }, /* Drunken singing */
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_RIDING, P_BASIC },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Hack[] = {
    /* 5lo: They've mostly learned by movies and video games */
    { P_DAGGER, P_EXPERT },		{ P_KNIFE, P_EXPERT },
    { P_LONG_SWORD, P_EXPERT },		{ P_CLUB, P_EXPERT },
    { P_PADDLE, P_EXPERT },		{ P_MACE, P_BASIC },
    { P_FLAIL, P_BASIC },		{ P_HAMMER, P_SKILLED },
    { P_POLEARMS, P_SKILLED },		{ P_TRIDENT, P_BASIC },
    { P_LANCE, P_BASIC },		{ P_SLING, P_BASIC },
    { P_FIREARM, P_BASIC },		{ P_CROSSBOW, P_SKILLED },
    { P_DART, P_EXPERT },		{ P_BOOMERANG, P_BASIC },

    { P_LIGHTSABER, P_BASIC }, /* They've seen Star Wars plenty of times */

    { P_DIVINATION_SPELL, P_SKILLED },
    { P_PROTECTION_SPELL, P_SKILLED },    { P_BODY_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_SKILLED },

#ifdef STEED
    { P_RIDING, P_EXPERT },
#endif
    { P_MARTIAL_ARTS, P_BASIC },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Gee[] = {
    /* 5lo: Effectively a much more limited Hacker */
    { P_DAGGER, P_EXPERT },		{ P_KNIFE, P_EXPERT },
    { P_LONG_SWORD, P_SKILLED },	{ P_CLUB, P_EXPERT },
    { P_PADDLE, P_SKILLED },		{ P_MACE, P_BASIC },
    { P_FLAIL, P_BASIC },		{ P_HAMMER, P_SKILLED },
    { P_LANCE, P_BASIC },		{ P_SLING, P_BASIC },
    { P_FIREARM, P_BASIC },		{ P_CROSSBOW, P_SKILLED },
    { P_DART, P_EXPERT },		{ P_BOOMERANG, P_BASIC },

    { P_LIGHTSABER, P_BASIC }, /* They've seen Star Wars plenty of times */

    { P_DIVINATION_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_SKILLED },

#ifdef STEED
    { P_RIDING, P_EXPERT },
#endif
    { P_MARTIAL_ARTS, P_BASIC },
    { P_NONE, 0 }
};


static const struct def_skill Skill_H[] = {
    { P_DAGGER, P_SKILLED },		{ P_KNIFE, P_EXPERT },
    { P_SHORT_SWORD, P_SKILLED },	{ P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },		{ P_CLUB, P_SKILLED },
    { P_PADDLE, P_SKILLED },
    { P_MACE, P_BASIC },		{ P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_BASIC },		{ P_SPEAR, P_BASIC },
    { P_JAVELIN, P_BASIC },		{ P_TRIDENT, P_BASIC },
    { P_SLING, P_SKILLED },		{ P_DART, P_EXPERT },
    { P_SHURIKEN, P_SKILLED },		{ P_UNICORN_HORN, P_EXPERT },

    { P_ATTACK_SPELL, P_BASIC },        { P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_BASIC },    { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_PROTECTION_SPELL, P_BASIC },    { P_BODY_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_BASIC },
#ifdef STEED
    { P_RIDING, P_BASIC },
#endif
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};


static const struct def_skill Skill_I[] = {
/*Resorts mostly to stabbing weapons*/
    { P_DAGGER, P_EXPERT },             { P_KNIFE,  P_SKILLED },
/*  { P_AXE, P_BASIC },                 { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_EXPERT },        { P_BROAD_SWORD, P_BASIC },*/
    { P_LONG_SWORD, P_SKILLED },      /*{ P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },          { P_SABER, P_SKILLED },
    { P_MACE, P_BASIC },                { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },               { P_HAMMER, P_BASIC },*/
    { P_QUARTERSTAFF, P_SKILLED },      { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },               { P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },             { P_LANCE, P_BASIC },
/*  { P_BOW, P_BASIC },                 { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },            { P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },            { P_BOOMERANG, P_BASIC },*/
    { P_WHIP, P_BASIC },                { P_UNICORN_HORN, P_SKILLED },

    { P_ATTACK_SPELL, P_BASIC },        { P_HEALING_SPELL, P_SKILLED },
    { P_DIVINATION_SPELL, P_BASIC },    { P_ENCHANTMENT_SPELL, P_EXPERT },
    { P_PROTECTION_SPELL, P_SKILLED },  { P_BODY_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_EXPERT },
    /*WAC - same as Flame Mage*/
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED }, { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

#ifdef JEDI
static const struct def_skill Skill_J[] = {
    /* 5lo: Straight from Jedi patch */
    { P_LIGHTSABER, P_EXPERT },
    { P_SHORT_SWORD, P_BASIC }, { P_BROAD_SWORD, P_BASIC },
    { P_LONG_SWORD, P_SKILLED }, { P_SABER, P_SKILLED },
    { P_HEALING_SPELL, P_BASIC },
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_BASIC }, { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
#endif

static const struct def_skill Skill_Incantifier[] = {
    { P_ATTACK_SPELL, P_EXPERT },	{ P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_EXPERT },	{ P_ENCHANTMENT_SPELL, P_EXPERT },
    { P_PROTECTION_SPELL, P_EXPERT },	{ P_BODY_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_K[] = {
    { P_DAGGER, P_BASIC },		{ P_KNIFE, P_BASIC },
    { P_AXE, P_SKILLED },		{ P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_SKILLED },	{ P_BROAD_SWORD, P_EXPERT },
    { P_LONG_SWORD, P_EXPERT },	{ P_TWO_HANDED_SWORD, P_EXPERT},
    { P_SCIMITAR, P_BASIC },		{ P_SABER, P_SKILLED },
    { P_CLUB, P_BASIC },                { P_PADDLE, P_BASIC },
    { P_MACE, P_BASIC },
    { P_MORNING_STAR, P_SKILLED },	{ P_FLAIL, P_BASIC },
    { P_HAMMER, P_BASIC },		{ P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },		{ P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },		{ P_LANCE, P_EXPERT },
    { P_BOW, P_BASIC },			{ P_CROSSBOW, P_SKILLED },
    { P_HEALING_SPELL, P_SKILLED },
    { P_PROTECTION_SPELL, P_SKILLED },      { P_BODY_SPELL, P_BASIC },
/*WAC - removed the attack spells - no good having knights summoning
        undead!  Replaced with skilled body spells*/

#ifdef STEED
    { P_RIDING, P_EXPERT },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED }, { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_War[] = {
    /* 5lo: uses Dwarf skills from Slash'EM 0.0.6 with modifications */
    { P_PICK_AXE, P_SKILLED},     { P_AXE, P_SKILLED },
    { P_SHORT_SWORD, P_EXPERT },  { P_SCIMITAR, P_SKILLED },
    { P_FLAIL, P_SKILLED },       { P_HAMMER, P_SKILLED },
    { P_DAGGER, P_BASIC },        { P_SPEAR, P_SKILLED },
    { P_JAVELIN, P_BASIC },       { P_CLUB, P_BASIC },
    { P_LONG_SWORD, P_EXPERT },    { P_SABER, P_BASIC },
    { P_MACE, P_SKILLED }, { P_BROAD_SWORD, P_EXPERT },
    { P_TWO_HANDED_SWORD, P_EXPERT },
    { P_CROSSBOW, P_EXPERT },
    /* Warriors avoid magic */
    { P_DIVINATION_SPELL, P_BASIC },
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },  { P_NONE, 0 }
};

static const struct def_skill Skill_Off[] = {
    /* 5lo: Elf skills from Slash'EM 0.0.6 with modifications */
    { P_DAGGER, P_EXPERT },             { P_KNIFE, P_SKILLED },
    { P_SHORT_SWORD, P_EXPERT },        { P_BROAD_SWORD, P_EXPERT },
    { P_LONG_SWORD, P_SKILLED },        { P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },          { P_SABER, P_SKILLED },
    { P_QUARTERSTAFF, P_EXPERT },       { P_CLUB, P_EXPERT },
    { P_SPEAR, P_EXPERT },              { P_JAVELIN, P_BASIC },
    { P_BOW, P_EXPERT },                { P_SLING, P_BASIC },
    { P_CROSSBOW, P_BASIC },
#ifdef FIREARMS
    { P_FIREARM, P_EXPERT },
#endif
    { P_MATTER_SPELL, P_BASIC },        { P_HEALING_SPELL, P_SKILLED },
    { P_ENCHANTMENT_SPELL, P_EXPERT },
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_EXPERT },  { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Mon[] = {
    { P_PADDLE, P_SKILLED },
    { P_QUARTERSTAFF, P_SKILLED },	{ P_SPEAR, P_BASIC },
    { P_JAVELIN, P_BASIC },		{ P_BOW, P_BASIC },
    { P_SHURIKEN, P_BASIC },            { P_MARTIAL_ARTS, P_GRAND_MASTER },

    { P_ATTACK_SPELL, P_BASIC },    { P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_BASIC },	{ P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_PROTECTION_SPELL, P_EXPERT },	{ P_BODY_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_BASIC },	{ P_MUSICALIZE, P_BASIC },
/*WAC - monks are good healers - expert healing - and expert protect*/
    { P_NONE, 0 }
};
#if 0 /* 5lo: Deferred */
static const struct def_skill Skill_Psi[] = {
    /* 5lo: Copy-paste of the Monk */
    { P_PADDLE, P_SKILLED },
    { P_QUARTERSTAFF, P_SKILLED },	{ P_SPEAR, P_BASIC },
    { P_JAVELIN, P_BASIC },		{ P_BOW, P_BASIC },
    { P_SHURIKEN, P_BASIC },            { P_MARTIAL_ARTS, P_GRAND_MASTER },

    { P_ATTACK_SPELL, P_BASIC },    { P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_BASIC },	{ P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_PROTECTION_SPELL, P_EXPERT },	{ P_BODY_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_BASIC },
    { P_NONE, 0 }
};
#endif
static const struct def_skill Skill_Nob[] = {
    /* 5lo: Straight from dNethack */
    { P_DAGGER, P_BASIC },		{ P_KNIFE, P_EXPERT },
    { P_AXE, P_EXPERT },
    { P_SHORT_SWORD, P_SKILLED },	{ P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_EXPERT },
    { P_SCIMITAR, P_EXPERT },		{ P_SABER, P_EXPERT },
    { P_CLUB, P_BASIC },		{ P_MACE, P_EXPERT },
    { P_MORNING_STAR, P_BASIC },	{ P_FLAIL, P_BASIC },
    { P_POLEARMS, P_BASIC },
    { P_SPEAR, P_EXPERT },		{ P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },		{ P_LANCE, P_SKILLED },
    { P_BOW, P_SKILLED },		{ P_CROSSBOW, P_BASIC },
    { P_ATTACK_SPELL, P_SKILLED },	{ P_HEALING_SPELL, P_SKILLED },
    { P_BODY_SPELL, P_SKILLED },	{ P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_MUSICALIZE, P_BASIC },
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_N[] = {
    { P_DAGGER, P_EXPERT },             { P_KNIFE,  P_SKILLED },
    { P_AXE, P_SKILLED },               { P_PICK_AXE, P_SKILLED },
    { P_CLUB, P_SKILLED },              { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },       { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_BASIC },               { P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },             { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },               { P_SHURIKEN, P_BASIC },

    { P_ATTACK_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
/*WAC-  expert of dark arts - attack spells,  skilled in matter
        -for fireball and cone of cold*/
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};

static struct def_skill Skill_P[] = {
	/* KMH -- Long sword for Sunsword */
    { P_LONG_SWORD, P_SKILLED },
    { P_CLUB, P_EXPERT },               { P_PADDLE, P_EXPERT },
    { P_MACE, P_EXPERT },
    { P_MORNING_STAR, P_EXPERT },	{ P_FLAIL, P_EXPERT },
    { P_HAMMER, P_EXPERT },		{ P_QUARTERSTAFF, P_EXPERT },
    { P_POLEARMS, P_SKILLED },		{ P_SPEAR, P_SKILLED },
    { P_JAVELIN, P_SKILLED },		{ P_TRIDENT, P_SKILLED },
    { P_LANCE, P_BASIC },		{ P_BOW, P_BASIC },
    { P_SLING, P_BASIC },		{ P_CROSSBOW, P_BASIC },
    { P_DART, P_BASIC },		{ P_SHURIKEN, P_BASIC },
    { P_BOOMERANG, P_BASIC },		{ P_UNICORN_HORN, P_SKILLED },

    /* [ALI] Depending on the spellbook which priests enter the dungeon with,
     * one of the maximum skill levels listed here will be raised by one.
     */
    { P_ATTACK_SPELL, P_BASIC },        { P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_EXPERT },   { P_ENCHANTMENT_SPELL, P_BASIC },
    { P_PROTECTION_SPELL, P_SKILLED },  { P_BODY_SPELL, P_BASIC },
    { P_MATTER_SPELL, P_BASIC },        { P_MUSICALIZE, P_BASIC },

    { P_BARE_HANDED_COMBAT, P_BASIC },  /* the monk is added in slash */ 
    { P_NONE, 0 }
};

static struct def_skill Skill_Pal[] = {
    /* 5lo: Mesh of Knight and Priest skills */
    { P_DAGGER, P_BASIC },		{ P_KNIFE, P_BASIC },
    { P_SHORT_SWORD, P_EXPERT },	{ P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_SKILLED },	{ P_TWO_HANDED_SWORD, P_SKILLED },
    { P_SCIMITAR, P_BASIC },		{ P_SABER, P_SKILLED },
    { P_CLUB, P_EXPERT },		{ P_SLING, P_BASIC },
    { P_MACE, P_EXPERT },		{ P_DART, P_BASIC},
    { P_MORNING_STAR, P_SKILLED }, { P_QUARTERSTAFF, P_EXPERT },
    { P_HAMMER, P_BASIC },		{ P_POLEARMS, P_EXPERT },
    { P_SPEAR, P_SKILLED },		{ P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },		{ P_LANCE, P_SKILLED },
    { P_BOW, P_BASIC },			{ P_CROSSBOW, P_SKILLED },
    { P_HEALING_SPELL, P_SKILLED },
    { P_PROTECTION_SPELL, P_SKILLED },      { P_DIVINATION_SPELL, P_EXPERT },
#ifdef STEED
    { P_RIDING, P_EXPERT },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED }, { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};


static const struct def_skill Skill_Pir[] = {
    /* 5lo: From the Pirate patch + Firearms */
    { P_DAGGER, P_SKILLED },	{ P_KNIFE,  P_EXPERT },
    { P_AXE, P_SKILLED },	    { P_SHORT_SWORD, P_BASIC },
    { P_BROAD_SWORD, P_EXPERT },{ P_LONG_SWORD, P_BASIC },
    { P_SCIMITAR, P_EXPERT },	{ P_SABER, P_EXPERT },
    { P_CLUB, P_BASIC },		{ P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_EXPERT },		{ P_SPEAR, P_SKILLED },
    { P_JAVELIN, P_SKILLED },	{ P_TRIDENT, P_EXPERT },
    { P_CROSSBOW, P_EXPERT },   { P_DART, P_SKILLED },
#ifdef FIREARMS
    { P_FIREARM, P_SKILLED },
#endif
    { P_WHIP, P_SKILLED },   	{ P_UNICORN_HORN, P_BASIC },

    { P_ATTACK_SPELL, P_BASIC },{ P_DIVINATION_SPELL, P_BASIC },
    { P_ENCHANTMENT_SPELL, P_BASIC },{ P_BODY_SPELL, P_SKILLED },
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Div[] = {
    /* 5lo: from Fyr's userpage on nethackwiki */
    { P_TRIDENT, P_EXPERT },	{ P_KNIFE,  P_EXPERT },
    { P_FLAIL, P_EXPERT },	{ P_SHORT_SWORD, P_BASIC },
    { P_DAGGER, P_BASIC },	{ P_SPEAR, P_SKILLED },
    { P_JAVELIN, P_SKILLED },

    { P_DIVINATION_SPELL, P_BASIC }, { P_BODY_SPELL, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Cor[] = {
    /* 5lo: from Fyr's userpage on nethackwiki */
    { P_DAGGER, P_EXPERT },	{ P_LONG_SWORD,  P_SKILLED },
    { P_PICK_AXE, P_BASIC },	    { P_SHORT_SWORD, P_BASIC },
    { P_DART, P_BASIC },{ P_KNIFE, P_SKILLED },
    { P_AXE, P_SKILLED },	{ P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },		{ P_TRIDENT, P_SKILLED },
    { P_SCIMITAR, P_EXPERT },		{ P_WHIP, P_EXPERT },
    { P_SABER, P_EXPERT },	
#ifdef FIREARMS
    { P_FIREARM, P_SKILLED },
#endif
    { P_ATTACK_SPELL, P_EXPERT },{ P_DIVINATION_SPELL, P_BASIC },
    /* 5lo: Attack spells since we don't have voodoo implemented */
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Gla[] = {
    /* 5lo: from Fyr's userpage on nethackwiki */
    { P_SABER, P_SKILLED },	{ P_PICK_AXE,  P_BASIC },
    { P_UNICORN_HORN, P_EXPERT },	    { P_CROSSBOW, P_BASIC },
    { P_KNIFE, P_SKILLED },	{ P_JAVELIN, P_SKILLED },
    { P_POLEARMS, P_SKILLED },	{ P_SCIMITAR, P_SKILLED },
    { P_FLAIL, P_SKILLED },		{ P_BROAD_SWORD, P_SKILLED },
    { P_TWO_HANDED_SWORD, P_SKILLED },		{ P_MORNING_STAR, P_SKILLED },
    { P_HAMMER, P_SKILLED },	{ P_LANCE, P_SKILLED },
    { P_WHIP, P_SKILLED },   { P_LONG_SWORD, P_EXPERT },
    { P_SHORT_SWORD, P_EXPERT },   	{ P_DAGGER, P_EXPERT },
    { P_SPEAR, P_EXPERT },	{ P_TRIDENT, P_EXPERT },

    { P_ATTACK_SPELL, P_EXPERT },{ P_BODY_SPELL, P_EXPERT },
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_RIDING, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_MASTER },
    { P_NONE, 0 }
};

static const struct def_skill Skill_R[] = {
    { P_DAGGER, P_EXPERT },		{ P_KNIFE,  P_EXPERT },
    { P_SHORT_SWORD, P_EXPERT },	{ P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_SKILLED },	{ P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },		{ P_SABER, P_SKILLED },
    { P_CLUB, P_SKILLED },              { P_PADDLE, P_SKILLED },
    { P_MACE, P_SKILLED },              { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },               { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_BASIC },            { P_SPEAR, P_BASIC },
#ifdef FIREARMS
    { P_FIREARM, P_EXPERT },
#endif
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_EXPERT },		{ P_SHURIKEN, P_SKILLED },
    { P_DIVINATION_SPELL, P_SKILLED },        { P_BODY_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_SKILLED },
/*WAC Left as is*/
#ifdef STEED
    { P_RIDING, P_BASIC },
#endif
    { P_TWO_WEAPON_COMBAT, P_EXPERT },  { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Loc[] = {
    /* 5lo: Copy-paste of Rogue */
    { P_DAGGER, P_EXPERT },		{ P_KNIFE,  P_EXPERT },
    { P_SHORT_SWORD, P_EXPERT },	{ P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_SKILLED },	{ P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },		{ P_SABER, P_SKILLED },
    { P_CLUB, P_SKILLED },              { P_PADDLE, P_SKILLED },
    { P_MACE, P_SKILLED },              { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },               { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_BASIC },            { P_SPEAR, P_BASIC },
#ifdef FIREARMS
    { P_FIREARM, P_EXPERT },
#endif
    { P_CROSSBOW, P_EXPERT },
    { P_DART, P_EXPERT },		{ P_SHURIKEN, P_SKILLED },
    { P_DIVINATION_SPELL, P_SKILLED },        { P_BODY_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_SKILLED },
/*WAC Left as is*/
#ifdef STEED
    { P_RIDING, P_BASIC },
#endif
    { P_TWO_WEAPON_COMBAT, P_EXPERT },  { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};


static const struct def_skill Skill_Nin[] = {
    /* 5lo: Mesh of Rogue and Samurai */
    { P_DAGGER, P_EXPERT },		{ P_KNIFE,  P_EXPERT },
    { P_SHORT_SWORD, P_EXPERT },	{ P_BROAD_SWORD, P_EXPERT },
    { P_LONG_SWORD, P_EXPERT },		{ P_TWO_HANDED_SWORD, P_BASIC },
    /*{ P_SCIMITAR, P_BASIC },		{ P_SABER, P_SKILLED },*/
    { P_CLUB, P_SKILLED },
    { P_MACE, P_SKILLED },              { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_EXPERT },              { P_HAMMER, P_BASIC },
    { P_POLEARMS, P_SKILLED },            { P_SPEAR, P_BASIC },
    { P_BOW, P_SKILLED },
    { P_DART, P_SKILLED },		{ P_SHURIKEN, P_EXPERT },
    { P_PROTECTION_SPELL, P_SKILLED },  { P_BODY_SPELL, P_SKILLED },
#ifdef STEED
    { P_RIDING, P_BASIC },
#endif
    { P_TWO_WEAPON_COMBAT, P_EXPERT },  { P_MARTIAL_ARTS, P_MASTER },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Ran[] = {
    { P_DAGGER, P_EXPERT },		 { P_KNIFE,  P_SKILLED },
    { P_AXE, P_SKILLED },	 { P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_BASIC },	 { P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_SKILLED },	 { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC }, { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },	 { P_JAVELIN, P_EXPERT },
    { P_TRIDENT, P_BASIC },	 { P_BOW, P_EXPERT },
    { P_SLING, P_EXPERT },	 { P_CROSSBOW, P_EXPERT },
    { P_DART, P_EXPERT },	 { P_SHURIKEN, P_SKILLED },
    { P_BOOMERANG, P_EXPERT },	 { P_WHIP, P_BASIC },
    { P_HEALING_SPELL, P_BASIC },
    { P_DIVINATION_SPELL, P_EXPERT },
    { P_BODY_SPELL, P_BASIC },
#ifdef STEED
    { P_RIDING, P_BASIC },
#endif
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Mus[] = {
    /* 5lo: Copy-paste of the Bard */
    { P_DAGGER, P_SKILLED },		{ P_KNIFE,  P_BASIC },
    { P_SHORT_SWORD, P_BASIC },		{ P_SCIMITAR, P_BASIC },
    { P_CLUB, P_SKILLED },		{ P_FLAIL, P_BASIC },
    { P_QUARTERSTAFF, P_SKILLED },	{ P_POLEARMS, P_BASIC },
    { P_JAVELIN, P_SKILLED },		{ P_SPEAR, P_BASIC },
    { P_SLING, P_SKILLED },		{ P_DART, P_EXPERT },	 
    { P_ENCHANTMENT_SPELL, P_SKILLED },	{ P_BODY_SPELL, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_MUSICALIZE, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_S[] = {
    { P_DAGGER, P_BASIC },		{ P_KNIFE,  P_SKILLED },
    { P_SHORT_SWORD, P_EXPERT },	{ P_BROAD_SWORD, P_SKILLED },
    { P_LONG_SWORD, P_EXPERT },		{ P_TWO_HANDED_SWORD, P_EXPERT },
    { P_SCIMITAR, P_BASIC },		{ P_SABER, P_BASIC },
    { P_PADDLE, P_BASIC },
    { P_FLAIL, P_SKILLED },		{ P_QUARTERSTAFF, P_BASIC },
    { P_POLEARMS, P_SKILLED },		{ P_SPEAR, P_BASIC },
    { P_JAVELIN, P_BASIC },		{ P_LANCE, P_SKILLED },
    { P_BOW, P_EXPERT },		{ P_SHURIKEN, P_EXPERT },
    { P_PROTECTION_SPELL, P_SKILLED },      { P_BODY_SPELL, P_BASIC },
    { P_MUSICALIZE, P_BASIC },

/* WAC - removed the attack spells, replace with body.  clairvoyance, meditate*/
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_EXPERT },
    { P_MARTIAL_ARTS, P_MASTER },
    { P_NONE, 0 }
};

#ifdef TOURIST
static const struct def_skill Skill_T[] = {
    { P_DAGGER, P_EXPERT },		{ P_KNIFE,  P_SKILLED },
    { P_AXE, P_BASIC },			{ P_PICK_AXE, P_BASIC },
    { P_SHORT_SWORD, P_EXPERT },	{ P_BROAD_SWORD, P_BASIC },
    { P_LONG_SWORD, P_BASIC },		{ P_TWO_HANDED_SWORD, P_BASIC },
    { P_SCIMITAR, P_SKILLED },		{ P_SABER, P_SKILLED },
    { P_PADDLE, P_BASIC },
    { P_MACE, P_BASIC },		{ P_MORNING_STAR, P_BASIC },
    { P_FLAIL, P_BASIC },		{ P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC },	{ P_POLEARMS, P_BASIC },
    { P_SPEAR, P_BASIC },		{ P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },		{ P_LANCE, P_BASIC },
    { P_BOW, P_BASIC },			{ P_SLING, P_BASIC },
#ifdef FIREARMS
    { P_FIREARM, P_BASIC },
#endif
    { P_CROSSBOW, P_BASIC },		{ P_DART, P_EXPERT },
    { P_SHURIKEN, P_BASIC },		{ P_BOOMERANG, P_BASIC },
    { P_WHIP, P_BASIC },		{ P_UNICORN_HORN, P_SKILLED },
    { P_DIVINATION_SPELL, P_BASIC },	{ P_ENCHANTMENT_SPELL, P_BASIC },
    { P_BODY_SPELL, P_SKILLED },        { P_MUSICALIZE, P_SKILLED },
/*WAC left alone*/
#ifdef STEED
    { P_RIDING, P_BASIC },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED }, { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};
#endif /* TOURIST */

static const struct def_skill Skill_U[] = {
/*WAC
 * -made dagger skill expert too,  since it's a starting weapon
 * -made spear skill Expert rather than Skilled
 *      Slayer artifact is a spear,  after all
 * -made crossbow skill Expert - Dracula movies
 * -changed bare handed basic to martial arts master-Buffy the Vampire Slayer
 * -Added whip Expert - Castlevania
 * -made club, flail, mace, morning star, hammer, quarterstaff Skilled
        from Expert to balance
 * -removed Trident skill - from Skilled to Restricted
 * -removed Lance skill - from Basic to Restricted
 */
    { P_DAGGER, P_EXPERT },             { P_LONG_SWORD, P_SKILLED },
    { P_CLUB, P_SKILLED },              { P_PADDLE, P_SKILLED },
    { P_MACE, P_SKILLED },              { P_MORNING_STAR, P_SKILLED },
    { P_FLAIL, P_SKILLED },             { P_HAMMER, P_SKILLED },
    { P_QUARTERSTAFF, P_SKILLED },      { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_EXPERT },              { P_JAVELIN, P_SKILLED },
    { P_BOW, P_BASIC },			        { P_SLING, P_BASIC },
#ifdef FIREARMS
    { P_FIREARM, P_EXPERT },
#endif
    { P_CROSSBOW, P_EXPERT },           { P_DART, P_BASIC },
    { P_SHURIKEN, P_BASIC },		{ P_BOOMERANG, P_BASIC },
    { P_WHIP, P_EXPERT },               { P_UNICORN_HORN, P_SKILLED },

    { P_PROTECTION_SPELL, P_SKILLED },    { P_BODY_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_BASIC },
/*WAC - added PROTECTION spells,  body spells as skilled, basic
        matter spells - for the fire vs undead*/
    { P_BARE_HANDED_COMBAT, P_GRAND_MASTER },    { P_NONE, 0 }
};

static const struct def_skill Skill_Unt[] = {
    /* 5lo: Same weapon set as necromancers + Expert in Knives/pick axes */
    { P_DAGGER, P_EXPERT },             { P_KNIFE,  P_EXPERT },
    { P_AXE, P_SKILLED },               { P_PICK_AXE, P_EXPERT },
    { P_CLUB, P_SKILLED },              { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },       { P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_BASIC },               { P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },             { P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },               { P_SHURIKEN, P_BASIC },
    /* 5lo: Same spell set as Undead Slayers */
    { P_PROTECTION_SPELL, P_SKILLED },    { P_BODY_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};

static const struct def_skill Skill_V[] = {
    { P_DAGGER, P_EXPERT },		{ P_AXE, P_EXPERT },
    { P_PICK_AXE, P_SKILLED },		{ P_SHORT_SWORD, P_SKILLED },
    { P_BROAD_SWORD, P_SKILLED },	{ P_LONG_SWORD, P_EXPERT },
    { P_TWO_HANDED_SWORD, P_EXPERT },	{ P_SCIMITAR, P_BASIC },
    { P_SABER, P_BASIC },		{ P_HAMMER, P_EXPERT },
    { P_QUARTERSTAFF, P_BASIC },	{ P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_SKILLED },		{ P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },		{ P_LANCE, P_SKILLED },
    { P_SLING, P_BASIC },
    { P_BODY_SPELL, P_BASIC },          { P_MATTER_SPELL, P_EXPERT },
/*  replace attack spell with matter spell - cone of cold, lightning
 *  Boosted to Expert
 */
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED },
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_W[] = {
    { P_DAGGER, P_EXPERT },		{ P_KNIFE,  P_SKILLED },
    { P_AXE, P_SKILLED },		{ P_SHORT_SWORD, P_BASIC },
    { P_CLUB, P_SKILLED },              { P_PADDLE, P_BASIC },
    { P_MACE, P_BASIC },
    { P_QUARTERSTAFF, P_EXPERT },	{ P_POLEARMS, P_SKILLED },
    { P_SPEAR, P_BASIC },		{ P_JAVELIN, P_BASIC },
    { P_TRIDENT, P_BASIC },		{ P_SLING, P_SKILLED },
    { P_DART, P_EXPERT },		{ P_SHURIKEN, P_BASIC },

    { P_ATTACK_SPELL, P_EXPERT },	{ P_HEALING_SPELL, P_SKILLED },
    { P_DIVINATION_SPELL, P_SKILLED },  { P_ENCHANTMENT_SPELL, P_SKILLED },
    { P_PROTECTION_SPELL, P_SKILLED },  { P_BODY_SPELL, P_SKILLED },
    { P_MATTER_SPELL, P_SKILLED },
/*      added matter spell skilled,  as fireball and cone of cold are
        matter spells, but now specialty of F/I Mages*/
#ifdef STEED
    { P_RIDING, P_BASIC },
#endif
    { P_BARE_HANDED_COMBAT, P_BASIC },
    { P_NONE, 0 }
};
#if 0 /* Deferred */
static const struct def_skill Skill_Pok[] = {
    /* 5lo: These can stay the same, honestly */
    { P_ATTACK_SPELL, P_EXPERT },	{ P_HEALING_SPELL, P_EXPERT },
    { P_DIVINATION_SPELL, P_EXPERT },  { P_ENCHANTMENT_SPELL, P_EXPERT },
    { P_PROTECTION_SPELL, P_EXPERT },  { P_BODY_SPELL, P_EXPERT },
    { P_MATTER_SPELL, P_EXPERT },
#ifdef STEED
    { P_RIDING, P_EXPERT },
#endif
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};
#endif
static const struct def_skill Skill_Zoo[] = {
    /* 5lo: Heavily modified Hobbit skillset from Slash'EM 0.0.6 */
    { P_DAGGER, P_EXPERT },             { P_KNIFE, P_BASIC },
    { P_CLUB, P_SKILLED },              { P_WHIP, P_EXPERT },
    { P_SHORT_SWORD, P_SKILLED },       { P_BROAD_SWORD, P_BASIC },
    { P_SCIMITAR, P_BASIC },            { P_HAMMER, P_BASIC },
    { P_QUARTERSTAFF, P_BASIC },        { P_SPEAR, P_EXPERT },
#ifdef FIREARMS
    { P_FIREARM, P_SKILLED }, /* think of game hunters */
#endif
    { P_SLING, P_SKILLED },             { P_CROSSBOW, P_BASIC },
    { P_DART, P_BASIC },                { P_UNICORN_HORN, P_SKILLED },
    { P_BOOMERANG, P_SKILLED },
#ifdef STEED
    { P_RIDING, P_SKILLED },
#endif
    { P_TWO_WEAPON_COMBAT, P_BASIC },
    { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }

};

#ifdef YEOMAN
static const struct def_skill Skill_Y[] = {
    { P_DAGGER, P_SKILLED },            { P_KNIFE, P_BASIC },
    { P_AXE, P_SKILLED },               { P_SHORT_SWORD, P_EXPERT },
    { P_BROAD_SWORD, P_BASIC },         { P_LONG_SWORD, P_SKILLED },
    { P_SABER, P_SKILLED },             { P_PADDLE, P_SKILLED },
    { P_HAMMER, P_BASIC },              { P_QUARTERSTAFF, P_SKILLED },
    { P_POLEARMS, P_EXPERT },           { P_SPEAR, P_BASIC },
    { P_JAVELIN, P_SKILLED },           { P_TRIDENT, P_SKILLED },
    { P_LANCE, P_SKILLED },             { P_BOW, P_EXPERT },
    { P_SLING, P_BASIC },
#ifdef FIREARMS
    { P_FIREARM, P_SKILLED },
#endif
    { P_CROSSBOW, P_SKILLED },          { P_DART, P_BASIC },
    { P_WHIP, P_BASIC },                { P_UNICORN_HORN, P_BASIC },

    { P_ENCHANTMENT_SPELL, P_SKILLED }, { P_PROTECTION_SPELL, P_SKILLED },
    { P_BODY_SPELL, P_BASIC },
#ifdef STEED
    { P_RIDING, P_EXPERT },
#endif
    { P_TWO_WEAPON_COMBAT, P_SKILLED }, { P_BARE_HANDED_COMBAT, P_SKILLED },
    { P_NONE, 0 }
};
#endif

#if 0 /* 5lo: No longer used */
/* Racial Skills here */

static const struct def_skill Skill_Drow[] = {
    { P_MUSICALIZE, P_SKILLED },	{ P_BOW, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Elf[] = {
    { P_MUSICALIZE, P_SKILLED },	{ P_BOW, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Dwarf[] = {
    { P_PICK_AXE, P_SKILLED },	{ P_AXE, P_SKILLED },
    { P_NONE, 0 }
};

/*static const struct def_skill Skill_Giant[] = {

};*/

static const struct def_skill Skill_Gnome[] = {
    { P_CROSSBOW, P_SKILLED },	{ P_CLUB, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Hobbit[] = {
    { P_SLING, P_EXPERT },
    { P_NONE, 0 }
};

/*static const struct def_skill Skill_Illithid[] = {

};*/

static const struct def_skill Skill_Kobold[] = {
    { P_SPEAR, P_BASIC },	{ P_DAGGER, P_BASIC },
    { P_DART, P_BASIC },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Lycanthrope[] = {
    { P_BARE_HANDED_COMBAT, P_EXPERT },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Nymph[] = {
    { P_MUSICALIZE, P_SKILLED},	{ P_HEALING_SPELL, P_SKILLED },
    { P_NONE, 0 }
};

static const struct def_skill Skill_Ogre[] = {
    { P_CLUB, P_EXPERT },
    { P_NONE, 0 }
};

/*static const struct def_skill Skill_Orc[] = {

};*/

static const struct def_skill Skill_Troll[] = {
    { P_POLEARMS, P_EXPERT },
    { P_NONE, 0 }
};
#endif
STATIC_OVL void
knows_object(obj)
register int obj;
{
	discover_object(obj,TRUE,FALSE);
	objects[obj].oc_pre_discovered = 1;	/* not a "discovery" */
}

/* Know ordinary (non-magical) objects of a certain class,
 * like all gems except the loadstone and luckstone.
 */
STATIC_OVL void
knows_class(sym)
register char sym;
{
	register int ct;
	for (ct = 1; ct < NUM_OBJECTS; ct++)
		if (objects[ct].oc_class == sym && !objects[ct].oc_magic)
			knows_object(ct);
}

/* Raise the player character's skill cap for a particular skill. */
void
augment_skill_cap(int skill, int augment, int minimum, int maximum)
{
	int count = 0;
	if (P_SKILL(skill) == P_ISRESTRICTED)
		P_SKILL(skill) = P_UNSKILLED;
	else if (P_SKILL(skill) != P_ISRESTRICTED)
		P_SKILL(skill) = P_BASIC;	
	while (count++ < augment && P_MAX_SKILL(skill) < maximum)
		P_MAX_SKILL(skill) = P_MAX_SKILL(skill) + 1;
	if (P_MAX_SKILL(skill) < minimum)
		P_MAX_SKILL(skill) = minimum;
}

/* [ALI] Raise one spell skill by one level. Priorities:
 * - The skill for the chosen spellbook if not already expert.
 * - A skill currently at skilled level.
 * - A skill currently at basic level.
 * Where more than one skill is possible at a priority level, choose one
 * at random.
 *
 * The idea is that where a role may be given spellbooks in which the
 * role is normally at basic level, then the Skill array can be tweaked
 * to reduce one skill from expert to skilled. After choosing the
 * spellbook we can then dynamically raise one skill which will either be
 * the one for the spellbook if that is currently basic (and so avoid the
 * warning message from skill_init) or raise the tweaked skill to expert.
 *
 * Currently only used by priests.
 */

static void
spellbook_skill_raise(class_skill, spellbook)
register struct def_skill *class_skill;
int spellbook;
{
    register int i, j;
    j = spell_skilltype(spellbook);
    for(i = 0; class_skill[i].skill != P_NONE; i++)
	if (class_skill[i].skill == j)
	    break;
    if (class_skill[i].skill == P_NONE)
	pline("Warning: No entry for %s in Skill array.",
	  obj_typename(spellbook));
    else if (class_skill[i].skmax < P_EXPERT)
	class_skill[i].skmax++;
    else
    {
	j = 0;
	for(i = 0; class_skill[i].skill != P_NONE; i++) {
	    if (class_skill[i].skill >= P_FIRST_SPELL &&
	      class_skill[i].skill <= P_LAST_SPELL &&
	      class_skill[i].skmax == P_SKILLED)
		j++;
	}
	if (j) {
	    j = rn2(j);
	    for(i = 0; class_skill[i].skill != P_NONE; i++) {
		if (class_skill[i].skill >= P_FIRST_SPELL &&
		  class_skill[i].skill <= P_LAST_SPELL &&
		  class_skill[i].skmax == P_SKILLED)
		    if (!j--) {
			class_skill[i].skmax++;
			break;
		    }
	    }
	}
	else {
	    for(i = 0; class_skill[i].skill != P_NONE; i++) {
		if (class_skill[i].skill >= P_FIRST_SPELL &&
		  class_skill[i].skill <= P_LAST_SPELL &&
		  class_skill[i].skmax >= P_BASIC &&
		  class_skill[i].skmax < P_EXPERT)
		    j++;
	    }
	    if (j) {
		j = rn2(j);
		for(i = 0; class_skill[i].skill != P_NONE; i++) {
		    if (class_skill[i].skill >= P_FIRST_SPELL &&
		      class_skill[i].skill <= P_LAST_SPELL &&
		      class_skill[i].skmax >= P_BASIC &&
		      class_skill[i].skmax < P_EXPERT)
			if (!j--) {
			    class_skill[i].skmax++;
			    break;
			}
		}
	    }
	}
    }
}

/* Know 5 to 15 random magical objects (wands, potions, scrolls, ...)
   For now we decide that tools while possibly magical (bag of holding/tricks,
   magic lamp) are excempt because the Bard already knows all instruments
   which appear to be more than enough tools.
   We might also add GEM_CLASS with oc_material != GLASS 
*** Contributed by Johanna Ploog */
STATIC_OVL void
know_random_obj()
{
        register int obj, count, ct;

        count = rn1(11,5);
        for (ct = 500; ct > 0 && count > 0; ct--) {
           obj = rn2(NUM_OBJECTS);
           if (objects[obj].oc_magic &&

               /* We have to make an exception for those dummy
                  objects (wand and scroll) that exist to allow
                  for additional descriptions. */

               obj_descr[(objects[obj].oc_name_idx)].oc_name != 0 /*&&

              (objects[obj].oc_class == ARMOR_CLASS &&*/

               /* Dragon scales and mails are considered magical,
                  but as they don't have different descriptions,
                  they don't appear in the discovery list,
                  so as not to rob the player of an opportunity... */

                /*!(obj > HELM_OF_TELEPATHY && obj < PLATE_MAIL) ||

               objects[obj].oc_class == RING_CLASS ||
               objects[obj].oc_class == POTION_CLASS ||
               objects[obj].oc_class == SCROLL_CLASS ||
               objects[obj].oc_class == SPBOOK_CLASS ||
               objects[obj].oc_class == WAND_CLASS ||
               objects[obj].oc_class == AMULET_CLASS)*/)
            {
              knows_object(obj);
              count--;
            }
        }
}

void
u_init()
{
	register int i, temp, racebounus, rolebounus, alignbounus, genderbounus, maxbounus;
	struct attack* attkptr;
	int no_extra_food = FALSE;

	flags.female = flags.initgend;
	flags.beginner = 1;

	/* WAC -- Clear Tech List since adjabil will init the 1st level techs*/
	for (i = 0; i <= MAXTECH; i++) tech_list[i].t_id = NO_TECH;

	/* Initialize spells */
	for (i = 0; i <= MAXSPELL; i++) spl_book[i].sp_id = NO_SPELL;

	/* Initialize the "u" structure.
	 * Note that some values may have been incorrectly set by a failed restore.
	 */
	(void) memset((genericptr_t)&u, 0, sizeof(u));
	setustuck((struct monst *)0);
#if 0	/* documentation of more zero values as desirable */
	u.usick_cause[0] = 0;
	u.uluck  = u.moreluck = 0;
# ifdef TOURIST
	uarmu = 0;
# endif
	uarm = uarmc = uarmh = uarms = uarmg = uarmf = 0;
	uwep = uball = uchain = uleft = uright = 0;
	/* WAC Added uswapwep, uquiver*/
	uswapwep = uquiver = 0;
	/* KMH -- added two-weapon combat */
	u.twoweap = 0;
	u.ublessed = 0;				/* not worthy yet */
	u.ugangr   = 0;				/* gods not angry */
	u.ugifts   = 0;				/* no divine gifts bestowed */
	/* WAC not needed - use techs */
/*	u.unextuse = 0;
	u.ulastuse = 0;*/
# ifdef ELBERETH
	u.uevent.uhand_of_elbereth = 0;
# endif
	u.uevent.uheard_tune = 0;
	u.uevent.uopened_dbridge = 0;
	u.uevent.udemigod = 0;		/* not a demi-god yet... */
	u.udg_cnt = 0;
	u.mh = u.mhmax = Upolyd = 0;
	u.uz.dnum = u.uz0.dnum = 0;
	u.utotype = 0;
#endif	/* 0 */

	u.uz.dlevel = 1;
	u.uz0.dlevel = 0;
	u.utolev = u.uz;

	u.umoved = FALSE;
	u.umortality = 0;
	u.ugrave_arise = Role_if(PM_PIRATE) ? PM_SKELETAL_PIRATE : NON_PM;
	
	u.ukinghill = 0;
	u.protean = 0;

	u.umonnum = u.umonster = (flags.female &&
			urole.femalenum != NON_PM) ? urole.femalenum :
			urole.malenum;

	init_uasmon();

	u.ulevel = 0;	/* set up some of the initial attributes */
	u.uhp = u.uhpmax = u.uhplast = newhp();
	u.uenmax = urole.enadv.infix + urace.enadv.infix;
	if (urole.enadv.inrnd > 0)
	    u.uenmax += rnd(urole.enadv.inrnd);
	if (urace.enadv.inrnd > 0)
	    u.uenmax += rnd(urace.enadv.inrnd);
	u.uen = u.uenmax;
	u.uspellprot = 0;
	adjabil(0,1);
	u.ulevel = u.ulevelmax = 1;

//	if(Race_if(PM_INCANTIFIER)) u.uenmax += 1200;
	init_uhunger();
	u.ublesscnt = 300;			/* no prayers just yet */

#ifdef MORE_SPAWNS
	/* This NEEDS to be set here, because otherwise saving/restoring will set
	 * a new value, allowing the player to cheat. --Amy */
	u.monstertimeout = rnd(10000)+15000;
	u.monstertimefinish = 70000L-u.monstertimeout;
#endif
	u.next_check = 600; /* 5lo: Different method of doing attribute checks in attrib.c */

/* In order to make the game even more interesting for lost souls, they cannot level teleport or branchport at all. */

	u.ualignbase[A_CURRENT] = u.ualignbase[A_ORIGINAL] = u.ualign.type =
			aligns[flags.initalign].value;
	u.ulycn = NON_PM;
#if defined(BSD) && !defined(POSIX_TYPES)
	(void) time((long *)&u.ubirthday);
#else
	(void) time(&u.ubirthday);
#endif
	/*
	 *  For now, everyone but elves, cavemen and lycanthropes starts
	 *  out with a night vision range of 1 and their xray range disabled.
	 */
	u.nv_range   =  1;
	u.xray_range = -1;

	/* Role-specific initializations */
	switch (Role_switch) {
	case PM_ARCHEOLOGIST:
		switch (rnd(5)) {   
		    case 1: Archeologist[A_BOOK].trotyp = SPE_DETECT_FOOD; break;
		    case 2: Archeologist[A_BOOK].trotyp = SPE_DETECT_MONSTERS; break;
		    case 3: Archeologist[A_BOOK].trotyp = SPE_LIGHT; break;
		    case 4: Archeologist[A_BOOK].trotyp = SPE_KNOCK; break;
		    case 5: Archeologist[A_BOOK].trotyp = SPE_WIZARD_LOCK; break;
		    default: break;
		}
		ini_inv(Archeologist);
		if(!rn2(4)) ini_inv(Blindfold);
#ifdef TOURIST
		else if(!rn2(4)) ini_inv(Towel);
		if(!rn2(4)) ini_inv(Leash);
#endif
		if(!rn2(4)) ini_inv(Tinopener);
		else if(!rn2(4))
		  (rn2(100) > 50 ? ini_inv(Lamp) : ini_inv(Torch));
		if(!rn2(8)) ini_inv(Magicmarker);
		knows_object(TOUCHSTONE);
		knows_object(SACK);
		skill_init(Skill_A);
		break;
	case PM_BARBARIAN:
		if (rn2(100) >= 50) {   /* see Elf comment */
		    Barbarian[B_MAJOR].trotyp = BATTLE_AXE;
		    Barbarian[B_MINOR].trotyp = SHORT_SWORD;
		}
		ini_inv(Barbarian);
		if(!rn2(6)) ini_inv(Torch);
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		skill_init(Skill_B);
		break;
	case PM_BARD:
		if (Race_if(PM_ORC)) Bard[BARD_INSTR].trotyp = (rn2(100) >= 50) ? BUGLE : TOOLED_HORN;
		else if (rn2(100) >= 50) Bard[BARD_INSTR].trotyp = WOODEN_FLUTE;
		if (rn2(100) >= 85) Bard[BARD_WHISTLE].trotyp = BELL;
		Bard[BARD_BOOZE].trquan = rn1(2, 5);
		ini_inv(Bard);
		/* This depends on the order in objects.c */
		for (i = TIN_WHISTLE; i <= DRUM_OF_EARTHQUAKE; i++)
			knows_object(i);
		/* Bards know about the enchantment spellbooks, though they don't know
		   the spells */
		knows_object(SPE_SLEEP);
		knows_object(SPE_CONFUSE_MONSTER);
		knows_object(SPE_SLOW_MONSTER);
		knows_object(SPE_CAUSE_FEAR);
		knows_object(SPE_CHARM_MONSTER);
		/* Bards also know a lot about legendary & magical stuff. */
		know_random_obj();
		skill_init(Skill_Bard);
		break;
	case PM_CAVEMAN:
		u.nv_range = 2;
		Cave_man[C_AMMO].trquan = rn1(11, 10);	/* 10..20 */
		ini_inv(Cave_man);
		skill_init(Skill_C);
		break;
	case PM_CHEF:
		ini_inv(Chef);
		skill_init(Skill_Chef);
		break;
#if 0
	case PM_POKEMON:
		ini_inv(Pokemon);
		skill_init(Skill_Pok);
		break;
#endif
	case PM_FLAME_MAGE:
		switch (rnd(2)) {                
			case 1: Flame_Mage[F_BOOK].trotyp = SPE_DETECT_MONSTERS; break;
			case 2: Flame_Mage[F_BOOK].trotyp = SPE_LIGHT; break;
			default: break;
		}
		ini_inv(Flame_Mage);
		if(!rn2(5)) ini_inv(Lamp);
		else if(!rn2(5)) ini_inv(Blindfold);
		else if(!rn2(5)) ini_inv(Magicmarker);
		skill_init(Skill_F);
		break;

	case PM_LUNATIC:
        ini_inv(Lunatic);
        skill_init(Skill_Lun);
        u.nv_range = 2;
		switch (rnd(7)) {
		case 1: 	    u.ulycn = PM_WEREWOLF; break;
		case 2: 	    u.ulycn = PM_WEREJACKAL; break;
		case 3: 	    u.ulycn = PM_WERERAT; break;
		case 4: 	    u.ulycn = PM_WEREPANTHER; break;
		case 5: 	    u.ulycn = PM_WERETIGER; break;
		case 6: 	    u.ulycn = PM_WERESNAKE; break;
		case 7: 	    u.ulycn = PM_WERESPIDER; break;
		default: 	    u.ulycn = PM_WEREWOLF; break;
		}
		break;

#ifdef CONVICT
	case PM_CONVICT:
        ini_inv(Convict);
        knows_object(SKELETON_KEY);
        knows_object(GRAPPLING_HOOK);
        skill_init(Skill_Con);
	u.ualign.sins += 16; /* You have sinned */
        /* On the verge of hungry */
		if(Race_if(PM_INCANTIFIER)){
			u.uen = 200;
		}
		else u.uhunger = 200;
    	u.ualignbase[A_CURRENT] = u.ualignbase[A_ORIGINAL] =
        u.ualign.type = A_CHAOTIC; /* Override racial alignment */
        urace.hatemask |= urace.lovemask;   /* Hated by the race's allies */
        urace.lovemask = 0; /* Convicts are pariahs of their race */
	change_luck(-1); /* both their alignment and luck start out negative */
        break;
#endif	/* CONVICT */
	case PM_HEALER:
#ifndef GOLDOBJ
		u.ugold = u.ugold0 = rn1(1000, 1001);
#else
		u.umoney0 = rn1(1000, 1001);
#endif
		ini_inv(Healer);
		knows_class(POTION_CLASS); /* WAC - remove? */
		knows_object(POT_SICKNESS);
		knows_object(POT_BLINDNESS);
		knows_object(POT_HALLUCINATION);
		knows_object(POT_RESTORE_ABILITY);
		knows_object(POT_FULL_HEALING);
		knows_object(HEALTHSTONE);	/* KMH */
		if(!rn2(5)) ini_inv(Lamp);
		if(!rn2(5)) ini_inv(Magicmarker);
		if(!rn2(5)) ini_inv(Blindfold);
		skill_init(Skill_H);
		break;
	case PM_ICE_MAGE:
		switch (rnd(2)) {                
			case 1: Ice_Mage[I_BOOK].trotyp = SPE_CONFUSE_MONSTER; break;
			case 2: Ice_Mage[I_BOOK].trotyp = SPE_SLOW_MONSTER; break;
			default: break;
		}
		ini_inv(Ice_Mage);
		if(!rn2(5)) ini_inv(Lamp);
		else if(!rn2(5)) ini_inv(Blindfold);
		else if(!rn2(5)) ini_inv(Magicmarker);
		skill_init(Skill_I);
		break;

	case PM_ELECTRIC_MAGE:
		switch (rnd(2)) {                
			case 1: Electric_Mage[E_BOOK].trotyp = SPE_LIGHT; break;
			case 2: Electric_Mage[E_BOOK].trotyp = SPE_SLOW_MONSTER; break;
			default: break;
		}
		ini_inv(Electric_Mage);
		if(!rn2(5)) ini_inv(Lamp);
		else if(!rn2(5)) ini_inv(Blindfold);
		else if(!rn2(5)) ini_inv(Magicmarker);
		skill_init(Skill_Ele);
		break;

	case PM_ACID_MAGE:
		switch (rnd(2)) {                
			case 1: Acid_Mage[ACID_BOOK].trotyp = SPE_CONFUSE_MONSTER; break;
			case 2: Acid_Mage[ACID_BOOK].trotyp = SPE_SLEEP; break;
			default: break;
		}
		ini_inv(Acid_Mage);
		if(!rn2(5)) ini_inv(Lamp);
		else if(!rn2(5)) ini_inv(Blindfold);
		else if(!rn2(5)) ini_inv(Magicmarker);
		skill_init(Skill_Aci);
		break;

	case PM_HACKER:
		Hacker[H_IC].trquan = rn2(7) + 1;
		ini_inv(Hacker);
		skill_init(Skill_Hack);
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		knows_object(PACK_OF_FLOPPIES);
		knows_object(POT_JOLT_COLA);
		knows_object(DIODE);
		knows_object(TRANSISTOR);
		knows_object(IC);
		break;

	case PM_GEEK:
		/* 5lo: Give them some lunch money */
#ifndef GOLDOBJ
		u.ugold = u.ugold0 = rnd(50);
#else
		u.umoney0 = rnd(50);
#endif
		ini_inv(Geek);
		skill_init(Skill_Gee);
		break;

#ifdef JEDI
	case PM_JEDI:
		ini_inv(Jedi);
#ifdef D_SABER
		switch(rnd(3)) {
			case 1: ini_inv(RedSaber); break;
			case 2: ini_inv(BlueSaber); break;
			case 3: ini_inv(GreenSaber); break;
			default: break;
		}
#else
		switch(rnd(2)) {
			case 1: ini_inv(RedSaber); break;
			case 2: ini_inv(GreenSaber); break;
			default: break;
		}
#endif
		if(!rn2(2)) ini_inv(Blindfold);
		skill_init(Skill_J);
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		break;
#endif
	case PM_KNIGHT:
		ini_inv(Knight);
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		/* give knights chess-like mobility
		 * -- idea from wooledge@skybridge.scl.cwru.edu */
		HJumping |= FROMOUTSIDE;
		skill_init(Skill_K);
		break;
	case PM_MONK:
		switch (rn2(90) / 30) {
		case 0: Monk[M_BOOK].trotyp = SPE_HEALING; break;
		case 1: Monk[M_BOOK].trotyp = SPE_PROTECTION; break;
		case 2: Monk[M_BOOK].trotyp = SPE_SLEEP; break;
		}
		ini_inv(Monk);
		if(!rn2(5)) ini_inv(Magicmarker);
		else if(!rn2(10)) ini_inv(Lamp);
		knows_class(ARMOR_CLASS);
		skill_init(Skill_Mon);
		break;
#if 0
	case PM_PSION:
		switch (rn2(90) / 30) {
		case 0: Psion[M_BOOK].trotyp = SPE_KNOCK; break;
		case 1: Psion[M_BOOK].trotyp = SPE_WIZARD_LOCK; break;
		case 2: Psion[M_BOOK].trotyp = SPE_CHARM_MONSTER; break;
		}
		ini_inv(Psion);
		if(!rn2(5)) ini_inv(Blindfold);
		else if(!rn2(10)) ini_inv(Magicmarker);
		knows_class(SPBOOK_CLASS);
		knows_class(ARMOR_CLASS);
		skill_init(Skill_Psi);
		break;
#endif
	case PM_NOBLEMAN:
		if(flags.female){
			Noble[NOB_SHIRT].trotyp = VICTORIAN_UNDERWEAR;
		}
		ini_inv(Noble);
		knows_class(ARMOR_CLASS);
		skill_init(Skill_Nob);
		break;
	case PM_PIRATE:
		Pirate[PIR_BULLET].trquan = rn1(10, 25);

#ifndef GOLDOBJ
		u.ugold = u.ugold0 = rnd(300);
#else
		u.umoney0 = rnd(300);
#endif
		Pirate[PIR_KNIVES].trquan = rn1(2, 2);
		if(!rn2(4)) Pirate[PIR_SNACK].trotyp = KELP_FROND;
		Pirate[PIR_SNACK].trquan += rn2(4);
		if(rn2(100)<50)	Pirate[PIR_JEWELRY].trotyp = RIN_ADORNMENT;
		if(rn2(100)<50)	Pirate[PIR_TOOL].trotyp = GRAPPLING_HOOK;
		ini_inv(Pirate);
		knows_object(OILSKIN_SACK);
		knows_object(OILSKIN_CLOAK);
		knows_object(GRAPPLING_HOOK);
		skill_init(Skill_Pir);
		break;
	case PM_CORSAIR:
#ifndef GOLDOBJ
		u.ugold = u.ugold0 = rnd(300);
#else
		u.umoney0 = rnd(300);
#endif
		ini_inv(Corsair);
		knows_object(OILSKIN_SACK);
		knows_object(OILSKIN_CLOAK);
		knows_object(GRAPPLING_HOOK);
		skill_init(Skill_Cor);
		break;
	case PM_GLADIATOR:
		ini_inv(Gladiator);
		skill_init(Skill_Gla);
		knows_class(ARMOR_CLASS);
		knows_class(WEAPON_CLASS);
		break;
	case PM_DIVER:
		ini_inv(Diver);
		knows_object(OILSKIN_SACK);
		knows_object(OILSKIN_CLOAK);
		knows_object(GRAPPLING_HOOK);
		skill_init(Skill_Div);
		break;
	case PM_NECROMANCER:
		switch (rnd(5)) {   
                    case 1: Necromancer[N_BOOK].trotyp = SPE_FORCE_BOLT; break;
                    case 2: Necromancer[N_BOOK].trotyp = SPE_KNOCK; break;
                    case 3: Necromancer[N_BOOK].trotyp = SPE_MAGIC_MISSILE; break;
                    case 4: Necromancer[N_BOOK].trotyp = SPE_CREATE_MONSTER; break;
                    case 5: Necromancer[N_BOOK].trotyp = SPE_WIZARD_LOCK; break;
		    default: break;
		}
		ini_inv(Necromancer);
		knows_class(SPBOOK_CLASS);
		if(!rn2(5)) ini_inv(Magicmarker);
		if(!rn2(5)) ini_inv(Blindfold);
		skill_init(Skill_N);
		break;
	case PM_WARRIOR:
		Warrior[WAR_BOLT].trquan = rn1(20, 26);
		ini_inv(Warrior);
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		skill_init(Skill_War);
		break;
	case PM_PRIEST:
		switch (rnd(9)) {   
		    case 1: Priest[P_BOOK].trotyp = SPE_FORCE_BOLT; break;
		    case 2: Priest[P_BOOK].trotyp = SPE_SLEEP; break;
		    case 3: Priest[P_BOOK].trotyp = SPE_RESIST_POISON; break;
		    case 4: Priest[P_BOOK].trotyp = SPE_RESIST_SLEEP; break;
		    case 5: Priest[P_BOOK].trotyp = SPE_DETECT_FOOD; break;
		    case 6: Priest[P_BOOK].trotyp = SPE_DETECT_MONSTERS; break;
		    case 7: Priest[P_BOOK].trotyp = SPE_LIGHT; break;
		    case 8: Priest[P_BOOK].trotyp = SPE_KNOCK; break;
		    case 9: Priest[P_BOOK].trotyp = SPE_WIZARD_LOCK; break;
		    default: break;
		}
		ini_inv(Priest);
		if(!rn2(10)) ini_inv(Magicmarker);
		else if(!rn2(10)) 
		  (rn2(100) > 50 ? ini_inv(Lamp) : ini_inv(Torch));
		knows_object(POT_WATER);
		spellbook_skill_raise(Skill_P, Priest[P_BOOK].trotyp);
		skill_init(Skill_P);
		/* KMH, conduct --
		 * Some may claim that this isn't agnostic, since they
		 * are literally "priests" and they have holy water.
		 * But we don't count it as such.  Purists can always
		 * avoid playing priests and/or confirm another player's
		 * role in their YAAP.
		 */
		break;
	case PM_PALADIN:
		ini_inv(Paladin);
		if(!rn2(10)) ini_inv(Magicmarker);
		else if(!rn2(10)) 
		  (rn2(100) > 50 ? ini_inv(Lamp) : ini_inv(Torch));
		knows_object(POT_WATER);
		skill_init(Skill_Pal);
		break;
	case PM_RANGER:
		Ranger[RAN_TWO_ARROWS].trquan = rn1(10, 50);
		Ranger[RAN_ZERO_ARROWS].trquan = rn1(10, 30);
		ini_inv(Ranger);
		skill_init(Skill_Ran);
		break;
	case PM_ROGUE:
		Rogue[R_DAGGERS].trquan = rn1(10, 6);
		Rogue[R_DARTS].trquan = rn1(10, 25);
#ifdef FIREARMS
		if (rn2(100) < 30) {
			Rogue[R_DAGGERS].trotyp = PISTOL;
			Rogue[R_DAGGERS].trquan = 1;
			Rogue[R_DARTS].trotyp = BULLET;
		}
#endif
#ifndef GOLDOBJ
		u.ugold = u.ugold0 = rn1(500 ,1500);
#else
		u.umoney0 = rn1(500 ,1500);
#endif
		ini_inv(Rogue);
		if(!rn2(5)) ini_inv(Blindfold);
		knows_object(OILSKIN_SACK);
		skill_init(Skill_R);
		break;

	case PM_NINJA:
		knows_class(WEAPON_CLASS);
		Ninja[NIN_SHURIKEN].trquan = rn1(9, 6);
		ini_inv(Ninja);
		skill_init(Skill_Nin);
		break;

	case PM_DRUNK:
		ini_inv(Drunk);
		skill_init(Skill_Dru);
		break;

	case PM_OFFICER:
		Officer[COP_BULLET].trquan = rn1(10, 30);
		ini_inv(Officer);
		skill_init(Skill_Off);
		break;

	case PM_UNDERTAKER:
		ini_inv(Undertaker);
		skill_init(Skill_Unt);
		break;

	case PM_MUSICIAN:
		ini_inv(Musician);
		skill_init(Skill_Mus);
		break;

	case PM_ZOOKEEPER:
		ini_inv(Zookeeper);
		skill_init(Skill_Zoo);
		break;

	case PM_FIREFIGHTER:
		ini_inv(Firefighter);
		skill_init(Skill_Fir);
		break;

	case PM_LOCKSMITH:
		ini_inv(Locksmith);
		skill_init(Skill_Loc);
		break;

	case PM_SAMURAI:
		Samurai[S_ARROWS].trquan = rn1(20, 26);
		ini_inv(Samurai);
		if(!rn2(5)) ini_inv(Blindfold);
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		skill_init(Skill_S);
		break;
#ifdef TOURIST
	case PM_TOURIST:
		Tourist[T_DARTS].trquan = rn1(20, 21);
#ifndef GOLDOBJ
		u.ugold = u.ugold0 = rn1(500,1000);
#else
		u.umoney0 = rn1(500,1000);
#endif
		ini_inv(Tourist);
		if(!rn2(25)) ini_inv(Tinopener);
		else if(!rn2(25)) ini_inv(Leash);
		else if(!rn2(25)) ini_inv(Towel);
		else if(!rn2(25)) ini_inv(Magicmarker);
		skill_init(Skill_T);
		break;
#endif /* TOURIST */
	case PM_UNDEAD_SLAYER:
		switch (rn2(100) / 25) {
		    case 0:	/* Pistol and silver bullets */
#ifdef FIREARMS
			UndeadSlayer[U_MINOR].trotyp = PISTOL;
			UndeadSlayer[U_RANGE].trotyp = SILVER_BULLET;
			UndeadSlayer[U_RANGE].trquan = rn1(10, 30);
			break;
#endif
		    case 1:	/* Crossbow and bolts */
			UndeadSlayer[U_MINOR].trotyp = CROSSBOW;
			UndeadSlayer[U_RANGE].trotyp = CROSSBOW_BOLT;
			UndeadSlayer[U_RANGE].trquan = rn1(10, 30);
			UndeadSlayer[U_MISC].trotyp = LOW_BOOTS;
			UndeadSlayer[U_MISC].trspe = 1;
			UndeadSlayer[U_ARMOR].trotyp = LEATHER_JACKET;
			UndeadSlayer[U_ARMOR].trspe = 1;
			/* helmet & armour are no longer candidates for
			 * substitution for orcish versions so no extra
			 * food should be given in compensation.
			 */
			if (Race_if(PM_ORC))
			    no_extra_food = TRUE;
		        break;
		    case 2:	/* Whip and daggers */
		        UndeadSlayer[U_MINOR].trotyp = BULLWHIP;
		        UndeadSlayer[U_MINOR].trspe = 2;
		        break;
		    case 3:	/* Silver spear and daggers */
			break;
		}
		ini_inv(UndeadSlayer);
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		if(!rn2(6)) ini_inv(Lamp);
		skill_init(Skill_U);
		break;
	case PM_VALKYRIE:
		ini_inv(Valkyrie);
		if(!rn2(6)) 
		  (rn2(100) > 50 ? ini_inv(Lamp) : ini_inv(Torch));
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		skill_init(Skill_V);
		break;
	case PM_WIZARD:
		switch (rnd(2)) {                
		    case 1: Wizard[W_BOOK1].trotyp = SPE_FORCE_BOLT; break;
		    case 2: Wizard[W_BOOK1].trotyp = SPE_SLEEP; break;
		    default: break;
		}
		switch (rnd(2)) {
		    case 1: Wizard[W_BOOK2].trotyp = SPE_RESIST_POISON; break;
		    case 2: Wizard[W_BOOK2].trotyp = SPE_RESIST_SLEEP; break;
		    default: break;
		}
		switch (rnd(5)) {   
		    case 1: Wizard[W_BOOK3].trotyp = SPE_DETECT_FOOD; break;
		    case 2: Wizard[W_BOOK3].trotyp = SPE_DETECT_MONSTERS; break;
		    case 3: Wizard[W_BOOK3].trotyp = SPE_LIGHT; break;
		    case 4: Wizard[W_BOOK3].trotyp = SPE_KNOCK; break;
		    case 5: Wizard[W_BOOK3].trotyp = SPE_WIZARD_LOCK; break;
		    default: break;
		}
		switch (rnd(9)) {
		    case 1: Wizard[W_BOOK4].trotyp = SPE_MAGIC_MISSILE; break;
		    case 2: Wizard[W_BOOK4].trotyp = SPE_CONFUSE_MONSTER; break;
		    case 3: Wizard[W_BOOK4].trotyp = SPE_SLOW_MONSTER; break;
		    case 4: Wizard[W_BOOK4].trotyp = SPE_CURE_BLINDNESS; break;
		    case 5: Wizard[W_BOOK4].trotyp = SPE_ENDURE_HEAT; break;
		    case 6: Wizard[W_BOOK4].trotyp = SPE_ENDURE_COLD; break;
		    case 7: Wizard[W_BOOK4].trotyp = SPE_INSULATE; break;
		    case 8: Wizard[W_BOOK4].trotyp = SPE_CREATE_MONSTER; break;
		    case 9: Wizard[W_BOOK4].trotyp = SPE_HEALING; break;
		    default: break;
		}
		ini_inv(Wizard);
		knows_class(SPBOOK_CLASS);
		if(!rn2(5)) ini_inv(Magicmarker);
		if(!rn2(5)) ini_inv(Blindfold);
		skill_init(Skill_W);
		break;

#ifdef YEOMAN
	case PM_YEOMAN:
		ini_inv(Yeoman);
		knows_class(WEAPON_CLASS);
		knows_class(ARMOR_CLASS);
		skill_init(Skill_Y);
		break;
#endif

	default:	/* impossible */
		break;
	}

	/*** Race-specific initializations ***/
	switch (Race_switch) {
	case PM_HUMAN:
	    /* Nothing special */
	    break;

	case PM_INCANTIFIER:
//	    skill_add(Skill_Incantifier);
	    if (!Role_if(PM_HEALER)) ini_inv(HealingBook);
	    if (!Role_if(PM_WIZARD)) ini_inv(ForceBook);
	    if (Role_if(PM_WIZARD) || Role_if(PM_HEALER)) ini_inv(ExtraBook);
	    augment_skill_cap(P_ATTACK_SPELL, 1, P_EXPERT, P_EXPERT);
	    augment_skill_cap(P_HEALING_SPELL, 1, P_EXPERT, P_EXPERT);
	    augment_skill_cap(P_DIVINATION_SPELL, 1, P_EXPERT, P_EXPERT);
	    augment_skill_cap(P_ENCHANTMENT_SPELL, 1, P_EXPERT, P_EXPERT);
	    augment_skill_cap(P_PROTECTION_SPELL, 1, P_EXPERT, P_EXPERT);
	    augment_skill_cap(P_BODY_SPELL, 1, P_EXPERT, P_EXPERT);
	    augment_skill_cap(P_MATTER_SPELL, 1, P_EXPERT, P_EXPERT);
	    knows_object(SCR_BLANK_PAPER);
	    knows_object(SPE_BLANK_PAPER);
    	    break;

	case PM_ELF:
	    /*
	     * Elves are people of music and song, or they are warriors.
	     * Non-warriors get an instrument.  We use a kludge to
	     * get only non-magic instruments.
	     */
	    if (Role_if(PM_PRIEST) || Role_if(PM_WIZARD)) {
		static int trotyp[] = {
		    WOODEN_FLUTE, TOOLED_HORN, WOODEN_HARP,
		    BELL, BUGLE, LEATHER_DRUM
		};
		Instrument[0].trotyp = trotyp[rn2(SIZE(trotyp))];
		ini_inv(Instrument);
	    }

	    /* Elves can recognize all elvish objects */
	    knows_object(ELVEN_SHORT_SWORD);
	    knows_object(ELVEN_ARROW);
	    knows_object(ELVEN_BOW);
	    knows_object(ELVEN_SPEAR);
	    knows_object(ELVEN_DAGGER);
	    knows_object(ELVEN_BROADSWORD);
	    knows_object(ELVEN_MITHRIL_COAT);
	    knows_object(ELVEN_LEATHER_HELM);
	    knows_object(ELVEN_SHIELD);
	    knows_object(ELVEN_BOOTS);
	    knows_object(ELVEN_CLOAK);
//	    skill_add(Skill_Elf);
	    augment_skill_cap(P_MUSICALIZE, 1, P_SKILLED, P_EXPERT);
	    augment_skill_cap(P_BOW, 1, P_SKILLED, P_EXPERT);
	    break;
	case PM_DROW:
	    /* Drows can recognize all droven objects */
	    knows_object(DARK_ELVEN_SHORT_SWORD);
	    knows_object(DARK_ELVEN_ARROW);
	    knows_object(DARK_ELVEN_BOW);
	    knows_object(DARK_ELVEN_DAGGER);
	    knows_object(DARK_ELVEN_MITHRIL_COAT);
//	    skill_add(Skill_Drow);
	    augment_skill_cap(P_MUSICALIZE, 1, P_BASIC, P_SKILLED);
	    augment_skill_cap(P_BOW, 1, P_SKILLED, P_EXPERT);
	    augment_skill_cap(P_SABER, 1, P_BASIC, P_SKILLED);
	    break;

	case PM_DWARF:
	    /* Dwarves can recognize all dwarvish objects */
	    knows_object(DWARVISH_SPEAR);
	    knows_object(DWARVISH_SHORT_SWORD);
	    knows_object(DWARVISH_MATTOCK);
	    knows_object(DWARVISH_IRON_HELM);
	    knows_object(DWARVISH_MITHRIL_COAT);
	    knows_object(DWARVISH_CLOAK);
	    knows_object(DWARVISH_ROUNDSHIELD);
//	    skill_add(Skill_Dwarf);
	    augment_skill_cap(P_PICK_AXE, 1, P_SKILLED, P_EXPERT);
	    augment_skill_cap(P_AXE, 1, P_SKILLED, P_EXPERT);
	    augment_skill_cap(P_HAMMER, 1, P_SKILLED, P_EXPERT);
	    break;

	case PM_GNOME:
	    knows_object(GNOMISH_HELM);
	    knows_object(GNOMISH_BOOTS);
	    knows_object(GNOMISH_SUIT);
//	    skill_add(Skill_Gnome);
	    if(!Role_if(PM_CAVEMAN)) augment_skill_cap(P_CROSSBOW, 1, P_BASIC, P_SKILLED);
	    augment_skill_cap(P_CLUB, 1, P_BASIC, P_SKILLED);
	    augment_skill_cap(P_PICK_AXE, 1, P_BASIC, P_BASIC);
	    break;
	case PM_HUMAN_WEREWOLF:
	    if (!Role_if(PM_LUNATIC)) u.ulycn = PM_WEREWOLF;
/*	    u.nv_range = 2;
	    u.uen = u.uenmax += 6;
	    ini_inv(Lycanthrope);*/
//	    skill_add(Skill_Lycanthrope);
	    augment_skill_cap(P_BARE_HANDED_COMBAT, 2, P_EXPERT, P_MASTER);
	    augment_skill_cap(P_SHORT_SWORD, 1, P_SKILLED, P_EXPERT);
	    break;

	case PM_ORC:
	    /* compensate for generally inferior equipment */
	    if (!no_extra_food && !Role_if(PM_WIZARD) &&
		    !Role_if(PM_FLAME_MAGE) && !Role_if(PM_ICE_MAGE) &&
		    !Role_if(PM_ELECTRIC_MAGE) && !Role_if(PM_ACID_MAGE) &&
		    !Role_if(PM_NECROMANCER))
#ifdef CONVICT
        if (!Role_if(PM_CONVICT))
#endif /* CONVICT */
		ini_inv(Xtra_food);
	    /* Orcs can recognize all orcish objects */
	    knows_object(ORCISH_SHORT_SWORD);
	    knows_object(ORCISH_ARROW);
	    knows_object(ORCISH_BOW);
	    knows_object(ORCISH_SPEAR);
	    knows_object(ORCISH_DAGGER);
	    knows_object(ORCISH_CHAIN_MAIL);
	    knows_object(ORCISH_RING_MAIL);
	    knows_object(ORCISH_HELM);
	    knows_object(ORCISH_SHIELD);
	    knows_object(URUK_HAI_SHIELD);
	    knows_object(ORCISH_CLOAK);
	    augment_skill_cap(P_SCIMITAR, 1, P_SKILLED, P_EXPERT);
	    break;
	case PM_VAMPIRE:
	    /* Vampires start off with gods not as pleased, luck penalty */
	    /* 5lo: Unless they're a necromancer */
	    if (!Role_if(PM_NECROMANCER) && u.ualign.type == A_CHAOTIC ) {
	    adjalign(-5); 
	    u.ualign.sins += 5;
	    change_luck(-1);
	    }
	    knows_object(POT_BLOOD);
	    knows_object(POT_VAMPIRE_BLOOD);
	    augment_skill_cap(P_DAGGER, 1, P_BASIC, P_SKILLED);
	    augment_skill_cap(P_KNIFE, 1, P_BASIC, P_SKILLED);
	    augment_skill_cap(P_BARE_HANDED_COMBAT, 1, P_BASIC, P_SKILLED);
	    break;
	case PM_KOBOLD:
	    if(!Role_if(PM_CONVICT) && !Role_if(PM_CAVEMAN)) {
		KoboldItem[0].trquan = rn1(10, 5);
		ini_inv(KoboldItem);
		if(rn2(2)) ini_inv(KoboldItemB);		
		else ini_inv(KoboldItemC);
	    }
//	    skill_add(Skill_Kobold);
	    augment_skill_cap(P_DART, 1, P_BASIC, P_SKILLED);
	    augment_skill_cap(P_SPEAR, 1, P_BASIC, P_SKILLED);
	    if(!Role_if(PM_CAVEMAN)) augment_skill_cap(P_DAGGER, 1, P_BASIC, P_SKILLED);
	    break;
	case PM_GHOUL:
	    knows_object(POT_BLOOD);
	    knows_object(POT_VAMPIRE_BLOOD);
	    augment_skill_cap(P_BARE_HANDED_COMBAT, 1, P_SKILLED, P_EXPERT);
	    augment_skill_cap(P_ATTACK_SPELL, 1, P_BASIC, P_SKILLED);
	    break;
	case PM_CLOCKWORK_AUTOMATON:
            if(!Role_if(PM_CONVICT)) ini_inv(AutomatonItem);
	    augment_skill_cap(P_LONG_SWORD, 1, P_BASIC, P_SKILLED);
	    break;
	case PM_TROLL:
		if(!Role_if(PM_CONVICT)) switch (rnd(4)) {
			case 1: ini_inv(TrollItemA); break;
			case 2: ini_inv(TrollItemB); break;
			case 3: ini_inv(TrollItemC); break;
			case 4: ini_inv(TrollItemD); break;
			default: break;
		}
//		skill_add(Skill_Troll);
		augment_skill_cap(P_POLEARMS, 1, P_SKILLED, P_EXPERT);
		break;
	case PM_OGRE:
         	if(!Role_if(PM_CONVICT)) ini_inv(OgreItem);
		augment_skill_cap(P_CLUB, 1, P_SKILLED, P_EXPERT);
//		skill_add(Skill_Ogre);
		break;
	case PM_GIANT:
		augment_skill_cap(P_TWO_WEAPON_COMBAT, 1, P_SKILLED, P_EXPERT);
		if(!Role_if(PM_CAVEMAN)) augment_skill_cap(P_TWO_HANDED_SWORD, 1, P_SKILLED, P_EXPERT);
		break;
	case PM_HOBBIT:
		augment_skill_cap(P_SLING, 1, P_SKILLED, P_EXPERT);
//		skill_add(Skill_Hobbit);
		break;
	case PM_NYMPH:
		augment_skill_cap(P_MUSICALIZE, 1, P_SKILLED, P_EXPERT);
		augment_skill_cap(P_HEALING_SPELL, 1, P_BASIC, P_SKILLED);
//		skill_add(Skill_Nymph);
		break;
	case PM_DOPPELGANGER:
		if(!Role_if(PM_CAVEMAN)) augment_skill_cap(P_DAGGER, 1, P_BASIC, P_SKILLED);
		augment_skill_cap(P_BODY_SPELL, 1, P_SKILLED, P_EXPERT);
		break;
	case PM_ILLITHID:
		augment_skill_cap(P_ATTACK_SPELL, 1, P_EXPERT, P_EXPERT);
		augment_skill_cap(P_MATTER_SPELL, 1, P_EXPERT, P_EXPERT);
		augment_skill_cap(P_ENCHANTMENT_SPELL, 1, P_SKILLED, P_EXPERT);
		augment_skill_cap(P_DIVINATION_SPELL, 1, P_SKILLED, P_EXPERT);
		break;
	default:	/* impossible */
		break;
	}
#ifdef EASY_MODE
		/* Players will start with a few healing and identify items now. --Amy */
          ini_inv(AlwaysStartItem);
          ini_inv(AlwaysStartItemB);
#endif /* EASY_MODE */
	if (flags.lostsoul && !flags.uberlostsoul) { 
          ini_inv(LostSoulItem);  /* In Angband or TOME these would be scrolls of cure hunger instead of food rations. */
          ini_inv(LostSoulItemX);
          ini_inv(LostSoulItemY);
	} 

	if (flags.uberlostsoul) { /* lots of items, but you're gonna need them! --Amy */
          ini_inv(LostSoulItem);
          ini_inv(LostSoulItemX);
          ini_inv(LostSoulItemY);
          ini_inv(UberLostSoulItemA);
          ini_inv(UberLostSoulItemB);
          if (!rn2(20)) ini_inv(UberLostSoulItemC);
          if (!rn2(20)) ini_inv(UberLostSoulItemD);
          if (!rn2(25)) ini_inv(UberLostSoulItemE);
          if (!rn2(50)) ini_inv(UberLostSoulItemF);
          if (!rn2(33)) ini_inv(UberLostSoulItemG);
          if (!rn2(75)) ini_inv(UberLostSoulItemH);
          if (!rn2(100)) ini_inv(UberLostSoulItemI);
          if (!rn2(40)) ini_inv(UberLostSoulItemJ);
          if (!rn2(5)) ini_inv(UberLostSoulItemK);
          if (!rn2(25)) ini_inv(UberLostSoulItemL);
          if (!rn2(10)) ini_inv(UberLostSoulItemM);
          if (!rn2(5)) ini_inv(UberLostSoulItemN);
          if (!rn2(100)) ini_inv(UberLostSoulItemO);
          if (!rn2(100)) ini_inv(UberLostSoulItemP);
          if (!rn2(500)) ini_inv(UberLostSoulItemQ);
          if (!rn2(20)) ini_inv(UberLostSoulItemR);
          if (!rn2(40)) ini_inv(UberLostSoulItemR);
          if (!rn2(80)) ini_inv(UberLostSoulItemR);
          if (!rn2(160)) ini_inv(UberLostSoulItemR);
          if (!rn2(320)) ini_inv(UberLostSoulItemR);
          if (!rn2(640)) ini_inv(UberLostSoulItemR);
          if (!rn2(1280)) ini_inv(UberLostSoulItemR);
	} 
	/* Even if you actually get most of these items (by being lucky or savescumming), your level 1 character probably
	 * won't stand a chance on dlvl 64 with no means of levelporting or branchporting, so this is still fair. --Amy */

	/*knows_class(VENOM_CLASS);*/

	if (discover)
		ini_inv(Wishing);

#ifdef WIZARD
	if (wizard)
		read_wizkit();
#endif

#ifndef GOLDOBJ
	u.ugold0 += hidden_gold();	/* in case sack has gold in it */
#else
	if (u.umoney0) ini_inv(Money);
	u.umoney0 += hidden_gold();	/* in case sack has gold in it */
#endif

	temp = rn1(10,70);
	init_attr(temp);		/* init attribute values */
	find_ac();			/* get initial ac value */
	max_rank_sz();			/* set max str size for class ranks */
/*
 *	Do we really need this?
 */
	for(i = 0; i < A_MAX; i++)
	    if(!rn2(20)) {
		register int xd = rn2(7) - 2;	/* biased variation */
		(void) adjattrib(i, xd, TRUE);
		if (ABASE(i) < AMAX(i)) AMAX(i) = ABASE(i);
	    }
	/* make sure you can carry all you have - especially for Tourists */
	/* but also prevent players from having ridiculous starting strength or constitution, especially for ghasts --Amy */
	while (inv_weight() > 0 && ( (weight_cap() < 1000 && rn2(10)) || (weight_cap() > 999 && rn2(2)) ) ) {

		if (ABASE(A_STR) < 18) {
			if (adjattrib(A_STR, 1, TRUE)) continue;
		}
		if (ABASE(A_CON) < 18) {
			if (adjattrib(A_CON, 1, TRUE)) continue;
		}
		/* only get here when didn't boost strength or constitution */
		break;
	}

	return;
}


/* skills aren't initialized, so we use the role-specific skill lists */
STATIC_OVL boolean
restricted_spell_discipline(otyp)
int otyp;
{
    const struct def_skill *skills;
    int this_skill = spell_skilltype(otyp);

    switch (Role_switch) {
     case PM_ARCHEOLOGIST:	skills = Skill_A; break;
     case PM_BARBARIAN:		skills = Skill_B; break;
     case PM_BARD:		skills = Skill_Bard; break;
     case PM_CAVEMAN:		skills = Skill_C; break;
#ifdef CONVICT
     case PM_CONVICT:		skills = Skill_Con; break;
#endif  /* CONVICT */
     case PM_LUNATIC:		skills = Skill_Lun; break;
     case PM_PALADIN:		skills = Skill_Pal; break;
     case PM_ELECTRIC_MAGE:		skills = Skill_Ele; break;
     case PM_ACID_MAGE:		skills = Skill_Aci; break;
     case PM_FLAME_MAGE:		skills = Skill_F; break;
     case PM_HACKER:		skills = Skill_Hack; break;
     case PM_HEALER:		skills = Skill_H; break;
     case PM_JEDI:		skills = Skill_J; break;
     case PM_KNIGHT:		skills = Skill_K; break;
     case PM_WARRIOR:		skills = Skill_War; break;
     case PM_MONK:		skills = Skill_Mon; break;
#if 0
     case PM_PSION:		skills = Skill_Psi; break;
     case PM_POKEMON:		skills = Skill_Pok; break;
#endif
     case PM_PIRATE:		skills = Skill_Pir; break;
     case PM_CORSAIR:		skills = Skill_Cor; break;
     case PM_GLADIATOR:		skills = Skill_Gla; break;
     case PM_DIVER:		skills = Skill_Div; break;
     case PM_PRIEST:		skills = Skill_P; break;
     case PM_RANGER:		skills = Skill_Ran; break;
     case PM_ROGUE:		skills = Skill_R; break;
     case PM_SAMURAI:		skills = Skill_S; break;
#ifdef TOURIST
     case PM_TOURIST:		skills = Skill_T; break;
#endif
     case PM_VALKYRIE:		skills = Skill_V; break;
     case PM_WIZARD:		skills = Skill_W; break;

     case PM_DRUNK:		skills = Skill_Dru; break;
     case PM_MUSICIAN:		skills = Skill_Mus; break;
     case PM_LOCKSMITH:		skills = Skill_Loc; break;
     case PM_FIREFIGHTER:		skills = Skill_Fir; break;
     case PM_OFFICER:		skills = Skill_Off; break;
     case PM_ZOOKEEPER:		skills = Skill_Zoo; break;
     case PM_NINJA:		skills = Skill_Nin; break;
     case PM_UNDERTAKER:		skills = Skill_Unt; break;
     case PM_GEEK:		skills = Skill_Gee; break;

     default:			skills = 0; break;	/* lint suppression */
    }

    while (skills->skill != P_NONE) {
	if (skills->skill == this_skill) return FALSE;
	++skills;
    }
    return TRUE;
}

STATIC_OVL void
ini_inv(trop)
register struct trobj *trop;
{
	struct obj *obj;
	int otyp, i;

	while (trop->trclass) {
		if (trop->trotyp != UNDEF_TYP) {
			otyp = (int)trop->trotyp;
			if (urace.malenum != PM_HUMAN) {
			    /* substitute specific items for generic ones */
			    for (i = 0; inv_subs[i].race_pm != NON_PM; ++i)
				if (inv_subs[i].race_pm == urace.malenum &&
					otyp == inv_subs[i].item_otyp) {
				    otyp = inv_subs[i].subs_otyp;
				    break;
				}
			}
			for (i = 0; inv_asubs[i].align != A_NONE; ++i)
			    if (inv_asubs[i].align == u.ualign.type &&
				    otyp == inv_asubs[i].item_otyp) {
				otyp = inv_asubs[i].subs_otyp;
				break;
			    }
			obj = mksobj(otyp, TRUE, FALSE);
			if(obj->otyp == POT_BLOOD)
				obj->corpsenm = PM_HUMAN;
		} else {	/* UNDEF_TYP */
			static NEARDATA short nocreate = STRANGE_OBJECT;
			static NEARDATA short nocreate2 = STRANGE_OBJECT;
			static NEARDATA short nocreate3 = STRANGE_OBJECT;
			static NEARDATA short nocreate4 = STRANGE_OBJECT;
			static NEARDATA short nocreate5 = STRANGE_OBJECT;
			static NEARDATA short nocreate6 = STRANGE_OBJECT;
			static NEARDATA short nocreate7 = STRANGE_OBJECT;
		/*
		 * For random objects, do not create certain overly powerful
		 * items: wand of wishing, ring of levitation, or the
		 * polymorph/polymorph control combination.  Specific objects,
		 * i.e. the discovery wishing, are still OK.
		 * Also, don't get a couple of really useless items.  (Note:
		 * punishment isn't "useless".  Some players who start out with
		 * one will immediately read it and use the iron ball as a
		 * weapon.)
		 */
			obj = mkobj(trop->trclass, FALSE);
			otyp = obj->otyp;
			while (otyp == WAN_WISHING
				|| otyp == WAN_ACQUIREMENT
				|| otyp == SCR_WISHING
				|| otyp == SCR_ACQUIREMENT
				|| otyp == nocreate
				|| otyp == nocreate2
				|| otyp == nocreate3
				|| otyp == nocreate4
				|| otyp == nocreate5
				|| otyp == nocreate6
				|| otyp == nocreate7
				|| otyp == RIN_LEVITATION
				|| ((Role_if(PM_FLAME_MAGE) || Role_if(PM_ICE_MAGE))
						&&
				    (otyp == RIN_FIRE_RESISTANCE || 
				     otyp == RIN_COLD_RESISTANCE ||
				     otyp == SPE_ENDURE_HEAT ||
				     otyp == SPE_ENDURE_COLD))

				/* KMH -- Hobbits shouldn't get ring of invis. */
				 || (Role_if(PM_HOBBIT) && otyp == RIN_INVISIBILITY)

				/* 5lo -- Doppelgangers shouldn't get ring of poly control. */
				 || (Race_if(PM_DOPPELGANGER) && otyp == RIN_POLYMORPH_CONTROL)

				/* Nor should nymphs get these rings. */
				 || (Race_if(PM_NYMPH) &&
					(otyp == RIN_TELEPORTATION
					 || otyp == RIN_TELEPORT_CONTROL))


				/* KMH, balance patch -- now an amulet */
				|| (Role_if(PM_NECROMANCER) &&
						otyp == AMULET_OF_DRAIN_RESISTANCE)
				/* 5lo -- Drunks shouldn't get these */
				|| ((Role_if(PM_DRUNK)) &&
					(otyp == POT_BLOOD
					|| otyp == POT_VAMPIRE_BLOOD
					|| otyp == POT_PAN_GALACTIC_GARGLE_BLASTE
					|| otyp == POT_CYANIDE
					|| otyp == POT_RADIUM))


				/* 'useless' or over powerful items */
				|| otyp == POT_HALLUCINATION
				|| otyp == POT_ACID
				|| otyp == POT_INVULNERABILITY
				|| otyp == POT_HEROISM
				|| otyp == SCR_AMNESIA
				|| otyp == SCR_FIRE
				|| otyp == SCR_BLANK_PAPER
				|| otyp == SPE_BLANK_PAPER
#ifdef NEPHI_PHOTOGRAPHY
				|| otyp == SCR_PHOTOGRAPH
				|| otyp == SPE_PHOTO_ALBUM
#endif
				|| otyp == RIN_AGGRAVATE_MONSTER
				|| otyp == RIN_HUNGER
				|| otyp == RIN_SLEEPING
				|| otyp == WAN_NOTHING
				/* 5lo: Adding TnG special objects */
				|| otyp == SCR_ROOT_PASSWORD_DETECTION
				|| otyp == AMULET_OF_DATA_STORAGE
				|| otyp == RIN_TIMELY_BACKUP
				/* Monks don't use weapons */
				|| (otyp == SCR_ENCHANT_WEAPON &&
				    Role_if(PM_MONK))
				/* wizard patch -- they already have one */
				|| (otyp == SPE_FORCE_BOLT &&
				    (Role_if(PM_WIZARD) || Race_if(PM_INCANTIFIER)))
				|| (otyp == SPE_HEALING &&
				    Race_if(PM_INCANTIFIER))
				/* powerful spells are either useless to
				   low level players or unbalancing; also
				   spells in restricted skill categories */
				|| (obj->oclass == SPBOOK_CLASS &&
				    (objects[otyp].oc_level > 3 ||
				    restricted_spell_discipline(otyp)))
							) {
				dealloc_obj(obj);
				obj = mkobj(trop->trclass, FALSE);
				otyp = obj->otyp;
			}

#ifdef JEDI
			if (is_lightsaber(obj))
				obj->age = 1500;
			/* start with maxed lightsaber
			only Jedi start with one, so no need to check
			Role_if here */
#endif

			/* Don't start with +0 or negative rings */
			if (objects[otyp].oc_charged && obj->spe </*=*/ 0)
				obj->spe = rne(2);
#if 0
			if ((obj->oclass == WEAPON_CLASS || obj->oclass == ARMOR_CLASS) && !rn2(3))
				obj->spe = rne(2);
#endif
			/* Heavily relies on the fact that 1) we create wands
			 * before rings, 2) that we create rings before
			 * spellbooks, and that 3) not more than 1 object of a
			 * particular symbol is to be prohibited.  (For more
			 * objects, we need more nocreate variables...)
			 */
			switch (otyp) {
			    case WAN_POLYMORPH:
			    case RIN_POLYMORPH:
			    case POT_POLYMORPH:
				nocreate = RIN_POLYMORPH_CONTROL;
				break;
			    case RIN_POLYMORPH_CONTROL:
				nocreate = RIN_POLYMORPH;
				nocreate2 = SPE_POLYMORPH;
				nocreate3 = POT_POLYMORPH;
			}
			/* Don't have 2 of the same ring or spellbook */
			if (obj->oclass == RING_CLASS ||
			    obj->oclass == SPBOOK_CLASS)
				if(nocreate4 == STRANGE_OBJECT) nocreate4 = otyp;
				else if(nocreate5 == STRANGE_OBJECT) nocreate5 = otyp;
				else if(nocreate6 == STRANGE_OBJECT) nocreate6 = otyp;
				else if(nocreate7 == STRANGE_OBJECT) nocreate7 = otyp;
		}

#ifdef GOLDOBJ
		if (trop->trclass == COIN_CLASS) {
			/* no "blessed" or "identified" money */
			obj->quan = u.umoney0;
		} else {
#endif
			obj->dknown = obj->bknown = obj->rknown = 1;
			if (objects[otyp].oc_uses_known) obj->known = 1;
			obj->cursed = 0;
			if (obj->opoisoned && u.ualign.type != A_CHAOTIC)
			    obj->opoisoned = 0;
			if (obj->oclass == WEAPON_CLASS ||
				obj->oclass == TOOL_CLASS) {
			    obj->quan = (long) trop->trquan;
			    trop->trquan = 1;
			} else if (obj->oclass == GEM_CLASS &&
				is_graystone(obj) && obj->otyp != FLINT) {
			    obj->quan = 1L;
			}
#ifdef CONVICT
            if (obj->otyp == STRIPED_SHIRT ) {
                obj->cursed = TRUE;
            }
#endif /* CONVICT */
            if (obj->otyp == LOADSTONE ) {
                obj->cursed = TRUE;
            }
			if (trop->trspe != UNDEF_SPE)
			    obj->spe = trop->trspe;
			if (trop->trbless != UNDEF_BLESS)
			    obj->blessed = trop->trbless;
#ifdef GOLDOBJ
		}
#endif
#ifdef NEPHI_PHOTOGRAPHY
		/* for a touch of color, put predefined photo in photo album */
		/* do before calculating weight */
		if(obj->otyp == SPE_PHOTO_ALBUM) {
			struct obj *photo;
			photo = make_special_photo(PHOTO_SPECIAL_FAM);
			bless(photo);
			photo->bknown = 1;
			(void)add_to_container(obj,photo);
		}
#endif
		/* defined after setting otyp+quan + blessedness */
		obj->owt = weight(obj);
				
		obj = addinv(obj);

		/* Make the type known if necessary */
		if (OBJ_DESCR(objects[otyp]) && obj->known)
			discover_object(otyp, TRUE, FALSE);
		if (otyp == OIL_LAMP)
			discover_object(POT_OIL, TRUE, FALSE);

		if(obj->oclass == ARMOR_CLASS){
			if (is_shield(obj) && !uarms) {
				setworn(obj, W_ARMS);
				if (uswapwep) 
				  setuswapwep((struct obj *) 0, TRUE);
			} else if (is_helmet(obj) && !uarmh)
				setworn(obj, W_ARMH);
			else if (is_gloves(obj) && !uarmg)
				setworn(obj, W_ARMG);
#ifdef TOURIST
			else if (is_shirt(obj) && !uarmu)
				setworn(obj, W_ARMU);
#endif
			else if (is_cloak(obj) && !uarmc)
				setworn(obj, W_ARMC);
			else if (is_boots(obj) && !uarmf)
				setworn(obj, W_ARMF);
			else if (is_suit(obj) && !uarm)
				setworn(obj, W_ARM);
		}

		if (obj->oclass == WEAPON_CLASS || is_weptool(obj) ||
			otyp == TIN_OPENER || otyp == FLINT || otyp == ROCK) {
		    if (is_ammo(obj) || is_missile(obj)) {
			if (!uquiver) setuqwep(obj);
		    } else if (!uwep) setuwep(obj, FALSE);
		    else if (!uswapwep) setuswapwep(obj, FALSE);
		}
		if (obj->oclass == SPBOOK_CLASS &&
				obj->otyp != SPE_BLANK_PAPER
#ifdef NEPHI_PHOTOGRAPHY
				&& obj->otyp != SPE_PHOTO_ALBUM
#endif
				)
		    initialspell(obj);

#if !defined(PYRAMID_BUG) && !defined(MAC)
		if(--trop->trquan) continue;	/* make a similar object */
#else
		if(trop->trquan) {		/* check if zero first */
			--trop->trquan;
			if(trop->trquan)
				continue;	/* make a similar object */
		}
#endif
		
		trop++;
	}

	/*if (!strncmpi(plname, "lostsoul", 8)) { 
	goto_level(&medusa_level, FALSE, FALSE, FALSE); inspired by Tome, an Angband mod --Amy
	}*/

}

/*u_init.c*/
