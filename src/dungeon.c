/* File: dungeon.c */

/* Purpose: Angband game engine */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#include "lua/lua.h"
#include "tolua.h"
extern lua_State* L;

#define TY_CURSE_CHANCE 100
#define DG_CURSE_CHANCE 50
#define AUTO_CURSE_CHANCE 15
#define CHAINSWORD_NOISE 100

/*
 * I created this when a bug misplaced my character and the game wasn't able
 * to load it again.. very frustrating.
 * So this hack will generate a new level without calling dungeon(), and
 * then the normal behavior will apply.
 */
/* #define SAVE_HACK */
#ifdef SAVE_HACK
bool save_hack = TRUE;
#endif


/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
byte value_check_aux1(object_type *o_ptr)
{
	/* Artifacts */
	if (artifact_p(o_ptr))
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr)) return (SENSE_TERRIBLE);

		/* Normal */
		return (SENSE_SPECIAL);
	}

	/* Ego-Items */
	if (ego_item_p(o_ptr))
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr)) return (SENSE_WORTHLESS);

		/* Normal */
		return (SENSE_EXCELLENT);
	}

	/* Cursed items */
	if (cursed_p(o_ptr)) return (SENSE_CURSED);

	/* Good "armor" bonus */
	if (o_ptr->to_a > 0) return (SENSE_GOOD_HEAVY);

	/* Good "weapon" bonus */
	if (o_ptr->to_h + o_ptr->to_d > 0) return (SENSE_GOOD_HEAVY);

	/* Default to "average" */
	return (SENSE_AVERAGE);
}

/* Amy: pseudo-id for stuff like rings, amulets, instruments... */
byte value_check_aux1_misc(object_type *o_ptr)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Artifacts */
	if (artifact_p(o_ptr))
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr)) return (SENSE_TERRIBLE);

		/* Normal */
		return (SENSE_SPECIAL);
	}

	/* Ego-Items */
	if (ego_item_p(o_ptr))
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr)) return (SENSE_WORTHLESS);

		/* Normal */
		return (SENSE_EXCELLENT);
	}

	/* Cursed items */
	if (cursed_p(o_ptr)) return (SENSE_CURSED);

	/* no artifact, no ego, not cursed? then it depends on value --Amy */
	if (k_ptr->cost < 6) return (SENSE_WORTHLESS);

	if (k_ptr->cost < 1000) return (SENSE_AVERAGE);

	if (k_ptr->cost < 10000) return (SENSE_GOOD_HEAVY);

	if (k_ptr->cost >= 10000) return (SENSE_EXCELLENT);

	/* Default to "average" */
	return (SENSE_AVERAGE);
}

byte value_check_aux1_magic(object_type *o_ptr)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];


	switch (o_ptr->tval)
	{
		/* Scrolls, Potions, Wands, Staves and Rods */
	case TV_SCROLL:
	case TV_POTION:
	case TV_POTION2:
	case TV_WAND:
	case TV_STAFF:
	case TV_ROD:
	case TV_ROD_MAIN:
		{
			/* "Cursed" scrolls/potions have a cost of 0 */
			if (k_ptr->cost == 0) return (SENSE_TERRIBLE);

			/* Artifacts */
			if (artifact_p(o_ptr)) return (SENSE_SPECIAL);

			/* Scroll of Nothing, Apple Juice, etc. */
			if (k_ptr->cost < 3) return (SENSE_WORTHLESS);

			/*
			 * Identify, Phase Door, Cure Light Wounds, etc. are
			 * just average
			 */
			if (k_ptr->cost < 100) return (SENSE_AVERAGE);

			/* Enchant Armor, *Identify*, Restore Stat, etc. */
			if (k_ptr->cost < 10000) return (SENSE_GOOD_HEAVY);

			/* Acquirement, Deincarnation, Strength, Blood of Life, ... */
			if (k_ptr->cost >= 10000) return (SENSE_EXCELLENT);

			break;
		}

		/* Food */
	case TV_FOOD:
		{
			/* "Cursed" food */
			if (k_ptr->cost == 0) return (SENSE_TERRIBLE);

			/* Artifacts */
			if (artifact_p(o_ptr)) return (SENSE_SPECIAL);

			/* Normal food (no magical properties) */
			if (k_ptr->cost <= 10) return (SENSE_AVERAGE);

			/* Everything else is good */
			if (k_ptr->cost > 10) return (SENSE_GOOD_HEAVY);

			break;
		}
	}

	/* No feeling */
	return (SENSE_NONE);
}


/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
byte value_check_aux2(object_type *o_ptr)
{
	/* Cursed items (all of them) */
	if (cursed_p(o_ptr)) return (SENSE_CURSED);

	/* Artifacts -- except cursed/broken ones */
	if (artifact_p(o_ptr)) return (SENSE_GOOD_LIGHT);

	/* Ego-Items -- except cursed/broken ones */
	if (ego_item_p(o_ptr)) return (SENSE_GOOD_LIGHT);

	/* Good armor bonus */
	if (o_ptr->to_a > 0) return (SENSE_GOOD_LIGHT);

	/* Good weapon bonuses */
	if (o_ptr->to_h + o_ptr->to_d > 0) return (SENSE_GOOD_LIGHT);

	/* No feeling */
	return (SENSE_NONE);
}


byte value_check_aux2_magic(object_type *o_ptr)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];


	switch (o_ptr->tval)
	{
		/* Scrolls, Potions, Wands, Staves and Rods */
	case TV_SCROLL:
	case TV_POTION:
	case TV_POTION2:
	case TV_WAND:
	case TV_STAFF:
	case TV_ROD:
	case TV_ROD_MAIN:
		{
			/* "Cursed" scrolls/potions have a cost of 0 */
			if (k_ptr->cost == 0) return (SENSE_CURSED);

			/* Artifacts */
			if (artifact_p(o_ptr)) return (SENSE_GOOD_LIGHT);

			/* Scroll of Nothing, Apple Juice, etc. */
			if (k_ptr->cost < 3) return (SENSE_AVERAGE);

			/*
			 * Identify, Phase Door, Cure Light Wounds, etc. are
			 * just average
			 */
			if (k_ptr->cost < 100) return (SENSE_AVERAGE);

			/* Enchant Armor, *Identify*, Restore Stat, etc. */
			if (k_ptr->cost < 10000) return (SENSE_GOOD_LIGHT);

			/* Acquirement, Deincarnation, Strength, Blood of Life, ... */
			if (k_ptr->cost >= 10000) return (SENSE_GOOD_LIGHT);

			break;
		}

		/* Food */
	case TV_FOOD:
		{
			/* "Cursed" food */
			if (k_ptr->cost == 0) return (SENSE_CURSED);

			/* Artifacts */
			if (artifact_p(o_ptr)) return (SENSE_GOOD_LIGHT);

			/* Normal food (no magical properties) */
			if (k_ptr->cost <= 10) return (SENSE_AVERAGE);

			/* Everything else is good */
			if (k_ptr->cost > 10) return (SENSE_GOOD_LIGHT);

			break;
		}
	}

	/* No feeling */
	return (SENSE_NONE);
}

byte value_check_aux2_misc(object_type *o_ptr)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Cursed items (all of them) */
	if (cursed_p(o_ptr)) return (SENSE_CURSED);

	/* Artifacts -- except cursed/broken ones */
	if (artifact_p(o_ptr)) return (SENSE_GOOD_LIGHT);

	/* Ego-Items -- except cursed/broken ones */
	if (ego_item_p(o_ptr)) return (SENSE_GOOD_LIGHT);

	/* no artifact, no ego, not cursed? then it depends on value --Amy */
	if (k_ptr->cost < 6) return (SENSE_AVERAGE);

	if (k_ptr->cost < 1000) return (SENSE_AVERAGE);

	if (k_ptr->cost < 10000) return (SENSE_GOOD_LIGHT);

	if (k_ptr->cost >= 10000) return (SENSE_GOOD_LIGHT);

	/* No feeling */
	return (SENSE_NONE);
}


/*
 * Can a player be resurrected?
 */
static bool granted_resurrection(void)
{
	PRAY_GOD(GOD_ERU)
	{
		if (p_ptr->grace > 100000)
		{
			if (magik(70)) return (TRUE);
			else return (FALSE);
		}
	}
	return (FALSE);
}

byte select_sense(object_type *o_ptr, bool ok_combat, bool ok_magic, bool ok_misc)
{
	/* Valid "tval" codes */
	switch (o_ptr->tval)
	{
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
	case TV_AMMO_PISTOL:
	case TV_AMMO_RIFLE:
	case TV_AMMO_SHOTGUN:
	case TV_AMMO_SMG:
	case TV_AMMO_ASSAULT:
	case TV_BOW:
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_SWORD:
	case TV_MSTAFF:
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
	case TV_BOOMERANG:
	case TV_TRAPKIT:
		{
			if (ok_combat) return 1;
			break;
		}

	case TV_POTION:
	case TV_POTION2:
	case TV_SCROLL:
	case TV_WAND:
	case TV_STAFF:
	case TV_ROD:
	case TV_ROD_MAIN:
		{
			if (ok_magic) return 2;
			break;
		}

		/* Dual use? */
	case TV_DAEMON_BOOK:
		{
			if (ok_combat || ok_magic) return 1;
			break;
		}
	case TV_INSTRUMENT:
	case TV_AMULET:
	case TV_RING:
		{
			if (ok_misc) return 3;
			break;
		}
	}
	return 0;
}

/*
 * Sense the inventory
 *
 * Combat items (weapons and armour) - Fast, weak if combat skill < 10, strong
 * otherwise.
 *
 * Magic items (scrolls, staffs, wands, potions etc) - Slow, weak if
 * magic skill < 10, strong otherwise.
 *
 * addition by Amy: misc items (instruments, rings, amulets) - Relatively slow,
 * weak if sum of combat + magic skill < 20, strong otherwise.
 *
 * It shouldn't matter a lot to discriminate against magic users, because
 * they learn one form of ID or another, and because most magic items are
 * easy_know.
 */
static void sense_inventory(void)
{
	int i, combat_lev, magic_lev;

	bool heavy_combat, heavy_magic, heavy_misc;
	bool ok_combat, ok_magic, ok_misc;

	byte feel = SENSE_NONE; /* appease compiler which thinks this may be used uninitialized --Amy */

	object_type *o_ptr;

	char o_name[80];

	/* pseudo id nastytrap prevents any pseudo identification --Amy */
	if (p_ptr->nastytrap106) return;

	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;

	/* Can we pseudo id */
#if 0

	if (0 == rand_int(133 - get_skill_scale(SKILL_COMBAT, 130))) ok_combat = TRUE;
	if (0 == rand_int(133 - get_skill_scale(SKILL_MAGIC, 130))) ok_magic = TRUE;

#endif

	/*
	 * In Angband, the chance of pseudo-id uses two different formulae:
	 *
	 * (1) Fast. 0 == rand_int(BASE / (plev * plev + 40)
	 * (2) Slow. 0 == rand_int(BASE / (plev + 5)
	 *
	 * Warriors: Fase with BASE == 9000
	 * Magi: Slow with BASE == 240000
	 * Priests: Fast with BASE == 10000
	 * Rogues: Fase with BASE == 20000
	 * Rangers: Slow with BASE == 120000
	 * Paladins: Fast with BASE == 80000
	 *
	 * In other words, those who have identify spells are penalised.
	 * The problems with Pern/Tome since it externalised player classes
	 * is that it uses the same and slow formula for spellcasters and
	 * fighters.
	 *
	 * In the following code, combat item pseudo-ID improves exponentially,
	 * (fast with BASE 9000) and magic one linear (slow with base 60000 --
	 * twice faster than V rangers).
	 *
	 * I hope this makes it closer to the original model -- pelpel
	 */

	/* The combat skill affects weapon/armour pseudo-ID */
	combat_lev = get_skill(SKILL_COMBAT);

	/* Use the fast formula */
	ok_combat = (0 == rand_int(9000L / (combat_lev * combat_lev + 40)));

	/* The magic skill affects magic item pseudo-ID */
	magic_lev = get_skill(SKILL_MAGIC);

	/*
	 * Use the slow formula, because spellcasters have id spells
	 *
	 * Lowered the base value because V rangers are known to have
	 * pretty useless pseudo-ID. This should make it ten times more often.
	 */
	ok_magic = (0 == rand_int(12000L / (magic_lev + 5)));

	ok_misc = (0 == rand_int(10000L / (combat_lev + magic_lev + 10)));

	/* Both ID rolls failed */
	if (!ok_combat && !ok_magic) return;

	/* Higher skill levels give the player better sense of items */
	heavy_combat = (combat_lev > 10) ? TRUE : FALSE;
	heavy_magic = (magic_lev > 10) ? TRUE : FALSE;
	heavy_misc = ((combat_lev + magic_lev) > 20) ? TRUE : FALSE;


	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		byte okay = 0;

		o_ptr = &p_ptr->inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		okay = select_sense(o_ptr, ok_combat, ok_magic, ok_misc);
		/* 1 = combat, 2 = magic, 3 = misc */

		/* Skip non-sense machines */
		if (!okay) continue;

		/* We know about it already, do not tell us again */
		if (o_ptr->ident & (IDENT_SENSE)) continue;

		/* It is fully known, no information needed */
		if (object_known_p(o_ptr)) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_WIELD) && (0 != rand_int(5))) continue;

		/* Check for a feeling */
		if (okay == 1)
		{
			feel = (heavy_combat ? value_check_aux1(o_ptr) : value_check_aux2(o_ptr));
		}
		else if (okay == 2)
		{
			feel = (heavy_magic ? value_check_aux1_magic(o_ptr) : value_check_aux2_magic(o_ptr));
		}
		else if (okay == 3)
		{
			feel = (heavy_misc ? value_check_aux1_misc(o_ptr) : value_check_aux2_misc(o_ptr));
		}

		/* Skip non-feelings */
		if (feel == SENSE_NONE) continue;

		/* Stop everything */
		if (disturb_minor && !p_ptr->nastytrap160) disturb(0, 0);

		/* Get an object description */
		object_desc(o_name, o_ptr, FALSE, 0);

		/* Message (equipment) */
		if (i >= INVEN_WIELD)
		{
			msg_format("You feel the %s (%c) you are %s %s %s...",
			           o_name, index_to_label(i), describe_use(i),
			           ((o_ptr->number == 1) ? "is" : "are"), sense_desc[feel]);
		}

		/* Message (inventory) */
		else
		{
			msg_format("You feel the %s (%c) in your pack %s %s...",
			           o_name, index_to_label(i),
			           ((o_ptr->number == 1) ? "is" : "are"), sense_desc[feel]);
		}

		/* We have "felt" it */
		o_ptr->ident |= (IDENT_SENSE);

		/* Set sense property */
		o_ptr->sense = feel;

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);
	}

	/* Squelch ! */
	squeltch_inventory();
}


/*
 * Go to any level (ripped off from wiz_jump)
 */
static void pattern_teleport(void)
{
	/* Ask for level */
	if (get_check("Teleport level? "))
	{
		/* holy shit that was giving WIZMODE levelports????? man... these programmers... --Amy */
		(void)teleport_player_level();
		return;
	}
	else if (get_check("Normal teleport? "))
	{
		teleport_player(200);
		return;
	}
	else
	{
		return;
	}

	/* Paranoia */
	if (command_arg < 0) command_arg = 0;

	/* Paranoia */
	if (command_arg > 99) command_arg = 99;

	/* Accept request */
	msg_format("You teleport to dungeon level %d.", command_arg);

	if (autosave_l)
	{
		is_autosave = TRUE;
		msg_print("Autosaving the game...");
		do_cmd_save_game();
		is_autosave = FALSE;
	}

	/* Change level */
	dun_level = command_arg;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


/*
 * Returns TRUE if we are on the Straight Road...
 */
static bool pattern_effect(void)
{
	if ((cave[p_ptr->py][p_ptr->px].feat < FEAT_PATTERN_START) ||
	                (cave[p_ptr->py][p_ptr->px].feat > FEAT_PATTERN_XTRA2)) return (FALSE);

	if (cave[p_ptr->py][p_ptr->px].feat == FEAT_PATTERN_END)
	{
		(void)set_poisoned(0);
		(void)set_image(0);
		(void)set_stun(0);
		(void)set_cut(0);
		(void)set_blind(0);
		(void)set_afraid(0);
		(void)do_res_stat(A_STR, TRUE);
		(void)do_res_stat(A_INT, TRUE);
		(void)do_res_stat(A_WIS, TRUE);
		(void)do_res_stat(A_DEX, TRUE);
		(void)do_res_stat(A_CON, TRUE);
		(void)do_res_stat(A_CHR, TRUE);
		(void)restore_level();
		(void)hp_player(1000);
		cave_set_feat(p_ptr->py, p_ptr->px, FEAT_PATTERN_OLD);
		msg_print("This section of the Straight Road looks less powerful.");
	}


	/*
	 * We could make the healing effect of the
	 * Pattern center one-time only to avoid various kinds
	 * of abuse, like luring the win monster into fighting you
	 * in the middle of the pattern...
	 */
	else if (cave[p_ptr->py][p_ptr->px].feat == FEAT_PATTERN_OLD)
	{
		/* No effect */
	}
	else if (cave[p_ptr->py][p_ptr->px].feat == FEAT_PATTERN_XTRA1)
	{
		pattern_teleport();
	}
	else if (cave[p_ptr->py][p_ptr->px].feat == FEAT_PATTERN_XTRA2)
	{
		if (!(p_ptr->invuln))
			take_hit(200, "walking the corrupted Straight Road");
	}

	else
	{
		if (!(p_ptr->invuln))
			take_hit(damroll(1, 3), "walking the Straight Road");
	}

	return (TRUE);
}


/*
 * If player has inscribed the object with "!!", let him know when it's
 * recharged. -LM-
 */
static void recharged_notice(object_type *o_ptr)
{
	char o_name[80];

	cptr s;


	/* No inscription */
	if (!o_ptr->note) return;

	/* Find a '!' */
	s = strchr(quark_str(o_ptr->note), '!');

	/* Process notification request. */
	while (s)
	{
		/* Find another '!' */
		if (s[1] == '!')
		{
			/* Describe (briefly) */
			object_desc(o_name, o_ptr, FALSE, 0);

			/* Notify the player */
			if (o_ptr->number > 1)
			{
				msg_format("Your %s are recharged.", o_name);
			}
			else
			{
				msg_format("Your %s is recharged.", o_name);
			}

			/* Done. */
			return;
		}

		/* Keep looking for '!'s */
		s = strchr(s + 1, '!');
	}
}



/*
 * Regenerate hit points				-RAK-
 */
static void regenhp(int percent)
{
	s32b new_chp, new_chp_frac;

	int old_chp;


	/* Only if alive */
	if (!(p_ptr->necro_extra & CLASS_UNDEAD))
	{
		/* Save the old hitpoints */
		old_chp = p_ptr->chp;

		/* Extract the new hitpoints */
		new_chp = ((long)p_ptr->mhp) * percent + PY_REGEN_HPBASE;

		/* div 65536 */
		p_ptr->chp += new_chp >> 16;

		/* check for overflow */
		if ((p_ptr->chp < 0) && (old_chp > 0)) p_ptr->chp = MAX_SHORT;

		/* mod 65536 */
		new_chp_frac = (new_chp & 0xFFFF) + p_ptr->chp_frac;

		if (new_chp_frac >= 0x10000L)
		{
			p_ptr->chp_frac = new_chp_frac - 0x10000L;
			p_ptr->chp++;
		}
		else
		{
			p_ptr->chp_frac = new_chp_frac;
		}

		/* Fully healed */
		if (p_ptr->chp >= p_ptr->mhp)
		{
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
		}

		/* Notice changes */
		if (old_chp != p_ptr->chp)
		{
			/* Redraw */
			p_ptr->redraw |= (PR_HP);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}
	}
}


/*
 * Regenerate mana points				-RAK-
 */
static void regenmana(int percent)
{
	s32b new_mana, new_mana_frac;

	int old_csp;

	/* Incraese regen with int */
	percent += adj_str_blow[p_ptr->stat_ind[A_INT]] * 3;

	/* silenced? then don't regenerate, and always have 0 mana --Amy */
	if (p_ptr->tim_manasilence && (percent > 0)) {
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;
		return;
	}

	/* mana void? then don't regenerate --Amy */
	if (p_ptr->tim_manavoid && (percent > 0)) return;

	old_csp = p_ptr->csp;
	new_mana = ((long)p_ptr->msp) * percent + PY_REGEN_MNBASE;

	/* div 65536 */
	p_ptr->csp += new_mana >> 16;

	/* check for overflow */
	if ((p_ptr->csp < 0) && (old_csp > 0))
	{
		p_ptr->csp = MAX_SHORT;
	}

	/* mod 65536 */
	new_mana_frac = (new_mana & 0xFFFF) + p_ptr->csp_frac;

	if (new_mana_frac >= 0x10000L)
	{
		p_ptr->csp_frac = new_mana_frac - 0x10000L;
		p_ptr->csp++;
	}
	else
	{
		p_ptr->csp_frac = new_mana_frac;
	}

	/* Must set frac to zero even if equal */
	if (p_ptr->csp >= p_ptr->msp)
	{
		p_ptr->csp = p_ptr->msp;
		p_ptr->csp_frac = 0;
	}

	/* Redraw mana */
	if (old_csp != p_ptr->csp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}
}






/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_monsters(void)
{
	int i, frac;

	object_type *o_ptr = &p_ptr->inventory[INVEN_CARRY];


	if (o_ptr->k_idx)
	{
		monster_race *r_ptr = &r_info[o_ptr->pval];

		/* Allow regeneration (if needed) */
		if (o_ptr->pval2 < o_ptr->pval3)
		{
			/* Hack -- Base regeneration */
			frac = o_ptr->pval3 / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & (RF2_REGENERATE)) frac *= 2;


			/* Hack -- Regenerate */
			o_ptr->pval2 += frac;

			/* Do not over-regenerate */
			if (o_ptr->pval2 > o_ptr->pval3) o_ptr->pval2 = o_ptr->pval3;

			/* Redraw (later) */
			p_ptr->redraw |= (PR_MH);
		}
	}

	/* Regenerate everyone */
	for (i = 1; i < m_max; i++)
	{
		/* Check the i'th monster */
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = race_inf(m_ptr);

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Dont regen bleeding/poisonned monsters */
		if (m_ptr->bleeding || m_ptr->poisoned) continue;

		/* Allow regeneration (if needed) */
		if (m_ptr->hp < m_ptr->maxhp)
		{
			/* Hack -- Base regeneration */
			frac = m_ptr->maxhp / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & (RF2_REGENERATE)) frac *= 2;


			/* Hack -- Regenerate */
			m_ptr->hp += frac;

			/* Do not over-regenerate */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

			/* Redraw (later) if needed */
			if (health_who == i) p_ptr->redraw |= (PR_HEALTH);
		}
	}
}


