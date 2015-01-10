/*	SCCS Id: @(#)makemon.c	3.4	2003/09/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "epri.h"
#include "emin.h"
#include "edog.h"
#ifdef REINCARNATION
#include <ctype.h>
#endif

STATIC_VAR NEARDATA struct monst zeromonst;

/* this assumes that a human quest leader or nemesis is an archetype
   of the corresponding role; that isn't so for some roles (tourist
   for instance) but is for the priests and monks we use it for... */
#define quest_mon_represents_role(mptr,role_pm) \
		(mptr->mlet == S_HUMAN && Role_if(role_pm) && \
		  (mptr->msound == MS_LEADER || mptr->msound == MS_NEMESIS))

#ifdef OVL0
STATIC_DCL boolean FDECL(uncommon, (int));
STATIC_DCL int FDECL(align_shift, (struct permonst *));
#endif /* OVL0 */
STATIC_DCL boolean FDECL(wrong_elem_type, (struct permonst *));
STATIC_DCL void FDECL(m_initgrp,(struct monst *,int,int,int));
STATIC_DCL void FDECL(m_initthrow,(struct monst *,int,int));
STATIC_DCL void FDECL(m_initweap,(struct monst *));
STATIC_DCL void FDECL(m_initxtraitem,(struct monst *));
STATIC_DCL void FDECL(m_initweap_normal,(struct monst *));
#ifdef OVL1
STATIC_DCL void FDECL(m_initinv,(struct monst *));
#endif /* OVL1 */

extern const int monstr[];

#define m_initsgrp(mtmp, x, y)	m_initgrp(mtmp, x, y, 3)
#define m_initlgrp(mtmp, x, y)	m_initgrp(mtmp, x, y, 10)
#define m_initvlgrp(mtmp, x, y)  m_initgrp(mtmp, x, y, 20)
#define toostrong(monindx, lev) (monstr[monindx] > lev)
#define tooweak(monindx, lev)	(monstr[monindx] < lev)

#ifdef OVLB
boolean
is_home_elemental(ptr)
register struct permonst *ptr;
{
	if (ptr->mlet == S_ELEMENTAL)
	    switch (monsndx(ptr)) {
		case PM_AIR_ELEMENTAL: return Is_airlevel(&u.uz);
		case PM_FIRE_ELEMENTAL: return Is_firelevel(&u.uz);
		case PM_EARTH_ELEMENTAL: return Is_earthlevel(&u.uz);
		case PM_WATER_ELEMENTAL: return Is_waterlevel(&u.uz);
	    }
	return FALSE;
}

/*
 * Return true if the given monster cannot exist on this elemental level.
 */
STATIC_OVL boolean
wrong_elem_type(ptr)
    register struct permonst *ptr;
{
    if (ptr->mlet == S_ELEMENTAL) {
	return((boolean)(!is_home_elemental(ptr)));
    } else if (Is_earthlevel(&u.uz)) {
	/* no restrictions? */
    } else if (Is_waterlevel(&u.uz)) {
	/* just monsters that can swim */
	if(!is_swimmer(ptr)) return TRUE;
    } else if (Is_firelevel(&u.uz)) {
	if (!pm_resistance(ptr,MR_FIRE)) return TRUE;
    } else if (Is_airlevel(&u.uz)) {
	if(!(is_flyer(ptr) && ptr->mlet != S_TRAPPER) && !is_floater(ptr)
	   && !amorphous(ptr) && !noncorporeal(ptr) && !is_whirly(ptr))
	    return TRUE;
    }
    return FALSE;
}

STATIC_OVL void
m_initgrp(mtmp, x, y, n)	/* make a group just like mtmp */
register struct monst *mtmp;
register int x, y, n;
{
	coord mm;
	register int cnt = rnd(n);
	struct monst *mon;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	/* There is an unresolved problem with several people finding that
	 * the game hangs eating CPU; if interrupted and restored, the level
	 * will be filled with monsters.  Of those reports giving system type,
	 * there were two DG/UX and two HP-UX, all using gcc as the compiler.
	 * hcroft@hpopb1.cern.ch, using gcc 2.6.3 on HP-UX, says that the
	 * problem went away for him and another reporter-to-newsgroup
	 * after adding this debugging code.  This has almost got to be a
	 * compiler bug, but until somebody tracks it down and gets it fixed,
	 * might as well go with the "but it went away when I tried to find
	 * it" code.
	 */
	int cnttmp,cntdiv;

	cnttmp = cnt;
# ifdef DEBUG
	pline("init group call x=%d,y=%d,n=%d,cnt=%d.", x, y, n, cnt);
# endif
	cntdiv = ((u.ulevel < 3) ? 4 : (u.ulevel < 5) ? 2 : 1);
#endif
	/* Tuning: cut down on swarming at low character levels [mrs] */
	if (u.ulevel < 5) cnt /= 2;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	if (cnt != (cnttmp/cntdiv)) {
		pline("cnt=%d using %d, cnttmp=%d, cntdiv=%d", cnt,
			(u.ulevel < 3) ? 4 : (u.ulevel < 5) ? 2 : 1,
			cnttmp, cntdiv);
	}
#endif
	if(!cnt) cnt++;
#if defined(__GNUC__) && (defined(HPUX) || defined(DGUX))
	if (cnt < 0) cnt = 1;
	if (cnt > 10) cnt = 10;
#endif

	mm.x = x;
	mm.y = y;
	while(cnt--) {
		if (peace_minded(mtmp->data)) continue;
		/* Don't create groups of peaceful monsters since they'll get
		 * in our way.  If the monster has a percentage chance so some
		 * are peaceful and some are not, the result will just be a
		 * smaller group.
		 */
		if (enexto(&mm, mm.x, mm.y, mtmp->data)) {
		    mon = makemon(mtmp->data, mm.x, mm.y, NO_MM_FLAGS);
		    mon->mpeaceful = FALSE;
		    mon->mavenge = 0;
		    set_malign(mon);
		    /* Undo the second peace_minded() check in makemon(); if the
		     * monster turned out to be peaceful the first time we
		     * didn't create it at all; we don't want a second check.
		     */
		}
	}
}

STATIC_OVL
void
m_initthrow(mtmp,otyp,oquan)
struct monst *mtmp;
int otyp,oquan;
{
	register struct obj *otmp;

	otmp = mksobj(otyp, TRUE, FALSE);
	otmp->quan = (long) rn1(oquan, 3);
	otmp->owt = weight(otmp);
	if (otyp == ORCISH_ARROW) otmp->opoisoned = TRUE;
	(void) mpickobj(mtmp, otmp);
}

#endif /* OVLB */
#ifdef OVL2

STATIC_OVL void
m_initweap_normal(mtmp)
register struct monst *mtmp;
{
	register struct permonst *ptr = mtmp->data;
	int bias;

	bias = is_lord(ptr) + is_prince(ptr) * 2 + extra_nasty(ptr);
	switch(rnd(14 - (2 * bias))) {
	    case 1:
		if(strongmonst(ptr)) (void) mongets(mtmp, BATTLE_AXE);
		else m_initthrow(mtmp, DART, 20);
		break;
	    case 2:
		if(strongmonst(ptr))
		    (void) mongets(mtmp, TWO_HANDED_SWORD);
		else {
		    (void) mongets(mtmp, CROSSBOW);
		    m_initthrow(mtmp, CROSSBOW_BOLT, 30);
		}
		break;
	    case 3:
		(void) mongets(mtmp, BOW);
		m_initthrow(mtmp, ARROW, 25);
		break;
	    case 4:
		if(strongmonst(ptr)) (void) mongets(mtmp, LONG_SWORD);
		else m_initthrow(mtmp, DAGGER, 3);
		break;
	    case 5:
		if(strongmonst(ptr))
		    (void) mongets(mtmp, LUCERN_HAMMER);
		else (void) mongets(mtmp, AKLYS);
		break;
	    /* [Tom] added some more */
	    case 6:
		if(strongmonst(ptr))
		    (void) mongets(mtmp, AXE);
		else (void) mongets(mtmp, SHORT_SWORD);
		break;
	    case 7:
		if(strongmonst(ptr))
		    (void) mongets(mtmp, MACE);
		else (void) mongets(mtmp, CLUB);
		break;
	    default:
		break;
	}

	return;
}

STATIC_OVL void
m_initxtraitem(mtmp)
register struct monst *mtmp;
{
	register struct permonst *ptr = mtmp->data;
	/*struct obj *otmpX;*/

	if (!rn2(100)) {
			struct obj *otmpZ = mksobj(SCR_STANDARD_ID, TRUE, FALSE);
			otmpZ->quan = 1;
			otmpZ->owt = weight(otmpZ);
			(void) mpickobj(mtmp, otmpZ);
			}

	if (!rn2(60)) {
			struct obj *otmpW = mksobj(SCR_HEALING, TRUE, FALSE);
			otmpW->quan = 1;
			otmpW->owt = weight(otmpW);
			(void) mpickobj(mtmp, otmpW);
			}


	if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
	if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
	if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));

	if (!rn2(2000)) {
	struct obj *otmpX = mksobj(EGG,TRUE,FALSE);
	otmpX->spe = 0;
	otmpX->quan = 1;
	otmpX->owt = weight(otmpX);
	otmpX->corpsenm = egg_type_from_parent(PM_COCKATRICE, FALSE);
	attach_egg_hatch_timeout(otmpX);
	kill_egg(otmpX); /* make sure they're stale --Amy */
	mpickobj(mtmp,otmpX);
	}

	if (!rn2(500)) {

	struct obj *otmpY = mksobj(CORPSE,TRUE,FALSE);

	otmpY->spe = 0;
	otmpY->quan = 1;
	otmpY->owt = /*weight(otmpY)*/30;
	otmpY->corpsenm = PM_COCKATRICE;
	mpickobj(mtmp,otmpY);
	struct obj *otmpZ = mksobj(rnd_class(PLASTEEL_GLOVES,GAUNTLETS_OF_DEXTERITY),
	      TRUE, FALSE);
	otmpZ->quan = 1;
	otmpZ->owt = weight(otmpZ);
	start_corpse_timeout(otmpZ); /* gotta make sure they time out after a while! --Amy */
	(void) mpickobj(mtmp, otmpZ);

	}

}

