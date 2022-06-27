/*	SCCS Id: @(#)pager.c	3.4	2003/08/13	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* This file contains the command routines dowhatis() and dohelp() and */
/* a few other help related facilities */

#include "hack.h"
#include "dlb.h"
#include "edog.h"

#ifdef UNICODE
#include <wchar.h>
#include "unicode.h"
#endif

STATIC_DCL boolean FDECL(is_swallow_sym, (int));
STATIC_DCL int FDECL(append_str, (char *, const char *));
STATIC_DCL struct permonst * FDECL(lookat, (int, int, char *, char *));
STATIC_DCL int FDECL(do_look, (BOOLEAN_P));
STATIC_DCL boolean FDECL(help_menu, (int *));
#ifdef PORT_HELP
extern void NDECL(port_help);
#endif
#ifdef EXTENDED_INFO
STATIC_DCL char * FDECL(get_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL char * FDECL(get_generation_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL char * FDECL(get_resistance_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL char * FDECL(get_flag_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL char * FDECL(get_speed_description_of_monster_type, (struct permonst *, char *));
STATIC_DCL int FDECL(generate_list_of_resistances, (char *, /*uchar*/unsigned long));
STATIC_DCL void FDECL(append_newline_to_pline_string, (char * buf));
#endif
/* Returns "true" for characters that could represent a monster's stomach. */
STATIC_OVL boolean
is_swallow_sym(c)
int c;
{
    int i;
    for (i = S_sw_tl; i <= S_sw_br; i++)
	if ((int)showsyms[i] == c) return TRUE;
    return FALSE;
}

/*
 * Append new_str to the end of buf if new_str doesn't already exist as
 * a substring of buf.  Return 1 if the string was appended, 0 otherwise.
 * It is expected that buf is of size BUFSZ.
 */
STATIC_OVL int
append_str(buf, new_str)
    char *buf;
    const char *new_str;
{
    int space_left;	/* space remaining in buf */

    if (strstri(buf, new_str)) return 0;

    space_left = BUFSZ - strlen(buf) - 1;
    (void) strncat(buf, " or ", space_left);
    (void) strncat(buf, new_str, space_left - 4);
    return 1;
}

/*
 * Return the name of the glyph found at (x,y).
 * If not hallucinating and the glyph is a monster, also monster data.
 */
STATIC_OVL struct permonst *
lookat(x, y, buf, monbuf)
    int x, y;
    char *buf, *monbuf;
{
    register struct monst *mtmp = (struct monst *) 0;
    struct permonst *pm = (struct permonst *) 0;
    int glyph;

    buf[0] = monbuf[0] = 0;
    glyph = glyph_at(x,y);
    if (u.ux == x && u.uy == y && senseself()) {
	char race[QBUFSZ];

	/* if not polymorphed, show both the role and the race */
	race[0] = 0;
	if (!Upolyd) {
	    Sprintf(race, "%s ", urace.adj);
	}

	Sprintf(buf, "%s%s%s called %s",
		Invis ? "invisible " : "",
		race,
		mons[u.umonnum].mname,
		plname);
	/* file lookup can't distinguish between "gnomish wizard" monster
	   and correspondingly named player character, always picking the
	   former; force it to find the general "wizard" entry instead */
	if (Role_if(PM_WIZARD) && Race_if(PM_GNOME) && !Upolyd)
	    pm = &mons[PM_WIZARD];

#if 0
	char race[QBUFSZ], role[QBUFSZ];

	/* if not polymorphed, show both the role and the race */
	role[0] = 0;
	race[0] = 0;
	
	if (!Upolyd) {
	    Sprintf(race, "%s ", urace.adj);
	    Sprintf(role, "%s ", urole.name);
	} else Sprintf(race, "%s ", mons[u.umonnum].mname);

	Sprintf(buf, "%s%s%s called %s",
		Invis ? "invisible " : "",
		race,
		role,
		plname);
#endif


#ifdef STEED
	if (u.usteed) {
	    char steedbuf[BUFSZ];

	    Sprintf(steedbuf, ", mounted on %s", y_monnam(u.usteed));
	    /* assert((sizeof buf >= strlen(buf)+strlen(steedbuf)+1); */
	    Strcat(buf, steedbuf);
	}
#endif
	/* When you see yourself normally, no explanation is appended
	   (even if you could also see yourself via other means).
	   Sensing self while blind or swallowed is treated as if it
	   were by normal vision (cf canseeself()). */
	if ((Invisible || u.uundetected) && !Blind && !u.uswallow) {
	    unsigned how = 0;

	    if (Infravision)	 how |= 1;
	    if (Unblind_telepat) how |= 2;
	    if (Detect_monsters) how |= 4;

	    if (how)
		Sprintf(eos(buf), " [seen: %s%s%s%s%s]",
			(how & 1) ? "infravision" : "",
			/* add comma if telep and infrav */
			((how & 3) > 2) ? ", " : "",
			(how & 2) ? "telepathy" : "",
			/* add comma if detect and (infrav or telep or both) */
			((how & 7) > 4) ? ", " : "",
			(how & 4) ? "monster detection" : "");
	}
    } else if (u.uswallow) {
	/* all locations when swallowed other than the hero are the monster */
	Sprintf(buf, "interior of %s",
				    Blind ? "a monster" : a_monnam(u.ustuck));
	pm = u.ustuck->data;
    } else if (glyph_is_monster(glyph)) {
	bhitpos.x = x;
	bhitpos.y = y;
	mtmp = m_at(x,y);
	if (mtmp != (struct monst *) 0) {
	    char *name, monnambuf[BUFSZ];
	    boolean accurate = !Hallucination;

	    if (mtmp->data == &mons[PM_COYOTE] && accurate)
		name = coyotename(mtmp, monnambuf);
	    else
		name = distant_monnam(mtmp, ARTICLE_NONE, monnambuf);

	    pm = mtmp->data;
	    Sprintf(buf, "%s%s%s",
		    (mtmp->mx != x || mtmp->my != y) ?
			((mtmp->isshk && accurate)
				? "tail of " : "tail of a ") : "",
		    (mtmp->mtame && accurate) ? 
		    (EDOG(mtmp)->friend ? "friendly " : "tame ") :
		    (mtmp->mpeaceful && accurate) ? "peaceful " : "",
		    name);
	    char *mwounds = mon_wounds(mtmp);
	    if (mwounds) {
		Strcat(buf, ", ");
		Strcat(buf, mwounds);
	    } if (u.ustuck == mtmp)
		Strcat(buf, (Upolyd && sticks(youmonst.data)) ?
			", being held" : ", holding you");
	    if (mtmp->mleashed)
		Strcat(buf, ", leashed to you");

	    if (mtmp->mtrapped && cansee(mtmp->mx, mtmp->my)) {
		struct trap *t = t_at(mtmp->mx, mtmp->my);
		int tt = t ? t->ttyp : NO_TRAP;

		/* newsym lets you know of the trap, so mention it here */
		if (tt == BEAR_TRAP || tt == PIT ||
			tt == SPIKED_PIT || tt == WEB)
		    Sprintf(eos(buf), ", trapped in %s",
			    an(defsyms[trap_to_defsym(tt)].explanation));
	    }

	    {
		int ways_seen = 0, normal = 0, xraydist;
		boolean useemon = (boolean) canseemon(mtmp);

		xraydist = (u.xray_range<0) ? -1 : u.xray_range * u.xray_range;
		/* normal vision */
		if ((mtmp->wormno ? worm_known(mtmp) : cansee(mtmp->mx, mtmp->my)) &&
			mon_visible(mtmp) && !mtmp->minvis) {
		    ways_seen++;
		    normal++;
		}
		/* see invisible */
		if (useemon && mtmp->minvis)
		    ways_seen++;
		/* infravision */
		if ((!mtmp->minvis || See_invisible) && see_with_infrared(mtmp))
		    ways_seen++;
		/* telepathy */
		if (tp_sensemon(mtmp))
		    ways_seen++;
		/* xray */
		if (useemon && xraydist > 0 &&
			distu(mtmp->mx, mtmp->my) <= xraydist)
		    ways_seen++;
		if (Detect_monsters)
		    ways_seen++;
		if (MATCH_WARN_OF_MON(mtmp))
		    ways_seen++;

		if (ways_seen > 1 || !normal) {
		    if (normal) {
			Strcat(monbuf, "normal vision");
			/* can't actually be 1 yet here */
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (useemon && mtmp->minvis) {
			Strcat(monbuf, "see invisible");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if ((!mtmp->minvis || See_invisible) &&
			    see_with_infrared(mtmp)) {
			Strcat(monbuf, "infravision");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (tp_sensemon(mtmp)) {
			Strcat(monbuf, "telepathy");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (useemon && xraydist > 0 &&
			    distu(mtmp->mx, mtmp->my) <= xraydist) {
			/* Eyes of the Overworld */
			Strcat(monbuf, "astral vision");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (Detect_monsters) {
			Strcat(monbuf, "monster detection");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_POISON_WARNING && poisonous(mtmp->data)) {
			Strcat(monbuf, "warned of poisonous monsters");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_OWN_RACE_WARNING && your_race(mtmp->data)) {
			Strcat(monbuf, "warned of monsters that are the same race as you");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (uamul && uamul->otyp == AMULET_OF_COVETOUS_WARNING && is_covetous(mtmp->data)) {
			Strcat(monbuf, "warned of covetous monsters");
			if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		    if (MATCH_WARN_OF_MON(mtmp) || (uamul && uamul->otyp == AMULET_OF_UNDEAD_WARNING && is_undead(mtmp->data) )) {
		    	char wbuf[BUFSZ];
			if (Hallucination)
				Strcat(monbuf, "paranoid delusion");
			else {
				Sprintf(wbuf, "warned of %s",
					makeplural(mtmp->data->mname));
		    		Strcat(monbuf, wbuf);
		    	}
		    	if (ways_seen-- > 1) Strcat(monbuf, ", ");
		    }
		}
	    }
	}
    }
    else if (glyph_is_object(glyph)) {
	struct obj *otmp = vobj_at(x,y);

	if (!otmp || otmp->otyp != glyph_to_obj(glyph)) {
	    if (glyph_to_obj(glyph) != STRANGE_OBJECT) {
		otmp = mksobj(glyph_to_obj(glyph), FALSE, FALSE);
		if (otmp->oclass == COIN_CLASS)
		    otmp->quan = 2L; /* to force pluralization */
		else if (otmp->otyp == SLIME_MOLD)
		    otmp->spe = current_fruit;	/* give the fruit a type */
		Strcpy(buf, distant_name(otmp, xname));
		dealloc_obj(otmp);
	    }
	} else
	    Strcpy(buf, distant_name(otmp, xname));

	if (levl[x][y].typ == STONE || levl[x][y].typ == SCORR)
	    Strcat(buf, " embedded in stone");
	else if (IS_WALL(levl[x][y].typ) || levl[x][y].typ == SDOOR)
	    Strcat(buf, " embedded in a wall");
	else if (closed_door(x,y))
	    Strcat(buf, " embedded in a door");
	else if (is_pool(x,y, FALSE))
	    Strcat(buf, " in water");
	else if (is_lava(x,y))
	    Strcat(buf, " in molten lava");	/* [can this ever happen?] */
    } else if (glyph_is_trap(glyph)) {
	int tnum = what_trap(glyph_to_trap(glyph));
	Strcpy(buf, defsyms[trap_to_defsym(tnum)].explanation);
    } else if(!glyph_is_cmap(glyph)) {
	Strcpy(buf,"unexplored area");
    } else switch(glyph_to_cmap(glyph)) {
    case S_altar:
	if(!In_endgame(&u.uz))
	    Sprintf(buf, "%s altar",
		align_str(Amask2align(levl[x][y].altarmask & ~AM_SHRINE)));
	else Sprintf(buf, "aligned altar");
	break;
    case S_ndoor:
	if (is_drawbridge_wall(x, y) >= 0)
	    Strcpy(buf,"open drawbridge portcullis");
	else if ((levl[x][y].doormask & ~D_TRAPPED) == D_BROKEN)
	    Strcpy(buf,"broken door");
	else
	    Strcpy(buf,"doorway");
	break;
    case S_cloud:
	Strcpy(buf, Is_airlevel(&u.uz) ? "cloudy area" : "fog/vapor cloud");
	break;
    case S_tree:
	Strcpy(buf,rmname(&levl[x][y]));
	break;
    case S_water:
    case S_pool:
	Strcpy(buf, level.flags.lethe? "sparkling water" : "water");
	break;
    default:
	Strcpy(buf,defsyms[glyph_to_cmap(glyph)].explanation);
	break;
    }

    return ((pm && !Hallucination) ? pm : (struct permonst *) 0);
}

/*
 * Look in the "data" file for more info.  Called if the user typed in the
 * whole name (user_typed_name == TRUE), or we've found a possible match
 * with a character/glyph and flags.help is TRUE.
 *
 * NOTE: when (user_typed_name == FALSE), inp is considered read-only and
 *	 must not be changed directly, e.g. via lcase(). We want to force
 *	 lcase() for data.base lookup so that we can have a clean key.
 *	 Therefore, we create a copy of inp _just_ for data.base lookup.
 */
void
checkfile(inp, pm, user_typed_name, without_asking)
    char *inp;
    struct permonst *pm;
    boolean user_typed_name, without_asking;
{
    dlb *fp;
    char buf[BUFSZ], newstr[BUFSZ];
    char *ep, *dbase_str;
    long txt_offset;
    int chk_skip;
    boolean found_in_file = FALSE, skipping_entry = FALSE;

    fp = dlb_fopen_area(NH_DATAAREA, NH_DATAFILE, "r");
    if (!fp) {
	pline("Cannot open data file!");
	return;
    }

    /* To prevent the need for entries in data.base like *ngel to account
     * for Angel and angel, make the lookup string the same for both
     * user_typed_name and picked name.
     */
    if (pm != (struct permonst *) 0 && !user_typed_name)
	dbase_str = strcpy(newstr, pm->mname);
    else dbase_str = strcpy(newstr, inp);
    (void) lcase(dbase_str);

    if (!strncmp(dbase_str, "interior of ", 12))
	dbase_str += 12;
    if (!strncmp(dbase_str, "a ", 2))
	dbase_str += 2;
    else if (!strncmp(dbase_str, "an ", 3))
	dbase_str += 3;
    else if (!strncmp(dbase_str, "the ", 4))
	dbase_str += 4;
    if (!strncmp(dbase_str, "tame ", 5))
	dbase_str += 5;
    else if (!strncmp(dbase_str, "peaceful ", 9))
	dbase_str += 9;
    if (!strncmp(dbase_str, "invisible ", 10))
	dbase_str += 10;
    if (!strncmp(dbase_str, "statue of ", 10))
	dbase_str[6] = '\0';
    else if (!strncmp(dbase_str, "figurine of ", 12))
	dbase_str[8] = '\0';

    /* Make sure the name is non-empty. */
    if (*dbase_str) {
	/* adjust the input to remove "named " and convert to lower case */
	char *alt = 0;	/* alternate description */

	if ((ep = strstri(dbase_str, " named ")) != 0)
	    alt = ep + 7;
	else
	    ep = strstri(dbase_str, " called ");
	if (!ep) ep = strstri(dbase_str, ", ");
	if (ep && ep > dbase_str) *ep = '\0';

	/*
	 * If the object is named, then the name is the alternate description;
	 * otherwise, the result of makesingular() applied to the name is. This
	 * isn't strictly optimal, but named objects of interest to the user
	 * will usually be found under their name, rather than under their
	 * object type, so looking for a singular form is pointless.
	 */

	if (!alt)
	    alt = makesingular(dbase_str);
	else
	    if (user_typed_name)
		(void) lcase(alt);

	/* skip first record; read second */
	txt_offset = 0L;
	if (!dlb_fgets(buf, BUFSZ, fp) || !dlb_fgets(buf, BUFSZ, fp)) {
	    impossible("can't read 'data' file");
	    (void) dlb_fclose(fp);
	    return;
	} else if (sscanf(buf, "%8lx\n", &txt_offset) < 1 || txt_offset <= 0)
	    goto bad_data_file;

	/* look for the appropriate entry */
	while (dlb_fgets(buf,BUFSZ,fp)) {
	    if (*buf == '.') break;  /* we passed last entry without success */

	    if (digit(*buf)) {
		/* a number indicates the end of current entry */
		skipping_entry = FALSE;
	    } else if (!skipping_entry) {
		if (!(ep = index(buf, '\n'))) goto bad_data_file;
		*ep = 0;
		/* if we match a key that begins with "~", skip this entry */
		chk_skip = (*buf == '~') ? 1 : 0;
		if (pmatch(&buf[chk_skip], dbase_str) ||
			(alt && pmatch(&buf[chk_skip], alt))) {
		    if (chk_skip) {
			skipping_entry = TRUE;
			continue;
		    } else {
			found_in_file = TRUE;
			break;
		    }
		}
	    }
	}
    }

    if(found_in_file) {
	long entry_offset;
	int  entry_count;
	int  i;

	/* skip over other possible matches for the info */
	do {
	    if (!dlb_fgets(buf, BUFSZ, fp)) goto bad_data_file;
	} while (!digit(*buf));
	if (sscanf(buf, "%ld,%d\n", &entry_offset, &entry_count) < 2) {
bad_data_file:	impossible("'data' file in wrong format");
		(void) dlb_fclose(fp);
		return;
	}

	if (user_typed_name || without_asking || yn("More info?") == 'y') {
	    winid datawin;

	    if (dlb_fseek(fp, txt_offset + entry_offset, SEEK_SET) < 0) {
		pline("? Seek error on 'data' file!");
		(void) dlb_fclose(fp);
		return;
	    }
	    datawin = create_nhwindow(NHW_MENU);
	    for (i = 0; i < entry_count; i++) {
		if (!dlb_fgets(buf, BUFSZ, fp)) goto bad_data_file;
		if ((ep = index(buf, '\n')) != 0) *ep = 0;
		if (index(buf+1, '\t') != 0) (void) tabexpand(buf+1);
		putstr(datawin, 0, buf+1);
	    }
	    display_nhwindow(datawin, FALSE);
	    destroy_nhwindow(datawin);
	}
    } else if (user_typed_name)
	pline("I don't have any information on those things.");

    (void) dlb_fclose(fp);
}

/* getpos() return values */
#define LOOK_TRADITIONAL	0	/* '.' -- ask about "more info?" */
#define LOOK_QUICK		1	/* ',' -- skip "more info?" */
#define LOOK_ONCE		2	/* ';' -- skip and stop looping */
#define LOOK_VERBOSE		3	/* ':' -- show more info w/o asking */

/* also used by getpos hack in do_name.c */
const char what_is_an_unknown_object[] = "an unknown object";

STATIC_OVL int
do_look(quick)
    boolean quick;	/* use cursor && don't search for "more info" */
{
    char    out_str[BUFSZ], look_buf[BUFSZ];
    const char *x_str, *firstmatch = 0;
    struct permonst *pm = 0;
    int     i, ans = 0;
    int     sym;		/* typed symbol or converted glyph */
    int	    found;		/* count of matching syms found */
    coord   cc;			/* screen pos of unknown glyph */
    boolean save_verbose;	/* saved value of flags.verbose */
    boolean from_screen;	/* question from the screen */
    boolean need_to_look;	/* need to get explan. from glyph */
    boolean hit_trap;		/* true if found trap explanation */
    int skipped_venom;		/* non-zero if we ignored "splash of venom" */
    static const char *mon_interior = "the interior of a monster";

    if (quick) {
	from_screen = TRUE;	/* yes, we want to use the cursor */
    } else {
	i = ynq("Specify unknown object by cursor?");
	if (i == 'q') return 0;
	from_screen = (i == 'y');
    }

    if (from_screen) {
	cc.x = u.ux;
	cc.y = u.uy;
	sym = 0;		/* gcc -Wall lint */
    } else {
	getlin("Specify what? (type the word)", out_str);
	if (out_str[0] == '\0' || out_str[0] == '\033')
	    return 0;

	if (out_str[1]) {	/* user typed in a complete string */
	    checkfile(out_str, pm, TRUE, TRUE);
	    return 0;
	}
	sym = out_str[0];
    }

    /* Save the verbose flag, we change it later. */
    save_verbose = flags.verbose;
    flags.verbose = flags.verbose && !quick;
    /*
     * The user typed one letter, or we're identifying from the screen.
     */
    do {
	/* Reset some variables. */
	need_to_look = FALSE;
	pm = (struct permonst *)0;
	skipped_venom = 0;
	found = 0;
	out_str[0] = '\0';

	if (from_screen) {
	    int glyph;	/* glyph at selected position */

	    if (flags.verbose)
		pline("Please move the cursor to %s.",
		       what_is_an_unknown_object);
	    else {
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		pline("Pick an object."); }

	    ans = getpos(&cc, quick, what_is_an_unknown_object);
	    if (ans < 0 || cc.x < 0) {
		flags.verbose = save_verbose;
		return 0;	/* done */
	    }
	    flags.verbose = FALSE;	/* only print long question once */

	    /* Convert the glyph at the selected position to a symbol. */
	    glyph = glyph_at(cc.x,cc.y);
	    if (glyph_is_cmap(glyph)) {
		sym = showsyms[glyph_to_cmap(glyph)];
	    } else if (glyph_is_trap(glyph)) {
		sym = showsyms[trap_to_defsym(glyph_to_trap(glyph))];
	    } else if (glyph_is_object(glyph)) {
		sym = oc_syms[(int)objects[glyph_to_obj(glyph)].oc_class];
		if (sym == '`' && iflags.bouldersym && (int)glyph_to_obj(glyph) == BOULDER)
			sym = iflags.bouldersym;
	    } else if (glyph_is_monster(glyph)) {
		/* takes care of pets, detected, ridden, and regular mons */
		sym = monsyms[(int)mons[glyph_to_mon(glyph)].mlet];
	    } else if (glyph_is_swallow(glyph)) {
		sym = showsyms[glyph_to_swallow(glyph)+S_sw_tl];
	    } else if (glyph_is_invisible(glyph)) {
		sym = DEF_INVISIBLE;
	    } else if (glyph_is_warning(glyph)) {
		sym = glyph_to_warning(glyph);
	    	sym = warnsyms[sym];
	    } else {
		impossible("do_look:  bad glyph %d at (%d,%d)",
						glyph, (int)cc.x, (int)cc.y);
		sym = ' ';
	    }
	}

	/*
	 * Check all the possibilities, saving all explanations in a buffer.
	 * When all have been checked then the string is printed.
	 */

	/* Check for monsters */
	for (i = 0; i < MAXMCLASSES; i++) {
	    if (sym == (from_screen ? monsyms[i] : def_monsyms[i]) &&
		monexplain[i]) {
		need_to_look = TRUE;
		if (!found) {
		    Sprintf(out_str, "%c       %s", sym, an(monexplain[i]));
		    firstmatch = monexplain[i];
		    found++;
		} else {
		    found += append_str(out_str, an(monexplain[i]));
		}
	    }
	}
	/* handle '@' as a special case if it refers to you and you're
	   playing a character which isn't normally displayed by that
	   symbol; firstmatch is assumed to already be set for '@' */
	if ((from_screen ?
		(sym == monsyms[S_HUMAN] && cc.x == u.ux && cc.y == u.uy) :
		(sym == def_monsyms[S_HUMAN] && !iflags.showrace)) &&
	    !(Race_if(PM_HUMAN) || Race_if(PM_ELF)) && !Upolyd)
	    found += append_str(out_str, "you");	/* tack on "or you" */

	/*
	 * Special case: if identifying from the screen, and we're swallowed,
	 * and looking at something other than our own symbol, then just say
	 * "the interior of a monster".
	 */
	if (u.uswallow && from_screen && is_swallow_sym(sym)) {
	    if (!found) {
		Sprintf(out_str, "%c       %s", sym, mon_interior);
		firstmatch = mon_interior;
	    } else {
		found += append_str(out_str, mon_interior);
	    }
	    need_to_look = TRUE;
	}

	/* Now check for objects */
	for (i = 1; i < MAXOCLASSES; i++) {
	    if (sym == (from_screen ? oc_syms[i] : def_oc_syms[i])) {
		need_to_look = TRUE;
		if (from_screen && i == VENOM_CLASS) {
		    skipped_venom++;
		    continue;
		}
		if (!found) {
		    Sprintf(out_str, "%c       %s", sym, an(objexplain[i]));
		    firstmatch = objexplain[i];
		    found++;
		} else {
		    found += append_str(out_str, an(objexplain[i]));
		}
	    }
	}

	if (sym == DEF_INVISIBLE) {
	    if (!found) {
		Sprintf(out_str, "%c       %s", sym, an(invisexplain));
		firstmatch = invisexplain;
		found++;
	    } else {
		found += append_str(out_str, an(invisexplain));
	    }
	}

#define is_cmap_trap(i) ((i) >= S_arrow_trap && (i) <= /*S_polymorph_trap*/S_magic_beam_trap)
#define is_cmap_drawbridge(i) ((i) >= S_vodbridge && (i) <= S_hcdbridge)

	/* Now check for graphics symbols */
	for (hit_trap = FALSE, i = 0; i < MAXPCHARS; i++) {
	    x_str = defsyms[i].explanation;
	    if (sym == (from_screen ? showsyms[i] : defsyms[i].sym) && *x_str) {
		/* avoid "an air", "a water", or "a floor of a room" */
		int article = ((i == S_room)||(i == S_darkroom)) ? 2 :		/* 2=>"the" */
			      !(strcmp(x_str, "air") == 0 ||	/* 1=>"an"  */
				strcmp(x_str, "shallow water") == 0 || /*0>(none)*/
				strcmp(x_str, "water") == 0);	/* 0=>(none)*/

		if (!found) {
		    if (is_cmap_trap(i)) {
			Sprintf(out_str, "%c       a trap", sym);
			hit_trap = TRUE;
		    } else if (level.flags.lethe && !strcmp(x_str, "water")) {
			Sprintf(out_str, "%c       sparkling water", sym);
		    } else {
				Sprintf(out_str,  "%C       %s", sym, 
				article == 2 ? the(x_str) :
				article == 1 ? an(x_str) : x_str);
		    }
		    firstmatch = x_str;
		    found++;
		} else if (!u.uswallow && !(hit_trap && is_cmap_trap(i)) &&
			   !(found >= 3 && is_cmap_drawbridge(i))) {
		    if (level.flags.lethe && !strcmp(x_str, "water"))
			found += append_str(out_str, "sparkling water");
		    else
		    	found += append_str(out_str,
					article == 2 ? the(x_str) :
					article == 1 ? an(x_str) : x_str);
		    if (is_cmap_trap(i)) hit_trap = TRUE;
		}

		if (i == S_altar || is_cmap_trap(i) || i == S_tree)
		    need_to_look = TRUE;
	    }
	}

	/* Now check for warning symbols */
	for (i = /*1*/0; i < WARNCOUNT; i++) { /* fixed annoying bug --Amy */
	    x_str = def_warnsyms[i].explanation;
	    if (sym == (from_screen ? warnsyms[i] : def_warnsyms[i].sym)) {
		if (!found) {
			Sprintf(out_str, "%c       %s",
				sym, def_warnsyms[i].explanation);
			firstmatch = def_warnsyms[i].explanation;
			found++;
		} else {
			found += append_str(out_str, def_warnsyms[i].explanation);
		}
		/* Kludge: warning trumps boulders on the display.
		   Reveal the boulder too or player can get confused */
		if (from_screen && sobj_at(BOULDER, cc.x, cc.y))
			Strcat(out_str, " co-located with a boulder");
		break;	/* out of for loop*/
	    }
	}
    
	/* if we ignored venom and list turned out to be short, put it back */
	if (skipped_venom && found < 2) {
	    x_str = objexplain[VENOM_CLASS];
	    if (!found) {
		Sprintf(out_str, "%c       %s", sym, an(x_str));
		firstmatch = x_str;
		found++;
	    } else {
		found += append_str(out_str, an(x_str));
	    }
	}

	/* handle optional boulder symbol as a special case */ 
	if (iflags.bouldersym && sym == iflags.bouldersym) {
	    if (!found) {
		firstmatch = "boulder";
		Sprintf(out_str, "%c       %s", sym, an(firstmatch));
		found++;
	    } else {
		found += append_str(out_str, "boulder");
	    }
	}
	
	/*
	 * If we are looking at the screen, follow multiple possibilities or
	 * an ambiguous explanation by something more detailed.
	 */
	if (from_screen) {
	    if (found > 1 || need_to_look) {
		char monbuf[BUFSZ];
		char temp_buf[BUFSZ];

		pm = lookat(cc.x, cc.y, look_buf, monbuf);
		firstmatch = look_buf;
		if (*firstmatch) {
		    Sprintf(temp_buf, " (%s)", firstmatch);
		    (void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		    found = 1;	/* we have something to look up */
		}
		if (monbuf[0]) {
		    Sprintf(temp_buf, " [seen: %s]", monbuf);
		    (void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		}
#ifdef WIZARD
		if (wizard && pm) {
		    struct monst *mtmp = m_at(cc.x, cc.y);
		    if (mtmp && mtmp->oldmonnm != monsndx(pm)) {
			Sprintf(temp_buf, " [polymorphed from a %s]",
				mons[mtmp->oldmonnm].mname);
			(void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		    }
		}
#endif
#ifdef EXTENDED_INFO
		if(pm != (struct permonst *) 0) {
			append_newline_to_pline_string(out_str);
			temp_buf[0]='\0';
			get_description_of_monster_type(pm, temp_buf);
			(void)strncat(out_str, temp_buf, BUFSZ-strlen(out_str)-1);
		}
#endif
	    }
	}

	/* Finally, print out our explanation. */
	if (found) {

	    pline("%s", out_str);
	/*winid datawin = create_nhwindow(NHW_MENU);
	    putstr(datawin, 0, out_str);

	display_nhwindow(datawin, TRUE);
	destroy_nhwindow(datawin);*/

	    /* check the data file for information about this thing */
	    if (found == 1 && ans != LOOK_QUICK && ans != LOOK_ONCE &&
			(ans == LOOK_VERBOSE || (flags.help && !quick))) {
		char temp_buf[BUFSZ];
		Strcpy(temp_buf, level.flags.lethe 
					&& !strcmp(firstmatch, "water")?
				"lethe" : firstmatch);
		checkfile(temp_buf, pm, FALSE, (boolean)(ans == LOOK_VERBOSE));
	    }
	} else {
	    pline("I've never heard of such things.");
	}

    } while (from_screen && !quick && ans != LOOK_ONCE);

    flags.verbose = save_verbose;
    return 0;
}
#ifdef EXTENDED_INFO
int
plined_length(char * buf)
{
	int i = 0;
	int current_line = 0;
	int current_word = 0;
	int total = 0;
	do {
		boolean not_a_whitespace = (buf[i] != ' ' && buf[i] != '\0');
		boolean space_after_space = (i>0 && buf[i-1] == ' ' && buf[i] == ' ');
		//otherwise - end a word
		if (not_a_whitespace) {
			current_word++;
		} else if (space_after_space) {
			total++;
			current_line++;
			if (current_line == CO - 1) {
				current_line = 0;
			}
		} else {
			int available_space = CO - current_line - 1;
			if (current_word == 0) {
				//end of string, previous character was space
			} else if (buf[i] == ' ' && current_word < available_space) {
				//space after word will fit into current line
				total += current_word + 1;
				current_line += current_word + 1;
			} else if (current_word <= available_space) {
				//word will fit into line without space
				total += current_word;
				current_line = 0; //new line
			} else if (buf[i] == ' ' && current_word < CO - 1 ) {
				//word will fit into next line with a space
				total += current_word + 1 + available_space;
				current_line = current_word + 1;
			} else if (current_word < CO) {
				//word will fit into next line without space
				total += current_word + available_space;
				current_line = 0;
			} else {
				//word will take entire next line and more of the next one. Or maybe even multiple lines.
				if (current_line == 0) {
					total += current_word;
				} else {
					total += current_word + available_space;
				}
				current_line = current_word % (CO-1);
				if (buf[i] == ' ' && current_line > 0) {
					total++;
					current_line = (current_line+1) % (CO-1);
				}
			}
			current_word = 0;
		}
		i++;
	} while (buf[i-1] != '\0');
	return total;
}

void
append_newline_to_pline_string(char * buf)
{
	int length = plined_length(buf);
	int line_length = CO - 1;
	int addditional_required = (line_length - (length % line_length)) % line_length;
	boolean trailing_space = FALSE;
	if (addditional_required == 0 && buf[strlen(buf)-1] != ' ') {
		trailing_space = TRUE;
	}
	#ifdef USE_TILES
		trailing_space = TRUE;
	#endif
	if(trailing_space) {
		strcat(buf, " ");
	}
	#ifdef USE_TILES
		/* there is limited amount of lines available for message and wide text field. Adding enters would make text hard to read */
		return;
	#endif
	while(addditional_required--) {
		strcat(buf, " ");
	}
}

int
append(char * buf, int condition, char * text, boolean many)
{
	if (condition) {
		if (buf != NULL) {
			if (many) {
				(void)strcat(buf, ", ");
			}
			(void)strcat(buf, text);
		}
		return many+1;
	}
	return many;
}

int
generate_list_of_resistances(char * temp_buf, /*uchar*/unsigned long flags)
{
	int many = 0;
	many = append(temp_buf, (flags & MR_FIRE), "fire", many);
	many = append(temp_buf, (flags & MR_COLD), "cold", many);
	many = append(temp_buf, (flags & MR_SLEEP), "sleep", many);
	many = append(temp_buf, (flags & MR_DISINT), "disintegration", many);
	many = append(temp_buf, (flags & MR_ELEC), "electricity", many);
	many = append(temp_buf, (flags & MR_POISON), "poison", many);
	many = append(temp_buf, (flags & MR_ACID), "acid", many);
	many = append(temp_buf, (flags & MR_STONE), "petrification", many);
	many = append(temp_buf, (flags & MR_DRAIN), "level drain", many);
	many = append(temp_buf, (flags & MR_DEATH), "death magic", many);
	many = append(temp_buf, (flags & MR_PLUSONE), "needs +1 weapon to hit", many);
	many = append(temp_buf, (flags & MR_PLUSTWO), "needs +2 weapon to hit", many);
	many = append(temp_buf, (flags & MR_PLUSTHREE), "needs +3 weapon to hit", many);
	many = append(temp_buf, (flags & MR_PLUSFOUR), "needs +4 weapon to hit", many);
	many = append(temp_buf, (flags & MR_HITASONE), "hits as a +1 weapon", many);
	many = append(temp_buf, (flags & MR_HITASTWO), "hits as a +2 weapon", many);
	many = append(temp_buf, (flags & MR_HITASTHREE), "hits as a +3 weapon", many);
	many = append(temp_buf, (flags & MR_HITASFOUR), "hits as a +4 weapon", many);
	return many;
}

char *
get_generation_description_of_monster_type(struct permonst * ptr, char * temp_buf)
{
	int many = 0;
	if ((ptr->geno & G_NOGEN) == 0) {
		strcat(temp_buf, "Normally appears ");
		many = append(temp_buf, ((ptr->geno & G_NOHELL) == 0) && ((ptr->geno & G_HELL) == 0), "everywhere", many);
		many = append(temp_buf, (ptr->geno & G_NOHELL), "outside Gehennom", many);
		many = append(temp_buf, (ptr->geno & G_HELL), "in Gehennom", many);
	} else {
		many = append(temp_buf, (ptr->geno & G_NOGEN), "Special generation", many);
	}
	many = append(temp_buf, (ptr->geno & G_UNIQ), "unique", many);
	many = 0;
	many = append(temp_buf, (ptr->geno & G_SGROUP), " in groups", many);
	many = append(temp_buf, (ptr->geno & G_LGROUP), " in large groups", many);
	many = append(temp_buf, (ptr->geno & G_VLGROUP), " in very large groups", many);
	if ((ptr->geno & G_NOGEN) == 0) {
		char frequency[BUFSZ] = "";
		Sprintf(frequency, ", with frequency %d.", (ptr->geno & G_FREQ));
		strcat(temp_buf, frequency);
	} else {
		strcat(temp_buf, ".");
	}
	return temp_buf;
}

char *
get_resistance_description_of_monster_type(struct permonst * ptr, char * description)
{
	char temp_buf[BUFSZ] = "";
	temp_buf[0] = '\0';
	int count = generate_list_of_resistances(temp_buf, ptr->mresists);
	if(count == 0) {
		strcat(description, " No resistances.");
	} else {
		strcat(description, " Resists ");
		strcat(description, temp_buf);
		strcat(description, ".");
	}

	temp_buf[0] = '\0';
	count = generate_list_of_resistances(temp_buf, ptr->mconveys);
	if ((ptr->geno & G_NOCORPSE) != 0) {
		strcat(description, " Leaves no corpse.");
	} else if (count == 0) {
		strcat(description, " No conveyed resistances.");
	} else {
		strcat(description, " Conveys ");
		strcat(description, temp_buf);
		if (count == 1) {
			strcat(description, " resistance.");
		} else {
			strcat(description, " resistances.");
		}
	}

	return description;
}

char *
get_flag_description_of_monster_type(struct permonst * ptr, char * description)
{
	char temp_buf[BUFSZ] = "";
	char size[BUFSZ] = "";
	char adjectives[BUFSZ] = "";
	char special_adjectives[BUFSZ] = "";
	char noun[BUFSZ] = "";

	if (verysmall(ptr)) {
		strcat(size, "small");
	} else if (bigmonst(ptr)) {
		strcat(size, "big");
	}

	int adjective_counter = 0;
	adjective_counter = append(adjectives, (ptr->geno & G_GENO), "genocidable", adjective_counter);
	adjective_counter = append(adjectives, !(ptr->geno & G_GENO), "not genocidable", adjective_counter);
	adjective_counter = append(adjectives, (breathless(ptr)), "breathless", adjective_counter);
	adjective_counter = append(adjectives, (amphibious(ptr)), "amphibious", adjective_counter);
	adjective_counter = append(adjectives, (passes_walls(ptr)), "phasing", adjective_counter);
	adjective_counter = append(adjectives, (amorphous(ptr)), "amorphous", adjective_counter);
	adjective_counter = append(adjectives, (noncorporeal(ptr)), "noncorporeal", adjective_counter);
	adjective_counter = append(adjectives, (unsolid(ptr)), "unsolid", adjective_counter);
	adjective_counter = append(adjectives, (acidic(ptr)), "acidic", adjective_counter);
	adjective_counter = append(adjectives, (carnivorous(ptr)), "carnivorous", adjective_counter);
	adjective_counter = append(adjectives, (herbivorous(ptr)), "herbivorous", adjective_counter);
	adjective_counter = append(adjectives, (metallivorous(ptr)), "metallivorous", adjective_counter);
	adjective_counter = append(adjectives, (lithivorous(ptr)), "lithivorous", adjective_counter);
	adjective_counter = append(adjectives, (poisonous(ptr)), "poisonous", adjective_counter);
	adjective_counter = append(adjectives, (regenerates(ptr)), "regenerating", adjective_counter);
	adjective_counter = append(adjectives, (can_teleport(ptr)), "teleporting", adjective_counter);
	adjective_counter = append(adjectives, (is_reviver(ptr)), "reviving", adjective_counter);
	adjective_counter = append(adjectives, (pm_invisible(ptr)), "invisible", adjective_counter);
	adjective_counter = append(adjectives, (thick_skinned(ptr)), "thick-skinned", adjective_counter);
	adjective_counter = append(adjectives, (nonliving(ptr) && !is_undead(ptr)), "nonliving", adjective_counter);

	append(special_adjectives, (is_undead(ptr)), "undead", 0);

	int many = 0;
	many = append(noun, (is_hider(ptr)), "hider", many);
	many = append(noun, (is_swimmer(ptr)), "swimmer", many);
	many = append(noun, (is_flyer(ptr)), "flyer", many);
	many = append(noun, (is_floater(ptr)), "floater", many);
	many = append(noun, (is_clinger(ptr)), "clinger", many);
	if (tunnels(ptr)) {
		if (needspick(ptr)) {
			many = append(noun, TRUE, "miner", many);
		} else {
			many = append(noun, TRUE, "digger", many);
		}
	}
	
	// <size><adjectives><special_adjectives><noun>
	if (strlen(size) > 0) {
		if (adjective_counter <= 1 && (strlen(special_adjectives) > 0 || strlen(noun) > 0)) {
			/* huge undead */
			/* small noncorporeal miner */
			strcat(temp_buf, size);
			strcat(temp_buf, " ");
		} else if (adjective_counter >= 1) {
			/* small, genocideable, amphibious swimmer */
			/* big, poisonous, invisible miner */
			/* big, poisonous, invisible hider, swimmer, flyer*/
			/* huge */
			/* small, noncorporeal */
			/* big, poisonous, invisible */
			/* big, poisonous, invisible*/
			strcat(temp_buf, size);
			strcat(temp_buf, ", ");
		} else if (adjective_counter == 0){
			/* small swimmer */
			/* big miner */
			/* big swimmer, flyer*/
			/* huge */
			/* small undead digger */
			strcat(temp_buf, size);
			strcat(temp_buf, " ");
		} else {
			impossible("impossible happened in get_flag_description_of_monster_type");
		}
	}
	if(strlen(adjectives) > 0) {
		strcat(temp_buf, adjectives);
		strcat(temp_buf, " ");
	}
	if(strlen(special_adjectives) > 0) {
		strcat(temp_buf, special_adjectives);
		strcat(temp_buf, " ");
	}
	if(strlen(noun) > 0) {
		strcat(temp_buf, noun);
		strcat(temp_buf, " ");
	}
	if (strlen(temp_buf) > 0) {
		upstart(temp_buf);
		temp_buf[strlen(temp_buf)-1] = '.'; //replaces last space
		strcat(description, " ");
		strcat(description, temp_buf);
	}
	if (perceives(ptr)) {
		strcat(description, " See invisible.");
	}
	if (control_teleport(ptr)) {
		strcat(description, " Controls teleport.");
	}
	if (your_race(ptr)) {
		strcat(description, " Is the same race as you.");
	}
	if (touch_petrifies(ptr)) {
		strcat(description, " It can turn you to stone if you touch it.");
	}
	if (is_mind_flayer(ptr)) {
		strcat(description, " Is a mind flayer.");
	}
	if (is_giant(ptr)) {
		strcat(description, " Is a giant.");
	}
	if (hates_silver(ptr)) {
		strcat(description, " Silver weapons do extra damage to it.");
	}
	if (vegan(ptr)) {
		strcat(description, " May be eaten by vegans.");
	} else if (vegetarian(ptr)) {
		strcat(description, " May be eaten by vegetarians.");
	}
	/*
	Unfortunately keepdogs function is quite mysterious
		- Cthulhu and Orcus never follow (M2_STALK and STRAT_WAITFORU flag)
		- Vlad follows (M2_STALK and STRAT_WAITFORU flag)*/
	if (ptr->mflags2 & M2_STALK) {
		strcat(description, " Follows you to other levels.");
	}

	if (is_covetous(ptr)) {
		strcat(description, " Covetous.");
	}
	if (infravision(ptr)) {
		strcat(description, " Has infravision.");
	}
	if (infravisible(ptr)) {
		strcat(description, " Can be seen with infravision.");
	}
	if (throws_rocks(ptr)) {
		strcat(description, " Can pick up and throw rocks.");
	}
	if (can_betray(ptr)) {
		strcat(description, " Can spontaneously betray you if tame.");
	}
	if (cannot_be_tamed(ptr)) {
		strcat(description, " Can't be tamed.");
	}
	if (avoid_player(ptr)) {
		strcat(description, " Tries to avoid moving right next to you.");
	}
	if (is_petty(ptr)) {
		strcat(description, " Can be tamed by throwing kelp frond or a poke ball.");
	}
	if (is_pokemon(ptr)) {
		strcat(description, " Is a pokemon (more likely to be caught in a poke ball).");
	}
	if (is_mplayer(ptr)) {
		strcat(description, " Is a playable role and thus likely to be well-armed.");
	}
	
	if (polyok(ptr)) {
		strcat(description, " Is a valid polymorph form.");
	} else if (monpolyok(ptr)) {
		strcat(description, " Is a valid polymorph form for monsters only.");
	} else {
		strcat(description, " Is not a valid polymorph form.");
	}
	return description;
}

char *
get_speed_description_of_monster_type(struct permonst * ptr, char * description)
{
	if(ptr->mmove > 35) {
		sprintf(description, "Extremely fast (%d). ", ptr->mmove);
	} else if(ptr->mmove > 19) {
		sprintf(description, "Very fast (%d). ", ptr->mmove);
	} else if(ptr->mmove > 12) {
		sprintf(description, "Fast (%d). ", ptr->mmove);
	} else if(ptr->mmove == 12) {
		sprintf(description, "Normal speed (%d). ", ptr->mmove);
	} else if(ptr->mmove > 8) {
		sprintf(description, "Slow (%d). ", ptr->mmove);
	} else if(ptr->mmove > 3) {
		sprintf(description, "Very slow (%d). ", ptr->mmove);
	} else if(ptr->mmove > 0) {
		sprintf(description, "Almost immobile (%d). ", ptr->mmove);
	} else {
		sprintf(description, "Sessile (%d). ", ptr->mmove);
	}
	return description;
}

char *
get_description_of_attack_type(uchar id)
{
	switch(id){
		case AT_NONE: return "passive";
		case AT_CLAW: return "claw (punch, hit, etc.)";
		case AT_BITE: return "bite";
		case AT_KICK: return "kick";
		case AT_BUTT: return "head butt";
		case AT_TUCH: return "touches";
		case AT_STNG: return "sting";
		case AT_HUGS: return "crushing bearhug";
		case AT_SPIT: return "spits substance";
		case AT_ENGL: return "engulf";
		case AT_BREA: return "breath";
		case AT_EXPL: return "explodes - proximity";
		case AT_BOOM: return "explodes when killed";
		case AT_GAZE: return "gaze";
		case AT_TENT: return "tentacles";
		case AT_SCRA: return "scratch";
		case AT_LASH: return "lash";
		case AT_TRAM: return "trample";
		/* AT_NTCH isn't entirely accurate but works good
		 * enough for now
		 */
		case AT_NTCH: return "plays music";
		case AT_TALK: return "talks";
#if 0 /* Deferred */
		case AT_RIDL: return "asks a riddle";
#endif
		case AT_WEAP: return "uses weapon";
		case AT_MAGC: return "uses magic spell(s)";
		case AT_MULTIPLY: return "multiplies";
		default: impossible("bug in get_description_of_attack_type(%d)", id); return "<MISSING DECRIPTION, THIS IS A BUG>";
	}
}

char *
get_description_of_damage_type(uchar id)
{
	switch(id){
		case AD_PHYS: return "ordinary physical";
		case AD_MAGM: return "magic missiles";
		case AD_FIRE: return "fire damage";
		case AD_COLD: return "frost damage";
		case AD_SLEE: return "sleep ray";
		case AD_DISN: return "disintegration";
		case AD_ELEC: return "shock damage";
		case AD_DRST: return "drains strength";
		case AD_ACID: return "acid damage";
		case AD_SPC2: return "for extension of buzz()";
		case AD_BLND: return "blinds";
		case AD_STUN: return "stuns";
		case AD_SLOW: return "slows";
		case AD_PLYS: return "paralyses";
		case AD_DRLI: return "drains life levels";
		case AD_DREN: return "drains magic energy";
		case AD_LEGS: return "damages legs";
		case AD_STON: return "petrifies";
		case AD_STCK: return "sticks to you ";
		case AD_SGLD: return "steals gold";
		case AD_SITM: return "steals item";
		case AD_SEDU: return "seduces and steals multiple items";
		case AD_TLPT: return "teleports you";
		case AD_RUST: return "rusts armour";
		case AD_CONF: return "confuses";
		case AD_DGST: return "digests opponent";
		case AD_HEAL: return "heals opponent's wounds";
		case AD_WRAP: return "wraps around";
		case AD_WERE: return "confers lycanthropy";
		case AD_DRDX: return "drains dexterity";
		case AD_DRCO: return "drains constitution";
		case AD_DRIN: return "drains intelligence";
		case AD_DISE: return "confers diseases";
		case AD_DCAY: return "decays organics";
		case AD_SSEX: return "foocubus seduction";
		case AD_HALU: return "causes hallucination";
		case AD_DETH: return "unique Death attack";
		case AD_PEST: return "unique Pestilence attack";
		case AD_FAMN: return "unique Famine attack";
		case AD_SLIM: return "turns you into green slime";
		case AD_ENCH: return "remove enchantment";
		case AD_CORR: return "corrode armor";
		case AD_CLRC: return "random clerical spell";
		case AD_SPEL: return "random magic spell";
		case AD_RBRE: return "random breath weapon";
		case AD_SAMU: return "hits, may steal Amulet";
		case AD_CURS: return "random curse";
		case AD_LITE: return "light";
		case AD_CALM: return "calms";
		case AD_POLY: return "polymorphs you";
		case AD_TCKL: return "tickles you";
		case AD_NGRA: return "removes engravings";
		case AD_GLIB: return "disarms you";
		case AD_DARK: return "causes darkness around you";
		case AD_WTHR: return "withers items";
		case AD_LUCK: return "drains luck";
		case AD_DEPR: return "causes depression";
		case AD_CHRM: return "charms you or other monster";
		case AD_SHOE: return "damages, steals or fixes shoes";
		case AD_SCLD: return "scalds you (similar to AD_FIRE)";
		case AD_EGLD: return "eats gold objects";
		case AD_FLVR: return "random quark effects";
		case AD_HNGY: return "causes hunger (similar to AD_FAMN)";
		case AD_ENDS: return "placeholder attack";
		default: impossible("bug in get_description_of_damage_type(%d)", id); return "<MISSING DESCRIPTION, THIS IS A BUG>";
	}
}

char *
get_description_of_attack(struct attack *mattk, char * main_temp_buf)
{
	if(!(mattk->damn + mattk->damd + mattk->aatyp + mattk->adtyp)) {
		main_temp_buf[0]='\0';
		return main_temp_buf;
	}
	
	char temp_buf[BUFSZ] = "";
	if(mattk->damn + mattk->damd) {
		sprintf(main_temp_buf, "%dd%d", mattk->damn, mattk->damd);
		#ifndef USE_TILES
			strcat(main_temp_buf, ",");
		#endif
		strcat(main_temp_buf, " ");
	} else {
		main_temp_buf[0] = '\0';
	}
	#ifndef USE_TILES
		while (strlen(main_temp_buf) < 6) {
			strcat(main_temp_buf, " ");
		}
	#endif
	sprintf(temp_buf, "%s - %s", get_description_of_attack_type(mattk->aatyp), get_description_of_damage_type(mattk->adtyp));
	strcat(main_temp_buf, temp_buf);
	#ifdef USE_TILES
		strcat(main_temp_buf, "; ");
	#endif
	return main_temp_buf;
}

char *
get_description_of_monster_type(struct permonst * ptr, char * description)
{
/*
	pline("%d<><><>", plined_length("12345678901234567890123456789012345678901234567890123456789012345678901234567890"));//0 passed
	pline("%d<><><>", plined_length("1234567890123456789012345678901234567890123456789012345678901234567890123456789"));
*/
	char temp_buf[BUFSZ] = "";
	char main_temp_buf[BUFSZ] = "";

	temp_buf[0]='\0';
	sprintf(temp_buf, "Accessing %s entry for %s... ", Hallucination ? "Pokedex" : "Datalog", ptr->mname);
	strcat(description, temp_buf);
	append_newline_to_pline_string(description);
	strcat(description, " ");
	append_newline_to_pline_string(description);
	strcat(description, "Base statistics of this monster type:");
	append_newline_to_pline_string(description);

	sprintf(temp_buf, "Base level = %d, AC = %d, magic resistance = %d. ", ptr->mlevel, ptr->ac, ptr->mr);
	strcat(description, temp_buf);
	temp_buf[0]='\0';
	strcat(description, get_speed_description_of_monster_type(ptr, temp_buf));

	temp_buf[0]='\0';
	strcat(description, get_generation_description_of_monster_type(ptr, temp_buf));
	temp_buf[0]='\0';
	strcat(description, get_resistance_description_of_monster_type(ptr, temp_buf));
	temp_buf[0]='\0';
	strcat(description, get_flag_description_of_monster_type(ptr, temp_buf));

	append_newline_to_pline_string(description);
	strcat(description, "Attacks:");
	append_newline_to_pline_string(description);
	struct attack *mattk;
	struct attack alt_attk;
	int sum[NATTK];
	int i;
	for(i = 0; i < NATTK; i++) {
		sum[i] = 0;
		mattk = getmattk(ptr, i, sum, &alt_attk);
		main_temp_buf[0]='\0';
		get_description_of_attack(mattk, temp_buf);
		if(temp_buf[0] == '\0') {
			if (i == 0) {
				#ifndef USE_TILES
					strcat(description, "    ");
				#endif
				strcat(description, "none");
				append_newline_to_pline_string(description);
			}
			break;
		}
		#ifndef USE_TILES
			strcat(main_temp_buf, "    ");
		#endif
		strcat(main_temp_buf, temp_buf);
		append_newline_to_pline_string(main_temp_buf);
		strcat(description, main_temp_buf);
	}
	return description;
}
#endif

int
dowhatis()
{
	return do_look(FALSE);
}

int
doquickwhatis()
{
	return do_look(TRUE);
}

int
doidtrap()
{
	register struct trap *trap;
	int x, y, tt;

	if (!getdir("^")) return 0;
	x = u.ux + u.dx;
	y = u.uy + u.dy;
	for (trap = ftrap; trap; trap = trap->ntrap)
	    if (trap->tx == x && trap->ty == y) {
		if (!trap->tseen) break;
		tt = trap->ttyp;
		if (u.dz) {
		    if (u.dz < 0 ? (tt == TRAPDOOR || tt == HOLE) :
			    tt == ROCKTRAP) break;
		}
		tt = what_trap(tt);
		pline("That is %s%s%s.",
		      an(defsyms[trap_to_defsym(tt)].explanation),
		      !trap->madeby_u ? "" : (tt == WEB) ? " woven" :
			  /* trap doors & spiked pits can't be made by
			     player, and should be considered at least
			     as much "set" as "dug" anyway */
			  (tt == HOLE || tt == PIT) ? " dug" : " set",
		      !trap->madeby_u ? "" : " by you");
		return 0;
	    }
	pline("I can't see a trap there.");
	return 0;
}

char *
dowhatdoes_core(q, cbuf)
char q;
char *cbuf;
{
	dlb *fp;
	char bufr[BUFSZ];
	register char *buf = &bufr[6], *ep, ctrl, meta;

	fp = dlb_fopen_area(NH_CMDHELPAREA, NH_CMDHELPFILE, "r");
	if (!fp) {
		pline("Cannot open data file!");
		return 0;
	}

  	ctrl = ((q <= '\033') ? (q - 1 + 'A') : 0);
	meta = ((0x80 & q) ? (0x7f & q) : 0);
	while(dlb_fgets(buf,BUFSZ-6,fp)) {
	    if ((ctrl && *buf=='^' && *(buf+1)==ctrl) ||
		(meta && *buf=='M' && *(buf+1)=='-' && *(buf+2)==meta) ||
		*buf==q) {
		if ((ep = index(buf, '\n')) != 0) *ep = 0;
#ifdef MSDOS
		if ((ep = index(buf, '\r')) != 0) *ep = 0;
#endif
		if (ctrl && buf[2] == '\t'){
			buf = bufr + 1;
			(void) strncpy(buf, "^?      ", 8);
			buf[1] = ctrl;
		} else if (meta && buf[3] == '\t'){
			buf = bufr + 2;
			(void) strncpy(buf, "M-?     ", 8);
			buf[2] = meta;
		} else if(buf[1] == '\t'){
			buf = bufr;
			buf[0] = q;
			(void) strncpy(buf+1, "       ", 7);
		}
		(void) dlb_fclose(fp);
		Strcpy(cbuf, buf);
		return cbuf;
	    }
	}
	(void) dlb_fclose(fp);
	return (char *)0;
}

int
dowhatdoes()
{
	char bufr[BUFSZ];
	char q, *reslt;

#if defined(UNIX) || defined(VMS)
	introff();
#endif
	q = yn_function("What command?", (char *)0, '\0');
#if defined(UNIX) || defined(VMS)
	intron();
#endif
	reslt = dowhatdoes_core(q, bufr);
	if (reslt)
		pline("%s", reslt);
	else
		pline("I've never heard of such commands.");
	return 0;
}

/* data for help_menu() */
static const char *help_menu_items[] = {
/* 0*/	"Long description of the game and commands.",
/* 1*/	"List of game commands.",
/* 2*/	"Concise history of Slash'EM.",
/* 3*/	"Info on a character in the game display.",
/* 4*/	"Info on what a given key does.",
/* 5*/	"List of game options.",
/* 6*/	"Longer explanation of game options.",
/* 7*/	"List of extended commands.",
/* 8*/	"The NetHack license.",
#ifndef MAC
/*WAC Add access to txt guidebook*/
/* 9*/  "The Slash'EM Guidebook.",
#endif
#ifdef PORT_HELP
	"%s-specific help and commands.",
#endif
#ifdef WIZARD
	"List of wizard-mode commands.",
#endif
	"",
	(char *)0
};

enum {
  LICENSE_SLOT=8,
#ifndef MAC
  GUIDEBOOK_SLOT,
#endif
#ifdef PORT_HELP
  PORT_HELP_ID,
#endif
#ifdef WIZARD
  WIZHLP_SLOT,
#endif
  NULL_SLOT
};

STATIC_OVL boolean
help_menu(sel)
	int *sel;
{
	winid tmpwin = create_nhwindow(NHW_MENU);
#ifdef PORT_HELP
	char helpbuf[QBUFSZ];
#endif
	int i, n;
	menu_item *selected;
	anything any;

	any.a_void = 0;		/* zero all bits */
	start_menu(tmpwin);
#ifdef WIZARD
	if (!wizard) help_menu_items[WIZHLP_SLOT] = "",
		     help_menu_items[WIZHLP_SLOT+1] = (char *)0;
#endif
	for (i = 0; help_menu_items[i]; i++)
#ifdef PORT_HELP
	    /* port-specific line has a %s in it for the PORT_ID */
	    if (help_menu_items[i][0] == '%') {
		Sprintf(helpbuf, help_menu_items[i], PORT_ID);
		any.a_int = PORT_HELP_ID + 1;
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_NONE,
			 helpbuf, MENU_UNSELECTED);
	    } else
#endif
	    {
		any.a_int = (*help_menu_items[i]) ? i+1 : 0;
		add_menu(tmpwin, NO_GLYPH, &any, 0, 0,
			ATR_NONE, help_menu_items[i], MENU_UNSELECTED);
	    }
	end_menu(tmpwin, "Select one item:");
	n = select_menu(tmpwin, PICK_ONE, &selected);
	destroy_nhwindow(tmpwin);
	if (n > 0) {
	    *sel = selected[0].item.a_int - 1;
	    free((genericptr_t)selected);
	    return TRUE;
	}
	return FALSE;
}

int
dohelp()
{
	int sel = 0;

	if (help_menu(&sel)) {
		switch (sel) {
			case  0:  display_file_area(NH_HELP_AREA, NH_HELP, TRUE);
				  break;
			case  1:  display_file_area(NH_SHELP_AREA, NH_SHELP, TRUE);
				  break;
			case  2:  (void) dohistory();  break;
			case  3:  (void) dowhatis();  break;
			case  4:  (void) dowhatdoes();  break;
			case  5:  option_help();  break;
			case  6:  display_file_area(NH_OPTIONAREA,
				    NH_OPTIONFILE, TRUE);
				  break;
			case  7:  (void) doextlist();  break;
			case  8:  display_file_area(NH_LICENSE_AREA,
				    NH_LICENSE, TRUE);
				  break;
#ifndef MAC
/*WAC add guidebook.*/
                        case  GUIDEBOOK_SLOT:  display_file_area(NH_GUIDEBOOK_AREA,
				    NH_GUIDEBOOK, TRUE);
				  break;
#endif
#ifdef PORT_HELP
			case PORT_HELP_ID:  port_help();  break;
#endif
#ifdef WIZARD
                        case  WIZHLP_SLOT:  display_file_area(NH_DEBUGHELP_AREA,
				    NH_DEBUGHELP, TRUE);
				  break;
#endif
		}
	}
	return 0;
}

int
dohistory()
{
	display_file_area(NH_HISTORY_AREA, NH_HISTORY, TRUE);
	return 0;
}

/*pager.c*/