/*
 * Forcibly pseudo-identify an object in the inventory
 * (or on the floor)
 */
bool psychometry(void)
{
	int item;

	object_type *o_ptr;

	char o_name[80];

	byte feel;

	cptr q, s;


	/* Get an item */
	q = "Meditate on which item? ";
	s = "You have nothing appropriate. There needs to be a very looooooong message now so that your next keystroke isn't misinterpreted as something you didn't want to do at all, so as to force a --More--.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* It is fully known, no information needed */
	if ((object_known_p(o_ptr)) || (o_ptr->ident & IDENT_SENSE))
	{
		msg_print("You cannot find out anything more about that.");
		return (TRUE);
	}

	/* Check for a feeling */
	feel = value_check_aux1_magic(o_ptr);
	if (feel == SENSE_NONE) feel = value_check_aux1(o_ptr);

	/* Get an object description */
	object_desc(o_name, o_ptr, FALSE, 0);

	/* Skip non-feelings */
	if (!feel)
	{
		msg_format("You do not perceive anything unusual about the %s.", o_name);
		return (TRUE);
	}

	msg_format("You feel that the %s %s %s...",
	           o_name, ((o_ptr->number == 1) ? "is" : "are"), sense_desc[feel]);

	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* Set sense property */
	o_ptr->sense = feel;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Something happened */
	return (TRUE);
}

/*
 * can the player be ambushed? --Amy
 * repel potions prevent it for a while like in Pokemon
 * sneakiness skill reduces the risk 
 */
bool ambush_ok(void)
{
	if (p_ptr->tim_repelling) return FALSE;

	if (get_skill(SKILL_SNEAK) >= 1) {
		if (randint(20 + get_skill(SKILL_SNEAK)) > 20) return FALSE;
	}

	return TRUE;
}

/*
 * Does an object decay?
 *
 * Should belong to object1.c, renamed to object_decays() -- pelpel
 */
bool decays(object_type *o_ptr)
{
	u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;


	/* Extract some flags */
	object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

	if (f3 & TR3_DECAY) return (TRUE);

	return (FALSE);
}


static int process_lasting_spell(s16b music)
{
	int oldtop, use_mana;

	if (music > 0) return FALSE;

	oldtop = lua_gettop(L);

	music = -music;

	/* Push the function */
	lua_getglobal(L, "exec_lasting_spell");

	/* Push the spell */
	tolua_pushnumber(L, music);

	/* Call the function */
	if (lua_call(L, 1, 1))
	{
		cmsg_format(TERM_VIOLET, "ERROR in lua_call while calling lasting spell");
		return 0;
	}

	use_mana = tolua_getnumber(L, -(lua_gettop(L) - oldtop), 0);
	lua_settop(L, oldtop);
	return use_mana;
}