STATIC_OVL void
m_initweap(mtmp)
register struct monst *mtmp;
{
	register struct permonst *ptr = mtmp->data;
	register int mm = monsndx(ptr);
	struct obj *otmp;

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) return;
#endif
/*
 *	first a few special cases:
 *
 *		giants get a boulder to throw sometimes.
 *		ettins get clubs
 *		kobolds get darts to throw
 *		centaurs get some sort of bow & arrows or bolts
 *		soldiers get all sorts of things.
 *		kops get clubs & cream pies.
 */

	switch (ptr->mlet) {
	    case S_GIANT:
		if (rn2(2)) (void)mongets(mtmp, (mm != PM_ETTIN) ?
				    BOULDER : CLUB);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		break;
	    case S_WALLMONST:
		    switch (mm) {
			case PM_KALASHNIKOV_WALL:
			case PM_PETTY_KALASHNIKOV_WALL:
			  	(void) mongets(mtmp, HEAVY_MACHINE_GUN);
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);

				if (!rn2(20) ) {
	
				otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
					      TRUE, FALSE);
				otmp->quan = 1;
				otmp->owt = weight(otmp);
				(void) mpickobj(mtmp, otmp);
				}
	
				if (!rn2(100) ) {
	
				otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
					      TRUE, FALSE);
				otmp->quan = 1;
				otmp->owt = weight(otmp);
				(void) mpickobj(mtmp, otmp);
				}

			default:
			break;
			}
		break;

	    case S_RUBMONST:
		    switch (mm) {

			case PM_LIGHTNINGROD:

			  	(void) mongets(mtmp, POT_SPEED);
			  	(void) mongets(mtmp, WAN_CREATE_HORDE);

				if (!rn2(20) ) {
	
				otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
					      TRUE, FALSE);
				otmp->quan = 1;
				otmp->owt = weight(otmp);
				(void) mpickobj(mtmp, otmp);
				}
	
				if (!rn2(100) ) {
	
				otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
					      TRUE, FALSE);
				otmp->quan = 1;
				otmp->owt = weight(otmp);
				(void) mpickobj(mtmp, otmp);
				}

			default:
			break;
			}
		break;


	    case S_HUMAN:
		if(is_mercenary(ptr) || mm == PM_SHOPKEEPER
#ifdef YEOMAN
				|| mm == PM_CHIEF_YEOMAN_WARDER || mm == PM_YEOMAN_WARDER
#endif
#ifdef JEDI
				|| mm == PM_JEDI || mm == PM_PADAWAN || mm == PM_STORMTROOPER
#endif
				|| mm == PM_ENEMY_TROOPER || mm == PM_INFANTRYMAN || mm == PM_CUNTGUN_TROOPER
				|| mm == PM_GI_TROOPER || mm == PM_HEAVY_WEAPON_DUDE || mm == PM_RIFLEMAN
				|| mm == PM_SNIPER || mm == PM_RIOT_BREAKER || mm == PM_TANK_BREAKER
				|| mm == PM_EVASIVE_ENEMY_TROOPER || mm == PM_EVASIVE_INFANTRYMAN || mm == PM_EVASIVE_CUNTGUN_TROOPER
				|| mm == PM_EVASIVE_GI_TROOPER || mm == PM_EVASIVE_HEAVY_WEAPON_DUDE || mm == PM_EVASIVE_RIFLEMAN
				|| mm == PM_EVASIVE_SNIPER || mm == PM_EVASIVE_RIOT_BREAKER || mm == PM_EVASIVE_TANK_BREAKER
		) {

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		    int w1 = 0, w2 = 0;
		    switch (mm) {
			case PM_SOLDIER:
#ifdef FIREARMS
			  w1 = rn2(2) ? RIFLE : SUBMACHINE_GUN;
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  w2 = rn2(2) ? KNIFE : DAGGER;
			  (void) mongets(mtmp, FRAG_GRENADE);
			  break;
#endif
			case PM_WATCHMAN:
			case PM_ANGRY_WATCHMAN:
			  if (!rn2(3)) {
			      w1 = rn1(BEC_DE_CORBIN - PARTISAN + 1, PARTISAN);
			      w2 = rn2(2) ? DAGGER : KNIFE;
			  } else w1 = rn2(2) ? SPEAR : SHORT_SWORD;
			  break;
			case PM_WATCH_CAPTAIN:
			case PM_ANGRY_WATCH_CAPTAIN:
			  w1 = rn2(2) ? LONG_SWORD : SILVER_SABER;
			  break;
			case PM_ANGRY_WATCH_LIEUTENANT:
			case PM_WATCH_LIEUTENANT:
			  w1 = rn2(2) ? LONG_SWORD : SILVER_SABER;
			  break;
			case PM_ANGRY_WATCH_LEADER:
			case PM_WATCH_LEADER:
			  w1 = rn2(2) ? LONG_SWORD : SILVER_SABER;
			  break;
			case PM_LIEUTENANT:
#ifdef FIREARMS
			  if (rn2(2)) {
			  	w1 = HEAVY_MACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  } else {
			  	w1 = SUBMACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  w2 = rn2(2) ? KNIFE : DAGGER;
			  if (rn2(2)) {
			  	(void) mongets(mtmp, FRAG_GRENADE);
			  	(void) mongets(mtmp, FRAG_GRENADE);
			  } else {
			  	(void) mongets(mtmp, GAS_GRENADE);
			  	(void) mongets(mtmp, GAS_GRENADE);
			  }
			  break;
#endif
			case PM_SERGEANT:
#ifdef FIREARMS
			  if (rn2(2)) {
			  	w1 = AUTO_SHOTGUN;
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 10);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 10);
			  } else {
			  	w1 = ASSAULT_RIFLE;
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  w2= rn2(2) ? DAGGER : KNIFE;
			  if (rn2(2)) {
			  	m_initthrow(mtmp, FRAG_GRENADE, 5);
			  } else {
			  	m_initthrow(mtmp, GAS_GRENADE, 5);
			  }
			  if (!rn2(5)) (void) mongets(mtmp, GRENADE_LAUNCHER);
			  break;
#endif
#ifdef YEOMAN
			case PM_YEOMAN_WARDER:
#endif
			  w1 = rn2(2) ? FLAIL : MACE;
			  break;
#ifdef CONVICT
			case PM_PRISON_GUARD:
#endif /* CONVICT */
			case PM_CAPTAIN:
			case PM_GENERAL:
#ifdef FIREARMS
			  if (rn2(2)) {
			  	w1 = AUTO_SHOTGUN;
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  } else if (rn2(2)) {
			  	w1 = HEAVY_MACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  } else {
			  	w1 = ASSAULT_RIFLE;
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  }
			  if (rn2(2)) {
				  w2 = ROCKET_LAUNCHER;
			  	  m_initthrow(mtmp, ROCKET, 5);
			  } else if (rn2(2)) {
				  (void) mongets(mtmp, GRENADE_LAUNCHER);			  
			  	  m_initthrow(mtmp, 
			  	  	(rn2(2) ? FRAG_GRENADE : GAS_GRENADE), 
			  	  	5);
			  } else {
				  w2 = rn2(2) ? SILVER_SABER : DAGGER;
			  }
			  break;
#endif
#ifdef YEOMAN
			case PM_CHIEF_YEOMAN_WARDER:
#endif
			  w1 = rn2(2) ? BROADSWORD : LONG_SWORD;
			  break;
#ifdef JEDI
			case PM_PADAWAN:
			case PM_JEDI:
			  switch(rnd(3)){
			    case 1: mongets(mtmp, RED_LIGHTSABER); break;
			    case 2: mongets(mtmp, BLUE_LIGHTSABER); break;
			    case 3: mongets(mtmp, GREEN_LIGHTSABER); break;
			    default: break;
			  }
			  break;
			case PM_THE_JEDI_MASTER:
			  mongets(mtmp, BLUE_LIGHTSABER);
			  mongets(mtmp, SPEED_BOOTS);
			  mongets(mtmp, AMULET_OF_REFLECTION);
			  break;
			case PM_STORMTROOPER:
#ifdef FIREARMS
			  if (rn2(2)) {
			  	w1 = HEAVY_MACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 50);
			  } else {
			  	w1 = SUBMACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  mongets(mtmp, PLASTEEL_ARMOR);
			  mongets(mtmp, PLASTEEL_GLOVES);
			  mongets(mtmp, PLASTEEL_BOOTS);
			  mongets(mtmp, PLASTEEL_HELM);
			  break;
#endif // FIREARMS
#endif // JEDI

#ifdef FIREARMS
			case PM_HEAVY_WEAPON_DUDE:
			case PM_EVASIVE_HEAVY_WEAPON_DUDE:
			  	w1 = HEAVY_MACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_INFANTRYMAN:
			case PM_EVASIVE_INFANTRYMAN:
			  	w1 = SUBMACHINE_GUN;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_ENEMY_TROOPER:
			case PM_EVASIVE_ENEMY_TROOPER:
			  	w1 = PISTOL;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_CUNTGUN_TROOPER:
			case PM_EVASIVE_CUNTGUN_TROOPER:
			  	w1 = RIFLE;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_GI_TROOPER:
			case PM_EVASIVE_GI_TROOPER:
			  	w1 = ASSAULT_RIFLE;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_SNIPER:
			case PM_EVASIVE_SNIPER:
			  	w1 = SNIPER_RIFLE;
			  	m_initthrow(mtmp, BULLET, 50);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_RIFLEMAN:
			case PM_EVASIVE_RIFLEMAN:
			  	w1 = SHOTGUN;
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_RIOT_BREAKER:
			case PM_EVASIVE_RIOT_BREAKER:
			  	w1 = AUTO_SHOTGUN;
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  mongets(mtmp, CHAIN_MAIL);
			  break;

			case PM_TANK_BREAKER:
			case PM_EVASIVE_TANK_BREAKER:
			  	w1 = ROCKET_LAUNCHER;
			  	m_initthrow(mtmp, ROCKET, 7);
			  mongets(mtmp, CHAIN_MAIL);
			  break;
#endif
			case PM_SHOPKEEPER:
#ifdef FIREARMS
			  (void) mongets(mtmp,SHOTGUN);
			  m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  m_initthrow(mtmp, SHOTGUN_SHELL, 20);
#endif
			  /* Fallthrough */
			default:
			  if (!rn2(4)) w1 = DAGGER;
			  if (!rn2(7)) w2 = SPEAR;
			  break;
		    }
		    if (w1) (void)mongets(mtmp, w1);
		    if (!w2 && w1 != DAGGER && !rn2(4)) w2 = KNIFE;
		    if (w2) (void)mongets(mtmp, w2);
		} else if (is_elf(ptr)) {

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		    if (mm == PM_DROW) {
			(void) mongets(mtmp, DARK_ELVEN_MITHRIL_COAT);
			(void) mongets(mtmp, DARK_ELVEN_SHORT_SWORD);
			(void) mongets(mtmp, DARK_ELVEN_BOW);
			m_initthrow(mtmp, DARK_ELVEN_ARROW, 25);
		    } else {
		    if (rn2(2))
			(void) mongets(mtmp,
				   !rn2(4) ? ELVEN_MITHRIL_COAT : ELVEN_CLOAK);
		      if (!rn2(3)) (void)mongets(mtmp, ELVEN_LEATHER_HELM);
		    else if (!rn2(4)) (void)mongets(mtmp, ELVEN_BOOTS);
		      if (!rn2(3)) (void)mongets(mtmp, ELVEN_DAGGER);
		    switch (rn2(3)) {
			case 0:
			    if (!rn2(4)) (void)mongets(mtmp, ELVEN_SHIELD);
			    if (rn2(3)) (void)mongets(mtmp, ELVEN_SHORT_SWORD);
			    (void)mongets(mtmp, ELVEN_BOW);
			    m_initthrow(mtmp, ELVEN_ARROW, 25);
			    break;
			case 1:
			    (void)mongets(mtmp, ELVEN_BROADSWORD);
			    if (rn2(2)) (void)mongets(mtmp, ELVEN_SHIELD);
			    break;
			case 2:
			    if (rn2(2)) {
				(void)mongets(mtmp, ELVEN_SPEAR);
				(void)mongets(mtmp, ELVEN_SHIELD);
			    }
			    break;
		    }
		    if (mm == PM_ELVENKING) {
			if (rn2(3) || (in_mklev && Is_earthlevel(&u.uz)))
			    (void)mongets(mtmp, PICK_AXE);
			if (!rn2(50)) (void)mongets(mtmp, CRYSTAL_BALL);
		    }
		    } /* normal elves */                
		} else /* enemy characters! */ /* They will always get stuff now. --Amy */
		 if ((mm >= PM_ARCHEOLOGIST && mm <= PM_WIZARD /*&& rn2(4)*/) || (mm >= PM_UNDEAD_ARCHEOLOGIST && mm <= PM_UNDEAD_WIZARD) ) { 

			if (mtmp->m_lev > rnd(50)) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (mtmp->m_lev > rnd(50)) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (mtmp->m_lev > rnd(100)) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (mtmp->m_lev > rnd(250)) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (mtmp->m_lev > rnd(500)) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (mtmp->m_lev > rnd(1000)) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		  switch (mm) {
		   case PM_ARCHEOLOGIST:
		   case PM_UNDEAD_ARCHEOLOGIST:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));

		     (void)mongets(mtmp, BULLWHIP);
		     (void)mongets(mtmp, LEATHER_JACKET);
		     (void)mongets(mtmp, FEDORA);
		     if (rn2(2)) (void)mongets(mtmp, PICK_AXE);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_ZOOKEEPER:
		   case PM_UNDEAD_ZOOKEEPER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));

		     (void)mongets(mtmp, BULLWHIP);
		   break;

		   case PM_BARBARIAN:
		   case PM_UNDEAD_BARBARIAN:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

		     (void)mongets(mtmp, BATTLE_AXE);
		     if (!rn2(2)) (void)mongets(mtmp, TWO_HANDED_SWORD);
		     (void)mongets(mtmp, RING_MAIL);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_BINDER:
		   case PM_UNDEAD_BINDER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, VOULGE);
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, LEATHER_CLOAK);
		   break;

		   case PM_BLEEDER:
		   case PM_UNDEAD_BLEEDER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, LAB_COAT);
		   break;

		   case PM_BARD: case PM_UNDEAD_BARD:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, LEATHER_CLOAK);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_CAVEMAN: case PM_CAVEWOMAN:
		   case PM_UNDEAD_CAVEMAN: case PM_UNDEAD_CAVEWOMAN:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, CLUB);
		     if (rn2(3)) {
			(void)mongets(mtmp, BOW);
			 m_initthrow(mtmp, ARROW, 25);
		     }
		     (void)mongets(mtmp, LEATHER_ARMOR);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_DOPPELGANGER:
		     (void)mongets(mtmp, SILVER_DAGGER);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_GEEK:
		   case PM_UNDEAD_GEEK:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, SILVER_DAGGER);
		   break;
		   case PM_ELF:
		     /* gets taken care of later... */
		     /*(void)mongets(mtmp, ELVEN_SHORT_SWORD);
		     if (rn2(3)) {
			(void)mongets(mtmp, ELVEN_BOW);
			 m_initthrow(mtmp, ELVEN_ARROW, 25);
		     }
		     if (rn2(3)) (void)mongets(mtmp, ELVEN_CLOAK);
		     else (void)mongets(mtmp, ELVEN_MITHRIL_COAT);*/
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_FLAME_MAGE:
		   case PM_UNDEAD_FLAME_MAGE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, STUDDED_LEATHER_ARMOR);
		     (void)mongets(mtmp, WAN_FIRE);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_FIREFIGHTER:
		   case PM_UNDEAD_FIREFIGHTER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, AXE);
		     (void)mongets(mtmp, WAN_FIRE);
		   break;

		   case PM_ACID_MAGE:
		   case PM_UNDEAD_ACID_MAGE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, STUDDED_LEATHER_ARMOR);
		     (void)mongets(mtmp, WAN_ACID);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_UNDEAD_ROCKER: /* needs to be filled */
		   case PM_ROCKER: /* needs to be filled */
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

			(void)mongets(mtmp, SLING);
			 m_initthrow(mtmp, FLINT, 25);
			(void)mongets(mtmp, PICK_AXE);

			struct obj *otmp = mksobj(EGG,TRUE,FALSE);
			otmp->spe = 0;
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			otmp->corpsenm = egg_type_from_parent(PM_COCKATRICE, FALSE);
			attach_egg_hatch_timeout(otmp);
			kill_egg(otmp); /* make sure they're stale --Amy */
			mpickobj(mtmp,otmp);

		   break;

		   case PM_CONVICT:
		   case PM_UNDEAD_CONVICT:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, HEAVY_IRON_BALL);
		     (void)mongets(mtmp, STRIPED_SHIRT);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);
		     (void)mongets(mtmp, ROCK);

		   break;

		   case PM_SPACEWARS_FIGHTER:
		   case PM_UNDEAD_SPACEWARS_FIGHTER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, GREEN_LIGHTSABER);
		     (void)mongets(mtmp, DAGGER);
		     (void)mongets(mtmp, PLASTEEL_ARMOR);
		     if (!rn2(3)) (void)mongets(mtmp, WAN_SOLAR_BEAM);

		   break;

		   case PM_COURIER:
		   case PM_UNDEAD_COURIER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, PISTOL);
			 m_initthrow(mtmp, BULLET, 25);
		     (void)mongets(mtmp, KNIFE);

		   break;

		   case PM_ZYBORG:
		   case PM_UNDEAD_ZYBORG:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, PLATE_MAIL);
		     (void)mongets(mtmp, GREEN_LIGHTSABER);
		     (void)mongets(mtmp, WAN_SOLAR_BEAM);

		   break;

		   case PM_LUNATIC:
		   case PM_UNDEAD_LUNATIC:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, STEEL_SHIELD);
		     (void)mongets(mtmp, STEEL_WHIP);

		   break;

		   case PM_GANGSTER:
		   case PM_UNDEAD_GANGSTER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, SUBMACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, LEATHER_JACKET);

		   break;

		   case PM_DEATH_EATER:
		   case PM_UNDEAD_DEATH_EATER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, SHOTGUN);
			 m_initthrow(mtmp, SHOTGUN_SHELL, 10);
		     (void)mongets(mtmp, CLOAK_OF_REFLECTION);
		     if (!rn2(10)) (void) mongets(mtmp, WAN_DEATH);
		     if (!rn2(10)) (void) mongets(mtmp, AMULET_OF_LIFE_SAVING);

		   break;

		   case PM_POKEMON:
		   case PM_UNDEAD_POKEMON:
		   case PM_MUSICIAN:
		   case PM_UNDEAD_MUSICIAN:
		   case PM_GRADUATE:
		   case PM_UNDEAD_GRADUATE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		   break;

		   case PM_WARRIOR:
		   case PM_UNDEAD_WARRIOR:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, MACE);
		     (void)mongets(mtmp, CROSSBOW);
			 m_initthrow(mtmp, CROSSBOW_BOLT, 50);
		     (void)mongets(mtmp, LARGE_SHIELD);

			otmp = mksobj(rnd_class(PLATE_MAIL,YELLOW_DRAGON_SCALES),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

		   break;

		   case PM_NOBLEMAN:
		   case PM_UNDEAD_NOBLEMAN:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, RAPIER);
		     (void)mongets(mtmp, RUFFLED_SHIRT);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, LEATHER_CLOAK);

		   break;

		   case PM_NOBLEWOMAN:
		   case PM_UNDEAD_NOBLEWOMAN:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, RAPIER);
		     (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, LEATHER_CLOAK);

		   break;

		   case PM_DRUNK:
		   case PM_UNDEAD_DRUNK:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, RUBBER_HOSE);

		   break;

		   case PM_PIRATE:
		   case PM_UNDEAD_PIRATE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, SCIMITAR);
		     (void)mongets(mtmp, PISTOL);
		     (void)mongets(mtmp, KNIFE);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, LEATHER_JACKET);
		     (void)mongets(mtmp, HIGH_BOOTS);

		   break;

		   case PM_OFFICER:
		   case PM_UNDEAD_OFFICER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, CLUB);
		     (void)mongets(mtmp, PISTOL);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, LEATHER_ARMOR);
		     (void)mongets(mtmp, rn2(2) ? WAN_STRIKING : WAN_MAGIC_MISSILE);

		   break;

		   case PM_GLADIATOR:
		   case PM_UNDEAD_GLADIATOR:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, SILVER_SPEAR);
		     (void)mongets(mtmp, SHIELD_OF_REFLECTION);

		   break;

		   case PM_GOFF:
		   case PM_UNDEAD_GOFF:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, HIPPIE_HEELS);
		     (void)mongets(mtmp, PISTOL);
			 m_initthrow(mtmp, BULLET, 50);
		     (void)mongets(mtmp, POT_ACID);
		     (void)mongets(mtmp, POT_ACID);

		   break;

		   case PM_KORSAIR:
		   case PM_UNDEAD_KORSAIR:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     (void)mongets(mtmp, DAGGER);
		     (void)mongets(mtmp, SCIMITAR);

		   break;

		   case PM_DIVER:
		   case PM_UNDEAD_DIVER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, TRIDENT);
		     (void)mongets(mtmp, SPEAR);
		     (void)mongets(mtmp, OILSKIN_CLOAK);

		   break;

		   case PM_CHEVALIER:
		   case PM_UNDEAD_CHEVALIER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, TRIDENT);
		     (void)mongets(mtmp, ROBE);
		     (void)mongets(mtmp, SMALL_SHIELD);

		   break;

		   case PM_TRANSVESTITE:
		   case PM_UNDEAD_TRANSVESTITE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

			otmp = mksobj(rnd_class(WEDGED_LITTLE_GIRL_SANDAL,SPIKED_BATTLE_BOOT),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(rnd_class(WEDGE_SANDALS,COMBAT_STILETTOS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

		   break;

		   case PM_TOPMODEL:
		   case PM_UNDEAD_TOPMODEL:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, KNIFE);
		     (void)mongets(mtmp, RIFLE);
			 m_initthrow(mtmp, BULLET, 50);

			otmp = mksobj(rnd_class(WEDGE_SANDALS,COMBAT_STILETTOS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

		   break;

		   case PM_ACTIVISTOR:
		   case PM_UNDEAD_ACTIVISTOR:

			if (moves < 2) mtmp->female = 1;

			if (mtmp->female) (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
			else (void)mongets(mtmp, RUFFLED_SHIRT);

		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, FLY_SWATTER);

		   break;


		   case PM_ELECTRIC_MAGE:
		   case PM_UNDEAD_ELECTRIC_MAGE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, STUDDED_LEATHER_ARMOR);
		     (void)mongets(mtmp, WAN_LIGHTNING);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_HEALER:
		   case PM_UNDEAD_HEALER:
		   case PM_SCIENTIST:
		   case PM_UNDEAD_SCIENTIST:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, SCALPEL);
		     (void)mongets(mtmp, LEATHER_GLOVES);
		     (void)mongets(mtmp, WAN_HEALING);
		     (void)mongets(mtmp, WAN_SLEEP);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 20);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 20));
#endif
		   break;
		   case PM_ICE_MAGE:
		   case PM_UNDEAD_ICE_MAGE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, QUARTERSTAFF);
		     (void)mongets(mtmp, STUDDED_LEATHER_ARMOR);
		     (void)mongets(mtmp, WAN_COLD);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
#ifdef YEOMAN
		   case PM_YEOMAN:
#endif
		   case PM_UNDEAD_YEOMAN:
		   case PM_KNIGHT:
		   case PM_UNDEAD_KNIGHT:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     (void)mongets(mtmp, LONG_SWORD);
		     (void)mongets(mtmp, PLATE_MAIL);
		     (void)mongets(mtmp, LARGE_SHIELD);
		     (void)mongets(mtmp, HELMET);
		     (void)mongets(mtmp, LEATHER_GLOVES);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		     break;
		   case PM_MONK:
		   case PM_UNDEAD_MONK:
		   case PM_PSION:
		   case PM_UNDEAD_PSION:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));


#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 5);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 5));
#endif
		   break;
		   case PM_NECROMANCER:
		   case PM_UNDEAD_NECROMANCER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

		     (void)mongets(mtmp, ATHAME);
		     (void)mongets(mtmp, SCR_SUMMON_UNDEAD);
		     if (!rn2(10)) (void)mongets(mtmp, WAN_SUMMON_UNDEAD);
		     if (!rn2(4)) (void)mongets(mtmp, PICK_AXE);
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     (void) mongets(mtmp, rnd_defensive_item(mtmp));
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_PRIEST:
		   case PM_PRIESTESS:
		   case PM_UNDEAD_PRIEST:
		   case PM_UNDEAD_PRIESTESS:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, MACE);
		     (void)mongets(mtmp, rn1(ROBE_OF_WEAKNESS - ROBE + 1, ROBE));
		     (void)mongets(mtmp, SMALL_SHIELD);
		     if (!rn2(4)) {
			int v,vials;
			vials = rn2(4)+1;
			for (v=0;v<vials;v++) {
			  otmp = mksobj(POT_WATER, FALSE, FALSE);
			  bless(otmp);
			  mpickobj(mtmp, otmp);
			}
		     }
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_ROGUE:
		   case PM_UNDEAD_ROGUE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, SHORT_SWORD);
		     (void)mongets(mtmp, LEATHER_ARMOR);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 25);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 25));
#endif
		   break;

		   case PM_LOCKSMITH:
		   case PM_UNDEAD_LOCKSMITH:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		   break;

		   case PM_RANGER:
		   case PM_ELPH:
		   case PM_UNDEAD_RANGER:
		   case PM_UNDEAD_ELPH:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, ELVEN_DAGGER);
		     (void)mongets(mtmp, ELVEN_BOW);
			    m_initthrow(mtmp, ELVEN_ARROW, 25);

		   break;

		   case PM_NINJA:
		   case PM_UNDEAD_NINJA:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, SHORT_SWORD);
		     (void)mongets(mtmp, POT_INVISIBILITY);
			m_initthrow(mtmp, SHURIKEN, 25);
		   break;

		   case PM_SAMURAI:
		   case PM_UNDEAD_SAMURAI:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     (void)mongets(mtmp, KATANA);
		     if (rn2(2)) (void)mongets(mtmp, SHORT_SWORD);
		     if (rn2(3)) {
			(void)mongets(mtmp, YUMI);
			m_initthrow(mtmp, YA, 25);
		     }
		     (void)mongets(mtmp, SPLINT_MAIL);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
#ifdef TOURIST
		   case PM_TOURIST:
		   case PM_UNDEAD_TOURIST:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     m_initthrow(mtmp, DART, 20);
		     (void)mongets(mtmp, HAWAIIAN_SHIRT);
		     if (rn2(2)) (void)mongets(mtmp, EXPENSIVE_CAMERA);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 20);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 20));
#endif
		   break;
