/*	SCCS Id: @(#)artilist.h 3.4	2003/02/12	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "config.h" /* WAC for blackmarket,  spoon */

#ifdef MAKEDEFS_C
/* in makedefs.c, all we care about is the list of names */

#define A(nam,typ,s1,s2,mt,atk,dfn,cry,inv,al,cl,rac,cost) nam

static const char *artifact_names[] = {
#else
/* in artifact.c, set up the actual artifact list structure */

#define A(nam,typ,s1,s2,mt,atk,dfn,cry,inv,al,cl,rac,cost) \
 { typ, nam, s1, s2, mt, atk, dfn, cry, inv, al, cl, rac, cost, 0 }

#define     NO_ATTK	{0,0,0,0}		/* no attack */
#define     NO_DFNS	{0,0,0,0}		/* no defense */
#define     NO_CARY	{0,0,0,0}		/* no carry effects */
#define     DFNS(c)	{0,c,0,0}
#define     CARY(c)	{0,c,0,0}
#define     PHYS(a,b)	{0,AD_PHYS,a,b}		/* physical */
#define     DRLI(a,b)	{0,AD_DRLI,a,b}		/* life drain */
#define     COLD(a,b)	{0,AD_COLD,a,b}
#define     FIRE(a,b)	{0,AD_FIRE,a,b}
#define     ELEC(a,b)	{0,AD_ELEC,a,b}		/* electrical shock */
#define     STUN(a,b)	{0,AD_STUN,a,b}		/* magical attack */
#define     ACID(a,b)	{0,AD_ACID,a,b} 	/* acidic attack */
#define     DRST(a,b)   {0,AD_DRST,a,b} 	/* poison attack */

STATIC_OVL NEARDATA struct artifact artilist[] = {
#endif	/* MAKEDEFS_C */

/* Artifact cost rationale:
 * 1.  The more useful the artifact, the better its cost.
 * 2.  Quest artifacts are highly valued.
 * 3.  Chaotic artifacts are inflated due to scarcity (and balance).
 */


/* [Tom] rearranged by alignment, so when people ask... */
/* KMH -- Further arranged:
 * 1.  By alignment (but quest artifacts last).
 * 2.  By weapon class (skill).
 */

/*  dummy element #0, so that all interesting indices are non-zero */
A("",				STRANGE_OBJECT,
	0, 0, 0, NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L ),

/*** Lawful artifacts ***/
A("Firewall",                  ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(4,4),      FIRE(0,0),      NO_CARY,        0, A_LAWFUL, PM_FLAME_MAGE, NON_PM, 400L ),

/*
 *	The combination of SPFX_WARN and M2_something on an artifact
 *	will trigger EWarn_of_mon for all monsters that have the appropriate
 *	M2_something flags.  In Sting's case it will trigger EWarn_of_mon
 *	for M2_ORC monsters.
 */
A("Sting",			ELVEN_DAGGER,
	(SPFX_WARN|SPFX_DCLAS), 0, M2_ORC,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, PM_ELF, 800L ),

A("Giantkiller",                AXE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_GIANT,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 800L ),

A("Quick Blade",                ELVEN_SHORT_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(9,2),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("Orcrist",                    ELVEN_BROADSWORD,
	SPFX_DFLAG2, 0, M2_ORC,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, PM_ELF, 2000L ),

A("Dragonbane",			BROADSWORD,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_DRAGON,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 500L ),

A("Excalibur",                  LONG_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_SEARCH),0,0,
	PHYS(5,10),	DRLI(0,0),	NO_CARY,	0, A_LAWFUL, PM_KNIGHT, NON_PM, 4000L ),

A("Clarent",			SHORT_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_LUCK|SPFX_DFLAG1), 0, M1_THICK_HIDE,
	PHYS(8,2),	NO_DFNS,	NO_CARY,	LEADERSHIP, A_LAWFUL, PM_KNIGHT, NON_PM, 4000L ),

#ifdef CONVICT
A("Shadowblade",			KNIFE,
	(SPFX_RESTR|SPFX_STLTH),0,0,
	PHYS(5,6),	NO_DFNS,	NO_CARY,	INVIS, A_CHAOTIC, PM_CONVICT, NON_PM, 3000L ),
#endif /* CONVICT */
A("Sunsword",                   LONG_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5,0),	DFNS(AD_BLND),	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1500L ),

/*
 *	Ah, never shall I forget the cry,
 *		or the shriek that shrieked he,
 *	As I gnashed my teeth, and from my sheath
 *		I drew my Snickersnee!
 *			--Koko, Lord high executioner of Titipu
 *			  (From Sir W.S. Gilbert's "The Mikado")
 */
A("Snickersnee",                KATANA,
	SPFX_RESTR, 0, 0,
	PHYS(0,8),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 1200L ),

A("Yoichi no yumi",                YUMI, /* 5lo: Artifact from dNethack */
	SPFX_RESTR, 0, 0,
	PHYS(20,0),	NO_DFNS,	NO_CARY,	CREATE_AMMO, A_LAWFUL, NON_PM, NON_PM, 1200L ),

A("Kiku-ichimonji",                KATANA, /* 5lo: New sacrifice gift for Samurai */
	SPFX_RESTR, 0, 0,
	PHYS(4,12),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_SAMURAI, NON_PM, 1200L ),

