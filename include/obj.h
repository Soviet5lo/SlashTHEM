/*	SCCS Id: @(#)obj.h	3.4	2002/01/07	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef OBJ_H
#define OBJ_H

/* #define obj obj_nh */ /* uncomment for SCO UNIX, which has a conflicting
			  * typedef for "obj" in <sys/types.h> */

union vptrs {
	    struct obj *v_nexthere;	/* floor location lists */
	    struct obj *v_ocontainer;	/* point back to container */
	    struct monst *v_ocarry;	/* point back to carrying monst */
};

struct obj {
	struct obj *nobj;
	union vptrs v;
#define nexthere	v.v_nexthere
#define ocontainer	v.v_ocontainer
#define ocarry		v.v_ocarry

	struct obj *cobj;	/* contents list for containers */
	unsigned o_id;
	xchar ox,oy;
	short otyp;		/* object class number */
#ifdef UNPOLYPILE
	short oldtyp;	/* WAC for unpolymorph */
#endif
	unsigned owt;
	long quan;		/* number of items */

	schar spe;		/* quality of weapon, armor or ring (+ or -)
				   number of charges for wand ( >= -1 )
				   marks your eggs, spinach tins
				   royal coffers for a court ( == 2)
				   tells which fruit a fruit is
				   special for uball and amulet
				   historic and gender for statues */
#define STATUE_HISTORIC 0x01
#define STATUE_MALE     0x02
#define STATUE_FEMALE   0x04
	char	oclass;		/* object class */
	char	invlet;		/* designation in inventory */
	int	oartifact;	/* artifact array index */
	schar 	altmode; 	/* alternate modes - eg. SMG, double Lightsaber */
				/* WP_MODEs are in decreasing speed */
#define WP_MODE_AUTO	0	/* Max firing speed */
#define WP_MODE_BURST	1	/* 1/3 of max rate */
#define WP_MODE_SINGLE 	2	/* Single shot */

	xchar where;		/* where the object thinks it is */
#define OBJ_FREE	0		/* object not attached to anything */
#define OBJ_FLOOR	1		/* object on floor */
#define OBJ_CONTAINED	2		/* object in a container */
#define OBJ_INVENT	3		/* object in the hero's inventory */
#define OBJ_MINVENT	4		/* object in a monster inventory */
#define OBJ_MIGRATING	5		/* object sent off to another level */
#define OBJ_BURIED	6		/* object buried */
#define OBJ_ONBILL	7		/* object on shk bill */
#define NOBJ_STATES	8
	xchar timed;		/* # of fuses (timers) attached to this obj */

	Bitfield(cursed,1);
	Bitfield(blessed,1);
	Bitfield(unpaid,1);	/* on some bill */
	Bitfield(no_charge,1);	/* if shk shouldn't charge for this */
	Bitfield(known,1);	/* exact nature known */
	Bitfield(dknown,1);	/* color or text known */
	Bitfield(bknown,1);	/* blessing or curse known */
	Bitfield(rknown,1);	/* rustproof or not known */

	Bitfield(oeroded,2);	/* rusted/burnt weapon/armor */
	Bitfield(oeroded2,2);	/* corroded/rotted weapon/armor */
#define greatest_erosion(otmp) (int)((otmp)->oeroded > (otmp)->oeroded2 ? (otmp)->oeroded : (otmp)->oeroded2)
#define MAX_ERODE 3
#define orotten oeroded		/* rotten food */
#define odiluted oeroded	/* diluted potions */
#define norevive oeroded2
	Bitfield(oerodeproof,1); /* erodeproof weapon/armor */
	Bitfield(olocked,1);	/* object is locked */
#define axeinuse olocked	/* axe collateral damage does not chain */
#define oarmed olocked
#define odrained olocked	/* drained corpse */
	Bitfield(obroken,1);	/* lock has been broken */
	Bitfield(otrapped,1);	/* container is trapped */
				/* or accidental tripped rolling boulder trap */
#define opoisoned otrapped	/* object (weapon) is coated with poison */

	Bitfield(recharged,3);	/* number of times it's been recharged */
	Bitfield(lamplit,1);	/* a light-source -- can be lit */
#ifdef INVISIBLE_OBJECTS
	Bitfield(oinvis,1);	/* invisible */
#endif
	Bitfield(greased,1);	/* covered with grease */
	Bitfield(oattached,2);	/* obj struct has special attachment */
#define OATTACHED_NOTHING 0
#define OATTACHED_MONST   1	/* monst struct in oextra */
#define OATTACHED_M_ID    2	/* monst id in oextra */
#define OATTACHED_UNUSED3 3
	Bitfield(in_use,1);	/* for magic items before useup items */
	Bitfield(bypass,1);	/* mark this as an object to be skipped by bhito() */

