/*	SCCS Id: @(#)artifact.c 3.4	2003/08/11	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "artifact.h"
#ifdef OVLB
#include "artilist.h"
#else
STATIC_DCL struct artifact artilist[];
#endif
/*
 * Note:  both artilist[] and artiexist[] have a dummy element #0,
 *	  so loops over them should normally start at #1.  The primary
 *	  exception is the save & restore code, which doesn't care about
 *	  the contents, just the total size.
 */

extern boolean notonhead;	/* for long worms */

#define get_artifact(o) \
		(((o)&&(o)->oartifact) ? &artilist[(o)->oartifact] : 0)

STATIC_DCL int FDECL(spec_applies, (const struct artifact *,struct monst *));
STATIC_DCL int FDECL(arti_invoke, (struct obj*));
STATIC_DCL boolean FDECL(Mb_hit, (struct monst *magr,struct monst *mdef,
				  struct obj *,int *,int,BOOLEAN_P,char *));

/* The amount added to the victim's total hit points to insure that the
   victim will be killed even after damage bonus/penalty adjustments.
   Most such penalties are small, and 200 is plenty; the exception is
   half physical damage.  3.3.1 and previous versions tried to use a very
   large number to account for this case; now, we just compute the fatal
   damage by adding it to 2 times the total hit points instead of 1 time.
   Note: this will still break if they have more than about half the number
   of hit points that will fit in a 15 bit integer. */
#define FATAL_DAMAGE_MODIFIER 200

#ifndef OVLB
STATIC_DCL int spec_dbon_applies;
STATIC_DCL int artidisco[NROFARTIFACTS];
#else	/* OVLB */
/* coordinate effects from spec_dbon() with messages in artifact_hit() */
STATIC_OVL int spec_dbon_applies = 0;

/* flags including which artifacts have already been created */
static boolean artiexist[1+NROFARTIFACTS+1];
/* and a discovery list for them (no dummy first entry here) */

STATIC_OVL int artidisco[NROFARTIFACTS];

STATIC_DCL void NDECL(hack_artifacts);
STATIC_DCL boolean FDECL(attacks, (int,struct obj *));

int FDECL(dopetmenu, (const char *,struct obj *));
int FDECL(dolordsmenu, (const char *,struct obj *));

boolean
CountsAgainstGifts(x)
int x;
{
	return (x != ART_WARFORGER && \
			ART_KEY_OF_LAW && \
			ART_KEY_OF_NEUTRALITY && \
			ART_KEY_OF_CHAOS && \
			ART_HAND_OF_VECNA && \
			ART_NIGHTHORN && \
			ART_EYE_OF_THE_BEHOLDER && \
			ART_THIEFBANE && \
			ART_WAND_OF_MIGHT && \
			ART_KEY_OF_ACCESS && \
			ART_BURNED_MOTH_RELAY && \
			ART_SCALES_OF_THE_DRAGON_LORD);
}
/* handle some special cases; must be called after role_init() */
STATIC_OVL void
hack_artifacts()
{
	struct artifact *art;
	int alignmnt = aligns[flags.initalign].value;

	/* Fix up the alignments of "gift" artifacts */
	for (art = artilist+1; art->otyp; art++)
	    if (art->role == Role_switch && art->alignment != A_NONE)
		art->alignment = alignmnt;

	/* Excalibur can be used by any lawful character, not just knights
	 * So can Soulthief for chaotic characters, for that matter */
	if (!Role_if(PM_KNIGHT)) {
	    artilist[ART_EXCALIBUR].role = NON_PM;
	    artilist[ART_SOULTHIEF].role = NON_PM;
	}

#if 0
	/* Fix up the gifts */
	if (urole.gift1arti) {
		artilist[urole.gift1arti].alignment = alignmnt;
		artilist[urole.gift1arti].role = Role_switch;
	}
	if (urole.gift2arti) {
		artilist[urole.gift2arti].alignment = alignmnt;
		artilist[urole.gift2arti].role = Role_switch;
	}
#endif
	artilist[ART_MANTLE_OF_HEAVEN].otyp = find_cope();
	artilist[ART_VESTMENT_OF_HELL].otyp = find_opera_cloak();
	/* Fix up the quest artifact */
	if(Role_if(PM_NOBLEMAN) && Race_if(PM_VAMPIRE)){
		urole.questarti = ART_VESTMENT_OF_HELL;
	}
	if (urole.questarti) {
	    artilist[urole.questarti].alignment = alignmnt;
	    artilist[urole.questarti].role = Role_switch;
	}
	return;
}

/* zero out the artifact existence list */
void
init_artifacts()
{
	(void) memset((genericptr_t) artiexist, 0, sizeof artiexist);
	(void) memset((genericptr_t) artidisco, 0, sizeof artidisco);
	hack_artifacts();
}

/* Post u_init() initialization */
void
init_artifacts1()
{
#if 0
    /* KMH -- Should be at least skilled in first artifact gifts */
    if (urole.gift1arti &&
	    (objects[artilist[urole.gift1arti].otyp].oc_class == WEAPON_CLASS ||
	     objects[artilist[urole.gift1arti].otyp].oc_class == TOOL_CLASS)) {
	int skill = objects[artilist[urole.gift1arti].otyp].oc_skill;

	if (skill > P_NONE && P_SKILL(skill) < P_UNSKILLED)
	    P_SKILL(skill) = P_UNSKILLED;
	if (skill > P_NONE && P_MAX_SKILL(skill) < P_SKILLED) {
	    pline("Warning: %s should be at least skilled.  Fixing...",
		    artilist[urole.gift1arti].name);
		P_MAX_SKILL(skill) = P_SKILLED;
	}
    }
    if (urole.gift2arti &&
	    (objects[artilist[urole.gift2arti].otyp].oc_class == WEAPON_CLASS ||
	     objects[artilist[urole.gift2arti].otyp].oc_class == TOOL_CLASS)) {
	int skill = objects[artilist[urole.gift2arti].otyp].oc_skill;

	if (skill > P_NONE && P_SKILL(skill) < P_UNSKILLED)
	    P_SKILL(skill) = P_UNSKILLED;
	if (skill > P_NONE && P_MAX_SKILL(skill) < P_SKILLED) {
	    pline("Warning: %s should be at least skilled.  Fixing...",
		    artilist[urole.gift1arti].name);
	    P_MAX_SKILL(skill) = P_SKILLED;
	}
    }
#endif 
    /* KMH -- Should be expert in quest artifact */
    if (urole.questarti &&
	    (objects[artilist[urole.questarti].otyp].oc_class == WEAPON_CLASS ||
	     objects[artilist[urole.questarti].otyp].oc_class == TOOL_CLASS)) {
	int skill = objects[artilist[urole.questarti].otyp].oc_skill;

	if (skill > P_NONE && P_SKILL(skill) < P_UNSKILLED)
	    P_SKILL(skill) = P_UNSKILLED;
	if (skill > P_NONE && P_MAX_SKILL(skill) < P_EXPERT) {
	    pline("Warning: %s should be at least expert.  Fixing...",
		    artilist[urole.questarti].name);
	    P_MAX_SKILL(skill) = P_EXPERT;
	}
    }
}

void
save_artifacts(fd)
int fd;
{
	bwrite(fd, (genericptr_t) artiexist, sizeof artiexist);
	bwrite(fd, (genericptr_t) artidisco, sizeof artidisco);
}

void
restore_artifacts(fd)
int fd;
{
	mread(fd, (genericptr_t) artiexist, sizeof artiexist);
	mread(fd, (genericptr_t) artidisco, sizeof artidisco);
	hack_artifacts();	/* redo non-saved special cases */
}

const char *
artiname(artinum)
int artinum;
{
	if (artinum <= 0 || artinum > NROFARTIFACTS) return("");
	return(artilist[artinum].name);
}

/*
   Make an artifact.  If a specific alignment is specified, then an object of
   the appropriate alignment is created from scratch, or 0 is returned if
   none is available.  (If at least one aligned artifact has already been
   given, then unaligned ones also become eligible for this.)
   If no alignment is given, then 'otmp' is converted
   into an artifact of matching type, or returned as-is if that's not possible.
   For the 2nd case, caller should use ``obj = mk_artifact(obj, A_NONE);''
   for the 1st, ``obj = mk_artifact((struct obj *)0, some_alignment);''.
 */
struct obj *
mk_artifact(otmp, alignment)
struct obj *otmp;	/* existing object; ignored if alignment specified */
aligntyp alignment;	/* target alignment, or A_NONE */
{
	const struct artifact *a;
	int n, m;
	boolean by_align = (alignment != A_NONE);
	int o_typ = (by_align || !otmp) ? 0 : otmp->otyp;
	boolean unique = !by_align && otmp && objects[o_typ].oc_unique;
	int eligible[NROFARTIFACTS];


#if 0
	/* KMH, role patch -- Try first or second gift */
	m = urole.gift1arti;
	if (by_align && m && !artiexist[m]) {
		a = (struct artifact *)&artilist[m];
		goto make_artif;
	}
	m = urole.gift2arti;
	if (by_align && m && !artiexist[m]) {
		a = (struct artifact *)&artilist[m];
		goto make_artif;
	}
#endif

	/* gather eligible artifacts */
	for (n = 0, a = artilist+1, m = 1; a->otyp; a++, m++)
	    if ((!by_align ? a->otyp == o_typ :
		    (a->alignment == alignment ||
			(a->alignment == A_NONE && u.ugifts > 0))) &&
		(!(a->spfx & SPFX_NOGEN) || unique) && !artiexist[m]) {
		/*
		 * [ALI] The determination of whether an artifact is
		 * hostile to the player is a little more complex in
		 * Slash'EM than Vanilla since there are artifacts
		 * which are hostile to humans (eg., Deathsword) which
		 * aren't aligned to any race.
		 * Nevertheless, the rule remains the same: Gods don't
		 * grant artifacts which would be hostile to the player
		 * _in their normal form_.
		 */
		boolean hostile = FALSE;
		if (by_align) {
		    if (a->race != NON_PM && race_hostile(&mons[a->race]))
			hostile = TRUE;		/* enemies' equipment */
		    else if (a->spfx & SPFX_DBONUS) {
			struct artifact tmp;

			tmp = *a;
			tmp.spfx &= SPFX_DBONUS;
			if (Upolyd)
			    set_mon_data(&youmonst, &upermonst, 0);
			if (spec_applies(&tmp, &youmonst))
			    hostile = TRUE;	/* can blast unpolyd player */
			if (Upolyd)
			    set_mon_data(&youmonst, &mons[u.umonnum], 0);
		    }
		}
		if (hostile)
		    continue;
		if (by_align && Role_if(a->role))
		    goto make_artif;	/* 'a' points to the desired one */
		else if(by_align && Role_if(PM_PIRATE)) continue; /* pirates are not gifted artifacts */
		else
		    eligible[n++] = m;
	    }

	if (n) {		/* found at least one candidate */
	    m = eligible[rn2(n)];	/* [0..n-1] */
	    a = &artilist[m];

	    /* make an appropriate object if necessary, then christen it */
make_artif: if (by_align) {
		    otmp = mksobj((int)a->otyp, TRUE, FALSE);
	    }
	    otmp = oname(otmp, a->name);
	    otmp->oartifact = m;
	    artiexist[m] = TRUE;
	} else {
	    /* nothing appropriate could be found; return the original object */
	    if (by_align) otmp = 0;	/* (there was no original object) */
	}
	return otmp;
}

/*
 * Returns the full name (with articles and correct capitalization) of an
 * artifact named "name" if one exists, or NULL, it not.
 * The given name must be rather close to the real name for it to match.
 * The object type of the artifact is returned in otyp if the return value
 * is non-NULL.
 */
const char*
artifact_name(name, otyp)
const char *name;
int *otyp;
{
    register const struct artifact *a;
    register const char *aname;

    if(!strncmpi(name, "the ", 4)) name += 4;
    if(!strncmpi(name, "poisoned ", 9)) name += 9;

    for (a = artilist+1; a->otyp; a++) {
	aname = a->name;
	if(!strncmpi(aname, "the ", 4)) aname += 4;
	if(!strcmpi(name, aname)) {
	    *otyp = a->otyp;
	    return a->name;
	}
    }

    return (char *)0;
}
/* 5lo: Function for the Forge */
int
artifact_name2no(name)
const char *name;
{
	register const struct artifact *a;
	register int i;
	register const char *aname;

	if (!strncmpi(name,"the ",4)) name += 4;
	
	for (a = artilist+1,i=1; a->otyp; a++,i++) {
		aname = a->name;
		if (!strncmpi(aname,"the ",4)) aname += 4;
		if (!strcmpi(name, aname)) {
			return(i);
		}
	}
    
    return(0);
}

