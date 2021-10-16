/*	SCCS Id: @(#)uhitm.c	3.4	2003/02/18	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

STATIC_DCL boolean FDECL(known_hitum, (struct monst *,int,int *,struct attack *));
STATIC_DCL void FDECL(steal_it, (struct monst *, struct attack *));
#if 0
STATIC_DCL boolean FDECL(hitum, (struct monst *,int,int,struct attack *));
#endif
STATIC_DCL boolean FDECL(hmon_hitmon, (struct monst *,struct obj *,int));
#ifdef STEED
STATIC_DCL int FDECL(joust, (struct monst *,struct obj *));
#endif
STATIC_DCL void NDECL(demonpet);
STATIC_DCL boolean FDECL(m_slips_free, (struct monst *mtmp,struct attack *mattk));
STATIC_DCL int FDECL(explum, (struct monst *,struct attack *));
STATIC_DCL void FDECL(start_engulf, (struct monst *));
STATIC_DCL void NDECL(end_engulf);
STATIC_DCL int FDECL(gulpum, (struct monst *,struct attack *));
STATIC_DCL boolean FDECL(hmonas, (struct monst *,int));
STATIC_DCL void FDECL(nohandglow, (struct monst *));
STATIC_DCL boolean FDECL(shade_aware, (struct obj *));

static int NDECL(martial_dmg);

extern boolean notonhead;	/* for long worms */
/* The below might become a parameter instead if we use it a lot */
static int dieroll;
static int rolls[2][2];
#define dice(x) rolls[0][x]
#define tohit(x) rolls[1][x]
#define UWEP_ROLL	0
#define USWAPWEP_ROLL	1

/* Used to flag attacks caused by Stormbringer's maliciousness. */
static boolean override_confirmation = 0;

/* Used to control whether Drow's sleep attack should succeed. */
static boolean barehanded_hit = 0;

/* WAC for mhit,  two weapon attacking */
#define HIT_UWEP 	1
#define HIT_USWAPWEP 	2
#define HIT_BODY 	4		/* Hit with other body part */
#define HIT_OTHER 	8		/* Hit without touching */
#define HIT_FATAL 	16

#define PROJECTILE(obj)	((obj) && is_ammo(obj))

/* modified from hurtarmor() in mhitu.c */
/* This is not static because it is also used for monsters rusting monsters */
void
hurtmarmor(mdef, attk)
struct monst *mdef;
int attk;
{
	int	hurt;
	struct obj *target;

	switch(attk) {
	    /* 0 is burning, which we should never be called with */
	    case AD_RUST: hurt = 1; break;
	    case AD_CORR: hurt = 3; break;
	    default: hurt = 2; break;
	}
	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		target = which_armor(mdef, W_ARMH);
		if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
		    continue;
		break;
	    case 1:
		target = which_armor(mdef, W_ARMC);
		if (target) {
		    (void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
		    break;
		}
		if ((target = which_armor(mdef, W_ARM)) != (struct obj *)0) {
		    (void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
#ifdef TOURIST
		} else if ((target = which_armor(mdef, W_ARMU)) != (struct obj *)0) {
		    (void)rust_dmg(target, xname(target), hurt, TRUE, mdef);
#endif
		}
		break;
	    case 2:
		target = which_armor(mdef, W_ARMS);
		if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
		    continue;
		break;
	    case 3:
		target = which_armor(mdef, W_ARMG);
		if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
		    continue;
		break;
	    case 4:
		target = which_armor(mdef, W_ARMF);
		if (!target || !rust_dmg(target, xname(target), hurt, FALSE, mdef))
		    continue;
		break;
	    }
	    break; /* Out of while loop */
	}
}

/*
 * Now returns a bit mask of attacks that may proceed. Note that barehanded
 * returns HIT_UWEP. -ALI
 */

int
attack_checks(mtmp, barehanded)
register struct monst *mtmp;
boolean barehanded;
{
	int retval;
	char qbuf[QBUFSZ];

	if (barehanded || !u.twoweap || !uswapwep)
		retval = HIT_UWEP;
	else
		retval = HIT_UWEP | HIT_USWAPWEP;

	/* if you're close enough to attack, alert any waiting monster */
	mtmp->mstrategy &= ~STRAT_WAITMASK;

	if (u.uswallow && mtmp == u.ustuck) return retval;

	if (flags.forcefight) {
		/* Do this in the caller, after we checked that the monster
		 * didn't die from the blow.  Reason: putting the 'I' there
		 * causes the hero to forget the square's contents since
		 * both 'I' and remembered contents are stored in .glyph.
		 * If the monster dies immediately from the blow, the 'I' will
		 * not stay there, so the player will have suddenly forgotten
		 * the square's contents for no apparent reason.
		if (!canspotmon(mtmp) &&
		    !memory_is_invisible(u.ux+u.dx, u.uy+u.dy))
			map_invisible(u.ux+u.dx, u.uy+u.dy);
		 */
		return retval;
	}

	/* Put up an invisible monster marker, but with exceptions for
	 * monsters that hide and monsters you've been warned about.
	 * The former already prints a warning message and
	 * prevents you from hitting the monster just via the hidden monster
	 * code below; if we also did that here, similar behavior would be
	 * happening two turns in a row.  The latter shows a glyph on
	 * the screen, so you know something is there.
	 */
	if (!canspotmon(mtmp) &&
		    !glyph_is_warning(glyph_at(u.ux+u.dx,u.uy+u.dy)) &&
		    !memory_is_invisible(u.ux+u.dx, u.uy+u.dy) &&
		    !(!Blind && mtmp->mundetected && hides_under(mtmp->data))) {
		pline("Wait!  There's %s there you can't see!",
			something);
		map_invisible(u.ux+u.dx, u.uy+u.dy);
		/* if it was an invisible mimic, treat it as if we stumbled
		 * onto a visible mimic
		 */
		if(mtmp->m_ap_type && !Protection_from_shape_changers) {
		    if(!u.ustuck && !mtmp->mflee && dmgtype(mtmp->data,AD_STCK))
			setustuck(mtmp);
		}
		wakeup(mtmp); /* always necessary; also un-mimics mimics */
		return 0;
	}

	if (mtmp->m_ap_type && !Protection_from_shape_changers &&
	   !sensemon(mtmp) &&
	   !glyph_is_warning(glyph_at(u.ux+u.dx,u.uy+u.dy))) {
		/* If a hidden mimic was in a square where a player remembers
		 * some (probably different) unseen monster, the player is in
		 * luck--he attacks it even though it's hidden.
		 */
		if (memory_is_invisible(mtmp->mx, mtmp->my)) {
		    seemimic(mtmp);
		    return retval;
		}
		stumble_onto_mimic(mtmp);
		return 0;
	}

	if (mtmp->mundetected && !canseemon(mtmp) &&
		!glyph_is_warning(glyph_at(u.ux+u.dx,u.uy+u.dy)) &&
		(hides_under(mtmp->data) || mtmp->data->mlet == S_EEL)) {
	    mtmp->mundetected = mtmp->msleeping = 0;
	    newsym(mtmp->mx, mtmp->my);
	    if (memory_is_invisible(mtmp->mx, mtmp->my)) {
		seemimic(mtmp);
		return retval;
	    }
	    if (!(Blind ? Blind_telepat : Unblind_telepat)) {
		struct obj *obj;

		if (Blind || (is_pool(mtmp->mx,mtmp->my, FALSE) && !Underwater))
		    pline("Wait!  There's a hidden monster there!");
		else if ((obj = level.objects[mtmp->mx][mtmp->my]) != 0)
		    pline("Wait!  There's %s hiding under %s!",
			  an(l_monnam(mtmp)), doname(obj));
		return 0;
	    }
	}

	/*
	 * make sure to wake up a monster from the above cases if the
	 * hero can sense that the monster is there.
	 */
	if ((mtmp->mundetected || mtmp->m_ap_type) && sensemon(mtmp)) {
	    mtmp->mundetected = 0;
	    wakeup(mtmp);
	}

	if (flags.confirm && mtmp->mpeaceful
	    && !Confusion && !Hallucination && !Stunned) {
		/* Intelligent chaotic weapons (Stormbringer) want blood */
		if (!barehanded &&
		  uwep && uwep->oartifact == ART_STORMBRINGER) {
			override_confirmation = HIT_UWEP;
			return retval;
		}
		if (canspotmon(mtmp)) {
			Sprintf(qbuf, "Really attack %s?", mon_nam(mtmp));
			if (yn(qbuf) != 'y') {
				/* Stormbringer is not tricked so easily */
				if (!barehanded && u.twoweap && uswapwep &&
				  uswapwep->oartifact == ART_STORMBRINGER) {
					override_confirmation = HIT_USWAPWEP;
					/* Lose primary attack */
					return HIT_USWAPWEP;
				}
				flags.move = 0;
				return 0;
			}
		}
	}

	return retval;
}

/*
 * It is unchivalrous for a knight to attack the defenseless or from behind.
 */
void
check_caitiff(mtmp)
struct monst *mtmp;
{
	if (Role_if(PM_KNIGHT) && u.ualign.type == A_LAWFUL &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) &&
	    u.ualign.record > -10) {
		u.ualign.sins++;
	    You("caitiff!");
	    adjalign(-5);

	}

	if (Role_if(PM_PALADIN) &&
	    (!mtmp->mcanmove || mtmp->msleeping ||
	     (mtmp->mflee && !mtmp->mavenge)) &&
	    u.ualign.record > -10) {
		u.ualign.sins++;
	    You("caitiff!");
	    adjalign(-5);

	}
}

schar
find_roll_to_hit(mtmp)
register struct monst *mtmp;
{
	schar tmp;
	int tmp2;

	tmp = 1 + Luck + abon() + find_mac(mtmp) + u.uhitinc +
		maybe_polyd(youmonst.data->mlevel, u.ulevel);

	check_caitiff(mtmp);

/*	attacking peaceful creatures is bad for the samurai's giri */
	if (Role_if(PM_SAMURAI) && mtmp->mpeaceful &&
	    u.ualign.record > -10) {
	    You("dishonorably attack the innocent!");
		u.ualign.sins++;
	    adjalign(-5);
	}
#ifdef JEDI
/* as well as for the way of the Jedi */
	if (Role_if(PM_JEDI) && mtmp->mpeaceful &&
	    u.ualign.record > -10) {
	    You("violate the way of the Jedi!");
		u.ualign.sins++;
	    adjalign(-5);
	}
#endif

/*	Adjust vs. (and possibly modify) monster state.		*/

	if(mtmp->mstun) tmp += 2;
	if(mtmp->mflee) tmp += 2;

	if (mtmp->msleeping) {
		mtmp->msleeping = 0;
		tmp += 2;
	}
	if(!mtmp->mcanmove) {
		tmp += 4;
		if(!rn2(10)) {
			mtmp->mcanmove = 1;
			mtmp->mfrozen = 0;
			if(mtmp->data == &mons[PM_CLOCKWORK_AUTOMATON] &&
				!mtmp->mspec_used){
				mtmp->mfrozen = 1;
				mtmp->mcanmove = 0;
			}
		}
	}

	if (is_orc(mtmp->data) && maybe_polyd(is_elf(youmonst.data),
			Race_if(PM_ELF)))
	    tmp++;

#ifdef CONVICT
    /* Adding iron ball as a weapon skill gives a -4 penalty for
    unskilled vs no penalty for non-weapon objects.  Add 4 to
    compensate. */
    if (uwep && (uwep->otyp == HEAVY_IRON_BALL)) {
        tmp += 4;   /* Compensate for iron ball weapon skill -4
                    penalty for unskilled vs no penalty for non-
                    weapon objects. */
    }
#endif /* CONVICT */
#if 0
	if(Role_if(PM_MONK) && !Upolyd) {
	    if (uarm) {
		Your("armor is rather cumbersome...");
		tmp -= urole.spelarmr;
	    } else if (!uwep && !uarms) {
		tmp += (u.ulevel / 3) + 2;
	}
	}
#endif

	if(Role_if(PM_MONK) && !Upolyd) {
		if(!uwep && (!u.twoweap || !uswapwep) && !uarms && 
		  (!uarm || (uarm && uarm->otyp >= ROBE && 
		  	uarm->otyp <= ROBE_OF_WEAKNESS)))
		  	
		  tmp += (u.ulevel / 3) + 2;
		else if (!uwep && (!u.twoweap || !uswapwep)) {
		   pline("Your armor is rather cumbersome...");
		   tmp += (u.ulevel / 9) + 1;
		}
	}
#ifdef JEDI
	if(Role_if(PM_JEDI) && !Upolyd) {
		if (((uwep && is_lightsaber(uwep) && uwep->lamplit) ||
		    (uswapwep && u.twoweap && is_lightsaber(uswapwep) && uswapwep->lamplit)) &&
		   (uarm &&
		   (uarm->otyp < ROBE || uarm->otyp > ROBE_OF_WEAKNESS))){
		    char yourbuf[BUFSZ];
		    You("can't use %s %s effectively in this armor...", shk_your(yourbuf, uwep), xname(uwep));
		    tmp-=20; // sorry
		}
	}
#endif
	/* special class effect uses... */
	if (tech_inuse(T_KIII)) tmp += 4;
	if (tech_inuse(T_BERSERK)) tmp += 2;
	if (tech_inuse(T_SOULEATER)) tmp += 2;

/*	with a lot of luggage, your agility diminishes */
	if ((tmp2 = near_capacity()) != 0) tmp -= (tmp2*2) - 1;
	if (u.utrap) tmp -= 3;
/*	Some monsters have a combination of weapon attacks and non-weapon
 *	attacks.  It is therefore wrong to add hitval to tmp; we must add
 *	it only for the specific attack (in hmonas()).
 */
/* WAC This is now taken care of later in player's case - for twoweapon */
/*
	if (uwep && !Upolyd) {
		tmp += hitval(uwep, mtmp);
		tmp += weapon_hit_bonus(uwep);
	}
*/
	return tmp;
}

/* try to attack; return FALSE if monster evaded */
/* u.dx and u.dy must be set */
boolean
attack(mtmp)
register struct monst *mtmp;
{
	schar tmp;
	register struct permonst *mdat = mtmp->data;
	int mhit;

	/* This section of code provides protection against accidentally
	 * hitting peaceful (like '@') and tame (like 'd') monsters.
	 * Protection is provided as long as player is not: blind, confused,
	 * hallucinating or stunned.
	 * changes by wwp 5/16/85
	 * More changes 12/90, -dkh-. if its tame and safepet, (and protected
	 * 07/92) then we assume that you're not trying to attack. Instead,
	 * you'll usually just swap places if this is a movement command
	 */
	/* Intelligent chaotic weapons (Stormbringer) want blood */
	if (is_safepet(mtmp) && !flags.forcefight) {
	    if ((!uwep || uwep->oartifact != ART_STORMBRINGER) 
		&& (!u.twoweap || !uswapwep 
		   || uswapwep->oartifact != ART_STORMBRINGER)){
		/* there are some additional considerations: this won't work
		 * if in a shop or Punished or you miss a random roll or
		 * if you can walk thru walls and your pet cannot (KAA) or
		 * if your pet is a long worm (unless someone does better).
		 * there's also a chance of displacing a "frozen" monster.
		 * sleeping monsters might magically walk in their sleep.
		 */
		boolean foo = (Punished || !rn2(7) || is_longworm(mtmp->data)),
			inshop = FALSE;
		char *p;

		for (p = in_rooms(mtmp->mx, mtmp->my, SHOPBASE); *p; p++)
		    if (tended_shop(&rooms[*p - ROOMOFFSET])) {
			inshop = TRUE;
			break;
		    }

		if (inshop || foo ||
			(IS_ROCK(levl[u.ux][u.uy].typ) &&
					!passes_walls(mtmp->data))) {
		    char buf[BUFSZ];

		    monflee(mtmp, rnd(6), FALSE, FALSE);
		    Strcpy(buf, y_monnam(mtmp));
		    buf[0] = highc(buf[0]);
		    You("stop.  %s is in the way!", buf);
		    return(TRUE);
		} else if ((mtmp->mfrozen || (! mtmp->mcanmove)
				|| (mtmp->data->mmove == 0)) && rn2(6)) {
		    pline("%s doesn't seem to move!", Monnam(mtmp));
		    return(TRUE);
		} else return(FALSE);
	    }
	}

	/* possibly set in attack_checks;
	   examined in known_hitum, called via hitum or hmonas below */
	override_confirmation = 0;
	mhit = attack_checks(mtmp, !uwep);
	if (!mhit) return(TRUE);

	if (Upolyd) {
		/* certain "pacifist" monsters don't attack */
		if(noattacks(youmonst.data)) {
			You("have no way to attack monsters physically.");
			mtmp->mstrategy &= ~STRAT_WAITMASK;
			goto atk_done;
		}
	}

	if(check_capacity("You cannot fight while so heavily loaded."))
	    goto atk_done;

	if (u.twoweap && !can_twoweapon())
		untwoweapon();

	if(unweapon) {
	    unweapon = FALSE;
	    if(flags.verbose) {
		if(uwep)
		    You("begin bashing monsters with your %s.",
			aobjnam(uwep, (char *)0));
		else if (tech_inuse(T_EVISCERATE))
		    You("begin slashing monsters with your claws.");
		else if (!cantwield(youmonst.data)) {
		    if (P_SKILL(P_MARTIAL_ARTS) >= P_EXPERT)
			You("assume a martial arts stance.");
		    else You("begin %sing monsters with your %s %s.",
			Role_if(PM_MONK) ? "strik" : "bash",
			uarmg ? "gloved" : "bare",	/* Del Lamb */
			makeplural(body_part(HAND)));
	    }
	}
	}

	exercise(A_STR, TRUE);		/* you're exercising muscles */
	/* andrew@orca: prevent unlimited pick-axe attacks */
	u_wipe_engr(3);

	/* Is the "it died" check actually correct? */
	if(mdat->mlet == S_LEPRECHAUN && !mtmp->mfrozen && !mtmp->msleeping &&
	   !mtmp->mconf && mtmp->mcansee && !rn2(7) &&
	   (m_move(mtmp, 0) == 2 ||			    /* it died */
	   mtmp->mx != u.ux+u.dx || mtmp->my != u.uy+u.dy)) /* it moved */
		return(FALSE);

	tmp = find_roll_to_hit(mtmp);
	
	(void) hmonas(mtmp, tmp); /* hmonas handles all attacks now */
	
	/* berserk lycanthropes calm down after the enemy is dead */
	if (mtmp->mhp <= 0) repeat_hit = 0;
/*
	if (Upolyd)
		(void) hmonas(mtmp, tmp);
	else
		(void) hitum(mtmp, tmp, mhit, youmonst.data->mattk);
*/		
	mtmp->mstrategy &= ~STRAT_WAITMASK;

atk_done:
	/* see comment in attack_checks() */
	/* we only need to check for this if we did an attack_checks()
	 * and it returned 0 (it's okay to attack), and the monster didn't
	 * evade.
	 */
	if (flags.forcefight && mtmp->mhp > 0 && !canspotmon(mtmp) &&
	    !memory_is_invisible(u.ux+u.dx, u.uy+u.dy) &&
	    !(u.uswallow && mtmp == u.ustuck))
		map_invisible(u.ux+u.dx, u.uy+u.dy);

	return(TRUE);
}

STATIC_OVL boolean
known_hitum(mon, mattack, mhit, uattk)   /* returns TRUE if monster still lives */
register struct monst *mon;
int mattack;			/* Which weapons you attacked with -ALI */
register int *mhit;
struct attack *uattk;
{
	register boolean malive = TRUE;

	if (override_confirmation) {
	    /* this may need to be generalized if weapons other than
	       Stormbringer acquire similar anti-social behavior... */
	    if (flags.verbose)
		if (override_confirmation == HIT_UWEP)
		    Your("bloodthirsty blade attacks!");
		else
		    pline("The black blade will not be thwarted!");
	}