	Bitfield(yours,1);	/* obj is yours (eg. thrown by you) */
	Bitfield(was_thrown,1); /* thrown by the hero since last picked up */
	/* ? free bits */

	int	corpsenm;	/* type of corpse is mons[corpsenm] */
#define leashmon  corpsenm	/* gets m_id of attached pet */
#define spestudied corpsenm	/* # of times a spellbook has been studied */
#define fromsink  corpsenm	/* a potion from a sink */

#ifdef RECORD_ACHIEVE
#define record_achieve_special corpsenm
#endif

	unsigned oeaten;	/* nutrition left in food, if partly eaten */
	long age;		/* creation date */

	uchar onamelth;		/* length of name (following oxlth) */
	Bitfield(selfmade,1);	/* made with chemistry set or magic marker */
	short oxlth;		/* length of following data */
	/* in order to prevent alignment problems oextra should
	   be (or follow) a long int */
	long owornmask;
	long oextra[1];		/* used for name of ordinary objects - length
				   is flexible; amount for tmp gold objects */
};

#define newobj(xl)	(struct obj *)alloc((unsigned)(xl) + sizeof(struct obj))
#define ONAME(otmp)	(((char *)(otmp)->oextra) + (otmp)->oxlth)

/* All objects */
#ifdef UNPOLYPILE
#define is_hazy(otmp)	((otmp)->oldtyp != STRANGE_OBJECT)
#endif
/* [ALI] None of the objects listed here can be picked up by normal monsters.
 * If any such objects need to be marked as indestructible then consideration
 * will need to be given to what happens when such a monster disappears
 * carrying the object.
 */
#define evades_destruction(otmp) ( \
			(otmp)->otyp == AMULET_OF_YENDOR || \
			(otmp)->otyp == CANDELABRUM_OF_INVOCATION || \
			(otmp)->otyp == BELL_OF_OPENING || \
			(otmp)->otyp == SPE_BOOK_OF_THE_DEAD || \
			(otmp)->oartifact == ART_KEY_OF_LAW || \
			(otmp)->oartifact == ART_KEY_OF_NEUTRALITY || \
			(otmp)->oartifact == ART_KEY_OF_CHAOS)
#ifdef INVISIBLE_OBJECTS
#define always_visible(otmp) ( \
			(otmp)->otyp == MUMMY_WRAPPING || \
			(otmp)->oclass == COIN_CLASS)
#endif

/* Weapons and weapon-tools */
/* KMH -- now based on skill categories.  Formerly:
 *	#define is_sword(otmp)	(otmp->oclass == WEAPON_CLASS && \
 *			 objects[otmp->otyp].oc_wepcat == WEP_SWORD)
 *	#define is_blade(otmp)	(otmp->oclass == WEAPON_CLASS && \
 *			 (objects[otmp->otyp].oc_wepcat == WEP_BLADE || \
 *			  objects[otmp->otyp].oc_wepcat == WEP_SWORD))
 *	#define is_weptool(o)	((o)->oclass == TOOL_CLASS && \
 *			 objects[(o)->otyp].oc_weptool)
 *	#define is_multigen(otyp) (otyp <= SHURIKEN)
 *	#define is_poisonable(otyp) (otyp <= BEC_DE_CORBIN)
 */

#define is_sword(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= P_SHORT_SWORD && \
			 objects[(otmp)->otyp].oc_skill <= P_SABER)
#define is_blade(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= P_DAGGER && \
			 objects[(otmp)->otyp].oc_skill <= P_SABER)
#define is_pole(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			(otmp)->oclass == TOOL_CLASS) && \
			 (objects[(otmp)->otyp].oc_skill == P_POLEARMS))
#define is_spear(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= P_SPEAR && \
			 objects[(otmp)->otyp].oc_skill <= P_JAVELIN)
#define is_axe(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill == P_AXE)
#define is_launcher(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= P_BOW && \
			 objects[(otmp)->otyp].oc_skill <= P_CROSSBOW)
/* 5lo: Should also add 2-handed firearms? */
#define is_2h_launcher(otmp) 	((otmp->oclass == WEAPON_CLASS && \
			 (objects[otmp->otyp].oc_skill == P_BOW \
			  || objects [otmp->otyp].oc_skill == P_CROSSBOW)))
