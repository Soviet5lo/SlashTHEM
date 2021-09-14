/* 	SCCS Id: @(#)smith.c 	3.1 	94/10/04	*/
/* NetHack may be freely redistributed.  See license for details. */
/* Copyright 1994, Sebastian Klein */

#include "hack.h"
#include "lev.h"
#include <stdio.h>
#include <ctype.h>

#ifdef OVL0

static int time_finished = -1;
static struct obj *weapon = NULL;

static char *not_interested = "signalize that you are not interested anymore.";

struct prices_t
{
	int artifact;
	long price;
};

/* the code relies on that a) the artifacts are presented in ascending */
/*			      order of price */
/*		       and b) no two given artifacts have the same price. */
/* 5lo: Guidelines I'm setting for this are: No sacrifice gifts (to prevent
 * characters from buying their gift), No non-weapons (only for now?) and
 * no artifacts set to nogen.
 */
static struct prices_t prices[] = {
	{ ART_STING,			500 },
	{ ART_ORCRIST,			600 },
	{ ART_SLOW_BLADE,		700 },
	{ ART_QUICK_BLADE,		750 },
	{ ART_ELFRIST,			800 },
	{ ART_FUNGISWORD,		900 },
	{ ART_MUMAKBANE,		950 },
	{ ART_SILVERSHARD,		1000 },
	{ ART_LUCKBLADE,		1200 },
	{ ART_SWORD_OF_BALANCE,		1500 },
	{ ART_PETSLAYER,		2000 },
	{ ART_GRIMTOOTH,		2500 },
	{ ART_YOICHI_NO_YUMI,		3000 },
	{ ART_OGRESMASHER,		3500 },
	{ ART_TROLLSBANE,		4000 },
	{ ART_WORMBITER,		4500 },
	{ ART_DRAGONBANE,		5000 },
	{ ART_WEREBANE,			5200 },
	{ ART_SPINESEEKER,		6000 },
	{ ART_FIRE_BRAND,		6500 },
	{ ART_FROST_BRAND,		6750 },
	{ ART_SUNSWORD,			7000 },
	{ ART_SHOCK_BRAND,		7250 },
	{ ART_ACID_BRAND,		7500 },
	{ ART_DIPLOMAT,			8000 },
	{ ART_MIRROR_BRAND,		8250 },
	{ ART_GIANTKILLER,		8500 },
	{ ART_DEMONBANE,		9000 },
	{ ART_SNICKERSNEE,		9500 },
	{ ART_PLAGUE,			10000 },
	{ ART_HELLFIRE,			11000 },
	{ ART_STORMBRINGER,		12000 },
	{ ART_VORPAL_BLADE,		15000 },
	{ ART_GRAYSWANDIR,		17500 },
	{ ART_HOUCHOU,			50000 }, /* Price adjusted to be less sadistic */
	{ 0,				0 }
};

static boolean acceptable_artifact(int artifact)
{
	if (exists_nrartifact(artifact)) return(FALSE);
	if (nogen_nrartifact(artifact)) return(FALSE);
	return(TRUE);
}

static int artiid2i(int artifact)
{
	int i;

	i = 0;
	while (prices[i].artifact > 0) {
		if (prices[i].artifact == artifact) return(i);
		i++;
	}
	return(-1);
}

/* consistent with shk.c */
static long adjust_gold(long amount)
{
	long x;

	x = amount;
	if (ACURR(A_CHA) > 18) 		x *= 2L;
	else if (ACURR(A_CHA) > 17)	x += x / 3L;
	else if (ACURR(A_CHA) > 15)	x += x / 4L;
	else if (ACURR(A_CHA) < 6)	x /= 2L;
	else if (ACURR(A_CHA) < 8)	x -= x / 2L;
	else if (ACURR(A_CHA) < 11)	x -= x / 3L;
	if (x <= 0L) x = 1L;
	return(x);
}

