/*	SCCS Id: @(#)patchlevel.h	3.4	2003/12/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*The name of the compiled game- should be same as stuff in makefile*/
/*for makedefs*/
/* KMH -- Made it mixed case, from which upper & lower case versions are made */
#define DEF_GAME_NAME   "SlashTHEM"
/*#define DEF_GAME_NAME   "NetHack"*/

/* Version */
#define VERSION_MAJOR   0
#define VERSION_MINOR   9
/*
 * PATCHLEVEL is updated for each release.
 */
#define PATCHLEVEL      7
/*#define EDITLEVEL	0*/
/*#define FIXLEVEL        0*/

#define COPYRIGHT_BANNER_A \
"This is SuperLotsaAddedStuffHack-The Heavily Extended Mod 2014-2023"

#define COPYRIGHT_BANNER_B \
"NetHack, Copyright 1985-2023 Stichting Mathematisch Centrum, M. Stephenson."

#define COPYRIGHT_BANNER_C \
"Slash'EM by the Slash'EM Dev Team.  Slash'EM Extended by AmyBSOD at Github."

#define COPYRIGHT_BANNER_D \
"SlashTHEM by Soviet5lo at Github.  See license for details."

#if 0
/*
 * If two or more successive releases have compatible data files, define
 * this with the version number of the oldest such release so that the
 * new release will accept old save and bones files.  The format is
 *	0xMMmmPPeeL
 * 0x = literal prefix "0x", MM = major version, mm = minor version,
 * PP = patch level, ee = edit level, L = literal suffix "L",
 * with all four numbers specified as two hexadecimal digits.
 */
#define VERSION_COMPATIBILITY 0x00040100L
#endif

/*patchlevel.h*/