#define is_ammo(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			 (otmp)->oclass == GEM_CLASS) && \
			 objects[(otmp)->otyp].oc_skill >= -P_CROSSBOW && \
			 objects[(otmp)->otyp].oc_skill <= -P_BOW)
# define throwable_nut(otmp)    ((otmp->oclass == FOOD_CLASS) && \
				objects[(otmp)->otyp].oc_skill == -P_SLING && \
				!(otmp)->oeaten)

#define is_missile(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			 throwable_nut(otmp) || \
			 (otmp)->oclass == TOOL_CLASS) && \
			 objects[(otmp)->otyp].oc_skill >= -P_BOOMERANG && \
			 objects[(otmp)->otyp].oc_skill <= -P_DART)
#define is_grenade(otmp)	(is_ammo(otmp) && \
			 	 objects[(otmp)->otyp].w_ammotyp == WP_GRENADE)
#define is_multigen(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill >= -P_SHURIKEN && \
			 objects[(otmp)->otyp].oc_skill <= -P_BOW)
#ifdef FIREARMS
#define is_unpoisonable_firearm_ammo(otmp)	\
			 (is_bullet(otmp) || (otmp)->otyp == STICK_OF_DYNAMITE)
#else
#define is_unpoisonable_firearm_ammo(otmp)	0
#endif
#define is_poisonable(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 (objects[(otmp)->otyp].oc_skill <= P_SABER || \
			 (objects[(otmp)->otyp].oc_skill >= P_POLEARMS && \
			 objects[(otmp)->otyp].oc_skill <= P_LANCE)) && \
			 !is_unpoisonable_firearm_ammo(otmp))
#define uslinging()	(uwep && objects[uwep->otyp].oc_skill == P_SLING)
#define is_weptool(o)	((o)->oclass == TOOL_CLASS && \
			 objects[(o)->otyp].oc_skill != P_NONE)
#define is_instrument(o)	((o)->otyp >= WOODEN_FLUTE && \
			 (o)->otyp <= DRUM_OF_EARTHQUAKE)
#define is_pick(otmp)	(((otmp)->oclass == WEAPON_CLASS || \
			 (otmp)->oclass == TOOL_CLASS) && \
			 objects[(otmp)->otyp].oc_skill == P_PICK_AXE)

#define ammo_and_launcher(otmp,ltmp) \
			(is_ammo(otmp) && (ltmp) && \
			objects[(otmp)->otyp].oc_skill == -objects[(ltmp)->otyp].oc_skill && \
			  objects[(otmp)->otyp].w_ammotyp == objects[(ltmp)->otyp].w_ammotyp)
#define bimanual(otmp)	((!Race_if(PM_GIANT) || Race_if(PM_GIANT) && is_2h_launcher(otmp)) \
			 && ((otmp)->oclass == WEAPON_CLASS || \
			  (otmp)->oclass == TOOL_CLASS) && \
			 objects[(otmp)->otyp].oc_bimanual)

#ifdef LIGHTSABERS
#define is_lightsaber(otmp) (objects[(otmp)->otyp].oc_skill == P_LIGHTSABER)
#endif

#ifdef FIREARMS
#define is_firearm(otmp) \
			((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill == P_FIREARM)
#define is_bullet(otmp)	((otmp)->oclass == WEAPON_CLASS && \
			 objects[(otmp)->otyp].oc_skill == -P_FIREARM)
#endif

/* Armor */
#define is_shield(otmp) ((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_SHIELD)
#define is_helmet(otmp) ((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_HELM)
#define is_boots(otmp)	((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_BOOTS)
#define is_gloves(otmp) ((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_GLOVES)
#define is_cloak(otmp)	((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_CLOAK)
#define is_shirt(otmp)	((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_SHIRT)
#define is_suit(otmp)	((otmp)->oclass == ARMOR_CLASS && \
			 objects[(otmp)->otyp].oc_armcat == ARM_SUIT)
#define is_elven_armor(otmp)	((otmp)->otyp == ELVEN_LEATHER_HELM\
				|| (otmp)->otyp == ELVEN_MITHRIL_COAT\
				|| (otmp)->otyp == ELVEN_CLOAK\
				|| (otmp)->otyp == ELVEN_SHIELD\
				|| (otmp)->otyp == ELVEN_BOOTS)