boolean
exist_artifact(otyp, name)
register int otyp;
register const char *name;
{
	register const struct artifact *a;
	register boolean *arex;

	if (otyp && *name)
	    for (a = artilist+1,arex = artiexist+1; a->otyp; a++,arex++)
		if ((int) a->otyp == otyp && !strcmp(a->name, name))
		    return *arex;
	return FALSE;
}

void
artifact_exists(otmp, name, mod)
register struct obj *otmp;
register const char *name;
register boolean mod;
{
	register const struct artifact *a;

	if (otmp && *name)
	    for (a = artilist+1; a->otyp; a++)
		if (a->otyp == otmp->otyp && !strcmp(a->name, name)) {
		    register int m = a - artilist;
		    otmp->oartifact = (mod ? m : 0);
		    if (mod) {
			otmp->quan = 1; /* guarantee only one of this artifact */
#ifdef UNPOLYPILE	/* Artifacts are immune to unpolypile --ALI */
			if (is_hazy(otmp)) {
			    (void) stop_timer(UNPOLY_OBJ, (genericptr_t) otmp);
			    otmp->oldtyp = STRANGE_OBJECT;
			}
#endif
		    }
		    otmp->age = 0;
		    if(otmp->otyp == RIN_INCREASE_DAMAGE)
			otmp->spe = 0;
		    artiexist[m] = mod;
		    break;
		}
	return;
}

int
nartifact_exist()
{
    int a = 0;
    int n = SIZE(artiexist);

    while(n > 1)
	if(artiexist[--n] && CountsAgainstGifts(n)) a++;

    return a;
}
#endif /* OVLB */
#ifdef OVL0

boolean
spec_ability(otmp, abil)
struct obj *otmp;
unsigned long abil;
{
	const struct artifact *arti = get_artifact(otmp);

	return((boolean)(arti && (arti->spfx & abil)));
}

/* used so that callers don't need to known about SPFX_ codes */
boolean
confers_luck(obj)
struct obj *obj;
{
    /* might as well check for this too */
    if (obj->otyp == LUCKSTONE) return TRUE;

    if (obj->otyp == FEDORA && obj == uarmh) return TRUE;

    return (obj->oartifact && spec_ability(obj, SPFX_LUCK));
}

/* used to check whether a monster is getting reflection from an artifact */
boolean
arti_reflects(obj)
struct obj *obj;
{
    const struct artifact *arti = get_artifact(obj);

    if (arti) {      
	/* while being worn */
	if ((obj->owornmask & ~W_ART) && (arti->spfx & SPFX_REFLECT))
	    return TRUE;
	/* just being carried */
	if (arti->cspfx & SPFX_REFLECT) return TRUE;
    }
    return FALSE;
}

#endif /* OVL0 */
#ifdef OVLB

boolean
restrict_name(otmp, name)  /* returns 1 if name is restricted for otmp->otyp */
register struct obj *otmp;
register const char *name;
{
	register const struct artifact *a;
	register const char *aname;

	if (!*name) return FALSE;
	if (!strncmpi(name, "the ", 4)) name += 4;

		/* Since almost every artifact is SPFX_RESTR, it doesn't cost
		   us much to do the string comparison before the spfx check.
		   Bug fix:  don't name multiple elven daggers "Sting".
		 */
	for (a = artilist+1; a->otyp; a++) {
	    /*if (a->otyp != otmp->otyp) continue;*/ /* artifact naming bug --Amy */
	    aname = a->name;
	    if (!strncmpi(aname, "the ", 4)) aname += 4;
	    if (!strcmp(aname, name))
		return ((boolean)((a->spfx & (SPFX_NOGEN|SPFX_RESTR)) != 0 ||
			otmp->quan > 1L));
	}

	return FALSE;
}

STATIC_OVL boolean
attacks(adtyp, otmp)
register int adtyp;
register struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return((boolean)(weap->attk.adtyp == adtyp));
	return FALSE;
}

boolean
defends(adtyp, otmp)
register int adtyp;
register struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return((boolean)(weap->defn.adtyp == adtyp));
	return FALSE;
}

/* used for monsters */
boolean
protects(adtyp, otmp)
int adtyp;
struct obj *otmp;
{
	register const struct artifact *weap;

	if ((weap = get_artifact(otmp)) != 0)
		return (boolean)(weap->cary.adtyp == adtyp);
	return FALSE;
}

/*
 * a potential artifact has just been worn/wielded/picked-up or
 * unworn/unwielded/dropped.  Pickup/drop only set/reset the W_ART mask.
 */
void
set_artifact_intrinsic(otmp,on,wp_mask)
register struct obj *otmp;
boolean on;
long wp_mask;
{
	long *mask = 0;
	register const struct artifact *oart = get_artifact(otmp);
	uchar dtyp;
	long spfx;

	if (!oart) return;

	/* effects from the defn field */
	dtyp = (wp_mask != W_ART) ? oart->defn.adtyp : oart->cary.adtyp;

	if (dtyp == AD_FIRE)
	    mask = &EFire_resistance;
	else if (dtyp == AD_COLD)
	    mask = &ECold_resistance;
	else if (dtyp == AD_ELEC)
	    mask = &EShock_resistance;
	else if (dtyp == AD_ACID)
	    mask = &EAcid_resistance;
	else if (dtyp == AD_MAGM)
	    mask = &EAntimagic;
	else if (dtyp == AD_DISN)
	    mask = &EDisint_resistance;
	else if (dtyp == AD_DRST)
	    mask = &EPoison_resistance;
	else if (dtyp == AD_DRLI)
	    mask = &EDrain_resistance;

	if (mask && wp_mask == W_ART && !on) {
	    /* find out if some other artifact also confers this intrinsic */
	    /* if so, leave the mask alone */
	    register struct obj* obj;
	    for(obj = invent; obj; obj = obj->nobj)
		if(obj != otmp && obj->oartifact) {
		    register const struct artifact *art = get_artifact(obj);
		    if(art->cary.adtyp == dtyp) {
			mask = (long *) 0;
			break;
		    }
		}
	}
	if(mask) {
	    if (on) *mask |= wp_mask;
	    else *mask &= ~wp_mask;
	}

	/* intrinsics from the spfx field; there could be more than one */
	spfx = (wp_mask != W_ART) ? oart->spfx : oart->cspfx;
	if(spfx && wp_mask == W_ART && !on) {
	    /* don't change any spfx also conferred by other artifacts */
	    register struct obj* obj;
	    for(obj = invent; obj; obj = obj->nobj)
		if(obj != otmp && obj->oartifact) {
		    register const struct artifact *art = get_artifact(obj);
		    spfx &= ~art->cspfx;
		}
	}

	if (spfx & SPFX_SEARCH) {
	    if(on) ESearching |= wp_mask;
	    else ESearching &= ~wp_mask;
	}
	if (spfx & SPFX_HALRES) {
	    /* make_hallucinated must (re)set the mask itself to get
	     * the display right */
	    /* restoring needed because this is the only artifact intrinsic
	     * that can print a message--need to guard against being printed
	     * when restoring a game
	     */
	    (void) make_hallucinated((long)!on, restoring ? FALSE : TRUE, wp_mask);
	}
	if (spfx & SPFX_ESP) {
	    if(on) ETelepat |= wp_mask;
	    else ETelepat &= ~wp_mask;
	    see_monsters();
	}
	if (spfx & SPFX_STLTH) {
	    if (on) EStealth |= wp_mask;
	    else EStealth &= ~wp_mask;
	}
	if (spfx & SPFX_REGEN) {
	    if (on) ERegeneration |= wp_mask;
	    else ERegeneration &= ~wp_mask;
	}
	if (spfx & SPFX_TCTRL) {
	    if (on) ETeleport_control |= wp_mask;
	    else ETeleport_control &= ~wp_mask;
	}
	/* weapon warning is specially handled in mon.c */
	if (spfx & SPFX_WARN) {
	    if (spec_m2(otmp)) {
	    	if (on) {
			EWarn_of_mon |= wp_mask;
			flags.warntype |= spec_m2(otmp);
	    	} else {
			EWarn_of_mon &= ~wp_mask;
	    		flags.warntype &= ~spec_m2(otmp);
		}
		see_monsters();
	    } else {
		if (on) EWarning |= wp_mask;
	    	else EWarning &= ~wp_mask;
	    }
	}
	if (spfx & SPFX_EREGEN) {
	    if (on) EEnergy_regeneration |= wp_mask;
	    else EEnergy_regeneration &= ~wp_mask;
	}
	if (spfx & SPFX_HSPDAM) {
	    if (on) EHalf_spell_damage |= wp_mask;
	    else EHalf_spell_damage &= ~wp_mask;
	}
	if (spfx & SPFX_HPHDAM) {
	    if (on) EHalf_physical_damage |= wp_mask;
	    else EHalf_physical_damage &= ~wp_mask;
	}
	if (spfx & SPFX_PROTEC) {
	/* 5lo: The original code from NH:TnG doesn't work properly as the protection
	 * is duplicated upon saving and restoring, allowing for a player to easily 
	 * decrease their AC until it reaches the limit of -127.  Instead, let's just
	 * skip the protection intrinsic and directly modify the AC.
	 */
	    if (on) {
//		u.ublessed += 2;
//		EProtection |= wp_mask;
		mons[u.umonnum].ac -= 2;
	    } else {
//	    	u.ublessed -= 2;
//		if (u.ublessed <= 0) EProtection &= ~wp_mask;
		mons[u.umonnum].ac += 2;
	    }
	}
	if (spfx & SPFX_XRAY) {
	    /* this assumes that no one else is using xray_range */
	    if (on) u.xray_range = 3;
	    else u.xray_range = -1;
	    vision_full_recalc = 1;
	}
	/* KMH -- Reflection when wielded */
	if (spfx & SPFX_REFLECT) {
		/* Knights only have to carry the mirror; everyone else must wield it */
		if (Role_if(PM_KNIGHT)) {
			if (on) {
				EReflecting |= wp_mask;
			} else {
				EReflecting &= ~wp_mask;
			}
		} else if (wp_mask & W_WEP) {
	    		if (on) EReflecting |= wp_mask;
			else EReflecting &= ~wp_mask;

		}
	}
	if(wp_mask == W_ART && !on && oart->inv_prop) {
	    /* might have to turn off invoked power too */
	    if (oart->inv_prop <= LAST_PROP &&
		(u.uprops[oart->inv_prop].extrinsic & W_ARTI))
		(void) arti_invoke(otmp);
	}
}

/*
 * creature (usually player) tries to touch (pick up or wield) an artifact obj.
 * Returns 0 if the object refuses to be touched.
 * This routine does not change any object chains.
 * Ignores such things as gauntlets, assuming the artifact is not
 * fooled by such trappings.
 */
int
touch_artifact(obj,mon)
    struct obj *obj;
    struct monst *mon;
{
    register const struct artifact *oart = get_artifact(obj);
    boolean badclass, badalign, self_willed, yours;

    if(!oart) return 1;

    /* [ALI] Thiefbane has a special affinity with shopkeepers */
    if (mon->isshk && obj->oartifact == ART_THIEFBANE) return 1;

    yours = (mon == &youmonst);
    /* all quest artifacts are self-willed; it this ever changes, `badclass'
       will have to be extended to explicitly include quest artifacts */
    self_willed = ((oart->spfx & SPFX_INTEL) != 0);
    if (yours) {
	badclass = self_willed &&
		   ((oart->role != NON_PM && !Role_if(oart->role)) ||
		    (oart->race != NON_PM && !Race_if(oart->race)));
	badalign = (oart->spfx & SPFX_RESTR) && oart->alignment != A_NONE &&
		   (oart->alignment != u.ualign.type || u.ualign.record < 0);
    } else if (!is_covetous(mon->data) && !is_mplayer(mon->data)) {
	badclass = self_willed &&
		   oart->role != NON_PM && oart != &artilist[ART_EXCALIBUR];
	badalign = (oart->spfx & SPFX_RESTR) && oart->alignment != A_NONE &&
		   (oart->alignment != sgn(mon->data->maligntyp));
    } else {    /* an M3_WANTSxxx monster or a fake player */
	/* special monsters trying to take the Amulet, invocation tools or
	   quest item can touch anything except for `spec_applies' artifacts */
	badclass = badalign = FALSE;
    }
    /* weapons which attack specific categories of monsters are
       bad for them even if their alignments happen to match */
    if (!badalign && (oart->spfx & SPFX_DBONUS) != 0) {
	struct artifact tmp;

	tmp = *oart;
	tmp.spfx &= SPFX_DBONUS;
	badalign = !!spec_applies(&tmp, mon);
    }

