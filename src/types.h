/* File: types.h */

/* Purpose: global type declarations */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */


/*
 * This file should ONLY be included by "angband.h"
 */


/*
 * Note that "char" may or may not be signed, and that "signed char"
 * may or may not work on all machines.  So always use "s16b" or "s32b"
 * for signed values.  Also, note that unsigned values cause math problems
 * in many cases, so try to only use "u16b" and "u32b" for "bit flags",
 * unless you really need the extra bit of information, or you really
 * need to restrict yourself to a single byte for storage reasons.
 *
 * Also, if possible, attempt to restrict yourself to sub-fields of
 * known size (use "s16b" or "s32b" instead of "int", and "byte" instead
 * of "bool"), and attempt to align all fields along four-byte words, to
 * optimize storage issues on 32-bit machines.  Also, avoid "bit flags"
 * since these increase the code size and slow down execution.  When
 * you need to store bit flags, use one byte per flag, or, where space
 * is an issue, use a "byte" or "u16b" or "u32b", and add special code
 * to access the various bit flags.
 *
 * Many of these structures were developed to reduce the number of global
 * variables, facilitate structured program design, allow the use of ascii
 * template files, simplify access to indexed data, or facilitate efficient
 * clearing of many variables at once.
 *
 * Certain data is saved in multiple places for efficient access, currently,
 * this includes the tval/sval/weight fields in "object_type", various fields
 * in "header_type", and the "m_idx" and "o_idx" fields in "cave_type".  All
 * of these could be removed, but this would, in general, slow down the game
 * and increase the complexity of the code.
 */





/*
 * Template file header information (see "init.c").  16 bytes.
 *
 * Note that the sizes of many of the "arrays" are between 32768 and
 * 65535, and so we must use "unsigned" values to hold the "sizes" of
 * these arrays below.  Normally, I try to avoid using unsigned values,
 * since they can cause all sorts of bizarre problems, but I have no
 * choice here, at least, until the "race" array is split into "normal"
 * and "unique" monsters, which may or may not actually help.
 *
 * Note that, on some machines, for example, the Macintosh, the standard
 * "read()" and "write()" functions cannot handle more than 32767 bytes
 * at one time, so we need replacement functions, see "util.c" for details.
 *
 * Note that, on some machines, for example, the Macintosh, the standard
 * "malloc()" function cannot handle more than 32767 bytes at one time,
 * but we may assume that the "ralloc()" function can handle up to 65535
 * butes at one time.  We should not, however, assume that the "ralloc()"
 * function can handle more than 65536 bytes at a time, since this might
 * result in segmentation problems on certain older machines, and in fact,
 * we should not assume that it can handle exactly 65536 bytes at a time,
 * since the internal functions may use an unsigned short to specify size.
 *
 * In general, these problems occur only on machines (such as most personal
 * computers) which use 2 byte "int" values, and which use "int" for the
 * arguments to the relevent functions.
 */

typedef struct header header;

struct header
{
	byte	v_major;		/* Version -- major */
	byte	v_minor;		/* Version -- minor */
	byte	v_patch;		/* Version -- patch */
	byte	v_extra;		/* Version -- extra */


	u16b	info_num;		/* Number of "info" records */

	u16b	info_len;		/* Size of each "info" record */


	u32b	head_size;		/* Size of the "header" in bytes */

	u32b	info_size;		/* Size of the "info" array in bytes */

	u32b	name_size;		/* Size of the "name" array in bytes */

	u32b	text_size;		/* Size of the "text" array in bytes */
};


/*
 * "Themed" objects.
 * Probability in percent for each class of objects to be dropped.
 * This could perhaps be an array - but that wouldn't be as clear.
 */
typedef struct obj_theme obj_theme;
struct obj_theme
{
	byte treasure;
	byte combat;
	byte magic;
	byte tools;
};


/*
 * Information about terrain "features"
 */

typedef struct feature_type feature_type;

struct feature_type
{
	u32b name;              /* Name (offset) */
	u32b text;              /* Text (offset) */
	u32b tunnel;            /* Text for tunneling */
	u32b block;             /* Text for blocking */

	byte mimic;             /* Feature to mimic */

	u32b flags1;            /* First set of flags */

	byte extra;             /* Extra byte (unused) */

	s16b unused;		/* Extra bytes (unused) */

	byte d_attr;		/* Default feature attribute */
	char d_char;		/* Default feature character */


	byte x_attr;		/* Desired feature attribute */
	char x_char;		/* Desired feature character */

	byte shimmer[7];        /* Shimmer colors */

	int d_dice[4];                  /* Number of dices */
	int d_side[4];                  /* Number of sides */
	int d_frequency[4];             /* Frequency of damage (1 is the minimum) */
	int d_type[4];                  /* Type of damage */
};


/*
 * Information about object "kinds", including player knowledge.
 *
 * Only "aware" and "tried" are saved in the savefile
 */

typedef struct object_kind object_kind;

struct object_kind
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Object type */
	byte sval;			/* Object sub type */

	s32b pval;                      /* Object extra info */
	s32b pval2;                     /* Object extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b activate;		        /* Activation number */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage dice/sides */

	s32b weight;            /* Weight */

	s32b cost;			/* Object "base cost" */

	u32b flags1;		/* Flags, set 1 */
	u32b flags2;		/* Flags, set 2 */
	u32b flags3;		/* Flags, set 3 */
	u32b flags4;            /* Flags, set 4 */
	u32b flags5;            /* Flags, set 5 */
	u32b flags6;            /* Flags, set 6 */
	u32b flags7;            /* Flags, set 7 */
	u32b flags8;            /* Flags, set 8 */
	u32b flags9;            /* Flags, set 9 */
	u32b flags10;           /* Flags, set 10 */

	u32b oflags1;		/* Obvious Flags, set 1 */
	u32b oflags2;		/* Obvious Flags, set 2 */
	u32b oflags3;		/* Obvious Flags, set 3 */
	u32b oflags4;           /* Obvious Flags, set 4 */
	u32b oflags5;           /* Obvious Flags, set 5 */
	u32b oflags6;           /* Obvious Flags, set 6 */
	u32b oflags7;           /* Obvious Flags, set 7 */
	u32b oflags8;           /* Obvious Flags, set 8 */
	u32b oflags9;           /* Obvious Flags, set 9 */
	u32b oflags10;          /* Obvious Flags, set 10 */

	byte locale[10];		/* Allocation level(s) */
	s32b chance[10];		/* Allocation chance(s) */

	byte level;			/* Level */
	byte extra;			/* Something */


	byte d_attr;		/* Default object attribute */
	char d_char;		/* Default object character */


	byte x_attr;		/* Desired object attribute */
	char x_char;		/* Desired object character */


	byte flavor;			/* Special object flavor (or zero) */

	bool easy_know;		/* This object is always known (if aware) */


	bool aware;			/* The player is "aware" of the item's effects */

	bool tried;			/* The player has "tried" one of the items */

	bool know;                      /* extractable flag for the alchemist */

	u32b esp;                       /* ESP flags */
	u32b oesp;                      /* Obvious ESP flags */

	byte btval;                     /* Become Object type */
	byte bsval;                     /* Become Object sub type */
	bool artifact;                  /* Is it a normal artifact(already generated) */

	s16b power;                     /* Power granted(if any) */
};



/*
 * Information about "artifacts".
 *
 * Note that the save-file only writes "cur_num" to the savefile.
 *
 * Note that "max_num" is always "1" (if that artifact "exists")
 */

typedef struct artifact_type artifact_type;

struct artifact_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Artifact type */
	byte sval;			/* Artifact sub type */

	s16b pval;			/* Artifact extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b activate;			/* Activation Number */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage when hits */

	s16b weight;		/* Weight */

	s32b cost;			/* Artifact "cost" */

	u32b flags1;		/* Artifact Flags, set 1 */
	u32b flags2;		/* Artifact Flags, set 2 */
	u32b flags3;		/* Artifact Flags, set 3 */
	u32b flags4;            /* Artifact Flags, set 4 */
	u32b flags5;            /* Artifact Flags, set 5 */
	u32b flags6;            /* Artifact Flags, set 6 */
	u32b flags7;            /* Artifact Flags, set 7 */
	u32b flags8;            /* Artifact Flags, set 8 */
	u32b flags9;            /* Artifact Flags, set 9 */
	u32b flags10;           /* Artifact Flags, set 10 */

	u32b oflags1;		/* Obvious Flags, set 1 */
	u32b oflags2;		/* Obvious Flags, set 2 */
	u32b oflags3;		/* Obvious Flags, set 3 */
	u32b oflags4;           /* Obvious Flags, set 4 */
	u32b oflags5;           /* Obvious Flags, set 5 */
	u32b oflags6;           /* Obvious Flags, set 6 */
	u32b oflags7;           /* Obvious Flags, set 7 */
	u32b oflags8;           /* Obvious Flags, set 8 */
	u32b oflags9;           /* Obvious Flags, set 9 */
	u32b oflags10;          /* Obvious Flags, set 10 */

	byte level;			/* Artifact level */
	byte rarity;		/* Artifact rarity */

	byte cur_num;		/* Number created (0 or 1) */
	byte max_num;		/* Unused (should be "1") */

	u32b esp;                       /* ESP flags */
	u32b oesp;                       /* ESP flags */

	s16b power;                     /* Power granted(if any) */

	s16b set;               /* Does it belongs to a set ?*/
};


/*
 * Information about "ego-items".
 */

typedef struct ego_item_type ego_item_type;

struct ego_item_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	bool before;                    /* Before or after the object name ? */

	byte tval[20];
	byte min_sval[20];
	byte max_sval[20];

	byte rating;		/* Rating boost */

	byte level;			/* Minimum level */
	s32b rarity;            /* Object rarity */
	s32b mrarity;           /* Object rarity */

	s16b max_to_h;          /* Maximum to-hit bonus */
	s16b max_to_d;          /* Maximum to-dam bonus */
	s16b max_to_a;          /* Maximum to-ac bonus */

	s16b activate;			/* Activation Number */

	s32b max_pval;          /* Maximum pval */

	s32b cost;			/* Ego-item "cost" */

	byte rar[5];
	u32b flags1[5];            /* Ego-Item Flags, set 1 */
	u32b flags2[5];            /* Ego-Item Flags, set 2 */
	u32b flags3[5];            /* Ego-Item Flags, set 3 */
	u32b flags4[5];            /* Ego-Item Flags, set 4 */
	u32b flags5[5];            /* Ego-Item Flags, set 5 */
	u32b flags6[5];            /* Ego-Item Flags, set 6 */
	u32b flags7[5];            /* Ego-Item Flags, set 7 */
	u32b flags8[5];            /* Ego-Item Flags, set 8 */
	u32b flags9[5];            /* Ego-Item Flags, set 9 */
	u32b flags10[5];           /* Ego-Item Flags, set 10 */
	u32b esp[5];                       /* ESP flags */
	u32b oflags1[5];           /* Ego-Item Obvious Flags, set 1 */
	u32b oflags2[5];           /* Ego-Item Obvious Flags, set 2 */
	u32b oflags3[5];           /* Ego-Item Obvious Flags, set 3 */
	u32b oflags4[5];           /* Ego-Item Obvious Flags, set 4 */
	u32b oflags5[5];           /* Ego-Item Obvious Flags, set 5 */
	u32b oflags6[5];           /* Ego-Item Obvious Flags, set 6 */
	u32b oflags7[5];           /* Ego-Item Obvious Flags, set 7 */
	u32b oflags8[5];           /* Ego-Item Obvious Flags, set 8 */
	u32b oflags9[5];           /* Ego-Item Obvious Flags, set 9 */
	u32b oflags10[5];          /* Ego-Item Obvious Flags, set 10 */
	u32b oesp[5];              /* Obvious ESP flags */
	u32b fego[5];              /* ego flags */

	u32b need_flags1;            /* Ego-Item Flags, set 1 */
	u32b need_flags2;            /* Ego-Item Flags, set 2 */
	u32b need_flags3;            /* Ego-Item Flags, set 3 */
	u32b need_flags4;            /* Ego-Item Flags, set 4 */
	u32b need_flags5;            /* Ego-Item Flags, set 5 */
	u32b need_flags6;            /* Ego-Item Flags, set 6 */
	u32b need_flags7;            /* Ego-Item Flags, set 7 */
	u32b need_flags8;            /* Ego-Item Flags, set 8 */
	u32b need_flags9;            /* Ego-Item Flags, set 9 */
	u32b need_flags10;           /* Ego-Item Flags, set 10 */
	u32b need_esp;                       /* ESP flags */
	u32b forbid_flags1;            /* Ego-Item Flags, set 1 */
	u32b forbid_flags2;            /* Ego-Item Flags, set 2 */
	u32b forbid_flags3;            /* Ego-Item Flags, set 3 */
	u32b forbid_flags4;            /* Ego-Item Flags, set 4 */
	u32b forbid_flags5;            /* Ego-Item Flags, set 5 */
	u32b forbid_flags6;            /* Ego-Item Flags, set 6 */
	u32b forbid_flags7;            /* Ego-Item Flags, set 7 */
	u32b forbid_flags8;            /* Ego-Item Flags, set 8 */
	u32b forbid_flags9;            /* Ego-Item Flags, set 9 */
	u32b forbid_flags10;           /* Ego-Item Flags, set 10 */
	u32b forbid_esp;                       /* ESP flags */

	s16b power;                     /* Power granted(if any) */
};


/*
 * Information about "random artifacts parts".
 */
typedef struct randart_part_type randart_part_type;
struct randart_part_type
{
	byte tval[30];
	byte min_sval[30];
	byte max_sval[30];

	byte level;             /* Minimum level */
	s32b rarity;            /* Object rarity */
	s32b mrarity;           /* Object rarity */

	s16b max_to_h;          /* Maximum to-hit bonus */
	s16b max_to_d;          /* Maximum to-dam bonus */
	s16b max_to_a;          /* Maximum to-ac bonus */

	s32b max_pval;          /* Maximum pval */

	s32b value;             /* power value */
	s16b max;               /* Number of time it can appear on a single item */

	u32b flags1;            /* Ego-Item Flags, set 1 */
	u32b flags2;            /* Ego-Item Flags, set 2 */
	u32b flags3;            /* Ego-Item Flags, set 3 */
	u32b flags4;            /* Ego-Item Flags, set 4 */
	u32b flags5;            /* Ego-Item Flags, set 5 */
	u32b flags6;            /* Ego-Item Flags, set 6 */
	u32b flags7;            /* Ego-Item Flags, set 7 */
	u32b flags8;            /* Ego-Item Flags, set 8 */
	u32b flags9;            /* Ego-Item Flags, set 9 */
	u32b flags10;           /* Ego-Item Flags, set 10 */
	u32b esp;               /* ESP flags */
	u32b fego;              /* ego flags */