#endif
		   case PM_UNDEAD_SLAYER:
		   case PM_NON_UNDEAD_SLAYER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, SILVER_SPEAR);
		     (void)mongets(mtmp, CHAIN_MAIL);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;

		   case PM_UNDERTAKER:
		   case PM_UNDEAD_UNDERTAKER:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, SCALPEL);
		     (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
		   break;

		   case PM_VALKYRIE:
		   case PM_UNDEAD_VALKYRIE:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));

		     (void)mongets(mtmp, LONG_SWORD);
		     if (!rn2(3)) m_initthrow(mtmp, DAGGER, 4);
		     (void)mongets(mtmp, SMALL_SHIELD);
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   case PM_WIZARD:
		   case PM_UNDEAD_WIZARD:
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(20)) (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));

		     (void)mongets(mtmp, ATHAME);
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		     (void) mongets(mtmp, rnd_defensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
#ifndef GOLDOBJ
		     mtmp->mgold = (long) d(mtmp->m_lev, 15);
#else
		     mkmonmoney(mtmp, (long) d(mtmp->m_lev, 15));
#endif
		   break;
		   default:
		   break;
		  } 
		  if ((int) mtmp->m_lev > rn2(40))
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		  if ((int) mtmp->m_lev > rn2(40))
		     (void) mongets(mtmp, rnd_offensive_item(mtmp));
		  if ((int) mtmp->m_lev > rn2(40))
		     (void) mongets(mtmp, rnd_defensive_item(mtmp));
		  if ((int) mtmp->m_lev > rn2(40))
		     (void) mongets(mtmp, rnd_defensive_item(mtmp));
		  if ((int) mtmp->m_lev > rn2(40))
		     (void) mongets(mtmp, rnd_misc_item(mtmp));
		  if ((int) mtmp->m_lev > rn2(40))
		     (void) mongets(mtmp, rnd_misc_item(mtmp));
		     if (!rn2(200)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(200)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(200)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		     if (!rn2(200)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(200)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
		     if (!rn2(200)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
		 } /* end of other characters */
	       /*break;*/
		else if (ptr->msound == MS_PRIEST ||
			quest_mon_represents_role(ptr,PM_PRIEST)) {

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		    otmp = mksobj(MACE, FALSE, FALSE);
		    if(otmp) {
			otmp->spe = rnd(3);
			if(!rn2(2)) curse(otmp);
			(void) mpickobj(mtmp, otmp);
		    }

		} else if (mm == PM_PIRATE_BROTHER){
			(void)mongets(mtmp, SCIMITAR);
			(void)mongets(mtmp, LEATHER_ARMOR);
			(void)mongets(mtmp, HIGH_BOOTS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_SIZZLE){
		    otmp = mksobj(LONG_SWORD, FALSE, FALSE);

			int spe2;
			otmp = oname(otmp, artiname(ART_VORPAL_BLADE));
		    bless(otmp);
		    otmp->oerodeproof = TRUE;
		    spe2 = rn2(4);
		    otmp->spe = max(otmp->spe, spe2);
		    (void) mpickobj(mtmp, otmp);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}
		(void) tamedog(mtmp, (struct obj *)0); /* supposed to start out tame */

		} else if (mm == PM_ARABELLA){

			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, SPEED_BOOTS);
			(void)mongets(mtmp, WAN_BANISHMENT);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ANASTASIA){

			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, DANCING_SHOES);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_HENRIETTA){

			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, PLASTEEL_BOOTS);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_KATRIN){

			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, FEMININE_PUMPS);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_JANA){

			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, ELVEN_BOOTS);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_THE_CHAMPION_OF_POMPEJI){

			(void)mongets(mtmp, STYGIAN_PIKE);
			(void)mongets(mtmp, STONE_DRAGON_SCALE_MAIL);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DEATH_METAL_FREAK){
			if (rn2(2)) (void)mongets(mtmp, rn2(2) ? FIRE_HORN : FROST_HORN);
			(void)mongets(mtmp, SCR_LOCKOUT);

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}




		} else if (mm == PM_ACTIVIST){

			if (mtmp->female) (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
			else (void)mongets(mtmp, RUFFLED_SHIRT);

			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ACTIVIST_LEADER){

			if (mtmp->female) (void)mongets(mtmp, VICTORIAN_UNDERWEAR);
			else (void)mongets(mtmp, RUFFLED_SHIRT);

			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			otmp = mksobj(rnd_class(SCR_LAVA,SCR_LOCKOUT), TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DEATH_METAL_DRUMMER){
			if (rn2(2)) (void)mongets(mtmp, rn2(2) ? FIRE_HORN : FROST_HORN);
			(void)mongets(mtmp, SCR_LOCKOUT);
		     (void)mongets(mtmp, SUBMACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DEATH_METAL_ORCHESTRA_LEADER){
			if (rn2(2)) (void)mongets(mtmp, rn2(2) ? FIRE_HORN : FROST_HORN);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void)mongets(mtmp, SCR_LOCKOUT);
			(void)mongets(mtmp, SCR_LOCKOUT);
		     (void)mongets(mtmp, HEAVY_MACHINE_GUN);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

		} else if (mm == PM_DRACO_THE_SHARPSHOOTER){
		     (void)mongets(mtmp, SNIPER_RIFLE);
			 m_initthrow(mtmp, BULLET, 50);
			 m_initthrow(mtmp, BULLET, 50);

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

		} else if (mm == PM_GRANDPA_TANK_BREAKER){
		     (void)mongets(mtmp, ROCKET_LAUNCHER);
			 m_initthrow(mtmp, ROCKET, 10);
		     (void)mongets(mtmp, REALLY_HEAVY_IRON_BALL);

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

		} else if (mm == PM_DANCING_GUY){
			(void)mongets(mtmp, DANCING_SHOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DANCING_GIRL){
			(void)mongets(mtmp, DANCING_SHOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DANCING_KING){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, DANCING_SHOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DANCING_QUEEN){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, DANCING_SHOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ASIAN_GIRL){
			(void)mongets(mtmp, HUGGING_BOOT);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ESTRELLA_GIRL){
			(void)mongets(mtmp, SWEET_MOCASSINS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DARK_GIRL){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DARK_WOMAN){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			(void)mongets(mtmp, DANCING_SHOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DARK_LADY){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_REDGUARD_GIRL){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_REDGUARD_WOMAN){
			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, WEDGE_SANDALS);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_REDGUARD_LADY){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);
			(void)mongets(mtmp, AMULET_OF_LIFE_SAVING);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_THIEVING_GIRL){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_THIEVING_WOMAN){
			(void)mongets(mtmp, LACQUERED_DANCING_SHOE);
			(void)mongets(mtmp, SOFT_SNEAKERS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_THIEVING_LADY){
			(void)mongets(mtmp, SPIKED_BATTLE_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_KICKBOXING_GIRL){
			(void)mongets(mtmp, SOFT_SNEAKERS);
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_KICKBOXING_LADY){
			(void)mongets(mtmp, SOFT_SNEAKERS);
			(void)mongets(mtmp, WOODEN_GETA);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ESTRELLARINA){
			(void)mongets(mtmp, SWEET_MOCASSINS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_PROSTITUTE){
			(void)mongets(mtmp, BULLWHIP);
			(void)mongets(mtmp, HIPPIE_HEELS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_BLACK_PROSTITUTE){
			(void)mongets(mtmp, FLY_SWATTER);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, VICTORIAN_UNDERWEAR);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_GREEN_PROSTITUTE){
			(void)mongets(mtmp, KNIFE);
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, ROBE);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_STUDENT_ASSISTANT){
			(void)mongets(mtmp, HIPPIE_HEELS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_STUDENT_LEADER){
			(void)mongets(mtmp, HIPPIE_HEELS);
			(void)mongets(mtmp, SCR_CREATE_MONSTER);
			(void)mongets(mtmp, SCR_CREATE_MONSTER);
			(void)mongets(mtmp, SCR_CREATE_MONSTER);
			(void)mongets(mtmp, WAN_CREATE_HORDE);
			(void)mongets(mtmp, WAN_SUMMON_UNDEAD);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_HIPPIE_LADY){
			(void)mongets(mtmp, HIPPIE_HEELS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_HIPPIE_GUY){
			(void)mongets(mtmp, HIPPIE_HEELS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_SWEET_WOMAN){
			(void)mongets(mtmp, SWEET_MOCASSINS);
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_PETTY_SWEET_WOMAN){
			(void)mongets(mtmp, SWEET_MOCASSINS);
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_LITTLE_GIRL){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_LITTLE_BOY){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ASIAN_WOMAN){
			(void)mongets(mtmp, HUGGING_BOOT);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ASIAN_LADY){
			(void)mongets(mtmp, WOODEN_GETA);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_YOUNG_ADULT_GIRL){
			(void)mongets(mtmp, STURDY_PLATEAU_BOOT_FOR_GIRLS);
			(void)mongets(mtmp, WEDGE_SANDALS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_YOUNG_ADULT_BOY){
			(void)mongets(mtmp, SOFT_GIRL_SNEAKER);
			(void)mongets(mtmp, WEDGE_SANDALS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_YOUNG_WOMAN){
			(void)mongets(mtmp, HIGH_HEELED_SANDAL);
			(void)mongets(mtmp, FEMININE_PUMPS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_YOUNG_MAN){
			(void)mongets(mtmp, HIGH_HEELED_SANDAL);
			(void)mongets(mtmp, WEDGE_SANDALS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ADULT_LADY){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_ADULT_GENTLEMAN){
			(void)mongets(mtmp, SPIKED_BATTLE_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_OLD_GRANNY){
			(void)mongets(mtmp, SEXY_LEATHER_PUMP);
			(void)mongets(mtmp, FEMININE_PUMPS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_OLD_GRANDPA){
			(void)mongets(mtmp, WEDGED_LITTLE_GIRL_SANDAL);
			(void)mongets(mtmp, COMBAT_STILETTOS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_CHARMING_SCRATCHER){
			(void)mongets(mtmp, FEMININE_PUMPS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_KUNG_FU_GIRL){
			(void)mongets(mtmp, WEDGE_SANDALS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_NINJA_GIRL){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_NINJA_BOY){
			(void)mongets(mtmp, BLOCK_HEELED_COMBAT_BOOT);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_TAEKWONDO_GIRL){
			(void)mongets(mtmp, WOODEN_GETA);
			(void)mongets(mtmp, LEATHER_PEEP_TOES);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_BLACKBELT){
			(void)mongets(mtmp, SPIKED_BATTLE_BOOT);
			(void)mongets(mtmp, COMBAT_STILETTOS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_KUNG_FU_GUY){
			(void)mongets(mtmp, WEDGE_SANDALS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_FLYING_GIRL){
			(void)mongets(mtmp, COMBAT_STILETTOS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_FLYING_GUY){
			(void)mongets(mtmp, COMBAT_STILETTOS);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_DRAGONBALL_KID){

			otmp = mksobj(rnd_class(PLASTEEL_BOOTS,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_KAMEHAMEHA_FIGHTER){

			otmp = mksobj(rnd_class(PLASTEEL_BOOTS,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			if (!rn2(5)) mongets(mtmp, rnd_offensive_item_new(mtmp));

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_SUPER_SAIYAN){

			mongets(mtmp, rnd_offensive_item_new(mtmp));

			otmp = mksobj(rnd_class(PLASTEEL_BOOTS,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

		} else if (mm == PM_MAYOR_CUMMERBUND){

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


			int spe2;
		    otmp = mksobj(SCIMITAR, FALSE, FALSE);
		    curse(otmp);
		    otmp->oerodeproof = TRUE;
		    spe2 = d(1,3);
		    otmp->spe = max(otmp->spe, spe2);
		    (void) mpickobj(mtmp, otmp);
			
		    otmp = mksobj(LEATHER_JACKET, FALSE, FALSE);
		    otmp->oerodeproof = TRUE;
		    spe2 = d(2,3);
		    otmp->spe = max(otmp->spe, spe2);
		    (void) mpickobj(mtmp, otmp);
			
		    otmp = mksobj(SMALL_SHIELD, FALSE, FALSE);
		    otmp->oerodeproof = TRUE;
		    spe2 = d(1,3);
		    otmp->spe = max(otmp->spe, spe2);
		    (void) mpickobj(mtmp, otmp);

			(void)mongets(mtmp, LEATHER_CLOAK);
			(void)mongets(mtmp, HIGH_BOOTS);
			(void)mongets(mtmp, LEATHER_GLOVES);
		}

#ifdef CONVICT
		else if (mm == PM_MINER) {

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		    (void)mongets(mtmp, PICK_AXE);
		    otmp = mksobj(BRASS_LANTERN, TRUE, FALSE);
			(void) mpickobj(mtmp, otmp);
            begin_burn(otmp, FALSE);
		}
#endif /* CONVICT */

		break;
		case S_GHOST:
		if(mm == PM_BLACKBEARD_S_GHOST){

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


			int spe2;
		    otmp = mksobj(SCIMITAR, FALSE, FALSE);
		    curse(otmp);
		    otmp->oerodeproof = TRUE;
			otmp->oeroded = 1;
		    spe2 = d(2,3);
		    otmp->spe = max(otmp->spe, spe2);
		    (void) mpickobj(mtmp, otmp);

		    /* MRKR: Dwarves in the Mines sometimes carry torches */

		    if (In_mines(&u.uz)) {
		      if (!rn2(4)) {	
			otmp = mksobj(TORCH, TRUE, FALSE);
			otmp->quan = 1;
			(void) mpickobj(mtmp, otmp);

			/* If this spot is unlit, light the torch */

			if (!levl[mtmp->mx][mtmp->my].lit) {
			  begin_burn(otmp, FALSE);
			}		      
		      }
		    }
		} else if (mm == PM_IMELDA_S_GHOST) {

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		    (void)mongets(mtmp, LEATHER_PEEP_TOES);
		} 
		break;

	    case S_ANGEL:
		{

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		    int spe2;

		    /* create minion stuff; can't use mongets */
		    otmp = mksobj(LONG_SWORD, FALSE, FALSE);

		    /* maybe make it special */
		    if (!rn2(20) || is_lord(ptr))
			otmp = oname(otmp, artiname(
				rn2(2) ? ART_DEMONBANE : ART_SUNSWORD));
		    bless(otmp);
		    otmp->oerodeproof = TRUE;
		    spe2 = rn2(4);
		    otmp->spe = max(otmp->spe, spe2);
		    (void) mpickobj(mtmp, otmp);

		    otmp = mksobj(!rn2(4) || is_lord(ptr) ?
				  SHIELD_OF_REFLECTION : LARGE_SHIELD,
				  FALSE, FALSE);
		    otmp->cursed = FALSE;
		    otmp->oerodeproof = TRUE;
		    otmp->spe = 0;
		    (void) mpickobj(mtmp, otmp);
		}
		break;

	    case S_GOLEM:
		{
			if (mm == PM_ANIMATED_WEDGE_SANDAL) (void)mongets(mtmp, WEDGE_SANDALS);
			else if (mm == PM_ANIMATED_SEXY_LEATHER_PUMP) (void)mongets(mtmp, FEMININE_PUMPS);
			else if (mm == PM_ANIMATED_LEATHER_PEEP_TOE) (void)mongets(mtmp, LEATHER_PEEP_TOES);
			else if (mm == PM_ANIMATED_COMBAT_STILETTO) (void)mongets(mtmp, COMBAT_STILETTOS);

		}
		break;

	    case S_IMP:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		switch (mm) {
			case PM_LEGION_DEVIL_GRUNT:



#ifdef FIREARMS
			  (void) mongets(mtmp, rn2(2) ? RIFLE : SUBMACHINE_GUN);
		  	  m_initthrow(mtmp, BULLET, 25);
		  	  m_initthrow(mtmp, BULLET, 25);
			  (void) mongets(mtmp, rn2(2) ? KNIFE : DAGGER);
			  (void) mongets(mtmp, FRAG_GRENADE);
			  break;
#endif
			case PM_LEGION_DEVIL_SOLDIER:



#ifdef FIREARMS
			  if (rn2(2)) {
			  	(void) mongets(mtmp, HEAVY_MACHINE_GUN);
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  	m_initthrow(mtmp, BULLET, 50);
			  } else {
			  	(void) mongets(mtmp, SUBMACHINE_GUN);
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  (void) mongets(mtmp, rn2(2) ? KNIFE : DAGGER);
			  if (rn2(2)) {
			  	(void) mongets(mtmp, FRAG_GRENADE);
			  	(void) mongets(mtmp, FRAG_GRENADE);
			  } else {
			  	(void) mongets(mtmp, GAS_GRENADE);
			  	(void) mongets(mtmp, GAS_GRENADE);
			  }
			  break;
#endif
			case PM_LEGION_DEVIL_SERGEANT:



#ifdef FIREARMS
			  if (rn2(2)) {
			  	(void) mongets(mtmp, AUTO_SHOTGUN);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 10);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 10);
			  } else {
			  	(void) mongets(mtmp, ASSAULT_RIFLE);
			  	m_initthrow(mtmp, BULLET, 30);
			  	m_initthrow(mtmp, BULLET, 30);
			  }
			  (void) mongets(mtmp, rn2(2) ? DAGGER : KNIFE);
			  if (rn2(2)) {
			  	m_initthrow(mtmp, FRAG_GRENADE, 5);
			  } else {
			  	m_initthrow(mtmp, GAS_GRENADE, 5);
			  }
			  if (!rn2(5)) (void) mongets(mtmp, GRENADE_LAUNCHER);
			  break;
#endif
			case PM_LEGION_DEVIL_CAPTAIN:

#ifdef FIREARMS
			  if (rn2(2)) {
			  	(void) mongets(mtmp, AUTO_SHOTGUN);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  	m_initthrow(mtmp, SHOTGUN_SHELL, 20);
			  } else if (rn2(2)) {
			  	(void) mongets(mtmp, HEAVY_MACHINE_GUN);
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  } else {
			  	(void) mongets(mtmp, ASSAULT_RIFLE);
			  	m_initthrow(mtmp, BULLET, 60);
			  	m_initthrow(mtmp, BULLET, 60);
			  }
			  if (rn2(2)) {
				  (void) mongets(mtmp, ROCKET_LAUNCHER);
			  	  m_initthrow(mtmp, ROCKET, 5);
			  } else if (rn2(2)) {
				  (void) mongets(mtmp, GRENADE_LAUNCHER);			  
			  	  m_initthrow(mtmp, 
			  	  	(rn2(2) ? FRAG_GRENADE : GAS_GRENADE), 
			  	  	5);
			  } else {
				  (void) mongets(mtmp, rn2(2) ? SILVER_SABER : DAGGER);
			  }
			  break;
#endif
		    default:
			m_initweap_normal(mtmp);
			break;
		}	
		break;

	    case S_GNOME:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		switch (mm) {
		    case PM_GNOLL:
			if(!rn2(3)) (void) mongets(mtmp, ORCISH_HELM);
			if(!rn2(3)) (void) mongets(mtmp, STUDDED_LEATHER_ARMOR);
			if(!rn2(3)) (void) mongets(mtmp, ORCISH_SHIELD);
			if(!rn2(4)) (void) mongets(mtmp, SPEAR);
			break;

		    case PM_GNOLL_WARRIOR:
			if(!rn2(2)) (void) mongets(mtmp, ORCISH_HELM);

			if (!rn2(20))
			    (void) mongets(mtmp, ORANGE_DRAGON_SCALE_MAIL);
			else if (rn2(3))
			    (void) mongets(mtmp, SCALE_MAIL);
			else
			    (void) mongets(mtmp, SPLINT_MAIL);

			if(!rn2(2)) (void) mongets(mtmp, ORCISH_SHIELD);
		 	if(!rn2(3)) (void) mongets(mtmp, KATANA);
			break;

		    case PM_GNOLL_CHIEFTAIN:
			(void) mongets(mtmp, ORCISH_HELM);

			if (!rn2(10))
			    (void) mongets(mtmp, BLUE_DRAGON_SCALE_MAIL);
			else
			    (void) mongets(mtmp, CRYSTAL_PLATE_MAIL);

			(void) mongets(mtmp, ORCISH_SHIELD);
			(void) mongets(mtmp, KATANA);
			(void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
			break;

		    case PM_GNOLL_SHAMAN:
			if (!rn2(10))
			    (void) mongets(mtmp, SILVER_DRAGON_SCALE_MAIL);
			else if (rn2(5))
			    (void) mongets(mtmp, CRYSTAL_PLATE_MAIL);
			else
			    (void) mongets(mtmp, RED_DRAGON_SCALE_MAIL);

			(void) mongets(mtmp, ATHAME);
			m_initthrow(mtmp, SHURIKEN, 40);
			(void) mongets(mtmp, rnd_offensive_item(mtmp));
			(void) mongets(mtmp, rnd_offensive_item(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
		     if (!rn2(100)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
			break;

		    default:
			m_initweap_normal(mtmp);
			break;
		}	
		break;
	    case S_HUMANOID:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		if (is_dwarf(ptr)) {
		    if (rn2(7)) (void)mongets(mtmp, DWARVISH_CLOAK);
		    if (rn2(7)) (void)mongets(mtmp, IRON_SHOES);
		    if (!rn2(4)) {
			(void)mongets(mtmp, DWARVISH_SHORT_SWORD);
			/* note: you can't use a mattock with a shield */
			if (rn2(2)) (void)mongets(mtmp, DWARVISH_MATTOCK);
			else {
				(void)mongets(mtmp, AXE);
				(void)mongets(mtmp, DWARVISH_ROUNDSHIELD);
			}
			(void)mongets(mtmp, DWARVISH_IRON_HELM);
			if (!rn2(4))
			    (void)mongets(mtmp, DWARVISH_MITHRIL_COAT);
		    } else {
			(void)mongets(mtmp, !rn2(3) ? PICK_AXE : DAGGER);
		    }
		} else if(mm == PM_GITHYANKI_PIRATE){
			(void)mongets(mtmp, TWO_HANDED_SWORD);
			(void)mongets(mtmp, BRONZE_PLATE_MAIL);
			(void)mongets(mtmp, LEATHER_GLOVES);
			(void)mongets(mtmp, HIGH_BOOTS);
		} else if (is_hobbit(ptr)) {
			    switch (rn2(3)) {
			        case 0:
				    (void)mongets(mtmp, DAGGER);
				    break;
				case 1:
				    (void)mongets(mtmp, ELVEN_DAGGER);
				    break;
				case 2:
			            (void)mongets(mtmp, SLING);
			            /* WAC give them some rocks to throw */
			            m_initthrow(mtmp, ROCK, 2); 
						break;
			      }
				/* WAC add 50% chance of leather */
			    if (!rn2(10)) (void)mongets(mtmp, ELVEN_MITHRIL_COAT);
				else if (!rn2(2)) (void)mongets(mtmp, LEATHER_ARMOR);
				if (!rn2(10)) (void)mongets(mtmp, DWARVISH_CLOAK);
		} else switch(mm) {
          /* Mind flayers get robes */
          case PM_MIND_FLAYER:
            if (!rn2(2)) (void)mongets(mtmp, ROBE);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


            break;
          case PM_GITHYANKI_PIRATE:
            (void)mongets(mtmp, SCIMITAR);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


            break;
          case PM_ANUBITE:
            (void)mongets(mtmp, TRIDENT);
            (void)mongets(mtmp, RING_MAIL);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


            break;
          case PM_MASTER_MIND_FLAYER:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


            if (!rn2(10)) (void)mongets(mtmp, ROBE_OF_PROTECTION);
			else if (!rn2(10)) (void)mongets(mtmp, ROBE_OF_POWER);
			else (void)mongets(mtmp, ROBE);
            break;
		  case PM_GNOLL:
			if (!rn2(2)) switch (rn2(3)) {
				case 0: (void)mongets(mtmp, BARDICHE); break;
				case 1: (void)mongets(mtmp, VOULGE); break;
				case 2: (void)mongets(mtmp, HALBERD); break;
			}
			if (!rn2(2)) (void)mongets(mtmp, LEATHER_ARMOR);
			break;
          default:
	          break;                     
		}
		break;
# ifdef KOPS
	    case S_KOP:		/* create Keystone Kops with cream pies to
				 * throw. As suggested by KAA.	   [MRS]
				 */
		if (!rn2(4)) m_initthrow(mtmp, CREAM_PIE, 2);
		if (!rn2(3)) (void)mongets(mtmp,(rn2(2)) ? CLUB : RUBBER_HOSE);

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		break;
# endif
	    case S_ORC:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


                /* All orcs will get at least an orcish dagger*/
		if(rn2(2)) (void)mongets(mtmp, ORCISH_HELM);
		switch (mm != PM_ORC_CAPTAIN ? mm :
			rn2(2) ? PM_MORDOR_ORC : PM_URUK_HAI) {
		    case PM_MORDOR_ORC:
			if(!rn2(3)) (void)mongets(mtmp, SCIMITAR);
                            else (void)mongets(mtmp, ORCISH_DAGGER);
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHIELD);
			if(!rn2(3)) (void)mongets(mtmp, KNIFE);
                        /* WAC add possible orcish spear */
                        if (!rn2(4)) m_initthrow(mtmp, ORCISH_SPEAR, 1);
			if(!rn2(3)) (void)mongets(mtmp, KNIFE);
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_CHAIN_MAIL);
			break;
		    case PM_URUK_HAI:
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_CLOAK);
			if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHORT_SWORD);
                            else (void)mongets(mtmp, ORCISH_DAGGER);
			if(!rn2(3)) (void)mongets(mtmp, IRON_SHOES);
			if(!rn2(3)) {
			    (void)mongets(mtmp, ORCISH_BOW);
			    m_initthrow(mtmp, ORCISH_ARROW, 25);
			}
			if(!rn2(3)) (void)mongets(mtmp, URUK_HAI_SHIELD);
			break;
                    case PM_GOBLIN:
                        if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHORT_SWORD);
                            else (void)mongets(mtmp, ORCISH_DAGGER);
			break;
                    case PM_HOBGOBLIN:
                        if(!rn2(3)) (void)mongets(mtmp, MORNING_STAR);
                            else if(!rn2(3)) (void)mongets(mtmp, ORCISH_SHORT_SWORD);
                            else (void)mongets(mtmp, ORCISH_DAGGER);
			break;
		    default:
			if (mm != PM_ORC_SHAMAN && rn2(2))
/*                          (void)mongets(mtmp, (mm == PM_GOBLIN || rn2(2) == 0)*/
                          (void)mongets(mtmp, (rn2(2) == 0)
						   ? ORCISH_DAGGER : SCIMITAR);
		}
		break;
	    case S_OGRE:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		if (!rn2(mm == PM_OGRE_KING ? 3 : mm == PM_OGRE_LORD ? 6 : 12))
		    (void) mongets(mtmp, BATTLE_AXE);
		else
		    (void) mongets(mtmp, CLUB);
		break;
	    case S_KOBOLD:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


                /* WAC gets orcish 1:4, otherwise darts
                        (used to be darts 1:4)
                   gets orcish short sword 1:4, otherwise orcish dagger */
                if (!rn2(4)) m_initthrow(mtmp, ORCISH_SPEAR, 1);
                   else m_initthrow(mtmp, DART, 20);
                if (!rn2(4)) mongets(mtmp, ORCISH_SHORT_SWORD);
                   else mongets(mtmp, ORCISH_DAGGER);
		break;

	    case S_CENTAUR:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		if (rn2(2)) {
		    if(ptr == &mons[PM_FOREST_CENTAUR]) {
			(void)mongets(mtmp, BOW);
			m_initthrow(mtmp, ARROW, 25);
		    } else {
			(void)mongets(mtmp, CROSSBOW);
			m_initthrow(mtmp, CROSSBOW_BOLT, 25);
		    }
		}
		break;
	    case S_UMBER:
		if(mm == PM_SHOTGUN_HORROR || mm == PM_SHOTGUN_TERROR) { (void)mongets(mtmp, CROSSBOW);
		m_initthrow(mtmp, CROSSBOW_BOLT, 25);	}
		break;
	    case S_WRAITH:
		(void)mongets(mtmp, KNIFE);
		(void)mongets(mtmp, LONG_SWORD);
			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		break;
	    case S_ZOMBIE:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		if(mm == PM_SKELETAL_PIRATE){
		    otmp = rn2(2) ? mksobj(SCIMITAR, FALSE, FALSE) : mksobj(KNIFE, FALSE, FALSE);
		    curse(otmp);
			otmp->oeroded = 1;
		    (void) mpickobj(mtmp, otmp);
			
		    otmp = rn2(2) ? mksobj(HIGH_BOOTS, FALSE, FALSE) : mksobj(LEATHER_JACKET, FALSE, FALSE);
		    curse(otmp);
			otmp->oeroded2 = 1;
		    (void) mpickobj(mtmp, otmp);
			break;
		}
		if (!rn2(4)) (void)mongets(mtmp, LEATHER_ARMOR);
		if (!rn2(4))
			(void)mongets(mtmp, (rn2(3) ? KNIFE : SHORT_SWORD));
		break;
	    case S_LIZARD:
		if (mm == PM_SALAMANDER)
			(void)mongets(mtmp, (rn2(7) ? SPEAR : rn2(3) ?
					     TRIDENT : STILETTO));
		break;
	    case S_TROLL:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		if (!rn2(2)) switch (rn2(4)) {
		    case 0: (void)mongets(mtmp, RANSEUR); break;
		    case 1: (void)mongets(mtmp, PARTISAN); break;
		    case 2: (void)mongets(mtmp, GLAIVE); break;
		    case 3: (void)mongets(mtmp, SPETUM); break;
		}
		break;
	    case S_DEMON:

			if (!rn2(20) ) {

			otmp = mksobj(rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}

			if (!rn2(100) ) {

			otmp = mksobj(rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS),
				      TRUE, FALSE);
			otmp->quan = 1;
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
			}


		switch (mm) {
			case PM_DAMNED_PIRATE:
				otmp = mksobj(SCIMITAR, FALSE, FALSE);
				curse(otmp);
				(void) mpickobj(mtmp, otmp);
				
				otmp = mksobj(LEATHER_ARMOR, FALSE, FALSE);
				curse(otmp);
				otmp->oeroded = 1;
				(void) mpickobj(mtmp, otmp);
			break;
		    case PM_BALROG:
			(void)mongets(mtmp, BULLWHIP);
			(void)mongets(mtmp, BROADSWORD);
			break;
		    case PM_ORCUS:
			(void)mongets(mtmp, WAN_DEATH); /* the Wand of Orcus */
			break;
		    case PM_HORNED_DEVIL:
			(void)mongets(mtmp, rn2(4) ? TRIDENT : BULLWHIP);
			break;
		    case PM_DISPATER:
			(void)mongets(mtmp, WAN_STRIKING);
			break;
		    case PM_YEENOGHU:
			(void)mongets(mtmp, FLAIL);
			break;
		}
		/* prevent djinnis and mail daemons from leaving objects when
		 * they vanish
		 */
		if (!is_demon(ptr) && mm != PM_DAMNED_PIRATE) break;
		/* fall thru */
/*
 *	Now the general case, Some chance of getting some type
 *	of weapon for "normal" monsters.  Certain special types
 *	of monsters will get a bonus chance or different selections.
 */
	    default:
	      m_initweap_normal(mtmp);
	      break;
	}
/*    if ((int) mtmp->m_lev > rn2(120)) */        


      if ((int) mtmp->m_lev > rn2(200))
		(void) mongets(mtmp, rnd_offensive_item(mtmp));
	if (!rn2(400)) (void) mongets(mtmp, rnd_offensive_item_new(mtmp));
}

#endif /* OVL2 */
#ifdef OVL1

#ifdef GOLDOBJ
/*
 *   Makes up money for monster's inventory.
 *   This will change with silver & copper coins
 */
void 
mkmonmoney(mtmp, amount)
struct monst *mtmp;
long amount;
{
    struct obj *gold = mksobj(GOLD_PIECE, FALSE, FALSE);
    gold->quan = amount;
    add_to_minv(mtmp, gold);
}
#endif

STATIC_OVL void
m_initinv(mtmp)
register struct	monst	*mtmp;
{
	register int cnt;
	register struct obj *otmp;
	register struct permonst *ptr = mtmp->data;
/* 	char *opera_cloak = "opera cloak";*/
	int i;

#ifdef REINCARNATION
	if (Is_rogue_level(&u.uz)) return;
#endif

/*
 *	Soldiers get armour & rations - armour approximates their ac.
 *	Nymphs may get mirror or potion of object detection.
 */
	switch(ptr->mlet) {
	    case S_HUMAN:
		if (monsndx(ptr) == PM_FORD_PERFECT || monsndx(ptr) == PM_ARTHUR_DENT) {
			mongets(mtmp,PEANUT_BAG);
		} 
		if (monsndx(ptr) == PM_FORD_PERFECT) {
			mongets(mtmp,TOWEL);
		}
		if (ptr == &mons[PM_MAUD_AGENT] || ptr == &mons[PM_DUAM_XNAHT_AGENT]) (void) mongets(mtmp, POT_AMNESIA);

		if(is_mercenary(ptr)
#ifdef YEOMAN
				|| ptr == &mons[PM_CHIEF_YEOMAN_WARDER]
				|| ptr == &mons[PM_YEOMAN_WARDER]
#endif
		) {
		    register int mac;

		    switch(monsndx(ptr)) {
			case PM_GUARD: mac = -1; break;
#ifdef CONVICT
			case PM_PRISON_GUARD: mac = -2; break;
#endif /* CONVICT */
			case PM_SOLDIER: mac = 3; break;
			case PM_SERGEANT: mac = 0; break;
			case PM_LIEUTENANT: mac = -2; break;
			case PM_CAPTAIN: mac = -3; break;
			case PM_GENERAL: mac = -3; break;
#ifdef YEOMAN
			case PM_YEOMAN_WARDER:
#endif
			case PM_WATCHMAN: mac = 3; break;
			case PM_ANGRY_WATCHMAN: mac = 3; break;
#ifdef YEOMAN
			case PM_CHIEF_YEOMAN_WARDER:
				mongets(mtmp, TALLOW_CANDLE);
#endif
			case PM_WATCH_CAPTAIN: mac = -2; break;
			case PM_WATCH_LIEUTENANT: mac = -2; break;
			case PM_WATCH_LEADER: mac = -2; break;
			case PM_ANGRY_WATCH_CAPTAIN: mac = -2; break;
			case PM_ANGRY_WATCH_LIEUTENANT: mac = -2; break;
			case PM_ANGRY_WATCH_LEADER: mac = -2; break;
			default: impossible("odd mercenary %d?", monsndx(ptr));
				mac = 0;
				break;
		    }

		    if (mac < -1 && rn2(5))
			mac += 7 + mongets(mtmp, (rn2(5)) ?
					   PLATE_MAIL : CRYSTAL_PLATE_MAIL);
		    else if (mac < 3 && rn2(5))
			mac += 6 + mongets(mtmp, (rn2(3)) ?
					   SPLINT_MAIL : BANDED_MAIL);
		    else if (rn2(5))
			mac += 3 + mongets(mtmp, (rn2(3)) ?
					   RING_MAIL : STUDDED_LEATHER_ARMOR);
		    else
			mac += 2 + mongets(mtmp, LEATHER_ARMOR);

		    if (mac < 10 && rn2(3))
			mac += 1 + mongets(mtmp, HELMET);
		    else if (mac < 10 && rn2(2))
			mac += 1 + mongets(mtmp, DENTED_POT);
		    if (mac < 10 && rn2(3))
			mac += 1 + mongets(mtmp, SMALL_SHIELD);
		    else if (mac < 10 && rn2(2))
			mac += 2 + mongets(mtmp, LARGE_SHIELD);
		    if (mac < 10 && rn2(3))
			mac += 1 + mongets(mtmp, LOW_BOOTS);
		    else if (mac < 10 && rn2(2))
			mac += 2 + mongets(mtmp, HIGH_BOOTS);
		    if (mac < 10 && rn2(3))
			mac += 1 + mongets(mtmp, LEATHER_GLOVES);
		    else if (mac < 10 && rn2(2))
			mac += 1 + mongets(mtmp, LEATHER_CLOAK);

		    if(ptr != &mons[PM_GUARD] &&
#ifdef CONVICT
			ptr != &mons[PM_PRISON_GUARD] &&
#endif /* CONVICT */
			ptr != &mons[PM_WATCHMAN] &&
			ptr != &mons[PM_ANGRY_WATCHMAN] &&
			ptr != &mons[PM_ANGRY_WATCH_LIEUTENANT] &&
			ptr != &mons[PM_ANGRY_WATCH_LEADER] &&
			ptr != &mons[PM_WATCH_LEADER] &&
			ptr != &mons[PM_ANGRY_WATCH_CAPTAIN] &&
			ptr != &mons[PM_WATCH_LIEUTENANT] &&
			ptr != &mons[PM_WATCH_CAPTAIN]) {
			if (!rn2(3)) (void) mongets(mtmp, K_RATION);
			if (!rn2(2)) (void) mongets(mtmp, C_RATION);
			if (ptr != &mons[PM_SOLDIER] && !rn2(3))
				(void) mongets(mtmp, BUGLE);
		    } else
			   if (ptr == &mons[PM_WATCHMAN] && rn2(3))
				(void) mongets(mtmp, TIN_WHISTLE);
		} else if (ptr == &mons[PM_TEACHER] && (!rn2(5)) ) {
			/* They're going to need it :-) */
			mongets(mtmp,rn2(3) ? RING_MAIL : PLATE_MAIL);
		} else if (ptr == &mons[PM_PRINCIPAL]) {
		    			/* They're going to need it :-) */
			mongets(mtmp,rn2(3) ? RING_MAIL : PLATE_MAIL);
		  
		}  else if (ptr == &mons[PM_SHOPKEEPER]) {
		    (void) mongets(mtmp,SKELETON_KEY);
		    /* STEPHEN WHITE'S NEW CODE                
		     *
		     * "Were here to pump *clap* YOU up!"  -Hans and Frans
		     *                                      Saterday Night Live
		     */
#ifndef FIREARMS
		    switch (rn2(4)) {
		    /* MAJOR fall through ... */
		    case 0: (void) mongets(mtmp, WAN_MAGIC_MISSILE);
		    case 1: (void) mongets(mtmp, POT_EXTRA_HEALING);
		    case 2: (void) mongets(mtmp, POT_HEALING);
		    case 3: (void) mongets(mtmp, WAN_STRIKING);
		    }
#endif
		    switch (rnd(4)) {
			/* MAJOR fall through ... */
			case 1: (void) mongets(mtmp,POT_HEALING);
			case 2: (void) mongets(mtmp,POT_EXTRA_HEALING);
			case 3: (void) mongets(mtmp,SCR_TELEPORTATION);
			case 4: (void) mongets(mtmp,WAN_TELEPORTATION);
			default:
				break;
		    }
		} else if (ptr->msound == MS_PRIEST ||
			quest_mon_represents_role(ptr,PM_PRIEST)) {
		    (void) mongets(mtmp,
			    rn2(7) ? rn1(ROBE_OF_WEAKNESS - ROBE + 1, ROBE) :
					     rn2(3) ? CLOAK_OF_PROTECTION :
						 CLOAK_OF_MAGIC_RESISTANCE);
		    (void) mongets(mtmp, SMALL_SHIELD);
#ifndef GOLDOBJ
		    mtmp->mgold = (long)rn1(10,20);
#else
		    mkmonmoney(mtmp,(long)rn1(10,20));
#endif
		} else if (quest_mon_represents_role(ptr,PM_MONK)) {
		    (void) mongets(mtmp, rn2(11) ? ROBE :
					     CLOAK_OF_MAGIC_RESISTANCE);
		}
		if (mtmp->data == &mons[PM_WIZARD_OF_YENDOR]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);

		break;
	    case S_NYMPH:
	      if(mtmp->data == &mons[PM_SATYR]){
	        if(!rn2(2)) (void) mongets(mtmp, POT_BOOZE);
	        (void) mongets(mtmp, WOODEN_FLUTE);
		}
		if (mtmp->data == &mons[PM_APHRODITE]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if(!rn2(2)) (void) mongets(mtmp, MIRROR);
		if(!rn2(2)) (void) mongets(mtmp, POT_OBJECT_DETECTION);
		if (ptr == &mons[PM_NYMPH_QUEEN]) {
			struct obj *otmp = mksobj(SKELETON_KEY,TRUE,FALSE);
			otmp = oname(otmp,artiname(ART_KEY_OF_ACCESS));
			mpickobj(mtmp,otmp);
		}
		break;
          case S_RODENT:
		if (mtmp->data == &mons[PM_THE_RAT_KING]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		break;
	    case S_GIANT:
		if (ptr == &mons[PM_MINOTAUR]) {
		    if (!rn2(3) || (in_mklev && Is_earthlevel(&u.uz)))
			(void) mongets(mtmp, WAN_DIGGING);
		} else if (is_giant(ptr)) {
		    for (cnt = rn2((int)(mtmp->m_lev / 2)); cnt; cnt--) {

			if (!rn2(3)) { /* greatly lowered chance --Amy */
			otmp = mksobj(rnd_class(DILITHIUM_CRYSTAL,LUCKSTONE-1),
				      FALSE, FALSE);
			otmp->quan = rnd(3); /* lower amount */
			otmp->owt = weight(otmp);
				if(!rn2(8)) {
					otmp->spe = rne(2);
					if (rn2(2)) otmp->blessed = rn2(2);
					 else	blessorcurse(otmp, 3);
				} else if(!rn2(10)) {
					if (rn2(10)) curse(otmp);
					 else	blessorcurse(otmp, 3);
					otmp->spe = -rne(2);
				} else	blessorcurse(otmp, 10);
			(void) mpickobj(mtmp, otmp);
			}

		    }
		}
		break;
	    case S_WRAITH:
	    if (!rn2(2)) (void)mongets(mtmp, ROBE);
		if (ptr == &mons[PM_NAZGUL]) {
			otmp = mksobj(RIN_INVISIBILITY, FALSE, FALSE);
			curse(otmp);
			(void) mpickobj(mtmp, otmp);
		}
		break;
	    case S_LICH:
		if (ptr == &mons[PM_MASTER_LICH] && !rn2(13))
			(void)mongets(mtmp, (rn2(7) ? ATHAME : WAN_NOTHING));
		else if (ptr == &mons[PM_ARCH_LICH] && !rn2(3)) {
			otmp = mksobj(rn2(3) ? ATHAME : QUARTERSTAFF,
				      TRUE, rn2(13) ? FALSE : TRUE);
			if (otmp->spe < 2) otmp->spe = rnd(3);
			if (!rn2(4)) otmp->oerodeproof = 1;
			(void) mpickobj(mtmp, otmp);
		}
		else if (ptr == &mons[PM_PHARAO]) {
		  	(void) mongets(mtmp, (rn2(5) ? ASSAULT_RIFLE : HEAVY_MACHINE_GUN));
		  	m_initthrow(mtmp, BULLET, 60);
		  	m_initthrow(mtmp, BULLET, 60);
		}
		break;
	    case S_MUMMY:
		if (rn2(7)) (void)mongets(mtmp, MUMMY_WRAPPING);
		break;
	    case S_QUANTMECH:
		if (monsndx(ptr) == PM_QUANTUM_MECHANIC && !rn2(20)) {
			otmp = mksobj(LARGE_BOX, FALSE, FALSE);
			otmp->spe = 1; /* flag for special box */
			otmp->owt = weight(otmp);
			(void) mpickobj(mtmp, otmp);
		}
	      if (monsndx(ptr) == PM_CLOCKWORK_AUTOMATON) {
	        (void) mongets(mtmp, rn2(2)? BROADSWORD : SCIMITAR);
	        (void) mongets(mtmp, BRONZE_PLATE_MAIL);
	      }
		if (monsndx(ptr) == PM_DOCTOR_FRANKENSTEIN) {
			(void)mongets(mtmp, LAB_COAT);
			(void)mongets(mtmp, WAN_POLYMORPH);
			(void)mongets(mtmp, SPE_POLYMORPH);
		}
		break;
	    case S_LEPRECHAUN:
#ifndef GOLDOBJ
		mtmp->mgold = (long) d(level_difficulty(), 30);
#else
		mkmonmoney(mtmp, (long) d(level_difficulty(), 30));
#endif

		break;
       case S_ZRUTY:
      if (ptr == &mons[PM_BANNIK] && !rn2(5))
        mongets(mtmp, TOWEL);
      else if (ptr == &mons[PM_LESHY])
        mongets(mtmp, APPLE);
      break;
	    case S_ELEMENTAL:        
  /*            if(ptr == &mons[PM_WATER_WEIRD]){
			otmp = mksobj(WAN_WISHING,TRUE,FALSE);
			otmp->spe=3;
			otmp->blessed=0;
			mpickobj(mtmp, otmp);
		}*/
		break;	
	    case S_VAMPIRE:
		/* [Lethe] Star and fire vampires don't get this stuff */
		if (ptr == &mons[PM_STAR_VAMPIRE] || 
				ptr == &mons[PM_FIRE_VAMPIRE])
		    break;
	    	/* Get opera cloak */
/*	    	otmp = readobjnam(opera_cloak);
		if (otmp && otmp != &zeroobj) mpickobj(mtmp, otmp);*/
		for (i = STRANGE_OBJECT; i < NUM_OBJECTS; i++) {
			register const char *zn;
			if ((zn = OBJ_DESCR(objects[i])) && !strcmpi(zn, "opera cloak")) {
				if (!OBJ_NAME(objects[i])) i = STRANGE_OBJECT;
				break;
			}
		}
		if (i != NUM_OBJECTS) (void)mongets(mtmp, i);
		if (rn2(2)) {
		    if ((int) mtmp->m_lev > rn2(30))
			(void)mongets(mtmp, POT_VAMPIRE_BLOOD);
		    else
			(void)mongets(mtmp, POT_BLOOD);
		}
		break;
	    case S_DEMON:
		/* moved here from m_initweap() because these don't
		   have AT_WEAP so m_initweap() is not called for them */
		if (ptr == &mons[PM_ICE_DEVIL] && !rn2(4)) {
			(void)mongets(mtmp, SPEAR);
		/* [DS] Cthulhu isn't fully integrated yet, and he won't be
		 *      until Moloch's Sanctum is rearranged */
		} else if (ptr == &mons[PM_CTHULHU]) {
			(void)mongets(mtmp, AMULET_OF_YENDOR);
			(void)mongets(mtmp, WAN_DEATH);
			(void)mongets(mtmp, POT_FULL_HEALING);
		} else if (ptr == &mons[PM_ASMODEUS]) {
			(void)mongets(mtmp, WAN_COLD);
			(void)mongets(mtmp, WAN_FIRE);
		}
		if (mtmp->data == &mons[PM_BAEL]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ZUGGTMOY]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAPHOMET]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DAUGHTER_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MAMMON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_XINIVRAE]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MALCANTHET]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MOTHER_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BELIAL]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_CRONE_LILITH]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_LYNKHAB]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_LEVIATHAN]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_MEPHISTOPHELES]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAALPHEGOR]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_GRAZ_ZT]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_PALE_NIGHT]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_VERIER]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_JUIBLEX]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_JUBILEX]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_YEENOGHU]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ORCUS]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_GERYON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DISPATER]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_BAALZEBUB]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_ASMODEUS]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_DEMOGORGON]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		if (mtmp->data == &mons[PM_CTHULHU]) (void) mongets(mtmp, SCR_ROOT_PASSWORD_DETECTION);
		break;
	    case S_ANGEL:
		if (monsndx(ptr) == PM_DOUGLAS_ADAMS) {
			mongets(mtmp,HITCHHIKER_S_GUIDE_TO_THE_GALA);
		}
		break;

	    case S_XORN:
		if (monsndx(ptr) == PM_MARVIN) {
			int i;
			int t[3] = {DIODE, TRANSISTOR, IC};
			for (i=0;i<3;i++) {
				otmp = mksobj(t[i],FALSE,FALSE);
				otmp->quan = rn2(7) + 1;
				otmp->owt = weight(otmp);
				mpickobj(mtmp,otmp);
			}
		}
		break;
 
	    case S_XAN:
		if (monsndx(ptr) == PM_DEEP_THOUGHT) {
			otmp = mksobj(IC,FALSE,FALSE);
			otmp->quan = 42;
 			otmp->owt = weight(otmp);
			mpickobj(mtmp,otmp);
 		}
 		break;

	    case S_YETI:
		if (monsndx(ptr) == PM_ZAPHOD_BREEBLEBROX) {
			otmp = mksobj(POT_PAN_GALACTIC_GARGLE_BLASTE,FALSE,FALSE);
			otmp->quan = d(2,4);
			otmp->owt = weight(otmp);
			mpickobj(mtmp,otmp);
		}
 		break;
	    case S_DRAGON:
		if (ptr == &mons[PM_DRAGON_LORD]) {
			struct obj *otmp;
			otmp = mksobj(GRAY_DRAGON_SCALE_MAIL,TRUE,FALSE);
			otmp = oname(otmp,artiname(ART_SCALES_OF_THE_DRAGON_LORD));
			mpickobj(mtmp,otmp);
		}
		break;

	    default:
		break;
	}

	/* ordinary soldiers rarely have access to magic (or gold :-) */
	if (ptr == &mons[PM_SOLDIER] && rn2(15)) return;

	if ((int) mtmp->m_lev > rn2(200))
		(void) mongets(mtmp, rnd_defensive_item(mtmp));
	if ((int) mtmp->m_lev > rn2(200))
		(void) mongets(mtmp, rnd_misc_item(mtmp));
	if (!rn2(400)) (void) mongets(mtmp, rnd_defensive_item_new(mtmp));
	if (!rn2(400)) (void) mongets(mtmp, rnd_misc_item_new(mtmp));
#ifndef GOLDOBJ
	if (likes_gold(ptr) && !mtmp->mgold && !rn2(5))
		mtmp->mgold =
		      (long) d(level_difficulty(), mtmp->minvent ? 5 : 10);
#else
	if (likes_gold(ptr) && !findgold(mtmp->minvent) && !rn2(5))
		mkmonmoney(mtmp, (long) d(level_difficulty(), mtmp->minvent ? 5 : 10));
#endif
}

/* Note: for long worms, always call cutworm (cutworm calls clone_mon) */
struct monst *
clone_mon(mon, x, y)
struct monst *mon;
xchar x, y;	/* clone's preferred location or 0 (near mon) */
{
	coord mm;
	struct monst *m2;

	/* may be too weak or have been extinguished for population control */
	if (mon->mhp <= 1 || (mvitals[monsndx(mon->data)].mvflags & G_EXTINCT))
	    return (struct monst *)0;

	if (x == 0) {
	    mm.x = mon->mx;
	    mm.y = mon->my;
	    if (!enexto(&mm, mm.x, mm.y, mon->data) || MON_AT(mm.x, mm.y))
		return (struct monst *)0;
	} else if (!isok(x, y)) {
	    return (struct monst *)0;	/* paranoia */
	} else {
	    mm.x = x;
	    mm.y = y;
	    if (MON_AT(mm.x, mm.y)) {
		if (!enexto(&mm, mm.x, mm.y, mon->data) || MON_AT(mm.x, mm.y))
		    return (struct monst *)0;
	    }
	}
	m2 = newmonst(0);
	*m2 = *mon;			/* copy condition of old monster */
	m2->nmon = fmon;
	fmon = m2;
	m2->m_id = flags.ident++;
	if (!m2->m_id) m2->m_id = flags.ident++;	/* ident overflowed */
	m2->mx = mm.x;
	m2->my = mm.y;

	m2->minvent = (struct obj *) 0; /* objects don't clone */
	m2->mleashed = FALSE;
#ifndef GOLDOBJ
	m2->mgold = 0L;
#endif
	/* Max HP the same, but current HP halved for both.  The caller
	 * might want to override this by halving the max HP also.
	 * When current HP is odd, the original keeps the extra point.
	 */
	m2->mhpmax = mon->mhpmax;
	m2->mhp = mon->mhp / 2;
	mon->mhp -= m2->mhp;

	/* Same for the power */
	m2->m_enmax = mon->m_enmax;
	m2->m_en = mon->m_en /= 2;

	/* since shopkeepers and guards will only be cloned if they've been
	 * polymorphed away from their original forms, the clone doesn't have
	 * room for the extra information.  we also don't want two shopkeepers
	 * around for the same shop.
	 */
	if (mon->isshk) m2->isshk = FALSE;
	if (mon->isgd) m2->isgd = FALSE;
	if (mon->ispriest) m2->ispriest = FALSE;
	if (mon->isgyp) m2->isgyp = FALSE;
	m2->mxlth = 0;
	place_monster(m2, m2->mx, m2->my);
	if (emits_light(m2->data))
	    new_light_source(m2->mx, m2->my, emits_light(m2->data),
			     LS_MONSTER, (genericptr_t)m2);
	if (m2->mnamelth) {
	    m2->mnamelth = 0; /* or it won't get allocated */
	    m2 = christen_monst(m2, NAME(mon));
	} else if (mon->isshk) {
	    m2 = christen_monst(m2, shkname(mon));
	}

	/* not all clones caused by player are tame or peaceful */
	if (!flags.mon_moving) {
	    if (mon->mtame)
		m2->mtame = rn2(max(2 + u.uluck, 2)) ? mon->mtame : 0;
	    else if (mon->mpeaceful)
		m2->mpeaceful = rn2(max(2 + u.uluck, 2)) ? 1 : 0;
	}

	newsym(m2->mx,m2->my);	/* display the new monster */
	if (m2->mtame) {
	    struct monst *m3;

	    if (mon->isminion) {
		m3 = newmonst(sizeof(struct epri) + mon->mnamelth);
		*m3 = *m2;
		m3->mxlth = sizeof(struct epri);
		if (m2->mnamelth) Strcpy(NAME(m3), NAME(m2));
		*(EPRI(m3)) = *(EPRI(mon));
		replmon(m2, m3);
		m2 = m3;
	    } else {
		/* because m2 is a copy of mon it is tame but not init'ed.
		 * however, tamedog will not re-tame a tame dog, so m2
		 * must be made non-tame to get initialized properly.
		 */
		m2->mtame = 0;
		if ((m3 = tamedog(m2, (struct obj *)0)) != 0) {
		    m2 = m3;
		    *(EDOG(m2)) = *(EDOG(mon));
		}
	    }
	}
	set_malign(m2);

	return m2;
}

/*
 * Propagate a species
 *
 * Once a certain number of monsters are created, don't create any more
 * at random (i.e. make them extinct).  The previous (3.2) behavior was
 * to do this when a certain number had _died_, which didn't make
 * much sense.
 *
 * Returns FALSE propagation unsuccessful
 *         TRUE  propagation successful
 */
boolean
propagate(mndx, tally, ghostly)
int mndx;
boolean tally;
boolean ghostly;
{
	boolean result;
	uchar lim = mbirth_limit(mndx);
	boolean gone = (mvitals[mndx].mvflags & G_GONE); /* genocided or extinct */

	result = (((int) mvitals[mndx].born < lim) && !gone) ? TRUE : FALSE;

	/* if it's unique, don't ever make it again */
	if (mons[mndx].geno & G_UNIQ) mvitals[mndx].mvflags |= G_EXTINCT;

	if (mvitals[mndx].born < 255 && tally && (!ghostly || (ghostly && result)))
		 mvitals[mndx].born++;
	if ((int) mvitals[mndx].born >= lim && !(mons[mndx].geno & G_NOGEN) &&
		!(mvitals[mndx].mvflags & G_EXTINCT)) {
#if defined(DEBUG) && defined(WIZARD)
		if (wizard) pline("Automatically extinguished %s.",
					makeplural(mons[mndx].mname));
#endif
		mvitals[mndx].mvflags |= G_EXTINCT;
		reset_rndmonst(mndx);
	}
	return result;
}

/*
 * called with [x,y] = coordinates;
 *	[0,0] means anyplace
 *	[u.ux,u.uy] means: near player (if !in_mklev)
 *
 *	In case we make a monster group, only return the one at [x,y].
 */
struct monst *
makemon(ptr, x, y, mmflags)
register struct permonst *ptr;
register int	x, y;
register int	mmflags;
{
	register struct monst *mtmp;
	int mndx, mcham, ct, mitem, xlth;
	boolean anymon = (!ptr);
	boolean byyou = (x == u.ux && y == u.uy);
	boolean allow_minvent = ((mmflags & NO_MINVENT) == 0);
	boolean countbirth = ((mmflags & MM_NOCOUNTBIRTH) == 0);
	unsigned gpflags = (mmflags & MM_IGNOREWATER) ? MM_IGNOREWATER : 0;

	/* if caller wants random location, do it here */
	if(x == 0 && y == 0) {
		int tryct = 0;	/* careful with bigrooms */
		struct monst fakemon;

		fakemon.data = ptr;	/* set up for goodpos */
		do {
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);
		} while(!goodpos(x, y, ptr ? &fakemon : (struct monst *)0, gpflags) ||
			(!in_mklev && tryct++ < 50 && cansee(x, y)));
	} else if (byyou && !in_mklev) {
		coord bypos;

		if(enexto_core(&bypos, u.ux, u.uy, ptr, gpflags)) {
			x = bypos.x;
			y = bypos.y;
		} else
			return((struct monst *)0);
	}

	/* Does monster already exist at the position? */
	if(MON_AT(x, y)) {
		if ((mmflags & MM_ADJACENTOK) != 0) {
			coord bypos;
			if(enexto_core(&bypos, x, y, ptr, gpflags)) {
				x = bypos.x;
				y = bypos.y;
			} else
				return((struct monst *) 0);
		} else 
			return((struct monst *) 0);
	}

	if(ptr){
		mndx = monsndx(ptr);
		/* if you are to make a specific monster and it has
		   already been genocided, return */
		if (mvitals[mndx].mvflags & G_GENOD) return((struct monst *) 0);
#if defined(WIZARD) && defined(DEBUG)
		if (wizard && (mvitals[mndx].mvflags & G_EXTINCT))
		    pline("Explicitly creating extinct monster %s.",
			mons[mndx].mname);
#endif
	} else {
		/* make a random (common) monster that can survive here.
		 * (the special levels ask for random monsters at specific
		 * positions, causing mass drowning on the medusa level,
		 * for instance.)
		 */
		int tryct = 0;	/* maybe there are no good choices */
		struct monst fakemon;
		do {
			if(!(ptr = rndmonst())) {
#ifdef DEBUG
			    pline("Warning: no monster.");
#endif
			    return((struct monst *) 0);	/* no more monsters! */
			}
			fakemon.data = ptr;	/* set up for goodpos */
		} while(!goodpos(x, y, &fakemon, gpflags) && tryct++ < 50);
		mndx = monsndx(ptr);
	}
	(void) propagate(mndx, countbirth, FALSE);
	xlth = ptr->pxlth;
	if (mmflags & MM_EDOG) xlth += sizeof(struct edog);
	else if (mmflags & MM_EMIN) xlth += sizeof(struct emin);
	mtmp = newmonst(xlth);
	*mtmp = zeromonst;		/* clear all entries in structure */
	(void)memset((genericptr_t)mtmp->mextra, 0, xlth);
	mtmp->nmon = fmon;
	fmon = mtmp;
	mtmp->m_id = flags.ident++;
	if (!mtmp->m_id) mtmp->m_id = flags.ident++;	/* ident overflowed */
	set_mon_data(mtmp, ptr, 0);
	if (mtmp->data->msound == MS_LEADER)
	    quest_status.leader_m_id = mtmp->m_id;
	mtmp->mxlth = xlth;
	mtmp->mnum = mndx;

	mtmp->m_lev = adj_lev(ptr);
	
	/* WAC set oldmonnm */
	mtmp->oldmonnm = monsndx(ptr);
	
	if (ptr >= &mons[PM_ARCHEOLOGIST] && ptr <= &mons[PM_WIZARD]) {
	   /* enemy characters are of varying level */
	   int base_you, base_lev;
	   base_you = (u.ulevel / 2)+1;
	   base_lev = level_difficulty()+1;
	   if (base_you < 1) base_you = 1;
	   if (base_lev < 1) base_lev = 1;
	   mtmp->m_lev = (1 + rn2(base_you) + rn2(base_lev) / 2)+1;
	}

	/* Set HP, HPmax */	
	if (is_golem(ptr)) {
	    mtmp->mhpmax = mtmp->mhp = golemhp(mndx);
	} else if (is_rider(ptr)) {
	    /* We want low HP, but a high mlevel so they can attack well */
		mtmp->mhpmax = mtmp->mhp = d(10,8) + 20;
	} else if (ptr->mlevel > 49) {
	    /* "special" fixed hp monster
	     * the hit points are encoded in the mlevel in a somewhat strange
	     * way to fit in the 50..127 positive range of a signed character
	     * above the 1..49 that indicate "normal" monster levels */
	    mtmp->mhpmax = mtmp->mhp = 5*(ptr->mlevel - 6); /*hp increase --Amy */
	    mtmp->m_lev = mtmp->mhp / 4;	/* approximation */
	} else if (ptr->mlet == S_DRAGON && mndx >= PM_GRAY_DRAGON) {
	    /* adult dragons */
	    mtmp->mhpmax = mtmp->mhp = (int) (In_endgame(&u.uz) ?
		(8 * mtmp->m_lev) : (4 * mtmp->m_lev + d((int)mtmp->m_lev, 4)));
	} else if (!mtmp->m_lev) {
	    mtmp->mhpmax = mtmp->mhp = rnd(4);
	} else {
	    mtmp->mhpmax = mtmp->mhp = d((int)mtmp->m_lev, 8);
	    
	    if (is_home_elemental(ptr))
		mtmp->mhpmax = (mtmp->mhp *= 3);
	    else mtmp->mhpmax = mtmp->mhp = 
		d((int)mtmp->m_lev, 8) + (mtmp->m_lev*rnd(2));
	}

	/* Assign power */
	/*if (mindless(ptr)) {
	    mtmp->m_enmax = mtmp->m_en = 0;
	} else {*/
	    /* This is actually quite similar to hit dice,  
	     * but with more randomness 
	     * Mindless monsters may do some spellcasting now, mainly for sephirahs --Amy
	     */
	    mtmp->m_enmax = mtmp->m_en =
		d((int)mtmp->m_lev * 2, 4) + (mtmp->m_lev*rnd(2));
		if (mindless(ptr)) {mtmp->m_enmax = mtmp->m_enmax / 2; mtmp->m_en = mtmp->m_en / 2; }
	/*}*/
	if (is_female(ptr)) mtmp->female = TRUE;
	else if (is_male(ptr)) mtmp->female = FALSE;
	else mtmp->female = rn2(2);	/* ignored for neuters */

	if (In_sokoban(&u.uz) && !mindless(ptr))  /* know about traps here */
	    mtmp->mtrapseen = (1L << (PIT - 1)) | (1L << (HOLE - 1));
	if (ptr->msound == MS_LEADER)		/* leader knows about portal */
	    mtmp->mtrapseen |= (1L << (MAGIC_PORTAL-1));

	place_monster(mtmp, x, y);
	mtmp->mcansee = mtmp->mcanmove = TRUE;
	mtmp->mpeaceful = (mmflags & MM_ANGRY) ? FALSE : peace_minded(ptr);
	mtmp->mtraitor  = FALSE;

	/* Everything that can hide under an object will now do so. --Amy */
      if(x && y && hides_under(ptr) )
	  (void) mkobj_at(0, x, y, TRUE);
	if(/*in_mklev && */hides_under(ptr) && OBJ_AT(x, y))
	  mtmp->mundetected = TRUE;

	switch(ptr->mlet) {
		case S_MIMIC:
			set_mimic_sym(mtmp);
			break;
		case S_KOP:
			if (mtmp->data == &mons[PM_NETZAH_SEPHIRAH]) set_mimic_sym(mtmp);

			if (mndx == PM_KEYSTONE_POLICEMAN) mtmp->minvis = TRUE;
			if (mndx == PM_POLICEMAN_SERGEANT) mtmp->minvis = TRUE;
			if (mndx == PM_POLICEMAN_LIEUTENANT) mtmp->minvis = TRUE;
			if (mndx == PM_POLICEMAN_KAPTAIN) mtmp->minvis = TRUE;
			if (mndx == PM_POLICEMAN_KOMMISSIONER) mtmp->minvis = TRUE;
			if (mndx == PM_POLICEMAN_KCHIEF) mtmp->minvis = TRUE;

			if (mndx == PM_KEYSTONE_WARDER) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_SERGEANT) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_LIEUTENANT) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_KAPTAIN) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_KOMMISSIONER) set_mimic_sym(mtmp);
			if (mndx == PM_WARDER_KCHIEF) set_mimic_sym(mtmp);

			if (mndx == PM_HUMAN_WEREMIMIC) set_mimic_sym(mtmp);

			break;
		case S_XORN:
			if (mtmp->data == &mons[PM_XEROC]) set_mimic_sym(mtmp);
			break;
		case S_BAD_COINS:
			if (mtmp->data == &mons[PM_FATA_MORGANA]) set_mimic_sym(mtmp);
			break;
		case S_ZOMBIE:
			if (mtmp->data == &mons[PM_ALIENATED_UFO_PART]) set_mimic_sym(mtmp);
			if (mndx == PM_UNDEFINED_UFO_PART) mtmp->minvis = TRUE;
			break;
		case S_NAGA:
			if (mtmp->data == &mons[PM_MULTI_HUED_NAGA]) set_mimic_sym(mtmp);
			break;
		case S_MUMMY:
			if (mtmp->data == &mons[PM_MIMIC_MUMMY]) set_mimic_sym(mtmp);
			break;
		case S_RUBMONST:
			if (mtmp->data == &mons[PM_MIMICRY_RUBBER]) set_mimic_sym(mtmp);
			break;
		case S_GHOST:
			if (mtmp->data == &mons[PM_CURSED_SPIRIT]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_DEVILISH_SPIRIT]) set_mimic_sym(mtmp);
			break;
		case S_VORTEX:
			if (mtmp->data == &mons[PM_MIMIC_VORTEX]) set_mimic_sym(mtmp);
			break;
		case S_ANT:
			if (mtmp->data == &mons[PM_DISGUISED_SOLDIER_ANT]) set_mimic_sym(mtmp);
			break;
		case S_HUMAN:
			if (is_pool(x, y) && (mndx == PM_HUMAN_WEREPIRANHA || mndx == PM_HUMAN_WEREEEL || mndx == PM_HUMAN_WEREKRAKEN) )
			    mtmp->mundetected = TRUE;
			if (mtmp->data == &mons[PM_OSCILLATOR]) set_mimic_sym(mtmp);
			break;
		case S_EYE:
			if (mtmp->data == &mons[PM_VOLTORB]) set_mimic_sym(mtmp);
			if (mtmp->data == &mons[PM_ELECTRODE]) set_mimic_sym(mtmp);
			break;
		case S_SPIDER:
		case S_SNAKE:
			/* if(in_mklev)
			    if(x && y)
				(void) mkobj_at(0, x, y, TRUE);
			if(in_mklev && hides_under(ptr) && OBJ_AT(x, y))
			    mtmp->mundetected = TRUE;*/
			break;
		case S_LIGHT:
		case S_ELEMENTAL:
			if (mndx == PM_STALKER || mndx == PM_BLACK_LIGHT || mndx == PM_POLTERGEIST) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}
			break;
		case S_EEL:
			if (is_pool(x, y))
			    mtmp->mundetected = TRUE;
			if (mtmp->data == &mons[PM_CAMO_FISH] || mtmp->data == &mons[PM_AMORPHOUS_FISH]) set_mimic_sym(mtmp);
			break;
		case S_PUDDING:
			if (ptr == &mons[PM_PHANTOM]) mtmp->minvis = TRUE;
			break;
		case S_LEPRECHAUN:

			if (mtmp->data == &mons[PM_HEHEHE_HE_GUY]) set_mimic_sym(mtmp);

      if (mndx == PM_POOKA){
	        if(!rn2(5)) (void) mongets(mtmp, CARROT);
#ifdef TOURIST
		    if (!rn2(8)) (void) mongets(mtmp, HAWAIIAN_SHIRT); 
#endif
		    if (!rn2(4)) (void)mongets(mtmp, ELVEN_LEATHER_HELM);
		    switch (rn2(3)) {
		case 0:
			    if (!rn2(4)) (void)mongets(mtmp, ELVEN_SHIELD);
			    if (rn2(2)) (void)mongets(mtmp, ELVEN_SHORT_SWORD);
         case 1:
			    (void)mongets(mtmp, ELVEN_DAGGER);
			    break;
		case 2:
			    if (rn2(2)) {
				(void)mongets(mtmp, ELVEN_SPEAR);
				(void)mongets(mtmp, ELVEN_SHIELD);
			    }
			    break;
		    }
      }
         if (ptr == &mons[PM_POOKA] && !rn2(3)){
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
         }
			mtmp->msleeping = 1;
			break;
		case S_JABBERWOCK:
		case S_NYMPH:
	      if (mndx == PM_SATYR)
	        if (rn2(2))
	          (void) mongets(mtmp, DAGGER);

			if (rn2(5) && !u.uhave.amulet) mtmp->msleeping = 1;

			if (mndx == PM_PIXIE || mndx == PM_PIXIE_ARCHER) {        
/*  			    mtmp->perminvis = TRUE;*/
  			    mtmp->minvis = TRUE;
			}
			break;
		case S_DRAGON:
			if (mndx == PM_KLIEAU_MANTICORE) {        
  			    mtmp->minvis = TRUE;
			}
			break;
		case S_ORC:
			if (Race_if(PM_ELF)) mtmp->mpeaceful = FALSE;
			break;
		case S_UNICORN:
			if (is_unicorn(ptr) &&
					sgn(u.ualign.type) == sgn(ptr->maligntyp))
				mtmp->mpeaceful = TRUE;
			break;
		case S_BAT:
			if ((Race_if(PM_IMPERIAL) || (Inhell && !Race_if(PM_HERETIC) ) || !strncmpi(plname, "Gehenna", 7)) && is_bat(ptr))
			    mon_adjust_speed(mtmp, 2, (struct obj *)0);
			break;
		case S_VAMPIRE:
			/* [DS] Star vampires are invisible until they feed */
			if (mndx == PM_STAR_VAMPIRE) {
			    mtmp->perminvis = TRUE;
			    mtmp->minvis = TRUE;
			}

			if (mtmp->data == &mons[PM_VAMPIRE_SHADOWCLOAK]) set_mimic_sym(mtmp);
			break;
	}
	if ((ct = emits_light(mtmp->data)) > 0)
		new_light_source(mtmp->mx, mtmp->my, ct,
				 LS_MONSTER, (genericptr_t)mtmp);

	mitem = 0;	/* extra inventory item for this monster */

	if ((mcham = pm_to_cham(mndx)) != CHAM_ORDINARY) {
		/* If you're protected with a ring, don't create
		 * any shape-changing chameleons -dgk
		 */
		if (Protection_from_shape_changers)
			mtmp->cham = CHAM_ORDINARY;
		else {
			mtmp->cham = mcham;
			(void) mon_spec_poly(mtmp, rndmonst(), 0L, FALSE, FALSE, FALSE, FALSE);
		}
	} else if (mndx == PM_WIZARD_OF_YENDOR) {
		mtmp->iswiz = TRUE;
		flags.no_of_wizards++;
		if (flags.no_of_wizards == 1 && Is_earthlevel(&u.uz))
			mitem = SPE_DIG;
	} else if (mndx == PM_DJINNI) {
		flags.djinni_count++;
	} else if (mndx == PM_GHOST) {
		flags.ghost_count++;
		if (!(mmflags & MM_NONAME))
			mtmp = christen_monst(mtmp, rndghostname());

	/* undead player monsters don't have titles, so I'm assigning names to them instead --Amy */
	} else if (mndx >= PM_UNDEAD_ARCHEOLOGIST && mndx <= PM_UNDEAD_WIZARD && !(mmflags & MM_NONAME) ) {

		mtmp = christen_monst(mtmp, mtmp->female ? rndplrmonnamefemale() : rndplrmonname() );

	} else if (mndx == PM_NIGHTMARE) {
		struct obj *otmp;

		otmp = oname(mksobj(SKELETON_KEY, TRUE, FALSE),
				artiname(ART_KEY_OF_LAW));
		if (otmp) {
			otmp->blessed = otmp->cursed = 0;
			mpickobj(mtmp, otmp);
		}
	} else if (mndx == PM_BEHOLDER) {
		struct obj *otmp;

		otmp = oname(mksobj(SKELETON_KEY, TRUE, FALSE),
				artiname(ART_KEY_OF_NEUTRALITY));
		if (otmp) {
			otmp->blessed = otmp->cursed = 0;
			mpickobj(mtmp, otmp);
		}
	} else if (mndx == PM_VECNA) {
		struct obj *otmp;

		otmp = oname(mksobj(SKELETON_KEY, TRUE, FALSE),
				artiname(ART_KEY_OF_CHAOS));
		if (otmp) {
			otmp->blessed = otmp->cursed = 0;
			mpickobj(mtmp, otmp);
		}
	} else if (mndx == PM_GYPSY) {
		/* KMH -- Gypsies are randomly generated; initialize them here */
		gypsy_init(mtmp);
	} else if (mndx == PM_VLAD_THE_IMPALER) {
		mitem = CANDELABRUM_OF_INVOCATION;
	} else if (mndx == PM_CROESUS) {
		mitem = TWO_HANDED_SWORD;
	} else if (ptr->msound == MS_NEMESIS && ptr->mlet != S_NEMESE ) {
		mitem = BELL_OF_OPENING; /* prevent new random nemesis monsters from dropping the bell --Amy */
	} else if (mndx == PM_PESTILENCE) {
		mitem = POT_SICKNESS;
	} 



	if (mitem && allow_minvent) (void) mongets(mtmp, mitem);

	if(in_mklev) {
		if(((is_ndemon(ptr)) ||
		    (mndx == PM_WUMPUS) ||
		    (mndx == PM_LONG_WORM) ||
		    (mndx == PM_GIANT_EEL)) && !u.uhave.amulet && rn2(5))
			mtmp->msleeping = 1;
	} else {
		if(byyou) {
			newsym(mtmp->mx,mtmp->my);
			set_apparxy(mtmp);
		}
	}
	if(is_dprince(ptr) && ptr->msound == MS_BRIBE) {
	    mtmp->mpeaceful = mtmp->minvis = mtmp->perminvis = 1;
	    mtmp->mavenge = 0;
	    if (uwep && uwep->oartifact == ART_EXCALIBUR)
		mtmp->mpeaceful = mtmp->mtame = FALSE;
	}