	if(!*mhit) {
	    if (mattack & HIT_UWEP)
		missum(mon, tohit(UWEP_ROLL), dice(UWEP_ROLL), uattk);
	    if (mattack & HIT_USWAPWEP)
	    	missum(mon, tohit(USWAPWEP_ROLL), dice(USWAPWEP_ROLL), uattk);
	} else {
	    int oldhp = mon->mhp,
		x = u.ux + u.dx, y = u.uy + u.dy;

	    /* we hit the monster; be careful: it might die or
	       be knocked into a different location */
	    notonhead = (mon->mx != x || mon->my != y);
	    if (*mhit & HIT_UWEP) {
		/* KMH, conduct */
		if (uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == CHAIN_CLASS || is_weptool(uwep)))
		    u.uconduct.weaphit++;
		dieroll = dice(UWEP_ROLL);
		malive = hmon(mon, uwep, 0);
	    } else if (mattack & HIT_UWEP)
		missum(mon, tohit(UWEP_ROLL), dice(UWEP_ROLL), uattk);
	    if ((mattack & HIT_USWAPWEP) && malive && m_at(x, y) == mon) {
		/* KMH, ethics */
	    	if (*mhit & HIT_USWAPWEP) {
		    if (uswapwep) u.uconduct.weaphit++;
		    dieroll = dice(USWAPWEP_ROLL);
		    malive = hmon(mon, uswapwep, 0);
	    	} else
		    missum(mon, tohit(USWAPWEP_ROLL), dice(USWAPWEP_ROLL), uattk);
	    }
	    if (malive) {
		/* monster still alive */
		if(!rn2(25) && mon->mhp < mon->mhpmax/2
			    && !(u.uswallow && mon == u.ustuck)) {
		    /* maybe should regurgitate if swallowed? */
		    if(!rn2(3)) {
			monflee(mon, rnd(100), FALSE, TRUE);
		    } else monflee(mon, 0, FALSE, TRUE);

		    if(u.ustuck == mon && !u.uswallow && !sticks(youmonst.data))
			setustuck(0);
		}
		/* Vorpal Blade hit converted to miss */
		/* could be headless monster or worm tail */
		if (mon->mhp == oldhp) {
		    *mhit = 0;
		    /* a miss does not break conduct */
		    if (uwep &&
			(uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == CHAIN_CLASS || is_weptool(uwep)))
			--u.uconduct.weaphit;
		}
		if (mon->wormno && *mhit) {
		    int dohit = *mhit;
		    if (!u.twoweap || (dohit & HIT_UWEP)) {
			if (cutworm(mon, x, y, uwep))
			    dohit = 0;	/* Don't try and cut a worm twice */
		    }
		    if (u.twoweap && (dohit & HIT_USWAPWEP))
			(void) cutworm(mon, x, y, uswapwep);
		}
	    }

	    /* Lycanthropes sometimes go a little berserk! 
	     * If special is on,  they will multihit and stun!
	     */
	    if (( (Race_if(PM_HUMAN_WEREWOLF) || Role_if(PM_LUNATIC) ) && (mon->mhp > 0)) ||
				tech_inuse(T_EVISCERATE)) {
		if (tech_inuse(T_EVISCERATE)) {
		    /*make slashing message elsewhere*/
		    if (repeat_hit == 0) {
			/* [max] limit to 4 (0-3) */
			repeat_hit = (tech_inuse(T_EVISCERATE) > 5) ?
						4 : (tech_inuse(T_EVISCERATE) - 2);
			/* [max] limit to 4 */
			mon->mfrozen = (tech_inuse(T_EVISCERATE) > 5) ?
						4 : (tech_inuse(T_EVISCERATE) - 2); 
		    }
		    mon->mstun = 1;
		    mon->mcanmove = 0;
		} else if (!rn2(24)) {
		    repeat_hit += rn2(4)+1;
		    mon->mfrozen = repeat_hit; /* Lycanthropes suck badly enough already, k? --Amy */
		    mon->mcanmove = 0;
		    /* Length of growl depends on how angry you get */
		    switch (repeat_hit) {
		    	case 0: /* This shouldn't be possible, but... */
			case 1: pline("Grrrrr!"); break;
			case 2: pline("Rarrrgh!"); break;
			case 3: pline("Grrarrgh!"); break;
			case 4: pline("Rarggrrgh!"); break;
			case 5: pline("Raaarrrrrr!"); break;
			case 6: 
			default:pline("Grrrrrrarrrrg!"); break;
		    }
		}
	    }
	}
	return(malive);
}

#if 0 /* Obsolete */
STATIC_OVL boolean
hitum(mon, tmp, mhit, uattk)          /* returns TRUE if monster still lives */
struct monst *mon;
int tmp;
int mhit;
struct attack *uattk;
{
	boolean malive;
	int mattack = mhit;
	int tmp1 = tmp, tmp2 = tmp;

	if (mhit & HIT_UWEP)
	{
		if (uwep) tmp1 += hitval(uwep, mon);
	
		tohit(UWEP_ROLL) = tmp1;
	
		if (tmp1 <= (dice(UWEP_ROLL) = rnd(20)) && !u.uswallow)
			mhit &= ~HIT_UWEP;

		if (tmp1 > dice(UWEP_ROLL)) exercise(A_DEX, TRUE);
#ifdef DEBUG
		pline("(%i/20)", tmp1);
#endif
	}
	
	if (mhit & HIT_USWAPWEP && u.twoweap) {
		if (uswapwep) tmp2 += hitval(uswapwep, mon) - 2;

		tohit(USWAPWEP_ROLL) = tmp2;

		if (tmp2 <= (dice(USWAPWEP_ROLL) = rnd(20)) && !u.uswallow)
			mhit &= ~HIT_USWAPWEP;

		if (tmp2 > dice(USWAPWEP_ROLL)) exercise(A_DEX, TRUE);
#ifdef DEBUG
		pline("((%i/20))", tmp2);
#endif
	}
	
	malive = known_hitum(mon, mattack, &mhit, uattk);
	(void) passive(mon, mhit, malive, AT_WEAP);
	/* berserk lycanthropes calm down after the enemy is dead */
	if (!malive) repeat_hit = 0;
	return(malive);
}
#endif

/* WAC Seperate martial arts damage function */
int
martial_dmg()
{
        int damage;
        /* WAC   plateau at 16 if Monk and Grand Master (6d4)
                            13 if Grand Master
                            11 if Master
                             9 if Expert
                             7 if Skilled
                             5 if Basic  (1d4)
         */

        if ((Role_if(PM_MONK) && !Upolyd)
                && (P_SKILL(P_MARTIAL_ARTS) == P_GRAND_MASTER)
                && (u.ulevel > 16)) damage = d(6,2);                                
        else if (u.ulevel > (2*(P_SKILL(P_MARTIAL_ARTS) - P_BASIC) + 5))
                damage = d((int) (P_SKILL(P_MARTIAL_ARTS) - P_UNSKILLED),2);
        else
                damage = d((int) ((u.ulevel+2)/3),2);

        if((!uarm || (uarm && (uarm->otyp >= ROBE &&
            uarm->otyp <= ROBE_OF_WEAKNESS))) && (!uarms))
                damage *= 2;
        else damage += 2;
        return (damage);
}

boolean			/* general "damage monster" routine */
hmon(mon, obj, thrown)		/* return TRUE if mon still alive */
struct monst *mon;
struct obj *obj;
int thrown;	/* 0: not thrown, 1: launched with uwep,
		   2: launched with uswapwep, 3: thrown by some other means */
{
	boolean result, anger_guards;

	anger_guards = (mon->mpeaceful &&
			    (mon->ispriest || mon->isshk ||
			     mon->data == &mons[PM_WATCHMAN] ||
			     mon->data == &mons[PM_WATCH_LIEUTENANT] ||
			     mon->data == &mons[PM_WATCH_LEADER] ||
			     mon->data == &mons[PM_WATCH_CAPTAIN]));
	result = hmon_hitmon(mon, obj, thrown);
	if (mon->ispriest && !rn2(2)) ghod_hitsu(mon);
	if (anger_guards) (void)angry_guards(!flags.soundok);
	return result;
}