static long unadjust_gold(long ramount)
{
	long x;

	x = ramount;
	if (ACURR(A_CHA) > 18)		x /= 2L;
	else if (ACURR(A_CHA) > 17)	x -= x / 3L;
	else if (ACURR(A_CHA) > 15)	x -= x / 4L;
	else if (ACURR(A_CHA) < 6)	x *= 2L;
	else if (ACURR(A_CHA) < 8)	x += x / 2L;
	else if (ACURR(A_CHA) < 11)	x += x / 3L;
	if (x <= 0L) x = 1L;
	return(x);
}

static void apply_extras(struct obj *o,int extras)
{
	boolean id;

	id = FALSE;
	while (extras > 0) {
		switch(rn2(5)) {
			case 0:	if (!o->blessed) {
					o->cursed = FALSE;
					o->blessed = TRUE;
					break;
				} /* else fallthrough */
			case 1: if (!id) {
					makeknown(o->otyp);
					o->known = o->dknown = o->bknown = TRUE;
					o->rknown = id = TRUE;
					break;
				} /* else fallthrough */
			case 2:	if (!o->oerodeproof) {
					o->oerodeproof = TRUE;
					o->greased = FALSE;
					break;
				} /* else fallthrough (twice) */
			case 3: if (!o->greased && !o->oerodeproof) {
					o->greased = TRUE;
					break;
				} /* else fallthrough */
			case 4: o->spe++;
				break;
		}
		extras--;
	}
	if (o->spe > 5) o->spe = 5;
}

static boolean select_weapon(long ramount)
{
	struct obj *otmp;
	int i,j;

	if (ramount < prices[0].price) {
		verbalize("What? That little? You're never going to get a personal weapon for that!");
		if (ramount >= 100) {	
			verbalize("But you may have this.");
			otmp = mksobj(LONG_SWORD,TRUE,FALSE);
			otmp->spe = rn2(5) ? 0 : 1;
			otmp->oeroded = 1;		
			hold_another_object(otmp,"Oops! %s out of your grasp!",The(aobjnam(otmp,"slip")),(const char *) 0);
			return(TRUE);
		} else {
			return(FALSE);
		}
	}

	i = 0;
	while (ramount > prices[i].price && prices[i].artifact != 0) i++;
	if (prices[i].artifact == 0) i--;

	j = 0;
	while (i >= 0 && j <= 4 && !acceptable_artifact(prices[i].artifact)) {
		j++;
		i--;
	}
	if (!acceptable_artifact(prices[i].artifact)) {
		verbalize("I'm sorry, but I am out of stock in this price class.");
		return(FALSE);
	}

	weapon = mksobj(nrartifact_obtype(prices[i].artifact),TRUE,FALSE);
	weapon = oname(weapon,artiname(prices[i].artifact)); /* 5lo: Minor edit */
	weapon->cursed = FALSE;
	apply_extras(weapon,j);

	return(TRUE);
}

static void make_buy1()
{
	char artname[BUFSZ],buf[BUFSZ];
	int artino,artiind;
	boolean ok;
	int i;
	int extras;
	long amount;
	char c;
	int time_to_take;

	if (weapon != NULL) {
		impossible("weapon left over? (1)");
		weapon = NULL;
	}

	ok = FALSE;
	do {
		getlin("How do you want me to call your new artifact?",artname);
		if (artname[0] == 0) {
			You(not_interested);
			return;
		}			
		artino = artifact_name2no(artname);
		if (!artino) {
			verbalize
("I think this is a dull name. I really can't associate anything with it.");
			continue;
		}
		if (exists_nrartifact(artino)) {
			verbalize
("I have already used that name and I never use the same name again.");
			continue;
		}
		artiind = artiid2i(artino);
		if (!acceptable_artifact(artino) || artiind == -1) {
			verbalize
			("I don't know ... I don't think that's a good name.");
			continue;
		}
		ok = TRUE;
	} while(!ok);

	getlin("How many extras do you want?",buf);
	i = 0;
	while (buf[i] != 0) {
		if (!isdigit(buf[i])) {
			verbalize("What a strange number this is!");
			return;
		}
		i++;
	}
	extras = atoi(buf);
	if (extras > 6) {
		verbalize("Sorry, but I cannot offer more that 6 extras!");
		return;
	}

	amount = unadjust_gold(prices[artiind].price + extras*1000L);

	verbalize("That will cost you %li zorkmids.",amount);
	if (u.ugold < amount) {
		pline("Unfortunately, you don't have enough money.");
		You(not_interested);
		return;
	}
	c = yn_function("Accept?","yn",'y');
	if (tolower(c) == 'n') {
		You(not_interested);
		return;
	}
	u.ugold -= amount;

	weapon = mksobj(nrartifact_obtype(prices[artiind].artifact),TRUE,FALSE);
	weapon = oname(weapon,artiname(prices[artiind].artifact)); /* 5lo: Minor edit */
	weapon->cursed = FALSE;
	apply_extras(weapon,extras);

	time_to_take = adjust_gold(amount) / 10;
	verbalize("I'll immediately start working. Please come back in approximately %i00 turns.",time_to_take / 100);
	verbalize("to receive your new weapon.");
#ifdef WIZARD
	pline("(%i)",time_to_take);
#endif
	time_finished = moves + time_to_take;
}

