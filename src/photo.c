/*	SCCS Id: @(#)photo.c	3.4	2003/01/17	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#ifdef NEPHI_PHOTOGRAPHY

STATIC_DCL char *FDECL(short_desc,(struct photograph*,char,char* (*)(const char*)));
STATIC_DCL boolean FDECL(photographable, (struct permonst*));
STATIC_DCL void FDECL(film_mon, (struct mon_photo *,struct monst *,char*));
STATIC_DCL void FDECL(film_obj, (struct obj_photo *,struct obj *,char*));
STATIC_DCL void FDECL(film_u, (struct mon_photo *,char*));
STATIC_DCL const char *FDECL(objtype_desc, (struct photograph *));
STATIC_DCL int FDECL(photo_interest, (struct obj*,char));
STATIC_DCL int FDECL(map_char_to_location, (unsigned));
STATIC_DCL const char *FDECL(select_preposition, (uchar,boolean,boolean));

extern const struct shclass shtypes[];
extern int n_dgns; /* from dungeon.c */
extern boolean notonhead;	/* for long worms */

#define scc static const char
scc photograph_str[] = "photograph";
scc photograph_of[] = "photograph of ";
scc autographed[] = "autographed ";
scc out_of_focus[] = "blurry";
scc invalid_photo[] = "unexposed photograph";
scc invalid_photo_hallu[] = "photograph of the inside of your head";
scc the_inside_of[] = "the inside of ";
/* dungeon features */
scc TREE_STR[] = "tree";
scc POOL_STR[] = "pool";
scc MOAT_STR[] = "moat";
scc WATER_STR[] = "water";
scc DRAWBRIDGE_UP_STR[] = "raised drawbridge";
scc LAVAPOOL_STR[] = "pool of lava";
scc IRONBARS_STR[] = "some iron bars";
scc DOOR_STR[] = "door";
scc CORR_STR[] = "corridor";
scc ROOM_STR[] = "room";
scc STAIRS_STR[] = "stairs";
scc LADDER_STR[] = "ladder";
scc FOUNTAIN_STR[] = "fountain";
scc THRONE_STR[] = "throne";
scc SINK_STR[] = "sink";
scc TOILET_STR[] = "toilet";
scc GRAVE_STR[] = "grave";
scc ALTAR_STR[] = "altar";
scc ICE_STR[] = "patch of ice";
scc DRAWBRIDGE_DOWN_STR[] = "lowered drawbridge";
scc AIR_STR[] = "empty air";
scc CLOUD_STR[] = "cloud";
scc PHOTO_FLOOR_STR[] = "floor";
/* ceilings */
scc CEILING_CEIL_STR[] = "ceiling";
scc CEILING_ROCK_STR[] = "rocky ceiling";
/* traps */
scc ARROW_TRAP_STR[] = "arrow trap";
scc DART_TRAP_STR[] = "dart trap";
scc ROCKTRAP_STR[] = "falling rock trap";
scc SQKY_BOARD_STR[] = "squeaky board";
scc BEAR_TRAP_STR[] = "bear trap";
scc LANDMINE_STR[] = "land mine";
scc ROLLING_BOULDER_TRAP_STR[] = "rolling boulder trap";
scc SLP_GAS_TRAP_STR[] = "gas trap";
scc RUST_TRAP_STR[] = "rust trap";
scc FIRE_TRAP_STR[] = "fire trap";
scc PIT_STR[] = "pit";
scc SPIKED_PIT_STR[] = "spiked pit";
scc HOLE_STR[] = "hole";
scc TRAPDOOR_STR[] = "trapdoor";
scc TELEP_TRAP_STR[] = "teleport trap";
scc LEVEL_TELEP_STR[] = "level teleport trap";
scc MAGIC_PORTAL_STR[] = "magic portal";
scc WEB_STR[] = "web";
scc STATUE_TRAP_STR[] = "statue";
scc MAGIC_TRAP_STR[] = "magic trap";
scc ANTI_MAGIC_STR[] = "anti-magic trap";
scc POLY_TRAP_STR[] = "polymorph trap";
scc ICE_TRAP_STR[] = "ice trap";
scc SPEAR_TRAP_STR[] = "spear trap";
scc COLLAPSE_TRAP_STR[] = "ceiling collapse trap";
scc MAGIC_BEAM_TRAP_STR[] = "magic beam trap";
/* special rooms */
scc COURT_STR[] = "throneroom";
scc SWAMP_STR[] = "swamp";
scc VAULT_STR[] = "magic memory vault";
scc BEEHIVE_STR[] = "giant beehive";
scc MORGUE_STR[] = "graveyard";
scc BARRACKS_STR[] = "military barracks";
scc ZOO_STR[] = "zoo";
scc DELPHI_STR[] = "Delphi";
scc TEMPLE_STR[] = "temple";
scc REALZOO_STR[] = "real zoo";
scc GIANTCOURT_STR[] = "giant court";
scc LEPREHALL_STR[] = "leprechaun hall";
scc DRAGONLAIR_STR[] = "dragon lair";
scc BADFOODSHOP_STR[] = "bad food shop";
scc COCKNEST_STR[] = "disgusting nest";
scc ANTHOLE_STR[] = "giant ant-hole";
scc LEMUREPIT_STR[] = "lemure pit";
scc MIGOHIVE_STR[] = "migo hive";
scc FUNGUSFARM_STR[] = "fungus farm";
scc CLINIC_STR[] = "clinic";
scc TERRORHALL_STR[] = "terror hall";
scc COINHALL_STR[] = "coin hall";
scc DOUGROOM_STR[] = "42 room";
scc ARMORY_STR[] = "abandoned armory";
scc GARDEN_STR[] = "beautiful garden";
scc TRAPROOM_STR[] = "trap room";
scc POOLROOM_STR[] = "pool room";
scc UNKNOWN_DUNGEON_STR[] = "an unfamiliar location";

scc * const autograph_text[] = {
	"Thanks for watching.",
	"To my #1 fan.",
	"Last night was magic for me ...",
	"To Maud: I'll never forget you.",
	"I'll be back.",
	"Good Luck!",
	"Here's something to remember me by, coward!",
};

scc fly_the_bird[] = "%s seems to be making an obscene gesture.";
#undef scc