#ifdef YEOMAN
/* KMH -- Renamed from Holy Sword of Law (Stephen White)
 * This is an actual sword used in British coronations!
 */
A("Sword of Justice",           LONG_SWORD,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(5,12),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_YEOMAN, NON_PM, 1500L ),
#endif

A("Demonbane",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_DEMON,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 2500L ),

A("Werebane",			SILVER_SABER,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_WERE,
	PHYS(5,0),	DFNS(AD_WERE),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1500L ),

A("Grayswandir",		SILVER_SABER,
	(SPFX_RESTR|SPFX_HALRES), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 8000L ),

A("Skullcrusher",               CLUB,
	SPFX_RESTR, 0, 0,
	PHYS(3,10),     NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_CAVEMAN, NON_PM, 300L ),

A("Liontamer",                BULLWHIP, /* 5lo: was quest artifact */
	(SPFX_RESTR|SPFX_DCLAS), 0, S_FELINE,
	PHYS(5,8),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, NON_PM, NON_PM, 800L ), // was for zookeepers

A("Trollsbane",                 MORNING_STAR,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_TROLL,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

A("Ogresmasher",		WAR_HAMMER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_OGRE,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 200L ),

A("Reaper",                     HALBERD,
	SPFX_RESTR, 0, 0,
	PHYS(5,20),      NO_DFNS,        NO_CARY,        0, A_LAWFUL, PM_YEOMAN, NON_PM, 1000L ),

A("Holy Spear of Light",        SILVER_SPEAR,
       (SPFX_RESTR|SPFX_INTEL|SPFX_DFLAG2), 0, M2_UNDEAD,
       PHYS(5,10),      NO_DFNS,  NO_CARY,     LIGHT_AREA, A_LAWFUL, PM_UNDEAD_SLAYER, NON_PM, 4000L ),

A("Rod of Lordly Might", MACE, /*needs quote*/
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(3,0),	NO_DFNS,	NO_CARY,
	LORDLY,	A_LAWFUL, PM_NOBLEMAN, NON_PM, 4000L ),

A("Itlachiayaque",             SHIELD_OF_REFLECTION,
	(SPFX_RESTR), (SPFX_HSPDAM), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_FIRE),  SMOKE_CLOUD,    A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, 2000L ),

A("Queen's Guard",		RAPIER,
	(SPFX_RESTR), 0, 0,
	PHYS(6,6),	NO_DFNS,	NO_CARY, 0, A_LAWFUL, PM_CORSAIR, NON_PM, 1500L ),

#ifdef FIREARMS
A("Peacekeeper",		PISTOL,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(4,9),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_OFFICER, NON_PM, 1500L ),
#else
A("Peacekeeper",		CLUB,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(4,9),	NO_DFNS,	NO_CARY,	0, A_LAWFUL, PM_OFFICER, NON_PM, 1500L ),
#endif

A("Frostbite",			BATTLE_AXE,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(6,6),	COLD(0,0),	NO_CARY,	0, A_LAWFUL, PM_FIREFIGHTER, NON_PM, 2000L ),

A("Silverstar", 		SILVER_MACE, 
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_REFLECT), 	0, 	0,
	PHYS(4,10),	DRLI(0,0),	NO_CARY,	0, A_LAWFUL, PM_PALADIN, NON_PM, 1500L ),

A("Vampire Killer",        LONG_BULLWHIP,
	(SPFX_RESTR|SPFX_DFLAG2), 0, (M2_UNDEAD|M2_DEMON|M2_WERE),
	PHYS(10,20),    DRLI(0,0),        NO_CARY,
	BLESS,              A_LAWFUL, NON_PM, NON_PM, 2500L ),

/*** Neutral artifacts ***/
A("Magicbane",                  ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	STUN(3,4),	DFNS(AD_MAGM),	NO_CARY,	0, A_NEUTRAL, PM_WIZARD, NON_PM, 3500L ),

A("Luckblade",                  SHORT_SWORD,
	(SPFX_RESTR|SPFX_LUCK), 0, 0,
	PHYS(5,5),      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, NON_PM, PM_GNOME, 1000L ),