	u32b aflags1;            /* Ego-Item Flags, set 1 */
	u32b aflags2;            /* Ego-Item Flags, set 2 */
	u32b aflags3;            /* Ego-Item Flags, set 3 */
	u32b aflags4;            /* Ego-Item Flags, set 4 */
	u32b aflags5;            /* Ego-Item Flags, set 5 */
	u32b aflags6;            /* Ego-Item Flags, set 6 */
	u32b aflags7;            /* Ego-Item Flags, set 7 */
	u32b aflags8;            /* Ego-Item Flags, set 8 */
	u32b aflags9;            /* Ego-Item Flags, set 9 */
	u32b aflags10;           /* Ego-Item Flags, set 10 */
	u32b aesp;               /* ESP flags */

	s16b power;             /* Power granted(if any) */
};

typedef struct randart_gen_type randart_gen_type;
struct randart_gen_type
{
	int chance;             /* Chance to have that number of powers */
	int dd;
	int ds;
	int plus;               /* xdy+plus power */
};


/*
 * Monster blow structure
 *
 *	- Method (RBM_*)
 *	- Effect (RBE_*)
 *	- Damage Dice
 *	- Damage Sides
 */

typedef struct monster_blow monster_blow;

struct monster_blow
{
	byte method;
	byte effect;
	byte d_dice;
	byte d_side;
};



/*
 * Monster "race" information, including racial memories
 *
 * Note that "d_attr" and "d_char" are used for MORE than "visual" stuff.
 *
 * Note that "x_attr" and "x_char" are used ONLY for "visual" stuff.
 *
 * Note that "cur_num" (and "max_num") represent the number of monsters
 * of the given race currently on (and allowed on) the current level.
 * This information yields the "dead" flag for Unique monsters.
 *
 * Note that "max_num" is reset when a new player is created.
 * Note that "cur_num" is reset when a new level is created.
 *
 * Note that several of these fields, related to "recall", can be
 * scrapped if space becomes an issue, resulting in less "complete"
 * monster recall (no knowledge of spells, etc).  All of the "recall"
 * fields have a special prefix to aid in searching for them.
 */


typedef struct monster_race monster_race;

struct monster_race
{
	u32b name;				/* Name (offset) */
	u32b text;                              /* Text (offset) */

	u16b hdice;				/* Creatures hit dice count */
	u16b hside;				/* Creatures hit dice sides */

	s16b ac;				/* Armour Class */

	s16b sleep;				/* Inactive counter (base) */
	byte aaf;				/* Area affect radius (1-100) */
	byte speed;				/* Speed (normally 110) */

	s32b mexp;				/* Exp value for kill */

	s32b weight;                            /* Weight of the monster */

	byte freq_inate;		/* Inate spell frequency */
	byte freq_spell;		/* Other spell frequency */

	u32b flags1;			/* Flags 1 (general) */
	u32b flags2;			/* Flags 2 (abilities) */
	u32b flags3;			/* Flags 3 (race/resist) */
	u32b flags4;			/* Flags 4 (inate/breath) */
	u32b flags5;			/* Flags 5 (normal spells) */
	u32b flags6;			/* Flags 6 (special spells) */
	u32b flags7;			/* Flags 7 (movement related abilities) */
	u32b flags8;			/* Flags 8 (wilderness info) */
	u32b flags9;			/* Flags 9 (drops info) */
	u32b flags10;			/* Flags 10 */
	u32b flags11;			/* Flags 11 */
	u32b flags12;			/* Flags 12 */
	u32b flags13;			/* Flags 13 */
	u32b flags14;			/* Flags 14 */
	u32b flags15;			/* Flags 15 */
	u32b flags16;			/* Flags 16 */
	u32b flags17;			/* Flags 17 */
	u32b flags18;			/* Flags 18 */
	u32b flags19;			/* Flags 19 */
	u32b flags20;			/* Flags 20 */

	monster_blow blow[4];           /* Up to four blows per round */

	byte body_parts[BODY_MAX];      /* To help to decide what to use when body changing */

	byte level;                     /* Level of creature */
	s32b rarity;			/* Rarity of creature */


	byte d_attr;			/* Default monster attribute */
	char d_char;			/* Default monster character */


	byte x_attr;			/* Desired monster attribute */
	char x_char;			/* Desired monster character */


	s16b max_num;                   /* Maximum population allowed per level */

	byte cur_num;			/* Monster population on current level */


	s16b r_sights;			/* Count sightings of this monster */
	s16b r_deaths;			/* Count deaths from this monster */

	s16b r_pkills;			/* Count monsters killed in this life */
	s16b r_tkills;			/* Count monsters killed in all lives */

	byte r_wake;			/* Number of times woken up (?) */
	byte r_ignore;			/* Number of times ignored (?) */

	byte r_xtra1;			/* Something (unused) */
	byte r_xtra2;			/* Something (unused) */

	byte r_drop_gold;		/* Max number of gold dropped at once */
	byte r_drop_item;		/* Max number of item dropped at once */

	byte r_cast_inate;		/* Max number of inate spells seen */
	byte r_cast_spell;		/* Max number of other spells seen */

	byte r_blows[4];		/* Number of times each blow type was seen */

	u32b r_flags1;			/* Observed racial flags */
	u32b r_flags2;			/* Observed racial flags */
	u32b r_flags3;			/* Observed racial flags */
	u32b r_flags4;			/* Observed racial flags */
	u32b r_flags5;			/* Observed racial flags */
	u32b r_flags6;			/* Observed racial flags */
	u32b r_flags7;			/* Observed racial flags */
	u32b r_flags8;                  /* Observed racial flags */
	u32b r_flags9;                  /* Observed racial flags */
	u32b r_flags10;                  /* Observed racial flags */
	u32b r_flags11;                  /* Observed racial flags */
	u32b r_flags12;                  /* Observed racial flags */
	u32b r_flags13;                  /* Observed racial flags */
	u32b r_flags14;                  /* Observed racial flags */
	u32b r_flags15;                  /* Observed racial flags */
	u32b r_flags16;                  /* Observed racial flags */
	u32b r_flags17;                  /* Observed racial flags */
	u32b r_flags18;                  /* Observed racial flags */
	u32b r_flags19;                  /* Observed racial flags */
	u32b r_flags20;                  /* Observed racial flags */

	bool on_saved;                  /* Is the (unique) on a saved level ? */

	byte total_visible;             /* Amount of this race that are visible */

	obj_theme drops;                /* The drops type */
};


typedef struct monster_ego monster_ego;

struct monster_ego
{
	u32b name;				/* Name (offset) */
	bool before;                            /* Display ego before or after */

	monster_blow blow[4];                   /* Up to four blows per round */
	byte blowm[4][2];

	s16b hdice;                             /* Creatures hit dice count */
	s16b hside;                             /* Creatures hit dice sides */

	s16b ac;				/* Armour Class */

	s16b sleep;				/* Inactive counter (base) */
	s16b aaf;                               /* Area affect radius (1-100) */
	s16b speed;                             /* Speed (normally 110) */

	s32b mexp;				/* Exp value for kill */

	s32b weight;                            /* Weight of the monster */

	byte freq_inate;                /* Inate spell frequency */
	byte freq_spell;		/* Other spell frequency */

	/* Ego flags */
	u32b flags1;                    /* Flags 1 */
	u32b flags2;                    /* Flags 1 */
	u32b flags3;                    /* Flags 1 */
	u32b flags4;                    /* Flags 1 */
	u32b flags5;                    /* Flags 1 */
	u32b flags6;                    /* Flags 1 */
	u32b flags7;                    /* Flags 1 */
	u32b flags8;                    /* Flags 1 */
	u32b flags9;                    /* Flags 1 */
	u32b flags10;                    /* Flags 1 */
	u32b flags11;                    /* Flags 1 */
	u32b flags12;                    /* Flags 1 */
	u32b flags13;                    /* Flags 1 */
	u32b flags14;                    /* Flags 1 */
	u32b flags15;                    /* Flags 1 */
	u32b flags16;                    /* Flags 1 */
	u32b flags17;                    /* Flags 1 */
	u32b flags18;                    /* Flags 1 */
	u32b flags19;                    /* Flags 1 */
	u32b flags20;                    /* Flags 1 */
	u32b hflags1;                    /* Flags 1 */
	u32b hflags2;                    /* Flags 1 */
	u32b hflags3;                    /* Flags 1 */
	u32b hflags4;                    /* Flags 1 */
	u32b hflags5;                    /* Flags 1 */
	u32b hflags6;                    /* Flags 1 */
	u32b hflags7;                    /* Flags 1 */
	u32b hflags8;                    /* Flags 1 */
	u32b hflags9;                    /* Flags 1 */
	u32b hflags10;                    /* Flags 1 */
	u32b hflags11;                    /* Flags 1 */
	u32b hflags12;                    /* Flags 1 */
	u32b hflags13;                    /* Flags 1 */
	u32b hflags14;                    /* Flags 1 */
	u32b hflags15;                    /* Flags 1 */
	u32b hflags16;                    /* Flags 1 */
	u32b hflags17;                    /* Flags 1 */
	u32b hflags18;                    /* Flags 1 */
	u32b hflags19;                    /* Flags 1 */
	u32b hflags20;                    /* Flags 1 */

	/* Monster flags */
	u32b mflags1;                    /* Flags 1 (general) */
	u32b mflags2;                    /* Flags 2 (abilities) */
	u32b mflags3;                    /* Flags 3 (race/resist) */
	u32b mflags4;                    /* Flags 4 (inate/breath) */
	u32b mflags5;                    /* Flags 5 (normal spells) */
	u32b mflags6;                    /* Flags 6 (special spells) */
	u32b mflags7;                    /* Flags 7 (movement related abilities) */
	u32b mflags8;                    /* Flags 8 (wilderness info) */
	u32b mflags9;                    /* Flags 9 (drops info) */
	u32b mflags10;                   /* Flags 10 */
	u32b mflags11;                   /* Flags 11 */
	u32b mflags12;                   /* Flags 12 */
	u32b mflags13;                   /* Flags 13 */
	u32b mflags14;                   /* Flags 14 */
	u32b mflags15;                   /* Flags 15 */
	u32b mflags16;                   /* Flags 16 */
	u32b mflags17;                   /* Flags 17 */
	u32b mflags18;                   /* Flags 18 */
	u32b mflags19;                   /* Flags 19 */
	u32b mflags20;                   /* Flags 20 */

	/* Negative Flags, to be removed from the monster flags */
	u32b nflags1;                    /* Flags 1 (general) */
	u32b nflags2;                    /* Flags 2 (abilities) */
	u32b nflags3;                    /* Flags 3 (race/resist) */
	u32b nflags4;                    /* Flags 4 (inate/breath) */
	u32b nflags5;                    /* Flags 5 (normal spells) */
	u32b nflags6;                    /* Flags 6 (special spells) */
	u32b nflags7;                    /* Flags 7 (movement related abilities) */
	u32b nflags8;                    /* Flags 8 (wilderness info) */
	u32b nflags9;                    /* Flags 9 (drops info) */
	u32b nflags10;                   /* Flags 10 */
	u32b nflags11;                   /* Flags 11 */
	u32b nflags12;                   /* Flags 12 */
	u32b nflags13;                   /* Flags 13 */
	u32b nflags14;                   /* Flags 14 */
	u32b nflags15;                   /* Flags 15 */
	u32b nflags16;                   /* Flags 16 */
	u32b nflags17;                   /* Flags 17 */
	u32b nflags18;                   /* Flags 18 */
	u32b nflags19;                   /* Flags 19 */
	u32b nflags20;                   /* Flags 20 */

	s16b level;                     /* Level of creature */
	s16b rarity;                    /* Rarity of creature */


	byte d_attr;			/* Default monster attribute */
	char d_char;			/* Default monster character */

	byte g_attr;                    /* Overlay graphic attribute */
	char g_char;                    /* Overlay graphic character */

	char r_char[10];                 /* Monster race allowed */
	char nr_char[10];                /* Monster race not allowed */
};



/*
 * Information about "vault generation"
 */

typedef struct vault_type vault_type;

struct vault_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte typ;			/* Vault type */

	byte rat;			/* Vault rating */

	byte hgt;			/* Vault height */
	byte wid;			/* Vault width */

	s16b lvl;                       /* level of special (if any) */
	byte dun_type;                  /* Dungeon type where the level will show up */

	s16b mon[10];                   /* special monster */
	int item[3];                   /* number of item (usually artifact) */
};


/* jk */
/* name and description are in some other arrays */
typedef struct trap_type trap_type;

struct trap_type
{
	s16b probability; /* probability of existence */
	s16b another;     /* does this trap easily combine */
	s16b p1valinc;     /* how much does this trap attribute to p1val */
	byte difficulty;  /* how difficult to disarm */
	byte minlevel;    /* what is the minimum level on which the traps should be */
	byte color;       /* what is the color on screen */
	u32b flags;       /* where can these traps go - and perhaps other flags */
	bool ident;       /* do we know the name */
	s16b known;       /* how well is this trap known */
	s16b name;        /* normal name like weakness */
	s16b dd, ds;      /* base damage */
	s16b text;        /* longer description once you've met this trap */
	byte g_attr;                    /* Overlay graphic attribute */
	char g_char;                    /* Overlay graphic character */
};



/*
 * A single "grid" in a Cave
 *
 * Note that several aspects of the code restrict the actual cave
 * to a max size of 256 by 256.  In partcular, locations are often
 * saved as bytes, limiting each coordinate to the 0-255 range.
 *
 * The "o_idx" and "m_idx" fields are very interesting.  There are
 * many places in the code where we need quick access to the actual
 * monster or object(s) in a given cave grid.  The easiest way to
 * do this is to simply keep the index of the monster and object
 * (if any) with the grid, but this takes 198*66*4 bytes of memory.
 * Several other methods come to mind, which require only half this
 * amound of memory, but they all seem rather complicated, and would
 * probably add enough code that the savings would be lost.  So for
 * these reasons, we simply store an index into the "o_list" and
 * "m_list" arrays, using "zero" when no monster/object is present.
 *
 * Note that "o_idx" is the index of the top object in a stack of
 * objects, using the "next_o_idx" field of objects (see below) to
 * create the singly linked list of objects.  If "o_idx" is zero
 * then there are no objects in the grid.
 *
 * Note the special fields for the "MONSTER_FLOW" code.
 */