/* gives short description of photo onto str and returns str */
/* if you don't want to override photo->subject_type, pass in -1 */
STATIC_OVL char*
short_desc(photo,override,article_func)
struct photograph* photo;
char override;
char* FDECL((*article_func), (const char*));
{
#ifdef LINT	/* static char buf[BUFSZ]; */
	char str[BUFSZ];
#else
	static char str[BUFSZ];
#endif
	const char *tmp;
	struct permonst *mptr;
	boolean wants_article;

	*str = '\0';
	
	if(!photo || photo->subject_type == PHOTO_INVALID) {
		Strcat(str, Hallucination ? 
			invalid_photo_hallu : invalid_photo);
		return(str);
	}

	if(override<0)
		override = photo->subject_type;
	switch(override) {
	case PHOTO_MON_INT:
		Strcat(str, the_inside_of);
		/* fall through */
	case PHOTO_MON:
		mptr = &mons[photo->mon.montype];
		tmp = mptr->mname;
		if(photo->mnamelen) {
		/* use a different name scheme: 
		"Tina the kitten" rather then "a kitten named Tina"
		so we can distinguish photographed monster names from photgraph names */
			Strcat(str,PHOTO_MNAME(photo));
			if(article_func != the && photo->special_name==0) {
				Strcat(str, " ");
				Strcat(str, the(tmp));
			}
		}
		else {
			if(!type_is_pname(mptr) && article_func)
				Strcat(str,article_func(tmp));
			else
				Strcat(str,tmp);
		}
		return(str);
	case PHOTO_OBJ_INT:
		Strcat(str, the_inside_of);
		/* fall through */
	case PHOTO_OBJ:
		Strcat(str, objtype_desc(photo));
		return(str);
	case PHOTO_FEATURE:
		wants_article=TRUE;
		switch(photo->feature) {
		case TREE:			tmp = TREE_STR;break;
		case POOL:			tmp = POOL_STR;break;
		case MOAT:			tmp = MOAT_STR;break;
		case WATER:			tmp = WATER_STR;wants_article=FALSE;break;
		case DRAWBRIDGE_UP:	tmp = DRAWBRIDGE_UP_STR;break;
		case LAVAPOOL:		tmp = LAVAPOOL_STR;break;
		case IRONBARS:		tmp = IRONBARS_STR;wants_article=FALSE;break;
		case DOOR:			tmp = DOOR_STR;break;
		case CORR:			tmp = CORR_STR;break;
		case ROOM:			tmp = ROOM_STR;break;
		case STAIRS:		tmp = STAIRS_STR;wants_article=FALSE;break;
		case LADDER:		tmp = LADDER_STR;break;
		case FOUNTAIN:		tmp = FOUNTAIN_STR;break;
		case THRONE:		tmp = THRONE_STR;break;
		case SINK:			tmp = SINK_STR;break;
		case TOILET:		tmp = TOILET_STR;break;
		case GRAVE:			tmp = GRAVE_STR;break;
		case ALTAR:			tmp = ALTAR_STR;break;
		case ICE:			tmp = ICE_STR;break;
		case DRAWBRIDGE_DOWN:tmp = DRAWBRIDGE_DOWN_STR;break;
		case AIR:			tmp = AIR_STR;wants_article=FALSE;break;
		case CLOUD:			tmp = CLOUD_STR;wants_article=FALSE;break;
		case PHOTO_FLOOR:	tmp = PHOTO_FLOOR_STR;break;
		case PHOTO_TRAP_MASK|ARROW_TRAP:		tmp = ARROW_TRAP_STR;break;
		case PHOTO_TRAP_MASK|DART_TRAP:			tmp = DART_TRAP_STR;break;
		case PHOTO_TRAP_MASK|ROCKTRAP:			tmp = ROCKTRAP_STR;break;
		case PHOTO_TRAP_MASK|SQKY_BOARD:		tmp = SQKY_BOARD_STR;break;
		case PHOTO_TRAP_MASK|BEAR_TRAP:			tmp = BEAR_TRAP_STR;break;
		case PHOTO_TRAP_MASK|LANDMINE:			tmp = LANDMINE_STR;break;
		case PHOTO_TRAP_MASK|ROLLING_BOULDER_TRAP:tmp = ROLLING_BOULDER_TRAP_STR;break;
		case PHOTO_TRAP_MASK|SLP_GAS_TRAP:		tmp = SLP_GAS_TRAP_STR;break;
		case PHOTO_TRAP_MASK|RUST_TRAP:			tmp = RUST_TRAP_STR;break;
		case PHOTO_TRAP_MASK|FIRE_TRAP:			tmp = FIRE_TRAP_STR;break;
		case PHOTO_TRAP_MASK|PIT:				tmp = PIT_STR;break;
		case PHOTO_TRAP_MASK|SPIKED_PIT:		tmp = SPIKED_PIT_STR;break;
		case PHOTO_TRAP_MASK|HOLE:				tmp = HOLE_STR;break;
		case PHOTO_TRAP_MASK|TRAPDOOR:			tmp = TRAPDOOR_STR;break;
		case PHOTO_TRAP_MASK|TELEP_TRAP:		tmp = TELEP_TRAP_STR;break;
		case PHOTO_TRAP_MASK|LEVEL_TELEP:		tmp = LEVEL_TELEP_STR;break;
		case PHOTO_TRAP_MASK|MAGIC_PORTAL:		tmp = MAGIC_PORTAL_STR;break;
		case PHOTO_TRAP_MASK|WEB:				tmp = WEB_STR;break;
		case PHOTO_TRAP_MASK|STATUE_TRAP:		tmp = STATUE_TRAP_STR;break;
		case PHOTO_TRAP_MASK|MAGIC_TRAP:		tmp = MAGIC_TRAP_STR;break;
		case PHOTO_TRAP_MASK|ANTI_MAGIC:		tmp = ANTI_MAGIC_STR;break;
		case PHOTO_TRAP_MASK|POLY_TRAP:			tmp = POLY_TRAP_STR;break;
		case PHOTO_TRAP_MASK|ICE_TRAP:			tmp = ICE_TRAP_STR;break;
		case PHOTO_TRAP_MASK|SPEAR_TRAP:		tmp = SPEAR_TRAP_STR;break;
		case PHOTO_TRAP_MASK|COLLAPSE_TRAP:		tmp = COLLAPSE_TRAP_STR;break;
		case PHOTO_TRAP_MASK|MAGIC_BEAM_TRAP:		tmp = MAGIC_BEAM_TRAP_STR;break;
		default:
			Sprintf(str,"strange feature: %d",photo->feature);
			return(str);
		}
		break;
	case PHOTO_CEILING:
		wants_article=TRUE;
		switch(photo->feature) {
		case PHOTO_CEILING_ROCK:	tmp = CEILING_ROCK_STR;break;
		case PHOTO_CEILING_AIR:		tmp = AIR_STR;wants_article=FALSE;break;
		case PHOTO_CEILING_WATER:	tmp = WATER_STR;wants_article=FALSE;break;
		default:
		case PHOTO_CEILING_CEIL:	tmp = CEILING_CEIL_STR;break;
		}
		break;
	case PHOTO_SCENIC:
	default:
		wants_article=TRUE;
		switch(photo->special_room) {
		case COURT:		tmp = COURT_STR;break;
		case SWAMP:		tmp = SWAMP_STR;break;
		case VAULT:		tmp = VAULT_STR;break;
		case BEEHIVE:	tmp = BEEHIVE_STR;break;
		case MORGUE:	tmp = MORGUE_STR;break;
		case BARRACKS:	tmp = BARRACKS_STR;wants_article=FALSE;break;
		case ZOO:		tmp = ZOO_STR;break;
		case DELPHI:	tmp = DELPHI_STR;wants_article=FALSE;break;
		case TEMPLE:	tmp = TEMPLE_STR;break;
		case REALZOO:	tmp = REALZOO_STR;break;
		case GIANTCOURT: tmp = GIANTCOURT_STR;break;
		case LEPREHALL:	tmp = LEPREHALL_STR;break;
		case DRAGONLAIR: tmp = DRAGONLAIR_STR;break;
		case BADFOODSHOP: tmp = BADFOODSHOP_STR;break;
		case COCKNEST:	tmp = COCKNEST_STR;break;
		case ANTHOLE:	tmp = ANTHOLE_STR;break;
		case LEMUREPIT: tmp = LEMUREPIT_STR;break;
		case MIGOHIVE: tmp = MIGOHIVE_STR;break;
		case FUNGUSFARM: tmp = FUNGUSFARM_STR;break;
		case CLINIC: tmp = CLINIC_STR;break;
		case TERRORHALL: tmp = TERRORHALL_STR;break;
		case COINHALL: tmp = COINHALL_STR;break;
		case DOUGROOM: tmp = DOUGROOM_STR;break;
		case ARMORY:	tmp = ARMORY_STR;break;
		case GARDEN:	tmp = GARDEN_STR;break;
		case TRAPROOM: tmp = TRAPROOM_STR;break;
		case POOLROOM:	tmp = POOLROOM_STR;break;
		case ARMORSHOP:	
		case SCROLLSHOP:
		case POTIONSHOP:
		case WEAPONSHOP:
		case FOODSHOP:
		case RINGSHOP:
		case WANDSHOP:
		case TOOLSHOP:
		case PETSHOP:
		case TINSHOP:
		case BOOKSHOP:
		case GUNSHOP:
		case INSTRUMENTSHOP:
		case CANDLESHOP:
		case BLACKSHOP:

			tmp = shtypes[photo->special_room - SHOPBASE].name;
			break;
		case OROOM:
		default:
			if(dungeons[photo->location.dnum].dunlev_ureached) {
				if(strncmpi(tmp=dungeons[photo->location.dnum].dname,"the ",4)==0) {
					tmp += 4;
					Strcat(str, "the ");
				}
				if(Role_if(PM_TOURIST))
					Strcat(str, "scenic ");
			}
			else {
				tmp = UNKNOWN_DUNGEON_STR;
			}
			Strcat(str, tmp);
			return(str);
		}
	}
	if(wants_article && article_func) {
		Strcat(str,article_func(tmp));
	}
	else
		Strcat(str,tmp);
	return(str);
}