A("Sword of Balance",           SILVER_SHORT_SWORD,
	(SPFX_RESTR|SPFX_DALIGN), 0, 0,
	PHYS(2,5),      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Frost Brand",                LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,0),	COLD(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Fire Brand",                 LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	FIRE(5,0),	FIRE(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Shock Brand",                LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ELEC(5,0),	ELEC(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),

A("Acid Brand",                 LONG_SWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	ACID(5,0),	ACID(0,0),	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 3000L ),


/*
 *	Two problems:  1) doesn't let trolls regenerate heads,
 *	2) doesn't give unusual message for 2-headed monsters (but
 *	allowing those at all causes more problems than worth the effort).
 */
A("Vorpal Blade",		LONG_SWORD,
	(SPFX_RESTR|SPFX_BEHEAD), 0, 0,
	PHYS(5,1),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 4000L ),

A("Disrupter",                  MACE,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_UNDEAD,
	PHYS(5,30),     NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_PRIEST, NON_PM, 500L ),

/*
 *	Mjollnir will return to the hand of a Valkyrie when thrown
 *	if the wielder is a Valkyrie with strength of 25 or more.
 */
A("Mjollnir",                   HEAVY_HAMMER,           /* Mjo:llnir */
	(SPFX_RESTR|SPFX_ATTK),  0, 0,
	ELEC(5,24),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_VALKYRIE, NON_PM, 4000L ),

/* STEPHEN WHITE'S NEW CODE */
A("Gauntlets of Defense",    GAUNTLETS_OF_DEXTERITY,
	SPFX_RESTR, SPFX_HPHDAM, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,    INVIS, A_NEUTRAL, PM_MONK, NON_PM, 5000L ),

A("Mirrorbright",               SHIELD_OF_REFLECTION,
	(SPFX_RESTR|SPFX_HALRES|SPFX_REFLECT), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_HEALER, NON_PM, 5000L ),

A("Deluder",               CLOAK_OF_DISPLACEMENT,
	(SPFX_RESTR|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_JEDI, NON_PM, 5000L ),
/* 5lo: Generic artifact since Psions are deferred for now */
A("Snakeskin",             ROBE,
    (SPFX_RESTR|SPFX_HALRES|SPFX_PROTEC), 0, 0,
    NO_ATTK,    ACID(0,0),   NO_CARY,    0, A_NEUTRAL, 0, NON_PM, 700L ),

A("Poseidon's Treasure",	TRIDENT,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(10, 10),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_DIVER, NON_PM, 2000L ),

A("Gladius",		SHORT_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(8,7),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_GLADIATOR, NON_PM, 1500L ),

A("Hrunting",		LONG_SWORD,
	(SPFX_RESTR), 0, 0,
	PHYS(4,5),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_WARRIOR, NON_PM, 1000L ),

A("Debugger",       ATHAME,
    (SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
    ELEC(5,5),  ELEC(0,0), NO_CARY,     0, A_NEUTRAL, PM_ELECTRIC_MAGE, NON_PM, 400L ),
    
A("Acidfall",       ATHAME,
    (SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
    ACID(5,5),  ACID(0,0), NO_CARY,     0, A_NEUTRAL, PM_ACID_MAGE, NON_PM, 400L ),

A("Straightshot",   BOW,
    SPFX_RESTR, 0, 0,
    PHYS(10,3),     NO_DFNS,    NO_CARY,    0, A_NEUTRAL, PM_RANGER, NON_PM, 1000L ),
    
A("Shimmerstaff",   SILVER_CAPPED_STAFF,
    SPFX_RESTR, 0, 0,
    PHYS(8,5),     NO_DFNS,    NO_CARY,    0, A_NEUTRAL, PM_BARD, NON_PM, 1000L ),

A("Oathkeeper", 		STILETTO, /* temp name? */
    (SPFX_RESTR), 0, 0,
    PHYS(7,9),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_LOCKSMITH, NON_PM, 1500L ),

A("Blackshroud",		CLOAK_OF_PROTECTION,
    (SPFX_RESTR|SPFX_LUCK),	SPFX_WARN,	0,
    NO_ATTK,	DRLI(0,0),	NO_CARY,	0,	A_NEUTRAL,	PM_UNDERTAKER,	NON_PM, 1500L ),

A("Silvershard", SILVER_DAGGER,
    (SPFX_RESTR), 0, 0,
    PHYS(2,0),	NO_DFNS,	NO_CARY, 0, A_NONE, NON_PM, NON_PM, 1000L ),

A("Mirror Brand",                 SILVER_LONG_SWORD, /* dNethack */
	(SPFX_RESTR|SPFX_ATTK|SPFX_REFLECT|SPFX_DALIGN), 0, 0,
	STUN(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 3500L ),

A("David's Sling",                SLING,
	(SPFX_RESTR|SPFX_ATTK|SPFX_HPHDAM), 0, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("Tesla's Coat", LAB_COAT,
    (SPFX_RESTR|SPFX_DEFN), 0, 0,
    NO_ATTK,    ELEC(0,0),  NO_CARY,    0,  A_NEUTRAL, PM_GEEK, NON_PM, 2000L ),

A("Musa-Shiya", HAWAIIAN_SHIRT, 	/* Musa-Shiya the Shirtmaker */
	(SPFX_RESTR|SPFX_HALRES|SPFX_PROTEC), 0, 0,
	NO_ATTK, 	NO_DFNS, 	NO_CARY, 	
	ENLIGHTENING, 	A_NEUTRAL, PM_MUSICIAN, NON_PM, 1500L ),

A("Dirk", 		DAGGER, 
	SPFX_RESTR, 	0, 	0,
	PHYS(5,5),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1500L ),



A("Tenderizer",                   MALLET,           
	(SPFX_RESTR|SPFX_ATTK),  0, 0,
	PHYS(3,7),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_CHEF, NON_PM, 4000L ),

/* 5lo: Some more artifacts... */
A("Fungisword", LONG_SWORD,
    (SPFX_RESTR|SPFX_DCLAS), SPFX_HALRES, S_FUNGUS,
    PHYS(10,0), NO_DFNS, NO_CARY, 0, A_LAWFUL, NON_PM, NON_PM, 400L ),
    
A("Diplomat", ORNATE_MACE,
    (SPFX_RESTR|SPFX_DFLAG2), 0, M2_PNAME,
    PHYS(10,25), NO_DFNS, NO_CARY, 0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),
    
A("Petslayer", CLUB,
    (SPFX_RESTR|SPFX_DFLAG2), 0, M2_DOMESTIC,
    PHYS(5,10), NO_DFNS, NO_CARY, 0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

#ifdef NEWHON_ARTIFACTS
A("Mouser's Scalpel",			RAPIER,
	(SPFX_RESTR), 0, 0,
	PHYS(5,1),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 600L ),
    
A("Graywand",		TWO_HANDED_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	COLD(3,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),
    
A("Heartseeker",		SHORT_SWORD,
	(SPFX_RESTR),	0, 0,
	PHYS(3,3),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("Cat's Claw",		DAGGER,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_RODENT,
	PHYS(5,7),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, NON_PM, NON_PM, 1000L ),
#endif /* NEWHON_ARTIFACTS */

#ifdef TOURIST
A("Whisperfeet",               SPEED_BOOTS,
	(SPFX_RESTR|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, PM_TOURIST, NON_PM, 5000L ),
#else
A("Whisperfeet",               SPEED_BOOTS,
	(SPFX_RESTR|SPFX_STLTH|SPFX_LUCK), 0, 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),
#endif

A("Bugzapper",			FLY_SWATTER,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(3,10),	NO_DFNS,	NO_CARY,
	0, A_NEUTRAL, NON_PM, NON_PM, 2000L ),

A("The Beastmaster's Duster",		LEATHER_JACKET,
	(SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	PETMASTER,  A_NEUTRAL, PM_ZOOKEEPER, NON_PM, 9000L ),

/*** Chaotic artifacts ***/
A("Grimtooth",                  ORCISH_DAGGER,
	SPFX_RESTR, 0, 0,
	PHYS(2,6),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, PM_ORC, 300L ),

A("Deep Freeze",                ATHAME,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN), 0, 0,
	COLD(5,5),      COLD(0,0),      NO_CARY,        0, A_CHAOTIC, PM_ICE_MAGE, NON_PM, 400L ),

A("Serpent's Tongue",            DAGGER,
	SPFX_RESTR, 0, 0,
        PHYS(2,0),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_NECROMANCER, NON_PM, 400L ),
	/* See artifact.c for special poison damage */

A("The Marauder's Map", SCR_MAGIC_MAPPING,
	(SPFX_RESTR), 0, 0, 
	NO_ATTK,	NO_DFNS,	NO_CARY,        OBJECT_DET, A_CHAOTIC, PM_PIRATE, NON_PM, 2000L ),

A("Cleaver",                    BATTLE_AXE,
	SPFX_RESTR, 0, 0,
	PHYS(3,6),	NO_DFNS,	NO_CARY,	0, A_NEUTRAL, PM_BARBARIAN, NON_PM, 1500L ),

A("Doomblade",                  ORCISH_SHORT_SWORD,
	SPFX_RESTR, 0, 0,
	PHYS(0,10),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_LUNATIC, NON_PM, 1000L ),

A("Nightingale",                KATANA,
	SPFX_RESTR, 0, 0,
	PHYS(6,3),	NO_DFNS,	NO_CARY,	INVIS, A_CHAOTIC, PM_NINJA, NON_PM, 2000L ),
	/* 5lo: Sacrifice gift for Ninja */

/*
 * The Knight needed a chaotic logsword to obtain, since Excalibur
 * will be restricted for them...
 * 5lo: Dirge turned into chaotic counterpart to Clarent, Dark Knights get
 * Soulthief from Nethack-- instead
 */
A("Dirge",                     SHORT_SWORD,
       (SPFX_ATTK|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL),0,0,
       DRST(5,0),      DRST(0,0),      NO_CARY,        0, A_CHAOTIC, PM_KNIGHT, NON_PM, 4000L ),

A("Soulthief",                     LONG_SWORD,
       (SPFX_ATTK|SPFX_RESTR|SPFX_DEFN|SPFX_INTEL|SPFX_DRLI),0,0,
       DRLI(5,6),      DRLI(0,0),      NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),


/*
 *	Stormbringer only has a 2 because it can drain a level,
 *	providing 8 more.
 */
A("Stormbringer",               RUNESWORD,
	(SPFX_RESTR|SPFX_ATTK|SPFX_DEFN|SPFX_INTEL|SPFX_DRLI), 0, 0,
	DRLI(5,2),	DRLI(0,0),	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 8000L ),

A("Reaver",			SCIMITAR,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),0,0,
	PHYS(5,8),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, PM_PIRATE, NON_PM, 6000L ),

#ifdef BLACKMARKET
A("Thiefbane",                  LONG_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_BEHEAD|SPFX_DCLAS|SPFX_DRLI), 0, S_HUMAN,
	DRLI(5,1),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),
#endif /* BLACKMARKET */

A("Deathsword",                   TWO_HANDED_SWORD,
	(SPFX_RESTR|SPFX_DFLAG2), 0, M2_HUMAN,
	PHYS(5,14),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 5000L ),

A("Bat from Hell",                BASEBALL_BAT,
	(SPFX_RESTR), 0, 0,
	PHYS(3,20),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_ROGUE, NON_PM, 5000L ),

A("Elfrist",                    ORCISH_SPEAR,
	SPFX_DFLAG2, 0, M2_ELF,
	PHYS(5,15),     NO_DFNS,        NO_CARY,        0, A_CHAOTIC, PM_HUMAN_WEREWOLF, PM_ORC, 300L ),

A("Slow Blade",                ORCISH_SHORT_SWORD, /* 5lo: Now opposite of Quickblade... */
	SPFX_RESTR, 0, 0,
	PHYS(2,9),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 1000L ),

A("Thanatos",                     GRAIN_SCYTHE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK), 0, 0,
	DRLI(10,10),      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 2000L ),

A("Spineseeker",	STILETTO,
	SPFX_RESTR, 0, 0,
	PHYS(5,5),	NO_DFNS,	NO_CARY,	0, A_CHAOTIC, NON_PM, NON_PM, 1500L ),

A("The Pickpocket's Gloves",	GAUNTLETS_OF_DEXTERITY,
	(SPFX_RESTR), (SPFX_STLTH|SPFX_SEARCH), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_CHAOTIC,	NON_PM, NON_PM, 1000L ),

A("Plague", DARK_ELVEN_BOW,	/* KMH */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,7),        DFNS(AD_DRST),  NO_CARY,        0, A_CHAOTIC, PM_DROW, NON_PM, 6000L ),
	/* Auto-poison code in dothrow.c */

A("Mumakbane",			LONG_SWORD,
	(SPFX_RESTR|SPFX_DMONS), 0, PM_MUMAK,
	FIRE(5,60),	NO_DFNS,	NO_CARY,
	0, 		A_NEUTRAL,	NON_PM, NON_PM, 500L ),

A("Wormbiter",			CRYSKNIFE,
	(SPFX_RESTR|SPFX_DCLAS), 0, S_WORM,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 2000L ),

A("Shocker",			ELECTRIC_SWORD,
	(SPFX_RESTR|SPFX_ATTK), 0, 0,
	ELEC(3,0),	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	PM_HACKER, NON_PM, 1000L ),

A("Scales of the Dragon Lord",	GRAY_DRAGON_SCALE_MAIL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_PROTEC), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	DRAGON_BREATH,	A_CHAOTIC,	NON_PM, NON_PM, 5000L ),

A("The Burned Moth Relay",	RELAY,
	(SPFX_RESTR|SPFX_NOGEN), SPFX_PROTEC, 0, 	
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,		A_NEUTRAL,	NON_PM, NON_PM, 200L ),

A("The Key Of Access",		SKELETON_KEY,
	(SPFX_RESTR|SPFX_NOGEN),	0,		0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	CREATE_PORTAL,	A_NONE,		NON_PM, NON_PM, 200L ),

A("Crown Royal Cloak",	LEATHER_CLOAK,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_LUCK), 0, 0,
	NO_ATTK,	ACID(0,0),	NO_CARY,
	0,	A_NEUTRAL,	 PM_DRUNK, NON_PM, 1000L),

A("Hellfire", CROSSBOW,	/* WAC */
	(SPFX_RESTR|SPFX_DEFN), 0, 0,
	PHYS(5,7),        FIRE(0,0),  NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 4000L ),
	/* Auto-explode code in dothrow.c, uhitm.c */

A("The Gambler's Suit",		EXPENSIVE_SUIT,
	(SPFX_RESTR|SPFX_PROTEC|SPFX_LUCK), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,	0, A_NONE, NON_PM, NON_PM, 1000L ),

A("The Wand of Might",		WAN_WISHING,
	(SPFX_RESTR|SPFX_NOGEN), 0, 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,	A_NONE, NON_PM, NON_PM, 3000L ),

#ifdef P_SPOON
A("Houchou",                SPOON,
	(SPFX_RESTR), 0, 0,
        NO_ATTK,      NO_DFNS,        NO_CARY,        0, A_CHAOTIC, NON_PM, NON_PM, 50000L ),
#endif

/*** Special Artifacts ***/

#if 0 /* 5lo: Deferred, to be used as racial crowning gifts */
A("The Medallion of Shifters",  AMULET_OF_ESP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	PROT_POLY,      A_NEUTRAL, PM_DOPPELGANGER, NON_PM, 50000L ),

A("The Arkenstone",       DIAMOND,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK), 0, 0,
	NO_ATTK,      NO_DFNS,    CARY(AD_MAGM),
	OBJ_DETECTION,            A_LAWFUL, NON_PM, PM_DWARF, 50000L),

A("The Palantir of Westernesse",        CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_ESP|SPFX_REGEN|SPFX_HSPDAM), 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	TAMING,		A_CHAOTIC, NON_PM , PM_ELF, 50000L),

A("The Tentacle Staff",  QUARTERSTAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_ATTK|SPFX_DRLI), (SPFX_EREGEN|SPFX_HSPDAM), 0,
	DRLI(8,5),        NO_DFNS,        CARY(AD_MAGM),
	LEV_TELE,         A_CHAOTIC, NON_PM, PM_DROW, 50000L),

A("The Pick of Flandal Steelskin",       PICK_AXE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DALIGN), (SPFX_WARN), 0,
	FIRE(5,20),     NO_DFNS,        CARY(AD_FIRE),
	CONFLICT,       A_NEUTRAL, NON_PM, PM_GNOME, 50000L),

A("The One Ring", RIN_INVISIBILITY,
    (SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HSPDAM|SPFX_WARN), 0,
    NO_ATTK,        NO_DFNS,        CARY(AD_DRLI),
	CONFLICT,       A_LAWFUL,       NON_PM, PM_HOBBIT, 50000L),

#endif

# ifdef WALLET_O_P
/* KMH -- made it a bag of holding */
A("Wallet of Perseus",       BAG_OF_HOLDING,
        (SPFX_RESTR), 0, 0,
        NO_ATTK,        NO_DFNS,        NO_CARY,
        0,      A_NONE, NON_PM, NON_PM, 10000L ),
# endif /* WALLET_O_P */

A("Warforger",  HEAVY_HAMMER,   /* giant blacksmithing hammer */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK),  0, 0,
	PHYS(15,15),	FIRE(0,0),	NO_CARY, /* phys so fire resistance doesn't negate */
    0, A_NEUTRAL, NON_PM, NON_PM, 5000L ),

