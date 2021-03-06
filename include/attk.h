/* attk.h */
/* NetHack may be freely redistributed.  See license for details. */
/* Copyright 1988, M. Stephenson */

#ifndef ATTK_H
#define ATTK_H

typedef struct Attk {
	struct monst *amon;		/* Attacking monster */
	struct obj *wobj;		/* The wielded object, launcher, or wand */
	struct obj *tobj;		/* The thrown, kicked, or zapped object */
	struct monst *dmon;		/* Defending monster */
	xchar	atype;			/* The attack type (AT_ below) */
	xchar	admg;			/* The damage type (AD_ below) */
	xchar	skill;			/* Skill used (if any) */
	schar	aalign;			/* Attacking monster's alignment */
	schar	dalign;			/* Defending monster's alignment */
	xchar	ax, ay;			/* Location of attacker */
	schar	dx, dy, dz;		/* Direction of the attack */
	xchar	tx, ty;			/* Location of tobj */
	xchar	range;			/* How much further tobj can go */
	xchar	dist;			/* Distance from amon to dmon */
	long	tglyph;			/* Glyph to use for tobj */
	xchar	tohit;			/* Calculated to-hit value */
	xchar	dieroll;		/* The die roll used for the attack */
	xchar	damage;			/* Calculated amount of damage */
	Bitfield(ayou,1);		/* You are the attacker */
	Bitfield(avis,1);		/* Player can see amon */
	Bitfield(dyou,1);		/* You are the defender */
	Bitfield(dvis,1);		/* Player can see dmon */
	Bitfield(defensive,1);	/* This is the defensive response of another */
							/* attack by dmon against amon */
	Bitfield(poison,1);		/* Poison was used */
	Bitfield(poiskilled,1);
	Bitfield(poismgs,1);
	Bitfield(vampmsg,1);
	Bitfield(resists,1);	/* Defender resists */
	Bitfield(passes,1);		/* Attack passes harmlessly through dmon */
	Bitfield(dkilled,1);	/* Defender is killed */
	Bitfield(wlearn,1);		/* Character learns identity of wobj */
	Bitfield(hitmsg,1);
	Bitfield(skbonus,1);
} *Attk;

extern void attk_by_you(Attk);
extern void attk_by_mon(Attk, struct monst *);
extern void attk_by_god(Attk, SCHAR_P);
extern void attk_by_trap(Attk, XCHAR_P);
extern void attk_with_obj(Attk, struct obj *);
extern void attk_with_mon(Attk, XCHAR_P);
extern void attk_throw(Attk, struct obj *);
extern void attk_upon(Attk, struct monst *);
extern char *attk_aname(Attk);
extern char *attk_wname(Attk);
extern char *attk_tname(Attk);
extern char *attk_dname(Attk);

/*	Add new attack types below - ordering affects experience (exper.c).
 *	Attacks > AT_BUTT are worth extra experience.
 */
#define AT_NONE		0		/* passive monster (ex. acid blob) */
#define AT_CLAW		1		/* claw (punch, hit, etc.) */
#define AT_BITE		2		/* bite */
#define AT_KICK		3		/* kick */
#define AT_BUTT		4		/* head butt (ex. a unicorn) */
#define AT_TUCH		5		/* touches */
#define AT_STNG		6		/* sting */
#define AT_HUGS		7		/* crushing bearhug */
#define AT_SCRA		8		/* similar to claw */
#define AT_LASH		9		/* whirl around and hit */
#define AT_SPIT		10		/* spits substance - ranged */
#define AT_ENGL		11		/* engulf (swallow or by a cloud) */
#define AT_BREA		12		/* breath - ranged */
#define AT_EXPL		13		/* explodes - proximity */
#define AT_BOOM     14      /* explodes upon hit */
#define AT_GAZE     15      /* gaze - ranged */
#define AT_TENT     16      /* tentacles */
#define AT_TRAM     17      /* trample */
#define AT_MULTIPLY	18	/* RJ - multiplies (yes, it's an attack) */