char*
glance_photo(photo)
struct photograph* photo;
{
#ifdef LINT	/* static char buf[BUFSZ]; */
	char buf[BUFSZ];
#else
	static char buf[BUFSZ];
#endif

	buf[0] = '\0';
	if(photo && photo->subject_type!=PHOTO_INVALID) {
		if(photo->autographed)	Strcat(buf,autographed);
		if(photo->out_of_focus)	{
			Strcat(buf,out_of_focus);
			Strcat(buf," ");
		}
		Strcat(buf,photograph_of);
	}
	Strcat(buf,short_desc(photo,-1,an));

	return buf;
}

void
examine_photo(photo)
struct photograph* photo;
{
#ifdef LINT	/* static char buf[BUFSZ]; */
	char buf[BUFSZ];
#else
	static char buf[BUFSZ];
#endif
	struct mon_photo *mp;
	struct obj_photo *op;
	struct permonst *mptr;
	char mon_gender, range;
	const char *he, *his, *him;
	char He[4], His[4], Him[4];
	winid win = create_nhwindow(NHW_TEXT);

	Strcpy(buf,"You examine the photograph:");
	putstr(win, 0, buf);

	if(!photo || photo->subject_type==PHOTO_INVALID) {
		Sprintf(buf,"This is %s.", an(Hallucination ? 
			invalid_photo_hallu : invalid_photo));
		putstr(win,0,buf);
		goto out;
	}

	range = photo->range;
	buf[0]='\0';
	if(photo->out_of_focus)	{
		Sprintf(buf+strlen(buf),"The %s is quite %s. ",
			photograph_str,out_of_focus);
		putstr(win,0,buf);
	}
	mp = &photo->mon;
	mptr = &mons[mp->montype];
	op = &photo->obj;

	Sprintf(buf,"You see a%s%sshot of ",
		photo->frame_prob && photo->subject_type!=PHOTO_MON ?
		"n off-centered":"",
		range<=PHOTO_CLOSE_RANGE ? " close-up ": 
		(range>=PHOTO_LONG_RANGE ? " distant ":" "));
	switch(photo->subject_type) {
	case PHOTO_CEILING:
	case PHOTO_SCENIC:
	case PHOTO_FEATURE:
	case PHOTO_MON_INT:
	case PHOTO_OBJ_INT:
		Sprintf(buf+strlen(buf),"%s.  ",short_desc(photo,-1,an));
		putstr(win,0,buf);		
		break;
	case PHOTO_OBJ:
		Sprintf(buf+strlen(buf),"%s.  ",short_desc(photo,-1,an));
		/* give description of any dungeon features */
		if(photo->feature!=ROOM && photo->feature!=CORR) {
			Sprintf(buf+strlen(buf),"%s %s %s.  ", op->quan>1 ? "They are":"It is", 
				select_preposition(photo->feature,0,TRUE),
				short_desc(photo,PHOTO_FEATURE,an));
		}
		putstr(win,0,buf);
		break;
	case PHOTO_MON:
		mon_gender = (range>=PHOTO_LONG_RANGE || !humanoid(mptr)) && photo->mnamelen==0 ? 2 : mp->female;
		he = genders[mon_gender].he;
		his = genders[mon_gender].his;
		him = genders[mon_gender].him;
		Strcpy(He,he);
		*He = highc(*He);
		Strcpy(Him,him);
		*Him = highc(*Him);
		Strcpy(His,his);
		*His = highc(*His);

		if(mp->sleeping) {
			Sprintf(buf+strlen(buf),"%s, fast asleep.  ",short_desc(photo,-1,an));
		}
		else
			Sprintf(buf+strlen(buf),"%s.  ",short_desc(photo,-1,an));

		/* add some descriptors */
		if(mp->leashed) {
			Sprintf(buf+strlen(buf),"%s is leashed.  ",He);
		}
		if(BLUR_FACTOR(photo) < PHOTO_LONG_RANGE) {
			if(mp->flee) {
				Sprintf(buf+strlen(buf),"%s appears to be fleeing.  ",He);
			}
			if(!mp->canmove && !mp->sleeping) {
				Sprintf(buf+strlen(buf),"%s seems to be immobilized.  ",He);
			}
			if(mp->stun) {
				Sprintf(buf+strlen(buf),"%s looks rather stunned.  ",He);
			}
		}
		if(BLUR_FACTOR(photo) <= PHOTO_CLOSE_RANGE) {
			if(mp->conf) {
				Sprintf(buf+strlen(buf),"%s has a confused look on %s %s.  ",
					short_desc(photo,-1,The),
					his,pmbodypart(mptr,FACE));
			}
			if(!mp->cansee && haseyes(mptr)) {
				Sprintf(buf+strlen(buf),"%s looks blinded.  ",He);
			}
		}
		/* show object, if any */
		if(photo->mon_re_obj==PHOTO_AT_FEET) {
			Sprintf(buf+strlen(buf),"There %s %s at %s %s.  ", 
				op->quan>1?"are":"is",
				short_desc(photo,PHOTO_OBJ,an),
				his, makeplural(pmbodypart(mptr,FOOT)));
		} else if(photo->mon_re_obj != PHOTO_NO_OBJ) {
			Sprintf(buf+strlen(buf), "%s is %sing %s%s  ", 
				He,	photo->mon_re_obj==PHOTO_WIELD?"wield":
				(photo->mon_re_obj==PHOTO_WEAR?"wear":"carry"),
				short_desc(photo,PHOTO_OBJ,an),
				op->artifact?"!":".");
		}

		/* give description of any dungeon features */
		if(photo->feature!=ROOM && photo->feature!=CORR) {
			Sprintf(buf+strlen(buf),"%s is %s%s %s.  ", 
				He, mp->flying ? "floating " : "",
				select_preposition(photo->feature,mp->flying,mp->trapped),
				short_desc(photo,PHOTO_FEATURE,an));
		}
		putstr(win,0,buf);

		if(photo->frame_prob) {
			Sprintf(buf,"Part of %s %s is cut off.  ", 
				s_suffix(short_desc(photo,-1,the)),	
				pmbodypart(mptr,HEAD));
			putstr(win,0,buf);
		}

		if(photo->autographed) {
			Sprintf(buf,
				"%s appears to be personally signed by %s. It reads:",
				The(photograph_str), short_desc(photo,PHOTO_MON,the));
			putstr(win,0,buf);
			putstr(win,0,autograph_text[photo->autographed-1]);
			/* additional taunting for cowards */
			if(photo->autographed==AUTOGRAPH_COWARD) {
				Sprintf(buf,fly_the_bird,He);
				putstr(win,0,buf);
			}
		}
		break;
	}