typedef struct cave_type cave_type;

struct cave_type
{
	u16b info;		/* Hack -- cave flags */

	byte feat;		/* Hack -- feature type */

	s16b o_idx;		/* Object in this grid */

	s16b m_idx;		/* Monster in this grid */

	s16b t_idx;		/* trap index (in t_list) or zero       */

	s16b special, special2; /* Special cave info */

	s16b inscription;       /* Inscription of the grid */

	byte mana;              /* Magical energy of the grid */

	byte mimic;		/* Feature to mimic */

#ifdef MONSTER_FLOW

	byte cost;		/* Hack -- cost of flowing */
	byte when;		/* Hack -- when cost was computed */

#endif
	s16b effect;            /* The lasting effects */
};

/* Lasting spell effects(clouds, ..) */
typedef struct effect_type effect_type;
struct effect_type
{
	s16b    time;           /* For how long */
	s16b    dam;            /* How much damage */
	s16b    type;           /* Of which type */
	s16b    cy;             /* Center of the cast*/
	s16b    cx;             /* Center of the cast*/
	s16b    rad;            /* Radius -- if needed */
	u32b    flags;          /* Flags */
};

/*
 * Object information, for a specific object.
 *
 * Note that a "discount" on an item is permanent and never goes away.
 *
 * Note that inscriptions are now handled via the "quark_str()" function
 * applied to the "note" field, which will return NULL if "note" is zero.
 *
 * Note that "object" records are "copied" on a fairly regular basis,
 * and care must be taken when handling such objects.
 *
 * Note that "object flags" must now be derived from the object kind,
 * the artifact and ego-item indexes, and the two "xtra" fields.
 *
 * Each cave grid points to one (or zero) objects via the "o_idx"
 * field (above).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a "stack" of objects in the same grid.
 *
 * Each monster points to one (or zero) objects via the "hold_o_idx"
 * field (below).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a pile of objects held by the monster.
 *
 * The "held_m_idx" field is used to indicate which monster, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */

typedef struct object_type object_type;

struct object_type
{
	s16b k_idx;			/* Kind index (zero if "dead") */

	byte iy;			/* Y-position on map, or zero */
	byte ix;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	s32b pval;                      /* Item extra-parameter */
	s16b pval2;                     /* Item extra-parameter for some special
					   items*/
	s32b pval3;                     /* Item extra-parameter for some special
					   items*/

	s32b spellcontain;      /* because SOMEone thought it was a good idea to misuse pval2... --Amy */

	s32b tampered;		/* alchemy or runecraft has been used on this item --Amy */
	s32b morgycurse;		/* item has gained the AMC post-creation --Amy */
	s32b noattcurse;		/* item has gained the no attack flag post-creation --Amy */

	byte discount;		/* Discount (if any) */

	byte number;		/* Number of items */

	s32b weight;            /* Item weight */

	byte elevel;            /* Item exp level */
	s32b exp;               /* Item exp */

	s16b name1;			/* Artifact type, if any */
	s16b name2;                     /* Ego-Item type, if any */
	s16b name2b;                    /* Second Ego-Item type, if any */

	byte xtra1;			/* Extra info type */
	s16b xtra2;                     /* Extra info index */

	s16b to_h;			/* Plusses to hit */
	s16b to_d;			/* Plusses to damage */
	s16b to_a;			/* Plusses to AC */

	s16b ac;			/* Normal AC */

	byte dd, ds;		/* Damage dice/sides */

	s16b timeout;		/* Timeout Counter */

	byte ident;			/* Special flags  */

	byte marked;		/* Object is marked */

	u16b note;			/* Inscription index */
	u16b art_name;      /* Artifact name (random artifacts) */

	u32b art_flags1;        /* Flags, set 1  Alas, these were necessary */
	u32b art_flags2;        /* Flags, set 2  for the random artifacts of*/
	u32b art_flags3;        /* Flags, set 3  Zangband */
	u32b art_flags4;        /* Flags, set 4  PernAngband */
	u32b art_flags5;        /* Flags, set 5  PernAngband */
	u32b art_flags6;        /* Flags, set 6 */
	u32b art_flags7;        /* Flags, set 7 */
	u32b art_flags8;        /* Flags, set 8 */
	u32b art_flags9;        /* Flags, set 9 */
	u32b art_flags10;       /* Flags, set 10 */
	u32b art_esp;           /* Flags, set esp  PernAngband */

	u32b art_oflags1;       /* Obvious Flags, set 1 */
	u32b art_oflags2;       /* Obvious Flags, set 2 */
	u32b art_oflags3;       /* Obvious Flags, set 3 */
	u32b art_oflags4;       /* Obvious Flags, set 4 */
	u32b art_oflags5;       /* Obvious Flags, set 5 */
	u32b art_oflags6;       /* Obvious Flags, set 6 */
	u32b art_oflags7;       /* Obvious Flags, set 7 */
	u32b art_oflags8;       /* Obvious Flags, set 8 */
	u32b art_oflags9;       /* Obvious Flags, set 9 */
	u32b art_oflags10;      /* Obvious Flags, set 10 */
	u32b art_oesp;          /* Obvious Flags, set esp */

	s16b next_o_idx;	/* Next object in stack (if any) */

	s16b held_m_idx;	/* Monster holding us (if any) */

	byte sense;             /* Pseudo-id status */

	byte found;             /* How did we find it */
	s16b found_aux1;        /* Stores info for found */
	s16b found_aux2;        /* Stores info for found */
	s16b found_aux3;        /* Stores info for found */
	s16b found_aux4;        /* Stores info for found */
};


/*
 * Monster mind, use for skills and such
 */
typedef struct monster_mind monster_mind;
struct monster_mind
{
	/*
	 * Without this, bcc can't compile because it does not
	 * allow empty structure.  Remove this when you add some
	 * variables to this structure.  -- Kusunose
	 */
	byte dummy;
};



/*
 * Monster information, for a specific monster.
 *
 * Note: fy, fx constrain dungeon size to 256x256
 *
 * The "hold_o_idx" field points to the first object of a stack
 * of objects (if any) being carried by the monster (see above).
 */

typedef struct monster_type monster_type;

struct monster_type
{
	s16b r_idx;			/* Monster race index */

	u16b ego;                       /* Ego monster type */

	byte fy;			/* Y location on map */
	byte fx;			/* X location on map */

	s32b hp;			/* Current Hit points */
	s32b maxhp;			/* Max Hit points */

	monster_blow blow[4];           /* Up to four blows per round */

	byte speed;                     /* Speed (normally 110) */
	byte level;                     /* Level of creature */
	s16b ac;                        /* Armour Class */
	u32b exp;                       /* Experience */

	s16b csleep;		/* Inactive counter */

	byte mspeed;		/* Monster "speed" */
	byte energy;		/* Monster "energy" */

	byte stealdetect;		/* Monster has caught you stealing */

	byte stunned;		/* Monster is stunned */
	byte confused;		/* Monster is confused */
	byte monfear;		/* Monster is afraid */

	s16b bleeding;          /* Monster is bleeding */
	s16b poisoned;          /* Monster is poisoned */

	byte cdis;			/* Current dis from player */

	s32b mflag;			/* Extra monster flags */

	bool ml;			/* Monster is "visible" */

	s16b hold_o_idx;	/* Object being held (if any) */

#ifdef WDT_TRACK_OPTIONS

	byte ty;			/* Y location of target */
	byte tx;			/* X location of target */

	byte t_dur;			/* How long are we tracking */

	byte t_bit;			/* Up to eight bit flags */

#endif

	u32b smart;			/* Field for "smart_learn" */

	s16b status;                    /* Status(friendly, pet, companion, ..) */

	s16b target;                    /* Monster target */

	s16b possessor;                 /* Is it under the control of a possessor ? */

	monster_race *sr_ptr;           /* Does it have a specific race(not in r_info) */

	monster_mind *mind;             /* Does it have a mind? */
};




/*
 * An entry for the object/monster allocation functions
 *
 * Pass 1 is determined from allocation information
 * Pass 2 is determined from allocation restriction
 * Pass 3 is determined from allocation calculation
 */

typedef struct alloc_entry alloc_entry;

struct alloc_entry
{
	s16b index;		/* The actual index */

	byte level;		/* Base dungeon level */
	s32b prob1;		/* Probability, pass 1 */
	s32b prob2;		/* Probability, pass 2 */
	s32b prob3;		/* Probability, pass 3 */

	u16b total;		/* Unused for now */
};



/*
 * Available "options"
 *
 *	- Address of actual option variable (or NULL)
 *
 *	- Normal Value (TRUE or FALSE)
 *
 *	- Option Page Number (or zero)
 *
 *	- Savefile Set (or zero)
 *	- Savefile Bit in that set
 *
 *	- Textual name (or NULL)
 *	- Textual description
 */

typedef struct option_type option_type;

struct option_type
{
	bool	*o_var;

	byte	o_norm;

	byte	o_page;

	byte	o_bit;

	cptr	o_text;
	cptr	o_desc;
};

/*
 * A store owner
 */
typedef struct owner_type owner_type;

struct owner_type
{
	u32b name;                      /* Name (offset) */

	s16b max_cost;                  /* Purse limit */

	byte max_inflate;               /* Inflation (max) */
	byte min_inflate;               /* Inflation (min) */

	byte haggle_per;                /* Haggle unit */

	byte insult_max;                /* Insult limit */

	u32b races[2][2];                  /* Liked/hated races */
	u32b classes[2][2];                /* Liked/hated classes */

	s32b costs[3];                  /* Costs for liked people */
};




/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
typedef struct store_type store_type;

struct store_type
{
	u16b st_idx;

	u16b owner;                     /* Owner index */

	s16b insult_cur;		/* Insult counter */

	s16b investment;		/* by Amy: how much you've invested in a shop */

	s16b good_buy;			/* Number of "good" buys */
	s16b bad_buy;			/* Number of "bad" buys */

	s32b store_open;		/* Closed until this turn */

	s32b last_visit;		/* Last visited on this turn */

	byte stock_num;			/* Stock -- Number of entries */
	s16b stock_size;		/* Stock -- Total Size of Array */
	object_type *stock;		/* Stock -- Actual stock items */
};

/*
 * A store/building type
 */
typedef struct store_info_type store_info_type;

struct store_info_type
{
	u32b name;                      /* Name (offset) */

	s16b table[STORE_CHOICES][2];   /* Table -- Legal item kinds */
	byte table_num;                 /* Number of items */
	s16b max_obj;                   /* Number of items this store can hold */

	u16b owners[4];                 /* List of owners(refers to ow_info) */

	u16b actions[6];                /* Actions(refers to ba_info) */

	byte d_attr;			/* Default building attribute */
	char d_char;			/* Default building character */

	byte x_attr;			/* Desired building attribute */
	char x_char;			/* Desired building character */

	u32b flags1;                    /* Flags */
};

/*
 * Stores/buildings actions
 */
typedef struct store_action_type store_action_type;

struct store_action_type
{
	u32b name;                      /* Name (offset) */

	s32b costs[3];                  /* Costs for liked people */
	char letter;                    /* Action letter */
	char letter_aux;                /* Action letter */
	s16b action;                    /* Action code */
	s16b action_restr;              /* Action restriction */
};

/*
 * the spell function must provide the desc
 */
typedef struct magic_type magic_type;

struct magic_type
{
	byte slevel;		        /* Required level (to learn) */
	byte smana;			/* Required mana (to cast) */
	byte sfail;			/* Minimum chance of failure */
	byte sexp;			/* Encoded experience bonus */
};

/*
 * Player sex info
 */

typedef struct player_sex player_sex;

struct player_sex
{
	cptr title;			/* Type of sex */

	cptr winner;		/* Name of winner */
};


/*
 * Player racial info
 */

typedef struct player_race player_race;

struct player_race
{
	s32b title;                     /* Type of race */
	s32b desc;

	s16b r_adj[6];		/* Racial stat bonuses */

	char luck;              /* Luck */

	s16b r_dis;			/* disarming */
	s16b r_dev;			/* magic devices */
	s16b r_sav;			/* saving throw */
	s16b r_stl;			/* stealth */
	s16b r_srh;			/* search ability */
	s16b r_fos;			/* search frequency */
	s16b r_thn;			/* combat (normal) */
	s16b r_thb;			/* combat (shooting) */

	byte r_mhp;			/* Race hit-dice modifier */
	u16b r_exp;                     /* Race experience factor */

	byte b_age;			/* base age */
	byte m_age;			/* mod age */

	byte m_b_ht;		/* base height (males) */
	byte m_m_ht;		/* mod height (males) */
	byte m_b_wt;		/* base weight (males) */
	byte m_m_wt;		/* mod weight (males) */

	byte f_b_ht;		/* base height (females) */
	byte f_m_ht;		/* mod height (females)	  */
	byte f_b_wt;		/* base weight (females) */
	byte f_m_wt;		/* mod weight (females) */

	byte infra;             /* Infra-vision range */

	u32b choice[2];            /* Legal class choices */

	s16b powers[4];         /* Powers of the race */

	byte body_parts[BODY_MAX];      /* To help to decide what to use when body changing */

	s16b chart;             /* Chart history */

	u32b flags1;
	u32b flags2;            /* flags */

	u32b oflags1[PY_MAX_LEVEL + 1];
	u32b oflags2[PY_MAX_LEVEL + 1];
	u32b oflags3[PY_MAX_LEVEL + 1];
	u32b oflags4[PY_MAX_LEVEL + 1];
	u32b oflags5[PY_MAX_LEVEL + 1];
	u32b oflags6[PY_MAX_LEVEL + 1];
	u32b oflags7[PY_MAX_LEVEL + 1];
	u32b oflags8[PY_MAX_LEVEL + 1];
	u32b oflags9[PY_MAX_LEVEL + 1];
	u32b oflags10[PY_MAX_LEVEL + 1];
	u32b oesp[PY_MAX_LEVEL + 1];
	s16b opval[PY_MAX_LEVEL + 1];

	char skill_basem[MAX_SKILLS];
	u32b skill_base[MAX_SKILLS];
	char skill_modm[MAX_SKILLS];
	s16b skill_mod[MAX_SKILLS];

	s16b obj_tval[5];
	s16b obj_sval[5];
	s16b obj_pval[5];
	s16b obj_dd[5];
	s16b obj_ds[5];
	s16b obj_num;

