/*	SCCS Id: @(#)objects.c	3.4	2002/07/31	*/
/* Copyright (c) Mike Threepoint, 1989.				  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef OBJECTS_PASS_2_
/* first pass */
struct monst { struct monst *dummy; };	/* lint: struct obj's union */
#include "config.h"
#include "obj.h"
#include "objclass.h"
#include "prop.h"
#include "skills.h"
#include "skills.h"

#else	/* !OBJECTS_PASS_2_ */
/* second pass */
#include "color.h"
#  define COLOR_FIELD(X) X,
#endif	/* !OBJECTS_PASS_2_ */



/* objects have symbols: ) [ = " ( % ! ? + / $ * ` 0 _ . */

/*
 *	Note:  OBJ() and BITS() macros are used to avoid exceeding argument
 *	limits imposed by some compilers.  The ctnr field of BITS currently
 *	does not map into struct objclass, and is ignored in the expansion.
 *	The 0 in the expansion corresponds to oc_pre_discovered, which is
 *	set at run-time during role-specific character initialization.
 */

#ifndef OBJECTS_PASS_2_
/* first pass -- object descriptive text */
# define OBJ(name,desc) name,desc
# define OBJECT(obj,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,nut,color) \
	{obj}

NEARDATA struct objdescr obj_descr[] = {
#else
/* second pass -- object definitions */

# define BITS(nmkn,mrg,uskn,ctnr,mgc,chrg,uniq,nwsh,big,tuf,dir,sub,mtrl) \
	nmkn,mrg,uskn,0,mgc,chrg,uniq,nwsh,big,tuf,dir,mtrl,sub /* SCO ODT 1.1 cpp fodder */
# define OBJECT(obj,bits,prp,sym,prob,dly,wt,cost,sdam,ldam,oc1,oc2,nut,color) \
	{0, 0, (char *)0, bits, prp, sym, dly, COLOR_FIELD(color) \
	 prob, wt, cost, sdam, ldam, oc1, oc2, nut}
# ifndef lint
#  define HARDGEM(n) (n >= 8)
# else
#  define HARDGEM(n) (0)
# endif

NEARDATA struct objclass objects[] = {
#endif
/* dummy object[0] -- description [2nd arg] *must* be NULL */
	OBJECT(OBJ("strange object",(char *)0), BITS(1,0,0,0,0,0,0,0,0,0,0,0,0),
			0, ILLOBJ_CLASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),

/* weapons ... */
#define WEAPON(name,app,kn,mg,bi,prob,wt,cost,sdam,ldam,hitbon,typ,sub,metal,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,mg,1,0,0,1,0,0,bi,0,typ,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, 0, wt, color )
#define PROJECTILE(name,app,kn,prob,wt,cost,sdam,ldam,hitbon,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,1,1,0,0,1,0,0,0,0,PIERCE,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, WP_GENERIC, wt, color )
#define BOW(name,app,kn,bi,prob,wt,cost,hitbon,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,0,1,0,0,1,0,0,bi,0,0,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, 0, 0, hitbon, WP_GENERIC, wt, color )
#define BULLET(name,app,kn,prob,wt,cost,sdam,ldam,hitbon,ammotyp,typ,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,1,1,0,0,1,0,0,0,0,typ,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, ammotyp, wt, color )
#define GUN(name,app,kn,bi,prob,wt,cost,range,rof,hitbon,ammotyp,metal,sub,color) \
	OBJECT( \
		OBJ(name,app), BITS(kn,0,1,0,0,1,0,0,bi,0,0,sub,metal), 0, \
		WEAPON_CLASS, prob, 0, \
		wt, cost, range, rof, hitbon, ammotyp, wt, color )

/* Note: for weapons that don't do an even die of damage (ex. 2-7 or 3-18)
 * the extra damage is added on in weapon.c, not here! */

#define P PIERCE
#define S SLASH
#define B WHACK
#define E EXPLOSION

/* Daggers */
WEAPON("orcish dagger", "crude dagger",
	0, 1, 0,  5, 4,  4,  2,  1, 1, P,   P_DAGGER, IRON, CLR_BLACK),
WEAPON("dagger", (char *)0,
	1, 1, 0,  9, 4,  4,  3,  2, 1, P,   P_DAGGER, IRON, HI_METAL),
WEAPON("athame", (char *)0,
	1, 1, 0,  2, 4,  4,  4,  3, 1, S,   P_DAGGER, IRON, HI_METAL),
WEAPON("silver dagger", (char *)0,
	1, 1, 0,  2, 4, 40,  3,  2, 1, P,   P_DAGGER, SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
	/* WAC silver dagger now pierces, to be same as other daggers
        allows it to be thrown without penalty as well*/
WEAPON("elven dagger", "runed dagger",
	0, 1, 0,  5, 4,  4,  3,  3, 1, P,   P_DAGGER, WOOD, HI_METAL),
WEAPON("dark elven dagger", "black runed dagger",
	0, 1, 0,  2, 4,  4,  4,  4, 1, P,   P_DAGGER, WOOD, CLR_BLACK),

WEAPON("sharpened pencil", (char *)0,
	1, 1, 0,  1, 2,  4,  2,  2, 1, P,   P_DAGGER, WOOD, CLR_YELLOW),
/* 5lo: From Nethack--, very low damage but can engrave without dulling */
WEAPON("wooden stake", (char *)0,
	1, 0, 0,  2, 8, 50,  4,  3, 1, P,   P_DAGGER, WOOD, HI_WOOD),
	/* STEPHEN WHITE'S NEW CODE */
    /* Base for artifact (Stake of Van Helsing) */
WEAPON("great dagger", (char *)0,
	1, 0, 0,  2, 8,500,  5,  4, 1, P,   P_DAGGER, METAL, CLR_BLACK),
	/* STEPHEN WHITE'S NEW CODE */
	/* for necromancer artifact... */

/* Knives */
WEAPON("worm tooth", (char *)0,
	1, 0, 0,  2, 2,  2,  2,  2, 0, 0,   P_KNIFE, 0, CLR_WHITE),
WEAPON("knife", (char *)0,
	1, 1, 0, 7,  2,  4,  3,  2, 0, P|S, P_KNIFE, IRON, HI_METAL),
WEAPON("stiletto", (char *)0,
	1, 1, 0,  5,  2,  4,  3,  2, 1, P|S, P_KNIFE, IRON, HI_METAL),
WEAPON("scalpel", (char *)0,
	1, 1, 0,  2,  2,  4,  3,  3, 2, S,   P_KNIFE, METAL, HI_METAL),
WEAPON("crysknife", (char *)0,
	1, 0, 0,  2, 2,100, 20, 30, 3, P,   P_KNIFE, MINERAL, CLR_WHITE),

WEAPON("tooth of an algolian suntiger",	"very sharp tooth",
	0, 0, 0,  1, 10,500, 15, 15, 5, 0, P_KNIFE, MINERAL, CLR_WHITE),

	/* [Tom] increased crysknife damage from d10/d10 */
	/* to d20/d30 (otherwise, it's useless to make them...) */

/* Axes */
WEAPON("axe", (char *)0,
	1, 0, 0, 35, 24,  8,  8,  4, 0, S,   P_AXE, IRON, HI_METAL),
WEAPON("battle-axe", "double-headed axe", /* "double-bitted" ? */
	0, 0, 1, 10,48, 40,  12,  9, 0, S,   P_AXE, IRON, HI_METAL),

/* Pick-axes */
/* (also weptool pick-axe) */
WEAPON("dwarvish mattock", "broad pick",
	0, 0, 1, 13,48, 50, 18,  12,0, B,   P_PICK_AXE, IRON, HI_METAL),

/* Short swords */
WEAPON("orcish short sword", "crude short sword",
	0, 0, 0,  3, 12, 10,  5,  8, 0, P,   P_SHORT_SWORD, IRON, CLR_BLACK),
WEAPON("short sword", (char *)0,
	1, 0, 0,  6, 12, 10,  6,  8, 0, P,   P_SHORT_SWORD, IRON, HI_METAL),
WEAPON("silver short sword", (char *)0,
	1, 0, 0,  2, 12, 50,  6,  8, 0, P,   P_SHORT_SWORD, SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
WEAPON("dwarvish short sword", "broad short sword",
	0, 0, 0,  2, 12, 10,  7,  8, 0, P,   P_SHORT_SWORD, IRON, HI_METAL),
WEAPON("elven short sword", "runed short sword",
	0, 0, 0,  2, 12, 10,  8,  8, 0, P,   P_SHORT_SWORD, WOOD, HI_METAL),
WEAPON("dark elven short sword", "black runed short sword",
	0, 0, 0,  2, 12, 10,  9,  9, 0, P,   P_SHORT_SWORD, WOOD, CLR_BLACK),

/* Broadswords */
WEAPON("broadsword", (char *)0,
	1, 0, 0, 18, 30, 10,  7,  10, 0, S,   P_BROAD_SWORD, IRON, HI_METAL),
						/* +d4 small, +1 large */
WEAPON("runesword", "runic broadsword",
	0, 0, 0,  2, 30,300,  7,  10, 0, S,   P_BROAD_SWORD, IRON, CLR_BLACK),
	/* +d4 small, +1 large; base for artifact (Stormbringer) */
	/* +5d2 +d8 from level drain */
WEAPON("elven broadsword", "runed broadsword",
	0, 0, 0,  4, 30, 10,  10,  10, 0, S,   P_BROAD_SWORD, WOOD, HI_METAL),
						/* +d4 small, +1 large */

/* Long swords */
WEAPON("long sword", (char *)0,
	1, 0, 0, 45, 16, 15,  8, 12, 0, S,   P_LONG_SWORD, IRON, HI_METAL),
WEAPON("silver long sword", (char *)0,
	1, 0, 0,  2, 16, 75,  8, 12, 0, S,   P_LONG_SWORD,SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
WEAPON("katana", "samurai sword",
	0, 0, 0,  4, 16, 80, 10, 12, 1, S,   P_LONG_SWORD, IRON, HI_METAL),
WEAPON("electric sword", (char *)0,
	1, 0, 0,  1, 16,200, 12, 16, 2, S,   P_LONG_SWORD, IRON, HI_METAL),

/* Two-handed swords */
WEAPON("two-handed sword", (char *)0,
	1, 0, 1, 25,60, 50, 18,  9, 0, S,   P_TWO_HANDED_SWORD, IRON, HI_METAL),
	/* +2d6 large */
WEAPON("tsurugi", "long samurai sword",
	0, 0, 1,  2, 24,500, 24,  12, 2, S,   P_TWO_HANDED_SWORD, METAL, HI_METAL),
	/* +2d6 large; base for artifact (T of Muramasa) */

/* Scimitars */
WEAPON("scimitar", "curved sword",
	0, 0, 0, 14, 16, 15,  8,  8, 0, S,   P_SCIMITAR, IRON, HI_METAL),
WEAPON("bent sable", "bent sword",
	0, 0, 0, 1, 16, 15,  12,  12, 0, S,   P_SCIMITAR, IRON, HI_METAL),

/* Sabers */
WEAPON("rapier", (char *)0,
	1, 0, 0,  2, 12, 40,  6,  8,  0, P,  P_SABER, METAL, CLR_BLACK),
	/* STEPHEN WHITE'S NEW CODE */
	/* Base for artifact (Scalpel) */
WEAPON("silver saber", (char *)0,
	1, 0, 0, 24, 12, 75,  8,  8, 0, S,   P_SABER, SILVER, HI_SILVER),
WEAPON("golden saber", (char *)0,
	1, 0, 0,  1, 12, 75,  10,  10, 0, S,   P_SABER, GOLD, CLR_YELLOW),

/* Clubs */
WEAPON("club", (char *)0,
	1, 0, 0, 12, 12,  3,  6,  3, 0, B,   P_CLUB, WOOD, HI_WOOD),
WEAPON("aklys", "thonged club",
	0, 0, 0, 10, 7,  4,  7,  3, 0, B,   P_CLUB, IRON, HI_METAL),
WEAPON("baseball bat", (char *)0,
	1, 0, 0,  4, 12, 50,  8,  6, 0, B,   P_CLUB, WOOD, HI_WOOD),
WEAPON("metal club", "heavy club",
	0, 0, 0, 1, 7,  4,  10,  8, 0, B,   P_CLUB, IRON, HI_METAL),

	/* STEPHEN WHITE'S NEW CODE */
	/* Base for artifact */

/* Paddles */
/* Good to-hit and small damage, but low large damage */
WEAPON("fly swatter", (char *)0,
	1, 0, 0,  10, 4,  3, 15,  2, 2, B,   P_PADDLE, PLASTIC, CLR_GREEN),
WEAPON("insect squasher", (char *)0,
	1, 0, 0,  1, 4,  3, 30,  2, 4, B,   P_PADDLE, PLASTIC, CLR_GREEN),

/* Maces */
WEAPON("silver mace", (char *)0,
	1, 0, 0, 12, 12, 65,  6,  7, 0, B,   P_MACE, SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
WEAPON("mace", (char *)0,
	1, 0, 0, 35, 12,  5,  6,  7, 0, B,   P_MACE, IRON, HI_METAL),
	/* +1 small */

WEAPON("flanged mace", (char *)0,
	1, 0, 0,  1, 12,  5,  8,  9, 0, B,   P_MACE, IRON, HI_METAL),

/* Morning stars */
WEAPON("morning star", (char *)0,
	1, 0, 0, 20,48, 10,  4, 12, 0, B,   P_MORNING_STAR, IRON, HI_METAL),
						/* +d4 small, +1 large */

WEAPON("jagged star", (char *)0,
	1, 0, 0, 4,48, 10,  7,  15, 0, B,   P_MORNING_STAR, IRON, HI_METAL),

WEAPON("devil star", (char *)0,
	1, 0, 0, 1,48, 10,  12,  18, 0, B,   P_MORNING_STAR, IRON, HI_METAL),

/* Flails */
WEAPON("flail", (char *)0,
	1, 0, 0, 30, 7,  4,  6,  4, 1, B,   P_FLAIL, IRON, HI_METAL),
	/* +1 small, +1d4 large */

WEAPON("knout", (char *)0,
	1, 0, 0,  6, 7,  4,  9,  6, 2, B,   P_FLAIL, IRON, HI_METAL),

WEAPON("obsid", (char *)0,
	1, 0, 0,  1, 7,  4,  12,  8, 3, B,   P_FLAIL, IRON, HI_METAL),

/* Hammers */
WEAPON("war hammer", (char *)0,
	1, 0, 0, 24, 20,  5,  7,  7, 0, B,   P_HAMMER, IRON, HI_METAL),
	/* +1 small */
WEAPON("heavy hammer", (char *)0,
	1, 0, 0,  2, 24,500, 10, 10, 0, B,   P_HAMMER, METAL, HI_METAL),
	/* STEPHEN WHITE'S NEW CODE */
	/* Base for artifact */

WEAPON("mallet", (char *)0,
	1, 0, 0,  1, 20,500, 14, 14, 0, B,   P_HAMMER, WOOD, HI_WOOD),

/* Yes, these are melee weapons for whacking enemies to death. Hammer skill, for obvious reasons. --Amy */
WEAPON("wedged little-girl sandal", (char *)0,
	1, 0, 0, 1, 1,  5,  4,  4, 2, B,   P_HAMMER, MINERAL, CLR_WHITE),
WEAPON("soft girl sneaker", (char *)0,
	1, 0, 0, 1, 2,  100,  2,  2, 5, B,   P_HAMMER, LEATHER, CLR_RED),
WEAPON("sturdy plateau boot for girls", (char *)0,
	1, 0, 0, 1, 10,  200,  6,  6, 1, B,   P_HAMMER, METAL, CLR_BRIGHT_BLUE),
WEAPON("hugging boot", (char *)0,
	1, 0, 0, 1, 5,  100,  9,  8, 0, B,   P_HAMMER, MITHRIL, CLR_ORANGE),
WEAPON("block-heeled combat boot", (char *)0,
	1, 0, 0, 1, 5,  100,  11,  3, 0, B,   P_HAMMER, LEATHER, CLR_BLACK),
/* Some of these are expensive because they're supposed to be brands like Nike, Adidas etc. */
/* We won't use those brand names in-game though. */

/* adult versions - these do more damage, but monsters will rarely start with them in their inventory */
WEAPON("wooden geta", (char *)0,
	1, 0, 0, 1, 1,  5,  10,  10, 0, B,   P_HAMMER, WOOD, CLR_BROWN),
WEAPON("lacquered dancing shoe", (char *)0,
	1, 0, 0, 1, 2,  75,  6,  11, 0, B,   P_HAMMER, MINERAL, CLR_BLACK),
WEAPON("high-heeled sandal", (char *)0,
	1, 0, 0, 1, 1,  10,  9,  7, 0, B,   P_HAMMER, MINERAL, CLR_WHITE),
WEAPON("sexy leather pump", (char *)0,
	1, 0, 0, 1, 2,  50,  8, 10, 4, B,   P_HAMMER, LEATHER, CLR_BRIGHT_MAGENTA),
WEAPON("spiked battle boot", (char *)0,
	1, 0, 0, 1, 5,  150,  9,  9, 2, B,   P_HAMMER, METAL, HI_METAL),

/* Quarterstaves */
WEAPON("quarterstaff", "staff",
	0, 0, 1, 10, 16,  5,  9,  9, 0, B,   P_QUARTERSTAFF, WOOD, HI_WOOD),
WEAPON("silver capped staff", (char *)0,
	0, 0, 1, 2,  16, 100, 9,  9, 0, B,	 P_QUARTERSTAFF, SILVER, HI_SILVER),
WEAPON("battle staff", (char *)0,
	0, 0, 1, 1, 16,  155,  15,  15, 0, B,   P_QUARTERSTAFF, METAL, HI_METAL),

/* Polearms */
/* (also weptool fishing pole) */
/* spear-type */
WEAPON("partisan", "vulgar polearm",
	0, 0, 1,  3, 32, 10, 10,  9, 0, P,   P_POLEARMS, IRON, HI_METAL),
						/* +1 large */
WEAPON("glaive", "single-edged polearm",
	0, 0, 1,  4, 32,  6,  9, 15, 2, S,   P_POLEARMS, IRON, HI_METAL),
WEAPON("spetum", "forked polearm",
	0, 0, 1,  3, 20,  5,  9,  9, 1, P,   P_POLEARMS, IRON, HI_METAL),
						/* +1 small, +d6 large */
WEAPON("ranseur", "hilted polearm",
	0, 0, 1,  3, 20,  6,  6,  6, 3, P,   P_POLEARMS, IRON, HI_METAL),
	/* +d4 both */
/* axe-type */
WEAPON("bardiche", "long poleaxe",
	0, 0, 1,  2,48,  7,  6,  6, 0, S,   P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small, +2d4 large */
WEAPON("voulge", "pole cleaver",
	0, 0, 1,  2,48,  5,  6,  6, 1, S,   P_POLEARMS, IRON, HI_METAL),
						/* +d4 both */
WEAPON("halberd", "angled poleaxe",
	0, 0, 1,  4,60, 10, 15,  9, 0, P|S, P_POLEARMS, IRON, HI_METAL),
	/* +1d6 large */
/* curved/hooked */
WEAPON("fauchard", "pole sickle",
	0, 0, 1,  3, 24,  5,  9,  12, 1, P|S, P_POLEARMS, IRON, HI_METAL),
WEAPON("guisarme", "pruning hook",
	0, 0, 1,  3, 32,  5,  6,  13, 1, S,   P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
WEAPON("bill-guisarme", "hooked polearm",
	0, 0, 1,  2,48,  7,  6, 15, 1, P|S, P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
/* other */
WEAPON("lucern hammer", "pronged polearm",
	0, 0, 1,  3,60,  7,  6,  16, 0, B|P, P_POLEARMS, IRON, HI_METAL),
						/* +1d4 small */
WEAPON("bec de corbin", "beaked polearm",
	0, 0, 1,  2,40,  8,  15,  9, 2, B|P, P_POLEARMS, IRON, HI_METAL),

/* Spears */
WEAPON("orcish spear", "crude spear",
	0, 1, 0, 10, 12,  3,  5,  8, 0, P,   P_SPEAR, IRON, CLR_BLACK),
WEAPON("spear", (char *)0,
	1, 1, 0, 26, 12,  3,  6,  8, 0, P,   P_SPEAR, IRON, HI_METAL),
WEAPON("silver spear", (char *)0,
	1, 1, 0,  3, 12, 40,  6,  8, 0, P,   P_SPEAR, SILVER, HI_SILVER),
	/* STEPHEN WHITE'S NEW CODE */
WEAPON("elven spear", "runed spear",
	0, 1, 0,  7, 12,  3,  7,  8, 0, P,   P_SPEAR, WOOD, HI_METAL),
WEAPON("dwarvish spear", "stout spear",
	0, 1, 0,  9, 12,  3,  8,  8, 0, P,   P_SPEAR, IRON, HI_METAL),

/* Javelins */
WEAPON("javelin", "throwing spear",
	0, 1, 0, 15, 8,  3,  6,  6, 1, P,   P_JAVELIN, IRON, HI_METAL),
WEAPON("spirit thrower", (char *)0,
	0, 1, 0, 5, 8,  3,  10,  10, 1, P,   P_JAVELIN, IRON, HI_METAL),
WEAPON("torpedo", (char *)0,
	0, 1, 0, 1, 8,  3,  16,  16, 1, P,   P_JAVELIN, IRON, HI_METAL),

/* Tridents */
WEAPON("trident", (char *)0,
	1, 0, 0,  12, 10,  5,  6,  4, 1, P,   P_TRIDENT, IRON, HI_METAL),
	/* +1 small, +2d4 large */

WEAPON("stygian pike", (char *)0,
	1, 0, 0,  1, 10,  5,  13,  11, 4, P,   P_TRIDENT, IRON, HI_METAL),

/* Lances */
WEAPON("lance", (char *)0,
	1, 0, 0,  1,75, 10,  6,  8, 0, P,   P_LANCE, IRON, HI_METAL),

/* Bows (and arrows) */
/* KMH, balance patch -- damage of launchers restored to d2 big and small */
BOW("orcish bow", "crude bow",	0, 1, 10, 12, 60, 0, WOOD, P_BOW, CLR_BLACK),
BOW("bow", (char *)0,			1, 1, 24, 12, 60, 0, WOOD, P_BOW, HI_WOOD),
BOW("elven bow", "runed bow",	0, 1, 12, 12, 60, 0, WOOD, P_BOW, HI_WOOD),
BOW("dark elven bow",  "black runed bow", 0, 1, 2, 12, 60, 1, WOOD, P_BOW, CLR_BLACK),
BOW("yumi", "long bow",			0, 1, 0, 12, 60, 0, WOOD, P_BOW, HI_WOOD),
PROJECTILE("orcish arrow", "crude arrow",
	0, 15, 0, 2, 6, 6, 0, IRON, -P_BOW, CLR_BLACK),
PROJECTILE("arrow", (char *)0,
	1, 48, 0, 2, 7, 7, 0, IRON, -P_BOW, HI_METAL),
PROJECTILE("silver arrow", (char *)0,
	1, 15, 0, 5, 8, 8, 0, SILVER, -P_BOW, HI_SILVER),
PROJECTILE("elven arrow", "runed arrow",
	0, 25, 0, 2, 9, 9, 0, WOOD, -P_BOW, HI_METAL),
PROJECTILE("dark elven arrow", "black runed arrow",
	0,  2, 0, 2, 10, 10, 1, WOOD, -P_BOW, CLR_BLACK),
PROJECTILE("ya", "bamboo arrow",
	0, 10, 0, 4, 9, 9, 2, METAL, -P_BOW, HI_METAL),

/* Slings */
BOW("sling", (char *)0,		1, 0, 40,  2, 20, 2, WOOD, P_SLING, HI_LEATHER),

/* Firearms */
#ifdef FIREARMS
GUN("pistol", (char *)0,	   1,  0, 1,  8,  100, 15,  0,  4, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("submachine gun", (char *)0,   1,  0, 1,  8,  250, 10,  3, 3, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("heavy machine gun", (char *)0,1,  1, 1, 200, 2000, 20,  8, 0, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("rifle", (char *)0,		   1,  1, 1,  12,  150, 22, -1,  5, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("assault rifle", (char *)0,	   1,  0, 1,  16, 1000, 20,  5, 2, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("sniper rifle", (char *)0,	   1,  1, 1,  20, 4000, 25, -3,  8, WP_BULLET, IRON, P_FIREARM, HI_METAL),
GUN("shotgun", (char *)0,	   1,  0, 1,  15,  200,  3, -1,  7,  WP_SHELL, IRON, P_FIREARM, HI_METAL),
GUN("auto shotgun", (char *)0,	   1,  1, 1,  24, 1500,  3,  2,  4,  WP_SHELL, IRON, P_FIREARM, HI_METAL),
GUN("rocket launcher", (char *)0,  1,  1, 1, 250, 3500, 20, -5, 0, WP_ROCKET, IRON, P_FIREARM, HI_METAL),
GUN("grenade launcher", (char *)0, 1,  1, 1,  25, 1500,  6, -3, 1,WP_GRENADE, IRON, P_FIREARM, HI_METAL),
BULLET("bullet", (char *)0,
	1,  1,   0,   5, 24, 36, 1, WP_BULLET,   P,   IRON, -P_FIREARM, HI_METAL),
BULLET("silver bullet", (char *)0,
	1,  1,   0,  15, 24, 36, 3, WP_BULLET,   P, SILVER, -P_FIREARM, HI_SILVER),
BULLET("shotgun shell", (char *)0,
	1,  1,   0,  10, 35, 55, 2,  WP_SHELL,   P,   IRON, -P_FIREARM, CLR_RED),
BULLET("rocket", (char *)0,
	1,  1, 0, 450, 60, 90, 5, WP_ROCKET, P|E,   IRON, -P_FIREARM, CLR_BRIGHT_BLUE),
BULLET("frag grenade", (char *)0,
	1,  1,  0, 350, 0, 0, 2,  WP_GRENADE, B|E,   IRON, -P_FIREARM, CLR_GREEN),
BULLET("gas grenade", (char *)0,
	1,  1,  0, 350, 0, 0, 2,  WP_GRENADE, B|E,   IRON, -P_FIREARM, CLR_ORANGE),
BULLET("stick of dynamite", "red stick",
	0,  1,  0, 150, 0, 0, 2,  WP_GENERIC,   B,   PLASTIC, P_NONE, CLR_RED),
#endif

/* Crossbows (and bolts) */
/* Crossbow range is now independant of strength */
GUN("crossbow", (char *)0,	0, 1, 45, 20, 40, 12, -1, 1, WP_GENERIC, WOOD, P_CROSSBOW, HI_WOOD),
PROJECTILE("crossbow bolt", (char *)0,
	1, 38, 0, 2, 14, 14, 1, IRON, -P_CROSSBOW, HI_METAL),

/* Darts */
/* (also weptoool spoon) */
WEAPON("dart", (char *)0,
	1, 1, 0, 53,  0,  4,  5,  2, 0, P,   -P_DART, IRON, HI_METAL),

/* Shurikens */
WEAPON("shuriken", "throwing star",
	0, 1, 0, 25,  0,  5,  8,  6, 2, P,   -P_SHURIKEN, IRON, HI_METAL),

/* Boomerangs */
WEAPON("boomerang", (char *)0,
	1, 1, 0, 15,  0, 20,  9,  9, 3, 0,   -P_BOOMERANG, WOOD, HI_WOOD),

/* Whips */


WEAPON("bullwhip", (char *)0,
	1, 0, 0,  9, 8,  4,  5,  3, 2, 0,   P_WHIP, LEATHER, CLR_BROWN),

WEAPON("steel whip", (char *)0,
	1, 0, 0,  1, 8,  4,  10,  6, 4, 0,   P_WHIP, METAL, HI_METAL),

#ifdef KOPS
WEAPON("rubber hose", (char *)0,
	1, 0, 0,  3, 8,  3,  4,  3, 5, B,   P_WHIP, PLASTIC, CLR_BROWN),
#endif

/* With shuffled appearances... */
#undef P
#undef S
#undef B
#undef E

#undef WEAPON
#undef PROJECTILE
#undef BOW
#undef BULLET
#undef GUN

/* armor ... */
/* IRON denotes ferrous metals, including steel.
 * Only IRON weapons and armor can rust.
 * Only COPPER (including brass) corrodes.
 * Some creatures are vulnerable to SILVER.
 */
#define ARMOR(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,sub,metal,c) \
	OBJECT( \
		OBJ(name,desc), BITS(kn,0,1,0,mgc,1,0,0,blk,0,0,sub,metal), power, \
		ARMOR_CLASS, prob, delay, wt, cost, \
		0, 0, 10 - ac, can, wt, c )
#define CLOAK(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_CLOAK,metal,c)
#define HELM(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_HELM,metal,c)
#define GLOVES(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_GLOVES,metal,c)
#define SHIELD(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,blk,power,prob,delay,wt,cost,ac,can,ARM_SHIELD,metal,c)
#define BOOTS(name,desc,kn,mgc,power,prob,delay,wt,cost,ac,can,metal,c) \
	ARMOR(name,desc,kn,mgc,0,power,prob,delay,wt,cost,ac,can,ARM_BOOTS,metal,c)

#ifdef TOURIST
/* Shirts */
ARMOR("Hawaiian shirt", (char *)0,
	1, 0, 0, 0,	 5, 0,	 2,   3, 10, 0, ARM_SHIRT, CLOTH, CLR_MAGENTA),
ARMOR("T-shirt", (char *)0,
	1, 0, 0, 0,	 2, 0,	 2,   2, 10, 0, ARM_SHIRT, CLOTH, CLR_WHITE),
#endif /* TOURIST */
# ifdef CONVICT
ARMOR("striped shirt", (char *)0,
	1, 0, 0, 0,	 1, 0,	 5,   2, 10, 0, ARM_SHIRT, CLOTH, CLR_GRAY),
# endif /* CONVICT */
/*Ruffled shirts are little different from other shirts*/
ARMOR("ruffled shirt", (char *)0,
	1, 0, 0, 0,	 1, 0,	 5,   2, 10, 0, ARM_SHIRT, CLOTH, CLR_WHITE),
/* victorian underwear, on the other hand, inflicts a penalty to AC but grants MC 3 */
ARMOR("victorian underwear", (char *)0,
	1, 0, 0, 0,	 1, 5,	 5,   2, 10, 3, ARM_SHIRT, CLOTH, CLR_WHITE),
/* 5lo: Expensive suit from Nethack--, grants a charisma bonus and a MC 2 */
ARMOR("expensive suit", (char *)0,
	1, 0, 0, 0,	 2, 0,   5,  50, 10, 2, ARM_SHIRT, CLOTH, CLR_BLACK), 


/* Suits of armor */
ARMOR("plate mail", (char *)0,
	1, 0, 1, 0,	40, 5, 200, 600,  3, 2, ARM_SUIT, IRON, HI_METAL),
#ifdef JEDI
ARMOR("plasteel armor", (char *)0,
	1, 0, 1, 0,      5, 5, 50,  80,  4, 0, ARM_SUIT, PLASTIC, CLR_WHITE),
#endif
ARMOR("crystal plate mail", (char *)0,
	1, 0, 1, 0,	10, 5, 200, 820,  3, 2, ARM_SUIT, GLASS, CLR_WHITE),
#ifdef TOURIST
ARMOR("bronze plate mail", (char *)0,
	1, 0, 1, 0,	25, 5, 200, 400,  4, 0, ARM_SUIT, COPPER, HI_COPPER),
#else
ARMOR("bronze plate mail", (char *)0,
	1, 0, 1, 0,	35, 5, 200, 400,  4, 0, ARM_SUIT, COPPER, HI_COPPER),
#endif
ARMOR("splint mail", (char *)0,
	1, 0, 1, 0,	64, 5, 160,  80,  4, 1, ARM_SUIT, IRON, HI_METAL),
ARMOR("banded mail", (char *)0,
	1, 0, 1, 0,	72, 5, 160,  90,  4, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR("dwarvish mithril-coat", (char *)0,
	1, 0, 0, 0,	10, 1, 60, 240,  5, 3, ARM_SUIT, MITHRIL, HI_METAL),
ARMOR("dark elven mithril-coat", (char *)0,
	1, 0, 0, 0,  2, 1, 60, 240,  3, 3, ARM_SUIT, MITHRIL, CLR_BLACK),
ARMOR("elven mithril-coat", (char *)0,
	1, 0, 0, 0,	15, 1, 60, 240,  4, 3, ARM_SUIT, MITHRIL, HI_METAL),
ARMOR("gnomish suit", "little blue vest",
	0, 0, 0, 0,	1, 0, 20,  10,  9, 0, ARM_SUIT, CLOTH, CLR_BRIGHT_BLUE),
ARMOR("chain mail", (char *)0,
	1, 0, 0, 0,	70, 5, 120,  75,  5, 1, ARM_SUIT, IRON, HI_METAL),
ARMOR("orcish chain mail", "crude chain mail",
	0, 0, 0, 0,	20, 5, 120,  75,  6, 1, ARM_SUIT, IRON, CLR_BLACK),
ARMOR("scale mail", (char *)0,
	1, 0, 0, 0,	55, 5, 100,  45,  6, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR("studded leather armor", (char *)0,
	1, 0, 0, 0,	68, 3, 80,  15,  7, 1, ARM_SUIT, LEATHER, HI_LEATHER),
ARMOR("ring mail", (char *)0,
	1, 0, 0, 0,	60, 5, 100, 100,  7, 0, ARM_SUIT, IRON, HI_METAL),
ARMOR("orcish ring mail", "crude ring mail",
	0, 0, 0, 0,	20, 5, 100,  80,  8, 1, ARM_SUIT, IRON, CLR_BLACK),
ARMOR("leather armor", (char *)0,
	1, 0, 0, 0,	75, 3, 60,   5,  8, 0, ARM_SUIT, LEATHER, HI_LEATHER),
ARMOR("leather jacket", (char *)0,
	1, 0, 0, 0,	11, 0,	12,  10,  9, 0, ARM_SUIT, LEATHER, CLR_BLACK),

/* Robes */
/* STEPHEN WHITE'S NEW CODE */
ARMOR("robe", "red robe",
	0, 0, 0, 0,		1, 1,  16,  50,  9, 0, ARM_SUIT, CLOTH, CLR_RED),
ARMOR("robe of protection", "blue robe",
	0, 1, 0, PROTECTION,	1, 1,  16,  50,  5, 0, ARM_SUIT, CLOTH, CLR_BRIGHT_BLUE),
ARMOR("robe of power", "orange robe",
	0, 1, 0, 0,		1, 1,  16,  50,  9, 0, ARM_SUIT, CLOTH, CLR_ORANGE),
ARMOR("robe of weakness", "green robe",
	0, 1, 0, STUNNED,		1, 1,  16,  50,  9, 0, ARM_SUIT, CLOTH, CLR_GREEN),

/*
 * Dragon suits
 * There is code in polyself.c that assumes (1) and (2).
 * There is code in obj.h, objnam.c, mon.c, read.c that assumes (2).
 *
 *	(1) The dragon scale mails and the dragon scales are together.
 *	(2) That the order of the dragon scale mail and dragon scales is the
 *	    the same defined in monst.c.
 */
#define DRGN_ARMR(name,mgc,power,cost,ac,color) \
	ARMOR(name,(char *)0,1,mgc,1,power,1,5,20,cost,ac,0,ARM_SUIT,DRAGON_HIDE,color)
/* 3.4.1: dragon scale mail reclassified as "magic" since magic is
   needed to create them */
DRGN_ARMR("gray dragon scale mail",   1, ANTIMAGIC,  1500, 1, CLR_GRAY),
DRGN_ARMR("silver dragon scale mail", 1, REFLECTING, 1500, 1, SILVER),
DRGN_ARMR("shimmering dragon scale mail", 1, DISPLACED, 1200, 1, CLR_CYAN),
DRGN_ARMR("deep dragon scale mail",   1, DRAIN_RES,  1200, 1, CLR_MAGENTA),
DRGN_ARMR("red dragon scale mail",    1, FIRE_RES,    900, 1, CLR_RED),
DRGN_ARMR("white dragon scale mail",  1, COLD_RES,    900, 1, CLR_WHITE),
DRGN_ARMR("orange dragon scale mail", 1, SLEEP_RES,   900, 1, CLR_ORANGE),
DRGN_ARMR("black dragon scale mail",  1, DISINT_RES, 1200, 1, CLR_BLACK),
DRGN_ARMR("blue dragon scale mail",   1, SHOCK_RES,   900, 1, CLR_BRIGHT_BLUE),
DRGN_ARMR("green dragon scale mail",  1, POISON_RES,  900, 1, CLR_GREEN),
DRGN_ARMR("golden dragon scale mail",  1, SICK_RES,  900, 1, CLR_YELLOW),
DRGN_ARMR("stone dragon scale mail",  1, STONE_RES,  900, 1, CLR_GRAY),
DRGN_ARMR("yellow dragon scale mail", 1, ACID_RES,    900, 1, CLR_YELLOW),

/* For now, only dragons leave these. */
/* 3.4.1: dragon scales left classified as "non-magic"; they confer
   magical properties but are produced "naturally" */
DRGN_ARMR("gray dragon scales",   0, ANTIMAGIC,  800, 5, CLR_GRAY),
DRGN_ARMR("silver dragon scales", 0, REFLECTING, 800, 5, SILVER),
DRGN_ARMR("shimmering dragon scales", 0, DISPLACED,  700, 5, CLR_CYAN),
DRGN_ARMR("deep dragon scales",   0, DRAIN_RES,  500, 5, CLR_MAGENTA),
DRGN_ARMR("red dragon scales",    0, FIRE_RES,   500, 5, CLR_RED),
DRGN_ARMR("white dragon scales",  0, COLD_RES,   500, 5, CLR_WHITE),
DRGN_ARMR("orange dragon scales", 0, SLEEP_RES,  500, 5, CLR_ORANGE),
DRGN_ARMR("black dragon scales",  0, DISINT_RES, 700, 5, CLR_BLACK),
DRGN_ARMR("blue dragon scales",   0, SHOCK_RES,  500, 5, CLR_BRIGHT_BLUE),
DRGN_ARMR("green dragon scales",  0, POISON_RES, 500, 5, CLR_GREEN),
DRGN_ARMR("golden dragon scales",   0, SICK_RES,  500, 5, CLR_YELLOW),
DRGN_ARMR("stone dragon scales",  0, STONE_RES, 500, 5, CLR_GRAY),
DRGN_ARMR("yellow dragon scales", 0, ACID_RES,   500, 5, CLR_YELLOW),
#undef DRGN_ARMR

/* Cloaks */
/*  'cope' is not a spelling mistake... leave it be */
CLOAK("mummy wrapping", (char *)0,
		1, 0,	0,	    1, 0,  2,  2, 10, 1, CLOTH, CLR_GRAY),
CLOAK("orcish cloak", "coarse mantelet",
		0, 0,	0,	    6, 0, 4, 40, 10, 2, CLOTH, CLR_BLACK),
CLOAK("dwarvish cloak", "hooded cloak",
		0, 0,	0,	    7, 0, 4, 50, 10, 2, CLOTH, HI_CLOTH),
CLOAK("oilskin cloak", "slippery cloak",
		0, 0,	0,	    7, 0, 4, 50,  9, 3, CLOTH, HI_CLOTH),
CLOAK("elven cloak", "faded pall",
		0, 1,	STEALTH,    6, 0, 4, 60,  9, 3, CLOTH, CLR_BLACK),
CLOAK("lab coat", "white coat",
		0, 1,   POISON_RES,10, 0, 4, 60,  9, 3, CLOTH, CLR_WHITE),
CLOAK("leather cloak", (char *)0,
		1, 0,	0,	    7, 0, 7, 40,  9, 1, LEATHER, CLR_BROWN),
CLOAK("plasteel cloak", (char *)0,
		1, 0,	0,	    1, 0, 7, 70,  8, 2, PLASTIC, CLR_WHITE),
CLOAK("cloak of warmth", "metal cloak",
		0, 1,	COLD_RES,  1, 0, 4, 50,  7, 3, METAL, CLR_GRAY),
CLOAK("cloak of grounding", "granite cloak",
		0, 1,	SHOCK_RES,  1, 0, 4, 50,  8, 2, MINERAL, CLR_GRAY),
CLOAK("cloak of quenching", "asbestos cloak",
		0, 1,	FIRE_RES,  1, 0, 4, 50,  9, 3, MITHRIL, CLR_ORANGE),

#if 0
CLOAK("robe", (char *)0,
		1, 1,	0,	    3, 0, 7, 50,  8, 3, CLOTH, CLR_RED),
CLOAK("alchemy smock", "apron",
		0, 1,	POISON_RES, 9, 0, 4, 50,  9, 1, CLOTH, CLR_WHITE),
#endif
/* With shuffled appearances... */
CLOAK("cloak of protection", "tattered cape",
		0, 1,	PROTECTION, 9, 0, 4, 50,  7, 3, CLOTH, HI_CLOTH),
CLOAK("poisonous cloak", "dirty rag",
		0, 1,   0,          2, 0, 4, 50, 10, 3, CLOTH, CLR_GREEN),
CLOAK("cloak of invisibility", "opera cloak",
		0, 1,	INVIS,	   9, 0, 4, 50,  9, 2, CLOTH, CLR_BRIGHT_MAGENTA),
CLOAK("cloak of magic resistance", "ornamental cope",
		0, 1,	ANTIMAGIC,  4, 0, 4, 50,  9, 3, CLOTH, CLR_WHITE),
CLOAK("cloak of reflection", "funeral cloak",
		0, 1,	REFLECTING,  2, 0, 4, 50,  9, 3, CLOTH, CLR_BRIGHT_GREEN),
CLOAK("manacloak", "dragonhide cloak",
		0, 1,	ENERGY_REGENERATION,  1, 0, 4, 50,  7, 3, DRAGON_HIDE, CLR_BRIGHT_BLUE),
CLOAK("cloak of displacement", "piece of cloth",
		0, 1,	DISPLACED, 7, 0, 4, 50,  9, 2, CLOTH, HI_CLOTH),
/* Helmets */
HELM("elven leather helm", "leather hat",
		0, 0, 0,			5, 1,  2,   8, 8, 0, LEATHER, HI_LEATHER),
HELM("gnomish helm", "little red hat",
		0, 0,  0,			1, 1,  1,   8, 10, 0, CLOTH, CLR_RED),
HELM("orcish helm", "iron skull cap",
		0, 0, 0,			4, 1, 12,  10, 9, 0, IRON, CLR_BLACK),
HELM("dwarvish iron helm", "hard hat",
		0, 0, 0,			6, 1, 16,  20, 8, 0, IRON, HI_METAL),
HELM("fedora", (char *)0,
		1, 0, 0,			1, 0,  2,   1,10, 0, CLOTH, CLR_BROWN),
HELM("cornuthaum", "conical hat",
		0, 1, CLAIRVOYANT,	3, 1,  2,  80,10, 2, CLOTH, CLR_BRIGHT_BLUE),
HELM("dunce cap", "conical hat",
		0, 1, 0,			3, 1,  2,  80,10, 0, CLOTH, CLR_BRIGHT_BLUE),
HELM("dented pot", (char *)0,
		1, 0, 0,			2, 0, 4,   8, 9, 0, IRON, CLR_BLACK),
/* 5lo: Fire Helmet from SLASH 6, grants Fire Resistance */
HELM("fire helmet", "red shiny helmet",
		0, 0, FIRE_RES,			2, 1, 25, 50, 9, 0, IRON, CLR_RED),
#ifdef JEDI
HELM("plasteel helm", (char *)0,
		1, 0, 0,                        1, 1,  2,  20, 7, 9, PLASTIC, CLR_WHITE),
#endif
/* ...with shuffled appearances */
HELM("helmet", "plumed helmet",
		0, 0, 0,		   6, 1, 12,  50, 9, 0, IRON, HI_METAL),
HELM("helm of brilliance", "etched helmet",
		0, 1, 0,			6, 1, 20,  50, 9, 0, IRON, CLR_GREEN),
HELM("helm of opposite alignment", "crested helmet",
		0, 1, 0,			5, 1, 20,  50, 9, 0, IRON, HI_METAL),
HELM("helm of telepathy", "visored helmet",
		0, 1, TELEPAT,		2, 1, 20,  50, 9, 0, IRON, HI_METAL),

/* Gloves */
/* these have their color but not material shuffled, so the IRON and PLASTIC
 * must stay CLR_BROWN (== HI_LEATHER)
 */
#ifdef JEDI
GLOVES("plasteel gloves", "white gloves",
		0, 0,  0,                        1, 1,  2, 25,  8, 0, PLASTIC, CLR_WHITE),
#endif
GLOVES("leather gloves", "old gloves",
		0, 0,  0,			14, 1, 4,  50,  9, 0, LEATHER, HI_LEATHER),
GLOVES("rubber gloves", "gardening gloves",
	0, 0,  0,	1, 1, 10, 50,	9, 0, PLASTIC, CLR_BROWN),
GLOVES("gauntlets of fumbling", "padded gloves",
		0, 1,  FUMBLING,	8, 1, 4, 50,  9, 0, LEATHER, HI_LEATHER),
GLOVES("gauntlets of power", "riding gloves",
		0, 1,  0,			7, 1, 4, 50,  9, 0, IRON, CLR_BROWN),
GLOVES("gauntlets of typing", "brand-new gloves",
		0, 1, 0, 			1, 1, 4,  50,  8, 0, LEATHER, HI_LEATHER),
GLOVES("gauntlets of swimming", "black gloves",
		0, 1,  SWIMMING,	8, 1, 4, 50,  9, 0, LEATHER, HI_LEATHER),
GLOVES("gauntlets of reflection", "archery gloves",
		0, 1, REFLECTING,	2, 1, 10, 50, 9, 0, SILVER, CLR_BROWN),
GLOVES("gauntlets of dexterity", "fencing gloves",
		0, 1,  0,			8, 1, 4, 50,  9, 0, LEATHER, HI_LEATHER),

/* Shields */
SHIELD("small shield", (char *)0,
		1, 0, 0, 0,	     6, 0, 12,	3,  8, 0, WOOD, HI_WOOD),
/* Elven ... orcish shields can't be differentiated by feel */
SHIELD("elven shield", "blue and green shield",
		0, 0, 0, 0,	     2, 0, 20,	7,  7, 0, WOOD, CLR_GREEN),
SHIELD("Uruk-hai shield", "white-handed shield",
		0, 0, 0, 0,	     2, 0, 20,	7,  6, 0, IRON, HI_METAL),
SHIELD("orcish shield", "red-eyed shield",
		0, 0, 0, 0,	     2, 0, 20,	7,  6, 0, IRON, CLR_RED),
SHIELD("large shield", (char *)0,
		1, 0, 1, 0,	     6, 0,40, 10,  5, 0, IRON, HI_METAL),
SHIELD("dwarvish roundshield", "large round shield",
		0, 0, 0, 0,	     4, 0,40, 10,  5, 0, IRON, HI_METAL),
SHIELD("shield of reflection", "polished silver shield",
		0, 1, 0, REFLECTING, 3, 0, 20, 50,  4, 0, SILVER, HI_SILVER),

/* Boots */
#ifdef JEDI
BOOTS("plasteel boots", "white boots", /* these things were crashing the game for binders and certain other chars! */
		0, 0,  0,          1, 2, 2,  25,  8, 9, PLASTIC, CLR_WHITE),
#endif
BOOTS("low boots", "walking shoes",
		0, 0,  0,	  17, 2, 4,  8,  9, 0, LEATHER, HI_LEATHER),
BOOTS("iron shoes", "hard shoes",
		0, 0,  0,	   7, 2, 20, 16,  8, 0, IRON, HI_METAL),
BOOTS("gnomish boots", "little black boots",
		0, 0,  0,	   1, 2, 4, 16,  10, 0, LEATHER, CLR_BLACK),
BOOTS("high boots", "jackboots",
		0, 0,  0,	  13, 2, 8, 12,  8, 0, LEATHER, HI_LEATHER),
BOOTS("wedge sandals", "heeled sandals",
		0, 0,  0,          1, 5, 2,  25,  10, 0, MINERAL, CLR_WHITE),
BOOTS("dancing shoes", "soft footwear", /* not high-heeled - transvestites won't benefit from wearing this pair! --Amy */
		0, 0,  0,          1, 1, 1,  80,   8, 0, LEATHER, CLR_BRIGHT_BLUE),
BOOTS("sweet mocassins", "womens footwear", /* not high-heeled*/
		0, 0,  0,          1, 1, 1,  50,   9, 0, LEATHER, CLR_ORANGE),
BOOTS("soft sneakers", "light footwear", /* not high-heeled*/
		0, 0,  0,          1, 2, 2,  150,   8, 0, LEATHER, CLR_WHITE),
BOOTS("feminine pumps", "female footwear",
		0, 0,  0,          1, 2, 4,  100,  10, 0, LEATHER, CLR_ORANGE),
BOOTS("leather peep-toes", "asian footwear",
		0, 0,  0,          1, 3, 4,  200,   9, 0, LEATHER, CLR_BLACK),
BOOTS("hippie heels", "red overknees",
		0, 0,  HALLUC,          1, 3, 5,  200,  5, 0, DRAGON_HIDE, CLR_ORANGE),
BOOTS("combat stilettos", "heeled boots",
		0, 0,  0,          1, 5, 10,  250,  9, 0, DRAGON_HIDE, CLR_BLACK),
/* ...with shuffled appearances */
BOOTS("speed boots", "combat boots",
		0, 1,  FAST,	  12, 2, 8, 50,  9, 0, LEATHER, HI_LEATHER),
/* With shuffled appearances... */
BOOTS("water walking boots", "jungle boots",
		0, 1,  WWALKING,  12, 2, 8, 50,  9, 0, LEATHER, HI_LEATHER),
BOOTS("jumping boots", "hiking boots",
		0, 1,  JUMPING,   11, 2, 8, 50,  9, 0, LEATHER, HI_LEATHER),
BOOTS("flying boots", "battle boots",
		0, 1,  FLYING,   1, 2, 8, 50,  9, 0, SILVER, CLR_GRAY),
BOOTS("elven boots", "mud boots",
		0, 1,  STEALTH,   12, 2, 7,  50,  9, 0, LEATHER, HI_LEATHER),
BOOTS("kicking boots", "steel boots",
		0, 1,  0,         12, 2, 7,  50,  9, 0, IRON, CLR_BROWN),
BOOTS("fumble boots", "riding boots",
		0, 1,  FUMBLING,  11, 2, 8, 50,  9, 0, LEATHER, HI_LEATHER),
BOOTS("zipper boots", "persian boots",
		0, 1,  WOUNDED_LEGS,  1, 2, 8, 50,  6, 3, DRAGON_HIDE, CLR_YELLOW),
BOOTS("levitation boots", "snow boots",
		0, 1,  LEVITATION,12, 2, 7, 50,  9, 0, LEATHER, HI_LEATHER),

#undef HELM
#undef CLOAK
#undef SHIELD
#undef GLOVES
#undef BOOTS
#undef ARMOR

/* rings ... */
/* [Tom] looks like there are no probs to change... */
#define RING(name,power,stone,cost,prob,mgc,spec,mohs,metal,color) OBJECT( \
		OBJ(name,stone), \
		BITS(0,0,spec,0,mgc,spec,0,0,0,HARDGEM(mohs),0,0,metal), \
		power, RING_CLASS, prob, 0, 0, cost, 0, 0, 0, 0, 15, color )
RING("adornment", ADORNED, "wooden",        300, 23, 1, 1, 2, WOOD, HI_WOOD),
RING("hunger",     HUNGER,     "topaz",     300, 18, 1, 0, 8, GEMSTONE, CLR_CYAN),
RING("disarming",     GLIB,     "wet",     300,  5, 1, 0, 1, VEGGY, CLR_BRIGHT_BLUE),
RING("mood",       0,          "ridged",    300, 28, 1, 0, 8, IRON, HI_METAL),
RING("protection", PROTECTION, "black onyx",300, 33, 1, 1, 7, MINERAL, CLR_BLACK),
RING("protection from shape changers", PROT_FROM_SHAPE_CHANGERS, "shiny",
					    					300, 33, 1, 0, 5, IRON, CLR_BRIGHT_CYAN),
RING("sleeping",   SLEEPING,   "wedding",   300, 23, 1, 0, 7, GEMSTONE, CLR_WHITE),
RING("stealth", STEALTH, "jade",            300, 33, 1, 0, 6, GEMSTONE, CLR_GREEN),
RING("sustain ability", FIXED_ABIL, "bronze",
					    300, 33, 1, 0, 4, COPPER, HI_COPPER),
RING("warning",    WARNING,    "diamond",   300, 33, 1, 0,10, GEMSTONE, CLR_WHITE),
RING("aggravate monster", AGGRAVATE_MONSTER, "sapphire",
					    300, 23, 1, 0, 9, GEMSTONE, CLR_BRIGHT_BLUE),
RING("cold resistance", COLD_RES, "brass",  300, 23, 1, 0, 4, COPPER, HI_COPPER),
RING("gain constitution", 0,   "opal",      300, 33, 1, 1, 7, MINERAL,  HI_MINERAL),
RING("gain dexterity",  0,     "obsidian",  300, 33, 1, 1, 7, GEMSTONE, CLR_BLACK),
RING("gain intelligence", 0,   "plain",     300, 33, 1, 1, 7, MINERAL,  HI_MINERAL),
RING("gain strength", 0,       "granite",   300, 33, 1, 1, 7, MINERAL, HI_MINERAL),
RING("gain wisdom",     0,     "glass",     300, 33, 1, 1, 7, MINERAL,  CLR_CYAN),
RING("timely backup",	  0,	"rusty",	  300, 5,  1, 1, 4, IRON,     CLR_BROWN),
RING("increase accuracy", 0,   "clay",      300, 33, 1, 1, 4, MINERAL, CLR_RED),
RING("increase damage", 0,     "coral",     300, 33, 1, 1, 4, MINERAL, CLR_ORANGE),
RING("slow digestion",  SLOW_DIGESTION, "steel",
					    300, 33, 1, 0, 8, IRON, HI_METAL),
RING("invisibility", INVIS,    "wire",      300, 33, 1, 0, 5, IRON, HI_METAL),
RING("poison resistance", POISON_RES, "pearl",
					    300, 23, 1, 0, 4, IRON, CLR_WHITE),
RING("see invisible", SEE_INVIS, "engagement",
											300, 33, 1, 0, 5, IRON, HI_METAL),
RING("shock resistance", SHOCK_RES, "copper",
					    300, 23, 1, 0, 3, COPPER, HI_COPPER),
RING("sickness resistance", SICK_RES, "fragile",
					    300, 10, 1, 0, 3, GLASS, HI_COPPER),
RING("fire resistance", FIRE_RES, "iron",   300, 23, 1, 0, 5, IRON, HI_METAL),
RING("free action",     FREE_ACTION, "twisted",
					    300, 33, 1, 0, 6, IRON, HI_METAL),
/*RING("infravision", 0, "zinc",              300, 33, 1, 0, 5, MITHRIL, HI_METAL),*/
RING("levitation", LEVITATION, "agate",     300, 33, 1, 0, 7, GEMSTONE, CLR_RED),
RING("regeneration", REGENERATION, "moonstone",
											300, 33, 1, 0, 6, MINERAL, HI_MINERAL),
RING("searching",  SEARCHING,  "tiger eye", 300, 23, 1, 0, 6, GEMSTONE, CLR_BROWN),
RING("teleportation", TELEPORT, "silver",   300, 33, 1, 0, 3, SILVER, HI_SILVER),
RING("conflict",   CONFLICT, "ruby",        300, 23, 1, 0, 9, GEMSTONE, CLR_RED),
RING("polymorph", POLYMORPH, "ivory",       300, 33, 1, 0, 4, BONE, CLR_WHITE),
RING("polymorph control", POLYMORPH_CONTROL, "emerald",
					    300, 33, 1, 0, 8, GEMSTONE, CLR_BRIGHT_GREEN),
RING("teleport control", TELEPORT_CONTROL, "gold",
											300, 34, 1, 0, 3, GOLD, HI_GOLD),
RING((char *)0, 0,  "quartz", 0, 0, 0, 0, 7, GLASS, HI_GLASS),
RING((char *)0, 0,  "porcelain", 0, 0, 0, 0, 8, GLASS, HI_MINERAL),
RING((char *)0, 0,  "ceramic", 0, 0, 0, 0, 8, GLASS, HI_MINERAL),
RING((char *)0, 0,  "rigged", 0, 0, 0, 0, 8, IRON, HI_METAL),
RING((char *)0, 0,  "high", 0, 0, 0, 0, 7, GEMSTONE, CLR_WHITE),
RING((char *)0, 0,  "mithril", 0, 0, 0, 0, 5, SILVER, HI_SILVER),
RING((char *)0, 0,  "platinum", 0, 0, 0, 0, 3, PLATINUM, HI_METAL),
RING((char *)0, 0,  "jacinth", 0, 0, 0, 0, 9, GEMSTONE, CLR_ORANGE),
RING((char *)0, 0,  "citrine", 0, 0, 0, 0, 6, GEMSTONE, CLR_ORANGE),
RING((char *)0, 0,  "amber", 0, 0, 0, 0, 2, GEMSTONE, CLR_BROWN),
RING((char *)0, 0,  "jet", 0, 0, 0, 0, 7, GEMSTONE, CLR_BLACK),
RING((char *)0, 0,  "chrysoberyl", 0, 0, 0, 0, 5, GEMSTONE, CLR_YELLOW),
RING((char *)0, 0,  "plastic", 0, 0, 0, 0, 1, PLASTIC, CLR_BRIGHT_MAGENTA),
RING((char *)0, 0,  "cameo", 0, 0, 0, 0, 3, PLASTIC, CLR_BRIGHT_MAGENTA),
RING((char *)0, 0,  "intaglio", 0, 0, 0, 0, 3, PLASTIC, CLR_BRIGHT_MAGENTA),
RING((char *)0, 0,  "dazzling", 0, 0, 0, 0, 3, PLASTIC, CLR_BRIGHT_GREEN),
RING((char *)0, 0,  "deep", 0, 0, 0, 0, 3, PLASTIC, CLR_BRIGHT_BLUE),
RING((char *)0, 0,  "lighting", 0, 0, 0, 0, 3, PLASTIC, CLR_YELLOW),
RING((char *)0, 0,  "bright", 0, 0, 0, 0, 3, PLASTIC, CLR_ORANGE),
RING((char *)0, 0,  "pretty", 0, 0, 0, 0, 3, PLASTIC, CLR_BRIGHT_CYAN),
RING((char *)0, 0,  "amethyst", 0, 0, 0, 0, 3, GEMSTONE, CLR_BRIGHT_MAGENTA),
RING((char *)0, 0,  "alexandrite", 0, 0, 0, 0, 4, GEMSTONE, CLR_BROWN),
RING((char *)0, 0,  "carnelian", 0, 0, 0, 0, 3, GEMSTONE, CLR_BROWN),
RING((char *)0, 0,  "germanium", 0, 0, 0, 0, 2, MINERAL, CLR_WHITE),
RING((char *)0, 0,  "garnet", 0, 0, 0, 0, 5, GEMSTONE, CLR_RED),
RING((char *)0, 0,  "kryptonite", 0, 0, 0, 0, 4, MINERAL, CLR_BLACK),
RING((char *)0, 0,  "lapis lazuli", 0, 0, 0, 0, 7, GEMSTONE, CLR_BRIGHT_BLUE),
RING((char *)0, 0,  "peridot", 0, 0, 0, 0, 7, GEMSTONE, CLR_BRIGHT_GREEN),
RING((char *)0, 0,  "stibotantalite", 0, 0, 0, 0, 6, MINERAL, CLR_GRAY),
RING((char *)0, 0,  "taaffeite", 0, 0, 0, 0, 4, MINERAL, CLR_ORANGE),
RING((char *)0, 0,  "zircon", 0, 0, 0, 0, 5, GEMSTONE, CLR_BRIGHT_CYAN),
/* More descriptions: cameo, intaglio */
#undef RING

/* amulets ... - THE Amulet comes last because it is special */
#define AMULET(name,desc,power,prob,metal,color) OBJECT( \
		OBJ(name,desc), BITS(0,0,0,0,1,0,0,0,0,0,0,0,metal), power, \
		AMULET_CLASS, prob, 0, 0, 150, 0, 0, 0, 0, 20, color )
AMULET("amulet of change",        "square",     0,          110,	IRON,	CLR_BLACK),
AMULET("amulet of drain resistance","warped",   DRAIN_RES,   60, 	IRON,	CLR_GRAY),
AMULET("amulet of ESP",           "circular",   TELEPAT,    110, 	IRON,	CLR_WHITE),
AMULET("amulet of undead warning", "occult",   WARN_UNDEAD,   5, 	MITHRIL,	CLR_BLACK),
AMULET("amulet of own race warning", "cabbalist",   0,   5, PLASTIC,	CLR_CYAN),
AMULET("amulet of poison warning", "splatted",   0,   5, LIQUID,	CLR_BRIGHT_GREEN),
AMULET("amulet of covetous warning", "egyptian",   0,   5, GOLD,	CLR_MAGENTA),
AMULET("amulet of flying",        "convex",     FLYING,      50, MINERAL,	CLR_RED),
AMULET("amulet of life saving",   "spherical",  LIFESAVED,   60, GLASS,	CLR_BRIGHT_BLUE),
AMULET("amulet of magical breathing", "octagonal", MAGICAL_BREATHING, 50, GEMSTONE,	CLR_GREEN),
AMULET("amulet of reflection",    "hexagonal",  REFLECTING,  60, GEMSTONE,	CLR_BRIGHT_GREEN),
AMULET("amulet of restful sleep", "triangular", SLEEPING,   100, GEMSTONE,	CLR_MAGENTA),
AMULET("amulet of blindness", "radiant", BLINDED,   10, DRAGON_HIDE,	CLR_WHITE),
AMULET("amulet of strangulation", "oval",       STRANGLED,  90, PLASTIC,	CLR_BRIGHT_MAGENTA),
AMULET("amulet of unchanging",    "concave",    UNCHANGING,	 50, MINERAL,	CLR_CYAN),
AMULET("amulet versus poison",    "pyramidal",  POISON_RES, 105, GEMSTONE,	CLR_BRIGHT_CYAN),
AMULET("amulet versus stone",     "lunate",   /*STONE_RES*/0,50, COPPER,	CLR_BRIGHT_MAGENTA),
AMULET("amulet of depetrify",     "charmed",   STONE_RES,10, BONE,	CLR_RED),
AMULET("amulet of magic resistance",     "rare",   ANTIMAGIC,1, PAPER,	CLR_ORANGE),
AMULET("amulet of sickness resistance",     "sparkling",   SICK_RES,9, GLASS,	CLR_BRIGHT_GREEN),
AMULET("amulet of swimming",     "watery",   SWIMMING,10, LIQUID,	CLR_BRIGHT_BLUE),
AMULET("amulet of disintegration resistance",     "hardrock",   DISINT_RES,5, MINERAL,	CLR_BLACK),
AMULET("amulet of acid resistance",     "venomous",   ACID_RES,5, LIQUID,	CLR_GREEN),
AMULET("amulet of regeneration",     "vivid",   REGENERATION,5, CLOTH,	CLR_BRIGHT_CYAN),
AMULET("amulet of conflict",     "sounding",   CONFLICT,5, METAL,	CLR_WHITE),
AMULET("amulet of fumbling",     "clumsy",   FUMBLING,5, LEATHER,	CLR_ORANGE),
AMULET("amulet of second chance", "tetraedical",SECOND_CHANCE, 5, GEMSTONE,	CLR_BRIGHT_BLUE),
AMULET("amulet of data storage",  "icosaedrical", 0,		5, GEMSTONE,	CLR_BRIGHT_GREEN),
AMULET("amulet of waterwalking",     "rubber",   WWALKING,5, PLASTIC,	CLR_BRIGHT_BLUE),
AMULET("amulet of hunger",     "gleaming",   HUNGER,5, WAX,	CLR_BRIGHT_MAGENTA),
AMULET((char *)0,                     "rotated",     0, 0, METAL,	CLR_ORANGE),
AMULET((char *)0,                     "rounded",     0, 0, METAL,	CLR_YELLOW),
AMULET((char *)0,                     "moony",     0, 0, COPPER,	CLR_MAGENTA),
AMULET((char *)0,                     "spiked",     0, 0, SILVER,	CLR_BROWN),
AMULET((char *)0,                     "rectangular",0, 0, METAL,	CLR_GREEN),
AMULET((char *)0,                     "elliptic",   0, 0, IRON,	CLR_BRIGHT_BLUE),
AMULET((char *)0,                     "rhomboedric",   0, 0, METAL,	CLR_ORANGE),
AMULET((char *)0,                     "radiating",     0, 0, DRAGON_HIDE,	CLR_ORANGE),
AMULET((char *)0,                     "powerful",     0, 0, GOLD,	CLR_BRIGHT_CYAN),
AMULET((char *)0,                     "burdening",     0, 0, BONE,	CLR_RED),
AMULET((char *)0,                     "worthless",     0, 0, WOOD,	CLR_BROWN),
AMULET((char *)0,                     "dark",     0, 0, SILVER,	CLR_MAGENTA),
AMULET((char *)0,                     "ghostly",     0, 0, PLATINUM,	CLR_GRAY),
AMULET((char *)0,                     "black",     0, 0, PLATINUM,	CLR_BLACK),
AMULET((char *)0,                     "bright",     0, 0, BONE,	CLR_WHITE),
AMULET((char *)0,                     "tetragonal",     0, 0, GEMSTONE,	CLR_BRIGHT_GREEN),
AMULET((char *)0,                     "edged",   0, 0, GLASS,	CLR_BRIGHT_BLUE),
AMULET((char *)0,                     "glaring",     0, 0, GLASS,	CLR_YELLOW),
AMULET((char *)0,                     "illusionary",0, 0, MINERAL,	CLR_GREEN),
AMULET((char *)0,                     "standard",     0, 0, MINERAL,	CLR_CYAN),
AMULET((char *)0,                     "hidden",     0, 0, GOLD,	CLR_CYAN),
AMULET((char *)0,                     "remote",     0, 0, PLATINUM,	CLR_GRAY),
AMULET((char *)0,                     "secluded",     0, 0, MITHRIL,	CLR_BLACK),
AMULET((char *)0,                     "Yendorian",     0, 0, GOLD,	CLR_WHITE),
OBJECT(OBJ("cheap plastic imitation of the Amulet of Yendor",
	"Amulet of Yendor"), BITS(0,0,1,0,0,0,0,0,0,0,0,0,PLASTIC), 0,
	AMULET_CLASS, 0, 0, 0,    0, 0, 0, 0, 0,  1, HI_METAL), /* must give exactly 1 point of nutrition, see makedefs.c */
OBJECT(OBJ("Amulet of Yendor",	/* note: description == name */
	"Amulet of Yendor"), BITS(0,0,1,0,1,0,1,1,0,0,0,0,MITHRIL), 0,
	AMULET_CLASS, 0, 0, 0, 30000, 0, 0, 0, 0, 20, HI_METAL),
#undef AMULET
/* since there are plastic amulets now, the makedefs routine no longer recognizes the fake amulet just because it's
 * made of plastic, so we need another distinction; I chose the nutritional value, which is 20 for every other amulet
 * but 1 for the fake. --Amy */

/* tools ... */
/* tools with weapon characteristics come last */
#define TOOL(name,desc,kn,mrg,mgc,chg,prob,wt,cost,mat,color) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,mrg,chg,0,mgc,chg,0,0,0,0,0,P_NONE,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, 0, 0, 0, 0, wt, color )
#define CONTAINER(name,desc,kn,mgc,chg,bi,prob,wt,cost,mat,color) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,0,chg,1,mgc,chg,0,0,bi,0,0,P_NONE,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, 0, 0, 0, 0, wt, color )
#define WEPTOOL(name,desc,kn,mgc,chg,bi,prob,wt,cost,sdam,ldam,hitbon,typ,sub,mat,clr) \
	OBJECT( OBJ(name,desc), \
		BITS(kn,0,1,chg,mgc,1,0,0,bi,0,typ,sub,mat), \
		0, TOOL_CLASS, prob, 0, \
		wt, cost, sdam, ldam, hitbon, 0, wt, clr )
/* Containers */
CONTAINER("large box", (char *)0,       1, 0, 0, 1,  35,30,   8, WOOD, HI_WOOD),
CONTAINER("chest", (char *)0,           1, 0, 0, 1,  35,60,  16, WOOD, HI_WOOD),
CONTAINER("ice box", (char *)0,         1, 0, 0, 1,  10,90,  42, PLASTIC, CLR_WHITE),
CONTAINER("sack", "bag",                0, 0, 0, 0,  30, 5, 100, CLOTH, HI_CLOTH),
CONTAINER("oilskin sack", "bag",        0, 0, 0, 0,  20, 5, 100, CLOTH, HI_CLOTH),
CONTAINER("ugly backpack", (char *)0,   0, 0, 0, 0,   0, 5, 100, CLOTH, CLR_MAGENTA),
CONTAINER("bag of holding", "bag",      0, 1, 0, 0,  20, 5, 100, CLOTH, HI_CLOTH),
CONTAINER("bag of digestion","bag",     0, 1, 0, 0,   5, 5, 100, CLOTH, HI_CLOTH),
CONTAINER("bag of tricks", "bag",       0, 1, 1, 0,  20, 5, 100, CLOTH, HI_CLOTH),

/* Unlocking tools */
TOOL("skeleton key", "key",     0, 0, 0, 0,  80,  0,  10, IRON, HI_METAL),
#ifdef TOURIST
TOOL("lock pick", (char *)0,    1, 0, 0, 0,  60,  0,  20, IRON, HI_METAL),
TOOL("credit card", (char *)0,  1, 0, 0, 0,  15,  0,  10, PLASTIC, CLR_WHITE),
#else
TOOL("lock pick", (char *)0,    1, 0, 0, 0,  75,  0,  20, IRON, HI_METAL),
#endif

/* Light sources */
/* [Tom] made candles cheaper & more common */
TOOL("tallow candle", "candle", 0, 1, 0, 0,  50,  0, 100, WAX, CLR_WHITE),
TOOL("wax candle", "candle",    0, 1, 0, 0,  40,  0, 100, WAX, CLR_WHITE),
TOOL("magic candle",  "candle", 0, 1, 1, 0,   5,  0, 100, WAX, CLR_WHITE),
TOOL("oil lamp", "lamp",        0, 0, 0, 0,  25, 0,  100, COPPER, CLR_YELLOW),
TOOL("brass lantern", (char *)0,1, 0, 0, 0,  15, 0,  100, COPPER, CLR_YELLOW),
TOOL("magic lamp", "lamp",      0, 0, 1, 0,  10, 0,100, COPPER, CLR_YELLOW),

/* Instruments */
/* KMH -- made less common */
TOOL("tin whistle", "whistle",  0, 0, 0, 0,  60,  0,  10, METAL, HI_METAL),
TOOL("magic whistle", "whistle",0, 0, 1, 0,  25,  0,  10, METAL, HI_METAL),
/* "If tin whistles are made out of tin, what do they make foghorns out of?" */
TOOL("wooden flute", "flute",   0, 0, 0, 0,   2,  0,  50, WOOD, HI_WOOD),
TOOL("magic flute", "flute",    0, 0, 1, 1,   1,  0,  50, WOOD, HI_WOOD),
TOOL("tooled horn", "horn",     0, 0, 0, 0,   2, 0,  50, BONE, CLR_WHITE),
TOOL("frost horn", "horn",      0, 0, 1, 1,   1, 0,  50, BONE, CLR_WHITE),
TOOL("fire horn", "horn",       0, 0, 1, 1,   1, 0,  50, BONE, CLR_WHITE),
TOOL("horn of plenty", "horn",  0, 0, 1, 1,   1, 0,  50, BONE, CLR_WHITE),
TOOL("wooden harp", "harp",     0, 0, 0, 0,   2, 0,  50, WOOD, HI_WOOD),
TOOL("magic harp", "harp",      0, 0, 1, 1,   1, 0,  50, WOOD, HI_WOOD),
TOOL("bell", (char *)0,         1, 0, 0, 0,   1, 0,  50, COPPER, HI_COPPER),
TOOL("bugle", (char *)0,        1, 0, 0, 0,   2, 0,  15, COPPER, HI_COPPER),
TOOL("leather drum", "drum",    0, 0, 0, 0,   2, 0,  25, LEATHER, HI_LEATHER),
TOOL("drum of earthquake", "drum",
                                0, 0, 1, 1,   1, 0,  25, LEATHER, HI_LEATHER),

/* Traps */
TOOL("land mine",(char *)0,     1, 0, 0, 0,   1,30, 180, IRON, CLR_RED),
TOOL("beartrap", (char *)0,     1, 0, 0, 0,   1,20,  60, IRON, HI_METAL),

/* Weapon-tools */
# ifdef P_SPOON
/* Added by Tsanth, in homage to Final Fantasy 2 */
/* KMH -- Not randomly generated (no damage!) */
WEPTOOL("spoon", (char *)0,
	1, 0, 0, 0,  1,  1, 5000,  0,  0, 0, WHACK,  -P_DART, PLATINUM, HI_METAL),
# endif /* P_SPOON */
#ifdef LIGHTSABERS
# ifdef D_SABER
WEPTOOL("pick-axe", (char *)0,
	1, 0, 0, 0, 15, 32,   50,  6,  3, 0, WHACK,  P_PICK_AXE, IRON, HI_METAL),
# else
WEPTOOL("pick-axe", (char *)0,
	1, 0, 0, 0, 16, 32,   50,  6,  3, 0, WHACK,  P_PICK_AXE, IRON, HI_METAL),
# endif
#else	/* LIGHTSABERS */
WEPTOOL("pick-axe", (char *)0,
	1, 0, 0, 0, 19, 32,   50,  6,  3, 0, WHACK,  P_PICK_AXE, IRON, HI_METAL),
#endif
WEPTOOL("fishing pole", (char *)0,
	1, 0, 0, 0,  5,  12,  50,  2,  6, 0, WHACK,  P_POLEARMS, METAL, HI_METAL),
WEPTOOL("grappling hook", "iron hook",
	0, 0, 0, 0,  5,  12,  50,  4,  8, 0, WHACK,  P_FLAIL, IRON, HI_METAL),
/* 3.4.1: unicorn horn left classified as "magic" */
WEPTOOL("unicorn horn", (char *)0,
	1, 1, 0, 1,  1,  8, 100, 12, 12, 0, PIERCE, P_UNICORN_HORN, BONE, CLR_WHITE),
/* WEPTOOL("torch", (char *)0,
	   1, 0, 0,  1,  8, 8, 5, 2, WHACK, P_CLUB, WOOD, HI_WOOD), */

OBJECT(OBJ("torch", (char *)0),
	BITS(1,1,1,0,0,1,0,0,0,0,WHACK,P_CLUB,WOOD),
	0, TOOL_CLASS, 25, 0,
	20, 8, 2, 5, WHACK, 0, 20, HI_WOOD ),

#ifdef LIGHTSABERS
/* [WAC]
 * Lightsabers are -3 to hit 
 * Double lightsaber is -4 to hit (only red)
 * DMG is increased: 10.5/15.5
 * green :9 + d3, 13 + d5
 * blue : 8 + d5, 12 + d7
 * red :  6 + d9, 10 + d11
 * red double: 6 + d9 + d9, 10 + d11 + d11  (15/21) in double mode
 */
WEPTOOL("green lightsaber", "lightsaber",
	0, 0, 1, 0,  1, 24, 500, 3,  5, 0, SLASH, P_LIGHTSABER, PLASTIC, HI_METAL),
# ifdef D_SABER
WEPTOOL("blue lightsaber",  "lightsaber",
	0, 0, 1, 0,  1, 24, 500, 5,  7, 0, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
# endif /* D_SABER */
WEPTOOL("red lightsaber",  "lightsaber",
	0, 0, 1, 0,  1, 24, 500, 9,  11, 0, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
WEPTOOL("red double lightsaber",  "double lightsaber",
	0, 0, 1, 1,  1, 24,1000, 15, 20, 0, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
# ifdef D_SABER
#if 0 // just an idea...
WEPTOOL("yellow lightsaber",  "lightsaber",
	0, 0, 1, 0,  0, 60, 500, 5,  7, 0, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
WEPTOOL("violet lightsaber",  "lightsaber",
	0, 0, 1, 0,  0, 60, 500, 5,  7, 0, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
WEPTOOL("white lightsaber",  "lightsaber",
	0, 0, 1, 0,  0, 60, 500, 5,  7, 0, SLASH, P_LIGHTSABER, PLATINUM, HI_METAL),
# endif /* D_SABER */
#endif
#endif	/* LIGHTSABERS */

/* Other tools */
#ifdef TOURIST
TOOL("expensive camera", (char *)0,
				1, 0, 0, 1,  10, 0, 200, PLASTIC, CLR_BLACK),
TOOL("mirror", "looking glass", 0, 0, 0, 0,  40, 0,  10, GLASS, HI_SILVER),
#else
TOOL("mirror", "looking glass", 0, 0, 0, 0,  50, 0,  10, GLASS, HI_SILVER),
#endif
TOOL("crystal ball", "glass orb",
								0, 0, 1, 1,  10,0,  60, GLASS, HI_GLASS),
#if 0
/* STEPHEN WHITE'S NEW CODE */
/* KMH -- removed because there's potential for abuse */
TOOL("orb of enchantment", "glass orb",
				0, 0, 1, 1,   5, 0, 750, GLASS, HI_GLASS),
TOOL("orb of charging", "glass orb",
				0, 0, 1, 1,   5, 0, 750, GLASS, HI_GLASS),
TOOL("orb of destruction", "glass orb",
				0, 0, 0, 0,   5, 0, 750, GLASS, HI_GLASS),
#endif
TOOL("lenses", (char *)0,	1, 0, 0, 0,   5,  0,  80, GLASS, HI_GLASS),
TOOL("blindfold", (char *)0,    1, 0, 0, 0,  50,  0,  20, CLOTH, CLR_BLACK),
TOOL("towel", (char *)0,        1, 0, 0, 0,  20,  0,  50, CLOTH, CLR_MAGENTA),
#ifdef STEED
TOOL("saddle", (char *)0,       1, 0, 0, 0,  35,0, 150, LEATHER, HI_LEATHER),
TOOL("leash", (char *)0,        1, 0, 0, 0,  14, 0,  20, LEATHER, HI_LEATHER),
#else
TOOL("leash", (char *)0,        1, 0, 0, 0,  49, 0,  20, LEATHER, HI_LEATHER),
#endif
TOOL("stethoscope", (char *)0,  1, 0, 0, 0,  25,  0,  75, IRON, HI_METAL),
TOOL("tinning kit", (char *)0,  1, 0, 0, 1,  15, 0,  30, IRON, HI_METAL),
CONTAINER("medical kit", "leather bag",
				0, 0, 0, 0,  10, 0, 500, LEATHER, HI_LEATHER),
TOOL("tin opener", (char *)0,   1, 0, 0, 0,  25,  0,  30, IRON, HI_METAL),
TOOL("can of grease", (char *)0,1, 0, 0, 1,  15, 0,  20, IRON, HI_METAL),
TOOL("figurine", (char *)0,     1, 0, 1, 0,  55, 0,  80, MINERAL, HI_MINERAL),
TOOL("magic marker", (char *)0, 1, 0, 1, 1,  15,  0,  50, PLASTIC, CLR_RED),
TOOL("Hitchhiker's Guide To The Galaxy", "highly-complicated electronic device",
				0, 0, 0, 0,   0, 42,  1000, IRON, HI_METAL),
TOOL("diode",		"two-wired thing",
				0, 1, 0, 0,   1,  1,    50, IRON, CLR_WHITE),
TOOL("transistor",	"three-wired thing",
				0, 1, 0, 0,   1,  1,   100, IRON, CLR_WHITE),
TOOL("IC",              "many-wired thing",
				0, 1, 0, 0,   1,  1,  5000, IRON, CLR_WHITE),
TOOL("pack of floppies",	"box containing little plastic cards",
				0, 1, 0, 0,   1,  3,   300,PLASTIC, CLR_RED),
TOOL("god-o-meter",	"highly-complicated electronic device",
				0, 0, 0, 0,   1,  5,   500, IRON, HI_METAL),
TOOL("relay",		"four-wired thing",	
				0, 1, 0, 0,   1,  2,   100, IRON, CLR_WHITE),
TOOL("bottle",		NULL,
				1, 1, 0, 0,  10,  1,     5, GLASS, HI_METAL),
TOOL("chemistry set",	"box of obscure-looking glass devices",
				0, 0, 0, 1,   5, 20,   500, GLASS, CLR_ORANGE),

/* Two pseudo tools. These can never exist outside of medical kits. */
OBJECT(OBJ("bandage", (char *)0),
		BITS(1,1,0,0,0,0,0,1,0,0,0,P_NONE,CLOTH), 0,
		TOOL_CLASS, 0, 0, 1, 1, 0, 0, 0, 0, 0, CLR_WHITE),
OBJECT(OBJ("phial", (char *)0),
		BITS(1,1,0,0,0,0,0,1,0,0,0,P_NONE,GLASS), 0,
		TOOL_CLASS, 0, 0, 2, 1, 0, 0, 0, 0, 1, HI_GLASS),

/* Two special unique artifact "tools" */
OBJECT(OBJ("Candelabrum of Invocation", "candelabrum"),
		BITS(0,0,1,0,1,0,1,1,0,0,0,0,GOLD), 0,
		TOOL_CLASS, 0, 0,10, 5000, 0, 0, 0, 0, 200, HI_GOLD),
OBJECT(OBJ("Bell of Opening", "silver bell"),
		BITS(0,0,1,0,1,1,1,1,0,0,0,0,SILVER), 0,
		TOOL_CLASS, 0, 0,10, 5000, 0, 0, 0, 0, 50, HI_SILVER),
#undef TOOL
#undef CONTAINER
#undef WEPTOOL


/* Comestibles ... */
#define FOOD(name,prob,delay,wt,unk,tin,nutrition,color) OBJECT( \
		OBJ(name,(char *)0), BITS(1,1,unk,0,0,0,0,0,0,0,0,0,tin), 0, \
		FOOD_CLASS, prob, delay, \
		wt, nutrition/20 + 5, 0, 0, 0, 0, nutrition, color )
/* all types of food (except tins & corpses) must have a delay of at least 1. */
/* delay on corpses is computed and is weight dependant */
/* dog eats foods 0-4 but prefers tripe rations above all others */
/* fortune cookies can be read */
/* carrots improve your vision */
/* +0 tins contain monster meat */
/* +1 tins (of spinach) make you stronger (like Popeye) */
/* food CORPSE is a cadaver of some type */
/* meatballs/sticks/rings are only created from objects via stone to flesh */

/* banana cures hallucination, melon cures confusion, pear cures stun, asian pear cures both confusion and stun --Amy */

/* Meat */
FOOD("tripe ration",       200, 2, 0, 0, FLESH, 250, CLR_BROWN),
FOOD("corpse",              50, 1,  0, 0, FLESH,   0, CLR_BROWN),
FOOD("egg",                 70, 1,  0, 1, FLESH,  100, CLR_WHITE),
FOOD("meatball",             1, 1,  0, 0, FLESH,   10, CLR_BROWN),
FOOD("meat stick",           1, 1,  0, 0, FLESH,   10, CLR_BROWN),
FOOD("huge chunk of meat",   1,20,0, 0, FLESH,2000, CLR_BROWN),
/* special case because it's not mergable */
OBJECT(OBJ("meat ring", (char *)0),
    BITS(1,0,0,0,0,0,0,0,0,0,0,0,FLESH),
    0, FOOD_CLASS, 0, 1, 5, 1, 0, 0, 0, 0, 5, CLR_BROWN),


/* Body parts.... eeeww */
FOOD("eyeball",              0, 1,  0, 0, FLESH,  10, CLR_WHITE),
FOOD("severed hand",         0, 1,  0, 0, FLESH,  40, CLR_BROWN),

/* Fruits & veggies */
FOOD("kelp frond",           1, 1,  0, 0, VEGGY,  75, CLR_GREEN),
FOOD("eucalyptus leaf",      9, 1,  0, 0, VEGGY, 100, CLR_GREEN),
FOOD("clove of garlic",     17, 1,  0, 0, VEGGY,  50, CLR_WHITE),
FOOD("sprig of wolfsbane",  17, 1,  0, 0, VEGGY,  50, CLR_GREEN),
FOOD("apple",               43, 1,  0, 0, VEGGY, 100, CLR_RED),
FOOD("carrot",              35, 1,  0, 0, VEGGY,  75, CLR_ORANGE),
FOOD("pear",                19, 1,  0, 0, VEGGY, 100, CLR_BRIGHT_GREEN),
FOOD("asian pear",          11, 1,  0, 0, VEGGY, 150, CLR_BRIGHT_GREEN),
FOOD("banana",              20, 1,  0, 0, VEGGY, 120, CLR_YELLOW),
FOOD("orange",              20, 1,  0, 0, VEGGY, 120, CLR_ORANGE),
FOOD("mushroom",             1, 1,  0, 0, VEGGY, 130, CLR_BLACK),
FOOD("melon",               19, 1,  0, 0, VEGGY, 175, CLR_BRIGHT_GREEN),
FOOD("slime mold",          75, 1,  0, 0, VEGGY, 250, HI_ORGANIC),
FOOD("peanut bag",	     5, 3,  0, 0, VEGGY, 800, HI_ORGANIC),

/* People food */
FOOD("lump of royal jelly",  1, 1,  0, 0, VEGGY, 200, CLR_YELLOW),
FOOD("cream pie",           10, 1, 0, 0, VEGGY, 100, CLR_WHITE),
FOOD("sandwich",            10, 1, 0, 0, FLESH, 100, CLR_WHITE),
FOOD("candy bar",           13, 1,  0, 0, VEGGY, 100, CLR_BROWN),
FOOD("fortune cookie",      75, 1,  0, 0, VEGGY,  40, CLR_YELLOW),
FOOD("pancake",             13, 2,  0, 0, VEGGY, 200, CLR_YELLOW),
FOOD("doughnut",	     1, 2,  0, 0, VEGGY,  20, CLR_BROWN),
FOOD("tortilla",             1, 2,  0, 0, VEGGY,  80, CLR_WHITE),
/* [Tom] more food.... taken off pancake (25) */
FOOD("cheese",              10, 2,  0, 0, FLESH, 250, CLR_YELLOW),
FOOD("pill",                 1, 1,  0, 0, VEGGY,   0, CLR_BRIGHT_MAGENTA),
FOOD("holy wafer",           7, 1,  0, 0, VEGGY, 400, CLR_WHITE),
FOOD("lembas wafer",        20, 2,  0, 0, VEGGY, 800, CLR_WHITE),
FOOD("cram ration",         20, 3, 0, 0, VEGGY, 750, HI_ORGANIC),
FOOD("food ration",        100, 5, 0, 0, VEGGY, 1000, HI_ORGANIC),
FOOD("Hacker's Food",	     1, 1, 0, 0, VEGGY, 800, HI_METAL),
FOOD("K-ration",             1, 1, 0, 0, VEGGY, 400, HI_ORGANIC),
FOOD("C-ration",             1, 1, 0, 0, VEGGY, 300, HI_ORGANIC),
FOOD("tin",                100, 0, 0, 1, METAL,   0, HI_METAL),
#undef FOOD

/* potions ... */
#define POTION(name,desc,mgc,power,prob,cost,color) OBJECT( \
		OBJ(name,desc), BITS(0,1,0,0,mgc,0,0,0,0,0,0,0,GLASS), power, \
		POTION_CLASS, prob, 0, 0, cost, 0, 0, 0, 0, 10, color )
POTION("booze", "brown",                0, 0,          30, 300, CLR_BROWN),
POTION("fruit juice", "dark",           0, 0,          40, 300, CLR_BLACK),
POTION("see invisible", "magenta",      1, SEE_INVIS,  33, 300, CLR_MAGENTA),
POTION("sickness", "fizzy",             0, 0,          25, 300, CLR_CYAN),
POTION("sleeping",  "effervescent",     1, 0,          25, 300, CLR_GRAY),
POTION("clairvoyance","luminescent",    1, 0,          20, 300, CLR_WHITE),
POTION("confusion", "orange",           1, CONFUSION,  35, 300, CLR_ORANGE),
POTION("hallucination", "sky blue",     1, HALLUC,     35, 300, CLR_CYAN),
POTION("healing", "purple-red",         1, 0,          55, 300, CLR_MAGENTA),
POTION("extra healing", "puce",         1, 0,          45, 300, CLR_RED),
POTION("restore ability", "pink",       1, 0,          40, 300, CLR_BRIGHT_MAGENTA),
POTION("blindness", "yellow",           1, BLINDED,    33, 300, CLR_YELLOW),
POTION("ESP", "muddy",                  1, TELEPAT,    20, 300, CLR_BROWN),
POTION("gain energy", "cloudy",         1, 0,          35, 300, CLR_WHITE),
POTION("gain health", "iridescent",     1, 0,           5, 300, CLR_ORANGE),
POTION("invisibility", "brilliant blue",1, INVIS,      40, 300, CLR_BRIGHT_BLUE),
POTION("monster detection", "bubbly",   1, 0,          38, 300, CLR_WHITE),
POTION("object detection", "smoky",     1, 0,          38, 300, CLR_GRAY),
POTION("enlightenment", "swirly",       1, 0,          20, 300, CLR_BROWN),
POTION("full healing",  "black",        1, 0,          35, 300, CLR_BLACK),
POTION("levitation", "cyan",            1, LEVITATION, 38, 300, CLR_CYAN),
POTION("polymorph", "golden",           1, 0,          10, 300, CLR_YELLOW),
POTION("speed", "dark green",           1, FAST,       38, 300, CLR_GREEN),
POTION("acid", "white",                 0, 0,          20, 300, CLR_WHITE),
POTION("oil", "murky",                  0, 0,          30, 300, CLR_BROWN),
POTION("gain ability", "ruby",          1, 0,          38, 300, CLR_RED),
POTION("gain level", "milky",           1, 0,          20, 300, CLR_WHITE),
POTION("invulnerability", "icy",        1, 0,           5, 300, CLR_BRIGHT_BLUE),
POTION("paralysis", "emerald",          1, 0,          38, 300, CLR_BRIGHT_GREEN),
POTION("extreme power",	"turquoise",    1, 0, 	        5, 300, CLR_CYAN),
POTION("recovery",	"reddish",      1, 0,           5, 300, CLR_RED),
POTION("heroism",	    "tan",	          1, 0,           5, 300, CLR_BROWN),
POTION("cyanide",	    "vermillion",     0, 0,           5, 300, CLR_RED),
POTION("radium",	    "crimson",        0, 0,           5, 300, CLR_RED),
POTION("Jolt Cola",	    "illegal",  0, 0,           5, 300, CLR_BROWN),
POTION("Pan Galactic Gargle Blaster",
			    "radiating",	    0, 0,           5, 300, CLR_WHITE),
POTION((char *)0, "thaumaturgic",              0, 0,           0,   0, CLR_BROWN),
POTION((char *)0, "shining",          0, 0,           0,   0, CLR_CYAN),
POTION((char *)0, "incandescent",        0, 0,           0,   0, CLR_WHITE),
POTION((char *)0, "freezing",                0, 0,           0,   0, CLR_BRIGHT_BLUE),
POTION((char *)0, "squishy",            0, 0,           0,   0, CLR_GREEN),
POTION((char *)0, "greasy",             0, 0,           0,   0, CLR_BLACK),
POTION((char *)0, "slimy",              0, 0,           0,   0, CLR_BROWN),
POTION((char *)0, "soapy",              0, 0,           0,   0, CLR_RED),
POTION((char *)0, "ochre",              0, 0,           0,   0, CLR_BROWN),
POTION((char *)0, "steamy",             0, 0,           0,   0, CLR_WHITE),
POTION((char *)0, "gooey",              0, 0,           0,   0, CLR_MAGENTA),
POTION((char *)0, "silver",             0, 0,           0,   0, HI_SILVER),
POTION((char *)0, "dangerous",          0, 0,           0,   0, CLR_RED),
POTION((char *)0, "creepy",             0, 0,           0,   0, CLR_GREEN),
POTION((char *)0, "distilled",             0, 0,           0,   0, CLR_BROWN),
POTION((char *)0, "warping",             0, 0,           0,   0, CLR_BRIGHT_BLUE),
POTION((char *)0, "unnatural",             0, 0,           0,   0, CLR_MAGENTA),
POTION((char *)0, "venomous",             0, 0,           0,   0, CLR_CYAN),
POTION((char *)0, "colorless",             0, 0,           0,   0, CLR_GRAY),
POTION((char *)0, "alchemical",             0, 0,           0,   0, CLR_BLACK),
POTION((char *)0, "fluorescent",             0, 0,           0,   0, CLR_ORANGE),
POTION((char *)0, "illuminated",             0, 0,           0,   0, CLR_YELLOW),
POTION((char *)0, "bright",             0, 0,           0,   0, CLR_WHITE),
POTION((char *)0, "carcinogenic",             0, 0,           0,   0, CLR_BRIGHT_CYAN),
POTION((char *)0, "liquid",             0, 0,           0,   0, CLR_BRIGHT_GREEN),
POTION((char *)0, "buzzing",             0, 0,           0,   0, CLR_BRIGHT_BLUE),
POTION((char *)0, "concocted",             0, 0,           0,   0, CLR_BRIGHT_MAGENTA),
POTION((char *)0, "amber",             0, 0,           0,   0, CLR_BROWN),
POTION((char *)0, "aquamarine",             0, 0,           0,   0, CLR_CYAN),
POTION((char *)0, "blue",             0, 0,           0,   0, CLR_BRIGHT_BLUE),
POTION((char *)0, "ecru",             0, 0,           0,   0, CLR_MAGENTA),
POTION((char *)0, "green",             0, 0,           0,   0, CLR_GREEN),
POTION((char *)0, "gray",             0, 0,           0,   0, CLR_GRAY),
POTION((char *)0, "plaid",             0, 0,           0,   0, CLR_WHITE),
POTION((char *)0, "purple",             0, 0,           0,   0, CLR_BRIGHT_MAGENTA),
POTION((char *)0, "red",             0, 0,           0,   0, CLR_RED),
POTION((char *)0, "tangerine",             0, 0,           0,   0, CLR_ORANGE),
POTION((char *)0, "topaz",             0, 0,           0,   0, CLR_YELLOW),
POTION((char *)0, "violet",             0, 0,           0,   0, CLR_BRIGHT_MAGENTA),
POTION("water", "clear",                0, 0,          55, 300, CLR_CYAN),
POTION("blood", "blood-red",            0, 0,           5, 300, CLR_RED),
POTION("vampire blood", "blood-red",    1, 0,           5, 300, CLR_RED),
POTION("amnesia", "sparkling",          1, 0,          16, 300, CLR_CYAN),
#undef POTION

/* scrolls ... */
#define SCROLL(name,text,sub,mgc,prob,cost,color) OBJECT( \
		OBJ(name,text), BITS(0,1,0,0,mgc,0,0,0,0,0,0,sub,PAPER), 0, \
		SCROLL_CLASS, prob, 0, 0, cost, 0, 0, 0, 0, 6, color )
/* Attack */
SCROLL("create monster",        "LEP GEX VEN ZEA",      P_ATTACK_SPELL, 1,  30, 300, CLR_RED),
SCROLL("summon undead",        "NO SPACE",      P_ATTACK_SPELL, 1,   5, 300, CLR_WHITE),
/* Enchantment */
SCROLL("taming",                "PRIRUTSENIE",          P_ENCHANTMENT_SPELL, 1,  35, 300, CLR_WHITE),
/* Divination */
SCROLL("light",                 "VERR YED HORRE",       P_DIVINATION_SPELL, 1,  20, 300, CLR_BLACK),
SCROLL("food detection",        "YUM YUM",              P_DIVINATION_SPELL, 1,  15, 300, CLR_YELLOW),
SCROLL("gold detection",        "THARR",                P_DIVINATION_SPELL, 1,  23, 300, CLR_BROWN),
SCROLL("identify",              "KERNOD WEL",           P_DIVINATION_SPELL, 1, 200, 300, CLR_GREEN),
SCROLL("inventory id",              "NOW YOU SEE IT",           P_DIVINATION_SPELL, 1,  5, 300, CLR_YELLOW),
SCROLL("magic mapping",         "ELAM EBOW",            P_DIVINATION_SPELL, 1,  35, 300, CLR_CYAN),
SCROLL("flood",                 "AQUE BRAGH",           P_DIVINATION_SPELL, 1,   5, 300, CLR_BRIGHT_BLUE),
SCROLL("gain mana",         "DAMN IT",            P_DIVINATION_SPELL, 1,  5, 300, CLR_BRIGHT_MAGENTA),
/* Enchantment */
SCROLL("confuse monster",       "NR 9",                 P_ENCHANTMENT_SPELL,  1,  23, 300, CLR_MAGENTA),
SCROLL("scare monster",         "XIXAXA XOXAXA XUXAXA", P_ENCHANTMENT_SPELL,  1,  35, 300, CLR_BRIGHT_BLUE),
SCROLL("enchant weapon",        "DAIYEN FOOELS",        P_ENCHANTMENT_SPELL,  1,  75, 300, CLR_RED),
SCROLL("enchant armor",         "ZELGO MER",            P_ENCHANTMENT_SPELL,  1,  63, 300, CLR_GREEN),
/* Protection */
SCROLL("remove curse",          "PRATYAVAYAH",          P_PROTECTION_SPELL, 1,  60, 300, CLR_BRIGHT_MAGENTA),
/* Body */
SCROLL("teleportation",         "VENZAR BORGAVVE",      P_BODY_SPELL, 1,  55, 300, CLR_BROWN),
SCROLL("tele level",         "GON GET YA",      P_BODY_SPELL, 1,  5, 300, CLR_BRIGHT_CYAN),
SCROLL("warping",         "MY SPUTA WILL FLOW DOWN",      P_BODY_SPELL, 1,  5, 300, CLR_BRIGHT_CYAN),
/* Matter */
SCROLL("fire",                  "ANDOVA BEGARIN",       P_MATTER_SPELL, 1,  33, 300, CLR_WHITE),
SCROLL("earth",                 "KIRJE",                P_MATTER_SPELL, 1,  20, 300, CLR_CYAN),

SCROLL("destroy armor",         "JUYED AWK YACC",       P_NONE, 1,  20, 300, CLR_YELLOW),
SCROLL("amnesia",               "DUAM XNAHT",           P_NONE, 1,  10, 300, CLR_BRIGHT_BLUE),
SCROLL("healing",               "LIFE RESTORATION",           P_NONE, 1,   5, 300, CLR_BRIGHT_CYAN),
SCROLL("standard id",               "WHAT THE FUCK IS THIS",           P_NONE, 1,   5, 300, CLR_ORANGE),
SCROLL("charging",              "HACKEM MUCHE",         P_NONE, 1,  25, 300, CLR_GREEN),
SCROLL("genocide",              "ELBIB YLOH",           P_NONE, 1,  15, 300, CLR_MAGENTA),
SCROLL("punishment",            "VE FORBRYDERNE",       P_NONE, 1,  15, 300, CLR_GRAY),
SCROLL("stinking cloud",	"VELOX NEB",            P_NONE, 1,  15, 300, CLR_GRAY),
SCROLL("trap detection",	"HOYO HOYO",            P_NONE, 1,  5, 300, CLR_YELLOW),
SCROLL("acquirement",	"THE RESULT IS THE FRUIT",            P_NONE, 1,  5, 300, CLR_GRAY),
SCROLL("proof armor",         "I AM THE KING",       P_NONE, 1,  5, 300, CLR_ORANGE),
SCROLL("proof weapon",         "YOU ARE DOOMED",       P_NONE, 1,  5, 300, CLR_GRAY),
SCROLL("mass murder",		"ENEMATZU MEMALEZU",	P_NONE,  1,  5, 300, CLR_BLACK),
SCROLL("undo genocide",		"42",			      P_NONE,  1,  5, 300, CLR_ORANGE),
SCROLL("reverse identify",	"OH CAPTAIN MY CAPTAIN",P_NONE,  1,  5, 300, CLR_RED),
SCROLL("wishing",		"TRICK 13",		      P_NONE,  1,  5, 300, CLR_WHITE),
SCROLL("consecration",		"MAY THE FORCE BE WITH YOU",
								      P_NONE,  1,  5, 300, CLR_BRIGHT_CYAN),
SCROLL("enthronization",		"XUS MODA",	      P_NONE,  1,  5, 300, CLR_BLACK),
SCROLL("fountain building",		"XOR ME HSALS",	      P_NONE,  1,  5, 300, CLR_YELLOW),
SCROLL("sinking",		"SPORKHACK ASCENSION",	      P_NONE,  1,  5, 300, CLR_CYAN),
SCROLL("WC",		"LETS PLAY NETHACK",	      P_NONE,  1,  5, 300, CLR_WHITE),
SCROLL("lava",		"LETS FAIL AND ROLL",	      P_NONE,  1,  5, 300, CLR_GRAY),
SCROLL("growth",		"WELCOME TO ANGBAND",	      P_NONE,  1,  5, 300, CLR_BRIGHT_GREEN),
SCROLL("ice",		"IMMORTAL DUNGEON",	      P_NONE,  1,  5, 300, CLR_GREEN),
SCROLL("clouds",		"VOLARE",	      P_NONE,  1,  5, 300, CLR_BRIGHT_BLUE),
SCROLL("barrhing",		"CONVICT PATCH",	      P_NONE,  1,  5, 300, CLR_BRIGHT_MAGENTA),
SCROLL("lockout",		"WE WILL HURT YOU",	      P_NONE,  1,  5, 300, CLR_ORANGE),
SCROLL("root password detection","XYZZY",		      P_NONE,  1,  5, 300, CLR_GRAY),
SCROLL("trap creation","TOP SECRET DO NOT READ",      P_NONE,  1,  5, 300, CLR_BLACK),
SCROLL("sleep","I AM SO FUCKING TIRED",         P_NONE, 1,   5, 300, CLR_BRIGHT_MAGENTA),
SCROLL((char *)0,		"FOOBIE BLETCH",        P_NONE, 1,   0, 300, CLR_CYAN),
SCROLL((char *)0,		"TEMOV",                P_NONE, 1,   0, 300, CLR_BLACK),
SCROLL((char *)0,		"GARVEN DEH",           P_NONE, 1,   0, 300, CLR_GREEN),
SCROLL((char *)0,		"READ ME",              P_NONE, 1,   0, 300, CLR_WHITE),
SCROLL((char *)0,               "ETAOIN SHRDLU",   P_NONE,      1,   0, 300, CLR_BRIGHT_CYAN),
SCROLL((char *)0,               "LOREM IPSUM",     P_NONE,      1,   0, 300, CLR_BRIGHT_BLUE),
SCROLL((char *)0,               "FNORD",           P_NONE,      1,   0, 300, CLR_BROWN), /* Illuminati */
SCROLL((char *)0,               "KO BATE",         P_NONE,      1,   0, 300, CLR_RED), /* Kurd Lasswitz */
SCROLL((char *)0,               "ACHAT SHTAYIM SHALOSH",P_NONE, 1,   0, 300, CLR_BRIGHT_BLUE), /* Uri Geller */
SCROLL((char *)0,               "ABRA KA DABRA",     P_NONE,    1,   0, 300, CLR_BLACK), /* traditional incantation */
SCROLL((char *)0,               "ASHPD",          P_NONE,       1,   0, 300, CLR_BLACK), /* Portal */
SCROLL((char *)0,               "SODALG",          P_NONE,      1,   0, 300, CLR_RED), /* Portal */
SCROLL((char *)0,               "ZLORFIK",         P_NONE,      1,   0, 300, CLR_ORANGE), /* Zak McKracken */
SCROLL((char *)0,               "GNIK SISI VLE",     P_NONE,    1,   0, 300, CLR_GREEN), /* Zak McKracken */
SCROLL((char *)0,               "HAPAX LEGOMENON",  P_NONE,     1,   0, 300, CLR_BRIGHT_BLUE),
SCROLL((char *)0,               "HZLRC KSTSBD MPFNG", P_NONE,   1,   0, 300, CLR_YELLOW),
SCROLL((char *)0,               "EIRIS SAZUN IDISI",  P_NONE,   1,   0, 300, CLR_BRIGHT_GREEN), /* Merseburg Incantations */
SCROLL((char *)0,               "PHOL ENDE WODAN",   P_NONE,    1,   0, 300, CLR_GRAY), /* Merseburg Incantations */
SCROLL((char *)0,               "GHOTI",             P_NONE,    1,   0, 300, CLR_BRIGHT_BLUE), /* pronounced as 'fish', George Bernard Shaw */
SCROLL((char *)0,               "MAPIRO MAHAMA DIROMAT",P_NONE, 1,   0, 300, CLR_BRIGHT_MAGENTA), /* Wizardry */
SCROLL((char *)0,               "VAS CORP BET MANI", P_NONE,    1,   0, 300, CLR_GRAY), /* Ultima */
/* these must come last because they have special descriptions */
SCROLL((char *)0,               "CLONE MONSTER",  P_NONE,     1,   0, 300, CLR_BRIGHT_GREEN),
SCROLL((char *)0,               "RESURRECTION",  P_NONE,     1,   0, 300, CLR_WHITE),
SCROLL((char *)0,               "APOCALYPSE",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "WORLD FALL",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "CATACLYSM",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "HAHAHAHAHAHAHA",  P_NONE,     1,   0, 300, CLR_BRIGHT_GREEN),
SCROLL((char *)0,               "LA VIVME",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "U FOKIENG SUK",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "SIR I HAVE TO TAKE A SHIT",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "YOU DONT STAND A CHANCE",  P_NONE,     1,   0, 300, CLR_WHITE),
SCROLL((char *)0,               "VUS AS CORIUMA",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "SE FERSE PARUM",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "ABOR EFECTOR",  P_NONE,     1,   0, 300, CLR_GRAY),
SCROLL((char *)0,               "ALIUS PRA",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "AM PO COLVATIO",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "CO APPESTUS",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "CONE ALACIO",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "CRIAEDO",  P_NONE,     1,   0, 300, CLR_MAGENTA),
SCROLL((char *)0,               "DEVIENEX COER",  P_NONE,     1,   0, 300, CLR_CYAN),
SCROLL((char *)0,               "DUCTUM ADIS",  P_NONE,     1,   0, 300, CLR_GRAY),
SCROLL((char *)0,               "INE IGO PIO",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "INQUE COMUS",  P_NONE,     1,   0, 300, CLR_BRIGHT_GREEN),
SCROLL((char *)0,               "INS CUBITOR",  P_NONE,     1,   0, 300, CLR_YELLOW),
SCROLL((char *)0,               "IUSTIO QUE PIS",  P_NONE,     1,   0, 300, CLR_BRIGHT_BLUE),
SCROLL((char *)0,               "LAM EQUENUS",  P_NONE,     1,   0, 300, CLR_BRIGHT_MAGENTA),
SCROLL((char *)0,               "LENS ALITUS",  P_NONE,     1,   0, 300, CLR_BRIGHT_CYAN),
SCROLL((char *)0,               "MA MILLOCOR",  P_NONE,     1,   0, 300, CLR_WHITE),
SCROLL((char *)0,               "NIOQUO TARGO",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "NONO STENTUM",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "PAS DIO LEO",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "PONUS MOSTAS",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "PRIUS SA",  P_NONE,     1,   0, 300, CLR_MAGENTA),
SCROLL((char *)0,               "PROR PERPERCA",  P_NONE,     1,   0, 300, CLR_CYAN),
SCROLL((char *)0,               "PUTIUS CONSIO",  P_NONE,     1,   0, 300, CLR_GRAY),
SCROLL((char *)0,               "PUTO FER LO",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "REQUICO SIDES",  P_NONE,     1,   0, 300, CLR_BRIGHT_GREEN),
SCROLL((char *)0,               "SALIQUAS",  P_NONE,     1,   0, 300, CLR_YELLOW),
SCROLL((char *)0,               "SERO GRIUS",  P_NONE,     1,   0, 300, CLR_BRIGHT_BLUE),
SCROLL((char *)0,               "OXHE SRIAN",  P_NONE,     1,   0, 300, CLR_BRIGHT_MAGENTA),
SCROLL((char *)0,               "SEVER OPEROFOR",  P_NONE,     1,   0, 300, CLR_BRIGHT_CYAN),
SCROLL((char *)0,               "SUS COMNITUS",  P_NONE,     1,   0, 300, CLR_WHITE),
SCROLL((char *)0,               "SUS QUO SUM",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "SUSY THENSE",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "TO DO CANCENUS",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "TRAETOR HORIGO",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "UTOR COMPERIO",  P_NONE,     1,   0, 300, CLR_MAGENTA),
SCROLL((char *)0,               "VITIO PUS RO",  P_NONE,     1,   0, 300, CLR_CYAN),
SCROLL((char *)0,               "VITIO SUS",  P_NONE,     1,   0, 300, CLR_GRAY),
SCROLL((char *)0,               "VO AMMITAM SA",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "VO FACIAM",  P_NONE,     1,   0, 300, CLR_BRIGHT_GREEN),
SCROLL((char *)0,               "WIZZY LABOR CO",  P_NONE,     1,   0, 300, CLR_YELLOW),
SCROLL((char *)0,               "KAME HAME HA",  P_NONE,     1,   0, 300, CLR_BRIGHT_BLUE),
SCROLL((char *)0,               "SECTUM SEMPRA",  P_NONE,     1,   0, 300, CLR_BRIGHT_MAGENTA),
SCROLL((char *)0,               "EYGOORTS TOGAAL JEZEHH",  P_NONE,     1,   0, 300, CLR_BRIGHT_MAGENTA),
SCROLL((char *)0,               "YOU WILL DIE A SLOW SLOW DEATH",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "BLUE SCREEN OF DEATH",  P_NONE,     1,   0, 300, CLR_BRIGHT_BLUE),
SCROLL((char *)0,               "SEE IF I CARE",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "VIRUS INFECTION DETECTED",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "INTRUDER ALERT",  P_NONE,     1,   0, 300, CLR_GRAY),
SCROLL((char *)0,               "USER DEATH IMMINENT",  P_NONE,     1,   0, 300, CLR_WHITE),
SCROLL((char *)0,               "I HERE SOMETHING",  P_NONE,     1,   0, 300, CLR_GRAY),
SCROLL((char *)0,               "GOT SOMETHING FOUR YOU",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "MY PRIVATE IS TENMETERS ACROSS",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "ECNETOPINMO",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "SUTSI",  P_NONE,     1,   0, 300, CLR_MAGENTA),
SCROLL((char *)0,               "YIK MECH SHEGOTH",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "KAPOW",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "YNORI ERUP",  P_NONE,     1,   0, 300, CLR_BRIGHT_BLUE),
SCROLL((char *)0,               "REGNAD",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "ODIV",  P_NONE,     1,   0, 300, CLR_WHITE),
SCROLL((char *)0,               "ZTORF",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "HITME",  P_NONE,     1,   0, 300, CLR_CYAN),
SCROLL((char *)0,               "MANANNANMACLIR",  P_NONE,     1,   0, 300, CLR_BRIGHT_CYAN),
SCROLL((char *)0,               "MEAD NI NAJAP",  P_NONE,     1,   0, 300, CLR_CYAN),
SCROLL((char *)0,               "MKMNFST",  P_NONE,     1,   0, 300, CLR_YELLOW),
SCROLL((char *)0,               "DOGO FUTSF",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "MANUALIRK GLEKNOW MIZK",  P_NONE,     1,   0, 300, CLR_BRIGHT_CYAN),
SCROLL((char *)0,               "KLATHA DATHA NY ARYATHA",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "IAZNAB",  P_NONE,     1,   0, 300, CLR_BRIGHT_GREEN),
SCROLL((char *)0,               "WOPAK",  P_NONE,     1,   0, 300, CLR_MAGENTA),
SCROLL((char *)0,               "HCUO",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "ENO OWT EERHT",  P_NONE,     1,   0, 300, CLR_BRIGHT_MAGENTA),
SCROLL((char *)0,               "GNOZAB",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "ABABILA",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "MUNCH MUNCH",  P_NONE,     1,   0, 300, CLR_BRIGHT_GREEN),
SCROLL((char *)0,               "FROTZ",  P_NONE,     1,   0, 300, CLR_YELLOW),
SCROLL((char *)0,               "EDEVESH HANAGASH",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "YIKES",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "IDNUM AIROLG TISNART CIS",  P_NONE,     1,   0, 300, CLR_ORANGE),
SCROLL((char *)0,               "RAMBA ZAMBA",  P_NONE,     1,   0, 300, CLR_BLACK),
SCROLL((char *)0,               "KTTDLWRGT",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "GNAY GNIY",  P_NONE,     1,   0, 300, CLR_GRAY),
SCROLL((char *)0,               "CARPE DIEM",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "CUTME",  P_NONE,     1,   0, 300, CLR_BRIGHT_CYAN),
SCROLL((char *)0,               "FOO BAR",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "FPMAM",  P_NONE,     1,   0, 300, CLR_BROWN),
SCROLL((char *)0,               "FOX NAWAGU",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "YTTOCS PU EM MAEB",  P_NONE,     1,   0, 300, CLR_BRIGHT_CYAN),
SCROLL((char *)0,               "NADSIL SEETRURA",  P_NONE,     1,   0, 300, CLR_MAGENTA),
SCROLL((char *)0,               "ACME",  P_NONE,     1,   0, 300, CLR_GREEN),
SCROLL((char *)0,               "CHUTZPAH",  P_NONE,     1,   0, 300, CLR_CYAN),
SCROLL((char *)0,               "TLASFO SENIL",  P_NONE,     1,   0, 300, CLR_WHITE),
SCROLL((char *)0,               "RW NW PRT M HRW",  P_NONE,     1,   0, 300, CLR_RED),
SCROLL((char *)0,               "MORPH BALL",  P_NONE,     1,   0, 300, CLR_GRAY),
SCROLL((char *)0,               "TUOE DISNI",  P_NONE,     1,   0, 300, CLR_BRIGHT_GREEN),
SCROLL((char *)0,               "FASUN SEEEEP",  P_NONE,     1,   0, 300, CLR_BRIGHT_MAGENTA),
#ifdef MAIL
SCROLL("mail",                  "stamped",              P_NONE, 0,   0,   0, CLR_BRIGHT_BLUE),
#endif
SCROLL("blank paper",           "unlabeled",            P_NONE, 0,  28, 300, CLR_GRAY),
#undef SCROLL

/* spell books ... */
#define SPELL(name,desc,sub,prob,delay,level,mgc,dir,color) OBJECT( \
		OBJ(name,desc), BITS(0,0,1,0,mgc,1,0,0,0,0,dir,sub,PAPER), 0, \
		SPBOOK_CLASS, prob, delay, \
		0, level*100, 0, 0, 0, level, 20, color )
/* Attack spells */
SPELL("force bolt",      "red",         P_ATTACK_SPELL, 25,  2, 1, 1, IMMEDIATE, CLR_RED),
SPELL("create monster",  "turquoise",   P_ATTACK_SPELL, 20,  3, 2, 1, NODIR,     CLR_BRIGHT_CYAN),
SPELL("drain life",      "velvet",      P_ATTACK_SPELL, 10,  4, 3, 1, IMMEDIATE, CLR_MAGENTA),
  /* NEEDS TILE */ /* WAC -- probs from force bolt and extra healing */
SPELL("command undead",  "dark",        P_ATTACK_SPELL, 10,  7, 5, 1, IMMEDIATE, CLR_BLACK),
SPELL("summon undead",   "black",       P_ATTACK_SPELL, 10,  7, 5, 1, IMMEDIATE, CLR_BLACK),
/* Healing spells */
SPELL("stone to flesh",	 "thick",       P_HEALING_SPELL, 15,  1, 3, 1, IMMEDIATE, HI_PAPER),
SPELL("healing",         "white",       P_HEALING_SPELL, 25,  2, 1, 1, IMMEDIATE, CLR_WHITE),
SPELL("cure blindness",  "yellow",      P_HEALING_SPELL, 20,  2, 2, 1, IMMEDIATE, CLR_YELLOW),
SPELL("cure sickness",   "indigo",      P_HEALING_SPELL, 20,  3, 3, 1, NODIR,     CLR_BRIGHT_BLUE),
SPELL("cure hallucination", "glowing",  P_HEALING_SPELL, 15,  3, 3, 1, NODIR,     CLR_YELLOW),
SPELL("cure confusion",   "moldy",      P_HEALING_SPELL, 15,  3, 3, 1, NODIR,     CLR_BROWN),
SPELL("cure stun",   "fat",      	    P_HEALING_SPELL, 15,  3, 3, 1, NODIR,     CLR_BRIGHT_MAGENTA),
SPELL("extra healing",   "plaid",       P_HEALING_SPELL, 15,  5, 3, 1, IMMEDIATE, CLR_GREEN),
SPELL("full healing",   "heretical",       P_HEALING_SPELL, 5,  8, 6, 1, IMMEDIATE, CLR_BRIGHT_MAGENTA),
SPELL("restore ability", "light brown", P_HEALING_SPELL, 15,  5, 4, 1, NODIR,     CLR_BROWN),
SPELL("create familiar", "glittering",  P_HEALING_SPELL, 10,  7, 6, 1, NODIR,     CLR_WHITE),
/* Divination spells */
SPELL("light",           "cloth",       P_DIVINATION_SPELL, 15,  1, 1, 1, NODIR,     HI_CLOTH),
SPELL("detect monsters", "leather",     P_DIVINATION_SPELL, 15,  1, 1, 1, NODIR,     HI_LEATHER),
SPELL("detect food",     "cyan",        P_DIVINATION_SPELL,  5,  3, 2, 1, NODIR,     CLR_CYAN),
SPELL("clairvoyance",    "dark blue",   P_DIVINATION_SPELL, 15,  3, 3, 1, NODIR,     CLR_BRIGHT_BLUE),
SPELL("detect unseen",   "violet",      P_DIVINATION_SPELL, 15,  4, 3, 1, NODIR,     CLR_MAGENTA),
SPELL("identify",        "bronze",      P_DIVINATION_SPELL, 30,  8, 5, 1, NODIR,     HI_COPPER),
SPELL("detect treasure", "gray",        P_DIVINATION_SPELL, 15,  5, 4, 1, NODIR,     CLR_GRAY),
SPELL("magic mapping",   "dusty",       P_DIVINATION_SPELL, 15,  7, 5, 1, NODIR,     HI_PAPER),
SPELL("entrapping",    "scintillating", P_DIVINATION_SPELL,  5,  7, 6, 1, NODIR,     CLR_BRIGHT_MAGENTA),
SPELL("finger",		 "old",	    P_DIVINATION_SPELL,	 5,  3, 2, 1, IMMEDIATE, HI_PAPER),
SPELL("chemistry",	 "brand-new",   P_DIVINATION_SPELL,	10, 10, 1, 1, NODIR,     HI_PAPER),
SPELL("detect foot",	 "stinking",  P_DIVINATION_SPELL,	 5,  2, 7, 1, NODIR, CLR_GREEN),
/* Enchantment spells */
SPELL("confuse monster", "orange",      P_ENCHANTMENT_SPELL, 10,  2, 2, 1, IMMEDIATE, CLR_ORANGE),
SPELL("slow monster",    "light green", P_ENCHANTMENT_SPELL, 25,  2, 2, 1, IMMEDIATE, CLR_BRIGHT_GREEN),
SPELL("cause fear",      "light blue",  P_ENCHANTMENT_SPELL, 10,  3, 3, 1, NODIR,     CLR_BRIGHT_BLUE),
SPELL("charm monster",   "magenta",     P_ENCHANTMENT_SPELL, 10,  3, 3, 1, IMMEDIATE, CLR_MAGENTA),
SPELL("enchant weapon",  "dull",        P_ENCHANTMENT_SPELL, 15,  8, 7, 1, NODIR,     CLR_WHITE),
SPELL("enchant armor",   "thin",        P_ENCHANTMENT_SPELL, 15,  8, 7, 1, NODIR,     CLR_WHITE),
SPELL("charging",   "stellar",          P_ENCHANTMENT_SPELL, 5,   8, 7, 1, NODIR,     CLR_WHITE),
/* Protection spells */
SPELL("protection",      "wide",        P_PROTECTION_SPELL,  5,  3, 1, 1, NODIR,     HI_PAPER),
SPELL("resist poison",   "big",         P_PROTECTION_SPELL, 10,  2, 1, 1, NODIR,     CLR_GRAY),
SPELL("resist sleep",    "fuzzy",       P_PROTECTION_SPELL, 15,  2, 1, 1, NODIR,     CLR_BROWN),
SPELL("endure cold",     "deep",        P_PROTECTION_SPELL, 10,  3, 2, 1, NODIR,     HI_PAPER),
SPELL("endure heat",     "spotted",     P_PROTECTION_SPELL, 10,  3, 2, 1, NODIR,     HI_PAPER),
SPELL("insulate",        "long",        P_PROTECTION_SPELL, 10,  3, 2, 1, NODIR,     HI_PAPER),
SPELL("remove curse",    "wrinkled",    P_PROTECTION_SPELL, 25,  5, 5, 1, NODIR,     HI_PAPER),
SPELL("turn undead",     "copper",      P_PROTECTION_SPELL, 15,  8, 6, 1, IMMEDIATE, HI_COPPER),
SPELL("anti disintegration",        "doorstopping",        P_PROTECTION_SPELL, 5,  6, 6, 1, NODIR,     CLR_GREEN),
SPELL("botox resist",        "leafy",   P_PROTECTION_SPELL, 5,  6, 6, 1, NODIR,     CLR_BRIGHT_GREEN),
SPELL("acidshield",        "acidic",   P_PROTECTION_SPELL, 5,  6, 6, 1, NODIR,     CLR_BRIGHT_GREEN),
SPELL("godmode",        "heavenly",   P_PROTECTION_SPELL, 5,  8, 7, 1, NODIR,     CLR_BRIGHT_CYAN),
SPELL("resist petrification",        "rocky",   P_PROTECTION_SPELL, 5,  8, 7, 1, NODIR,     CLR_GRAY),
/* Body spells */
SPELL("jumping",         "torn",        P_BODY_SPELL, 15,  3, 1, 1, IMMEDIATE, HI_PAPER),
SPELL("haste self",      "purple",      P_BODY_SPELL,  5,  4, 3, 1, NODIR,     CLR_MAGENTA),
SPELL("enlighten",       "faded",       P_BODY_SPELL, 15,  5, 4, 1, NODIR,     CLR_GRAY),
SPELL("invisibility",    "dark brown",  P_BODY_SPELL, 10,  5, 4, 1, NODIR,     CLR_BROWN),
SPELL("levitation",      "tan",         P_BODY_SPELL, 15,  4, 4, 1, NODIR,     CLR_BROWN),
SPELL("teleport away",   "gold",        P_BODY_SPELL, 15,  6, 6, 1, IMMEDIATE, HI_GOLD),
SPELL("passwall",        "ochre",       P_BODY_SPELL,  5,  7, 6, 1, NODIR,     CLR_YELLOW),
SPELL("polymorph",       "silver",      P_BODY_SPELL, 15,  8, 6, 1, IMMEDIATE, HI_SILVER),
/* Matter spells */
SPELL("knock",           "pink",        P_MATTER_SPELL, 25,  1, 1, 1, IMMEDIATE, CLR_BRIGHT_MAGENTA),
SPELL("flame sphere",    "canvas",      P_MATTER_SPELL, 20,  2, 1, 1, NODIR, CLR_BROWN),
SPELL("freeze sphere",   "hardcover",   P_MATTER_SPELL, 20,  2, 1, 1, NODIR, CLR_BROWN),
SPELL("shocking sphere", "insulated",   P_MATTER_SPELL, 5,   2, 1, 1, NODIR, CLR_YELLOW),
SPELL("acid sphere", "corroded",   P_MATTER_SPELL, 5,   2, 1, 1, NODIR, CLR_YELLOW),
SPELL("wizard lock",     "dark green",  P_MATTER_SPELL, 25,  3, 2, 1, IMMEDIATE, CLR_GREEN),
SPELL("dig",             "parchment",   P_MATTER_SPELL, 20,  6, 5, 1, RAY,       HI_PAPER),
SPELL("cancellation",    "shining",     P_MATTER_SPELL,  5,  8, 7, 1, IMMEDIATE, CLR_WHITE),
SPELL("reflection",	 "withered",    P_MATTER_SPELL,  5,  7, 6, 1, IMMEDIATE, CLR_BROWN),
SPELL("repair armor",	 "pergament",   P_MATTER_SPELL,  5,  6, 3, 1, IMMEDIATE, HI_PAPER),
/* Ray type spells are all here.  Kludge for zap.c */
SPELL("magic missile",   "vellum",      P_ATTACK_SPELL, 40,  3, 2, 1, RAY,       HI_PAPER),
SPELL("fireball",        "ragged",      P_MATTER_SPELL, 15,  6, 4, 1, RAY,       HI_PAPER),
SPELL("cone of cold",    "dog eared",   P_MATTER_SPELL, 15,  8, 5, 1, RAY,       HI_PAPER),
SPELL("sleep",           "mottled",     P_ENCHANTMENT_SPELL, 35,  1, 1, 1, RAY,  HI_PAPER),
SPELL("finger of death", "stained",     P_ATTACK_SPELL,  5, 10, 7, 1, RAY,       HI_PAPER),
SPELL("lightning",       "rainbow",     P_MATTER_SPELL, 10,  7, 4, 1, RAY,       HI_PAPER),
SPELL("poison blast",    "tattered",    P_ATTACK_SPELL, 10,  7, 4, 1, RAY,       HI_PAPER),
SPELL("acid stream",     "colorful",    P_MATTER_SPELL, 10,  7, 4, 1, RAY,       HI_PAPER),
SPELL("solar beam",     "mirroring",    P_MATTER_SPELL, 5,  10, 6, 1, RAY,       CLR_BRIGHT_CYAN),
/* Description placeholders and special spellbooks */
SPELL((char *)0,		 "tartan",      P_NONE,  0,  0, 0, 1, 0,         CLR_RED),
SPELL((char *)0,		 "stylish",     P_NONE,  0,  0, 0, 1, 0,         HI_PAPER),
SPELL((char *)0,		 "psychedelic", P_NONE,  0,  0, 0, 1, 0,         CLR_BRIGHT_MAGENTA),
SPELL((char *)0,		 "spiral-bound",P_NONE,  0,  0, 0, 1, 0,         HI_PAPER),
SPELL((char *)0,		 "left-handed", P_NONE,  0,  0, 0, 1, 0,         HI_PAPER),
SPELL((char *)0,		 "stapled",     P_NONE,  0,  0, 0, 1, 0,         HI_PAPER),
SPELL((char *)0,		 "ancient",     P_NONE,  0,  0, 0, 1, 0,         CLR_BRIGHT_GREEN),
SPELL((char *)0,         "chartreuse",  P_NONE, 0, 0, 0, 1, 0, CLR_GREEN),
SPELL((char *)0,         "decrepit",    P_NONE, 0, 0, 0, 1, 0, CLR_BROWN),
SPELL((char *)0,         "paperback",   P_NONE, 0, 0, 0, 1, 0, HI_PAPER),
SPELL((char *)0,         "crimson",     P_NONE, 0, 0, 0, 1, 0, CLR_RED),
SPELL((char *)0,         "charcoal",    P_NONE, 0, 0, 0, 1, 0, CLR_BLACK),
SPELL((char *)0,         "cryptic",    P_NONE, 0, 0, 0, 1, 0, CLR_BLACK),
SPELL((char *)0,         "encrypted",    P_NONE, 0, 0, 0, 1, 0, CLR_BLACK),
SPELL("blank paper",	 "plain",		P_NONE, 15,  0, 0, 0, 0,         HI_PAPER),
	/* ...Blank spellbook must come last because it retains its description */
OBJECT(OBJ("Book of the Dead", "papyrus"),
	BITS(0,0,1,0,1,0,1,1,0,0,0,0,PAPER), 0,
	SPBOOK_CLASS, 0, 0,20, 10000, 0, 0, 0, 7, 20, HI_PAPER),
	/* ...A special, one of a kind, spellbook */
#undef SPELL

/* wands ... */
#define WAND(name,typ,prob,cost,mgc,dir,metal,color) OBJECT( \
		OBJ(name,typ), BITS(0,0,1,0,mgc,1,0,0,0,0,dir,0,metal), 0, \
		WAND_CLASS, prob, 0, 0, cost, 0, 0, 0, 0, 30, color )
WAND("light",          "glass",    35, 500, 1, NODIR,     GLASS,    HI_GLASS),
WAND("nothing",        "oak",       5, 500, 0, IMMEDIATE, WOOD,     HI_WOOD),
WAND("enlightenment",  "crystal",  30, 500, 1, NODIR,     GLASS,    HI_GLASS),
WAND("healing",        "bamboo",   50, 500, 1, IMMEDIATE, WOOD,     CLR_YELLOW),
WAND("locking",        "aluminum", 25, 500, 1, IMMEDIATE, METAL,    HI_METAL),
WAND("make invisible", "marble",   25, 500, 1, IMMEDIATE, MINERAL,  HI_MINERAL),
WAND("identify",       "wedged",    5, 500, 1, NODIR,     WOOD,     CLR_BRIGHT_GREEN),
WAND("remove curse",   "wiry",      5, 500, 1, NODIR,     METAL,    CLR_GRAY),
WAND("punishment",     "edgy",      5, 500, 1, NODIR,     PAPER,    CLR_WHITE),
WAND("opening",        "zinc",     25, 500, 1, IMMEDIATE, METAL,    HI_METAL),
WAND("probing",        "uranium",  20, 500, 1, IMMEDIATE, METAL,    HI_METAL),
WAND("secret door detection", "balsa",
                                   20, 500, 1, NODIR,     WOOD,     HI_WOOD),
WAND("entrapping",   "blasted",  5, 500, 1, NODIR,     MITHRIL,  CLR_BRIGHT_MAGENTA),
WAND("trap creation",   "weird",  5, 500, 1, NODIR,     BONE,  CLR_WHITE),
WAND("darkness",   "obscure",  5, 500, 1, NODIR,     SILVER,  CLR_BLACK),
WAND("magic mapping",   "shapeless",  5, 500, 1, NODIR,     DRAGON_HIDE,  CLR_BRIGHT_GREEN),
WAND("detect monsters", "exploding",  5, 500, 1, NODIR,     WOOD,     CLR_GREEN),
WAND("objection", "cinnabar", 5, 500, 1, NODIR,     SILVER,   CLR_ORANGE),
WAND("slow monster",   "tin",      45, 500, 1, IMMEDIATE, METAL,    HI_METAL),
WAND("speed monster",  "brass",    35, 500, 1, IMMEDIATE, COPPER,   HI_COPPER),
WAND("haste monster",  "royal",    5, 500, 1, IMMEDIATE, GOLD,   CLR_ORANGE),
WAND("striking",       "ebony",    60, 500, 1, IMMEDIATE, WOOD,     HI_WOOD),
WAND("undead turning", "copper",   45, 500, 1, IMMEDIATE, COPPER,   HI_COPPER),
WAND("draining",       "ceramic",  25, 500, 1, IMMEDIATE, MINERAL,  HI_MINERAL),
		/* KMH -- 15/1000 probability from light */
WAND("cancellation",   "platinum", 35, 500, 1, IMMEDIATE, PLATINUM, CLR_WHITE),
WAND("create monster", "maple",    30, 500, 1, NODIR,     WOOD,     HI_WOOD),
WAND("summon undead", "shining",     5, 500, 1, NODIR,     GEMSTONE,     CLR_YELLOW),
WAND("fear",           "rusty",    20, 500, 1, IMMEDIATE, IRON,     CLR_RED),
WAND("wind",           "timber",     5, 500, 1, IMMEDIATE, WOOD,     CLR_BROWN),
WAND("polymorph",      "silver",   45, 500, 1, IMMEDIATE, SILVER,   HI_SILVER),
WAND("teleportation",  "iridium",  45, 500, 1, IMMEDIATE, METAL,    CLR_BRIGHT_CYAN),
/*WAND("banishment",  "cocky",   5, 500, 1, IMMEDIATE, METAL,    CLR_BRIGHT_GREEN),*/
WAND("create horde",   "black",     5, 500, 1, NODIR,     PLASTIC,  CLR_BLACK),
WAND("extra healing",  "bronze",   30, 500, 1, IMMEDIATE, COPPER,   CLR_YELLOW),
WAND("full healing",   "hawthorn", 10, 500, 1, IMMEDIATE, LEATHER,  CLR_BROWN),
WAND("wonder",	"elven-lettered", 5, 500, 1, NODIR,     IRON,	 HI_METAL),
WAND("bugging",	"wooden",		5, 500, 1, NODIR,     WOOD,	 HI_METAL),
WAND("wishing",        "pine",      5, 500, 1, NODIR,     WOOD,     HI_WOOD),
WAND("acquirement",   "important",  5, 500, 1, NODIR,     SILVER,     CLR_CYAN),
WAND("clone monster",  "radiating", 5, 500, 1, IMMEDIATE, GOLD,    CLR_BRIGHT_GREEN),
WAND("charging", "buzzing",     5, 500, 1, NODIR,     METAL,     CLR_BRIGHT_CYAN),
/* Ray wands have to come last, and in this order. */
/* This is extremely kludgy, but that's what zap.c expects. */
WAND("digging",        "iron",     50, 500, 1, RAY,       IRON,     HI_METAL),
WAND("magic missile",  "steel",    50, 500, 1, RAY,       IRON,     HI_METAL),
WAND("fire",           "hexagonal",25, 500, 1, RAY,       IRON,     HI_METAL),
WAND("cold",           "short",    30, 500, 1, RAY,       IRON,     HI_METAL),
WAND("sleep",          "runed",    50, 500, 1, RAY,       IRON,     HI_METAL),
WAND("death",          "long",      5, 500, 1, RAY,       IRON,     HI_METAL),
WAND("lightning",      "curved",   20, 500, 1, RAY,       IRON,     HI_METAL),
WAND("fireball",       "octagonal", 5, 500, 1, RAY,       IRON,     HI_METAL),
WAND("acid",		"measured",	5, 500, 1, RAY,       METAL,	 HI_METAL),
WAND("solar beam",	"beaming",	5, 500, 1, RAY,       METAL,	 CLR_BRIGHT_CYAN),
WAND((char *)0,        "forked",    0, 500, 1, 0,         WOOD,     HI_WOOD),
WAND((char *)0,        "spiked",    0, 500, 1, 0,         IRON,     HI_METAL),
WAND((char *)0,        "jeweled",   0, 500, 1, 0,         IRON,     HI_MINERAL),
WAND((char *)0,        "porcelain", 0, 500, 1, 0,         GLASS,    HI_MINERAL),
WAND((char *)0,        "quartz",    0, 500, 1, 0,         GLASS,    HI_GLASS),
WAND((char *)0,        "metal",    0, 500, 1, 0,         COPPER,   HI_COPPER),
WAND((char *)0,        "decayed",     0, 500, 1, 0,         IRON,     CLR_RED),
WAND((char *)0,        "dodecagonal", 0, 500, 1, 0,         IRON,     HI_METAL),
WAND((char *)0,        "stoutwood",    0, 500, 1, 0,         WOOD,     CLR_YELLOW),
WAND((char *)0,        "walnut",    0, 500, 1, 0,         WOOD,     HI_WOOD),
WAND((char *)0,        "mahogany",  0, 500, 1, 0,         WOOD,     HI_WOOD),
WAND((char *)0,        "cedar",     0, 500, 1, 0,         WOOD,     HI_WOOD),
WAND((char *)0,        "chrome",    0, 500, 1, 0,         METAL,    HI_SILVER),
WAND((char *)0,        "titanium",  0, 500, 1, 0,         METAL,    HI_METAL),
WAND((char *)0,        "nickel",    0, 500, 1, 0,         METAL,    HI_METAL),
WAND((char *)0,        "mithril",   0, 500, 1, 0,         MITHRIL,  HI_SILVER),
WAND((char *)0,        "grooved",   0, 500, 1, 0,         WOOD,     HI_WOOD),
WAND((char *)0,        "bent",      0, 500, 1, 0,         METAL,    CLR_BRIGHT_GREEN),
WAND((char *)0,        "plastic",   0, 500, 1, 0,         PLASTIC,  CLR_BRIGHT_GREEN),
WAND((char *)0,        "bone",      0, 500, 1, 0,         BONE,     CLR_WHITE),
WAND((char *)0,        "alabaster", 0, 500, 1, 0,         MINERAL,  CLR_BROWN),
WAND((char *)0,        "orichalcum",0, 500, 1, 0,         METAL,    HI_METAL),
WAND((char *)0,        "electrum",  0, 500, 1, 0,         METAL,    HI_GOLD),
WAND((char *)0,        "conundrum",0, 500, 1, 0,         METAL,    CLR_BRIGHT_GREEN),
WAND((char *)0,        "lavender",0, 500, 1, 0,         METAL,    CLR_BRIGHT_MAGENTA),
WAND((char *)0,        "viridian",0, 500, 1, 0,         METAL,    CLR_GREEN),
WAND((char *)0,        "cerulean",0, 500, 1, 0,         METAL,    CLR_BRIGHT_BLUE),
WAND((char *)0,        "saffron",0, 500, 1, 0,         METAL,    CLR_ORANGE),
WAND((char *)0,        "fuchsia",0, 500, 1, 0,         METAL,    CLR_RED),
WAND((char *)0,        "pallet",0, 500, 1, 0,         METAL,    CLR_MAGENTA),
WAND((char *)0,        "cyan",0, 500, 1, 0,         METAL,    CLR_CYAN),
WAND((char *)0,        "rusting",0, 500, 1, 0,         METAL,    CLR_GRAY),
WAND((char *)0,        "ominous",0, 500, 1, 0,         METAL,    CLR_BLACK),
WAND((char *)0,        "golden",0, 500, 1, 0,         METAL,    CLR_YELLOW),
WAND((char *)0,        "iridescent",0, 500, 1, 0,         METAL,    CLR_BRIGHT_CYAN),
WAND((char *)0,        "tetragonal", 0, 500, 1, 0,         IRON,     CLR_BRIGHT_GREEN),
WAND((char *)0,        "trigonal",0, 500, 1, 0,         METAL,    CLR_BRIGHT_MAGENTA),
WAND((char *)0,        "cubic",0, 500, 1, 0,         METAL,    CLR_RED),
WAND((char *)0,        "orthorhombic",0, 500, 1, 0,         METAL,    CLR_BRIGHT_MAGENTA),
WAND((char *)0,        "skew",0, 500, 1, 0,         METAL,    CLR_BLACK),
WAND((char *)0,        "oblique",0, 500, 1, 0,         METAL,    CLR_BRIGHT_CYAN),
WAND((char *)0,        "monoclinic",0, 500, 1, 0,         METAL,    CLR_ORANGE),
WAND((char *)0,        "triclinic",0, 500, 1, 0,         METAL,    CLR_YELLOW),
WAND((char *)0,        "angular",0, 500, 1, 0,         METAL,    CLR_YELLOW),
WAND((char *)0,        "pentagonal",0, 500, 1, 0,         METAL,    CLR_ORANGE),
WAND((char *)0,        "circular",0, 500, 1, 0,         METAL,    CLR_BRIGHT_GREEN),
WAND((char *)0,        "round",0, 500, 1, 0,         METAL,    CLR_BLACK),
WAND((char *)0,        "shaped",0, 500, 1, 0,         METAL,    CLR_BRIGHT_MAGENTA),
WAND((char *)0,        "deformed",0, 500, 1, 0,         METAL,    CLR_GRAY),
WAND((char *)0,        "teaching",0, 500, 1, 0,         WOOD,    CLR_RED),
WAND((char *)0,        "archmage",0, 500, 1, 0,         MITHRIL,    CLR_BRIGHT_CYAN),
WAND((char *)0,        "avocadowood",0, 500, 1, 0,         WOOD,    CLR_GREEN),
WAND((char *)0,        "banyan",0, 500, 1, 0,         WOOD,    CLR_RED),
WAND((char *)0,        "birch",0, 500, 1, 0,         WOOD,    CLR_YELLOW),
WAND((char *)0,        "cherry",0, 500, 1, 0,         WOOD,    CLR_ORANGE),
WAND((char *)0,        "cypress",0, 500, 1, 0,         WOOD,    CLR_YELLOW),
WAND((char *)0,        "dogwood",0, 500, 1, 0,         WOOD,    CLR_WHITE),
WAND((char *)0,        "driftwood",0, 500, 1, 0,         WOOD,    CLR_BROWN),
WAND((char *)0,        "elm",0, 500, 1, 0,         WOOD,    CLR_GREEN),
WAND((char *)0,        "eucalyptus",0, 500, 1, 0,         WOOD,    CLR_BRIGHT_GREEN),
WAND((char *)0,        "fall",0, 500, 1, 0,         WOOD,    CLR_BROWN),
WAND((char *)0,        "hemlock",0, 500, 1, 0,         WOOD,    CLR_RED),
WAND((char *)0,        "holly",0, 500, 1, 0,         WOOD,    CLR_BRIGHT_MAGENTA),
WAND((char *)0,        "ironwood",0, 500, 1, 0,         WOOD,    CLR_CYAN),
WAND((char *)0,        "kukui wood",0, 500, 1, 0,         WOOD,    CLR_BLACK),
WAND((char *)0,        "manzanita",0, 500, 1, 0,         WOOD,    CLR_BROWN),
WAND((char *)0,        "persimmon wood",0, 500, 1, 0,         WOOD,    CLR_GREEN),
WAND((char *)0,        "pecan",0, 500, 1, 0,         WOOD,    CLR_YELLOW),
WAND((char *)0,        "wintry",0, 500, 1, 0,         WOOD,    CLR_BRIGHT_CYAN),
WAND((char *)0,        "poplar",0, 500, 1, 0,         WOOD,    CLR_WHITE),
WAND((char *)0,        "willow",0, 500, 1, 0,         WOOD,    CLR_WHITE),
WAND((char *)0,        "redwood",0, 500, 1, 0,         WOOD,    CLR_RED),
WAND((char *)0,        "rosewood",0, 500, 1, 0,         WOOD,    CLR_MAGENTA),
WAND((char *)0,        "spruce",0, 500, 1, 0,         WOOD,    CLR_GREEN),
WAND((char *)0,        "teak",0, 500, 1, 0,         WOOD,    CLR_BROWN),
WAND((char *)0,        "zebrawood",0, 500, 1, 0,         WOOD,    CLR_BLACK),
WAND((char *)0,        "beryllium",0, 500, 1, 0,         METAL,    CLR_BRIGHT_GREEN),
WAND((char *)0,        "lead",0, 500, 1, 0,         PLATINUM,    CLR_GRAY),
WAND((char *)0,        "magnesium",0, 500, 1, 0,         MINERAL,    CLR_WHITE),
WAND((char *)0,        "mercury",0, 500, 1, 0,         COPPER,    CLR_GRAY),
WAND((char *)0,        "pewter",0, 500, 1, 0,         IRON,    CLR_GRAY),
WAND((char *)0,        "silicon",0, 500, 1, 0,         MINERAL,    CLR_WHITE),
WAND((char *)0,        "titanium",0, 500, 1, 0,         METAL,    CLR_ORANGE),
WAND((char *)0,        "tungsten",0, 500, 1, 0,         METAL,    CLR_BLACK),

#undef WAND

/* coins ... - so far, gold is all there is */
#define COIN(name,prob,metal,worth) OBJECT( \
		OBJ(name,(char *)0), BITS(0,1,0,0,0,0,0,0,0,0,0,P_NONE,metal), 0, \
		COIN_CLASS, prob, 0, 1, worth, 0, 0, 0, 0, 0, HI_GOLD )
	COIN("gold piece",      1000, GOLD,1),
#undef COIN

/* gems ... - includes stones and rocks but not boulders */
#define GEM(name,desc,prob,wt,gval,nutr,mohs,glass,color) OBJECT( \
	    OBJ(name,desc), \
	    BITS(0,1,0,0,0,0,0,0,0,HARDGEM(mohs),0,-P_SLING,glass), 0, \
	    GEM_CLASS, prob, 0, 1, gval, 12, 12, 0, 0, nutr, color )
#define ROCK(name,desc,kn,prob,wt,gval,sdam,ldam,mgc,nutr,mohs,glass,color) OBJECT( \
	    OBJ(name,desc), \
	    BITS(kn,1,0,0,mgc,0,0,0,0,HARDGEM(mohs),0,-P_SLING,glass), 0, \
	    GEM_CLASS, prob, 0, wt, gval, sdam, ldam, 0, 0, nutr, color )
GEM("dilithium crystal", "white",      1,  0, 4500, 15,  5, GEMSTONE, CLR_WHITE),
GEM("diamond", "white",                2,  0, 4000, 15, 10, GEMSTONE, CLR_WHITE),
GEM("ruby", "red",                     3,  0, 3500, 15,  9, GEMSTONE, CLR_RED),
GEM("jacinth", "orange",               2,  0, 3250, 15,  9, GEMSTONE, CLR_ORANGE),
GEM("sapphire", "blue",                3,  0, 3000, 15,  9, GEMSTONE, CLR_BRIGHT_BLUE),
GEM("black opal", "black",             2,  0, 2500, 15,  8, GEMSTONE, CLR_BLACK),
GEM("emerald", "green",                3,  0, 2500, 15,  8, GEMSTONE, CLR_GREEN),
GEM("turquoise", "green",              4,  0, 2000, 15,  6, GEMSTONE, CLR_GREEN),
GEM("citrine", "yellow",               3,  0, 1500, 15,  6, GEMSTONE, CLR_YELLOW),
GEM("aquamarine", "green",             4,  0, 1500, 15,  8, GEMSTONE, CLR_GREEN),
GEM("amber", "yellowish brown",        5,  0, 1000, 15,  2, GEMSTONE, CLR_BROWN),
GEM("topaz", "yellowish brown",        5,  0,  900, 15,  8, GEMSTONE, CLR_BROWN),
GEM("jet", "black",                    4,  0,  850, 15,  7, GEMSTONE, CLR_BLACK),
GEM("opal", "white",                   6,  0,  800, 15,  6, GEMSTONE, CLR_WHITE),
GEM("chrysoberyl", "yellow",           5,  0,  700, 15,  5, GEMSTONE, CLR_YELLOW),
GEM("garnet", "red",                   6,  0,  700, 15,  7, GEMSTONE, CLR_RED),
GEM("amethyst", "violet",              6,  0,  600, 15,  7, GEMSTONE, CLR_MAGENTA),
GEM("jasper", "red",                   7,  0,  500, 15,  7, GEMSTONE, CLR_RED),
GEM("fluorite", "violet",              7,  0,  400, 15,  4, GEMSTONE, CLR_MAGENTA),
GEM("obsidian", "black",               5,  0,  200, 15,  6, GEMSTONE, CLR_BLACK),
GEM("agate", "orange",                 6,  0,  200, 15,  6, GEMSTONE, CLR_ORANGE),
GEM("jade", "green",                   5,  0,  300, 15,  6, GEMSTONE, CLR_GREEN),
GEM("worthless piece of white glass", "white",   56, 0, 0, 6, 5, GLASS, CLR_WHITE),
GEM("worthless piece of blue glass", "blue",     56, 0, 0, 6, 5, GLASS, CLR_BRIGHT_BLUE),
GEM("worthless piece of red glass", "red",       56, 0, 0, 6, 5, GLASS, CLR_RED),
GEM("worthless piece of yellowish brown glass", "yellowish brown",
												 56, 0, 0, 6, 5, GLASS, CLR_BROWN),
GEM("worthless piece of orange glass", "orange", 56, 0, 0, 6, 5, GLASS, CLR_ORANGE),
GEM("worthless piece of yellow glass", "yellow", 56, 0, 0, 6, 5, GLASS, CLR_YELLOW),
GEM("worthless piece of black glass",  "black",  56, 0, 0, 6, 5, GLASS, CLR_BLACK),
GEM("worthless piece of green glass", "green",   56, 0, 0, 6, 5, GLASS, CLR_GREEN),
GEM("worthless piece of violet glass", "violet", 56, 0, 0, 6, 5, GLASS, CLR_MAGENTA),

/* Placement note: there is a wishable subrange for   
 * "gray stones" in the o_ranges[] array in objnam.c  
 * that is currently everything between luckstones and flint (inclusive).
 */
ROCK("luckstone", "gray",	0,  6,  0, 60, 15, 15, 1, 10, 7, MINERAL, CLR_GRAY),
ROCK("healthstone", "gray",	0,  8,  0, 60, 15, 15, 1, 10, 7, MINERAL, CLR_GRAY),
ROCK("loadstone", "gray",	0, 10, 1200,  60, 40, 40, 1, 1200, 6, MINERAL, CLR_GRAY),
ROCK("touchstone", "gray",	0,  6,  0, 60, 15, 15, 1, 10, 6, MINERAL, CLR_GRAY),
ROCK("whetstone", "gray",	0,  2,  0, 60, 15, 15, 1, 10, 7, MINERAL, CLR_GRAY),
ROCK("flint", "gray",		0, 10,  0, 60, 20, 20, 0, 10, 7, MINERAL, CLR_GRAY),
ROCK("rock", (char *)0,		1,360,  0,  0, 10, 10, 0, 10, 7, MINERAL, CLR_GRAY),
#undef GEM
#undef ROCK

/* miscellaneous ... */
/* Note: boulders and rocks are not normally created at random; the
 * probabilities only come into effect when you try to polymorph them.
 * Boulders weigh more than MAX_CARR_CAP; statues use corpsenm to take
 * on a specific type and may act as containers (both affect weight).
 */
OBJECT(OBJ("boulder",(char *)0), BITS(1,0,0,0,0,0,0,0,1,0,0,P_NONE,MINERAL), 0,
		ROCK_CLASS,   100, 0, 6000,  0, 20, 20, 0, 0, 1000, HI_MINERAL),
OBJECT(OBJ("statue", (char *)0), BITS(1,0,0,1,0,0,0,0,0,0,0,P_NONE,MINERAL), 0,
		ROCK_CLASS,   900, 0, 2500,  0, 20, 20, 0, 0, 200, CLR_WHITE),

OBJECT(OBJ("heavy iron ball", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_FLAIL,IRON), 0,
		BALL_CLASS,  889, 0,  1200, 10, 25, 25, 0, 0,  200, HI_METAL),
						/* +d4 when "very heavy" */
OBJECT(OBJ("quite heavy iron ball", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_FLAIL,IRON), 0,
		BALL_CLASS,  100, 0,  1600, 10, 30, 30, 0, 0,  200, CLR_GREEN),
OBJECT(OBJ("really heavy iron ball", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_FLAIL,IRON), 0,
		BALL_CLASS,  10, 0,  2000, 10, 35, 35, 0, 0,  200, CLR_RED),
OBJECT(OBJ("extremely heavy iron ball", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_FLAIL,IRON), 0,
		BALL_CLASS,  1, 0,  2400, 10, 40, 40, 0, 0,  200, CLR_ORANGE),

OBJECT(OBJ("iron chain", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_FLAIL,IRON), 0,
		CHAIN_CLASS, 889, 0,  120,  0,  4,  4, 0, 0,  200, HI_METAL),
						/* +1 both l & s */
OBJECT(OBJ("rotating chain", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_FLAIL,IRON), 0,
		CHAIN_CLASS, 100, 0,  240,  0,  8,  8, 0, 0,  200, CLR_BRIGHT_BLUE),
OBJECT(OBJ("scourge", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_FLAIL,IRON), 0,
		CHAIN_CLASS, 10, 0,  360,  0,  15,  15, 0, 0,  200, CLR_BRIGHT_MAGENTA),
OBJECT(OBJ("nunchiaku", (char *)0), BITS(1,0,0,0,0,0,0,0,0,0,WHACK,P_FLAIL,IRON), 0,
/* spelled wrong on purpose, as the samurai has another item with that name --Amy */
		CHAIN_CLASS, 1, 0,  480,  0,  24,  24, 0, 0,  200, CLR_BRIGHT_GREEN),
/* extra balls and chains for convict mainly (he can advance flail skill by swinging them) --Amy */

OBJECT(OBJ("blinding venom", "splash of venom"),
		BITS(0,1,0,0,0,0,0,1,0,0,0,P_NONE,LIQUID), 0,
		VENOM_CLASS,  499, 0,	 1,  0,  0,  0, 0, 0,	 0, HI_ORGANIC),
OBJECT(OBJ("tail spikes", "splash of venom"),
		BITS(0,1,0,0,0,0,0,1,0,0,0,P_NONE,LIQUID), 0,
		VENOM_CLASS,  1, 0,	 1,  0,  6,  6, 0, 0,	 0, CLR_BLACK),
OBJECT(OBJ("faerie floss rhing", "splash of venom"),
		BITS(0,1,0,0,0,0,0,1,0,0,0,P_NONE,LIQUID), 0,
		VENOM_CLASS,  1, 0,	 1,  0,  0,  0, 0, 0,	 0, CLR_BRIGHT_MAGENTA),
OBJECT(OBJ("acid venom", "splash of venom"),
		BITS(0,1,0,0,0,0,0,1,0,0,0,P_NONE,LIQUID), 0,
		VENOM_CLASS,  499, 0,	 1,  0,  6,  6, 0, 0,	 0, HI_ORGANIC),
		/* +d6 small or large */

/* fencepost, the deadly Array Terminator -- name [1st arg] *must* be NULL */
	OBJECT(OBJ((char *)0,(char *)0), BITS(0,0,0,0,0,0,0,0,0,0,0,P_NONE,0), 0,
		ILLOBJ_CLASS, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
}; /* objects[] */

#ifndef OBJECTS_PASS_2_

/* perform recursive compilation for second structure */
#  undef OBJ
#  undef OBJECT
#  define OBJECTS_PASS_2_
#include "objects.c"

void NDECL(objects_init);

/* dummy routine used to force linkage */
void
objects_init()
{
    return;
}

#endif	/* !OBJECTS_PASS_2_ */

/*objects.c*/