/* guts of hmon() */
STATIC_OVL boolean
hmon_hitmon(mon, obj, thrown)
struct monst *mon;
struct obj *obj;
int thrown;
{
	int tmp, canhitmon = 0, objenchant;
	struct permonst *mdat = mon->data;
	int barehand_silver_rings = 0;
	/* The basic reason we need all these booleans is that we don't want
	 * a "hit" message when a monster dies, so we have to know how much
	 * damage it did _before_ outputting a hit message, but any messages
	 * associated with the damage don't come out until _after_ outputting
	 * a hit message.
	 */
	boolean hittxt = FALSE, destroyed = FALSE, already_killed = FALSE;
	boolean get_dmg_bonus = TRUE;
	boolean ispoisoned = FALSE, needpoismsg = FALSE, poiskilled = FALSE;
	/* Giants instakiilled with sling + stone */
	boolean giantkill = FALSE;
	boolean silvermsg = FALSE, silverobj = FALSE;
	boolean needrustmsg = FALSE;
	boolean disint_obj = FALSE;
	boolean valid_weapon_attack = FALSE;
	boolean unarmed = !uwep && !uarm && !uarms;
	boolean not_melee_weapon = FALSE;
#ifdef STEED
	int jousting = 0;
	int joustdmg;
#endif
	boolean vapekilled = FALSE; /* WAC added boolean for vamps vaporize */
	boolean burnmsg = FALSE;
	boolean shockmsg = FALSE;  /* 5lo: electric version of burnmsg used for electric swords */
	boolean no_obj = !obj;	/* since !obj can change if weapon breaks, etc. */
	boolean noeffect;
	int wtype;
	struct obj *monwep;
	struct obj *launcher;
	char yourbuf[BUFSZ];
	char unconventional[BUFSZ];	/* substituted for word "attack" in msg */
	char saved_oname[BUFSZ];
	int saved_mhp = mon->mhp;

	if (thrown == 1) launcher = uwep;
	else if (thrown == 2) launcher = uswapwep;
	else launcher = 0;

	objenchant = !thrown && no_obj || obj->spe < 0 ? 0 : obj->spe;

	if (need_one(mon))    canhitmon = 1;
	if (need_two(mon))    canhitmon = 2;
	if (need_three(mon))  canhitmon = 3;
	if (need_four(mon))   canhitmon = 4;

	/*
	 * If you are a creature that can hit as a +2 weapon, then YOU can
	 * hit as a +2 weapon. - SW
	 */

	if (Upolyd) {       /* Is Upolyd correct? */
	    /* a monster that needs a +1 weapon to hit it hits as a +1 weapon... */
	    if (need_one(&youmonst))		objenchant = 1;
	    if (need_two(&youmonst))		objenchant = 2;
	    if (need_three(&youmonst))		objenchant = 3;
	    if (need_four(&youmonst))		objenchant = 4;
	    /* overridden by specific flags */
	    if (hit_as_one(&youmonst))		objenchant = 1;
	    if (hit_as_two(&youmonst))		objenchant = 2;
	    if (hit_as_three(&youmonst))	objenchant = 3;
	    if (hit_as_four(&youmonst))		objenchant = 4;
	}

	unconventional[0] = '\0';
	saved_oname[0] = '\0';

	wakeup(mon);
	if(!thrown && no_obj) {      /* attack with bare hands */
	    if (Role_if(PM_MONK) && !Upolyd && u.ulevel/4 > objenchant)
		objenchant = u.ulevel/4;
	    noeffect = objenchant < canhitmon;
	    if (martial_bonus()) {
		if (mdat == &mons[PM_SHADE]) {
		    tmp = rn2(3);
		} else {
		    tmp = martial_dmg();
		}
	    } else {
	    if (mdat == &mons[PM_SHADE])
		tmp = 0;
		else tmp = rnd(2);
	    }

	    valid_weapon_attack = (tmp > 0);

            /* silver gloves give bonuses against silver haters */
            if (uarmg && objects[uarmg->otyp].oc_material == SILVER
                && hates_silver(mdat)) {
                tmp += rnd(20);
                silvermsg = TRUE;
                silverobj = TRUE;
                Strcpy(saved_oname, "gloves");
            }
	    /* Blessed gloves give bonuses when fighting 'bare-handed'.
             * Only one of the silver/blessed bonuses applies.
             */
	    else if (uarmg && uarmg->blessed &&
                     (is_undead(mdat) || is_demon(mdat)))
		tmp += rnd(4);
	    
	    if (uarmg && uarmg->spe) tmp += uarmg->spe; /* WAC plusses from gloves */

	    /* Silver rings also give bonuses.  Note: rings are worn under
             * gloves, so you don't get bonuses from both rings and gloves.
	     */
	    if (!uarmg) {
		if (uleft && objects[uleft->otyp].oc_material == SILVER)
		    barehand_silver_rings++;
		if (uright && objects[uright->otyp].oc_material == SILVER)
		    barehand_silver_rings++;
		if (barehand_silver_rings && hates_silver(mdat)) {
		    tmp += rnd(20);
		    silvermsg = TRUE;
		}

		if (touch_disintegrates(mdat) && !mon->mcan && (mon->mhp>6)){
		    int dis_dmg;
		    valid_weapon_attack =0;
		    Sprintf(unconventional,"barehandedly striking %s", 
		      an(mdat->mname));
		    if (!flags.verbose) You("hit it.");
		    else You("%s %s%s", Role_if(PM_BARBARIAN) ? "smite" : "hit",
		      mon_nam(mon), canseemon(mon) ? exclam(tmp) : ".");
		    dis_dmg = instadisintegrate(unconventional);
		    tmp = min( dis_dmg, tmp);
		    unconventional[0] = '\0';
		    disint_obj = TRUE;
		    hittxt = TRUE;
		}
	  } else {
		if(touch_disintegrates(mdat) && !mon->mcan && (mon->mhp>6) &&
		  !oresist_disintegration(uarmg)){
		    int dis_dmg = uarmg->owt;
		    weight_dmg(dis_dmg);
		    if (!flags.verbose) You("hit it.");
		    else You("%s %s%s", Role_if(PM_BARBARIAN) ? "smite" : "hit",
		      mon_nam(mon), canseemon(mon) ? exclam(tmp) : ".");
		    hittxt = TRUE;
		    destroy_arm(uarmg);
		    tmp = min( dis_dmg, tmp);
		    disint_obj = TRUE;
		    valid_weapon_attack =0;
		}

	    }

	    /* WAC - Hand-to-Hand Combat Techniques */

	    if ((tech_inuse(T_CHI_STRIKE))  && (u.uen > 0)) {
		You("feel a surge of force.");
		tmp += (u.uen > (10 + (u.ulevel / 5)) ? 
			 (10 + (u.ulevel / 5)) : u.uen);
		u.uen -= (10 + (u.ulevel / 5));
		if (u.uen < 0) u.uen = 0;
	    }
	    
	    if (tech_inuse(T_E_FIST)) {
	    	int dmgbonus = 0;
		hittxt = TRUE;
		dmgbonus = noeffect ? 0 : d(2,4);
		switch (rn2(4)) {
		    case 0: /* Fire */
			if (!Blind) pline("%s is on fire!", Monnam(mon));
			dmgbonus += destroy_mitem(mon, SCROLL_CLASS, AD_FIRE);
			dmgbonus += destroy_mitem(mon, SPBOOK_CLASS, AD_FIRE);
			if (noeffect || resists_fire(mon)) {
			    if (!noeffect)
				shieldeff(mon->mx, mon->my);
			    if (!Blind) 
				pline_The("fire doesn't heat %s!", mon_nam(mon));
			    golemeffects(mon, AD_FIRE, dmgbonus);
			    if (!noeffect)
				dmgbonus = 0;
			    else
				noeffect = 0;
			}
			/* only potions damage resistant players in destroy_item */
			dmgbonus += destroy_mitem(mon, POTION_CLASS, AD_FIRE);
			break;
		    case 1: /* Cold */
		    	if (!Blind) pline("%s is covered in frost!", Monnam(mon));
			if (noeffect || resists_cold(mon)) {
			    if (!noeffect)
				shieldeff(mon->mx, mon->my);
			    if (!Blind) 
				pline_The("frost doesn't chill %s!", mon_nam(mon));
			    golemeffects(mon, AD_COLD, dmgbonus);
			    dmgbonus = 0;
			    noeffect = 0;
			}
			dmgbonus += destroy_mitem(mon, POTION_CLASS, AD_COLD);
			break;
		    case 2: /* Elec */
			if (!Blind) pline("%s is zapped!", Monnam(mon));
			dmgbonus += destroy_mitem(mon, WAND_CLASS, AD_ELEC);
			if (noeffect || resists_elec(mon)) {
			    if (!noeffect)
				shieldeff(mon->mx, mon->my);
			    if (!Blind)
				pline_The("zap doesn't shock %s!", mon_nam(mon));
			    golemeffects(mon, AD_ELEC, dmgbonus);
			    if (!noeffect)
				dmgbonus = 0;
			    else
				noeffect = 0;
			}
			/* only rings damage resistant players in destroy_item */
			dmgbonus += destroy_mitem(mon, RING_CLASS, AD_ELEC);
			break;
		    case 3: /* Acid */
			if (!Blind)
			    pline("%s is covered in acid!", Monnam(mon));
			if (noeffect || resists_acid(mon)) {
			    if (!Blind)
				pline_The("acid doesn't burn %s!", Monnam(mon));
			    dmgbonus = 0;
			    noeffect = 0;
			}
			break;
		}
		if (dmgbonus > 0)
		    tmp += dmgbonus;
	    } /* Techinuse Elemental Fist */		

	  } else if (touch_disintegrates(mdat) && !oresist_disintegration(obj) &&
	    mon->mhp>6 && !mon->mcan){
	    disint_obj = TRUE;
	    valid_weapon_attack = FALSE;
	    tmp = obj->owt;
	    weight_dmg(tmp);
	  } else {
	    if (obj->oartifact == ART_MAGICBANE) objenchant = 4;
	    else if (obj->oartifact) objenchant += 2;

#ifdef LIGHTSABERS
	    if (is_lightsaber(obj)) objenchant = 4;
#endif

	    if (is_poisonable(obj) && obj->opoisoned)
		ispoisoned = TRUE;
		/* 5lo: Cleaver will actually cleave through monsters.
		 * Code is from Nethack Fourk, with some slight modification */
                if (is_axe(obj) && !obj->axeinuse && !u.uswallow &&
                    obj->oartifact == ART_CLEAVER && rn2(5)) {
                    /* Axes also hit adjacent enemies */
                    coord posn1, posn2;
                    struct monst *ctarg; /* collateral target */
                    pline("You swing %s in a wide arc.", xname(obj));
                    obj->axeinuse = 1;
                    if (u.ux == mon->mx) {
                        posn1.x = mon->mx + 1;
                        posn2.x = mon->mx - 1;
                        posn1.y = mon->my;
                        posn2.y = mon->my;
                    } else if (u.uy == mon->my) {
                        posn1.x = mon->mx;
                        posn2.x = mon->mx;
                        posn1.y = mon->my + 1;
                        posn2.y = mon->my - 1;
                    } else {
                        posn1.x = u.ux;
                        posn1.y = mon->my;
                        posn2.x = mon->mx;
                        posn2.y = u.uy;
                    }
                    if (isok(posn1.x, posn1.y) &&
                        (ctarg = m_at(posn1.x, posn1.y)) &&
                        !ctarg->mtame && !ctarg->mpeaceful) {
                        hmon(ctarg, obj, thrown);
                    }
                    if (isok(posn2.x, posn2.y) &&
                        (ctarg = m_at(posn2.x, posn2.y)) &&
                        !ctarg->mtame && !ctarg->mpeaceful) {
                        hmon(ctarg, obj, thrown);
                    }
                    obj->axeinuse = 0;
                }

	    noeffect = objenchant < canhitmon && !ispoisoned;

	    Strcpy(saved_oname, cxname(obj));
	    if(obj->oclass == WEAPON_CLASS || is_weptool(obj) ||
#ifdef CONVICT
	       obj->oclass == GEM_CLASS || obj->oclass == BALL_CLASS || obj->oclass == CHAIN_CLASS) {
#else
	       obj->oclass == GEM_CLASS) {
#endif /* CONVICT */

		/* is it not a melee weapon? */
		/* KMH, balance patch -- new macros */
		if (/* if you strike with a bow... */
		    is_launcher(obj) ||
		    /* or strike with a missile in your hand... */
		    (!thrown && (is_missile(obj) || is_ammo(obj))) ||
		    /* or use a pole at short range and not mounted... */
		    (!thrown &&
#ifdef STEED
		     !u.usteed &&
#endif
		     is_pole(obj)) ||
#ifdef LIGHTSABERS
		    /* lightsaber that isn't lit ;) */
		    (is_lightsaber(obj) && !obj->lamplit) ||
#endif
		    /* or throw a missile without the proper bow... */
		    (thrown == 1 && is_ammo(obj) && 
		    	!ammo_and_launcher(obj, launcher)) || 
		    /* This case isn't actually needed so far since 
		     * you can only throw in two-weapon mode when both
		     * launchers take the same ammo
		     */
		    (thrown == 2 && is_ammo(obj) && 
		    	!ammo_and_launcher(obj, launcher))) {
		    /* then do only 1-2 points of damage */
                    not_melee_weapon = TRUE;
		    if (mdat == &mons[PM_SHADE] && obj->otyp != SILVER_ARROW)
			tmp = 0;
		    else
			tmp = rnd(2);

		    /* 5lo: Jedi know how to use an unlit lightsaber as a weapon */
		    if (is_lightsaber(obj) && !obj->lamplit && Role_if(PM_JEDI)) {
		    	tmp = d(1,4) + obj->spe + (P_SKILL(P_BARE_HANDED_COMBAT) - P_UNSKILLED);
			use_skill(P_BARE_HANDED_COMBAT, 1); // throw them a bone
		    }

		    if (!thrown && (obj == uwep || obj == uswapwep) && 
				obj->otyp == BOOMERANG && !rnl(4) == 4-1) {
			boolean more_than_1 = (obj->quan > 1L);

			pline("As you hit %s, %s%s %s breaks into splinters.",
			      mon_nam(mon), more_than_1 ? "one of " : "",
			      shk_your(yourbuf, obj), xname(obj));
			if (!more_than_1) {
			    if (obj == uwep)
				uwepgone();   /* set unweapon */
			    else
				setuswapwep((struct obj *)0, FALSE);
			}
			useup(obj);
			if (!more_than_1) obj = (struct obj *) 0;
			hittxt = TRUE;
			if (mdat != &mons[PM_SHADE])
			    tmp++;
		   }
		} else {
		    tmp = dmgval(obj, mon);
		    /* a minimal hit doesn't exercise proficiency */
		    valid_weapon_attack = (tmp > 0);
#if 0
		    if (!valid_weapon_attack || mon == u.ustuck || u.twoweap) {
#endif
		    if (!valid_weapon_attack || mon == u.ustuck) {
			;	/* no special bonuses */
		    } else if (mon->mflee && (Role_if(PM_ROGUE) || Role_if(PM_NINJA) ||
					obj->oartifact == ART_SPINESEEKER) && !Upolyd && !thrown) {
			You("strike %s from behind!", mon_nam(mon));
			tmp += rnd(u.ulevel);
			hittxt = TRUE;
		    } else if (dieroll == 2 && obj == uwep &&
			  !u.twoweap &&
			  obj->oclass == WEAPON_CLASS &&
			  (bimanual(obj) ||
			    (Role_if(PM_SAMURAI) && obj->otyp == KATANA && !uarms)) &&
			  ((wtype = uwep_skill_type()) != P_NONE &&
			    P_SKILL(wtype) >= P_SKILLED) &&
			  ((monwep = MON_WEP(mon)) != 0 &&
			   !is_flimsy(monwep) &&
			   !obj_resists(monwep,
				 50 + 15 * greatest_erosion(obj), 100))) {
			/*
			 * 2.5% chance of shattering defender's weapon when
			 * using a two-handed weapon; less if uwep is rusted.
			 * [dieroll == 2 is most successful non-beheading or
			 * -bisecting hit, in case of special artifact damage;
			 * the percentage chance is (1/20)*(50/100).]
			 * WAC.	Bimanual, or samurai and Katana without shield.
			 *	No twoweapon.
			 */
			setmnotwielded(mon,monwep);
			MON_NOWEP(mon);
			mon->weapon_check = NEED_WEAPON;
			pline("%s %s %s from the force of your blow!",
			      s_suffix(Monnam(mon)), xname(monwep),
			      otense(monwep, "shatter"));
			m_useup(mon, monwep);
			/* If someone just shattered MY weapon, I'd flee! */
			if (rn2(4)) {
			    monflee(mon, d(2,3), TRUE, TRUE);
			}
			hittxt = TRUE;
		    }

#ifdef JEDI
		    else if (obj == uwep &&
			  (Role_if(PM_JEDI) && is_lightsaber(obj)) &&
			  ((wtype = uwep_skill_type()) != P_NONE &&
			    P_SKILL(wtype) >= P_SKILLED) &&
			  ((monwep = MON_WEP(mon)) != 0 &&
			   !is_lightsaber(monwep) && // no cutting other lightsabers :)
			   !monwep->oartifact && // no cutting artifacts either
			   !obj_resists(monwep,
				 50 + 15 * greatest_erosion(obj), 100))) {
			setmnotwielded(mon,monwep);
			MON_NOWEP(mon);
			mon->weapon_check = NEED_WEAPON;
			Your("%s cuts %s %s in half!",
			      xname(obj),
			      s_suffix(mon_nam(mon)), xname(monwep));
			m_useup(mon, monwep);
			/* If someone just shattered MY weapon, I'd flee! */
			if (rn2(4)) {
			    monflee(mon, d(2,3), TRUE, TRUE);
			}
			hittxt = TRUE;
		    }
#endif
		    if (obj->oartifact &&
			artifact_hit(&youmonst, mon, obj, &tmp, dieroll)) {
			if(mon->mhp <= 0) /* artifact killed monster */
			    return FALSE;
			if (tmp == 0) return TRUE;
			hittxt = TRUE;
		    }
		    if (objects[obj->otyp].oc_material == SILVER
				&& hates_silver(mdat)) {
			silvermsg = TRUE; silverobj = TRUE;
		    }
#ifdef STEED
		    if (u.usteed && !thrown && tmp > 0 &&
			    weapon_type(obj) == P_LANCE && mon != u.ustuck) {
			jousting = joust(mon, obj);
			/* exercise skill even for minimal damage hits */
			if (jousting) valid_weapon_attack = TRUE;
		    }
#endif
		    if (thrown && (is_ammo(obj) || is_missile(obj))) {
#ifdef P_SPOON
			if (obj->oartifact == ART_HOUCHOU) {
			    pline("There is a bright flash as it hits %s.",
				the(mon_nam(mon)));
			    tmp = dmgval(obj, mon);
			}
#endif /* P_SPOON */
			if (ammo_and_launcher(obj, launcher)) {
			    if (launcher->oartifact)
				tmp += spec_dbon(launcher, mon, tmp);
			    /* Elves and Samurai do extra damage using
			     * their bows&arrows; they're highly trained.
			     * WAC Only elves get dmg bonus from flurry. Change?
			     */
			    if (Role_if(PM_SAMURAI) &&
				    obj->otyp == YA && launcher->otyp == YUMI)
				tmp++;
			    /*else */
				if (Race_if(PM_ELF)) {
				if (obj->otyp == ELVEN_ARROW &&
					launcher->otyp == ELVEN_BOW) {
				tmp++;
				    /* WAC Extra damage if in special ability*/
				    if (tech_inuse(T_FLURRY)) tmp += 2;
				} else if (objects[obj->otyp].oc_skill == P_BOW
					&& tech_inuse(T_FLURRY)) {
				tmp++;
				}
			    } /*else */
				if (Race_if(PM_DROW)) {
				if (obj->otyp == DARK_ELVEN_ARROW &&
					launcher->otyp == DARK_ELVEN_BOW) {
				    tmp += 2;
				    /* WAC Mucho damage if in special ability*/
				    if (tech_inuse(T_FLURRY)) tmp *= 2;
				} else if (objects[obj->otyp].oc_skill == P_BOW
					&& tech_inuse(T_FLURRY)) {
				    tmp++;
				}
			    }
			}
		    }
		    /* MRKR: Hitting with a lit torch does extra */
		    /*       fire damage, but uses up the torch  */
		    /*       more quickly.                       */

		    if(obj->otyp == TORCH && obj->lamplit
		       && !resists_fire(mon)) {

		      burnmsg = TRUE;

		      tmp++;
		      if (resists_cold(mon)) tmp += rnd(3);

		      /* Additional damage due to burning armor */
		      /* & equipment is delayed to below, after */
		      /* the hit messages are printed. */
		    }

		    /* 5lo: Electric Swords may "spark" to life */

		    if(obj->otyp == ELECTRIC_SWORD && !obj->oartifact && 
		       !resists_elec(mon) && !rn2(10)) {

		      shockmsg = TRUE;

		      tmp += rnd(3);

		    }

		}
	    } else if(obj->oclass == POTION_CLASS) {
		if (!u.twoweap || obj == uwep) {
		if (obj->quan > 1L)
		    obj = splitobj(obj, 1L);
		else
		    setuwep((struct obj *)0, FALSE);
		} else if (u.twoweap && obj == uswapwep) {
		    if (obj->quan > 1L)
			setworn(splitobj(obj, 1L), W_SWAPWEP);
		    else
			setuswapwep((struct obj *)0, FALSE);
		}
		freeinv(obj);
		potionhit(mon, obj, TRUE);
		obj = (struct obj *)0; /* The object has been free'd */
		if (mon->mhp <= 0) return FALSE;	/* killed */
		hittxt = TRUE;
		/* in case potion effect causes transformation */
		mdat = mon->data;
		tmp = (mdat == &mons[PM_SHADE]) ? 0 : 1;
	    } else {
		if (mdat == &mons[PM_SHADE] && !shade_aware(obj)) {
		    tmp = 0;
		    Strcpy(unconventional, cxname(obj));
		} else {
		switch(obj->otyp) {
		    case BOULDER:		/* 1d20 */
		    case HEAVY_IRON_BALL:	/* 1d25 */
		    case IRON_CHAIN:		/* 1d4+1 */
			tmp = dmgval(obj, mon);
			break;
		    case MIRROR:
			if (breaktest(obj)) {
			    You("break %s mirror.  That's bad luck!",
				shk_your(yourbuf, obj));
			    change_luck(-2);
			    useup(obj);
			    obj = (struct obj *) 0;
			    unarmed = FALSE;	/* avoid obj==0 confusion */
			    get_dmg_bonus = FALSE;
			    hittxt = TRUE;
			}
			tmp = 1;
			break;
#ifdef TOURIST
		    case EXPENSIVE_CAMERA:
			You("succeed in destroying %s camera.  Congratulations!",
			    shk_your(yourbuf, obj));
			useup(obj);
			return(TRUE);
			/*NOTREACHED*/
			break;
#endif
		    case CORPSE:		/* fixed by polder@cs.vu.nl */
			if (touch_petrifies(&mons[obj->corpsenm])) {
			    static const char withwhat[] = "corpse";
			    tmp = 1;
			    hittxt = TRUE;
			    You("hit %s with %s %s.", mon_nam(mon),
				obj->dknown ? the(mons[obj->corpsenm].mname) :
				an(mons[obj->corpsenm].mname),
				(obj->quan > 1) ? makeplural(withwhat) : withwhat);
			    if (!munstone(mon, TRUE))
				minstapetrify(mon, TRUE);
			    if (resists_ston(mon)) break;
			    /* note: hp may be <= 0 even if munstoned==TRUE */
			    return (boolean) (mon->mhp > 0);
#if 0
			} else if (touch_petrifies(mdat)) {
			    /* maybe turn the corpse into a statue? */
#endif
			}
			tmp = (obj->corpsenm >= LOW_PM ?
					mons[obj->corpsenm].msize : 0) + 1;
			break;
		    case EGG:
		      {
#define useup_eggs(o)	{ if (thrown) obfree(o,(struct obj *)0); \
			  else useupall(o); \
			  o = (struct obj *)0; }	/* now gone */
			long cnt = obj->quan;

			tmp = 1;		/* nominal physical damage */
			get_dmg_bonus = FALSE;
			hittxt = TRUE;		/* message always given */
			/* egg is always either used up or transformed, so next
			   hand-to-hand attack should yield a "bashing" mesg */
			if (obj == uwep) unweapon = TRUE;
			if (obj->spe && obj->corpsenm >= LOW_PM) {
			    if (obj->quan < 5)
				change_luck((schar) -(obj->quan));
			    else
				change_luck(-5);
			}

			if (touch_petrifies(&mons[obj->corpsenm])) {
			    /*learn_egg_type(obj->corpsenm);*/
			    pline("Splat! You hit %s with %s %s egg%s!",
				mon_nam(mon),
				obj->known ? "the" : cnt > 1L ? "some" : "a",
				obj->known ? mons[obj->corpsenm].mname : "petrifying",
				plur(cnt));
#if 0
			    obj->known = 1;	/* (not much point...) */
#endif
			    useup_eggs(obj);
			    if (!munstone(mon, TRUE))
				minstapetrify(mon, TRUE);
			    if (resists_ston(mon)) break;
			    return (boolean) (mon->mhp > 0);
			} else {	/* ordinary egg(s) */
			    const char *eggp =
				     (obj->corpsenm != NON_PM && obj->known) ?
					      the(mons[obj->corpsenm].mname) :
					      (cnt > 1L) ? "some" : "an";
			    You("hit %s with %s egg%s.",
				mon_nam(mon), eggp, plur(cnt));
			    if (touch_petrifies(mdat) && !stale_egg(obj)) {
				pline_The("egg%s %s alive any more...",
				      plur(cnt),
				      (cnt == 1L) ? "isn't" : "aren't");
				if (obj->timed) obj_stop_timers(obj);
				obj->otyp = ROCK;
				obj->oclass = GEM_CLASS;
				obj->oartifact = 0;
				obj->spe = 0;
				obj->known = obj->dknown = obj->bknown = 0;
				obj->owt = weight(obj);
				if (thrown) place_object(obj, mon->mx, mon->my);
			    } else {
				pline("Splat!");
				useup_eggs(obj);
				exercise(A_WIS, FALSE);
			    }
			}
			break;
#undef useup_eggs
		      }
		    case CLOVE_OF_GARLIC:	/* no effect against demons */
			if (is_undead(mdat)) {
			    monflee(mon, d(2, 4), FALSE, TRUE);
			}
			tmp = 1;
			break;
		    case CREAM_PIE:
		    case BLINDING_VENOM:
			mon->msleeping = 0;
			if (can_blnd(&youmonst, mon, (uchar)
				    (obj->otyp == BLINDING_VENOM
				     ? AT_SPIT : AT_WEAP), obj)) {
			    if (Blind) {
				pline(obj->otyp == CREAM_PIE ?
				      "Splat!" : "Splash!");
			    } else if (obj->otyp == BLINDING_VENOM) {
				pline_The("venom blinds %s%s!", mon_nam(mon),
					  mon->mcansee ? "" : " further");
			    } else {
				char *whom = mon_nam(mon);
				char *what = The(xname(obj));
				if (!thrown && obj->quan > 1)
				    what = An(singular(obj, xname));
				/* note: s_suffix returns a modifiable buffer */
				if (haseyes(mdat)
				    && mdat != &mons[PM_FLOATING_EYE])
				    whom = strcat(strcat(s_suffix(whom), " "),
						  mbodypart(mon, FACE));
				pline("%s %s over %s!",
				      what, vtense(what, "splash"), whom);
			    }
			    setmangry(mon);
			    mon->mcansee = 0;
			    tmp = rn1(25, 21);
			    if(((int) mon->mblinded + tmp) > 127)
				mon->mblinded = 127;
			    else mon->mblinded += tmp;
			} else {
			    pline(obj->otyp==CREAM_PIE ? "Splat!" : "Splash!");
			    setmangry(mon);
			}
			{
				boolean more_than_1 = (obj->quan > 1L);
				if (thrown) obfree(obj, (struct obj *)0);
				else useup(obj);
				if(!more_than_1 || thrown) obj = (struct obj *)0; /* obj has been free'd */
			}
			hittxt = TRUE;
			get_dmg_bonus = FALSE;
			tmp = 0;
			break;
		    case ACID_VENOM: /* thrown (or spit) */
			if (resists_acid(mon)) {
				Your("venom hits %s harmlessly.",
					mon_nam(mon));
				tmp = 0;
			} else {
				Your("venom burns %s!", mon_nam(mon));
				tmp = dmgval(obj, mon);
			}
			{
				boolean more_than_1 = (obj->quan > 1L);
				if (thrown) obfree(obj, (struct obj *)0);
				else useup(obj);
				if(!more_than_1 || thrown) obj = (struct obj *)0; /* obj has been free'd */
			}
			hittxt = TRUE;
			get_dmg_bonus = FALSE;
			break;
		    case WATER_VENOM: /* thrown (or spit) */
			tmp = 0;
			if(does_rust(mdat)){
			    tmp = d(1,6);
			} 
			needrustmsg = TRUE;
			break;
		    default:
			/* non-weapons can damage because of their weight */
			/* (but not too much) */
			tmp = obj->owt/100;
			if(tmp < 1) tmp = 1;
			else tmp = rnd(tmp);
			if(tmp > 6) tmp = 6;
			/*
			 * Things like silver wands can arrive here so
			 * so we need another silver check.
			 */
			if (objects[obj->otyp].oc_material == SILVER
						&& hates_silver(mdat)) {
				tmp += rnd(20);
				silvermsg = TRUE; silverobj = TRUE;
			}
		    }
		}
	    }
	}

	/****** NOTE: perhaps obj is undefined!! (if !thrown && BOOMERANG)
	 *      *OR* if attacking bare-handed!! */

	if (get_dmg_bonus && tmp > 0) {
		tmp += u.udaminc;
		/* If you throw using a propellor, you don't get a strength
		 * bonus but you do get an increase-damage bonus.
		 * 5lo: Give slings stregnth bonus.  Credit to Percival Tiglao.
		 */
		if(!thrown || !obj || !uwep || !ammo_and_launcher(obj, launcher) || uslinging())
		    tmp += dbon();
	}

	/*
	 * Ki special ability, see cmd.c in function special_ability.
	 * In this case, we do twice damage! Wow!
	 *
	 * Berserk special ability only does +4 damage. - SW
	 * 5lo: Berserk time gets extended with every active hit
	 */
	/*Lycanthrope claws do +level bare hands dmg
                (multi-hit, stun/freeze)..- WAC*/

	if (tech_inuse(T_KIII)) tmp *= 2;
	if (tech_inuse(T_BERSERK)) {
		tmp += 4;
		extend_tech_time(T_BERSERK, rnd(4));
	}
	if (tech_inuse(T_SOULEATER)) {
		tmp += d((u.ulevel / 4), 8);
		/* Unholy damage, not ignored from fire resistance */
		pline("Dark flames envelop %s!", mon_nam(mon));
		hittxt = TRUE;
	}
	if (tech_inuse(T_EVISCERATE)) {
		tmp += rnd((int) (u.ulevel/2 + 1)) + (u.ulevel/2); /* [max] was only + u.ulevel */
                You("slash %s!", mon_nam(mon));
		hittxt = TRUE;
	}

	if (valid_weapon_attack) {
	    struct obj *wep;

	    /* to be valid a projectile must have had the correct projector */
	    wep = PROJECTILE(obj) ? launcher : obj;
	    tmp += weapon_dam_bonus(wep);
	    /* [this assumes that `!thrown' implies wielded...] */
	    wtype = weapon_type(wep);
	    if (thrown || !u.twoweap || !rn2(2)) use_skill(wtype, 1);
	    else if (u.twoweap) use_skill(P_TWO_WEAPON_COMBAT,1);
	}

	if (uslinging() && ammo_and_launcher(obj, uwep)
	   		&& (P_SKILL(wtype) >= P_EXPERT ||
			uwep->oartifact == ART_DAVID_S_SLING)
			&& mdat->mlet == S_GIANT)
	{
		giantkill = TRUE;
	}

	if (ispoisoned || (obj && obj->oartifact == ART_DIRGE)) {
	    int nopoison = (10 - (obj->owt/10));            
	    if(nopoison < 2) nopoison = 2;
	    if Role_if(PM_SAMURAI) {
		You("dishonorably use a poisoned weapon!");
		adjalign(-sgn(u.ualign.type));
		u.ualign.sins++;
	    } else if ((u.ualign.type == A_LAWFUL) && (u.ualign.record > -10)) {
		You_feel("like an evil coward for using a poisoned weapon.");
		adjalign(Role_if(PM_KNIGHT) ? -10 : -5);
	    }
	    if (obj && !rn2(nopoison) && obj->oartifact != ART_DIRGE) {
		obj->opoisoned = FALSE;
		Your("%s %s no longer poisoned.", xname(obj),
		     otense(obj, "are"));
	    }
	    if (disint_obj) ;
	    else if (resists_poison(mon))
		needpoismsg = TRUE;
	    else if (rn2(100))
		tmp += rnd(6);
	    else poiskilled = TRUE;
	}
	  
	if (tmp < 1) {
	    /* make sure that negative damage adjustment can't result
	       in inadvertently boosting the victim's hit points */
	    tmp = 0;
	    if (mdat == &mons[PM_SHADE]) {
		if (!hittxt) {
		    const char *what = unconventional[0] ? unconventional : "attack";
		    Your("%s %s harmlessly through %s.",
		    	what, vtense(what, "pass"),
			mon_nam(mon));
		    hittxt = TRUE;
		}
	    } else {
		if (get_dmg_bonus) tmp = 1;
	    }
	}

	if(disint_obj && obj){ /* all the hit msgs, no object destruction */
	    if(obj->oclass == POTION_CLASS || obj->oclass == VENOM_CLASS ||
	       obj->otyp == EGG || obj->otyp == CREAM_PIE){
		if (cansee(mon->mx, mon->my))
		    pline_The("%s %s in a %s of green light!", 
		      xname(obj), vtense(xname(obj),"vanish"), 
		      (obj->oclass == VENOM_CLASS)?"twinkle":"flash");
		else
		    pline("Vip!");
		hittxt=TRUE;
	    } else if (u.usteed && !thrown && tmp > 0 &&
	      weapon_type(obj) == P_LANCE && mon != u.ustuck && joust(mon,obj)) {
		You("joust %s%s",
		  mon_nam(mon), canseemon(mon) ? exclam(tmp) : ".");
		Your("%s vanishes on impact!", xname(obj));
		hittxt = TRUE;
	   }
	} else

#ifdef STEED
	if (jousting) {
               /*
                * jousting damage is a bit too strong in the early game
                * ...another change necessitated by making the Knight 100%
                * to ride his starting pony.
                *
                * While it's appropriate to consider that a fully-armored Knight
                * on a horse would be able to completely mop up the Mines,
                * game balance says there should be at least SOME effort
                * involved in getting to the luckstone.... 
                */
                joustdmg = 5 + u.ulevel/3;      /* 2d5 -> 2d15 */
           tmp += d(2, (obj == uwep) ? joustdmg : 2);  /* [was in dmgval()] */

	    You("joust %s%s",
			 mon_nam(mon), canseemon(mon) ? exclam(tmp) : ".");
	    if (jousting < 0) {
		Your("%s shatters on impact!", xname(obj));
		/* (must be either primary or secondary weapon to get here) */
		u.twoweap = FALSE;      /* untwoweapon() is too verbose here */
		if (obj == uwep) uwepgone();            /* set unweapon */
		/* minor side-effect: broken lance won't split puddings */
		useup(obj);
		obj = 0;
	    }
	    /* avoid migrating a dead monster */
	    if (mon->mhp > tmp) {
		mhurtle(mon, u.dx, u.dy, 1);
		mdat = mon->data; /* in case of a polymorph trap */
		if (DEADMONSTER(mon)) already_killed = TRUE;
	    }
	    hittxt = TRUE;
	} else
#endif

	/* VERY small chance of stunning opponent if unarmed. */
	if (unarmed && tmp > 1 && !thrown && !obj && !Upolyd) {
	    if (rnd(100) < P_SKILL(P_BARE_HANDED_COMBAT) &&
			!bigmonst(mdat) && !thick_skinned(mdat)) {
		if (canspotmon(mon))
		    pline("%s %s from your powerful strike!", Monnam(mon),
			  makeplural(stagger(mon->data, "stagger")));
		/* avoid migrating a dead monster */
		if (mon->mhp > tmp) {
		    mhurtle(mon, u.dx, u.dy, 1);
		    mdat = mon->data; /* in case of a polymorph trap */
		    if (DEADMONSTER(mon)) already_killed = TRUE;
		}
		hittxt = TRUE;
	    }
	}

	if (tmp && noeffect) {
	    if (silvermsg)
		tmp = 8;
	    else {
		Your("attack doesn't seem to harm %s.", mon_nam(mon));
		hittxt = TRUE;
		tmp = 0;
	    }
	}

        /* WAC Added instant kill from wooden stakes vs vampire */
        /* based off Poison Code */
        /* fixed stupid mistake - check that obj exists before comparing...*/
        if (obj && (obj->otyp == WOODEN_STAKE) && is_vampire(mdat)) {
            if (Role_if(PM_UNDEAD_SLAYER) 
              || (P_SKILL(weapon_type(obj)) >= P_EXPERT)
              || obj->oartifact == ART_STAKE_OF_VAN_HELSING) {
                if (!rn2(10)) {
                    You("plunge your stake into the heart of %s.",
                        mon_nam(mon));
                    vapekilled = TRUE;
                } else {
                    You("drive your stake into %s.", mon_nam(mon));
                    tmp += rnd(6) + 2;
                    hittxt = TRUE;
                }
            } else {
                You("drive your stake into %s.", mon_nam(mon));
                tmp += rnd(6);
                hittxt = TRUE;
            }
        }

	/* Special monk strikes */
	if (Role_if(PM_MONK) && !Upolyd && !thrown && no_obj &&
		(!uarm || (uarm && uarm->otyp >= ROBE &&
		 uarm->otyp <= ROBE_OF_WEAKNESS)) && !uarms &&
		 distu(mon->mx, mon->my) <= 2) {
	    /* just so we don't need another variable ... */
	    canhitmon = rnd(100);
	    if (canhitmon < u.ulevel / 8 && !thick_skinned(mdat)) {
		if (canspotmon(mon))
		    You("strike %s extremely hard!", mon_nam(mon));
		tmp *= 2;
		hittxt = TRUE;
	    } else if (canhitmon < u.ulevel / 4 && !thick_skinned(mdat)) {
		if (canspotmon(mon))
		    You("strike %s very hard!", mon_nam(mon));
		tmp += tmp / 2;
		hittxt = TRUE;
	    } else if (canhitmon < u.ulevel / 2 && !bigmonst(mon->data) &&
		    !thick_skinned(mdat)) {
		if (canspotmon(mon))
		    pline("%s %s from your powerful strike!", Monnam(mon),
			  makeplural(stagger(mon->data, "stagger")));
		/* avoid migrating a dead monster */
		if (mon->mhp > tmp) {
		    mhurtle(mon, u.dx, u.dy, 1);
		    mdat = mon->data; /* in case of a polymorph trap */
		    if (DEADMONSTER(mon)) already_killed = TRUE;
		}
		hittxt = TRUE;
	    }
	}

	if (!already_killed) mon->mhp -= tmp;
	/* adjustments might have made tmp become less than what
	   a level draining artifact has already done to max HP */
	if (mon->mhp > mon->mhpmax) mon->mhp = mon->mhpmax;
	if (mon->mhp < 1)
		destroyed = TRUE;
	/* fixed bug with hitting tame monster with non-magic weapon */        
	if (mon->mtame && (!mon->mflee || mon->mfleetim) && tmp > 0) {

		abuse_dog(mon);
		monflee(mon, 10 * rnd(tmp), FALSE, FALSE);
	}
	if((mdat == &mons[PM_BLACK_PUDDING] || mdat == &mons[PM_BROWN_PUDDING])
		   && obj /* && obj == uwep -- !thrown and obj == weapon */
		   && !thrown
		   && objects[obj->otyp].oc_material == IRON
		   && mon->mhp > 1 && !thrown && !mon->mcan && !rn2(10) /*slowing down pudding farming --Amy*/
		   /* && !destroyed  -- guaranteed by mhp > 1 */ ) {
		if (clone_mon(mon, 0, 0)) {
			pline("%s divides as you hit it!", Monnam(mon));
			hittxt = TRUE;
		}
	}

	if (!hittxt &&			/*( thrown => obj exists )*/
	  (!destroyed || (thrown && m_shot.n > 1 && m_shot.o == obj->otyp))) {
		if (thrown) hit(mshot_xname(obj), mon, exclam(tmp));
		else if (!flags.verbose) You("hit it.");
                else You("%s %s%s", !obj ? barehitmsg(&youmonst) :
                          not_melee_weapon ? "hit" : weaphitmsg(obj,TRUE),
			 mon_nam(mon), canseemon(mon) ? exclam(tmp) : ".");
	}

	if (burnmsg) {
	  /* A chance of setting the monster's */
	  /* armour + equipment on fire */
	  /* (this does not do any extra damage) */

	  if (!Blind) {
	    Your("%s %s %s.", xname(obj),
		 (mon->data == &mons[PM_WATER_ELEMENTAL]) ?
		 "vaporizes part of" : "burns", mon_nam(mon));
	  }

	  if (!rn2(2) && burnarmor(mon)) {
	    if (!rn2(33)) 
	      (void)destroy_mitem(mon, POTION_CLASS, AD_FIRE);
	    if (!rn2(33)) 
	      (void)destroy_mitem(mon, SCROLL_CLASS, AD_FIRE);
	    if (!rn2(50)) 
	      (void)destroy_mitem(mon, SPBOOK_CLASS, AD_FIRE);
	  }

	  if (mon->data == &mons[PM_WATER_ELEMENTAL]) {
	    if (!Blind) {
	      Your("%s goes out.", xname(obj));
	    }
	    end_burn(obj, TRUE);
	  }
	  else {
	    /* use up the torch more quickly */	    
	    burn_faster(obj, 1);
	  }
	}

	if (shockmsg) {
	  /* A very small chance of destroying the monster's
	   * wands and rings */

	  if (!Blind) {
	    Your("%s buzzes %s.", xname(obj), mon_nam(mon));

	    if (!rn2(20)) 
	      (void)destroy_mitem(mon, RING_CLASS, AD_ELEC);
	    if (!rn2(20)) 
	      (void)destroy_mitem(mon, WAND_CLASS, AD_ELEC);
	  }
	}

	if (disint_obj && obj) {
	    if(!hittxt){
		if(cansee(mon->mx,mon->my)){
		    pline_The("%s %s!", mshot_xname(obj),
		      (obj->oartifact)?"dissolves":"disintegrates");
		} else {
		    pline("Vip!%s", 
		       (!thrown) ? "  Your weapon vanishes from your grip!":"");
		}
	    }
	    if (!thrown) {
		u.twoweap = FALSE;	/* untwoweapon() is too verbose here */
		if (obj == uwep) uwepgone();		/* set unweapon */
		useupall(obj);
		obj = 0;
	    } else {
		/*obfree(obj, (struct obj *) 0 ); handled: elsewhere */
	    }
	} 

	if (silvermsg) {
		const char *fmt;
		char *whom = mon_nam(mon);
		char silverobjbuf[BUFSZ];

		if (canspotmon(mon)) {
		    if (barehand_silver_rings == 1)
			fmt = "Your silver ring sears %s!";
		    else if (barehand_silver_rings == 2)
			fmt = "Your silver rings sear %s!";
		    else if (silverobj && saved_oname[0]) {
		    	Sprintf(silverobjbuf, "Your %s%s %s %%s!",
		    		strstri(saved_oname, "silver") ?
					"" : "silver ",
				saved_oname, vtense(saved_oname, "sear"));
		    	fmt = silverobjbuf;
		    } else
			fmt = "The silver sears %s!";
		} else {
		    *whom = highc(*whom);	/* "it" -> "It" */
		    fmt = "%s is seared!";
		}
		/* note: s_suffix returns a modifiable buffer */
		if (!noncorporeal(mdat))
		    whom = strcat(s_suffix(whom), " flesh");
		pline(fmt, whom);
	}

	if(giantkill)
	{
		You("smite %s on the forehead!", mon_nam(mon));
		if(!already_killed) {
			xkilled(mon, 1);
		}
	}
	if (needrustmsg){
	    hurtmarmor(mon,AD_RUST);
	    if(mdat == &mons[PM_GREMLIN]){
		(void)split_mon(mon, (struct monst *)0);
	    } else if ( mdat == &mons[PM_IRON_GOLEM] ) {
		if (canseemon(mon))pline("%s rusts.", Monnam(mon));
		setmangry(mon);
		abuse_dog(mon);
	    } 
	}
	if (needpoismsg)
		pline_The("poison doesn't seem to affect %s.", mon_nam(mon));
	if (poiskilled) {
		pline_The("poison was deadly...");
		if (!already_killed) xkilled(mon, 0);
		return FALSE;
/* For vamps */
        } else if (vapekilled) {
                if (cansee(mon->mx, mon->my))
                    pline("%s%ss body vaporizes!", Monnam(mon),
                            canseemon(mon) ? "'" : "");                
                if (!already_killed) xkilled(mon, 2);
		return FALSE;
	} else if (destroyed) {
		if (!already_killed)
		    killed(mon);	/* takes care of most messages */
	} else if(u.umconf && !thrown) {
		nohandglow(mon);
		if (!mon->mconf && !resist(mon, SPBOOK_CLASS, 0, NOTELL)) {
			mon->mconf = 1;
			if (!mon->mstun && mon->mcanmove && !mon->msleeping &&
				canseemon(mon))
			    pline("%s appears confused.", Monnam(mon));
		}
	} if (!destroyed) print_mon_wounded(mon, saved_mhp);

#ifdef SHOW_DMG
	if (!destroyed) showdmg(tmp);
#endif
	return((boolean)(destroyed ? FALSE : TRUE));
}