A("Nighthorn", UNICORN_HORN,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_REFLECT), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_LAWFUL, NON_PM, NON_PM, 10000L ),

A("The Key of Law", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_LAWFUL, NON_PM, NON_PM, 1000L ),

A("The Eye of the Beholder", EYEBALL,
	(SPFX_NOGEN|SPFX_RESTR), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	DEATH_GAZE,     A_NEUTRAL, NON_PM, NON_PM, 500L ),

A("The Key of Neutrality", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_NEUTRAL, NON_PM, NON_PM, 1000L ),

A("The Hand of Vecna",       SEVERED_HAND,
	(SPFX_NOGEN|SPFX_RESTR), (SPFX_REGEN|SPFX_HPHDAM), 0,
	NO_ATTK,        DRLI(0,0),      CARY(AD_COLD),
	SUMMON_UNDEAD,          A_CHAOTIC, NON_PM, NON_PM, 700L ),

A("The Key of Chaos", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,      A_CHAOTIC, NON_PM, NON_PM, 1000L ),


/*** The artifacts for the quest dungeon, all self-willed ***/

A("The Orb of Detection",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	INVIS,		A_LAWFUL, PM_ARCHEOLOGIST, NON_PM, 2500L ),

A("The Heart of Ahriman",	LUCKSTONE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_STLTH, 0,
	/* this stone does double damage if used as a projectile weapon */
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	LEVITATION,	A_NEUTRAL, PM_BARBARIAN, NON_PM, 2500L ),

