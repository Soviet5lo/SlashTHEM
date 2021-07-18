/*	SCCS Id: @(#)mhitu.c	3.4	2003/11/26	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#include "edog.h"

STATIC_VAR NEARDATA struct obj *otmp;

STATIC_DCL void FDECL(urustm, (struct monst *, struct obj *));
# ifdef OVL1
STATIC_DCL boolean FDECL(u_slip_free, (struct monst *,struct attack *));
STATIC_DCL int FDECL(passiveum, (struct permonst *,struct monst *,struct attack *));
# endif /* OVL1 */

#ifdef OVLB
# ifdef SEDUCE
STATIC_DCL void FDECL(mayberem, (struct obj *, const char *));
# endif
#endif /* OVLB */

STATIC_DCL int FDECL(hitmu, (struct monst *,struct attack *));
STATIC_DCL int FDECL(gulpmu, (struct monst *,struct attack *));
STATIC_DCL int FDECL(explmu, (struct monst *,struct attack *,BOOLEAN_P));
STATIC_DCL void FDECL(missmu,(struct monst *,int,int,struct attack *));
STATIC_DCL void FDECL(mswings,(struct monst *,struct obj *));
STATIC_DCL void FDECL(wildmiss, (struct monst *,struct attack *));

//STATIC_DCL void FDECL(hurtarmor,(int));
STATIC_DCL void NDECL(witherarmor);
STATIC_DCL void FDECL(hitmsg,(struct monst *,struct attack *));

/* See comment in mhitm.c.  If we use this a lot it probably should be */
/* changed to a parameter to mhitu. */
static int dieroll;

#ifdef OVL1

const char *
weaphitmsg(obj,uhitm)
struct obj *obj;
boolean uhitm;
{
       /* lucern hammers and bec-de-corbins both whack and pierce */
       return ((objects[obj->otyp].oc_dir & WHACK &&
               (!(objects[obj->otyp].oc_dir & PIERCE) || rn2(2))) ?
                       ((objects[obj->otyp].oc_skill == P_CLUB ||
                       objects[obj->otyp].oc_skill == P_MACE ||
                       objects[obj->otyp].oc_skill == P_MORNING_STAR) ?
                           "club" : "whack") :
               (objects[obj->otyp].oc_dir & PIERCE &&
               (!(objects[obj->otyp].oc_dir & SLASH) || rn2(2))) ?
                       (is_blade(obj) ? "stab" : "jab") :
               (objects[obj->otyp].oc_dir & SLASH) ?
                       (uhitm && Role_if(PM_BARBARIAN) ? "smite" :
                        rn2(2) ? "hack" : is_axe(obj) ? "hew" : "slash") :
               (objects[obj->otyp].oc_skill == P_WHIP) ?
                       "whip" :
               "hit");
}

const char *
barehitmsg(mtmp)
struct monst *mtmp;
{
       if (!strcmp(mbodypart(mtmp, HAND),"claw") ||
	   !strcmp(mbodypart(mtmp, HAND),"paw") ||
               !strcmp(mbodypart(mtmp, HAND),"foreclaw") || is_bird(mtmp->data))
                return "claw";
       if (!strcmp(mbodypart(mtmp, HAND),"swirl") || /* elementals */
           !strcmp(mbodypart(mtmp, HAND),"tentacle")) { /* krakens */
               if (mtmp->data == &mons[PM_EARTH_ELEMENTAL])
                   return "pummel";
               return "lash";
       } if (is_undead(mtmp->data))
               return "scratch";
       if (mtmp->data == &mons[PM_MONK] || mtmp->data == &mons[PM_SAMURAI]
               || (martial_bonus() &&
                  (mtmp == &youmonst ||
                   /* Assumes monk or samurai quest monsters */
                  mtmp->data->msound == MS_LEADER ||
                  mtmp->data->msound == MS_GUARDIAN ||
                  mtmp->data->msound == MS_NEMESIS)))
               return "strike";
       return "punch";
}

STATIC_OVL void
hitmsg(mtmp, mattk)
register struct monst *mtmp;
register struct attack *mattk;
{
	int compat;
	int monsterlev;
	int armproX = 0;
	int randomkick;
	char buf[BUFSZ];

	/* Note: if opposite gender, "seductively" */
	/* If same gender, "engagingly" for nymph, normal msg for others */
	if((compat = could_seduce(mtmp, &youmonst, mattk)) && !mtmp->mcan &&
	    !mtmp->mspec_used) {
	        pline("%s %s you %s.", Monnam(mtmp), Blind ? "talks to" :
	              "smiles at", compat == 2 ? "engagingly" :
	              "seductively");
	} /*else*/ switch (mattk->aatyp) {
		case AT_BITE:
			pline("%s %ss!", Monnam(mtmp), has_beak(mtmp->data) ?
				"peck" : "bite");
			break;
		case AT_KICK:
			pline("%s kicks you%c", Monnam(mtmp),
				    thick_skinned(youmonst.data) ? '.' : '!');
			break;
		case AT_STNG:
			pline("%s stings you!", Monnam(mtmp));
			break;
		case AT_BUTT:
			pline("%s butts you!", Monnam(mtmp));
			break;
		case AT_SCRA:
			pline("%s scratches you!", Monnam(mtmp));
			break;
		case AT_LASH:
			pline("%s lashes you!", Monnam(mtmp));
			break;
		case AT_TRAM:
			pline("%s tramples over you!", Monnam(mtmp));
			break;
		case AT_TUCH:
			pline("%s touches you!", Monnam(mtmp));
			losehp(1, "icy touch", KILLED_BY_AN);
			break;
		case AT_TENT:
			pline("%s tentacles suck you!",
				        s_suffix(Monnam(mtmp)));
			monsterlev = ((mtmp->m_lev) + 1);
			monsterlev /= 5;
			if (monsterlev <= 0) monsterlev = 1;
			losehp((monsterlev), "sucking tentacle attack", KILLED_BY_AN);
			break;
		case AT_EXPL:
		case AT_BOOM:
			pline("%s explodes!", Monnam(mtmp));
			break;
		case AT_TALK:
			pline("%s talks to you!", Monnam(mtmp));
			break;
                case AT_WEAP:
                        if (MON_WEP(mtmp)) {
 			   Sprintf(buf, weaphitmsg(MON_WEP(mtmp),FALSE));
                            if (is_launcher(MON_WEP(mtmp)) ||
                                is_missile(MON_WEP(mtmp)) ||
                                is_ammo(MON_WEP(mtmp)) ||
                                is_pole(MON_WEP(mtmp)))
                                    Sprintf(buf,"hit");
                            pline("%s %s%s!", Monnam(mtmp),
                                makeplural(buf), !strcmp(buf,"whip") ||
 						!strcmp(buf,"hack") ||
 						!strcmp(buf,"hew") ||
 						!strcmp(buf,"jab") ? " you" : "");
                            break;
                        } /*fallthrough*/
                case AT_CLAW:
 		       Sprintf(buf, barehitmsg(mtmp));
                        pline("%s %s%s!", Monnam(mtmp), makeplural(buf),
 						!strcmp(buf,"scratch") ||
 						!strcmp(buf,"strike") ||
 						!strcmp(buf,"punch") ? "" : " you");
                        break;
		case AT_MULTIPLY:
			/* No message. */
		break;
		default:
			pline("%s hits you!", Monnam(mtmp));
	    }
}


STATIC_OVL void
missmu(mtmp, target, roll, mattk)           /* monster missed you */
register struct monst *mtmp;
register int target;
register int roll;
register struct attack *mattk;
{
	register boolean nearmiss = (target == roll);
	register struct obj *blocker = (struct obj *)0;	
		/* 3 values for blocker
		 *	No blocker:  (struct obj *) 0  
		 * 	Piece of armour:  object
		 *	magical: &zeroobj
		 */

	if (target < roll) {
		/* get object responsible 
		 * Work from the closest to the skin outwards
		 */
#ifdef TOURIST
		/* Try undershirt if tourist */
		if (uarmu && target <= roll) {
			target += ARM_BONUS(uarmu);
			if (target > roll) blocker = uarmu;
		}
#endif
		/* Try body armour */
		if (uarm && target <= roll) {
			target += ARM_BONUS(uarm);
			if (target > roll) blocker = uarm;
		}

		if (uarmg && !rn2(10)) {
			/* Try gloves */
			target += ARM_BONUS(uarmg);
			if (target > roll) blocker = uarmg;
		}
		if (uarmf && !rn2(10)) {
			/* Try boots */
			target += ARM_BONUS(uarmf);
			if (target > roll) blocker = uarmf;
		}
		if (uarmh && !rn2(5)) {
			/* Try helm */
			target += ARM_BONUS(uarmh);
			if (target > roll) blocker = uarmh;
		}
		if (uarmc && target <= roll) {
			/* Try cloak */
			target += ARM_BONUS(uarmc);
			if (target > roll) blocker = uarmc;
		}
		if (uarms && target <= roll) {
			/* Try shield */
			target += ARM_BONUS(uarms);
			if (target > roll) blocker = uarms;
		}
		if (target <= roll) {
			/* Try spell protection */
			target += u.uspellprot;
			if (target > roll) blocker = &zeroobj;
		}			
	}

	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);

	if(could_seduce(mtmp, &youmonst, mattk) && !mtmp->mcan)
	    pline("%s pretends to be friendly.", Monnam(mtmp));
	else {
	    if (!flags.verbose || !nearmiss && !blocker)
		pline("%s misses.", Monnam(mtmp));
	    else if (!blocker)
		pline("%s just misses!", Monnam(mtmp));
	    else if (blocker == &zeroobj)
		pline("%s is stopped by the golden haze.", Monnam(mtmp));
	    else
		Your("%s %s%s %s attack.", 
			simple_typename(blocker->otyp),
			rn2(2) ? "block" : "deflect",
			(blocker == uarmg || blocker == uarmf) ? "" : "s",
			s_suffix(mon_nam(mtmp)));

	    if (MON_WEP(mtmp)) {
		struct obj *obj = MON_WEP(mtmp);
		obj->owornmask &= ~W_WEP;
		if (rnd(100) < (obj->oeroded * 5 / 2)) {
		    if (obj->spe > -5) {    
			obj->spe--;
			pline("%s %s is damaged further!",
				s_suffix(Monnam(mtmp)), xname(obj));
		    } else
			pline("%s %s is badly battered!", 
				s_suffix(Monnam(mtmp)), xname(obj));
		}
	    }
	}
	stop_occupation();
}

STATIC_OVL void
mswings(mtmp, otemp)		/* monster swings obj */
register struct monst *mtmp;
register struct obj *otemp;
{
        if (!flags.verbose || Blind || !mon_visible(mtmp)) return;
	pline("%s %s %s %s.", Monnam(mtmp),
	      (objects[otemp->otyp].oc_dir & PIERCE) ? "thrusts" : "swings",
	      mhis(mtmp), singular(otemp, xname));
}

/* return how a poison attack was delivered */
const char *
mpoisons_subj(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	if (mattk->aatyp == AT_WEAP) {
	    struct obj *mwep = (mtmp == &youmonst) ? uwep : MON_WEP(mtmp);
	    /* "Foo's attack was poisoned." is pretty lame, but at least
	       it's better than "sting" when not a stinging attack... */
	    return (!mwep || !mwep->opoisoned) ? "attack" : "weapon";
	} else {
	    return (mattk->aatyp == AT_TUCH) ? "contact" :
		   (mattk->aatyp == AT_GAZE) ? "gaze" :
		   (mattk->aatyp == AT_BITE) ? "bite" : "sting";
	}
}

