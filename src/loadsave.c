/* File: loadsave.c */

/* Purpose: interact with savefiles. This file was made by
   unifying load2.c and save.c from the old codebase. Doing it
   this way makes maintenance easier and lets us share code. */

#include "angband.h"

/* Don't play with this yet */
/* #define BZ_SAVES */
#ifdef BZ_SAVES
#include <bzlib.h>
#endif /* BZ_SAVES */

static void do_byte(byte *, int);
static void do_u16b(u16b *, int);
static void do_s16b(s16b *, int);
static void do_u32b(u32b *, int);
static void do_s32b(s32b *, int);
static void do_string(char *, int, int);
static void do_lore(int, int);
static void do_monster(monster_type *, int);
static void do_randomizer(int flag);
static void do_spells(int, int);
static void note(cptr);
static void do_fate(int, int);
static void do_item(object_type *, int);
static void do_options(int);
static bool do_store(store_type *, int);
static void do_messages(int flag);
static void do_xtra(int, int);
static bool do_savefile_aux(int);
static void junkinit(void);
static void morejunk(void);
static bool do_inventory(int);
static bool do_dungeon(int, bool);
static void do_grid(int);
static void my_sentinel(char *, u16b, int);

#ifdef BZ_SAVES
static void bz_done(int);
static void bz_prep(int);
#endif

static void do_ver_byte(byte *, u32b, byte, int);
static void do_ver_u16b(u16b *, u32b, u16b, int);
static void do_ver_s16b(s16b *, u32b, s16b, int);
static void do_ver_u32b(u32b *, u32b, u32b, int);
static void do_ver_s32b(s32b *, u32b, s32b, int);
static void do_ver_string(char *, int, u32b, char *, int);

static void skip_ver_byte(u32b, int);
static void skip_ver_u16b(u32b, int);
static void skip_ver_s16b(u32b, int);
static void skip_ver_u32b(u32b, int);
static void skip_ver_s32b(u32b, int);
static void skip_ver_string(u32b, int);

errr rd_savefile(void);

#ifdef SAFER_PANICS
bool panicload;
#endif

static FILE *fff; 	/* Local savefile ptr */

#ifdef BZ_SAVES
BZFILE *bzf;
int bzerr;
#endif /* BZ_SAVES */

/*
 * Basic byte-level reading from savefile. This provides a single point
 * of interface to the pseudoencryption that ToME (and Angband)
 * uses. I'm thinking about if it might be faster/better to modify all
 * the do_* functions to directly do this stuff -- it'd make the code
 * somewhat uglier to maintain, but concievably might be much faster. Or
 * is it better maybe to scrap the pseudoencryption entirely and adopt
 * some other means of obfuscation, should it still prove useful in any
 * way? -- Improv
 *
 * What's the point of encryption on savefiles anyway? If I wanted to
 * make a cheater savefile, I'd activate debug mode, and hack the game
 * not to save it. There's no point. -- takkaria
 */

static byte sf_get(void)
{
	byte c;

	/* Get a character, decode the value */
#ifndef BZ_SAVES
	c = getc(fff) & 0xFF;
#else
BZ2_bzRead(&bzerr, bzf, &c, 1);

	if (bzerr != 0)
	{
		note(format("Compression error on read"));
		bz_done(LS_LOAD);
		printf("Died in sf_get\n");
		exit(0);
	}
#endif /* BZ_SAVES */

	/* Return the value */
	return (c);
}


static void sf_put(byte v)
{
#ifndef BZ_SAVES
	(void)putc((int)v, fff);
#else
	BZ2_bzWrite(&bzerr, bzf, &v, 1);

	if (bzerr != 0)
	{
		note(format("Compression error on write"));
		bz_done(LS_SAVE);
		printf("Died in sf_put\n");
		exit(0);
	}
#endif
}

/*
 * This function does nothing if BZ_SAVES in undefined.
 */
#ifdef BZ_SAVES
static void bz_prep(int flag)
{

	if (flag == LS_LOAD)
	{
		bzf = BZ2_bzReadOpen(&bzerr, fff, 0, 0, NULL, 0);
	}

	if (flag == LS_SAVE)
	{
		bzf = BZ2_bzWriteOpen(&bzerr, fff, 9, 0, 30);
	}

	if (bzerr == 0)
	{
		return ; 					/* All is Good */
	}

	/* Otherwise, all is bad */
	note(format("Compression error on prep"));
	printf("Died in bz_prep\n");
	exit(0);

	/* Unreachable code */
	bz_done(flag);
}

static void bz_done(int flag)
{
	if (flag == LS_LOAD)
	{
		BZ2_bzReadClose(&bzerr, bzf);
	}
	if (flag == LS_SAVE)
	{
		BZ2_bzWriteClose(&bzerr, bzf, 0, NULL, NULL);
	}
}
#endif	/* BZ_SAVES */

/*
 * Do object memory and similar stuff
 */
static void do_xtra(int k_idx, int flag)
{
	byte tmp8u = 0;
	object_kind *k_ptr = &k_info[k_idx];

	if (flag == LS_SAVE)
	{
		if (k_ptr->aware) tmp8u |= 0x01;
		if (k_ptr->tried) tmp8u |= 0x02;
		if (k_ptr->know) tmp8u |= 0x04;
		if (k_ptr->artifact) tmp8u |= 0x80;

		do_byte(&tmp8u, flag);
	}
	if (flag == LS_LOAD)
	{
		do_byte(&tmp8u, flag);
		k_ptr->aware = ((tmp8u & 0x01) ? TRUE : FALSE);
		k_ptr->tried = ((tmp8u & 0x02) ? TRUE : FALSE);
		k_ptr->know = ((tmp8u & 0x04) ? TRUE : FALSE);
		k_ptr->artifact = ((tmp8u & 0x80) ? TRUE : FALSE);
	}
}

/*
 * Load/Save quick start data
 */
void do_quick_start(int flag)
{
	int i;

	do_s16b(&previous_char.sex, flag);
	do_s16b(&previous_char.race, flag);
	do_s16b(&previous_char.rmod, flag);
	do_s16b(&previous_char.pclass, flag);
	do_s16b(&previous_char.spec, flag);
	do_s32b(&previous_char.quests, flag);
	do_byte(&previous_char.god, flag);
	do_s32b(&previous_char.grace, flag);
	do_s16b(&previous_char.age, flag);
	do_s16b(&previous_char.wt, flag);
	do_s16b(&previous_char.ht, flag);
	do_s16b(&previous_char.sc, flag);
	do_s32b(&previous_char.au, flag);

	for (i = 0; i < 6; i++) do_s16b(&(previous_char.stat[i]), flag);
	do_s16b(&previous_char.luck, flag);

	do_s16b(&previous_char.chaos_patron, flag);
	do_u32b(&previous_char.weapon, flag);
	do_byte((byte*)&previous_char.quick_ok, flag);

	for (i = 0; i < 4; i++) do_string(previous_char.history[i], 60, flag);
}

/*
 * The special saved subrace
 */
static void do_subrace(int flag)
{
	player_race_mod *sr_ptr = &race_mod_info[SUBRACE_SAVE];
	int i;
	char buf[81];

	if (flag == LS_SAVE)
		strcpy(buf, sr_ptr->title + rmp_name);
	do_string(buf, 80, flag);
	if (flag == LS_LOAD)
		strcpy(sr_ptr->title + rmp_name, buf);

	if (flag == LS_SAVE)
		strcpy(buf, sr_ptr->desc + rmp_text);
	do_string(buf, 80, flag);
	if (flag == LS_LOAD)
		strcpy(sr_ptr->desc + rmp_text, buf);

	do_byte((byte*)&sr_ptr->place, flag);

	for (i = 0; i < 6; i++)
		do_s16b(&sr_ptr->r_adj[i], flag);

	do_byte((byte*)&sr_ptr->luck, flag);
	do_s16b(&sr_ptr->mana, flag);

	do_s16b(&sr_ptr->r_dis, flag);
	do_s16b(&sr_ptr->r_dev, flag);
	do_s16b(&sr_ptr->r_sav, flag);
	do_s16b(&sr_ptr->r_stl, flag);
	do_s16b(&sr_ptr->r_srh, flag);
	do_s16b(&sr_ptr->r_fos, flag);
	do_s16b(&sr_ptr->r_thn, flag);
	do_s16b(&sr_ptr->r_thb, flag);

	do_byte((byte*)&sr_ptr->r_mhp, flag);
	do_s16b(&sr_ptr->r_exp, flag);

	do_byte((byte*)&sr_ptr->b_age, flag);
	do_byte((byte*)&sr_ptr->m_age, flag);

	do_byte((byte*)&sr_ptr->m_b_ht, flag);
	do_byte((byte*)&sr_ptr->m_m_ht, flag);
	do_byte((byte*)&sr_ptr->f_b_ht, flag);
	do_byte((byte*)&sr_ptr->f_m_ht, flag);

	do_byte((byte*)&sr_ptr->m_b_wt, flag);
	do_byte((byte*)&sr_ptr->m_m_wt, flag);
	do_byte((byte*)&sr_ptr->f_b_wt, flag);
	do_byte((byte*)&sr_ptr->f_m_wt, flag);

	do_byte((byte*)&sr_ptr->infra, flag);

	for (i = 0; i < 4; i++)
		do_s16b(&sr_ptr->powers[i], flag);

	for (i = 0; i < BODY_MAX; i++)
		do_byte((byte*)&sr_ptr->body_parts[i], flag);

	do_u32b(&sr_ptr->flags1, flag);
	do_u32b(&sr_ptr->flags2, flag);

	for (i = 0; i < PY_MAX_LEVEL + 1; i++)
	{
		do_u32b(&sr_ptr->oflags1[i], flag);
		do_u32b(&sr_ptr->oflags2[i], flag);
		do_u32b(&sr_ptr->oflags3[i], flag);
		do_u32b(&sr_ptr->oflags4[i], flag);
		do_u32b(&sr_ptr->oflags5[i], flag);
		do_u32b(&sr_ptr->oflags6[i], flag);
		do_u32b(&sr_ptr->oflags7[i], flag);
		do_u32b(&sr_ptr->oflags8[i], flag);
		do_u32b(&sr_ptr->oflags9[i], flag);
		do_u32b(&sr_ptr->oflags10[i], flag);
		do_u32b(&sr_ptr->oesp[i], flag);
		do_s16b(&sr_ptr->opval[i], flag);
	}

	do_byte(&sr_ptr->g_attr, flag);
	do_byte((byte*)&sr_ptr->g_char, flag);

	for (i = 0; i < MAX_SKILLS; i++)
	{
		do_byte((byte*)&sr_ptr->skill_basem[i], flag);
		do_u32b(&sr_ptr->skill_base[i], flag);
		do_byte((byte*)&sr_ptr->skill_modm[i], flag);
		do_s16b(&sr_ptr->skill_mod[i], flag);
	}
}

/*
 * Misc. other data
 */