	/* engraving */
	if(photo->subject_type<=PHOTO_OBJ
		&& photo->ph_engrlen > 0) {
		const char *surface;
		switch(photo->feature) {
		case TREE:			surface = TREE_STR;break;
		case STAIRS:		surface = STAIRS_STR;break;
		case LADDER:		surface = LADDER_STR;break;
		case ALTAR:			surface = ALTAR_STR;break;
		case ICE:			surface = ICE_STR;break;
		case AIR:			surface = AIR_STR;break;
		case PHOTO_TRAP_MASK|PIT:
		case PHOTO_TRAP_MASK|SPIKED_PIT:		surface = "floor of the pit";break;
		default:
			surface = PHOTO_FLOOR_STR;
		}
		switch(photo->ph_engr_type) {
	    case DUST:
			Sprintf(buf,"You see %s written in the %s.", something,
				photo->feature==ICE ? "frost" : "dust");
		break;
	    case ENGRAVE:
	    case HEADSTONE:
			Sprintf(buf,"You see %s engraved on %s.",
				something,
				photo->feature==GRAVE ? "the tombstone":the(surface));
		break;
	    case BURN:
			Sprintf(buf,"Some text was %s into %s.",
				photo->feature==ICE ? "melted" : "burned",
				the(surface));
		break;
	    case MARK:
			Sprintf(buf,"There's some graffiti on %s.",
				the(surface));
		break;
	    case ENGR_BLOOD:
			Sprintf(buf,"You see a message scrawled in blood on %s.",
				the(surface));
		break;
	    default:
		impossible("%s is written in a very strange way.",
				Something);
	    }
		if(photo->out_of_focus) {
			Sprintf(buf+strlen(buf), "  Unfortunately, it is too %s to be read.",
				out_of_focus);
		} else {
			Sprintf(buf+strlen(buf),"  It reads: \"%s\"", PHOTO_ENGR(photo));
		}
		putstr(win,0,buf);
	}

	if(photo->subject_type!=PHOTO_SCENIC && photo->subject_type<=PHOTO_MON
		&& !photo->autographed) {
		/* give idea of surrounding area */
		if((range < PHOTO_CLOSE_RANGE || photo->out_of_focus)) {
			Strcpy(buf,"You can't make out much of the surrounding area.  ");
		} else {
			Sprintf(buf,"It looks like this %s was taken in %s.  ",
				photograph_str,
				short_desc(photo,PHOTO_SCENIC,an));
		}
		putstr(win,0,buf);
	}

out:
	display_nhwindow(win, TRUE);
	destroy_nhwindow(win);	
}

struct photo_buff {
	struct photograph film;
	char mname[PL_PSIZ];
	char oname[PL_PSIZ];
};

struct obj*
expose_film(photo,camera,x,y,dx,dy,dz,range,flash,where)
struct obj* photo,*camera;
int x,y,dx,dy,dz,range,where;
boolean flash;
{
	struct mon_photo *mphoto;
	struct obj_photo *ophoto;
	struct monst *mtmp;
	struct obj *curr,*interesting,*here,*someinvent,*wielded;
	struct trap *ttmp;
	struct rm *r;
	uchar typ,interest1,interest2;
	char trange,mon_re_obj;
	struct photo_buff pbuff;
	struct photograph *film;
	int sx=x,sy=y;

	(void)memset(&pbuff,0,sizeof(struct photo_buff));

	film = &pbuff.film;	
	mphoto = &film->mon;
	ophoto = &film->obj;

	film->location = u.uz;
	film->range = 0;

	/* logic for upwards or downwards shots: 
		if camera is in an inventory and dz<0 then photo will be ceiling
		if camera is on the floor and dz>0 then photo will be floor
		if camera is in an inventory and dz>0 then photo will be object on floor or feature, or floor if none
		if camera is on the floor and dz<0 then photo will be monster in square, or ceiling if none
	*/
	if((dz<0 && (where==OBJ_INVENT || where==OBJ_MINVENT)) || 
		(dz>0 && where==OBJ_FLOOR))
		range = 0;
	else if(dz || (dx==0 && dy==0 && dz==0))
		range = 1;

	film->subject_type = level.locations[x][y].lit || flash ? PHOTO_SCENIC : PHOTO_INVALID;
	film->feature = 0;
	trange = range;
	while(trange-- > 0) {

	    x += dx;
	    y += dy;

	    if(!isok(x, y))	break;

		r = &level.locations[x][y];
		typ = r->typ;
		if(!ZAP_POS(typ) && typ!=TREE)	
		/* allow photos of trees */
			break;
		
		if(!flash && !r->lit)
			/* dark square */
			continue;

		film->special_room = rooms[r->roomno - ROOMOFFSET].rtype;
		
		here = is_pool(x,y) || dz<0 ? (struct obj*)0 : level.objects[x][y];
#ifdef INVISIBLE_OBJECTS
		while(here && here->oinvis)
			here=here->nexthere;
#endif
		/* don't let camera photograph itself */
		if(here && here==camera)
			here=here->nexthere;

		/* film feature, unless shot is upwards */
		if(dz>=0) {
			ttmp = t_at(x,y);
			if(ttmp && ttmp->tseen)	
				typ = PHOTO_TRAP_MASK | ttmp->ttyp;

			if(!film->feature && typ!=ROOM && typ!=CORR && 
				(!IS_DOOR(typ) || r->doormask >= D_ISOPEN)) {
				film->feature = typ;
				film->subject_type = PHOTO_FEATURE;
				film->range = range-trange;
			}
		}

		mtmp = (struct monst*)0;
		/* decide on a monster to film, unless shot is downwards */
		if(dz<=0) {
			if(u.uswallow && x==u.ux && y==u.uy) {
				mtmp = u.ustuck;
			} else if(x==u.ux && y==u.uy && !Invisible) {
				if(!Upolyd || photographable(&mons[u.umonnum])) {
					film_u(mphoto,pbuff.mname);
					someinvent = invent;
					wielded = uwep;
					goto film_mon;
				}
			} else if(mtmp = m_at(x,y)) {
				/* handle disguised creatures */
				switch(mtmp->m_ap_type) {
				case M_AP_FURNITURE:
					typ = map_char_to_location(mtmp->mappearance);
					if(!film->feature && !IS_DOOR(typ)) {
						film->feature = typ;
						film->subject_type = PHOTO_FEATURE;
						film->range = range-trange;
					}
					break;
				case M_AP_OBJECT:
					if(film->subject_type < PHOTO_OBJ) {
						memset(ophoto,0,sizeof(ophoto));
						ophoto->type = mtmp->mappearance;
						ophoto->quan = 1;
						goto film_obj;
					}
					break;
				case M_AP_MONSTER:
					film_mon(mphoto,mtmp,pbuff.mname);
					mphoto->montype = mtmp->mappearance;
					someinvent = mtmp->minvent;
					wielded = MON_WEP(mtmp);
					goto film_mon;
				}
			}
		}

		if (mtmp && mtmp->m_ap_type==M_AP_NOTHING) {
			if(!mtmp->minvis && !mtmp->mundetected
				&& photographable(mtmp->data)) {
				film_mon(mphoto,mtmp,pbuff.mname);
				someinvent = mtmp->minvent;
				wielded = MON_WEP(mtmp);
film_mon:
				film->subject_type = PHOTO_MON;
				film->feature = typ;
				film->range = range-trange;
				if(is_pool(x,y) && !is_flyer(mtmp->data))
					film->out_of_focus=1;
				pbuff.oname[0] = '\0';
				/* see if there is some interesting object along with monster */
				interesting = (struct obj *)0;
				interest1=interest2=0;
				for(curr=someinvent;curr;curr = curr->nobj) {
					/* don't let camera photograph itself */
					if(curr==camera) continue;
					mon_re_obj = curr==wielded ? PHOTO_WIELD : 
						(curr->owornmask & (W_ARMOR | W_RING | W_AMUL | W_TOOL
#ifdef STEED
							| W_SADDLE
#endif
							) ? PHOTO_WEAR : PHOTO_CARRY);
					interest1 = photo_interest(curr,mon_re_obj);
					if(interest1>interest2) {
						interest2 = interest1;
						interesting = curr;
						film->mon_re_obj = mon_re_obj;
					}
				}
				if(here && photo_interest(here,PHOTO_AT_FEET)>interest2) {
					interesting = here;
					film->mon_re_obj = PHOTO_AT_FEET;
				}
				film_obj(ophoto,interesting,pbuff.oname);
				break;
			}
		}
		if(here && film->subject_type < PHOTO_OBJ) {
			film_obj(ophoto,here,pbuff.oname);
film_obj:
			film->subject_type = PHOTO_OBJ;
			film->feature = typ;
			film->range = range-trange;
			if(ophoto->type == BOULDER)	break;
		}

		if(typ == DRAWBRIDGE_UP || closed_door(x,y) || typ==TREE)
			break;
	}
	if(film->feature==ROOM && dz>0)
		film->feature = PHOTO_FLOOR;
	/* if nothing was filmed */
	if(film->subject_type==PHOTO_SCENIC) {
		film->range = range-trange;
		if(dz<0) {
			/* film ceiling */
			typ = level.locations[x][y].typ;
			film->subject_type = PHOTO_CEILING;
			if (IS_AIR(typ)) {
				film->feature = PHOTO_CEILING_AIR;
			} else if (is_pool(x,y)) {
				if(where==OBJ_FLOOR)
					film->feature = PHOTO_CEILING_WATER;
				else
					film->feature = PHOTO_CEILING_CEIL;
			} else if ((IS_ROOM(typ) && !Is_earthlevel(&u.uz)) ||
				IS_WALL(typ) || IS_DOOR(typ) || typ == SDOOR) {
				film->feature = PHOTO_CEILING_CEIL;
			} else {
				film->feature = PHOTO_CEILING_ROCK;
			}
		} else if(dz>0) {
			/* film floor */
			film->subject_type = PHOTO_FEATURE;
			film->feature = PHOTO_FLOOR;
		}
	}

	/* film engraving if downward or of tombstone at short range */
	if( (film->subject_type>=PHOTO_SCENIC 
		&& film->subject_type<=PHOTO_OBJ) &&
		(dz>0 || (film->feature==GRAVE && 
		film->range<=PHOTO_CLOSE_RANGE)) ) {
		struct engr *e = engr_at(sx+dx*film->range,sy+dy*film->range);
		if(e) {
			film->ph_engr_type = e->engr_type;
			Strcpy(pbuff.mname,e->engr_txt);
		}
	}

	film->mnamelen = strlen(pbuff.mname);
	film->onamelen = strlen(pbuff.oname);
	if(pbuff.oname[0])
		memmove(pbuff.mname+film->mnamelen+1,pbuff.oname,film->onamelen+1);
	return realloc_obj(photo,
		sizeof(struct photograph)+film->mnamelen+film->onamelen+2,
		film,*ONAME(photo)?strlen(ONAME(photo))+1:0,ONAME(photo));
}