A("The Sceptre of Might",	MACE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DALIGN), 0, 0,
	PHYS(3,5),      NO_DFNS,        CARY(AD_MAGM),
	CONFLICT,	A_LAWFUL, PM_CAVEMAN, NON_PM, 2500L ),

#ifdef CONVICT
A("The Iron Ball of Liberation", HEAVY_IRON_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_STLTH|SPFX_SEARCH|SPFX_WARN), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	PHASING,	A_NEUTRAL, PM_PRISONER, NON_PM, 5000L ),
#endif	/* CONVICT */

#if 0	/* OBSOLETE */
A("The Palantir of Westernesse",	CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL),
		(SPFX_ESP|SPFX_REGEN|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	TAMING,		A_CHAOTIC, NON_PM , PM_ELF, 8000L ),
#endif

/* STEPHEN WHITE'S NEW CODE */


A("The Golden Whistle of Nora",          MAGIC_WHISTLE, /* 5lo: temp name? */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_HPHDAM|SPFX_ESP), 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	TAMING,         A_LAWFUL, PM_ZOOKEEPER, NON_PM, 1000L ),

A("Fuma-itto no Ken",                BROADSWORD,    /* 5lo: New quest artifact for Ninja - Credit to Chris as this comes from dNethack with minor changes */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DALIGN), 0, 0, /* This artifact has special privilage to not go by "the X of (the) Y" format */
	PHYS(8,8),      NO_DFNS,     CARY(AD_DRLI),        CREATE_AMMO, A_CHAOTIC, PM_NINJA, NON_PM, 5000L ),