    if (((badclass || badalign) && self_willed) ||
       (badalign && (!yours || !rn2(4))))  {
	int dmg;
	char buf[BUFSZ];

	if (!yours) return 0;
	You("are blasted by %s power!", s_suffix(the(xname(obj))));
	dmg = d((Antimagic ? 6 : 8), (self_willed ? 10 : 6));
	Sprintf(buf, "touching %s", oart->name);
	losehp(dmg, buf, KILLED_BY);
	exercise(A_WIS, FALSE);
    }

    /* can pick it up unless you're totally non-synch'd with the artifact */
    if (badclass && badalign && self_willed) {
	if (yours) pline("%s your grasp!", Tobjnam(obj, "evade"));
	return 0;
    }
#ifdef CONVICT
    /* This is a kludge, but I'm not sure where else to put it */
    if (oart == &artilist[ART_IRON_BALL_OF_LIBERATION]) {
	if (Role_if(PM_CONVICT) && (!obj->oerodeproof)) {
	    obj->oerodeproof = TRUE;
	    obj->owt = 900; /* Magically lightened, but still heavy */
	}

	if (Punished && (obj != uball)) {
	    unpunish(); /* Remove a mundane heavy iron ball */
	}
    }
#endif /* CONVICT */

    return 1;
}

#endif /* OVLB */
#ifdef OVL1

/* 5lo: Functions for the forge */
boolean nogen_nrartifact(int artino)
{
    return(artilist[artino].spfx % SPFX_NOGEN);
}

boolean exists_nrartifact(int artino)
{
    return(artiexist[artino]);
}

int nrartifact_obtype(int artino)
{
    return(artilist[artino].otyp);
}

/* decide whether an artifact's special attacks apply against mtmp */
STATIC_OVL int
spec_applies(weap, mtmp)
register const struct artifact *weap;
struct monst *mtmp;
{
	int retval = TRUE;
	struct permonst *ptr;
	boolean yours;

	if(!(weap->spfx & (SPFX_DBONUS | SPFX_ATTK)))
	    return(weap->attk.adtyp == AD_PHYS);

	yours = (mtmp == &youmonst);
	ptr = mtmp->data;

	/* [ALI] Modified to support multiple DBONUS and ATTK flags set.
	 * Not all combinations are possible because many DBONUS flags
	 * use mtype and would conflict. Where combinations are possible,
	 * both checks must pass in order for the special attack to
	 * apply against mtmp.
	 */
	if (weap->spfx & SPFX_DMONS) {
	    retval &= (ptr == &mons[(int)weap->mtype]);
	} else if (weap->spfx & SPFX_DCLAS) {
	    retval &= (weap->mtype == (unsigned long)ptr->mlet);
	} else if (weap->spfx & SPFX_DFLAG1) {
	    retval &= ((ptr->mflags1 & weap->mtype) != 0L);
	} else if (weap->spfx & SPFX_DFLAG2) {
	    retval &= ((ptr->mflags2 & weap->mtype) || (yours &&
			   ((!Upolyd && (urace.selfmask & weap->mtype)) ||
			    ((weap->mtype & M2_WERE) && u.ulycn >= LOW_PM))));
	}
	if (weap->spfx & SPFX_DALIGN) {
	    retval &= yours ? (u.ualign.type != weap->alignment) :
			   (ptr->maligntyp == A_NONE ||
				sgn(ptr->maligntyp) != weap->alignment);
	}
	if (weap->spfx & SPFX_ATTK) {
	    struct obj *defending_weapon = (yours ? uwep : MON_WEP(mtmp));

	    if (defending_weapon && defending_weapon->oartifact &&
		    defends((int)weap->attk.adtyp, defending_weapon))
		return FALSE;
	    switch(weap->attk.adtyp) {
		case AD_FIRE:
			if (yours ? Fire_resistance : resists_fire(mtmp))
			    retval = FALSE;
			break;
		case AD_COLD:
			if (yours ? Cold_resistance : resists_cold(mtmp))
			    retval = FALSE;
			break;
		case AD_ELEC:
			if (yours ? Shock_resistance : resists_elec(mtmp))
			    retval = FALSE;
			break;
		case AD_ACID:
			if (yours ? Acid_resistance : resists_acid(mtmp))
			    retval = FALSE;
			break;
		case AD_MAGM:
		case AD_STUN:
			if (yours ? Antimagic : (rn2(100) < ptr->mr))
			    retval = FALSE;
			break;
		case AD_DRST:
			if (yours ? Poison_resistance : resists_poison(mtmp))
			    retval = FALSE;
			break;
		case AD_DRLI:
			if (yours ? Drain_resistance : resists_drli(mtmp))
			    retval = FALSE;
			break;
		case AD_STON:
			if (yours ? Stone_resistance : resists_ston(mtmp))
			    retval = FALSE;
			break;
		case AD_PHYS:
			break;
		default:	impossible("Weird weapon special attack.");
	    }
	}
	return retval;
}

/* return the M2 flags of monster that an artifact's special attacks apply against */
long
spec_m2(otmp)
struct obj *otmp;
{
	register const struct artifact *artifact = get_artifact(otmp);
	if (artifact)
		return artifact->mtype;
	return 0L;
}

/* special attack bonus */
int
spec_abon(otmp, mon)
struct obj *otmp;
struct monst *mon;
{
	register const struct artifact *weap = get_artifact(otmp);

	/* no need for an extra check for `NO_ATTK' because this will
	   always return 0 for any artifact which has that attribute */

	if (weap && weap->attk.damn && spec_applies(weap, mon))
	    return (int)weap->attk.damn;
	return 0;
}

/* special damage bonus */
int
spec_dbon(otmp, mon, tmp)
struct obj *otmp;
struct monst *mon;
int tmp;
{
	register const struct artifact *weap = get_artifact(otmp);

	if (!weap || (weap->attk.adtyp == AD_PHYS && /* check for `NO_ATTK' */
			weap->attk.damn == 0 && weap->attk.damd == 0))
	    spec_dbon_applies = FALSE;
	else
	    spec_dbon_applies = spec_applies(weap, mon);

	if (spec_dbon_applies)
	    return weap->attk.damd ? (int)weap->attk.damd :
		    /* [ALI] Unlike melee weapons, damd == 0 means no
		     * bonus for launchers.
		     */
		    is_launcher(otmp) ? 0 : max(tmp,1);
	return 0;
}

/* add identified artifact to discoveries list */
void
discover_artifact(m)
int m;
{
    int i;

    /* look for this artifact in the discoveries list;
       if we hit an empty slot then it's not present, so add it */
    for (i = 0; i < NROFARTIFACTS; i++)
	if (artidisco[i] == 0 || artidisco[i] == m) {
	    artidisco[i] = m;
	    return;
	}
    /* there is one slot per artifact, so we should never reach the
       end without either finding the artifact or an empty slot... */
    impossible("couldn't discover artifact (%d)", (int)m);
}

/* used to decide whether an artifact has been fully identified */
boolean
undiscovered_artifact(m)
int m;
{
    int i;

    /* look for this artifact in the discoveries list;
       if we hit an empty slot then it's undiscovered */
    for (i = 0; i < NROFARTIFACTS; i++)
	if (artidisco[i] == m)
	    return FALSE;
	else if (artidisco[i] == 0)
	    break;
    return TRUE;
}

/* display a list of discovered artifacts; return their count */
int
disp_artifact_discoveries(tmpwin)
winid tmpwin;		/* supplied by dodiscover() */
{
    int i, m, otyp;
    char buf[BUFSZ];
    anything any;

    any.a_void = 0;
    for (i = 0; i < NROFARTIFACTS; i++) {
	if (artidisco[i] == 0) break;	/* empty slot implies end of list */
	if (i == 0)
	    add_menu(tmpwin, NO_GLYPH, &any, 0, 0, iflags.menu_headings,
	      "Artifacts", MENU_UNSELECTED);
	m = artidisco[i];
	otyp = artilist[m].otyp;
	Sprintf(buf, "  %s [%s %s]", artiname(m),
		align_str(artilist[m].alignment), simple_typename(otyp));
	add_menu(tmpwin, objnum_to_glyph(otyp), &any, 0, 0, ATR_NONE,
	  buf, MENU_UNSELECTED);
    }
    return i;
}

#endif /* OVL1 */

#ifdef OVLB

/* will freeing this object from inventory cause levitation to end? */
boolean
finesse_ahriman(obj)
struct obj *obj;
{
    const struct artifact *oart;
    struct prop save_Lev;
    boolean result;

    /* if we aren't levitating or this isn't an artifact which confers
       levitation via #invoke then freeinv() won't toggle levitation */
    if (!Levitation || (oart = get_artifact(obj)) == 0 ||
	    oart->inv_prop != LEVITATION || !(ELevitation & W_ARTI))
	return FALSE;

    /* arti_invoke(off) -> float_down() clears I_SPECIAL|TIMEOUT & W_ARTI;
       probe ahead to see whether that actually results in floating down;
       (this assumes that there aren't two simultaneously invoked artifacts
       both conferring levitation--safe, since if there were two of them,
       invoking the 2nd would negate the 1st rather than stack with it) */
    save_Lev = u.uprops[LEVITATION];
    HLevitation &= ~(I_SPECIAL|TIMEOUT);
    ELevitation &= ~W_ARTI;
    result = (boolean)!Levitation;
    u.uprops[LEVITATION] = save_Lev;
    return result;
}

	/*
	 * Magicbane's intrinsic magic is incompatible with normal
	 * enchantment magic.  Thus, its effects have a negative
	 * dependence on spe.  Against low mr victims, it typically
	 * does "double athame" damage, 2d4.  Occasionally, it will
	 * cast unbalancing magic which effectively averages out to
	 * 4d4 damage (3d4 against high mr victims), for spe = 0.
	 *
	 * Prior to 3.4.1, the cancel (aka purge) effect always
	 * included the scare effect too; now it's one or the other.
	 * Likewise, the stun effect won't be combined with either
	 * of those two; it will be chosen separately or possibly
	 * used as a fallback when scare or cancel fails.
	 *
	 * [Historical note: a change to artifact_hit() for 3.4.0
	 * unintentionally made all of Magicbane's special effects
	 * be blocked if the defender successfully saved against a
	 * stun attack.  As of 3.4.1, those effects can occur but
	 * will be slightly less likely than they were in 3.3.x.]
	 */
#define MB_MAX_DIEROLL		8	/* rolls above this aren't magical */
static const char * const mb_verb[2][4] = {
	{ "probe", "stun", "scare", "cancel" },
	{ "prod", "amaze", "tickle", "purge" },
};
#define MB_INDEX_PROBE		0
#define MB_INDEX_STUN		1
#define MB_INDEX_SCARE		2
#define MB_INDEX_CANCEL		3