STATIC_OVL boolean
photographable(mdata)
struct permonst* mdata;
{
	return(mdata->mlet!=S_VAMPIRE && mdata->mlet!=S_GHOST);
}

STATIC_OVL void
film_mon(mphoto,mon,name)
struct mon_photo *mphoto;
struct monst *mon;
char* name;
{
	if(!mphoto || !mon)	return;
	if(mon->mnamelth)
		Strcpy(name,NAME(mon));
	else
		name[0] = '\0';
	mphoto->montype = mon->mnum;
	mphoto->female = mon->female;
	mphoto->flee = mon->mflee;
	mphoto->cansee = mon->mcansee;
	mphoto->canmove = mon->mcanmove;
	mphoto->sleeping = mon->msleeping;
	mphoto->stun = mon->mstun;
	mphoto->conf = mon->mconf;
	mphoto->peaceful = mon->mpeaceful;
	mphoto->trapped = mon->mtrapped;
	mphoto->leashed = mon->mleashed;
	mphoto->flying = is_flyer(mon->data);
}

STATIC_OVL void
film_u(mphoto,name)
struct mon_photo *mphoto;
char* name;
{
	if(!mphoto)	return;
	Strcpy(name,plname);
	mphoto->montype = Upolyd ? u.umonnum:u.umonster;
	mphoto->female = flags.female;
	mphoto->flee = 0;
	mphoto->cansee = !Blind;
	mphoto->canmove = multi ? 0:1;
	mphoto->sleeping = is_fainted() || u.usleep;
	mphoto->stun = Stunned;
	mphoto->conf = Confusion;
	mphoto->peaceful = 1;
	mphoto->trapped = u.utrap ? 1:0;
	mphoto->leashed = 0;
	mphoto->flying = Levitation || Flying;
}

STATIC_OVL void
film_obj(ophoto,obj,name)
struct obj_photo *ophoto;
struct obj *obj;
char* name;
{
	if(!ophoto || !obj)	return;
	if(obj->onamelth)
		Strcpy(name,ONAME(obj));
	else
		name[0] = '\0';
	ophoto->type = obj->otyp;
	ophoto->artifact = obj->oartifact;
	ophoto->quan = obj->quan;
	ophoto->corpsenm = obj->corpsenm;
	ophoto->eroded = obj->oeroded;
	ophoto->eroded2 = obj->oeroded2;
	ophoto->greased = obj->greased;
}

STATIC_OVL const char *
objtype_desc(photo)
struct photograph *photo;
{
	struct obj_photo *ophoto;
	char* desc;
	struct obj *otmp;
	char range;

	ophoto = &photo->obj;

	if(ophoto->type == SCR_PHOTOGRAPH) {
		desc = an(photograph_str);
	}
	else if(ophoto->artifact && 
		!undiscovered_artifact(ophoto->artifact)) {
		desc = (char*)artiname(ophoto->artifact);
	} else {
		range = BLUR_FACTOR(photo);
		otmp = mksobj(ophoto->type,FALSE,FALSE);
		otmp->quan = ophoto->quan;
		otmp->corpsenm = ophoto->corpsenm;
		if(range <= PHOTO_CLOSE_RANGE) {
			otmp->oeroded = ophoto->eroded;
			otmp->oeroded2 = ophoto->eroded2;
			otmp->greased = ophoto->greased;
		}
		if(range < PHOTO_LONG_RANGE) {
			otmp->dknown = 1;
			desc = doname(otmp);
		} else
			desc = distant_name(otmp,doname);

		/* use a different name scheme: 
		"Slicer the long sword" rather then "a long sword named Slicer"
		so we can distinguish photographed object names from photograph names
		*/
		if(photo->onamelen) {
			char *o,*p;
			o = desc[1]=='n'?desc+3:desc+2;
			p = PHOTO_ONAME(photo);
			memmove(desc+strlen(p)+5,o,strlen(o)+1);
			memcpy(desc,p,strlen(p));
			memcpy(desc+strlen(p)," the ",5);
		}

		obj_extract_self(otmp);
		dealloc_obj(otmp);
	}
	return(desc);
}

/* decide how interesting object is in relation to monster 
   in order to decide what to film */
STATIC_DCL int 
photo_interest(obj,mon_re_obj)
struct obj* obj;
char mon_re_obj;
{
	int i;
	struct objclass* oc = &objects[obj->otyp];
#ifdef INVISIBLE_OBJECTS
	if(obj->oinvis)
		return(0);
#endif
	/*	make single use objects carried/worn by monster
		uninteresting to help avoid abuse */
	if((mon_re_obj==PHOTO_CARRY && 
		(oc->oc_class==POTION_CLASS || oc->oc_class==SCROLL_CLASS))
		|| (mon_re_obj==PHOTO_WEAR && obj->otyp==AMULET_OF_LIFE_SAVING))
		return(0);
	i = 1;
	if(obj->oartifact)
		i+=50;
	if(oc->oc_unique)
		i+=100;
	if(mon_re_obj==PHOTO_WIELD)
		i+=10;
	if(oc->oc_big)
		i+=4;
	if(oc->oc_magic)
		i+=5;
	switch(obj->owornmask) {
	case W_ARM:	i+=8;
	case W_ARMC:i+=7;
	case W_ARMH:i+=6;
	case W_ARMS:i+=5;
	case W_ARMG:i+=4;
	case W_ARMF:i+=3;
	}
	return(i);
}