static void gere_class_special()
{
	switch (p_ptr->druid_extra2)
	{
		/* Lay a path of mana on the floor */
	case CLASS_MANA_PATH:
		{
			/* Does the player have enought mana ? */
			if (p_ptr->csp < (s32b)(p_ptr->druid_extra & 255))
			{
				p_ptr->druid_extra = 0;
				p_ptr->druid_extra2 = CLASS_NONE;
				msg_print("You stop laying a mana path.");
			}
			else
			{
				/* Use some mana */
				p_ptr->csp -= (p_ptr->druid_extra & 255);

				if ((p_ptr->druid_extra >> 8) & CLASS_MANA_PATH_ERASE)
				{
					/* Absorb some of the mana of the grid */
					p_ptr->csp += cave[p_ptr->py][p_ptr->px].mana / 50;
					if (p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;

					/* Set the new grid mana */
					cave[p_ptr->py][p_ptr->px].mana = p_ptr->druid_extra & 255;
				}
				else
				{
					int m = cave[p_ptr->py][p_ptr->px].mana;

					if (m + (p_ptr->druid_extra & 255) > 255)
					{
						cave[p_ptr->py][p_ptr->px].mana = 255;
					}
					else
					{
						cave[p_ptr->py][p_ptr->px].mana += p_ptr->druid_extra & 255;
					}
				}
			}

			break;
		}

		/* Lay a path of mana on the floor */
	case CLASS_WINDS_MANA:
		{
			/* Does the player have enought mana ? */
			if (p_ptr->csp < (s32b)(p_ptr->druid_extra & 255))
			{
				p_ptr->druid_extra = CLASS_NONE;
				msg_print("You stop expulsing mana winds.");
			}
			else
			{
				int dam = 0;

				/* Use some mana */
				p_ptr->csp -= (p_ptr->druid_extra & 255);

				if ((p_ptr->druid_extra >> 8) & CLASS_MANA_PATH_ERASE)
				{
					dam = (p_ptr->druid_extra & 255) + 256;
				}
				else
				{
					dam = (p_ptr->druid_extra & 255);
				}

				fire_explosion(p_ptr->py, p_ptr->px, GF_WINDS_MANA, 2, dam);
			}

			break;
		}

	case CLASS_CANALIZE_MANA:
		{
			if (p_ptr->druid_extra & CLASS_CANALIZE_MANA_EXTRA)
			{
				p_ptr->csp += cave[p_ptr->py][p_ptr->px].mana / 10;
			}
			else
			{
				p_ptr->csp += cave[p_ptr->py][p_ptr->px].mana / 20;
			}

			if (p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;

			cave[p_ptr->py][p_ptr->px].mana = 0;

			break;
		}

		/* CLASS_NONE, possibly others? */
	default:
		{
			/* No mana update */
			return;
		}
	}

	/* Redraw mana */
	p_ptr->update |= (PU_BONUS);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
}


static void check_music()
{
	int use_mana;

	/* Music sung by player */
	if (!p_ptr->music_extra) return;

	use_mana = process_lasting_spell(p_ptr->music_extra);

	if (p_ptr->csp < use_mana)
	{
		msg_print("You stop your spell.");
		p_ptr->music_extra = MUSIC_NONE;
		p_ptr->music_extra2 = MUSIC_NONE;
		p_ptr->update |= (PU_BONUS | PU_TORCH | PU_BODY | PU_POWERS);
	}
	else
	{
		p_ptr->csp -= use_mana;

		/* Redraw mana */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}
}


/*
 * Generate the feature effect
 */
void apply_effect(int y, int x)
{
	cave_type *c_ptr = &cave[y][x];

	feature_type *f_ptr = &f_info[c_ptr->feat];


	if (f_ptr->d_frequency[0] != 0)
	{
		int i;

		for (i = 0; i < 4; i++)
		{
			/* Check the frequency */
			if (f_ptr->d_frequency[i] == 0) continue;

			if (((turn % f_ptr->d_frequency[i]) == 0) &&
			                ((f_ptr->d_side[i] != 0) || (f_ptr->d_dice[i] != 0)))
			{
				int l, dam = 0;
				int d = f_ptr->d_dice[i], s = f_ptr->d_side[i];

				if (d == -1) d = p_ptr->lev;
				if (s == -1) s = p_ptr->lev;

				/* Roll damage */
				for (l = 0; l < d; l++)
				{
					dam += randint(s);
				}

				/* Apply damage */
				project( -100, 0, y, x, dam, f_ptr->d_type[i],
				         PROJECT_KILL | PROJECT_HIDE);

				/* Hack -- notice death */
				if (!alive || death) return;
			}

			if (p_ptr->nastytrap58 && (( (turn + 910) % f_ptr->d_frequency[i]) == 1) &&
			                ((f_ptr->d_side[i] != 0) || (f_ptr->d_dice[i] != 0)))
			{
				int l, dam = 0;
				int d = f_ptr->d_dice[i], s = f_ptr->d_side[i];

				if (d == -1) d = p_ptr->lev;
				if (s == -1) s = p_ptr->lev;

				/* Roll damage */
				for (l = 0; l < d; l++)
				{
					dam += randint(s);
				}

				/* Apply damage */
				project( -100, 0, y, x, dam, f_ptr->d_type[i],
				         PROJECT_KILL | PROJECT_HIDE);

				/* Hack -- notice death */
				if (!alive || death) return;
			}
		}
	}
}


#if 0
/*
 * Activate corruptions' effects on player
 *
 * All the rolls against arbitrarily chosen numbers are normalised
 * (i.e. zero). They might have some cabalistic(?) significance,
 * but I seriously doubt if processors take care of the Judeo-Christian
 * tradition :) -- pelpel
 */
static void process_corruption_effects(void)
{}

#endif



#ifdef pelpel

/*
 * Handle staying spell effects once every 10 game turns
 */
static void process_effects(void)
{
	int i, j;

	/* Every 10 game turns */
	if (turn % 10) return;

	/* Not in the small-scale wilderness map */
	if (p_ptr->wild_mode) return;


	/* Handle spell effects */
	for (j = 0; j < cur_hgt - 1; j++)
	{
		for (i = 0; i < cur_wid - 1; i++)
		{
			int e = cave[j][i].effect;

			if (e)
			{
				effect_type *e_ptr = &effects[e];

				if (e_ptr->time)
				{
					/* Apply damage */
					project(0, 0, j, i, e_ptr->dam, e_ptr->type,
					        PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE);
				}
				else
				{
					cave[j][i].effect = 0;
				}

				/* Hack -- notice death */
				if (!alive || death) return;

				if (((e_ptr->flags & EFF_WAVE) && !(e_ptr->flags & EFF_LAST)) || ((e_ptr->flags & EFF_STORM) && !(e_ptr->flags & EFF_LAST)))
				{
					if (distance(e_ptr->cy, e_ptr->cx, j, i) < e_ptr->rad - 1)
						cave[j][i].effect = 0;
				}
			}
		}
	}


	/* Reduce & handle effects */
	for (i = 0; i < MAX_EFFECTS; i++)
	{
		/* Skip empty slots */
		if (effects[i].time == 0) continue;

		/* Reduce duration */
		effects[i].time--;

		/* Creates a "wave" effect*/
		if (effects[i].flags & EFF_WAVE)
		{
			effect_type *e_ptr = &effects[i];
			int x, y;

			e_ptr->rad++;
			for (y = e_ptr->cy - e_ptr->rad; y <= e_ptr->cy + e_ptr->rad; y++)
			{
				for (x = e_ptr->cx - e_ptr->rad; x <= e_ptr->cx + e_ptr->rad; x++)
				{
					if (!in_bounds(y, x)) continue;

					if (los(e_ptr->cy, e_ptr->cx, y, x) &&
					                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
						cave[y][x].effect = i;
				}
			}
		}
		/* Creates a "storm" effect*/
		else if (effects[i].flags & EFF_STORM)
		{
			effect_type *e_ptr = &effects[i];
			int x, y;

			e_ptr->cy = p_ptr->py;
			e_ptr->cx = p_ptr->px;
			for (y = e_ptr->cy - e_ptr->rad; y <= e_ptr->cy + e_ptr->rad; y++)
			{
				for (x = e_ptr->cx - e_ptr->rad; x <= e_ptr->cx + e_ptr->rad; x++)
				{
					if (!in_bounds(y, x)) continue;

					if (los(e_ptr->cy, e_ptr->cx, y, x) &&
					                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
						cave[y][x].effect = i;
				}
			}
		}
	}

	/* Apply sustained effect in the player grid, if any */
	apply_effect(p_ptr->py, p_ptr->px);
}

#endif /* pelpel */


/* XXX XXX XXX */
bool is_recall = FALSE;


/*
 * Handle certain things once every 10 game turns
 *
 * Note that a single movement in the overhead wilderness mode
 * consumes 132 times as much energy as a normal one...
 */
static void process_world(void)
{
	timer_type *t_ptr;

	int x, y, i, j;

	int regen_amount;
	bool cave_no_regen = FALSE;
	int upkeep_factor = 0;

	dungeon_info_type *d_ptr = &d_info[dungeon_type];

	cave_type *c_ptr;

	object_type *o_ptr;
	object_kind *k_ptr;
	u32b f1 = 0 , f2 = 0 , f3 = 0, f4 = 0, f5 = 0, f6 = 0, f7 = 0, f8 = 0, f9 = 0, f10 = 0, esp = 0;

	int monsterallocchance;

	/*
	 * Every 10 game turns -- which means this section is invoked once
	 * in a player turn under the normal speed, and 132 times in a move
	 * in the reduced map mode.
	 */
	if (turn % 10) return;

	/*
	 * I don't know if this is the right thing to do because I'm totally
	 * ignorant (yes, I must admit that...) about the scripting part of
	 * the game, but since there have been complaints telling us it
	 * runs terribly slow in the reduced map mode... -- pelpel
	 *
	 * Note to coders: if it is desirable to make this active in the
	 * reduced map mode, remove the if condition surrounding the line
	 * and move the code inside into every 1000 game turns section.
	 */
	if (dun_level || (!p_ptr->wild_mode))
	{
		/* Let the script live! */
		process_hooks(HOOK_PROCESS_WORLD, "()");

		/* Handle the player song */
		check_music();
	}

	/* Handle the timers */
	for (t_ptr = gl_timers; t_ptr != NULL; t_ptr = t_ptr->next)
	{
		if (!t_ptr->enabled) continue;

		t_ptr->countdown--;
		if (!t_ptr->countdown)
		{
			t_ptr->countdown = t_ptr->delay;
			call_lua(t_ptr->callback, "()", "");
		}
	}

	/* Handle class special actions */
	gere_class_special();

	/* Check the fate
	 * Amy edit: it makes less than zero sense if you can't get them at lower XLs! man! why do I always have to fix such
	 * things... why can't the devs just make a game without arrgh factors so that I can simply play :(
	 * bad fate nastytrap makes it so you can get fates even if you turned the option off */
	if ((fate_option || p_ptr->nastytrap129) /*&& (p_ptr->lev > 10)*/)
	{
		/*
		 * WAS: == 666 against randint(50000).
		 * Since CPU's don't know Judeo-Christian / Cabalistic traditions,
		 * and since comparisons with zero is more efficient in many
		 * architectures...
		 */
		if (rand_int(50000) == 0) gain_fate(0);
	}

	/*** Is the wielded monsters still hypnotised ***/
	o_ptr = &p_ptr->inventory[INVEN_CARRY];

	if (o_ptr->k_idx)
	{
		monster_race *r_ptr = &r_info[o_ptr->pval];

		if ((randint(1000) < r_ptr->level - ((p_ptr->lev * 2) + get_skill(SKILL_SYMBIOTIC))))
		{
			msg_format("%s thinks you are not enough in symbiosis.",
			           symbiote_name(TRUE));
			carried_make_attack_normal(o_ptr->pval);
		}
	}

	/*** Check the Time and Load ***/

	/*
	 * Every 1000 game turns -- which means this section is invoked every
	 * 100 player turns under the normal speed, and slightly more than
	 * one per move in the reduced map.
	 */
	if ((turn % 1000) == 0)
	{
		/* Check time and load */
		if ((0 != check_time()) || (0 != check_load()))
		{
			/* Warning */
			if (closing_flag <= 2)
			{
				/* Disturb */
				disturb(0, 0);

				/* Count warnings */
				closing_flag++;

				/* Message */
				msg_print("The gates to Middle Earth are closing...");
				msg_print("Please finish up and/or save your game.");
			}

			/* Slam the gate */
			else
			{
				/* Message */
				msg_print("The gates to Middle Earth are now closed.");

				/* Stop playing */
				alive = FALSE;

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}
	}

	/*** Attempt timed autosave ***/
	if (autosave_t && autosave_freq)
	{
		if ((turn % ((s32b)autosave_freq * 10)) == 0)
		{
			is_autosave = TRUE;
			msg_print("Autosaving the game...");
			do_cmd_save_game();
			is_autosave = FALSE;
		}
	}


	/*** Handle the wilderness/town (sunshine) ***/

	/* While in town/wilderness and not in the overworld mode */
	if (!dun_level && !p_ptr->wild_mode)
	{
		/* Hack -- Daybreak/Nighfall in town */
		if ((turn % ((10L * DAY) / 2)) == 0)
		{
			bool dawn;

			/* Check for dawn */
			dawn = ((turn % (10L * DAY)) == 0);

			/* Day breaks */
			if (dawn)
			{
				/* Message */
				msg_print("The sun has risen.");

				/* Hack -- Scan the town */
				for (y = 0; y < cur_hgt; y++)
				{
					for (x = 0; x < cur_wid; x++)
					{
						/* Get the cave grid */
						c_ptr = &cave[y][x];

						/* Assume lit */
						c_ptr->info |= (CAVE_GLOW);

						/* Hack -- Memorize lit grids if allowed */
						if (view_perma_grids && !p_ptr->nastytrap159) c_ptr->info |= (CAVE_MARK);

						/* Hack -- Notice spot */
						note_spot(y, x);
					}
				}
			}

			/* Night falls */
			else
			{
				/* Message */
				msg_print("The sun has set.");

				/* Hack -- Scan the town */
				for (y = 0; y < cur_hgt; y++)
				{
					for (x = 0; x < cur_wid; x++)
					{
						/* Get the cave grid */
						c_ptr = &cave[y][x];

						/* Darken "boring" features */
						if (cave_plain_floor_grid(c_ptr))
						{
							/* Forget the grid */
							c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);

							/* Hack -- Notice spot */
							note_spot(y, x);
						}
					}
				}
			}

			/* Update the monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD);
		}
	}

	/* Tell a day passed */
	if (((turn + (DAY_START * 10L)) % (10L * DAY)) == 0)
	{
		char buf[20];

		sprintf(buf, get_day(bst(YEAR, turn) + START_YEAR));
		cmsg_format(TERM_L_GREEN,
		            "Today it is %s of the %s year of the third age.",
		            get_month_name(bst(DAY, turn), wizard, FALSE), buf);
	}

	/* Set back the rewards once a day */
	if ((turn % (10L * STORE_TURNS)) == 0)
	{
		/* Select new bounties. */
		if (magik(20)) select_bounties();
	}

	/* Modify loan */
	if (p_ptr->loan)
	{
		if (p_ptr->loan_time) p_ptr->loan_time--;

		if (((turn % 5000) == 0) && !p_ptr->loan_time)
		{
			cmsg_print(TERM_RED, "You should pay your loan...");

			p_ptr->loan += p_ptr->loan / 12;

			if (p_ptr->loan > PY_MAX_GOLD) p_ptr->loan = PY_MAX_GOLD;

			/* Do a nasty stuff */
			if (p_ptr->wild_mode && rand_int(2))
			{
				/* Discount player items */
				int z = 0, tries = 200;
				object_type *o_ptr = NULL;

				while (tries--)
				{
					z = rand_int(INVEN_TOTAL);
					o_ptr = &p_ptr->inventory[z];

					if (!o_ptr->k_idx) continue;

					if (o_ptr->discount >= 100) continue;

					break;
				}

				if (tries)
				{
					o_ptr->discount += 70;
					if (o_ptr->discount >= 100) o_ptr->discount = 100;

					inven_item_optimize(z);
					inven_item_describe(z);

					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}
			}

			else
			{
				int merc = test_monster_name("Mean-looking mercenary");
				int agent = test_monster_name("Agent of the black market");
				int num = 5 + (p_ptr->lev / 3), z;

				for (z = 0; z < num; z++)
				{
					int yy, xx, attempts = 200, m_idx;

					/* Summon */
					while (1)
					{
						scatter(&yy, &xx, p_ptr->py, p_ptr->px, 6, 0);

						/* Accept an empty grid within the boundary */
						if (in_bounds(yy, xx) && cave_floor_bold(yy, xx)) break;

						/* Max number of retries reached */
						if (--attempts == 0) break;
					}

					/* All the attempts failed */
					if (attempts == 0) continue;

					/* Summon a monster */
					m_idx = place_monster_one(yy, xx, magik(80) ? merc : agent,
					                          0, FALSE, MSTATUS_ENEMY);

					/* Level it */
					if (m_idx)
					{
						monster_type *m_ptr = &m_list[m_idx];

						m_ptr->exp = MONSTER_EXP(p_ptr->lev * 2);
						monster_check_experience(m_idx, TRUE);
					}
				}
			}
		}
	}

	/*** Process the monsters ***/

	monsterallocchance = d_info[(dun_level) ? dungeon_type : DUNGEON_WILDERNESS].max_m_alloc_chance;
	if (p_ptr->nastytrap81) {
		monsterallocchance /= 5;
		if (monsterallocchance < 1) monsterallocchance = 1;
	}

	/* Check for creature generation. */
	if (!p_ptr->wild_mode &&
	                !p_ptr->inside_arena &&
	                !p_ptr->inside_quest &&
	                (rand_int(monsterallocchance) == 0))
	{
		/* Make a new monster
		 * Amy edit: lame if special levels don't spawn anything over time. just make them spawn stuff slooooowly */
		if (!(dungeon_flags2 & DF2_NO_NEW_MONSTER) || (randint(20) == 1) )
		{
			(void)alloc_monster(MAX_SIGHT + 5, FALSE);
		}
	}

	/* Hack -- Check for creature regeneration */
	if (!p_ptr->wild_mode && ((turn % 100) == 0)) regen_monsters();


	/*** Damage over Time ***/

	/* Take damage from poison */
	if (p_ptr->poisoned && !p_ptr->invuln)
	{
		/* Take damage */
		take_hit(1, "poison");
	}


	/* Vampires take damage from sunlight */
	if (p_ptr->sensible_lite)
	{
		if ((!dun_level) && (((turn / ((10L * DAY) / 2)) % 2) == 0))
		{
			if (cave[p_ptr->py][p_ptr->px].info & (CAVE_GLOW))
			{
				/* Take damage */
				msg_print("The sun's rays scorch your undead flesh!");
				take_hit(1, "sunlight");
				cave_no_regen = TRUE;
				drop_from_wild();
			}
		}

		if ((p_ptr->inventory[INVEN_LITE].tval != 0) &&
		                (p_ptr->inventory[INVEN_LITE].sval >= SV_LITE_GALADRIEL) &&
		                (p_ptr->inventory[INVEN_LITE].sval <= SV_STONE_LORE) &&
		                (p_ptr->inventory[INVEN_LITE].sval != SV_LITE_UNDEATH))
		{
			object_type * o_ptr = &p_ptr->inventory[INVEN_LITE];
			char o_name [80];
			char ouch [80];

			/* Get an object description */
			object_desc(o_name, o_ptr, FALSE, 0);

			msg_format("The %s scorches your undead flesh!", o_name);

			cave_no_regen = TRUE;

			/* Get an object description */
			object_desc(o_name, o_ptr, TRUE, 0);

			sprintf(ouch, "wielding %s", o_name);
			take_hit(1, ouch);
		}
	}

	/* Drown in deep water unless the player have levitation, water walking
	   water breathing, or magic breathing.*/
	if (!p_ptr->ffall && !p_ptr->fly && !p_ptr->walk_water && !p_ptr->magical_breath &&
	                !p_ptr->water_breath &&
	                (cave[p_ptr->py][p_ptr->px].feat == FEAT_DEEP_WATER))
	{
		if (calc_total_weight() > ((weight_limit()) / 2))
		{
			/* Take damage */
			msg_print("You are drowning!");
			take_hit(randint(p_ptr->lev), "drowning");
			cave_no_regen = TRUE;
		}
	}


	/* Spectres -- take damage when moving through walls */

	/*
	 * Added: ANYBODY takes damage if inside through walls
	 * without wraith form -- NOTE: Spectres will never be
	 * reduced below 0 hp by being inside a stone wall; others
	 * WILL BE!
	 */
	if (!cave_floor_bold(p_ptr->py, p_ptr->px))
	{
		int feature = cave[p_ptr->py][p_ptr->px].feat;

		/* Player can walk through or fly over trees */
		if ((has_ability(AB_TREE_WALK) || p_ptr->fly) && (feature == FEAT_TREES))
		{
			/* Do nothing */
		}
		/* Player can climb over mountains */
		else if ((p_ptr->climb) && (f_info[feature].flags1 & FF1_CAN_CLIMB))
		{
			/* Do nothing */
		}
		else if (PRACE_FLAG(PR1_SEMI_WRAITH) && (!p_ptr->wraith_form) && (f_info[cave[p_ptr->py][p_ptr->px].feat].flags1 & FF1_CAN_PASS))
		{
			/* way too strong, gotta drain more --Amy */
			int amt = 1 + randint(p_ptr->lev);

			cave_no_regen = TRUE;
			if (amt > p_ptr->chp - 1) amt = p_ptr->chp - 1;
			take_hit(amt, " walls ...");
		}
	}


	/* Take damage from cuts */
	if ((p_ptr->cut) && !(p_ptr->invuln))
	{
		/* Mortal wound or Deep Gash */
		if (p_ptr->cut > 200)
		{
			i = 3;
		}

		/* Severe cut */
		else if (p_ptr->cut > 100)
		{
			i = 2;
		}

		/* Other cuts */
		else
		{
			i = 1;
		}

		/* Take damage */
		take_hit(i, "a fatal wound");
	}


	/*** Check the Food, and Regenerate ***/

	/* Digest normally */
	if (p_ptr->food < PY_FOOD_MAX)
	{
		/* Every 100 game turns */
		if ((turn % 100) == 0)
		{
			int speed_use = p_ptr->pspeed;

			/* Maximum */
			if (speed_use > 199)
			{
				speed_use = 199;
			}

			/* Minimum */
			else if (speed_use < 0)
			{
				speed_use = 0;
			}

			/* Basic digestion rate based on speed */
			i = extract_energy[speed_use] * 2;

			/* Regeneration takes more food */
			if (p_ptr->regenerate) i += 30;

			/* Rapid hunger does what it says on the tin --Amy */
			if (p_ptr->rapid_hunger) i += 100;

			/* fast metabolism trap means you need to eat *really* much */
			if (p_ptr->nastytrap83) i += 1000;

			/* Regeneration takes more food */
			if (p_ptr->tim_regen) i += p_ptr->tim_regen_pow / 10;

			/* Invisibility consume a lot of food */
			i += p_ptr->invis / 2;

			/* Invulnerability consume a lot of food */
			if (p_ptr->invuln) i += 40;

			/* Wraith Form consume a lot of food */
			if (p_ptr->wraith_form) i += 30;

			/* Get the weapon */
			o_ptr = &p_ptr->inventory[INVEN_WIELD];

			/* Examine the sword */
			object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

			/* Hitpoints multiplier consume a lot of food */
			if (o_ptr->k_idx && (f2 & (TR2_LIFE))) i += o_ptr->pval * 5;

			/* Slow digestion takes less food */
			if (p_ptr->slow_digest) i -= 10;

			/* Minimal digestion */
			if (i < 1) i = 1;

			/* Digest some food */
			(void)set_food(p_ptr->food - i);
		}
	}

	/* Digest quickly when gorged */
	else
	{
		/* Digest a lot of food */
		(void)set_food(p_ptr->food - 100);
	}

	/* Starve to death (slowly) */
	if (p_ptr->food < PY_FOOD_STARVE)
	{
		/* Calculate damage */
		i = (PY_FOOD_STARVE - p_ptr->food) / 10;

		/* Take damage */
		if (!(p_ptr->invuln)) take_hit(i, "starvation");
	}

	/* Default regeneration */
	regen_amount = PY_REGEN_NORMAL;

	/* Getting Weak */
	if (p_ptr->food < PY_FOOD_WEAK)
	{
		/* Lower regeneration */
		if (p_ptr->food < PY_FOOD_STARVE)
		{
			regen_amount = 0;
		}
		else if (p_ptr->food < PY_FOOD_FAINT)
		{
			regen_amount = PY_REGEN_FAINT;
		}
		else
		{
			regen_amount = PY_REGEN_WEAK;
		}

		/* Getting Faint */
		if (p_ptr->food < PY_FOOD_FAINT)
		{
			/* Faint occasionally */
			if (!p_ptr->paralyzed && (rand_int(100) < 10))
			{
				/* Message */
				msg_print("You faint from the lack of food.");
				disturb(1, 0);

				/* Hack -- faint (bypass free action) */
				(void)set_paralyzed(p_ptr->paralyzed + 1 + rand_int(5));
			}
		}
	}

	/* Are we walking the pattern? */
	if (!p_ptr->wild_mode && pattern_effect())
	{
		cave_no_regen = TRUE;
	}
	else
	{
		/* Regeneration ability */
		if (p_ptr->regenerate)
		{
			regen_amount = regen_amount * 2;
		}
	}


	/* Searching or Resting */
	if (p_ptr->searching || resting)
	{
		regen_amount = regen_amount * 2;
	}

	/* contamination slows down regeneration */
	if (regen_amount > 0) {
		if (p_ptr->contamination >= 10000) {
			regen_amount /= 10;
		}
		else if (p_ptr->contamination >= 2000) {
			regen_amount /= 3;
		}
		else if (p_ptr->contamination >= 1000) {
			regen_amount /= 2;
		}
		else if (p_ptr->contamination >= 500) {
			regen_amount *= 7;
			regen_amount /= 10;
		}
		else if (p_ptr->contamination >= 200) {
			regen_amount *= 4;
			regen_amount /= 5;
		}
		else if (p_ptr->contamination >= 100) {
			regen_amount *= 9;
			regen_amount /= 10;
		}

		if (regen_amount < 1) regen_amount = 1;
	}

	if (total_friends)
	{
		int upkeep_divider = 20;

		if (has_ability(AB_PERFECT_CASTING))
			upkeep_divider = 15;

#ifdef TRACK_FRIENDS

		if (wizard) msg_format("Total friends: %d.", total_friends);

#endif /* TRACK_FRIENDS */

		if (total_friends > 1 + (p_ptr->lev / (upkeep_divider)))
		{
			upkeep_factor = (total_friend_levels);

			if (upkeep_factor > 100) upkeep_factor = 100;
			else if (upkeep_factor < 10) upkeep_factor = 10;

#ifdef TRACK_FRIENDS

			if (wizard) msg_format("Levels %d, upkeep %d", total_friend_levels,
				                       upkeep_factor);

#endif /* TRACK_FRIENDS */
		}
	}

	/* Regenerate the mana */
	if (p_ptr->csp < p_ptr->msp)
	{
		if (upkeep_factor)
		{
			s16b upkeep_regen = (((100 - upkeep_factor) * regen_amount) / 100);
			regenmana(upkeep_regen);

#ifdef TRACK_FRIENDS

			if (wizard)
			{
				msg_format("Regen: %d/%d", upkeep_regen, regen_amount);
			}

#endif /* TRACK_FRIENDS */
		}
		else
		{
			regenmana(regen_amount);
		}
	}

	/* Eru piety incraese with time */
	/* Amy edit: removed due to potential of abuse (intentionally overburdening yourself and then walking around) */
	/*if (((turn % 100) == 0) && (!p_ptr->did_nothing) && (!p_ptr->wild_mode))
	{
		NOT_PRAY_GOD(GOD_ERU)
		{
			int inc = wisdom_scale(10);*/

			/* Increase by wisdom/4 */
			/*if (!inc) inc = 1;
			inc_piety(GOD_ERU, inc);
		}
	}*/
	/* Most gods piety decrease with time */
	/* Amy edit: replace "most" by "all", and do so regardless of whether you're in the dungeon */
	if (((turn % 300) == 0) && (!p_ptr->did_nothing) && (!p_ptr->wild_mode) /*&& (dun_level)*/)
	{
		/* if you pretend to be a god yourself, your god isn't all that pleased --Amy */
		if (p_ptr->mimic_form == resolve_mimic_name("Valar"))
		{
			inc_piety(GOD_ERU, -100);
			inc_piety(GOD_MANWE, -100);
			inc_piety(GOD_TULKAS, -100);
			inc_piety(GOD_MELKOR, -100);
			inc_piety(GOD_YAVANNA, -100);
			inc_piety(GOD_AMYBSOD, -100);
			inc_piety(GOD_AULE, -100);
			inc_piety(GOD_VARDA, -100);
			inc_piety(GOD_ULMO, -100);
			inc_piety(GOD_MANDOS, -100);
			inc_piety(GOD_OROME, -100);
			inc_piety(GOD_INGEBORG, -100);
			inc_piety(GOD_NIENNA, -100);
			inc_piety(GOD_ESTE, -100);
			inc_piety(GOD_VANA, -100);
			inc_piety(GOD_VAIRE, -100);
		}

		GOD(GOD_MANWE)
		{
			int dec = 4 - wisdom_scale(3);

			PRAY_GOD(GOD_MANWE)
			dec++;
			if (PRACE_FLAG(PR1_ELF))
				dec -= wisdom_scale(2);
			if (dec < 1) dec = 1;
			inc_piety(GOD_MANWE, -dec);
		}
		GOD(GOD_ESTE)
		{
			int dec = 4 - wisdom_scale(3);

			PRAY_GOD(GOD_ESTE)
			dec++;
			if (dec < 1) dec = 1;
			inc_piety(GOD_ESTE, -dec);
		}
		GOD(GOD_VANA)
		{
			int dec = 3 - wisdom_scale(1);

			PRAY_GOD(GOD_VANA)
			dec += 2;
			if (dec < 1) dec = 1;
			inc_piety(GOD_VANA, -dec);
		}
		GOD(GOD_ERU)
		{
			int dec = 2 - wisdom_scale(1);

			PRAY_GOD(GOD_ERU)
			dec++;
			if (dec < 1) dec = 1;
			inc_piety(GOD_ERU, -dec);
		}
		GOD(GOD_AULE)
		{
			int dec = 2 - wisdom_scale(1);

			PRAY_GOD(GOD_AULE)
			dec++;
			if (dec < 1) dec = 1;
			inc_piety(GOD_AULE, -dec);
		}
		GOD(GOD_VARDA)
		{
			int dec = 4 - wisdom_scale(1);

			PRAY_GOD(GOD_VARDA)
			dec++;
			if (dec < 1) dec = 1;
			inc_piety(GOD_VARDA, -dec);
		}
		GOD(GOD_MANDOS)
		{
			int dec = 4 - wisdom_scale(2);

			PRAY_GOD(GOD_MANDOS)
			dec += 15 - wisdom_scale(8);
			if (dec < 1) dec = 1;
			inc_piety(GOD_MANDOS, -dec);
		}
		GOD(GOD_MELKOR)
		{
			int dec = 8 - wisdom_scale(6);

			PRAY_GOD(GOD_MELKOR)
			dec++;
			if (PRACE_FLAG(PR1_ELF))
				dec += 5 - wisdom_scale(4);
			if (dec < 1) dec = 1;
			inc_piety(GOD_MELKOR, -dec);
		}
		GOD(GOD_INGEBORG)
		{
			int dec = 8 - wisdom_scale(6);

			PRAY_GOD(GOD_INGEBORG)
			dec += 4 - wisdom_scale(2);
			if (dec < 1) dec = 1;
			inc_piety(GOD_INGEBORG, -dec);
		}
		GOD(GOD_ULMO)
		{
			int dec = 8 - wisdom_scale(5);

			PRAY_GOD(GOD_ULMO)
			dec += 5;
			if (dec < 1) dec = 1;
			inc_piety(GOD_ULMO, -dec);
		}
		GOD(GOD_VAIRE)
		{
			int dec = 6 - wisdom_scale(3);

			PRAY_GOD(GOD_VAIRE)
			dec += 1;
			if (dec < 1) dec = 1;
			inc_piety(GOD_VAIRE, -dec);
		}
		GOD(GOD_AMYBSOD)
		{
			int dec = 20 - wisdom_scale(15);

			PRAY_GOD(GOD_AMYBSOD)
			dec += 10;
			if (dec < 1) dec = 1;
			inc_piety(GOD_AMYBSOD, -dec);
		}
		GOD(GOD_OROME)
		{
			int dec = 4 - wisdom_scale(2);

			PRAY_GOD(GOD_OROME)
			dec += 4;
			if (dec < 1) dec = 1;
			inc_piety(GOD_OROME, -dec);
		}
		GOD(GOD_NIENNA)
		{
			int dec = 4 - wisdom_scale(2);

			PRAY_GOD(GOD_NIENNA)
			dec += 3;
			if (dec < 1) dec = 1;
			inc_piety(GOD_NIENNA, -dec);
		}
		GOD(GOD_TULKAS)
		{
			int dec = 2 - wisdom_scale(1);
			PRAY_GOD(GOD_TULKAS)
			dec += (2 - wisdom_scale(1));

			if (dec < 1) dec = 1;
			inc_piety(GOD_TULKAS, -dec);
		}
	}
	/* Yavanna piety decrease with time */
	if (((turn % 400) == 0) && (!p_ptr->did_nothing) && (!p_ptr->wild_mode) /*&& (dun_level)*/)
	{
		GOD(GOD_YAVANNA)
		{
			int dec = 5 - wisdom_scale(3);

			/* Blech what an hideous hack */
			if (!strcmp(rp_ptr->title + rp_name, "Ent"))
				dec -= wisdom_scale(2);
			if (dec < 1) dec = 1;
			inc_piety(GOD_YAVANNA, -dec);
		}
	}
	p_ptr->did_nothing = FALSE;

	/* Increase regen by tim regen */
	if (p_ptr->tim_regen) regen_amount += p_ptr->tim_regen_pow;

	/* Poisoned or cut yields no healing */
	if (p_ptr->poisoned) regen_amount = 0;
	if (p_ptr->cut) regen_amount = 0;

	/* Special floor -- Pattern, in a wall -- yields no healing */
	if (cave_no_regen) regen_amount = 0;

	/* purified water by Amy, generated by quaffing water curing while standing on tainted water */
	if (cave[p_ptr->py][p_ptr->px].feat == FEAT_PURIFIED_WATER) {
		regen_amount += 400;
	}

	/* Being over grass allows Yavanna to regen you */
	PRAY_GOD(GOD_YAVANNA)
	{
		if (cave[p_ptr->py][p_ptr->px].feat == FEAT_GRASS)
		{
			regen_amount += 200 + wisdom_scale(800);
		}

		/* Amy edit: and if there's flowers as well, regenerate more, rather than not at all (like wtf did the original devs just forget this terrain exists?!) */
		if (cave[p_ptr->py][p_ptr->px].feat == FEAT_GRASS_ELANOR || cave[p_ptr->py][p_ptr->px].feat == FEAT_GRASS_FUMELLA || cave[p_ptr->py][p_ptr->px].feat == FEAT_GRASS_ANEMONE || cave[p_ptr->py][p_ptr->px].feat == FEAT_GRASS_NIPHREDIL || cave[p_ptr->py][p_ptr->px].feat == FEAT_GRASS_IRIS || cave[p_ptr->py][p_ptr->px].feat == FEAT_FLOWER)
		{
			regen_amount += 300 + wisdom_scale(1200);
		}
	}

	/* Regenerate Hit Points if needed */
	if ((p_ptr->chp < p_ptr->mhp) && !cave_no_regen)
	{
		if ((cave[p_ptr->py][p_ptr->px].feat < FEAT_PATTERN_END) &&
		                (cave[p_ptr->py][p_ptr->px].feat >= FEAT_PATTERN_START))
		{
			/* Hmmm. this should never happen? */
			regenhp(regen_amount / 5);
		}
		else
		{
			regenhp(regen_amount);
		}
	}


	/*** Timeout Various Things ***/

	/* Handle temporary stat drains */
	for (i = 0; i < 6; i++)
	{
		if (p_ptr->stat_cnt[i] > 0)
		{
			p_ptr->stat_cnt[i]--;
			if (p_ptr->stat_cnt[i] == 0)
			{
				do_res_stat(i, FALSE);
			}
		}
	}

	/* Hack -- Hallucinating */
	if (p_ptr->image)
	{
		(void)set_image(p_ptr->image - 1);
	}

	/* Holy Aura */
	if (p_ptr->holy)
	{
		(void)set_holy(p_ptr->holy - 1);
	}

	/* Soul absorbtion */
	if (p_ptr->absorb_soul)
	{
		(void)set_absorb_soul(p_ptr->absorb_soul - 1);
	}

	/* Undead loose Death Points */
	if (p_ptr->necro_extra & CLASS_UNDEAD)
	{
		int old_chp = p_ptr->chp;
		int warning = (p_ptr->mhp * hitpoint_warn / 10);

		/* Bypass invulnerability and wraithform */
		p_ptr->chp--;

		/* Display the hitpoints */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		/* Dead player */
		if (p_ptr->chp < 0)
		{
			bool old_quick = quick_messages;

			/* Sound */
			sound(SOUND_DEATH);

			/* Hack -- Note death */
			if (!last_words)
			{
				msg_print("You die.");
				msg_print(NULL);
			}
			else
			{
				char death_message[80];

				(void)get_rnd_line("death.txt", death_message);
				msg_print(death_message);
			}

			/* Note cause of death */
			(void)strcpy(died_from, "being undead too long");

			if (p_ptr->image) strcat(died_from, "(?)");

			/* No longer a winner */
			total_winner = FALSE;

			/* Leaving */
			p_ptr->leaving = TRUE;

			/* Note death */
			death = TRUE;

			quick_messages = FALSE;
			if (get_check("Make a last screenshot? "))
			{
				do_cmd_html_dump();
			}
			quick_messages = old_quick;

			/* Dead */
			return;
		}

		/* Hitpoint warning */
		if (p_ptr->chp < warning)
		{
			/* Hack -- bell on first notice */
			if (alert_hitpoint && (old_chp > warning)) bell();

			sound(SOUND_WARN);

			/* Message */
			msg_print("*** LOW DEATHPOINT WARNING! ***");
			msg_print(NULL);
		}
	}

	/* Walk water */
	if (p_ptr->walk_water)
	{
		(void)set_walk_water(p_ptr->walk_water - 1);
	}

	/* True Strike */
	if (p_ptr->strike)
	{
		(void)set_strike(p_ptr->strike - 1);
	}

	/* Meditation */
	if (p_ptr->meditation)
	{
		(void)set_meditation(p_ptr->meditation - 1);
	}

	/* Timed project */
	if (p_ptr->tim_project)
	{
		(void)set_project(p_ptr->tim_project - 1, p_ptr->tim_project_gf, p_ptr->tim_project_dam, p_ptr->tim_project_rad, p_ptr->tim_project_flag);
	}

	/* Timed roots */
	if (p_ptr->tim_roots)
	{
		(void)set_roots(p_ptr->tim_roots - 1, p_ptr->tim_roots_ac, p_ptr->tim_roots_dam);
	}

	/* Timed breath */
	if (p_ptr->tim_water_breath)
	{
		(void)set_tim_breath(p_ptr->tim_water_breath - 1, FALSE);
	}
	if (p_ptr->tim_magic_breath)
	{
		(void)set_tim_breath(p_ptr->tim_magic_breath - 1, TRUE);
	}

	/* Timed regen */
	if (p_ptr->tim_regen)
	{
		(void)set_tim_regen(p_ptr->tim_regen - 1, p_ptr->tim_regen_pow);
	}

	/* Timed Disrupt shield */
	if (p_ptr->disrupt_shield)
	{
		(void)set_disrupt_shield(p_ptr->disrupt_shield - 1);
	}

	/* Timed Parasite */
	if (p_ptr->parasite)
	{
		(void)set_parasite(p_ptr->parasite - 1, p_ptr->parasite_r_idx);
	}

	/* Timed Reflection */
	if (p_ptr->tim_reflect)
	{
		(void)set_tim_reflect(p_ptr->tim_reflect - 1);
	}

	/* Timed Prob Travel */
	if (p_ptr->prob_travel)
	{
		(void)set_prob_travel(p_ptr->prob_travel - 1);
	}

	/* Timed Time Resistance */
	if (p_ptr->tim_res_time)
	{
		(void)set_tim_res_time(p_ptr->tim_res_time - 1);
	}

	/* Timed Levitation */
	if (p_ptr->tim_ffall)
	{
		(void)set_tim_ffall(p_ptr->tim_ffall - 1);
	}
	if (p_ptr->tim_fly)
	{
		(void)set_tim_fly(p_ptr->tim_fly - 1);
	}

	/* Thunderstorm */
	if (p_ptr->tim_thunder)
	{
		int dam = damroll(p_ptr->tim_thunder_p1, p_ptr->tim_thunder_p2);
		int i, tries = 600;
		monster_type *m_ptr = NULL;

		while (tries)
		{
			/* Access the monster */
			m_ptr = &m_list[i = rand_range(1, m_max - 1)];

			tries--;

			/* Ignore "dead" monsters */
			if (!m_ptr->r_idx) continue;

			/* Cant see ? cant hit */
			if (!los(p_ptr->py, p_ptr->px, m_ptr->fy, m_ptr->fx)) continue;

			/* Do not hurt friends! */
			if (is_friend(m_ptr) >= 0) continue;
			break;
		}

		if (tries)
		{
			char m_name[80];

			monster_desc(m_name, m_ptr, 0);
			msg_format("Lightning strikes %s.", m_name);
			project(0, 0, m_ptr->fy, m_ptr->fx, dam / 3, GF_ELEC,
			        PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE);
			project(0, 0, m_ptr->fy, m_ptr->fx, dam / 3, GF_LITE,
			        PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE);
			project(0, 0, m_ptr->fy, m_ptr->fx, dam / 3, GF_SOUND,
			        PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE);
		}

		(void)set_tim_thunder(p_ptr->tim_thunder - 1, p_ptr->tim_thunder_p1, p_ptr->tim_thunder_p2);
	}

	if (p_ptr->tim_ttempest)
	{
		int dam = damroll(p_ptr->tim_ttempest_p1, p_ptr->tim_ttempest_p2);
		int i, tries = 600;
		monster_type *m_ptr = NULL;

		while (tries)
		{
			/* Access the monster */
			m_ptr = &m_list[i = rand_range(1, m_max - 1)];

			tries--;

			/* Ignore "dead" monsters */
			if (!m_ptr->r_idx) continue;

			/* Cant see ? cant hit */
			if (!los(p_ptr->py, p_ptr->px, m_ptr->fy, m_ptr->fx)) continue;

			/* Do not hurt friends! */
			if (is_friend(m_ptr) >= 0) continue;
			break;
		}

		if (tries)
		{
			char m_name[80];

			monster_desc(m_name, m_ptr, 0);
			msg_format("A vortex of time engulfs %s.", m_name);
			project(0, 0, m_ptr->fy, m_ptr->fx, dam, GF_TIME,
			        PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE);
		}

		(void)set_tim_ttempest(p_ptr->tim_ttempest - 1, p_ptr->tim_ttempest_p1, p_ptr->tim_ttempest_p2);
	}

	/* Poisonned hands */
	if (p_ptr->tim_poison)
	{
		(void)set_poison(p_ptr->tim_poison - 1);
	}

	/* Timed Fire Aura */
	if (p_ptr->tim_fire_aura)
	{
		(void)set_tim_fire_aura(p_ptr->tim_fire_aura - 1);
	}

	/* Brightness */
	if (p_ptr->tim_lite)
	{
		(void)set_lite(p_ptr->tim_lite - 1);
	}

	/* Blindness */
	if (p_ptr->blind)
	{
		(void)set_blind(p_ptr->blind - 1);
	}

	/* Timed no_breeds */
	if (no_breeds)
	{
		(void)set_no_breeders(no_breeds - 1);
	}

	/* Timed mimic */
	if (p_ptr->tim_mimic)
	{
		(void)set_mimic(p_ptr->tim_mimic - 1, p_ptr->mimic_form, p_ptr->mimic_level);
	}

	/* Timed special move commands */
	if (p_ptr->immov_cntr)
	{
		p_ptr->immov_cntr--;
	}

	/* Timed invisibility */
	if (p_ptr->tim_invisible)
	{
		(void)set_invis(p_ptr->tim_invisible - 1, p_ptr->tim_inv_pow);
	}

	/* Times see-invisible */
	if (p_ptr->tim_invis)
	{
		(void)set_tim_invis(p_ptr->tim_invis - 1);
	}

	if (multi_rew)
	{
		multi_rew = FALSE;
	}

	/* Timed esp */
	if (p_ptr->tim_esp)
	{
		(void)set_tim_esp(p_ptr->tim_esp - 1);
	}

	/* Timed animal esp */
	if (p_ptr->tim_esp_animal)
	{
		(void)set_tim_esp_animal(p_ptr->tim_esp_animal - 1);
	}

	if (p_ptr->tim_peace)
	{
		(void)set_tim_peace(p_ptr->tim_peace - 1);
	}

	if (p_ptr->tim_device)
	{
		(void)set_tim_device(p_ptr->tim_device - 1);
	}

	/* Timed bullseye */
	if (p_ptr->tim_bullseye)
	{
		(void)set_tim_bullseye(p_ptr->tim_bullseye - 1);
	}

	/* Timed sniper */
	if (p_ptr->tim_sniper)
	{
		(void)set_tim_sniper(p_ptr->tim_sniper - 1);
	}

	/* Timed rapidfire */
	if (p_ptr->tim_rapidfire)
	{
		(void)set_tim_rapidfire(p_ptr->tim_rapidfire - 1);
	}

	/* Timed bombsquad */
	if (p_ptr->tim_bombsquad)
	{
		(void)set_tim_bombsquad(p_ptr->tim_bombsquad - 1);
	}

	/* Timed dancing */
	if (p_ptr->tim_dancing)
	{
		(void)set_tim_dancing(p_ptr->tim_dancing - 1, p_ptr->am_dancing);
		if (p_ptr->tim_dancing < 1) p_ptr->am_dancing = 0;
	}

	/* Timed elemlull */
	if (p_ptr->tim_elemlull)
	{
		(void)set_tim_elemlull(p_ptr->tim_elemlull - 1, p_ptr->am_elemlull);
		if (p_ptr->tim_elemlull < 1) p_ptr->am_elemlull = 0;
	}

	/* Timed magbreath */
	if (p_ptr->tim_magbreath)
	{
		(void)set_tim_magbreath(p_ptr->tim_magbreath - 1, p_ptr->am_magbreath);
		if (p_ptr->tim_magbreath < 1) p_ptr->am_magbreath = 0;
	}

	if (p_ptr->tim_macarena)
	{
		(void)set_tim_macarena(p_ptr->tim_macarena - 1, p_ptr->am_macarena);
		if (p_ptr->tim_macarena < 1) p_ptr->am_macarena = 0;
	}

	if (p_ptr->tim_savmus)
	{
		(void)set_tim_savmus(p_ptr->tim_savmus - 1, p_ptr->am_savmus);
		if (p_ptr->tim_savmus < 1) p_ptr->am_savmus = 0;
	}

	if (p_ptr->tim_minersong)
	{
		(void)set_tim_minersong(p_ptr->tim_minersong - 1, p_ptr->am_minersong);
		if (p_ptr->tim_minersong < 1) p_ptr->am_minersong = 0;
	}

	if (p_ptr->tim_stealthmus)
	{
		(void)set_tim_stealthmus(p_ptr->tim_stealthmus - 1, p_ptr->am_stealthmus);
		if (p_ptr->tim_stealthmus < 1) p_ptr->am_stealthmus = 0;
	}

	if (p_ptr->tim_warboogie)
	{
		(void)set_tim_warboogie(p_ptr->tim_warboogie - 1, p_ptr->am_warboogie);
		if (p_ptr->tim_warboogie < 1) p_ptr->am_warboogie = 0;
	}

	if (p_ptr->tim_tankmus)
	{
		(void)set_tim_tankmus(p_ptr->tim_tankmus - 1, p_ptr->am_tankmus);
		if (p_ptr->tim_tankmus < 1) p_ptr->am_tankmus = 0;
	}

	if (p_ptr->tim_slaughtersong)
	{
		(void)set_tim_slaughtersong(p_ptr->tim_slaughtersong - 1, p_ptr->am_slaughtersong);
		if (p_ptr->tim_slaughtersong < 1) p_ptr->am_slaughtersong = 0;
	}

	if (p_ptr->tim_mightymus)
	{
		(void)set_tim_mightymus(p_ptr->tim_mightymus - 1, p_ptr->am_mightymus);
		if (p_ptr->tim_mightymus < 1) p_ptr->am_mightymus = 0;
	}

	/* Mana void */
	if (p_ptr->tim_manavoid)
	{
		(void)set_tim_manavoid(p_ptr->tim_manavoid - 1);
	}

	/* Dragon blood potions */
	if (p_ptr->tim_dragonblood_fire)
	{
		(void)set_tim_dragonblood_fire(p_ptr->tim_dragonblood_fire - 1);
	}

	if (p_ptr->tim_dragonblood_cold)
	{
		(void)set_tim_dragonblood_cold(p_ptr->tim_dragonblood_cold - 1);
	}

	if (p_ptr->tim_dragonblood_elec)
	{
		(void)set_tim_dragonblood_elec(p_ptr->tim_dragonblood_elec - 1);
	}

	if (p_ptr->tim_dragonblood_acid)
	{
		(void)set_tim_dragonblood_acid(p_ptr->tim_dragonblood_acid - 1);
	}

	/* temporary ESP */
	if (p_ptr->tim_esp_pot)
	{
		(void)set_tim_esp_pot(p_ptr->tim_esp_pot - 1);
	}

	/* temporary levitation */
	if (p_ptr->tim_levi_pot)
	{
		(void)set_tim_levi_pot(p_ptr->tim_levi_pot - 1);
	}

	/* temporary free action */
	if (p_ptr->tim_jolt_fa)
	{
		(void)set_tim_jolt_fa(p_ptr->tim_jolt_fa - 1);
	}

	/* temporary boost */
	if (p_ptr->tim_boost_pot)
	{
		(void)set_tim_boost_pot(p_ptr->tim_boost_pot - 1);
	}

	/* temporary luck */
	if (p_ptr->tim_luck_pot)
	{
		(void)set_tim_luck_pot(p_ptr->tim_luck_pot - 1);
	}

	/* temporary high resist */
	if (p_ptr->tim_reshi_pot)
	{
		(void)set_tim_reshi_pot(p_ptr->tim_reshi_pot - 1);
	}

	/* temporary rare resist */
	if (p_ptr->tim_resrare_pot)
	{
		(void)set_tim_resrare_pot(p_ptr->tim_resrare_pot - 1);
	}

	/* temporary magical breathing */
	if (p_ptr->tim_air_pot)
	{
		(void)set_tim_air_pot(p_ptr->tim_air_pot - 1);
	}

	/* temporary antimagic */
	if (p_ptr->tim_antimagic_pot)
	{
		(void)set_tim_antimagic_pot(p_ptr->tim_antimagic_pot - 1);
	}

	/* temporary time flow */
	if (p_ptr->tim_timeflow_pot)
	{
		(void)set_tim_timeflow_pot(p_ptr->tim_timeflow_pot - 1);
	}

	/* temporary regeneration */
	if (p_ptr->tim_regen_pot)
	{
		(void)set_tim_regen_pot(p_ptr->tim_regen_pot - 1);
	}

	/* temporary power */
	if (p_ptr->tim_power_pot)
	{
		(void)set_tim_power_pot(p_ptr->tim_power_pot - 1);
	}

	/* Timed infra-vision +3 */
	if (p_ptr->tim_infra_x)
	{
		(void)set_tim_infra_x(p_ptr->tim_infra_x - 1);
	}

	/* Timed infra-vision +5 */
	if (p_ptr->tim_infra_xx)
	{
		(void)set_tim_infra_xx(p_ptr->tim_infra_xx - 1);
	}

	/* Silenced */
	if (p_ptr->tim_manasilence)
	{
		(void)set_tim_manasilence(p_ptr->tim_manasilence - 1);
	}

	/* Between sickness */
	if (p_ptr->betweensickness)
	{
		(void)set_betweensickness(p_ptr->betweensickness - 1);
	}

	/* Stair sickness */
	if (p_ptr->stairsickness)
	{
		(void)set_stairsickness(p_ptr->stairsickness - 1);
	}

	/* Repelling */
	if (p_ptr->tim_repelling)
	{
		(void)set_tim_repelling(p_ptr->tim_repelling - 1);
	}

	/* Timed infra-vision */
	if (p_ptr->tim_infra)
	{
		(void)set_tim_infra(p_ptr->tim_infra - 1);
	}

	/* Paralysis */
	if (p_ptr->paralyzed)
	{
		(void)set_paralyzed(p_ptr->paralyzed - 1);
	}

	/* Confusion */
	if (p_ptr->confused)
	{
		(void)set_confused(p_ptr->confused - 1);
	}

	/* Afraid */
	if (p_ptr->afraid)
	{
		(void)set_afraid(p_ptr->afraid - 1);
	}

	/* Fast */
	if (p_ptr->fast)
	{
		(void)set_fast(p_ptr->fast - 1, p_ptr->speed_factor);
	}

	/* Light speed */
	if (p_ptr->lightspeed)
	{
		(void)set_light_speed(p_ptr->lightspeed - 1);
	}

	/* Slow */
	if (p_ptr->slow)
	{
		(void)set_slow(p_ptr->slow - 1);
	}

	/* Protection from evil */
	if (p_ptr->protevil)
	{
		(void)set_protevil(p_ptr->protevil - 1);
	}

	/* Protection from good */
	if (p_ptr->protgood)
	{
		(void)set_protgood(p_ptr->protgood - 1);
	}

	/* Protection from undead */
	if (p_ptr->protundead)
	{
		(void)set_protundead(p_ptr->protundead - 1);
	}

	/* Invulnerability */
	if (p_ptr->invuln)
	{
		(void)set_invuln(p_ptr->invuln - 1);
	}

	/* Wraith form */
	if (p_ptr->tim_wraith)
	{
		(void)set_shadow(p_ptr->tim_wraith - 1);
	}

	/* Heroism */
	if (p_ptr->hero)
	{
		(void)set_hero(p_ptr->hero - 1);
	}

	/* Super Heroism */
	if (p_ptr->shero)
	{
		(void)set_shero(p_ptr->shero - 1);
	}

	/* Blessed */
	if (p_ptr->blessed)
	{
		(void)set_blessed(p_ptr->blessed - 1);
	}

	/* Shield */
	if (p_ptr->shield)
	{
		(void)set_shield(p_ptr->shield - 1, p_ptr->shield_power, p_ptr->shield_opt, p_ptr->shield_power_opt, p_ptr->shield_power_opt2);
	}

	/* Oppose Acid */
	if (p_ptr->oppose_acid)
	{
		(void)set_oppose_acid(p_ptr->oppose_acid - 1);
	}

	/* Oppose Lightning */
	if (p_ptr->oppose_elec)
	{
		(void)set_oppose_elec(p_ptr->oppose_elec - 1);
	}

	/* Oppose Fire */
	if (p_ptr->oppose_fire)
	{
		(void)set_oppose_fire(p_ptr->oppose_fire - 1);
	}

	/* Oppose Cold */
	if (p_ptr->oppose_cold)
	{
		(void)set_oppose_cold(p_ptr->oppose_cold - 1);
	}

	/* Oppose Poison */
	if (p_ptr->oppose_pois)
	{
		(void)set_oppose_pois(p_ptr->oppose_pois - 1);
	}

	/* Oppose Light & Dark */
	if (p_ptr->oppose_ld)
	{
		(void)set_oppose_ld(p_ptr->oppose_ld - 1);
	}

	/* Oppose Chaos & Confusion */
	if (p_ptr->oppose_cc)
	{
		(void)set_oppose_cc(p_ptr->oppose_cc - 1);
	}

	/* Oppose Sound & Shards */
	if (p_ptr->oppose_ss)
	{
		(void)set_oppose_ss(p_ptr->oppose_ss - 1);
	}

	/* Oppose Nexus */
	if (p_ptr->oppose_nex)
	{
		(void)set_oppose_nex(p_ptr->oppose_nex - 1);
	}

	/* Mental Barrier */
	if (p_ptr->tim_mental_barrier)
	{
		(void)set_mental_barrier(p_ptr->tim_mental_barrier - 1);
	}

	/* The rush */
	if (p_ptr->rush)
	{
		(void)set_rush(p_ptr->rush - 1);
	}


	/* Timed mimicry */
	if (get_skill(SKILL_MIMICRY))
	{
		/* Extract the value and the flags */
		u32b value = p_ptr->mimic_extra >> 16;

		u32b att = p_ptr->mimic_extra & 0xFFFF;

		if ((att & CLASS_LEGS) || (att & CLASS_WALL) || (att & CLASS_ARMS))
		{
			value--;

			if (!value)
			{
				if (att & CLASS_LEGS) msg_print("You lose your extra pair of legs.");
				if (att & CLASS_ARMS) msg_print("You lose your extra pair of arms.");
				if (att & CLASS_WALL) msg_print("You lose your affinity for walls.");

				att &= ~(CLASS_ARMS);
				att &= ~(CLASS_LEGS);
				att &= ~(CLASS_WALL);

				if (disturb_state && !p_ptr->nastytrap160) disturb(0, 0);
			}

			p_ptr->update |= (PU_BODY);
			p_ptr->mimic_extra = att + (value << 16);
		}
	}


	/*** Poison and Stun and Cut ***/

	/* Poison */
	if (p_ptr->poisoned)
	{
		int adjust = (adj_con_fix[p_ptr->stat_ind[A_CON]] + 1);

		/* Apply some healing */
		(void)set_poisoned(p_ptr->poisoned - adjust);
	}

	/* Stun */
	if (p_ptr->stun)
	{
		int adjust = (adj_con_fix[p_ptr->stat_ind[A_CON]] + 1);

		/* Apply some healing */
		(void)set_stun(p_ptr->stun - adjust);
	}

	/* Cut */
	if (p_ptr->cut)
	{
		int adjust = (adj_con_fix[p_ptr->stat_ind[A_CON]] + 1);

		/* Hack -- Truly "mortal" wound */
		if (p_ptr->cut > 1000) adjust = 0;

		/* Apply some healing */
		(void)set_cut(p_ptr->cut - adjust);
	}

	/* Hack - damage done by the dungeon -SC- */
	if ((dun_level != 0) && (d_ptr->d_frequency[0] != 0))
	{
		int i, j, k;

		/* Apply damage to every grid in the dungeon */
		for (i = 0; i < 4; i++)
		{
			/* Check the frequency */
			if (d_ptr->d_frequency[i] == 0) continue;

			if (((turn % d_ptr->d_frequency[i]) == 0) &&
			                ((d_ptr->d_side[i] != 0) || (d_ptr->d_dice[i] != 0)))
			{
				for (j = 0; j < cur_hgt - 1; j++)
				{
					for (k = 0; k < cur_wid - 1; k++)
					{
						int l, dam = 0;

						if (!(dungeon_flags1 & DF1_DAMAGE_FEAT))
						{
							/* If the grid is empty, skip it */
							if ((cave[j][k].o_idx == 0) &&
							                ((j != p_ptr->py) && (i != p_ptr->px))) continue;
						}

						/* Let's not hurt poor monsters */
						if (cave[j][k].m_idx) continue;

						/* Roll damage */
						for (l = 0; l < d_ptr->d_dice[i]; l++)
						{
							dam += randint(d_ptr->d_side[i]);
						}

						/* Apply damage */
						project( -100, 0, j, k, dam, d_ptr->d_type[i],
						         PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE);
					}
				}
			}

			if (p_ptr->nastytrap58 && (( (turn + 910) % d_ptr->d_frequency[i]) == 0) &&
			                ((d_ptr->d_side[i] != 0) || (d_ptr->d_dice[i] != 0)))
			{
				for (j = 0; j < cur_hgt - 1; j++)
				{
					for (k = 0; k < cur_wid - 1; k++)
					{
						int l, dam = 0;

						if (!(dungeon_flags1 & DF1_DAMAGE_FEAT))
						{
							/* If the grid is empty, skip it */
							if ((cave[j][k].o_idx == 0) &&
							                ((j != p_ptr->py) && (i != p_ptr->px))) continue;
						}

						/* Let's not hurt poor monsters */
						if (cave[j][k].m_idx) continue;

						/* Roll damage */
						for (l = 0; l < d_ptr->d_dice[i]; l++)
						{
							dam += randint(d_ptr->d_side[i]);
						}

						/* Apply damage */
						project( -100, 0, j, k, dam, d_ptr->d_type[i],
						         PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE);
					}
				}
			}

		}
	}

#ifndef pelpel

	/* handle spell effects */
	if (!p_ptr->wild_mode)
	{
		/*
		 * I noticed significant performance degrade after the introduction
		 * of staying spell effects. I believe serious optimisation effort
		 * is required before another release.
		 *
		 * More important is to fix that display weirdness...
		 *
		 * It seems that the game never expects that monster deaths and
		 * terrain feature changes should happen here... Moving these
		 * to process_player() [before resting code, with "every 10 game turn"
		 * 'if'] may or may not fix the problem... -- pelpel to DG
		 */
		for (j = 0; j < cur_hgt - 1; j++)
		{
			for (i = 0; i < cur_wid - 1; i++)
			{
				int e = cave[j][i].effect;

				if (e)
				{
					effect_type *e_ptr = &effects[e];

					if (e_ptr->time)
					{
						/* Apply damage */
						project(0, 0, j, i, e_ptr->dam, e_ptr->type,
						        PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE);
					}
					else
					{
						cave[j][i].effect = 0;
					}

					if ((e_ptr->flags & EFF_WAVE) && !(e_ptr->flags & EFF_LAST))
					{
						if (distance(e_ptr->cy, e_ptr->cx, j, i) < e_ptr->rad - 1)
							cave[j][i].effect = 0;
					}
					else if ((e_ptr->flags & EFF_STORM) && !(e_ptr->flags & EFF_LAST))
					{
						cave[j][i].effect = 0;
					}

					lite_spot(j, i);
				}
			}
		}

		/* Reduce & handle effects */
		for (i = 0; i < MAX_EFFECTS; i++)
		{
			/* Skip empty slots */
			if (effects[i].time == 0) continue;

			/* Reduce duration */
			effects[i].time--;

			/* Creates a "wave" effect*/
			if (effects[i].flags & EFF_WAVE)
			{
				effect_type *e_ptr = &effects[i];
				int x, y, z;

				e_ptr->rad++;

				/* What a frelling ugly line of ifs ... */
				if (effects[i].flags & EFF_DIR8)
					for (y = e_ptr->cy - e_ptr->rad, z = 0; y <= e_ptr->cy; y++, z++)
					{
						for (x = e_ptr->cx - (e_ptr->rad - z); x <= e_ptr->cx + (e_ptr->rad - z); x++)
						{
							if (!in_bounds(y, x)) continue;

							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
				else if (effects[i].flags & EFF_DIR2)
					for (y = e_ptr->cy, z = e_ptr->rad; y <= e_ptr->cy + e_ptr->rad; y++, z--)
					{
						for (x = e_ptr->cx - (e_ptr->rad - z); x <= e_ptr->cx + (e_ptr->rad - z); x++)
						{
							if (!in_bounds(y, x)) continue;

							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
				else if (effects[i].flags & EFF_DIR6)
					for (x = e_ptr->cx, z = e_ptr->rad; x <= e_ptr->cx + e_ptr->rad; x++, z--)
					{
						for (y = e_ptr->cy - (e_ptr->rad - z); y <= e_ptr->cy + (e_ptr->rad - z); y++)
						{
							if (!in_bounds(y, x)) continue;

							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
				else if (effects[i].flags & EFF_DIR4)
					for (x = e_ptr->cx - e_ptr->rad, z = 0; x <= e_ptr->cx; x++, z++)
					{
						for (y = e_ptr->cy - (e_ptr->rad - z); y <= e_ptr->cy + (e_ptr->rad - z); y++)
						{
							if (!in_bounds(y, x)) continue;

							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
				else if (effects[i].flags & EFF_DIR9)
					for (y = e_ptr->cy - e_ptr->rad; y <= e_ptr->cy; y++)
					{
						for (x = e_ptr->cx; x <= e_ptr->cx + e_ptr->rad; x++)
						{
							if (!in_bounds(y, x)) continue;

							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
				else if (effects[i].flags & EFF_DIR1)
					for (y = e_ptr->cy; y <= e_ptr->cy + e_ptr->rad; y++)
					{
						for (x = e_ptr->cx - e_ptr->rad; x <= e_ptr->cx; x++)
						{
							if (!in_bounds(y, x)) continue;

							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
				else if (effects[i].flags & EFF_DIR7)
					for (y = e_ptr->cy - e_ptr->rad; y <= e_ptr->cy; y++)
					{
						for (x = e_ptr->cx - e_ptr->rad; x <= e_ptr->cx; x++)
						{
							if (!in_bounds(y, x)) continue;

							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
				else if (effects[i].flags & EFF_DIR3)
					for (y = e_ptr->cy; y <= e_ptr->cy + e_ptr->rad; y++)
					{
						for (x = e_ptr->cx; x <= e_ptr->cx + e_ptr->rad; x++)
						{
							if (!in_bounds(y, x)) continue;

							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
				else
					for (y = e_ptr->cy - e_ptr->rad; y <= e_ptr->cy + e_ptr->rad; y++)
					{
						for (x = e_ptr->cx - e_ptr->rad; x <= e_ptr->cx + e_ptr->rad; x++)
						{
							if (!in_bounds(y, x)) continue;

							/* This is *slow* -- pelpel */
							if (los(e_ptr->cy, e_ptr->cx, y, x) &&
							                (distance(e_ptr->cy, e_ptr->cx, y, x) == e_ptr->rad))
								cave[y][x].effect = i;
						}
					}
			}
			/* Creates a "storm" effect*/
			else if (effects[i].flags & EFF_STORM)
			{
				effect_type *e_ptr = &effects[i];
				int x, y;

				e_ptr->cy = p_ptr->py;
				e_ptr->cx = p_ptr->px;
				for (y = e_ptr->cy - e_ptr->rad; y <= e_ptr->cy + e_ptr->rad; y++)
				{
					for (x = e_ptr->cx - e_ptr->rad; x <= e_ptr->cx + e_ptr->rad; x++)
					{
						if (!in_bounds(y, x)) continue;

						if (los(e_ptr->cy, e_ptr->cx, y, x) &&
						                (distance(e_ptr->cy, e_ptr->cx, y, x) <= e_ptr->rad))
						{
							cave[y][x].effect = i;
							lite_spot(y, x);
						}
					}
				}
			}
		}

		apply_effect(p_ptr->py, p_ptr->px);
	}

#endif /* !pelpel */

	/* Arg cannot breath? */
	if ((dungeon_flags2 & DF2_WATER_BREATH) && (!p_ptr->water_breath))
	{
		cmsg_print(TERM_L_RED, "You cannot breathe water!  You suffocate!");
		take_hit(damroll(3, p_ptr->lev), "suffocating");
	}
	if ((dungeon_flags2 & DF2_NO_BREATH) && (!p_ptr->magical_breath))
	{
		cmsg_print(TERM_L_RED, "There is no air there!  You suffocate!");
		take_hit(damroll(3, p_ptr->lev), "suffocating");
	}

	/*
	 * Every 1500 turns, warn about any Black Breath not gotten from
	 * an equipped object, and stop any resting. -LM-
	 *
	 * It's apparent that someone has halved the frequency... -- pelpel
	 */
	if (((turn % 3000) == 0) && p_ptr->black_breath)
	{
		u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10;

		bool be_silent = FALSE;

		/* check all equipment for the Black Breath flag. */
		for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
		{
			o_ptr = &p_ptr->inventory[i];

			/* Skip non-objects */
			if (!o_ptr->k_idx) continue;

			/* Extract the item flags */
			object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

			/* No messages if object has the flag, to avoid annoyance. */
			if (f4 & (TR4_BLACK_BREATH)) be_silent = TRUE;

		}
		/* If we are allowed to speak, warn and disturb. */

		if (!be_silent)
		{
			cmsg_print(TERM_L_DARK, "The Black Breath saps your soul!");
			disturb(0, 0);
		}
	}


	/*** Process Light ***/

	/* Check for light being wielded */
	o_ptr = &p_ptr->inventory[INVEN_LITE];

	/* Burn some fuel in the current lite */
	if (o_ptr->tval == TV_LITE)
	{
		/* Extract the item flags */
		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

		/* Hack -- Use some fuel */
		if ((f4 & TR4_FUEL_LITE) && (o_ptr->timeout > 0))
		{
			/* Decrease life-span */
			o_ptr->timeout--;

			/* Hack -- notice interesting fuel steps */
			if ((o_ptr->timeout < 100) || ((o_ptr->timeout % 100) == 0))
			{
				/* Window stuff */
				p_ptr->window |= (PW_EQUIP);
			}

			/* Hack -- Special treatment when blind */
			if (p_ptr->blind)
			{
				/* Hack -- save some light for later */
				if (o_ptr->timeout == 0) o_ptr->timeout++;
			}

			/* The light is now out */
			else if (o_ptr->timeout < 1)
			{
				disturb(0, 0);
				cmsg_print(TERM_YELLOW, "Your light has gone out!");
			}

			/* The light is getting dim */
			else if (o_ptr->timeout < 100)
			{
				if (disturb_minor && !p_ptr->nastytrap160) disturb(0, 0);
				cmsg_print(TERM_YELLOW, "Your light is growing faint.");
				drop_from_wild();
			}
		}
	}

	/* Calculate torch radius */
	p_ptr->update |= (PU_TORCH);


	/*** Process Inventory ***/

	/*
	 * Handle experience draining.  In Oangband, the effect is worse,
	 * especially for high-level characters.  As per Tolkien, hobbits
	 * are resistant.
	 */
	if (p_ptr->black_breath)
	{
		byte chance = 0;
		int plev = p_ptr->lev;

		if (PRACE_FLAG(PR1_RESIST_BLACK_BREATH)) chance = 2;
		else chance = 5;

		if ((rand_int(100) < chance) && (p_ptr->exp > 0))
		{
			p_ptr->exp -= 1 + plev / 5;
			p_ptr->max_exp -= 1 + plev / 5;
			(void)do_dec_stat(rand_int(6), STAT_DEC_NORMAL);
			check_experience();
		}
	}

	/* Drain Mana */
	if (p_ptr->drain_mana && p_ptr->csp)
	{
		p_ptr->csp -= p_ptr->drain_mana;
		if (magik(75)) p_ptr->csp -= p_ptr->drain_mana;

		if (p_ptr->csp < 0)
		{
			p_ptr->csp = 0;
			disturb(0, 0);
		}

		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}

	/* Partial summons drain mana */
	if (p_ptr->maintain_sum)
	{
		u32b oldcsp = p_ptr->csp;
		p_ptr->csp -= p_ptr->maintain_sum / 10000;

		if (p_ptr->csp < 0)
		{
			p_ptr->csp = 0;
			disturb(0, 0);

			p_ptr->maintain_sum = 0;
		}
		else
		{
			/* Leave behind any fractional sp */
			p_ptr->maintain_sum -= (oldcsp - p_ptr->csp) * 10000;
		}

		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

	}

	/* Drain Hitpoints */
	if (p_ptr->drain_life)
	{
		int drain = p_ptr->drain_life + rand_int(p_ptr->mhp * p_ptr->drain_life / 40);

		p_ptr->chp -= (drain < p_ptr->chp ? drain : p_ptr->chp);

		if (p_ptr->chp == 0)
		{
			disturb(0, 0);
		}

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

	}

	/* Handle experience draining */
	if (p_ptr->exp_drain)
	{
		if ((rand_int(100) < 50) && (p_ptr->exp > 0))
		{
			p_ptr->exp--;
			p_ptr->max_exp--;
			check_experience();
		}
	}

	if (p_ptr->nastytrap121)
	{
		p_ptr->praying = FALSE;
	}

	/* Ingeborg causes random stuff, and if your piety is negative, also random bad stuff */
	GOD(GOD_INGEBORG) {
		if (!(p_ptr->wild_mode)) {
			if (p_ptr->grace < 0 && rand_int(1000) == 0) {
				msg_format("Ingeborg produces %s farting noises with her sexy butt.", magik(50) ? "loud" : "disgusting");
				do_fart_effect();
				disturb(0, 0);
			}
			if (rand_int(10000) == 0) {
				msg_print("Ingeborg calls upon the chaos patron...");
				gain_level_reward(0);
				disturb(0, 0);
			}
		}
	}

	if ((p_ptr->contamination >= 100) && rand_int(10000) == 0) {
		p_ptr->contamination -= 100;
		msg_print("You are consumed by your contamination!");
		do_dec_stat(A_WIS, STAT_DEC_PERMANENT_NORESIST);
		/* Display the hitpoints */
		p_ptr->redraw |= (PR_SANITY);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}

	if (p_ptr->nastytrap152 && !dun_level && rand_int(100) == 0) {
		msg_print("Etherwind is blowing!");
		contaminate(1);
	}

	if (p_ptr->nastytrap153 && rand_int(100) == 0) {
		contaminate(1);
	}

	if (p_ptr->nastytrap154 && rand_int(10000) == 0) {
		int ktt;
		trap_type *ft_ptr;

		for (ktt = 0; ktt < max_t_idx; ktt++)
		{
			ft_ptr = &t_info[ktt];
			if (magik(2)) ft_ptr->ident = FALSE;
		}
	}

	if (p_ptr->nastytrap155 && rand_int(10000) == 0) {
		int ktt;
		object_kind *fk_ptr;

		for (ktt = 0; ktt < max_k_idx; ktt++)
		{
			fk_ptr = &k_info[ktt];
			if (fk_ptr->flavor && magik(2)) {
				fk_ptr->aware = FALSE;
				fk_ptr->tried = FALSE;
			}
		}

	}

	if (p_ptr->nastytrap12 && (rand_int(TY_CURSE_CHANCE) == 0) ) {
		activate_ty_curse();
	}
	if (p_ptr->nastytrap13 && (rand_int(DG_CURSE_CHANCE) == 0) ) {
		activate_dg_curse();
	}

	if (p_ptr->nastytrap151 && (rand_int(10000) == 0) ) {
		seed_flavor = rand_int(0x10000000);
		flavor_init();
	}

	if (p_ptr->nastytrap16 && !(p_ptr->wild_mode) && (rand_int(1000) == 0) ) {
		trap_creation();
	}

	if (p_ptr->nastytrap126 && (rand_int(100) == 0) ) {
		set_tim_manavoid(p_ptr->tim_manavoid + 20);
	}

	if (p_ptr->nastytrap26 && (rand_int(100) == 0) ) {
		curse_equipment(50, 10);
	}

	if (p_ptr->nastytrap49 && !(p_ptr->wild_mode) && (rand_int(100) == 0) ) {
		do_fart_effect();
		disturb(0, 0);
	}

	if (p_ptr->nastytrap51 && !(p_ptr->wild_mode) && (rand_int(100) == 0) ) {
		alloc_trap();
	}

	if (p_ptr->nastytrap64 && (rand_int(100) == 0) ) {
		if (randint(100) == 0) (void)do_dec_stat(rand_int(6), STAT_DEC_PERMANENT);
		else if (randint(10) == 0) (void)do_dec_stat(rand_int(6), STAT_DEC_NORMAL);
		else (void)do_dec_stat(rand_int(6), STAT_DEC_TEMPORARY);
	}

	if (p_ptr->nastytrap68 && (rand_int(100) == 0) ) {
		switch (randint(5)) {
			case 1:
				(void)set_confused(p_ptr->confused + rand_int(25) + 4);
				break;
			case 2:
				set_afraid(p_ptr->afraid + 3 + randint(25));
				break;
			case 3:
				(void)set_stun(p_ptr->stun + randint(40));
				break;
			case 4:
				set_image(p_ptr->image + randint(40) + 10);
				break;
			case 5:
				(void)set_poisoned(p_ptr->poisoned + rand_int(15) + 10);
				break;
		}
	}

	if (p_ptr->nastytrap80 && (rand_int(100) == 0) ) {
		s32b gold = (p_ptr->au / 10) + randint(25);

		if (gold < 2) gold = 2;
		if (gold > 5000) gold = (p_ptr->au / 20) + randint(3000);
		if (gold > p_ptr->au) gold = p_ptr->au;

		p_ptr->au -= gold;
		if (gold <= 0)
		{
			msg_print("A malicious hand tries to filch some gold from your wallet, but doesn't find anything.");
		}
		else if (p_ptr->au)
		{
			msg_print("A malicious hand filches some gold from your wallet.");
			msg_format("%ld coins were stolen!", (long)gold);
		}
		else
		{
			msg_print("A malicious hand filches some gold from your wallet.");
			msg_print("All of your coins were stolen!");
		}
		p_ptr->redraw |= (PR_GOLD);

	}

	if (p_ptr->nastytrap85 && (rand_int(1000) == 0) ) {
		if (lose_all_info())
		{
			msg_print("Your memories fade away.");
		}
	}

	/* exp drain nastytrap: like black breath, i.e. have to get the XP back the hard way --Amy */
	if (p_ptr->nastytrap99 && (rand_int(100) == 0) ) {
		if (p_ptr->exp > 0)
		{
			int plev = p_ptr->lev;
			p_ptr->exp -= 1 + plev / 5;
			p_ptr->max_exp -= 1 + plev / 5;
			check_experience();
		}

	}

	/* teleportitis nastytrap: had to be moved here, rather than the equipment below --Amy */
	if (p_ptr->nastytrap100 && (rand_int(100) < 1))
	{
		disturb(0, 0);

		/* Teleport player */
		teleport_player(20 + randint(80));
		/* FUCKing interrupt so that you don't bump into an AMHD with the next overshot keystroke --Amy */
		msg_print("You suddenly get teleported!");
		msg_print(NULL);
	}

	/* auto destruct trap: constantly interrupt player even if nothing dangerous is nearby --Amy */
	if (p_ptr->nastytrap21) {
		disturb(0, 0);
	}

	/* silenced? then your mana is always set to zero --Amy */
	if (p_ptr->tim_manasilence) {
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;
		p_ptr->redraw |= (PR_MANA);
		return;
	}


	/* Process equipment */
	for (j = 0, i = INVEN_WIELD; i < INVEN_TOTAL; i++)
	{
		/* Get the object */
		o_ptr = &p_ptr->inventory[i];

		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);


		/* TY Curse */
		if ((f3 & TR3_TY_CURSE) && (rand_int(TY_CURSE_CHANCE) == 0))
		{
			activate_ty_curse();
		}

		/* DG Curse */
		if ((f4 & TR4_DG_CURSE) && (rand_int(DG_CURSE_CHANCE) == 0))
		{
			activate_dg_curse();

			/* The object recurse itself ! */
			o_ptr->ident |= IDENT_CURSED;
		}

		/* Auto Curse */
		if ((f3 & TR3_AUTO_CURSE) && (rand_int(AUTO_CURSE_CHANCE) == 0))
		{
			/* The object recurse itself ! */
			o_ptr->ident |= IDENT_CURSED;
		}

		/*
		 * Hack: Uncursed teleporting items (e.g. Dragon Weapons)
		 * can actually be useful!
		 */
		if ((f3 & TR3_TELEPORT) && (rand_int(100) < 1))
		{
			if ((o_ptr->ident & IDENT_CURSED) && !p_ptr->anti_tele && !p_ptr->nastytrap15)
			{
				disturb(0, 0);

				/* Teleport player */
				teleport_player(40);
				/* FUCKing interrupt so that you don't bump into an AMHD with the next overshot keystroke --Amy */
				msg_print("You suddenly get teleported!");
				msg_print(NULL);
			}
			else
			{
				if (p_ptr->wild_mode ||
				                (o_ptr->note && strchr(quark_str(o_ptr->note), '.')))
				{
					/* Do nothing */
					/* msg_print("Teleport aborted.") */;
				}
				else if (get_check("Teleport? "))
				{
					disturb(0, 0);
					teleport_player(50);
				}
			}
		}


		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Hack: Skip wielded lights that need fuel (already handled above) */
		if ((i == INVEN_LITE) && (o_ptr->tval == TV_LITE) && (f4 & TR4_FUEL_LITE)) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			/* Notice changes */
			if (o_ptr->timeout == 0)
			{
				recharged_notice(o_ptr);
				j++;
			}
		}

		/* Recharge second spell in Mage Staffs of Spells - taken out by Amy because of eternal bugs */
		if (/*is_ego_p(o_ptr, EGO_MSTAFF_SPELL) && (o_ptr->xtra2 > 0)*/FALSE)
		{
			/* Recharge */
			o_ptr->xtra2--;

			/* Notice changes */
			if (o_ptr->xtra2 == 0) j++;
		}
	}

	/* Notice changes */
	if (j)
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Recharge rods */
	for (j = 0, i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &p_ptr->inventory[i];
		k_ptr = &k_info[o_ptr->k_idx];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Examine the rod */
		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

		/* Temporary items are destroyed */
		if (f5 & TR5_TEMPORARY)
		{
			o_ptr->timeout--;

			if (o_ptr->timeout <= 0)
			{
				inven_item_increase(i, -99);
				inven_item_describe(i);
				inven_item_optimize(i);

				/* Combine and Reorder pack */
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);
			}
		}

		/* Examine all charging rods or stacks of charging rods. */
		if ((o_ptr->tval == TV_ROD_MAIN) && !p_ptr->nastytrap82 && (o_ptr->timeout < o_ptr->pval2))
		{
			/* Increase the rod's mana. */
			o_ptr->timeout += (f4 & TR4_CHARGING) ? 2 : 1;

			/* Always notice */
			j++;

			/* Notice changes, provide message if object is inscribed. */
			if (o_ptr->timeout >= o_ptr->pval2)
			{
				o_ptr->timeout = o_ptr->pval2;
				recharged_notice(o_ptr);
			}
		}

		/* Examine all charging random artifacts */
		if ((f5 & TR5_ACTIVATE_NO_WIELD) && (o_ptr->timeout > 0))
		{
			/* Charge it */
			o_ptr->timeout--;

			/* Notice changes */
			if (o_ptr->timeout == 0)
			{
				j++;
				recharged_notice(o_ptr);
			}
		}

		/* Decay objects in pack */
		if (decays(o_ptr))
		{
			/* Decay it */
			if (o_ptr->pval != 0)
			{
				if (o_ptr->timeout > 0)
				{
					if (dungeon_flags1 & DF1_HOT)
					{
						o_ptr->pval -= 2;
					}
					else if ((dungeon_flags1 & DF1_COLD) && rand_int(2))
					{
						if (magik(50)) o_ptr->pval--;
					}
					else
					{
						o_ptr->pval--;
					}
				}

				if ((o_ptr->timeout > 0) && o_ptr->timeout < o_ptr->weight) o_ptr->timeout--;

				/* Notice changes */
				if (o_ptr->pval <= 0)
				{
					pack_decay(i);
					j++;
				}
			}
		}

		/* Hatch eggs */
		if (o_ptr->tval == TV_EGG)
		{
			int mx, my;

			if (o_ptr->timeout == 0)
			{
				o_ptr->pval--;

				/* Notice changes */
				if (o_ptr->pval <= 0)
				{
					monster_type *m_ptr;
					monster_race *r_ptr;

					mx = p_ptr->px;
					my = p_ptr->py + 1;
					get_pos_player(5, &my, &mx);
					msg_print("Your egg hatches!");
					place_monster_aux(my, mx, o_ptr->pval2, FALSE, FALSE, MSTATUS_PET);

					m_ptr = &m_list[cave[my][mx].m_idx];
					r_ptr = race_inf(m_ptr);

					if ((r_ptr->flags9 & RF9_IMPRESED) && can_create_companion())
					{
						msg_format("And you have given the imprint to your %s!",
						           r_name + r_ptr->name);
						m_ptr->status = MSTATUS_COMPANION;
					}

					inven_item_increase(i, -1);
					inven_item_describe(i);
					inven_item_optimize(i);
					j++;
				}
			}
		}
	}

	/* Notice changes */
	if (j)
	{
		/* Combine pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
	}

	/* Feel the p_ptr->inventory */
	if (dun_level || (!p_ptr->wild_mode))
	{
		sense_inventory();
	}

	/*** Process Objects ***/

	/* Process objects */
	for (i = 1; i < o_max; i++)
	{
		/* Access object */
		o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Examine the rod */
		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

		/* Temporary items are destroyed */
		if (f5 & TR5_TEMPORARY)
		{
			o_ptr->timeout--;

			if (o_ptr->timeout <= 0)
			{
				floor_item_increase(i, -99);
				floor_item_optimize(i);

				/* Combine and Reorder pack */
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);
			}
		}

		/* Recharge rods on the ground.  No messages. */
		if ((o_ptr->tval == TV_ROD_MAIN) && !p_ptr->nastytrap82 && (o_ptr->timeout < o_ptr->pval2))
		{
			/* Increase the rod's mana. */
			o_ptr->timeout += (f4 & TR4_CHARGING) ? 2 : 1;

			/* Do not overflow */
			if (o_ptr->timeout >= o_ptr->pval2)
			{
				o_ptr->timeout = o_ptr->pval2;
			}
		}

		/* Decay objects on the ground*/
		if (decays(o_ptr))
		{
			/* Decay it */
			if (o_ptr->pval != 0)
			{
				if (o_ptr->timeout > 0)
				{
					if (dungeon_flags1 & DF1_HOT)
					{
						o_ptr->pval -= 2;
					}
					else if ((dungeon_flags1 & DF1_COLD) && rand_int(2))
					{
						if (magik(50)) o_ptr->pval--;
					}
					else
					{
						o_ptr->pval--;
					}
				}

				if ((o_ptr->timeout > 0) && o_ptr->timeout < o_ptr->weight) o_ptr->timeout--;

				/* Turn it into a skeleton */
				if (o_ptr->pval <= 0)
				{
					floor_decay(i);
				}
			}
		}

		/* Hatch eggs */
		if (o_ptr->tval == TV_EGG)
		{
			int mx, my;
			if (o_ptr->timeout > 0) o_ptr->pval--;

			/* Notice changes */
			if (o_ptr->pval <= 0)
			{
				mx = o_ptr->ix;
				my = o_ptr->iy;
				get_pos_player(5, &my, &mx);
				msg_print("An egg hatches!");
				place_monster_one(my, mx, o_ptr->pval2, 0, FALSE, MSTATUS_ENEMY);
				floor_item_increase(i, -1);
				floor_item_describe(i);
				floor_item_optimize(i);
			}
		}
	}


	/*** Involuntary Movement ***/

	/* Delayed Word-of-Recall */
	if (p_ptr->word_recall)
	{
		/* Can we ? */
		if (process_hooks(HOOK_RECALL, "()", ""))
		{
			p_ptr->word_recall = 0;
		}

		/* No recall. sorry */
		else if (dungeon_flags2 & DF2_NO_RECALL_OUT)
		{
			cmsg_print(TERM_L_DARK, "You cannot recall from here.");
			p_ptr->word_recall = 0;
		}

		/* Cannot WoR out of death fate levels */
		else if (dungeon_type == DUNGEON_DEATH)
		{
			cmsg_print(TERM_L_DARK, "You are fated to die here.  FIGHT for your life!");
			p_ptr->word_recall = 0;
		}

		/* I think the 'inside_quest' code belongs here -- pelpel */

		/* They cannot use word of recall until reaching surface */
		else if (p_ptr->astral)
		{
			msg_print("As an astral being you can't recall.");
			p_ptr->word_recall = 0;
		}

		/* Normal WoR */
		else
		{
			/*
			 * HACK: Autosave BEFORE resetting the recall counter (rr9)
			 * The player is yanked up/down as soon as
			 * he loads the autosaved game.
			 */
			if (autosave_l && (p_ptr->word_recall == 1))
			{
				is_autosave = TRUE;
				msg_print("Autosaving the game...");
				do_cmd_save_game();
				is_autosave = FALSE;
			}

			/* Make SURE that persistent levels are saved
			 * I don't know if this is needed, but I'm getting reports,
			 * so I'm adding this extra save -- Neil
			 */
			save_dungeon();

			/* Count down towards recall */
			p_ptr->word_recall--;

			/* Activate the recall */
			if (p_ptr->word_recall == 0)
			{
				/* Disturbing! */
				disturb(0, 0);

				/* Determine the level */
				if (p_ptr->inside_quest)
				{
					msg_print("The recall is cancelled by a powerful magic force!");
				}
				else if (dun_level)
				{
					msg_print("You feel yourself yanked upwards!");

					if (old_turn > 0) old_turn = 0;

					p_ptr->recall_dungeon = dungeon_type;
					dungeon_type = DUNGEON_WILDERNESS;
					dun_level = 0;

					is_recall = TRUE;

					p_ptr->inside_quest = 0;
					p_ptr->leaving = TRUE;
				}
				else
				{
					msg_print("You feel yourself yanked downwards!");

					if (old_turn > 0) old_turn = 0;

					/* New depth */
					dungeon_type = p_ptr->recall_dungeon;
					dun_level = max_dlv[dungeon_type];
					if (dun_level < 1) dun_level = 1;

					/* Reset player position */
					p_ptr->oldpx = p_ptr->px;
					p_ptr->oldpy = p_ptr->py;

					/* Leaving */
					is_recall = TRUE;

					p_ptr->leaving = TRUE;
					p_ptr->wild_mode = FALSE;
				}

				/* Sound */
				sound(SOUND_TPLEVEL);
			}
		}
	}
}


/*
 * Verify use of "wizard" mode
 */
static bool enter_wizard_mode(void)
{
#if 0
	/* Ask first time */
	if (!(noscore & 0x0002))
#else
	/* Ask first time, but not while loading a dead char with the -w option
	 * Amy edit: too annoying, I want a confirmation anyway! */
	if (/*!noscore && !(p_ptr->chp < 0)*/TRUE)
#endif
	{
		/* Mention effects */
		msg_print("Wizard mode is for debugging and experimenting.");
		msg_print("The game will not be scored if you enter wizard mode.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to enter wizard mode? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		noscore |= 0x0002;
	}

	/* Success */
	return (TRUE);
}


#ifdef ALLOW_WIZARD

/*
 * Verify use of "debug" commands
 */
static bool enter_debug_mode(void)
{
	/* Ask first time */
#if 0
	if (!(noscore & 0x0008))
#else
	/* Amy note: I want a confirmation as long as I'm not in wizard mode! */
if (/*!noscore &&*/ !wizard)
#endif
	{
		/* Mention effects */
		msg_print("The debug commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use debug commands.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to use debug commands? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		noscore |= 0x0008;
	}

	/* Success */
	return (TRUE);
}


/*
 * Hack -- Declare the Debug Routines
 */
extern void do_cmd_debug(void);

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

/*
 * Verify use of "borg" commands
 */
static bool enter_borg_mode(void)
{
	/* Ask first time */
	if (!(noscore & 0x0010))
	{
		/* Mention effects */
		msg_print("The borg commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use borg commands.");
		msg_print(NULL);

		/* Verify request */
		if (!get_check("Are you sure you want to use borg commands? "))
		{
			return (FALSE);
		}

		/* Mark savefile */
		noscore |= 0x0010;
	}

	/* Success */
	return (TRUE);
}


/*
 * Hack -- Declare the Ben Borg
 */
extern void do_cmd_borg(void);

#endif /* ALLOW_BORG */



/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 *
 * XXX XXX XXX Make some "blocks"
 */
static void process_command(void)
{
	char error_m[80];

#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	repeat_check();

	four_dim_mode = FALSE;
	lifesave_no_mortal = FALSE;

#endif /* ALLOW_REPEAT -- TNB */

	/* Process the appropriate hooks */
	if (process_hooks(HOOK_KEYPRESS, "(d)", command_cmd)) return;

	/* Parse the command */
	switch (command_cmd)
	{
		/* Ignore */
	case ESCAPE:
	case ' ':
	case 0:
		{
			break;
		}

		/* Ignore return */
	case '\r':
		{
			break;
		}

#ifdef ALLOW_QUITTING

	case KTRL('L'):
		{
			quit("CHEATER");
			break;
		}

#endif

#ifdef ALLOW_WIZARD

		/*** Wizard Commands ***/

		/* Toggle Wizard Mode */
	case KTRL('W'):
		{
			if (wizard)
			{
				wizard = FALSE;
				msg_print("Wizard mode off.");
			}
			else if (enter_wizard_mode())
			{
				wizard = TRUE;
				msg_print("Wizard mode on.");
			}

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw "title" */
			p_ptr->redraw |= (PR_TITLE);

			break;
		}

		/* Special "debug" commands */
	case KTRL('A'):
		{
			/* Enter debug mode */
			if (enter_debug_mode())
			{
				do_cmd_debug();
			}
			break;
		}

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

		/* Special "borg" commands */
	case KTRL('Z'):
		{
			/* Enter borg mode */
			if (enter_borg_mode())
			{
				if (!p_ptr->wild_mode) do_cmd_borg();
			}

			break;
		}

#endif /* ALLOW_BORG */


		/*** Inventory Commands ***/

		/* Wear/wield equipment */
	case 'w':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_wield();
			break;
		}

		/* Take off equipment */
	case 't':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_takeoff();
			p_ptr->redraw |= (PR_MH);
			break;
		}

		/* Drop an item */
	case 'd':
		{
			if (do_control_drop()) break;
			if (!p_ptr->wild_mode) do_cmd_drop();
			break;
		}

		/* Destroy an item */
	case 'k':
		{
			if (p_ptr->control) break;
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
	case 'e':
		{
			if (p_ptr->control) break;
			do_cmd_equip();
			break;
		}

		/* Inventory list */
	case 'i':
		{
			if (do_control_inven()) break;
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
	case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
	case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}


		/*** Standard "Movement" Commands ***/

		/* Alter a grid */
	case '+':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_alter();
			break;
		}

		/* Dig a tunnel */
	case 'T':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_tunnel();
			break;
		}

		/* Move (usually pick up things) */
	case ';':
		{
			if (do_control_walk()) break;

			do_cmd_walk((always_pickup || p_ptr->nastytrap161), TRUE);

			break;
		}

		/* Move (usually do not pick up) */
	case '-':
		{
			if (do_control_walk()) break;

			do_cmd_walk(!always_pickup, TRUE);

			break;
		}


		/*** Running, Resting, Searching, Staying */

		/* Begin Running -- Arg is Max Distance */
	case '.':
		{
			if (p_ptr->control || p_ptr->wild_mode) break;
			do_cmd_run();
			break;
		}

		/* Stay still (usually pick things up) */
	case ',':
		{
			if (do_control_pickup()) break;
			do_cmd_stay(always_pickup || p_ptr->nastytrap161);
			break;
		}

		/* Stay still (usually do not pick up) */
	case 'g':
		{
			if (p_ptr->control) break;
			do_cmd_stay(!always_pickup);
			break;
		}

		/* Rest -- Arg is time */
	case 'R':
		{
			if (p_ptr->control) break;
			do_cmd_rest();
			break;
		}

		/* Search for traps/doors */
	case 's':
		{
			if (p_ptr->control) break;
			do_cmd_search();
			break;
		}

		/* Toggle search mode */
	case 'S':
		{
			if (p_ptr->control) break;
			do_cmd_toggle_search();
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Enter store */
	case '_':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_store();
			break;
		}

#if 0 /* Merged with the '>' command -- pelpel */

		/* Enter quest level -KMW- */
	case '[':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_quest();
			break;
		}

#endif /* 0 */

		/* Go up staircase */
	case '<':
		{
			object_type *o_ptr;
			u32b f1 = 0 , f2 = 0 , f3 = 0, f4 = 0, f5 = 0, f6 = 0, f7 = 0, f8 = 0, f9 = 0, f10 = 0, esp = 0;


			/* Check for light being wielded */
			o_ptr = &p_ptr->inventory[INVEN_LITE];
			/* Burn some fuel in the current lite */
			if (o_ptr->tval == TV_LITE)
				/* Extract the item flags */
				object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

			/* Cannot move if rooted in place */
			if (p_ptr->tim_roots) break;

			if (p_ptr->control) break;
			/* Normal cases */
			if (p_ptr->wild_mode || dun_level || is_quest(dun_level))
			{
				do_cmd_go_up(FALSE);
			}
			else if (vanilla_town)
			{
				/* Do nothing */
			}
			/* Don't let the player < when he'd just drop right back down */
			else if (p_ptr->food < PY_FOOD_ALERT)
			{
				msg_print("You are too hungry to travel.");
			}
			else if (p_ptr->sensible_lite &&
			                (((turn / ((10L * DAY) / 2)) % 2) == 0))
			{
				/* Burn vampires! burn! */
				msg_print("You can't travel during the day!");
			}
			else if (p_ptr->sensible_lite &&
			                (o_ptr->tval != 0) &&
			                (o_ptr->sval >= SV_LITE_GALADRIEL) &&
			                (o_ptr->sval <= SV_STONE_LORE) &&
			                (o_ptr->sval != SV_LITE_UNDEATH))
			{
				msg_print("Travel with your present light would be unsafe.");
			}
			else if (p_ptr->cut || p_ptr->poisoned)
			{
				/* I actually died this way once -- neil */
				msg_print("You are too injured to travel.");
			}
			else if (ambush_flag)
			{
				msg_print("To flee the ambush you have to reach the edge of the map.");
			}
			else if (o_ptr->tval && (f4 & TR4_FUEL_LITE) && (o_ptr->timeout < 100))
			{
				msg_print("Your light is too low on fuel for you to travel with it.");
			}
			/* TODO: make the above stuff use this hook */
			else if (!process_hooks(HOOK_FORBID_TRAVEL, "()"))
			{
				p_ptr->oldpx = p_ptr->px;
				p_ptr->oldpy = p_ptr->py;
				change_wild_mode();

				/* Update the known wilderness */
				reveal_wilderness_around_player(p_ptr->wilderness_y,
				                                p_ptr->wilderness_x,
				                                0, WILDERNESS_SEE_RADIUS);

				/*msg_format("encounter level %d", wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].level);*/

				if ((p_ptr->wild_mode &&
	                magik(wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].level - (p_ptr->lev * 2))) || (p_ptr->wild_mode && (rand_int(p_ptr->nastytrap75 ? 4 : 20) < 1) ) ) {

					if (ambush_ok()) {
						change_wild_mode();

						/* HACk -- set the encouter flag for the wilderness generation */
						generate_encounter = TRUE;
						p_ptr->oldpx = MAX_WID / 2;
						p_ptr->oldpy = MAX_HGT / 2;

						/* Inform the player of his horrible fate :=) */
						msg_print("The monsters have been waiting for you, and now you're ambushed. Bwarharharharhar!");
					}

				}


			}

			break;
		}

		/* Go down staircase */
	case '>':
		{
			/* Cannot move if rooted in place */
			if (p_ptr->tim_roots) break;

			if (p_ptr->control) break;
			/* Normal cases */
			if (!p_ptr->wild_mode)
			{
				do_cmd_go_down();
			}

			/* Special cases */
			else
			{
				if ((wf_info[wild_map[p_ptr->py][p_ptr->px].feat].entrance >= 1000) ||
				                (wild_map[p_ptr->py][p_ptr->px].entrance > 1000))
				{
					p_ptr->wilderness_x = p_ptr->px;
					p_ptr->wilderness_y = p_ptr->py;
					p_ptr->wild_mode = !p_ptr->wild_mode;
					do_cmd_go_down();

					if (dun_level == 0)
					{
						p_ptr->wild_mode = !p_ptr->wild_mode;
					}
					else
					{
						p_ptr->wilderness_x = p_ptr->px;
						p_ptr->wilderness_y = p_ptr->py;
						change_wild_mode();
					}
				}
				else
				{
					p_ptr->wilderness_x = p_ptr->px;
					p_ptr->wilderness_y = p_ptr->py;
					change_wild_mode();
				}
			}

			break;
		}

		/* Open a door or chest */
	case 'o':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_open();
			break;
		}

		/* Close a door */
	case 'c':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_close();
			break;
		}

		/* Give an item */
	case 'y':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_give();
			break;
		}

		/* Chat */
	case 'Y':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_chat();
			break;
		}

		/* Jam a door with spikes */
	case 'j':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_spike();
			break;
		}

		/* Bash a door */
	case 'B':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_bash();
			break;
		}

		/* Disarm a trap or chest */
	case 'D':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_disarm();
			break;
		}


		/*** Magic and Prayers ***/

		/* Interact with skills */
	case 'G':
		{
			if (p_ptr->control) break;
			do_cmd_skill();
			break;
		}

		/* Interact with abilities */
	case 'N':
		{
			if (p_ptr->control) break;
			do_cmd_ability();
			break;
		}

		/* Browse a book */
	case 'b':
		{
			if (p_ptr->control) break;
			do_cmd_browse();
			break;
		}

		/* Cast a spell */
	case 'm':
		{
			if (do_control_magic()) break;

			/* No magic in the overworld map */
			if (p_ptr->wild_mode) break;

			/* Neither in the Arena */
			if (p_ptr->inside_arena)
			{
				msg_print("The arena absorbs all attempted magic!");

				break;
			}
			do_cmd_activate_skill();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Pray a prayer */
	case 'p':
		{
			if (p_ptr->control || p_ptr->wild_mode) break;
			do_cmd_pray();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Issue commands to pets */
	case 'P':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_pet();
			break;
		}

		/* Cut up a corpse */
	case 'h':
		{
			if (p_ptr->control || p_ptr->wild_mode) break;
			do_cmd_cut_corpse();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Cure some meat */
	case 'K':
		{
			if (p_ptr->control) break;
			do_cmd_cure_meat();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Steal an item form a monster */
	case 'Z':
		{
			if (p_ptr->control || p_ptr->wild_mode) break;
			do_cmd_steal();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/*** Use various objects ***/

		/* Inscribe an object */
	case '{':
		{
			if (p_ptr->control) break;
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
	case '}':
		{
			if (p_ptr->control) break;
			do_cmd_uninscribe();
			break;
		}

		/* Activate an artifact */
	case 'A':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("The arena absorbs all attempted magic!");
				msg_print(NULL);
				break;
			}

			do_cmd_activate();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Eat some food */
	case 'E':
		{
			if (p_ptr->control) break;
			do_cmd_eat_food();
			break;
		}

		/* Fuel your lantern/torch */
	case 'F':
		{
			if (p_ptr->control) break;
			do_cmd_refill();
			break;
		}

		/* Fire an item */
	case 'f':
		{
			object_type *j_ptr;

			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("You're in the arena now. This is hand-to-hand!");
				msg_print(NULL);
				break;
			}

			j_ptr = &p_ptr->inventory[INVEN_BOW];

			if (process_hooks(HOOK_FIRE, "(O)", j_ptr))
				break;

			if (j_ptr->tval == TV_BOOMERANG)
			{
				do_cmd_boomerang();
			}
			else
			{
				do_cmd_fire();
			}

			break;
		}

		/* Throw an item */
	case 'v':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("You're in the arena now. This is hand-to-hand!");
				msg_print(NULL);
				break;
			}

			do_cmd_throw();
			break;
		}

		/* Aim a wand */
	case 'a':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("The arena absorbs all attempted magic!");
				msg_print(NULL);
				break;
			}

			do_cmd_aim_wand();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Zap a rod */
	case 'z':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("The arena absorbs all attempted magic!");
				msg_print(NULL);
				break;
			}

			do_cmd_zap_rod();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Quaff a potion */
	case 'q':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("The arena absorbs all attempted magic!");
				msg_print(NULL);
				break;
			}

			do_cmd_quaff_potion();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Drink from a fountain -SC- */
	case 'H':
		{
			cave_type *c_ptr = &cave[p_ptr->py][p_ptr->px];

			if (p_ptr->control) break;
			if ((c_ptr->feat == FEAT_FOUNTAIN) ||
			                (c_ptr->feat == FEAT_EMPTY_FOUNTAIN))
			{
				do_cmd_drink_fountain();
				squeltch_inventory();
				squeltch_grid();
			}
			else
			{
				msg_print("You see no fountain here.");
			}

			break;
		}

		/* Read a scroll */
	case 'r':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("The arena absorbs all attempted magic!");
				msg_print(NULL);
				break;
			}

			do_cmd_read_scroll();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Use a staff */
	case 'u':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("The arena absorbs all attempted magic!");
				msg_print(NULL);
				break;
			}

			do_cmd_use_staff();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Use racial power */
	case 'U':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (p_ptr->inside_arena)
			{
				msg_print("The arena absorbs all attempted magic!");
				msg_print(NULL);
				break;
			}

			do_cmd_power();
			squeltch_inventory();
			squeltch_grid();
			break;
		}

		/* Sacrifice at an altar */
	case 'O':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			if (PRACE_FLAG(PR1_NO_GOD))
			{
				msg_print("You cannot worship gods.");
			}
			else
			{
				do_cmd_sacrifice();
			}

			break;
		}

		/*** Looking at Things (nearby or on map) ***/

		/* Full dungeon map */
	case 'M':
		{
			if (!p_ptr->wild_mode) do_cmd_view_map();
			break;
		}

		/* Locate player on map */
	case 'L':
		{
			do_cmd_locate();
			break;
		}

		/* Look around */
	case 'l':
		{
			do_cmd_look();
			break;
		}

		/* Target monster or location */
	case '*':
		{
			if (p_ptr->control) break;
			if (!p_ptr->wild_mode) do_cmd_target();
			break;
		}

		/* Engrave the floor */
	case 'x':
		{
			if (p_ptr->control) break;
			if (p_ptr->wild_mode) break;

			/* No point in engraving if there isn't any mana on this grid. */
			/* DG - actualy there is, it doesnt break macros */
			do_cmd_sense_grid_mana();
			do_cmd_engrave();

			break;
		}

		/*** Help and Such ***/

		/* Help */
	case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
	case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
	case 'C':
		{
			do_cmd_change_name();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
	case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
	case '"':
		{
			do_cmd_pref();
			break;
		}

		/* Interact with macros */
	case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
	case '%':
		{
			do_cmd_visuals();
			break;
		}

		/* Interact with colors */
	case '&':
		{
			do_cmd_colors();
			break;
		}

		/* Interact with options */
	case '=':
		{
			do_cmd_options();
			break;
		}


		/*** Misc Commands ***/

		/* Take notes */
	case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
	case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
	case KTRL('F'):
		{
			if (!p_ptr->wild_mode)
				do_cmd_feeling();
			break;
		}

		/* Show previous message */
	case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
	case KTRL('P'):
		{
			do_cmd_messages();
			break;
		}

		/* Show quest status -KMW- */
	case KTRL('Q'):
				case CMD_QUEST:
{
			do_cmd_checkquest();
			break;
		}

		/* Redraw the screen */
	case KTRL('R'):
		{
			do_cmd_redraw();
			break;
		}

#ifndef VERIFY_SAVEFILE

		/* Hack -- Save and don't quit */
	case KTRL('S'):
		{
			is_autosave = FALSE;
			do_cmd_save_game();
			break;
		}

#endif /* VERIFY_SAVEFILE */

	case KTRL('T'):
		{
			do_cmd_time();
		}
		break;

		/* Save and quit */
	case KTRL('X'):
		{
			alive = FALSE;

			/* Leaving */
			p_ptr->leaving = TRUE;

			break;
		}

		/* Quit (commit suicide) */
	case 'Q':
		{
			do_cmd_suicide();
			break;
		}

		/* Activate cmovie */
	case '|':
		{
			/* Stop ? */
			if (do_movies == 1)
			{
				do_stop_cmovie();
				msg_print("Cmovie recording stopped.");
			}
			else
			{
				do_start_cmovie();
			}
			break;
		}

		/* Extended command */
	case '#':
		{
			do_cmd_cli();
			break;
		}

		/* Check artifacts, uniques, objects */
	case '~':
		{
			do_cmd_knowledge();
			break;
		}

		/* Commands only available as extended commands: */

		/* Extended command help. */
	case CMD_CLI_HELP:
		{
			do_cmd_cli_help();
			break;
		}

		/* Connect to IRC. */
	case CMD_IRC_CONNECT:
		{
			irc_connect();
			break;
		}

		/* Speak on IRC. */
	case CMD_IRC_CHAT:
		{
			irc_chat();
			break;
		}

		/* Disconnect from IRC. */
	case CMD_IRC_DISCON:
		{
			irc_disconnect();
			break;
		}

		/* Game time. */
	case CMD_SHOW_TIME:
		{
			do_cmd_time();
			break;
		}

		/* Check skills. */
	case CMD_SHOW_SKILL:
		{
			do_cmd_skill();
			break;
		}

		/* Check abilities. */
	case CMD_SHOW_ABILITY:
		{
			do_cmd_ability();
			break;
		}

		/* Save a html screenshot. */
	case CMD_DUMP_HTML:
		{
			do_cmd_html_dump();
			break;
		}

		/* Record a macro. */
	case '$':
	case CMD_MACRO:
		{
			do_cmd_macro_recorder();
			break;
		}
	case CMD_BLUNDER:
		{
			if (do_control_walk()) break;
			do_cmd_walk((always_pickup || p_ptr->nastytrap161), FALSE);
			break;
		}
		/* Hack -- Unknown command */
	default:
		{
			int insanity = (p_ptr->msane - p_ptr->csane) * 100 / p_ptr->msane;

			/* Would like to have an option disabling this -- pelpel */
			if (rand_int(100) < insanity)
			{
				get_rnd_line("error.txt", error_m);
				sound(SOUND_ILLEGAL);
				msg_print(error_m);
			}
			else
			{
				prt("Type '?' for help.", 0, 0);
			}

			break;
		}
	}
}




/*
 * Process the player
 *
 * Notice the annoying code to handle "pack overflow", which
 * must come first just in case somebody manages to corrupt
 * the savefiles by clever use of menu commands or something.
 */
void process_player(void)
{
	int i, j;

	int speed_use;


	/*** Apply energy ***/

	if (hack_corruption)
	{
		msg_print("You feel different!");
		(void)gain_random_corruption(0);
		hack_corruption = FALSE;
	}

	/* Obtain current speed */
	speed_use = p_ptr->pspeed;

	/* Maximum value */
	if (speed_use > 199)
	{
		speed_use = 199;
	}

	/* Minimum value */
	else if (speed_use < 0)
	{
		speed_use = 0;
	}

	/* Give the player some energy */
	p_ptr->energy += extract_energy[speed_use];

	/* No turn yet */
	if (p_ptr->energy < 100) return;


	/*** Check for interupts ***/

	/* Complete resting */
	if (resting < 0)
	{
		/* Basic resting */
		if (resting == -1)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) && (p_ptr->csp >= p_ptr->msp))
			{
				disturb(0, 0);
			}
		}

		/* Complete resting */
		else if (resting == -2)
		{
			bool stop = TRUE;
			object_type *o_ptr;
			monster_race *r_ptr;

			/* Get the carried monster */
			o_ptr = &p_ptr->inventory[INVEN_CARRY];
			r_ptr = &r_info[o_ptr->pval];

			/* Stop resting */
			if ((!p_ptr->drain_life) && (p_ptr->chp != p_ptr->mhp)) stop = FALSE;
			if ((!p_ptr->drain_mana) && (p_ptr->csp != p_ptr->msp)) stop = FALSE;
			if (o_ptr->pval2 < o_ptr->pval3) stop = FALSE;
			if (p_ptr->blind || p_ptr->confused) stop = FALSE;
			if (p_ptr->poisoned || p_ptr->afraid) stop = FALSE;
			if (p_ptr->stun || p_ptr->cut) stop = FALSE;
			if (p_ptr->slow || p_ptr->paralyzed) stop = FALSE;
			if (p_ptr->image || p_ptr->word_recall) stop = FALSE;
			if (p_ptr->immov_cntr != 0) stop = FALSE;

			for (i = 0; i < 6; i++)
			{
				if (p_ptr->stat_cnt[i] > 0) stop = FALSE;
			}

			if (stop)
			{
				disturb(0, 0);
			}
			p_ptr->redraw |= (PR_STATE);
		}
	}

	/* Handle "abort" */
	if (!avoid_abort)
	{
		/* Check for "player abort" (semi-efficiently for resting) */
		if (running || command_rep || (resting && !(resting & 0x0F)))
		{
			/* Do not wait */
			inkey_scan = TRUE;

			/* Check for a key */
			if (inkey())
			{
				/* Flush input */
				flush();

				/* Disturb */
				disturb(0, 0);

				/* Hack -- Show a Message */
				msg_print("Cancelled.");
			}
		}
	}


	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	while (p_ptr->energy >= 100)
	{
		/* Notice stuff (if needed) */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff (if needed) */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff (if needed) */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff (if needed) */
		if (p_ptr->window) window_stuff();

		/* Hack -- mark current wilderness location as known */
		if (!p_ptr->wild_mode && dun_level == 0)
			wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].known = TRUE;


		/* Place the cursor on the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Refresh (optional) */
		if (fresh_before) Term_fresh();

		/* Hack -- Pack Overflow */
		if (p_ptr->inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			char o_name[80];

			object_type *o_ptr;

			/* Access the slot to be dropped */
			o_ptr = &p_ptr->inventory[item];

			/* Disturbing */
			disturb(0, 0);

			/* Warning */
			msg_print("Your pack overflows!");

			/* Describe */
			object_desc(o_name, o_ptr, TRUE, 3);

			/* Message */
			msg_format("You drop %s (%c).", o_name, index_to_label(item));

			/* Drop it (carefully) near the player */
			drop_near(o_ptr, 0, p_ptr->py, p_ptr->px);

			/* Modify, Describe, Optimize */
			inven_item_increase(item, -255);
			inven_item_describe(item);
			inven_item_optimize(item);

			/* Notice stuff (if needed) */
			if (p_ptr->notice) notice_stuff();

			/* Update stuff (if needed) */
			if (p_ptr->update) update_stuff();

			/* Redraw stuff (if needed) */
			if (p_ptr->redraw) redraw_stuff();

			/* Redraw stuff (if needed) */
			if (p_ptr->window) window_stuff();
		}


		/* Hack -- cancel "lurking browse mode" */
		if (!command_new) command_see = FALSE;


		/* Assume free turn */
		energy_use = 0;


		/* Paralyzed or Knocked Out */
		if ((p_ptr->paralyzed) || (p_ptr->stun >= 300))
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Resting */
		else if (resting)
		{
			/* Timed rest */
			if (resting > 0)
			{
				/* Reduce rest count */
				resting--;

				/* Redraw the state */
				p_ptr->redraw |= (PR_STATE);
			}

			p_ptr->did_nothing = TRUE;

			/* Take a turn */
			energy_use = 100;
		}

		/* Running */
		else if (running)
		{
			/* Take a step */
			run_step(0);

			/*
			 * Commented out because it doesn't make any sense
			 * to require a player holding down direction keys
			 * instead of using running commands when s/he follows
			 * Eru and do the opposite for the other deities -- pelpel
			 */
			/* p_ptr->did_nothing = TRUE; */
		}

		/* Repeated command */
		else if (command_rep)
		{
			/* Count this execution */
			command_rep--;

			/* Redraw the state */
			p_ptr->redraw |= (PR_STATE);

			/* Redraw stuff */
			redraw_stuff();

			/* Hack -- Assume messages were seen */
			msg_flag = FALSE;

			/* Clear the top line */
			prt("", 0, 0);

			/* Process the command */
			process_command();

			p_ptr->did_nothing = TRUE;
		}

		/* Normal command */
		else
		{
			/* Place the cursor on the player */
			move_cursor_relative(p_ptr->py, p_ptr->px);

			/* Get a command (normal) */
			request_command(FALSE);

			/* Process the command */
			process_command();
		}


		/*** Clean up ***/

		/* Significant */
		if (energy_use)
		{
			/* Use some energy */
			p_ptr->energy -= energy_use;


			/* Hack -- constant hallucination */
			if (p_ptr->image) p_ptr->redraw |= (PR_MAP);


			/* Shimmer monsters if needed */
			if (!avoid_other && !use_graphics && shimmer_monsters)
			{
				/* Clear the flag */
				shimmer_monsters = FALSE;

				/* Shimmer multi-hued monsters */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					monster_race *r_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Access the monster race */
					r_ptr = race_inf(m_ptr);

					/* Skip non-multi-hued monsters */
					if (!(r_ptr->flags1 & (RF1_ATTR_MULTI))) continue;

					/* Reset the flag */
					shimmer_monsters = TRUE;

					/* Redraw regardless */
					lite_spot(m_ptr->fy, m_ptr->fx);
				}
			}

			/* Shimmer objects if needed and requested */
			if (!avoid_other && !avoid_shimmer && !use_graphics &&
			                shimmer_objects)
			{
				/* Clear the flag */
				shimmer_objects = FALSE;

				/* Shimmer multi-hued objects */
				for (i = 1; i < o_max; i++)
				{
					/* Acquire object -- for speed only base items are allowed to shimmer */
					object_type *o_ptr = &o_list[i];
					object_kind *k_ptr = &k_info[o_ptr->k_idx];

					/* Skip dead or carried objects */
					if ((!o_ptr->k_idx) || (!o_ptr->ix)) continue;

					/* Skip non-multi-hued monsters */
					if (!(k_ptr->flags5 & (TR5_ATTR_MULTI))) continue;

					/* Reset the flag */
					shimmer_objects = TRUE;

					/* Redraw regardless */
					lite_spot(o_ptr->iy, o_ptr->ix);
				}
			}

			/*
			 * Shimmer features if needed and requested
			 *
			 * Note: this can be unbearably slow when a player chooses
			 * to use a REALLY big screen in levels filled with shallow
			 * water.  I believe this also hurts a lot on multiuser systems.
			 * However fast modern processors are, I/O cannot be made that
			 * fast, and that's why shimmering has been limited to small
			 * number of monsters -- pelpel
			 */
			if (!avoid_other && !avoid_shimmer && !use_graphics &&
			                !resting && !running)
			{
				for (j = panel_row_min; j <= panel_row_max; j++)
				{
					for (i = panel_col_min; i <= panel_col_max; i++)
					{
						cave_type *c_ptr = &cave[j][i];
						feature_type *f_ptr;

						/* Apply terrain feature mimics */
						if (c_ptr->mimic)
						{
							f_ptr = &f_info[c_ptr->mimic];
						}
						else
						{
							f_ptr = &f_info[f_info[c_ptr->feat].mimic];
						}

						/* Skip normal features */
						if (!(f_ptr->flags1 & (FF1_ATTR_MULTI))) continue;

						/* Redraw a shimmering spot */
						lite_spot(j, i);
					}
				}
			}


			/* Handle monster detection */
			if (repair_monsters)
			{
				/* Reset the flag */
				repair_monsters = FALSE;

				/* Rotate detection flags */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Nice monsters get mean */
					if (m_ptr->mflag & (MFLAG_NICE))
					{
						/* Nice monsters get mean */
						m_ptr->mflag &= ~(MFLAG_NICE);
					}

					/* Handle memorized monsters */
					if (m_ptr->mflag & (MFLAG_MARK))
					{
						/* Maintain detection */
						if (m_ptr->mflag & (MFLAG_SHOW))
						{
							/* Forget flag */
							m_ptr->mflag &= ~(MFLAG_SHOW);

							/* Still need repairs */
							repair_monsters = TRUE;
						}

						/* Remove detection */
						else
						{
							/* Forget flag */
							m_ptr->mflag &= ~(MFLAG_MARK);

							/* Assume invisible */
							m_ptr->ml = FALSE;

							/* Update the monster */
							update_mon(i, FALSE);

							/* Redraw regardless */
							lite_spot(m_ptr->fy, m_ptr->fx);
						}
					}
				}
			}

			/*
			 * Moved from dungeon() -- It'll get called whenever player
			 * spends energy, so that maze isn't incredibly easy for
			 * Sorcerors and alike any longer -- pelpel
			 *
			 * Forget everything when requested hehe I'm *NASTY*
			 */
			if (dun_level && (dungeon_flags1 & DF1_FORGET))
			{
				wiz_dark();
			}
			if (p_ptr->nastytrap114)
			{
				wiz_dark();
			}
		}


		/* Hack -- notice death */
		if (!alive || death) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;
	}
}