/* called when someone is being hit by Magicbane */
STATIC_OVL boolean
Mb_hit(magr, mdef, mb, dmgptr, dieroll, vis, hittee)
struct monst *magr, *mdef;	/* attacker and defender */
struct obj *mb;			/* Magicbane */
int *dmgptr;			/* extra damage target will suffer */
int dieroll;			/* d20 that has already scored a hit */
boolean vis;			/* whether the action can be seen */
char *hittee;			/* target's name: "you" or mon_nam(mdef) */
{
    struct permonst *old_uasmon;
    const char *verb;
    boolean youattack = (magr == &youmonst),
	    youdefend = (mdef == &youmonst),
	    resisted = FALSE, do_stun, do_confuse, result;
    int attack_indx, scare_dieroll = MB_MAX_DIEROLL / 2;

    result = FALSE;		/* no message given yet */
    /* the most severe effects are less likely at higher enchantment */
    if (mb->spe >= 3)
	scare_dieroll /= (1 << (mb->spe / 3));
    /* if target successfully resisted the artifact damage bonus,
       reduce overall likelihood of the assorted special effects */
    if (!spec_dbon_applies) dieroll += 1;

    /* might stun even when attempting a more severe effect, but
       in that case it will only happen if the other effect fails;
       extra damage will apply regardless; 3.4.1: sometimes might
       just probe even when it hasn't been enchanted */
    do_stun = (max(mb->spe,0) < rn2(spec_dbon_applies ? 11 : 7));

    /* the special effects also boost physical damage; increments are
       generally cumulative, but since the stun effect is based on a
       different criterium its damage might not be included; the base
       damage is either 1d4 (athame) or 2d4 (athame+spec_dbon) depending
       on target's resistance check against AD_STUN (handled by caller)
       [note that a successful save against AD_STUN doesn't actually
       prevent the target from ending up stunned] */
    attack_indx = MB_INDEX_PROBE;
    *dmgptr += rnd(4);			/* (2..3)d4 */
    if (do_stun) {
	attack_indx = MB_INDEX_STUN;
	*dmgptr += rnd(4);		/* (3..4)d4 */
    }
    if (dieroll <= scare_dieroll) {
	attack_indx = MB_INDEX_SCARE;
	*dmgptr += rnd(4);		/* (3..5)d4 */
    }
    if (dieroll <= (scare_dieroll / 2)) {
	attack_indx = MB_INDEX_CANCEL;
	*dmgptr += rnd(4);		/* (4..6)d4 */
    }

    /* give the hit message prior to inflicting the effects */
    verb = mb_verb[!!Hallucination][attack_indx];
    if (youattack || youdefend || vis) {
	result = TRUE;
	pline_The("magic-absorbing blade %s %s!",
		  vtense((const char *)0, verb), hittee);
	/* assume probing has some sort of noticeable feedback
	   even if it is being done by one monster to another */
	if (attack_indx == MB_INDEX_PROBE && !canspotmon(mdef))
	    map_invisible(mdef->mx, mdef->my);
    }

    /* now perform special effects */
    switch (attack_indx) {
    case MB_INDEX_CANCEL:
	old_uasmon = youmonst.data;
	/* No mdef->mcan check: even a cancelled monster can be polymorphed
	 * into a golem, and the "cancel" effect acts as if some magical
	 * energy remains in spellcasting defenders to be absorbed later.
	 */
	if (!cancel_monst(mdef, mb, youattack, FALSE, FALSE)) {
	    resisted = TRUE;
	} else {
	    do_stun = FALSE;
	    if (youdefend) {
		if (youmonst.data != old_uasmon)
		    *dmgptr = 0;    /* rehumanized, so no more damage */
		if (u.uenmax > 0) {
		    You("lose magical energy!");
		    u.uenmax--;
		    if (u.uen > 0) u.uen--;
		    flags.botl = 1;
		}
	    } else {
		if (mdef->data == &mons[PM_CLAY_GOLEM])
		    mdef->mhp = 1;	/* cancelled clay golems will die */
		if (youattack && attacktype(mdef->data, AT_MAGC)) {
		    You("absorb magical energy!");
		    u.uenmax++;
		    u.uen++;
		    flags.botl = 1;
		}
	    }
	}
	break;

    case MB_INDEX_SCARE:
	if (youdefend) {
	    if (Antimagic) {
		resisted = TRUE;
	    } else {
		nomul(-3, "being scared stiff");
		nomovemsg = "";
		if (magr && magr == u.ustuck && sticks(youmonst.data)) {
		    setustuck((struct monst *)0);
		    You("release %s!", mon_nam(magr));
		}
	    }
	} else {
	    if (rn2(2) && resist(mdef, WEAPON_CLASS, 0, NOTELL))
		resisted = TRUE;
	    else
		monflee(mdef, 3, FALSE, (mdef->mhp > *dmgptr));
	}
	if (!resisted) do_stun = FALSE;
	break;

    case MB_INDEX_STUN:
	do_stun = TRUE;		/* (this is redundant...) */
	break;

    case MB_INDEX_PROBE:
	if (youattack && (mb->spe == 0 || !rn2(3 * abs(mb->spe)))) {
	    pline_The("%s is insightful.", verb);
	    /* pre-damage status */
	    probe_monster(mdef);
	}
	break;
    }
    /* stun if that was selected and a worse effect didn't occur */
    if (do_stun) {
	if (youdefend)
	    make_stunned((HStun + 3), FALSE);
	else
	    mdef->mstun = 1;
	/* avoid extra stun message below if we used mb_verb["stun"] above */
	if (attack_indx == MB_INDEX_STUN) do_stun = FALSE;
    }
    /* lastly, all this magic can be confusing... */
    do_confuse = !rn2(12);
    if (do_confuse) {
	if (youdefend)
	    make_confused(HConfusion + 4, FALSE);
	else
	    mdef->mconf = 1;
    }

    if (youattack || youdefend || vis) {
	(void) upstart(hittee);	/* capitalize */
	if (resisted) {
	    pline("%s %s!", hittee, vtense(hittee, "resist"));
	    shieldeff(youdefend ? u.ux : mdef->mx,
		      youdefend ? u.uy : mdef->my);
	}
	if ((do_stun || do_confuse) && flags.verbose) {
	    char buf[BUFSZ];

	    buf[0] = '\0';
	    if (do_stun) Strcat(buf, "stunned");
	    if (do_stun && do_confuse) Strcat(buf, " and ");
	    if (do_confuse) Strcat(buf, "confused");
	    pline("%s %s %s%c", hittee, vtense(hittee, "are"),
		  buf, (do_stun && do_confuse) ? '!' : '.');
	}
    }

    return result;
}
  
/* Function used when someone attacks someone else with an artifact
 * weapon.  Only adds the special (artifact) damage, and returns a 1 if it
 * did something special (in which case the caller won't print the normal
 * hit message).  This should be called once upon every artifact attack;
 * dmgval() no longer takes artifact bonuses into account.  Possible
 * extension: change the killer so that when an orc kills you with
 * Stormbringer it's "killed by Stormbringer" instead of "killed by an orc".
 */