STATIC_OVL boolean
shade_aware(obj)
struct obj *obj;
{
	if (!obj) return FALSE;
	/*
	 * The things in this list either
	 * 1) affect shades.
	 *  OR
	 * 2) are dealt with properly by other routines
	 *    when it comes to shades.
	 */
	if (obj->otyp == BOULDER || obj->otyp == HEAVY_IRON_BALL
	    || obj->otyp == IRON_CHAIN		/* dmgval handles those first three */
	    || obj->otyp == MIRROR		/* silver in the reflective surface */
	    || obj->otyp == CLOVE_OF_GARLIC	/* causes shades to flee */
	    || objects[obj->otyp].oc_material == SILVER)
		return TRUE;
	return FALSE;
}

/* check whether slippery clothing protects from hug or wrap attack */
/* [currently assumes that you are the attacker] */
STATIC_OVL boolean
m_slips_free(mdef, mattk)
struct monst *mdef;
struct attack *mattk;
{
	struct obj *obj;

	if (mattk->adtyp == AD_DRIN) {
	    /* intelligence drain attacks the head */
	    obj = which_armor(mdef, W_ARMH);
	} else {
	    /* grabbing attacks the body */
	    obj = which_armor(mdef, W_ARMC);		/* cloak */
	    if (!obj) obj = which_armor(mdef, W_ARM);	/* suit */
#ifdef TOURIST
	    if (!obj) obj = which_armor(mdef, W_ARMU);	/* shirt */
#endif
	}

	/* if your cloak/armor is greased, monster slips off; this
	   protection might fail (33% chance) when the armor is cursed */
	if (obj && (obj->greased || obj->otyp == OILSKIN_CLOAK) &&
		(!obj->cursed || rn2(3))) {
	    You("%s %s %s %s!",
		mattk->adtyp == AD_WRAP ?
			"slip off of" : "grab, but cannot hold onto",
		s_suffix(mon_nam(mdef)),
		obj->greased ? "greased" : "slippery",
		/* avoid "slippery slippery cloak"
		   for undiscovered oilskin cloak */
		(obj->greased || objects[obj->otyp].oc_name_known) ?
			xname(obj) : cloak_simple_name(obj));

	    if (obj->greased && !rn2(2)) {
		pline_The("grease wears off.");
		obj->greased = 0;
	    }
	    return TRUE;
	}
	return FALSE;
}

/* used when hitting a monster with a lance while mounted */
STATIC_OVL int	/* 1: joust hit; 0: ordinary hit; -1: joust but break lance */
joust(mon, obj)
struct monst *mon;	/* target */
struct obj *obj;	/* weapon */
{
    int skill_rating, joust_dieroll;

    if (Fumbling || Stunned) return 0;
    /* sanity check; lance must be wielded in order to joust */
    if (obj != uwep && (obj != uswapwep || !u.twoweap)) return 0;

    /* if using two weapons, use worse of lance and two-weapon skills */
    skill_rating = P_SKILL(weapon_type(obj));	/* lance skill */
    if (u.twoweap && P_SKILL(P_TWO_WEAPON_COMBAT) < skill_rating)
	skill_rating = P_SKILL(P_TWO_WEAPON_COMBAT);
    if (skill_rating == P_ISRESTRICTED) skill_rating = P_UNSKILLED; /* 0=>1 */

    /* odds to joust are expert:80%, skilled:60%, basic:40%, unskilled:20% */
    if ((joust_dieroll = rn2(5)) < skill_rating) {
	if (joust_dieroll == 0 && rnl(50) == (50-1) &&
		!unsolid(mon->data) && !obj_resists(obj, 0, 100))
	    return -1;	/* hit that breaks lance */
	return 1;	/* successful joust */
    }
    return 0;	/* no joust bonus; revert to ordinary attack */
}

/*
 * Send in a demon pet for the hero.  Exercise wisdom.
 *
 * This function used to be inline to damageum(), but the Metrowerks compiler
 * (DR4 and DR4.5) screws up with an internal error 5 "Expression Too Complex."
 * Pulling it out makes it work.
 */
STATIC_OVL void
demonpet()
{
	int i;
	struct permonst *pm;
	struct monst *dtmp;

	pline("Some hell-p has arrived!");
	i = !rn2(6) ? ndemon(u.ualign.type) : NON_PM;
	pm = i != NON_PM ? &mons[i] : youmonst.data;
	if ((dtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS)) != 0)
	    (void)tamedog(dtmp, (struct obj *)0);
	exercise(A_WIS, TRUE);
}

/*
 * Player uses theft attack against monster.
 *
 * If the target is wearing body armor, take all of its possesions;
 * otherwise, take one object.  [Is this really the behavior we want?]
 *
 * This routine implicitly assumes that there is no way to be able to
 * resist petfication (ie, be polymorphed into a xorn or golem) at the
 * same time as being able to steal (poly'd into nymph or succubus).
 * If that ever changes, the check for touching a cockatrice corpse
 * will need to be smarter about whether to break out of the theft loop.
 */
STATIC_OVL void
steal_it(mdef, mattk)
struct monst *mdef;
struct attack *mattk;
{
	struct obj *otmp, *stealoid, **minvent_ptr;
	long unwornmask;

	if (!mdef->minvent) return;		/* nothing to take */