/* called when your intrinsic speed is taken away */
void
u_slow_down()
{
	HFast = 0L;
	if (!Fast) You("slow down.");
	   /* speed boots */
	else Your("quickness feels less natural.");
	exercise(A_DEX, FALSE);
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL void
wildmiss(mtmp, mattk)		/* monster attacked your displaced image */
	register struct monst *mtmp;
	register struct attack *mattk;
{
	int compat;

	/* no map_invisible() -- no way to tell where _this_ is coming from */

	if (!flags.verbose) return;
	if (!cansee(mtmp->mx, mtmp->my)) return;
		/* maybe it's attacking an image around the corner? */

	compat = (mattk->adtyp == AD_SEDU || mattk->adtyp == AD_SSEX) &&
		 could_seduce(mtmp, &youmonst, (struct attack *)0);

	if (!mtmp->mcansee || (Invis && !perceives(mtmp->data))) {
	    const char *swings =
		mattk->aatyp == AT_BITE ? "snaps" :
		mattk->aatyp == AT_KICK ? "kicks" :
		(mattk->aatyp == AT_STNG ||
		 mattk->aatyp == AT_BUTT ||
		 nolimbs(mtmp->data)) ? "lunges" : "swings";

	    if (compat)
		pline("%s tries to %s and misses!", Monnam(mtmp),
		    (mattk->aatyp == AT_NTCH) ? "hit the high note": /* satyr */
		    "touch you"); 
	    else
		switch(rn2(3)) {
		case 0: pline("%s %s wildly and misses!", Monnam(mtmp),
			      swings);
		    break;
		case 1: pline("%s attacks a spot beside you.", Monnam(mtmp));
		    break;
		case 2: pline("%s strikes at %s!", Monnam(mtmp),
				levl[mtmp->mux][mtmp->muy].typ == WATER
				    ? "empty water" : "thin air");
		    break;
		default:pline("%s %s wildly!", Monnam(mtmp), swings);
		    break;
		}
	} else if (Displaced) {
	    if (compat)
		pline("%s smiles %s at your %sdisplaced image...",
			Monnam(mtmp),
			compat == 2 ? "engagingly" : "seductively",
			Invis ? "invisible " : "");
	    else
		pline("%s strikes at your %sdisplaced image and misses you!",
			/* Note: if you're both invisible and displaced,
			 * only monsters which see invisible will attack your
			 * displaced image, since the displaced image is also
			 * invisible.
			 */
                        Monnam(mtmp),Invis ? "invisible " : "");

	} else if (Underwater) {
	    /* monsters may miss especially on water level where
	       bubbles shake the player here and there */
	    if (compat)
		pline("%s reaches towards your distorted image.",Monnam(mtmp));
	    else
		pline("%s is fooled by water reflections and misses!",Monnam(mtmp));

	} else impossible("%s attacks you without knowing your location?",
		Monnam(mtmp));
}

void
expels(mtmp, mdat, message)
register struct monst *mtmp;
register struct permonst *mdat; /* if mtmp is polymorphed, mdat != mtmp->data */
boolean message;
{
	if (message) {
		if (is_animal(mdat))
			You("get regurgitated!");
		else {
			char blast[40];
			register int i;

			blast[0] = '\0';
			for(i = 0; i < NATTK; i++)
				if(mdat->mattk[i].aatyp == AT_ENGL)
					break;
			if (mdat->mattk[i].aatyp != AT_ENGL)
			      impossible("Swallower has no engulfing attack?");
			else {
				if (is_whirly(mdat)) {
					switch (mdat->mattk[i].adtyp) {
						case AD_ELEC:
							Strcpy(blast,
						      " in a shower of sparks");
							break;
						case AD_COLD:
							Strcpy(blast,
							" in a blast of frost");
							break;
					}
				} else
					Strcpy(blast, " with a squelch");
                                You("get expelled from %s%s!",mon_nam(mtmp), blast);
			}
		}
	}
	unstuck(mtmp);	/* ball&chain returned in unstuck() */
	mnexto(mtmp);
	newsym(u.ux,u.uy);
	spoteffects(TRUE);
	/* to cover for a case where mtmp is not in a next square */
	if(um_dist(mtmp->mx,mtmp->my,1))
		pline("Brrooaa...  You land hard at some distance.");
}

#endif /* OVLB */
#ifdef OVL0

/* select a monster's next attack, possibly substituting for its usual one */
struct attack *
getmattk(mptr, indx, prev_result, alt_attk_buf)
struct permonst *mptr;
int indx, prev_result[];
struct attack *alt_attk_buf;
{
    struct attack *attk = &mptr->mattk[indx];

    /* prevent a monster with two consecutive disease or hunger attacks
       from hitting with both of them on the same turn; if the first has
       already hit, switch to a stun attack for the second */
    if (indx > 0 && prev_result[indx - 1] > 0 &&
	    (attk->adtyp == AD_DISE ||
		attk->adtyp == AD_PEST ||
		attk->adtyp == AD_FAMN) &&
	    attk->adtyp == mptr->mattk[indx - 1].adtyp) {
	*alt_attk_buf = *attk;
	attk = alt_attk_buf;
	attk->adtyp = AD_STUN;
    }
    return attk;
}

/* Intelligent monsters try and avoid "blue on blue" incidents.
 */
STATIC_OVL int
blue_on_blue(mtmp)
struct monst *mtmp;
{
    int x, y;
    struct monst *mon;
    if (!mtmp->mconf && !Conflict && !mtmp->mflee && !mindless(mtmp->data)) {
	if (!lined_up(mtmp))
	    return FALSE;	/* Irrelevant; monster won't attack anyway */
	x = mtmp->mx + sgn(tbx);
	y = mtmp->my + sgn(tby);
	while(x != mtmp->mux || y != mtmp->muy) {
	    mon = m_at(x, y);
	    if (mon && m_cansee(mtmp, x, y) && !mon->mundetected &&
		    (!mon->minvis || perceives(mtmp->data)))
		return TRUE;
	    x += sgn(tbx);
	    y += sgn(tby);
	}
    }
    return FALSE;
}

/*
 * mattacku: monster attacks you
 *	returns 1 if monster dies (e.g. "yellow light"), 0 otherwise
 *	Note: if you're displaced or invisible the monster might attack the
 *		wrong position...
 *	Assumption: it's attacking you or an empty square; if there's another
 *		monster which it attacks by mistake, the caller had better
 *		take care of it...
 */
int
mattacku(mtmp)
	register struct monst *mtmp;
{
	struct	attack	*mattk, alt_attk;
	int	i, j, tmp, sum[NATTK];
	struct	permonst *mdat = mtmp->data;
	boolean ranged = (distu(mtmp->mx, mtmp->my) > 3);
		/* Is it near you?  Affects your actions */
	boolean range2 = !monnear(mtmp, mtmp->mux, mtmp->muy);
		/* Does it think it's near you?  Affects its actions */
	boolean foundyou = (mtmp->mux==u.ux && mtmp->muy==u.uy);
		/* Is it attacking you or your image? */
	boolean youseeit = canseemon(mtmp);
		/* Might be attacking your image around the corner, or
		 * invisible, or you might be blind....
		 */
	
	if(!ranged) nomul(0, 0);
	if(mtmp->mhp <= 0 || (Underwater && !is_swimmer(mtmp->data)))
	    return(0);

	/* If swallowed, can only be affected by u.ustuck */
	if(u.uswallow) {
		if(mtmp != u.ustuck) return(0);
	    u.ustuck->mux = u.ux;
	    u.ustuck->muy = u.uy;
	    range2 = 0;
	    foundyou = 1;
	    if(u.uinvulnerable) return (0); /* stomachs can't hurt you! */
	}

#ifdef STEED
	else if (u.usteed) {
		if (mtmp == u.usteed)
			/* Your steed won't attack you */
			return (0);
		/* Orcs like to steal and eat horses and the like */
		if (!rn2(is_orc(mtmp->data) ? 2 : 4) &&
				distu(mtmp->mx, mtmp->my) <= 2) {
			/* Attack your steed instead */
			i = mattackm(mtmp, u.usteed);
			if ((i & MM_AGR_DIED))
				return (1);
			if (i & MM_DEF_DIED || u.umoved)
				return (0);
			/* Let your steed retaliate */
			return (!!(mattackm(u.usteed, mtmp) & MM_DEF_DIED));
		}
	}
#endif

	if (u.uundetected && !range2 && foundyou && !u.uswallow) {
		u.uundetected = 0;
		if (is_hider(youmonst.data)) {
		    coord cc; /* maybe we need a unexto() function? */
		    struct obj *obj;

		    You("fall from the %s!", ceiling(u.ux,u.uy));
		    if (enexto(&cc, u.ux, u.uy, youmonst.data)) {
			remove_monster(mtmp->mx, mtmp->my);
			newsym(mtmp->mx,mtmp->my);
			place_monster(mtmp, u.ux, u.uy);
			if(mtmp->wormno) worm_move(mtmp);
			teleds(cc.x, cc.y, TRUE);
			set_apparxy(mtmp);
			newsym(u.ux,u.uy);
		    } else {
			pline("%s is killed by a falling %s (you)!",
					Monnam(mtmp), youmonst.data->mname);
			killed(mtmp);
			newsym(u.ux,u.uy);
			if (mtmp->mhp > 0) return 0;
			else return 1;
		    }
		    if (youmonst.data->mlet != S_PIERCER)
			return(0);	/* trappers don't attack */

		    obj = which_armor(mtmp, WORN_HELMET);
		    if (obj && is_metallic(obj)) {
			Your("blow glances off %s helmet.",
			               s_suffix(mon_nam(mtmp)));
		    } else {
			if (3 + find_mac(mtmp) <= rnd(20)) {
			    pline("%s is hit by a falling piercer (you)!",
								Monnam(mtmp));
			    if ((mtmp->mhp -= d(3,6)) < 1)
				killed(mtmp);
			} else
			  pline("%s is almost hit by a falling piercer (you)!",
								Monnam(mtmp));
		    }
		} else {
		    if (!youseeit)
			pline("It tries to move where you are hiding.");
		    else {
			/* Ugly kludge for eggs.  The message is phrased so as
			 * to be directed at the monster, not the player,
			 * which makes "laid by you" wrong.  For the
			 * parallelism to work, we can't rephrase it, so we
			 * zap the "laid by you" momentarily instead.
			 */
			struct obj *obj = level.objects[u.ux][u.uy];

			if (obj ||
			      (youmonst.data->mlet == S_EEL && is_pool(u.ux, u.uy, FALSE))) {
			    int save_spe = 0; /* suppress warning */
			    if (obj) {
				save_spe = obj->spe;
				if (obj->otyp == EGG) obj->spe = 0;
			    }
			    if (youmonst.data->mlet == S_EEL)
		pline("Wait, %s!  There's a hidden %s named %s there!",
				m_monnam(mtmp), youmonst.data->mname, plname);
			    else
	     pline("Wait, %s!  There's a %s named %s hiding under %s!",
				m_monnam(mtmp), youmonst.data->mname, plname,
				doname(level.objects[u.ux][u.uy]));
			    if (obj) obj->spe = save_spe;
			} else
			    impossible("hiding under nothing?");
		    }
		    newsym(u.ux,u.uy);
		}
		return(0);
	}
	if (youmonst.data->mlet == S_MIMIC && youmonst.m_ap_type &&
		    !range2 && foundyou && !u.uswallow) {
		if (!youseeit) pline("It gets stuck on you.");
		else pline("Wait, %s!  That's a %s named %s!",
			   m_monnam(mtmp), youmonst.data->mname, plname);
		setustuck(mtmp);
		youmonst.m_ap_type = M_AP_NOTHING;
		youmonst.mappearance = 0;
		newsym(u.ux,u.uy);
		return(0);
	}

	/* player might be mimicking an object */
	if (youmonst.m_ap_type == M_AP_OBJECT && !range2 && foundyou && !u.uswallow) {
	    if (!youseeit)
		 pline("%s %s!", Something,
			(likes_gold(mtmp->data) && youmonst.mappearance == GOLD_PIECE) ?
			"tries to pick you up" : "disturbs you");
	    else pline("Wait, %s!  That %s is really %s named %s!",
			m_monnam(mtmp),
			mimic_obj_name(&youmonst),
			an(mons[u.umonnum].mname),
			plname);
	    if (multi < 0) {	/* this should always be the case */
		char buf[BUFSZ];
		Sprintf(buf, "You appear to be %s again.",
			Upolyd ? (const char *) an(youmonst.data->mname) :
			    (const char *) "yourself");
		unmul(buf);	/* immediately stop mimicking */
	    }
	    return 0;
	}

/*	Work out the armor class differential	*/
	tmp = AC_VALUE(u.uac) + 10;		/* tmp ~= 0 - 20 */
	tmp += mtmp->m_lev;
	if(multi < 0) tmp += 4;
        if((Invis && !perceives(mdat)) || !mtmp->mcansee) tmp -= 2;
	if(mtmp->mtrapped) tmp -= 2;
	if(tmp <= 0) tmp = 1;

	/* make eels visible the moment they hit/miss us */
	if(mdat->mlet == S_EEL && mtmp->minvis && cansee(mtmp->mx,mtmp->my)) {
		mtmp->minvis = 0;
		newsym(mtmp->mx,mtmp->my);
	}

	/* Make Star Vampires visible the moment they hit/miss us */
	if(mtmp->data == &mons[PM_STAR_VAMPIRE] && mtmp->minvis
	   && cansee(mtmp->mx, mtmp->my)) {
	    mtmp->minvis = 0;
	    newsym(mtmp->mx, mtmp->my);
	}

/*	Special demon handling code */
	if(!mtmp->cham && is_demon(mdat) && !range2
	   && mtmp->data != &mons[PM_BALROG]
	   && mtmp->data != &mons[PM_SUCCUBUS]
	   && mtmp->data != &mons[PM_INCUBUS]
 	   && mtmp->data != &mons[PM_NEWS_DAEMON]
 	   && mtmp->data != &mons[PM_PRINTER_DAEMON])
	    if(!mtmp->mcan && !rn2(23))	msummon(mtmp);

/*	Special lycanthrope handling code */
	if(!mtmp->cham && is_were(mdat) && !range2) {
	    if(is_human(mdat)) {
		if(!rn2(5 - (night() * 2)) && !mtmp->mcan) new_were(mtmp);
	    } else if(!rn2(30) && !mtmp->mcan) new_were(mtmp);
	    mdat = mtmp->data;

	    if(!rn2(10) && !mtmp->mcan) {
	    	int numseen, numhelp;
		char buf[BUFSZ], genericwere[BUFSZ];

		Strcpy(genericwere, "creature");
		numhelp = were_summon(mdat, FALSE, &numseen, genericwere);
		if (youseeit) {
			pline("%s summons help!", Monnam(mtmp));
			if (numhelp > 0) {
			    if (numseen == 0)
				You_feel("hemmed in.");
			} else pline("But none comes.");
		} else {
			const char *from_nowhere;

			if (flags.soundok) {
				pline("%s %s!", Something,
					makeplural(growl_sound(mtmp)));
				from_nowhere = "";
			} else from_nowhere = " from nowhere";
			if (numhelp > 0) {
			    if (numseen < 1) You_feel("hemmed in.");
			    else {
				if (numseen == 1)
			    		Sprintf(buf, "%s appears",
							an(genericwere));
			    	else
			    		Sprintf(buf, "%s appear",
							makeplural(genericwere));
				pline("%s%s!", upstart(buf), from_nowhere);
			    }
			} /* else no help came; but you didn't know it tried */
		}
	    }
	}

	if(u.uinvulnerable) {
	    /* monsters won't attack you */
	    if(mtmp == u.ustuck)
		pline("%s loosens its grip slightly.", Monnam(mtmp));
	    else if(!range2) {
		if (youseeit || sensemon(mtmp))
		    pline("%s starts to attack you, but pulls back.",
			  Monnam(mtmp));
		else
		    You_feel("%s move nearby.", something);
	    }
	    return (0);
	}

	/* Unlike defensive stuff, don't let them use item _and_ attack. */
	if(!blue_on_blue(mtmp) && find_offensive(mtmp)) {
		int foo = use_offensive(mtmp);

		if (foo != 0) return(foo==1);
	}

	for(i = 0; i < NATTK; i++) {

	    sum[i] = 0;
	    mattk = getmattk(mdat, i, sum, &alt_attk);
	        if (mdat == & mons[PM_CLOCKWORK_AUTOMATON]){
		    if ((mtmp->mspec_used < CLOCKWORK_PANIC) ||
		        (mtmp->mspec_used < CLOCKWORK_LOW && i) ||
		        (mtmp->mspec_used < CLOCKWORK_MED && i>=2) ||
		        (mtmp->mspec_used < CLOCKWORK_HIGH && i>=4) ) 
		        continue;
		    else
		        mtmp->mspec_used -= 15;
	       }
	    if (u.uswallow && (mattk->aatyp != AT_ENGL))
		continue;
	    switch(mattk->aatyp) {
use_natural:
		case AT_CLAW:	/* "hand to hand" attacks */
		case AT_KICK:
		case AT_BITE:
		case AT_LASH:
		case AT_TRAM:
		case AT_SCRA:
		case AT_STNG:
		case AT_TUCH:
		case AT_BUTT:
		case AT_TENT:
			if(!range2 && (!MON_WEP(mtmp) || mtmp->mconf || Conflict ||
					!touch_petrifies(youmonst.data))) {
			    if (foundyou) {
				if(tmp > (j = rnd(20+i))) {
				    if (mattk->aatyp != AT_KICK ||
					    !thick_skinned(youmonst.data))
					sum[i] = hitmu(mtmp, mattk);
				} else
				    missmu(mtmp, tmp, j, mattk);
			    } else wildmiss(mtmp, mattk);
			}
			break;
		case AT_NTCH:
			if(!range2){
			    if (foundyou){
				if(tmp > (j = rnd(20+1))){
				    sum[i] = hitmu(mtmp, mattk);
				} else 
				    missmu(mtmp, tmp, j, mattk);
			    } else
				wildmiss(mtmp, mattk);
		    	}
			break;
		case AT_HUGS:	/* automatic if prev two attacks succeed */
			/* Note: if displaced, prev attacks never succeeded */
		                if((!range2 && i>=2 && sum[i-1] && sum[i-2]) || mtmp == u.ustuck)
				sum[i]= hitmu(mtmp, mattk);
			break;
		case AT_GAZE:	/* can affect you either ranged or not */
			/* Medusa gaze already operated through m_respond in
			 * dochug(); don't gaze more than once per round.
			 */
			if (mdat != &mons[PM_MEDUSA])
				sum[i] = gazemu(mtmp, mattk);
			break;
		case AT_EXPL:	/* automatic hit if next to, and aimed at you */
			if(!range2) sum[i] = explmu(mtmp, mattk, foundyou);
			break;
		case AT_ENGL:
			if (mdat == &mons[PM_BANDERSNATCH] && !yeasty_food(youmonst.data)){
			    mattk->aatyp = AT_BITE;
			    mattk->adtyp = AD_PHYS;
			    mattk->damn  = 1;
			    mattk->damd  = 5;
			    goto use_natural; 
			    break;
			}
			if (!range2) {
			    if(foundyou) {
				if(u.uswallow || tmp > (j = rnd(20+i))) {
				    /* Force swallowing monster to be
				     * displayed even when player is
				     * moving away */
				    flush_screen(1);
				    sum[i] = gulpmu(mtmp, mattk);
				} else {
		                                missmu(mtmp, tmp, j, mattk);
				}
			    } else if (is_animal(mtmp->data)) {
				pline("%s gulps some air!", Monnam(mtmp));
			    } else {
				if (youseeit)
				    pline("%s lunges forward and recoils!",
					  Monnam(mtmp));
				else
				    You_hear("a %s nearby.",
					     is_whirly(mtmp->data) ?
						"rushing noise" : "splat");
			   }
			}
			break;
		case AT_BREA:
			if (range2 && !blue_on_blue(mtmp))
			    sum[i] = breamu(mtmp, mattk);
			/* Note: breamu takes care of displacement */
			break;
		case AT_SPIT:
			if (range2 && !blue_on_blue(mtmp))
			    sum[i] = spitmu(mtmp, mattk);
			/* Note: spitmu takes care of displacement */
			break;
		case AT_MULTIPLY:
			/*
			 * Monster multiplying is an AT_ for the following
			 * reasons:
			 *   1. Monsters will only multiply when they're close
			 *      to you.  The whole level will not become clogged
			 *      up with giant lice from monsters multiplying
			 *      where you can't see them.
			 *   2. Tame monsters won't multiply.  Too bad! (unless
			 *      they are conflicted or confused from hunger.
			 *      A bit of a "tactic" -- but then you'll have to
			 *      let them bite you, and anyway who really wants
			 *      a dozen pet fleas to feed?)
			 *   3. Monsters have to be next to you to multiply.
			 *      This makes the inevitable altar abuse a little
			 *      harder.
			 *   4. Elbereth will stop monsters multiplying.
			 *      Otherwise a ring of conflict would crowd out a
			 *      whole level in no time.
			 *   5. It is a hack.  (Shrug)
			 *
			 * Multiplying monsters must be low-level and
			 * low-frequency, so as to minimise altar/experience
			 * abuse.  Any multiplying monsters above about
			 * level 5 should be G_NOCORPSE.
			 *
			 * RJ
			 */
			if (!range2 && (!rn2(5)) ) /* greatly reduced chance --Amy */
			    clone_mon(mtmp, 0, 0);
			break;
		case AT_TALK:
			if (!ranged) {
				pline("%s talks to you!", youseeit ? Monnam(mtmp) : "It");
				sum[i] = hitmu(mtmp,mattk);
			}
			break;
		case AT_WEAP:
			if(range2 || mtmp->data == &mons[PM_POLTERGEIST]) {
#ifdef REINCARNATION
				if (!Is_rogue_level(&u.uz))
#endif
				    if (!blue_on_blue(mtmp))
					thrwmu(mtmp);
			} else {
			    int hittmp = 0;

			    /* Rare but not impossible.  Normally the monster
			     * wields when 2 spaces away, but it can be
			     * teleported or whatever....
			     */
			     if (mtmp->weapon_check == NEED_WEAPON || !MON_WEP(mtmp)) {
				mtmp->weapon_check = NEED_HTH_WEAPON;
				/* mon_wield_item resets weapon_check as
				 * appropriate */
				if (mon_wield_item(mtmp) != 0) break;
			    }
			    if (foundyou) {
				otmp = MON_WEP(mtmp);
				if (otmp) {
				    hittmp = hitval(otmp, &youmonst);
				    tmp += hittmp;
				    mswings(mtmp, otmp);
				}
				if(tmp > (j = dieroll = rnd(20+i)))
				    sum[i] = hitmu(mtmp, mattk);
				else
				    missmu(mtmp, tmp , j, mattk);
				/* KMH -- Don't accumulate to-hit bonuses */
				if (otmp)
					tmp -= hittmp;
			     } else wildmiss(mtmp, mattk);
			}
			break;
		case AT_MAGC:
			if (!rn2(4)) /* yeah they need to be toned down a lot */{

			if (range2) {
			    if (!blue_on_blue(mtmp))
				sum[i] = buzzmu(mtmp, mattk);
			} else {
			    if (foundyou)
				sum[i] = castmu(mtmp, mattk, TRUE, TRUE);
			    else
				sum[i] = castmu(mtmp, mattk, TRUE, FALSE);
			}

			}
			break;

		default:		/* no attack */
			break;
	    }
	    if(flags.botl) bot();
	/* give player a chance of waking up before dying -kaa */
	    if(sum[i] == 1) {	    /* successful attack */
		if (u.usleep && u.usleep < monstermoves && !rn2(10)) {
		    multi = -1;
		    nomovemsg = "The combat suddenly awakens you.";
		}
	    }
	    if(sum[i] == 2) return 1;		/* attacker dead */
	    if(sum[i] == 3) break;  /* attacker teleported, no more attacks */
	    /* sum[i] == 0: unsuccessful attack */
	}
	return(0);
}

#endif /* OVL0 */
#ifdef OVLB

/*
 * helper function for some compilers that have trouble with hitmu
 */

void
hurtarmor(attk)
int attk;
{
	int	hurt;

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
	 *
	 * WAC fixed code so that it keeps looping until it either hits
	 * your body or finds a rustable item
	 * changed the last parm of !rust_dmg for non-body targets to FALSE
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !rust_dmg(uarmh, xname(uarmh), hurt, FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)rust_dmg(uarmc, xname(uarmc), hurt, TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)rust_dmg(uarm, xname(uarm), hurt, TRUE, &youmonst);
#ifdef TOURIST
		else if (uarmu)
		    (void)rust_dmg(uarmu, xname(uarmu), hurt, TRUE, &youmonst);
#endif
		break;
	    case 2:
		if (!uarms || !rust_dmg(uarms, xname(uarms), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !rust_dmg(uarmg, xname(uarmg), hurt, FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !rust_dmg(uarmf, xname(uarmf), hurt, FALSE, &youmonst))
		    continue;
		break;
	    }
	    
	    break; /* Out of while loop */
	}
}

STATIC_OVL void
witherarmor()
{

	/* What the following code does: it keeps looping until it
	 * finds a target for the rust monster.
	 * Head, feet, etc... not covered by metal, or covered by
	 * rusty metal, are not targets.  However, your body always
	 * is, no matter what covers it.
	 *
	 * WAC fixed code so that it keeps looping until it either hits
	 * your body or finds a rustable item
	 * changed the last parm of !rust_dmg for non-body targets to FALSE
	 */
	while (1) {
	    switch(rn2(5)) {
	    case 0:
		if (!uarmh || !wither_dmg(uarmh, xname(uarmh), rn2(4), FALSE, &youmonst))
			continue;
		break;
	    case 1:
		if (uarmc) {
		    (void)wither_dmg(uarmc, xname(uarmc), rn2(4), TRUE, &youmonst);
		    break;
		}
		/* Note the difference between break and continue;
		 * break means it was hit and didn't rust; continue
		 * means it wasn't a target and though it didn't rust
		 * something else did.
		 */
		if (uarm)
		    (void)wither_dmg(uarm, xname(uarm), rn2(4), TRUE, &youmonst);
#ifdef TOURIST
		else if (uarmu)
		    (void)wither_dmg(uarmu, xname(uarmu), rn2(4), TRUE, &youmonst);
#endif
		break;
	    case 2:
		if (!uarms || !wither_dmg(uarms, xname(uarms), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 3:
		if (!uarmg || !wither_dmg(uarmg, xname(uarmg), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    case 4:
		if (!uarmf || !wither_dmg(uarmf, xname(uarmf), rn2(4), FALSE, &youmonst))
		    continue;
		break;
	    }
	    
	    break; /* Out of while loop */
	}
}

#endif /* OVLB */
#ifdef OVL1

boolean
diseasemu(mdat)
struct permonst *mdat;
{
	if (Sick_resistance) {
		You_feel("a slight illness.");
		return FALSE;
	} else {
		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			mdat->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}

STATIC_OVL boolean
digeasemu(mtmp)
struct permonst *mtmp;
{
	if (Sick_resistance) {
		You_feel("a slight illness.");
		return FALSE;
	} else {
		make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON), 40),
			mtmp->mname, TRUE, SICK_NONVOMITABLE);
		return TRUE;
	}
}

/* check whether slippery clothing protects from hug or wrap attack */
STATIC_OVL boolean
u_slip_free(mtmp, mattk)
struct monst *mtmp;
struct attack *mattk;
{
	struct obj *obj = (uarmc ? uarmc : uarm);

#ifdef TOURIST
	if (!obj) obj = uarmu;
#endif
	if (mattk->adtyp == AD_DRIN) obj = uarmh;

	/* if your cloak/armor is greased, monster slips off; this
	   protection might fail (33% chance) when the armor is cursed */
	if (obj && (obj->greased || obj->otyp == OILSKIN_CLOAK) &&
		(!obj->cursed || rn2(3))) {
	    pline("%s %s your %s %s!",
		  Monnam(mtmp),
		  (mattk->adtyp == AD_WRAP) ?
			"slips off of" : "grabs you, but cannot hold onto",
		  obj->greased ? "greased" : "slippery",
		  /* avoid "slippery slippery cloak"
		     for undiscovered oilskin cloak */
		  (obj->greased || objects[obj->otyp].oc_name_known) ?
			xname(obj) : cloak_simple_name(obj));

	    if (obj->greased && !rn2(2)) {
		pline_The("grease wears off.");
		obj->greased = 0;
		update_inventory();
	    }
	    return TRUE;
	/* 50% chance (with a luck bonus) of slipping free with free action */
	} else if (Free_action && (rnl(10) < 5)) {
                pline("%s %s you, but you quickly free yourself!",
                       Monnam(mtmp),
                       (mattk->adtyp == AD_WRAP) ?
                      "swings itself around of" : "grabs");
                return TRUE;
	}
	return FALSE;
}

/* armor that sufficiently covers the body might be able to block magic */
int
magic_negation(mon)
struct monst *mon;
{
	struct obj *armor;
	int armpro = 0;

	armor = (mon == &youmonst) ? uarm : which_armor(mon, W_ARM);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmc : which_armor(mon, W_ARMC);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmh : which_armor(mon, W_ARMH);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

	/* armor types for shirt, gloves, shoes, and shield don't currently
	   provide any magic cancellation but we might as well be complete */
#ifdef TOURIST
	armor = (mon == &youmonst) ? uarmu : which_armor(mon, W_ARMU);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif
	armor = (mon == &youmonst) ? uarmg : which_armor(mon, W_ARMG);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmf : which_armor(mon, W_ARMF);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarms : which_armor(mon, W_ARMS);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

#ifdef STEED
	/* this one is really a stretch... */
	armor = (mon == &youmonst) ? 0 : which_armor(mon, W_SADDLE);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif

	return armpro;
}

/*
 * hitmu: monster hits you
 *	  returns 2 if monster dies (e.g. "yellow light"), 1 otherwise
 *	  3 if the monster lives but teleported/paralyzed, so it can't keep
 *	       attacking you
 */
STATIC_OVL int
hitmu(mtmp, mattk)
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	register struct permonst *mdat = mtmp->data;
	register int tmp = d((int)mattk->damn, (int)mattk->damd*10);
	register boolean not_affected = defends((int)mattk->adtyp, uwep);
	register int uncancelled, ptmp;
	register struct engr *ep = engr_at(u.ux,u.uy);
	int dmg, armpro, permdmg;
	char	 buf[BUFSZ];
	struct permonst *olduasmon = youmonst.data;
	int res;
	boolean burnmsg = FALSE;
	boolean shockmsg = FALSE;

	/*int randattack = 0;*/
	uchar atttyp;

	if (!canspotmon(mtmp))
	    map_invisible(mtmp->mx, mtmp->my);

/*	If the monster is undetected & hits you, you should know where
 *	the attack came from.
 */
	if(mtmp->mundetected && (hides_under(mdat) || mdat->mlet == S_EEL
				|| mdat == &mons[PM_LABYRINTH_TRAPPER])) {
	    mtmp->mundetected = 0;
	    if (!(Blind ? Blind_telepat : Unblind_telepat)) {
		struct obj *obj;
		const char *what;

		if(mdat == &mons[PM_LABYRINTH_TRAPPER]) 
		    pline("%s came out the wall!", Amonnam(mtmp));
		else if ((obj = level.objects[mtmp->mx][mtmp->my]) != 0) {
		    if (Blind && !obj->dknown)
			what = something;
		    else if (is_pool(mtmp->mx, mtmp->my, TRUE) && !Underwater)
			what = "the water";
		    else
			what = doname(obj);

		    pline("%s was hidden under %s!", Amonnam(mtmp), what);
		}
		newsym(mtmp->mx, mtmp->my);
	    }
	}

/*	First determine the base damage done */
	dmg = d((int)mattk->damn, (int)mattk->damd);
	if(is_undead(mdat) && midnight())
		dmg += d((int)mattk->damn, (int)mattk->damd); /* extra damage */
/*	Next a cancellation factor	*/

/*	Use uncancelled when the cancellation factor takes into account certain
 *	armor's special magic protection.  Otherwise just use !mtmp->mcan.
 */
	armpro = magic_negation(&youmonst);
	uncancelled = !mtmp->mcan && ((rn2(3) >= armpro) || !rn2(50));

	permdmg = 0;
/*	Now, adjust damages via resistances or specific attacks */

	/* Monsters with AD_RBRE will choose a random attack instead. --Amy */

	atttyp = mattk->adtyp;

	if (atttyp == AD_RBRE) {
		while (atttyp == AD_ENDS ||atttyp == AD_RBRE || atttyp == AD_SPC2 || atttyp == AD_WERE) {
			atttyp = rn2(AD_ENDS); }
		/*randattack = 1;*/
	}

	switch(atttyp) {
	    case AD_PHYS:
		if (mattk->aatyp == AT_HUGS && !sticks(youmonst.data)) {
		    if(!u.ustuck && rn2(2)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else {
			    setustuck(mtmp);
			    pline("%s grabs you!", Monnam(mtmp));
			}
		    } else if(u.ustuck == mtmp) {
			exercise(A_STR, FALSE);
			if (mtmp->data == &mons[PM_ROPE_GOLEM] && Breathless) {
			    You("are being strangled.");
			    dmg = (dmg+1) / 2;
			} else
			    You("are being %s.",
				    (mtmp->data == &mons[PM_ROPE_GOLEM])
				    ? "choked" : "crushed");
		    }
		} else {			  /* hand to hand weapon */
		    if(mattk->aatyp == AT_WEAP && otmp) {
			int nopoison = (10 - (otmp->owt/10));
			if (otmp->otyp == CORPSE &&
				touch_petrifies(&mons[otmp->corpsenm])) {
			    dmg = 1;
			    pline("%s hits you with the %s corpse.",
				Monnam(mtmp), mons[otmp->corpsenm].mname);
			    if (!Stoned) goto do_stone;
			}

			/* MRKR: If hit with a burning torch,     */
			/*       then do an extra point of damage */
			/*       but save the message till after  */
			/*       the hitmsg()                     */

			if (otmp->otyp == TORCH && otmp->lamplit &&
			    !Fire_resistance) {
			  burnmsg = TRUE;
			  dmg++;
			}

			/* 5lo: Electric swords have a 10% chance of
			 * "sparking" to life */

			if (otmp->otyp == ELECTRIC_SWORD && !otmp->oartifact &&
			    !Shock_resistance && !rn2(10)) {
			  shockmsg = TRUE;
			  dmg++;
			}

			/* WAC -- Real weapon?
			 * Could be stuck with a cursed bow/polearm it wielded
			 */
			if (/* if you strike with a bow... */
				is_launcher(otmp) ||
				/* or strike with a missile in your hand... */
				(is_missile(otmp) || is_ammo(otmp)) ||
#ifdef LIGHTSABERS
				/* lightsaber that isn't lit ;) */
				(is_lightsaber(otmp) && !otmp->lamplit) ||
#endif
				/* WAC -- or using a pole at short range... */
				(is_pole(otmp))) {
			    /* then do only 1-2 points of damage */
			    if (u.umonnum == PM_SHADE && otmp->otyp != SILVER_ARROW)
				dmg = 0;
			    else
				dmg = rnd(2);

#if 0 /* Monsters don't wield boomerangs */
			    if (otmp->otyp == BOOMERANG /* && !rnl(3) */) {
				pline("As %s hits you, %s breaks into splinters.",
				      mon_nam(mtmp), the(xname(otmp)));
				useup(otmp);
				otmp = (struct obj *) 0;
				possibly_unwield(mtmp);
				if (u.umonnum != PM_SHADE)
				    dmg++;
			    }
#endif
			} else dmg += dmgval(otmp, &youmonst);

			if (objects[otmp->otyp].oc_material == SILVER &&
				hates_silver(youmonst.data)) {
			    pline("The silver sears your flesh!");
			}
			/* Stakes do extra dmg agains vamps */
			if (otmp->otyp == WOODEN_STAKE &&
				is_vampire(youmonst.data)) {
			    if (otmp->oartifact == ART_STAKE_OF_VAN_HELSING) {
				if (!rn2(10)) {
				    pline("%s plunges the stake into your heart.",
					    Monnam(mtmp));
				    killer = "a wooden stake in the heart.";
				    killer_format = KILLED_BY_AN;
				    u.ugrave_arise = NON_PM; /* No corpse */
				    done(DIED);
				} else {
				    pline("%s drives the stake into you.",
					    Monnam(mtmp));
				    dmg += rnd(6) + 2;
				}
			    } else {
				pline("%s drives the stake into you.",
					Monnam(mtmp));
				dmg += rnd(6);
			    }
			}

			if (otmp->opoisoned) {
			    poisoned(simple_typename(otmp->otyp), A_STR,
				    killer_xname(otmp), 10);
			    if (nopoison < 2) nopoison = 2;
			    if (!rn2(nopoison)) {
				otmp->opoisoned = FALSE;
				pline("%s %s no longer poisoned.",
				       s_suffix(Monnam(mtmp)),
				       aobjnam(otmp, "are"));
			    }
			}
			if (dmg <= 0) dmg = 1;
			if (!otmp->oartifact || !artifact_hit(mtmp, &youmonst,
				otmp, &dmg, dieroll))
			     hitmsg(mtmp, mattk);

			if (burnmsg) {
			  boolean plural = (Blind ? FALSE : otmp->quan > 1L);
			  boolean water = (youmonst.data ==
					   &mons[PM_WATER_ELEMENTAL]);

			  pline("%s %s%s %syou!",
				(Blind ? "It" : Yname2(otmp)),
				(water ? "vaporize" : "burn"),
				(plural ? "" : "s"),
				(water ? "part of " : ""));

			  if (!rn2(2) && burnarmor(&youmonst)) {
			    dmg++;

			    /* Torch flame is not hot enough to guarantee */
			    /* burning away slime */

			    if (!rn2(4)) burn_away_slime();
			    if (!rn2(33))
			      (void)destroy_item(POTION_CLASS, AD_FIRE);
			    if (!rn2(33))
			      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
			    if (!rn2(50))
			      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
			  }
			  burn_faster(otmp, 1);
			}

			/* 5lo: Occasional "buzz" from Electric Swords */
			if (shockmsg) {
			  pline("%s buzzes you!",
				(Blind ? "It" : Yname2(otmp)));
			    dmg += rnd(3);
			    if (!rn2(25))
			      (void)destroy_item(WAND_CLASS, AD_ELEC);
			    if (!rn2(25))
			      (void)destroy_item(RING_CLASS, AD_ELEC);
			  }

			if (!dmg) break;
			if (u.mh > 1 && u.mh > ((u.uac>0) ? dmg : dmg+u.uac) &&
				   objects[otmp->otyp].oc_material == IRON &&
					(u.umonnum==PM_BLACK_PUDDING
					|| u.umonnum==PM_BROWN_PUDDING)) {
			    /* This redundancy necessary because you have to
			     * take the damage _before_ being cloned.
			     */
			    if (u.uac < 0) dmg += u.uac;
			    if (dmg < 1) dmg = 1;
			    if (dmg > 1) exercise(A_STR, FALSE);
			    u.mh -= dmg;
			    flags.botl = 1;
			    dmg = 0;
			    if (!rn2(50)) { if(cloneu()) /* greatly reduce ability to farm puddings --Amy */
			    You("divide as %s hits you!",mon_nam(mtmp));
				}
			}
			urustm(mtmp, otmp);
		    } else if (mattk->aatyp != AT_TUCH || dmg != 0 ||
			    mtmp != u.ustuck)
			hitmsg(mtmp, mattk);
		}
		break;
	    case AD_DISE:
		hitmsg(mtmp, mattk);
		if (rn2(3)) break;
                if (!diseasemu(mdat) || Invulnerable) dmg = 0;
		break;
	    case AD_NGRA:
		hitmsg(mtmp, mattk);

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("The engraving underneath you vanishes!");
		    del_engr(ep);
		    ep = (struct engr *)0;
		}

		break;
	    case AD_GLIB:
		hitmsg(mtmp, mattk);

		/* hurt the player's hands --Amy */
		pline("Your hands got hit hard!");
		incr_itimeout(&Glib, dmg);

		break;
	    case AD_DARK:
		hitmsg(mtmp, mattk);

		/* create darkness around the player --Amy */
		pline("That felt evil and sinister!");
			litroomlite(FALSE);
		break;
	    case AD_FIRE:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're %s!", on_fire(youmonst.data, mattk));
		    if (youmonst.data == &mons[PM_STRAW_GOLEM] ||
		        youmonst.data == &mons[PM_PAPER_GOLEM]) {
			    You("roast!");
			    /* KMH -- this is okay with unchanging */
			    rehumanize();
			    break;
		    } else if (Fire_resistance) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
                        } else if (u.umonnum == PM_STRAW_GOLEM ||
				   u.umonnum == PM_PAPER_GOLEM ||
				   u.umonnum == PM_WAX_GOLEM) {
				/* This code ASSUMES that you are polymorphed 
				 * Code will need to be changed if we ever implement
				 * Golems as a class.
				 */
				You("burn up!");
				u.uhp -= mons[u.umonnum].mlevel;
				u.uhpmax -= mons[u.umonnum].mlevel;
				if (u.uhpmax < 1) u.uhpmax = 1;
				/* KMH, balance patch -- this is okay with unchanging */
				u.mh = 0; /* Kill monster form */
				rehumanize();
				break;
		    }
/*		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(SCROLL_CLASS, AD_FIRE);
		    if((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_FIRE);
		    if((int) mtmp->m_lev > rn2(25))
			destroy_item(SPBOOK_CLASS, AD_FIRE);*/
		    if (!rn2(33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);
		    burn_away_slime();
		} else dmg = 0;
		break;
	    case AD_COLD:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    pline("You're covered in frost!");
		    if (Cold_resistance) {
			pline_The("frost doesn't seem cold!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		} else dmg = 0;
		break;
	    case AD_ELEC:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    You("get zapped!");
		    if (Shock_resistance) {
			pline_The("zap doesn't shock you!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		} else dmg = 0;
		break;
	    case AD_SLEE:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(5)) {
		    if (is_educator(mtmp) && rn2(5)) break;
		    if (Sleep_resistance) break;
		    fall_asleep(-rnd(10), TRUE);
		    if (Blind) You("are put to sleep!");
		    else You("are put to sleep by %s!", mon_nam(mtmp));
		}
		break;
	    case AD_BLND:
		hitmsg(mtmp, mattk);
		if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0)) {
		    if (!Blind) pline("%s blinds you!", Monnam(mtmp));
		    make_blinded(Blinded+(long)dmg,FALSE);
		    if (!Blind) Your(vision_clears);
		}
		dmg = 0;
		break;
	    case AD_DRST:
		ptmp = A_STR;
		goto dopois;
	    case AD_DRDX:
		ptmp = A_DEX;
		goto dopois;
	    case AD_DRCO:
		ptmp = A_CON;
dopois:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(8)) {
		    Sprintf(buf, "%s %s",
			    s_suffix(Monnam(mtmp)), mpoisons_subj(mtmp, mattk));
		    poisoned(buf, ptmp, mdat->mname, 30);
		}
		break;
	    case AD_DRIN:
		hitmsg(mtmp, mattk);
		if (defends(AD_DRIN, uwep) || !has_head(youmonst.data)) {
		    You("don't seem harmed.");
		    /* Not clear what to do for green slimes */
		    break;
		}
		if (u_slip_free(mtmp,mattk)) break;

		if (uarmh && rn2(8)) {
		    /* not body_part(HEAD) */
		    Your("helmet blocks the attack to your head.");
		    break;
		}
		
                /* conflicted dog, perhaps? */
		if (mtmp->mtame && !mtmp->isminion) {
		    EDOG(mtmp)->hungrytime += rnd(60);
		    mtmp->mconf = 0;
		}

		if (Half_physical_damage) dmg = (dmg+1) / 2;
		mdamageu(mtmp, dmg);

		if (!uarmh || uarmh->otyp != DUNCE_CAP) {
		    Your("brain is eaten!");
		    /* No such thing as mindless players... */
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
		}
		/* adjattrib gives dunce cap message when appropriate */
		if (!rn2(10)) (void) adjattrib(A_INT, -rnd(2), FALSE);
		else if (!rn2(2)) (void) adjattrib(A_INT, -1, FALSE);
		if (!Keen_memory) {
		    forget_levels(5);	/* lose memory of 25% of levels */
		    forget_objects(5);	/* lose memory of 25% of objects */
		}
		exercise(A_WIS, FALSE);
		break;
	    case AD_PLYS:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action) {
			You("momentarily stiffen.");            
		    } else {
			if (Blind) You("are frozen!");
			else You("are frozen by %s!", mon_nam(mtmp));
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10), "paralyzed by a monster attack");
			exercise(A_DEX, FALSE);
		    }
		}
		break;
	    case AD_TCKL:
		hitmsg(mtmp, mattk);
		if (uncancelled && multi >= 0 && !rn2(3)) {
		    if (Free_action)
			You_feel("horrible tentacles probing your flesh!");
		    else {
			if (Blind) You("are mercilessly tickled!");
			else You("are mercilessly tickled by %s!", mon_nam(mtmp));
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10), "tickled by a monster");
			exercise(A_DEX, FALSE);
			exercise(A_CON, FALSE);
		    }
		}
		break;
	    case AD_DRLI:
drain_life:
		hitmsg(mtmp, mattk);
		/* if vampire biting (and also a pet) */
		if (is_vampire(mtmp->data) && mattk->aatyp == AT_BITE &&
			has_blood(youmonst.data)) {
			   Your("blood is being drained!");
			   /* Get 1/20th of full corpse value
			    * Therefore 4 bites == 1 drink
			    */
			    if (mtmp->mtame && !mtmp->isminion)
			    	EDOG(mtmp)->hungrytime += ((int)((youmonst.data)->cnutrit / 20) + 1);
		}
		
		if (uncancelled && !rn2(3) && !Drain_resistance) {
		    losexp("life drainage", FALSE);
		}
		break;
	    case AD_LEGS:
		{ register long side = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
		  const char *sidestr = (side == RIGHT_SIDE) ? "right" : "left";

		/* This case is too obvious to ignore, but Nethack is not in
		 * general very good at considering height--most short monsters
		 * still _can_ attack you when you're flying or mounted.
		 * [FIXME: why can't a flying attacker overcome this?]
		 */
		  if (
#ifdef STEED
			u.usteed ||
#endif
				    Levitation || Flying) {
		    pline("%s tries to reach your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    dmg = 0;
		  } else if (mtmp->mcan) {
		    pline("%s nuzzles against your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    dmg = 0;
		  } else {
		    if (uarmf) {
			if (rn2(2) && (uarmf->otyp == LOW_BOOTS ||
					     uarmf->otyp == IRON_SHOES))
			    pline("%s scratches the exposed part of your %s %s!",
				Monnam(mtmp), sidestr, body_part(LEG));
			else if (!rn2(5))
			    pline("%s scratches through your %s boot!",
				Monnam(mtmp), sidestr);
			else {
			    pline("%s scratches your %s boot!", Monnam(mtmp),
				sidestr);
			    dmg = 0;
			    break;
			}
		    } else pline("%s scratches your %s %s!", Monnam(mtmp),
			  sidestr, body_part(LEG));
		    set_wounded_legs(side, rnd(60-ACURR(A_DEX)));
		    exercise(A_STR, FALSE);
		    exercise(A_DEX, FALSE);
		  }
		  break;
		}
	    case AD_STON:	/* cockatrice */
		hitmsg(mtmp, mattk);
		if(!rn2(3)) {
		    if (mtmp->mcan) {
			if (flags.soundok)
			    You_hear("a cough from %s!", mon_nam(mtmp));
		    } else {
			if (flags.soundok)
			    You_hear("%s hissing!", s_suffix(mon_nam(mtmp)));
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
 do_stone:
			    if (!Stoned && !Stone_resistance
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				Stoned = 7;
				delayed_killer = mtmp->data->mname;
				if (mtmp->data->geno & G_UNIQ) {
				    if (!type_is_pname(mtmp->data)) {
					static char kbuf[BUFSZ];

					/* "the" buffer may be reallocated */
					Strcpy(kbuf, the(delayed_killer));
					delayed_killer = kbuf;
				    }
				    killer_format = KILLED_BY;
				} else killer_format = KILLED_BY_AN;
				return(1);
				/* You("turn to stone..."); */
				/* done_in_by(mtmp); */
			    }
			}
		    }
		}
		break;
	    case AD_STCK:
		hitmsg(mtmp, mattk);
		if (uncancelled && !u.ustuck && !sticks(youmonst.data))
			setustuck(mtmp);
		break;
	    case AD_WRAP:
		if ((!mtmp->mcan || u.ustuck == mtmp) && !sticks(youmonst.data)) {
		    if (!u.ustuck && !rn2(10)) {
			if (u_slip_free(mtmp, mattk)) {
			    dmg = 0;
			} else {
			    pline("%s swings itself around you!",
				  Monnam(mtmp));
			    setustuck(mtmp);
			}
		    } else if(u.ustuck == mtmp) {

			if (is_pool(mtmp->mx,mtmp->my, FALSE)) {

		/* Being drowned should ALWAYS involve your stuff getting wet. 
		 * Even if you're unbreathing, swimming or whatever. Your stuff isn't immune just because you are.  --Amy	*/

			    pline("%s pulls you into the water!", Monnam(mtmp));
				water_damage(invent, FALSE, FALSE);
				if (level.flags.lethe) lethe_damage(invent, FALSE, FALSE);

			}

			if (is_lava(mtmp->mx,mtmp->my)) {

		/* drowning in lava  --Amy	*/
		/* This can only be done by monsters that can go on lava (flying couatls for example) and have wrap attacks */

			    pline("%s pulls you into the lava!", Monnam(mtmp));

				if (Slimed) {        
			      pline("The slime that covers you is burned away!");
			      Slimed = 0;
				}

			    destroy_item(SCROLL_CLASS, AD_FIRE);
			    destroy_item(SPBOOK_CLASS, AD_FIRE);
			    destroy_item(POTION_CLASS, AD_FIRE);
				burnarmor(&youmonst);

			}

			if (is_lava(mtmp->mx,mtmp->my) && !rn2(3)) {

				if (!Fire_resistance) {
					You("burn to ashes...");
					killer_format = KILLED_BY_AN;
					    Sprintf(buf, "pool of lava by %s", an(mtmp->data->mname));
					    killer = buf;
					done(BURNING);

				}
				else {
					You("scream "); verbalize("HAHAHAHAHAHAHAAAAAAAA!"); /* Super Mario 64 */
					u.uhpmax -= rnd(10);
				    losehp(10, "lava dive", KILLED_BY_AN);
				}

			}

			if (is_pool(mtmp->mx,mtmp->my, FALSE) && !Swimming
			    && !Amphibious && !rn2(3) ) { /* greatly lowered chance of getting drowned --Amy */
			    boolean moat =
				(levl[mtmp->mx][mtmp->my].typ != POOL) &&
				(levl[mtmp->mx][mtmp->my].typ != WATER) &&
				!Is_medusa_level(&u.uz) &&
				!Is_waterlevel(&u.uz);

			    pline("%s drowns you...", Monnam(mtmp));
			    killer_format = KILLED_BY_AN;
			    Sprintf(buf, "%s by %s",
				    moat ? "moat" : "pool of water",
				    an(mtmp->data->mname));
			    killer = buf;
			    done(DROWNING);
			} /*else*/ if(mattk->aatyp == AT_HUGS)
			    You("are being crushed.");
		    } else {
			dmg = 0;
			if(flags.verbose)
			    pline("%s brushes against your %s.", Monnam(mtmp),
				   body_part(LEG));
		    }
		} else dmg = 0;
		break;
	    case AD_WERE:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(4) && u.ulycn == NON_PM &&
			!Protection_from_shape_changers &&
			!is_were(youmonst.data) &&
			!defends(AD_WERE,uwep)) {
		    You_feel("feverish.");
		    exercise(A_CON, FALSE);
		    u.ulycn = monsndx(mdat);
			if (u.ulycn == PM_HUMAN_WERERAT) u.ulycn = PM_WERERAT;
			if (u.ulycn == PM_HUMAN_WEREJACKAL) u.ulycn = PM_WEREJACKAL;
			if (u.ulycn == PM_HUMAN_WEREWOLF) u.ulycn = PM_WEREWOLF;
			if (u.ulycn == PM_HUMAN_WEREPANTHER) u.ulycn = PM_WEREPANTHER;
			if (u.ulycn == PM_HUMAN_WERETIGER) u.ulycn = PM_WERETIGER;
			if (u.ulycn == PM_HUMAN_WERESNAKE) u.ulycn = PM_WERESNAKE;
			if (u.ulycn == PM_HUMAN_WERESPIDER) u.ulycn = PM_WERESPIDER;
		    upermonst.mflags2 |= (M2_WERE);
		}
		break;
	    case AD_SGLD:
		hitmsg(mtmp, mattk);
		/* 5lo: Allow an exception for Muggers */
		if ((youmonst.data->mlet == mdat->mlet) && mdat != &mons[PM_MUGGER]) break;
		if(!mtmp->mcan) stealgold(mtmp);
		break;
	    case AD_EGLD: 
		if (u.umonnum == PM_GOLD_GOLEM){
		    pline("%s gnaws on you!", Monnam(mtmp));
		    (mtmp->mcan)?dmg=1:rehumanize();
		} else {
		    struct obj * geatme;
		    int how = 0;
		    dmg = 0;
		    buf[0] = 0;
#ifndef GOLDOBJ
		    geatme = g_at(u.ux, u.uy);
		    if (geatme){
			if(mtmp->mcan){
			    mtmp->mgold += geatme->quan;
			} else {
			    int metab_time = mtmp->mhpmax - mtmp->mhp;
			    if (geatme->quan < metab_time) metab_time = geatme->quan; 
			    mtmp->mhp += metab_time;
			    mtmp->mspec_used += metab_time/2 + 1;  /* instead of meating */
			}
			if(!Blind)
			    pline("%s quickly %s some gold from %s your %s!",
			      Monnam(mtmp), (mtmp->mcan)?"nabs":"gnoshes on", 
			      (Levitation)?"beneath":"between", makeplural(body_part(FOOT)));
			if (meatmetal_effects(mtmp, geatme) == 3) return 2;
			newsym(u.ux, u.uy);
			break;
		    } 
#endif /*  !GOLDOBJ */
		    if (geatme =
		      ochain_has_material(level.objects[u.ux][u.uy], GOLD, 0)){
			obj_extract_self(geatme);
			Sprintf(buf, "from %s your %s",
			  (Levitation)?"beneath":"between", makeplural(body_part(FOOT)));
		    } else if ( geatme = ochain_has_material(invent, GOLD, 0)){
#ifdef GOLDOBJ
			if (geatme->otyp == GOLD_PIECE){
			    int tmp;
			    const int gold_price = objects[GOLD_PIECE].oc_cost;
			    if (u.umonnum == PM_LEPRECHAUN){
				pline("%s tries to get your money, but fails...",
				  Monnam(mtmp));
				break;
			    }
			    tmp = (somegold(money_cnt(invent)) + gold_price - 1) / gold_price;
			    tmp = min(tmp, geatme->quan);
			    if (tmp < geatme->quan) geatme = splitobj(geatme, tmp);
			    freeinv(geatme);
			    flags.botl = 1;
			} else
#endif
			    obj_extract_self(geatme);
		    }
		    if (geatme){
			if (!(mtmp->mcan || geatme->otyp == AMULET_OF_STRANGULATION ||
			    geatme->otyp == RIN_SLOW_DIGESTION)){
				mtmp->mspec_used += geatme->owt/2 + 1;  /* instead of meating */
				how = 1;
			}
			pline("%s %s %s%s!",
			  Monnam(mtmp), (how)?"gobbles":"nabs", 
			  yname(geatme), buf);
			if (how) {
			    if (meatmetal_effects(mtmp, geatme) == 3) return 2;
			} else
			    mpickobj(mtmp, geatme);
			break;
		    } 
#ifndef GOLDOBJ 
		    if (u.ugold) {
			int tmp;
			if (u.umonnum == PM_LEPRECHAUN){
			    pline("%s tries to get your money, but fails...",
			      Monnam(mtmp));
			break;
			}
			u.ugold -= (tmp = somegold());
			Your("purse feels lighter.");
			if (mtmp->mcan){
			    mtmp->mgold += tmp;
			    break;
			}
			if (tmp > mtmp->mhpmax - mtmp->mhp)
			    tmp = mtmp->mhpmax - mtmp->mhp;
			mtmp->mspec_used = tmp;
			mtmp->mhp+=tmp;
		    }
#endif /*GOLDOBJ */
	    }
	    break;

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
		if (is_animal(mtmp->data)) {
			hitmsg(mtmp, mattk);
			if (mtmp->mcan) break;
			/* Continue below */
		} else if (dmgtype(youmonst.data, AD_SEDU)
#ifdef SEDUCE
			|| dmgtype(youmonst.data, AD_SSEX)
#endif
						) {
			pline("%s %s.", Monnam(mtmp), mtmp->minvent ?
		    "brags about the goods some dungeon explorer provided" :
		    "makes some remarks about how difficult theft is lately");
			if (mtmp->data == &mons[PM_SATYR] && !mtmp->mspec_used){
			    struct obj * o;
			    if(o = mon_has_item(mtmp, WOODEN_FLUTE, 1)){
				pline("%s plays %s %s.", 
				    Monnam(mtmp), mhis(mtmp), distant_name(o,xname));
				mcharmu(mtmp, dmg, TRUE);
				dmg = 0;
			    }
			} else if (!tele_restrict(mtmp)) (void) rloc(mtmp, TRUE);
			return 3;
		} else if (mtmp->mcan) {
		    if (!Blind) {
			/*
			 * We use flags.female here on the basis that the
			 * monster chooses whether to charm or to seduce
			 * based on your visible gender.  --ALI
			 */
			int do_charm = is_neuter(mdat) || \
			  flags.female == mtmp->female;
			pline("%s tries to %s you, but you seem %s.",
			    Adjmonnam(mtmp, "plain"),
			    (!flags.female)^(!mtmp->female) ? "seduce" : "charm",
			    (!flags.female)^(!mtmp->female) ? "uninterested" : "unaffected");
		    }
		    if(rn2(3) && mtmp->data != &mons[PM_SATYR]) {
			if (!tele_restrict(mtmp)) (void) rloc(mtmp, TRUE);
			return 3;
		    }
		    break;
		}
		if (mtmp->data == &mons[PM_SATYR]){
		    struct obj * o;
		    if(o = mon_has_item(mtmp, WOODEN_FLUTE, 1)){
			pline("%s plays %s %s.", Monnam(mtmp), 
			    mhis(mtmp), distant_name(o,xname));
			if (!mcharmu(mtmp,dmg, TRUE)) break;
			dmg=0;
		    } else 
		      break;
		}
		buf[0] = '\0';
		switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if(mtmp->data == &mons[PM_SATYR]){
			    pline("%s thanks you for the gift.", Monnam(mtmp));
			    return 3;
			    break;
			}
			if (!is_animal(mtmp->data) && !tele_restrict(mtmp))
			    (void) rloc(mtmp, TRUE);
			if (is_animal(mtmp->data) && *buf) {
			    if (canseemon(mtmp))
				pline("%s tries to %s away with %s.",
				      Monnam(mtmp),
				      locomotion(mtmp->data, "run"),
				      buf);
			}
			monflee(mtmp, 0, FALSE, FALSE);
			return 3;
		}
		break;