boolean
artifact_hit(magr, mdef, otmp, dmgptr, dieroll)
struct monst *magr, *mdef;
struct obj *otmp;
int *dmgptr;
int dieroll; /* needed for Magicbane and vorpal blades */
{
	boolean youattack = (magr == &youmonst);
	boolean youdefend = (mdef == &youmonst);
	boolean vis = (!youattack && magr && cansee(magr->mx, magr->my))
	    || (!youdefend && cansee(mdef->mx, mdef->my))
	    || (youattack && u.uswallow && mdef == u.ustuck && !Blind);
	boolean realizes_damage;
	const char *wepdesc;
	static const char you[] = "you";
	char hittee[BUFSIZ];
	boolean special_applies;

	strcpy(hittee, youdefend ? you : mon_nam(mdef));

	/* The following takes care of most of the damage, but not all--
	 * the exception being for level draining, which is specially
	 * handled.  Messages are done in this function, however.
	 */
	*dmgptr += spec_dbon(otmp, mdef, *dmgptr);

	if (spec_dbon_applies)
	    special_applies = TRUE;
	else {
	    const struct artifact *weap = get_artifact(otmp);
	    special_applies = weap && spec_applies(weap, mdef);
	}

	if (youattack && youdefend) {
	    impossible("attacking yourself with weapon?");
	    return FALSE;
	}

	realizes_damage = (youdefend || vis || 
			   /* feel the effect even if not seen */
			   (youattack && mdef == u.ustuck));

	/* the four basic attacks: fire, cold, shock and missiles */
	if (attacks(AD_FIRE, otmp)) {
	    if (realizes_damage)
		pline_The("fiery blade %s %s%c",
			!spec_dbon_applies ? "hits" :
			(mdef->data == &mons[PM_WATER_ELEMENTAL]) ?
			"vaporizes part of" : "burns",
			hittee, !spec_dbon_applies ? '.' : '!');
	    if (!rn2(50)) (void) destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
	    if (!rn2(50)) (void) destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
	    if (!rn2(75)) (void) destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
	    if (youdefend && Slimed) burn_away_slime();
	    return realizes_damage;
	}
	if (attacks(AD_COLD, otmp)) {
	    if (realizes_damage)
		pline_The("ice-cold blade %s %s%c",
			!spec_dbon_applies ? "hits" : "freezes",
			hittee, !spec_dbon_applies ? '.' : '!');
	    if (!rn2(100)) (void) destroy_mitem(mdef, POTION_CLASS, AD_COLD);
	    return realizes_damage;
	}
	if (attacks(AD_ELEC, otmp)) {
	    if (realizes_damage)
		if(otmp->oartifact == ART_MJOLLNIR)
		    pline_The("massive hammer hits%s %s%c",
		    !spec_dbon_applies ? "" : "!  Lightning strikes",
		    hittee, !spec_dbon_applies ? '.' : '!');
		else if (otmp->oartifact == ART_POSEIDON_S_TREASURE)
		    pline_The("mystic trident %s %s%c",
		    !spec_dbon_applies ? "strikes" : "shocks",
		    hittee, !spec_dbon_applies ? '.' : '!');
		else
		    pline_The("sparking blade %s %s%c",
		    !spec_dbon_applies ? "hits" : "shocks",
		    hittee, !spec_dbon_applies ? '.' : '!');
	    if (!rn2(150)) (void) destroy_mitem(mdef, RING_CLASS, AD_ELEC);
	    if (!rn2(150)) (void) destroy_mitem(mdef, WAND_CLASS, AD_ELEC);
	    return realizes_damage;
	}
	if (attacks(AD_ACID, otmp)) {	/* 5lo: Adapted from BarclayII's Slashem-up */
		if (realizes_damage)
		pline_The("acidic blade %s %s%c",
				!spec_dbon_applies ? "hits" :
				"burns",
				hittee, !spec_dbon_applies ? '.' : '!');
		return realizes_damage;
	}
	if (attacks(AD_MAGM, otmp)) {
	    if (realizes_damage)
		pline_The("imaginary widget hits%s %s%c",
			  !spec_dbon_applies ? "" :
				"!  A hail of magic missiles strikes",
			  hittee, !spec_dbon_applies ? '.' : '!');
	    return realizes_damage;
	}
       /* the fifth basic attack: poison */
       if (attacks(AD_DRST, otmp)) {
               if (realizes_damage) {
                       pline_The("venomous blade %s %s%c",spec_dbon_applies ? "strikes" : "nicks",
                               hittee, spec_dbon_applies ? '!' : '.');
                       return realizes_damage;
               }
       }

	if (attacks(AD_STUN, otmp) && dieroll <= MB_MAX_DIEROLL) {
	    /* Magicbane's special attacks (possibly modifies hittee[]) */
	    return Mb_hit(magr, mdef, otmp, dmgptr, dieroll, vis, hittee);
	}

	if (!spec_dbon_applies && !spec_ability(otmp, SPFX_BEHEAD) ||
		!special_applies) {
	    /* since damage bonus didn't apply, nothing more to do;  
	       no further attacks have side-effects on inventory */
	    /* [ALI] The Tsurugi of Muramasa has no damage bonus but
	       is handled below so avoid early exit if SPFX_BEHEAD set
	       and the defender is vulnerable */
	    return FALSE;
	}

	if(otmp->oartifact == ART_REAVER){
	 if(youattack){
	  if(mdef->minvent && (Role_if(PM_PIRATE) || !rn2(10) ) ){
		struct obj *otmp2, **minvent_ptr;
		long unwornmask;

		if((otmp2 = mdef->minvent) != 0) {
			/* take the object away from the monster */
			obj_extract_self(otmp2);
			if ((unwornmask = otmp2->owornmask) != 0L) {
				mdef->misc_worn_check &= ~unwornmask;
				if (otmp2->owornmask & W_WEP) {
					setmnotwielded(mdef,otmp2);
					MON_NOWEP(mdef);
				}
				otmp2->owornmask = 0L;
				update_mon_intrinsics(mdef, otmp2, FALSE, FALSE);
			}
			/* give the object to the character */
			otmp2 = (Role_if(PM_PIRATE) || Role_if(PM_CORSAIR) ) ? 
				hold_another_object(otmp2, "Ye snatched but dropped %s.",
						   doname(otmp2), "Ye steal: ") :
				hold_another_object(otmp2, "You snatched but dropped %s.",
						   doname(otmp2), "You steal: ");
			if (otmp2->otyp == CORPSE &&
				touch_petrifies(&mons[otmp2->corpsenm]) && !uarmg) {
				char kbuf[BUFSZ];

				Sprintf(kbuf, "stolen %s corpse", mons[otmp2->corpsenm].mname);
				instapetrify(kbuf);
			}
			/* more take-away handling, after theft message */
			if (unwornmask & W_WEP) {		/* stole wielded weapon */
				possibly_unwield(mdef, FALSE);
			} else if (unwornmask & W_ARMG) {	/* stole worn gloves */
				mselftouch(mdef, (const char *)0, TRUE);
				if (mdef->mhp <= 0)	/* it's now a statue */
					return 1; /* monster is dead */
			}
		}
	  }
	 }
	 else if(youdefend){
		char buf[BUFSZ];
		buf[0] = '\0';
		steal(magr, buf/*, TRUE*/);
	 }
	 else{
		struct obj *obj;
		/* find an object to steal, non-cursed if magr is tame */
		for (obj = mdef->minvent; obj; obj = obj->nobj)
		    if (!magr->mtame || !obj->cursed)
				break;

		if (obj) {
			char buf[BUFSZ], onambuf[BUFSZ], mdefnambuf[BUFSZ];

			/* make a special x_monnam() call that never omits
			   the saddle, and save it for later messages */
			Strcpy(mdefnambuf, x_monnam(mdef, ARTICLE_THE, (char *)0, 0, FALSE));
#ifdef STEED
			if (u.usteed == mdef &&
					obj == which_armor(mdef, W_SADDLE))
				/* "You can no longer ride <steed>." */
				dismount_steed(DISMOUNT_POLY);
#endif
			obj_extract_self(obj);
			if (obj->owornmask) {
				mdef->misc_worn_check &= ~obj->owornmask;
				if (obj->owornmask & W_WEP)
				    setmnotwielded(mdef,obj);
				obj->owornmask = 0L;
				update_mon_intrinsics(mdef, obj, FALSE, FALSE);
			}
			/* add_to_minv() might free obj [if it merges] */
			if (vis)
				Strcpy(onambuf, doname(obj));
			(void) add_to_minv(magr, obj);
			if (vis) {
				Strcpy(buf, Monnam(magr));
				pline("%s steals %s from %s!", buf,
				    onambuf, mdefnambuf);
			}
			possibly_unwield(mdef, FALSE);
			mdef->mstrategy &= ~STRAT_WAITFORU;
			mselftouch(mdef, (const char *)0, FALSE);
			if (mdef->mhp <= 0)
				return 1;
		}
	 }
	}

	/* STEPHEN WHITE'S NEW CODE */
	if (otmp->oartifact == ART_SERPENT_S_TONGUE) {
	    otmp->dknown = TRUE;
	    pline_The("twisted blade poisons %s!",
		    youdefend ? "you" : mon_nam(mdef));
	    if (youdefend ? Poison_resistance : resists_poison(mdef)) {
		if (youdefend)
		    You("are not affected by the poison.");
		else
		    pline("%s seems unaffected by the poison.", Monnam(mdef));
		return TRUE;
	    }
	    switch (rnd(10)) {
		case 1:
		case 2:
		case 3:
		case 4:
		    *dmgptr += d(1,6) + 2;
		    break;
		case 5:
		case 6:
		case 7:
		    *dmgptr += d(2,6) + 4;
		    break;
		case 8:
		case 9:
		    *dmgptr += d(3,6) + 6;
		    break;
		case 10:
			if (!rn2(20)) {
		    pline_The("poison was deadly...");
		    *dmgptr = 2 *
			    (youdefend ? Upolyd ? u.mh : u.uhp : mdef->mhp) +
			    FATAL_DAMAGE_MODIFIER;
			}
			else { *dmgptr += d(4,6) + 6; }
		    break;
	    }
	    return TRUE;
	}

       if (otmp->oartifact == ART_DOOMBLADE && dieroll < 6) {
	    if (youattack)
		You("plunge the Doomblade deeply into %s!",
			mon_nam(mdef));
	    else
		pline("%s plunges the Doomblade deeply into %s!",
			Monnam(magr), hittee);
	    *dmgptr += rnd(4) * 5;
	    return TRUE;
       }
      /* END OF STEPHEN WHITE'S NEW CODE */

#ifdef NEWHON_ARTIFACTS /* 5lo: Reviving these from Slash 6 */
	   if (otmp->oartifact == ART_MOUSER_S_SCALPEL && dieroll < 10) { /* Credits to BarclayII for Mouser's Scalpel rename and mechanic */
		/* faster than a speeding bullet is the Gray Mouser... */
		pline("There is a flurry of blows!");
		int time = 1;
		/* I suppose this could theoretically continue forever... */
		while (dieroll < 5) {
		   *dmgptr += rnd(8) + 1 + otmp->spe;
		   time++;
		   dieroll = rn2(11);
		}
		if (time == 1)
			pline_The("rapier strikes %s!", hittee);
		else if (time == 2)
			pline_The("rapier strikes %s twice!", hittee);
		else
			pline_The("rapier strikes %s %d times in a row!", hittee, time); 
	   }
	   if (otmp->oartifact == ART_HEARTSEEKER && dieroll < 3) {
		/* this weapon just sounds nasty... yuck... */
		if (!youdefend) {
		   You("plunge Heartseeker into %s!",mon_nam(mdef));
		} else {
		   pline("%s plunges Heartseeker into you!",mon_nam(mdef));
		}
		*dmgptr += rnd(6)+rnd(6)+rnd(6)+rnd(6)+4;
	   }
#endif /* NEWHON_ARTIFACTS */

	if (otmp->oartifact == ART_NIGHTINGALE && dieroll < 3 ) { /* 5lo: Nightingale special effect */
	    pline_The("night blade howls as it slices through %s!", hittee);
	    *dmgptr += rnd(5) * 6;
	    return TRUE;
	}
	if (otmp->oartifact == ART_SPINESEEKER && dieroll < 5 ) { /* 5lo: Another artifact effect */
	    if(!youdefend) {
	    	pline_The("gleaming blade cuts into %s's %s!", hittee, mbodypart(mdef,SPINE));
		*dmgptr += rnd(5);
		mdef->mcanmove = 0;
		mdef->mfrozen = rnd(5);
	    } else {
	    	pline_The("gleaming blade cuts into your %s!", body_part(SPINE));
		*dmgptr += rnd(5);
		nomovemsg = "";
		nomul(-rnd(5), "paralyzed by Spineseeker");
	    }
	}

	if (otmp->oartifact == ART_WARFORGER && dieroll < 5) { /* 5lo: Warforger special effect. */
		pline("The slag of the forge sears %s!", hittee);
		*dmgptr += rnd(6) * 6; /* Best be kind to him, now */
		if (!rn2(50)) (void) destroy_mitem(mdef, POTION_CLASS, AD_FIRE);
		if (!rn2(50)) (void) destroy_mitem(mdef, SCROLL_CLASS, AD_FIRE);
		if (!rn2(75)) (void) destroy_mitem(mdef, SPBOOK_CLASS, AD_FIRE);
		/* Note: Warforger doesn't use fire as its element.  This is
		* to prevent people having fire resistance from ignoring the
		* extra damage it can do.  It still burns things, however. */
		return TRUE;
	}

	/* We really want "on a natural 20" but Nethack does it in */
	/* reverse from AD&D. */
	if (spec_ability(otmp, SPFX_BEHEAD)) {
	    if ( (otmp->oartifact == ART_TSURUGI_OF_MURAMASA) && dieroll < 3) {
		wepdesc = "The razor-sharp blade";
		/* not really beheading, but so close, why add another SPFX */
		if (youattack && u.uswallow && mdef == u.ustuck) {
		    You("slice %s wide open!", mon_nam(mdef));
		    *dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
		    return TRUE;
		}
		if (!youdefend) {
			/* allow normal cutworm() call to add extra damage */
			if(notonhead)
			    return FALSE;

			if (bigmonst(mdef->data)) {
				if (youattack)
					You("slice deeply into %s!",
						mon_nam(mdef));
				else if (vis)
					pline("%s cuts deeply into %s!",
					      Monnam(magr), hittee);
				*dmgptr *= 2;
				return TRUE;
			}
			*dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
			pline("%s cuts %s in half!", wepdesc, mon_nam(mdef));
			otmp->dknown = TRUE;
			return TRUE;
		} else {
			/* Invulnerable player won't be bisected */
			if (bigmonst(youmonst.data) || Invulnerable) {
				pline("%s cuts deeply into you!",
				      magr ? Monnam(magr) : wepdesc);
				*dmgptr *= 2;
				return TRUE;
			}

			/* Players with negative AC's take less damage instead
			 * of just not getting hit.  We must add a large enough
			 * value to the damage so that this reduction in
			 * damage does not prevent death.
			 */
			*dmgptr = 2 * (Upolyd ? u.mh : u.uhp) + FATAL_DAMAGE_MODIFIER;
			pline("%s cuts you in half!", wepdesc);
			otmp->dknown = TRUE;
			return TRUE;
		}
	    } else if (dieroll < 3 || otmp->oartifact == ART_VORPAL_BLADE &&
				      mdef->data == &mons[PM_JABBERWOCK]) {
		static const char * const behead_msg[2] = {
		     "%s beheads %s!",
		     "%s decapitates %s!"
		};

		if (youattack && u.uswallow && mdef == u.ustuck)
			return FALSE;
		wepdesc = artilist[otmp->oartifact].name;
		if (!youdefend) {
			if (!has_head(mdef->data) || notonhead || u.uswallow) {
				if (youattack)
					pline("Somehow, you miss %s wildly.",
						mon_nam(mdef));
				else if (vis)
					pline("Somehow, %s misses wildly.",
						mon_nam(magr));
				*dmgptr = 0;
				return ((boolean)(youattack || vis));
			}
			if (noncorporeal(mdef->data) || amorphous(mdef->data)) {
				pline("%s slices through %s %s.", wepdesc,
				      s_suffix(mon_nam(mdef)),
				      mbodypart(mdef,NECK));
				return TRUE;
			}
			*dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
			pline(behead_msg[rn2(SIZE(behead_msg))],
			      wepdesc, mon_nam(mdef));
			otmp->dknown = TRUE;
			return TRUE;
		} else {
			if (!has_head(youmonst.data)) {
				pline("Somehow, %s misses you wildly.",
				      magr ? mon_nam(magr) : wepdesc);
				*dmgptr = 0;
				return TRUE;
			}
			if (noncorporeal(youmonst.data) || amorphous(youmonst.data)) {
				pline("%s slices through your %s.",
				      wepdesc, body_part(NECK));
				return TRUE;
			}
			*dmgptr = 2 * (Upolyd ? u.mh : u.uhp)
				  + FATAL_DAMAGE_MODIFIER;
			if (Invulnerable) {
				pline("%s slices into your %s.",
				      wepdesc, body_part(NECK));
				return TRUE;
			}
			pline(behead_msg[rn2(SIZE(behead_msg))],
			      wepdesc, "you");
			otmp->dknown = TRUE;
			/* Should amulets fall off? */
			return TRUE;
		}
	    }
	}
	if (spec_ability(otmp, SPFX_DRLI)) {
		if (!youdefend) {
		    if (!resists_drli(mdef)) {
			if (vis) {
			    if(otmp->oartifact == ART_STORMBRINGER)
				pline_The("%s blade draws the life from %s!",
				      hcolor(NH_BLACK),
				      mon_nam(mdef));
#if 0	/* OBSOLETE */
			    else if(otmp->oartifact == ART_TENTACLE_STAFF)
				pline("The writhing tentacles draw the life from %s!",
				      mon_nam(mdef));
#endif
			    else
				pline("%s draws the life from %s!",
				      The(distant_name(otmp, xname)),
				      mon_nam(mdef));
			}
			if (mdef->m_lev == 0) {
			    *dmgptr = 2 * mdef->mhp + FATAL_DAMAGE_MODIFIER;
			} else {
			    int drain = rnd(8);
			    *dmgptr += drain;
			    mdef->mhpmax -= drain;
			    mdef->m_lev--;
			    drain /= 2;
			    if (drain) healup(drain, 0, FALSE, FALSE);
			}
			return vis;
		    }
		} else if (!Drain_resistance) { /* youdefend */
			int oldhpmax = u.uhpmax;

			if (Blind)
				You_feel("an %s drain your life!",
				    otmp->oartifact == ART_STORMBRINGER ?
				    "unholy blade" : "object");
			else if (otmp->oartifact == ART_STORMBRINGER)
				pline_The("%s blade drains your life!",
				      hcolor(NH_BLACK));
			else
				pline("%s drains your life!",
				      The(distant_name(otmp, xname)));
			losexp("life drainage", FALSE);
			if (magr && magr->mhp < magr->mhpmax) {
			    magr->mhp += (oldhpmax - u.uhpmax)/2;
			    if (magr->mhp > magr->mhpmax) magr->mhp = magr->mhpmax;
			}
			return TRUE;
		}
	}
	/* WAC -- 1/6 chance of cancellation with foobane weapons */
	if (otmp->oartifact == ART_GIANTKILLER ||
	    otmp->oartifact == ART_ORCRIST ||
	    otmp->oartifact == ART_DRAGONBANE ||
	    otmp->oartifact == ART_DEMONBANE ||
	    otmp->oartifact == ART_WEREBANE ||
	    otmp->oartifact == ART_TROLLSBANE ||
#ifdef BLACKMARKET
	    otmp->oartifact == ART_THIEFBANE ||
#endif
	    otmp->oartifact == ART_OGRESMASHER ||
	    otmp->oartifact == ART_ELFRIST) {
		if (dieroll < 4) {
		    if (realizes_damage) {
			pline("%s %s!", The(distant_name(otmp, xname)), Blind ?
				"roars deafeningly" : "shines brilliantly");
			pline("It strikes %s!", hittee);
		    }
		    cancel_monst(mdef, otmp, youattack, TRUE, magr == mdef);
		    return TRUE;
		}
	}
	return FALSE;
}

