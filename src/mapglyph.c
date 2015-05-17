/*	SCCS Id: @(#)mapglyph.c	3.4	2003/01/08	*/
/* Copyright (c) David Cohrs, 1991				  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#if defined(TTY_GRAPHICS)
#include "wintty.h"	/* for prototype of has_color() only */
#endif
#include "color.h"
#define HI_DOMESTIC CLR_WHITE	/* monst.c */

int explcolors[] = {
	CLR_BLACK,	/* dark    */
	CLR_GREEN,	/* noxious */
	CLR_BROWN,	/* muddy   */
	CLR_BLUE,	/* wet     */
	CLR_MAGENTA,	/* magical */
	CLR_ORANGE,	/* fiery   */
	CLR_WHITE,	/* frosty  */
};

#if !defined(TTY_GRAPHICS)
#define has_color(n)  TRUE
#endif

#ifdef TEXTCOLOR
#define zap_color(n)  color = iflags.use_color ? (Hallucination ? rn2(15) : zapcolors[n]) : NO_COLOR
#ifndef USER_DUNGEONCOLOR
#define cmap_color(n) color = iflags.use_color ? (Hallucination ? rn2(15) : defsyms[n].color) : NO_COLOR
#else
#define cmap_color(n) color = iflags.use_color ? showsymcolors[n] : NO_COLOR
#endif
#define obj_color(n)  color = iflags.use_color ? (Hallucination ? rn2(15) : objects[n].oc_color) : NO_COLOR
#define mon_color(n)  color = iflags.use_color ? (Hallucination ? rn2(15) : mons[n].mcolor) : NO_COLOR
#define invis_color(n) color = NO_COLOR
#define pet_color(n)  color = iflags.use_color ? (Hallucination ? rn2(15) : mons[n].mcolor) : NO_COLOR
#define warn_color(n) color = iflags.use_color ? (Hallucination ? rn2(15) : def_warnsyms[n].color) : NO_COLOR
#define explode_color(n) color = iflags.use_color ? (Hallucination ? rn2(15) : explcolors[n]) : NO_COLOR
# if defined(REINCARNATION) && defined(ASCIIGRAPH)
#  define ROGUE_COLOR
# endif

#else	/* no text color */

#define zap_color(n)
#define cmap_color(n)
#define obj_color(n)
#define mon_color(n)
#define invis_color(n)
#define pet_color(c)
#define warn_color(n)
#define explode_color(n)
#endif

#ifdef ROGUE_COLOR
# if defined(USE_TILES) && defined(MSDOS)
#define HAS_ROGUE_IBM_GRAPHICS (iflags.IBMgraphics && !iflags.grmode && \
	Is_rogue_level(&u.uz))
# else
#define HAS_ROGUE_IBM_GRAPHICS (iflags.IBMgraphics && Is_rogue_level(&u.uz))
# endif
#endif