static void make_buy2()
{
	char buf[BUFSZ];
	int ok;
	int i;
	long amount;
	int time_to_take;


	if (weapon != NULL) {
		impossible("weapon left over (2)?");
		weapon = NULL;
	}

	do {
		ok = 1;
		getlin("How much money are you willing to invest?",buf);
		i = 0;
		if (buf[0] == 0) {
			You(not_interested);
			return;
		}
		while (buf[i] != 0) {
	 		if (!isdigit(buf[i])) ok = 0;
			i++;
		}
		if (!ok) {
			verbalize("Please talk English to me!");
			continue;
		}
		amount = atol(buf);
		if (amount <= 0) {
			You(not_interested);
			return;
		}
		if (amount > u.ugold) {
			You("don't have that much money!");
			ok = 0;
			continue;
		}
	} while (!ok);

	if (select_weapon(adjust_gold(amount))) {
		u.ugold -= amount;
		You("hand the smith %li zorkmids.",amount);
	}

	if (!weapon) return;

	time_to_take = adjust_gold(amount) / 10;
	
	verbalize("I will immediately start working. Please come back in approximately %i00 turns",time_to_take / 100);
	verbalize("to receive your new weapon.");
#ifdef WIZARD
	if (wizard) pline("(%i)",time_to_take);
#endif
	time_finished = moves + time_to_take;
}

static void hand_object(void)
{
	if (weapon == NULL) {
		impossible("no weapon to give?");
		return;
	}

	verbalize("This is your weapon. It shall be known as %s from now on.",artiname(weapon->oartifact));
	hold_another_object(weapon,"Oops! You accidentally drop your new weapon!",(const char *) 0,(const char *) 0);
	time_finished = -1;
	weapon = NULL;
}

void smith_selling(struct monst *smith)
{
	if (time_finished == -1) {
		if (ACURR(A_CHA) > 14) {
			make_buy1();
		} else {
			make_buy2();
		}
	} else if (time_finished <= moves) {
		hand_object();
	} else {
		verbalize("I'm working. Please don't disturb me again!");
		time_finished += 5;	/* he was interrupted */
	}
}

/* used in save.c */
void savesmithstate(register int fd, register int mode) {
	if (!perform_bwrite(mode))
		return;
	bwrite(fd, (genericptr_t) &time_finished, sizeof (int));
	if (time_finished != -1) {
		unsigned int xl;
		xl = weapon->oxlth + weapon->onamelth;
		bwrite(fd, (genericptr_t) &xl, sizeof(unsigned int));
		bwrite(fd, (genericptr_t) weapon, xl + sizeof(struct obj));
	}
}

/* used in restore.c */
void restsmithstate(int fd) {
	mread(fd, (genericptr_t) &time_finished, sizeof(int));
	if (time_finished != -1) {
		unsigned int xl;
		mread(fd, (genericptr_t) &xl, sizeof (unsigned int));
		weapon = newobj(xl);
		mread(fd, (genericptr_t) weapon, xl + sizeof (struct obj));
	}
}

#endif /* OVL0 */

/* smith.c */