static NEARDATA const char recharge_type[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static NEARDATA const char invoke_types[] = { ALL_CLASSES, 0 };
		/* #invoke: an "ugly check" filters out most objects */

int
doinvoke()
{
    register struct obj *obj;

    obj = getobj(invoke_types, "invoke");
    if (!obj) return 0;
    if (obj->oartifact && !touch_artifact(obj, &youmonst)) return 1;
    return arti_invoke(obj);
}

STATIC_OVL int
arti_invoke(obj)
    struct obj *obj;
{
    register const struct artifact *oart = get_artifact(obj);
	    register struct monst *mtmp;
	    register struct monst *mtmp2;
	    register struct permonst *pm;

    int summon_loop;
    int unseen;
/*
    int kill_loop;
 */

    if(!oart || !oart->inv_prop) {
	if(obj->otyp == CRYSTAL_BALL)
	    use_crystal_ball(&obj);
#ifdef ASTR_ESC
	else if (obj->otyp == AMULET_OF_YENDOR || obj->otyp == FAKE_AMULET_OF_YENDOR)
	    /* The Amulet is not technically an artifact in the usual sense... */
	    return invoke_amulet(obj);
#endif
	else
	    pline(nothing_happens);
	return 1;
    }

    if(oart->inv_prop > LAST_PROP) {
	/* It's a special power, not "just" a property */
	if(obj->age > monstermoves &&
		oart->inv_prop != LORDLY) {
	    /* the artifact is tired :-) */
	    You_feel("that %s %s ignoring you.",
		     the(xname(obj)), otense(obj, "are"));
	    /* and just got more so; patience is essential... */
	    obj->age += (long) d(3,10);
	    return 1;
	}
	if (oart->inv_prop != LORDLY)
	    obj->age = monstermoves + rnz(100);

	switch(oart->inv_prop) {
	case TAMING: {
	    struct obj pseudo;

	    pseudo = zeroobj;	/* neither cursed nor blessed */
	    pseudo.otyp = SCR_TAMING;
	    (void) seffects(&pseudo);
	    break;
	  }
	case IDENTIFY: {
		struct obj *pseudo = mksobj(SPE_IDENTIFY, FALSE, FALSE);
		pseudo->blessed = pseudo->cursed = 0;
		pseudo->quan = 42L;		/* do not let useup get it */
		(void) seffects(pseudo);
		obfree(pseudo, (struct obj *) 0);
		break;
	    }
	case HEALING: {
	    int healamt = (u.uhpmax + 1 - u.uhp) / 2;
	    long creamed = (long)u.ucreamed;

	    if (Upolyd) healamt = (u.mhmax + 1 - u.mh) / 2;
	    if (healamt || Sick || Slimed || Blinded > creamed)
		You_feel("better.");
	    else
		goto nothing_special;
	    if (healamt > 0) {
		if (Upolyd) u.mh += healamt;
		else u.uhp += healamt;
	    }
	    if(Sick) make_sick(0L,(char *)0,FALSE,SICK_ALL);
	    if(Slimed) Slimed = 0L;
	    if (Blinded > creamed) make_blinded(creamed, FALSE);
	    flags.botl = 1;
	    break;
	  }
	case ENERGY_BOOST: {
	    int epboost = (u.uenmax + 1 - u.uen) / 2;
	    if (epboost > 120) epboost = 120;		/* arbitrary */
	    else if (epboost < 12) epboost = u.uenmax - u.uen;
	    if(epboost) {
		You_feel("re-energized.");
		u.uen += epboost;
		flags.botl = 1;
	    } else
		goto nothing_special;
	    break;
	  }
	case UNTRAP: {
	    if(!untrap(TRUE)) {
		obj->age = 0; /* don't charge for changing their mind */
		return 0;
	    }
	    break;
	  }
	case CHARGE_OBJ: {
	    struct obj *otmp = getobj(recharge_type, "charge");
	    boolean b_effect;

	    if (!otmp) {
		obj->age = 0;
		return 0;
	    }
	    b_effect = obj->blessed &&
		(Role_switch == oart->role || !oart->role);
	    recharge(otmp, b_effect ? 1 : obj->cursed ? -1 : 0);
	    update_inventory();
	    break;
	  }
	case LEV_TELE:
	      if (!flags.lostsoul && !flags.uberlostsoul) level_tele();
		else pline("You are disallowed to use this ability.");
	    break;
	case DRAGON_BREATH:
	    getdir(NULL);
	    buzz(20+AD_FIRE-1,6,u.ux,u.uy,u.dx,u.dy);
	/*       ^^^^^^^^^^^^ - see zap.c / ZT_* defines */
	    break;
	case LIGHT_AREA:
	    if (!Blind)
		pline("%s shines brightly for an instant!", The(xname(obj)));
	    else
		pline("%s grows warm for a second!", The(xname(obj)));
	    litroom(TRUE, obj);
	    vision_recalc(0);
	    if (is_undead(youmonst.data)) {
		You("burn in the radiance!");
		/* This is ground zero.  Not good news ... */
		u.uhp /= 100;
		if (u.uhp < 1) {
		    u.uhp = 0;
		    killer_format = KILLED_BY;
		    killer = "the Holy Spear of Light";
		    done(DIED);
		}
	    }
	    /* Undead and Demonics can't stand the light */
	    unseen = 0;
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
	    	if (distu(mtmp->mx, mtmp->my) > 9*9) continue;
		if (couldsee(mtmp->mx, mtmp->my) &&
			(is_undead(mtmp->data) || is_demon(mtmp->data)) &&
			!resist(mtmp, '\0', 0, TELL)) {
		    if (canseemon(mtmp))
			pline("%s burns in the radiance!", Monnam(mtmp));
		    else
			unseen++;
		    /* damage depends on distance, divisor ranges from 10 to 2 */
		    mtmp->mhp /= (10 - (distu(mtmp->mx, mtmp->my) / 10));
		    if (mtmp->mhp < 1) mtmp->mhp = 1;
		}
	    }
	    if (unseen)
		You_hear("%s of intense pain!", unseen > 1 ? "cries" : "a cry");
	    break;
	case DEATH_GAZE:
	    if (u.uluck < -9) {
		pline_The("Eye turns on you!");
		u.uhp = 0;
		killer_format = KILLED_BY;
		killer = "the Eye of the Beholder";
		done(DIED);
	    }
	    pline_The("Eye looks around with its icy gaze!");
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if (DEADMONSTER(mtmp)) continue;
		/* The eye is never blind ... */
		if (couldsee(mtmp->mx, mtmp->my) && !is_undead(mtmp->data)) {
		    pline("%s screams in agony!", Monnam(mtmp));
		    mtmp->mhp /= 3;
		    if (mtmp->mhp < 1) mtmp->mhp = 1;
		}
	    }
	    /* Tsk,tsk.. */
	    adjalign(-3);
	    u.uluck -= 3;
	    break;
	case SUMMON_UNDEAD:
	    if (u.uluck < -9) {
		u.uhp -= rn2(20) + 5;
		pline_The("Hand claws you with its icy nails!");
		if (u.uhp <= 0) {
		    killer_format = KILLED_BY;
		    killer="the Hand of Vecna";
		    done(DIED);
		}
	    }
	    summon_loop = rn2(4) + 4;
	    pline("Creatures from the grave surround you!");
	    do {
		switch (rn2(6) + 1) {
		    case 1:
			pm = mkclass(S_VAMPIRE, 0);
			break;
		    case 2:
		    case 3:
			pm = mkclass(S_ZOMBIE, 0);
			break;
		    case 4:
			pm = mkclass(S_MUMMY, 0);
			break;
		    case 5:
			pm = mkclass(S_GHOST, 0);
			break;
		    default:
			pm = mkclass(S_WRAITH, 0);
			break;
		}
		mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
	        if ((mtmp2 = tamedog(mtmp, (struct obj *)0)) != 0)
		    mtmp = mtmp2;
		mtmp->mtame = 30;
		summon_loop--;
	    } while (summon_loop);
	    /* Tsk,tsk.. */
	    adjalign(-3);
	    u.uluck -= 3;
	    break;
	case PROT_POLY:
	    You("feel more observant.");
	    rescham();
	    break;
	case SUMMON_FIRE_ELEMENTAL:
	    pm = !rn2(4) ? &mons[PM_GREATER_FIRE_ELEMENTAL] : &mons[PM_FIRE_ELEMENTAL];
	    mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
   
	    pline("You summon an elemental.");
   
	    if ((mtmp2 = tamedog(mtmp, (struct obj *)0)) != 0)
			mtmp = mtmp2;
	    mtmp->mtame = 30;
	    break;
	case SUMMON_WATER_ELEMENTAL:
	    if (obj->oartifact == ART_STORM_WHISTLE)
		    pm = !rn2(4) ? &mons[PM_ICE_ELEMENTAL] : &mons[PM_ICE_VORTEX];
	    else
		    pm = !rn2(4) ? &mons[PM_GREATER_WATER_ELEMENTAL] : &mons[PM_WATER_ELEMENTAL];

	    mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
   
	    pline("You summon an elemental.");
	    
	    if ((mtmp2 = tamedog(mtmp, (struct obj *)0)) != 0)
			mtmp = mtmp2;
	    mtmp->mtame = 30;
	    break;
	case SUMMON_EARTH_ELEMENTAL: /* 5lo: New summons */
            pm = !rn2(4) ? &mons[PM_GREATER_EARTH_ELEMENTAL] : &mons[PM_EARTH_ELEMENTAL];
	    mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
   
	    pline("You summon an elemental.");
	    
	    if ((mtmp2 = tamedog(mtmp, (struct obj *)0)) != 0)
			mtmp = mtmp2;
	    mtmp->mtame = 30;
	    break;
	case SUMMON_AIR_ELEMENTAL:
            pm = !rn2(4) ? &mons[PM_GREATER_AIR_ELEMENTAL] : &mons[PM_AIR_ELEMENTAL];
	    mtmp = makemon(pm, u.ux, u.uy, NO_MM_FLAGS);
   
	    pline("You summon an elemental.");
	    
	    if ((mtmp2 = tamedog(mtmp, (struct obj *)0)) != 0)
			mtmp = mtmp2;
	    mtmp->mtame = 30;
	    break;
	case OBJ_DETECTION:
		(void)object_detect(obj, 0);
		break;
	case CREATE_PORTAL: 
		if (flags.lostsoul || flags.uberlostsoul) break;
				{
	    int i, num_ok_dungeons, last_ok_dungeon = 0;
	    d_level newlev;
	    extern int n_dgns; /* from dungeon.c */
	    winid tmpwin = create_nhwindow(NHW_MENU);
	    anything any;

	    any.a_void = 0;	/* set all bits to zero */
 #ifdef BLACKMARKET           
	    if (Is_blackmarket(&u.uz) && *u.ushops) {
		You("feel very disoriented for a moment.");
		destroy_nhwindow(tmpwin);
		break;
	    }
 #endif
	    start_menu(tmpwin);
	    /* use index+1 (cant use 0) as identifier */
	    for (i = num_ok_dungeons = 0; i < n_dgns; i++) {
		if (!dungeons[i].dunlev_ureached) continue;
		any.a_int = i+1;
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 dungeons[i].dname, MENU_UNSELECTED);
		num_ok_dungeons++;
		last_ok_dungeon = i;
	    }
	    end_menu(tmpwin, "Open a portal to which dungeon?");
	    if (num_ok_dungeons > 1) {
		/* more than one entry; display menu for choices */
		menu_item *selected;
		int n;

		n = select_menu(tmpwin, PICK_ONE, &selected);
		if (n <= 0) {
		    destroy_nhwindow(tmpwin);
		    goto nothing_special;
		}
		i = selected[0].item.a_int - 1;
		free((genericptr_t)selected);
	    } else
		i = last_ok_dungeon;	/* also first & only OK dungeon */
	    destroy_nhwindow(tmpwin);

	    /*
	     * i is now index into dungeon structure for the new dungeon.
	     * Find the closest level in the given dungeon, open
	     * a use-once portal to that dungeon and go there.
	     * The closest level is either the entry or dunlev_ureached.
	     */
	    newlev.dnum = i;
	    if(dungeons[i].depth_start >= depth(&u.uz))
		newlev.dlevel = dungeons[i].entry_lev;
	    else
		newlev.dlevel = dungeons[i].dunlev_ureached;
	    if(u.uhave.amulet || In_endgame(&u.uz) || In_endgame(&newlev) ||
	       newlev.dnum == u.uz.dnum) {
		You_feel("very disoriented for a moment.");
	    } else {
		if(!Blind) You("are surrounded by a shimmering sphere!");
		else You_feel("weightless for a moment.");
		goto_level(&newlev, FALSE, FALSE, FALSE);
	    }
	    break;
	  }
	case ENLIGHTENING:
	    enlightenment(0);
	    break;
	case CREATE_AMMO: {
	    struct obj *otmp;
	    if(obj->oartifact == ART_LONGBOW_OF_DIANA) otmp = mksobj(ARROW, TRUE, FALSE);
	    else if(obj->oartifact == ART_YOICHI_NO_YUMI) otmp = mksobj(YA, TRUE, FALSE);
	    else if(obj->oartifact == ART_FUMA_ITTO_NO_KEN) otmp = mksobj(SHURIKEN, TRUE, FALSE);
	    else otmp = mksobj(ARROW, TRUE, FALSE); /* default */

	    if (!otmp) goto nothing_special;
	    otmp->blessed = obj->blessed;
	    otmp->cursed = obj->cursed;
	    otmp->bknown = obj->bknown;
	    if (obj->blessed) {
		if (otmp->spe < 0) otmp->spe = 0;
		otmp->quan += rnd(10);
	    } else if (obj->cursed) {
		if (otmp->spe > 0) otmp->spe = 0;
	    } else
		otmp->quan += rnd(5);
	    otmp->owt = weight(otmp);
	    otmp = hold_another_object(otmp, "Suddenly %s out.",
				       aobjnam(otmp, "fall"), (const char *)0);
	    break;
	case OBJECT_DET:
		object_detect(obj, 0);
		artifact_detect(obj);
		break;
#ifdef CONVICT
	case PHASING:   /* Walk through walls and stone like a xorn */
        if (Passes_walls) goto nothing_special;
	    if (oart == &artilist[ART_IRON_BALL_OF_LIBERATION]) {
		if (Punished && (obj != uball)) {
		    unpunish(); /* Remove a mundane heavy iron ball */
		}
		
		if (!Punished) {
		    setworn(mkobj(CHAIN_CLASS, TRUE), W_CHAIN);
		    setworn(obj, W_BALL);
		    uball->spe = 1;
		    if (!u.uswallow) {
			placebc();
			if (Blind) set_bc(1);	/* set up ball and chain variables */
			newsym(u.ux,u.uy);		/* see ball&chain if can't see self */
		    }
		    Your("%s chains itself to you!", xname(obj));
		}
	    }
        if (!Hallucination) {    
            Your("body begins to feel less solid.");
        } else {
            You_feel("one with the spirit world.");
        }
        incr_itimeout(&Phasing, (50 + rnd(100)));
        obj->age += Phasing; /* Time begins after phasing ends */
        break;
#endif /* CONVICT */
	case SMOKE_CLOUD: { /* 5lo: from Itlachiayaque patch, credits to L */
	coord cc;
	cc.x = u.ux;
	cc.y = u.uy;
	/* Cause trouble if cursed or player is wrong role */
	if (obj->cursed || (Role_switch == oart->role || !oart->role)) {
	    You("may summon a stinking cloud.");
	    pline("Where do you want to center the cloud?");
	    if (getpos(&cc, TRUE, "the desired position") < 0) {
		pline(Never_mind);
		obj->age = 0;
		return 0;
	    }
	    if (!cansee(cc.x, cc.y) || distu(cc.x, cc.y) >= 32) {
		You("smell rotten eggs.");
		return 0;
	    }
	}
	pline("A cloud of toxic smoke pours out!");
	(void) create_gas_cloud(cc.x, cc.y, 3+bcsign(obj),
	8+4*bcsign(obj));
	break;
	}
    case SHARPEN: { /* 5lo: from Invoke Muramasa patch by L */
	int sharpnum = (obj->cursed ? -1 : (obj->blessed ? 1 : 0));

	if (obj->spe >= sharpnum) {
	    pline("%s is %ssharp enough already.", The(xname(obj)), 
	    (obj->spe > -1 ? (obj->spe > 0 ? "more than " : ""): "almost "));
	    obj->age = 0;
	    return 0;
	}
	obj->spe = sharpnum;
	pline("%s is restored to %sits original keenness!", The(xname(obj)),
	(obj->spe > -1 ? (obj->spe > 0 ? "better than " : ""): "slightly less than "));
	break;
	   }
    case BLESS: {
	if(obj->owornmask&(~(W_ART|W_ARTI))){
		You("can't bless your artifact while it is worn, wielded, or readied.");
		obj->age = 0;
	} else {
		You("bless your artifact.");
	if(cansee(u.ux, u.uy)) pline("Holy light shines upon it!");
	obj->cursed = 0;
	obj->blessed = 1;
	obj->oeroded = 0;
	obj->oeroded2 = 0;
	obj->oerodeproof = 1;
	if(obj->spe < 3) {
		obj->spe = 3;
	}
	}
	}
    case LEADERSHIP: {
	    (void) pet_detect_and_tame(obj);
	    break;
	}
    case MAP_LEVEL: {
	    pline("An image suddenly forms in your mind!");
	    (void)do_mapping();
	    break;
	}
    case PETMASTER:{
	    int pet_effect = 0;
	    if(uarm && uarm == obj && yn("Take something out of your pockets?") == 'y'){
		pet_effect = dopetmenu("Take what out of your pockets?", obj);
		switch(pet_effect){
		    case 0:
			break;
		    case SELECT_WHISTLE:
			otmp = mksobj(MAGIC_WHISTLE, TRUE, FALSE);
			otmp->blessed = obj->blessed;
			otmp->cursed = obj->cursed;
			otmp->bknown = obj->bknown;
			hold_another_object(otmp, "You fumble and %s.",
			  aobjnam(otmp, "fall"), (const char *)0);
			break;
		    case SELECT_LEASH:
			otmp = mksobj(LEASH, TRUE, FALSE);
			otmp->blessed = obj->blessed;
			otmp->cursed = obj->cursed;
			otmp->bknown = obj->bknown;
			hold_another_object(otmp, "You fumble and %s.",
			  aobjnam(otmp, "fall"), (const char *)0);
			break;
		    case SELECT_SADDLE:
			otmp = mksobj(SADDLE, TRUE, FALSE);
			otmp->blessed = obj->blessed;
			otmp->cursed = obj->cursed;
			otmp->bknown = obj->bknown;
			hold_another_object(otmp, "You fumble and %s.",
			  aobjnam(otmp, "fall"), (const char *)0);
			break;
		    case SELECT_TRIPE:
			otmp = mksobj(TRIPE_RATION, TRUE, FALSE);
			otmp->blessed = obj->blessed;
			otmp->cursed = obj->cursed;
			otmp->bknown = obj->bknown;
			hold_another_object(otmp, "You fumble and %s.",
			  aobjnam(otmp, "fall"), (const char *)0);
			break;
		    case SELECT_APPLE:
			otmp = mksobj(APPLE, TRUE, FALSE);
			otmp->blessed = obj->blessed;
			otmp->cursed = obj->cursed;
			otmp->bknown = obj->bknown;
			hold_another_object(otmp, "You fumble and %s.",
			  aobjnam(otmp, "fall"), (const char *)0);
			break;
		    case SELECT_BANANA:
			otmp = mksobj(BANANA, TRUE, FALSE);
			otmp->blessed = obj->blessed;
			otmp->cursed = obj->cursed;
			otmp->bknown = obj->bknown;
			hold_another_object(otmp, "You fumble and %s.",
			  aobjnam(otmp, "fall"), (const char *)0);
			break;
		}
	    }else if(uarm && uarm == obj && yn("Restore discipline?") == 'y'){
		(void) pet_detect_and_tame(obj);
	    }else{
		if(!(uarm && uarm == obj)) You_feel("that you should be wearing the %s", xname(obj));
		obj->age = 0; //didn't invoke
	    }
	    break;
	}

	case LORDLY: {
		if(uwep && uwep == obj){
			//struct obj *otmp;
			int lordlydictum = dolordsmenu("What is your command, my Lord?", obj);
			switch(lordlydictum){
				case 0:
				break;
				/*These effects can be used at any time*/
				case COMMAND_RAPIER:
					uwep->otyp = RAPIER;
				break;
				case COMMAND_AXE:
					uwep->otyp = AXE;
				break;
				case COMMAND_MACE:
					uwep->otyp = MACE;
				break;
				case COMMAND_SPEAR:
					uwep->otyp = SPEAR;
				break;
				case COMMAND_LANCE:
					uwep->otyp = LANCE;
				break;
				/*These effects are limited by timeout*/
				case COMMAND_LADDER:
					if(u.uswallow){
						mtmp = u.ustuck;
						if (!is_whirly(mtmp->data)) {
							if (is_animal(mtmp->data))
								pline("The Rod quickly lengthens and pierces %s %s wall!",
								s_suffix(mon_nam(mtmp)), mbodypart(mtmp, STOMACH));
							if(mtmp->data == &mons[PM_JUIBLEX])
							  mtmp->mhp = (int)(.75*mtmp->mhp + 1);
							else mtmp->mhp = 1;		/* almost dead */
							expels(mtmp, mtmp->data, !is_animal(mtmp->data));
						} else{
							pline("The Rod quickly lengthens and pierces %s %s",
								s_suffix(mon_nam(mtmp)), mbodypart(mtmp, STOMACH));
							pline("However, %s is unfazed", mon_nam(mtmp));
						}
				break;
					} else if(!u.uhave.amulet){
						if(Can_rise_up(u.ux, u.uy, &u.uz)) {
							int newlev = depth(&u.uz)-1;
							d_level newlevel;
							
							pline("The Rod extends quickly, reaching for the %s",ceiling(u.ux,u.uy));
							
							get_level(&newlevel, newlev);
							if(on_level(&newlevel, &u.uz)) {
								pline("However, it is unable to pierce the %s.",ceiling(u.ux,u.uy));
				break;
							} else pline("The %s obediently yields before the Rod, and you climb to the level above.",ceiling(u.ux,u.uy));
							goto_level(&newlevel, FALSE, FALSE, FALSE);
						}
					} else{
						if (!Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz) && !Underwater) {
							pline("The Rod begins to extend quickly upwards.");
							pline("However, a mysterious force slams it back into the %s below.", surface(u.ux, u.uy));
							watch_dig((struct monst *)0, u.ux, u.uy, TRUE);
							(void) dighole(FALSE);
						}
					}
				break;
				case COMMAND_CLAIRVOYANCE:
//					do_vicinity_map(u.ux,u.uy); /*Note that this is not blocked by pointy hats*/
					do_vicinity_map();
				break;
				case COMMAND_FEAR:
					You("thrust the Rod into the air, that all may know of your Might.");
					for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
						if (DEADMONSTER(mtmp)) continue;
						if(mtmp->mcansee && couldsee(mtmp->mx,mtmp->my)) {
//								if (! resist(mtmp, sobj->oclass, 0, NOTELL))
							monflee(mtmp, 0, FALSE, FALSE);
						}
					}
				break;
				case COMMAND_LIFE:
					if(!getdir((char *)0) ||
						(!u.dx && !u.dy) ||
						((mtmp = m_at(u.ux+u.dx,u.uy+u.dy)) == 0)
					){
						pline("The Rod glows and then fades.");
					} else {
						int dmg = d(2,8);
						int rcvr;
						if (resists_drli(mtmp)){
							shieldeff(mtmp->mx, mtmp->my);
				break;
						} else {
							mtmp->mhp -= 2*dmg;
							mtmp->mhpmax -= dmg;
							mtmp->m_lev -= 2;
							if (mtmp->mhp <= 0 || mtmp->mhpmax <= 0 || mtmp->m_lev < 1)
								xkilled(mtmp, 1);
							else {
							if (canseemon(mtmp))
								pline("%s suddenly seems weaker!", Monnam(mtmp));
							}
							healup(2*dmg, 0, FALSE, TRUE);
							You_feel("better.");
						}
					}
				break;
				case COMMAND_KNEEL:
					if(!getdir((char *)0) ||
						(!u.dx && !u.dy) ||
						((mtmp = m_at(u.ux+u.dx,u.uy+u.dy)) == 0)
					){
						pline("The Rod glows and then fades.");
					} else{
						mtmp->mcanmove = 0;
						mtmp->mfrozen = max(1, u.ulevel - ((int)(mtmp->m_lev)));
						pline("%s kneels before you.",Monnam(mtmp));
					}
				break;
				default:
					pline("What is this strange command!?");
				break;
			}
			if(lordlydictum >= COMMAND_LADDER) obj->age = monstermoves + (long)(rnz(100)*(Role_if(PM_PRIEST) ? .8 : 1)); 
		} else You_feel("that you should be wielding %s", the(xname(obj)));;
	    break;
	     }
	  }
	}
    } else {
	long eprop = (u.uprops[oart->inv_prop].extrinsic ^= W_ARTI),
	     iprop = u.uprops[oart->inv_prop].intrinsic;
	boolean on = (eprop & W_ARTI) != 0; /* true if invoked prop just set */

	if(on && obj->age > monstermoves) {
	    /* the artifact is tired :-) */
	    u.uprops[oart->inv_prop].extrinsic ^= W_ARTI;
	    You_feel("that %s %s ignoring you.",
		     the(xname(obj)), otense(obj, "are"));
	    /* can't just keep repeatedly trying */
	    obj->age += (long) d(3,10);
	    return 1;
	} else if(!on) {
	    /* when turning off property, determine downtime */
	    /* arbitrary for now until we can tune this -dlc */
	    obj->age = monstermoves + rnz(100);
	}

	if ((eprop & ~W_ARTI) || iprop) {
nothing_special:
	    /* you had the property from some other source too */
	    if (carried(obj))
		You_feel("a surge of power, but nothing seems to happen.");
	    return 1;
	}
	switch(oart->inv_prop) {
	case CONFLICT:
	    if(on) You_feel("like a rabble-rouser.");
	    else You_feel("the tension decrease around you.");
	    break;
	case LEVITATION:
	    if(on) {
		float_up();
		spoteffects(FALSE);
	    } else (void) float_down(I_SPECIAL|TIMEOUT, W_ARTI);
	    break;
	case INVIS:
	    if (BInvis || Blind) goto nothing_special;
	    newsym(u.ux, u.uy);
	    if (on)
		    Your("body takes on a %s transparency...",
			 Hallucination ? "normal" : "strange");
	    else
		    Your("body seems to unfade...");
	    break;
	    /*
	case HARMONIZE:
	    {
		struct monst *mtmp;
		struct obj *otmp;
		int i,j;

		for(i = -u.ulevel; i <= u.ulevel; i++)
		    for(j = -u.ulevel; j <= u.ulevel; j++)
			if (isok(u.ux+i,u.uy+j) 
			    && i*i + j*j < u.ulevel * u.ulevel) {
			    mtmp = m_at(u.ux+i,u.uy+j);
			    if (mtmp->mtame && distu(mtmp->mx, mtmp->my) < u.ulevel) {
				mtmp->mhp = mtmp->mhpmax;
				mtmp->msleeping = mtmp->mflee = mtmp->mblinded = mtmp->mcanmove = mtmp->mfrozen = mtmp->mstun = mtmp->mconf = 0;
				mtmp->mcansee = 1;
				mtmp->mtame++;
			    }
			}
	    }
	    break;
	    */

	}
    }

    return 1;
}