#ifndef DCC30_BUG
	if (mndx == PM_LONG_WORM && (mtmp->wormno = get_wormno()) != 0)
#else
	/* DICE 3.0 doesn't like assigning and comparing mtmp->wormno in the
	 * same expression.
	 */
	if (mndx == PM_LONG_WORM &&
		(mtmp->wormno = get_wormno(), mtmp->wormno != 0))
#endif
	{
	    /* we can now create worms with tails - 11/91 */
	    initworm(mtmp, rn2(5));
	    if (count_wsegs(mtmp)) place_worm_tail_randomly(mtmp, x, y);
	}
	set_malign(mtmp);		/* having finished peaceful changes */
	if(anymon) {
	    if ((ptr->geno & G_SGROUP) && rn2(2)) {
		m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    } else if (ptr->geno & G_LGROUP) {
		if(rn2(3))  m_initlgrp(mtmp, mtmp->mx, mtmp->my);
		else	    m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    }
	    else if(ptr->geno & G_VLGROUP) {
			if(rn2(3))  m_initvlgrp(mtmp, mtmp->mx, mtmp->my);
			else if(rn2(3))  m_initlgrp(mtmp, mtmp->mx, mtmp->my);
			else        m_initsgrp(mtmp, mtmp->mx, mtmp->my);
	    }
	}

	if (allow_minvent) {
	    if(is_armed(ptr))
		m_initweap(mtmp);	/* equip with weapons / armor */
	    m_initinv(mtmp);  /* add on a few special items incl. more armor */
		m_initxtraitem(mtmp);
	    m_dowear(mtmp, TRUE);
	} else {
	    if (mtmp->minvent) discard_minvent(mtmp);
	    mtmp->minvent = (struct obj *)0;    /* caller expects this */
	    mtmp->minvent = (struct obj *)0;    /* caller expects this */
		m_initxtraitem(mtmp);
	}
	if ((ptr->mflags3 & M3_WAITMASK) && !(mmflags & MM_NOWAIT)) {
		if (ptr->mflags3 & M3_WAITFORU)
			mtmp->mstrategy |= STRAT_WAITFORU;
		if (ptr->mflags3 & M3_CLOSE)
			mtmp->mstrategy |= STRAT_CLOSE;
	}

	if (mndx == PM_UNFORTUNATE_VICTIM && in_mklev ) { /* These are supposed to spawn already dead. --Amy */
			monkilled(mtmp, "", AD_PHYS);
	} 
	if (mndx == PM_SHOCKING_SPHERE && Role_if(PM_ACID_MAGE) && Is_nemesis(&u.uz) ) {
			(void) mon_spec_polyX(mtmp,  &mons[PM_LIGHTNING_PROOF_WALL], 0L, FALSE, FALSE, FALSE, FALSE);
	} 


	if (!in_mklev)
	    newsym(mtmp->mx,mtmp->my);	/* make sure the mon shows up */

	return(mtmp);
}