#ifdef SEDUCE
	    case AD_SSEX:
		if(could_seduceX(mtmp, &youmonst, mattk) == 1
			&& !mtmp->mcan)
		    if (doseduce(mtmp))
			return 3;
		break;
#endif
	    case AD_SAMU:
		hitmsg(mtmp, mattk);
		/* when the Wiz hits, 1/20 steals the amulet */
		if (u.uhave.amulet ||
		     u.uhave.bell || u.uhave.book || u.uhave.menorah
		     || u.uhave.questart) /* carrying the Quest Artifact */
		    if (!rn2(20)) stealamulet(mtmp);
		break;

	    case AD_TLPT:
		hitmsg(mtmp, mattk);
		if (uncancelled) {
		    if(flags.verbose)
			Your("position suddenly seems very uncertain!");
		    teleX();
		}
		break;
	    case AD_RUST:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		if (rn2(3)) hurtarmor(AD_RUST);
		break;
	    case AD_CORR:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (rn2(3)) hurtarmor(AD_CORR);
		break;
	    case AD_WTHR:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (rn2(3)) witherarmor();
		break;
	    case AD_LUCK:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (!rn2(3)) {change_luck(-1);
				pline("You feel unlucky.");
				}
		break;
	    case AD_HALU:
		hitmsg(mtmp, mattk);
		not_affected |= Blind ||
			(u.umonnum == PM_BLACK_LIGHT ||
			 u.umonnum == PM_VIOLET_FUNGUS ||
			 dmgtype(youmonst.data, AD_STUN));
		if (!not_affected && rn2(2) ) {
		    boolean chg;
		    if (!Hallucination)
			You("are caught in a blast of kaleidoscopic light!");
		    chg = make_hallucinated(HHallucination + (long)tmp,FALSE,0L);
		    You("%s", chg ? "inhale a cloud of psychedelic drugs!" : "took another hit of the good stuff!");
		}
		break;
