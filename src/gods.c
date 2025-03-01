/* File: gods.c */

/* Purpose: Deities code */

/*
 * Copyright (c) 2002 DarkGod
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

/*
 * Add amt piety is god is god
 */
void inc_piety(int god, s32b amt)
{
	s32b old = p_ptr->grace;

	/* valarin and nether skills, by Amy; I have no idea what they used to do, apart from modifying the damage done
	 * to you by hellfire/holy fire, but decided they should modify your piety gain/loss */
	bool goodgod = TRUE;
	if (god == GOD_AMYBSOD || god == GOD_MELKOR || god == GOD_INGEBORG) goodgod = FALSE;

	/* don't gain piety if impious trap is active */
	if (p_ptr->nastytrap4 && amt > 0) return;

	/* valarin skill is for good goods (Eru, Yavanna, Manwe, Tulkas, Ulmo, Aule, Mandos, Varda)
	 * nether skill is for evil gods (Melkor, Amy)
	 * the skill for your god increases both piety gain and loss, so it can be a double-edged sword
	 * the skill that isn't for your god will reduce piety loss, which can couteract the negatives of the former skill
	 * that way, neither of the two skills will be useless, unless your char is an atheist */

	if (goodgod) {
		amt *= (50 + get_skill(SKILL_VALARIN));
		amt /= 50;
		if (amt < 0) {
			amt *= (100 - get_skill_scale(SKILL_NETHER, 20));
			amt /= 100;
			if (amt == 0) amt = -1; /* fail safe */
		}
	} else {
		amt *= (50 + get_skill(SKILL_NETHER));
		amt /= 50;
		if (amt < 0) {
			amt *= (100 - get_skill_scale(SKILL_VALARIN, 20));
			amt /= 100;
			if (amt == 0) amt = -1; /* fail safe */
		}
	}

	/* it should be harder to gain piety if you already have a lot of it --Amy */
	if (amt > 0) {

		if (p_ptr->grace > 275000) {
			amt /= (4 + randint(196));
		} else if (p_ptr->grace > 250000) {
			amt /= (4 + randint(96));
		} else if (p_ptr->grace > 200000) {
			amt /= (4 + randint(46));
		} else if (p_ptr->grace > 180000) {
			amt /= (4 + randint(36));
		} else if (p_ptr->grace > 160000) {
			amt /= (4 + randint(31));
		} else if (p_ptr->grace > 140000) {
			amt /= (4 + randint(26));
		} else if (p_ptr->grace > 120000) {
			amt /= (4 + randint(21));
		} else if (p_ptr->grace > 100000) {
			amt /= (4 + randint(16));
		} else if (p_ptr->grace > 90000) {
			amt /= (4 + randint(14));
		} else if (p_ptr->grace > 80000) {
			amt /= (4 + randint(12));
		} else if (p_ptr->grace > 70000) {
			amt /= (4 + randint(10));
		} else if (p_ptr->grace > 60000) {
			amt /= (4 + randint(8));
		} else if (p_ptr->grace > 50000) {
			amt /= (4 + randint(7));
		} else if (p_ptr->grace > 45000) {
			amt /= (4 + randint(6));
		} else if (p_ptr->grace > 40000) {
			amt /= 5;
		} else if (p_ptr->grace > 35000) {
			amt *= 3; amt /= 10;
		} else if (p_ptr->grace > 30000) {
			amt *= 2; amt /= 5;
		} else if (p_ptr->grace > 25000) {
			amt /= 2;
		} else if (p_ptr->grace > 20000) {
			amt *= 3; amt /= 5;
		} else if (p_ptr->grace > 15000) {
			amt *= 7; amt /= 10;
		} else if (p_ptr->grace > 10000) {
			amt *= 4; amt /= 5;
		} else if (p_ptr->grace > 5000) {
			amt *= 9; amt /= 10;
		}
		/* but gain at least one point no matter what */
		if (amt < 1) amt = 1;
	}

	if ((god == GOD_ALL) || (god == p_ptr->pgod))
	{
		set_grace(p_ptr->grace + amt);
	
		if(amt > 0 && p_ptr->grace <= old)
			set_grace(300000);

		if(amt < 0 && p_ptr->grace >= old)
			set_grace(-300000);
	}
}

/*
 * Renounce to religion
 */
void abandon_god(int god)
{
	if ((god == GOD_ALL) || (god == p_ptr->pgod))
	{
		p_ptr->pgod = GOD_NONE;
		set_grace(0);
	}
}

/*
 * Get a religion
 */
void follow_god(int god, bool silent)
{
	/* Poor unbelievers, i'm so mean ... BOUHAHAHA
	 * Amy edit: hardcode it to the unbeliever role using an ugly h@ck */
	/*if (get_skill(SKILL_ANTIMAGIC))*/
	if (!strcmp(spp_ptr->title + c_name, "Unbeliever") || !strcmp(spp_ptr->title + c_name, "Total Ascetic") || !strcmp(spp_ptr->title + c_name, "Atheist Priest") )
	{
		msg_print("Don't be silly; you don't believe in gods.");
		return;
	}

	/* Are we allowed ? */
	if (process_hooks(HOOK_FOLLOW_GOD, "(d,s)", god, "ask"))
		return;

	if (p_ptr->pgod == GOD_NONE)
	{
		p_ptr->pgod = god;

		/* Melkor offer Udun magic */
		GOD(GOD_MELKOR)
		{
			s_info[SKILL_UDUN].hidden = FALSE;
			if (!silent) msg_print("You feel the dark powers of Melkor in you.  You can now use the Udun skill.");
		}

		GOD(GOD_AMYBSOD)
		{
			s_info[SKILL_UDUN].hidden = FALSE;
			if (!silent) msg_print("You feel the fleecy colors of AmyBSOD in you.  You can now use the Udun skill.");
		}

		GOD(GOD_INGEBORG)
		{
			s_info[SKILL_UDUN].hidden = FALSE;
			if (!silent) msg_print("You feel the chaotic desires of Ingeborg in you.  You can now use the Udun skill.");
		}

		/* Anything to be done? */
		process_hooks(HOOK_FOLLOW_GOD, "(d,s)", god, "done");
	}
}

/*
 * Show religious info.
 */
bool show_god_info(bool ext)
{
	int pgod = p_ptr->pgod;
	int tmp;

	deity_type *d_ptr;

	if (pgod < 0)
	{
		msg_print("You don't worship anyone.");
		msg_print(NULL);
		return FALSE;
	}
	else
	{
		int i;

		d_ptr = &deity_info[pgod];

		msg_print(NULL);

		character_icky = TRUE;
		Term_save();

		text_out(format("You worship %s. ", d_ptr->name));
		for (i = 0; (i < 10) && (strcmp(d_ptr->desc[i], "")); i++)
			text_out(d_ptr->desc[i]);
		text_out("\n");

		tmp = inkey();

		Term_load();
		character_icky = FALSE;
	}

	return TRUE;
}

/*
 * Rescale the wisdom value to a 0 <-> max range
 */
int wisdom_scale(int max)
{
	int i = p_ptr->stat_ind[A_WIS];

	return (i * max) / 37;
}

/* Find a god by name */
int find_god(cptr name)
{
	int i;

	for (i = 0; i < max_gods; i++)
	{
		/* The name matches */
		if (streq(deity_info[i].name, name)) return (i);
	}
	return -1;
}