/*ARGSUSED*/
void
mapglyph(glyph, ochar, ocolor, ospecial, x, y)
int glyph, *ocolor, x, y;
int *ochar;
unsigned *ospecial;
{
	register int offset;
#if defined(TEXTCOLOR) || defined(ROGUE_COLOR)
	int color = NO_COLOR;
#endif
	uchar ch;
	unsigned special = 0;

    /*
     *  Map the glyph back to a character and color.
     *
     *  Warning:  For speed, this makes an assumption on the order of
     *		  offsets.  The order is set in display.h.
     */
    if ((offset = (glyph - GLYPH_WARNING_OFF)) >= 0) {	/* a warning flash */
    	ch = warnsyms[offset];
# ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;
	else
# endif
	    warn_color(offset);
    } else if ((offset = (glyph - GLYPH_SWALLOW_OFF)) >= 0) {	/* swallow */
	/* see swallow_to_glyph() in display.c */
	ch = (uchar) showsyms[S_sw_tl + (offset & 0x7)];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = NO_COLOR;
	else
#endif
	    mon_color(offset >> 3);
    } else if ((offset = (glyph - GLYPH_ZAP_OFF)) >= 0) {	/* zap beam */
	/* see zapdir_to_glyph() in display.c */
	ch = showsyms[S_vbeam + (offset & 0x3)];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = NO_COLOR;
	else
#endif
	    zap_color((offset >> 2));
    } else if ((offset = (glyph - GLYPH_EXPLODE_OFF)) >= 0) {	/* explosion */
	ch = showsyms[(offset % MAXEXPCHARS) + S_explode1];
	explode_color(offset / MAXEXPCHARS);
    } else if ((offset = (glyph - GLYPH_CMAP_OFF)) >= 0) {	/* cmap */
	ch = showsyms[offset];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color) {
	    if (offset >= S_vwall && offset <= S_hcdoor)
		color = CLR_BROWN;
	    else if (offset >= S_arrow_trap && offset <= S_pile_of_shit)
		color = CLR_MAGENTA;
	    else if (offset == S_corr || offset == S_litcorr)
		color = CLR_GRAY;
	    else if (offset >= S_room && offset <= S_water && offset != S_darkroom)
		color = CLR_GREEN;
	    else
		color = NO_COLOR;
	} else
#endif
#ifdef TEXTCOLOR
	    /* provide a visible difference if normal and lit corridor
	     * use the same symbol */
	    if (iflags.use_color &&
#ifndef USER_DUNGEONCOLOR
		offset == S_litcorr && ch == showsyms[S_corr])
		color = CLR_WHITE;
#else
		offset == S_litcorr && ch == showsyms[S_corr] &&
		    showsymcolors[S_corr] == showsymcolors[S_litcorr]) {
	if (showsymcolors[S_corr] != CLR_WHITE) {
	    color = showsymcolors[S_litcorr] = CLR_WHITE;
	} else {
	    color = showsymcolors[S_litcorr] = CLR_GRAY;
	}
    }