	/* look for worn body armor */
	stealoid = (struct obj *)0;
	if (could_seduce(&youmonst, mdef, mattk)) {
	    /* find armor, and move it to end of inventory in the process */
	    minvent_ptr = &mdef->minvent;
	    while ((otmp = *minvent_ptr) != 0)
		if (otmp->owornmask & W_ARM) {
		    if (stealoid) panic("steal_it: multiple worn suits");
		    *minvent_ptr = otmp->nobj;	/* take armor out of minvent */
		    stealoid = otmp;
		    stealoid->nobj = (struct obj *)0;
		} else {
		    minvent_ptr = &otmp->nobj;
		}
	    *minvent_ptr = stealoid;	/* put armor back into minvent */
	}

	if (stealoid) {		/* we will be taking everything */
	    if (gender(mdef) == (int) u.mfemale &&
			youmonst.data->mlet == S_NYMPH)
		You("charm %s.  She gladly hands over her possessions.",
		    mon_nam(mdef));
	    else
		You("seduce %s and %s starts to take off %s clothes.",
		    mon_nam(mdef), mhe(mdef), mhis(mdef));
	}

	while ((otmp = mdef->minvent) != 0) {
	    if (!Upolyd) break;		/* no longer have ability to steal */
	    /* take the object away from the monster */
	    obj_extract_self(otmp);
	    if ((unwornmask = otmp->owornmask) != 0L) {
		mdef->misc_worn_check &= ~unwornmask;
		if (otmp->owornmask & W_WEP) {
		    setmnotwielded(mdef,otmp);
		    MON_NOWEP(mdef);
		}
		otmp->owornmask = 0L;
		update_mon_intrinsics(mdef, otmp, FALSE, FALSE);

		if (otmp == stealoid)	/* special message for final item */
		    pline("%s finishes taking off %s suit.",
			  Monnam(mdef), mhis(mdef));
	    }
	    /* give the object to the character */
	    otmp = hold_another_object(otmp, "You snatched but dropped %s.",
				       doname(otmp), "You steal: ");
	    if (otmp->where != OBJ_INVENT) continue;
	    if (otmp->otyp == CORPSE &&
		    touch_petrifies(&mons[otmp->corpsenm]) && !uarmg) {
		char kbuf[BUFSZ];

		Sprintf(kbuf, "stolen %s corpse", mons[otmp->corpsenm].mname);
		instapetrify(kbuf);
		break;		/* stop the theft even if hero survives */
	    }
	    /* more take-away handling, after theft message */
	    if (unwornmask & W_WEP) {		/* stole wielded weapon */
		possibly_unwield(mdef, FALSE);
	    } else if (unwornmask & W_ARMG) {	/* stole worn gloves */
		mselftouch(mdef, (const char *)0, TRUE);
		if (mdef->mhp <= 0)	/* it's now a statue */
		    return;		/* can't continue stealing */
	    }

	    if (!stealoid) break;	/* only taking one item */
	}
}

int
damageum(mdef, mattk)
register struct monst *mdef;
register struct attack *mattk;
{
	register struct permonst *pd = mdef->data;
	register int	tmp = d((int)mattk->damn, (int)mattk->damd);
	int armpro;
	boolean negated;
	register int    enchantlvl = 0;
	boolean noeffect = FALSE;

	armpro = magic_negation(mdef);
	if (mdef->data->mr >= 49) armpro++; /* highly magic resistant monsters should have magic cancellation --Amy */
	if (mdef->data->mr >= 69) armpro++;
	if (mdef->data->mr >= 99) armpro++;
	/* since hero can't be cancelled, only defender's armor applies */
	negated = !((rn2(3) >= armpro) || !rn2(50));

	if (hit_as_one(&youmonst))    enchantlvl = 1; 
	if (hit_as_two(&youmonst))    enchantlvl = 2;         
	if (hit_as_three(&youmonst))  enchantlvl = 3; 
	if (hit_as_four(&youmonst))   enchantlvl = 4;         

	if (need_one(mdef)   && enchantlvl < 1) noeffect = TRUE; 
	if (need_two(mdef)   && enchantlvl < 2) noeffect = TRUE;      
	if (need_three(mdef) && enchantlvl < 3) noeffect = TRUE;  
	if (need_four(mdef)  && enchantlvl < 4) noeffect = TRUE;  

	if (is_demon(youmonst.data) && !rn2(23) && !uwep
		&& u.umonnum != PM_SUCCUBUS && u.umonnum != PM_INCUBUS
		&& u.umonnum != PM_BALROG && u.umonnum != PM_NEWS_DAEMON
		&& u.umonnum != PM_PRINTER_DAEMON) {
	    demonpet();
	    return(0);
	}
	switch(mattk->adtyp) {
	    case AD_STUN:
		if(!Blind)
		    pline("%s %s for a moment.", Monnam(mdef),
			  makeplural(stagger(mdef->data, "stagger")));
		mdef->mstun = 1;
		goto physical;
	    case AD_LEGS:
	     /* if (u.ucancelled) { */
	     /*    tmp = 0;	    */
	     /*    break;	    */
	     /* }		    */
		goto physical;
	    case AD_WERE:	    /* no special effect on monsters */
	    case AD_HEAL:	    /* likewise */
	    case AD_PHYS:
 physical:
		if(mattk->aatyp == AT_WEAP) {
		    if(uwep) tmp = 0;
		} else if(mattk->aatyp == AT_KICK) {
		    if(thick_skinned(mdef->data)) tmp = 0;
		    if(mdef->data == &mons[PM_SHADE]) {
			if (!(uarmf && uarmf->blessed)) {
			    impossible("bad shade attack function flow?");
			    tmp = 0;
			} else
			    tmp = rnd(4); /* bless damage */
		    }
		} else if(mattk->aatyp == AT_HUGS &&
			u.umonnum == PM_ROPE_GOLEM) {
		    if (breathless(mdef->data)) tmp = (tmp + 1) / 2;
		}
		break;
	    case AD_FIRE:
		if (negated) {
		    tmp = 0;
		    break;
		}
		if (!Blind)
		    pline("%s is %s!", Monnam(mdef),
			  on_fire(mdef->data, mattk));
		if (pd == &mons[PM_STRAW_GOLEM] ||
		    pd == &mons[PM_PAPER_GOLEM]) {
		    if (!Blind)
		    	pline("%s burns completely!", Monnam(mdef));
		    xkilled(mdef,2);
		    tmp = 0;
		    break;
		    /* Don't return yet; keep hp<1 and tmp=0 for pet msg */
		}
		if (pd == &mons[PM_STRAW_GOLEM] ||
			pd == &mons[PM_PAPER_GOLEM] ||
		    pd == &mons[PM_WAX_GOLEM]) {
		    if (!Blind)
			pline("%s falls to pieces!", Monnam(mdef));
			xkilled(mdef,3);
			return(2);
		}
		tmp += destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
		tmp += destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
		if (resists_fire(mdef)) {
		    if (!Blind)
			pline_The("fire doesn't heat %s!", mon_nam(mdef));
		    golemeffects(mdef, AD_FIRE, tmp);
		    shieldeff(mdef->mx, mdef->my);
		    tmp = 0;
		}
		/* only potions damage resistant players in destroy_item */
		tmp += destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
		break;
	    case AD_COLD:
		if (negated) {
		    tmp = 0;
		    break;
		}
		if (!Blind) pline("%s is covered in frost!", Monnam(mdef));
		if (resists_cold(mdef)) {
		    shieldeff(mdef->mx, mdef->my);
		    if (!Blind)
			pline_The("frost doesn't chill %s!", mon_nam(mdef));
		    golemeffects(mdef, AD_COLD, tmp);
		    tmp = 0;
		}
		tmp += destroy_mitem(mdef, POTION_CLASS, AD_COLD);
		break;
	    case AD_ELEC:
		if (negated) {
		    tmp = 0;
		    break;
		}
		if (!Blind) pline("%s is zapped!", Monnam(mdef));
		tmp += destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
		if (resists_elec(mdef)) {
		    if (!Blind)
			pline_The("zap doesn't shock %s!", mon_nam(mdef));
		    golemeffects(mdef, AD_ELEC, tmp);
		    shieldeff(mdef->mx, mdef->my);
		    tmp = 0;
		}
		/* only rings damage resistant players in destroy_item */
		tmp += destroy_mitem(mdef, RING_CLASS, AD_ELEC);
		break;
	    case AD_ACID:
		if (resists_acid(mdef)) tmp = 0;
		break;
	    case AD_STON:
		if (!munstone(mdef, TRUE))
		    minstapetrify(mdef, TRUE);
		tmp = 0;
		break;
#ifdef SEDUCE
	    case AD_SSEX:
#endif
	    case AD_SEDU:
	    case AD_SITM:
		    /*Your("stealing attack goes off"); for debug purposes */
		if (!rn2(4)) steal_it(mdef, mattk);
		tmp = 0;
		break;
	    case AD_EGLD:
	    case AD_SGLD:
#ifndef GOLDOBJ
		if (mdef->mgold) {
		    u.ugold += mdef->mgold;
		    mdef->mgold = 0;
		    (mattk->adtyp) ? You("nab some gold.") : Your("purse feels heavier.");
		}
#else
                /* This you as a leprechaun, so steal
                   real gold only, no lesser coins */
	        {
		    struct obj *mongold = findgold(mdef->minvent);
	            if (mongold) {
		        obj_extract_self(mongold);  
		        if (merge_choice(invent, mongold) || inv_cnt() < 52) {
			    addinv(mongold);
			    (mattk->adtyp) ? You("nab some gold.") : Your("purse feels heavier.");
			} else {
                            You("grab %s's gold, but find no room in your knapsack.", mon_nam(mdef));
			    dropy(mongold);
		        }
		    }
	        }
#endif
		exercise(A_DEX, TRUE);
		tmp = 0;
		break;
	    case AD_TLPT:
		if (tmp <= 0) tmp = 1;
		if (!negated && tmp < mdef->mhp) {
		    char nambuf[BUFSZ];
		    boolean u_saw_mon = canseemon(mdef) ||
					(u.uswallow && u.ustuck == mdef);
		    /* record the name before losing sight of monster */
		    Strcpy(nambuf, Monnam(mdef));
		    if (u_teleport_mon(mdef, FALSE) &&
			    u_saw_mon && !canseemon(mdef))
			pline("%s suddenly disappears!", nambuf);
		}
		break;
	    case AD_BLND:
		if (can_blnd(&youmonst, mdef, mattk->aatyp, (struct obj*)0)) {
		    if(!Blind && mdef->mcansee)
			pline("%s is blinded.", Monnam(mdef));
		    mdef->mcansee = 0;
		    tmp += mdef->mblinded;
		    if (tmp > 127) tmp = 127;
		    mdef->mblinded = tmp;
		}
		tmp = 0;
		break;
	    case AD_CURS:
		if (night() && !rn2(10) && !mdef->mcan) {
		    if (mdef->data == &mons[PM_CLAY_GOLEM]) {
			if (!Blind)
			    pline("Some writing vanishes from %s head!",
				s_suffix(mon_nam(mdef)));
			xkilled(mdef, 0);
			/* Don't return yet; keep hp<1 and tmp=0 for pet msg */
		    } else {
			mdef->mcan = 1;
			You("chuckle.");
		    }
		}
		tmp = 0;
		break;
	    case AD_DRLI:
		if (!negated && !rn2(3) && !resists_drli(mdef)) {
			int xtmp = d(2,6);
			if (mdef->mhp < xtmp) xtmp = mdef->mhp;
			/* Player vampires are smart enough not to feed while
			 * biting if they might have trouble getting it down */
			if (maybe_polyd(is_vampire(youmonst.data), 
			    Race_if(PM_VAMPIRE)) && mattk->aatyp == AT_BITE &&
			    has_blood(pd) && u.uhunger <= 1420 ) {
				/* For the life of a creature is in the blood
				   (Lev 17:11) */
				if (flags.verbose)
				    You("feed on the lifeblood.");
				/* [ALI] Biting monsters does not count against
				   eating conducts. The draining of life is
				   considered to be primarily a non-physical
				   effect */
				lesshungry(xtmp * 12);
			}
			pline("%s suddenly seems weaker!", Monnam(mdef));
			mdef->mhpmax -= xtmp;
#ifdef SHOW_DMG
			if (xtmp < mdef->mhp) showdmg(xtmp);
#endif
			if ((mdef->mhp -= xtmp) <= 0 || !mdef->m_lev) {
				pline("%s dies!", Monnam(mdef));
				xkilled(mdef,0);
			} else
				mdef->m_lev--;
		tmp = 0;
		}
		break;
	    case AD_RUST:
		if (pd == &mons[PM_IRON_GOLEM]) {
			pline("%s falls to pieces!", Monnam(mdef));
			xkilled(mdef,0);
		}
		hurtmarmor(mdef, AD_RUST);
		tmp = 0;
		break;
	    case AD_CORR:
		hurtmarmor(mdef, AD_CORR);
		tmp = 0;
		break;
	    case AD_DCAY:
		if (pd == &mons[PM_WOOD_GOLEM] ||
		    pd == &mons[PM_LEATHER_GOLEM]) {
			pline("%s falls to pieces!", Monnam(mdef));
			xkilled(mdef,0);
		}
		hurtmarmor(mdef, AD_DCAY);
		tmp = 0;
		break;
	    case AD_DRST:
	    case AD_DRDX:
	    case AD_DRCO:
		if (!negated && !rn2(8)) {
		    Your("%s was poisoned!", mpoisons_subj(&youmonst, mattk));
		    if (resists_poison(mdef))
			pline_The("poison doesn't seem to affect %s.",
				mon_nam(mdef));
		    else {
			if (!rn2(100)) {
			    Your("poison was deadly...");
			    tmp = mdef->mhp;
			} else tmp += rn1(10,6);
		    }
		}
		break;
	    case AD_DRIN:
		if (notonhead || !has_head(mdef->data)) {
		    pline("%s doesn't seem harmed.", Monnam(mdef));
		    tmp = 0;
		    if (!Unchanging && mdef->data == &mons[PM_GREEN_SLIME]) {
			if (!Slimed) {
			    You("suck in some slime and don't feel very well.");
			    Slimed = 100L;
			}
		    }
		    break;
		}
		if (m_slips_free(mdef, mattk)) break;

		if ((mdef->misc_worn_check & W_ARMH) && rn2(8)) {
		    pline("%s helmet blocks your attack to %s head.",
			  s_suffix(Monnam(mdef)), mhis(mdef));
		    break;
		}
		if (rn2(5)) break; /* would otherwise be way too overpowered --Amy */

		You("eat %s brain!", s_suffix(mon_nam(mdef)));
		u.uconduct.food++;
		if (touch_petrifies(mdef->data) && !Stone_resistance && !Stoned) {
		    Stoned = 7;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mdef->data->mname;
		}
          	/*		handled in tohit

                  } else if (touch_disintegrates(mdef->data)) {
                  tmp += instadisintegrate(mdef->data->mname); */

		if (!vegan(mdef->data))
		    u.uconduct.unvegan++;
		if (!vegetarian(mdef->data))
		    violated_vegetarian();
		if (mindless(mdef->data)) {
		    pline("%s doesn't notice.", Monnam(mdef));
		    break;
		}
		tmp += rnd(10);
		morehungry(-rnd(30)); /* cannot choke */
		if (ABASE(A_INT) < AMAX(A_INT)) {
			ABASE(A_INT) += rnd(4);
			if (ABASE(A_INT) > AMAX(A_INT))
				ABASE(A_INT) = AMAX(A_INT);
			flags.botl = 1;
		}
		exercise(A_WIS, TRUE);
		break;
	    case AD_STCK:
		if (!negated && !sticks(mdef->data))
		    setustuck(mdef); /* it's now stuck to you */
		break;
	    case AD_WRAP:
		if (!sticks(mdef->data)) {
		    if (!u.ustuck && !rn2(10)) {
			if (m_slips_free(mdef, mattk)) {
			    tmp = 0;
			} else {
			    You("swing yourself around %s!",
				  mon_nam(mdef));
			    setustuck(mdef);
			}
		    } else if(u.ustuck == mdef) {
			/* Monsters don't wear amulets of magical breathing */
			if (is_pool(u.ux,u.uy, FALSE) && !is_swimmer(mdef->data) &&
			    !amphibious(mdef->data)) {
			    You("drown %s...", mon_nam(mdef));
			    tmp = mdef->mhp;
			} else if(mattk->aatyp == AT_HUGS)
			    pline("%s is being crushed.", Monnam(mdef));
		    } else {
			tmp = 0;
			if (flags.verbose)
			    You("brush against %s %s.",
				s_suffix(mon_nam(mdef)),
				mbodypart(mdef, LEG));
		    }
		} else tmp = 0;
		break;
	    case AD_PLYS:
		if (!negated && mdef->mcanmove && !rn2(3) && tmp < mdef->mhp) {
		    if (!Blind) pline("%s is frozen by you!", Monnam(mdef));
		    mdef->mcanmove = 0;
		    mdef->mfrozen = rnd(10);
		}
		break;
	    case AD_DEPR:
		pline("%s seems vaguely bored.", Monnam(mdef));
		break;
	    case AD_TCKL:
		if (!negated && mdef->mcanmove && !rn2(3) && tmp < mdef->mhp) {
		    if (!Blind) You("mercilessly tickle %s!", mon_nam(mdef));
		    mdef->mcanmove = 0;
		    mdef->mfrozen = rnd(10);
		}
		break;
	    case AD_SLEE:
		if (mattk->aatyp == AT_GAZE && mon_reflects(mdef, (char *)0)) {
		    tmp = 0;
		    (void) mon_reflects(mdef, "But it reflects from %s %s!");
		    if (Sleep_resistance || Free_action) {
			pline("You yawn.");
			break;
		    } else {
			nomul(-rnd(10), "sleeping from your own reflected gaze");
			u.usleep = 1;
			nomovemsg = "You wake up.";
			if (Blind)  You("are put to sleep!");
			else You("are put to sleep by your reflected gaze!");
			break;
		    }
		}

		if (!negated && !mdef->msleeping &&
			(mattk->aatyp != AT_WEAP || barehanded_hit) &&
			sleep_monst(mdef, rnd(10), -1)) {
		    if (!Blind)
			pline("%s is put to sleep by you!", Monnam(mdef));
		    slept_monst(mdef);
		}
		else
		    tmp = 0;
		break;
	    case AD_SLIM:
		if (negated) break;	/* physical damage only */
		if (!rn2(400) && !flaming(mdef->data) &&
				mdef->data != &mons[PM_GREEN_SLIME]) {
		    You("turn %s into slime.", mon_nam(mdef));
		    (void) newcham(mdef, &mons[PM_GREEN_SLIME], FALSE, !Blind);
		    tmp = 0;
		}
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		/* There's no msomearmor() function, so just do damage */
	     /* if (negated) break; */
		break;
	    case AD_SLOW:
		if (!negated && mdef->mspeed != MSLOW) {
		    unsigned int oldspeed = mdef->mspeed;

		    mon_adjust_speed(mdef, -1, (struct obj *)0);
		    if (mdef->mspeed != oldspeed && canseemon(mdef))
			pline("%s slows down.", Monnam(mdef));
		}
		break;
	    case AD_CONF:
		if (!mdef->mconf) {
		    if (canseemon(mdef))
			pline("%s looks confused.", Monnam(mdef));
		    mdef->mconf = 1;
		}
		else
		{
		    if (canseemon(mdef))
			pline("%s is getting more and more confused.",
				Monnam(mdef));
		    mdef->mconf++;
		}
		break;
	    case AD_POLY:
		if (tmp < mdef->mhp) {
		    if (resists_magm(mdef)) {
			/* magic resistance protects from polymorph traps,
			 * so make it guard against involuntary polymorph
			 * attacks too... */
			shieldeff(mdef->mx, mdef->my);
#if 0
		    } else if (!rn2(25) || !mon_poly(mdef)) {
			if (canseemon(mdef)) {
			    pline("%s shudders!", Monnam(mdef));
			}
			/* no corpse after system shock */
			tmp = rnd(30);
#endif
		    } else if (!mon_poly(mdef, TRUE,
			    "%s undergoes a freakish metamorphosis!"))
			/* prevent killing the monster again - 
			 * could be killed in mon_poly */
			tmp = 0;
		}
		break;
		/* WAC -- for death gazes - but all messages should be generic */
	    case AD_DETH:
		if (rn2(16)) {
		    /* Just damage */
		    break;
		}
		if (mattk->aatyp == AT_GAZE) 
		    You("look directly at %s!", mon_nam(mdef));
		if ((mattk->aatyp == AT_GAZE) && (mon_reflects(mdef, (char *)0))) {
		    /* WAC reflected gaze 
		     * Oooh boy...that was a bad move :B 
		     */
		    tmp = 0;
		    shieldeff(mdef->mx, mdef->my);
		    (void) mon_reflects(mdef, "But it reflects from %s %s!");
		    if (Antimagic) {
			You("shudder momentarily...");
			break;
		    }
		    You("die...");
		    killer_format = KILLED_BY;
		    killer = "a reflected gaze of death";
		    done(DIED);
		} else if (is_undead(mdef->data)) {
		    /* Still does normal damage */
		    if (!Blind) pline("Something didn't work...");
		    break;
		} else if (resists_magm(mdef)) {
		    if (!Blind)
			pline("%s shudders momentarily...", Monnam(mdef));
		} else {
		    tmp = mdef->mhp;
		}
		break;
	    case AD_DREN:
	    	if (resists_magm(mdef)) {
		    if (!Blind) {
			shieldeff(mdef->mx,mdef->my);
			pline("%s is unaffected.", Monnam(mdef));
		    }
	    	} else {
	    	    mon_drain_en(mdef, 
				((mdef->m_lev > 0) ? (rnd(mdef->m_lev)) : 0) + 1);
	    	}
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		/* Certain monsters aren't even made peaceful. */
		if (!mdef->iswiz && mdef->data != &mons[PM_MEDUSA] &&
				!(mdef->data->mflags3 & M3_COVETOUS) &&
				!(mdef->data->geno & G_UNIQ)) {
		    pline("You calm %s.", mon_nam(mdef));
		    mdef->mpeaceful = 1;
		    mdef->mtame = 0;
		    tmp = 0;
		}
		break;
	    case AD_SCLD:
	    if (negated) {
		tmp = 0;
		break;
	    }
	    if (!Blind)
		pline("%s is being steamed!", Monnam(mdef));
		if (resists_fire(mdef)) {
		    if (!Blind)
			pline_The("steam doesn't scald %s!", mon_nam(mdef));
		    shieldeff(mdef->mx, mdef->my);
		    tmp = 0;
		}
		/* only potions damage resistant players in destroy_item */
		tmp += destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
		if(!rn2(10)) hurtmarmor(mdef, AD_RUST);
		break;
	    /* this should be #monster-able, not attack */
	    case AD_FLVR:
		if (negated){ tmp = 0; break;}
		switch (rn2(6)){
		    case 0: /* up, copied from muse: MUSE_POT_GAIN_LEVEL */
			if (Can_rise_up(mdef->mx, mdef->my, &u.uz)){
			    register int tolev=depth(&u.uz)-1;
			    d_level tolevel;
			    get_level(&tolevel, tolev);
			    if(on_level(&tolevel, &u.uz)) goto uhitm_flvr_strange;
			    if (canseemon(mdef))
				pline("%s rises up, through the %s!",
				  Monnam(mdef), ceiling(mdef->mx, mdef->my));
			    migrate_to_level(mdef, ledger_no(&tolevel),
			    MIGR_RANDOM, (coord *)0);
			    break;
			}
			else{goto uhitm_flvr_strange;}
			break;
		    case 1: /* down */
			if (Can_fall_thru(&u.uz) /* && !In_sokoban(&u.uz)*/ ){
			    register int tolev=depth(&u.uz)+1;
			    d_level tolevel;
			    get_level(&tolevel, tolev);
			    if (mon_has_amulet(mdef) || In_endgame(&u.uz) 
			      || on_level(&tolevel, &u.uz))
				goto uhitm_flvr_strange;
			    if (canseemon(mdef))
				pline("%s sinks down, through the %s!", 
				  Monnam(mdef), surface(mdef->mx, mdef->my));
			    migrate_to_level(mdef,ledger_no(&tolevel),
			      MIGR_RANDOM, (coord *)0);
			    break;
			}
			else goto uhitm_flvr_strange;
		    case 2: /* top, teleport to dlev1, top level of branch? */
		    case 3: /* bottom, teleport to Moloch's sanctum, bot lev of branch? */ 
		    case 4: /* strange */
uhitm_flvr_strange:
			if (canseemon(mdef)) pline("%s reacts strangely.", Monnam(mdef));
			mdef->mconf = 1;
			mdef->mstrategy &= ~STRAT_WAITFORU;
			break;
		    case 5: /* charm, fall through */ 
			(void)docharm(); 
		} 
		break;
	    case AD_CHRM:
		tmp=0;
		break;
	    default:	/*tmp = 0;*/ /*making uncommon polymorph forms with weird attacks like AD_DARK stronger. --Amy*/
		break;
	}

	if ( (Race_if(PM_HUMAN_WEREWOLF) || Role_if(PM_LUNATIC) ) && Upolyd) tmp += rnd(u.ulevel); /* come on, werewolves need some love too! --Amy */

	mdef->mstrategy &= ~STRAT_WAITFORU; /* in case player is very fast */
	if (tmp && noeffect && !DEADMONSTER(mdef)) {
	     You("don't seem to harm %s.", mon_nam(mdef));
	     tmp = 0;
	     return 1;
	}

#ifdef SHOW_DMG
	if (tmp < mdef->mhp) showdmg(tmp);
#endif
	
	/* if tmp == 0, DON'T xkilled/killed the monster even if hp < 1 
	 *	- xkilled/killed via other method... */

	if((mdef->mhp -= tmp) < 1) {
	    if (mdef->mtame && !cansee(mdef->mx,mdef->my)) {
		You_feel("embarrassed for a moment.");
		if (tmp) xkilled(mdef, 0); /* !tmp but hp<1: already killed */
	    } else if (!flags.verbose) {
		You("destroy it!");
		if (tmp) xkilled(mdef, 0);
	    } else
		if (tmp) killed(mdef);
	    return(2);
	}
	return(1);
}