/*
 * Interact with the current dungeon level.
 *
 * This function will not exit until the level is completed,
 * the user dies, or the game is terminated.
 */
static void dungeon(void)
{
	/* Reset various flags */
	hack_mind = FALSE;
	char dummy[5];

	/* Not leaving */
	p_ptr->leaving = FALSE;

	/* Reset the "command" vars */
	command_cmd = 0;
	command_new = 0;
	command_rep = 0;
	command_arg = 0;
	command_dir = 0;

	/* Make sure partial summoning counter is initialized. */
	p_ptr->maintain_sum = 0;

	/* Cancel the target */
	target_who = 0;

	/* Cancel the health bar */
	health_track(0);


	/* Check visual effects */
	shimmer_monsters = TRUE;
	shimmer_objects = TRUE;
	repair_monsters = TRUE;
	repair_objects = TRUE;


	/* Disturb */
	disturb(1, 0);

	/* Track maximum player level */
	if (p_ptr->max_plv < p_ptr->lev)
	{
		p_ptr->max_plv = p_ptr->lev;
	}

	/* Track maximum dungeon level (if not in quest -KMW-) */
	if ((max_dlv[dungeon_type] < dun_level) && !p_ptr->inside_quest)
	{
		max_dlv[dungeon_type] = dun_level;
	}
	if ((max_dlv_real[dungeon_type] < dun_level) && !p_ptr->inside_quest)
	{
		max_dlv_real[dungeon_type] = dun_level;
	}

	/* No stairs down from Quest */
	if (is_quest(dun_level) && (is_quest(dun_level) != QUEST_RANDOM) && !p_ptr->astral)
	{
		create_down_stair = FALSE;
		create_down_shaft = FALSE;
	}

	/* Paranoia -- no stairs from town or wilderness */
	if (!dun_level) create_down_stair = create_up_stair = FALSE;
	if (!dun_level) create_down_shaft = create_up_shaft = FALSE;

	/* Option -- no connected stairs, forced with disconnect nastytrap */
	if (!dungeon_stair || p_ptr->nastytrap156) create_down_stair = create_up_stair = FALSE;
	if (!dungeon_stair || p_ptr->nastytrap156) create_down_shaft = create_up_shaft = FALSE;

	/* no connecting stairs on special levels */
	if (!(dungeon_flags2 & DF2_NO_STAIR)) create_down_stair = create_up_stair = FALSE;
	if (!(dungeon_flags2 & DF2_NO_STAIR)) create_down_shaft = create_up_shaft = FALSE;

	/* don't place a fucking staircase every time you re-enter a special level!!! --Amy */
	if (get_dungeon_save(dummy)) create_down_stair = create_up_stair = FALSE;
	if (get_dungeon_save(dummy)) create_down_shaft = create_up_shaft = FALSE;

	/* Make a stairway. */
	if ((create_up_stair || create_down_stair ||
	                create_up_shaft || create_down_shaft) &&
	                !get_fbranch())
	{
		/* Place a stairway */
		if (cave_valid_bold(p_ptr->py, p_ptr->px))
		{
			/* XXX XXX XXX */
			delete_object(p_ptr->py, p_ptr->px);

			/* Make stairs */
			if (create_down_stair)
			{
				cave_set_feat(p_ptr->py, p_ptr->px, (dungeon_flags1 & DF1_FLAT) ? FEAT_WAY_MORE : FEAT_MORE);
			}
			else if (create_down_shaft)
			{
				cave_set_feat(p_ptr->py, p_ptr->px, (dungeon_flags1 & DF1_FLAT) ? FEAT_WAY_MORE : FEAT_SHAFT_DOWN);
			}
			else if (create_up_shaft)
			{
				cave_set_feat(p_ptr->py, p_ptr->px, (dungeon_flags1 & DF1_FLAT) ? FEAT_WAY_LESS : FEAT_SHAFT_UP);
			}
			else
			{
				cave_set_feat(p_ptr->py, p_ptr->px, (dungeon_flags1 & DF1_FLAT) ? FEAT_WAY_LESS : FEAT_LESS);
			}
		}

		/* Cancel the stair request */
		create_down_stair = create_up_stair = FALSE;
		create_down_shaft = create_up_shaft = FALSE;
	}

	/* Hack - Assume invalid panel */
	panel_row_min = cur_hgt;
	panel_row_max = 0;
	panel_col_min = cur_wid;
	panel_col_max = 0;

	/* Center the panel */
	verify_panel();

	/* Flush messages */
	msg_print(NULL);


	/* Enter "xtra" mode */
	character_xtra = TRUE;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER);

	/* Redraw dungeon */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS | PU_POWERS | PU_SANITY | PU_BODY);

	/* Calculate torch radius */
	p_ptr->update |= (PU_TORCH);

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Redraw stuff */
	window_stuff();

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_FLOW | PU_DISTANCE | PU_MON_LITE);

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS | PU_POWERS | PU_BODY);

	/* Combine / Reorder the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Notice stuff */
	notice_stuff();

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Window stuff */
	window_stuff();

	/* Refresh */
	Term_fresh();


	/* Announce (or repeat) the feeling */
	if (dun_level) do_cmd_feeling();


	/* Hack -- notice death or departure */
	if (!alive || death) return;

	/*** Process this dungeon level ***/

	/* Reset the monster generation level */
	monster_level = dun_level;
	/*msg_format("Monster level set to %d.", monster_level);*/
	/*if (!dun_level && !p_ptr->wild_mode) {
		monster_level = wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].level + rand_int(p_ptr->lev);
	}*/

	/* Reset the object generation level */
	object_level = dun_level;

	hack_mind = TRUE;

	/* Mega Hack, if needed wipe all stairs */
	if (dungeon_type == DUNGEON_DEATH)
	{
		int i, j;

		for (i = 0; i < cur_wid; i++)
		{
			for (j = 0; j < cur_hgt; j++)
			{
				cave_type *c_ptr = &cave[j][i];

				switch (c_ptr->feat)
				{
				case FEAT_MORE:
				case FEAT_LESS:
				case FEAT_SHAFT_UP:
				case FEAT_SHAFT_DOWN:
					{
						cave_set_feat(j, i, FEAT_FLOOR);
						break;
					}
				}
			}
		}

		/* Reset the monster generation level */
		monster_level = 127;

		/* Reset the object generation level */
		object_level = 0;
	}

	/* Main loop */
	while (TRUE)
	{
		/* Hack -- Compact the monster list occasionally */
		if (m_cnt + 32 > max_m_idx) compact_monsters(64);

		/* Hack -- Compress the monster list occasionally */
		if (m_cnt + 32 < m_max) compact_monsters(0);


		/* Hack -- Compact the object list occasionally */
		if (o_cnt + 32 > max_o_idx) compact_objects(64);

		/* Hack -- Compress the object list occasionally */
		if (o_cnt + 32 < o_max) compact_objects(0);



		/* Process the player */
		process_player();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!alive || death) break;