int
mbirth_limit(mndx)
int mndx;
{
	/* assert(MAXMONNO < 255); */
	return (mndx == PM_NAZGUL ? 9 : mndx == PM_ERINYS ? 3 : MAXMONNO); 
}

/* used for wand/scroll/spell of create monster */
/* returns TRUE iff you know monsters have been created */

boolean
create_critters(cnt, mptr)
int cnt;
struct permonst *mptr;		/* usually null; used for confused reading */
{
	coord c;
	int x, y;
	struct monst *mon;
	boolean known = FALSE;
#ifdef WIZARD
	boolean ask = wizard;
#endif

	while (cnt--) {
#ifdef WIZARD
	    if (ask) {
		if (create_particular()) {
		    known = TRUE;
		    continue;
		}
		else ask = FALSE;	/* ESC will shut off prompting */
	    }
#endif
	    x = u.ux,  y = u.uy;
	    /* if in water, try to encourage an aquatic monster
	       by finding and then specifying another wet location */
	    if (!mptr && u.uinwater && enexto(&c, x, y, &mons[PM_GIANT_EEL]))
		x = c.x,  y = c.y;

	    mon = makemon(mptr, x, y, NO_MM_FLAGS);
	    if (mon && canspotmon(mon)) known = TRUE;
	}
	return known;
}