STATIC_OVL int
explum(mdef, mattk)
register struct monst *mdef;
register struct attack *mattk;
{
	register int tmp = d((int)mattk->damn, (int)mattk->damd);

	You("explode!");
	switch(mattk->adtyp) {
	    boolean resistance; /* only for cold/fire/elec */

	    case AD_BLND:
		if (!resists_blnd(mdef)) {
		    pline("%s is blinded by your flash of light!", Monnam(mdef));
		    mdef->mblinded = min((int)mdef->mblinded + tmp, 127);
		    mdef->mcansee = 0;
		}
		break;
	    case AD_HALU:
		if (haseyes(mdef->data) && mdef->mcansee) {
		    pline("%s is affected by your flash of light!",
			  Monnam(mdef));
		    mdef->mconf = 1;
		}
		break;
	    case AD_COLD:
		resistance = resists_cold(mdef);
		goto common;
	    case AD_FIRE:
	    case AD_SCLD:
		resistance = resists_fire(mdef);
		goto common;
	    case AD_ELEC:
		resistance = resists_elec(mdef);
common:
		if (!resistance) {
		    pline("%s gets blasted!", Monnam(mdef));
		    mdef->mhp -= tmp;
		    if (mdef->mhp <= 0) {
			 killed(mdef);
			 return(2);
		    }
		} else {
		    shieldeff(mdef->mx, mdef->my);
		    if (is_golem(mdef->data))
			golemeffects(mdef, (int)mattk->adtyp, tmp);
		    else
			pline_The("blast doesn't seem to affect %s.",
				mon_nam(mdef));
		}
		break;
	    default:
		break;
	}
	return(1);
}

STATIC_OVL void
start_engulf(mdef)
struct monst *mdef;
{
	if (!Invisible) {
		map_location(u.ux, u.uy, TRUE);
		tmp_at(DISP_ALWAYS, mon_to_glyph(&youmonst));
		tmp_at(mdef->mx, mdef->my);
	}
	You("engulf %s!", mon_nam(mdef));
	delay_output();
	delay_output();
}

STATIC_OVL void
end_engulf()
{
	if (!Invisible) {
		tmp_at(DISP_END, 0);
		newsym(u.ux, u.uy);
	}
}

STATIC_OVL int
gulpum(mdef,mattk)
register struct monst *mdef;
register struct attack *mattk;
{
	register int tmp;
	register int dam = d((int)mattk->damn, (int)mattk->damd);
	struct obj *otmp;
	/* Not totally the same as for real monsters.  Specifically, these
	 * don't take multiple moves.  (It's just too hard, for too little
	 * result, to program monsters which attack from inside you, which
	 * would be necessary if done accurately.)  Instead, we arbitrarily
	 * kill the monster immediately for AD_DGST and we regurgitate them
	 * after exactly 1 round of attack otherwise.  -KAA
	 */

	if(mdef->data->msize >= MZ_HUGE) return 0;

	if(YouHunger < (Race_if(PM_INCANTIFIER) ? u.uenmax*3/4 : 1500) && !u.uswallow) {
	    for (otmp = mdef->minvent; otmp; otmp = otmp->nobj)
		(void) snuff_lit(otmp);

	    if((!touch_petrifies(mdef->data) || Stone_resistance)
		&& (!touch_disintegrates(mdef->data) || Disint_resistance)){
#ifdef LINT	/* static char msgbuf[BUFSZ]; */
		char msgbuf[BUFSZ];
#else
		static char msgbuf[BUFSZ];
#endif
		start_engulf(mdef);
		switch(mattk->adtyp) {
		    case AD_DGST:
			/* eating a Rider or its corpse is fatal */
			if (is_rider(mdef->data)) {
			 pline("Unfortunately, digesting any of it is fatal.");
			    end_engulf();
			    Sprintf(msgbuf, "unwisely tried to eat %s",
				    mdef->data->mname);
			    killer = msgbuf;
			    killer_format = NO_KILLER_PREFIX;
			    done(DIED);
			    return 0;		/* lifesaved */
			}

			if (Slow_digestion) {
			    dam = 0;
			    break;
			}

			/* KMH, conduct */
			u.uconduct.food++;
			if (!vegan(mdef->data))
			     u.uconduct.unvegan++;
			if (!vegetarian(mdef->data))
			     violated_vegetarian();

			/* Use up amulet of life saving */
			if (!!(otmp = mlifesaver(mdef))) m_useup(mdef, otmp);

			newuhs(FALSE);
			xkilled(mdef,2);
			if (mdef->mhp > 0) { /* monster lifesaved */
			    You("hurriedly regurgitate the sizzling in your %s.",
				body_part(STOMACH));
			} else {
			    tmp = 1 + (mdef->data->cwt >> 8);
			    if (corpse_chance(mdef, &youmonst, TRUE) &&
				!(mvitals[monsndx(mdef->data)].mvflags &
				  G_NOCORPSE)) {
				/* nutrition only if there can be a corpse */
				if(Race_if(PM_INCANTIFIER)) u.uen += mdef->m_lev;
				else u.uhunger += (mdef->data->cnutrit+1) / 2;
			    } else tmp = 0;
			    Sprintf(msgbuf, "You totally digest %s.",
					    mon_nam(mdef));
			    if (tmp != 0) {
				/* setting afternmv = end_engulf is tempting,
				 * but will cause problems if the player is
				 * attacked (which uses his real location) or
				 * if his See_invisible wears off
				 */
				You("digest %s.", mon_nam(mdef));
				if (Slow_digestion) tmp *= 2;
				nomul(-tmp, "digesting a monster");
				nomovemsg = msgbuf;
			    } else pline("%s", msgbuf);
			    if (mdef->data == &mons[PM_GREEN_SLIME]) {
				Sprintf(msgbuf, "%s isn't sitting well with you.",
					The(mdef->data->mname));
				if (!Unchanging) {
					Slimed = 50L;
					flags.botl = 1;
				}
			    } else
			    exercise(A_CON, TRUE);
			}
			end_engulf();
			return(2);
			break;
		    case AD_PHYS:
			if (youmonst.data == &mons[PM_FOG_CLOUD]) {
			    pline("%s is laden with your moisture.",
				  Monnam(mdef));
			    if (amphibious(mdef->data) &&
				!flaming(mdef->data)) {
				dam = 0;
				pline("%s seems unharmed.", Monnam(mdef));
			    }
			} else
			    pline("%s is pummeled with your debris!",
				  Monnam(mdef));
			break;
		    case AD_ACID:
			pline("%s is covered with your goo!", Monnam(mdef));
			if (resists_acid(mdef)) {
			    pline("It seems harmless to %s.", mon_nam(mdef));
			    dam = 0;
			}
			break;
		    case AD_BLND:
			if (can_blnd(&youmonst, mdef, mattk->aatyp, (struct obj *)0)) {
			    if (mdef->mcansee)
				pline("%s can't see in there!", Monnam(mdef));
			    mdef->mcansee = 0;
			    dam += mdef->mblinded;
			    if (dam > 127) dam = 127;
			    mdef->mblinded = dam;
			}
			dam = 0;
			break;
		    case AD_ELEC:
			if (rn2(2)) {
			    pline_The("air around %s crackles with electricity.", mon_nam(mdef));
			    if (resists_elec(mdef)) {
				pline("%s seems unhurt.", Monnam(mdef));
				dam = 0;
			    }
			    golemeffects(mdef,(int)mattk->adtyp,dam);
			} else dam = 0;
			break;
		    case AD_COLD:
			if (rn2(2)) {
			    if (resists_cold(mdef)) {
				pline("%s seems mildly chilly.", Monnam(mdef));
				dam = 0;
			    } else
				pline("%s is freezing to death!",Monnam(mdef));
			    golemeffects(mdef,(int)mattk->adtyp,dam);
			} else dam = 0;
			break;
		    case AD_FIRE:
			if (rn2(2)) {
			    if (resists_fire(mdef)) {
				pline("%s seems mildly hot.", Monnam(mdef));
				dam = 0;
			    } else
				pline("%s is burning to a crisp!",Monnam(mdef));
			    golemeffects(mdef,(int)mattk->adtyp,dam);
			} else dam = 0;
			break;
		    case AD_SCLD:
			if (rn2(2)) {
			    if (resists_fire(mdef)) {
				pline("%s seems mildly hot.", Monnam(mdef));
				dam = 0;
			    } else
				pline("%s is severly scalded!", Monnam(mdef));
				if(!rn2(3)) hurtmarmor(mdef, AD_RUST);
			} else dam=0;
			break;
		}
		end_engulf();
		if ((mdef->mhp -= dam) <= 0) {
		    killed(mdef);
		    if (mdef->mhp <= 0)	/* not lifesaved */
			return(2);
		}
		You("%s %s!", is_animal(youmonst.data) ? "regurgitate"
			: "expel", mon_nam(mdef));
		if (Slow_digestion || is_animal(youmonst.data)) {
		    pline("Obviously, you didn't like %s taste.",
			  s_suffix(mon_nam(mdef)));
		}
	    } else {
		char kbuf[BUFSZ];

		You("bite into %s.", mon_nam(mdef));
		Sprintf(kbuf, "swallowing %s whole", an(mdef->data->mname));
		instapetrify(kbuf);
		instadisintegrate(kbuf);
	    }
	}
	return(0);
}

void
missum(mdef, target, roll, mattk)
register struct monst *mdef;
register struct attack *mattk;
register int target;
register int roll;
{
	register boolean nearmiss = (target == roll);
	register struct obj *blocker = (struct obj *)0;
	long mwflags = mdef->misc_worn_check;

		/* 3 values for blocker
		 *	No blocker:  (struct obj *) 0  
		 * 	Piece of armour:  object
		 */

	/* This is a hack,  since there is no fast equivalent for uarm, uarms, etc.  
	 * Technically, we really should check from the inside out...
	 */
	if (target < roll) {
	    for (blocker = mdef->minvent; blocker; blocker = blocker->nobj) {
		if (blocker->owornmask & mwflags) {
			target += ARM_BONUS(blocker);
			if (target > roll) break;
		}
	    }
	}
	
	if (could_seduce(&youmonst, mdef, mattk)) {
		You("pretend to be friendly to %s.", mon_nam(mdef));
	} else if(canspotmon(mdef) && flags.verbose) {
		if (nearmiss || !blocker) {
		    You("%smiss %s.", (nearmiss ? "just " : ""),mon_nam(mdef));
		} else {
        	    /* Blocker */
        	    pline("%s %s %s your attack.", 
        		s_suffix(Monnam(mdef)),
        		aobjnam(blocker, (char *)0),
        		(rn2(2) ? "blocks" : "deflects"));    
		}
	} else {
		You("%smiss it.", ((flags.verbose && nearmiss) ? "just " : ""));
	}
	if (!mdef->msleeping && mdef->mcanmove)
		wakeup(mdef);
}

/*
 * [WAC] This code now handles twoweapon in the following way:
 *	-monster with one or 2 AT_WEAP can get primary and secondary attacks
 *	-monster with AT_WEAP and another hand attack(s) will lose one
 *		of the other hand attacks
 *	-monster with several hand attacks will do primary weapon and secondary
 *		weapon attacks with the first 2 hand attacks,  then claw attacks
 *		for the rest
 *	-Code assumes that a single AT_WEAP or 2 AT_WEAP entries still mean 
 *		at most 1 or 2 hands attack.  i.e. 1 handed monsters with AT_WEAP
 *		or monsters with 3+ handed monsters where more than 2 hands are
 *		AT_WEAP are not handled properly
 *		(I don't think any exist yet)
 * This code now handles ALL hand to hand whether you are poly'ed or not
 * (uses your current race as the monster type)
 *
 * [ALI] Returns TRUE if you hit (and maybe killed) the monster.
 */