A("The Pick of the Grave",		PICK_AXE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_DRLI|SPFX_DEFN), (SPFX_REGEN|SPFX_HPHDAM|SPFX_TCTRL), 0,
	DRLI(8,10),	COLD(0,0),   NO_CARY,
	BLESS,		A_NEUTRAL, PM_UNDERTAKER, NON_PM, 3000L ),

A("The Flute of Slime",          MAGIC_FLUTE, /* 5lo: temp name */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_TCTRL), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_ELEC),
	SUMMON_EARTH_ELEMENTAL,         A_CHAOTIC, PM_ACID_MAGE, NON_PM, 1000L ),
/* Earth is associated with Acid in D&D */

A("The Fire Chief Helmet",                FIRE_HELMET,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_WARN|SPFX_PROTEC), (SPFX_HSPDAM|SPFX_HPHDAM), 0,
	NO_ATTK,      NO_DFNS,        NO_CARY,
	SUMMON_WATER_ELEMENTAL, A_LAWFUL, PM_FIREFIGHTER, NON_PM, 2000L ),

A("The Candle of Eternal Flame",        MAGIC_CANDLE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_TCTRL), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_COLD),
	SUMMON_FIRE_ELEMENTAL,         A_NEUTRAL, PM_FLAME_MAGE, NON_PM, 50000L ),

A("The Nethack Sources",	PACK_OF_FLOPPIES,
	(SPFX_NOGEN | SPFX_RESTR | SPFX_INTEL),
		 (SPFX_SEARCH | SPFX_ESP | SPFX_REGEN), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	IDENTIFY,	A_NEUTRAL,	PM_HACKER, NON_PM, 5000L ),

A("Master Boot Disk",	PACK_OF_FLOPPIES,
	(SPFX_NOGEN | SPFX_RESTR | SPFX_INTEL),
		 (SPFX_REFLECT), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	PHASING,	A_NEUTRAL,	PM_GEEK, NON_PM, 5000L ),