	struct
	{
		s16b    ability;
		s16b    level;
	} abilities[10];                /* Abilitiers to be gained by level(doesnt take prereqs in account) */
};

typedef struct player_race_mod player_race_mod;

struct player_race_mod
{
	s32b title;                     /* Type of race mod */
	s32b desc;                      /* Desc */
	bool place;                     /* TRUE = race race modifier, FALSE = Race modifier race */

	s16b r_adj[6];                  /* (+) Racial stat bonuses */

	char luck;                      /* Luck */
	s16b mana;                      /* Mana % */

	s16b r_dis;                     /* (+) disarming */
	s16b r_dev;                     /* (+) magic devices */
	s16b r_sav;                     /* (+) saving throw */
	s16b r_stl;                     /* (+) stealth */
	s16b r_srh;                     /* (+) search ability */
	s16b r_fos;                     /* (+) search frequency */
	s16b r_thn;                     /* (+) combat (normal) */
	s16b r_thb;                     /* (+) combat (shooting) */

	char r_mhp;                     /* (+) Race mod hit-dice modifier */
	s16b r_exp;                     /* (+) Race mod experience factor */

	char b_age;                     /* (+) base age */
	char m_age;                     /* (+) mod age */

	char m_b_ht;            /* (+) base height (males) */
	char m_m_ht;            /* (+) mod height (males) */
	char m_b_wt;            /* (+) base weight (males) */
	char m_m_wt;            /* (+) mod weight (males) */

	char f_b_ht;            /* (+) base height (females) */
	char f_m_ht;            /* (+) mod height (females)   */
	char f_b_wt;            /* (+) base weight (females) */
	char f_m_wt;            /* (+) mod weight (females) */

	char infra;             /* (+) Infra-vision range */

	u32b choice[2];            /* Legal race choices */

	u32b pclass[2];            /* Classes allowed */
	u32b mclass[2];            /* Classes restricted */

	s16b powers[4];        /* Powers of the subrace */

	char body_parts[BODY_MAX];      /* To help to decide what to use when body changing */

	u32b flags1;
	u32b flags2;            /* flags */

	u32b oflags1[PY_MAX_LEVEL + 1];
	u32b oflags2[PY_MAX_LEVEL + 1];
	u32b oflags3[PY_MAX_LEVEL + 1];
	u32b oflags4[PY_MAX_LEVEL + 1];
	u32b oflags5[PY_MAX_LEVEL + 1];
	u32b oflags6[PY_MAX_LEVEL + 1];
	u32b oflags7[PY_MAX_LEVEL + 1];
	u32b oflags8[PY_MAX_LEVEL + 1];
	u32b oflags9[PY_MAX_LEVEL + 1];
	u32b oflags10[PY_MAX_LEVEL + 1];
	u32b oesp[PY_MAX_LEVEL + 1];
	s16b opval[PY_MAX_LEVEL + 1];

	byte g_attr;                    /* Overlay graphic attribute */
	char g_char;                    /* Overlay graphic character */

	char skill_basem[MAX_SKILLS];
	u32b skill_base[MAX_SKILLS];
	char skill_modm[MAX_SKILLS];
	s16b skill_mod[MAX_SKILLS];

	s16b obj_tval[5];
	s16b obj_sval[5];
	s16b obj_pval[5];
	s16b obj_dd[5];
	s16b obj_ds[5];
	s16b obj_num;

	struct
	{
		s16b    ability;
		s16b    level;
	} abilities[10];                /* Abilitiers to be gained by level(doesnt take prereqs in account) */
};


/*
 * Player class info
 */

typedef struct player_spec player_spec;

struct player_spec
{
	s32b title;                     /* Type of class spec */
	s32b desc;                      /* Small desc of the class spec */

	char skill_basem[MAX_SKILLS];   /* Mod for value */
	u32b skill_base[MAX_SKILLS];    /* value */
	char skill_modm[MAX_SKILLS];    /* mod for mod */
	s16b skill_mod[MAX_SKILLS];     /* mod */

	u32b skill_ideal[MAX_SKILLS];   /* Ideal skill levels at level 50 */

	s16b obj_tval[5];
	s16b obj_sval[5];
	s16b obj_pval[5];
	s16b obj_dd[5];
	s16b obj_ds[5];
	s16b obj_num;

	u32b gods;

	u32b flags1;
	u32b flags2;            /* flags */

	struct
	{
		s16b    ability;
		s16b    level;
	} abilities[10];                /* Abilitiers to be gained by level(doesnt take prereqs in account) */
};

typedef struct player_class player_class;

struct player_class
{
	s32b title;                     /* Type of class */
	s32b desc;                      /* Small desc of the class */
	s32b titles[PY_MAX_LEVEL / 5];

	s16b c_adj[6];		/* Class stat modifier */

	s16b c_dis;			/* class disarming */
	s16b c_dev;			/* class magic devices */
	s16b c_sav;			/* class saving throws */
	s16b c_stl;			/* class stealth */
	s16b c_srh;			/* class searching ability */
	s16b c_fos;			/* class searching frequency */
	s16b c_thn;			/* class to hit (normal) */
	s16b c_thb;			/* class to hit (bows) */

	s16b x_dis;			/* extra disarming */
	s16b x_dev;			/* extra magic devices */
	s16b x_sav;			/* extra saving throws */
	s16b x_stl;			/* extra stealth */
	s16b x_srh;			/* extra searching ability */
	s16b x_fos;			/* extra searching frequency */
	s16b x_thn;			/* extra to hit (normal) */
	s16b x_thb;			/* extra to hit (bows) */

	s16b c_mhp;			/* Class hit-dice adjustment */
	s16b c_exp;			/* Class experience factor */

	s16b powers[4];        /* Powers of the class */

	s16b spell_book;		/* Tval of spell books (if any) */
	s16b spell_stat;		/* Stat for spells (if any)  */
	s16b spell_lev;          /* The higher it is the higher the spells level are */
	s16b spell_fail;         /* The higher it is the higher the spells failure are */
	s16b spell_mana;         /* The higher it is the higher the spells mana are */
	s16b spell_first;        /* Level of first spell */
	s16b spell_weight;       /* Weight that hurts spells */
	byte max_spell_level;   /* Maximun spell level */
	byte magic_max_spell;  /* Maximun numbner of spells one can learn by natural means */

	u32b flags1;            /* flags */
	u32b flags2;            /* flags */

	s16b mana;
	s16b blow_num;
	s16b blow_wgt;
	s16b blow_mul;
	s16b extra_blows;

	s32b sense_base;
	s32b sense_pl;
	s32b sense_plus;
	byte sense_heavy;
	byte sense_heavy_magic;

	s16b obj_tval[5];
	s16b obj_sval[5];
	s16b obj_pval[5];
	s16b obj_dd[5];
	s16b obj_ds[5];
	s16b obj_num;

	char body_parts[BODY_MAX];      /* To help to decide what to use when body changing */

	u32b oflags1[PY_MAX_LEVEL + 1];
	u32b oflags2[PY_MAX_LEVEL + 1];
	u32b oflags3[PY_MAX_LEVEL + 1];
	u32b oflags4[PY_MAX_LEVEL + 1];
	u32b oflags5[PY_MAX_LEVEL + 1];
	u32b oflags6[PY_MAX_LEVEL + 1];
	u32b oflags7[PY_MAX_LEVEL + 1];
	u32b oflags8[PY_MAX_LEVEL + 1];
	u32b oflags9[PY_MAX_LEVEL + 1];
	u32b oflags10[PY_MAX_LEVEL + 1];
	u32b oesp[PY_MAX_LEVEL + 1];
	s16b opval[PY_MAX_LEVEL + 1];

	char skill_basem[MAX_SKILLS];
	u32b skill_base[MAX_SKILLS];
	char skill_modm[MAX_SKILLS];
	s16b skill_mod[MAX_SKILLS];

	u32b gods;

	player_spec spec[MAX_SPEC];

	struct
	{
		s16b    ability;
		s16b    level;
	} abilities[10];                /* Abilitiers to be gained by level(doesnt take prereqs in account) */
};

typedef struct meta_class_type meta_class_type;
struct meta_class_type
{
	char name[80];     /* Name */
	byte color;
	s16b *classes;          /* list of classes */
};

/* Help type */
typedef struct help_info help_info;
struct help_info
{
	bool enabled;           /* ingame help enabled */

	u32b help1;             /* help flags 1 */
};


/*
 * Most of the "player" information goes here.
 *
 * This stucture gives us a large collection of player variables.
 *
 * This structure contains several "blocks" of information.
 *   (1) the "permanent" info
 *   (2) the "variable" info
 *   (3) the "transient" info
 *
 * All of the "permanent" info, and most of the "variable" info,
 * is saved in the savefile.  The "transient" info is recomputed
 * whenever anything important changes.
 */

typedef struct player_type player_type;

struct player_type
{
	s32b lives;             /* How many times we resurected */

	s16b oldpy;		/* Previous player location -KMW- */
	s16b oldpx;		/* Previous player location -KMW- */

	s16b py;		/* Player location */
	s16b px;		/* Player location */

	byte psex;              /* Sex index */
	byte prace;             /* Race index */
	byte pracem;            /* Race Mod index */
	byte pclass;		/* Class index */
	byte pspec;		/* Class spec index */
	byte mimic_form;        /* Actualy transformation */
	s16b mimic_level;       /* Level of the mimic effect */
	byte oops;              /* Unused */

	object_type inventory[INVEN_TOTAL];     /* Player inventory */

	byte hitdie;		/* Hit dice (sides) */
	u16b expfact;           /* Experience factor */

	byte maximize;		/* Maximize stats */
	byte preserve;		/* Preserve artifacts */
	byte lvling_system;	/* Monsters level up with the dungeon's depth --Amy */
	byte hard_mode;		/* hard mode, by Amy */
	byte maso_mode;		/* masochist mode, by Amy */
	byte special;           /* Special levels */
	byte allow_one_death;   /* Blood of life */

	s16b age;			/* Characters age */
	s16b ht;			/* Height */
	s16b wt;			/* Weight */
	s16b sc;			/* Social Class */


	s32b au;			/* Current Gold */

	s32b max_exp;		/* Max experience */
	s32b exp;			/* Cur experience */
	u16b exp_frac;		/* Cur exp frac (times 2^16) */

	s16b lev;			/* Level */

	s32b overlevel;		/* for gaining more skill slots from gaining EXP */

	s16b town_num;			/* Current town number */
	s16b arena_number;		/* monster number in arena -KMW- */
	s16b inside_arena;		/* Is character inside arena? */
	s16b inside_quest;		/* Inside quest level */
	bool exit_bldg;			/* Goal obtained in arena? -KMW- */

	s32b wilderness_x;              /* Coordinates in the wilderness */
	s32b wilderness_y;
	bool wild_mode;                 /* TRUE = Small map, FLASE = Big map */
	bool old_wild_mode;             /* TRUE = Small map, FLASE = Big map */

	s16b mhp;			/* Max hit pts */
	s16b chp;			/* Cur hit pts */
	u16b chp_frac;                  /* Cur hit frac (times 2^16) */
	s16b hp_mod;                    /* A modificator(permanent) */

	s16b msp;                       /* Max mana pts */
	s16b csp;                       /* Cur mana pts */
	u16b csp_frac;          /* Cur mana frac (times 2^16) */

	s16b msane;                   /* Max sanity */
	s16b csane;                   /* Cur sanity */
	u16b csane_frac;              /* Cur sanity frac */

	s32b grace;                     /* Your God's appreciation factor. */
	byte pgod;                      /* Your God. */
	bool praying;                   /* Praying to your god. */
	s16b melkor_sacrifice;          /* How much hp has been sacrified for damage */

	s16b contamination;             /* caused by ether, weeping angels etc. */

	s16b max_plv;                   /* Max Player Level */

	s16b stat_max[6];               /* Current "maximal" stat values */
	s16b stat_cur[6];               /* Current "natural" stat values */

	s16b luck_cur;                  /* Current "natural" luck value (range -30 <> 30) */
	s16b luck_max;                  /* Current "maximal base" luck value (range -30 <> 30) */
	s16b luck_base;                 /* Current "base" luck value (range -30 <> 30) */

	s16b speed_factor;		/* Timed -- Fast */
	s16b fast;			/* Timed -- Fast */
	s16b lightspeed;                /* Timed -- Light Speed */
	s16b slow;			/* Timed -- Slow */
	s16b blind;			/* Timed -- Blindness */
	s16b paralyzed;		/* Timed -- Paralysis */
	s16b confused;		/* Timed -- Confusion */
	s16b afraid;		/* Timed -- Fear */
	s16b image;			/* Timed -- Hallucination */
	s16b poisoned;		/* Timed -- Poisoned */
	s16b cut;			/* Timed -- Cut */
	s16b stun;			/* Timed -- Stun */

	s16b protevil;          /* Timed -- Protection from Evil*/
	s16b protgood;          /* Timed -- Protection from Good*/
	s16b protundead;        /* Timed -- Protection from Undead*/
	s16b invuln;		/* Timed -- Invulnerable */
	s16b hero;			/* Timed -- Heroism */
	s16b shero;			/* Timed -- Super Heroism */
	s16b shield;		/* Timed -- Shield Spell */
	s16b shield_power;      /* Timed -- Shield Spell Power */
	s16b shield_opt;        /* Timed -- Shield Spell options */
	s16b shield_power_opt;  /* Timed -- Shield Spell Power */
	s16b shield_power_opt2; /* Timed -- Shield Spell Power */
	s16b blessed;		/* Timed -- Blessed */
	s16b tim_invis;		/* Timed -- See Invisible */
	s16b tim_infra;		/* Timed -- Infra Vision */

	s16b oppose_acid;	/* Timed -- oppose acid */
	s16b oppose_elec;	/* Timed -- oppose lightning */
	s16b oppose_fire;	/* Timed -- oppose heat */
	s16b oppose_cold;	/* Timed -- oppose cold */
	s16b oppose_pois;	/* Timed -- oppose poison */
	s16b oppose_ld;         /* Timed -- oppose light & dark */
	s16b oppose_cc;         /* Timed -- oppose chaos & confusion */
	s16b oppose_ss;         /* Timed -- oppose sound & shards */
	s16b oppose_nex;        /* Timed -- oppose nexus */

	s16b rush;              /* Rush and Bush */