#endif /* USER_DUNGEONCOLOR */
		/* special level colors by Amy, code partly stolen from dnethack */
		/* 5lo: Let's rewrite this to be much, much shorter...*/
		if(iflags.use_color && iflags.dungeon_colors &&
		    (offset >= S_vwall && offset <= S_hcdoor)){
			if(Is_lawful_quest(&u.uz)) {
				color = CLR_BLACK;
			} else if(Is_nymph_level(&u.uz)) {
				color = CLR_GREEN;
				if(offset >= S_corr && offset <= S_litcorr){
				    color = CLR_BRIGHT_GREEN;
				}
			} else if(Is_forge_level(&u.uz)) {
				color = CLR_ORANGE;
			} else if(Is_hitch_level(&u.uz)) {
				color = CLR_BRIGHT_CYAN;
			} else if(Is_compu_level(&u.uz)) {
				color = CLR_YELLOW;
			} else if(Is_key_level(&u.uz)) {
				color = CLR_MAGENTA;
			} else if(Is_mtemple_level(&u.uz)) {
				color = CLR_ORANGE;
			} else if(Is_stronghold(&u.uz)) {
				color = CLR_RED;
			} else if(In_hell(&u.uz) && !Is_valley(&u.uz)) {
				color = CLR_RED;
			} else if(Is_valley(&u.uz)) {
				color = CLR_BLACK;
			} else if(In_sheol(&u.uz)) {
				color = CLR_BRIGHT_CYAN;
			} else if(In_mines(&u.uz) && !*in_rooms(x,y,0)) {
				color = CLR_BROWN;
			} else if(*in_rooms(x,y,BEEHIVE)) {
				color = CLR_YELLOW;
			} else if(*in_rooms(x,y,COURT)) {
				color = CLR_MAGENTA;
			} else if(*in_rooms(x,y,VAULT)) {
				color = level.flags.vault_is_aquarium ?
				 CLR_BLUE : CLR_YELLOW;
			} else if(*in_rooms(x,y,DELPHI)) {
				color = CLR_BRIGHT_BLUE;
			} else if(*in_rooms(x,y,TEMPLE)) {
				color = CLR_CYAN;
			} else if(*in_rooms(x,y,LEPREHALL)) {
				color = CLR_GREEN;
			} else if(*in_rooms(x,y,SWAMP)) {
				color = CLR_GREEN;
			} else if(*in_rooms(x,y,MORGUE)) {
				color = CLR_BLACK;
			} else if(*in_rooms(x,y,BARRACKS)) {
				color = CLR_RED;
			} else if(*in_rooms(x,y,ZOO)) {
				color = CLR_BROWN;
			} else if(*in_rooms(x,y,REALZOO)) {
				color = CLR_BROWN;
			} else if(*in_rooms(x,y,GIANTCOURT)) {
				color = CLR_RED;
			} else if(*in_rooms(x,y,DRAGONLAIR)) {
				color = CLR_BRIGHT_MAGENTA;
			} else if(*in_rooms(x,y,BADFOODSHOP)) {
				color = CLR_RED;
			} else if(*in_rooms(x,y,COCKNEST)) {
				color = CLR_YELLOW;
			} else if(*in_rooms(x,y,ANTHOLE)) {
				color = CLR_BROWN;
			} else if(*in_rooms(x,y,LEMUREPIT)) {
				color = CLR_BLACK;
			} else if(*in_rooms(x,y,MIGOHIVE)) {
				color = CLR_BRIGHT_GREEN;
			} else if(*in_rooms(x,y,FUNGUSFARM)) {
				color = CLR_BRIGHT_GREEN;
			} else if(*in_rooms(x,y,CLINIC)) {
				color = CLR_ORANGE;
			} else if(*in_rooms(x,y,TERRORHALL)) {
				color = CLR_BRIGHT_CYAN;
			} else if(*in_rooms(x,y,ELEMHALL)) {
				color = CLR_GRAY;
			} else if(*in_rooms(x,y,ANGELHALL)) {
				color = CLR_WHITE;
			} else if(*in_rooms(x,y,NYMPHHALL)) {
				color = CLR_GREEN;
			} else if(*in_rooms(x,y,SPIDERHALL)) {
				color = CLR_GRAY;
			} else if(*in_rooms(x,y,TROLLHALL)) {
				color = CLR_BROWN;
			} else if(*in_rooms(x,y,HUMANHALL)) {
				color = CLR_BRIGHT_BLUE;
			} else if(*in_rooms(x,y,GOLEMHALL)) {
				color = CLR_GRAY;
			} else if(*in_rooms(x,y,COINHALL)) {
				color = CLR_YELLOW;
			} else if(*in_rooms(x,y,DOUGROOM)) {
				color = CLR_BRIGHT_CYAN;
			} else if(*in_rooms(x,y,ARMORY)) {
				color = CLR_CYAN;
			} else if(*in_rooms(x,y,TENSHALL)) {
				color = rn2(15);
			} else if(*in_rooms(x,y,POOLROOM)) {
				color = CLR_BRIGHT_BLUE;
			}
		}
		/* And turned this into it's own option, as some people don't like this by default */
		if(iflags.use_color && iflags.random_room_colors&&
		    (offset >= S_vwall && offset <= S_hcdoor)){
			if(*in_roomscolouur(x,y,1) && *in_rooms(x,y,OROOM)) {
				color = 1;
			} else if(*in_roomscolouur(x,y,2) && *in_rooms(x,y,OROOM)) {
				color = 2;
			} else if(*in_roomscolouur(x,y,3) && *in_rooms(x,y,OROOM)) {
				color = 3;
			} else if(*in_roomscolouur(x,y,4) && *in_rooms(x,y,OROOM)) {
				color = 4;
			} else if(*in_roomscolouur(x,y,5) && *in_rooms(x,y,OROOM)) {
				color = 5;
			} else if(*in_roomscolouur(x,y,6) && *in_rooms(x,y,OROOM)) {
				color = 6;
			} else if(*in_roomscolouur(x,y,7) && *in_rooms(x,y,OROOM)) {
				color = 7;
			} else if(*in_roomscolouur(x,y,8) && *in_rooms(x,y,OROOM)) {
				color = 8;
			} else if(*in_roomscolouur(x,y,9) && *in_rooms(x,y,OROOM)) {
				color = 9;
			} else if(*in_roomscolouur(x,y,10) && *in_rooms(x,y,OROOM)) {
				color = 10;
			} else if(*in_roomscolouur(x,y,11) && *in_rooms(x,y,OROOM)) {
				color = 11;
			} else if(*in_roomscolouur(x,y,12) && *in_rooms(x,y,OROOM)) {
				color = 12;
			} else if(*in_roomscolouur(x,y,13) && *in_rooms(x,y,OROOM)) {
				color = 13;
			} else if(*in_roomscolouur(x,y,14) && *in_rooms(x,y,OROOM)) {
				color = 14;
			} else if(*in_roomscolouur(x,y,20) && *in_rooms(x,y,OROOM)) {
				color = rn2(15);
			}
		}