/*
 * Artifact is dipped into water
 * -1 not handled here (not used up here)
 *  0 no effect but used up
 *  else return
 *  AD_FIRE, etc.
 *  Note caller should handle what happens to the medium in these cases.
 *      This only prints messages about the actual artifact.
 */

int
artifact_wet(obj, silent)
struct obj *obj;
boolean silent;
{
	 if (!obj->oartifact) return (-1);
	 switch (artilist[(int) (obj)->oartifact].attk.adtyp) {
		 case AD_FIRE:
			 if (!silent) {
				pline("A cloud of steam rises.");
				pline("%s is untouched.", The(xname(obj)));
			 }
			 return (AD_FIRE);
		 case AD_COLD:
			 if (!silent) {
				pline("Icicles form and fall from the freezing %s.",
			             the(xname(obj)));
			 }
			 return (AD_COLD);
		 case AD_ELEC:
			 if (!silent) {
				pline_The("humid air crackles with electricity from %s.",
						the(xname(obj)));
			 }
			 return (AD_ELEC);
		 case AD_ACID:
			 if (!silent) {
				 pline("Foul slimy liquid drips from %s.",
						 the(xname(obj)));
			 }
			 return (AD_ACID);
		 case AD_DRLI:
			 if (!silent) {
				pline("%s absorbs the water!", The(xname(obj)));
			 }
			 return (AD_DRLI);
		 default:
			 break;
	}
	return (-1);
}