#ifdef pelpel

		/* Process spell effects */
		process_effects();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!alive || death) break;

#endif /* pelpel */


		total_friends = 0;
		total_friend_levels = 0;

		/* Process all of the monsters */
		process_monsters();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!alive || death) break;

		/* is the player silenced? then don't allow them to sneak in a cast by quaffing restore mana while having fast speed --Amy */
		if (p_ptr->tim_manasilence) {
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
			p_ptr->redraw |= (PR_MANA);
		}

		/* Process the world */
		process_world();

		/* Process the appropriate hooks */
		process_hooks(HOOK_END_TURN, "(d)", is_quest(dun_level));

		/* Make it pulsate and live !!!! */
		if ((dungeon_flags1 & DF1_EVOLVE) && dun_level)
		{
			if (!(turn % 10)) evolve_level(TRUE);
		}

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Window stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(p_ptr->py, p_ptr->px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!alive || death) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Count game turns */
		turn++;
	}

	/* Did we leave a dungeon ? */
	if ((dun_level < d_info[dungeon_type].mindepth) && !is_recall)
	{
		dun_level = 0;

		if (d_info[dungeon_type].ix > -1)
		{
			p_ptr->wilderness_x = d_info[dungeon_type].ix;
			p_ptr->wilderness_y = d_info[dungeon_type].iy;
		}

		dungeon_type = DUNGEON_WILDERNESS;
	}

	if (dun_level > d_info[dungeon_type].maxdepth)
	{
		dun_level = 0;

		if (d_info[dungeon_type].ox > -1)
		{
			p_ptr->wilderness_x = d_info[dungeon_type].ox;
			p_ptr->wilderness_y = d_info[dungeon_type].oy;
		}

		dungeon_type = DUNGEON_WILDERNESS;
	}

	is_recall = FALSE;
}