static char loaded_game_module[80];
static bool do_extra(int flag)
{
	int i, j;
	byte tmp8u;
	s16b tmp16s;
	u32b tmp32u;
	u16b tmp16b;

	do_string(player_name, 32, flag);

	do_string(died_from, 80, flag);

	for (i = 0; i < 4; i++)
	{
		do_string(history[i], 60, flag);
	}

	/* Handle the special levels info */
	if (flag == LS_SAVE)
	{
		tmp8u = max_d_idx;
		tmp16s = MAX_DUNGEON_DEPTH;
	}
	do_byte(&tmp8u, flag);

	if (flag == LS_LOAD)
	{
		if (tmp8u > max_d_idx)
		{
			note(format("Too many (%d) dungeon types!", tmp8u));
		}
	}

	do_s16b(&tmp16s, flag);

	if (flag == LS_LOAD)
	{
		if (tmp16s > MAX_DUNGEON_DEPTH)
		{
			note(format("Too many (%d) max level by dungeon type!", tmp16s));
		}
	}

	/* Load the special levels history */
	for (i = 0; i < tmp8u; i++)
	{
		for (j = 0; j < tmp16s; j++)
		{
			do_byte((byte*)&special_lvl[j][i], flag);
		}
	}

	do_byte((byte*)&generate_special_feeling, flag);

	/* Load the quick start data */
	do_quick_start(flag);

	/* Load/save the special subrace */
	do_subrace(flag);

	/* Race/Class/Gender/Spells */
	do_s32b(&p_ptr->lives, flag);
	do_byte(&p_ptr->prace, flag);
	do_byte(&p_ptr->pracem, flag);
	do_byte(&p_ptr->pclass, flag);
	do_byte(&p_ptr->pspec, flag);
	do_byte(&p_ptr->psex, flag);
	do_u16b(&tmp16b, flag);
	do_u16b(&tmp16b, flag);
	do_byte(&p_ptr->mimic_form, flag);
	do_s16b(&p_ptr->mimic_level, flag);
	if (flag == LS_SAVE) tmp8u = 0;

	do_byte(&p_ptr->hitdie, flag);
	do_u16b(&p_ptr->expfact, flag);

	do_s16b(&p_ptr->age, flag);
	do_s16b(&p_ptr->ht, flag);
	do_s16b(&p_ptr->wt, flag);

	/* Dump the stats (maximum and current) */
	for (i = 0; i < 6; ++i) do_s16b(&p_ptr->stat_max[i], flag);
	for (i = 0; i < 6; ++i) do_s16b(&p_ptr->stat_cur[i], flag);
	for (i = 0; i < 6; ++i) do_s16b(&p_ptr->stat_cnt[i], flag);
	for (i = 0; i < 6; ++i) do_s16b(&p_ptr->stat_los[i], flag);

	/* Dump the skills */
	do_s16b(&p_ptr->skill_points, flag);
	do_s16b(&p_ptr->skill_last_level, flag);
	do_s16b(&p_ptr->melee_style, flag);
	do_s16b(&p_ptr->use_piercing_shots, flag);

	tmp16s = MAX_SKILLS;
	do_s16b(&tmp16s, flag);

	if ((flag == LS_LOAD) && (tmp16s > MAX_SKILLS))
	{
		quit("Too many skills");
	}

	if (flag == LS_SAVE) old_max_s_idx = max_s_idx;
	do_u16b(&old_max_s_idx, flag);
	for (i = 0; i < tmp16s; ++i)
	{
		if (i < old_max_s_idx)
		{
			do_s32b(&s_info[i].value, flag);
			do_s32b(&s_info[i].mod, flag);
			do_byte((byte*)&s_info[i].dev, flag);
			do_byte((byte*)&s_info[i].hidden, flag);
			do_u32b(&s_info[i].uses, flag);
		}
		else
		{
			do_u32b(&tmp32u, flag);
			do_s16b(&tmp16s, flag);
			do_byte(&tmp8u, flag);
			do_byte(&tmp8u, flag);
			do_u32b(&tmp32u, flag);
		}
	}

	tmp16s = max_ab_idx;
	do_s16b(&tmp16s, flag);

	if ((flag == LS_LOAD) && (tmp16s > max_ab_idx))
	{
		quit("Too many abilities");
	}

	for (i = 0; i < tmp16s; ++i)
	{
		do_byte((byte*)&ab_info[i].acquired, flag);
	}

	do_s16b(&p_ptr->luck_base, flag);
	do_s16b(&p_ptr->luck_max, flag);

	/* Found 24 unused bytes here...
	   Converted it to be the alchemist's
	   known artifact flags.
	   Note that the ego flags and the gained levels
	   record are recorded here too, but we use the
	   _ver_ format to protect save file compatablity.
	   Note that the other alchemist knowledge (item types known)
	   is stored in do_aux, and is a bit flag in a previously
	   unused bit.
	   */
	for (i = 0; i < 11 ; ++i)
		do_u32b(&alchemist_known_artifacts[i], flag);

	for (i = 0; i < 32 ; ++i)
		do_u32b(&alchemist_known_egos[i], flag);

	do_u32b(&alchemist_gained, flag);

	do_s32b(&p_ptr->au, flag);

	do_s32b(&p_ptr->max_exp, flag);
	do_s32b(&p_ptr->exp, flag);
	do_u16b(&p_ptr->exp_frac, flag);
	do_s16b(&p_ptr->lev, flag);
	do_s32b(&p_ptr->overlevel, flag);

	do_s16b(&p_ptr->town_num, flag); 	/* -KMW- */

	/* Write arena and rewards information -KMW- */
	do_s16b(&p_ptr->arena_number, flag);
	do_s16b(&p_ptr->inside_arena, flag);
	do_s16b(&p_ptr->inside_quest, flag);
	do_byte((byte*)&p_ptr->exit_bldg, flag);


	/* Save/load spellbinder */
	do_byte(&p_ptr->spellbinder_num, flag);
	do_byte(&p_ptr->spellbinder_trigger, flag);
	for (i = 0; i < 4; i++)
		do_u32b(&p_ptr->spellbinder[i], flag);


	do_byte(&tmp8u, flag); 		/* tmp8u should be 0 at this point */

	if (flag == LS_SAVE) tmp8u = MAX_PLOTS;
	do_byte(&tmp8u, flag);

	if ((flag == LS_LOAD) && (tmp8u > MAX_PLOTS))
	{
		quit(format("Too many plots, %d %d", tmp8u, MAX_PLOTS));
	}

	for (i = 0; i < tmp8u; i++)
	{
		do_s16b(&plots[i], flag);
	}

	if (flag == LS_SAVE)
	{
		tmp32u = MAX_RANDOM_QUEST;
	}
	do_s32b(&tmp32u, flag);

	if ((flag == LS_LOAD) &&
	                (tmp32u > MAX_RANDOM_QUEST)) quit("Too many random quests");
	for (i = 0; i < tmp32u; i++)
	{
		do_byte(&random_quests[i].type, flag);
		do_s16b(&random_quests[i].r_idx, flag);
		do_byte((byte*)&random_quests[i].done, flag);
	}

	do_s16b(&p_ptr->oldpx, flag);
	do_s16b(&p_ptr->oldpy, flag);

	do_s16b(&p_ptr->mhp, flag);
	do_s16b(&p_ptr->chp, flag);
	do_u16b(&p_ptr->chp_frac, flag);
	do_s16b(&p_ptr->hp_mod, flag);

	do_s16b(&p_ptr->msane, flag);
	do_s16b(&p_ptr->csane, flag);
	do_u16b(&p_ptr->csane_frac, flag);

	do_s16b(&p_ptr->msp, flag);
	do_s16b(&p_ptr->csp, flag);
	do_u16b(&p_ptr->csp_frac, flag);

	/* XXX
	   Here's where tank points were.
	   Those who run the estate of you-know-who is really stupid.
	   I'll never even consider reading her books now. -- neil */
	do_s16b(&tmp16s, flag);
	do_s16b(&tmp16s, flag);
	do_s16b(&tmp16s, flag);
	do_s16b(&tmp16s, flag);

	/* Gods */
	do_s32b(&p_ptr->grace, flag);
	do_byte((byte*)&p_ptr->praying, flag);
	do_s16b(&p_ptr->melkor_sacrifice, flag);
	do_byte(&p_ptr->pgod, flag);

	/* Contamination, by Amy, but originally from dnethack (weeping angels aka anti-fun squad) */
	do_s16b(&p_ptr->contamination, flag);

	/* Max Player and Dungeon Levels */
	do_s16b(&p_ptr->max_plv, flag);

	if (flag == LS_SAVE)
		tmp8u = max_d_idx;
	do_byte(&tmp8u, flag);
	for (i = 0; i < tmp8u; i++)
	{
		if (flag == LS_SAVE)
			tmp16s = max_dlv[i];
		do_s16b(&tmp16s, flag);
		if ((flag == LS_LOAD) && (i <= max_d_idx))
			max_dlv[i] = tmp16s;

		if (flag == LS_SAVE)
			tmp16s = max_dlv_real[i];
		do_s16b(&tmp16s, flag);
		if ((flag == LS_LOAD) && (i <= max_d_idx))
			max_dlv_real[i] = tmp16s;
	}
	/* Repair max player level??? */
	if ((flag == LS_LOAD) && (p_ptr->max_plv < p_ptr->lev))
		p_ptr->max_plv = p_ptr->lev;

	do_byte((byte*)&(p_ptr->help.enabled), flag);
	do_u32b(&(p_ptr->help.help1), flag);

	/* More info */
	tmp16s = 0;
	do_s16b(&p_ptr->sc, flag);
	do_s16b(&p_ptr->blind, flag);
	do_s16b(&p_ptr->paralyzed, flag);
	do_s16b(&p_ptr->confused, flag);
	do_s16b(&p_ptr->food, flag);
	do_s32b(&p_ptr->energy, flag);
	do_s16b(&p_ptr->fast, flag);
	do_s16b(&p_ptr->speed_factor, flag);
	do_s16b(&p_ptr->slow, flag);
	do_s16b(&p_ptr->afraid, flag);
	do_s16b(&p_ptr->cut, flag);
	do_s16b(&p_ptr->stun, flag);
	do_s16b(&p_ptr->poisoned, flag);
	do_s16b(&p_ptr->image, flag);
	do_s16b(&p_ptr->protevil, flag);
	do_s16b(&p_ptr->protundead, flag);
	do_s16b(&p_ptr->invuln, flag);
	do_s16b(&p_ptr->hero, flag);
	do_s16b(&p_ptr->shero, flag);
	do_s16b(&p_ptr->shield, flag);
	do_s16b(&p_ptr->shield_power, flag);
	do_s16b(&p_ptr->shield_power_opt, flag);
	do_s16b(&p_ptr->shield_power_opt2, flag);
	do_s16b(&p_ptr->shield_opt, flag);
	do_s16b(&p_ptr->blessed, flag);
	do_s16b(&p_ptr->control, flag);
	do_byte(&p_ptr->control_dir, flag);
	do_s16b(&p_ptr->tim_thunder, flag);
	do_s16b(&p_ptr->tim_thunder_p1, flag);
	do_s16b(&p_ptr->tim_thunder_p2, flag);
	do_s16b(&p_ptr->tim_ttempest, flag);
	do_s16b(&p_ptr->tim_ttempest_p1, flag);
	do_s16b(&p_ptr->tim_ttempest_p2, flag);
	do_s16b(&p_ptr->tim_project, flag);
	do_s16b(&p_ptr->tim_project_dam, flag);
	do_s16b(&p_ptr->tim_project_gf, flag);
	do_s16b(&p_ptr->tim_project_rad, flag);
	do_s16b(&p_ptr->tim_project_flag, flag);

	do_s16b(&p_ptr->tim_magic_breath, flag);
	do_s16b(&p_ptr->tim_water_breath, flag);

	do_s16b(&p_ptr->tim_roots, flag);
	do_s16b(&p_ptr->tim_roots_ac, flag);
	do_s16b(&p_ptr->tim_roots_dam, flag);

	do_s16b(&p_ptr->tim_invis, flag);
	do_s16b(&p_ptr->word_recall, flag);
	do_s16b(&p_ptr->recall_dungeon, flag);
	do_s16b(&p_ptr->see_infra, flag);
	do_s16b(&p_ptr->tim_infra, flag);
	do_s16b(&p_ptr->oppose_fire, flag);
	do_s16b(&p_ptr->oppose_cold, flag);
	do_s16b(&p_ptr->oppose_acid, flag);
	do_s16b(&p_ptr->oppose_elec, flag);
	do_s16b(&p_ptr->oppose_pois, flag);
	do_s16b(&p_ptr->oppose_ld, flag);
	do_s16b(&p_ptr->oppose_cc, flag);
	do_s16b(&p_ptr->oppose_ss, flag);
	do_s16b(&p_ptr->oppose_nex, flag);

	do_s16b(&p_ptr->tim_esp, flag);
	do_s16b(&p_ptr->tim_esp_animal, flag);
	do_s16b(&p_ptr->tim_peace, flag);
	do_s16b(&p_ptr->tim_device, flag);
	do_s16b(&p_ptr->tim_elemlull, flag);
	do_s16b(&p_ptr->am_elemlull, flag);
	do_s16b(&p_ptr->tim_magbreath, flag);
	do_s16b(&p_ptr->am_magbreath, flag);
	do_s16b(&p_ptr->tim_macarena, flag);
	do_s16b(&p_ptr->am_macarena, flag);
	do_s16b(&p_ptr->tim_savmus, flag);
	do_s16b(&p_ptr->am_savmus, flag);
	do_s16b(&p_ptr->tim_minersong, flag);
	do_s16b(&p_ptr->am_minersong, flag);
	do_s16b(&p_ptr->tim_stealthmus, flag);
	do_s16b(&p_ptr->am_stealthmus, flag);
	do_s16b(&p_ptr->tim_warboogie, flag);
	do_s16b(&p_ptr->am_warboogie, flag);
	do_s16b(&p_ptr->tim_tankmus, flag);
	do_s16b(&p_ptr->am_tankmus, flag);
	do_s16b(&p_ptr->tim_slaughtersong, flag);
	do_s16b(&p_ptr->am_slaughtersong, flag);
	do_s16b(&p_ptr->tim_mightymus, flag);
	do_s16b(&p_ptr->am_mightymus, flag);
	do_s16b(&p_ptr->tim_bullseye, flag);
	do_s16b(&p_ptr->tim_sniper, flag);
	do_s16b(&p_ptr->tim_rapidfire, flag);
	do_s16b(&p_ptr->tim_bombsquad, flag);
	do_s16b(&p_ptr->tim_dancing, flag);
	do_s16b(&p_ptr->am_dancing, flag);
	do_s16b(&p_ptr->tim_manavoid, flag);
	do_s16b(&p_ptr->tim_dragonblood_fire, flag);
	do_s16b(&p_ptr->tim_dragonblood_cold, flag);
	do_s16b(&p_ptr->tim_dragonblood_elec, flag);
	do_s16b(&p_ptr->tim_dragonblood_acid, flag);
	do_s16b(&p_ptr->tim_esp_pot, flag);
	do_s16b(&p_ptr->tim_levi_pot, flag);
	do_s16b(&p_ptr->tim_jolt_fa, flag);
	do_s16b(&p_ptr->tim_boost_pot, flag);
	do_s16b(&p_ptr->tim_luck_pot, flag);
	do_s16b(&p_ptr->tim_reshi_pot, flag);
	do_s16b(&p_ptr->tim_resrare_pot, flag);
	do_s16b(&p_ptr->tim_air_pot, flag);
	do_s16b(&p_ptr->tim_antimagic_pot, flag);
	do_s16b(&p_ptr->tim_timeflow_pot, flag);
	do_s16b(&p_ptr->tim_regen_pot, flag);
	do_s16b(&p_ptr->tim_power_pot, flag);
	do_s16b(&p_ptr->tim_infra_x, flag);
	do_s16b(&p_ptr->tim_infra_xx, flag);
	do_s16b(&p_ptr->tempres_fear, flag);
	do_s16b(&p_ptr->tempres_conf, flag);
	do_s16b(&p_ptr->tempres_hallu, flag);
	do_s16b(&p_ptr->tempres_blind, flag);
	do_s16b(&p_ptr->tim_manasilence, flag);
	do_s16b(&p_ptr->betweensickness, flag);
	do_s16b(&p_ptr->stairsickness, flag);
	do_s16b(&p_ptr->tim_repelling, flag);
	do_s16b(&p_ptr->tim_wraith, flag);
	do_s16b(&p_ptr->tim_ffall, flag);
	do_ver_s16b(&p_ptr->tim_fly, SAVEFILE_VERSION, 0, flag);
	do_s16b(&p_ptr->tim_fire_aura, flag);
	do_ver_s16b(&p_ptr->tim_poison, SAVEFILE_VERSION, 0, flag);
	do_s16b(&p_ptr->resist_magic, flag);
	do_s16b(&p_ptr->tim_invisible, flag);
	do_s16b(&p_ptr->tim_inv_pow, flag);
	do_s16b(&p_ptr->tim_mimic, flag);
	do_s16b(&p_ptr->lightspeed, flag);
	do_s16b(&p_ptr->tim_lite, flag);
	do_ver_s16b(&p_ptr->tim_regen, SAVEFILE_VERSION, 0, flag);
	do_ver_s16b(&p_ptr->tim_regen_pow, SAVEFILE_VERSION, 0, flag);
	do_s16b(&p_ptr->holy, flag);
	do_s16b(&p_ptr->walk_water, flag);
	do_s16b(&p_ptr->tim_mental_barrier, flag);
	do_s16b(&p_ptr->immov_cntr, flag);
	do_s16b(&p_ptr->strike, flag);
	do_s16b(&p_ptr->meditation, flag);
	do_s16b(&p_ptr->tim_reflect, flag);
	do_s16b(&p_ptr->tim_res_time, flag);
	do_s16b(&p_ptr->tim_deadly, flag);
	do_s16b(&p_ptr->prob_travel, flag);
	do_s16b(&p_ptr->disrupt_shield, flag);
	do_s16b(&p_ptr->parasite, flag);
	do_s16b(&p_ptr->parasite_r_idx, flag);
	do_s32b(&p_ptr->loan, flag);
	do_s32b(&p_ptr->loan_time, flag);
	do_s16b(&p_ptr->absorb_soul, flag);

	do_s16b(&p_ptr->chaos_patron, flag);

	do_s16b(&p_ptr->gained_demon, flag);
	do_s16b(&p_ptr->gained_preserv, flag);

	if (flag == LS_SAVE) tmp16s = max_corruptions;
	do_s16b(&tmp16s, flag);

	for (i = 0; i < tmp16s; i++)
	{
		if ((flag == LS_SAVE) && (i < max_corruptions))
			tmp8u = p_ptr->corruptions[i];

		do_byte(&tmp8u, flag);

		if ((flag == LS_LOAD) && (i < max_corruptions))
			p_ptr->corruptions[i] = tmp8u;
	}

	do_byte(&p_ptr->confusing, flag);
	do_byte((byte*)&p_ptr->black_breath, flag);
	do_byte((byte*)&fate_flag, flag);
	do_byte(&p_ptr->searching, flag);
	do_byte(&p_ptr->maximize, flag);
	do_byte(&p_ptr->lvling_system, flag);
	do_byte(&p_ptr->preserve, flag);
	do_byte(&p_ptr->hard_mode, flag);
	do_byte(&p_ptr->maso_mode, flag);
	do_byte(&p_ptr->special, flag);
	do_byte((byte*)&ambush_flag, flag);
	do_byte(&p_ptr->allow_one_death, flag);
	do_s16b(&p_ptr->xtra_spells, flag);

	do_byte(&vanilla_town, flag);

	do_u16b(&no_breeds, flag);
	do_s16b(&p_ptr->protgood, flag);

	/* Auxilliary variables */
	do_u32b(&p_ptr->mimic_extra, flag);
	do_u32b(&p_ptr->antimagic_extra, flag);
	do_u32b(&p_ptr->druid_extra, flag);
	do_u32b(&p_ptr->druid_extra2, flag);
	do_u32b(&p_ptr->druid_extra3, flag);
	do_u32b(&p_ptr->music_extra, flag);
	do_u32b(&p_ptr->music_extra2, flag);
	do_u32b(&p_ptr->necro_extra, flag);
	do_u32b(&p_ptr->necro_extra2, flag);

	do_u32b(&p_ptr->race_extra1, flag);
	do_u32b(&p_ptr->race_extra2, flag);
	do_u32b(&p_ptr->race_extra3, flag);
	do_u32b(&p_ptr->race_extra4, flag);
	do_u32b(&p_ptr->race_extra5, flag);
	do_u32b(&p_ptr->race_extra6, flag);
	do_u32b(&p_ptr->race_extra7, flag);

	do_u16b(&p_ptr->body_monster, flag);
	do_byte((byte*)&p_ptr->disembodied, flag);

	do_byte((byte*)&p_ptr->nastytrap1, flag);
	do_byte((byte*)&p_ptr->nastytrap2, flag);
	do_byte((byte*)&p_ptr->nastytrap3, flag);
	do_byte((byte*)&p_ptr->nastytrap4, flag);
	do_byte((byte*)&p_ptr->nastytrap5, flag);
	do_byte((byte*)&p_ptr->nastytrap6, flag);
	do_byte((byte*)&p_ptr->nastytrap7, flag);
	do_byte((byte*)&p_ptr->nastytrap8, flag);
	do_byte((byte*)&p_ptr->nastytrap9, flag);
	do_byte((byte*)&p_ptr->nastytrap10, flag);
	do_byte((byte*)&p_ptr->nastytrap11, flag);
	do_byte((byte*)&p_ptr->nastytrap12, flag);
	do_byte((byte*)&p_ptr->nastytrap13, flag);
	do_byte((byte*)&p_ptr->nastytrap14, flag);
	do_byte((byte*)&p_ptr->nastytrap15, flag);
	do_byte((byte*)&p_ptr->nastytrap16, flag);
	do_byte((byte*)&p_ptr->nastytrap17, flag);
	do_byte((byte*)&p_ptr->nastytrap18, flag);
	do_byte((byte*)&p_ptr->nastytrap19, flag);
	do_byte((byte*)&p_ptr->nastytrap20, flag);
	do_byte((byte*)&p_ptr->nastytrap21, flag);
	do_byte((byte*)&p_ptr->nastytrap22, flag);
	do_byte((byte*)&p_ptr->nastytrap23, flag);
	do_byte((byte*)&p_ptr->nastytrap24, flag);
	do_byte((byte*)&p_ptr->nastytrap25, flag);
	do_byte((byte*)&p_ptr->nastytrap26, flag);
	do_byte((byte*)&p_ptr->nastytrap27, flag);
	do_byte((byte*)&p_ptr->nastytrap28, flag);
	do_byte((byte*)&p_ptr->nastytrap29, flag);
	do_byte((byte*)&p_ptr->nastytrap30, flag);
	do_byte((byte*)&p_ptr->nastytrap31, flag);
	do_byte((byte*)&p_ptr->nastytrap32, flag);
	do_byte((byte*)&p_ptr->nastytrap33, flag);
	do_byte((byte*)&p_ptr->nastytrap34, flag);
	do_byte((byte*)&p_ptr->nastytrap35, flag);
	do_byte((byte*)&p_ptr->nastytrap36, flag);
	do_byte((byte*)&p_ptr->nastytrap37, flag);
	do_byte((byte*)&p_ptr->nastytrap38, flag);
	do_byte((byte*)&p_ptr->nastytrap39, flag);
	do_byte((byte*)&p_ptr->nastytrap40, flag);
	do_byte((byte*)&p_ptr->nastytrap41, flag);
	do_byte((byte*)&p_ptr->nastytrap42, flag);
	do_byte((byte*)&p_ptr->nastytrap43, flag);
	do_byte((byte*)&p_ptr->nastytrap44, flag);
	do_byte((byte*)&p_ptr->nastytrap45, flag);
	do_byte((byte*)&p_ptr->nastytrap46, flag);
	do_byte((byte*)&p_ptr->nastytrap47, flag);
	do_byte((byte*)&p_ptr->nastytrap48, flag);
	do_byte((byte*)&p_ptr->nastytrap49, flag);
	do_byte((byte*)&p_ptr->nastytrap50, flag);
	do_byte((byte*)&p_ptr->nastytrap51, flag);
	do_byte((byte*)&p_ptr->nastytrap52, flag);
	do_byte((byte*)&p_ptr->nastytrap53, flag);
	do_byte((byte*)&p_ptr->nastytrap54, flag);
	do_byte((byte*)&p_ptr->nastytrap55, flag);
	do_byte((byte*)&p_ptr->nastytrap56, flag);
	do_byte((byte*)&p_ptr->nastytrap57, flag);
	do_byte((byte*)&p_ptr->nastytrap58, flag);
	do_byte((byte*)&p_ptr->nastytrap59, flag);
	do_byte((byte*)&p_ptr->nastytrap60, flag);
	do_byte((byte*)&p_ptr->nastytrap61, flag);
	do_byte((byte*)&p_ptr->nastytrap62, flag);
	do_byte((byte*)&p_ptr->nastytrap63, flag);
	do_byte((byte*)&p_ptr->nastytrap64, flag);
	do_byte((byte*)&p_ptr->nastytrap65, flag);
	do_byte((byte*)&p_ptr->nastytrap66, flag);
	do_byte((byte*)&p_ptr->nastytrap67, flag);
	do_byte((byte*)&p_ptr->nastytrap68, flag);
	do_byte((byte*)&p_ptr->nastytrap69, flag);
	do_byte((byte*)&p_ptr->nastytrap70, flag);
	do_byte((byte*)&p_ptr->nastytrap71, flag);
	do_byte((byte*)&p_ptr->nastytrap72, flag);
	do_byte((byte*)&p_ptr->nastytrap73, flag);
	do_byte((byte*)&p_ptr->nastytrap74, flag);
	do_byte((byte*)&p_ptr->nastytrap75, flag);
	do_byte((byte*)&p_ptr->nastytrap76, flag);
	do_byte((byte*)&p_ptr->nastytrap77, flag);
	do_byte((byte*)&p_ptr->nastytrap78, flag);
	do_byte((byte*)&p_ptr->nastytrap79, flag);
	do_byte((byte*)&p_ptr->nastytrap80, flag);
	do_byte((byte*)&p_ptr->nastytrap81, flag);
	do_byte((byte*)&p_ptr->nastytrap82, flag);
	do_byte((byte*)&p_ptr->nastytrap83, flag);
	do_byte((byte*)&p_ptr->nastytrap84, flag);
	do_byte((byte*)&p_ptr->nastytrap85, flag);
	do_byte((byte*)&p_ptr->nastytrap86, flag);
	do_byte((byte*)&p_ptr->nastytrap87, flag);
	do_byte((byte*)&p_ptr->nastytrap88, flag);
	do_byte((byte*)&p_ptr->nastytrap89, flag);
	do_byte((byte*)&p_ptr->nastytrap90, flag);
	do_byte((byte*)&p_ptr->nastytrap91, flag);
	do_byte((byte*)&p_ptr->nastytrap92, flag);
	do_byte((byte*)&p_ptr->nastytrap93, flag);
	do_byte((byte*)&p_ptr->nastytrap94, flag);
	do_byte((byte*)&p_ptr->nastytrap95, flag);
	do_byte((byte*)&p_ptr->nastytrap96, flag);
	do_byte((byte*)&p_ptr->nastytrap97, flag);
	do_byte((byte*)&p_ptr->nastytrap98, flag);
	do_byte((byte*)&p_ptr->nastytrap99, flag);
	do_byte((byte*)&p_ptr->nastytrap100, flag);
	do_byte((byte*)&p_ptr->nastytrap101, flag);
	do_byte((byte*)&p_ptr->nastytrap102, flag);
	do_byte((byte*)&p_ptr->nastytrap103, flag);
	do_byte((byte*)&p_ptr->nastytrap104, flag);
	do_byte((byte*)&p_ptr->nastytrap105, flag);
	do_byte((byte*)&p_ptr->nastytrap106, flag);
	do_byte((byte*)&p_ptr->nastytrap107, flag);
	do_byte((byte*)&p_ptr->nastytrap108, flag);
	do_byte((byte*)&p_ptr->nastytrap109, flag);
	do_byte((byte*)&p_ptr->nastytrap110, flag);
	do_byte((byte*)&p_ptr->nastytrap111, flag);
	do_byte((byte*)&p_ptr->nastytrap112, flag);
	do_byte((byte*)&p_ptr->nastytrap113, flag);
	do_byte((byte*)&p_ptr->nastytrap114, flag);
	do_byte((byte*)&p_ptr->nastytrap115, flag);
	do_byte((byte*)&p_ptr->nastytrap116, flag);
	do_byte((byte*)&p_ptr->nastytrap117, flag);
	do_byte((byte*)&p_ptr->nastytrap118, flag);
	do_byte((byte*)&p_ptr->nastytrap119, flag);
	do_byte((byte*)&p_ptr->nastytrap120, flag);
	do_byte((byte*)&p_ptr->nastytrap121, flag);
	do_byte((byte*)&p_ptr->nastytrap122, flag);
	do_byte((byte*)&p_ptr->nastytrap123, flag);
	do_byte((byte*)&p_ptr->nastytrap124, flag);
	do_byte((byte*)&p_ptr->nastytrap125, flag);
	do_byte((byte*)&p_ptr->nastytrap126, flag);
	do_byte((byte*)&p_ptr->nastytrap127, flag);
	do_byte((byte*)&p_ptr->nastytrap128, flag);
	do_byte((byte*)&p_ptr->nastytrap129, flag);
	do_byte((byte*)&p_ptr->nastytrap130, flag);
	do_byte((byte*)&p_ptr->nastytrap131, flag);
	do_byte((byte*)&p_ptr->nastytrap132, flag);
	do_byte((byte*)&p_ptr->nastytrap133, flag);
	do_byte((byte*)&p_ptr->nastytrap134, flag);
	do_byte((byte*)&p_ptr->nastytrap135, flag);
	do_byte((byte*)&p_ptr->nastytrap136, flag);
	do_byte((byte*)&p_ptr->nastytrap137, flag);
	do_byte((byte*)&p_ptr->nastytrap138, flag);
	do_byte((byte*)&p_ptr->nastytrap139, flag);
	do_byte((byte*)&p_ptr->nastytrap140, flag);
	do_byte((byte*)&p_ptr->nastytrap141, flag);
	do_byte((byte*)&p_ptr->nastytrap142, flag);
	do_byte((byte*)&p_ptr->nastytrap143, flag);
	do_byte((byte*)&p_ptr->nastytrap144, flag);
	do_byte((byte*)&p_ptr->nastytrap145, flag);
	do_byte((byte*)&p_ptr->nastytrap146, flag);
	do_byte((byte*)&p_ptr->nastytrap147, flag);
	do_byte((byte*)&p_ptr->nastytrap148, flag);
	do_byte((byte*)&p_ptr->nastytrap149, flag);
	do_byte((byte*)&p_ptr->nastytrap150, flag);
	do_byte((byte*)&p_ptr->nastytrap151, flag);
	do_byte((byte*)&p_ptr->nastytrap152, flag);
	do_byte((byte*)&p_ptr->nastytrap153, flag);
	do_byte((byte*)&p_ptr->nastytrap154, flag);
	do_byte((byte*)&p_ptr->nastytrap155, flag);
	do_byte((byte*)&p_ptr->nastytrap156, flag);
	do_byte((byte*)&p_ptr->nastytrap157, flag);
	do_byte((byte*)&p_ptr->nastytrap158, flag);
	do_byte((byte*)&p_ptr->nastytrap159, flag);
	do_byte((byte*)&p_ptr->nastytrap160, flag);
	do_byte((byte*)&p_ptr->nastytrap161, flag);
	do_byte((byte*)&p_ptr->nastytrap162, flag);
	do_byte((byte*)&p_ptr->nastytrap163, flag);
	do_byte((byte*)&p_ptr->nastytrap164, flag);
	do_byte((byte*)&p_ptr->nastytrap165, flag);
	do_byte((byte*)&p_ptr->nastytrap166, flag);
	do_byte((byte*)&p_ptr->nastytrap167, flag);
	do_byte((byte*)&p_ptr->nastytrap168, flag);
	do_byte((byte*)&p_ptr->nastytrap169, flag);
	do_byte((byte*)&p_ptr->nastytrap170, flag);
	do_byte((byte*)&p_ptr->nastytrap171, flag);
	do_byte((byte*)&p_ptr->nastytrap172, flag);
	do_byte((byte*)&p_ptr->nastytrap173, flag);
	do_byte((byte*)&p_ptr->nastytrap174, flag);
	do_byte((byte*)&p_ptr->nastytrap175, flag);
	do_byte((byte*)&p_ptr->nastytrap176, flag);
	do_byte((byte*)&p_ptr->nastytrap177, flag);
	do_byte((byte*)&p_ptr->nastytrap178, flag);
	do_byte((byte*)&p_ptr->nastytrap179, flag);
	do_byte((byte*)&p_ptr->nastytrap180, flag);
	do_byte((byte*)&p_ptr->nastytrap181, flag);
	do_byte((byte*)&p_ptr->nastytrap182, flag);
	do_byte((byte*)&p_ptr->nastytrap183, flag);
	do_byte((byte*)&p_ptr->nastytrap184, flag);
	do_byte((byte*)&p_ptr->nastytrap185, flag);
	do_byte((byte*)&p_ptr->nastytrap186, flag);
	do_byte((byte*)&p_ptr->nastytrap187, flag);
	do_byte((byte*)&p_ptr->nastytrap188, flag);
	do_byte((byte*)&p_ptr->nastytrap189, flag);
	do_byte((byte*)&p_ptr->nastytrap190, flag);
	do_byte((byte*)&p_ptr->nastytrap191, flag);
	do_byte((byte*)&p_ptr->nastytrap192, flag);
	do_byte((byte*)&p_ptr->nastytrap193, flag);
	do_byte((byte*)&p_ptr->nastytrap194, flag);
	do_byte((byte*)&p_ptr->nastytrap195, flag);
	do_byte((byte*)&p_ptr->nastytrap196, flag);
	do_byte((byte*)&p_ptr->nastytrap197, flag);
	do_byte((byte*)&p_ptr->nastytrap198, flag);
	do_byte((byte*)&p_ptr->nastytrap199, flag);
	do_byte((byte*)&p_ptr->nastytrap200, flag);
	do_byte((byte*)&p_ptr->nastytrap201, flag);
	do_byte((byte*)&p_ptr->nastytrap202, flag);
	do_byte((byte*)&p_ptr->nastytrap203, flag);
	do_byte((byte*)&p_ptr->nastytrap204, flag);
	do_byte((byte*)&p_ptr->nastytrap205, flag);
	do_byte((byte*)&p_ptr->nastytrap206, flag);
	do_byte((byte*)&p_ptr->nastytrap207, flag);
	do_byte((byte*)&p_ptr->nastytrap208, flag);
	do_byte((byte*)&p_ptr->nastytrap209, flag);
	do_byte((byte*)&p_ptr->nastytrap210, flag);
	do_byte((byte*)&p_ptr->nastytrap211, flag);
	do_byte((byte*)&p_ptr->nastytrap212, flag);
	do_byte((byte*)&p_ptr->nastytrap213, flag);
	do_byte((byte*)&p_ptr->nastytrap214, flag);
	do_byte((byte*)&p_ptr->nastytrap215, flag);
	do_byte((byte*)&p_ptr->nastytrap216, flag);
	do_byte((byte*)&p_ptr->nastytrap217, flag);
	do_byte((byte*)&p_ptr->nastytrap218, flag);
	do_byte((byte*)&p_ptr->nastytrap219, flag);
	do_byte((byte*)&p_ptr->nastytrap220, flag);
	do_byte((byte*)&p_ptr->nastytrap221, flag);
	do_byte((byte*)&p_ptr->nastytrap222, flag);
	do_byte((byte*)&p_ptr->nastytrap223, flag);
	do_byte((byte*)&p_ptr->nastytrap224, flag);
	do_byte((byte*)&p_ptr->nastytrap225, flag);
	do_byte((byte*)&p_ptr->nastytrap226, flag);
	do_byte((byte*)&p_ptr->nastytrap227, flag);
	do_byte((byte*)&p_ptr->nastytrap228, flag);
	do_byte((byte*)&p_ptr->nastytrap229, flag);
	do_byte((byte*)&p_ptr->nastytrap230, flag);
	do_byte((byte*)&p_ptr->nastytrap231, flag);
	do_byte((byte*)&p_ptr->nastytrap232, flag);
	do_byte((byte*)&p_ptr->nastytrap233, flag);
	do_byte((byte*)&p_ptr->nastytrap234, flag);
	do_byte((byte*)&p_ptr->nastytrap235, flag);
	do_byte((byte*)&p_ptr->nastytrap236, flag);
	do_byte((byte*)&p_ptr->nastytrap237, flag);
	do_byte((byte*)&p_ptr->nastytrap238, flag);
	do_byte((byte*)&p_ptr->nastytrap239, flag);
	do_byte((byte*)&p_ptr->nastytrap240, flag);
	do_byte((byte*)&p_ptr->nastytrap241, flag);
	do_byte((byte*)&p_ptr->nastytrap242, flag);
	do_byte((byte*)&p_ptr->nastytrap243, flag);
	do_byte((byte*)&p_ptr->nastytrap244, flag);
	do_byte((byte*)&p_ptr->nastytrap245, flag);
	do_byte((byte*)&p_ptr->nastytrap246, flag);
	do_byte((byte*)&p_ptr->nastytrap247, flag);
	do_byte((byte*)&p_ptr->nastytrap248, flag);
	do_byte((byte*)&p_ptr->nastytrap249, flag);
	do_byte((byte*)&p_ptr->nastytrap250, flag);
	do_byte((byte*)&p_ptr->nastytrap251, flag);
	do_byte((byte*)&p_ptr->nastytrap252, flag);
	do_byte((byte*)&p_ptr->nastytrap253, flag);
	do_byte((byte*)&p_ptr->nastytrap254, flag);
	do_byte((byte*)&p_ptr->nastytrap255, flag);
	do_byte((byte*)&p_ptr->nastytrap256, flag);
	do_byte((byte*)&p_ptr->nastytrap257, flag);
	do_byte((byte*)&p_ptr->nastytrap258, flag);
	do_byte((byte*)&p_ptr->nastytrap259, flag);
	do_byte((byte*)&p_ptr->nastytrap260, flag);
	do_byte((byte*)&p_ptr->nastytrap261, flag);
	do_byte((byte*)&p_ptr->nastytrap262, flag);
	do_byte((byte*)&p_ptr->nastytrap263, flag);
	do_byte((byte*)&p_ptr->nastytrap264, flag);
	do_byte((byte*)&p_ptr->nastytrap265, flag);
	do_byte((byte*)&p_ptr->nastytrap266, flag);
	do_byte((byte*)&p_ptr->nastytrap267, flag);
	do_byte((byte*)&p_ptr->nastytrap268, flag);
	do_byte((byte*)&p_ptr->nastytrap269, flag);
	do_byte((byte*)&p_ptr->nastytrap270, flag);
	do_byte((byte*)&p_ptr->nastytrap271, flag);
	do_byte((byte*)&p_ptr->nastytrap272, flag);
	do_byte((byte*)&p_ptr->nastytrap273, flag);
	do_byte((byte*)&p_ptr->nastytrap274, flag);
	do_byte((byte*)&p_ptr->nastytrap275, flag);
	do_byte((byte*)&p_ptr->nastytrap276, flag);
	do_byte((byte*)&p_ptr->nastytrap277, flag);
	do_byte((byte*)&p_ptr->nastytrap278, flag);
	do_byte((byte*)&p_ptr->nastytrap279, flag);
	do_byte((byte*)&p_ptr->nastytrap280, flag);
	do_byte((byte*)&p_ptr->nastytrap281, flag);
	do_byte((byte*)&p_ptr->nastytrap282, flag);
	do_byte((byte*)&p_ptr->nastytrap283, flag);
	do_byte((byte*)&p_ptr->nastytrap284, flag);
	do_byte((byte*)&p_ptr->nastytrap285, flag);
	do_byte((byte*)&p_ptr->nastytrap286, flag);
	do_byte((byte*)&p_ptr->nastytrap287, flag);
	do_byte((byte*)&p_ptr->nastytrap288, flag);
	do_byte((byte*)&p_ptr->nastytrap289, flag);
	do_byte((byte*)&p_ptr->nastytrap290, flag);
	do_byte((byte*)&p_ptr->nastytrap291, flag);
	do_byte((byte*)&p_ptr->nastytrap292, flag);
	do_byte((byte*)&p_ptr->nastytrap293, flag);
	do_byte((byte*)&p_ptr->nastytrap294, flag);
	do_byte((byte*)&p_ptr->nastytrap295, flag);
	do_byte((byte*)&p_ptr->nastytrap296, flag);
	do_byte((byte*)&p_ptr->nastytrap297, flag);
	do_byte((byte*)&p_ptr->nastytrap298, flag);
	do_byte((byte*)&p_ptr->nastytrap299, flag);
	do_byte((byte*)&p_ptr->nastytrap300, flag);
	do_byte((byte*)&p_ptr->nastytrap301, flag);
	do_byte((byte*)&p_ptr->nastytrap302, flag);
	do_byte((byte*)&p_ptr->nastytrap303, flag);
	do_byte((byte*)&p_ptr->nastytrap304, flag);
	do_byte((byte*)&p_ptr->nastytrap305, flag);
	do_byte((byte*)&p_ptr->nastytrap306, flag);
	do_byte((byte*)&p_ptr->nastytrap307, flag);
	do_byte((byte*)&p_ptr->nastytrap308, flag);
	do_byte((byte*)&p_ptr->nastytrap309, flag);
	do_byte((byte*)&p_ptr->nastytrap310, flag);
	do_byte((byte*)&p_ptr->nastytrap311, flag);
	do_byte((byte*)&p_ptr->nastytrap312, flag);
	do_byte((byte*)&p_ptr->nastytrap313, flag);
	do_byte((byte*)&p_ptr->nastytrap314, flag);
	do_byte((byte*)&p_ptr->nastytrap315, flag);
	do_byte((byte*)&p_ptr->nastytrap316, flag);
	do_byte((byte*)&p_ptr->nastytrap317, flag);
	do_byte((byte*)&p_ptr->nastytrap318, flag);
	do_byte((byte*)&p_ptr->nastytrap319, flag);
	do_byte((byte*)&p_ptr->nastytrap320, flag);
	do_byte((byte*)&p_ptr->nastytrap321, flag);
	do_byte((byte*)&p_ptr->nastytrap322, flag);
	do_byte((byte*)&p_ptr->nastytrap323, flag);
	do_byte((byte*)&p_ptr->nastytrap324, flag);
	do_byte((byte*)&p_ptr->nastytrap325, flag);
	do_byte((byte*)&p_ptr->nastytrap326, flag);
	do_byte((byte*)&p_ptr->nastytrap327, flag);
	do_byte((byte*)&p_ptr->nastytrap328, flag);
	do_byte((byte*)&p_ptr->nastytrap329, flag);
	do_byte((byte*)&p_ptr->nastytrap330, flag);
	do_byte((byte*)&p_ptr->nastytrap331, flag);
	do_byte((byte*)&p_ptr->nastytrap332, flag);
	do_byte((byte*)&p_ptr->nastytrap333, flag);
	do_byte((byte*)&p_ptr->nastytrap334, flag);
	do_byte((byte*)&p_ptr->nastytrap335, flag);
	do_byte((byte*)&p_ptr->nastytrap336, flag);
	do_byte((byte*)&p_ptr->nastytrap337, flag);
	do_byte((byte*)&p_ptr->nastytrap338, flag);
	do_byte((byte*)&p_ptr->nastytrap339, flag);
	do_byte((byte*)&p_ptr->nastytrap340, flag);
	do_byte((byte*)&p_ptr->nastytrap341, flag);
	do_byte((byte*)&p_ptr->nastytrap342, flag);
	do_byte((byte*)&p_ptr->nastytrap343, flag);
	do_byte((byte*)&p_ptr->nastytrap344, flag);
	do_byte((byte*)&p_ptr->nastytrap345, flag);
	do_byte((byte*)&p_ptr->nastytrap346, flag);
	do_byte((byte*)&p_ptr->nastytrap347, flag);
	do_byte((byte*)&p_ptr->nastytrap348, flag);
	do_byte((byte*)&p_ptr->nastytrap349, flag);
	do_byte((byte*)&p_ptr->nastytrap350, flag);
	do_byte((byte*)&p_ptr->nastytrap351, flag);
	do_byte((byte*)&p_ptr->nastytrap352, flag);
	do_byte((byte*)&p_ptr->nastytrap353, flag);
	do_byte((byte*)&p_ptr->nastytrap354, flag);
	do_byte((byte*)&p_ptr->nastytrap355, flag);
	do_byte((byte*)&p_ptr->nastytrap356, flag);
	do_byte((byte*)&p_ptr->nastytrap357, flag);
	do_byte((byte*)&p_ptr->nastytrap358, flag);
	do_byte((byte*)&p_ptr->nastytrap359, flag);
	do_byte((byte*)&p_ptr->nastytrap360, flag);
	do_byte((byte*)&p_ptr->nastytrap361, flag);
	do_byte((byte*)&p_ptr->nastytrap362, flag);
	do_byte((byte*)&p_ptr->nastytrap363, flag);
	do_byte((byte*)&p_ptr->nastytrap364, flag);
	do_byte((byte*)&p_ptr->nastytrap365, flag);
	do_byte((byte*)&p_ptr->nastytrap366, flag);
	do_byte((byte*)&p_ptr->nastytrap367, flag);
	do_byte((byte*)&p_ptr->nastytrap368, flag);
	do_byte((byte*)&p_ptr->nastytrap369, flag);
	do_byte((byte*)&p_ptr->nastytrap370, flag);
	do_byte((byte*)&p_ptr->nastytrap371, flag);
	do_byte((byte*)&p_ptr->nastytrap372, flag);
	do_byte((byte*)&p_ptr->nastytrap373, flag);
	do_byte((byte*)&p_ptr->nastytrap374, flag);
	do_byte((byte*)&p_ptr->nastytrap375, flag);
	do_byte((byte*)&p_ptr->nastytrap376, flag);
	do_byte((byte*)&p_ptr->nastytrap377, flag);
	do_byte((byte*)&p_ptr->nastytrap378, flag);
	do_byte((byte*)&p_ptr->nastytrap379, flag);
	do_byte((byte*)&p_ptr->nastytrap380, flag);
	do_byte((byte*)&p_ptr->nastytrap381, flag);
	do_byte((byte*)&p_ptr->nastytrap382, flag);
	do_byte((byte*)&p_ptr->nastytrap383, flag);
	do_byte((byte*)&p_ptr->nastytrap384, flag);
	do_byte((byte*)&p_ptr->nastytrap385, flag);
	do_byte((byte*)&p_ptr->nastytrap386, flag);
	do_byte((byte*)&p_ptr->nastytrap387, flag);
	do_byte((byte*)&p_ptr->nastytrap388, flag);
	do_byte((byte*)&p_ptr->nastytrap389, flag);
	do_byte((byte*)&p_ptr->nastytrap390, flag);
	do_byte((byte*)&p_ptr->nastytrap391, flag);
	do_byte((byte*)&p_ptr->nastytrap392, flag);
	do_byte((byte*)&p_ptr->nastytrap393, flag);
	do_byte((byte*)&p_ptr->nastytrap394, flag);
	do_byte((byte*)&p_ptr->nastytrap395, flag);
	do_byte((byte*)&p_ptr->nastytrap396, flag);
	do_byte((byte*)&p_ptr->nastytrap397, flag);
	do_byte((byte*)&p_ptr->nastytrap398, flag);
	do_byte((byte*)&p_ptr->nastytrap399, flag);
	do_byte((byte*)&p_ptr->nastytrap400, flag);
	do_byte((byte*)&p_ptr->nastytrap401, flag);
	do_byte((byte*)&p_ptr->nastytrap402, flag);
	do_byte((byte*)&p_ptr->nastytrap403, flag);
	do_byte((byte*)&p_ptr->nastytrap404, flag);
	do_byte((byte*)&p_ptr->nastytrap405, flag);
	do_byte((byte*)&p_ptr->nastytrap406, flag);
	do_byte((byte*)&p_ptr->nastytrap407, flag);
	do_byte((byte*)&p_ptr->nastytrap408, flag);
	do_byte((byte*)&p_ptr->nastytrap409, flag);
	do_byte((byte*)&p_ptr->nastytrap410, flag);
	do_byte((byte*)&p_ptr->nastytrap411, flag);
	do_byte((byte*)&p_ptr->nastytrap412, flag);
	do_byte((byte*)&p_ptr->nastytrap413, flag);
	do_byte((byte*)&p_ptr->nastytrap414, flag);
	do_byte((byte*)&p_ptr->nastytrap415, flag);
	do_byte((byte*)&p_ptr->nastytrap416, flag);
	do_byte((byte*)&p_ptr->nastytrap417, flag);
	do_byte((byte*)&p_ptr->nastytrap418, flag);
	do_byte((byte*)&p_ptr->nastytrap419, flag);
	do_byte((byte*)&p_ptr->nastytrap420, flag);
	do_byte((byte*)&p_ptr->nastytrap421, flag);
	do_byte((byte*)&p_ptr->nastytrap422, flag);
	do_byte((byte*)&p_ptr->nastytrap423, flag);
	do_byte((byte*)&p_ptr->nastytrap424, flag);
	do_byte((byte*)&p_ptr->nastytrap425, flag);
	do_byte((byte*)&p_ptr->nastytrap426, flag);
	do_byte((byte*)&p_ptr->nastytrap427, flag);
	do_byte((byte*)&p_ptr->nastytrap428, flag);
	do_byte((byte*)&p_ptr->nastytrap429, flag);
	do_byte((byte*)&p_ptr->nastytrap430, flag);
	do_byte((byte*)&p_ptr->nastytrap431, flag);
	do_byte((byte*)&p_ptr->nastytrap432, flag);
	do_byte((byte*)&p_ptr->nastytrap433, flag);
	do_byte((byte*)&p_ptr->nastytrap434, flag);
	do_byte((byte*)&p_ptr->nastytrap435, flag);
	do_byte((byte*)&p_ptr->nastytrap436, flag);
	do_byte((byte*)&p_ptr->nastytrap437, flag);
	do_byte((byte*)&p_ptr->nastytrap438, flag);
	do_byte((byte*)&p_ptr->nastytrap439, flag);
	do_byte((byte*)&p_ptr->nastytrap440, flag);
	do_byte((byte*)&p_ptr->nastytrap441, flag);
	do_byte((byte*)&p_ptr->nastytrap442, flag);
	do_byte((byte*)&p_ptr->nastytrap443, flag);
	do_byte((byte*)&p_ptr->nastytrap444, flag);
	do_byte((byte*)&p_ptr->nastytrap445, flag);
	do_byte((byte*)&p_ptr->nastytrap446, flag);
	do_byte((byte*)&p_ptr->nastytrap447, flag);
	do_byte((byte*)&p_ptr->nastytrap448, flag);
	do_byte((byte*)&p_ptr->nastytrap449, flag);
	do_byte((byte*)&p_ptr->nastytrap450, flag);
	do_byte((byte*)&p_ptr->nastytrap451, flag);
	do_byte((byte*)&p_ptr->nastytrap452, flag);
	do_byte((byte*)&p_ptr->nastytrap453, flag);
	do_byte((byte*)&p_ptr->nastytrap454, flag);
	do_byte((byte*)&p_ptr->nastytrap455, flag);
	do_byte((byte*)&p_ptr->nastytrap456, flag);
	do_byte((byte*)&p_ptr->nastytrap457, flag);
	do_byte((byte*)&p_ptr->nastytrap458, flag);
	do_byte((byte*)&p_ptr->nastytrap459, flag);
	do_byte((byte*)&p_ptr->nastytrap460, flag);
	do_byte((byte*)&p_ptr->nastytrap461, flag);
	do_byte((byte*)&p_ptr->nastytrap462, flag);
	do_byte((byte*)&p_ptr->nastytrap463, flag);
	do_byte((byte*)&p_ptr->nastytrap464, flag);
	do_byte((byte*)&p_ptr->nastytrap465, flag);
	do_byte((byte*)&p_ptr->nastytrap466, flag);
	do_byte((byte*)&p_ptr->nastytrap467, flag);
	do_byte((byte*)&p_ptr->nastytrap468, flag);
	do_byte((byte*)&p_ptr->nastytrap469, flag);
	do_byte((byte*)&p_ptr->nastytrap470, flag);
	do_byte((byte*)&p_ptr->nastytrap471, flag);
	do_byte((byte*)&p_ptr->nastytrap472, flag);
	do_byte((byte*)&p_ptr->nastytrap473, flag);
	do_byte((byte*)&p_ptr->nastytrap474, flag);
	do_byte((byte*)&p_ptr->nastytrap475, flag);
	do_byte((byte*)&p_ptr->nastytrap476, flag);
	do_byte((byte*)&p_ptr->nastytrap477, flag);
	do_byte((byte*)&p_ptr->nastytrap478, flag);
	do_byte((byte*)&p_ptr->nastytrap479, flag);
	do_byte((byte*)&p_ptr->nastytrap480, flag);
	do_byte((byte*)&p_ptr->nastytrap481, flag);
	do_byte((byte*)&p_ptr->nastytrap482, flag);
	do_byte((byte*)&p_ptr->nastytrap483, flag);
	do_byte((byte*)&p_ptr->nastytrap484, flag);
	do_byte((byte*)&p_ptr->nastytrap485, flag);
	do_byte((byte*)&p_ptr->nastytrap486, flag);
	do_byte((byte*)&p_ptr->nastytrap487, flag);
	do_byte((byte*)&p_ptr->nastytrap488, flag);
	do_byte((byte*)&p_ptr->nastytrap489, flag);
	do_byte((byte*)&p_ptr->nastytrap490, flag);
	do_byte((byte*)&p_ptr->nastytrap491, flag);
	do_byte((byte*)&p_ptr->nastytrap492, flag);
	do_byte((byte*)&p_ptr->nastytrap493, flag);
	do_byte((byte*)&p_ptr->nastytrap494, flag);
	do_byte((byte*)&p_ptr->nastytrap495, flag);
	do_byte((byte*)&p_ptr->nastytrap496, flag);
	do_byte((byte*)&p_ptr->nastytrap497, flag);
	do_byte((byte*)&p_ptr->nastytrap498, flag);
	do_byte((byte*)&p_ptr->nastytrap499, flag);
	do_byte((byte*)&p_ptr->nastytrap500, flag);

	do_byte((byte*)&p_ptr->eternalnastytrap1, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap2, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap3, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap4, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap5, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap6, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap7, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap8, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap9, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap10, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap11, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap12, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap13, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap14, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap15, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap16, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap17, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap18, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap19, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap20, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap21, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap22, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap23, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap24, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap25, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap26, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap27, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap28, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap29, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap30, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap31, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap32, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap33, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap34, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap35, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap36, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap37, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap38, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap39, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap40, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap41, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap42, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap43, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap44, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap45, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap46, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap47, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap48, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap49, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap50, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap51, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap52, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap53, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap54, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap55, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap56, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap57, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap58, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap59, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap60, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap61, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap62, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap63, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap64, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap65, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap66, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap67, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap68, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap69, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap70, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap71, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap72, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap73, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap74, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap75, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap76, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap77, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap78, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap79, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap80, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap81, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap82, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap83, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap84, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap85, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap86, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap87, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap88, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap89, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap90, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap91, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap92, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap93, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap94, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap95, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap96, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap97, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap98, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap99, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap100, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap101, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap102, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap103, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap104, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap105, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap106, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap107, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap108, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap109, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap110, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap111, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap112, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap113, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap114, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap115, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap116, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap117, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap118, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap119, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap120, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap121, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap122, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap123, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap124, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap125, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap126, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap127, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap128, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap129, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap130, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap131, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap132, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap133, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap134, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap135, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap136, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap137, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap138, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap139, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap140, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap141, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap142, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap143, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap144, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap145, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap146, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap147, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap148, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap149, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap150, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap151, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap152, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap153, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap154, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap155, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap156, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap157, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap158, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap159, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap160, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap161, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap162, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap163, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap164, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap165, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap166, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap167, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap168, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap169, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap170, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap171, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap172, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap173, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap174, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap175, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap176, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap177, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap178, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap179, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap180, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap181, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap182, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap183, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap184, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap185, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap186, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap187, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap188, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap189, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap190, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap191, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap192, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap193, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap194, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap195, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap196, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap197, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap198, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap199, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap200, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap201, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap202, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap203, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap204, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap205, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap206, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap207, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap208, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap209, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap210, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap211, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap212, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap213, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap214, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap215, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap216, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap217, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap218, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap219, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap220, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap221, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap222, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap223, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap224, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap225, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap226, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap227, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap228, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap229, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap230, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap231, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap232, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap233, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap234, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap235, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap236, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap237, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap238, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap239, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap240, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap241, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap242, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap243, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap244, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap245, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap246, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap247, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap248, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap249, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap250, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap251, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap252, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap253, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap254, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap255, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap256, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap257, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap258, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap259, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap260, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap261, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap262, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap263, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap264, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap265, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap266, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap267, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap268, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap269, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap270, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap271, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap272, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap273, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap274, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap275, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap276, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap277, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap278, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap279, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap280, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap281, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap282, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap283, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap284, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap285, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap286, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap287, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap288, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap289, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap290, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap291, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap292, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap293, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap294, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap295, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap296, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap297, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap298, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap299, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap300, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap301, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap302, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap303, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap304, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap305, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap306, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap307, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap308, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap309, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap310, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap311, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap312, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap313, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap314, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap315, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap316, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap317, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap318, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap319, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap320, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap321, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap322, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap323, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap324, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap325, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap326, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap327, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap328, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap329, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap330, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap331, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap332, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap333, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap334, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap335, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap336, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap337, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap338, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap339, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap340, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap341, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap342, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap343, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap344, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap345, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap346, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap347, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap348, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap349, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap350, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap351, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap352, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap353, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap354, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap355, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap356, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap357, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap358, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap359, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap360, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap361, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap362, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap363, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap364, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap365, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap366, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap367, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap368, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap369, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap370, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap371, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap372, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap373, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap374, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap375, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap376, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap377, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap378, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap379, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap380, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap381, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap382, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap383, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap384, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap385, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap386, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap387, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap388, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap389, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap390, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap391, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap392, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap393, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap394, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap395, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap396, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap397, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap398, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap399, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap400, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap401, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap402, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap403, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap404, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap405, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap406, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap407, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap408, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap409, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap410, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap411, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap412, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap413, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap414, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap415, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap416, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap417, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap418, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap419, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap420, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap421, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap422, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap423, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap424, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap425, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap426, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap427, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap428, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap429, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap430, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap431, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap432, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap433, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap434, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap435, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap436, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap437, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap438, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap439, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap440, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap441, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap442, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap443, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap444, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap445, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap446, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap447, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap448, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap449, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap450, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap451, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap452, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap453, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap454, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap455, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap456, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap457, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap458, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap459, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap460, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap461, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap462, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap463, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap464, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap465, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap466, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap467, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap468, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap469, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap470, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap471, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap472, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap473, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap474, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap475, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap476, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap477, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap478, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap479, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap480, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap481, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap482, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap483, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap484, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap485, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap486, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap487, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap488, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap489, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap490, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap491, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap492, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap493, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap494, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap495, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap496, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap497, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap498, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap499, flag);
	do_byte((byte*)&p_ptr->eternalnastytrap500, flag);

	do_byte((byte*)&p_ptr->femitrap1, flag);
	do_byte((byte*)&p_ptr->femitrap2, flag);
	do_byte((byte*)&p_ptr->femitrap3, flag);
	do_byte((byte*)&p_ptr->femitrap4, flag);
	do_byte((byte*)&p_ptr->femitrap5, flag);
	do_byte((byte*)&p_ptr->femitrap6, flag);
	do_byte((byte*)&p_ptr->femitrap7, flag);
	do_byte((byte*)&p_ptr->femitrap8, flag);
	do_byte((byte*)&p_ptr->femitrap9, flag);
	do_byte((byte*)&p_ptr->femitrap10, flag);
	do_byte((byte*)&p_ptr->femitrap11, flag);
	do_byte((byte*)&p_ptr->femitrap12, flag);
	do_byte((byte*)&p_ptr->femitrap13, flag);
	do_byte((byte*)&p_ptr->femitrap14, flag);
	do_byte((byte*)&p_ptr->femitrap15, flag);
	do_byte((byte*)&p_ptr->femitrap16, flag);
	do_byte((byte*)&p_ptr->femitrap17, flag);
	do_byte((byte*)&p_ptr->femitrap18, flag);
	do_byte((byte*)&p_ptr->femitrap19, flag);
	do_byte((byte*)&p_ptr->femitrap20, flag);
	do_byte((byte*)&p_ptr->femitrap21, flag);
	do_byte((byte*)&p_ptr->femitrap22, flag);
	do_byte((byte*)&p_ptr->femitrap23, flag);
	do_byte((byte*)&p_ptr->femitrap24, flag);
	do_byte((byte*)&p_ptr->femitrap25, flag);
	do_byte((byte*)&p_ptr->femitrap26, flag);
	do_byte((byte*)&p_ptr->femitrap27, flag);
	do_byte((byte*)&p_ptr->femitrap28, flag);
	do_byte((byte*)&p_ptr->femitrap29, flag);
	do_byte((byte*)&p_ptr->femitrap30, flag);
	do_byte((byte*)&p_ptr->femitrap31, flag);
	do_byte((byte*)&p_ptr->femitrap32, flag);
	do_byte((byte*)&p_ptr->femitrap33, flag);
	do_byte((byte*)&p_ptr->femitrap34, flag);
	do_byte((byte*)&p_ptr->femitrap35, flag);
	do_byte((byte*)&p_ptr->femitrap36, flag);
	do_byte((byte*)&p_ptr->femitrap37, flag);
	do_byte((byte*)&p_ptr->femitrap38, flag);
	do_byte((byte*)&p_ptr->femitrap39, flag);
	do_byte((byte*)&p_ptr->femitrap40, flag);
	do_byte((byte*)&p_ptr->femitrap41, flag);
	do_byte((byte*)&p_ptr->femitrap42, flag);
	do_byte((byte*)&p_ptr->femitrap43, flag);
	do_byte((byte*)&p_ptr->femitrap44, flag);
	do_byte((byte*)&p_ptr->femitrap45, flag);
	do_byte((byte*)&p_ptr->femitrap46, flag);
	do_byte((byte*)&p_ptr->femitrap47, flag);
	do_byte((byte*)&p_ptr->femitrap48, flag);
	do_byte((byte*)&p_ptr->femitrap49, flag);
	do_byte((byte*)&p_ptr->femitrap50, flag);
	do_byte((byte*)&p_ptr->femitrap51, flag);
	do_byte((byte*)&p_ptr->femitrap52, flag);
	do_byte((byte*)&p_ptr->femitrap53, flag);
	do_byte((byte*)&p_ptr->femitrap54, flag);
	do_byte((byte*)&p_ptr->femitrap55, flag);
	do_byte((byte*)&p_ptr->femitrap56, flag);
	do_byte((byte*)&p_ptr->femitrap57, flag);
	do_byte((byte*)&p_ptr->femitrap58, flag);
	do_byte((byte*)&p_ptr->femitrap59, flag);
	do_byte((byte*)&p_ptr->femitrap60, flag);
	do_byte((byte*)&p_ptr->femitrap61, flag);
	do_byte((byte*)&p_ptr->femitrap62, flag);
	do_byte((byte*)&p_ptr->femitrap63, flag);
	do_byte((byte*)&p_ptr->femitrap64, flag);
	do_byte((byte*)&p_ptr->femitrap65, flag);
	do_byte((byte*)&p_ptr->femitrap66, flag);
	do_byte((byte*)&p_ptr->femitrap67, flag);
	do_byte((byte*)&p_ptr->femitrap68, flag);
	do_byte((byte*)&p_ptr->femitrap69, flag);
	do_byte((byte*)&p_ptr->femitrap70, flag);
	do_byte((byte*)&p_ptr->femitrap71, flag);
	do_byte((byte*)&p_ptr->femitrap72, flag);
	do_byte((byte*)&p_ptr->femitrap73, flag);
	do_byte((byte*)&p_ptr->femitrap74, flag);
	do_byte((byte*)&p_ptr->femitrap75, flag);
	do_byte((byte*)&p_ptr->femitrap76, flag);
	do_byte((byte*)&p_ptr->femitrap77, flag);
	do_byte((byte*)&p_ptr->femitrap78, flag);
	do_byte((byte*)&p_ptr->femitrap79, flag);
	do_byte((byte*)&p_ptr->femitrap80, flag);
	do_byte((byte*)&p_ptr->femitrap81, flag);
	do_byte((byte*)&p_ptr->femitrap82, flag);
	do_byte((byte*)&p_ptr->femitrap83, flag);
	do_byte((byte*)&p_ptr->femitrap84, flag);
	do_byte((byte*)&p_ptr->femitrap85, flag);
	do_byte((byte*)&p_ptr->femitrap86, flag);
	do_byte((byte*)&p_ptr->femitrap87, flag);
	do_byte((byte*)&p_ptr->femitrap88, flag);
	do_byte((byte*)&p_ptr->femitrap89, flag);
	do_byte((byte*)&p_ptr->femitrap90, flag);
	do_byte((byte*)&p_ptr->femitrap91, flag);
	do_byte((byte*)&p_ptr->femitrap92, flag);
	do_byte((byte*)&p_ptr->femitrap93, flag);
	do_byte((byte*)&p_ptr->femitrap94, flag);
	do_byte((byte*)&p_ptr->femitrap95, flag);
	do_byte((byte*)&p_ptr->femitrap96, flag);
	do_byte((byte*)&p_ptr->femitrap97, flag);
	do_byte((byte*)&p_ptr->femitrap98, flag);
	do_byte((byte*)&p_ptr->femitrap99, flag);
	do_byte((byte*)&p_ptr->femitrap100, flag);
	do_byte((byte*)&p_ptr->femitrap101, flag);
	do_byte((byte*)&p_ptr->femitrap102, flag);
	do_byte((byte*)&p_ptr->femitrap103, flag);
	do_byte((byte*)&p_ptr->femitrap104, flag);
	do_byte((byte*)&p_ptr->femitrap105, flag);
	do_byte((byte*)&p_ptr->femitrap106, flag);
	do_byte((byte*)&p_ptr->femitrap107, flag);
	do_byte((byte*)&p_ptr->femitrap108, flag);
	do_byte((byte*)&p_ptr->femitrap109, flag);
	do_byte((byte*)&p_ptr->femitrap110, flag);
	do_byte((byte*)&p_ptr->femitrap111, flag);
	do_byte((byte*)&p_ptr->femitrap112, flag);
	do_byte((byte*)&p_ptr->femitrap113, flag);
	do_byte((byte*)&p_ptr->femitrap114, flag);
	do_byte((byte*)&p_ptr->femitrap115, flag);
	do_byte((byte*)&p_ptr->femitrap116, flag);
	do_byte((byte*)&p_ptr->femitrap117, flag);
	do_byte((byte*)&p_ptr->femitrap118, flag);
	do_byte((byte*)&p_ptr->femitrap119, flag);
	do_byte((byte*)&p_ptr->femitrap120, flag);
	do_byte((byte*)&p_ptr->femitrap121, flag);
	do_byte((byte*)&p_ptr->femitrap122, flag);
	do_byte((byte*)&p_ptr->femitrap123, flag);
	do_byte((byte*)&p_ptr->femitrap124, flag);
	do_byte((byte*)&p_ptr->femitrap125, flag);
	do_byte((byte*)&p_ptr->femitrap126, flag);
	do_byte((byte*)&p_ptr->femitrap127, flag);
	do_byte((byte*)&p_ptr->femitrap128, flag);
	do_byte((byte*)&p_ptr->femitrap129, flag);
	do_byte((byte*)&p_ptr->femitrap130, flag);
	do_byte((byte*)&p_ptr->femitrap131, flag);
	do_byte((byte*)&p_ptr->femitrap132, flag);
	do_byte((byte*)&p_ptr->femitrap133, flag);
	do_byte((byte*)&p_ptr->femitrap134, flag);
	do_byte((byte*)&p_ptr->femitrap135, flag);
	do_byte((byte*)&p_ptr->femitrap136, flag);
	do_byte((byte*)&p_ptr->femitrap137, flag);
	do_byte((byte*)&p_ptr->femitrap138, flag);
	do_byte((byte*)&p_ptr->femitrap139, flag);
	do_byte((byte*)&p_ptr->femitrap140, flag);
	do_byte((byte*)&p_ptr->femitrap141, flag);
	do_byte((byte*)&p_ptr->femitrap142, flag);
	do_byte((byte*)&p_ptr->femitrap143, flag);
	do_byte((byte*)&p_ptr->femitrap144, flag);
	do_byte((byte*)&p_ptr->femitrap145, flag);
	do_byte((byte*)&p_ptr->femitrap146, flag);
	do_byte((byte*)&p_ptr->femitrap147, flag);
	do_byte((byte*)&p_ptr->femitrap148, flag);
	do_byte((byte*)&p_ptr->femitrap149, flag);
	do_byte((byte*)&p_ptr->femitrap150, flag);
	do_byte((byte*)&p_ptr->femitrap151, flag);
	do_byte((byte*)&p_ptr->femitrap152, flag);
	do_byte((byte*)&p_ptr->femitrap153, flag);
	do_byte((byte*)&p_ptr->femitrap154, flag);
	do_byte((byte*)&p_ptr->femitrap155, flag);
	do_byte((byte*)&p_ptr->femitrap156, flag);
	do_byte((byte*)&p_ptr->femitrap157, flag);
	do_byte((byte*)&p_ptr->femitrap158, flag);
	do_byte((byte*)&p_ptr->femitrap159, flag);
	do_byte((byte*)&p_ptr->femitrap160, flag);
	do_byte((byte*)&p_ptr->femitrap161, flag);
	do_byte((byte*)&p_ptr->femitrap162, flag);
	do_byte((byte*)&p_ptr->femitrap163, flag);
	do_byte((byte*)&p_ptr->femitrap164, flag);
	do_byte((byte*)&p_ptr->femitrap165, flag);
	do_byte((byte*)&p_ptr->femitrap166, flag);
	do_byte((byte*)&p_ptr->femitrap167, flag);
	do_byte((byte*)&p_ptr->femitrap168, flag);
	do_byte((byte*)&p_ptr->femitrap169, flag);
	do_byte((byte*)&p_ptr->femitrap170, flag);
	do_byte((byte*)&p_ptr->femitrap171, flag);
	do_byte((byte*)&p_ptr->femitrap172, flag);
	do_byte((byte*)&p_ptr->femitrap173, flag);
	do_byte((byte*)&p_ptr->femitrap174, flag);
	do_byte((byte*)&p_ptr->femitrap175, flag);
	do_byte((byte*)&p_ptr->femitrap176, flag);
	do_byte((byte*)&p_ptr->femitrap177, flag);
	do_byte((byte*)&p_ptr->femitrap178, flag);
	do_byte((byte*)&p_ptr->femitrap179, flag);
	do_byte((byte*)&p_ptr->femitrap180, flag);
	do_byte((byte*)&p_ptr->femitrap181, flag);
	do_byte((byte*)&p_ptr->femitrap182, flag);
	do_byte((byte*)&p_ptr->femitrap183, flag);
	do_byte((byte*)&p_ptr->femitrap184, flag);
	do_byte((byte*)&p_ptr->femitrap185, flag);
	do_byte((byte*)&p_ptr->femitrap186, flag);
	do_byte((byte*)&p_ptr->femitrap187, flag);
	do_byte((byte*)&p_ptr->femitrap188, flag);
	do_byte((byte*)&p_ptr->femitrap189, flag);
	do_byte((byte*)&p_ptr->femitrap190, flag);
	do_byte((byte*)&p_ptr->femitrap191, flag);
	do_byte((byte*)&p_ptr->femitrap192, flag);
	do_byte((byte*)&p_ptr->femitrap193, flag);
	do_byte((byte*)&p_ptr->femitrap194, flag);
	do_byte((byte*)&p_ptr->femitrap195, flag);
	do_byte((byte*)&p_ptr->femitrap196, flag);
	do_byte((byte*)&p_ptr->femitrap197, flag);
	do_byte((byte*)&p_ptr->femitrap198, flag);
	do_byte((byte*)&p_ptr->femitrap199, flag);
	do_byte((byte*)&p_ptr->femitrap200, flag);
	do_byte((byte*)&p_ptr->femitrap201, flag);
	do_byte((byte*)&p_ptr->femitrap202, flag);
	do_byte((byte*)&p_ptr->femitrap203, flag);
	do_byte((byte*)&p_ptr->femitrap204, flag);
	do_byte((byte*)&p_ptr->femitrap205, flag);
	do_byte((byte*)&p_ptr->femitrap206, flag);
	do_byte((byte*)&p_ptr->femitrap207, flag);
	do_byte((byte*)&p_ptr->femitrap208, flag);
	do_byte((byte*)&p_ptr->femitrap209, flag);
	do_byte((byte*)&p_ptr->femitrap210, flag);
	do_byte((byte*)&p_ptr->femitrap211, flag);
	do_byte((byte*)&p_ptr->femitrap212, flag);
	do_byte((byte*)&p_ptr->femitrap213, flag);
	do_byte((byte*)&p_ptr->femitrap214, flag);
	do_byte((byte*)&p_ptr->femitrap215, flag);
	do_byte((byte*)&p_ptr->femitrap216, flag);
	do_byte((byte*)&p_ptr->femitrap217, flag);
	do_byte((byte*)&p_ptr->femitrap218, flag);
	do_byte((byte*)&p_ptr->femitrap219, flag);
	do_byte((byte*)&p_ptr->femitrap220, flag);
	do_byte((byte*)&p_ptr->femitrap221, flag);
	do_byte((byte*)&p_ptr->femitrap222, flag);
	do_byte((byte*)&p_ptr->femitrap223, flag);
	do_byte((byte*)&p_ptr->femitrap224, flag);
	do_byte((byte*)&p_ptr->femitrap225, flag);
	do_byte((byte*)&p_ptr->femitrap226, flag);
	do_byte((byte*)&p_ptr->femitrap227, flag);
	do_byte((byte*)&p_ptr->femitrap228, flag);
	do_byte((byte*)&p_ptr->femitrap229, flag);
	do_byte((byte*)&p_ptr->femitrap230, flag);
	do_byte((byte*)&p_ptr->femitrap231, flag);
	do_byte((byte*)&p_ptr->femitrap232, flag);
	do_byte((byte*)&p_ptr->femitrap233, flag);
	do_byte((byte*)&p_ptr->femitrap234, flag);
	do_byte((byte*)&p_ptr->femitrap235, flag);
	do_byte((byte*)&p_ptr->femitrap236, flag);
	do_byte((byte*)&p_ptr->femitrap237, flag);
	do_byte((byte*)&p_ptr->femitrap238, flag);
	do_byte((byte*)&p_ptr->femitrap239, flag);
	do_byte((byte*)&p_ptr->femitrap240, flag);
	do_byte((byte*)&p_ptr->femitrap241, flag);
	do_byte((byte*)&p_ptr->femitrap242, flag);
	do_byte((byte*)&p_ptr->femitrap243, flag);
	do_byte((byte*)&p_ptr->femitrap244, flag);
	do_byte((byte*)&p_ptr->femitrap245, flag);
	do_byte((byte*)&p_ptr->femitrap246, flag);
	do_byte((byte*)&p_ptr->femitrap247, flag);
	do_byte((byte*)&p_ptr->femitrap248, flag);
	do_byte((byte*)&p_ptr->femitrap249, flag);
	do_byte((byte*)&p_ptr->femitrap250, flag);
	do_byte((byte*)&p_ptr->femitrap251, flag);
	do_byte((byte*)&p_ptr->femitrap252, flag);
	do_byte((byte*)&p_ptr->femitrap253, flag);
	do_byte((byte*)&p_ptr->femitrap254, flag);
	do_byte((byte*)&p_ptr->femitrap255, flag);
	do_byte((byte*)&p_ptr->femitrap256, flag);
	do_byte((byte*)&p_ptr->femitrap257, flag);
	do_byte((byte*)&p_ptr->femitrap258, flag);
	do_byte((byte*)&p_ptr->femitrap259, flag);
	do_byte((byte*)&p_ptr->femitrap260, flag);
	do_byte((byte*)&p_ptr->femitrap261, flag);
	do_byte((byte*)&p_ptr->femitrap262, flag);
	do_byte((byte*)&p_ptr->femitrap263, flag);
	do_byte((byte*)&p_ptr->femitrap264, flag);
	do_byte((byte*)&p_ptr->femitrap265, flag);
	do_byte((byte*)&p_ptr->femitrap266, flag);
	do_byte((byte*)&p_ptr->femitrap267, flag);
	do_byte((byte*)&p_ptr->femitrap268, flag);
	do_byte((byte*)&p_ptr->femitrap269, flag);
	do_byte((byte*)&p_ptr->femitrap270, flag);
	do_byte((byte*)&p_ptr->femitrap271, flag);
	do_byte((byte*)&p_ptr->femitrap272, flag);
	do_byte((byte*)&p_ptr->femitrap273, flag);
	do_byte((byte*)&p_ptr->femitrap274, flag);
	do_byte((byte*)&p_ptr->femitrap275, flag);
	do_byte((byte*)&p_ptr->femitrap276, flag);
	do_byte((byte*)&p_ptr->femitrap277, flag);
	do_byte((byte*)&p_ptr->femitrap278, flag);
	do_byte((byte*)&p_ptr->femitrap279, flag);
	do_byte((byte*)&p_ptr->femitrap280, flag);
	do_byte((byte*)&p_ptr->femitrap281, flag);
	do_byte((byte*)&p_ptr->femitrap282, flag);
	do_byte((byte*)&p_ptr->femitrap283, flag);
	do_byte((byte*)&p_ptr->femitrap284, flag);
	do_byte((byte*)&p_ptr->femitrap285, flag);
	do_byte((byte*)&p_ptr->femitrap286, flag);
	do_byte((byte*)&p_ptr->femitrap287, flag);
	do_byte((byte*)&p_ptr->femitrap288, flag);
	do_byte((byte*)&p_ptr->femitrap289, flag);
	do_byte((byte*)&p_ptr->femitrap290, flag);
	do_byte((byte*)&p_ptr->femitrap291, flag);
	do_byte((byte*)&p_ptr->femitrap292, flag);
	do_byte((byte*)&p_ptr->femitrap293, flag);
	do_byte((byte*)&p_ptr->femitrap294, flag);
	do_byte((byte*)&p_ptr->femitrap295, flag);
	do_byte((byte*)&p_ptr->femitrap296, flag);
	do_byte((byte*)&p_ptr->femitrap297, flag);
	do_byte((byte*)&p_ptr->femitrap298, flag);
	do_byte((byte*)&p_ptr->femitrap299, flag);
	do_byte((byte*)&p_ptr->femitrap300, flag);
	do_byte((byte*)&p_ptr->femitrap301, flag);
	do_byte((byte*)&p_ptr->femitrap302, flag);
	do_byte((byte*)&p_ptr->femitrap303, flag);
	do_byte((byte*)&p_ptr->femitrap304, flag);
	do_byte((byte*)&p_ptr->femitrap305, flag);
	do_byte((byte*)&p_ptr->femitrap306, flag);
	do_byte((byte*)&p_ptr->femitrap307, flag);
	do_byte((byte*)&p_ptr->femitrap308, flag);
	do_byte((byte*)&p_ptr->femitrap309, flag);
	do_byte((byte*)&p_ptr->femitrap310, flag);
	do_byte((byte*)&p_ptr->femitrap311, flag);
	do_byte((byte*)&p_ptr->femitrap312, flag);
	do_byte((byte*)&p_ptr->femitrap313, flag);
	do_byte((byte*)&p_ptr->femitrap314, flag);
	do_byte((byte*)&p_ptr->femitrap315, flag);
	do_byte((byte*)&p_ptr->femitrap316, flag);
	do_byte((byte*)&p_ptr->femitrap317, flag);
	do_byte((byte*)&p_ptr->femitrap318, flag);
	do_byte((byte*)&p_ptr->femitrap319, flag);
	do_byte((byte*)&p_ptr->femitrap320, flag);
	do_byte((byte*)&p_ptr->femitrap321, flag);
	do_byte((byte*)&p_ptr->femitrap322, flag);
	do_byte((byte*)&p_ptr->femitrap323, flag);
	do_byte((byte*)&p_ptr->femitrap324, flag);
	do_byte((byte*)&p_ptr->femitrap325, flag);
	do_byte((byte*)&p_ptr->femitrap326, flag);
	do_byte((byte*)&p_ptr->femitrap327, flag);
	do_byte((byte*)&p_ptr->femitrap328, flag);
	do_byte((byte*)&p_ptr->femitrap329, flag);
	do_byte((byte*)&p_ptr->femitrap330, flag);
	do_byte((byte*)&p_ptr->femitrap331, flag);
	do_byte((byte*)&p_ptr->femitrap332, flag);
	do_byte((byte*)&p_ptr->femitrap333, flag);
	do_byte((byte*)&p_ptr->femitrap334, flag);
	do_byte((byte*)&p_ptr->femitrap335, flag);
	do_byte((byte*)&p_ptr->femitrap336, flag);
	do_byte((byte*)&p_ptr->femitrap337, flag);
	do_byte((byte*)&p_ptr->femitrap338, flag);
	do_byte((byte*)&p_ptr->femitrap339, flag);
	do_byte((byte*)&p_ptr->femitrap340, flag);
	do_byte((byte*)&p_ptr->femitrap341, flag);
	do_byte((byte*)&p_ptr->femitrap342, flag);
	do_byte((byte*)&p_ptr->femitrap343, flag);
	do_byte((byte*)&p_ptr->femitrap344, flag);
	do_byte((byte*)&p_ptr->femitrap345, flag);
	do_byte((byte*)&p_ptr->femitrap346, flag);
	do_byte((byte*)&p_ptr->femitrap347, flag);
	do_byte((byte*)&p_ptr->femitrap348, flag);
	do_byte((byte*)&p_ptr->femitrap349, flag);
	do_byte((byte*)&p_ptr->femitrap350, flag);
	do_byte((byte*)&p_ptr->femitrap351, flag);
	do_byte((byte*)&p_ptr->femitrap352, flag);
	do_byte((byte*)&p_ptr->femitrap353, flag);
	do_byte((byte*)&p_ptr->femitrap354, flag);
	do_byte((byte*)&p_ptr->femitrap355, flag);
	do_byte((byte*)&p_ptr->femitrap356, flag);
	do_byte((byte*)&p_ptr->femitrap357, flag);
	do_byte((byte*)&p_ptr->femitrap358, flag);
	do_byte((byte*)&p_ptr->femitrap359, flag);
	do_byte((byte*)&p_ptr->femitrap360, flag);
	do_byte((byte*)&p_ptr->femitrap361, flag);
	do_byte((byte*)&p_ptr->femitrap362, flag);
	do_byte((byte*)&p_ptr->femitrap363, flag);
	do_byte((byte*)&p_ptr->femitrap364, flag);
	do_byte((byte*)&p_ptr->femitrap365, flag);
	do_byte((byte*)&p_ptr->femitrap366, flag);
	do_byte((byte*)&p_ptr->femitrap367, flag);
	do_byte((byte*)&p_ptr->femitrap368, flag);
	do_byte((byte*)&p_ptr->femitrap369, flag);
	do_byte((byte*)&p_ptr->femitrap370, flag);
	do_byte((byte*)&p_ptr->femitrap371, flag);
	do_byte((byte*)&p_ptr->femitrap372, flag);
	do_byte((byte*)&p_ptr->femitrap373, flag);
	do_byte((byte*)&p_ptr->femitrap374, flag);
	do_byte((byte*)&p_ptr->femitrap375, flag);
	do_byte((byte*)&p_ptr->femitrap376, flag);
	do_byte((byte*)&p_ptr->femitrap377, flag);
	do_byte((byte*)&p_ptr->femitrap378, flag);
	do_byte((byte*)&p_ptr->femitrap379, flag);
	do_byte((byte*)&p_ptr->femitrap380, flag);
	do_byte((byte*)&p_ptr->femitrap381, flag);
	do_byte((byte*)&p_ptr->femitrap382, flag);
	do_byte((byte*)&p_ptr->femitrap383, flag);
	do_byte((byte*)&p_ptr->femitrap384, flag);
	do_byte((byte*)&p_ptr->femitrap385, flag);
	do_byte((byte*)&p_ptr->femitrap386, flag);
	do_byte((byte*)&p_ptr->femitrap387, flag);
	do_byte((byte*)&p_ptr->femitrap388, flag);
	do_byte((byte*)&p_ptr->femitrap389, flag);
	do_byte((byte*)&p_ptr->femitrap390, flag);
	do_byte((byte*)&p_ptr->femitrap391, flag);
	do_byte((byte*)&p_ptr->femitrap392, flag);
	do_byte((byte*)&p_ptr->femitrap393, flag);
	do_byte((byte*)&p_ptr->femitrap394, flag);
	do_byte((byte*)&p_ptr->femitrap395, flag);
	do_byte((byte*)&p_ptr->femitrap396, flag);
	do_byte((byte*)&p_ptr->femitrap397, flag);
	do_byte((byte*)&p_ptr->femitrap398, flag);
	do_byte((byte*)&p_ptr->femitrap399, flag);
	do_byte((byte*)&p_ptr->femitrap400, flag);
	do_byte((byte*)&p_ptr->femitrap401, flag);
	do_byte((byte*)&p_ptr->femitrap402, flag);
	do_byte((byte*)&p_ptr->femitrap403, flag);
	do_byte((byte*)&p_ptr->femitrap404, flag);
	do_byte((byte*)&p_ptr->femitrap405, flag);
	do_byte((byte*)&p_ptr->femitrap406, flag);
	do_byte((byte*)&p_ptr->femitrap407, flag);
	do_byte((byte*)&p_ptr->femitrap408, flag);
	do_byte((byte*)&p_ptr->femitrap409, flag);
	do_byte((byte*)&p_ptr->femitrap410, flag);
	do_byte((byte*)&p_ptr->femitrap411, flag);
	do_byte((byte*)&p_ptr->femitrap412, flag);
	do_byte((byte*)&p_ptr->femitrap413, flag);
	do_byte((byte*)&p_ptr->femitrap414, flag);
	do_byte((byte*)&p_ptr->femitrap415, flag);
	do_byte((byte*)&p_ptr->femitrap416, flag);
	do_byte((byte*)&p_ptr->femitrap417, flag);
	do_byte((byte*)&p_ptr->femitrap418, flag);
	do_byte((byte*)&p_ptr->femitrap419, flag);
	do_byte((byte*)&p_ptr->femitrap420, flag);
	do_byte((byte*)&p_ptr->femitrap421, flag);
	do_byte((byte*)&p_ptr->femitrap422, flag);
	do_byte((byte*)&p_ptr->femitrap423, flag);
	do_byte((byte*)&p_ptr->femitrap424, flag);
	do_byte((byte*)&p_ptr->femitrap425, flag);
	do_byte((byte*)&p_ptr->femitrap426, flag);
	do_byte((byte*)&p_ptr->femitrap427, flag);
	do_byte((byte*)&p_ptr->femitrap428, flag);
	do_byte((byte*)&p_ptr->femitrap429, flag);
	do_byte((byte*)&p_ptr->femitrap430, flag);
	do_byte((byte*)&p_ptr->femitrap431, flag);
	do_byte((byte*)&p_ptr->femitrap432, flag);
	do_byte((byte*)&p_ptr->femitrap433, flag);
	do_byte((byte*)&p_ptr->femitrap434, flag);
	do_byte((byte*)&p_ptr->femitrap435, flag);
	do_byte((byte*)&p_ptr->femitrap436, flag);
	do_byte((byte*)&p_ptr->femitrap437, flag);
	do_byte((byte*)&p_ptr->femitrap438, flag);
	do_byte((byte*)&p_ptr->femitrap439, flag);
	do_byte((byte*)&p_ptr->femitrap440, flag);
	do_byte((byte*)&p_ptr->femitrap441, flag);
	do_byte((byte*)&p_ptr->femitrap442, flag);
	do_byte((byte*)&p_ptr->femitrap443, flag);
	do_byte((byte*)&p_ptr->femitrap444, flag);
	do_byte((byte*)&p_ptr->femitrap445, flag);
	do_byte((byte*)&p_ptr->femitrap446, flag);
	do_byte((byte*)&p_ptr->femitrap447, flag);
	do_byte((byte*)&p_ptr->femitrap448, flag);
	do_byte((byte*)&p_ptr->femitrap449, flag);
	do_byte((byte*)&p_ptr->femitrap450, flag);
	do_byte((byte*)&p_ptr->femitrap451, flag);
	do_byte((byte*)&p_ptr->femitrap452, flag);
	do_byte((byte*)&p_ptr->femitrap453, flag);
	do_byte((byte*)&p_ptr->femitrap454, flag);
	do_byte((byte*)&p_ptr->femitrap455, flag);
	do_byte((byte*)&p_ptr->femitrap456, flag);
	do_byte((byte*)&p_ptr->femitrap457, flag);
	do_byte((byte*)&p_ptr->femitrap458, flag);
	do_byte((byte*)&p_ptr->femitrap459, flag);
	do_byte((byte*)&p_ptr->femitrap460, flag);
	do_byte((byte*)&p_ptr->femitrap461, flag);
	do_byte((byte*)&p_ptr->femitrap462, flag);
	do_byte((byte*)&p_ptr->femitrap463, flag);
	do_byte((byte*)&p_ptr->femitrap464, flag);
	do_byte((byte*)&p_ptr->femitrap465, flag);
	do_byte((byte*)&p_ptr->femitrap466, flag);
	do_byte((byte*)&p_ptr->femitrap467, flag);
	do_byte((byte*)&p_ptr->femitrap468, flag);
	do_byte((byte*)&p_ptr->femitrap469, flag);
	do_byte((byte*)&p_ptr->femitrap470, flag);
	do_byte((byte*)&p_ptr->femitrap471, flag);
	do_byte((byte*)&p_ptr->femitrap472, flag);
	do_byte((byte*)&p_ptr->femitrap473, flag);
	do_byte((byte*)&p_ptr->femitrap474, flag);
	do_byte((byte*)&p_ptr->femitrap475, flag);
	do_byte((byte*)&p_ptr->femitrap476, flag);
	do_byte((byte*)&p_ptr->femitrap477, flag);
	do_byte((byte*)&p_ptr->femitrap478, flag);
	do_byte((byte*)&p_ptr->femitrap479, flag);
	do_byte((byte*)&p_ptr->femitrap480, flag);
	do_byte((byte*)&p_ptr->femitrap481, flag);
	do_byte((byte*)&p_ptr->femitrap482, flag);
	do_byte((byte*)&p_ptr->femitrap483, flag);
	do_byte((byte*)&p_ptr->femitrap484, flag);
	do_byte((byte*)&p_ptr->femitrap485, flag);
	do_byte((byte*)&p_ptr->femitrap486, flag);
	do_byte((byte*)&p_ptr->femitrap487, flag);
	do_byte((byte*)&p_ptr->femitrap488, flag);
	do_byte((byte*)&p_ptr->femitrap489, flag);
	do_byte((byte*)&p_ptr->femitrap490, flag);
	do_byte((byte*)&p_ptr->femitrap491, flag);
	do_byte((byte*)&p_ptr->femitrap492, flag);
	do_byte((byte*)&p_ptr->femitrap493, flag);
	do_byte((byte*)&p_ptr->femitrap494, flag);
	do_byte((byte*)&p_ptr->femitrap495, flag);
	do_byte((byte*)&p_ptr->femitrap496, flag);
	do_byte((byte*)&p_ptr->femitrap497, flag);
	do_byte((byte*)&p_ptr->femitrap498, flag);
	do_byte((byte*)&p_ptr->femitrap499, flag);
	do_byte((byte*)&p_ptr->femitrap500, flag);

	do_byte((byte*)&p_ptr->eternalfemitrap1, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap2, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap3, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap4, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap5, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap6, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap7, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap8, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap9, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap10, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap11, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap12, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap13, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap14, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap15, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap16, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap17, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap18, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap19, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap20, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap21, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap22, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap23, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap24, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap25, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap26, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap27, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap28, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap29, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap30, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap31, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap32, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap33, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap34, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap35, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap36, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap37, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap38, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap39, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap40, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap41, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap42, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap43, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap44, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap45, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap46, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap47, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap48, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap49, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap50, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap51, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap52, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap53, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap54, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap55, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap56, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap57, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap58, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap59, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap60, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap61, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap62, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap63, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap64, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap65, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap66, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap67, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap68, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap69, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap70, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap71, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap72, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap73, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap74, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap75, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap76, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap77, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap78, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap79, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap80, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap81, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap82, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap83, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap84, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap85, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap86, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap87, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap88, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap89, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap90, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap91, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap92, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap93, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap94, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap95, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap96, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap97, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap98, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap99, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap100, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap101, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap102, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap103, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap104, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap105, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap106, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap107, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap108, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap109, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap110, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap111, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap112, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap113, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap114, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap115, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap116, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap117, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap118, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap119, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap120, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap121, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap122, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap123, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap124, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap125, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap126, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap127, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap128, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap129, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap130, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap131, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap132, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap133, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap134, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap135, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap136, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap137, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap138, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap139, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap140, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap141, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap142, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap143, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap144, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap145, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap146, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap147, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap148, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap149, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap150, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap151, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap152, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap153, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap154, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap155, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap156, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap157, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap158, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap159, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap160, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap161, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap162, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap163, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap164, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap165, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap166, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap167, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap168, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap169, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap170, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap171, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap172, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap173, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap174, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap175, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap176, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap177, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap178, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap179, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap180, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap181, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap182, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap183, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap184, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap185, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap186, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap187, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap188, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap189, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap190, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap191, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap192, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap193, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap194, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap195, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap196, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap197, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap198, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap199, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap200, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap201, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap202, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap203, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap204, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap205, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap206, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap207, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap208, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap209, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap210, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap211, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap212, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap213, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap214, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap215, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap216, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap217, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap218, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap219, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap220, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap221, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap222, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap223, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap224, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap225, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap226, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap227, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap228, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap229, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap230, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap231, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap232, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap233, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap234, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap235, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap236, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap237, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap238, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap239, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap240, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap241, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap242, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap243, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap244, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap245, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap246, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap247, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap248, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap249, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap250, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap251, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap252, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap253, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap254, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap255, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap256, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap257, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap258, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap259, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap260, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap261, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap262, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap263, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap264, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap265, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap266, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap267, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap268, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap269, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap270, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap271, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap272, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap273, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap274, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap275, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap276, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap277, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap278, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap279, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap280, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap281, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap282, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap283, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap284, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap285, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap286, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap287, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap288, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap289, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap290, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap291, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap292, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap293, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap294, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap295, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap296, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap297, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap298, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap299, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap300, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap301, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap302, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap303, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap304, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap305, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap306, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap307, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap308, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap309, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap310, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap311, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap312, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap313, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap314, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap315, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap316, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap317, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap318, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap319, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap320, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap321, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap322, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap323, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap324, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap325, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap326, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap327, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap328, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap329, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap330, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap331, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap332, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap333, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap334, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap335, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap336, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap337, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap338, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap339, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap340, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap341, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap342, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap343, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap344, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap345, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap346, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap347, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap348, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap349, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap350, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap351, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap352, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap353, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap354, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap355, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap356, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap357, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap358, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap359, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap360, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap361, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap362, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap363, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap364, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap365, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap366, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap367, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap368, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap369, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap370, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap371, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap372, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap373, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap374, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap375, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap376, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap377, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap378, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap379, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap380, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap381, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap382, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap383, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap384, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap385, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap386, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap387, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap388, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap389, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap390, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap391, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap392, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap393, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap394, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap395, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap396, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap397, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap398, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap399, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap400, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap401, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap402, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap403, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap404, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap405, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap406, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap407, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap408, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap409, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap410, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap411, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap412, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap413, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap414, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap415, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap416, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap417, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap418, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap419, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap420, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap421, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap422, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap423, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap424, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap425, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap426, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap427, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap428, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap429, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap430, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap431, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap432, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap433, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap434, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap435, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap436, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap437, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap438, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap439, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap440, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap441, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap442, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap443, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap444, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap445, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap446, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap447, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap448, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap449, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap450, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap451, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap452, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap453, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap454, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap455, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap456, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap457, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap458, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap459, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap460, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap461, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap462, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap463, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap464, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap465, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap466, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap467, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap468, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap469, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap470, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap471, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap472, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap473, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap474, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap475, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap476, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap477, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap478, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap479, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap480, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap481, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap482, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap483, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap484, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap485, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap486, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap487, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap488, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap489, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap490, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap491, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap492, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap493, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap494, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap495, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap496, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap497, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap498, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap499, flag);
	do_byte((byte*)&p_ptr->eternalfemitrap500, flag);


	/* Are we in astral mode? */
	do_byte((byte*)&p_ptr->astral, flag);

	if (flag == LS_SAVE) tmp16s = POWER_MAX_INIT;
	do_s16b(&tmp16s, flag);
	if ((flag == LS_LOAD) && (tmp16s > POWER_MAX_INIT))
		note(format("Too many (%u) powers!", tmp16s));
	if (flag == LS_SAVE) tmp16s = POWER_MAX_INIT;
	for (i = 0; i < tmp16s; i++)
		do_byte((byte*)&p_ptr->powers_mod[i], flag);

	skip_ver_byte(100, flag);

	/* The tactic */
	do_byte((byte*)&p_ptr->tactic, flag);

	/* The movement */
	do_byte((byte*)&p_ptr->movement, flag);

	/* The comapnions killed */
	do_s16b(&p_ptr->companion_killed, flag);

	/* How many times you've died so far */
	do_s16b(&p_ptr->char_death_count, flag);

	/* Current bounty monster */
	do_s16b(&p_ptr->current_bounty_mon, flag);

	/* The fate */
	do_byte((byte*)&p_ptr->no_mortal, flag);

	/* The bounties */
	for (i = 0; i < MAX_BOUNTIES; i++)
	{
		do_s16b(&bounties[i][0], flag);
		do_s16b(&bounties[i][1], flag);
	}
	do_u32b(&total_bounties, flag);
	do_s16b(&spell_num, flag);
	for (i = 0; i < MAX_SPELLS; i++)
		do_spells(i, flag);
	do_s16b(&rune_num, flag);
	for (i = 0; i < MAX_RUNES; i++)
	{
		do_string(rune_spells[i].name, 30, flag);
		do_s16b(&rune_spells[i].type, flag);
		do_s16b(&rune_spells[i].rune2, flag);
		do_s16b(&rune_spells[i].mana, flag);
	}

	/* Write the "object seeds" */
	do_u32b(&seed_dungeon, flag);
	do_u32b(&seed_flavor, flag);
	do_u32b(&seed_town, flag);

	/* Special stuff */
	do_u16b(&panic_save, flag);
	do_u16b(&total_winner, flag);
	do_u16b(&has_won, flag);
	do_u16b(&noscore, flag);

	/* Write death */
	if (flag == LS_SAVE) tmp8u = death;
	do_byte(&tmp8u, flag);
	if (flag == LS_LOAD) death = tmp8u;

	/* Incompatible module? */
	if (flag == LS_LOAD)
	{
		s32b ok;

		call_lua("module_savefile_loadable", "(s,d)", "d", loaded_game_module, death, &ok);

		/* Argh bad game module! */
		if (!ok)
		{
			note(format("Bad game module. Savefile was saved with module '%s' but game is '%s'.", loaded_game_module, game_module));
			return (FALSE);
		}
	}

	/* Write feeling */
	if (flag == LS_SAVE) tmp8u = feeling;
	do_byte(&tmp8u, flag);
	if (flag == LS_LOAD) feeling = tmp8u;

	/* Turn of last "feeling" */
	do_s32b(&old_turn, flag);

	/* Current turn */
	do_s32b(&turn, flag);

	return TRUE;
}