#define is_orcish_armor(otmp)	((otmp)->otyp == ORCISH_HELM\
				|| (otmp)->otyp == ORCISH_CHAIN_MAIL\
				|| (otmp)->otyp == ORCISH_RING_MAIL\
				|| (otmp)->otyp == ORCISH_CLOAK\
				|| (otmp)->otyp == URUK_HAI_SHIELD\
				|| (otmp)->otyp == ORCISH_SHIELD)
#define is_dwarvish_armor(otmp)	((otmp)->otyp == DWARVISH_IRON_HELM\
				|| (otmp)->otyp == DWARVISH_MITHRIL_COAT\
				|| (otmp)->otyp == DWARVISH_CLOAK\
				|| (otmp)->otyp == DWARVISH_ROUNDSHIELD)
#define is_gnomish_armor(otmp)	((otmp)->otyp == GNOMISH_HELM\
				|| (otmp)->otyp == GNOMISH_BOOTS\
				|| (otmp)->otyp == GNOMISH_SUIT)

				
/* Eggs and other food */
#define MAX_EGG_HATCH_TIME 200	/* longest an egg can remain unhatched */
#define stale_egg(egg)	((monstermoves - (egg)->age) > (2*MAX_EGG_HATCH_TIME))
#define ofood(o) ((o)->otyp == CORPSE || (o)->otyp == EGG || (o)->otyp == TIN)
#define polyfodder(obj)	(ofood(obj) && (obj)->corpsenm == PM_CHAMELEON)
#define mlevelgain(obj) (ofood(obj) && (obj)->corpsenm == PM_WRAITH)
#define mhealup(obj)	(ofood(obj) && (obj)->corpsenm == PM_NURSE)
#define drainlevel(corpse) (mons[(corpse)->corpsenm].cnutrit*4/5)

/* Containers */
#define carried(o)	((o)->where == OBJ_INVENT)
#define mcarried(o)	((o)->where == OBJ_MINVENT)
#define Has_contents(o) (/* (Is_container(o) || (o)->otyp == STATUE) && */ \
			 (o)->cobj != (struct obj *)0)
#define Is_container(o) ((o)->otyp == MEDICAL_KIT || \
			 (o)->otyp >= LARGE_BOX && (o)->otyp <= BAG_OF_TRICKS)
#define Is_box(otmp)	((otmp)->otyp == LARGE_BOX || (otmp)->otyp == CHEST)
#ifdef WALLET_O_P
#define Is_mbag(otmp)	((otmp)->otyp == BAG_OF_HOLDING || \
                         ((otmp)->oartifact && \
                          (otmp)->oartifact == ART_WALLET_OF_PERSEUS) || \
  			             (otmp)->otyp == BAG_OF_TRICKS)
#else
#define Is_mbag(otmp)	((otmp)->otyp == BAG_OF_HOLDING || \
  			 (otmp)->otyp == BAG_OF_TRICKS)
#endif

/* dragon gear */
#define Is_dragon_scales(obj)	((obj)->otyp >= GRAY_DRAGON_SCALES && \
				 (obj)->otyp <= YELLOW_DRAGON_SCALES)
#define Is_dragon_mail(obj)	((obj)->otyp >= GRAY_DRAGON_SCALE_MAIL && \
				 (obj)->otyp <= YELLOW_DRAGON_SCALE_MAIL)
#define Is_dragon_armor(obj)	(Is_dragon_scales(obj) || Is_dragon_mail(obj))
#define Dragon_scales_to_pm(obj) &mons[PM_GRAY_DRAGON + (obj)->otyp \
				       - GRAY_DRAGON_SCALES]
#define Dragon_mail_to_pm(obj)	&mons[PM_GRAY_DRAGON + (obj)->otyp \
				      - GRAY_DRAGON_SCALE_MAIL]
#define Dragon_to_scales(pm)	(GRAY_DRAGON_SCALES + (pm - mons))

/* Elven gear */
#define is_elven_weapon(otmp)	((otmp)->otyp == ELVEN_ARROW\
				|| (otmp)->otyp == ELVEN_SPEAR\
				|| (otmp)->otyp == ELVEN_DAGGER\
				|| (otmp)->otyp == ELVEN_SHORT_SWORD\
				|| (otmp)->otyp == ELVEN_BROADSWORD\
				|| (otmp)->otyp == ELVEN_BOW)
#define is_elven_obj(otmp)	(is_elven_armor(otmp) || is_elven_weapon(otmp))

/* Orcish gear */
#define is_orcish_obj(otmp)	(is_orcish_armor(otmp)\
				|| (otmp)->otyp == ORCISH_ARROW\
				|| (otmp)->otyp == ORCISH_SPEAR\
				|| (otmp)->otyp == ORCISH_DAGGER\
				|| (otmp)->otyp == ORCISH_SHORT_SWORD\
				|| (otmp)->otyp == ORCISH_BOW)