/*
 * Load some "user pref files"
 */
static void load_all_pref_files(void)
{
	char buf[1024];


	/* Access the "race" pref file */
	sprintf(buf, "%s.prf", rp_ptr->title + rp_name);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", spp_ptr->title + c_name);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_name);

	/* Process that file */
	process_pref_file(buf);

	/* Process player specific automatizer sets */
	tome_dofile_anywhere(ANGBAND_DIR_USER, format("%s.atm", player_name), FALSE);
}

/*
 * Actually play a game
 *
 * If the "new_game" parameter is true, then, after loading the
 * savefile, we will commit suicide, if necessary, to allow the
 * player to start a new game.
 */
void play_game(bool new_game)
{
	int i, tmp_dun;

	bool cheat_death = FALSE;

	hack_corruption = FALSE;

	/* Hack -- Character is "icky" */
	character_icky = TRUE;


	/* Make sure main term is active */
	Term_activate(angband_term[0]);

	/* Initialise the resize hook XXX XXX XXX */
	angband_term[0]->resize_hook = resize_map;

	/* XXX XXX XXX hardcoded number of terms */
	for (i = 1; i < 8; i++)
	{
		if (angband_term[i])
		{
			/* Add redraw hook */
			angband_term[i]->resize_hook = resize_window;
		}
	}


	/* Hack -- turn off the cursor */
	(void)Term_set_cursor(0);

	/* Character list */
	if (!new_game && !no_begin_screen) new_game = begin_screen();
	no_begin_screen = FALSE;

	/* Attempt to load */
	if (!load_player())
	{
		/* Oops */
		quit("broken savefile");
	}

	/* Nothing loaded */
	if (!character_loaded)
	{
		/* Make new player */
		new_game = TRUE;

		/* The dungeon is not ready */
		character_dungeon = FALSE;
	}
	else
	{
		int i;

		/* Init new skills to their defaults */
		for (i = old_max_s_idx; i < max_s_idx; i++)
		{
			s32b value = 0, mod = 0;

			compute_skills(&value, &mod, i);

			init_skill(value, mod, i);
		}
	}

#if 1

	/* Process old character */
	if (!new_game)
	{
		/* Process the player name */
		process_player_name(FALSE);
	}

#endif

	/* Init the RNG */
	if (Rand_quick)
	{
		u32b seed;

		/* Basic seed */
		seed = (time(NULL));

#ifdef SET_UID

		/* Mutate the seed on Unix machines */
		seed = ((seed >> 3) * (getpid() << 1));

#endif

		/* Use the complex RNG */
		Rand_quick = FALSE;

		/* Seed the "complex" RNG */
		Rand_state_init(seed);
	}

	/* Extract the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_page;
		int ob = option_info[i].o_bit;

		/* Set the "default" options */
		if (option_info[i].o_var)
		{
			/* Set */
			if (option_flag[os] & (1L << ob))
			{
				/* Set */
				(*option_info[i].o_var) = TRUE;
			}

			/* Clear */
			else
			{
				/* Clear */
				(*option_info[i].o_var) = FALSE;
			}
		}
	}

	/* Roll new character */
	if (new_game)
	{
		s32b ret;

		/* Are we authorized to create new chars? */
		call_lua("get_module_info", "(s)", "d", "allow_birth", &ret);

		if (!ret)
		{
			msg_box("Sorry, this module does not allow character creation.", -1, -1);

			/* Close stuff */
			close_game();

			/* Quit */
			quit(NULL);
		}

		process_hooks(HOOK_INIT, "()");

		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Hack -- seed for flavors */
		seed_flavor = rand_int(0x10000000);

		/* Hack -- seed for town layout */
		seed_town = rand_int(0x10000000);

		/* Roll up a new character */
		player_birth();

		/* Start in town, or not */
		if (p_ptr->astral) dun_level = 98;
		else dun_level = 0;
		p_ptr->inside_quest = 0;
		p_ptr->inside_arena = 0;

		/* Hack -- enter the world */
		/* Mega-hack Vampires and Spectres start in the dungeon */
		if (PRACE_FLAG(PR1_UNDEAD))
		{
			turn = (10L * DAY / 2) + (START_DAY * 10) + 1;
		}
		else
		{
			turn = (START_DAY * 10) + 1;
		}
	}

	/* Flash a message */
	prt("Please wait...", 0, 0);

	/* Flush the message */
	Term_fresh();

	/* Be sure to not bother the player */
	calc_powers_silent = TRUE;

	/* Hack -- Enter wizard mode */
	if (arg_wizard && enter_wizard_mode()) wizard = TRUE;

	/* Flavor the objects */
	flavor_init();

	/* Reset the visual mappings */
	reset_visuals();

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER);

	/* Window stuff */
	window_stuff();

	/* load user file */
	process_pref_file("user.prf");

	/* Load the "pref" files */
	load_all_pref_files();

	/* Set or clear "rogue_like_commands" if requested */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* Initialize vault info */
	if (init_v_info()) quit("Cannot initialize vaults");

	/* Initialize hooks */
	init_hooks();
	ingame_help(p_ptr->help.enabled);

	/* React to changes */
	Term_xtra(TERM_XTRA_REACT, 0);

	/* Mega hack, prevent lots of bugs */
	if ((p_ptr->px == 0) || (p_ptr->py == 0))
	{
		p_ptr->px = 1;
		p_ptr->py = 1;
	};

	/* Hack - if note file exists, load it */
	if (!new_game && take_notes)
	{
		add_note_type(NOTE_ENTER_DUNGEON);
	}

	/* Generate a dungeon level if needed */
	if (!character_dungeon) generate_cave();

	/* Ok tell the scripts that the game is about to start */
	process_hooks(HOOK_GAME_START, "()");

	/* Character is now "complete" */
	character_generated = TRUE;


	/* Hack -- Character is no longer "icky" */
	character_icky = FALSE;


	/* Start game */
	alive = TRUE;

	/* Hack -- Enforce "delayed death" */
	if (p_ptr->chp < 0) death = TRUE;

	/* Should we use old colors */
	if (autoload_old_colors)
	{
		process_pref_file("422color.prf");
	}


	/* Process */
	while (TRUE)
	{
		/* Save the level */
		old_dun_level = dun_level;
		p_ptr->old_wild_mode = p_ptr->wild_mode;

		/* We reached surface ? good, lets go down again !! */
		if (p_ptr->astral && !dun_level)
		{
			p_ptr->astral = FALSE;
			cmsg_print(TERM_L_GREEN,
			           "Well done ! You reached the town ! "
			           "You can now go down again.");
		}

		/* Update monster list window */
		p_ptr->window |= (PW_M_LIST);

#ifdef SAVE_HACK

		/* Process the level */
		if (!save_hack)
		{
			dungeon();
		}
		else
		{
			generate_cave();
		}

		save_hack = FALSE;

		p_ptr->leaving = TRUE;

#else

		/* Process the level */
		dungeon();

#endif

		/* Save the current level if in a persistent level */
		tmp_dun = dun_level;
		dun_level = old_dun_level;
		save_dungeon();
		dun_level = tmp_dun;

		/* A death fate affects level generation */
		for (i = 0; i < MAX_FATES; i++)
		{
			/* Ignore empty slots */
			if (!fates[i].fate) continue;

			/* Ignore non-applicable fates */
			if (fates[i].level != dun_level) continue;

			/* Non-serious fate fails to fire 50% of time */
			if (!fates[i].serious && (rand_int(2) == 0)) continue;

			/* Analyse fate */
			switch (fates[i].fate)
			{
				/* You are doomed */
			case FATE_DIE:
				{
					cmsg_print(TERM_L_DARK, "You were fated to die here.  DIE!");

					/* You shall perish there */
					dungeon_type = DUNGEON_DEATH;
					dun_level = d_info[dungeon_type].mindepth; /* was 1 */

					fates[i].fate = FATE_NONE;
					break;
				}
			}
		}

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Window stuff */
		if (p_ptr->window) window_stuff();

		/* Cancel the target */
		target_who = 0;

		/* Cancel the health bar */
		health_track(0);


		/* Forget the lite */
		forget_mon_lite();

		/* Forget the view */
		forget_view();

		/* Handle "quit and save" */
		if (!alive && !death) break;


		/* Erase the old cave */
		wipe_o_list();


		/* XXX XXX XXX */
		msg_print(NULL);

		/* Accidental Death */
		if (alive && death)
		{
			cheat_death = FALSE;
			object_type *lifesavingammy;
			int will_lifesave = 0; /* 1 = regular amulet is LS, 2 = second amulet is LS */

			/* amulet of life saving (from Nethack) by Amy; annoyingly we have to account for the fact that you
			 * can have two neck slots in certain monster forms */
			lifesavingammy = &p_ptr->inventory[INVEN_NECK];
			if (lifesavingammy && lifesavingammy->k_idx) {
				if (lifesavingammy->sval == SV_AMULET_LIFE_SAVING) {
					will_lifesave = 1;
				}
			}
			if (!will_lifesave) {
				lifesavingammy = &p_ptr->inventory[INVEN_NECK + 1];
				if (lifesavingammy && lifesavingammy->k_idx) {
					if (lifesavingammy->sval == SV_AMULET_LIFE_SAVING) {
						will_lifesave = 2;
					}
				}
			}

			/* Can we die ? please let us die ! */
			if (process_hooks(HOOK_DIE, "()"))
			{
				cheat_death = TRUE;
			}

			/* Deus ex machina */
			else if (granted_resurrection())
			{
				cheat_death = TRUE;
				p_ptr->grace = -200000;
				cmsg_format(TERM_L_GREEN,
				            "The power of %s raises you back from the grave!",
				            deity_info[p_ptr->pgod].name);
				msg_print(NULL);
			}

			else if (will_lifesave)
			{
				cheat_death = TRUE;

				if (will_lifesave == 1) {
					inven_item_increase(INVEN_NECK, -1);
					inven_item_optimize(INVEN_NECK);
				} else {
					inven_item_increase(INVEN_NECK + 1, -1);
					inven_item_optimize(INVEN_NECK + 1);
				}
				do_dec_stat(A_CON, STAT_DEC_PERMANENT); /* like in nethack */

				cmsg_print(TERM_L_GREEN, "But wait... Your amulet begins to glow!");
				cmsg_print(TERM_L_GREEN, "You feel much better! The amulet disintegrates!");
				msg_print(NULL);

			}

			/* Blood of life */
			else if (p_ptr->allow_one_death > 0)
			{
				cheat_death = TRUE;

				/* Lose one extra life */
				p_ptr->allow_one_death--;

				cmsg_print(TERM_L_GREEN,
				           "You have been saved by the Blood of Life!");
				msg_print(NULL);
			}

			/* Life saving if fated to not die by a mortal being, but only once because of game balance --Amy */
			else if (p_ptr->no_mortal && lifesave_no_mortal)
			{
				p_ptr->no_mortal = FALSE;
				lifesave_no_mortal = FALSE;

				for (i = 0; i < MAX_FATES; i++)
				{
					/* Ignore empty slots */
					if (!fates[i].fate) continue;

					/* Analyse fate */
					switch (fates[i].fate)
					{
						case FATE_NO_DIE_MORTAL:
						{
							fates[i].fate = FATE_NONE;
							break;
						}
					}
				}

				cheat_death = TRUE;
				cmsg_print(TERM_L_GREEN,
				           "You weren't supposed to die to that monster and therefore survive!");
				msg_print(NULL);
			}

			/* Cheat death option */
			else if ((wizard || cheat_live) && !get_check("Die? "))
			{
				cheat_death = TRUE;

				/* Mark social class, reset age, if needed */
				if (p_ptr->sc) p_ptr->sc = p_ptr->age = 0;

				/* Increase age */
				p_ptr->age++;

				/* Mark savefile */
				noscore |= 0x0001;
				msg_print("You invoke wizard mode and cheat death.");
				msg_print(NULL);
			}

			if (cheat_death)
			{
				/* Restore the winner status */
				total_winner = has_won;

				/* One more life spent */
				p_ptr->lives++;
				p_ptr->char_death_count++;

				/* Restore hit points */
				p_ptr->chp = p_ptr->mhp;
				p_ptr->chp_frac = 0;

				/* Heal sanity */
				p_ptr->csane = p_ptr->msane;
				p_ptr->csane_frac = 0;

				/* Restore spell points */
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;

				/* Hack -- Healing */
				(void)set_blind(0);
				(void)set_confused(0);
				(void)set_poisoned(0);
				(void)set_afraid(0);
				(void)set_paralyzed(0);
				(void)set_image(0);
				(void)set_stun(0);
				(void)set_cut(0);

				/* accounting for a new ailment. -LM- */
				p_ptr->black_breath = FALSE;

				/* Hack -- don't go to undead form */
				p_ptr->necro_extra &= ~CLASS_UNDEAD;

				/* Hack -- Prevent starvation */
				(void)set_food(PY_FOOD_MAX - 1);

				/* Hack -- cancel recall */
				if (p_ptr->word_recall)
				{
					/* Message */
					msg_print("A tension leaves the air around you...");
					msg_print(NULL);

					/* Hack -- Prevent recall */
					p_ptr->word_recall = 0;
				}

				/* Note cause of death XXX XXX XXX */
				(void)strcpy(died_from, "Cheating death");

				/* Do not die */
				death = FALSE;

				/* New depth -KMW- */
				/* dun_level = 0; */
				p_ptr->inside_arena = 0;
				leaving_quest = 0;
				p_ptr->inside_quest = 0;

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}

		/* Handle "death" */
		if (death)
		{
			break;
		}

		/* Mega hack */
		if (dun_level) p_ptr->wild_mode = FALSE;

		/* Make a new level */
		process_hooks(HOOK_NEW_LEVEL, "(d)", is_quest(dun_level));
		generate_cave();
	}

	/* Close stuff */
	close_game();

	/* Quit */
	quit(NULL);
}