#if 0 /* We're using Biodiversity's code here */
	    case AD_DISN:
		hitmsg(mtmp, mattk);

		if (!rn2(10))  {
		if (Disint_resistance) {
		    You("are not disintegrated.");
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
#endif
	    case AD_DCAY:
		hitmsg(mtmp, mattk);
		if (mtmp->mcan) break;
		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		if (rn2(3)) hurtarmor(AD_DCAY);
		break;
	    case AD_HEAL:
		/* a cancelled nurse is just an ordinary monster */
		if (mtmp->mcan) {
		    hitmsg(mtmp, mattk);
		    break;
		}
		if(!uwep
#ifdef TOURIST
		   && !uarmu
#endif
		   && !uarm && !uarmh && !uarms && !uarmg && !uarmc && !uarmf) {
		    boolean goaway = FALSE;
                    pline("%s %s you!  (I hope you don't mind.)", Monnam(mtmp), makeplural(barehitmsg(mtmp)));
		    if (Upolyd) {
			u.mh += rnd(7);
/* STEPHEN WHITE'S NEW CODE */                                            
			if (!rn2(7)) {
			    /* no upper limit necessary; effect is temporary */
			    u.mhmax++;
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		    } else {
			u.uhp += rnd(7);
			if (!rn2(7)) {
			    /* hard upper limit via nurse care: 25 * ulevel */
			    if (u.uhpmax < 5 * u.ulevel + d(2 * u.ulevel, 10)) {
				u.uhpmax++;
			    }
			    if (!rn2(13)) goaway = TRUE;
			}
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		    }
		    if (!rn2(3)) exercise(A_STR, TRUE);
		    if (!rn2(3)) exercise(A_CON, TRUE);
		    if (Sick) make_sick(0L, (char *) 0, FALSE, SICK_ALL);
		    flags.botl = 1;
		    if (goaway) {
			mongone(mtmp);
			return 2;
		    } else if (!rn2(33)) {
			if (!tele_restrict(mtmp)) (void) rloc(mtmp, FALSE);
			monflee(mtmp, d(3, 6), TRUE, FALSE);
			return 3;
		    }
		    dmg = 0;
		} else {
		    if (Role_if(PM_HEALER)) {
			if (flags.soundok && !(moves % 5))
		      verbalize("Doc, I can't help you unless you cooperate.");
			dmg = 0;
		    } else hitmsg(mtmp, mattk);
		}
		break;
	    case AD_CURS:
	    case AD_LITE:
		hitmsg(mtmp, mattk);
		if(!night() && mdat == &mons[PM_GREMLIN]) break;
		if(!mtmp->mcan && !rn2(10)) {
		    if (flags.soundok) {
			if (Blind) You_hear("laughter.");
			else       pline("%s chuckles.", Monnam(mtmp));
		    }
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
		    attrcurse();
		}
		break;
	    case AD_STUN:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4)) {
		    make_stunned(HStun + dmg, TRUE);
		    dmg /= 2;
		}
		break;
	    case AD_ACID:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(3)) {
		    if (Acid_resistance) {
			pline("You're covered in acid, but it seems harmless.");
			dmg = 0;
		    } else {
			pline("You're covered in acid! It burns!");
			exercise(A_STR, FALSE);
		    }
			if(rn2(30)) erode_armor(&youmonst, TRUE);
		} else		dmg = 0;
		break;
	    case AD_SLOW:
		hitmsg(mtmp, mattk);
		if (uncancelled && HFast &&
					!defends(AD_SLOW, uwep) && !rn2(4))
		    u_slow_down();
		break;
	    case AD_DREN:
		hitmsg(mtmp, mattk);
		if (uncancelled && !rn2(4))
		    drain_en(dmg);
		dmg = 0;
		break;
	    case AD_CONF:
		hitmsg(mtmp, mattk);
		if(!mtmp->mcan && !rn2(4) && !mtmp->mspec_used) {
		    mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
		    if(Confusion)
			 You("are getting even more confused.");
		    else You("are getting confused.");
		    make_confused(HConfusion + dmg, FALSE);
		}
		/*dmg = 0;*/
		break;
	    case AD_DETH:
		pline("%s reaches out with its deadly touch.", Monnam(mtmp));
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("Was that the touch of death?");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "touch of death";
			done(DIED);
			dmg = 0;
			break;
		    } /* else FALLTHRU */
		default: /* case 16: ... case 5: */
		    You_feel("your life force draining away...");
		    permdmg = 1;	/* actual damage done below */
		    break;
		case 4: case 3: case 2: case 1: case 0:
		    if (Antimagic) shieldeff(u.ux, u.uy);
		    pline("Lucky for you, it didn't work!");
		    dmg = 0;
		    break;
		}
		break;
	    case AD_PEST:
		pline("%s reaches out, and you feel fever and chills.",
			Monnam(mtmp));
		(void) diseasemu(mdat); /* plus the normal damage */
		/* No damage if invulnerable; setting dmg zero prevents
		 * "You are unharmed!" after a sickness inducing attack */
		if (Invulnerable) dmg = 0;
		break;
	    case AD_FAMN:
		pline("%s reaches out, and your body shrivels.",
			Monnam(mtmp));
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rn1(40,40));
		/* plus the normal damage */
		break;
	    case AD_DEPR:
		You("feel depressed.");
		switch(rn2(6) + u.uluck) {
			case 0:
				goto drain_life;
				break;
			case 1:
				(void) adjattrib(A_CON, -1, FALSE);
				if (ABASE(A_CON) < ATTRMIN(A_CON)) {
					killer = "depression";
					killer_format = KILLED_BY;
					done(DIED);
					/* lifesaved */
					ABASE(A_CON) = ATTRMIN(A_CON);
				}
				break;
			case 2:
				(void) adjattrib(A_INT, -1, FALSE);
				if (ABASE(A_INT) < ATTRMIN(A_INT)) {
					killer = "depression";
					killer_format = KILLED_BY;
					done(DIED);
					/* lifesaved */
					ABASE(A_INT) = ATTRMIN(A_INT);
				}
				break;
			default:
				pline("But the feeling subsides.");
				break;
		}
		dmg = 0;
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		hitmsg(mtmp, mattk);
		if (uncancelled)
		    docalm();
		break;
	    case AD_POLY:
		hitmsg(mtmp, mattk);
		if (uncancelled && !Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
		    polyself(FALSE);
		}
		break;
	    case AD_MAGM:
		hitmsg(mtmp, mattk);
		    if(Antimagic) {
			shieldeff(u.ux, u.uy);
			dmg = 0;
			pline("A hail of magic missiles narrowly misses you!");
		    } else {
			You("are hit by magic missiles appearing from thin air!");
	    }
	    break;
		break;
	    case AD_SLIM:    
		hitmsg(mtmp, mattk);
		if (!uncancelled) break;
		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		    dmg = 0;
		} else if (Unchanging ||
				youmonst.data == &mons[PM_GREEN_SLIME]) {
		    You("are unaffected.");
		    dmg = 0;
		} else if (!Slimed) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		hitmsg(mtmp, mattk);
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		if (uncancelled) {
		    struct obj *obj = some_armor(&youmonst);

		    if (drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
		    }
		}
		break;
	    case AD_DISN:
		hitmsg(mtmp, mattk);
		if (!mtmp->mcan && mtmp->mhp>6) {
		    int mass = 0, touched = 0;
		    struct obj * destroyme = 0;
		    if (Disint_resistance) {
			break;
		    } 
		    if (uarms) {
			if(!oresist_disintegration(uarms))
			    destroyme = uarms;
		    } else {
			switch (rn2(10)) { /* where it hits you */
			    case 0: /* head */
			    case 1:
				if (uarmc && (uarmc->otyp == DWARVISH_CLOAK ||
				  uarmc->otyp == MUMMY_WRAPPING)){
				    if(!oresist_disintegration(uarmc)){
					destroyme = uarmc;
				    }
				} else if (uarmh) {
				    if(!oresist_disintegration(uarmh)) {
					destroyme = uarmh;
				    }
				} else
				    touched = 1;
				break;
			    case 2: /* feet */
				if (uarmf) {
				    if(!oresist_disintegration(uarmf))
					destroyme = uarmf;
				    } else
					touched = 1;
				break;
			    case 3: /* hands (right) */
			    case 4:
				if (uwep) {
				    if (!oresist_disintegration(uwep)) {
					struct obj * otmp = uwep; 
					mass = otmp->owt;
					u.twoweap = FALSE;
					uwepgone();
					useup(otmp);
					dmg = 0;
				    }
				} else if (uarmg) {
				    if (!oresist_disintegration(uarmg))
					destroyme = uarmg;
				    } else
					touched = 1;
				break;
			    default: /* main body hit */
				if (uarmc) {
				    if(!oresist_disintegration(uarmc))
					destroyme = uarmc;
				    } else if (uarm) {
					if(!oresist_disintegration(uarm))
					    destroyme = uarm;
#ifdef TOURIST
				    } else if (uarmu) {
					if(!oresist_disintegration(uarmu))
					    destroyme = uarmu;
#endif
				    } else
					touched = 1;
				break;
			} 
		    }
		    if (destroyme) {
			mass = destroyme->owt;
			destroy_arm(destroyme);
			dmg = 0;
		    } else if(touched) {
			int recip_damage = instadisintegrate(mtmp->data->mname);
			if (recip_damage) {
			    dmg=0;
			    mtmp->mhp -= recip_damage;
			}
		    }
		    if (mass) {
			weight_dmg(mass);
			mtmp->mhp -= mass;
		    }
		}
		break;
	    case AD_CHRM:
		mcharmu(mtmp, dmg, FALSE);
		dmg=0;
		break;
	    case AD_SCLD:
		hitmsg(mtmp,mattk);
		if (uncancelled) {
		    pline("You're %s!", on_fire(youmonst.data, mattk));
		    if (Fire_resistance) {
			pline_The("steam doesn't feel hot.");
			dmg = 0;
		    } 
		    if((int) mtmp->m_lev > rn2(20))
		    destroy_item(POTION_CLASS, AD_FIRE);
		    if(!rn2(10)) hurtarmor(AD_RUST);
		} else {
		    pline("It feels merely creepy.");
		    dmg = 0;
		}
		break;
	    case AD_SHOE:
	    /* curse shoes, steal alt shoes, fumblize shoes, damage shoes */
		if (uarmf && !mtmp->mcan){
		    dmg = 0; 
		    if (canseemon(mtmp))
			pline("%s damages %s with %s little tools!",
			  Monnam(mtmp), yname(uarmf), mhis(mtmp));
		    else
			You_feel("some malicious cobbling!");
		    switch(rnd(3)){
			case 0:
			    if (uarmf->otyp != LOW_BOOTS  && uarmf->otyp != HIGH_BOOTS &&
				uarmf->otyp != IRON_SHOES && uarmf->otyp != FUMBLE_BOOTS){
				otmp = uarmf;
				Boots_off();
				otmp->otyp = (!rn2(4)) ? FUMBLE_BOOTS :
				  (otmp->otyp == KICKING_BOOTS) ? IRON_SHOES : LOW_BOOTS;
				setworn(otmp, W_ARMF);
				Boots_on(); 
				break;
			} 
			case 1:
			    if (uarmf->blessed){
				unbless(uarmf);
				break;
			    } else if(!(uarmf->cursed)){
				curse(uarmf);
				break;
			}
			case 2:
			    --uarmf->spe;
			    adj_abon(uarmf, -1); /* in case a boot is added that needs it */
			    break;
		    }
		}
		hitmsg(mtmp, mattk);
		break;
	    case AD_FLVR:
		if (mtmp->mcan || (rn2(5) > armpro) || !rn2(50) || mtmp->mspec_used) {
		    dmg = 0; 
		    break;
		}
		hitmsg(mtmp, mattk);
		switch (rn2(6)){
		    case 0: /* up, copied from muse: MUSE_POT_GAIN_LEVEL */
			if((ledger_no(&u.uz) == 1 && u.uhave.amulet) ||
			Can_rise_up(u.ux, u.uy, &u.uz)) {
			const char *riseup ="rise up, through the %s!";
			mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
			if(ledger_no(&u.uz) == 1) {
			    You(riseup, ceiling(u.ux,u.uy));
			    schedule_goto(&earth_level, FALSE, FALSE, FALSE, 0, 0);
			    return 4;
			    break;
			} else {
			    register int newlev = depth(&u.uz)-1;
			    d_level newlevel;
			    get_level(&newlevel, newlev);
			    if(on_level(&newlevel, &u.uz)) {
				goto mhitu_flvr_strange; 
				break;
			    } else You(riseup, ceiling(u.ux,u.uy));
			    schedule_goto(&newlevel, FALSE, FALSE, FALSE, 0, 0);
			    return 4;
			    break;
			}
		    } else
		        goto mhitu_flvr_strange;
		        break;
		    case 1: /* down */
			if (Can_fall_thru(&u.uz) && !In_sokoban(&u.uz) ){
			    d_level dtmp;
			    mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
			    pline("You sink down, through the %s!",surface(u.ux,u.uy));
			    if(*u.ushops) shopdig(1);
			    if (Is_stronghold(&u.uz)) {
				find_hell(&dtmp);
			    } else {
				dtmp.dnum = u.uz.dnum;
				dtmp.dlevel = dunlev(&u.uz)+1;
			    }
			    schedule_goto(&dtmp, FALSE, FALSE, FALSE, 0, 0);
			    return 4;
			    break;
			} else
			goto mhitu_flvr_strange;
			break;
		    case 2: /* top, teleport to dlev1, top level of branch? */
		    case 3: /* bottom, teleport to sanctum, bot lev of branch? */
			if(!In_sokoban(&u.uz) && !(In_quest(&u.uz)) && !In_endgame(&u.uz)){
			    int i,j;
			    struct monst * mamu;
			    d_level newlev;
			    if (u.uhave.amulet)
				mamu = 0;
			    else {
				for(i=u.ux-1;i<u.ux+1;++i)
				    for(j=u.uy;j<u.uy+1;++j)
					if((mamu = m_at(i,j)) && (levl_follower(mamu)) &&
					   (!mtmp->iswiz && mon_has_amulet(mtmp))){
					    i=u.ux+2;
					    break;
					} else {
					    mamu = 0;
					}
			    }
			    j = 0; /* Truth of Truth/Beauty */
			    if (u.uhave.amulet || mamu){
				sprintf(buf, "are back at the bottom!");
				newlev = sanctum_level;
			    } else if ((u.uevent.invoked && !mamu) || rn2(2)){
				if (Is_knox(&u.uz)) goto mhitu_flvr_strange;
				sprintf(buf,"are back at the top!");
				get_level(&newlev, 1);
				j = 1;
			    } else {
				get_level(&newlev, deepest_lev_reached(FALSE));
				sprintf(buf,"have reached the %s",
				on_level(&newlev,&sanctum_level)?"bottom!":"...bottom?");
			    }
			    if(on_level(&newlev,&u.uz)) goto mhitu_flvr_strange;
			    schedule_goto(&newlev, FALSE, FALSE, FALSE, 0, 0);
			    You(buf);
			    if (j){
				if (Hallucination) 
				    make_hallucinated(0,FALSE,0);
				exercise(A_WIS, TRUE);
				You("see the Truth!");
			    } else {
				if (adjattrib, A_CHA, 1)
				    pline("You gain an air of Beauty.");
			    }
			    return 4;
			} 
			break;
		    case 4: /* strange */
mhitu_flvr_strange:
			mtmp->mspec_used = mtmp->mspec_used + (dmg + rn2(6));
			if(Confusion||Hallucination)
			    pline("Things are getting even stranger.");
			else
			    pline("Things are getting strage.");
			make_confused(HConfusion + dmg + rn2(3), FALSE);
			if (!(u.umonnum == PM_BLACK_LIGHT ||
			    u.umonnum == PM_VIOLET_FUNGUS ||
			    dmgtype(youmonst.data, AD_STUN))){
			    make_hallucinated(HHallucination + (long)dmg+rn2(3),FALSE,0L);
			    }
			break;
		    case 5: /* charm */ 
			if (!mtmp->mpeaceful || mtmp->mpeacetim)
			    mcharmu(mtmp, dmg, TRUE);
			else 
			    goto mhitu_flvr_strange;
		    	} 
			dmg=0;
			break;

	    default:	dmg = 0;
			break;
	}

	/*if (randattack == 1) {
		mattk->adtyp = AD_RBRE;
		randattack = 0;
	}*/

	if(u.uhp < 1) done_in_by(mtmp);

/*	Negative armor class reduces damage done instead of fully protecting
 *	against hits.
 */
	if (dmg && u.uac < -10) {
		int tempval;
		tempval = rnd(-(10 + u.uac)/5+1);
		if (tempval < 1)  tempval = 1;
		if (tempval > 10) tempval = 10;
		dmg -= tempval;
		if (dmg < 1) dmg = 1;
	}

	if(dmg) {
	    if (Half_physical_damage
					/* Mitre of Holiness */
		|| (Role_if(PM_PRIEST) && uarmh && is_quest_artifact(uarmh) &&
		    (is_undead(mtmp->data) || is_demon(mtmp->data))))
		dmg = (dmg+1) / 2;

	    if (permdmg) {	/* Death's life force drain */
		int lowerlimit, *hpmax_p;
		/*
		 * Apply some of the damage to permanent hit points:
		 *	polymorphed	    100% against poly'd hpmax
		 *	hpmax > 25*lvl	    100% against normal hpmax
		 *	hpmax > 10*lvl	50..100%
		 *	hpmax >  5*lvl	25..75%
		 *	otherwise	 0..50%
		 * Never reduces hpmax below 1 hit point per level.
		 */
		permdmg = rn2(dmg / 2 + 1);
		if (Upolyd || u.uhpmax > 25 * u.ulevel) permdmg = dmg;
		else if (u.uhpmax > 10 * u.ulevel) permdmg += dmg / 2;
		else if (u.uhpmax > 5 * u.ulevel) permdmg += dmg / 4;

		if (Upolyd) {
		    hpmax_p = &u.mhmax;
		    /* [can't use youmonst.m_lev] */
		    lowerlimit = min((int)youmonst.data->mlevel, u.ulevel);
		} else {
		    hpmax_p = &u.uhpmax;
		    lowerlimit = u.ulevel;
		}
		if (*hpmax_p - permdmg > lowerlimit)
		    *hpmax_p -= permdmg;
		else if (*hpmax_p > lowerlimit)
		    *hpmax_p = lowerlimit;
		else	/* unlikely... */
		    ;	/* already at or below minimum threshold; do nothing */
		flags.botl = 1;
	    }

	    mdamageu(mtmp, dmg);
	}

	if (DEADMONSTER(mtmp))
	    res = 2;
	else if (dmg)
	    res = passiveum(olduasmon, mtmp, mattk);
	else
	    res = 1;
	stop_occupation();
	return res;
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL int
gulpmu(mtmp, mattk)	/* monster swallows you, or damage if u.uswallow */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	char	 buf[BUFSZ];
	struct trap *t = t_at(u.ux, u.uy);
	int	tmp = d((int)mattk->damn, (int)mattk->damd);
	int	tim_tmp;
	register struct obj *otmp2;
	int	i;
	/*int randattackA = 0;*/
	uchar atttypA;

	if (!u.uswallow) {	/* swallows you */
		if (youmonst.data->msize >= MZ_HUGE) return(0);
		if ((t && ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT))) &&
		    sobj_at(BOULDER, u.ux, u.uy))
			return(0);

		if (Punished) unplacebc();	/* ball&chain go away */
		remove_monster(mtmp->mx, mtmp->my);
		mtmp->mtrapped = 0;		/* no longer on old trap */
		place_monster(mtmp, u.ux, u.uy);
		newsym(mtmp->mx,mtmp->my);
#ifdef STEED
		if (is_animal(mtmp->data) && u.usteed) {
			char buf[BUFSZ];
			/* Too many quirks presently if hero and steed
			 * are swallowed. Pretend purple worms don't
			 * like horses for now :-)
			 */
			Strcpy(buf, mon_nam(u.usteed));
			pline ("%s lunges forward and plucks you off %s!",
				Monnam(mtmp), buf);
			dismount_steed(DISMOUNT_ENGULFED);
		} else