/* hack for mapping a mimic's appearance to location type */
STATIC_DCL int
map_char_to_location(appearance)
unsigned appearance;
{
	switch(appearance) {
	case S_upstair:
	case S_dnstair:
		return STAIRS;
	case S_upladder:
	case S_dnladder:
		return LADDER;
	case S_altar:
		return ALTAR;
	case S_grave:
		return GRAVE;
	case S_throne:
		return THRONE;
	case S_sink:
		return SINK;
	case S_fountain:
		return FOUNTAIN;
	case S_pool:
		return POOL;
	case S_ice:
		return ICE;
	case S_lava:
		return LAVAPOOL;
	case S_vodbridge:
	case S_hodbridge:
		return DRAWBRIDGE_DOWN;
	case S_vcdbridge:
	case S_hcdbridge:
		return DRAWBRIDGE_UP;
	case S_air:
		return AIR;
	case S_cloud:
		return CLOUD;
	case S_water:
		return WATER;
	}
	return 0;
}

STATIC_DCL const char *
select_preposition(feature,flying,trapped)
uchar feature;
boolean flying;
boolean trapped;
{
	static const char* preps[] = {
		"next to",
		"in",
		"on",
		"over",
		"under",
		"between",
		"stuck in"
	};

	if(flying) {
		switch(feature) {
		case AIR:
		case CLOUD:
			return preps[1];
		case PHOTO_TRAP_MASK|ROCKTRAP:
			return preps[4];
		default:
			return preps[3];
		}
	}

	switch(feature) {
	/* next to */
	case TREE:
	case LADDER:
	case STAIRS:
	case GRAVE:
	case FOUNTAIN:
	case THRONE:
	case SINK:
	case ALTAR:
	case DOOR:
	case PHOTO_TRAP_MASK|ARROW_TRAP:
	case PHOTO_TRAP_MASK|DART_TRAP:
	case PHOTO_TRAP_MASK|LANDMINE:
	case PHOTO_TRAP_MASK|ROLLING_BOULDER_TRAP:
	case PHOTO_TRAP_MASK|SLP_GAS_TRAP:
	case PHOTO_TRAP_MASK|RUST_TRAP:
	case PHOTO_TRAP_MASK|FIRE_TRAP:
	case PHOTO_TRAP_MASK|TELEP_TRAP:
	case PHOTO_TRAP_MASK|LEVEL_TELEP:
	case PHOTO_TRAP_MASK|MAGIC_PORTAL:
	case PHOTO_TRAP_MASK|STATUE_TRAP:
	case PHOTO_TRAP_MASK|MAGIC_TRAP:
	case PHOTO_TRAP_MASK|ANTI_MAGIC:
	case PHOTO_TRAP_MASK|POLY_TRAP:
		return preps[0];
	/* in */
	case CORR:
	case ROOM:
	case AIR:
	case CLOUD:
	case POOL:
	case MOAT:
	case WATER:
	case LAVAPOOL:
		return preps[1];
	/* on */
	case ICE:
	case DRAWBRIDGE_DOWN:
	case PHOTO_FLOOR:
	case PHOTO_TRAP_MASK|SQKY_BOARD:
		return preps[2];
	/* over */
	case PHOTO_TRAP_MASK|HOLE:
	case PHOTO_TRAP_MASK|TRAPDOOR:
		return preps[3];
	/* under */
	case PHOTO_TRAP_MASK|ROCKTRAP:
		return preps[4];
	/* between */
	case DRAWBRIDGE_UP:
	case IRONBARS:
		return preps[5];
	/* stuck in */
	case PHOTO_TRAP_MASK|WEB:
	case PHOTO_TRAP_MASK|PIT:
	case PHOTO_TRAP_MASK|SPIKED_PIT:
	case PHOTO_TRAP_MASK|BEAR_TRAP:
		return trapped ? preps[6]:preps[0];
	default:
		return preps[0];
	}	
}

struct obj*
gen_random_photo(photo)
struct obj* photo;
{
	d_level dtemp;
	struct mon_photo* mp;
	struct obj_photo* op;
	struct permonst* mptr;
	struct photograph* film;
	struct obj* newphoto;

	if(!photo)	return(photo);
	film = (struct photograph *)alloc(sizeof(struct photograph)+2);
	memset(film,0,sizeof(struct photograph)+2);
	/* Pick dungeon, excluding Elemental planes from consideration */
	/* This assumes that the Elemental planes are last on the list */
	if(!rn2(3))
		film->location.dnum = rn2(n_dgns-1);
	/* Pick level */
	film->location.dlevel = 
		rn2(dunlevs_in_dungeon(&film->location));
	/* Pick range */
	film->range = rn2(9)+1;
	/* to do: Pick special room */

	/* out of focus */
	film->out_of_focus = !rn2(5);
	/* frame problem */
	film->frame_prob = !rn2(5);
	/* subject type */
	film->subject_type = rn2(4)+PHOTO_SCENIC;
	switch(film->subject_type) {
	case PHOTO_MON:
		mp = &film->mon;
		/* hack: temporarily set u.uz = film->location */
		dtemp = u.uz;
		u.uz = film->location;
		mp->montype = rndmonnum();
		u.uz = dtemp;
		mptr = &mons[mp->montype];
		mp->female = is_female(mptr) ? 1 : (is_male(mptr)? 0:rn2(2));
		mp->cansee = rn2(12);
		mp->canmove = rn2(12);
		mp->sleeping = !rn2(12);
		if(!mp->sleeping && mp->canmove) {
			mp->flee = !rn2(12);
			mp->stun = !rn2(12);
			mp->conf = !rn2(12);
			mp->flying = is_flyer(mptr);
		}
		mp->peaceful = 1;
		if(!mp->flying) {
			if(mp->trapped = !rn2(50)) {
				switch(rn2(4))
				{
				case 0: film->feature = PHOTO_TRAP_MASK|WEB;break;
				case 1: film->feature = PHOTO_TRAP_MASK|PIT;break;
				case 2: film->feature = PHOTO_TRAP_MASK|SPIKED_PIT;break;
				case 3: film->feature = PHOTO_TRAP_MASK|BEAR_TRAP;break;
				}
			}
		}
		/* monster's object */
		if(!rn2(3)) {
			op = &film->obj;
			op->quan = 1;
			if(is_armed(mptr) && !rn2(3)) {
				op->type = rnd_class(STRANGE_OBJECT+1,CROSSBOW);
				film->mon_re_obj = PHOTO_WIELD;
			} else if(!cantweararm(mptr) && humanoid(mptr) && !rn2(3)) {
				op->type = rnd_class(ELVEN_LEATHER_HELM,LEVITATION_BOOTS);
				film->mon_re_obj = PHOTO_WEAR;
			} else if(!nohands(mptr) && rn2(2)) {
				op->type = rnd_class(STRANGE_OBJECT+1,ROCK);
				film->mon_re_obj = PHOTO_CARRY;
			} else {
				op->type = rnd_class(STRANGE_OBJECT+1,ROCK);
				film->mon_re_obj = PHOTO_AT_FEET;
			}
		}
		/* autograph */
		if(!rn2(25))	
			film->autographed = rn2(6)+1;
		/* feature */
		if(!mp->trapped && !rn2(5))
			goto feature;
		film->feature=ROOM;
		break;
	case PHOTO_OBJ:
		op = &film->obj;
		op->type = rnd_class(STRANGE_OBJECT+1,ROCK);
		op->quan = 1;
		/* set corpsenm */
		/* hack: temporarily set u.uz = film->location */
		dtemp = u.uz;
		u.uz = film->location;
		op->corpsenm = rndmonnum();
		u.uz = dtemp;
		if(!rn2(5))
			goto feature;
		film->feature=ROOM;
		break;
	case PHOTO_FEATURE:
feature:
		/* ugly */
		film->feature = rn2(MAX_TYPE - POOL + 1) + POOL;
		if(film->feature == MAX_TYPE)
			film->feature = TREE;
		break;
	}
	newphoto = realloc_obj(photo,sizeof(struct photograph)+2,film,
		*ONAME(photo)?strlen(ONAME(photo))+1:0,ONAME(photo));
	free((genericptr_t)film);
	return(newphoto);
}