/* Dwarvish gear */
#define is_dwarvish_obj(otmp)	(is_dwarvish_armor(otmp)\
				|| (otmp)->otyp == DWARVISH_SPEAR\
				|| (otmp)->otyp == DWARVISH_SHORT_SWORD\
				|| (otmp)->otyp == DWARVISH_MATTOCK)

/* Gnomish gear */
#define is_gnomish_obj(otmp)	(is_gnomish_armor(otmp))

/* Light sources */
#define Is_candle(otmp)	((otmp)->otyp == TALLOW_CANDLE || \
			 (otmp)->otyp == WAX_CANDLE || \
			 (otmp)->otyp == MAGIC_CANDLE)
/* maximum amount of oil in a potion of oil */
#define MAX_OIL_IN_FLASK 400

/* MAGIC_LAMP intentionally excluded below */
/* age field of this is relative age rather than absolute */
#define age_is_relative(otmp)	((otmp)->otyp == BRASS_LANTERN\
				|| (otmp)->otyp == OIL_LAMP\
				|| (otmp)->otyp == TORCH\
				|| (otmp)->otyp == CANDELABRUM_OF_INVOCATION\
				|| (otmp)->otyp == TALLOW_CANDLE\
				|| (otmp)->otyp == WAX_CANDLE\
				|| (otmp)->otyp == POT_OIL)
/* object can be ignited */
#define ignitable(otmp)	((otmp)->otyp == BRASS_LANTERN\
				|| (otmp)->otyp == OIL_LAMP\
				|| (otmp)->otyp == TORCH\
				|| (otmp)->otyp == CANDELABRUM_OF_INVOCATION\
				|| (otmp)->otyp == TALLOW_CANDLE\
				|| (otmp)->otyp == WAX_CANDLE\
				|| (otmp)->otyp == MAGIC_CANDLE\
				|| (otmp)->otyp == POT_OIL)

/* special stones */
#define is_graystone(obj)	((obj)->otyp == LUCKSTONE || \
				 (obj)->otyp == LOADSTONE || \
				 (obj)->otyp == FLINT     || \
				 (obj)->otyp == TOUCHSTONE || \
				 (obj)->otyp == HEALTHSTONE || \
				 (obj)->otyp == WHETSTONE)

/* misc */
#ifdef KOPS
#define is_flimsy(otmp)		(objects[(otmp)->otyp].oc_material <= LEATHER || \
				 (otmp)->otyp == RUBBER_HOSE)
#else
#define is_flimsy(otmp)		(objects[(otmp)->otyp].oc_material <= LEATHER)
#endif

# define oresist_disintegration(obj) \
		(objects[obj->otyp].oc_oprop == DISINT_RES || \
		 obj_resists(obj, 5, 50) || is_quest_artifact(obj) )
# define weight_dmg(i) {  \
	i = (i<=100)?1:i/100; \
	i = rnd(i); \
	if(i > 6) i = 6; \
}

/* helpers, simple enough to be macros */
#define is_plural(o)	((o)->quan > 1 || \
			 (o)->oartifact == ART_EYES_OF_THE_OVERWORLD)

/* Flags for get_obj_location(). */
#define CONTAINED_TOO	0x1
#define BURIED_TOO	0x2

#ifdef ITEMCAT
#ifdef ITEMCAT_OLDSTYLE
#   ifdef MAIL
#   define BKNOWN(otmp) (otmp->bknown || otmp->otyp == SCR_MAIL)
#   else
#   define BKNOWN(otmp) (otmp->bknown)
#   endif
#else
#   define BKNOWN(otmp) (1)
#endif

/* faster version of not_fully_identified() for item selection
 * (invent.c/pickup.c) */
#define NOT_IDENTIFIED_ITEMCAT(otmp) (otmp->oclass != COIN_CLASS && !(otmp->known && otmp->dknown && BKNOWN(otmp) && objects[otmp->otyp].oc_name_known) || (otmp->oartifact && undiscovered_artifact(otmp->oartifact)) || (!otmp->rknown && ((otmp->oclass == ARMOR_CLASS || otmp->oclass == WEAPON_CLASS || is_weptool(otmp) || otmp->oclass == BALL_CLASS)) && (is_rustprone(otmp) || is_corrodeable(otmp) || is_flammable(otmp))))

#endif /* ITEMCAT */

#endif /* OBJ_H */