#endif
		pline("%s engulfs you!", Monnam(mtmp));
		stop_occupation();
		reset_occupations();	/* behave as if you had moved */

		if (u.utrap) {
			You("are released from the %s!",
				u.utraptype==TT_WEB ? "web" : "trap");
			u.utrap = 0;
		}

		i = number_leashed();
		if (i > 0) {
		    const char *s = (i > 1) ? "leashes" : "leash";
		    pline_The("%s %s loose.", s, vtense(s, "snap"));
		    unleash_all();
		}

		if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
			minstapetrify(mtmp, TRUE);
			if (mtmp->mhp > 0) return 0;
			else return 2;
		}

		display_nhwindow(WIN_MESSAGE, FALSE);
		vision_recalc(2);	/* hero can't see anything */
		u.uswallow = 1;
		setustuck(mtmp);
		/* u.uswldtim always set > 1 */
		tim_tmp = 25 - (int)mtmp->m_lev;
		if (tim_tmp > 0) tim_tmp = rnd(tim_tmp) / 2;
		else if (tim_tmp < 0) tim_tmp = -(rnd(-tim_tmp) / 2);
		tim_tmp += -u.uac + 10;
		u.uswldtim = (unsigned)((tim_tmp < 2) ? 2 : tim_tmp);
		swallowed(1);
		for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
		    (void) snuff_lit(otmp2);
	}

	if (mtmp != u.ustuck) return(0);
	if (u.uswldtim > 0) u.uswldtim -= 1;

	/* Monsters with AD_RBRE have random engulfing attacks. --Amy */

	atttypA = mattk->adtyp;

	if (atttypA == AD_RBRE) {
		while (atttypA == AD_ENDS ||atttypA == AD_RBRE || atttypA == AD_SPC2 || atttypA == AD_WERE) {
			atttypA = rn2(AD_ENDS); }
		/*randattack = 1;*/
	}

	switch(atttypA) {

		case AD_DGST:
		    if (Slow_digestion) {
			/* Messages are handled below */
			u.uswldtim = 0;
			tmp = 0;
		    } else if (u.uswldtim == 0) {
			pline("%s totally digests you!", Monnam(mtmp));
			tmp = u.uhp;
			if (Half_physical_damage) tmp *= 2; /* sorry */
		    } else {
			pline("%s%s digests you!", Monnam(mtmp),
			      (u.uswldtim == 2) ? " thoroughly" :
			      (u.uswldtim == 1) ? " utterly" : "");
			exercise(A_STR, FALSE);
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
				pline("You hear a chuckling laughter.");
			    attrcurse();
			}
			pline("It curses you!");
			break;

		case AD_SLEE:
			if (!rn2(5) && multi >= 0) {
			    if (Sleep_resistance) break;
			    fall_asleep(-rnd(10), TRUE);
			    You("suddenly fall asleep!");
			}
			pline("It slaps you!");
			break;
		case AD_DRST:
			if (!rn2(8)) {
				You("feel your strength drain away!");
			    poisoned("The attack", A_STR, "strength drain", 30);
			}
			break;
		case AD_DRDX:
			if (!rn2(8)) {
				You("feel your muscles cramping!");
			    poisoned("The attack", A_DEX, "dexterity drain", 30);
			}
			break;
		case AD_DRCO:
			if (!rn2(8)) {
				You("feel a lack of force!");
			    poisoned("The attack", A_CON, "constitution drain", 30);
			}
			break;
	      case AD_STUN:
			{
				You("seem less steady!");
			    make_stunned(HStun + tmp, TRUE);
			}
			break;
	      case AD_SLOW:
			if (HFast && !defends(AD_SLOW, uwep) && !rn2(4))
			    u_slow_down();
			break;
	      case AD_PLYS:
			if (multi >= 0 && !rn2(3)) {
			    if (Free_action) {
				You("cramp for a moment.");            
			    } else {
				You("can't move!");
				nomovemsg = 0;	/* default: "you can move again" */
				nomul(-rnd(10), "paralyzed by an engulfing monster");
				exercise(A_DEX, FALSE);
			    }
			}
			pline("It whacks you!");
			break;
	      case AD_DRLI:
			pline("It sucks you!");
/* Imagine the facial expression of a player who thinks this is the mind flayer's amnesia attack. --Amy */
			if (!rn2(3) && (!Drain_resistance)  ) {
			    losexp("life drainage", FALSE);
			}
			break;
	      case AD_DREN:
			pline("It drains you!");
			if (!rn2(4)) drain_en(tmp);
			break;

	    case AD_GLIB:
			pline("A disgusting substance pours all over your hands!");

			/* hurt the player's hands --Amy */
			incr_itimeout(&Glib, tmp);
			break;

	    case AD_DARK:

			pline("You feel a constricting darkness...");

			/* create darkness around the player --Amy */
			litroomlite(FALSE);
			break;

	    case AD_LEGS:
			{ register long sideX = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
	
			if (tmp <= 5)
		    	    Your("legs itch badly for a moment.");
			else if (tmp <= 10)
			    pline("Wounds appear on your legs!");
			else if (tmp <= 20)
			    pline("Severe wounds appear on your legs!");
			else if (tmp <= 40)
			    Your("legs are covered with painful wounds!");
			else
			    Your("legs are covered with bloody wounds! It hurts like hell! Auuuggghhhh!");
			set_wounded_legs(sideX, rnd(60-ACURR(A_DEX)));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
			}
			break;
	    case AD_STON:
		if(!rn2(3)) {
			if (flags.soundok)
			    You_hear("a hissing noise!");
			if(!rn2(10) ||
			    (flags.moonphase == NEW_MOON && !have_lizard())) {
			    if (!Stoned && !Stone_resistance
				    && !(poly_when_stoned(youmonst.data) &&
					polymon(PM_STONE_GOLEM))) {
				Stoned = 7;
				delayed_killer = mtmp->data->mname;
				if (mtmp->data->geno & G_UNIQ) {
				    if (!type_is_pname(mtmp->data)) {
					static char kbuf[BUFSZ];

					/* "the" buffer may be reallocated */
					Strcpy(kbuf, the(delayed_killer));
					delayed_killer = kbuf;
				    }
				    killer_format = KILLED_BY;
				} else killer_format = KILLED_BY_AN;
				return(1);
				/* You("turn to stone..."); */
				/* done_in_by(mtmp); */
			    }
			}
		}
	    pline("It strikes you hard!");
		break;
	    case AD_STCK:
		if (!u.ustuck && !sticks(youmonst.data))
			setustuck(mtmp);
	    pline("You are covered with some sticky substance!");
		break;
	    case AD_SGLD:
		stealgold(mtmp);
		    pline("It shakes you!");
		break;

	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
	    case AD_SSEX:

			buf[0] = '\0';
		if ( (rnd(100) > ACURR(A_CHA)) && ((mtmp->female) && !flags.female && rn2(5) ) || ((!mtmp->female) && flags.female && rn2(3) ) || 
			((mtmp->female) && flags.female && rn2(2) ) || ((!mtmp->female) && !flags.female && rn2(2) ) )
			{ 
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp))
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, 0, FALSE, FALSE);
			return 3;
			};
		}
		    pline("It thrusts you!");
		break;

	    case AD_TLPT:
		    if(flags.verbose)
			pline("You are teleported away!");
		    teleX();
			pline("A stream of energy irradiates you!");
		break;

	    case AD_RUST:
		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		if (rn2(3)) hurtarmor(AD_RUST);
		pline("You are covered with rust!");
		break;
	    case AD_CORR:
		if (rn2(3)) hurtarmor(AD_CORR);
		pline("You are covered with acid!");
		break;
	    case AD_WTHR:
		if (rn2(3)) witherarmor();
		pline("You are covered with some aggressive substance!");
		break;
	    case AD_LUCK:
		if (!rn2(3)) change_luck(-1);
		pline("It's sucking away all of your good feelings!");
		break;
	    case AD_CONF:
		    pline("Your head spins wildly!");
		    make_confused(HConfusion + tmp, FALSE);
		break;
	    case AD_DCAY:
		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		if (rn2(3)) hurtarmor(AD_DCAY);
		pline("You are covered with decaying waste!");
		break;
	    case AD_HALU:
		    make_hallucinated(HHallucination + tmp,FALSE,0L);
		    pline("You inhale some great stuff!");
		break;
	    case AD_DETH:
		pline("You see your life flash in front of your eyes...");
		if (is_undead(youmonst.data)) {
		    /* Still does normal damage */
		    pline("But since you're undead already, this doesn't matter to you.");
		    break;
		}
		switch (rn2(20)) {
		case 19: /* case 18: case 17: */
		    if (!Antimagic) {
			killer_format = KILLED_BY_AN;
			killer = "engulf of death";
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
		    pline("Nothing happens.");
		    tmp = 0;
		    break;
		}
		break;
	    case AD_FAMN:
		pline("You are being eaten!");
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rnd(40));
		morehungry(tmp);
		/* plus the normal damage */
		break;
	    case AD_SLIM:    
		pline("A disgusting green goo pours all over you!");
		if (rn2(3)) break;
		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		    tmp = 0;
		} else if (Unchanging ||
				youmonst.data == &mons[PM_GREEN_SLIME]) {
		    You("are unaffected.");
		    tmp = 0;
		} else if (!Slimed) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		pline("You feel something sapping your energy!");
		    docalm();
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		pline("Mysterious waves pulsate through your body!");
		if (rn2(3)) break;
		    struct obj *obj = some_armor(&youmonst);

		    if (drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
		    }
		break;
	    case AD_POLY:
		pline("A weird green light pierces you!");
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			You("undergo a freakish metamorphosis!");
		    polyself(FALSE);
		}
		break;
	    case AD_TCKL:
		pline("A bunch of feelers suddenly reaches out for you!");
		if (multi >= 0 && !rn2(3)) {
		    if (Free_action)
			pline("They don't seem to do all that much, though...");
		    else {
			pline("Wahahahaha! Arrgh! It tickles!");
			nomovemsg = 0;	/* default: "you can move again" */
			nomul(-rnd(10), "tickled by a bunch of feelers");
			exercise(A_DEX, FALSE);
			exercise(A_CON, FALSE);
		    }
		}
		break;

		case AD_PHYS:
		    if (mtmp->data == &mons[PM_FOG_CLOUD]) {
			You("are laden with moisture and %s",
			    flaming(youmonst.data) ? "are smoldering out!" :
			    Breathless ? "find it mildly uncomfortable." :
			    amphibious(youmonst.data) ? "feel comforted." :
			    "can barely breathe!");
			/* NB: Amphibious includes Breathless */
			if (Amphibious && !flaming(youmonst.data)) tmp = 0;
		    } else {
			You("are pummeled with debris!");
			exercise(A_STR, FALSE);
		    }
		    break;
		case AD_ACID:
		    if (Acid_resistance) {
			You("are covered with a seemingly harmless goo.");
			tmp = 0;
		    } else {
		      if (Hallucination) pline("Ouch!  You've been slimed!");
		      else You("are covered in slime!  It burns!");
		      exercise(A_STR, FALSE);
		    }
		    /* Mik: Go corrode a few things... */
			/*for (otmp2 = invent; otmp2; otmp2 = otmp2->nobj)
			    if (is_corrodeable(otmp2) && !rn2(9))
		    		(void) rust_dmg(otmp2, xname(otmp2), 3, FALSE, 
					&youmonst);*/
			if(rn2(30)) erode_armor(&youmonst, TRUE);
		    break;
		case AD_BLND:
		    if (can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0)) {
			if(!Blind) {
			    You_cant("see in here!");
			    make_blinded((long)tmp,FALSE);
			    if (!Blind) Your(vision_clears);
			} else
			    /* keep him blind until disgorged */
			    make_blinded(Blinded+1,FALSE);
		    }
		    tmp = 0;
		    break;
		case AD_ELEC:
		    if(!mtmp->mcan && rn2(2)) {
			pline_The("air around you crackles with electricity.");
			if (Shock_resistance) {
				shieldeff(u.ux, u.uy);
				You("seem unhurt.");
				ugolemeffects(AD_ELEC,tmp);
				tmp = 0;
			}
		    } else tmp = 0;
		    break;
		case AD_COLD:
		    if(!mtmp->mcan && rn2(2)) {
			if (Cold_resistance) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly chilly.");
				ugolemeffects(AD_COLD,tmp);
				tmp = 0;
			} else You("are freezing to death!");
		    } else tmp = 0;
		    break;
		case AD_DRIN:
		    if(!mtmp->mcan && !rn2(4)) {
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
			if (!Keen_memory) {
		    	    forget_levels(5);	/* lose memory of 25% of levels */
		    	    forget_objects(5);	/* lose memory of 25% of objects */
			}
			exercise(A_WIS, FALSE);
		    } else tmp = 0;
		    break;
		case AD_FIRE:
		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance) {
				shieldeff(u.ux, u.uy);
				You_feel("mildly hot.");
				ugolemeffects(AD_FIRE,tmp);
				tmp = 0;
			} else You("are burning to a crisp!");
			burn_away_slime();
		    } else tmp = 0;
		    break;
		case AD_SCLD:
		    if(!mtmp->mcan && rn2(2)) {
			if (Fire_resistance) {
			    shieldeff(u.ux, u.uy);
			    You_feel("mildly hot.");
			    tmp=0;
			} else 
			    pline("You're %s!", on_fire(youmonst.data, mattk));
			if(!rn2(3)) hurtarmor(AD_RUST);
		    } else tmp = 0;
		    break;
		case AD_DISE:
		case AD_PEST:
		    if (!diseasemu(mtmp->data)) tmp = 0;
		    break;
		case AD_MAGM:
		    if(Antimagic) {
			tmp = 0;
		    } else {
			You("are irradiated with energy!");
		    }
		    break;
		case AD_DISN:
		    You("feel like a drill is tearing you apart!");
		if (!rn2(10))  {
		if (Disint_resistance) {
		    You("are not disintegrated.");
		    break;
            } else if (Invulnerable) {
                pline("You are unharmed!");
                break;
		}
		 else if (uarms) {
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
		default:
		    /*tmp = 0;*/
			pline(rn2(2) ? "It hits you!" : "It hits!");
		    break; /* There is absolutely no reason why this wouldn't do any damage. --Amy */
	}

	/*if (randattackA == 1) {
		mattk->adtyp = AD_RBRE;
		randattackA = 0;
	}*/

	if (Half_physical_damage) tmp = (tmp+1) / 2;

	mdamageu(mtmp, tmp);
	if (tmp) stop_occupation();

	if (touch_petrifies(youmonst.data) && !resists_ston(mtmp)) {
	    pline("%s very hurriedly %s you!", Monnam(mtmp),
		  is_animal(mtmp->data)? "regurgitates" : "expels");
	    expels(mtmp, mtmp->data, FALSE);
	} else if (!u.uswldtim || youmonst.data->msize >= MZ_HUGE) {
	    You("get %s!", is_animal(mtmp->data)? "regurgitated" : "expelled");
	    if (flags.verbose && (is_animal(mtmp->data) ||
		    (dmgtype(mtmp->data, AD_DGST) && Slow_digestion)))
		pline("Obviously %s doesn't like your taste.", mon_nam(mtmp));
	    expels(mtmp, mtmp->data, FALSE);
	}
	return(1);
}

STATIC_OVL int
explmu(mtmp, mattk, ufound)	/* monster explodes in your face */
register struct monst *mtmp;
register struct attack  *mattk;
boolean ufound;
{

    if (mtmp->mcan) return(0);

    if (!ufound)
	pline("%s explodes at a spot in %s!",
	    canseemon(mtmp) ? Monnam(mtmp) : "It",
	    levl[mtmp->mux][mtmp->muy].typ == WATER
		? "empty water" : "thin air");
    else {
	register int tmp = d((int)mattk->damn, (int)mattk->damd);
	register boolean not_affected = defends((int)mattk->adtyp, uwep);

	hitmsg(mtmp, mattk);

	switch (mattk->adtyp) {
	    case AD_CONF:
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			if (!Hallucination) You("are confused by a blast of light!");
			else pline("%s TO ORDER! %s MESSED!",urole.name.m,urole.name.m);
			make_confused(HConfusion + (long)tmp, FALSE); }
		break;

	    case AD_ACID:
		not_affected |= Acid_resistance;
		goto common;
	    case AD_COLD:
		not_affected |= Cold_resistance;
		goto common;
	    case AD_FIRE:
	    case AD_SCLD:
		not_affected |= Fire_resistance;
		goto common;
	    case AD_ELEC:
		not_affected |= Shock_resistance;
	    case AD_PHYS: /* just do some plain physical damage, for golem's selfdestruct attack et. al. --Amy */
common:

		if (!not_affected) {
		    if (ACURR(A_DEX) > rnd(200)) {
			You("duck some of the blast.");
			tmp = (tmp+1) / 2;
		    } else {
		        if (flags.verbose) You("get blasted!");
		    }
		    if (mattk->adtyp == AD_FIRE) burn_away_slime();
		    if (Half_physical_damage) tmp = (tmp+1) / 2;
		    mdamageu(mtmp, tmp);
		}
		break;

	    case AD_BLND:
		not_affected = resists_blnd(&youmonst);
		if (!not_affected) {
		    /* sometimes you're affected even if it's invisible */
		    if (mon_visible(mtmp) || (rnd(tmp /= 2) > u.ulevel)) {
			You("are blinded by a blast of light!");
			make_blinded((long)tmp, FALSE);
			if (!Blind) Your(vision_clears);
		    } else if (flags.verbose)
			You("get the impression it was not terribly bright.");
		}
		break;

	    case AD_RUST:

		water_damage(invent, FALSE, FALSE);

		break;

	    case AD_WTHR:

		withering_damage(invent, FALSE, FALSE);

		break;

	    case AD_STUN:
		not_affected = Blind;
		if (!not_affected) {
			if (!Hallucination) You("are stunned by a blast of light!");
			else pline("%s NUMBED CAN'T DO",urole.name.m);
			make_stunned(HStun + (long)tmp, FALSE);
		}
		break;

	    case AD_HALU:
		not_affected |= Blind ||
			(u.umonnum == PM_BLACK_LIGHT ||
			 u.umonnum == PM_VIOLET_FUNGUS ||
			 dmgtype(youmonst.data, AD_STUN));
		if (!not_affected) {
		    boolean chg;
		    if (!Hallucination)
			You("are caught in a blast of kaleidoscopic light!");
		    chg = make_hallucinated(HHallucination + (long)tmp,FALSE,0L);
		    You("%s.", chg ? "are freaked out" : "seem unaffected");
		}
		break;

	    default:
		break;
	}
	if (not_affected) {
	    You("seem unaffected by it.");
	    ugolemeffects((int)mattk->adtyp, tmp);
	}
    }
    mondead(mtmp);
    wake_nearto(mtmp->mx, mtmp->my, 7*7);
    if (mtmp->mhp > 0) return(0);
    return(2);	/* it dies */
}