/* Save the current persistent dungeon -SC- */
void save_dungeon(void)
{
	char tmp[16];
	char name[1024], buf[5];

	/* Save only persistent dungeons */
	if (!get_dungeon_save(buf) || (!dun_level)) return;

	/* Construct filename */
	sprintf(tmp, "%s.%s", player_base, buf);
	path_build(name, 1024, ANGBAND_DIR_SAVE, tmp);

	/* Grab permission */
	if (savefile_setuid) safe_setuid_grab();

	/* Open the file */
	fff = my_fopen(name, "wb");

	/* Drop permission */
	if (savefile_setuid) safe_setuid_drop();

#ifdef BZ_SAVES
	bz_prep(LS_SAVE);
#endif /* BZ_SAVES */

	/* Save the dungeon */
	do_dungeon(LS_SAVE, TRUE);

	/* Done */
#ifdef BZ_SAVES
	bz_done(LS_SAVE);
#endif /* BZ_SAVES */
	my_fclose(fff);
}

/*
 * Medium level player saver
 */
static bool save_player_aux(char *name)
{
	bool ok = FALSE;
	int fd = -1;
	int mode = 0644;

	/* No file yet */
	fff = NULL;

	/* File type is "SAVE" */
	FILE_TYPE(FILE_TYPE_SAVE);

	/* Grab permission */
	if (savefile_setuid) safe_setuid_grab();

	/* Create the savefile */
	fd = fd_make(name, mode);

	/* Drop permission */
	if (savefile_setuid) safe_setuid_drop();

	/* File is okay */
	if (fd >= 0)
	{
		/* Close the "fd" */
		(void)fd_close(fd);

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Open the savefile */
		fff = my_fopen(name, "wb");

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

#ifdef BZ_SAVES
		bz_prep(LS_SAVE);
#endif

		/* Successful open */
		if (fff)
		{
#ifdef BZ_SAVES
			bz_prep(LS_SAVE);
#endif /* BZ_SAVES */

			/* Write the savefile */
			if (do_savefile_aux(LS_SAVE)) ok = TRUE;

#ifdef BZ_SAVES
			bz_done(LS_SAVE);
#endif /* BZ_SAVES */
			/* Attempt to close it */
			if (my_fclose(fff)) ok = FALSE;
		}

		/* "broken" savefile */
		if (!ok)
		{
			/* Grab permission */
			if (savefile_setuid) safe_setuid_grab();

			/* Remove "broken" files */
			(void)fd_kill(name);

			/* Drop permission */
			if (savefile_setuid) safe_setuid_drop();
		}
	}

	/* Failure */
	if (!ok) return (FALSE);

	/* Successful save */
	character_saved = TRUE;

	/* Success */
	return (TRUE);
}