STATIC_OVL boolean
hmonas(mon, tmp)		/* attack monster as a monster. */
register struct monst *mon;
register int tmp;
{
	struct attack *mattk, alt_attk;
	int	i, sum[NATTK];
#if 0
	int	hittmp = 0;
#endif
	int	nsum = 0;
	int	dhit = 0;
	int 	mhit = 0; /* Used to pass the attacks used */
	int 	tmp1, tmp2;
	boolean Old_Upolyd = Upolyd;
	static const int hit_touch[] = {0, HIT_BODY, HIT_BODY|HIT_FATAL};
	static const int hit_notouch[] = {0, HIT_OTHER, HIT_OTHER|HIT_FATAL};
	
	/* Keeps track of which weapon hands have been used */
	boolean used_uwep = FALSE;

	for(i = 0; i < NATTK; i++) {
	    mhit = 0; /* Clear all previous attacks */

	    sum[i] = 0;
	    mattk = getmattk(youmonst.data, i, sum, &alt_attk);
	    
	    switch(mattk->aatyp) {
		case AT_WEAP:
use_weapon:	
	/* Certain monsters don't use weapons when encountered as enemies,
	 * but players who polymorph into them have hands or claws and thus
	 * should be able to use weapons.  This shouldn't prohibit the use
	 * of most special abilities, either.
	 */
	/* Potential problem: if the monster gets multiple weapon attacks,
	 * we currently allow the player to get each of these as a weapon
	 * attack.  Is this really desirable?
	 * [WAC] See Above ...  anyways,  this was changed in 3.3.0 so that
	 * only attack 0 would give a weapon attack...
	 * [ALI] Most monsters should get multiple weapon attacks since they
	 * only have two hands. There are exceptions such as mariliths which
	 * should get two weapon attacks and four barehanded attacks. Such
	 * monsters should be special cased in AT_CLAW below.
	 */
			mhit = used_uwep ? HIT_USWAPWEP : HIT_UWEP;
			used_uwep = !used_uwep;
			if (mhit == HIT_USWAPWEP && !u.twoweap)
			    continue;	/* Skip this attack */

			/* WAC if attacking cockatrice/etc, player is smart
			   if wielding a weapon.  So don't let him
			   touch the monster */
			if ((uwep || u.twoweap && uswapwep) &&
				(mhit == HIT_UWEP && !uwep ||
				 mhit == HIT_USWAPWEP && !uswapwep) &&
				(touch_petrifies(mon->data) ||
				 mon->data == &mons[PM_MEDUSA]))
			    break;

			dhit = mhit; /* Clear the miss counter as attacks miss */
			tmp1 = tmp2 = tmp;

#ifdef DEBUG
			pline("%i/20", tmp);
#endif

			if (mhit & HIT_UWEP) {
			    if (uwep) tmp1 = tmp + hitval(uwep, mon);
			    tohit(UWEP_ROLL) = tmp1;
			    if (tmp1 <= (dice(UWEP_ROLL) = rnd(20)) &&
				    !u.uswallow)
				dhit &= ~HIT_UWEP; /* missed */
				
			    if (tmp1 > dice(UWEP_ROLL)) exercise(A_DEX, TRUE);
#ifdef DEBUG
			    pline("(%i/20)", tmp1);
#endif
			}

			if (mhit & HIT_USWAPWEP) {
			    if (uswapwep)
				tmp2 = tmp + hitval(uswapwep, mon) - 2;

			    tohit(USWAPWEP_ROLL) = tmp2;

			    if (tmp2 <= (dice(USWAPWEP_ROLL) = rnd(20)) &&
				    !u.uswallow)
				dhit &= ~HIT_USWAPWEP;

			    if (tmp2 > dice(USWAPWEP_ROLL))
				exercise(A_DEX, TRUE);
#ifdef DEBUG
			    pline("((%i/20))", tmp2);
#endif
			}

			if (dhit && mattk->adtyp == AD_SLEE)
			    barehanded_hit = (dhit & HIT_UWEP) && !uwep ||
			      (dhit & HIT_USWAPWEP) && !uswapwep;

#if 0 /* Old code */
			if (uwep) {
			    hittmp = hitval(uwep, mon);
			    hittmp += weapon_hit_bonus(uwep);
			    tmp += hittmp;
			}
			if (tmp > (dice(UWEP_ROLL) = rnd(20)) || u.uswallow)
			    dhit = HIT_UWEP;
			else dhit = 0;
			/* KMH -- Don't accumulate to-hit bonuses */
			if (uwep) tmp -= hittmp;
#endif
			/* Enemy dead, before any special abilities used */
			if (!known_hitum(mon,mhit,&dhit,mattk)) {
			    sum[i] = dhit | HIT_FATAL;
			    break;
			} else sum[i] = dhit;
			/* might be a worm that gets cut in half */
			if (m_at(u.ux+u.dx, u.uy+u.dy) != mon) return((boolean)(nsum != 0));
			/* Do not print "You hit" message, since known_hitum
			 * already did it.
			 */
			if (dhit && mattk->adtyp != AD_SPEL
				&& mattk->adtyp != AD_PHYS)
			    if (damageum(mon,mattk) == 2)
				sum[i] |= HIT_FATAL;
			break;
		case AT_CLAW:
			if (!cantwield(youmonst.data) &&
				u.umonnum != PM_MARILITH)
			    goto use_weapon;
#ifdef SEDUCE
#if 0	/* Shouldn't matter where the first AT_CLAW is anymore */
			/* succubi/incubi are humanoid, but their _second_
			 * attack is AT_CLAW, not their first...
			 */
			if (i==1 && uwep && (u.umonnum == PM_SUCCUBUS ||
				u.umonnum == PM_INCUBUS)) goto use_weapon;
#endif
#endif
		case AT_BITE:
			/* [ALI] Vampires are also smart. They avoid biting
			   monsters if doing so would be fatal */
			if ((uwep || u.twoweap && uswapwep) &&
				is_vampire(youmonst.data) &&
				(is_rider(mon->data) ||
				 mon->data == &mons[PM_GREEN_SLIME]))
			    break;
		case AT_STNG:
		case AT_TUCH:
		case AT_BUTT:
		case AT_LASH:
		case AT_TRAM:
		case AT_SCRA:
		case AT_TENT:
		case AT_NTCH: /* currently stopped at "compat", so no touching 
				 may require moving if non-tempting monsters
				 get the non-touching attack */
use_natural:

			if (i==0 && uwep && (youmonst.data->mlet==S_LICH)) goto use_weapon;
			if ((uwep || u.twoweap && uswapwep) &&
				(touch_petrifies(mon->data) ||
				 mon->data == &mons[PM_MEDUSA]))
			    break;
		case AT_KICK:
			if ((dhit = (tmp > (dieroll = rnd(20)) || u.uswallow)) != 0) {
			    int compat;

			    if (!u.uswallow &&
				(compat=could_seduce(&youmonst, mon, mattk))) {
				You("%s %s %s.",
				    mon->mcansee && haseyes(mon->data)
				    ? "smile at" : "talk to",
				    mon_nam(mon),
				    compat == 2 ? "engagingly":"seductively");
				/* doesn't anger it; no wakeup() */
				sum[i] = hit_notouch[damageum(mon, mattk)];
				break;
			    }
			    wakeup(mon);
			    /* maybe this check should be in damageum()? */
			    if (mon->data == &mons[PM_SHADE] &&
					!(mattk->aatyp == AT_KICK &&
					    uarmf && uarmf->blessed)) {
				Your("attack passes harmlessly through %s.",
				    mon_nam(mon));
				break;
			    }
			    if (mattk->aatyp == AT_KICK)
				    You("kick %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_BITE)
                                    You("%s %s.", has_beak(youmonst.data) ?
                                        "peck" : "bite", mon_nam(mon));
			    else if (mattk->aatyp == AT_STNG)
				    You("sting %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_BUTT)
				    You("butt %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_LASH)
				    You("lash %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_TRAM)
				    You("trample over %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_SCRA)
				    You("scratch %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_TUCH)
				    You("touch %s.", mon_nam(mon));
			    else if (mattk->aatyp == AT_TENT)
				    Your("tentacles suck %s.", mon_nam(mon));
                            else if (mattk->aatyp == AT_CLAW) {
                                    You("%s %s.", barehitmsg(&youmonst),mon_nam(mon));
                            }
			    else You("hit %s.", mon_nam(mon));
			    if (touch_disintegrates(mon->data) && !mon->mcan && mon->mhp>1){
				int dis_dmg = 0;
				if(mattk->aatyp == AT_KICK && uarmf){
				    if(!oresist_disintegration(uarmf)){
					dis_dmg += uarmf->owt;
					destroy_arm(uarmf);
				    }
				} else if(uarmg && (mattk->aatyp == AT_WEAP ||
				  mattk->aatyp == AT_CLAW || mattk->aatyp == AT_TUCH)){
				    if(!oresist_disintegration(uarmg)){
					dis_dmg += uarmg->owt;
					destroy_arm(uarmg);
				    } 
				} else if(mattk->aatyp == AT_BUTT && uarmh) {
				    if(!oresist_disintegration(uarmh)){
					dis_dmg += (uarmh->owt);
					destroy_arm(uarmh);
				    }
				} else {
				    char kbuf[BUFSZ];
				    Sprintf(kbuf, "touching %s", an(mon->data->mname));
				    mon->mhp -= instadisintegrate(kbuf);
				}
				sum[i] = 1;
				if(dis_dmg){
				    weight_dmg(dis_dmg);
				}
				mon->mhp -= dis_dmg;
				if (mon->mhp < 1) mon->mhp = 1;
			} else
			    sum[i] = hit_touch[damageum(mon, mattk)];
			} else
			    missum(mon, tmp, dieroll, mattk);
			break;

		case AT_HUGS:
			/* automatic if prev two attacks succeed, or if
			 * already grabbed in a previous attack
			 */
			dhit = 1;
			wakeup(mon);
			if (mon->data == &mons[PM_SHADE])
			    Your("hug passes harmlessly through %s.",
				mon_nam(mon));
			else if (!sticks(mon->data) && !u.uswallow) {
			    if (mon==u.ustuck) {
				pline("%s is being %s.", Monnam(mon),
				    u.umonnum==PM_ROPE_GOLEM ?
				    breathless(mon->data) ? "strangled" :
				    "choked" : "crushed");
				sum[i] = hit_touch[damageum(mon, mattk)];
			    } else if(i >= 2 && sum[i-1] && sum[i-2]) {
				You("grab %s!", mon_nam(mon));
				setustuck(mon);
				sum[i] = hit_touch[damageum(mon, mattk)];
			    }
			}
			break;

		case AT_EXPL:	/* automatic hit if next to */
			dhit = -1;
			wakeup(mon);
			sum[i] = hit_notouch[explum(mon, mattk)];
			break;

		case AT_ENGL:
			if((youmonst.data == &mons[PM_BANDERSNATCH]) && 
			  !yeasty_food(mon->data)) {
			    mattk->aatyp = AT_BITE;
			    mattk->adtyp = AD_PHYS;
			    mattk->damn  = 1;
			    mattk->damd  = 5;
			    goto use_natural;
			}
			if((dhit = (tmp > (dieroll = rnd(20+i))))) {
				wakeup(mon);
				if (mon->data == &mons[PM_SHADE])
				    Your("attempt to surround %s is harmless.",
					mon_nam(mon));
				else {
				    sum[i]= hit_touch[gulpum(mon,mattk)];
				    if (sum[i] & HIT_FATAL &&
					    (mon->data->mlet == S_ZOMBIE ||
						mon->data->mlet == S_MUMMY) &&
					    rn2(5) &&
					    !Sick_resistance) {
					You_feel("%ssick.",
					    (Sick) ? "very " : "");
					mdamageu(mon, rnd(8));
				    }
				}
			} else
				missum(mon, tmp, dieroll, mattk);
			break;
		case AT_TALK:
			You("talk to %s.  But you don't seem to be too convincing.", mon_nam(mon));
			break;
		case AT_MAGC:
			/* No check for uwep; if wielding nothing we want to
			 * do the normal 1-2 points bare hand damage...
			 */
			if (i == 0 && (youmonst.data->mlet==S_KOBOLD
				|| youmonst.data->mlet==S_ORC
				|| youmonst.data->mlet==S_GNOME
				)) goto use_weapon;
		case AT_NONE:
		case AT_BOOM:
			continue;
			/* Not break--avoid passive attacks from enemy */

		case AT_BREA:
		case AT_SPIT:
			dhit = 0;
			break;

		case AT_GAZE:   /* WAC -- can be either ranged attack OR close */
			if (Blind) {
				dhit = 0;
				break;
			}
			if (!canseemon(mon) && rn2(3)) {
				You("gaze around,  but miss!");
				dhit = 0;
				break;
			}
			You("gaze at %s...", mon_nam(mon));

			if ((mon->data==&mons[PM_MEDUSA]) && !mon->mcan) {
				pline("Gazing at the awake Medusa is not a very good idea.");
				/* as if gazing at a sleeping anything is fruitful... */
				/*You("turn to stone...");
				killer_format = KILLED_BY;
				killer = "deliberately gazing at Medusa's hideous countenance";
				done(STONING);*/
				You("start turning to stone...");
				if (!Stoned) Stoned = 7;
			} else if (!mon->mcansee || mon->msleeping) {
				pline("But nothing happens.");
				dhit = 0;
				break;
			} else if (Invis && !perceives(mon->data)) {
				pline("%s seems not to notice your gaze.", Monnam(mon));
				break;
			}
			sum[i] = hit_notouch[damageum(mon, mattk)];
			break;

		case AT_MULTIPLY:
			/* Not a #monster ability -- this is something that the
			 * player must figure out -RJ */
			cloneu();
			break;

		default: /* Strange... */
			impossible("strange attack of yours (%d)",
				 mattk->aatyp);
	    }
	    if (dhit == -1) {
		u.mh = -1;	/* dead in the current form */
		rehumanize();
	    }
	    if (sum[i] & HIT_FATAL)
		return((boolean)passive(mon, sum[i], 0, mattk->aatyp));
							/* defender dead */
	    else {
		(void) passive(mon, sum[i], 1, mattk->aatyp);
		nsum |= sum[i];
	    }
	    if (Upolyd != Old_Upolyd)
		break; /* No extra attacks if no longer a monster */
	    if (multi < 0)
		break; /* If paralyzed while attacking, i.e. floating eye */
	}
	return((boolean)(nsum != 0));
}

/*	Special (passive) attacks on you by monsters done here.		*/

int
passive(mon, mhit, malive, aatyp)
register struct monst *mon;
register int mhit;
register int malive;
uchar aatyp;
{
	register struct permonst *ptr = mon->data;
	register int i, tmp;
	register struct engr *ep = engr_at(u.ux,u.uy);
	struct obj *target = mhit & HIT_UWEP ? uwep :
		mhit & HIT_USWAPWEP ? uswapwep : (struct obj *)0;
	char buf[BUFSZ];

	/*int randattackC = 0;*/
	uchar atttypC;

	if (mhit && aatyp == AT_BITE && is_vampire(youmonst.data)) {
	    if (bite_monster(mon))
		return 2;			/* lifesaved */
	}
	for(i = 0; ; i++) {
	    if(i >= NATTK) return(malive | mhit);	/* no passive attacks */
	    if(ptr->mattk[i].aatyp == AT_NONE /*||
	       ptr->mattk[i].aatyp == AT_BOOM*/) break; /* try this one */
	}
	/* Note: tmp not always used */
	if (ptr->mattk[i].damn)
	    tmp = d((int)ptr->mattk[i].damn, (int)ptr->mattk[i].damd);
	else if(ptr->mattk[i].damd)
	    tmp = d( ((int)mon->m_lev / 3)+1, (int)ptr->mattk[i].damd); /* Yes guys and girls, blue slimes were OP. --Amy */
	else
	    tmp = 0;


	/* Monsters with AD_RBRE are supposed to have a random passive attack every time they are hit. --Amy */

	atttypC = ptr->mattk[i].adtyp;

	if (atttypC == AD_RBRE) {
		while (atttypC == AD_ENDS ||atttypC == AD_RBRE || atttypC == AD_SPC2 || atttypC == AD_WERE) {
			atttypC = rn2(AD_ENDS); }
		/*randattack = 1;*/
	}

/*	These affect you even if they just died */
	switch(atttypC) {

	  case AD_ACID:
	    if(mhit && rn2(2)) {
		if (Blind || !flags.verbose) You("are splashed!");
		else	You("are splashed by %s acid!",
			                s_suffix(mon_nam(mon)));

		if (!Acid_resistance)
			mdamageu(mon, tmp);
		if(!rn2(30)) erode_armor(&youmonst, TRUE);
	    }
	    if (mhit) {
		if (aatyp == AT_KICK) {
		    if (uarmf && !rn2(6))
			(void)rust_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
		} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
			   aatyp == AT_MAGC || aatyp == AT_TUCH)
		    passive_obj(mon, target, &(ptr->mattk[i]));
	    }
	    exercise(A_STR, FALSE);
	    break;
	  case AD_TCKL:
		if(mhit && rn2(2)) {You("get hurt by %s spikes!", s_suffix(mon_nam(mon)));	
			mdamageu(mon, tmp);
		}
		break;
	  case AD_NGRA:

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("The engraving underneath you vanishes!");
		    del_engr(ep);
		    ep = (struct engr *)0;
		}

		break;
	  case AD_GLIB:

		/* hurt the player's hands --Amy */
		You("rip open your hands as you scrape along %s body!", s_suffix(mon_nam(mon)));
		incr_itimeout(&Glib, (tmp + 2) );

		break;
	  case AD_LUCK:

		pline("It feels like hurting this monster in melee was a bad idea.");
		change_luck(-1);
		/* Yes I know it's different from ADOM, but NetHack is way better than ADOM anyway, so who cares? --Amy */

	    break;
	  case AD_DARK:

		/* darken your surroundings --Amy */
		pline("A maleficient darkness comes over you.");
		litroomlite(FALSE);

		break;
	  case AD_TLPT:

		    if(flags.verbose)
			Your("position suddenly seems very uncertain!");
		    teleX();
		break;
	  case AD_DRIN:
	    if(!rn2(10)) {
		    if (ABASE(A_INT) <= ATTRMIN(A_INT)) {
			int lifesaved = 0;
			struct obj *wore_amulet = uamul;

			while(1) {
			    /* avoid looping on "die(y/n)?" */
			    if (lifesaved && (discover || wizard)) {
				if (wore_amulet && !uamul) {
				    /* used up AMULET_OF_LIFE_SAVING; still
				       subject to dying from brainlessness */
				    wore_amulet = 0;
				} else {
				    /* explicitly chose not to die;
				       arbitrarily boost intelligence */
				    ABASE(A_INT) = ATTRMIN(A_INT) + 2;
				    You_feel("like a scarecrow.");
				    break;
				}
			    }

			    if (lifesaved)
				pline("Unfortunately your brain is still gone.");
			    else
				Your("last thought fades away.");
			    killer = "brainlessness";
			    killer_format = KILLED_BY;
			    done(DIED);
			    lifesaved++;
			}
		    }

			if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
			else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
			forget_levels(5);	/* lose memory of 25% of levels */
			forget_objects(5);	/* lose memory of 25% of objects */
			exercise(A_WIS, FALSE);
		    } else tmp = 0;

		break;
	  case AD_STON:
	    if (mhit) {		/* successful attack */
		long protector = attk_protection((int)aatyp);
		boolean barehanded = mhit & HIT_BODY ||
			mhit & HIT_UWEP && !uwep ||
			mhit & HIT_USWAPWEP && !uswapwep;

		/* hero using monsters' AT_MAGC attack is hitting hand to
		   hand rather than casting a spell */
		if (aatyp == AT_MAGC) protector = W_ARMG;

		if (protector == 0L ||		/* no protection */
			(protector == W_ARMG && !uarmg && barehanded) ||
			(protector == W_ARMF && !uarmf) ||
			(protector == W_ARMH && !uarmh) ||
			(protector == (W_ARMC|W_ARMG) && (!uarmc || !uarmg))) {
		if (!Stone_resistance &&
			    !(poly_when_stoned(youmonst.data) &&
				polymon(PM_STONE_GOLEM))) {
			/* You("turn to stone...");
			done_in_by(mon);
			return 2; */
			if (!Stoned) Stoned = 7;

/* damn it! You should be able to do SOMEthing against those things! Insta-stoning on touch just plain sucks, okay?*/
/* One wrongly pressed key is supposed to end it all? And the travelto command rarely finds the way?*/
/* I mean, come on! Nobody has the patience to sit through eons of using the travelto command! --Amy*/
		}
	      }
	    }
	    break;
	  case AD_RUST:
	    if(mhit && !mon->mcan) {
		if (aatyp == AT_KICK) {
		    if (uarmf)
			(void)rust_dmg(uarmf, xname(uarmf), 1, TRUE, &youmonst);
		} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
			   aatyp == AT_MAGC || aatyp == AT_TUCH)
		    passive_obj(mon, target, &(ptr->mattk[i]));
	    }
	    break;
	  case AD_CORR:
	    if(mhit && !mon->mcan) {
		if (aatyp == AT_KICK) {
		    if (uarmf)
			(void)rust_dmg(uarmf, xname(uarmf), 3, TRUE, &youmonst);
		} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
			   aatyp == AT_MAGC || aatyp == AT_TUCH)
		    passive_obj(mon, target, &(ptr->mattk[i]));
	    }
	    break;
	  case AD_WTHR:
	    if(mhit && !mon->mcan) {
		if (aatyp == AT_KICK) {
		    if (uarmf)
			(void)wither_dmg(uarmf, xname(uarmf), rn2(4), TRUE, &youmonst);
		} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
			   aatyp == AT_MAGC || aatyp == AT_TUCH)
		    passive_obj(mon, target, &(ptr->mattk[i]));
	    }
	    break;
	  case AD_DCAY:
	    if(mhit && !mon->mcan) {
		if (aatyp == AT_KICK) {
		    if (uarmf)
			(void)rust_dmg(uarmf, xname(uarmf), 2, TRUE, &youmonst);
		} else if (aatyp == AT_WEAP || aatyp == AT_CLAW ||
			   aatyp == AT_MAGC || aatyp == AT_TUCH)
		    passive_obj(mon, target, &(ptr->mattk[i]));
	    }
	    break;
        case AD_CURS:
	  case AD_LITE:

			if(!rn2(10) || (night() && !rn2(3)) )  {
			    if (u.umonnum == PM_CLAY_GOLEM) {
				pline("Some writing vanishes from your head!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0;
				rehumanize();
				break;
			    }
				You("feel a dark void in your head!");
			    attrcurse();
			}
			break;
	  case AD_SLOW:
		if (HFast && !defends(AD_SLOW, uwep) && !rn2(4))
		    u_slow_down();
		break;
        case AD_DRLI:
			if (!rn2(3) && (!Drain_resistance)) {
			    losexp("life drainage", FALSE);
			}
			break;
	  case AD_DREN:
			if (!rn2(4)) drain_en(tmp);
			break;
	  case AD_LEGS:
			{ register long sideX = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
	
			pline("Your legs get grazed!");
			set_wounded_legs(sideX, rnd(60-ACURR(A_DEX)));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
			}
			break;
	  case AD_DETH:
		pline("Oh no, it uses the touch of death!");
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("Nothing seems to happen.");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "touch of death";
			done(DIED);
			tmp = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
			if (Antimagic || (Half_spell_damage)) {
				shieldeff(u.ux, u.uy);
				tmp /= 2;
			}
			u.uhpmax -= tmp/2;
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Lucky for you, it didn't work!");
		    tmp = 0;
		    break;
		}
		break;
	  case AD_FAMN:
		pline("Your body shrivels!");
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rnd(40));
		/* plus the normal damage */
		break;
	  case AD_SLIM:    
		if (!flaming(youmonst.data) && !Unchanging && youmonst.data != &mons[PM_GREEN_SLIME]) {

		 if (!Slimed) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mon->data->mname;
		} else
		    pline("Yuck!");
		}

		break;
	  case AD_CALM:	/* KMH -- koala attack */
		    docalm();
		break;
	  case AD_POLY:
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("suddenly feel very unstable!");
		    polyself(FALSE);
		}
		break;
	  case AD_MAGM:
	    /* wrath of gods for attacking Oracle */
	    if(Antimagic) {
		shieldeff(u.ux, u.uy);
		pline("A hail of magic missiles narrowly misses you!");
	    } else {
		You("are hit by magic missiles appearing from thin air!");
		mdamageu(mon, tmp);
	    }
	    break;
        case AD_DISE:
	        if (Sick_resistance) {
			You("are covered with botulism spores, but they seem to be inert.");
		} else {
			You("are covered with botulism spores!");
			make_sick(20, "bad case of botulism", TRUE, SICK_NONVOMITABLE);
		}
		break;
        case AD_PEST:
		  pline("You got hit by botox spores!");
		  (void) diseasemu(mon->data); /* plus the normal damage */
	        break;
	  default:
	    break;
	}