struct obj*
make_special_photo(type)
int type;
{
	struct photo_buff pbuff;
	struct photograph* film;
	struct obj *photo;
	int size, r;
	const char* name;

	memset(&pbuff,0,sizeof(struct photo_buff));
	film = &pbuff.film;

	switch(type) {
	case PHOTO_SPECIAL_FAM:
		r = rn2(3);
		
		film->range = 1;
		film->mon_re_obj = PHOTO_NO_OBJ;
		film->subject_type = PHOTO_MON;
		film->feature = TREE;
		film->mon.montype = urace.malenum;
		film->mon.cansee = 1;
		film->mon.canmove = 1;
		film->autographed = AUTOGRAPH_GOOD_LUCK;

		switch(r) {
		case 0:
			film->mon.female = 1;
			name = "your mother";
			break;
		case 1:
			name = "your father";
			break;
		case 2:
			name = "your sweetheart";
			film->mon.female = 1 - flags.female;
			film->autographed = AUTOGRAPH_LAST_NIGHT;
			break;
		}
		film->mnamelen = strlen(name);
		Strcpy(PHOTO_MNAME(film),name);
		film->special_name = 1;
		break;
	case PHOTO_SPECIAL_WIZ:
		film->range = 1;
		film->mon_re_obj = PHOTO_NO_OBJ;
		film->subject_type = PHOTO_MON;
		film->feature = ROOM;
		film->mon.montype = PM_WIZARD_OF_YENDOR;
		film->mon.cansee = 1;
		film->mon.canmove = 1;
		film->autographed = AUTOGRAPH_BACK;
		break;
	default:
		break;
	}

	film->mnamelen = strlen(pbuff.mname);
	film->onamelen = strlen(pbuff.oname);
	if(pbuff.oname[0])
		memmove(pbuff.mname+film->mnamelen+1,pbuff.oname,film->onamelen+1);
	size = sizeof(struct photograph)+film->mnamelen+film->onamelen+2;
	photo = mksobj(SCR_PHOTOGRAPH,FALSE,FALSE);
	return realloc_obj(photo,size,film,0,0);
}

/* use obj->corpsenm to store direction and flash for timer shots*/
#define ENCODE_CAM(flash,dx,dy,dz)	(((dx)+1)<<4|((dy)+1)<<2|((dz)+1)|((flash)?0x80:0))
#define DECODE_DX(i)	(((0x30&(i))>>4)-1)
#define DECODE_DY(i)	(((0xC&(i))>>2)-1)
#define DECODE_DZ(i)	((3&(i))-1)
#define DECODE_FLASH(i)	((i)&0x80?TRUE:FALSE)

static const char tools[] = { TOOL_CLASS, 0 };

int
use_camera_timer()
{
	boolean flash;
	char ch;
	char buf[BUFSZ];
	long turns;
 	struct obj* camera = getobj(tools, "set the timer on");

	if(!camera)	return 0;

	if (camera->otyp != EXPENSIVE_CAMERA) {
		pline("Sorry, I don't know about any timers on that.");
		return 0;
	}

	if(camera->timed > 0) {
		You("turn off the timer on %s.", the(xname(camera)));
		obj_stop_timers(camera);
	} else {
		if (camera->spe <= 0) {
			pline("%s is out of film.", The(xname(camera)));
			return 1;
		}

		You("set %s's timer.",the(xname(camera)));
		if(!getdir("Aim it in which direction?")) return 0;

		ch = ynq("Set the flash attachment?");
		if(ch=='q')	return 0;
		flash = ch=='y';

		getlin("Delay for how many turns?", buf);
		if (sscanf(buf, "%ld", &turns) != 1) turns = 1L;
		if(turns<1)	
			turns = 1;
		if(turns>PHOTO_MAX_TIMER)
			turns = PHOTO_MAX_TIMER;

		camera->corpsenm = ENCODE_CAM(flash,u.dx,u.dy,u.dz);
		(void)start_timer(turns, TIMER_OBJECT, TAKE_PICTURE, (genericptr_t)camera);
	}
	return 1;
}

void
take_picture(arg,timeout)
genericptr_t arg;
long timeout;
{
	struct obj* camera = (struct obj*)arg;
	int dx,dy,dz;
	boolean flash;

	dx = DECODE_DX(camera->corpsenm);
	dy = DECODE_DY(camera->corpsenm);
	dz = DECODE_DZ(camera->corpsenm);
	flash = DECODE_FLASH(camera->corpsenm);
	take_picture2(camera,dx,dy,dz,flash,TRUE);
}

#undef ENCODE_DIR
#undef DECODE_DX
#undef DECODE_DY
#undef DECODE_DZ
#undef DECODE_FLASH