#define AT_WEAP		64		/* Hand-to-hand weapon (monster or character) */
#define AT_THRO		65		/* Thrown object */
#define AT_SPEL		66		/* Magic spell (monster or character) */
#define AT_WAND		67		/* Wand or camera (monster or character) */
#define AT_MEGA		68		/* Mega spell/wand */
#define AT_TRAP		69		/* Trap (amon is null) */
#define AT_GODS		70		/* God (amon is null) */

/*#define AT_MAGC	66 */		/* uses magic spell(s) */


/*	Add new damage types below.
 *
 *	Note that 1-10 correspond to the types of attack used in buzz().
 *	Please don't disturb the order unless you rewrite the buzz() code.
 */
#define AD_PHYS		0		/* ordinary physical */
#define AD_MAGM		1		/* magic missiles */
#define AD_FIRE		2		/* fire damage */
#define AD_COLD		3		/* frost damage */
#define AD_SLEE		4		/* sleep ray */
#define AD_DISN		5		/* disintegration (death ray) */
#define AD_ELEC		6		/* shock damage */
#define AD_DRST		7		/* drains str (poison) */
#define AD_ACID		8		/* acid damage */
#define AD_LITE		9		/* light ray */
#define AD_SPC2		10		/* for extension of buzz() */
#define AD_BLND		11		/* blinds (glowing eye) */
#define AD_STUN		12		/* stuns */
#define AD_SLOW		13		/* slows */
#define AD_PLYS		14		/* paralyses */
#define AD_DRLI		15		/* drains life levels (Vampire) */
#define AD_DREN		16		/* drains magic energy */
#define AD_LEGS		17		/* damages legs (xan) */
#define AD_STON		18		/* petrifies (Medusa, Cockatrice) */
#define AD_STCK		19		/* sticks to you (Mimic) */
#define AD_SGLD		20		/* steals gold (Leppie) */
#define AD_SITM		21		/* steals item (Nymphs) */
#define AD_SEDU		22		/* seduces & steals multiple items */
#define AD_TLPT		23		/* teleports you (Quantum Mech.) */
#define AD_RUST		24		/* rusts armour (Rust Monster)*/
#define AD_CONF		25		/* confuses (Umber Hulk) */
#define AD_DGST		26		/* digests opponent (trapper, etc.) */
#define AD_HEAL		27		/* heals opponent's wounds (nurse) */
#define AD_WRAP		28		/* special "stick" for eels */
#define AD_WERE		29		/* confers lycanthropy */
#define AD_DRDX		30		/* drains dexterity (Quasit) */
#define AD_DRCO		31		/* drains constitution */
#define AD_DRIN		32		/* drains intelligence (mind flayer) */
#define AD_DISE		33		/* confers diseases */
#define AD_DCAY		34		/* decays organics (Brown pudding) */
#define AD_SSEX		35		/* Succubus seduction (extended) */
					/* If no SEDUCE then same as AD_SEDU */
#define AD_HALU		36		/* causes hallucination */
#define AD_DETH		37		/* for Death only */
#define AD_PEST		38		/* for Pestilence only */
#define AD_FAMN		39		/* for Famine only */
#define AD_SLIM		40		/* turns you into green slime */
#define AD_CALM		41	/* KMH -- calms its enemies (koala) */
#define AD_ENCH		42	/* KMH -- remove enchantment */
#define AD_POLY		43	/* RJ -- polymorphs (genetic engineer) */
#define AD_CORR		44	/* corrode armor (black pudding) */
#define AD_TCKL		45	/* Tickle (Nightgaunts) */
#define AD_ENDS		46	/* placeholder */

#define AD_CLRC		240		/* random clerical spell */
#define AD_SPEL		241		/* random magic spell */
#define AD_RBRE		242		/* random breath weapon */

#define AD_SAMU		252		/* hits, may steal Amulet (Wizard) */
#define AD_CURS		253		/* random curse (ex. gremlin) */


/*
 *  Monster to monster attacks.  When a monster attacks another (mattackm),
 *  any or all of the following can be returned.  See mattackm() for more
 *  details.
 */
#define MM_MISS		0x0	/* aggressor missed */
#define MM_HIT		0x1	/* aggressor hit defender */
#define MM_DEF_DIED	0x2	/* defender died */
#define MM_AGR_DIED	0x4	/* aggressor died */

#endif /* ATTK_H */
