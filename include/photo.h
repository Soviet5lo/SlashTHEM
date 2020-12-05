/*	SCCS Id: @(#)photo.h	3.4	2003/01/17	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */
#ifdef NEPHI_PHOTOGRAPHY
#ifndef PHOTOGRAPHY_H
#define PHOTOGRAPHY_H

struct obj_photo {
	short type;
	char artifact;
	long quan;
	int corpsenm;
	Bitfield(eroded,2);
	Bitfield(eroded2,2);
	Bitfield(greased,1);
};

struct mon_photo {
	short montype;
	Bitfield(female,1);
	Bitfield(flee,1);
	Bitfield(cansee,1);
	Bitfield(canmove,1);	/* paralysis */
	Bitfield(sleeping,1);
	Bitfield(stun,1);
	Bitfield(conf,1);
	Bitfield(peaceful,1);
	Bitfield(trapped,1);	/* trapped in a pit or bear trap */
	Bitfield(leashed,1);
	Bitfield(flying,1);
};

struct photograph {
	d_level location;
	char range;
	xchar special_room;

	Bitfield(out_of_focus,1);
	Bitfield(frame_prob,1);
	Bitfield(autographed,3);
#define NO_AUTOGRAPH			0
#define AUTOGRAPH_THANKS		1
#define AUTOGRAPH_FAN			2
#define AUTOGRAPH_LAST_NIGHT	3
#define AUTOGRAPH_MAUD			4
#define AUTOGRAPH_BACK			5
#define AUTOGRAPH_GOOD_LUCK		6
#define AUTOGRAPH_COWARD		7
	Bitfield(mon_re_obj,3);
#define PHOTO_NO_OBJ	0
#define PHOTO_AT_FEET	1
#define PHOTO_WIELD		2
#define PHOTO_WEAR		3
#define PHOTO_CARRY		4

#define ph_engr_type	mon_re_obj

	Bitfield(subject_type,3);
/* these are ordered by interestingness */
#define PHOTO_INVALID	0
#define PHOTO_CEILING	1
#define PHOTO_SCENIC	2
#define PHOTO_FEATURE	3
#define PHOTO_OBJ		4
#define PHOTO_MON		5
#define PHOTO_OBJ_INT	6
#define PHOTO_MON_INT	7
	
	Bitfield(special_name,1);

	struct mon_photo mon;
	struct obj_photo obj;
	uchar feature;
	char mnamelen;
#define ph_engrlen	mnamelen
	char onamelen;
#define PHOTO_TRAP_MASK	0x80
#define PHOTO_CEILING_CEIL	0
#define PHOTO_CEILING_ROCK	1
#define PHOTO_CEILING_AIR	2
#define PHOTO_CEILING_WATER	3
#define PHOTO_FLOOR			MAX_TYPE
};

#define PHOTOGRAPH(x)	((struct photograph *)((x)->oxlth ? (&(x)->oextra[0]):0))
#define PHOTO_CLOSE_RANGE	2
#define PHOTO_LONG_RANGE	6
#define PHOTO_MAX_RANGE		9
#define BLUR_FACTOR(x)	((x)->range + ((x)->out_of_focus ? 3:0))

#define PHOTO_MNAME(x)	(((char*)(x))+sizeof(struct photograph))
#define PHOTO_ONAME(x)	(((char*)(x))+sizeof(struct photograph)+(x)->mnamelen+1)
#define PHOTO_ENGR(x)	PHOTO_MNAME(x)

#define PHOTO_SPECIAL_FAM	1
#define PHOTO_SPECIAL_WIZ	2

#define PHOTO_MAX_TIMER		20

#endif /* PHOTOGRAPHY_H */
#endif