#endif /* OVL1 */
#ifdef OVL0

STATIC_OVL boolean
uncommon(mndx)
int mndx;
{
	if (mons[mndx].geno & (G_NOGEN | G_UNIQ)) return TRUE;
	if (mvitals[mndx].mvflags & G_GONE) return TRUE;
	/*if (Inhell)
		return(mons[mndx].maligntyp > A_NEUTRAL);
	else
		return((mons[mndx].geno & G_HELL) != 0);*/
	return FALSE;
}

/*
 *	shift the probability of a monster's generation by
 *	comparing the dungeon alignment and monster alignment.
 *	return an integer in the range of 0-5.
 */
STATIC_OVL int
align_shift(ptr)
register struct permonst *ptr;
{
    static NEARDATA long oldmoves = 0L;	/* != 1, starting value of moves */
    static NEARDATA s_level *lev;
    register int alshift;

    if(oldmoves != moves) {
	lev = Is_special(&u.uz);
	oldmoves = moves;
    }
    switch((lev) ? lev->flags.align : dungeons[u.uz.dnum].flags.align) {
    default:	/* just in case */
    case AM_NONE:	alshift = 0;
			break;
    case AM_LAWFUL:	alshift = (ptr->maligntyp+20)/(2*ALIGNWEIGHT);
			break;
    case AM_NEUTRAL:	alshift = (20 - abs(ptr->maligntyp))/ALIGNWEIGHT;
			break;
    case AM_CHAOTIC:	alshift = (-(ptr->maligntyp-20))/(2*ALIGNWEIGHT);
			break;
    }
    return alshift;
}