	s16b tim_esp;       	/* Timed ESP */
	s16b tim_wraith;    	/* Timed wraithform */
	s16b tim_ffall;     	/* Timed Levitation */
	s16b tim_fly;       	/* Timed Levitation */
	s16b tim_fire_aura; 	/* Timed Fire Aura */
	s16b tim_poison;    	/* Timed poison hands */
	s16b tim_thunder;   	/* Timed thunderstorm */
	s16b tim_thunder_p1;	/* Timed thunderstorm */
	s16b tim_thunder_p2;	/* Timed thunderstorm */
	s16b tim_ttempest;   	/* Timed time tempest */
	s16b tim_ttempest_p1;	/* Timed time tempest */
	s16b tim_ttempest_p2;	/* Timed time tempest */

	s16b tim_peace;       	/* Timed peacekeeping */
	s16b tim_device;       	/* Timed device mastery */
	s16b tim_elemlull;      /* Timed elemental resist */
	s16b am_elemlull;       /* Bonus elemental resist */
	s16b tim_magbreath;     /* Timed magical breathing */
	s16b am_magbreath;      /* Bonus magical breathing */
	s16b tim_macarena;      /* another song that is active */
	s16b am_macarena;       /* bonus from that song */
	s16b tim_savmus;        /* another song that is active */
	s16b am_savmus;         /* bonus from that song */
	s16b tim_minersong;     /* another song that is active */
	s16b am_minersong;      /* bonus from that song */
	s16b tim_stealthmus;    /* another song that is active */
	s16b am_stealthmus;     /* bonus from that song */
	s16b tim_warboogie;     /* another song that is active */
	s16b am_warboogie;      /* bonus from that song */
	s16b tim_tankmus;       /* another song that is active */
	s16b am_tankmus;        /* bonus from that song */
	s16b tim_slaughtersong; /* another song that is active */
	s16b am_slaughtersong;  /* bonus from that song */
	s16b tim_mightymus;     /* another song that is active */
	s16b am_mightymus;      /* bonus from that song */

	s16b tim_esp_animal;    /* Timed animal ESP */
	s16b tim_bullseye;      /* Timed bullseye */
	s16b tim_sniper;        /* Timed sniper */
	s16b tim_rapidfire;     /* Timed rapidfire */
	s16b tim_bombsquad;     /* Timed bombsquad */
	s16b tim_dancing;       /* Timed dancing */
	s16b am_dancing;        /* Bonus from dancing */
	s16b tim_manavoid;      /* Timed manavoid */
	s16b tim_dragonblood_fire;      /* Timed fire immunity */
	s16b tim_dragonblood_cold;      /* Timed cold immunity */
	s16b tim_dragonblood_elec;      /* Timed shock immunity */
	s16b tim_dragonblood_acid;      /* Timed acid immunity */
	s16b tim_esp_pot;       /* Timed full ESP */
	s16b tim_levi_pot;      /* Timed levitation */
	s16b tim_jolt_fa;       /* Timed free action */
	s16b tim_boost_pot;     /* Timed boost */
	s16b tim_luck_pot;      /* Timed luck */
	s16b tim_reshi_pot;     /* Timed high resists */
	s16b tim_resrare_pot;   /* Timed super resists */
	s16b tim_air_pot;       /* Timed magical breathing */
	s16b tim_antimagic_pot; /* Timed antimagic (50%) */
	s16b tim_timeflow_pot;  /* Timed time resist */
	s16b tim_regen_pot;     /* Timed regeneration */
	s16b tim_power_pot;     /* Timed power */
	s16b tim_infra_x;       /* Timed super infravision */
	s16b tim_infra_xx;      /* Timed ultra infravision */
	s16b tim_manasilence;   /* Timed silence */
	s16b betweensickness;   /* Timer before you can use a void jumpgate again --Amy */
	s16b stairsickness;     /* Timer before you can use a stair again --Amy */
	s16b tim_repelling;     /* Timed repelling */

	s16b gained_demon;      /* You gained demonology skill from the corruption (only works once) --Amy */
	s16b gained_preserv;    /* You gained corpse-preservation from the bounty quest (only works once) --Amy */

	s16b tim_project;       /* Timed project upon melee blow */
	s16b tim_project_dam;
	s16b tim_project_gf;
	s16b tim_project_rad;
	s16b tim_project_flag;

	s16b tim_roots;         /* Timed roots */
	s16b tim_roots_ac;
	s16b tim_roots_dam;

	s16b resist_magic;  /* Timed Resist Magic (later) */
	s16b tim_invisible; /* Timed Invisibility */
	s16b tim_inv_pow;   /* Power of timed invisibility */
	s16b tim_mimic;     /* Timed Mimic */
	s16b tim_lite;      /* Timed Lite */
	s16b tim_regen;     /* Timed extra regen */
	s16b tim_regen_pow; /* Timed extra regen power */
	s16b holy;          /* Holy Aura */
	s16b walk_water;    /* Walk over water as a god */
	s16b tim_mental_barrier; /* Sustain Int&Wis */
	s16b strike;        /* True Strike(+25 hit) */
	s16b meditation;    /* Meditation(+50 mana -25 to hit/to dam) */
	s16b tim_reflect;   /* Timed Reflection */
	s16b tim_res_time;  /* Timed Resistance to Time */
	s16b tim_deadly;    /* Timed deadly blow */
	s16b prob_travel;   /* Timed probability travel */
	s16b disrupt_shield;/* Timed disruption shield */
	s16b parasite;      /* Timed parasite */
	s16b parasite_r_idx;/* Timed parasite monster */
	s32b loan;			/* Amount of loan */
	s32b loan_time;		/* Timer -- time to payback loan */
	s16b absorb_soul;   /* Timed soul absordtion */
	s16b tim_magic_breath;      /* Magical breathing -- can breath anywhere */
	s16b tim_water_breath;      /* Water breathing -- can breath underwater */

	s16b immov_cntr;    /* Timed -- Last ``immovable'' command. */

	s16b chaos_patron;

	s16b recall_dungeon;    /* Recall in which dungeon */
	s16b word_recall;	/* Word of recall counter */

	s32b energy;            /* Current energy */

	s16b food;			/* Current nutrition */

	byte confusing;		/* Glowing hands */
	byte searching;		/* Currently searching */

	s16b new_spells;	/* Number of spells available */

	s16b old_spells;

	s16b xtra_spells;       /* Number of xtra spell learned(via potion) */

	bool old_cumber_armor;
	bool old_cumber_glove;
	bool old_heavy_wield;
	bool old_heavy_shoot;
	bool old_icky_wield;

	s16b old_lite;		/* Old radius of lite (if any) */
	s16b old_view;		/* Old radius of view (if any) */

	s16b old_food_aux;	/* Old value of food */


	bool cumber_armor;	/* Mana draining armor */
	bool cumber_glove;	/* Mana draining gloves */
	bool heavy_wield;	/* Heavy weapon */
	bool heavy_shoot;	/* Heavy shooter */
	bool icky_wield;	/* Icky weapon */
	bool immovable;         /* Immovable character */

	s16b cur_lite;		/* Radius of lite (if any) */


	u32b notice;		/* Special Updates (bit flags) */
	u32b update;		/* Pending Updates (bit flags) */
	u32b redraw;		/* Normal Redraws (bit flags) */
	u32b window;		/* Window Redraws (bit flags) */

	s16b stat_use[6];	/* Current modified stats */
	s16b stat_top[6];	/* Maximal modified stats */

	s16b stat_add[6];	/* Modifiers to stat values */
	s16b stat_ind[6];	/* Indexes into stat tables */
	s16b stat_cnt[6];	/* Counter for temporary drains */
	s16b stat_los[6];	/* Amount of temporary drains */

	bool immune_acid;	/* Immunity to acid */
	bool immune_elec;	/* Immunity to lightning */
	bool immune_fire;	/* Immunity to fire */
	bool immune_cold;	/* Immunity to cold */
	bool immune_neth;       /* Immunity to nether */
	bool immune_pois;       /* Immunity to poison */

	bool resist_acid;	/* Resist acid */
	bool resist_elec;	/* Resist lightning */
	bool resist_fire;	/* Resist fire */
	bool resist_cold;	/* Resist cold */
	bool resist_pois;	/* Resist poison */

	bool resist_conf;	/* Resist confusion */
	bool resist_sound;	/* Resist sound */
	bool resist_lite;	/* Resist light */
	bool resist_dark;	/* Resist darkness */
	bool resist_chaos;	/* Resist chaos */
	bool resist_disen;	/* Resist disenchant */
	bool resist_shard;	/* Resist shards */
	bool resist_nexus;	/* Resist nexus */
	bool resist_blind;	/* Resist blindness */
	bool resist_neth;	/* Resist nether */
	bool resist_fear;	/* Resist fear */
	bool resist_continuum;  /* Resist space-time continuum disruption */

	bool rapid_hunger;      /* Hunger rapidly */

	bool resist_water;	/* Resist water */
	bool resist_inertia;	/* Resist inertia */
	bool resist_plasma;     /* Resist plasma */
	bool resist_disint;     /* Resist disintegration */
	bool resist_time;       /* Resist time */

	bool resist_nerve;	/* Resist nerve */
	bool resist_mind;	/* Resist mind */
	bool resist_ether;	/* Resist ether */

	bool sensible_fire;     /* Fire does more damage on the player */
	bool sensible_lite;     /* Lite does more damage on the player and blinds her/him */

	bool reflect;       /* Reflect 'bolt' attacks */
	bool sh_fire;       /* Fiery 'immolation' effect */
	bool sh_elec;       /* Electric 'immolation' effect */
	bool wraith_form;   /* wraithform */

	bool anti_magic;    /* Anti-magic */
	bool anti_tele;     /* Prevent teleportation */

	bool sustain_str;	/* Keep strength */
	bool sustain_int;	/* Keep intelligence */
	bool sustain_wis;	/* Keep wisdom */
	bool sustain_dex;	/* Keep dexterity */
	bool sustain_con;	/* Keep constitution */
	bool sustain_chr;	/* Keep charisma */

	bool aggravate;		/* Aggravate monsters */
	bool teleport;		/* Random teleporting */

	bool exp_drain;		/* Experience draining */
	byte drain_mana;        /* mana draining */
	byte drain_life;        /* hp draining */

	bool plr_peacekeep;     /* peacekeeping */
	bool plr_invprot;       /* inventory protection */
	bool plr_devicemast;    /* device mastery */

	bool magical_breath;    /* Magical breathing -- can breath anywhere */
	bool water_breath;      /* Water breathing -- can breath underwater */
	bool climb;             /* Can climb mountains */
	bool fly;               /* Can fly over some features */
	bool ffall;             /* No damage falling */
	bool lite;              /* Permanent light */
	bool free_act;		/* Never paralyzed */
	bool see_inv;		/* Can see invisible */
	bool regenerate;	/* Regenerate hit pts */
	bool hold_life;		/* Resist life draining */
	u32b telepathy;         /* Telepathy */
	bool slow_digest;	/* Slower digestion */
	bool bless_blade;	/* Blessed blade */
	byte xtra_might;        /* Extra might bow */
	bool impact;		/* Earthquake blows */
	bool auto_id;           /* Auto id items */

	s16b invis;             /* Invisibility */

	s16b dis_to_h;		/* Known bonus to hit */
	s16b dis_to_d;		/* Known bonus to dam */
	s16b dis_to_a;		/* Known bonus to ac */

	s16b dis_ac;		/* Known base ac */

	s16b to_l;                      /* Bonus to life */
	s16b to_m;                      /* Bonus to mana */
	s16b to_s;                      /* Bonus to spell */
	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to dam */
	s16b to_h_melee;                /* Bonus to hit for melee */
	s16b to_d_melee;                /* Bonus to dam for melee */
	s16b to_h_ranged;               /* Bonus to hit for ranged */
	s16b to_d_ranged;               /* Bonus to dam for ranged */
	s16b to_a;			/* Bonus to ac */

	s16b ac;			/* Base ac */

	byte antimagic;         /* Power of the anti magic field */
	byte antimagic_dis;     /* Radius of the anti magic field */

	s16b see_infra;		/* Infravision range */

	s16b skill_dis;		/* Skill: Disarming */
	s16b skill_dev;		/* Skill: Magic Devices */
	s16b skill_sav;		/* Skill: Saving throw */
	s16b skill_stl;		/* Skill: Stealth factor */
	s16b skill_srh;		/* Skill: Searching ability */
	s16b skill_fos;		/* Skill: Searching frequency */
	s16b skill_thn;		/* Skill: To hit (normal) */
	s16b skill_thb;		/* Skill: To hit (shooting) */
	s16b skill_tht;		/* Skill: To hit (throwing) */
	s16b skill_dig;		/* Skill: Digging */

	s16b num_blow;		/* Number of blows */
	s16b num_fire;		/* Number of shots */
	s16b xtra_crit;         /* % of increased crits */

	byte throw_mult;	/* Multiplier for throw damage */

	byte tval_xtra;		/* Correct xtra tval */

	byte tval_ammo;		/* Correct ammo tval */

	s16b pspeed;		/* Current speed */

	u32b mimic_extra;       /* Mimicry powers use that */
	u32b antimagic_extra;   /* Antimagic powers */
	u32b druid_extra;       /* Druid powers */
	u32b druid_extra2;      /* Druid powers */
	u32b druid_extra3;      /* Druid powers */
	u32b music_extra;       /* Music songs */
	u32b music_extra2;      /* Music songs */
	u32b necro_extra;       /* Necro powers */
	u32b necro_extra2;       /* Necro powers */

	u32b race_extra1;       /* Variable for race */
	u32b race_extra2;       /* Variable for race */
	u32b race_extra3;       /* Variable for race */
	u32b race_extra4;       /* Variable for race */
	u32b race_extra5;       /* Variable for race */
	u32b race_extra6;       /* Variable for race */
	u32b race_extra7;       /* Variable for race */

	s16b dodge_chance;      /* Dodging chance */
	s16b mfind_bonus;       /* Bonus to finding useful items */
	s16b martial_bonus;     /* Bonus to martial arts skill */

	u32b maintain_sum;      /* Do we have partial summons */

	byte spellbinder_num;   /* Number of spells bound */
	u32b spellbinder[4];    /* Spell bounds */
	byte spellbinder_trigger;       /* Spellbinder trigger condition */

	cptr mimic_name;

	char tactic;                  /* from 128-4 extremely coward to */
	                              /* 128+4 berserker */
	char movement;                /* base movement way */

	s16b companion_killed;  /* Number of companion death */
	s16b char_death_count;  /* How often you died (including lifesaved deaths) */
	s16b current_bounty_mon;	/* Which monster does Sharra from the Beastmaster Shanty want? */