int
gazemu(mtmp, mattk)	/* monster gazes at you */
	register struct monst *mtmp;
	register struct attack  *mattk;
{
	register struct engr *ep = engr_at(u.ux,u.uy);
	char	 buf[BUFSZ];

	/*int randattackB = 0;*/
	uchar atttypB;

	/* Monsters with AD_RBRE can use any random gaze. --Amy */

	atttypB = mattk->adtyp;

	if (atttypB == AD_RBRE) {
		while (atttypB == AD_ENDS ||atttypB == AD_RBRE || atttypB == AD_SPC2 || atttypB == AD_WERE) {
			atttypB = rn2(AD_ENDS); }
		/*randattack = 1;*/
	}

	switch(atttypB) {
	    case AD_STON:
		if (mtmp->mcan || !mtmp->mcansee) {
		    if (!canseemon(mtmp)) break;	/* silently */
		    pline("%s %s.", Monnam(mtmp),
			  (mtmp->data == &mons[PM_MEDUSA] && mtmp->mcan) ?
				"doesn't look all that ugly" :
				"gazes ineffectually");
		    break;
		}
		if (Reflecting && couldsee(mtmp->mx, mtmp->my) &&
			mtmp->data == &mons[PM_MEDUSA]) {
		    /* hero has line of sight to Medusa and she's not blind */
		    boolean useeit = canseemon(mtmp);

		    if (useeit)
			(void) ureflects("%s gaze is reflected by your %s.",
					 s_suffix(Monnam(mtmp)));
		    if (mon_reflects(mtmp, !useeit ? (char *)0 :
				     "The gaze is reflected away by %s %s!"))
			break;
		    if (!m_canseeu(mtmp)) { /* probably you're invisible */
			if (useeit)
			    pline(
		      "%s doesn't seem to notice that %s gaze was reflected.",
				  Monnam(mtmp), mhis(mtmp));
			break;
		    }
		    if (useeit)
			pline("%s is turned to stone!", Monnam(mtmp));
		    stoned = TRUE;
		    killed(mtmp);

		    if (mtmp->mhp > 0) break;
		    return 2;
		}
		if (canseemon(mtmp) && couldsee(mtmp->mx, mtmp->my) &&
		    !Stone_resistance) {
		    You("meet %s gaze.", s_suffix(mon_nam(mtmp)));
		    stop_occupation();
		    if(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))
			break;
		    /*You("turn to stone...");
		    killer_format = KILLED_BY;
		    killer = mtmp->data->mname;
		    done(STONING);*/
		    You("start turning to stone...");
			if (!Stoned) Stoned = 7;
		}
		break;
	    case AD_CURS:
	    case AD_LITE:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
		pline("%s gives you a mean look!", Monnam(mtmp));

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
				pline("%s laughs fiendishly!", Monnam(mtmp));
			    attrcurse();
			}
		}
		break;
	    case AD_MAGM:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
		pline("%s's eye color suddenly changes!", Monnam(mtmp));
		    if(Antimagic) {
			shieldeff(u.ux, u.uy);
			pline("A hail of magic missiles narrowly misses you!");
		    } else {
			You("are hit by magic missiles appearing from thin air!");
	            mdamageu(mtmp, d(4,6));
		    }
		}
	    break;
	    case AD_DISN:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(4))
 		{
		if (!rn2(20))  {
		pline("%s's gaze seems to drill right into you!", Monnam(mtmp));
		if (Disint_resistance) {
		    You("are not disintegrated.");
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
		}
	      break;
	    case AD_ACID:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{

		if(!rn2(3)) {
		pline("%s sends a terrifying gaze at you!", Monnam(mtmp));
		    if (Acid_resistance) {
			pline("You're covered in acid, but it seems harmless.");
		    } else {
			pline("You're covered in acid! It burns!");
			exercise(A_STR, FALSE);
		    int dmg = d(2,6);
		    if (dmg) mdamageu(mtmp, dmg);
		    }
			if(rn2(30)) erode_armor(&youmonst, TRUE);
		}
		}
		break;
	      case AD_DRLI:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
			if (!rn2(7) && (!Drain_resistance)  ) {
				pline("%s seems to drain your life with its gaze!", Monnam(mtmp));
			    losexp("life drainage", FALSE);
			}
		}
		break;

	      case AD_DREN:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
			if (!rn2(4)) {drain_en(10);
			pline("%s seems to drain your energy with its gaze!", Monnam(mtmp));
			}
		}
		break;
	    case AD_NGRA:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{

		      if (ep && sengr_at("Elbereth", u.ux, u.uy) ) {
		/* This attack can remove any Elbereth engraving, even burned ones. --Amy */
			pline("%s seems to suck in the words engraved on the surface below you!", Monnam(mtmp));
		    del_engr(ep);
		    ep = (struct engr *)0;
			}
		}

		break;

	    case AD_GLIB:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(10))
 		{

		/* hurt the player's hands --Amy */
		pline("%s telepathically twists your hands!", Monnam(mtmp));
		incr_itimeout(&Glib, (int)mattk->damd );

		}
		break;

	    case AD_DARK:
		if (!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(4))
 		{

		/* create darkness around the player --Amy */
		pline("%s's sinister gaze fills your mind with dreadful, evil thoughts!", Monnam(mtmp));
		litroomlite(FALSE);
		}
		break;

	    case AD_LEGS:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
			{ register long sideX = rn2(2) ? RIGHT_SIDE : LEFT_SIDE;
	
			pline("%s's gaze makes your legs turn to jelly!", Monnam(mtmp));
			set_wounded_legs(sideX, rnd(60-ACURR(A_DEX)));
			exercise(A_STR, FALSE);
			exercise(A_DEX, FALSE);
			}
		}
		break;
	    case AD_SGLD:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
			pline("%s uses a telepathic gaze!", Monnam(mtmp));
			stealgold(mtmp);
		}
		break;

	    case AD_SLIM:    
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(15))
 		{

		pline("%s hurls some disgusting green goo at you!", Monnam(mtmp));

		if (flaming(youmonst.data)) {
		    pline_The("slime burns away!");
		} else if (Unchanging ||
				youmonst.data == &mons[PM_GREEN_SLIME]) {
		    You("are unaffected.");
		} else if (!Slimed) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = mtmp->data->mname;
		} else
		    pline("Yuck!");
		}
		break;
	    case AD_CALM:	/* KMH -- koala attack */
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && rn2(5))
 		{
		pline("%s gazes at you softly.", Monnam(mtmp));
		    docalm();
		}
		break;
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(25))
 		{
		pline("%s gazes at your belongings!", Monnam(mtmp));
		/* uncancelled is sufficient enough; please
		   don't make this attack less frequent */
		    struct obj *obj = some_armor(&youmonst);

		    if (drain_item(obj)) {
			Your("%s less effective.", aobjnam(obj, "seem"));
		    }
		}
		break;
	    case AD_POLY:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(20))
 		{
		if (!Unchanging && !Antimagic) {
		    if (flags.verbose)
			pline("%s throws a changing gaze at you!", Monnam(mtmp));
		    polyself(FALSE);
			}
		}
		break;

	    case AD_CONF:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
		    int conf = d(3,4);

		    mtmp->mspec_used = mtmp->mspec_used + (conf + rn2(6));
		    if(!Confusion)
			pline("%s gaze confuses you!",
			                  s_suffix(Monnam(mtmp)));
		    else
			You("are getting more and more confused.");
		    make_confused(HConfusion + conf, FALSE);
			if(mtmp->data == &mons[PM_LESHY] && !rn2(5)){
			    You_feel("a little lost");
			    forget_map(0);
			    docrt();
			}
		    stop_occupation();
		}
		break;
	    case AD_FAMN:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(4)) 		{
		pline("%s gazes at you with its hungry eyes!",
			Monnam(mtmp));
		exercise(A_CON, FALSE);
		if (!is_fainted()) morehungry(rnd(40));
		/* plus the normal damage */
		}
		break;
	    case AD_SITM:	/* for now these are the same */
	    case AD_SEDU:
	    case AD_SSEX:
	      if( (rnd(100) > ACURR(A_CHA)) &&  !mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && /*!rn2(25)*/ 
		( ((mtmp->female) && !flags.female && !rn2(5) ) || ((!mtmp->female) && flags.female && !rn2(15) ) || 
			((mtmp->female) && flags.female && !rn2(25) ) || ((!mtmp->female) && !flags.female && !rn2(25) ) )

		) 		{
		pline("%s gazes at you with its demanding eyes!",
			Monnam(mtmp));
		buf[0] = '\0';
			switch (steal(mtmp, buf)) {
		  case -1:
			return 2;
		  case 0:
			break;
		  default:
			if ( !tele_restrict(mtmp))
			    (void) rloc(mtmp, FALSE);
			monflee(mtmp, 0, FALSE, FALSE);
			return 3;
			};
		}
		break;
	    case AD_RUST:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		pline("%s squirts water at you!", Monnam(mtmp));

		if (u.umonnum == PM_IRON_GOLEM) {
			You("rust!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			/* KMH, balance patch -- this is okay with unchanging */
			u.mh = 0;
			rehumanize();
			break;
		}
		hurtarmor(AD_RUST);
		}
		break;
	    case AD_DCAY:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		pline("%s flings organic matter at you!", Monnam(mtmp));

		if (u.umonnum == PM_WOOD_GOLEM ||
		    u.umonnum == PM_LEATHER_GOLEM) {
			You("rot!");
			u.uhp -= mons[u.umonnum].mlevel;
			u.uhpmax -= mons[u.umonnum].mlevel;
			if (u.uhpmax < 1) u.uhpmax = 1;
			u.mh = 0;
			/* KMH, balance patch -- this is okay with unchanging */
			rehumanize();
			break;
		}
		hurtarmor(AD_DCAY);
		}
		break;
	    case AD_CORR:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		pline("%s throws corrosive stuff at you!", Monnam(mtmp));
		hurtarmor(AD_CORR);
		}
		break;
	    case AD_WTHR:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		pline("%s telepathically messes with your clothes!", Monnam(mtmp));
		witherarmor();
		}
		break;
	    case AD_LUCK:
	      if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(20)) 		{
		pline("%s's terrifying gaze makes you feel like you'll never be able to experience luck again!", Monnam(mtmp));
		change_luck(-1);
		}
		break;
	    case AD_STUN:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) &&
		   mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
		    int stun = d(2,6);

		    mtmp->mspec_used = mtmp->mspec_used + (stun + rn2(6));
		    pline("%s stares piercingly at you!", Monnam(mtmp));
		    make_stunned(HStun + stun, TRUE);
		    stop_occupation();
		}
		break;
	    case AD_BLND:
		if (mtmp->data == &mons[PM_UMBRAL_HULK]){
		    if (!mtmp->mspec_used && !Blind && couldsee(mtmp->mx, mtmp->my) &&
		      can_blnd(mtmp, &youmonst, mattk->aatyp, (struct obj*)0)) {
			pline("You meet %s gaze! The shadows merge into utter darkness!", 
			  s_suffix(mon_nam(mtmp)) );
			make_blinded(Blinded + d((int)mattk->damn, (int)mattk->damd), FALSE);
			if (!Blind) Your(vision_clears);
			}
		    break;
		}
		if (!mtmp->mcan && canseemon(mtmp) && !resists_blnd(&youmonst)
			&& distu(mtmp->mx,mtmp->my) <= BOLT_LIM*BOLT_LIM) {
		    int blnd = d((int)mattk->damn, (int)mattk->damd);

		    You("are blinded by %s radiance!",
			              s_suffix(mon_nam(mtmp)));
		    make_blinded((long)blnd,FALSE);
		    stop_occupation();
		    /* not blind at this point implies you're wearing
		       the Eyes of the Overworld; make them block this
		       particular stun attack too */
		    if (!Blind) Your(vision_clears);
		    else make_stunned((long)d(1,3),TRUE);
		}
		break;
	    case AD_FIRE:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
		    int dmg = d(2,6);

		    pline("%s attacks you with a fiery gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Fire_resistance) {
			pline_The("fire doesn't feel hot!");
			dmg = 0;
		    }
		    burn_away_slime();
		    /*if ((int) mtmp->m_lev > rn2(20))
			destroy_item(SCROLL_CLASS, AD_FIRE);
		    if ((int) mtmp->m_lev > rn2(20))
			destroy_item(POTION_CLASS, AD_FIRE);
		    if ((int) mtmp->m_lev > rn2(25))
			destroy_item(SPBOOK_CLASS, AD_FIRE);*/
		    if (!rn2(33)) /* new calculations --Amy */
		      (void)destroy_item(POTION_CLASS, AD_FIRE);
		    if (!rn2(33))
		      (void)destroy_item(SCROLL_CLASS, AD_FIRE);
		    if (!rn2(50))
		      (void)destroy_item(SPBOOK_CLASS, AD_FIRE);

		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
	    case AD_COLD:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int dmg = d(2,6);

		    pline("%s attacks you with an icy gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Cold_resistance) {
			pline_The("cold doesn't freeze you!");
			dmg = 0;
		    }
		    /*if ((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(POTION_CLASS, AD_COLD);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
	    case AD_ELEC:
		if (!mtmp->mcan && canseemon(mtmp) &&
			couldsee(mtmp->mx, mtmp->my) &&
			mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
		    int dmg = d(2,6);

		    pline("%s attacks you with a shocking gaze!", Monnam(mtmp));
		    stop_occupation();
		    if (Shock_resistance && rn2(20)) {
			pline_The("gaze doesn't shock you!");
			dmg = 0;
		    }
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(WAND_CLASS, AD_ELEC);
		    /*if((int) mtmp->m_lev > rn2(20))*/
		    if (!rn2(33)) /* new calculations --Amy */
			destroy_item(RING_CLASS, AD_ELEC);
		    if (dmg) mdamageu(mtmp, dmg);
		}
		break;
	    case AD_DRIN: /* not gaze/sight based. */
		if(!mtmp->mcan && couldsee(mtmp->mx, mtmp->my) &&
		  (!ublindf || ublindf->otyp != TOWEL)  &&
		  !mtmp->mspec_used){
		    pline("%s screeches at you!", Monnam(mtmp));
		    if (u.usleep){
			multi = -1;
			nomovemsg = "You wake.";
		    }
		    if (ABASE(A_INT) > ATTRMIN(A_INT) && !rn2(10)) {
		    /* adjattrib gives dunce cap message when appropriate */
			(void) adjattrib(A_INT, -1, FALSE);
			losespells();
			forget_map(0);
			docrt();
		    }
		    mtmp->mspec_used += ABASE(A_INT) * rn1(1,3);
		}
		break;
	    case AD_PLYS:
		if(!mtmp->mcan && mtmp->mcansee && canseemon(mtmp) && !mtmp->mspec_used && rn2(4) && 
		  multi>=0 && !((is_undead(youmonst.data)||is_demon(youmonst.data)) 
		  && is_undead(mtmp->data))){
			pline("%s aberrant stare frightens you to the %s!", 
			  s_suffix(Monnam(mtmp)), has_bones(youmonst.data)?"marrow":"core");
			if(Free_action){
			    pline("But you quickly regain composure.");
			} else {
			    int prlys = d((int)mattk->damn, (int)mattk->damd);
			    int numhelp, numseen;
			    nomul(-prlys,"paralyzed by a monster's gaze"); 
			    nomovemsg = 0;	/* default: "you can move again" */

			    if(!mtmp->cham && mtmp->data == &mons[PM_NOSFERATU] && 
			    !mtmp->mcan && !rn2(3)){ 
				numhelp = were_summon(mtmp->data, FALSE, &numseen, 0);
				pline("%s summons help!", Monnam(mtmp));
				if (numhelp > 0) {
				    if (numseen == 0)
				    You_feel("hemmed in.");
				} else pline("But none comes.");
			    }
			    mtmp->mspec_used += prlys*3/2 + rn2(prlys);
			}
		}
	    break;
	    case AD_HNGY:
		if(!mtmp->mcan && canseemon(mtmp) &&
		  couldsee(mtmp->mx, mtmp->my) && !is_fainted() &&
		  mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
		int hunger = 20 + d(3,4);

		mtmp->mspec_used = mtmp->mspec_used + (hunger + rn2(6));
		pline("%s gaze reminds you of delicious %s.",
		s_suffix(Monnam(mtmp)), fruitname(FALSE));
		morehungry(hunger);
		}
		break;
#ifdef PM_BEHOLDER /* work in progress */
#if 0
	    case AD_SLEE:
		if(!mtmp->mcan && canseemon(mtmp) &&
		   couldsee(mtmp->mx, mtmp->my) && mtmp->mcansee &&
		   multi >= 0 && !rn2(5) && !Sleep_resistance) {

		    fall_asleep(-rnd(10), TRUE);
		    pline("%s gaze makes you very sleepy...",
			  s_suffix(Monnam(mtmp)));
		}
		break;
#endif
	    case AD_SLOW:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee &&
		   (HFast & (INTRINSIC|TIMEOUT)) &&
		   !defends(AD_SLOW, uwep) && !rn2(4))

		    u_slow_down();
		    stop_occupation();
		break;
#endif
	    case AD_SLEE:
		if(!mtmp->mcan && canseemon(mtmp) &&
				mtmp->mcansee && !mtmp->mspec_used && rn2(3)) {
		    if (Displaced && rn2(3)) {
			if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
			    break;
		    }
		    if ((Invisible && rn2(3)) || rn2(4)) {
			if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
			break;
		    }
		    if (!Blind) pline("%s gazes directly at you!",Monnam(mtmp));
		    if(Reflecting && m_canseeu(mtmp) && !mtmp->mcan) {
			if(!Blind) {
		    	    (void) ureflects("%s gaze is reflected by your %s.",
		    			s_suffix(Monnam(mtmp)));
		    	    if (mon_reflects(mtmp,
		    			"The gaze is reflected away by %s %s!"))
				break;
			}
			if (sleep_monst(mtmp, rnd(10), -1) && !Blind)
			    pline("%s is put to sleep!", Monnam(mtmp));
			break;
		    } else if (Sleep_resistance) {
			pline("You yawn.");
		    } else {
			nomul(-rnd(10), "sleeping from a monster's gaze");
			u.usleep = 1;
			nomovemsg = "You wake up.";
			if (Blind)  You("are put to sleep!");
			else You("are put to sleep by %s!",mon_nam(mtmp));
		    }
		}
		break;
	    case AD_DETH:
		if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(18)) {
		    if (Displaced && rn2(3)) {
			if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
			    break;
		    }
		    if ((Invisible && rn2(3)) || rn2(4)) {
			if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
			break;
		    }
		    if ((!Blind) && !rn2(8)) pline("%s gazes directly at you!",Monnam(mtmp));
		    if(Reflecting && m_canseeu(mtmp) && !mtmp->mcan) {
			if(!Blind) {
		    	    (void) ureflects("%s gaze is reflected by your %s.",
		    			s_suffix(Monnam(mtmp)));
		    	    if (mon_reflects(mtmp,
		    			"The gaze is reflected away by %s %s!"))
				break;
			    pline("%s is killed by its own gaze of death!",
							Monnam(mtmp));
			}
			killed(mtmp);
			if (mtmp->mhp > 0) break;
			return 2;
		    } else if (is_undead(youmonst.data)) {
			/* Still does normal damage */
			pline("Was that the gaze of death?");
			break;
		    } else if (rn2(5) ) {
			/* Still does normal damage */
			pline("It is pitch black...");
			break;
		    } else if (Antimagic) {
			You("shudder momentarily...");
		    } else {
			You("die...");
			killer_format = KILLED_BY_AN;
			killer = "gaze of death";
			done(DIED);
		    }
		}
		break;
	    case AD_PHYS:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && rn2(3)) {
	                if (Displaced && rn2(3)) {
	                        if (!Blind) pline("%s gazes at your displaced image!",Monnam(mtmp));
	                        break;
	                }
	                if ((Invisible && rn2(3)) || rn2(4)) {
	                        if (!Blind) pline("%s gazes around, but misses you!",Monnam(mtmp));
	                        break;
	                }
	                if (!Blind) pline("%s gazes directly at you!",Monnam(mtmp));
	                pline("You are wracked with pains!");
	                mdamageu(mtmp, d(3,8));
	        }
	        break;
	    case AD_DRST:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && rn2(5)) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_STR, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_DRDX:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_DEX, mtmp->data->mname, 30);
	        }
	        break;
	    case AD_DRCO:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
	                pline("%s stares into your eyes...", Monnam(mtmp));
	                poisoned("The gaze", A_CON, mtmp->data->mname, 30);
	        }
	        break;
          case AD_DISE:
          case AD_PEST:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(12)) {
	                pline("%s leers down on you!", Monnam(mtmp));
			(void) digeasemu(mtmp); /* plus the normal damage */
	        }
	        break;
	    case AD_HALU:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !rn2(5)) 		{
		    boolean chg;
		    if (!Hallucination)
			You("suddenly see a mess of colors!");
		    chg = make_hallucinated(HHallucination + (int)mattk->damd*10,FALSE,0L);
		    You("%s.", chg ? "are getting very trippy" : "seem to get even more trippy");
		}
		break;
#if 0 /* 5lo: using the one from Biodiversity */
	    case AD_PLYS:
	        if(!mtmp->mcan && multi >= 0 && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(5)) {
	                pline("%s stares at you!", Monnam(mtmp));
	                if (Free_action) You("stiffen momentarily.");
	                else {
	                        You("are frozen by %s!", mon_nam(mtmp));
				nomovemsg = 0;
	                        nomul(-rnd(4), "paralyzed by a monster's gaze");
	                        exercise(A_DEX, FALSE);
	                }
	        }
	        break;
#endif
	    case AD_TLPT:
	        if(!mtmp->mcan && canseemon(mtmp) && mtmp->mcansee && !mtmp->mspec_used && !rn2(15)) {
	                pline("%s stares blinkingly at you!", Monnam(mtmp));
	                if(flags.verbose)
	                        Your("position suddenly seems very uncertain!");
	                teleX();
		}
		break;
	    default: /*impossible*/pline("Gaze attack %d?", mattk->adtyp);
		break;
	}

	/*if (randattackB == 1) {
		mattk->adtyp = AD_RBRE;
		randattackB = 0;
	}*/

	return(0);
}

/* return 1 charmed, 0 uncharmed */
int
mcharmu(mtmp, amt, always)
struct monst * mtmp;
int amt;
int always;
{
	if(!mtmp->mcan && !mtmp->mspec_used &&
	  (always || ( (mtmp->mhp<=mtmp->mhpmax/2 || mtmp->mhp<=10) &&
	  (!mtmp->mpeaceful || mtmp->mpeacetim)) )){
	    mtmp->mspec_used = mtmp->mspec_used + (amt + rn2(6));
	    if (rn2(30)+mtmp->m_lev > ACURR(A_CHA) + ACURR(A_INT)) {
		mtmp->malign = -15;
		mtmp->mpeaceful=1;
		if(!rnd(3)) mtmp->mpeacetim=0x7f;
		if (mtmp->mpeacetim != 0x7f){
		    int p = mtmp->mpeacetim+30*(1+rnd(mtmp->m_lev));
		    mtmp->mpeacetim = min(0x7e,p);
		}
		You("find %s charming.", mon_nam(mtmp));
		return 1;
	    } else
		You("find %s uncharming.", mon_nam(mtmp));
	}
	return 0;
}

#endif /* OVLB */
#ifdef OVL1

void
mdamageu(mtmp, n)	/* mtmp hits you for n points damage */
register struct monst *mtmp;
register int n;
{
	if (flags.iwbtg) {

		You("explode in a fountain of red pixels!");
		pline("GAME OVER - press R to try again");
		done_in_by(mtmp);

	}
#ifdef EASY_MODE
	/* sometimes you take less damage. The game is deadly enough already. --Amy */
	if (!rn2(3) && n >= 1) {n = n / 2; if (n < 1) n = 1;}
	if (!rn2(10) && n >= 1 && u.ulevel >= 10) {n = n / 3; if (n < 1) n = 1;}
	if (!rn2(20) && n >= 1 && u.ulevel >= 20) {n = n / 5; if (n < 1) n = 1;}
	if (!rn2(50) && n >= 1 && u.ulevel >= 30) {n = n / 10; if (n < 1) n = 1;}
#endif /* EASY_MODE */
	if (Invulnerable) n=0;
	if (n == 0) {
		pline("You are unharmed.");
		return;
	}

	/* WAC For consistency...DO be careful using techniques ;B */
	if (mtmp->mtame != 0 && tech_inuse(T_PRIMAL_ROAR)) {
		n *= 2; /* Double Damage! */
	}

#ifdef SHOW_DMG
	if (flags.showdmg) pline("[%d pts.]", n);
#endif
	flags.botl = 1; /* This needs to be AFTER the pline for botl to be 
	 		 * updated correctly -- Kelly Bailey
	 		 */

	if (Upolyd) {
		u.mh -= n;
		if (u.mh < 1) {                
			if (Polymorph_control || !rn2(3)) {
			    u.uhp -= mons[u.umonnum].mlevel;
			    u.uhpmax -= mons[u.umonnum].mlevel;
			    if (u.uhpmax < 1) u.uhpmax = 1;
			}
			rehumanize();
		}
	} else {
		u.uhplast = u.uhp;
		u.uhp -= n;
		if (u.uhp >= 1 && n > 0 && u.uhp*10 < u.uhpmax)	maybe_wail(); /* Wizard is about to die. --Amy */
		if(u.uhp < 1) done_in_by(mtmp);
	}
}

#endif /* OVL1 */
#ifdef OVLB

STATIC_OVL void
urustm(mon, obj)
register struct monst *mon;
register struct obj *obj;
{
	boolean vis;
	boolean is_acid;

	if (!mon || !obj) return; /* just in case */
	if (dmgtype(youmonst.data, AD_CORR))
	    is_acid = TRUE;
	else if (dmgtype(youmonst.data, AD_RUST))
	    is_acid = FALSE;
	else
	    return;

	vis = cansee(mon->mx, mon->my);

	if ((is_acid ? is_corrodeable(obj) : is_rustprone(obj)) &&
	    (is_acid ? obj->oeroded2 : obj->oeroded) < MAX_ERODE) {
		if (obj->greased || obj->oerodeproof || (obj->blessed && rn2(3))) {
		        if (vis) pline("Somehow, %s weapon is not affected.",
						s_suffix(mon_nam(mon)));
		    if (obj->greased && !rn2(2)) obj->greased = 0;
		} else {
		        if (vis) pline("%s %s%s!",
			        s_suffix(Monnam(mon)),
				aobjnam(obj, (is_acid ? "corrode" : "rust")),
			        (is_acid ? obj->oeroded2 : obj->oeroded)
				    ? " further" : "");
		    if (is_acid) obj->oeroded2++;
		    else obj->oeroded++;
		}
	}
}

#endif /* OVLB */
#ifdef OVL1