/*
 * Attempt to save the player in a savefile
 */
bool save_player(void)
{
	int result = FALSE;
	char safe[1024];
#ifdef SAFER_PANICS
	char panicsave[1024];
#endif /* SAFER PANICS */


#ifdef SAFER_PANICS
	if (panic_save)
	{
		/*
		 * Not sure how to do this so it's nicely portable to brain-damaged
		 * OS's with short filenames
		 */
		strcpy(panicsave, savefile);
		strcat(panicsave, ".pnc");

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Remove any old panic saves */
		fd_kill(panicsave);

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

		/* Save character */
		save_player_aux(panicsave);

		return TRUE;
	}
#endif /* SAFER_PANICS */


	/* New savefile */
	strcpy(safe, savefile);
	strcat(safe, ".new");

#ifdef VM
	/* Hack -- support "flat directory" usage on VM/ESA */
	strcpy(safe, savefile);
	strcat(safe, "n");
#endif /* VM */

	/* Grab permission */
	if (savefile_setuid) safe_setuid_grab();

	/* Remove it */
	fd_kill(safe);

	/* Drop permission */
	if (savefile_setuid) safe_setuid_drop();

	/* Attempt to save the player */
	if (save_player_aux(safe))
	{
		char temp[1024];

		/* Old savefile */
		strcpy(temp, savefile);
		strcat(temp, ".old");

#ifdef VM
		/* Hack -- support "flat directory" usage on VM/ESA */
		strcpy(temp, savefile);
		strcat(temp, "o");
#endif /* VM */

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Remove it */
		fd_kill(temp);

		/* Preserve old savefile */
		fd_move(savefile, temp);

		/* Activate new savefile */
		fd_move(safe, savefile);

		/* Remove preserved savefile */
		fd_kill(temp);

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

		/* Hack -- Pretend the character was loaded */
		character_loaded = TRUE;

#ifdef VERIFY_SAVEFILE

		/* Lock on savefile */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Remove lock file */
		fd_kill(temp);

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

#endif

		/* Success */
		result = TRUE;
	}

	save_savefile_names();

	/* Return the result */
	return (result);
}