	bool no_mortal;         /* Fated to never die by the hand of a mortal being */

	bool black_breath;      /* The Tolkien's Black Breath */

	bool precognition;      /* Like the cheat mode */

	/*** Extra flags -- used for lua and easying stuff ***/
	u32b xtra_f1;
	u32b xtra_f2;
	u32b xtra_f3;
	u32b xtra_f4;
	u32b xtra_f5;
	u32b xtra_f6;
	u32b xtra_f7;
	u32b xtra_f8;
	u32b xtra_f9;
	u32b xtra_f10;
	u32b xtra_esp;

	/* Corruptions */
	bool *corruptions;

	/*** Pet commands ***/
	byte pet_follow_distance; /* Length of the imaginary "leash" for pets */
	byte pet_open_doors;      /* flag - allow pets to open doors */
	byte pet_pickup_items;    /* flag - allow pets to pickup items */

	s16b control;                   /* Controlled monster */
	byte control_dir;               /* Controlled monster */

	/*** Body changing variables ***/
	u16b body_monster;        /* In which body is the player */
	bool disembodied;         /* Is the player in a body ? */
	byte body_parts[INVEN_TOTAL - INVEN_WIELD]; /* Which body parts does he have ? */

	s16b extra_body_parts[BODY_MAX];      /* Various body modifiers */

	/* Astral */
	bool astral;              /* We started at the bottom ? */

	/* Powers */
	bool *powers;     /* Actual powers */
	bool powers_mod[POWER_MAX_INIT]; /* Intrinsinc powers */

	/* Skills */
	s16b skill_points;
	s16b skill_last_level;  /* Prevents gaining skills by losing level and regaining them */
	s16b melee_style;       /* How are  */
	s16b use_piercing_shots; /* for archery */

	/* Help */
	help_info help;

	/*** Temporary fields ***/

	bool did_nothing;               /* True if the last action wasnt a real action */
	bool leaving;                   /* True if player is leaving */

	/* does the player have nasty trap effects active? --Amy */
	bool nastytrap1; /* unique saving trap - unique monsters without SPECIAL_GENE don't stay dead */
	bool nastytrap2; /* ensnaring trap - cannot detect traps at all anymore */
	bool nastytrap3; /* snaretype trap - cannot identify the type of a trap you trigger */
	bool nastytrap4; /* impious trap - cannot gain piety */
	bool nastytrap5; /* bad saving throw trap - your saving throw is always shit */
	bool nastytrap6; /* bad disarming trap - your disarming ability is always shit */
	bool nastytrap7; /* bad device trap - your devices ability is always shit */
	bool nastytrap8; /* bad stealth trap - your stealth ability is always shit */
	bool nastytrap9; /* bad searching trap - your searching ability is always shit */
	bool nastytrap10; /* bad perception trap - your perception ability is always shit */
	bool nastytrap11; /* heavy aggravation trap - you have permanent aggravate monster */
	bool nastytrap12; /* foul curse trap - periodically get hit by the TY curse */
	bool nastytrap13; /* morgoth trap - periodically get hit by the AMC */
	bool nastytrap14; /* antirecall trap - cannot recall anymore */
	bool nastytrap15; /* anti teleport trap - you cannot teleport */
	bool nastytrap16; /* endangering trap - creates traps around you every once in a while */
	bool nastytrap17; /* slowness trap - reduce your speed by 10 */
	bool nastytrap18; /* slowdown trap - divide your speed by 2 */
	bool nastytrap19; /* rmb loss trap - cannot farlook stuff */
	bool nastytrap20; /* display loss trap - turns HUD off */
	bool nastytrap21; /* auto destruct trap - get interrupted every turn */
	bool nastytrap22; /* memory trap - replace messages */
	bool nastytrap23; /* inventory trap - cannot open inventory */
	bool nastytrap24; /* luck trap - your luck is shit */
	bool nastytrap25; /* shades of grey trap - game becomes monochrome */
	bool nastytrap26; /* itemcursing trap - your items get cursed every now and then */
	bool nastytrap27; /* difficulty trap - increased monster difficulty */
	bool nastytrap28; /* confusing trap - you can still become confused with resistance */
	bool nastytrap29; /* blinding trap - you can still become blinded with resistance */
	bool nastytrap30; /* fearful trap - you can still become afraid with resistance */
	bool nastytrap31; /* mushroom samba trap - you can still become hallu with resistance */
	bool nastytrap32; /* venom trap - you can still become poisoned with resistance */
	bool nastytrap33; /* destructor trap - elemental damage destroys your items even with resistance */
	bool nastytrap34; /* dimension trap - 4D pocket doesn't work */
	bool nastytrap35; /* DSTW trap - potions sometimes don't work */
	bool nastytrap36; /* status trap - cannot heal status effects and have to wait them out */
	bool nastytrap37; /* pit creation trap - pit rooms spawn more often on new levels */
	bool nastytrap38; /* no vaults trap - new levels cannot have lesser or greater vaults */
	bool nastytrap39; /* mulch trap - ranged weapon ammo always mulches */
	bool nastytrap40; /* disenchanting trap - your items still get disenchanted with resistance */
	bool nastytrap41; /* reduce damage trap - your damage output is lower */
	bool nastytrap42; /* reduce accuracy trap - your to-hit is lower */
	bool nastytrap43; /* reduce armor trap - your armor class is lower */
	bool nastytrap44; /* carry cap trap - your carry cap is lower */
	bool nastytrap45; /* low item trap - items are of lower quality */
	bool nastytrap46; /* shafting trap - stairs take you down several levels */
	bool nastytrap47; /* unproof trap - items still get damaged even if they resist elemental damage */
	bool nastytrap48; /* no detection trap - detection methods don't work */
	bool nastytrap49; /* bad effect trap - bad stuff happens randomly */
	bool nastytrap50; /* bad loot trap - artifacts don't generate and other items are worse */
	bool nastytrap51; /* retrapping trap - new traps spawn over time */
	bool nastytrap52; /* monster scaling trap - monster levelscaling is much harsher */
	bool nastytrap53; /* sanity treble trap - you lose more sanity than normal */
	bool nastytrap54; /* stat decrease trap - your stats are lower */
	bool nastytrap55; /* bad health trap - your max HP is lower */
	bool nastytrap56; /* bad mana trap - your max mana is lower */
	bool nastytrap57; /* discount action trap - free action is less reliable */
	bool nastytrap58; /* ambience trap - background damage effects are more common */
	bool nastytrap59; /* full damage trap - monster melee attacks always deal full damage */
	bool nastytrap60; /* permacurse trap - you cannot uncurse items */
	bool nastytrap61; /* shrouded identity trap - star identify becomes normal identify */
	bool nastytrap62; /* cut nutrition trap - you gain less nutrition */
	bool nastytrap63; /* stealer trap - item-stealing monsters steal stuff more often */
	bool nastytrap64; /* stat decay trap - your stats get worse over time */
	bool nastytrap65; /* lootcut trap - monsters don't drop loot */
	bool nastytrap66; /* boring level trap - new dungeon levels don't have items on the ground */
	bool nastytrap67; /* trap increase trap - new dungeon levels have more traps */
	bool nastytrap68; /* horror trap - you randomly receive status effects */
	bool nastytrap69; /* crusher trap - monsters can crush loot */
	bool nastytrap70; /* dark mode trap - lit areas can't be seen at a distance */
	bool nastytrap71; /* charge loss trap - charge drain attacks instantly drain your shit to zero */
	bool nastytrap72; /* paranoia trap - no confirmation when walking into peaceful monsters, known traps etc. */
	bool nastytrap73; /* invis monster trap - see invisible doesn't work */
	bool nastytrap74; /* no dodging trap - your dodging chance is always zero */
	bool nastytrap75; /* ambush trap - ambushes on the world map happen more often */
	bool nastytrap76; /* ripoff trap - shopkeepers charge more for their items */
	bool nastytrap77; /* storelock trap - cannot enter shops at all */
	bool nastytrap78; /* explosive breeding trap - breeding monsters breed more quickly */
	bool nastytrap79; /* autocurse trap - equipping an item curses it */
	bool nastytrap80; /* money loss trap - your money supply decreases over time */
	bool nastytrap81; /* spawn rate trap - monsters spawn more quickly */
	bool nastytrap82; /* recharge trap - you cannot recharge your wands and staves, and rods don't charge over time */
	bool nastytrap83; /* fast metabolism trap - you hunger rapidly */
	bool nastytrap84; /* egomonster trap - monsters spawn with egotypes more often */
	bool nastytrap85; /* recurring amnesia trap - you get hit with amnesia every now and then */
	bool nastytrap86; /* starvation trap - monsters don't drop corpses or skeletons */
	bool nastytrap87; /* nonimmunity trap - you can only be resistant to elements, not immune, and also not doubly resistant */
	bool nastytrap88; /* antileveling trap - you cannot gain experience anymore */
	bool nastytrap89; /* unsustain trap - your stats get decreased even if you have them sustained */
	bool nastytrap90; /* infra trap - you cannot have infravision */
	bool nastytrap91; /* blow limit trap - you have less blows or shots per round */
	bool nastytrap92; /* no slay trap - slays on weapons don't work anymore */
	bool nastytrap93; /* no reflection trap - reflection doesn't work anymore */
	bool nastytrap94; /* bloody trap - your char can bleed even with shard resistance */
	bool nastytrap95; /* drain life trap - you get your experience drained even with resistance */
	bool nastytrap96; /* scramble trap - nexus gives all the side effects even with resistance */
	bool nastytrap97; /* antimagic shell trap - magical actions randomly fail */
	bool nastytrap98; /* antiregen trap - regeneration doesn't work */
	bool nastytrap99; /* exp drain trap - your experience gets reduced over time */
	bool nastytrap100; /* teleportitis trap - you teleport uncontrollably */
	bool nastytrap101; /* black gas trap - you have the black breath */
	bool nastytrap102; /* nodrop curse trap - you cannot drop cursed items */
	bool nastytrap103; /* uncritical trap - you can never land critical hits */
	bool nastytrap104; /* heavy equipment trap - your equipment cannot be lithe and hinders dodging more */
	bool nastytrap105; /* no esp trap - you cannot have any kind of ESP */
	bool nastytrap106; /* pseudo id trap - items don't pseudo-identify themselves */
	bool nastytrap107; /* highsnare trap - randomly generated traps can have higher levels */
	bool nastytrap108; /* no home trap - you cannot access your home */
	bool nastytrap109; /* spell failure trap - all spells have 25% autofail */
	bool nastytrap110; /* totter trap - your directional keys are inverted */
	bool nastytrap111; /* bad healing trap - all effects that heal the player's HP are halved */
	bool nastytrap112; /* activation trap - items that can be activated won't recharge */
	bool nastytrap113; /* ooop trap - stat gain doesn't work */
	bool nastytrap114; /* map amnesia trap - like dungeons with the FORGET flag, always forget the map */
	bool nastytrap115; /* edgy trap - cannot wield edged weapons without penalty, like worshipping eru */
	bool nastytrap116; /* crafting trap - runecraft and alchemy always fail */
	bool nastytrap117; /* covetous trap - teleporting monsters can teleport with you */
	bool nastytrap118; /* spellcasting trap - monsters never fail to cast a spell */
	bool nastytrap119; /* forgetful trap - amnesia will unidentify star-identified stuff */
	bool nastytrap120; /* antirush trap - cannot run in a direction */
	bool nastytrap121; /* no prayer trap - cannot pray to the gods with the "pray" command (but still cast prayer spells) */
	bool nastytrap122; /* powerless trap - U powers don't work */
	bool nastytrap123; /* inability trap - abilities like create ammo or spread blows don't work */
	bool nastytrap124; /* hard mode trap - like the hard mode option */
	bool nastytrap125; /* monster window trap - the monster window doesn't display anything */
	bool nastytrap126; /* intermittent mana trap - you get manavoided every now and then */
	bool nastytrap127; /* switchback trap - switcher will only fix this nasty trap and not the others */
	bool nastytrap128; /* black diamond trap - newly generated levels have more monsters on them */
	bool nastytrap129; /* bad fate trap - fates tend to be worse */
	bool nastytrap130; /* great horde trap - monsters that spawn in groups spawn in bigger groups */
	bool nastytrap131; /* unfeeling trap - disables level feelings and autoscumming */
	bool nastytrap132; /* worse drain trap - stat drain is harder to resist */
	bool nastytrap133; /* wakey wakey trap - monsters spawn awake */
	bool nastytrap134; /* grid bug trap - you cannot use diagonal directions */
	bool nastytrap135; /* wild danger trap - wilderness encounters are more dangerous */
	bool nastytrap136; /* nopeskill trap - cannot enhance skills */
	bool nastytrap137; /* unskilled trap - skills reduced by 10 */
	bool nastytrap138; /* skill-less trap - skills reduced by 20 */
	bool nastytrap139; /* minus skill trap - skills reduced by 3, can go negative */
	bool nastytrap140; /* negative skill trap - skills reduced by 10, can go negative */
	bool nastytrap141; /* missed info trap - some tiles don't display what a trap is */
	bool nastytrap142; /* feeler gauges trap - monsters always follow you */
	bool nastytrap143; /* mean magic trap - monster spell and breath damage is like vanilla */
	bool nastytrap144; /* mega scaling trap - monster levelscaling bonuses are like vanilla */
	bool nastytrap145; /* vegetarian trap - you cannot eat meat */
	bool nastytrap146; /* force haggle trap - you have to haggle even if the option is turned off */
	bool nastytrap147; /* void travel trap - void jumpgates don't work anymore */
	bool nastytrap148; /* stairdancing trap - using a stair prevents you from using another for much longer */
	bool nastytrap149; /* degradation trap - acid and disenchantment can lower your shit's pluses even more */
	bool nastytrap150; /* power breath trap - disables breath damage caps */
	bool nastytrap151; /* appearance shuffling trap - random item appearances can shuffle */
	bool nastytrap152; /* etherwind trap - slowly get contaminated while out in the open */
	bool nastytrap153; /* radioactivity trap - slowly get contaminated regardless of where you are */
	bool nastytrap154; /* obscuration trap - traps unidentify themselves over time */
	bool nastytrap155; /* knowledge loss trap - items unidentify themselves over time */
	bool nastytrap156; /* disconnect trap - don't spawn on a stair upon entering a level */
	bool nastytrap157; /* room alignment trap - rooms don't spawn aligned */
	bool nastytrap158; /* unstack trap - items won't stack */
	bool nastytrap159; /* de-light trap - can't see whether grids are permalit */
	bool nastytrap160; /* disturbance trap - things don't disturb (interrupt) you */
	bool nastytrap161; /* autopickup trap - you automatically pick up items you walk over */
	bool nastytrap162; /* monochrome trap - game doesn't use color */
	bool nastytrap163; /* hilite trap - cannot highlight the player or lit grids */
	bool nastytrap164; /* automore trap - more prompts automatically get cleared */
	bool nastytrap165; /* critical HP trap - no hitpoint warning */
	bool nastytrap166; /* grouping trap - monsters spawn in groups more often */
	bool nastytrap167; /* secret attack trap - monsters can use secret melee attacks */
	bool nastytrap168; /* secret spell trap - monsters can use secret spells */
	bool nastytrap169; /* arena trap - all dungeon levels are arenas */
	bool nastytrap170; /* thorns trap - meleeing a monster hurts you */
	bool nastytrap171; /* fleecy message trap - messages are randomly colored */
	bool nastytrap172; /* fleecescript trap - menu lines are randomly colored */
	bool nastytrap173; /* rainbow trap - glyphs are randomly colored */
	bool nastytrap174;
	bool nastytrap175;
	bool nastytrap176;
	bool nastytrap177;
	bool nastytrap178;
	bool nastytrap179;
	bool nastytrap180;
	bool nastytrap181;
	bool nastytrap182;
	bool nastytrap183;
	bool nastytrap184;
	bool nastytrap185;
	bool nastytrap186;
	bool nastytrap187;
	bool nastytrap188;
	bool nastytrap189;
	bool nastytrap190;
	bool nastytrap191;
	bool nastytrap192;
	bool nastytrap193;
	bool nastytrap194;
	bool nastytrap195;
	bool nastytrap196;
	bool nastytrap197;
	bool nastytrap198;
	bool nastytrap199;
	bool nastytrap200;
	bool nastytrap201;
	bool nastytrap202;
	bool nastytrap203;
	bool nastytrap204;
	bool nastytrap205;
	bool nastytrap206;
	bool nastytrap207;
	bool nastytrap208;
	bool nastytrap209;
	bool nastytrap210;
	bool nastytrap211;
	bool nastytrap212;
	bool nastytrap213;
	bool nastytrap214;
	bool nastytrap215;
	bool nastytrap216;
	bool nastytrap217;
	bool nastytrap218;
	bool nastytrap219;
	bool nastytrap220;
	bool nastytrap221;
	bool nastytrap222;
	bool nastytrap223;
	bool nastytrap224;
	bool nastytrap225;
	bool nastytrap226;
	bool nastytrap227;
	bool nastytrap228;
	bool nastytrap229;
	bool nastytrap230;
	bool nastytrap231;
	bool nastytrap232;
	bool nastytrap233;
	bool nastytrap234;
	bool nastytrap235;
	bool nastytrap236;
	bool nastytrap237;
	bool nastytrap238;
	bool nastytrap239;
	bool nastytrap240;
	bool nastytrap241;
	bool nastytrap242;
	bool nastytrap243;
	bool nastytrap244;
	bool nastytrap245;
	bool nastytrap246;
	bool nastytrap247;
	bool nastytrap248;
	bool nastytrap249;
	bool nastytrap250;
	bool nastytrap251;
	bool nastytrap252;
	bool nastytrap253;
	bool nastytrap254;
	bool nastytrap255;
	bool nastytrap256;
	bool nastytrap257;
	bool nastytrap258;
	bool nastytrap259;
	bool nastytrap260;
	bool nastytrap261;
	bool nastytrap262;
	bool nastytrap263;
	bool nastytrap264;
	bool nastytrap265;
	bool nastytrap266;
	bool nastytrap267;
	bool nastytrap268;
	bool nastytrap269;
	bool nastytrap270;
	bool nastytrap271;
	bool nastytrap272;
	bool nastytrap273;
	bool nastytrap274;
	bool nastytrap275;
	bool nastytrap276;
	bool nastytrap277;
	bool nastytrap278;
	bool nastytrap279;
	bool nastytrap280;
	bool nastytrap281;
	bool nastytrap282;
	bool nastytrap283;
	bool nastytrap284;
	bool nastytrap285;
	bool nastytrap286;
	bool nastytrap287;
	bool nastytrap288;
	bool nastytrap289;
	bool nastytrap290;
	bool nastytrap291;
	bool nastytrap292;
	bool nastytrap293;
	bool nastytrap294;
	bool nastytrap295;
	bool nastytrap296;
	bool nastytrap297;
	bool nastytrap298;
	bool nastytrap299;
	bool nastytrap300;
	bool nastytrap301;
	bool nastytrap302;
	bool nastytrap303;
	bool nastytrap304;
	bool nastytrap305;
	bool nastytrap306;
	bool nastytrap307;
	bool nastytrap308;
	bool nastytrap309;
	bool nastytrap310;
	bool nastytrap311;
	bool nastytrap312;
	bool nastytrap313;
	bool nastytrap314;
	bool nastytrap315;
	bool nastytrap316;
	bool nastytrap317;
	bool nastytrap318;
	bool nastytrap319;
	bool nastytrap320;
	bool nastytrap321;
	bool nastytrap322;
	bool nastytrap323;
	bool nastytrap324;
	bool nastytrap325;
	bool nastytrap326;
	bool nastytrap327;
	bool nastytrap328;
	bool nastytrap329;
	bool nastytrap330;
	bool nastytrap331;
	bool nastytrap332;
	bool nastytrap333;
	bool nastytrap334;
	bool nastytrap335;
	bool nastytrap336;
	bool nastytrap337;
	bool nastytrap338;
	bool nastytrap339;
	bool nastytrap340;
	bool nastytrap341;
	bool nastytrap342;
	bool nastytrap343;
	bool nastytrap344;
	bool nastytrap345;
	bool nastytrap346;
	bool nastytrap347;
	bool nastytrap348;
	bool nastytrap349;
	bool nastytrap350;
	bool nastytrap351;
	bool nastytrap352;
	bool nastytrap353;
	bool nastytrap354;
	bool nastytrap355;
	bool nastytrap356;
	bool nastytrap357;
	bool nastytrap358;
	bool nastytrap359;
	bool nastytrap360;
	bool nastytrap361;
	bool nastytrap362;
	bool nastytrap363;
	bool nastytrap364;
	bool nastytrap365;
	bool nastytrap366;
	bool nastytrap367;
	bool nastytrap368;
	bool nastytrap369;
	bool nastytrap370;
	bool nastytrap371;
	bool nastytrap372;
	bool nastytrap373;
	bool nastytrap374;
	bool nastytrap375;
	bool nastytrap376;
	bool nastytrap377;
	bool nastytrap378;
	bool nastytrap379;
	bool nastytrap380;
	bool nastytrap381;
	bool nastytrap382;
	bool nastytrap383;
	bool nastytrap384;
	bool nastytrap385;
	bool nastytrap386;
	bool nastytrap387;
	bool nastytrap388;
	bool nastytrap389;
	bool nastytrap390;
	bool nastytrap391;
	bool nastytrap392;
	bool nastytrap393;
	bool nastytrap394;
	bool nastytrap395;
	bool nastytrap396;
	bool nastytrap397;
	bool nastytrap398;
	bool nastytrap399;
	bool nastytrap400;
	bool nastytrap401;
	bool nastytrap402;
	bool nastytrap403;
	bool nastytrap404;
	bool nastytrap405;
	bool nastytrap406;
	bool nastytrap407;
	bool nastytrap408;
	bool nastytrap409;
	bool nastytrap410;
	bool nastytrap411;
	bool nastytrap412;
	bool nastytrap413;
	bool nastytrap414;
	bool nastytrap415;
	bool nastytrap416;
	bool nastytrap417;
	bool nastytrap418;
	bool nastytrap419;
	bool nastytrap420;
	bool nastytrap421;
	bool nastytrap422;
	bool nastytrap423;
	bool nastytrap424;
	bool nastytrap425;
	bool nastytrap426;
	bool nastytrap427;
	bool nastytrap428;
	bool nastytrap429;
	bool nastytrap430;
	bool nastytrap431;
	bool nastytrap432;
	bool nastytrap433;
	bool nastytrap434;
	bool nastytrap435;
	bool nastytrap436;
	bool nastytrap437;
	bool nastytrap438;
	bool nastytrap439;
	bool nastytrap440;
	bool nastytrap441;
	bool nastytrap442;
	bool nastytrap443;
	bool nastytrap444;
	bool nastytrap445;
	bool nastytrap446;
	bool nastytrap447;
	bool nastytrap448;
	bool nastytrap449;
	bool nastytrap450;
	bool nastytrap451;
	bool nastytrap452;
	bool nastytrap453;
	bool nastytrap454;
	bool nastytrap455;
	bool nastytrap456;
	bool nastytrap457;
	bool nastytrap458;
	bool nastytrap459;
	bool nastytrap460;
	bool nastytrap461;
	bool nastytrap462;
	bool nastytrap463;
	bool nastytrap464;
	bool nastytrap465;
	bool nastytrap466;
	bool nastytrap467;
	bool nastytrap468;
	bool nastytrap469;
	bool nastytrap470;
	bool nastytrap471;
	bool nastytrap472;
	bool nastytrap473;
	bool nastytrap474;
	bool nastytrap475;
	bool nastytrap476;
	bool nastytrap477;
	bool nastytrap478;
	bool nastytrap479;
	bool nastytrap480;
	bool nastytrap481;
	bool nastytrap482;
	bool nastytrap483;
	bool nastytrap484;
	bool nastytrap485;
	bool nastytrap486;
	bool nastytrap487;
	bool nastytrap488;
	bool nastytrap489;
	bool nastytrap490;
	bool nastytrap491;
	bool nastytrap492;
	bool nastytrap493;
	bool nastytrap494;
	bool nastytrap495;
	bool nastytrap496;
	bool nastytrap497;
	bool nastytrap498;
	bool nastytrap499;
	bool nastytrap500;
};