void
take_picture2(camera,dx,dy,dz,flash,timer_invoked)
struct obj* camera;
int dx,dy,dz;
boolean flash,timer_invoked;
{
	struct monst *mtmp = (struct monst*)0;
	struct obj *photo,*otmp;
	uchar x,y;
	boolean engulfed,inpit,usee;
	struct trap* ttmp;
	int where,i;
	int skill;

	consume_obj_charge(camera,!timer_invoked);

	where = camera->where;

	/* get camera's coordinates */
	otmp = camera;
	/* get outermost location */
	while(otmp && otmp->where==OBJ_CONTAINED)
		otmp=otmp->ocontainer;

	switch(otmp->where) {
	case OBJ_FLOOR:
		x = otmp->ox;
		y = otmp->oy;
		ttmp = t_at(x,y);
		inpit = ttmp && (ttmp->ttyp==PIT || ttmp->ttyp==SPIKED_PIT);
		break;
	case OBJ_INVENT:
		x = u.ux;
		y = u.uy;
		ttmp = t_at(x,y);
		inpit = ttmp && (ttmp->ttyp==PIT || ttmp->ttyp==SPIKED_PIT) && u.utrap;
		break;
	case OBJ_MINVENT:
		mtmp = otmp->ocarry;
		x = mtmp->mx;
		y = mtmp->my;
		ttmp = t_at(x,y);
		inpit = ttmp && (ttmp->ttyp==PIT || ttmp->ttyp==SPIKED_PIT) && mtmp->mtrapped;
		break;
	case OBJ_MIGRATING:
	case OBJ_BURIED:
	default:
		return;
	}

	/* see if camera is being used underwater */
	if((where==OBJ_FLOOR && is_pool(x,y))
		|| (where==OBJ_INVENT && Underwater)
		|| (where==OBJ_MINVENT && is_pool(x,y) && !is_flyer(camera->ocarry->data)))
	{
		/* "Using your camera underwater would void the warranty." */
		if(distu(x,y)<=PHOTO_MAX_RANGE*PHOTO_MAX_RANGE)
			You_hear("a strangled clicking sound.");
		water_damage(camera,TRUE,FALSE);
		camera->spe=0;
		return;
	}

	/* see if camera is inside an engulfer */
	engulfed = FALSE;
	if(where==OBJ_MINVENT) {
		for(i=0;i<NATTK;i++) {
			if(camera->ocarry->data->mattk[i].aatyp == AT_ENGL) {
				engulfed = TRUE;
				break;	
			}
		}
	}
		
	if (camera->cursed && !rn2(2)) {
		if(where==OBJ_INVENT) {
			dx = dy = dz = 0;
			flash = TRUE;
		}
		else {
			dx = rn2(3)-1;
			dy = rn2(3)-1;
			flash = rn2(2);
			dz = 0;
		}
	}

	usee = where!=OBJ_CONTAINED && !engulfed && cansee(x,y) &&
		(!inpit || (x==u.ux && y==u.uy)) && !u.usleep;

	if(timer_invoked) {
		if(usee && flash)
			You("see a flash from %s.",the(xname(camera)));
		else if(distu(x,y)<=PHOTO_MAX_RANGE*PHOTO_MAX_RANGE)
			You_hear("a %sclick.", where==OBJ_CONTAINED || engulfed ? "muffled ":"");
	}

	/* do flash */
	if(flash && where!=OBJ_CONTAINED && !engulfed) {
		if(dx==0 && dy==0) {
			if(dz==0) {
				if(where==OBJ_INVENT)
					mtmp = &youmonst;
				else if(where==OBJ_MINVENT)
					mtmp = camera->ocarry;
			}
			else if(dz<0 && where==OBJ_FLOOR) {
				if(u.ux==x && u.uy==y)
					mtmp = u.uswallow ? u.ustuck : &youmonst;
				else
					mtmp = m_at(x,y);
			}
		} else if(!(where==OBJ_INVENT && u.uswallow) && !inpit) {
			mtmp = flash_beam(x,y,dx,dy,PHOTO_MAX_RANGE,camera);
		}
	}

	/* make photograph */
	photo = mksobj(SCR_PHOTOGRAPH,FALSE,FALSE);
	photo->cursed = camera->cursed;
	photo->blessed = camera->blessed;
#ifdef INVISIBLE_OBJECTS
	/* invisible film doesn't get exposed */
	if(!camera->oinvis) 
#endif
	{
	if(flash && (
		where==OBJ_CONTAINED
		|| (u.uswallow && where==OBJ_INVENT && (dx || dy || dz))
		|| engulfed)) {
		/* film inside of something */
		struct photo_buff pbuff;
		struct photograph* film;

		memset(&pbuff,0,sizeof(struct photo_buff));
		film = &pbuff.film;

		if(where==OBJ_CONTAINED) {
			film->subject_type = PHOTO_OBJ_INT;
			film_obj(&film->obj,camera->ocontainer,pbuff.mname+1);
		} else {
			film->subject_type = PHOTO_MON_INT;
			film_mon(&film->mon,engulfed?camera->ocarry:u.ustuck,pbuff.mname);
		}
		photo = realloc_obj(photo,sizeof(struct photograph)+film->onamelen+2,film,0,0);
	} else if(inpit && (dx || dy) && (flash || level.locations[x][y].lit)) {
		/* film just the pit */
		struct photograph* film;
		photo = realloc_obj(photo,sizeof(struct photograph)+2,0,0,0);
		film = PHOTOGRAPH(photo);
		memset(film,0,sizeof(struct photograph)+2);
		film->subject_type = PHOTO_FEATURE;
		film->feature = PHOTO_TRAP_MASK | ttmp->ttyp;
	} else {
		photo = expose_film(photo,camera,x,y,dx,dy,dz,PHOTO_MAX_RANGE,flash,where);
	}

	/* Tourists are better photographers */
	/* 5lo: Geeks are pretty decent too */
	skill = ACURR(A_DEX) + (Role_if(PM_TOURIST) ? 4:
			       (Role_if(PM_GEEK) ? 2: 0)) 
		- (photo->cursed ? 8:0) + (photo->blessed ? 4:0);
	if(!timer_invoked)
		skill -= (Confusion ? 4:0)+(Stunned ? 4:0);

	if(rnl(20)>=skill)
		PHOTOGRAPH(photo)->out_of_focus = 1;
	if(rnl(20)>=skill)
		PHOTOGRAPH(photo)->frame_prob = 1;

	}

	/* zap victim 
	   Do it now, rather than before making photo 
	   so that subject isn't blinded until photo is done */
	if(mtmp) (void) flash_hits_mon(mtmp, camera);

	/* we need to recalculate usee since the hero may have been blinded by the flash */
	usee = usee && !Blind;
	if(where==OBJ_INVENT || usee)
		pline_The("camera emits %s.", Blind ? something: "a photograph");

	/* stick the photo somewhere */
	switch(where) {
	case OBJ_INVENT:
		(void)hold_another_object(photo,
			(u.uswallow || Is_airlevel(&u.uz) ||
			 u.uinwater || Is_waterlevel(&u.uz)) ?
				   "Oops!  It slips away from you!" :
				   "Oops!  It slips to the floor!",
				   (const char *)0,(const char *)0);
		break;
	case OBJ_CONTAINED:
		(void)add_to_container(camera->ocontainer,photo);
		break;
	case OBJ_MINVENT:
		(void)add_to_minv(camera->ocarry,photo);
		break;
	default:
		place_object(photo,x,y);
	}
}

/* Derived from bhit in zap.c
bhit doesn't work for cameras anymore, mostly because
it assumes that the hero is holding the camera */
struct monst* 
flash_beam(sx,sy,ddx,ddy,range,obj)
int sx,sy,ddx,ddy,range;
struct obj* obj;
{
	struct monst *mtmp;
	uchar typ;

	bhitpos.x = sx;
	bhitpos.y = sy;

    tmp_at(DISP_BEAM, cmap_to_glyph(S_flashbeam));

	while(range-- > 0) {
	    int x,y;

	    bhitpos.x += ddx;
	    bhitpos.y += ddy;
	    x = bhitpos.x; y = bhitpos.y;

	    if(!isok(x, y)) {
		bhitpos.x -= ddx;
		bhitpos.y -= ddy;
		break;
	    }

		typ = levl[bhitpos.x][bhitpos.y].typ;
		
		mtmp = bhitpos.x==u.ux && bhitpos.y==u.uy && u.uswallow ?
			u.ustuck :
			m_at(bhitpos.x, bhitpos.y);
	    if (mtmp != 0) {
			notonhead = (bhitpos.x != mtmp->mx || bhitpos.y != mtmp->my);
		    if (mtmp->minvis) {
				obj->ox = sx,  obj->oy = sy;
				(void) flash_hits_mon(mtmp, obj);
		    } else {
				tmp_at(DISP_END, 0);
		    	return(mtmp); 	/* caller will call flash_hits_mon */
		    }
		
	    } else if(bhitpos.x==u.ux && bhitpos.y==u.uy) {
			if(Invisible) {
				(void) flash_hits_mon(&youmonst,obj);
			} else {
				tmp_at(DISP_END, 0);
				return(&youmonst);
			}
		}

	    if(!ZAP_POS(typ) || closed_door(bhitpos.x, bhitpos.y)) {
		bhitpos.x -= ddx;
		bhitpos.y -= ddy;
		break;
	    }
	    
		/* 'I' present but no monster: erase */
		/* do this before the tmp_at() */
		if (memory_is_invisible(bhitpos.x, bhitpos.y) 
			&& cansee(x, y)) {
		    unmap_object(bhitpos.x, bhitpos.y);
		    newsym(x, y);
		}
		tmp_at(bhitpos.x, bhitpos.y);
		delay_output();	    
	}

	tmp_at(DISP_END, 0);

	return (struct monst *)0;
}
#endif