bool file_exist(char *buf)
{
	int fd;
	bool result;

	/* Grab permission */
	if (savefile_setuid) safe_setuid_grab();

	/* Open savefile */
	fd = fd_open(buf, O_RDONLY);

	/* Drop permission */
	if (savefile_setuid) safe_setuid_drop();

	/* File exists */
	if (fd >= 0)
	{
		fd_close(fd);
		result = TRUE;
	}
	else
		result = FALSE;

	return result;
}

/*
 * Attempt to Load a "savefile"
 *
 * On multi-user systems, you may only "read" a savefile if you will be
 * allowed to "write" it later, this prevents painful situations in which
 * the player loads a savefile belonging to someone else, and then is not
 * allowed to save his game when he quits.
 *
 * We return "TRUE" if the savefile was usable, and we set the global
 * flag "character_loaded" if a real, living, character was loaded.
 *
 * Note that we always try to load the "current" savefile, even if
 * there is no such file, so we must check for "empty" savefile names.
 */
bool load_player(void)
{
	int fd = -1;

	errr err = 0;

#ifdef SAFER_PANICS
	char panic_fname[1024]; 	/* Filename for panic savefiles */
	int testfd = -1;
#endif /* SAFER_PANICS */

#ifdef VERIFY_TIMESTAMP
	struct stat statbuf;
#endif /* VERIFY_TIMESTAMP */

	cptr what = "generic";

#ifdef SAFER_PANICS
	panicload = FALSE;
	strncpy(panic_fname, savefile, 1024);
	strcat(panic_fname, ".pnc"); 	/* This might concievably cause a buffer
										   overflow, but the rest of the code
										   in this file does likewise. If someone
										   ever audits pernband for security
										   problems, well, don't blame me. The rest
										   of the code was like this before I even
										   got here -- Pat */

#endif /* SAFER_PANICS */


	/* Paranoia */
	turn = 0;

	/* Paranoia */
	death = FALSE;


	/* Allow empty savefile name */
	if (!savefile[0]) return (TRUE);


	/* XXX XXX XXX Fix this */

	/* Verify the existance of the savefile */
	if ((!file_exist(savefile))
#ifdef SAFER_PANICS
	                && (!file_exist(panic_fname))
#endif /* SAFER_PANICS */
	   )
	{
		/* Give a message */
		msg_format("Savefile does not exist: %s", savefile);
		msg_print(NULL);

		/* Allow this */
		return (TRUE);
	}


#ifdef VERIFY_SAVEFILE

	/* Verify savefile usage */
	if (!err)
	{
		FILE *fkk;

		char temp[1024];

		/* Extract name of lock file */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Check for lock */
		fkk = my_fopen(temp, "r");

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

		/* Oops, lock exists */
		if (fkk)
		{
			/* Close the file */
			my_fclose(fkk);

			/* Message */
			msg_print("Savefile is currently in use.");
			msg_print(NULL);

			/* Oops */
			return (FALSE);
		}

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Create a lock file */
		fkk = my_fopen(temp, "w");

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

		/* Dump a line of info */
		fprintf(fkk, "Lock file for savefile '%s'\n", savefile);

		/* Close the lock file */
		my_fclose(fkk);
	}

#endif


	/* Okay */
	if (!err)
	{
		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Open the savefile */
		fd = fd_open(savefile, O_RDONLY);

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

		/* No file */
		if (fd < 0) err = -1;

		/* Message (below) */
		if (err) what = "Cannot open savefile";
	}

#ifdef SAFER_PANICS

	/* Grab permission */
	if (savefile_setuid) safe_setuid_grab();

	/* Open panic save file */
	testfd = fd_open(panic_fname, O_RDONLY);

	/* Drop permission */
	if (savefile_setuid) safe_setuid_drop();

	fd_close(testfd);

	/* A panic save exists, which is not normally the case */
	if (testfd > 0)
	{
		panicload = 1;

		/* Close the normal save file */
		fd_close(fd);

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Prefer panic saves over real saves */
		fd = fd_open(panic_fname, O_RDONLY);

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

		/* This is not the error if we're at this pt */
		what = "";
		err = 0;
	}

#endif /* SAFER_PANICS */

	/* Process file */
	if (!err)
	{
		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

#ifdef VERIFY_TIMESTAMP

		/* Get the timestamp */
		(void)fstat(fd, &statbuf);

#endif

		/* Open the file XXX XXX XXX XXX Should use Angband file interface */
		fff = my_fopen(savefile, "rb");
/*		fff = fdopen(fd, "r"); */

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

#ifdef BZ_SAVES
		bz_prep(LS_LOAD);
#endif /* BZ_SAVES */

		/* Read the first four bytes */
		do_u32b(&vernum, LS_LOAD);
		do_byte(&sf_extra, LS_LOAD);

#ifdef BZ_SAVES
		bz_done(LS_LOAD);
#endif /* BZ_SAVES */

		/* XXX XXX XXX XXX Should use Angband file interface */
		my_fclose(fff);
		/* fclose(fff) */

		/* Close the file */
		fd_close(fd);
	}

	/* Process file */
	if (!err)
	{

		/* Extract version */
		sf_major = VERSION_MAJOR;
		sf_minor = VERSION_MINOR;
		sf_patch = VERSION_PATCH;
		sf_extra = VERSION_EXTRA;

		/* Clear screen */
		Term_clear();

		/* Attempt to load */
		err = rd_savefile();

		/* Message (below) */
		if (err) what = "Cannot parse savefile";
	}

	/* Paranoia */
	if (!err)
	{
		/* Invalid turn */
		if (!turn) err = -1;

		/* Message (below) */
		if (err) what = "Broken savefile";
	}

#ifdef VERIFY_TIMESTAMP

	/* Verify timestamp */
	if (!err && !arg_wizard)
	{
		/* Hack -- Verify the timestamp */
		if (sf_when > (statbuf.st_ctime + 100) ||
		                sf_when < (statbuf.st_ctime - 100))
		{
			/* Message */
			what = "Invalid timestamp";

			/* Oops */
			err = -1;
		}
	}

#endif


	/* Okay */
	if (!err)
	{
		/* Maybe the scripts want to resurrect char */
		if (process_hooks_ret(HOOK_LOAD_END, "d", "(d)", death))
		{
			character_loaded = process_hooks_return[0].num;
			death = process_hooks_return[1].num;
			return TRUE;
		}

		/* Player is dead */
		if (death)
		{
			/* Player is no longer "dead" */
			death = FALSE;

			/* Cheat death (unless the character retired) */
			if (arg_wizard && !total_winner)
			{
				/* A character was loaded */
				character_loaded = TRUE;

				/* Done */
				return (TRUE);
			}

			/* Count lives */
			sf_lives++;

			/* Forget turns */
			turn = old_turn = 0;

			/* Done */
			return (TRUE);
		}

		/* A character was loaded */
		character_loaded = TRUE;

		/* Still alive */
		if (p_ptr->chp >= 0)
		{
			/* Reset cause of death */
			(void)strcpy(died_from, "(alive and well)");
		}

#ifdef SAFER_PANICS
		if (panicload)
		{
			/* Grab permission */
			if (savefile_setuid) safe_setuid_grab();

			/*
			 * Done loading, it'll either immediately panic and re-save, or
			 * we don't need the panicsave file anymore. Either way, it's safe
			 * to zap the original panicsave
			 */
			fd_kill(panic_fname);

			/* Drop permission */
			if (savefile_setuid) safe_setuid_drop();
		}
#endif /* SAFER_PANICS */

		/* Success */
		return (TRUE);
	}


#ifdef VERIFY_SAVEFILE

	/* Verify savefile usage */
	if (TRUE)
	{
		char temp[1024];

		/* Extract name of lock file */
		strcpy(temp, savefile);
		strcat(temp, ".lok");

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Remove lock */
		fd_kill(temp);

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();
	}

#endif


	/* Message */
	msg_format("Error (%s) reading %d.%d.%d savefile.",
	           what, sf_major, sf_minor, sf_patch);
	msg_print(NULL);

	/* Oops */
	return (FALSE);
}