/* For Monk martial arts */

typedef struct martial_arts martial_arts;

struct martial_arts
{
	cptr    desc;    /* A verbose attack description */
	int     min_level;  /* Minimum level to use */
	int     chance;     /* Chance of 'success' */
	int     dd;        /* Damage dice */
	int     ds;        /* Damage sides */
	s16b    effect;     /* Special effects */
	s16b    power;     /* Special effects power */
};



/* Powers - used by Mindcrafters and Necromancers */
typedef struct magic_power magic_power;

struct magic_power
{
	int     min_lev;
	int     mana_cost;
	int     fail;
	cptr    name;
	cptr    desc;
};

/* Border */
typedef struct border_type border_type;

struct border_type
{
	byte 	north[MAX_WID];
	byte 	south[MAX_WID];
	byte 	east[MAX_HGT];
	byte 	west[MAX_HGT];
	byte	north_west;
	byte	north_east;
	byte	south_west;
	byte	south_east;
};


/*
 * A structure describing a wilderness area
 * with a terrain, a town or a dungeon entrance
 */
typedef struct wilderness_type_info wilderness_type_info;

struct wilderness_type_info
{
	u32b    name;                   /* Name (offset) */
	u32b    text;                   /* Text (offset) */
	u16b    entrance;               /* Which town is there(<1000 i's a town, >=1000 it a dungeon) */
	s32b	wild_x;			/* Map coordinates (backed out while parsing map) */
	s32b	wild_y;
	byte    road;                   /* Flags of road */
	int     level;                  /* Difficulty level */
	u32b    flags1;                 /* Some flags */
	byte    feat;                   /* The feature of f_info.txt that is used to allow passing, ... and to get a char/color/graph */
	byte    terrain_idx;            /* Terrain index(defined in defines.h) */

	byte    terrain[MAX_WILD_TERRAIN];/* Feature types for the plasma generator */
};

/*
 * A structure describing a wilderness map
 */
typedef struct wilderness_map wilderness_map;

struct wilderness_map
{
	int     feat;                   /* Wilderness feature */
	u32b    seed;                   /* Seed for the RNG */
	u16b    entrance;               /* Entrance for dungeons */

	bool    known;                  /* Is it seen by the player ? */
};

/*
 * A structure describing a town with
 * stores and buildings
 */
typedef struct town_type town_type;
struct town_type
{
	cptr name;
	u32b seed; /* Seed for RNG */
	store_type *store;      /* The stores [max_st_idx] */
	byte numstores;

	byte flags;             /* Town flags */
	/* Left this for the sake of compatibility */
	bool stocked;           /* Is the town actualy stocked ? */

	bool destroyed;         /* Is the town destroyed? */
};


/* Alchemists */

typedef struct tval_desc2
{
	int        tval;
	cptr       desc;
} tval_desc2;

typedef struct alchemist_recipe alchemist_recipe;
struct alchemist_recipe
{
	int     sval_essence;
	byte	tval;
	byte	sval;
	byte	qty;
};

typedef struct artifact_select_flag artifact_select_flag;
struct artifact_select_flag {
	byte group;		/* Flag group to display it in */
	int flag;		/* item flag to set */
	byte level;		/* Player skill level to start at */
	int desc;		/* Display this description to select flag */
	u32b xp;		/* xp cost for this flag */
	bool pval;		/* indicates this flag benifits from pval */
	int item_desc;	/* Description of required item */
	int item_descp;	/* Description of required item */
	byte rtval;		/* Required items' tval */
	byte rsval;		/* Required items' sval */
	int  rpval;		/* Required items' pval (zero for no req) */
	int  rflag[6];	/* Monster Race flags for required Corpses */
};

/*
 A structure for deity information.
 */
typedef struct deity_type deity_type;
struct deity_type
{
	cptr name;
	char desc[10][80];
};

/* A structure for tactics */
typedef struct tactic_info_type tactic_info_type;

struct tactic_info_type
{
	s16b to_hit;
	s16b to_dam;
	s16b to_ac;
	s16b to_stealth;
	s16b to_disarm;
	s16b to_saving;
	cptr name;
};

/* A structure to describe a random artifact. */
typedef struct random_artifact random_artifact;

struct random_artifact
{
	char name_full[80];     /* Full name for the artifact */
	char name_short[80];    /* Un-Id'd name */
	byte level;             /* Level of the artifact */
	byte attr;              /* Color that is used on the screen */
	u32b cost;              /* Object's value */
	byte activation;        /* Activation. */
	s16b timeout;           /* Timeout. */
	byte generated;         /* Does it exist already? */
};