#endif
	    if (color == NO_COLOR) cmap_color(offset);
		if (Hallucination) color = rn2(15);
    } else if ((offset = (glyph - GLYPH_OBJ_OFF)) >= 0) {	/* object */
	if (On_stairs(x,y) && levl[x][y].seenv) special |= MG_STAIRS;
	if (offset == BOULDER && iflags.bouldersym) ch = iflags.bouldersym;
	else ch = oc_syms[(int)objects[offset].oc_class];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color) {
	    switch(objects[offset].oc_class) {
		case COIN_CLASS: color = CLR_YELLOW; break;
		case FOOD_CLASS: color = CLR_RED; break;
		default: color = CLR_BRIGHT_BLUE; break;
	    }
	} else
#endif
	    obj_color(offset);
	if (offset != BOULDER &&
		level.objects[x][y] &&
		level.objects[x][y]->nexthere)
		special |= MG_OBJPILE;
    } else if ((offset = (glyph - GLYPH_RIDDEN_OFF)) >= 0) {	/* mon ridden */
	ch = monsyms[(int)mons[offset].mlet];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    /* This currently implies that the hero is here -- monsters */
	    /* don't ride (yet...).  Should we set it to yellow like in */
	    /* the monster case below?  There is no equivalent in rogue. */
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    mon_color(offset);
	    special |= MG_RIDDEN;
    } else if ((offset = (glyph - GLYPH_BODY_OFF)) >= 0) {	/* a corpse */
	if (On_stairs(x,y) && levl[x][y].seenv) special |= MG_STAIRS;
	ch = oc_syms[(int)objects[CORPSE].oc_class];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color)
	    color = CLR_RED;
	else
#endif
	    mon_color(offset);
	    special |= MG_CORPSE;
	    if (offset != BOULDER &&
		level.objects[x][y] &&
		level.objects[x][y]->nexthere)
		special |= MG_OBJPILE;
    } else if ((offset = (glyph - GLYPH_DETECT_OFF)) >= 0) {	/* mon detect */
	ch = monsyms[(int)mons[offset].mlet];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    mon_color(offset);
	/* Disabled for now; anyone want to get reverse video to work? */
	/* is_reverse = TRUE; */
	    special |= MG_DETECT;
    } else if ((offset = (glyph - GLYPH_INVIS_OFF)) >= 0) {	/* invisible */
	ch = DEF_INVISIBLE;
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    invis_color(offset);
	    special |= MG_INVIS;
    } else if ((offset = (glyph - GLYPH_PET_OFF)) >= 0) {	/* a pet */
	ch = monsyms[(int)mons[offset].mlet];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS)
	    color = NO_COLOR;	/* no need to check iflags.use_color */
	else
#endif
	    pet_color(offset);
	    special |= MG_PET;
    } else {							/* a monster */
	ch = monsyms[(int)mons[glyph].mlet];
#ifdef ROGUE_COLOR
	if (HAS_ROGUE_IBM_GRAPHICS && iflags.use_color) {
	    if (x == u.ux && y == u.uy)
		/* actually player should be yellow-on-gray if in a corridor */
		color = CLR_YELLOW;
	    else
		color = NO_COLOR;
	} else
#endif
	{
	    mon_color(glyph);
	    /* special case the hero for `showrace' option */
#ifdef TEXTCOLOR
	    if (iflags.use_color && x == u.ux && y == u.uy &&
		    iflags.showrace && !Upolyd)
		color = HI_DOMESTIC;
#endif
	}
    }

#ifdef TEXTCOLOR
    /* Turn off color if no color defined, or rogue level w/o PC graphics. */
# ifdef REINCARNATION
#  ifdef ASCIIGRAPH
    if (!has_color(color) || (Is_rogue_level(&u.uz) && !HAS_ROGUE_IBM_GRAPHICS))
#  else
    if (!has_color(color) || Is_rogue_level(&u.uz))
#  endif
# else
    if (!has_color(color))
# endif
	color = NO_COLOR;
#endif

    *ochar = (int)ch;
    *ospecial = special;
#ifdef TEXTCOLOR
    *ocolor = color;
#endif
    return;
}

/*mapglyph.c*/