/*
 * Size-aware read/write routines for the savefile, do all their
 * work through sf_get and sf_put.
 */

static void do_byte(byte *v, int flag)
{
	if (flag == LS_LOAD)
	{
		*v = sf_get();
		return;
	}
	if (flag == LS_SAVE)
	{
		byte val = *v;
		sf_put(val);
		return;
	}
	/* We should never reach here, so bail out */
	printf("FATAL: do_byte passed %d\n", flag);
	exit(0);
}

static void do_u16b(u16b *v, int flag)
{
	if (flag == LS_LOAD)
	{
		(*v) = sf_get();
		(*v) |= ((u16b)(sf_get()) << 8);
		return;
	}
	if (flag == LS_SAVE)
	{
		u16b val;
		val = *v;
		sf_put((byte)(val & 0xFF));
		sf_put((byte)((val >> 8) & 0xFF));
		return;
	}
	/* Never should reach here, bail out */
	printf("FATAL: do_u16b passed %d\n", flag);
	exit(0);
}

static void do_s16b(s16b *ip, int flag)
{
	if (flag == LS_LOAD)
	{
		do_u16b((u16b *)ip, flag);
		return;
	}
	if (flag == LS_SAVE)
	{
		s16b val;
		val = *ip;
		do_u16b((u16b *)ip, flag);
		return;
	}
	/* Blah blah, never should reach here, die */
	printf("FATAL: do_s16b passed %d\n", flag);
	exit(0);
}

static void do_u32b(u32b *ip, int flag)
{
	if (flag == LS_LOAD)
	{
		(*ip) = sf_get();
		(*ip) |= ((u32b)(sf_get()) << 8);
		(*ip) |= ((u32b)(sf_get()) << 16);
		(*ip) |= ((u32b)(sf_get()) << 24);
		return;
	}
	if (flag == LS_SAVE)
	{
		u32b val = *ip;
		sf_put((byte)(val & 0xFF));
		sf_put((byte)((val >> 8) & 0xFF));
		sf_put((byte)((val >> 16) & 0xFF));
		sf_put((byte)((val >> 24) & 0xFF));
		return;
	}
	/* Bad news if you're here, because you're going down */
	printf("FATAL: do_u32b passed %d\n", flag);
	exit(0);
}

static void do_s32b(s32b *ip, int flag)
{
	if (flag == LS_LOAD)
	{
		do_u32b((u32b *)ip, flag);
		return;
	}
	if (flag == LS_SAVE)
	{
		do_u32b((u32b *)ip, flag);
		return;
	}
	/* Raus! Schnell! */
	printf("FATAL: do_s32b passed %d\n", flag);
	exit(0);
}

static void do_string(char *str, int max, int flag)
/* Max is ignored for writing */
{
	if (flag == LS_LOAD)
	{
		int i;

		/* Read the string */
		for (i = 0; TRUE; i++)
		{
			byte tmp8u;

			/* Read a byte */
			do_byte(&tmp8u, LS_LOAD);

			/* Collect string while legal */
			if (i < max) str[i] = tmp8u;

			/* End of string */
			if (!tmp8u) break;
		}
		/* Terminate */
		str[max - 1] = '\0';
		return;
	}
	if (flag == LS_SAVE)
	{
		while (*str)
		{
			do_byte((byte*)str, flag);
			str++;
		}
		do_byte((byte*)str, flag); 		/* Output a terminator */
		return;
	}
	printf("FATAL: do_string passed flag %d\n", flag);
	exit(0);
}

/*
 * Periodically, to reduce overhead and code clutter, we'll probably
 * want to convert all calls to do_ver_??? with direct do_??? calls, and
 * break the backwards compatibility. How often this needs to happen
 * remains to be seen, as the rate of accumulation isn't very
 * predictable. -- Improv
 */
static void do_ver_byte(byte *v, u32b version, byte defval, int flag)
{
	if ((flag == LS_LOAD) && (vernum < version))
	{
		*v = defval; 			/* Use the default value, and DO NOT READ */
		return;
	}

	do_byte(v, flag); 			/* Otherwise, go as normal */
}

static void skip_ver_byte(u32b version, int flag)
/* Reads and discards a byte if the savefile is as old as/older than version */
{
	if ((flag == LS_LOAD) && (vernum <= version))
	{
		byte forget;
		do_byte(&forget, flag);
	}
	return;
}

static void do_ver_u16b(u16b *v, u32b version, u16b defval, int flag)
{
	if ((flag == LS_LOAD) && (vernum < version))
	{
		*v = defval;
		return;
	}
	do_u16b(v, flag);
}

static void skip_ver_u16b(u32b version, int flag)
{
	if ((flag == LS_LOAD) && (vernum <= version))
	{
		u16b forget;
		do_u16b(&forget, flag);
	}
	return;
}

static void do_ver_s16b(s16b *v, u32b version, s16b defval, int flag)
{
	if ((flag == LS_LOAD) && (vernum < version))
	{
		*v = defval;
		return;
	}
	do_s16b(v, flag);
}

static void skip_ver_s16b(u32b version, int flag)
{
	if ((flag == LS_LOAD) && (vernum <= version))
	{
		s16b forget;
		do_s16b(&forget, flag);
	}
	return;
}

static void do_ver_u32b(u32b *v, u32b version, u32b defval, int flag)
{
	if ((flag == LS_LOAD) && (vernum < version))
	{
		*v = defval;
		return;
	}
	do_u32b(v, flag);
}

static void skip_ver_u32b(u32b version, int flag)
{
	if ((flag == LS_LOAD) && (vernum <= version))
	{
		u32b forget;
		do_u32b(&forget, flag);
	}
	return;
}

static void do_ver_s32b(s32b *v, u32b version, s32b defval, int flag)
{
	if ((flag == LS_LOAD) && (vernum < version))
	{
		*v = defval;
		return;
	}
	do_s32b(v, flag);
}

static void skip_ver_s32b(u32b version, int flag)
{
	if ((flag == LS_LOAD) && (vernum <= version))
	{
		s32b forget;
		do_s32b(&forget, flag);
	}
	return;
}

static void do_ver_string(char *str, int max, u32b version, char *defval,
                          int flag)
/* Careful, remember the argument order here */
{
	if ((flag == LS_LOAD) && (vernum < version))
	{
		strncpy(str, defval, max);
		str[max - 1] = '\0'; 	/* Ensure that whatever happens, the result string is term'd */
		return;
	}
	do_string(str, max, flag);
}

static void skip_ver_string(u32b version, int flag)
/* This function is slow and bulky */
{
	if ((flag == LS_LOAD) && (vernum <= version))
	{
		char forget[1000];
		do_string(forget, 999, flag);
	}
	return;
}

/*
 * Show information on the screen, one line at a time.
 *
 * Avoid the top two lines, to avoid interference with "msg_print()".
 */
static void note(cptr msg)
{
	static int y = 2;

	/* Draw the message */
	prt(msg, y, 0);

	/* Advance one line (wrap if needed) */
	if (++y >= 24) y = 2;

	/* Flush it */
	Term_fresh();
}


/*
 * Determine if an item can be wielded/worn (e.g. helmet, sword, bow, arrow)
 */
static bool wearable_p(object_type *o_ptr)
{
	/* Valid "tval" codes */
	switch (o_ptr->tval)
	{
	case TV_WAND:
	case TV_STAFF:
	case TV_ROD:
	case TV_ROD_MAIN:
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
	case TV_AMMO_PISTOL:
	case TV_AMMO_RIFLE:
	case TV_AMMO_SHOTGUN:
	case TV_AMMO_SMG:
	case TV_AMMO_ASSAULT:
	case TV_BOOMERANG:
	case TV_BOW:
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_MSTAFF:
	case TV_SWORD:
	case TV_AXE:
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HELM:
	case TV_CROWN:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_SOFT_ARMOR:
	case TV_HARD_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_SCROLL:
	case TV_LITE:
	case TV_POTION:
	case TV_POTION2:
	case TV_AMULET:
	case TV_RING:
	case TV_HYPNOS:
	case TV_INSTRUMENT:
	case TV_DAEMON_BOOK:
	case TV_TRAPKIT:
	case TV_TOOL:
		{
			return (TRUE);
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * rd/wr an object
 *
 * FIXME! This code probably has a lot of cruft from the old Z/V codebase.
 *
 */
static void do_item(object_type *o_ptr, int flag)
{
	byte old_dd;
	byte old_ds;

	u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

	object_kind *k_ptr;

	/* Kind */
	do_s16b(&o_ptr->k_idx, flag);

	/* Location */
	do_byte(&o_ptr->iy, flag);
	do_byte(&o_ptr->ix, flag);

	/* Type/Subtype */
	do_byte(&o_ptr->tval, flag);
	do_byte(&o_ptr->sval, flag);

	/* Special pval */
	do_s32b(&o_ptr->pval, flag);

	/* Special pval */
	do_s16b(&o_ptr->pval2, flag);

	/* Special pval */
	do_s32b(&o_ptr->pval3, flag);

	/* spell, by Amy so that the FUCKing pval2 flag is no longer improperly misused */
	do_s32b(&o_ptr->spellcontain, flag);

	do_s32b(&o_ptr->tampered, flag);
	do_s32b(&o_ptr->morgycurse, flag);
	do_s32b(&o_ptr->noattcurse, flag);
	do_s32b(&o_ptr->weightincrease, flag);

	do_byte(&o_ptr->discount, flag);
	do_byte(&o_ptr->number, flag);
	do_s32b(&o_ptr->weight, flag);

	do_s16b(&o_ptr->name1, flag);
	do_s16b(&o_ptr->name2, flag);
	do_s16b(&o_ptr->name2b, flag);
	do_s16b(&o_ptr->timeout, flag);

	do_s16b(&o_ptr->to_h, flag);
	do_s16b(&o_ptr->to_d, flag);
	do_s16b(&o_ptr->to_a, flag);

	do_s16b(&o_ptr->ac, flag);

	/* We do special processing of this flag when reading */
	if (flag == LS_LOAD)
	{
		do_byte(&old_dd, LS_LOAD);
		do_byte(&old_ds, LS_LOAD);
	}
	if (flag == LS_SAVE)
	{
		do_byte(&o_ptr->dd, LS_SAVE);
		do_byte(&o_ptr->ds, LS_SAVE);
	}

	do_byte(&o_ptr->ident, flag);

	do_byte(&o_ptr->marked, flag);

	/* flags */
	do_u32b(&o_ptr->art_flags1, flag);
	do_u32b(&o_ptr->art_flags2, flag);
	do_u32b(&o_ptr->art_flags3, flag);
	do_u32b(&o_ptr->art_flags4, flag);
	do_u32b(&o_ptr->art_flags5, flag);
	do_u32b(&o_ptr->art_flags6, flag);
	do_u32b(&o_ptr->art_flags7, flag);
	do_u32b(&o_ptr->art_flags8, flag);
	do_u32b(&o_ptr->art_flags9, flag);
	do_u32b(&o_ptr->art_flags10, flag);
	do_u32b(&o_ptr->art_esp, flag);

	/* obvious flags */
	do_u32b(&o_ptr->art_oflags1, flag);
	do_u32b(&o_ptr->art_oflags2, flag);
	do_u32b(&o_ptr->art_oflags3, flag);
	do_u32b(&o_ptr->art_oflags4, flag);
	do_u32b(&o_ptr->art_oflags5, flag);
	do_u32b(&o_ptr->art_oflags6, flag);
	do_u32b(&o_ptr->art_oflags7, flag);
	do_u32b(&o_ptr->art_oflags8, flag);
	do_u32b(&o_ptr->art_oflags9, flag);
	do_u32b(&o_ptr->art_oflags10, flag);
	do_u32b(&o_ptr->art_oesp, flag);

	/* Monster holding object */
	do_s16b(&o_ptr->held_m_idx, flag);

	/* Special powers */
	do_byte(&o_ptr->xtra1, flag);
	do_s16b(&o_ptr->xtra2, flag);

	do_byte(&o_ptr->elevel, flag);
	do_s32b(&o_ptr->exp, flag);

	/* Read the pseudo-id */
	do_byte(&o_ptr->sense, flag);

	/* Read the found info */
	do_byte(&o_ptr->found, flag);
	do_s16b(&o_ptr->found_aux1, flag);
	do_s16b(&o_ptr->found_aux2, flag);
	do_s16b(&o_ptr->found_aux3, flag);
	do_s16b(&o_ptr->found_aux4, flag);

	if (flag == LS_LOAD)
	{
		char buf[128];
		/* Inscription */
		do_string(buf, 128, LS_LOAD);
		/* Save the inscription */
		if (buf[0]) o_ptr->note = quark_add(buf);

		do_string(buf, 128, LS_LOAD);
		if (buf[0]) o_ptr->art_name = quark_add(buf);
	}
	if (flag == LS_SAVE)
	{
		/* Save the inscription (if any) */
		if (o_ptr->note)
		{
			do_string((char *)quark_str(o_ptr->note), 0, LS_SAVE);
		}
		else
		{
			do_string("", 0, LS_SAVE);
		}
		if (o_ptr->art_name)
		{
			do_string((char *)quark_str(o_ptr->art_name), 0, LS_SAVE);
		}
		else
		{
			do_string("", 0, LS_SAVE);
		}
	}

	if (flag == LS_SAVE) return ; 	/* Stick any more shared code before this. The rest
										   of this function is reserved for LS_LOAD's
										   cleanup functions */
	/*********** END OF LS_SAVE ***************/

	/* Obtain the "kind" template */
	k_ptr = &k_info[o_ptr->k_idx];

	/* Obtain tval/sval from k_info */
	o_ptr->tval = k_ptr->tval;
	if (o_ptr->tval != TV_RANDART) o_ptr->sval = k_ptr->sval;


	/* Repair non "wearable" items */
	if (!wearable_p(o_ptr))
	{
		/* Acquire correct fields */
		o_ptr->to_h = k_ptr->to_h;
		o_ptr->to_d = k_ptr->to_d;
		o_ptr->to_a = k_ptr->to_a;

		/* Acquire correct fields */
		o_ptr->ac = k_ptr->ac;
		o_ptr->dd = k_ptr->dd;
		o_ptr->ds = k_ptr->ds;

#if 0
		/* Acquire correct weight */
		if ((o_ptr->tval != TV_CORPSE) &&
		                (o_ptr->tval != TV_EGG)) o_ptr->weight = k_ptr->weight;

		/* Paranoia */
		o_ptr->name1 = o_ptr->name2 = 0;
#endif
		/* All done */
		return;
	}


	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

	/* Paranoia */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr;

		/* Obtain the artifact info */
		a_ptr = &a_info[o_ptr->name1];

		/* Verify that artifact */
		if (!a_ptr->name) o_ptr->name1 = 0;
	}

	/* Paranoia */
	if (o_ptr->name2)
	{
		ego_item_type *e_ptr;

		/* Obtain the ego-item info */
		e_ptr = &e_info[o_ptr->name2];

		/* Verify that ego-item */
		if (!e_ptr->name) o_ptr->name2 = 0;
	}


	/* Acquire standard fields */
	o_ptr->ac = k_ptr->ac;
	o_ptr->dd = k_ptr->dd;
	o_ptr->ds = k_ptr->ds;

	/* Artifacts */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr;

		/* Obtain the artifact info */
		a_ptr = &a_info[o_ptr->name1];

		/* Acquire new artifact fields */
		o_ptr->ac = a_ptr->ac;
		o_ptr->dd = a_ptr->dd;
		o_ptr->ds = a_ptr->ds;

		/* Acquire new artifact weight */
		o_ptr->weight = a_ptr->weight;
	}

	/* Ego items */
	if (o_ptr->name2)
	{
		ego_item_type *e_ptr;

		/* Obtain the ego-item info */
		e_ptr = &e_info[o_ptr->name2];


		o_ptr->dd = old_dd;
		o_ptr->ds = old_ds;
	}

	if (o_ptr->art_name)		/* A random artifact */
	{
		o_ptr->dd = old_dd;
		o_ptr->ds = old_ds;
	}
}




/*
 * Read a monster
 */
static void do_monster(monster_type *m_ptr, int flag)
{
	int i;
	bool tmp;

	/* Read the monster race */
	do_s16b(&m_ptr->r_idx, flag);

	do_u16b(&m_ptr->ego, flag);

	/* Read the other information */
	do_byte(&m_ptr->fy, flag);
	do_byte(&m_ptr->fx, flag);

	do_s32b(&m_ptr->hp, flag);
	do_s32b(&m_ptr->maxhp, flag);

	do_s16b(&m_ptr->csleep, flag);
	do_byte(&m_ptr->mspeed, flag);
	do_byte(&m_ptr->energy, flag);
	do_byte(&m_ptr->stunned, flag);
	do_byte(&m_ptr->confused, flag);
	do_byte(&m_ptr->monfear, flag);
	do_u32b(&m_ptr->smart, flag);
	do_s16b(&m_ptr->status, flag);
	do_s16b(&m_ptr->possessor, flag);
	do_byte(&m_ptr->speed, flag);
	do_byte(&m_ptr->level, flag);
	do_s16b(&m_ptr->ac, flag);
	do_u32b(&m_ptr->exp, flag);
	do_s16b(&m_ptr->target, flag);

	do_s16b(&m_ptr->bleeding, flag);
	do_s16b(&m_ptr->poisoned, flag);

	do_byte(&m_ptr->stealdetect, flag);

	do_s32b(&m_ptr->mflag, flag);

	if (flag == LS_LOAD) m_ptr->mflag &= PERM_MFLAG_MASK;

	/* Attacks */
	for (i = 0; i < 4; i++)
	{
		do_byte(&m_ptr->blow[i].method, flag);
		do_byte(&m_ptr->blow[i].effect, flag);
		do_byte(&m_ptr->blow[i].d_dice, flag);
		do_byte(&m_ptr->blow[i].d_side, flag);
	}

	/* Mind */
	tmp = (m_ptr->mind) ? TRUE : FALSE;
	do_byte((byte*)&tmp, flag);
	if (tmp)
	{
		if (flag == LS_LOAD)
		{
			MAKE(m_ptr->mind, monster_mind);
		}
	}

	/* Special race */
	tmp = (m_ptr->sr_ptr) ? TRUE : FALSE;
	do_byte((byte*)&tmp, flag);
	if (tmp)
	{
		if (flag == LS_LOAD)
		{
			MAKE(m_ptr->sr_ptr, monster_race);
		}
		do_u32b(&m_ptr->sr_ptr->name, flag);
		do_u32b(&m_ptr->sr_ptr->text, flag);

		do_u16b(&m_ptr->sr_ptr->hdice, flag);
		do_u16b(&m_ptr->sr_ptr->hside, flag);

		do_s16b(&m_ptr->sr_ptr->ac, flag);

		do_s16b(&m_ptr->sr_ptr->sleep, flag);
		do_byte(&m_ptr->sr_ptr->aaf, flag);
		do_byte(&m_ptr->sr_ptr->speed, flag);

		do_s32b(&m_ptr->sr_ptr->mexp, flag);

		do_s32b(&m_ptr->sr_ptr->weight, flag);

		do_byte(&m_ptr->sr_ptr->freq_inate, flag);
		do_byte(&m_ptr->sr_ptr->freq_spell, flag);

		do_u32b(&m_ptr->sr_ptr->flags1, flag);
		do_u32b(&m_ptr->sr_ptr->flags2, flag);
		do_u32b(&m_ptr->sr_ptr->flags3, flag);
		do_u32b(&m_ptr->sr_ptr->flags4, flag);
		do_u32b(&m_ptr->sr_ptr->flags5, flag);
		do_u32b(&m_ptr->sr_ptr->flags6, flag);
		do_u32b(&m_ptr->sr_ptr->flags7, flag);
		do_u32b(&m_ptr->sr_ptr->flags8, flag);
		do_u32b(&m_ptr->sr_ptr->flags9, flag);
		do_u32b(&m_ptr->sr_ptr->flags10, flag);
		do_u32b(&m_ptr->sr_ptr->flags11, flag);
		do_u32b(&m_ptr->sr_ptr->flags12, flag);
		do_u32b(&m_ptr->sr_ptr->flags13, flag);
		do_u32b(&m_ptr->sr_ptr->flags14, flag);
		do_u32b(&m_ptr->sr_ptr->flags15, flag);
		do_u32b(&m_ptr->sr_ptr->flags16, flag);
		do_u32b(&m_ptr->sr_ptr->flags17, flag);
		do_u32b(&m_ptr->sr_ptr->flags18, flag);
		do_u32b(&m_ptr->sr_ptr->flags19, flag);
		do_u32b(&m_ptr->sr_ptr->flags20, flag);

		/* Attacks */
		for (i = 0; i < 4; i++)
		{
			do_byte(&m_ptr->sr_ptr->blow[i].method, flag);
			do_byte(&m_ptr->sr_ptr->blow[i].effect, flag);
			do_byte(&m_ptr->sr_ptr->blow[i].d_dice, flag);
			do_byte(&m_ptr->sr_ptr->blow[i].d_side, flag);
		}

		for (i = 0; i < BODY_MAX; i++)
			do_byte(&m_ptr->sr_ptr->body_parts[i], flag);

		do_byte(&m_ptr->sr_ptr->level, flag);
		do_u32b(&m_ptr->sr_ptr->rarity, flag);

		do_byte((byte*)&m_ptr->sr_ptr->d_char, flag);
		do_byte(&m_ptr->sr_ptr->d_attr, flag);

		do_byte((byte*)&m_ptr->sr_ptr->x_char, flag);
		do_byte(&m_ptr->sr_ptr->x_attr, flag);

		do_s16b(&m_ptr->sr_ptr->max_num, flag);
		do_byte(&m_ptr->sr_ptr->cur_num, flag);
	}
}





/*
 * Handle monster lore
 */
static void do_lore(int r_idx, int flag)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Count sights/deaths/kills */
	do_s16b(&r_ptr->r_sights, flag);
	do_s16b(&r_ptr->r_deaths, flag);
	do_s16b(&r_ptr->r_pkills, flag);
	do_s16b(&r_ptr->r_tkills, flag);

	/* Count wakes and ignores */
	do_byte(&r_ptr->r_wake, flag);
	do_byte(&r_ptr->r_ignore, flag);

	/* Extra stuff */
	do_byte(&r_ptr->r_xtra1, flag);
	do_byte(&r_ptr->r_xtra2, flag);

	/* Count drops */
	do_byte(&r_ptr->r_drop_gold, flag);
	do_byte(&r_ptr->r_drop_item, flag);

	/* Count spells */
	do_byte(&r_ptr->r_cast_inate, flag);
	do_byte(&r_ptr->r_cast_spell, flag);

	/* Count blows of each type */
	do_byte(&r_ptr->r_blows[0], flag);
	do_byte(&r_ptr->r_blows[1], flag);
	do_byte(&r_ptr->r_blows[2], flag);
	do_byte(&r_ptr->r_blows[3], flag);

	/* Memorize flags */
	do_u32b(&r_ptr->r_flags1, flag); 	/* Just to remind you */
	do_u32b(&r_ptr->r_flags2, flag); 	/* flag is unrelated to */
	do_u32b(&r_ptr->r_flags3, flag); 	/* the other argument */
	do_u32b(&r_ptr->r_flags4, flag);
	do_u32b(&r_ptr->r_flags5, flag);
	do_u32b(&r_ptr->r_flags6, flag);
	do_u32b(&r_ptr->r_flags7, flag);
	do_u32b(&r_ptr->r_flags8, flag);
	do_u32b(&r_ptr->r_flags9, flag);
	do_u32b(&r_ptr->r_flags10, flag);
	do_u32b(&r_ptr->r_flags11, flag);
	do_u32b(&r_ptr->r_flags12, flag);
	do_u32b(&r_ptr->r_flags13, flag);
	do_u32b(&r_ptr->r_flags14, flag);
	do_u32b(&r_ptr->r_flags15, flag);
	do_u32b(&r_ptr->r_flags16, flag);
	do_u32b(&r_ptr->r_flags17, flag);
	do_u32b(&r_ptr->r_flags18, flag);
	do_u32b(&r_ptr->r_flags19, flag);
	do_u32b(&r_ptr->r_flags20, flag);

	/* Read the "Racial" monster tmp16b per level */
	do_s16b(&r_ptr->max_num, flag);

	do_byte((byte*)&r_ptr->on_saved, flag);

	if (flag == LS_LOAD)
	{
		/* Lore flag repair? */
		r_ptr->r_flags1 &= r_ptr->flags1;
		r_ptr->r_flags2 &= r_ptr->flags2;
		r_ptr->r_flags3 &= r_ptr->flags3;
		r_ptr->r_flags4 &= r_ptr->flags4;
		r_ptr->r_flags5 &= r_ptr->flags5;
		r_ptr->r_flags6 &= r_ptr->flags6;
		r_ptr->r_flags7 &= r_ptr->flags7;
		r_ptr->r_flags8 &= r_ptr->flags8;
		r_ptr->r_flags9 &= r_ptr->flags9;
		r_ptr->r_flags10 &= r_ptr->flags10;
		r_ptr->r_flags11 &= r_ptr->flags11;
		r_ptr->r_flags12 &= r_ptr->flags12;
		r_ptr->r_flags13 &= r_ptr->flags13;
		r_ptr->r_flags14 &= r_ptr->flags14;
		r_ptr->r_flags15 &= r_ptr->flags15;
		r_ptr->r_flags16 &= r_ptr->flags16;
		r_ptr->r_flags17 &= r_ptr->flags17;
		r_ptr->r_flags18 &= r_ptr->flags18;
		r_ptr->r_flags19 &= r_ptr->flags19;
		r_ptr->r_flags20 &= r_ptr->flags20;
	}
}