static NEARDATA struct {
	int choice_count;
	char mchoices[SPECIAL_PM];	/* value range is 0..127 */
} rndmonst_state = { -1, {0} };

/* select a random monster type */
struct permonst *
rndmonst()
{
	register struct permonst *ptr;
	register int mndx, ct;

	if(u.ukinghill){ /* You have pirate quest artifact in open inventory */
		if(rnd(100)>98){
			if(In_endgame(&u.uz)) return &mons[PM_GITHYANKI_PIRATE];
			else if(Inhell) return &mons[PM_DAMNED_PIRATE];
			else return &mons[PM_SKELETAL_PIRATE];
		}
	}
	

/* [Tom] this was locking up priest quest... who knows why? */
/* fixed it! no 'W' class monsters with corpses! oops! */
/*    if(u.uz.dnum == quest_dnum && (ptr = qt_montype())) return(ptr); */
/*	if(u.uz.dnum == quest_dnum) {
		if ((ptr = qt_montype())) {
			return(ptr);
		}
	}*/

	/* KMH -- February 2 is Groundhog Day! */
	if (Is_oracle_level(&u.uz) && (!!flags.groundhogday ^ !rn2(20)))
		return (&mons[PM_WOODCHUCK]);

      if (u.uz.dnum == quest_dnum && (!rn2(7)) && (ptr = qt_montype()) != 0)
	    return ptr; 

	if (rndmonst_state.choice_count < 0) {	/* need to recalculate */
	    int zlevel, minmlev, maxmlev;
	    boolean elemlevel;
#ifdef REINCARNATION
	    boolean upper;
#endif
	    rndmonst_state.choice_count = 0;
	    /* look for first common monster */
	    for (mndx = LOW_PM; mndx < SPECIAL_PM; mndx++) {
		if (!uncommon(mndx)) break;
		rndmonst_state.mchoices[mndx] = 0;
	    }		
	    if (mndx == SPECIAL_PM) {
		/* evidently they've all been exterminated */
#ifdef DEBUG
		pline("rndmonst: no common mons!");
#endif
		return (struct permonst *)0;
	    } /* else `mndx' now ready for use below */
	    zlevel = level_difficulty();
	    /* determine the level of the weakest monster to make. */
	    minmlev = /*zlevel / 6*/0;
	    /* determine the level of the strongest monster to make. */
	    maxmlev = (zlevel + u.ulevel + 1)>>1;
	    if (!rn2(100)) maxmlev *= 2;
	    if (!rn2(1000)) maxmlev *= 4;
	    if (!rn2(10000)) maxmlev = 127;
		if (maxmlev > 127) maxmlev = 127; /* maxmlev is an int, but better safe than sorry. --Amy */
#ifdef REINCARNATION
	    upper = Is_rogue_level(&u.uz);
#endif
	    elemlevel = In_endgame(&u.uz) && !Is_astralevel(&u.uz);

/*
 *	Find out how many monsters exist in the range we have selected.
 */
	     
loopback:
	    /* (`mndx' initialized above) */
	    for ( ; mndx < SPECIAL_PM; mndx++) {
		ptr = &mons[mndx];
		rndmonst_state.mchoices[mndx] = 0;
		if (tooweak(mndx, minmlev) || toostrong(mndx, maxmlev))
		    continue;
#ifdef REINCARNATION
		if (upper && !isupper((int)def_monsyms[(int)(ptr->mlet)])) continue;
#endif
		if (elemlevel && wrong_elem_type(ptr)) continue;
		if (uncommon(mndx)) continue;
		if (Inhell && (ptr->geno & G_NOHELL)) continue;
		ct = (int)(ptr->geno & G_FREQ) + align_shift(ptr);
		if (ct < 0 || ct > 127)
		    panic("rndmonst: bad count [#%d: %d]", mndx, ct);
		rndmonst_state.choice_count += ct;
		rndmonst_state.mchoices[mndx] = (char)ct;
	    }
/*
 *	    Possible modification:  if choice_count is "too low",
 *	    expand minmlev..maxmlev range and try again.
 */
	} /* choice_count+mchoices[] recalc */

	if (rndmonst_state.choice_count <= 0) {
	    /* maybe no common mons left, or all are too weak or too strong */
#ifdef DEBUG
	    Norep("rndmonst: choice_count=%d", rndmonst_state.choice_count);
#endif
	    return (struct permonst *)0;
	}

/*
 *	Now, select a monster at random.
 */
	ct = rnd(rndmonst_state.choice_count);
	for (mndx = LOW_PM; mndx < SPECIAL_PM; mndx++)
	    if ((ct -= (int)rndmonst_state.mchoices[mndx]) <= 0) break;

	if (mndx == SPECIAL_PM || uncommon(mndx)) {	/* shouldn't happen */
	    impossible("rndmonst: bad `mndx' [#%d]", mndx);
	    return (struct permonst *)0;
	}

	return &mons[mndx];

/*
 * [Tom] Your rival adventurers are a special case:
 *       - They have varying levels, and thus can always appear
 *       - But they need to be sorta rare or else they're a large
 *         percentage of the dungeon inhabitants.
 */

	/* Sorry, but this code is not working, so I'm commenting it out. --Amy */
/*	      if ((monsndx(ptr-1) >= PM_ARCHEOLOGIST) &&
		  (monsndx(ptr-1) <= PM_WIZARD)
		   && (rn2(4))) goto loopback;
*/

}

/* called when you change level (experience or dungeon depth) or when
   monster species can no longer be created (genocide or extinction) */
void
reset_rndmonst(mndx)
int mndx;	/* particular species that can no longer be created */
{
	/* cached selection info is out of date */
	if (mndx == NON_PM) {
	    rndmonst_state.choice_count = -1;	/* full recalc needed */
	} else if (mndx < SPECIAL_PM) {
	    rndmonst_state.choice_count -= rndmonst_state.mchoices[mndx];
	    rndmonst_state.mchoices[mndx] = 0;
	} /* note: safe to ignore extinction of unique monsters */
}

#endif /* OVL0 */
#ifdef OVL1

/*	The routine below is used to make one of the multiple types
 *	of a given monster class.  The second parameter specifies a
 *	special casing bit mask to allow the normal genesis
 *	masks to be deactivated.  Returns 0 if no monsters
 *	in that class can be made.
 */

struct permonst *
mkclass(class,spc)
char	class;
int	spc;
{
	register int    first;

	first = pm_mkclass(class,spc);
	
	if (first == -1) return((struct permonst *) 0);

	return(&mons[first]);
}

/* Called by mkclass() - returns the pm of the monster 
 * Returns -1 (PM_PLAYERMON) if can't find monster of the class
 *
 * spc may have G_UNIQ and/or G_NOGEN set to allow monsters of
 * this type (otherwise they will be ignored). It may also have
 * MKC_ULIMIT set to place an upper limit on the difficulty of
 * the monster returned.
 */
int
pm_mkclass(class,spc)
char    class;
int     spc;
{
	register int	first, last, num = 0;
	int maxmlev, mask = (G_NOGEN | G_UNIQ) & ~spc;

	maxmlev = level_difficulty() /*>> 1*/; /* what the heck? does that divide the actual result by 2?! --Amy */
	    if (!rn2(100)) maxmlev *= 2;
	    if (!rn2(1000)) maxmlev *= 4;
	    if (!rn2(5)) maxmlev += rnz(2);
	    if (!rn2(20)) maxmlev += rnz(3);
	    if (!rn2(100)) maxmlev += rnz(4);
	    if (!rn2(10000)) maxmlev = 127;
		if (maxmlev > 127) maxmlev = 127; /* very important! The game might otherwise crash or become unstable! */
	if(class < 1 || class >= MAXMCLASSES) {
	    impossible("mkclass called with bad class!");
	    return(-1);
	}

	/*pline("max spawn level %d",maxmlev);*/

/*	Assumption #1:	monsters of a given class are contiguous in the
 *			mons[] array.
 */
	for (first = LOW_PM; first < SPECIAL_PM; first++)
	    if (mons[first].mlet == class) break;
	if (first == SPECIAL_PM) return (-1);

	for (last = first;
		last < SPECIAL_PM && mons[last].mlet == class; last++)
	    if (!(mvitals[last].mvflags & G_GONE) && !(mons[last].geno & mask)
					&& !is_placeholder(&mons[last])) {
		/* consider it */
		if(spc & MKC_ULIMIT && toostrong(last, 4 * maxmlev)) break;
		if(num && toostrong(last, maxmlev) &&
		   monstr[last] != monstr[last-1] && rn2(2)) break;
		num += mons[last].geno & G_FREQ;
	    }

	if(!num) return(-1);

/*	Assumption #2:	monsters of a given class are presented in ascending
 *			order of strength.
 */
	for(num = rnd(num); num > 0; first++)
	    if (!(mvitals[first].mvflags & G_GONE) && !(mons[first].geno & mask)
					&& !is_placeholder(&mons[first])) {
		/* skew towards lower value monsters at lower exp. levels */
		num -= mons[first].geno & G_FREQ;
		if (num && adj_lev(&mons[first]) > (u.ulevel*2)) {
		    /* but not when multiple monsters are same level */
		    if (mons[first].mlevel != mons[first+1].mlevel)
			num--;
		}
	    }
	first--; /* correct an off-by-one error */

	return(first);
}

int
adj_lev(ptr)	/* adjust strength of monsters based on u.uz and u.ulevel */
register struct permonst *ptr;
{
	int	tmp, tmp2;

	if (ptr == &mons[PM_WIZARD_OF_YENDOR]) {
		/* does not depend on other strengths, but does get stronger
		 * every time he is killed
		 */
		tmp = ptr->mlevel + mvitals[PM_WIZARD_OF_YENDOR].died;
		if (tmp > 49) tmp = 49;
		return tmp;
	}

	if((tmp = ptr->mlevel) > 49) return(50); /* "special" demons/devils */
	tmp2 = (level_difficulty() - tmp);
	if(tmp2 < 0) tmp--;		/* if mlevel > u.uz decrement tmp */
	else tmp += (tmp2 / 5);		/* else increment 1 per five diff */

	tmp2 = (u.ulevel - ptr->mlevel);	/* adjust vs. the player */
	if(tmp2 > 0) tmp += (tmp2 / 4);		/* level as well */

	tmp2 = (3 * ((int) ptr->mlevel))/ 2;	/* crude upper limit */
	if (tmp2 > 49) tmp2 = 49;		/* hard upper limit */
	return((tmp > tmp2) ? tmp2 : (tmp > 0 ? tmp : 0)); /* 0 lower limit */
}

#endif /* OVL1 */
#ifdef OVLB

struct permonst *
grow_up(mtmp, victim)	/* `mtmp' might "grow up" into a bigger version */
struct monst *mtmp, *victim;
{
	int oldtype, newtype, max_increase, cur_increase,
	    lev_limit, hp_threshold;
	struct permonst *ptr = mtmp->data;

	/* monster died after killing enemy but before calling this function */
	/* currently possible if killing a gas spore */
	if (mtmp->mhp <= 0)
	    return ((struct permonst *)0);

	if (mtmp->oldmonnm != monsndx(ptr))
	    return ptr;		/* No effect if polymorphed */

	/* note:  none of the monsters with special hit point calculations
	   have both little and big forms */
	oldtype = monsndx(ptr);
	newtype = little_to_big(oldtype);
	if (newtype == PM_PRIEST && mtmp->female) newtype = PM_PRIESTESS;

	/* growth limits differ depending on method of advancement */
	if (victim) {		/* killed a monster */
	    /*
	     * The HP threshold is the maximum number of hit points for the
	     * current level; once exceeded, a level will be gained.
	     * Possible bug: if somehow the hit points are already higher
	     * than that, monster will gain a level without any increase in HP.
	     */
	    hp_threshold = mtmp->m_lev * 8;		/* normal limit */
	    if (!mtmp->m_lev)
		hp_threshold = 4;
	    else if (is_golem(ptr))	/* strange creatures */
		hp_threshold = ((mtmp->mhpmax / 10) + 1) * 10 - 1;
	    else if (is_home_elemental(ptr))
		hp_threshold *= 3;
	    lev_limit = /*3 * (int)ptr->mlevel / 2*/50;	/* same as adj_lev() */
	    /* If they can grow up, be sure the level is high enough for that */
	    if (oldtype != newtype && mons[newtype].mlevel > lev_limit)
		lev_limit = (int)mons[newtype].mlevel;
	    /* number of hit points to gain; unlike for the player, we put
	       the limit at the bottom of the next level rather than the top */
	    max_increase = rnd((int)victim->m_lev + 1);
	    if (mtmp->mhpmax + max_increase > hp_threshold + 1)
		max_increase = max((hp_threshold + 1) - mtmp->mhpmax, 0);
	    cur_increase = (max_increase > 1) ? rn2(max_increase) : 0;
	} else {
	    /* a gain level potion or wraith corpse; always go up a level
	       unless already at maximum (49 is hard upper limit except
	       for demon lords, who start at 50 and can't go any higher) */
	    max_increase = cur_increase = rnd(8) + rnd(2);
	    hp_threshold = 0;	/* smaller than `mhpmax + max_increase' */
	    lev_limit = 50;		/* recalc below */
	}

	mtmp->mhpmax += max_increase;
	mtmp->mhp += cur_increase;

	mtmp->m_enmax += max_increase;
	mtmp->m_en += cur_increase;
	
	if (mtmp->mhpmax <= hp_threshold)
	    return ptr;		/* doesn't gain a level */

	/* Allow to grow up even if grown up form would normally be
	 * out of range */
	if (lev_limit < mons[newtype].mlevel)
	    lev_limit = mons[newtype].mlevel;

	if (is_mplayer(ptr)) lev_limit = 30;    /* same as player */
	else if (lev_limit < 5) lev_limit = 5;  /* arbitrary */
	else if (lev_limit > 49) lev_limit = (ptr->mlevel > 49 ? 50 : 49);

	if ((int)++mtmp->m_lev >= mons[newtype].mlevel && newtype != oldtype) {
	    ptr = &mons[newtype];

		/* stupid sensemon function! Screw it, evolving messages will always be displayed now. --Amy */

		/*if (sensemon(mtmp)) {*/
			pline("What? %s is evolving!", mon_nam(mtmp) );
			pline("%s evolved into %s!", mon_nam(mtmp), an(ptr->mname) );
		/*}*/

	    if (mvitals[newtype].mvflags & G_GENOD) {	/* allow G_EXTINCT */
		if (sensemon(mtmp))
		    pline("As %s grows up into %s, %s %s!", mon_nam(mtmp),
			an(ptr->mname), mhe(mtmp),
			nonliving(ptr) ? "expires" : "dies");
		set_mon_data(mtmp, ptr, -1);	/* keep mvitals[] accurate */
		mondied(mtmp);
		return (struct permonst *)0;
	    }
	    set_mon_data(mtmp, ptr, 1);		/* preserve intrinsics */
	    newsym(mtmp->mx, mtmp->my);		/* color may change */
	    lev_limit = (int)mtmp->m_lev;	/* never undo increment */
	}
	/* sanity checks */
	if ((int)mtmp->m_lev > lev_limit) {
	    mtmp->m_lev--;	/* undo increment */
	    /* HP might have been allowed to grow when it shouldn't */
	    if (mtmp->mhpmax == hp_threshold + 1) mtmp->mhpmax--;
	}
	if (mtmp->mhpmax > 500*8) mtmp->mhpmax = 500*8;	  /* absolute limit */
	if (mtmp->mhp > mtmp->mhpmax) mtmp->mhp = mtmp->mhpmax;

	if (mtmp->m_enmax > 500*8) mtmp->m_enmax = 500*8;     /* absolute limit */
	if (mtmp->m_en > mtmp->m_enmax) mtmp->m_en = mtmp->m_enmax;

	if (mtmp->oldmonnm != monsndx(ptr)) mtmp->oldmonnm = monsndx(ptr);
	return ptr;
}

#endif /* OVLB */
#ifdef OVL1