/* A structure to describe an activation. */
typedef struct activation activation;

struct activation
{
	char desc[80];          /* Desc of the activation */
	u32b cost;              /* costs value */
	s16b spell;             /* Spell. */
};

/* A structure to describe a music. */
typedef struct music music;

struct music
{
	char desc[80];          /* Desc of the music */
	s16b music;             /* Music. */
	s16b dur;               /* Duration(if any) */
	s16b init_recharge;     /* Minimal recharge time */
	s16b turn_recharge;     /* Recharge time for each more turn */
	byte min_inst;          /* Minimum instrument for the music */
	byte rarity;            /* Rarity of the music(use 100 to unallow to be randomly generated) */
};

/* A structure to describe the random spells of the Power Mages */
typedef struct random_spell random_spell;

struct random_spell
{
	char desc[30];          /* Desc of the spell */
	char name[30];          /* Name of the spell */
	s16b mana;              /* Mana cost */
	s16b fail;              /* Failure rate */
	u32b proj_flags;        /* Project function flags */
	byte GF;                /* Type of the projection */
	byte radius;
	byte dam_sides;
	byte dam_dice;
	byte level;             /* Level needed */
	bool untried;           /* Is the spell was tried? */
};

/* A structure to describe the fate of the player */
typedef struct fate fate;

struct fate
{
	byte fate;      /* Which fate */
	byte level;     /* On which level */
	byte serious;   /* Is it sure? */
	s16b o_idx;     /* Object to find */
	s16b e_idx;     /* Ego-Item to find */
	s16b a_idx;     /* Artifact to find */
	s16b v_idx;     /* Vault to find */
	s16b r_idx;     /* Monster to find */
	s16b count;     /* Number of things */
	s16b time;      /* Turn before */
	bool know;      /* Has it been predicted? */
	bool icky;	/* Hackish runtime-only flag */
};

/* A structure for movements */
typedef struct move_info_type move_info_type;

struct move_info_type
{
	s16b to_speed;
	s16b to_search;
	s16b to_stealth;
	s16b to_percep;
	cptr name;
};

/* Define monster generation rules */
typedef struct rule_type rule_type;
struct rule_type
{
	byte mode;                      /* Mode of combination of the monster flags */
	byte percent;                   /* Percent of monsters affected by the rule */

	u32b mflags1;                   /* The monster flags that are allowed */
	u32b mflags2;
	u32b mflags3;
	u32b mflags4;
	u32b mflags5;
	u32b mflags6;
	u32b mflags7;
	u32b mflags8;
	u32b mflags9;
	u32b mflags10;
	u32b mflags11;
	u32b mflags12;
	u32b mflags13;
	u32b mflags14;
	u32b mflags15;
	u32b mflags16;
	u32b mflags17;
	u32b mflags18;
	u32b mflags19;
	u32b mflags20;

	char r_char[10];                 /* Monster race allowed */
};

/* A structure for the != dungeon types */
typedef struct dungeon_info_type dungeon_info_type;
struct dungeon_info_type
{
	u32b name;                      /* Name */
	u32b text;                      /* Description */
	char short_name[3];             /* Short name */

	char generator[30];             /* Name of the level generator */

	s16b floor1;                    /* Floor tile 1 */
	byte floor_percent1[2];         /* Chance of type 1 */
	s16b floor2;                    /* Floor tile 2 */
	byte floor_percent2[2];         /* Chance of type 2 */
	s16b floor3;                    /* Floor tile 3 */
	byte floor_percent3[2];         /* Chance of type 3 */
	s16b outer_wall;                /* Outer wall tile */
	s16b inner_wall;                /* Inner wall tile */
	s16b fill_type1;                /* Cave tile 1 */
	byte fill_percent1[2];          /* Chance of type 1 */
	s16b fill_type2;                /* Cave tile 2 */
	byte fill_percent2[2];          /* Chance of type 2 */
	s16b fill_type3;                /* Cave tile 3 */
	byte fill_percent3[2];          /* Chance of type 3 */
	byte fill_method;				/* Smoothing parameter for the above */

	s16b mindepth;                  /* Minimal depth */
	s16b maxdepth;                  /* Maximal depth */

	bool principal;                 /* If it's a part of the main dungeon */
	byte next;                      /* The next part of the main dungeon */
	byte min_plev;                  /* Minimal plev needed to enter -- it's an anti-cheating mesure */

	int min_m_alloc_level;          /* Minimal number of monsters per level */
	int max_m_alloc_chance;         /* There is a 1/max_m_alloc_chance chance per round of creating a new monster */

	u32b flags1;                    /* Flags 1 */
	u32b flags2;                    /* Flags 1 */

	int size_x, size_y;             /* Desired numers of panels */

	byte rule_percents[100];        /* Flat rule percents */
	rule_type rules[5];             /* Monster generation rules */

	int final_object;               /* The object you'll find at the bottom */
	int final_artifact;             /* The artifact you'll find at the bottom */
	int final_guardian;             /* The artifact's guardian. If an artifact is specified, then it's NEEDED */

	int ix, iy, ox, oy;             /* Wilderness coordinates of the entrance/output of the dungeon */

	obj_theme objs;                 /* The drops type */

	int d_dice[4];                  /* Number of dices */
	int d_side[4];                  /* Number of sides */
	int d_frequency[4];             /* Frequency of damage (1 is the minimum) */
	int d_type[4];                  /* Type of damage */

	s16b t_idx[TOWN_DUNGEON];       /* The towns */
	s16b t_level[TOWN_DUNGEON];     /* The towns levels */
	s16b t_num;                     /* Number of towns */
};

/* A structure for inscriptions */
typedef struct inscription_info_type inscription_info_type;
struct inscription_info_type
{
	char text[40];                  /* The inscription itself */
	byte when;                      /* When it is executed */
	bool know;                      /* Is the inscription know ? */
	byte mana;                      /* Grid mana needed */
};

/* To hold Runecrafters prefered spells */
typedef struct rune_spell rune_spell;
struct rune_spell
{
	char name[30];          /* name */

	s16b type;              /* Type of the spell(GF) */
	s16b rune2;             /* Modifiers */
	s16b mana;              /* Mana involved */
};

/* For level gaining artifacts, artifact creation, ... */
typedef struct flags_group flags_group;
struct flags_group
{
	char name[30];          /* Name */
	byte color;             /* Color */

	byte price;             /* Price to "buy" it */

	u32b flags1;            /* Flags set 1 */
	u32b flags2;            /* Flags set 2 */
	u32b flags3;            /* Flags set 3 */
	u32b flags4;            /* Flags set 4 */
	u32b flags5;            /* Flags set 5 */
	u32b flags6;            /* Flags set 6 */
	u32b flags7;            /* Flags set 7 */
	u32b flags8;            /* Flags set 8 */
	u32b flags9;            /* Flags set 9 */
	u32b flags10;           /* Flags set 10 */
	u32b esp;               /* ESP flags set */
};

/* For powers(racial, class, mutation, artifacts, ... */
typedef struct power_type power_type;
struct power_type
{
	char *name;              /* Name */
	char *desc_text;         /* Text describing power */
	char *gain_text;         /* Text displayed on gaining the power */
	char *lose_text;         /* Text displayed on losing the power */

	byte level;             /* Min level */
	s16b cost;              /* Mana/Life cost */
	byte stat;              /* Stat used */
	byte diff;              /* Difficulty */
};

/* Hooks */
typedef bool (*hook_type)(char *fmt);

/*
 * Structure for the "quests"
 */
typedef struct quest_type quest_type;

struct quest_type
{
	bool silent;

	bool dynamic_desc;      /* Do we need to ask a function to get the description ? */

	char name[40];          /* Quest name */

	char desc[10][80];      /* Quest desc */

	s16b status;            /* Is the quest taken, completed, finished? */

	s16b level;             /* Dungeon level */

	s16b *plot;             /* Which plot does it belongs to? */

	byte type;              /* Lua or C ? */

	bool (*init)(int q);    /* Function that takes care of generating hardcoded quests */

	s32b data[4];           /* Various datas used by the quests */
};
typedef struct random_quest random_quest;
struct random_quest
{
	byte type;              /* Type/number of monsters to kill(0 = no quest) */
	s16b r_idx;             /* Monsters to crush */
	bool done;              /* Done ? */
};

/* Monster powers for player uses */
typedef struct monster_power monster_power;
struct monster_power
{
	u32b    power;          /* Power RF?_xxx */
	cptr    name;           /* Name of it */
	int     mana;           /* Mana needed */
	bool    great;          /* Need the use of great spells */
};

/* Tval descs */
typedef struct tval_desc tval_desc;
struct tval_desc
{
	int     tval;           /* tval */
	cptr    desc;           /* desc */
};

/*
 * Between exit
 */
typedef struct between_exit between_exit;
struct between_exit
{
	s16b corresp;           /* Corresponding between gate */
	bool dungeon;           /* Do we exit in a dungeon or in the wild ? */

	s16b wild_x, wild_y;    /* Wilderness spot to land onto */
	s16b px, py;            /* Location of the map */

	s16b d_idx;             /* Dungeon to land onto */
	s16b level;
};

/*
 * A structure to hold "rolled" information
 */
typedef struct birther birther;
struct birther
{
	s16b sex;
	s16b race;
	s16b rmod;
	s16b pclass;
	s16b spec;

	s32b quests;

	byte god;
	s32b grace;
	s32b god_favor;

	s16b age;
	s16b wt;
	s16b ht;
	s16b sc;

	s32b au;

	s16b stat[6];
	s16b luck;

	s16b chaos_patron;

	u32b weapon;

	char history[4][60];

	bool quick_ok;
};

typedef struct hooks_chain hooks_chain;
struct hooks_chain
{
	hook_type hook;
	char name[40];
	char script[40];
	byte type;
	hooks_chain *next;
};

typedef union hook_return hook_return;
union hook_return
{
	s32b num;
	cptr str;
	object_type *o_ptr;
	monster_type *m_ptr;
};

/*
 * Forward declare
 */
typedef struct hist_type hist_type;

/*
 * Player background information
 */
struct hist_type
{
	s32b info;                              /* Textual History -- uses rp_text */

	byte roll;			        /* Frequency of this entry */
	s16b chart;                             /* Chart index */
	s16b next;                              /* Next chart index */
	byte bonus;			        /* Social Class Bonus + 50 */
};

/*
 * Item sets
 */
typedef struct set_type set_type;
struct set_type
{
	u32b name;                              /* Name */
	u32b desc;                              /* Desc */

	byte num;                               /* Number of artifacts used */
	byte num_use;                           /* Number actually wore */
	struct                                  /* the various items */
	{
		bool present;                   /* Is it actually wore ? */
		s16b a_idx;                     /* What artifact ? */
		s16b pval[6];                   /* Pval for each combination */
		u32b flags1[6];                 /* Flags */
		u32b flags2[6];                 /* Flags */
		u32b flags3[6];                 /* Flags */
		u32b flags4[6];                 /* Flags */
		u32b flags5[6];                 /* Flags */
		u32b flags6[6];                 /* Flags */
		u32b flags7[6];                 /* Flags */
		u32b flags8[6];                 /* Flags */
		u32b flags9[6];                 /* Flags */
		u32b flags10[6];                /* Flags */
		u32b esp[6];                    /* Flags */
	} arts[6];
};

/* A structure for CLI commands. */
typedef struct cli_comm cli_comm;
struct cli_comm
{
	cptr comm;	/* Extended name of the command. */
	cptr descrip;	/* Description of the command. */
	s16b key;	/* Key to convert command to. */
};

/*
 * Skills !
 */
typedef struct skill_type skill_type;
struct skill_type
{
	u32b name;                              /* Name */
	u32b desc;                              /* Description */
	u32b action_desc;                       /* Action Description */

	s16b action_mkey;                       /* Action do to */

	s32b i_value;                           /* Actual value */
	s32b i_mod;                             /* Modifier(1 skill point = modifier skill) */

	s32b value;                             /* Actual value */
	s32b mod;                               /* Modifier(1 skill point = modifier skill) */
	s16b rate;                              /* Modifier decreasing rate */

	u32b uses;                             	/* Number of times used */

	s16b action[MAX_SKILLS];                /* List of actions against other skills */

	s16b father;                            /* Father in the skill tree */
	bool dev;                               /* Is the branch developped ? */
	s16b order;                             /* Order in the tree */
	bool hidden;                            /* Innactive */

	byte random_gain_chance;                /* random gain chance, still needs the flag */

	u32b flags1;                            /* Skill flags */
};


/*
 * The spell function must provide the desc
 */
typedef struct spell_type spell_type;
struct spell_type
{
	cptr name;                      /* Name */
	byte skill_level;               /* Required level (to learn) */
	s16b mana;			/* Required mana at lvl 1 */
	s16b mana_max;			/* Required mana at max lvl */
	s16b fail;			/* Minimum chance of failure */
	s16b level;                     /* Spell level(0 = not learnt) */
};

typedef struct school_type school_type;
struct school_type
{
	cptr name;                      /* Name */
	s16b skill;                     /* Skill used for that school */
};

/*
 * Desc for GF_FOO
 */
typedef struct gf_name_type gf_name_type;
struct gf_name_type
{
	int gf;
	cptr name;
};

/*
 * Timers
 */
typedef struct timer_type timer_type;
struct timer_type
{
	timer_type *next;       /* The next timer in the list */

	bool enabled;           /* Is it currently counting? */

	s32b delay;             /* Delay between activations */
	s32b countdown;         /* The current number of turns passed, when it reaches delay it fires */

	cptr callback;          /* The lua function to call upon firing(no C callback yet .. maybe) */
};

/*
 * This is for lua functions that need to pass table to c functions
 */
typedef struct list_type list_type;
struct list_type
{
	cptr *list;
};

/*
 * Abilities
 */
typedef struct ability_type ability_type;
struct ability_type
{
	u32b name;                              /* Name */
	u32b desc;                              /* Description */
	u32b action_desc;                       /* Action Description */

	s16b action_mkey;                       /* Action do to */

	s16b cost;                              /* Skill points cost */

	bool acquired;                          /* Do the player actualylg ot it ? */

	/* Prereqs */
	s16b skills[10];                	/* List of prereq skills(10 max) */
	s16b skill_levels[10];                  /* List of prereq skills(10 max) */
	s16b stat[6];                		/* List of prereq stats */
	s16b need_abilities[10];              	/* List of prereq abilities(10 max) */
	s16b forbid_abilities[10];		/* List of forbidden abilities(10 max) */
};