/*
 * Read a store
 */
static bool do_store(store_type *str, int flag)
/* FIXME! Why does this return anything when
   it always returns the same thing? */
{
	int j;

	byte num;

	byte store_inven_max = STORE_INVEN_MAX;

	/* Some basic info */
	do_s32b(&str->store_open, flag);
	do_s16b(&str->insult_cur, flag);
	do_s16b(&str->investment, flag);
	do_u16b(&str->owner, flag);
	if (flag == LS_SAVE) num = str->stock_num;

	/* Could be cleaner, done this way for benefit of the for loop later on */
	do_byte(&num, flag);

	do_s16b(&str->good_buy, flag);
	do_s16b(&str->bad_buy, flag);

	/* Last visit */
	do_s32b(&str->last_visit, flag);

	/* Items */
	for (j = 0; j < num; j++)
	{
		if (flag == LS_LOAD)
			/* Can't this be cleaner? */
		{
			object_type forge;
			/* Wipe the object */
			object_wipe(&forge);
			/* Read the item */
			do_item(&forge, LS_LOAD);
			/* Acquire valid items */
			if ((str->stock_num < store_inven_max) && (str->stock_num < /*str->stock_size*/256))
			{
				int k = str->stock_num++;

				/* Acquire the item */
				object_copy(&str->stock[k], &forge);
			}
		}
		if (flag == LS_SAVE) do_item(&str->stock[j], flag);
	}

	/* Success */
	return (TRUE);
}

/*
 * RNG state
 */
static void do_randomizer(int flag)
{
	int i;

	u16b tmp16u = 0;

	/* Tmp */
	do_u16b(&tmp16u, flag);

	/* Place */
	do_u16b(&Rand_place, flag);

	/* State */
	for (i = 0; i < RAND_DEG; i++)
	{
		do_u32b(&Rand_state[i], flag);
	}

	/* Accept */
	if (flag == LS_LOAD)
	{
		Rand_quick = FALSE;
	}
}

/*
 * Handle options
 *
 * Normal options are stored as a set of 256 bit flags,
 * plus a set of 256 bit masks to indicate which bit flags were defined
 * at the time the savefile was created.  This will allow new options
 * to be added, and old options to be removed, at any time, without
 * hurting old savefiles.
 *
 * The window options are stored in the same way, but note that each
 * window gets 32 options, and their order is fixed by certain defines.
 */
static void do_options(int flag)
{
	int i, n;

	u32b oflag[8];
	u32b mask[8];

	/*** Special info */

	/* Read "delay_factor" */
	do_byte(&delay_factor, flag);

	/* Read "hitpoint_warn" */
	do_byte(&hitpoint_warn, flag);

	/*** Cheating options ***/
	if (flag == LS_LOAD)		/* There *MUST* be some nice way to unify this! */
	{
		u16b c;
		do_u16b(&c, LS_LOAD);
		if (c & 0x0002) wizard = TRUE;
		cheat_peek = (c & 0x0100) ? TRUE : FALSE;
		cheat_hear = (c & 0x0200) ? TRUE : FALSE;
		cheat_room = (c & 0x0400) ? TRUE : FALSE;
		cheat_xtra = (c & 0x0800) ? TRUE : FALSE;
		cheat_know = (c & 0x1000) ? TRUE : FALSE;
		cheat_live = (c & 0x2000) ? TRUE : FALSE;
	}
	if (flag == LS_SAVE)
	{
		u16b c = 0;
		if (wizard) c |= 0x0002;
		if (cheat_peek) c |= 0x0100;
		if (cheat_hear) c |= 0x0200;
		if (cheat_room) c |= 0x0400;
		if (cheat_xtra) c |= 0x0800;
		if (cheat_know) c |= 0x1000;
		if (cheat_live) c |= 0x2000;
		do_u16b(&c, LS_SAVE);
	}

	do_byte((byte*)&autosave_l, flag);
	do_byte((byte*)&autosave_t, flag);
	do_s16b(&autosave_freq, flag);

	if (flag == LS_LOAD)
	{
		/* Read the option flags */
		for (n = 0; n < 8; n++) do_u32b(&oflag[n], flag);

		/* Read the option masks */
		for (n = 0; n < 8; n++) do_u32b(&mask[n], flag);

		/* Analyze the options */
		for (n = 0; n < 8; n++)
		{
			/* Analyze the options */
			for (i = 0; i < 32; i++)
			{
				/* Process valid flags */
				if (mask[n] & (1L << i))
				{
					/* Process valid flags */
					if (option_mask[n] & (1L << i))
					{
						/* Set */
						if (oflag[n] & (1L << i))
						{
							/* Set */
							option_flag[n] |= (1L << i);
						}

						/* Clear */
						else
						{
							/* Clear */
							option_flag[n] &= ~(1L << i);
						}
					}
				}
			}
		}


		/*** Window Options ***/

		/* Read the window flags */
		for (n = 0; n < 8; n++) do_u32b(&oflag[n], flag);

		/* Read the window masks */
		for (n = 0; n < 8; n++) do_u32b(&mask[n], flag);

		/* Analyze the options */
		for (n = 0; n < 8; n++)
		{
			/* Analyze the options */
			for (i = 0; i < 32; i++)
			{
				/* Process valid flags */
				if (mask[n] & (1L << i))
				{
					/* Process valid flags */
					if (window_mask[n] & (1L << i))
					{
						/* Set */
						if (oflag[n] & (1L << i))
						{
							/* Set */
							window_flag[n] |= (1L << i);
						}

						/* Clear */
						else
						{
							/* Clear */
							window_flag[n] &= ~(1L << i);
						}
					}
				}
			}
		}
	}
	if (flag == LS_SAVE)
	{
		/* Analyze the options */
		for (i = 0; option_info[i].o_desc; i++)
		{
			int os = option_info[i].o_page;
			int ob = option_info[i].o_bit;

			/* Process real entries */
			if (option_info[i].o_var)
			{
				/* Set */
				if (*option_info[i].o_var)
				{
					/* Set */
					option_flag[os] |= (1L << ob);
				}

				/* Clear */
				else
				{
					/* Clear */
					option_flag[os] &= ~(1L << ob);
				}
			}
		}


		/*** Normal options ***/

		/* Dump the flags */
		for (i = 0; i < 8; i++) do_u32b(&option_flag[i], flag);

		/* Dump the masks */
		for (i = 0; i < 8; i++) do_u32b(&option_mask[i], flag);

		/*** Window options ***/

		/* Dump the flags */
		for (i = 0; i < 8; i++) do_u32b(&window_flag[i], flag);

		/* Dump the masks */
		for (i = 0; i < 8; i++) do_u32b(&window_mask[i], flag);
	}
}


/* Load/Save the random spells info */
static void do_spells(int i, int flag)
{
	random_spell *s_ptr = &random_spells[i];
	do_string(s_ptr->name, 30, flag);
	do_string(s_ptr->desc, 30, flag);
	do_s16b(&s_ptr->mana, flag);
	do_s16b(&s_ptr->fail, flag);
	do_u32b(&s_ptr->proj_flags, flag);
	do_byte(&s_ptr->GF, flag);
	do_byte(&s_ptr->radius, flag);
	do_byte(&s_ptr->dam_sides, flag);
	do_byte(&s_ptr->dam_dice, flag);
	do_byte(&s_ptr->level, flag);
	do_byte((byte*)&s_ptr->untried, flag);
}


/*
 * Handle player inventory
 *
 * FIXME! This function probably could be unified better
 * Note that the inventory is "re-sorted" later by "dungeon()".
 */
static bool do_inventory(int flag)
{
	if (flag == LS_LOAD)
	{
		int slot = 0;

		object_type forge;
		object_type *q_ptr;

		/* No items */
		inven_cnt = 0;
		equip_cnt = 0;

		/* Read until done */
		while (1)
		{
			u16b n;

			/* Get the next item index */
			do_u16b(&n, LS_LOAD);

			/* Nope, we reached the end */
			if (n == 0xFFFF) break;

			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Read the item */
			do_item(q_ptr, LS_LOAD);

			/* Hack -- verify item */
			if (!q_ptr->k_idx) return (FALSE);

			/* Wield equipment */
			if (n >= INVEN_WIELD)
			{
				/* Copy object */
				object_copy(&p_ptr->inventory[n], q_ptr);

				/* Take care of item sets */
				if (q_ptr->name1)
				{
					wield_set(q_ptr->name1, a_info[q_ptr->name1].set, TRUE);
				}

				/* One more item */
				equip_cnt++;
			}

			/* Warning -- backpack is full */
			else if (inven_cnt == INVEN_PACK)
			{
				/* Oops */
				note("Too many items in the inventory!");

				/* Fail */
				return (FALSE);
			}

			/* Carry inventory */
			else
			{
				/* Get a slot */
				n = slot++;

				/* Copy object */
				object_copy(&p_ptr->inventory[n], q_ptr);

				/* One more item */
				inven_cnt++;
			}
		}
	}
	if (flag == LS_SAVE)
	{
		u16b i;
		u16b sent = 0xFFFF;
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			object_type *o_ptr = &p_ptr->inventory[i];
			if (!o_ptr->k_idx) continue;
			do_u16b(&i, flag);
			do_item(o_ptr, flag);
		}
		do_u16b(&sent, LS_SAVE); 	/* Sentinel */
	}
	/* Success */
	return (TRUE);
}



/*
 * Read the saved messages
 */
static void do_messages(int flag)   /* FIXME! We should be able to unify this better */
{
	int i;
	char buf[128];
	byte color, type;

	s16b num;

	if (flag == LS_SAVE) num = (compress_savefile &&
		                            (message_num() > 40)) ? 40 : message_num();

	/* Total */
	do_s16b(&num, flag);

	/* Read the messages */
	if (flag == LS_LOAD)
	{
		for (i = 0; i < num; i++)
		{
			/* Read the message */
			do_string(buf, 128, LS_LOAD);
			do_byte(&color, flag);
			do_byte(&type, flag);

			/* Save the message */
			message_add(type, buf, color);
		}
	}
	if (flag == LS_SAVE)
	{
		byte holder;
		for (i = num - 1; i >= 0; i--)
		{
			do_string((char *)message_str((s16b)i), 0, LS_SAVE);
			holder = message_color((s16b)i);
			do_byte(&holder, flag);
			holder = message_type((s16b)i);
			do_byte(&holder, flag);
		}
	}
}

/*
 * Handle dungeon
 *
 * The monsters/objects must be loaded in the same order
 * that they were stored, since the actual indexes matter.
 */
static bool do_dungeon(int flag, bool no_companions)
{
	int i;

	cave_type *c_ptr;

	/* Read specific */
	u16b tmp16b = 0;

	my_sentinel("Before do_dungeon", 324, flag);

	/* Header info */
	do_s16b(&dun_level, flag);
	do_byte(&dungeon_type, flag);
	do_s16b(&num_repro, flag);
	do_s16b(&p_ptr->py, flag);
	do_s16b(&p_ptr->px, flag);
	do_s16b(&cur_hgt, flag);
	do_s16b(&cur_wid, flag);
	do_s16b(&max_panel_rows, flag);
	do_s16b(&max_panel_cols, flag);

	do_u32b(&dungeon_flags1, flag);
	do_u32b(&dungeon_flags2, flag);

	/* Last teleportation */
	do_s16b(&last_teleportation_y, flag);
	do_s16b(&last_teleportation_y, flag);

	/* Spell effects */
	tmp16b = MAX_EFFECTS;
	do_u16b(&tmp16b, flag);

	if ((flag == LS_LOAD) && (tmp16b > MAX_EFFECTS))
	{
		quit("Too many spell effects");
	}

	for (i = 0; i < tmp16b; ++i)
	{
		do_s16b(&effects[i].type, flag);
		do_s16b(&effects[i].dam, flag);
		do_s16b(&effects[i].time, flag);
		do_u32b(&effects[i].flags, flag);
		do_s16b(&effects[i].cx, flag);
		do_s16b(&effects[i].cy, flag);
		do_s16b(&effects[i].rad, flag);
	}

	/* TO prevent bugs with evolving dungeons */
	for (i = 0; i < 100; i++)
	{
		do_s16b(&floor_type[i], flag);
		do_s16b(&fill_type[i], flag);
	}

	if ((flag == LS_LOAD) && (!dun_level && !p_ptr->inside_quest))
	{
		int xstart = 0;
		int ystart = 0;
		/* Init the wilderness */
		process_dungeon_file(NULL, "w_info.txt", &ystart, &xstart, cur_hgt, cur_wid,
		                     TRUE);

		/* Init the town */
		xstart = 0;
		ystart = 0;
		init_flags = 0;
		process_dungeon_file(NULL, "t_info.txt", &ystart, &xstart, cur_hgt, cur_wid,
		                     TRUE);
	}

	do_grid(flag);

	/*** Objects ***/

	if (flag == LS_SAVE) compact_objects(0);
	if (flag == LS_SAVE) compact_monsters(0);
	if (flag == LS_SAVE)
	{
		tmp16b = o_max;
	
		if (no_companions)
		{
			for (i = 1; i < o_max; i++)
			{
				object_type *o_ptr = &o_list[i];

				if (o_ptr->held_m_idx && (m_list[o_ptr->held_m_idx].status == MSTATUS_COMPANION)) tmp16b--;
			}
		}

		/* Item count */
		do_u16b(&tmp16b, flag);

		tmp16b = o_max;
	}
	else
		/* Read item count */
		do_u16b(&tmp16b, flag);

	/* Verify maximum */
	if ((flag == LS_LOAD) && (tmp16b > max_o_idx))
	{
		note(format("Too many (%d) object entries!", tmp16b));
		return (FALSE);
	}

	/* Dungeon items */
	for (i = 1; i < tmp16b; i++)
	{
		int o_idx;

		object_type *o_ptr;

		if (flag == LS_SAVE)
		{
			o_ptr = &o_list[i];
			/* Don't save objects held by companions when no_companions is set */
			if (no_companions && o_ptr->held_m_idx && (m_list[o_ptr->held_m_idx].status == MSTATUS_COMPANION)) continue;

			do_item(o_ptr, LS_SAVE);
			continue; 			/* Saving is easy */
		}
		/* Until the end of the loop, this is all LS_LOAD */

		/* Get a new record */
		o_idx = o_pop();

		/* Oops */
		if (i != o_idx)
		{
			note(format("Object allocation error (%d <> %d)", i, o_idx));
			return (FALSE);
		}


		/* Acquire place */
		o_ptr = &o_list[o_idx];

		/* Read the item */
		do_item(o_ptr, LS_LOAD);

		/* Monster */
		if (o_ptr->held_m_idx)
		{
			monster_type *m_ptr;

			/* Monster */
			m_ptr = &m_list[o_ptr->held_m_idx];

			/* Build a stack */
			o_ptr->next_o_idx = m_ptr->hold_o_idx;

			/* Place the object */
			m_ptr->hold_o_idx = o_idx;
		}

		/* Dungeon */
		else
		{
			/* Access the item location */
			c_ptr = &cave[o_ptr->iy][o_ptr->ix];

			/* Build a stack */
			o_ptr->next_o_idx = c_ptr->o_idx;

			/* Place the object */
			c_ptr->o_idx = o_idx;
		}
	}

	/*** Monsters ***/

	if (flag == LS_SAVE)
	{
		tmp16b = m_max;

		if (no_companions)
		{
			for (i = 1; i < m_max; i++)
			{
				monster_type *m_ptr = &m_list[i];

				if (m_ptr->status == MSTATUS_COMPANION) tmp16b--;
			}
		}

		/* Write the monster count */
		do_u16b(&tmp16b, flag);

		tmp16b = m_max;
	}
	else
		/* Read the monster count */
		do_u16b(&tmp16b, flag);

	/* Validate */
	if ((flag == LS_LOAD) && (tmp16b > max_m_idx))
	{
		note(format("Too many (%d) monster entries!", tmp16b));
		return (FALSE);
	}

	/* Read the monsters */
	for (i = 1; i < tmp16b; i++)
	{
		int m_idx;
		monster_type *m_ptr;
		monster_race *r_ptr;

		if (flag == LS_SAVE)
		{
			m_ptr = &m_list[i];

			/* Don't save companions when no_companions is set */
			if (no_companions && m_ptr->status == MSTATUS_COMPANION) continue;

			do_monster(m_ptr, LS_SAVE);
			continue; 			/* Easy to save a monster */
		}
		/* From here on, it's all LS_LOAD */
		/* Get a new record */
		m_idx = m_pop();

		/* Oops */
		if (i != m_idx)
		{
			note(format("Monster allocation error (%d <> %d)", i, m_idx));
			return (FALSE);
		}

		/* Acquire monster */
		m_ptr = &m_list[m_idx];

		/* Read the monster */
		do_monster(m_ptr, LS_LOAD);

		/* Access grid */
		c_ptr = &cave[m_ptr->fy][m_ptr->fx];

		/* Mark the location */
		c_ptr->m_idx = m_idx;

		/* Controlled ? */
		if (m_ptr->mflag & MFLAG_CONTROL)
			p_ptr->control = m_idx;

		/* Access race */
		r_ptr = &r_info[m_ptr->r_idx];

		/* Count XXX XXX XXX */
		r_ptr->cur_num++;
	}

	/* Read the kept monsters */

	tmp16b = (flag == LS_SAVE && !no_companions) ? max_m_idx : 0;

	/* Read the monster count */
	do_u16b(&tmp16b, flag);

	/* Hack -- verify */
	if ((flag == LS_LOAD) && (tmp16b > max_m_idx))
	{
		note(format("Too many (%d) monster entries!", tmp16b));
		return (FALSE);
	}
	for (i = 1; i < tmp16b; i++)
	{
		monster_type *m_ptr;

		/* Acquire monster */
		m_ptr = &km_list[i];

		/* Read the monster */
		do_monster(m_ptr, flag);
	}

	/*** Success ***/

	/* The dungeon is ready */
	if (flag == LS_LOAD) character_dungeon = TRUE;

	/* Success */
	return (TRUE);
}

/* Returns TRUE if we successfully load the dungeon */
bool load_dungeon(char *ext)
{
	char tmp[16];
	char name[1024];
	byte old_dungeon_type = dungeon_type;
	s16b old_dun = dun_level;

	/* Construct name */
	sprintf(tmp, "%s.%s", player_base, ext);
	path_build(name, 1024, ANGBAND_DIR_SAVE, tmp);

	/* Grab permission */
	if (savefile_setuid) safe_setuid_grab();

	/* Open the file */
	fff = my_fopen(name, "rb");

	/* Drop permission */
	if (savefile_setuid) safe_setuid_drop();

	if (fff == NULL)
	{
		dun_level = old_dun;
		dungeon_type = old_dungeon_type;

		my_fclose(fff);
		return (FALSE);
	}

#ifdef BZ_SAVES
	bz_prep(LS_LOAD);
#endif /* BZ_SAVES */

	/* Read the dungeon */
	if (!do_dungeon(LS_LOAD, FALSE))
	{
		dun_level = old_dun;
		dungeon_type = old_dungeon_type;

#ifdef BZ_SAVES
		bz_done(LS_LOAD);
#endif /* BZ_SAVES */
		my_fclose(fff);
		return (FALSE);
	}

	dun_level = old_dun;
	dungeon_type = old_dungeon_type;

	/* Done */
#ifdef BZ_SAVES
	bz_done(LS_LOAD);
#endif /* BZ_SAVES */
	my_fclose(fff);
	return (TRUE);
}

void do_blocks(int flag)
/* Handle blocked-allocation stuff for quests and lua stuff
   This depends on a dyn_tosave array of s32b's. Adjust as needed
   if other data structures are desirable. This also is not hooked
   in yet. Ideally, plug it near the end of the savefile.
 */
{
	s16b numblks, n_iter = 0; 	/* How many blocks do we have? */
	do_s16b(&numblks, flag);
	while (n_iter < numblks)
	{
		/*	do_s32b(dyn_tosave[n_iter], flag); */
		n_iter++;
	}
	my_sentinel("In blocked-allocation area", 37, flag);
}

void do_fate(int i, int flag)
{
	if ((flag == LS_LOAD) && (i >= MAX_FATES)) i = MAX_FATES - 1;

	do_byte(&fates[i].fate, flag);
	do_byte(&fates[i].level, flag);
	do_byte(&fates[i].serious, flag);
	do_s16b(&fates[i].o_idx, flag);
	do_s16b(&fates[i].e_idx, flag);
	do_s16b(&fates[i].a_idx, flag);
	do_s16b(&fates[i].v_idx, flag);
	do_s16b(&fates[i].r_idx, flag);
	do_s16b(&fates[i].count, flag);
	do_s16b(&fates[i].time, flag);
	do_byte((byte*)&fates[i].know, flag);
}