/* WAC return TRUE if artifact is always lit */
boolean
artifact_light(obj)
    struct obj *obj;
{
    return get_artifact(obj) && (obj->oartifact == ART_SUNSWORD ||
	    obj->oartifact == ART_HOLY_SPEAR_OF_LIGHT ||
	    obj->oartifact == ART_CANDLE_OF_ETERNAL_FLAME);
}

/* KMH -- Talking artifacts are finally implemented */
void
arti_speak(obj)
    struct obj *obj;
{
	register const struct artifact *oart = get_artifact(obj);
	const char *line;
	char buf[BUFSZ];


	/* Is this a speaking artifact? */
	if (!oart || !(oart->spfx & SPFX_SPEAK))
		return;

	line = getrumor(bcsign(obj), buf, TRUE);
	if (!*line)
		line = "Slash'EM rumors file closed for renovation.";
	pline("%s:", Tobjnam(obj, "whisper"));
	verbalize("%s", line);
	return;
}

boolean
artifact_has_invprop(otmp, inv_prop)
struct obj *otmp;
uchar inv_prop;
{
	const struct artifact *arti = get_artifact(otmp);

	return((boolean)(arti && (arti->inv_prop == inv_prop)));
}

/* Return the price sold to the hero of a given artifact or unique item */
long
arti_cost(otmp)
struct obj *otmp;
{
	if (!otmp->oartifact)
	    return ((long)objects[otmp->otyp].oc_cost);
	else if (artilist[(int) otmp->oartifact].cost)
	    return (artilist[(int) otmp->oartifact].cost);
	else
	    return (100L * (long)objects[otmp->otyp].oc_cost);
}

static const char *random_seasound[] = {
	"distant waves",
	"distant surf",
	"the distant sea",
	"the call of the ocean",
	"waves against the shore",
	"flowing water",
	"the sighing of waves",
	"quarrelling gulls",
	"the song of the deep",
	"rumbling in the deeps",
	"the singing of Eidothea",
	"the laughter of the protean nymphs",
	"rushing tides",
	"the elusive sea change",
	"the silence of the sea",
	"the passage of the albatross",
	"dancing raindrops",
	"coins rolling on the seabed",
	"treasure galleons crumbling in the depths",
	"waves lapping against a hull"
};

/* Polymorph obj contents */
void
arti_poly_contents(obj)
    struct obj *obj;
{
    struct obj *dobj = 0;  /*object to be deleted*/
    struct obj *otmp;
	You_hear("%s.",random_seasound[rn2(SIZE(random_seasound))]);
	for (otmp = obj->cobj; otmp; otmp = otmp->nobj){
		if (dobj) {
			delobj(dobj);
			dobj = 0;
		}
		if(!obj_resists(otmp, 5, 95)){
			/* KMH, conduct */
			u.uconduct.polypiles++;
			/* any saved lock context will be dangerously obsolete */
			if (Is_box(otmp)) (void) boxlock(otmp, obj);

			if (obj_shudders(otmp)) {
				dobj = otmp;
			}
			else otmp = poly_obj(otmp, STRANGE_OBJECT);
		}
	}
	if (dobj) {
		delobj(dobj);
		dobj = 0;
	}
}

int
dopetmenu(prompt, obj)
const char *prompt;
struct obj *obj;
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	char incntlet = 'a';
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "What will you take out?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	Sprintf(buf, "Magic whistle");
	any.a_int = SELECT_WHISTLE;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	
	Sprintf(buf, "Leash");
	any.a_int = SELECT_LEASH;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	
	
	Sprintf(buf, "Saddle");
	any.a_int = SELECT_SADDLE;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	
	
	Sprintf(buf, "Tripe");
	any.a_int = SELECT_TRIPE;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	
	
	Sprintf(buf, "Apple");
	any.a_int = SELECT_APPLE;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	
	
	Sprintf(buf, "Banana");
	any.a_int = SELECT_BANANA;	/* must be non-zero */
	add_menu(tmpwin, NO_GLYPH, &any,
		incntlet, 0, ATR_NONE, buf,
		MENU_UNSELECTED);
	incntlet = (incntlet != 'z') ? (incntlet+1) : 'A';
	
	end_menu(tmpwin, prompt);

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	return (n > 0) ? selected[0].item.a_int : 0;
}


int
dolordsmenu(prompt, obj)
const char *prompt;
struct obj *obj;
{
	winid tmpwin;
	int n, how;
	char buf[BUFSZ];
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	Sprintf(buf, "What do you command?");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	
	if(obj->otyp != RAPIER){
		Sprintf(buf, "Become a rapier");
		any.a_int = COMMAND_RAPIER;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'r', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	if(obj->otyp != AXE){
		Sprintf(buf, "Become an axe");
		any.a_int = COMMAND_AXE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'a', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	if(obj->otyp != MACE){
		Sprintf(buf, "Become a mace");
		any.a_int = COMMAND_MACE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'm', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	if(obj->otyp != SPEAR){
		Sprintf(buf, "Become a spear");
		any.a_int = COMMAND_SPEAR;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			's', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	if(obj->otyp != LANCE){
		Sprintf(buf, "Become a lance");
		any.a_int = COMMAND_LANCE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'l', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	
	if(obj->age < monstermoves){
		if(obj->otyp == MACE && (
		   u.uswallow || 
		   (!u.uhave.amulet && Can_rise_up(u.ux, u.uy, &u.uz)) || 
		   (u.uhave.amulet && !Is_airlevel(&u.uz) && !Is_waterlevel(&u.uz) && !Underwater) 
		  )){
			Sprintf(buf, "Become a ladder");
			any.a_int = COMMAND_LADDER;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				'L', 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		
		Sprintf(buf, "Show me my surroundings");
		any.a_int = COMMAND_CLAIRVOYANCE;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'S', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		
		Sprintf(buf, "Inspire fear");
		any.a_int = COMMAND_FEAR;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'F', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
		
		if(obj->otyp == SPEAR){
			Sprintf(buf, "Give me your life");
			any.a_int = COMMAND_LIFE;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				'G', 0, ATR_NONE, buf,
				MENU_UNSELECTED);
		}
		
		Sprintf(buf, "Kneel");
		any.a_int = COMMAND_KNEEL;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			'K', 0, ATR_NONE, buf,
			MENU_UNSELECTED);
	}
	end_menu(tmpwin, prompt);

	how = PICK_ONE;
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	return (n > 0) ? selected[0].item.a_int : 0;
}

#endif /* OVLB */

/*artifact.c*/