/*	These only affect you if they still live */

	if(malive && !mon->mcan && rn2(3)) {

	    switch(atttypC) {

	      case AD_DRST:
	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", A_STR, "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
	       }
	      break;
	      case AD_DRDX:
	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", A_DEX, "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
	       }
	      break;
	      case AD_DRCO:
	       if (!Strangled && !Breathless) {
		 pline("You inhale a cloud of spores!");
		 poisoned("spores", A_CON, "spore cloud", 30);
	       } else {
		 pline("A cloud of spores surrounds you!");
	       }
	      break;
	      case AD_STCK:
	      case AD_WRAP:
			if (malive && !u.ustuck && !sticks(youmonst.data))
				setustuck(mon);
			break;
	      case AD_SGLD:
			if (malive)
				stealgold(mon);
			break;
	      case AD_SITM:	/* for now these are the same */
	      case AD_SEDU:
	      case AD_SSEX:
		      if(  (rnd(100) > ACURR(A_CHA)) &&  malive && ( ((mon->female) && !flags.female && !rn2(5) ) || ((!mon->female) && flags.female && !rn2(15) ) || 
				((mon->female) && flags.female && !rn2(25) ) || ((!mon->female) && !flags.female && !rn2(25) ) )
	
			) 		{
			pline("You feel a tug on your backpack!");
			buf[0] = '\0';
				switch (steal(mon, buf)) {
			  case -1:
				return 2;
			  case 0:
				break;
			  default:
				if ( !tele_restrict(mon))
				    (void) rloc(mon, FALSE);
				monflee(mon, 0, FALSE, FALSE);
				return 3;
				};
			}
			break;
	      case AD_SLEE:

		    if (Sleep_resistance) {
 			    You("yawn.");
			break;
			}
		    fall_asleep(-rnd(10), TRUE);
		    You("suddenly feel very tired!");

	      break;
	    case AD_BLND:
		    if (!Blind) pline("You are blinded by a flash of light!");
		    make_blinded(Blinded+(long)tmp,FALSE);
		break;
	    case AD_DISN:
		if (!rn2(10))  {
		if (Disint_resistance) {
		    You("are mildly shaked.");
		    break;
            } else if (Invulnerable) {
                pline("You are unharmed!");
                break;
		} else if (uarms) {
		    /* destroy shield; other possessions are safe */
		    (void) destroy_arm(uarms);
		    break;
		} else if (uarmc) {
		    /* destroy cloak; other possessions are safe */
		    (void) destroy_arm(uarmc);
		    break;
		} else if (uarm) {
		    /* destroy suit */
		    (void) destroy_arm(uarm);
		    break;
#ifdef TOURIST
		} else if (uarmu) {
		    /* destroy shirt */
		    (void) destroy_arm(uarmu);
		    break;
#endif
		}
	      else if (nonliving(youmonst.data) || is_demon(youmonst.data)) {
		You("seem unaffected.");
		break;
	    } else if (Antimagic) {
		You("aren't affected.");
		break;
	    }
	    done(DIED);
	    return 0; /* lifesaved */

		}
	      break;
	      case AD_PLYS:
		if(ptr == &mons[PM_FLOATING_EYE]) {
		    if (!canseemon(mon)) {
			break;
		    }
		    if(mon->mcansee) {
			if (ureflects("%s gaze is reflected by your %s.",
				    s_suffix(Monnam(mon))))
			    ;
			else if (Free_action)
			    You("momentarily stiffen under %s gaze!",
				    s_suffix(mon_nam(mon)));
			else {
			    You("are frozen by %s gaze!",
				  s_suffix(mon_nam(mon)));
			    nomul(-tmp, "paralyzed by a monster's passive gaze");
			}
		    } else {
			pline("%s cannot defend itself.",
				Adjmonnam(mon,"blind"));
			if(!rn2(500)) change_luck(-1);
		    }
		} else if (Free_action) {
		    You("momentarily stiffen.");
		} else { /* gelatinous cube */
		    You("are frozen by %s!", mon_nam(mon));
	    	    nomovemsg = 0;	/* default: "you can move again" */
		    nomul(-tmp, "paralyzed by a monster's passive attack");
		    exercise(A_DEX, FALSE);
		}
		break;
	      case AD_COLD:		/* brown mold or blue jelly */
		if(monnear(mon, u.ux, u.uy)) {
		    if(Cold_resistance) {
			shieldeff(u.ux, u.uy);
			You_feel("a mild chill.");
			ugolemeffects(AD_COLD, tmp);
			break;
		    }
		    You("are suddenly very cold!");
		    mdamageu(mon, tmp);
		/* monster gets stronger with your heat! */
			if(malive) {
		    mon->mhp += tmp / 2;
		    if (mon->mhpmax < mon->mhp) mon->mhpmax = mon->mhp;
		/* at a certain point, the monster will reproduce! */
		    if(mon->mhpmax > ((int) (mon->m_lev+1) * 8) && !rn2(5) ) /* prevent easy farming --Amy */
			(void)split_mon(mon, &youmonst);
			}
		}
		break;
	      case AD_STUN:		/* specifically yellow mold */
		    make_stunned(HStun + (long)tmp, TRUE);
		break;
	      case AD_CONF:		
			 You("feel confused!");
		    make_confused(HConfusion + (long)tmp, TRUE);
		break;
	      case AD_HALU:		
			 You("sniff some psychoactive substances!");
		    make_hallucinated(HHallucination + (long)tmp, TRUE, 0L);
		break;
	      case AD_SCLD:
		if(!rn2(10)) hurtmarmor(mon, AD_RUST);
		/* fall through */
	      case AD_FIRE:
		if(monnear(mon, u.ux, u.uy)) {
		    if(Fire_resistance) {
			shieldeff(u.ux, u.uy);
			You_feel("mildly warm.");
			ugolemeffects(AD_FIRE, tmp);
			break;
		    }
		    You("are suddenly very hot!");
		    mdamageu(mon, tmp);
		}
		break;
	      case AD_ELEC:
		if(Shock_resistance) {
		    shieldeff(u.ux, u.uy);
		    You_feel("a mild tingle.");
		    ugolemeffects(AD_ELEC, tmp);
		    break;
		}
		You("are jolted with electricity!");
		mdamageu(mon, tmp);
		break;
	      case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		if (mhit) {
		    struct obj *obj = target;

		    if (aatyp == AT_KICK) {
			obj = uarmf;
			if (!obj) break;
		    } else if (aatyp == AT_BITE || aatyp == AT_BUTT ||
			       (aatyp >= AT_STNG && aatyp < AT_WEAP)) {
			break;		/* no object involved */
		    } else if (!obj && mhit & (HIT_UWEP | HIT_USWAPWEP))
			obj = uarmg;
		    passive_obj(mon, obj, &(ptr->mattk[i]));
	    	}
	    	break;
	      default:
		break;
	    }
	}

	/*if (randattackC == 1) {
		ptr->mattk[i].adtyp = AD_RBRE;
		randattackC = 0;
	}*/

	return(malive | mhit);
}

/*
 * Special (passive) attacks on an attacking object by monsters done here.
 * Assumes the attack was successful.
 */
void
passive_obj(mon, obj, mattk)
register struct monst *mon;
register struct obj *obj;	/* null means pick uwep, uswapwep or uarmg */
struct attack *mattk;		/* null means we find one internally */
{
	register struct permonst *ptr = mon->data;
	register int i;

#if 0
	/* if caller hasn't specified an object, use uwep, uswapwep or uarmg */
	if (!obj) {
	    obj = (u.twoweap && uswapwep && !rn2(2)) ? uswapwep : uwep;
	    if (!obj && mattk->adtyp == AD_ENCH)
		obj = uarmg;		/* no weapon? then must be gloves */
	    if (!obj) return;		/* no object to affect */
	}
#else
	/* In Slash'EM, the caller always specifies the object */
	if (!obj) return;		/* no object to affect */
#endif

	/* if caller hasn't specified an attack, find one */
	if (!mattk) {
	    for(i = 0; ; i++) {
		if(i >= NATTK) return;	/* no passive attacks */
		if(ptr->mattk[i].aatyp == AT_NONE) break; /* try this one */
	    }
	    mattk = &(ptr->mattk[i]);
	}

	switch(mattk->adtyp) {

	case AD_ACID:
	    if(!rn2(6)) {
		erode_obj(obj, TRUE, FALSE);
	    }
	    break;
	case AD_RUST:
	    if(!mon->mcan) {
		erode_obj(obj, FALSE, FALSE);
	    }
	    break;
	case AD_CORR:
	    if(!mon->mcan) {
		erode_obj(obj, TRUE, FALSE);
	    }
	    break;
	case AD_WTHR:

	    if(!mon->mcan && obj->otyp != SPE_BOOK_OF_THE_DEAD && obj->otyp != AMULET_OF_YENDOR && obj->otyp != CANDELABRUM_OF_INVOCATION && obj->otyp != BELL_OF_OPENING && obj->oartifact != ART_KEY_OF_LAW && obj->oartifact != ART_KEY_OF_CHAOS && obj->oartifact != ART_KEY_OF_NEUTRALITY ) {
		/*erode_obj(obj, TRUE, FALSE);*/

		if (rn2(2)) {
				if (obj->oeroded < MAX_ERODE && !((obj->blessed && !rnl(4)))) 
				{obj->oeroded++;
				pline("%s weapon has taken damage!", carried(obj) ? "Your" : "A");
				}
			else if (obj->oeroded == MAX_ERODE && carried(obj) )
			{
	/* Unfortunately I can't seem to make it work for non-player inventory objects without crashing. --Amy */
		    
			pline("Your weapon withered away!");
			if (obj == uball) unpunish();
			delobj(obj);
			uwepgone();
			update_inventory();
	    
			}
		}
		else {
				if (obj->oeroded2 < MAX_ERODE && !((obj->blessed && !rnl(4))))
				{obj->oeroded2++;
				pline("%s weapon has taken damage!", carried(obj) ? "Your" : "A");
				}
			else if (obj->oeroded2 == MAX_ERODE && carried(obj) )
			{
		    
			pline("Your weapon withered away!");
			if (obj == uball) unpunish();
			delobj(obj);
			uwepgone();
			update_inventory();
	    
			}
		}

	    }
	    break;
	case AD_ENCH:
	    if (!mon->mcan) {
		if (drain_item(obj) && carried(obj) &&
		    (obj->known || obj->oclass == ARMOR_CLASS)) {
		    Your("%s less effective.", aobjnam(obj, "seem"));
	    	}
	    	break;
	    }
	  default:
	    break;
	}

	if (obj && carried(obj)) update_inventory();
}

/* Note: caller must ascertain mtmp is mimicking... */
void
stumble_onto_mimic(mtmp)
struct monst *mtmp;
{
	const char *fmt = "Wait!  That's %s!",
		   *generic = "a monster",
		   *what = 0;

	if(!u.ustuck && !mtmp->mflee && dmgtype(mtmp->data,AD_STCK))
	    setustuck(mtmp);

	if (Blind) {
	    if (!Blind_telepat)
		what = generic;		/* with default fmt */
	    else if (mtmp->m_ap_type == M_AP_MONSTER)
		what = a_monnam(mtmp);	/* differs from what was sensed */
	} else {
#ifdef DISPLAY_LAYERS
	    if (levl[u.ux+u.dx][u.uy+u.dy].mem_bg == S_hcdoor ||
		    levl[u.ux+u.dx][u.uy+u.dy].mem_bg == S_vcdoor)
		fmt = "The door actually was %s!";
	    else if (levl[u.ux+u.dx][u.uy+u.dy].mem_obj == GOLD_PIECE)
		fmt = "That gold was %s!";
#else
	    int glyph = levl[u.ux+u.dx][u.uy+u.dy].glyph;

	    if (glyph_is_cmap(glyph) &&
		    (glyph_to_cmap(glyph) == S_hcdoor ||
		     glyph_to_cmap(glyph) == S_vcdoor))
		fmt = "The door actually was %s!";
	    else if (glyph_is_object(glyph) &&
		    glyph_to_obj(glyph) == GOLD_PIECE)
		fmt = "That gold was %s!";
#endif

	    /* cloned Wiz starts out mimicking some other monster and
	       might make himself invisible before being revealed */
	    if (mtmp->minvis && !See_invisible)
		what = generic;
	    else
		what = a_monnam(mtmp);
	}
	if (what) pline(fmt, what);

	wakeup(mtmp);	/* clears mimicking */
}

STATIC_OVL void
nohandglow(mon)
struct monst *mon;
{
	char *hands=makeplural(body_part(HAND));

	if (!u.umconf || mon->mconf) return;
	if (u.umconf == 1) {
		if (Blind)
			Your("%s stop tingling.", hands);
		else
			Your("%s stop glowing %s.", hands, hcolor(NH_RED));
	} else {
		if (Blind)
			pline_The("tingling in your %s lessens.", hands);
		else
			Your("%s no longer glow so brightly %s.", hands,
				hcolor(NH_RED));
	}
	u.umconf--;
}

static const char blinded_by_the_flash[] = "blinded by the flash!";

int
flash_hits_mon(mtmp, otmp)
struct monst *mtmp;
struct obj *otmp;	/* source of flash */
{
	int tmp, amt, res = 0;
	boolean isyou = (mtmp==&youmonst);
#ifdef NEPHI_PHOTOGRAPHY
	boolean useeit = canseemon(mtmp) && !isyou;
#else
	int useeit = canseemon(mtmp);
#endif

#ifndef NEPHI_PHOTOGRAPHY 
	if (mtmp->msleeping) 
#else
	if (isyou && u.usleep) {
	    multi = -1;
	    nomovemsg = "The flash awakens you.";
	    res = 1;
	} else if (!isyou && mtmp->msleeping) 
#endif
	{
	    mtmp->msleeping = 0;
	    if (useeit) {
		pline_The("flash awakens %s.", mon_nam(mtmp));
		res = 1;
	    }
	} else if (mtmp->data->mlet != S_LIGHT) {
	    if (!resists_blnd(mtmp)) {
		/* BUG: If NEPHI_PHOTOGRAPHY is defined dist2 returns
		 * an improper result, causing monsters to never penic
		 * and blinding to last only 1 turn.
		 */
		tmp = dist2(otmp->ox, otmp->oy, mtmp->mx, mtmp->my);
		if (useeit) {
		    pline("%s is %s", Monnam(mtmp),blinded_by_the_flash);
		    res = 1;
		} else if (isyou) {
		    You("are %s",blinded_by_the_flash);
		    res = 1;
		}
		if (mtmp->data == &mons[PM_GREMLIN]) {
		    /* Rule #1: Keep them out of the light. */
		    amt = otmp->otyp == WAN_LIGHT ? d(1 + otmp->spe, 4) :
		          rn2(min(mtmp->mhp,4));
#ifdef NEPHI_PHOTOGRAPHY
		    if(isyou) {
			You("writhe in agony!");
			losehp(amt, "flash of light", KILLED_BY_AN);
		    } else {
#endif
		    pline("%s %s!", Monnam(mtmp), amt > mtmp->mhp / 2 ?
			  "wails in agony" : "cries out in pain");
		    if ((mtmp->mhp -= amt) <= 0) {
			if (flags.mon_moving)
			    monkilled(mtmp, (char *)0, AD_BLND);
			else
			    killed(mtmp);
		    } else if (cansee(mtmp->mx,mtmp->my) && !canspotmon(mtmp)){
			map_invisible(mtmp->mx, mtmp->my);
		    }
		}
#ifdef NEPHI_PHOTOGRAPHY
		}
		amt = tmp==0 ? rnd(100) : rnd(1 + 50/tmp);
		if(isyou) {
		    make_blinded((long)amt, FALSE);
		    if(otmp->where==OBJ_INVENT) makeknown(otmp->otyp);
		    if (!Blind) Your(vision_clears);
		} else if (mtmp->mhp > 0) {
		    if (!flags.mon_moving && otmp->where==OBJ_INVENT) setmangry(mtmp);
#else
		if (mtmp->mhp > 0) {
		    if (!flags.mon_moving) setmangry(mtmp);
#endif
		    if (tmp < 9 && !mtmp->isshk && rn2(4)) {
			if (rn2(4))
			    monflee(mtmp, rnd(100), FALSE, TRUE);
			else
			    monflee(mtmp, 0, FALSE, TRUE);
		    }
		    mtmp->mcansee = 0;
#ifdef NEPHI_PHOTOGRAPHY
		    mtmp->mblinded = amt;
#else
		    mtmp->mblinded = (tmp < 3) ? 0 : rnd(1 + 50/tmp);
#endif
		}
	    }
	}
	return res;
}
/*uhitm.c*/