int
could_seduce(magr,mdef,mattk)
struct monst *magr, *mdef;
struct attack *mattk;
/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
{
	register struct permonst *pagr;
	boolean agrinvis, defperc;
	xchar genagr, gendef;

	if (is_animal(magr->data)) return (0);
	if(magr == &youmonst) {
		pagr = youmonst.data;
		agrinvis = (Invis != 0);
		genagr = poly_gender();
	} else {
		pagr = magr->data;
		agrinvis = magr->minvis;
		genagr = gender(magr);
	}
	if(mdef == &youmonst) {
		defperc = (See_invisible != 0);
		gendef = poly_gender();
	} else {
		defperc = perceives(mdef->data);
		gendef = gender(mdef);
	}

	if(agrinvis && !defperc
#ifdef SEDUCE
		&& mattk && mattk->adtyp != AD_SSEX
#endif
		)
		return 0;

/*	if(pagr->mlet != S_NYMPH
		&& ((pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS])
#ifdef SEDUCE
		    || (mattk && mattk->adtyp != AD_SSEX)
#endif
		   ))
		return 0; */

	if (mattk && mattk->adtyp != AD_SSEX && mattk->adtyp != AD_SEDU && mattk->adtyp != AD_SITM && pagr->mlet != S_NYMPH
&& pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS]) return 0;
	
	if(genagr == 1 - gendef)
		return 1;
	else
		return (pagr->mlet == S_NYMPH) ? 2 : 0;
}

int
could_seduceX(magr,mdef,mattk)
struct monst *magr, *mdef;
struct attack *mattk;
/* returns 0 if seduction impossible,
 *	   1 if fine,
 *	   2 if wrong gender for nymph */
{
	register struct permonst *pagr;
	boolean agrinvis, defperc;
	xchar genagr, gendef;

	if (is_animal(magr->data)) return (0);
	if(magr == &youmonst) {
		pagr = youmonst.data;
		agrinvis = (Invis != 0);
		genagr = poly_gender();
	} else {
		pagr = magr->data;
		agrinvis = magr->minvis;
		genagr = gender(magr);
	}
	if(mdef == &youmonst) {
		defperc = (See_invisible != 0);
		gendef = poly_gender();
	} else {
		defperc = perceives(mdef->data);
		gendef = gender(mdef);
	}

	if(agrinvis && !defperc
#ifdef SEDUCE
		&& mattk && mattk->adtyp != AD_SSEX
#endif
		)
		return 0;

/*	if(pagr->mlet != S_NYMPH
		&& ((pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS])
#ifdef SEDUCE
		    || (mattk && mattk->adtyp != AD_SSEX)
#endif
		   ))
		return 0; */

	/*if (mattk && mattk->adtyp != AD_SSEX && mattk->adtyp != AD_SEDU && mattk->adtyp != AD_SITM && pagr->mlet != S_NYMPH
&& pagr != &mons[PM_INCUBUS] && pagr != &mons[PM_SUCCUBUS]) return 0;*/
	
	if(genagr == 1 - gendef)
		return 1;
	else
		return (pagr->mlet == S_NYMPH) ? 2 : 0;
}

#endif /* OVL1 */
#ifdef OVLB

#ifdef SEDUCE
/* Returns 1 if monster teleported */
int
doseduce(mon)
register struct monst *mon;
{
	int monsterlev;
	register struct obj *ring, *nring;
	boolean fem = /*(mon->data == &mons[PM_SUCCUBUS])*/ (mon->female); /* otherwise incubus */
	char qbuf[QBUFSZ];

	if (mon->mcan || mon->mspec_used) {
		pline("%s acts as though %s has got a %sheadache.",
		      Monnam(mon), mhe(mon),
		      mon->mcan ? "severe " : "");
		return 0;
	}

	if (unconscious()) {
		pline("%s seems dismayed at your lack of response.",
		      Monnam(mon));
		return 0;
	}

	if (Blind) pline("It caresses you...");
	else You_feel("very attracted to %s.", mon_nam(mon));

	for(ring = invent; ring; ring = nring) {
	    nring = ring->nobj;
	    if (ring->otyp != RIN_ADORNMENT) continue;
	    if (fem) {
		if (rn2(20) < ACURR(A_CHA)) {
		    Sprintf(qbuf, "\"That %s looks pretty.  May I have it?\"",
			safe_qbuf("",sizeof("\"That  looks pretty.  May I have it?\""),
			xname(ring), simple_typename(ring->otyp), "ring"));
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else pline("%s decides she'd like your %s, and takes it.",
			Blind ? "She" : Monnam(mon), xname(ring));
		makeknown(RIN_ADORNMENT);
		if (ring==uleft || ring==uright) Ring_gone(ring);
		if (ring==uwep) setuwep((struct obj *)0, FALSE);
		if (ring==uswapwep) setuswapwep((struct obj *)0, FALSE);
		if (ring==uquiver) setuqwep((struct obj *)0);
		freeinv(ring);
		(void) mpickobj(mon,ring);
	    } else {
		char buf[BUFSZ];

		if (uleft && uright && uleft->otyp == RIN_ADORNMENT
				&& uright->otyp==RIN_ADORNMENT)
			break;
		if (ring==uleft || ring==uright) continue;
		if (rn2(20) < ACURR(A_CHA)) {
		    Sprintf(qbuf,"\"That %s looks pretty.  Would you wear it for me?\"",
			safe_qbuf("",
			    sizeof("\"That  looks pretty.  Would you wear it for me?\""),
			    xname(ring), simple_typename(ring->otyp), "ring"));
		    makeknown(RIN_ADORNMENT);
		    if (yn(qbuf) == 'n') continue;
		} else {
		    pline("%s decides you'd look prettier wearing your %s,",
			Blind ? "He" : Monnam(mon), xname(ring));
		    pline("and puts it on your finger.");
		}
		makeknown(RIN_ADORNMENT);
		if (!uright) {
		    pline("%s puts %s on your right %s.",
			Blind ? "He" : Monnam(mon), the(xname(ring)), body_part(HAND));
		    setworn(ring, RIGHT_RING);
		} else if (!uleft) {
		    pline("%s puts %s on your left %s.",
			Blind ? "He" : Monnam(mon), the(xname(ring)), body_part(HAND));
		    setworn(ring, LEFT_RING);
		} else if (uright && uright->otyp != RIN_ADORNMENT) {
		    Strcpy(buf, xname(uright));
		    pline("%s replaces your %s with your %s.",
			Blind ? "He" : Monnam(mon), buf, xname(ring));
		    Ring_gone(uright);
		    setworn(ring, RIGHT_RING);
		} else if (uleft && uleft->otyp != RIN_ADORNMENT) {
		    Strcpy(buf, xname(uleft));
		    pline("%s replaces your %s with your %s.",
			Blind ? "He" : Monnam(mon), buf, xname(ring));
		    Ring_gone(uleft);
		    setworn(ring, LEFT_RING);
		} else impossible("ring replacement");
		Ring_on(ring);
		prinv((char *)0, ring, 0L);
	    }
	}

	if (!uarmc && !uarmf && !uarmg && !uarms && !uarmh
#ifdef TOURIST
								&& !uarmu
#endif
									)
		pline("%s murmurs sweet nothings into your ear.",
			Blind ? (fem ? "She" : "He") : Monnam(mon));
	else
		pline("%s murmurs in your ear, while helping you undress.",
			Blind ? (fem ? "She" : "He") : Monnam(mon));
	mayberem(uarmc, cloak_simple_name(uarmc));
	if(!uarmc)
		mayberem(uarm, "suit");
	mayberem(uarmf, "boots");
	if(!uwep || !welded(uwep))
		mayberem(uarmg, "gloves");
	/* 
	 * STEPHEN WHITE'S NEW CODE
	 *
	 * This will cause a game crash should the if statment be removed.
	 * It will try to de-referance a pointer that doesn't exist should 
	 * the player not have a shield
	 */

	if (uarms) mayberem(uarms, "shield");
	mayberem(uarmh, "helmet");
#ifdef TOURIST
	if(!uarmc && !uarm)
		mayberem(uarmu, "shirt");
#endif

	if (uarm || uarmc) {
		verbalize("You're such a %s; I wish...",
				flags.female ? "sweet lady" : "nice guy");
		if (!tele_restrict(mon)) (void) rloc(mon, TRUE);
		return 1;
	}
	if (u.ualign.type == A_CHAOTIC)
		adjalign(1);

	/* by this point you have discovered mon's identity, blind or not... */
	pline("Time stands still while you and %s lie in each other's arms...",
		noit_mon_nam(mon));
	/* Well,  IT happened ... */
	u.uconduct.celibacy++;
	
	if (rn2(135) > ACURR(A_CHA) + ACURR(A_INT)) /*much higher chance of negative outcome now --Amy */ {
		/* Don't bother with mspec_used here... it didn't get tired! */
		pline("%s seems to have enjoyed it more than you...",
			noit_Monnam(mon));
		switch (rn2(5)) {
			case 0: You_feel("drained of energy.");
				u.uen = 0;
				u.uenmax -= rnd(Half_physical_damage ? 5 : 10);
			        exercise(A_CON, FALSE);
				if (u.uenmax < 0) u.uenmax = 0;
				break;
			case 1: You("are down in the dumps.");
				(void) adjattrib(A_CON, -1, TRUE);
			        exercise(A_CON, FALSE);
				flags.botl = 1;
				break;
			case 2: Your("senses are dulled.");
				(void) adjattrib(A_WIS, -1, TRUE);
			        exercise(A_WIS, FALSE);
				flags.botl = 1;
				break;
			case 3:
				if (!Drain_resistance) {
				    You_feel("out of shape.");
				    losexp("overexertion", FALSE);
				} else {
				    You("have a curious feeling...");
				}
				break;
			case 4: {
				int tmp;
				You_feel("exhausted.");
			        exercise(A_STR, FALSE);
				tmp = rn1(10, 6);
				if(Half_physical_damage) tmp = (tmp+1) / 2;
				losehp(tmp, "exhaustion", KILLED_BY);
				break;
			}
		}
	} else {
		mon->mspec_used = rnd(100); /* monster is worn out */
		You("seem to have enjoyed it more than %s...",
		    noit_mon_nam(mon));
		switch (rn2(5)) {
		case 0: You_feel("raised to your full potential.");
			exercise(A_CON, TRUE);
			u.uen = (u.uenmax += rnd(5));
			break;
		case 1: You_feel("good enough to do it again.");
			(void) adjattrib(A_CON, 1, TRUE);
			exercise(A_CON, TRUE);
			flags.botl = 1;
			break;
		case 2: You("will always remember %s...", noit_mon_nam(mon));
			(void) adjattrib(A_WIS, 1, TRUE);
			exercise(A_WIS, TRUE);
			flags.botl = 1;
			break;
		case 3: pline("That was a very educational experience.");
			pluslvl(FALSE);
			exercise(A_WIS, TRUE);
			break;
		case 4: You_feel("restored to health!");
			u.uhp = u.uhpmax;
			if (Upolyd) u.mh = u.mhmax;
			exercise(A_STR, TRUE);
			flags.botl = 1;
			break;
		}
	}

	if (mon->mtame) /* don't charge */ ;
	else if (rn2(20) < ACURR(A_CHA)) {
		pline("%s demands that you pay %s, but you refuse...",
			noit_Monnam(mon),
			Blind ? (fem ? "her" : "him") : mhim(mon));
	} else if (u.umonnum == PM_LEPRECHAUN)
		pline("%s tries to take your money, but fails...",
				noit_Monnam(mon));
	else {
#ifndef GOLDOBJ
		long cost;

		if (u.ugold > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)u.ugold + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > u.ugold) cost = u.ugold;
		if (!cost) verbalize("It's on the house!");
		else {
		    pline("%s takes %ld %s for services rendered!",
			    noit_Monnam(mon), cost, currency(cost));
		    u.ugold -= cost;
		    mon->mgold += cost;
		    flags.botl = 1;
		}
#else
		long cost;
                long umoney = money_cnt(invent);

		if (umoney > (long)LARGEST_INT - 10L)
			cost = (long) rnd(LARGEST_INT) + 500L;
		else
			cost = (long) rnd((int)umoney + 10) + 500L;
		if (mon->mpeaceful) {
			cost /= 5L;
			if (!cost) cost = 1L;
		}
		if (cost > umoney) cost = umoney;
		if (!cost) verbalize("It's on the house!");
		else { 
		    pline("%s takes %ld %s for services rendered!",
			    noit_Monnam(mon), cost, currency(cost));
                    money2mon(mon, cost);
		    flags.botl = 1;
		}
#endif
	}
	if (!rn2(10)) mon->mcan = 1; /* monster is worn out; chance is much higher now --Amy */
	if (!tele_restrict(mon)) (void) rloc(mon, TRUE);
	return 1;
}

STATIC_OVL void
mayberem(obj, str)
register struct obj *obj;
const char *str;
{
	char qbuf[QBUFSZ];

	if (!obj || !obj->owornmask) return;

	if (rn2(120) < ACURR(A_CHA)) { /*much lower chance for the player to resist --Amy*/
		Sprintf(qbuf,"\"Shall I remove your %s, %s?\"",
			str,
			(!rn2(2) ? "lover" : !rn2(2) ? "dear" : "sweetheart"));
		if (yn(qbuf) == 'n') return;
	} else {
		char hairbuf[BUFSZ];

		Sprintf(hairbuf, "let me run my fingers through your %s",
			body_part(HAIR));
		verbalize("Take off your %s; %s.", str,
			(obj == uarm)  ? "let's get a little closer" :
			(obj == uarmc || obj == uarms) ? "it's in the way" :
			(obj == uarmf) ? "let me rub your feet" :
			(obj == uarmg) ? "they're too clumsy" :
#ifdef TOURIST
			(obj == uarmu) ? "let me massage you" :
#endif
			/* obj == uarmh */
			hairbuf);
	}
	remove_worn_item(obj, TRUE);
}
#endif  /* SEDUCE */

#endif /* OVLB */

#ifdef OVL1

STATIC_OVL int
passiveum(olduasmon,mtmp,mattk)
struct permonst *olduasmon;
register struct monst *mtmp;
register struct attack *mattk;
{
	int i, tmp;

	for(i = 0; ; i++) {
	    if(i >= NATTK) return 1;
	    if (olduasmon->mattk[i].aatyp == AT_NONE ||
	    		olduasmon->mattk[i].aatyp == AT_BOOM) break;
	}
	if (olduasmon->mattk[i].damn)
	    tmp = d((int)olduasmon->mattk[i].damn,
				    (int)olduasmon->mattk[i].damd);
	else if(olduasmon->mattk[i].damd)
	    tmp = d( ((int)olduasmon->mlevel / 5)+1, (int)olduasmon->mattk[i].damd); /* Players polymorphed into blue slimes or similar stuff aren't supposed to be OP. --Amy */
	else
	    tmp = 0;

	/* These affect the enemy even if you were "killed" (rehumanized) */
	switch(olduasmon->mattk[i].adtyp) {
	    case AD_ACID:
		if (!rn2(2)) {
		    pline("%s is splashed by your acid!", Monnam(mtmp));
		    if (resists_acid(mtmp)) {
			pline("%s is not affected.", Monnam(mtmp));
			tmp = 0;
		    }
		} else tmp = 0;
		if (!rn2(30)) erode_armor(mtmp, TRUE);
		if (!rn2(6)) erode_obj(MON_WEP(mtmp), TRUE, TRUE);
		goto assess_dmg;
	    case AD_STON: /* cockatrice */
	    {
		long protector = attk_protection((int)mattk->aatyp),
		     wornitems = mtmp->misc_worn_check;

		/* wielded weapon gives same protection as gloves here */
		if (MON_WEP(mtmp) != 0) wornitems |= W_ARMG;

		if (!resists_ston(mtmp) && (protector == 0L ||
			(protector != ~0L &&
			    (wornitems & protector) != protector))) {
		    if (poly_when_stoned(mtmp->data)) {
			mon_to_stone(mtmp);
			return (1);
		    }
		    pline("%s turns to stone!", Monnam(mtmp));
		    stoned = 1;
		    xkilled(mtmp, 0);
		    if (mtmp->mhp > 0) return 1;
		    return 2;
		}
		return 1;
	    }
	    case AD_ENCH:	/* KMH -- remove enchantment (disenchanter) */
	    	if (otmp) {
	    	    (void) drain_item(otmp);
	    	    /* No message */
	    	}
	    	return (1);
	    default:
		break;
	}
	if (!Upolyd) return 1;

	/* These affect the enemy only if you are still a monster */
	if (rn2(3)) switch(youmonst.data->mattk[i].adtyp) {
	    case AD_PHYS:
	    	if (youmonst.data->mattk[i].aatyp == AT_BOOM) {
	    	    You("explode!");
	    	    /* KMH, balance patch -- this is okay with unchanging */
	    	    rehumanize();
	    	    goto assess_dmg;
	    	}
	    	break;
	    case AD_PLYS: /* Floating eye */
		if (tmp > 127) tmp = 127;
		if (u.umonnum == PM_FLOATING_EYE) {
		    if (!rn2(4)) tmp = 127;
		    if (mtmp->mcansee && haseyes(mtmp->data) && rn2(3) &&
				(perceives(mtmp->data) || !Invis)) {
			if (Blind)
			    pline("As a blind %s, you cannot defend yourself.",
							youmonst.data->mname);
		        else {
			    if (mon_reflects(mtmp,
					    "Your gaze is reflected by %s %s."))
				return 1;
			    pline("%s is frozen by your gaze!", Monnam(mtmp));
			    mtmp->mcanmove = 0;
			    mtmp->mfrozen = tmp;
			    return 3;
			}
		    }
		} else { /* gelatinous cube */
		    pline("%s is frozen by you.", Monnam(mtmp));
		    mtmp->mcanmove = 0;
		    mtmp->mfrozen = tmp;
		    return 3;
		}
		return 1;
	    case AD_COLD: /* Brown mold or blue jelly */
		if (resists_cold(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is mildly chilly.", Monnam(mtmp));
		    golemeffects(mtmp, AD_COLD, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is suddenly very cold!", Monnam(mtmp));
		u.mh += tmp / 2;
		if (u.mhmax < u.mh) u.mhmax = u.mh;
		if (u.mhmax > ((youmonst.data->mlevel+1) * 8) && !rn2(25) ) /* slow down farming --Amy */
		    (void)split_mon(&youmonst, mtmp);
		break;
	    case AD_STUN: /* Yellow mold */
		if (!mtmp->mstun) {
		    mtmp->mstun = 1;
		    pline("%s %s.", Monnam(mtmp),
			  makeplural(stagger(mtmp->data, "stagger")));
		}
		tmp = 0;
		break;
	    case AD_SCLD:
	    case AD_FIRE: /* Red mold */
		if (resists_fire(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is mildly warm.", Monnam(mtmp));
		    golemeffects(mtmp, AD_FIRE, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is suddenly very hot!", Monnam(mtmp));
		break;
	    case AD_ELEC:
		if (resists_elec(mtmp)) {
		    shieldeff(mtmp->mx, mtmp->my);
		    pline("%s is slightly tingled.", Monnam(mtmp));
		    golemeffects(mtmp, AD_ELEC, tmp);
		    tmp = 0;
		    break;
		}
		pline("%s is jolted with your electricity!", Monnam(mtmp));
		break;
	    default: tmp = 0;
		break;
	}
	else tmp = 0;

    assess_dmg:
	if((mtmp->mhp -= tmp) <= 0) {
		pline("%s dies!", Monnam(mtmp));
		xkilled(mtmp,0);
		if (mtmp->mhp > 0) return 1;
		return 2;
	}
	return 1;
}

#endif /* OVL1 */
#ifdef OVLB

#include "edog.h"
struct monst *
cloneu()
{
	register struct monst *mon;
	int mndx = monsndx(youmonst.data);

	if (u.mh <= 1) return(struct monst *)0;
	if (mvitals[mndx].mvflags & G_EXTINCT) return(struct monst *)0;
	mon = makemon(youmonst.data, u.ux, u.uy, NO_MINVENT|MM_EDOG);
	mon = christen_monst(mon, plname);
	initedog(mon);
	mon->m_lev = youmonst.data->mlevel;
	mon->mhpmax = u.mhmax;
	mon->mhp = u.mh / 2;
	u.mh -= mon->mhp;
	flags.botl = 1;
	return(mon);
}

#endif /* OVLB */

/*mhitu.c*/