/*
 * Actually read the savefile
 */
static bool do_savefile_aux(int flag)
{
	int i, j;

	byte tmp8u;
	u16b tmp16u;
	u32b tmp32u;

	bool *reals;
	u16b real_max = 0;

	/* Mention the savefile version */
	if (flag == LS_LOAD)
	{
		if (vernum < 100)
		{
			note(format("Savefile version %lu too old! ", vernum));
			return FALSE;
		}
		else
		{
			note(format("Loading version %lu savefile... ", vernum));
		}
	}
	if (flag == LS_SAVE)
	{
		sf_when = time((time_t *) 0); 	/* Note when file was saved */
		sf_xtra = 0L; 			/* What the hell is this? */
		sf_saves++; 				/* Increment the saves ctr */
	}

	/* Handle version bytes. FIXME! DG wants me to change this all around */
	if (flag == LS_LOAD)
	{
		u32b mt32b;
		byte mtbyte;

		/* Discard all this, we've already read it */
		do_u32b(&mt32b, flag);
		do_byte(&mtbyte, flag);
	}
	if (flag == LS_SAVE)
	{
		u32b saver;
		saver = SAVEFILE_VERSION;
		do_u32b(&saver, flag);
		tmp8u = (byte)rand_int(256);
		do_byte(&tmp8u, flag); 	/* 'encryption' */
	}

	/* Operating system info? Not really. This is just set to 0L */
	do_u32b(&sf_xtra, flag);

	/* Time of last save */
	do_u32b(&sf_when, flag);

	/* Number of past lives */
	do_u16b(&sf_lives, flag);

	/* Number of times saved */
	do_u16b(&sf_saves, flag);

	/* Game module */
	if (flag == LS_SAVE)
		strcpy(loaded_game_module, game_module);
	do_string(loaded_game_module, 80, flag);

	/* Read RNG state */
	do_randomizer(flag);
	if ((flag == LS_LOAD) && (arg_fiddle)) note("Loaded Randomizer Info");

	/* Automatizer state */
	do_byte((byte*)&automatizer_enabled, flag);

	/* Then the options */
	do_options(flag);
	if ((flag == LS_LOAD) && (arg_fiddle)) note("Loaded Option Flags");

	/* Then the "messages" */
	do_messages(flag);
	if ((flag == LS_LOAD) && (arg_fiddle)) note("Loaded Messages");

	/* Monster Memory */
	if (flag == LS_SAVE) tmp16u = max_r_idx;
	do_u16b(&tmp16u, flag);

	/* Incompatible save files */
	if ((flag == LS_LOAD) && (tmp16u > max_r_idx))
	{
		note(format("Too many (%u) monster races!", tmp16u));
		return (FALSE);
	}

	/* Read the available records */
	for (i = 0; i < tmp16u; i++)
	{
		monster_race *r_ptr;

		/* Read the lore */
		do_lore(i, flag);

		/* Access that monster */
		r_ptr = &r_info[i]; 		/* FIXME! Why the hell are we doing this? */
	}

	if ((flag == LS_LOAD) && (arg_fiddle)) note("Loaded Monster Memory");
	/* Object Memory */
	if (flag == LS_SAVE) tmp16u = max_k_idx;
	do_u16b(&tmp16u, flag);

	/* Incompatible save files */
	if ((flag == LS_LOAD) && (tmp16u > max_k_idx))
	{
		note(format("Too many (%u) object kinds!", tmp16u));
		return (FALSE);
	}

	/* Read the object memory */
	for (i = 0; i < tmp16u; i++) do_xtra(i, flag);
	if ((flag == LS_LOAD) && (arg_fiddle)) note("Loaded Object Memory");
	if (flag == LS_LOAD) junkinit();

	{
		u16b max_towns_ldsv;
		u16b max_quests_ldsv;
		if (flag == LS_SAVE) max_towns_ldsv = max_towns;
		/* Number of towns */
		do_u16b(&max_towns_ldsv, flag);
		/* Incompatible save files */
		if ((flag == LS_LOAD) && (max_towns_ldsv > max_towns))
		{
			note(format("Too many (%u) towns!", max_towns_ldsv));
			return (FALSE);
		}
		/* Min of random towns */
		if (flag == LS_SAVE) max_towns_ldsv = TOWN_RANDOM;
		do_u16b(&max_towns_ldsv, flag);
		/* Incompatible save files */
		if ((flag == LS_LOAD) && (max_towns_ldsv != TOWN_RANDOM))
		{
			note(format("Different random towns base (%u)!", max_towns_ldsv));
			return (FALSE);
		}

		for (i = 0; i < max_towns; i++)
		{
			do_byte((byte*)&town_info[i].destroyed, flag);

			if (i >= TOWN_FOURDIM)
			{
				do_u32b(&town_info[i].seed, flag);
				do_byte(&town_info[i].numstores, flag);
				do_byte(&town_info[i].flags, flag);

				/* If the town is realy used create a sock */
				if ((town_info[i].flags & (TOWN_REAL)) && (flag == LS_LOAD))
				{
					create_stores_stock(i);
				}
			}
		}

		/* Number of dungeon */
		if (flag == LS_SAVE) max_towns_ldsv = max_d_idx;
		do_u16b(&max_towns_ldsv, flag);

		/* Incompatible save files */
		if ((flag == LS_LOAD) && (max_towns_ldsv > max_d_idx))
		{
			note(format("Too many dungeon types (%u)!", max_towns_ldsv));
			return (FALSE);
		}

		/* Number of towns per dungeon */
		if (flag == LS_SAVE) max_quests_ldsv = TOWN_DUNGEON;
		do_u16b(&max_quests_ldsv, flag);
		/* Incompatible save files */
		if ((flag == LS_LOAD) && (max_quests_ldsv > TOWN_DUNGEON))
		{
			note(format("Too many town per dungeons (%u)!", max_quests_ldsv));
			return (FALSE);
		}

		for (i = 0; i < max_towns_ldsv; i++)
		{
			for (j = 0; j < max_quests_ldsv; j++)
			{
				do_s16b(&(d_info[i].t_idx[j]), flag);
				do_s16b(&(d_info[i].t_level[j]), flag);
			}
			do_s16b(&(d_info[i].t_num), flag);
		}

		if (flag == LS_SAVE) max_quests_ldsv = MAX_Q_IDX_INIT;
		/* Number of quests */
		do_u16b(&max_quests_ldsv, flag);

		/* Incompatible save files */
		if ((flag == LS_LOAD) && (max_quests_ldsv > MAX_Q_IDX_INIT))
		{
			note(format("Too many (%u) quests!", max_quests_ldsv));
			return (FALSE);
		}

		for (i = 0; i < max_quests_ldsv; i++)
		{
			do_s16b(&quest[i].status, flag);
			for (j = 0; j < 4; j++)
			{
				do_s32b(&(quest[i].data[j]), flag);
			}

			/* Init the hooks */
			if ((flag == LS_LOAD) && (quest[i].type == HOOK_TYPE_C)) quest[i].init(i);
		}

		/* Position in the wilderness */
		do_s32b(&p_ptr->wilderness_x, flag);
		do_s32b(&p_ptr->wilderness_y, flag);
		do_byte((byte*)&p_ptr->wild_mode, flag);
		do_byte((byte*)&p_ptr->old_wild_mode, flag);

		{
			s32b wild_x_size, wild_y_size;
			if (flag == LS_SAVE)
			{
				wild_x_size = max_wild_x;
				wild_y_size = max_wild_y;
			}
			/* Size of the wilderness */
			do_s32b(&wild_x_size, flag);
			do_s32b(&wild_y_size, flag);
			/* Incompatible save files */
			if ((flag == LS_LOAD) &&
			                ((wild_x_size > max_wild_x) || (wild_y_size > max_wild_y)))
			{
				note(format("Wilderness is too big (%u/%u)!",
				            wild_x_size, wild_y_size));
				return (FALSE);
			}
			/* Wilderness seeds */
			for (i = 0; i < wild_x_size; i++)
			{
				for (j = 0; j < wild_y_size; j++)
				{
					do_u32b(&wild_map[j][i].seed, flag);
					do_u16b(&wild_map[j][i].entrance, flag);
					do_byte((byte*)&wild_map[j][i].known, flag);
				}
			}
		}
	}
	if ((flag == LS_LOAD) && (arg_fiddle)) note("Loaded Quests");

	/* Load the random artifacts. */
	if (flag == LS_SAVE) tmp16u = MAX_RANDARTS;
	do_u16b(&tmp16u, flag);
	if ((flag == LS_LOAD) && (tmp16u > MAX_RANDARTS))
	{
		note(format("Too many (%u) random artifacts!", tmp16u));
		return (FALSE);
	}
	for (i = 0; i < tmp16u; i++)
	{
		random_artifact *ra_ptr = &random_artifacts[i];

		do_string(ra_ptr->name_full, 80, flag);
		do_string(ra_ptr->name_short, 80, flag);
		do_byte(&ra_ptr->level, flag);
		do_byte(&ra_ptr->attr, flag);
		do_u32b(&ra_ptr->cost, flag);
		do_byte(&ra_ptr->activation, flag);
		do_byte(&ra_ptr->generated, flag);
	}

	/* Load the Artifacts */
	if (flag == LS_SAVE) tmp16u = max_a_idx;
	do_u16b(&tmp16u, flag);
	/* Incompatible save files */
	if ((flag == LS_LOAD) && (tmp16u > max_a_idx))
	{
		note(format("Too many (%u) artifacts!", tmp16u));
		return (FALSE);
	}

	/* Read the artifact flags */
	for (i = 0; i < tmp16u; i++)
	{
		do_byte(&(&a_info[i])->cur_num, flag);
	}
	if ((flag == LS_LOAD) && arg_fiddle) note("Loaded Artifacts");

	/* Fates */
	if (flag == LS_SAVE) tmp16u = MAX_FATES;
	do_u16b(&tmp16u, flag);

	/* Incompatible save files */
	if ((flag == LS_LOAD) && (tmp16u > MAX_FATES))
	{
		note(format("Too many (%u) fates!", tmp16u));
		return (FALSE);
	}

	/* Read the fate flags */
	for (i = 0; i < tmp16u; i++)
	{
		do_fate(i, flag);
	}
	if ((flag == LS_LOAD) && arg_fiddle) note("Loaded Fates");

	/* Load the Traps */
	if (flag == LS_SAVE) tmp16u = max_t_idx;
	do_u16b(&tmp16u, flag);

	/* Incompatible save files */
	if ((flag == LS_LOAD) && (tmp16u > max_t_idx))
	{
		note(format("Too many (%u) traps!", tmp16u));
		return (FALSE);
	}

	/* fate flags */
	for (i = 0; i < tmp16u; i++)
	{
		do_byte((byte*)&t_info[i].ident, flag);
	}
	if ((flag == LS_LOAD) && (arg_fiddle)) note("Loaded Traps");

	/* inscription knowledge */
	if (flag == LS_SAVE) tmp16u = MAX_INSCRIPTIONS;
	do_u16b(&tmp16u, flag);

	/* Incompatible save files */
	if ((flag == LS_LOAD) && (tmp16u > MAX_INSCRIPTIONS))
	{
		note(format("Too many (%u) inscriptions!", tmp16u));
		return (FALSE);
	}

	/* Read the inscription flag */
	for (i = 0; i < tmp16u; i++)
		do_byte((byte*)&inscription_info[i].know, flag);
	if ((flag == LS_LOAD) && arg_fiddle) note("Loaded Inscriptions");


	/* Read the extra stuff */
	if (!do_extra(flag))
		return FALSE;
	if ((flag == LS_LOAD) && arg_fiddle) note("Loaded extra information");


	/* player_hp array */
	if (flag == LS_SAVE) tmp16u = PY_MAX_LEVEL;
	do_u16b(&tmp16u, flag);
	/* Incompatible save files */
	if ((flag == LS_LOAD) && (tmp16u > PY_MAX_LEVEL))
	{
		note(format("Too many (%u) hitpoint entries!", tmp16u));
		return (FALSE);
	}

	/* Read the player_hp array */
	for (i = 0; i < tmp16u; i++)
	{
		do_s16b(&player_hp[i], flag);
	}

	if (flag == LS_LOAD) morejunk();

	/* Read the pet command settings */
	do_byte(&p_ptr->pet_follow_distance, flag);
	do_byte(&p_ptr->pet_open_doors, flag);
	do_byte(&p_ptr->pet_pickup_items, flag);

	/* Read the inventory */
	if (!do_inventory(flag) && (flag == LS_LOAD))	/* do NOT reverse this ordering */
	{
		note("Unable to read inventory");
		return (FALSE);
	}

	/* Note that this forbids max_towns from shrinking, but that is fine */
	C_MAKE(reals, max_towns, bool);

	/* Find the real towns */
	if (flag == LS_SAVE)
	{
		for (i = 1; i < max_towns; i++)
		{
			if (!(town_info[i].flags & (TOWN_REAL))) continue;
			reals[real_max++] = i;
		}
	}
	do_u16b(&real_max, flag);
	for (i = 0; i < real_max; i++)
	{
		do_byte((byte*)&reals[i], flag);
	}

	/* Read the stores */
	if (flag == LS_SAVE) tmp16u = max_st_idx;
	do_u16b(&tmp16u, flag);

	/* Ok now read the real towns */
	for (i = 0; i < real_max; i++)
	{
		int z = reals[i];

		/* Ultra paranoia */
		if (!town_info[z].stocked) create_stores_stock(z);

		for (j = 0; j < tmp16u; j++)
			do_store(&town_info[z].store[j], flag);
	}

	C_FREE(reals, max_towns, bool);

	if (flag == LS_SAVE) tmp32u = extra_savefile_parts;
	do_u32b(&tmp32u, flag);
	if (flag == LS_SAVE)
	{
		/* Save the stuff */
		process_hooks(HOOK_SAVE_GAME, "()");
	}

	if (flag == LS_LOAD)
	{
		u32b len = tmp32u;

		while (len)
		{
			char key_buf[100];

			/* Load a key */
			load_number_key(key_buf, &tmp32u);

			/* Process it -- the hooks can use it or ignore it */
			process_hooks(HOOK_LOAD_GAME, "(s,l)", key_buf, tmp32u);
			len--;
		}
	}

	/* I'm not dead yet... */
	if (!death)
	{
		/* Dead players have no dungeon */
		if (flag == LS_LOAD) note("Restoring Dungeon...");
		if ((flag == LS_LOAD) && (!do_dungeon(LS_LOAD, FALSE)))
		{
			note("Error reading dungeon data");
			return (FALSE);
		}
		if (flag == LS_SAVE) do_dungeon(LS_SAVE, FALSE);
		my_sentinel("Before ghost data", 435, flag);
		my_sentinel("After ghost data", 320, flag);
	}

	{
		byte foo = 0;
		if (flag == LS_SAVE)
		{
			/*
			 * Safety Padding. It's there
			 * for a good reason. Trust me on
			 * this. Keep this at the *END*
			 * of the file, and do *NOT* try to
			 * read it. Insert any new stuff before
			 * this position.
			 */
			do_byte(&foo, LS_SAVE);
		}
	}

	/* Success */
	return (TRUE);
}


/*
 * Actually read the savefile
 */
errr rd_savefile(void)
{
	errr err = 0;

#ifdef SAFER_PANICS
	char panic_fname[1024];
	if (!panicload)
	{
#endif /* SAFER_PANICS */

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* The savefile is a binary file */
		fff = my_fopen(savefile, "rb");

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

#ifdef BZ_SAVES
		bz_prep(LS_LOAD);
#endif /* BZ_SAVES */

#ifdef SAFER_PANICS
	}
	else
	{
		strcpy(panic_fname, savefile);
		strcat(panic_fname, ".pnc");

		/* Grab permission */
		if (savefile_setuid) safe_setuid_grab();

		/* Open panic save file */
		fff = my_fopen(panic_fname, "rb");

		/* Drop permission */
		if (savefile_setuid) safe_setuid_drop();

#ifdef BZ_SAVES
		if (fff)
		{
			bz_prep(LS_LOAD);
		}
#endif /* BZ_SAVES */

	}

#endif /* SAFER_PANICS */

	/* Paranoia */
	if (!fff) return ( -1);

	/* Call the sub-function */
	err = !do_savefile_aux(LS_LOAD);

	/* Check for errors */
	if (ferror(fff)) err = -1;

	/* Close the file */
#ifdef BZ_SAVES
	bz_done(LS_LOAD);
#endif /* BZ_SAVES */
	my_fclose(fff);

	/* Result */
	return (err);
}

/*
 * Note that this function may not be needed at all.
 * It was taken out of load_player_aux(). Do we need it?
 */
static void junkinit(void)
{
	int i, j;
	p_ptr->arena_number = 0;
	p_ptr->inside_arena = 0;
	p_ptr->inside_quest = 0;
	p_ptr->exit_bldg = TRUE;
	p_ptr->exit_bldg = TRUE;
	p_ptr->town_num = 1;
	p_ptr->wilderness_x = 4;
	p_ptr->wilderness_y = 4;
	for (i = 0; i < max_wild_x; i++)
	{
		for (j = 0; j < max_wild_y; j++)
		{
			wild_map[j][i].seed = rand_int(0x10000000);
		}
	}
}

static void morejunk(void)
{
	sp_ptr = &sex_info[p_ptr->psex]; 	/* Sex */
	rp_ptr = &race_info[p_ptr->prace]; 	/* Raceclass */
	rmp_ptr = &race_mod_info[p_ptr->pracem];
	cp_ptr = &class_info[p_ptr->pclass];
	spp_ptr = &class_info[p_ptr->pclass].spec[p_ptr->pspec];
}

#ifdef BZ_SAVES
static void do_grid(int flag)
{
	int y = 0, x = 0;
	cave_type *c_ptr;
	int ymax = cur_hgt, xmax = cur_wid;

	int part; 	/* Which section of the grid we're on */

	my_sentinel("Before grid", 17, flag);

	for (part = 0; part < 9; part++)	/* There are 8 fields to the grid, each stored
												   in a seperate data structure */
	{
		for (y = 0; y < ymax; y++)
		{
			for (x = 0; x < xmax; x++)
			{
				c_ptr = &cave[y][x];
				switch (part)
				{
				case 0:
					do_u16b(&c_ptr->info, flag);
					break;

				case 1:
					do_byte(&c_ptr->feat, flag);
					break;

				case 2:
					do_byte(&c_ptr->mimic, flag);
					break;

				case 3:
					do_u16b(&c_ptr->special, flag);
					break;

				case 4:
					do_u16b(&c_ptr->special2, flag);
					break;

				case 5:
					do_u16b(&c_ptr->t_idx, flag);
					break;

				case 6:
					do_u16b(&c_ptr->inscription, flag);
					break;

				case 7:
					do_byte(&c_ptr->mana, flag);
					break;

				case 8:
					do_u16b(&c_ptr->effect, 12, 0, flag);
					break;
				}
			}
		}
	}
	my_sentinel("In grid", 36, flag);
}
#endif /* BZ_SAVES */

#ifndef BZ_SAVES
static void do_grid(int flag)
/* Does the grid, RLE, blahblah. RLE sucks. I hate it. */
{
	int i = 0, y = 0, x = 0;
	byte count = 0;
	byte tmp8u = 0;
	s16b tmp16s = 0;
	cave_type *c_ptr;
	byte prev_char = 0;
	s16b prev_s16b = 0;
	int ymax = cur_hgt, xmax = cur_wid;

	int part; 	/* Which section of the grid we're on */

	for (part = 0; part < 9; part++)	/* There are 8 fields to the grid, each stored
												   in a seperate RLE data structure */
	{
		if (flag == LS_SAVE)
		{
			count = 0;
			prev_s16b = 0;
			prev_char = 0; 		/* Clear, prepare for RLE */
			for (y = 0; y < cur_hgt; y++)
			{
				for (x = 0; x < cur_wid; x++)
				{
					c_ptr = &cave[y][x];
					switch (part)
					{
					case 0:
						tmp16s = c_ptr->info;
						break;

					case 1:
						tmp8u = c_ptr->feat;
						break;

					case 2:
						tmp8u = c_ptr->mimic;
						break;

					case 3:
						tmp16s = c_ptr->special;
						break;

					case 4:
						tmp16s = c_ptr->special2;
						break;

					case 5:
						tmp16s = c_ptr->t_idx;
						break;

					case 6:
						tmp16s = c_ptr->inscription;
						break;

					case 7:
						tmp8u = c_ptr->mana;
						break;

					case 8:
						tmp16s = c_ptr->effect;
						break;
					}
					/* Flush a full run */
					if ((((part != 1) && (part != 2) && (part != 7)) &&
					                (tmp16s != prev_s16b)) || (((part == 1) || (part == 2)
					                                            || (part == 7)) &&
					                                           (tmp8u != prev_char)) ||
					                (count == MAX_UCHAR))
					{
						do_byte(&count, LS_SAVE);
						switch (part)
						{
						case 0:
						case 3:
						case 4:
						case 5:
						case 6:
						case 8:
							do_s16b(&prev_s16b, LS_SAVE);
							prev_s16b = tmp16s;
							break;

						case 1:
						case 2:
						case 7:
							do_byte(&prev_char, LS_SAVE);
							prev_char = tmp8u;
							break;
						}
						count = 1; 	/* Reset RLE */
					}
					else
						count++; 	/* Otherwise, keep going */
				}
			}
			/* Fallen off the end of the world, flush anything left */
			if (count)
			{
				do_byte(&count, LS_SAVE);
				switch (part)
				{
				case 0:
				case 3:
				case 4:
				case 5:
				case 6:
				case 8:
					do_s16b(&prev_s16b, LS_SAVE);
					break;

				case 1:
				case 2:
				case 7:
					do_byte(&prev_char, LS_SAVE);
					break;
				}
			}
		}
		if (flag == LS_LOAD)
		{
			x = 0;
			for (y = 0; y < ymax; )
			{
				do_byte(&count, LS_LOAD);
				switch (part)
				{
				case 0:
				case 3:
				case 4:
				case 5:
				case 6:
				case 8:
					do_s16b(&tmp16s, LS_LOAD);
					break;

				case 1:
				case 2:
				case 7:
					do_byte(&tmp8u, LS_LOAD);
					break;
				}
				for (i = count; i > 0; i--)	/* RLE */
				{
					c_ptr = &cave[y][x];
					switch (part)
					{
					case 0:
						c_ptr->info = tmp16s;
						break;

					case 1:
						c_ptr->feat = tmp8u;
						break;

					case 2:
						c_ptr->mimic = tmp8u;
						break;

					case 3:
						c_ptr->special = tmp16s;
						break;

					case 4:
						c_ptr->special2 = tmp16s;
						break;

					case 5:
						c_ptr->t_idx = tmp16s;
						break;

					case 6:
						c_ptr->inscription = tmp16s;
						break;

					case 7:
						c_ptr->mana = tmp8u;
						break;

					case 8:
						c_ptr->effect = tmp16s;
						break;
					}
					if (++x >= xmax)
					{
						/* Wrap */
						x = 0;
						if ((++y) >= ymax) break;
					}
				}
			}
		}
	}
}

#endif /* !BZ_SAVES */

static void my_sentinel(char *place, u16b value, int flag)
/* This function lets us know exactly where a savefile is
   broken by reading/writing conveniently a sentinel at this
   spot */
{
	if (flag == LS_SAVE)
	{
		do_u16b(&value, flag);
		return;
	}
	if (flag == LS_LOAD)
	{
		u16b found;
		do_u16b(&found, flag);
		if (found == value)		/* All is good */
			return;
		/* All is bad */
		note(format("Savefile broken %s", place));
		return;
	}
	note(format("Impossible has occurred")); 	/* Programmer error */
	exit(0);
}

/********** Variable savefile stuff **************/

/*
 * Add num slots to the savefile
 */
void register_savefile(int num)
{
	extra_savefile_parts += (num > 0) ? num : 0;
}

void save_number_key(char *key, u32b val)
{
	byte len = strlen(key);

	do_byte(&len, LS_SAVE);
	while (*key)
	{
		do_byte((byte*)key, LS_SAVE);
		key++;
	}
	do_u32b(&val, LS_SAVE);
}

void load_number_key(char *key, u32b *val)
{
	byte len, i = 0;

	do_byte(&len, LS_LOAD);
	while (i < len)
	{
		do_byte((byte*)&key[i], LS_LOAD);
		i++;
	}
	key[i] = '\0';
	do_u32b(val, LS_LOAD);
}