A("The Lyre of Orpheus",	WOODEN_HARP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK), 0, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	TAMING,		A_NEUTRAL, PM_BARD, NON_PM, 5000L ),

A("The Deluxe Yendorian Knife", KNIFE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), SPFX_ESP, 0,
	PHYS(6,20),  	FIRE(0,0), 	CARY(AD_ACID),
	SHARPEN,  A_NEUTRAL, PM_CHEF, NON_PM, 4000L ),

A("The Staff of Aesculapius",	QUARTERSTAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_DRLI|SPFX_REGEN), 0,0,
	DRLI(3,0),      DRLI(0,0),        NO_CARY,
	HEALING,	A_NEUTRAL, PM_HEALER, NON_PM, 5000L ),
/* STEPHEN WHITE'S NEW CODE */
A("The Storm Whistle",          MAGIC_WHISTLE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_TCTRL), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_FIRE),
	SUMMON_WATER_ELEMENTAL,         A_LAWFUL, PM_ICE_MAGE, NON_PM, 1000L ),

A("The Harp of Lightning",          MAGIC_HARP, 
 	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_WARN|SPFX_TCTRL), 0,
 	NO_ATTK,        NO_DFNS,        CARY(AD_ACID),
	SUMMON_AIR_ELEMENTAL,         A_NEUTRAL, PM_ELECTRIC_MAGE, NON_PM, 1000L ),
/* Air is associated with Electricity in D&D */

A("The Harp of Harmony",          MAGIC_HARP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK), (SPFX_WARN|SPFX_STLTH), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_DRLI),
	TAMING,		A_LAWFUL, PM_MUSICIAN, NON_PM, 5000L ),

A("The Magic Mirror of Merlin", MIRROR,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK|SPFX_REFLECT), (SPFX_REFLECT|SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	0,		A_LAWFUL, PM_KNIGHT, NON_PM, 1500L ),

A("The Cudgel of Cuthbert", QUARTERSTAFF, /* 5lo: from Nethack-- 3.1.3 */
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HALRES|SPFX_REGEN|SPFX_DALIGN),
	     (SPFX_WARN|SPFX_HSPDAM), 0,
	PHYS(5,0),	NO_DFNS,	CARY(AD_DRLI),
	BLESS,		A_LAWFUL, PM_PALADIN, NON_PM, 5000L ),

#ifdef NWAR
A("The Sword of Svyatogor", TWO_HANDED_SWORD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN), SPFX_HPHDAM, 0,
	PHYS(7,8),	COLD(0,0),	CARY(AD_MAGM),
	LEVITATION,	A_LAWFUL, PM_WARRIOR, NON_PM, 5000L ),
#endif

A("The Whistle of the Warden", MAGIC_WHISTLE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_TCTRL), 0,
	NO_ATTK,        NO_DFNS,  DRLI(0,0),
	LEADERSHIP, 	A_LAWFUL, PM_OFFICER, NON_PM, 3000L ),

#if 0 /* Deferred for now */
A("Gauntlets of Illusion",    GAUNTLETS_OF_DEXTERITY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), SPFX_HALRES, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,    INVIS, A_NEUTRAL, PM_POKEMON, NON_PM, 5000L ),

A("The Mantle of Knowledge",    HELM_OF_BRILLIANCE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HPHDAM|SPFX_ESP), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	ENERGY_BOOST, A_NEUTRAL, PM_PSION, NON_PM, 5000L ),
#endif
A("The Eyes of the Overworld",	LENSES,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_XRAY), 0, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	ENLIGHTENING,	A_NEUTRAL,	 PM_MONK, NON_PM, 2500L ),

A("The Mantle of Heaven",	LEATHER_CLOAK, /*Actually an ornamental cope in game*/
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM), 0, 0,
	NO_ATTK,	ELEC(0,0),	COLD(0,0), /* Plus double AC bonus */
	0,	A_LAWFUL,	 PM_NOBLEMAN, NON_PM, 2500L),

A("The Vestment of Hell",	LEATHER_CLOAK, /*Actually an opera cloak in game*/
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM), 0, 0,
	NO_ATTK,	ACID(0,0),	FIRE(0,0), /* Plus double AC bonus */
	0,	A_CHAOTIC,	 PM_NOBLEMAN, NON_PM, 2500L),

A("The Gourd of Infinity", HORN_OF_PLENTY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_ESP|SPFX_HSPDAM|SPFX_HALRES), 0,
	NO_ATTK,    NO_DFNS,    DRLI(0,0),
	CHARGE_OBJ, A_NEUTRAL,  PM_DRUNK,   NON_PM, 2500L),

A("The Great Dagger of Glaurgnaa",       GREAT_DAGGER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_DRLI|SPFX_DALIGN), 0, 0,
	DRLI(8,4),      NO_DFNS,        CARY(AD_MAGM),
	ENERGY_BOOST,   A_CHAOTIC, PM_NECROMANCER, NON_PM, 50000L ),

A("The Mitre of Holiness",	HELM_OF_BRILLIANCE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_DCLAS|SPFX_INTEL), 0, M2_UNDEAD,
	NO_ATTK,	NO_DFNS,	CARY(AD_FIRE),
	ENERGY_BOOST,	A_LAWFUL, PM_PRIEST, NON_PM, 2000L ),