int
mongets(mtmp, otyp)
register struct monst *mtmp;
register int otyp;
{
	register struct obj *otmp;
	int spe;

	if (!otyp) return 0;
	otmp = mksobj(otyp, TRUE, FALSE);
	if (otmp) {
	    if (mtmp->data->mlet == S_DEMON) {
		/* demons never get blessed objects */
		if (otmp->blessed) curse(otmp);
	    } else if(is_lminion(mtmp)) {
		/* lawful minions don't get cursed, bad, or rusting objects */
		otmp->cursed = FALSE;
		if(otmp->spe < 0) otmp->spe = 0;
		otmp->oerodeproof = TRUE;
	    } else if(In_endgame(&u.uz) && is_mplayer(mtmp->data) && is_sword(otmp)) {
		otmp->spe = (3 + rn2(3));
	    }
/*
 *      This seems to be covered under mkobj.c ...
 *
 *          * STEPHEN WHITE'S NEW CODE *
 *
 *          if ((otmp->otyp == ORCISH_DAGGER      && !rn2(8))  ||
 *              (otmp->otyp == ORCISH_SPEAR       && !rn2(10)) ||
 *              (otmp->otyp == ORCISH_SHORT_SWORD && !rn2(12)))
 *                      otmp->opoisoned = TRUE;
 *
 *           * It could be alread immune to rust ... *
 *          if (!otmp->oerodeproof && !is_rustprone(otmp) &&
 *              ((otmp->otyp >= SPEAR && otmp->otyp <= BULLWHIP) ||
 *               (otmp->otyp >= ELVEN_LEATHER_HELM &&
 *                otmp->otyp <= LEVITATION_BOOTS))) {
 *                   if (!rn2(10-otmp->spe)) otmp->oerodeproof = TRUE;
 *              else if (!rn2(10+otmp->spe)) otmp->oeroded = rn2(3);
 *          }
 */

	    if(otmp->otyp == CANDELABRUM_OF_INVOCATION) {
		otmp->spe = 0;
		otmp->age = 0L;
		otmp->lamplit = FALSE;
		otmp->blessed = otmp->cursed = FALSE;
	    } else if (otmp->otyp == BELL_OF_OPENING) {
		otmp->blessed = otmp->cursed = FALSE;
	    } else if (otmp->otyp == SPE_BOOK_OF_THE_DEAD) {
		otmp->blessed = FALSE;
		otmp->cursed = TRUE;
	    }

	    /* leaders don't tolerate inferior quality battle gear */
	    if (is_prince(mtmp->data)) {
		if (otmp->oclass == WEAPON_CLASS && otmp->spe < 1)
		    otmp->spe = 1;
		else if (otmp->oclass == ARMOR_CLASS && otmp->spe < 0)
		    otmp->spe = 0;
	    }

	    spe = otmp->spe;
	    (void) mpickobj(mtmp, otmp);	/* might free otmp */
	    return(spe);
	} else return(0);
}

#endif /* OVL1 */
#ifdef OVLB

int
golemhp(type)
int type;
{
	switch(type) {
		case PM_PAPER_GOLEM: return 36;
		case PM_STRAW_GOLEM: return 40;
		case PM_GARGOYLE: return 46;
		case PM_ROPE_GOLEM: return 60;
		case PM_LEATHER_GOLEM: return 80;
		case PM_GOLD_GOLEM: return 80;
		case PM_BRONZE_GOLEM: return 80;
		case PM_WOOD_GOLEM: return 100;
		case PM_GEODUDE: return 100;
		case PM_ANIMATED_ARMOR: return 100;
		case PM_ANIMATED_WEDGE_SANDAL: return 100;
		case PM_FLESH_GOLEM: return 120;
		case PM_SPELL_GOLEM: return 130;
		case PM_STATUE_GARGOYLE: return 140;
		case PM_CLAY_GOLEM: return 150;
		case PM_CLAY_STATUE: return 150;
		case PM_TREASURY_GOLEM: return 160;
		case PM_SNOW_GOLEM: return 160;
		case PM_STONE_GOLEM: return 180;
		case PM_STONE_STATUE: return 180;
		case PM_GLASS_GOLEM: return 140;
		case PM_BONE_GOLEM: return 200;
		case PM_ANIMATED_LEATHER_PEEP_TOE: return 250;
		case PM_IRON_GOLEM: return 240;
		case PM_ARGENTUM_GOLEM: return 240;
		case PM_ICE_GOLEM: return 240;
		case PM_ICE_STATUE: return 240;
		case PM_MIST_GOLEM: return 240;
		case PM_GRAVELER: return 250;
		case PM_RETRIEVER: return 250;
		case PM_RUBY_GOLEM: return 250;
		case PM_ANIMATED_SEXY_LEATHER_PUMP: return 250;
		case PM_DIAMOND_GOLEM: return 270;
		case PM_SAPPHIRE_GOLEM: return 280;
		case PM_STEEL_GOLEM: return 290;
		case PM_CRYSTAL_GOLEM: return 300;
		case PM_CRYSTAL_STATUE: return 300;
		case PM_CRYSTAL_ICE_GOLEM: return 300;
		case PM_ANIMATED_COMBAT_STILETTO: return 300;
		case PM_ETERNIUM_GOLEM: return 350;
		case PM_FRANKENSTEIN_S_MONSTER: return 400;
		case PM_GOLEM: return 400;
		case PM_BLOODY_BEAUTIES: return 500;
		case PM_WAX_GOLEM: return 40;
		case PM_PLASTIC_GOLEM: return 60;
		default: return 30; /* failsafe so undefined golems don't start with zero hp! --Amy */
	}
}

#endif /* OVLB */
#ifdef OVL1

/*
 *	Alignment vs. yours determines monster's attitude to you.
 *	( some "animal" types are co-aligned, but also hungry )
 */

boolean
peace_minded(ptr)
register struct permonst *ptr;
{
	if (Race_if(PM_ALBAE)) return FALSE; /* albae are hated by all other races --Amy */

	aligntyp mal = ptr->maligntyp, ual = u.ualign.type;

	if (always_peaceful(ptr)) return TRUE;

	if (ptr->mlet == S_KOBOLD && Race_if(PM_KOBOLT) && !Role_if(PM_CONVICT)) return TRUE;
	if (ptr->mlet == S_OGRE && Race_if(PM_OGRO) && !Role_if(PM_CONVICT)) return TRUE;
	if (ptr->mlet == S_GIANT && Race_if(PM_GIGANT) && !Role_if(PM_CONVICT)) return TRUE;
	if (ptr->mlet == S_TROLL && Race_if(PM_TROLLOR) && !Role_if(PM_CONVICT)) return TRUE;
	if (ptr->mlet == S_ANT && Race_if(PM_INSECTOID) && !Role_if(PM_CONVICT)) return TRUE; /* Go Team Ant! --Amy */
	if (ptr->mlet == S_SNAKE && Race_if(PM_SNAKEMAN) && !Role_if(PM_CONVICT)) return TRUE;
	if (ptr->mlet == S_SPIDER && Race_if(PM_SPIDERMAN) && !Role_if(PM_CONVICT)) return TRUE;
	if (ptr->mlet == S_LIZARD && Race_if(PM_ARGONIAN) && !Role_if(PM_CONVICT)) return TRUE;
	if (ptr->mlet == S_NYMPH && Race_if(PM_NYMPH) && !Role_if(PM_CONVICT)) return TRUE;
	if (ptr->mlet == S_HUMANOID && Race_if(PM_ILLITHID) && !Role_if(PM_CONVICT)) return TRUE;

	if (always_hostile(ptr)) return FALSE;
	if (ptr->msound == MS_LEADER || ptr->msound == MS_GUARDIAN)
		return TRUE;
	if (ptr->msound == MS_NEMESIS)	return FALSE;

	if (is_elf(ptr) && is_elf(youmonst.data)) {
		/* Light and dark elves are always hostile to each other.
		 * Suggested by Dr. Eva R. Myers.
		 */
		 if (ual > A_NEUTRAL && mal < A_NEUTRAL ||
		   ual < A_NEUTRAL && mal > A_NEUTRAL)
			return FALSE;
	}

	if (race_peaceful(ptr)) return TRUE;
	if (race_hostile(ptr)) return FALSE;

	/* the monster is hostile if its alignment is different from the
	 * player's */
	if (sgn(mal) != sgn(ual)) return FALSE;

	/* Negative monster hostile to player with Amulet. */
	if (mal < A_NEUTRAL && u.uhave.amulet) return FALSE;

	/* minions are hostile to players that have strayed at all */
	/* and they can also be hostile to players with good alignment --Amy */
	if (is_minion(ptr) && u.ualign.record < 0) return FALSE; /*return((boolean)(u.ualign.record >= 0));*/ 

	/* Last case:  a chance of a co-aligned monster being
	 * hostile.  This chance is greater if the player has strayed
	 * (u.ualign.record negative) or the monster is not strongly aligned.
	 * Edit by Amy - much higher chance of the monster being hostile.
	 */
	/*return((boolean)(!!rn2(16 + (u.ualign.record < -15 ? -15 : u.ualign.record)) &&
		!!rn2(2 + abs(mal))));*/
	if (u.ualign.record < 0 && rn2(100)) return FALSE;
	if (u.ualign.record == 0 && rn2(50)) return FALSE;
	if (u.ualign.record >= 1 && rnd(u.ualign.record) < (5 + rnd(20)) && rn2(33) ) return FALSE;
	if (!rn2(5 + abs(mal))) return FALSE;
	else return TRUE;
}

/* Set malign to have the proper effect on player alignment if monster is
 * killed.  Negative numbers mean it's bad to kill this monster; positive
 * numbers mean it's good.  Since there are more hostile monsters than
 * peaceful monsters, the penalty for killing a peaceful monster should be
 * greater than the bonus for killing a hostile monster to maintain balance.
 * Rules:
 *   it's bad to kill peaceful monsters, potentially worse to kill always-
 *	peaceful monsters
 *   it's never bad to kill a hostile monster, although it may not be good
 */
void
set_malign(mtmp)
struct monst *mtmp;
{
	schar mal = mtmp->data->maligntyp;
	boolean coaligned;

	if (mtmp->ispriest || mtmp->isminion) {
		/* some monsters have individual alignments; check them */
		if (mtmp->ispriest)
			mal = EPRI(mtmp)->shralign;
		else if (mtmp->isminion)
			mal = EMIN(mtmp)->min_align;
		/* unless alignment is none, set mal to -5,0,5 */
		/* (see align.h for valid aligntyp values)     */
		if(mal != A_NONE)
			mal *= 5;
		/* make priests of Moloch hostile */
		if (mal == A_NONE) mtmp->mpeaceful = 0;
	}


	coaligned = (sgn(mal) == sgn(u.ualign.type));
	if (mtmp->data->msound == MS_LEADER) {
		mtmp->malign = -20;
	} else if (mal == A_NONE) {
		if (mtmp->mpeaceful)
			mtmp->malign = 0;
		else
			mtmp->malign = 20;	/* really hostile */
	} else if (always_peaceful(mtmp->data)) {
		int absmal = abs(mal);
		if (mtmp->mpeaceful)
			mtmp->malign = -3*max(5,absmal);
		else
			mtmp->malign = 3*max(5,absmal); /* renegade */
	} else if (always_hostile(mtmp->data)) {
		int absmal = abs(mal);
		if (coaligned)
			mtmp->malign = 0;
		else
			mtmp->malign = max(5,absmal);
	} else if (coaligned) {
		int absmal = abs(mal);
		if (mtmp->mpeaceful)
			mtmp->malign = -3*max(3,absmal);
		else	/* renegade */
			mtmp->malign = max(3,absmal);
	} else	/* not coaligned and therefore hostile */
		mtmp->malign = abs(mal);
	if (mtmp->malign >= 1 && rn2(5))  mtmp->malign /= 3; /*rebuilding alignment should be harder --Amy*/
}

#endif /* OVL1 */
#ifdef OVLB

static NEARDATA char syms[] = {
	MAXOCLASSES, MAXOCLASSES+1, RING_CLASS, WAND_CLASS, WEAPON_CLASS,
	FOOD_CLASS, COIN_CLASS, SCROLL_CLASS, POTION_CLASS, ARMOR_CLASS,
	AMULET_CLASS, TOOL_CLASS, ROCK_CLASS, GEM_CLASS, SPBOOK_CLASS,
	S_MIMIC_DEF, S_MIMIC_DEF, S_MIMIC_DEF,
};

void
set_mimic_sym(mtmp)		/* KAA, modified by ERS */
register struct monst *mtmp;
{
	int typ, roomno, rt;
	unsigned appear, ap_type;
	int s_sym;
	struct obj *otmp;
	int mx, my;

	if (!mtmp) return;
	mx = mtmp->mx; my = mtmp->my;
	typ = levl[mx][my].typ;
					/* only valid for INSIDE of room */
	roomno = levl[mx][my].roomno - ROOMOFFSET;
	if (roomno >= 0)
		rt = rooms[roomno].rtype;
#ifdef SPECIALIZATION
	else if (IS_ROOM(typ))
		rt = OROOM,  roomno = 0;
#endif
	else	rt = 0;	/* roomno < 0 case for GCC_WARN */

	if (OBJ_AT(mx, my)) {
		ap_type = M_AP_OBJECT;
		appear = level.objects[mx][my]->otyp;
	} else if (IS_DOOR(typ) || IS_WALL(typ) ||
		   typ == SDOOR || typ == SCORR) {
		ap_type = M_AP_FURNITURE;
		/*
		 *  If there is a wall to the left that connects to this
		 *  location, then the mimic mimics a horizontal closed door.
		 *  This does not allow doors to be in corners of rooms.
		 */
		if (mx != 0 &&
			(levl[mx-1][my].typ == HWALL    ||
			 levl[mx-1][my].typ == TLCORNER ||
			 levl[mx-1][my].typ == TRWALL   ||
			 levl[mx-1][my].typ == BLCORNER ||
			 levl[mx-1][my].typ == TDWALL   ||
			 levl[mx-1][my].typ == CROSSWALL||
			 levl[mx-1][my].typ == TUWALL    ))
		    appear = S_hcdoor;
		else
		    appear = S_vcdoor;

		if(!mtmp->minvis || See_invisible)
		    block_point(mx,my);	/* vision */
	} else if (level.flags.is_maze_lev && rn2(2)) {
		ap_type = M_AP_OBJECT;
		appear = STATUE;
	} else if (roomno < 0) {
		ap_type = M_AP_OBJECT;

		if (!rn2(5)) {

			ap_type = M_AP_FURNITURE;

			switch (rnd(19)) {

				case 1:		appear = S_stone; break;
				case 2:		appear = S_bars; break;
				case 3:		appear = S_tree; break;
				case 4:		appear = S_room; break;
				case 5:		appear = S_darkroom; break;
				case 6:		appear = S_corr; break;
				case 7:		appear = S_litcorr; break;
				case 8:		appear = S_altar; break;
				case 9:		appear = S_grave; break;
				case 10:		appear = S_throne; break;
				case 11:		appear = S_sink; break;
				case 12:		appear = S_fountain; break;
				case 13:		appear = S_toilet; break;
				case 14:		appear = S_pool; break;
				case 15:		appear = S_ice; break;
				case 16:		appear = S_lava; break;
				case 17:		appear = S_air; break;
				case 18:		appear = S_cloud; break;
				case 19:		appear = S_water; break;
				default:		appear = S_stone; break;

			}

		}

		else if (rn2(10)) {

		s_sym = syms[rn2((int)sizeof(syms))];
		if (s_sym >= MAXOCLASSES) {
			ap_type = M_AP_FURNITURE;
			appear = s_sym == MAXOCLASSES ? S_upstair : S_dnstair;
		} else if (s_sym == COIN_CLASS) {
			ap_type = M_AP_OBJECT;
			appear = GOLD_PIECE;
		} else {
			ap_type = M_AP_OBJECT;
			if (s_sym == S_MIMIC_DEF) {
				appear = STRANGE_OBJECT;
			} else {
				otmp = mkobj( (char) s_sym, FALSE );
				appear = otmp->otyp;
				/* make sure container contents are free'ed */
				if (Has_contents(otmp))
					delete_contents(otmp);
				obfree(otmp, (struct obj *) 0);
			}
		}

		}
		else
		appear = BOULDER;
		if(!mtmp->minvis || See_invisible)
		    block_point(mx,my);	/* vision */
	} else if (rt == ZOO || rt == VAULT) {
		ap_type = M_AP_OBJECT;
		appear = GOLD_PIECE;
	} else if (rt == DELPHI) {
		if (rn2(2)) {
			ap_type = M_AP_OBJECT;
			appear = STATUE;
		} else {
			ap_type = M_AP_FURNITURE;
			appear = S_fountain;
		}
	} else if (rt == TEMPLE) {
		ap_type = M_AP_FURNITURE;
		appear = S_altar;
	/*
	 * We won't bother with beehives, morgues, barracks, throne rooms
	 * since they shouldn't contain too many mimics anyway...
	 */
	} else if (rt >= SHOPBASE) {
		s_sym = get_shop_item(rt - SHOPBASE);
		if (s_sym < 0) {
			ap_type = M_AP_OBJECT;
			appear = -s_sym;
		} else {
			if (s_sym == RANDOM_CLASS)
				s_sym = syms[rn2((int)sizeof(syms)-2) + 2];
			goto assign_sym;
		}
	} else {
		s_sym = syms[rn2((int)sizeof(syms))];
assign_sym:

		if (!rn2(5)) {

			ap_type = M_AP_FURNITURE;

			switch (rnd(19)) {

				case 1:		appear = S_stone; break;
				case 2:		appear = S_bars; break;
				case 3:		appear = S_tree; break;
				case 4:		appear = S_room; break;
				case 5:		appear = S_darkroom; break;
				case 6:		appear = S_corr; break;
				case 7:		appear = S_litcorr; break;
				case 8:		appear = S_altar; break;
				case 9:		appear = S_grave; break;
				case 10:		appear = S_throne; break;
				case 11:		appear = S_sink; break;
				case 12:		appear = S_fountain; break;
				case 13:		appear = S_toilet; break;
				case 14:		appear = S_pool; break;
				case 15:		appear = S_ice; break;
				case 16:		appear = S_lava; break;
				case 17:		appear = S_air; break;
				case 18:		appear = S_cloud; break;
				case 19:		appear = S_water; break;
				default:		appear = S_stone; break;

			}

		}

		else if (s_sym >= MAXOCLASSES) {
			ap_type = M_AP_FURNITURE;
			appear = s_sym == MAXOCLASSES ? S_upstair : S_dnstair;
		} else if (s_sym == COIN_CLASS) {
			ap_type = M_AP_OBJECT;
			appear = GOLD_PIECE;
		} else {
			ap_type = M_AP_OBJECT;
			if (s_sym == S_MIMIC_DEF) {
				appear = STRANGE_OBJECT;
			} else {
				otmp = mkobj( (char) s_sym, FALSE );
				appear = otmp->otyp;
				/* make sure container contents are free'ed */
				if (Has_contents(otmp))
					delete_contents(otmp);
				obfree(otmp, (struct obj *) 0);
			}
		}
	}
	mtmp->m_ap_type = ap_type;
	mtmp->mappearance = appear;
}

/* release a monster from a bag of tricks */
void
bagotricks(bag)
struct obj *bag;
{
    if (!bag || bag->otyp != BAG_OF_TRICKS) {
	impossible("bad bag o' tricks");
    } else if (bag->spe < 1) {
	pline(nothing_happens);
    } else {
	boolean gotone = FALSE;
	int cnt = 1;

	consume_obj_charge(bag, TRUE);

	if (!rn2(23)) cnt += rn1(7, 1);
	while (cnt-- > 0) {
	    if (makemon((struct permonst *)0, u.ux, u.uy, NO_MM_FLAGS))
		gotone = TRUE;
	}
	if (gotone) makeknown(BAG_OF_TRICKS);
    }
}

#endif /* OVLB */

/*makemon.c*/