A("The Treasury of Proteus",	CHEST,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK), 0, 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	0,	A_CHAOTIC,	 PM_PIRATE, NON_PM, 2500L ),

A("The Imperial Token", RIN_AGGRAVATE_MONSTER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REGEN|SPFX_DEFN), 0, 0,
	NO_ATTK,       FIRE(0,0),        CARY(AD_DRLI),
	LEADERSHIP,  A_NEUTRAL, PM_GLADIATOR, NON_PM, 2000L ), /* TODO: Better invoke? */

A("The Pearl of Wisdom", RIN_GAIN_WISDOM,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HPHDAM|SPFX_DEFN), 0, 0,
	NO_ATTK,       ELEC(0,0),        CARY(AD_MAGM),
	MAP_LEVEL,  A_NEUTRAL, PM_CORSAIR, NON_PM, 2000L ),

A("Maui's Fishhook",	GRAPPLING_HOOK,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_HSPDAM|SPFX_WARN|SPFX_SEARCH|SPFX_DCLAS), 0, S_EEL,
	PHYS(10,0),        NO_DFNS,        NO_CARY,
	0,		A_CHAOTIC, PM_DIVER, NON_PM, 4500L ),

A("The Longbow of Diana", BOW,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT), SPFX_ESP, 0,
	PHYS(5,0),	NO_DFNS,	NO_CARY,
	CREATE_AMMO, A_CHAOTIC, PM_RANGER, NON_PM, 4000L ),

A("The Master Key of Thievery", SKELETON_KEY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK),
		(SPFX_WARN|SPFX_TCTRL|SPFX_HPHDAM), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	UNTRAP,		A_CHAOTIC, PM_ROGUE, NON_PM, 3500L ),

A("The Lockpick of Arsene Lupin", LOCK_PICK,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_SPEAK|SPFX_LUCK),
		(SPFX_SEARCH|SPFX_ESP|SPFX_STLTH|SPFX_WARN), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	UNTRAP,		A_NEUTRAL, PM_LOCKSMITH, NON_PM, 3500L ),

A("The Tsurugi of Muramasa",	TSURUGI,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_BEHEAD|SPFX_LUCK), 0, 0,
	NO_ATTK,        NO_DFNS,        NO_CARY,
	0,		A_LAWFUL, PM_SAMURAI, NON_PM, 4500L ),

# ifdef TOURIST
A("The Platinum Yendorian Express Card", CREDIT_CARD,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_DEFN),
		(SPFX_ESP|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	CHARGE_OBJ,	A_NEUTRAL, PM_TOURIST, NON_PM, 7000L ),
# endif

/* KMH -- More effective against normal monsters
 * Was +10 to-hit, +d20 damage only versus vampires
 */
A("The Stake of Van Helsing",        WOODEN_STAKE,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), 0, 0,
	PHYS(5,12),    NO_DFNS,        CARY(AD_MAGM),
	0,              A_LAWFUL, PM_UNDEAD_SLAYER, NON_PM, 5000L ),

A("The Staff of Withering",	QUARTERSTAFF,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_ATTK|SPFX_INTEL|SPFX_DRLI|SPFX_DALIGN), 0, 0,
	DRLI(3,5),	NO_DFNS,	CARY(AD_COLD),
	ENERGY_BOOST,	A_CHAOTIC, PM_LUNATIC, NON_PM, 5000L ),

A("The Orb of Fate",		CRYSTAL_BALL,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_LUCK),
		(SPFX_WARN|SPFX_HSPDAM|SPFX_HPHDAM), 0,
	NO_ATTK,	NO_DFNS,	NO_CARY,
	LEV_TELE,	A_NEUTRAL, PM_VALKYRIE, NON_PM, 3500L ),

A("The Eye of the Aethiopica",	AMULET_OF_ESP,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_EREGEN|SPFX_HSPDAM), 0,
	NO_ATTK,	NO_DFNS,	CARY(AD_MAGM),
	CREATE_PORTAL,	A_NEUTRAL, PM_WIZARD, NON_PM, 4000L ),

#ifdef YEOMAN
A("The Crown of Saint Edward", HELM_OF_TELEPATHY,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL), (SPFX_HSPDAM), 0,
	NO_ATTK,        NO_DFNS,        CARY(AD_MAGM),
	0,  A_LAWFUL, PM_YEOMAN, NON_PM, 5000L ),
#endif

#ifdef JEDI
A("The Lightsaber Prototype", RED_LIGHTSABER,
	(SPFX_NOGEN|SPFX_RESTR|SPFX_INTEL|SPFX_REFLECT), 0, 0,
	PHYS(5, 10),	NO_DFNS, NO_CARY,
	ENERGY_BOOST,	A_LAWFUL, PM_JEDI, NON_PM, 3500L ),
#endif


/*
 *  terminator; otyp must be zero
 */
A(0, 0, 0, 0, 0, NO_ATTK, NO_DFNS, NO_CARY, 0, A_NONE, NON_PM, NON_PM, 0L )

};	/* artilist[] (or artifact_names[]) */

#undef	A

#ifndef MAKEDEFS_C
#undef	NO_ATTK
#undef	NO_DFNS
#undef	DFNS
#undef	PHYS
#undef	DRLI
#undef	COLD
#undef	FIRE
#undef	ELEC
#undef	STUN
#undef  ACID
#undef  DRST
#endif

/*artilist.h*/
