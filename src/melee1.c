/* File: melee1.c */

/* Purpose: Monster attacks */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/* god-in-the-heaven-o-matic, we actually need to copy over allllll the stuff from spells1.c... --Amy
 * and *all* just because that weird typedef cannot be easily externalized, it seems :( */

/*
 * This seems like a pretty standard "typedef"
 */
typedef int (*inven_func)(object_type *);

/*
 * Destroys a type of item on a given percent chance
 * Note that missiles are no longer necessarily all destroyed
 * Destruction taken from "melee.c" code for "stealing".
 * Returns number of items destroyed.
 */
static int inven_damage(inven_func typ, int perc)
{
	int i, j, k, amt;

	object_type *o_ptr;

	char o_name[80];


	/* Count the casualties */
	k = 0;

	/* Scan through the slots backwards */
	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &p_ptr->inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Hack -- for now, skip artifacts */
		if (artifact_p(o_ptr) || o_ptr->art_name) continue;

		/* Give this item slot a shot at death */
		if ((*typ)(o_ptr))
		{
			/* Count the casualties */
			for (amt = j = 0; j < o_ptr->number; ++j)
			{
				if (rand_int(100) < perc) amt++;
			}

			/* Some casualities */
			if (amt)
			{
				/* Get a description */
				object_desc(o_name, o_ptr, FALSE, 3);

				/* Message */
				msg_format("%sour %s (%c) %s destroyed!",
				           ((o_ptr->number > 1) ?
				            ((amt == o_ptr->number) ? "All of y" :
				             (amt > 1 ? "Some of y" : "One of y")) : "Y"),
						           o_name, index_to_label(i),
						           ((amt > 1) ? "were" : "was"));

				/* Potions smash open */
				if (k_info[o_ptr->k_idx].tval == TV_POTION)
		{
					(void)potion_smash_effect(0, p_ptr->py, p_ptr->px, o_ptr->sval);
				}

				/*
				 * Hack -- If rods or wand are destroyed, the total maximum 
				 * timeout or charges of the stack needs to be reduced, 
				 * unless all the items are being destroyed. -LM-
				 */
				if ( (o_ptr->tval == TV_WAND || o_ptr->tval == TV_ROD || o_ptr->tval == TV_ROD_MAIN)
				                && (amt < o_ptr->number))
				{
					o_ptr->pval -= o_ptr->pval * amt / o_ptr->number;
				}

				/* Destroy "amt" items */
				inven_item_increase(i, -amt);
				inven_item_optimize(i);

				/* Count the casualties */
				k += amt;
			}
		}
	}

	/* Return the casualty count */
	return (k);
}

/*
 * Critical blow.  All hits that do 95% of total possible damage,
 * and which also do at least 20 damage, or, sometimes, N damage.
 * This is used only to determine "cuts" and "stuns".
 */
static int monster_critical(int dice, int sides, int dam)
{
	int max = 0;
	int total = dice * sides;

	/* Must do at least 95% of perfect */
	if (dam < total * 19 / 20) return (0);

	/* Weak blows rarely work */
	if ((dam < 20) && (rand_int(100) >= dam)) return (0);

	/* Perfect damage */
	if (dam == total) max++;

	/* Super-charge */
	if (dam >= 20)
	{
		while (rand_int(100) < 2) max++;
	}

	/* Critical damage */
	if (dam > 45) return (6 + max);
	if (dam > 33) return (5 + max);
	if (dam > 25) return (4 + max);
	if (dam > 18) return (3 + max);
	if (dam > 11) return (2 + max);
	return (1 + max);
}





/*
 * Determine if a monster attack against the player succeeds.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match monster power against player armor.
 */
static int check_hit(int power, int level)
{
	int i, k, ac;

	/* Percentile dice */
	k = rand_int(100);

	/* Hack -- Always miss or hit */
	if (k < 10) return (k < 5);

	/* Calculate the "attack quality" */
	i = (power + (level * 3));

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Power and Level compete against Armor */
	if ((i > 0) && (randint(i - luck( -10, 10)) > ((ac * 3) / 4))) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_insult[] =
{
	"insults you!",
	"insults your mother!",
	"jumps around you!",
	"humiliates you!",
	"defiles you!",
	"dances around you!",
	"makes obnoxious gestures!",
	"pokes you!!!"
};



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_moan[] =
{
	"seems sad about something.",
	"asks if you have seen his dogs.",
	"tells you to get off his land.",
	"mumbles something about mushrooms.",

	/* Mathilde's sentence */
	"giggles at you.",
	"asks you if you want to giggle with her.",
	"says she is always happy."
};

/*
 * Attack the player via physical attacks.
 */
bool carried_make_attack_normal(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	int ap_cnt;

	int k, tmp, ac, rlev, sqrtrlev;
	int do_cut, do_stun;

	char ddesc[80] = "your symbiote";
	cptr sym_name = symbiote_name(TRUE);

	bool blinked;
	bool touched = FALSE, alive = TRUE;
	bool explode = FALSE;

	/* Not allowed to attack */
	if (r_ptr->flags1 & (RF1_NEVER_BLOW)) return (FALSE);

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Extract the effective monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);
	sqrtrlev = rlev;
	if (sqrtrlev > 1) {
		sqrtrlev = sroot(rlev);
		if (sqrtrlev < 1) sqrtrlev = 1;
	}

	/* Assume no blink */
	blinked = FALSE;

	/* Scan through all four blows */
	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		bool visible = FALSE;
		bool obvious = FALSE;

		int power = 0;
		int damage = 0;

		cptr act = NULL;

		/* Extract the attack infomation */
		int effect = r_ptr->blow[ap_cnt].effect;
		int method = r_ptr->blow[ap_cnt].method;
		int d_dice = r_ptr->blow[ap_cnt].d_dice;
		int d_side = r_ptr->blow[ap_cnt].d_side;

		/* secret attack nastytrap by Amy */
		if (p_ptr->nastytrap167 && magik(2) && (effect == RBE_HURT)) {
			switch (randint(40)) {
				default:
				case 1:
					effect = RBE_HURT; break;
				case 2:
					effect = RBE_POISON; break;
				case 3:
					effect = RBE_UN_BONUS; break;
				case 4:
					effect = RBE_UN_POWER; break;
				case 5:
					effect = RBE_EAT_GOLD; break;
				case 6:
					effect = RBE_EAT_LITE; break;
				case 7:
					effect = RBE_EAT_ITEM; break;
				case 8:
					effect = RBE_EAT_FOOD; break;
				case 9:
					effect = RBE_ACID; break;
				case 10:
					effect = RBE_ELEC; break;
				case 11:
					effect = RBE_FIRE; break;
				case 12:
					effect = RBE_COLD; break;
				case 13:
					effect = RBE_BLIND; break;
				case 14:
					effect = RBE_CONFUSE; break;
				case 15:
					effect = RBE_TERRIFY; break;
				case 16:
					effect = RBE_PARALYZE; break;
				case 17:
					effect = RBE_LOSE_STR; break;
				case 18:
					effect = RBE_LOSE_INT; break;
				case 19:
					effect = RBE_LOSE_DEX; break;
				case 20:
					effect = RBE_LOSE_CON; break;
				case 21:
					effect = RBE_LOSE_CHR; break;
				case 22:
					effect = RBE_LOSE_WIS; break;
				case 23:
					effect = RBE_LOSE_ALL; break;
				case 24:
					effect = RBE_SHATTER; break;
				case 25:
					effect = RBE_EXP_10; break;
				case 26:
					effect = RBE_EXP_20; break;
				case 27:
					effect = RBE_EXP_40; break;
				case 28:
					effect = RBE_EXP_80; break;
				case 29:
					effect = RBE_DISEASE; break;
				case 30:
					effect = RBE_TIME; break;
				case 31:
					effect = RBE_SANITY; break;
				case 32:
					effect = RBE_HALLU; break;
				case 33:
					effect = RBE_PARASITE; break;
				case 34:
					effect = RBE_ABOMINATION; break;
				case 35:
					effect = RBE_RAGNAROK; break;
				case 36:
					effect = RBE_CHAOS; break;
				case 37:
					effect = RBE_SHARDS; break;
				case 38:
					effect = RBE_INERTIA; break;
				case 39:
					effect = RBE_NEXUS; break;
				case 40:
					effect = RBE_ETHER; break;
			}
		}

		/* Hack -- no more attacks */
		if (!method) break;


		/* Stop if player is dead or gone */
		if (!alive || death) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Extract visibility (before blink) */
		visible = TRUE;

#if 0

		/* Extract visibility from carrying lite */
		if (r_ptr->flags9 & RF9_HAS_LITE) visible = TRUE;

#endif /* 0 */

		/* Extract the attack "power" */
		switch (effect)
		{
		case RBE_HURT:
			power = 60;
			break;
		case RBE_POISON:
			power = 5;
			break;
		case RBE_UN_BONUS:
			power = 20;
			break;
		case RBE_UN_POWER:
			power = 15;
			break;
		case RBE_EAT_GOLD:
			power = 5;
			break;
		case RBE_EAT_ITEM:
			power = 5;
			break;
		case RBE_EAT_FOOD:
			power = 5;
			break;
		case RBE_EAT_LITE:
			power = 5;
			break;
		case RBE_ACID:
			power = 0;
			break;
		case RBE_ELEC:
			power = 10;
			break;
		case RBE_FIRE:
			power = 10;
			break;
		case RBE_COLD:
			power = 10;
			break;
		case RBE_BLIND:
			power = 2;
			break;
		case RBE_CONFUSE:
			power = 10;
			break;
		case RBE_TERRIFY:
			power = 10;
			break;
		case RBE_PARALYZE:
			power = 2;
			break;
		case RBE_LOSE_STR:
			power = 0;
			break;
		case RBE_LOSE_DEX:
			power = 0;
			break;
		case RBE_LOSE_CON:
			power = 0;
			break;
		case RBE_LOSE_INT:
			power = 0;
			break;
		case RBE_LOSE_WIS:
			power = 0;
			break;
		case RBE_LOSE_CHR:
			power = 0;
			break;
		case RBE_LOSE_ALL:
			power = 2;
			break;
		case RBE_SHATTER:
			power = 60;
			break;
		case RBE_EXP_10:
			power = 5;
			break;
		case RBE_EXP_20:
			power = 5;
			break;
		case RBE_EXP_40:
			power = 5;
			break;
		case RBE_EXP_80:
			power = 5;
			break;
		case RBE_DISEASE:
			power = 5;
			break;
		case RBE_TIME:
			power = 5;
			break;
		case RBE_CHAOS:
			power = 5;
			break;
		case RBE_SHARDS:
			power = 5;
			break;
		case RBE_INERTIA:
			power = 5;
			break;
		case RBE_NEXUS:
			power = 5;
			break;
		case RBE_ETHER:
			power = 5;
			break;
		case RBE_SANITY:
			power = 60;
			break;
		case RBE_HALLU:
			power = 10;
			break;
		case RBE_PARASITE:
			power = 5;
			break;
		case RBE_ABOMINATION:
			power = 30;
			break;
		case RBE_RAGNAROK:
			power = 60;
			break;
		}


		/* Monster hits player */
		if (!effect || check_hit(power, rlev))
		{
			/* Always disturbing */
			disturb(1, 0);

			/* Hack -- Apply "protection from evil" */
			if ((p_ptr->protevil > 0) &&
			                (r_ptr->flags3 & (RF3_EVIL)) &&
			                (p_ptr->lev >= rlev) &&
			                ((rand_int(100) + p_ptr->lev) > 50))
			{
				/* Remember the Evil-ness */
				r_ptr->r_flags3 |= (RF3_EVIL);

				/* Message */
				msg_format("%s is repelled.", sym_name);

				/* Hack -- Next attack */
				continue;
			}

			/* Hack -- Apply "protection from good" */
			if ((p_ptr->protgood > 0) &&
			                (r_ptr->flags3 & (RF3_GOOD)) &&
			                (p_ptr->lev >= rlev) &&
			                ((rand_int(100) + p_ptr->lev) > 50))
			{
				/* Remember the Good-ness */
				r_ptr->r_flags3 |= (RF3_GOOD);

				/* Message */
				msg_format("%s is repelled.", sym_name);

				/* Hack -- Next attack */
				continue;
			}

			/* Assume no cut or stun */
			do_cut = do_stun = 0;

			/* Describe the attack method */
			switch (method)
			{
			case RBM_HIT:
				{
					act = "hits you.";
					do_cut = do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

			case RBM_TOUCH:
				{
					act = "touches you.";
					touched = TRUE;
					sound(SOUND_TOUCH);
					break;
				}

			case RBM_PUNCH:
				{
					act = "punches you.";
					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

			case RBM_KICK:
				{
					act = "kicks you.";
					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

			case RBM_CLAW:
				{
					act = "claws you.";
					touched = TRUE;
					do_cut = 1;
					sound(SOUND_CLAW);
					break;
				}

			case RBM_BITE:
				{
					act = "bites you.";
					do_cut = 1;
					touched = TRUE;
					sound(SOUND_BITE);
					break;
				}

			case RBM_STING:
				{
					act = "stings you.";
					touched = TRUE;
					sound(SOUND_STING);
					break;
				}

			case RBM_FART:
				{
					if (randint(2) == 1)
						act = "produces beautiful farting noises with her sexy butt.";
					else
						act = "produces squeaky farting noises with her sexy butt.";
					/* sound(SOUND_FART); but that would require someone recording such a sound :-P --Amy */
					break;
				}

			case RBM_FARTQUIET:
				{
					if (randint(2) == 1)
						act = "produces soft farting noises with her sexy butt.";
					else
						act = "produces tender farting noises with her sexy butt.";
					/* sound(SOUND_FART); but that would require someone recording such a sound :-P --Amy */
					break;
				}

			case RBM_FARTLOUD:
				{
					if (randint(2) == 1)
						act = "produces loud farting noises with her sexy butt.";
					else
						act = "produces disgusting farting noises with her sexy butt.";
					/* sound(SOUND_FART); but that would require someone recording such a sound :-P --Amy */
					break;
				}

			case RBM_BUTT:
				{
					act = "butts you.";
					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

			case RBM_CRUSH:
				{
					act = "crushes you.";
					do_stun = 1;
					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

			case RBM_ENGULF:
				{
					act = "engulfs you.";
					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

			case RBM_CHARGE:
				{
					act = "charges you.";
					touched = TRUE;
					sound(SOUND_BUY);  /* Note! This is "charges", not "charges at". */
					break;
				}

			case RBM_CRAWL:
				{
					act = "crawls on you.";
					touched = TRUE;
					sound(SOUND_SLIME);
					break;
				}

			case RBM_DROOL:
				{
					act = "drools on you.";
					sound(SOUND_SLIME);
					break;
				}

			case RBM_SPIT:
				{
					act = "spits on you.";
					sound(SOUND_SLIME);
					break;
				}

			case RBM_EXPLODE:
				{
					act = "explodes.";
					explode = TRUE;
					break;
				}

			case RBM_GAZE:
				{
					act = "gazes at you.";
					break;
				}

			case RBM_WAIL:
				{
					act = "wails at you.";
					sound(SOUND_WAIL);
					break;
				}

			case RBM_SPORE:
				{
					act = "releases spores at you.";
					sound(SOUND_SLIME);
					break;
				}

			case RBM_BEG:
				{
					act = "begs you for money.";
					sound(SOUND_MOAN);
					break;
				}

			case RBM_INSULT:
				{
					act = desc_insult[rand_int(8)];
					sound(SOUND_MOAN);
					break;
				}

			case RBM_MOAN:
				{
					act = desc_moan[rand_int(4)];
					sound(SOUND_MOAN);
					break;
				}

			case RBM_SHOW:
				{
					if (randint(3) == 1)
						act = "sings 'We are a happy family.'";
					else
						act = "sings 'I love you, you love me.'";
					sound(SOUND_SHOW);
					break;
				}
			}

			/* Message */
			if (act) msg_format("%s %s", sym_name, act);
			if (method == RBM_FART || method == RBM_FARTLOUD || method == RBM_FARTQUIET) do_fart_effect();

			/* Hack -- assume all attacks are obvious */
			obvious = TRUE;

			/* Roll out the damage */
			damage = damroll(d_dice, d_side);
			/* full damage nastytrap: always do max damage --Amy */
			if (p_ptr->nastytrap59) damage = d_dice * d_side;

			/* Apply appropriate damage */
			switch (effect)
			{
			case 0:
				{
					/* Hack -- Assume obvious */
					obvious = TRUE;

					/* Hack -- No damage */
					damage = 0;

					break;
				}

			case RBE_HURT:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Player armor reduces total damage */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					break;
				}

			case RBE_ABOMINATION:
				{
					/* Obvious */
					obvious = TRUE;

					/* Morph, but let mimicry skill have a chance to stop this */
					if (magik(60 - get_skill_scale(SKILL_MIMICRY, 25)))
					{
						/* Message */
						cmsg_print(TERM_VIOLET, "You feel the dark powers twisting your body!");

						set_mimic(damage, resolve_mimic_name("Abomination"), 50);
					}
					else
					{
						/* Message */
						cmsg_print(TERM_VIOLET, "You feel the dark powers trying to twisting your body, but they fail.");
					}

					break;
				}

			case RBE_SANITY:
				{
					obvious = TRUE;

					int sanitydamage;
					if (damage >= 1) sanitydamage = damage / 2;
					else sanitydamage = 1;
					if (sanitydamage < 1) sanitydamage = 1;

					/* change by Amy: deal half the usual amount of sanity damage, but also deal full physical damage */
					take_hit(damage, ddesc);
					cmsg_print(TERM_VIOLET, "You're going insane!");
					take_sanity_hit(sanitydamage, ddesc);
					break;
				}

			case RBE_POISON:
				{
					/* Take some damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Take "poison" effect */
					if (!(p_ptr->resist_pois || p_ptr->immune_pois || p_ptr->oppose_pois) || p_ptr->nastytrap32)
					{
						if (set_poisoned(p_ptr->poisoned + randint(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					break;
				}

			case RBE_UN_BONUS:
				{
					/* Take some damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Allow complete resist */
					if (!p_ptr->resist_disen || p_ptr->nastytrap40)
					{
						/* Apply disenchantment */
						if (apply_disenchant(0)) obvious = TRUE;
					}

					break;
				}

			case RBE_UN_POWER:
				{
					/* Take some damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);
					break;
				}

			case RBE_EAT_GOLD:
				{
					/* Take some damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);
					break;
				}

			case RBE_EAT_ITEM:
				{
					/* Take some damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);
					break;
				}

			case RBE_EAT_FOOD:
				{
					/* Take some damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);
					break;
				}

			case RBE_EAT_LITE:
				{
					/* Take some damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);
					break;
				}

			case RBE_ACID:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
					msg_print("You are covered in acid!");

					/* Special damage */
					carried_monster_hit = TRUE;
					acid_dam(damage, ddesc);

					break;
				}

			case RBE_ELEC:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
					msg_print("You are struck by electricity!");

					/* Special damage */
					carried_monster_hit = TRUE;
					elec_dam(damage, ddesc);


					break;
				}

			case RBE_FIRE:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
					msg_print("You are enveloped in flames!");

					/* Special damage */
					carried_monster_hit = TRUE;
					fire_dam(damage, ddesc);


					break;
				}

			case RBE_COLD:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
					msg_print("You are covered with frost!");

					/* Special damage */
					carried_monster_hit = TRUE;
					cold_dam(damage, ddesc);


					break;
				}

			case RBE_BLIND:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Increase "blind" */
					if (!p_ptr->resist_blind || p_ptr->nastytrap29 || (rand_int(100) < 5) )
					{
						if (set_blind(p_ptr->blind + 10 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}


					break;
				}

			case RBE_CONFUSE:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Increase "confused" */
					if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
					{
						if (set_confused(p_ptr->confused + 3 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}


					break;
				}

			case RBE_TERRIFY:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Increase "afraid" */
					if (p_ptr->resist_fear && !p_ptr->nastytrap30 && (rand_int(100) > 4) )
					{
						msg_print("You stand your ground!");
						obvious = TRUE;
					}
					else if (rand_int(100) < player_actual_saving_throw())
					{
						msg_print("You stand your ground!");
						obvious = TRUE;
					}
					else
					{
						if (set_afraid(p_ptr->afraid + 3 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}


					break;
				}

			case RBE_PARALYZE:
				{
					/* Hack -- Prevent perma-paralysis via damage */
					if (p_ptr->paralyzed && (damage < 1)) damage = 1;

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Increase "paralyzed" */
					if (p_ptr->free_act && (rand_int(p_ptr->nastytrap57 ? 20 : 100) > 0) )
					{
						msg_print("You are unaffected!");
						obvious = TRUE;
					}
					else if (p_ptr->resist_nerve && magik(50))
					{
						msg_print("You are unaffected!");
						obvious = TRUE;
					}
					else if (rand_int(100) < player_actual_saving_throw())
					{
						msg_print("You resist the effects!");
						obvious = TRUE;
					}
					else
					{
						if (set_paralyzed(p_ptr->paralyzed + 3 + randint(sqrtrlev)))
						{
							obvious = TRUE;
						}
					}


					break;
				}

			case RBE_LOSE_STR:
				{
					/* Damage (physical) */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_STR, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_INT:
				{
					/* Damage (physical) */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_INT, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_WIS:
				{
					/* Damage (physical) */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_WIS, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_DEX:
				{
					/* Damage (physical) */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_DEX, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_CON:
				{
					/* Damage (physical) */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_CON, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_CHR:
				{
					/* Damage (physical) */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Damage (stat) */
					if (do_dec_stat(A_CHR, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_ALL:
				{
					/* Damage (physical) */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Damage (stats) */
					if (do_dec_stat(A_STR, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_DEX, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_CON, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_INT, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_WIS, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_CHR, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_SHATTER:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Reduce damage based on the player armor class */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Radius 8 earthquake centered at the monster */
					if (damage > 23)
					{
						/* Prevent destruction of quest levels and town */
						if (!is_quest(dun_level) || (is_quest(dun_level) == QUEST_RANDOM)) {
							msg_print("There is a rumbling sound!");
							earthquake(p_ptr->py, p_ptr->px, 8);
						}
					}

					break;
				}

			case RBE_EXP_10:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 95))
					{
						msg_print("You keep hold of your life force!");
					}
					else
					{
						s32b d = damroll(10, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(d / 10);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(d);
						}
					}
					break;
				}

			case RBE_EXP_20:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 90))
					{
						msg_print("You keep hold of your life force!");
					}
					else
					{
						s32b d = damroll(20, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(d / 10);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(d);
						}
					}
					break;
				}

			case RBE_EXP_40:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 75))
					{
						msg_print("You keep hold of your life force!");
					}
					else
					{
						s32b d = damroll(40, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(d / 10);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(d);
						}
					}
					break;
				}

			case RBE_EXP_80:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 50))
					{
						msg_print("You keep hold of your life force!");
					}
					else
					{
						s32b d = damroll(80, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(d / 10);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(d);
						}
					}
					break;
				}

			case RBE_DISEASE:
				{
					/* Take some damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Take "poison" effect */
					if (!(p_ptr->resist_pois || p_ptr->immune_pois || p_ptr->oppose_pois) || p_ptr->nastytrap32)
					{
						if (set_poisoned(p_ptr->poisoned + randint(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					/* Damage CON (10% chance)*/
					if (!(p_ptr->immune_pois) && (randint(100) < 11) )
					{
						/* 1% chance for perm. damage */
						bool perm = (randint(10) == 1);
						if (dec_stat(A_CON, randint(10), perm)) obvious = TRUE;
					}

					break;
				}
			case RBE_PARASITE:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Obvious */
					obvious = TRUE;

					if (!p_ptr->parasite) set_parasite(damage, r_idx);

					break;
				}
			case RBE_RAGNAROK:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Obvious */
					obvious = TRUE;

					if (randint(64) == 1) ragnarok();

					break;
				}
			case RBE_HALLU:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					/* Increase "image" */
					if (!p_ptr->resist_chaos || p_ptr->nastytrap31 || (rand_int(100) < 5) )
					{
						if (set_image(p_ptr->image + 3 + randint(rlev / 2)))
						{
							obvious = TRUE;
						}
					}
					break;

				}

			case RBE_CHAOS:
				{
					if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
					{
						(void)set_confused(p_ptr->confused + rand_int(20) + 10);
					}
					if (!p_ptr->resist_chaos || p_ptr->nastytrap31 || (rand_int(100) < 5) )
					{
						(void)set_image(p_ptr->image + randint(10));
					}
					if ((!p_ptr->resist_neth && !p_ptr->resist_chaos) || p_ptr->nastytrap95)
					{
						if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 75))
						{
							msg_print("You keep hold of your life force!");
						}
						else if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(50 + (p_ptr->exp / 500) * MON_DRAIN_LIFE);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(500 + (p_ptr->exp / 50) * MON_DRAIN_LIFE);
						}
					}
					if ((!p_ptr->resist_chaos) || p_ptr->nastytrap33 || (randint(9) == 1))
					{
						if (!p_ptr->resist_chaos || p_ptr->nastytrap33 || !p_ptr->plr_invprot) inven_damage(set_elec_destroy, 2);
						if (!p_ptr->resist_chaos || p_ptr->nastytrap33 || !p_ptr->plr_invprot) inven_damage(set_fire_destroy, 2);
					}

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					break;
				}

			case RBE_SHARDS:
				{
					if (!p_ptr->resist_shard || p_ptr->nastytrap94) {
						(void)set_cut(p_ptr->cut + damage);
					}

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					break;
				}

			case RBE_INERTIA:
				{
					int inertiadamage = damage / 4;
					if (p_ptr->resist_inertia)
					{
						inertiadamage *= 6;
						inertiadamage /= (randint(6) + 6);
					}
					if (inertiadamage < 1) inertiadamage = 1;

					if ((!p_ptr->resist_inertia) || (randint(10) == 1))
					{
						(void)set_slow(p_ptr->slow + rand_int(4) + inertiadamage);
					}

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					break;
				}

			case RBE_NEXUS:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					if (!p_ptr->resist_nexus || p_ptr->nastytrap96) {
						apply_nexus(NULL);
					}

					break;
				}

			case RBE_ETHER:
				{
					int etherdamage = damage;
					if (p_ptr->resist_ether)
					{
						etherdamage *= 6;
						etherdamage /= (randint(6) + 6);
					}
					if (etherdamage < 1) etherdamage = 1;
					contaminate(etherdamage);

					/* Take damage */
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);

					break;
				}

			case RBE_TIME:
				{
					if (!p_ptr->resist_time) {
						switch (randint(10))
						{
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								msg_print("You feel life has clocked back.");
								lose_exp(100 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
								break;
							}

						case 6:
						case 7:
						case 8:
						case 9:
							{
								int stat = rand_int(6);

								switch (stat)
								{
								case A_STR:
									act = "strong";
									break;
								case A_INT:
									act = "bright";
									break;
								case A_WIS:
									act = "wise";
									break;
								case A_DEX:
									act = "agile";
									break;
								case A_CON:
									act = "hardy";
									break;
								case A_CHR:
									act = "beautiful";
									break;
								}

								msg_format("You're not as %s as you used to be...", act);

								p_ptr->stat_cur[stat] = (p_ptr->stat_cur[stat] * 3) / 4;
								if (p_ptr->stat_cur[stat] < 3) p_ptr->stat_cur[stat] = 3;
								p_ptr->update |= (PU_BONUS);
								break;
							}

						case 10:
							{
								msg_print("You're not as powerful as you used to be...");

								for (k = 0; k < 6; k++)
								{
									p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 3) / 4;
									if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
								}
								p_ptr->update |= (PU_BONUS);
								break;
							}
						}
					}
					carried_monster_hit = TRUE;
					take_hit(damage, ddesc);
					break;
				}
			}


			/* Hack -- only one of cut or stun */
			if (do_cut && do_stun)
			{
				/* Cancel cut */
				if (rand_int(100) < 50)
				{
					do_cut = 0;
				}

				/* Cancel stun */
				else
				{
					do_stun = 0;
				}
			}

			/* Handle cut */
			if (do_cut)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
				case 0:
					k = 0;
					break;
				case 1:
					k = randint(5);
					break;
				case 2:
					k = randint(5) + 5;
					break;
				case 3:
					k = randint(20) + 20;
					break;
				case 4:
					k = randint(50) + 50;
					break;
				case 5:
					k = randint(100) + 100;
					break;
				case 6:
					k = 300;
					break;
				default:
					k = 500;
					break;
				}

				/* Apply the cut */
				if (k) (void)set_cut(p_ptr->cut + k);
			}

			/* Handle stun */
			if (do_stun)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage
				 * Amy note: holy shit those values were faaaaaaar too high! technical instadeath galore! */
				switch (tmp)
				{
				case 0:
					k = 0;
					break;
				case 1:
					k = randint(5);
					break;
				case 2:
					k = randint(10) + 10;
					break;
				case 3:
					k = randint(15) + 15;
					break;
				case 4:
					k = randint(20) + 20;
					break;
				case 5:
					k = randint(25) + 25;
					break;
				case 6:
					k = randint(30) + 30;
					break;
				default:
					k = randint(32) + 32;
					break;
				}

				/* Apply the stun */
				if (k) (void)set_stun(p_ptr->stun + k);
			}

			if (touched)
			{
				if (p_ptr->sh_fire && alive)
				{
					r_ptr->r_flags3 |= RF3_IM_FIRE;
				}

				if (p_ptr->sh_elec && alive)
				{
					r_ptr->r_flags3 |= RF3_IM_ELEC;
				}
				touched = FALSE;
			}
		}

		/* Monster missed player */
		else
		{
			/* Analyze failed attacks */
			switch (method)
			{
			case RBM_HIT:
			case RBM_TOUCH:
			case RBM_PUNCH:
			case RBM_KICK:
			case RBM_CLAW:
			case RBM_BITE:
			case RBM_STING:
			case RBM_BUTT:
			case RBM_CRUSH:
			case RBM_ENGULF:
			case RBM_CHARGE:

				/* Disturbing */
				disturb(1, 0);

				/* Message */
				msg_format("%s misses you.", sym_name);

				break;
			}
		}


		/* Analyze "visible" monsters only */
		if (visible)
		{
			/* Count "obvious" attacks (and ones that cause damage) */
			if (obvious || damage || (r_ptr->r_blows[ap_cnt] > 10))
			{
				/* Count attacks of this type */
				if (r_ptr->r_blows[ap_cnt] < MAX_UCHAR)
				{
					r_ptr->r_blows[ap_cnt]++;
				}
			}
		}
	}
	/* Assume we attacked */
	return (TRUE);
}

/*
 * Give unprotected player the Black Breath with a 1 in (chance) probability
 *
 */
void black_breath_attack(int chance)
{
	if (!p_ptr->protundead && randint(chance) == 1)
	{
		 msg_print("Your foe calls upon your soul!");
		 msg_print("You feel the Black Breath slowly draining you of life...");
		 p_ptr->black_breath = TRUE;
	}
}

/*
 * Attack the player via physical attacks.
 */
bool make_attack_normal(int m_idx, byte divis)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = race_inf(m_ptr);

	int ap_cnt;

	int i, j, k, tmp, ac, rlev, sqrtrlev;
	int do_cut, do_stun, do_vampire;

	s32b gold;

	object_type *o_ptr;

	char o_name[80];

	char m_name[80];

	char ddesc[80];

	bool blinked;
	bool touched = FALSE, fear = FALSE, alive = TRUE;
	bool explode = FALSE;

	/* Not allowed to attack */
	if (r_ptr->flags1 & (RF1_NEVER_BLOW)) return (FALSE);

	/* ...nor if friendly */
	if (is_friend(m_ptr) >= 0)
	{
		if (p_ptr->control == m_idx) swap_position_ignorelos(m_ptr->fy, m_ptr->fx);
		return FALSE;
	}

	/* Cannot attack the player if mortal and player fated to never die by the ... */
	/*if ((r_ptr->flags7 & RF7_MORTAL) && (p_ptr->no_mortal)) return (FALSE);*/

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Extract the effective monster level */
	rlev = ((m_ptr->level >= 1) ? m_ptr->level : 1);
	sqrtrlev = rlev;
	if (sqrtrlev > 1) {
		sqrtrlev = sroot(rlev);
		if (sqrtrlev < 1) sqrtrlev = 1;
	}

	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/* Get the "died from" information (i.e. "a kobold") */
	monster_desc(ddesc, m_ptr, 0x88);


	/* Assume no blink */
	blinked = FALSE;

	/* Scan through all four blows */
	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		bool visible = FALSE;
		bool obvious = FALSE;

		int power = 0;
		int damage = 0;

		cptr act = NULL;

		/* Extract the attack infomation */
		int effect = m_ptr->blow[ap_cnt].effect;
		int method = m_ptr->blow[ap_cnt].method;
		int d_dice = m_ptr->blow[ap_cnt].d_dice;
		int d_side = m_ptr->blow[ap_cnt].d_side;

		/* secret attack nastytrap by Amy */
		if (p_ptr->nastytrap167 && magik(2) && (effect == RBE_HURT)) {
			switch (randint(40)) {
				default:
				case 1:
					effect = RBE_HURT; break;
				case 2:
					effect = RBE_POISON; break;
				case 3:
					effect = RBE_UN_BONUS; break;
				case 4:
					effect = RBE_UN_POWER; break;
				case 5:
					effect = RBE_EAT_GOLD; break;
				case 6:
					effect = RBE_EAT_LITE; break;
				case 7:
					effect = RBE_EAT_ITEM; break;
				case 8:
					effect = RBE_EAT_FOOD; break;
				case 9:
					effect = RBE_ACID; break;
				case 10:
					effect = RBE_ELEC; break;
				case 11:
					effect = RBE_FIRE; break;
				case 12:
					effect = RBE_COLD; break;
				case 13:
					effect = RBE_BLIND; break;
				case 14:
					effect = RBE_CONFUSE; break;
				case 15:
					effect = RBE_TERRIFY; break;
				case 16:
					effect = RBE_PARALYZE; break;
				case 17:
					effect = RBE_LOSE_STR; break;
				case 18:
					effect = RBE_LOSE_INT; break;
				case 19:
					effect = RBE_LOSE_DEX; break;
				case 20:
					effect = RBE_LOSE_CON; break;
				case 21:
					effect = RBE_LOSE_CHR; break;
				case 22:
					effect = RBE_LOSE_WIS; break;
				case 23:
					effect = RBE_LOSE_ALL; break;
				case 24:
					effect = RBE_SHATTER; break;
				case 25:
					effect = RBE_EXP_10; break;
				case 26:
					effect = RBE_EXP_20; break;
				case 27:
					effect = RBE_EXP_40; break;
				case 28:
					effect = RBE_EXP_80; break;
				case 29:
					effect = RBE_DISEASE; break;
				case 30:
					effect = RBE_TIME; break;
				case 31:
					effect = RBE_SANITY; break;
				case 32:
					effect = RBE_HALLU; break;
				case 33:
					effect = RBE_PARASITE; break;
				case 34:
					effect = RBE_ABOMINATION; break;
				case 35:
					effect = RBE_RAGNAROK; break;
				case 36:
					effect = RBE_CHAOS; break;
				case 37:
					effect = RBE_SHARDS; break;
				case 38:
					effect = RBE_INERTIA; break;
				case 39:
					effect = RBE_NEXUS; break;
				case 40:
					effect = RBE_ETHER; break;
			}
		}

		/* Hack -- no more attacks */
		if (!method) break;


		/* Stop if player is dead or gone */
		if (!alive || death) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Extract visibility (before blink) */
		if (m_ptr->ml) visible = TRUE;

#if 0

		/* Extract visibility from carrying lite */
		if (r_ptr->flags9 & RF9_HAS_LITE) visible = TRUE;

#endif

		/* Extract the attack "power" */
		switch (effect)
		{
		case RBE_HURT:
			power = 60;
			break;
		case RBE_POISON:
			power = 5;
			break;
		case RBE_UN_BONUS:
			power = 20;
			break;
		case RBE_UN_POWER:
			power = 15;
			break;
		case RBE_EAT_GOLD:
			power = 5;
			break;
		case RBE_EAT_ITEM:
			power = 5;
			break;
		case RBE_EAT_FOOD:
			power = 5;
			break;
		case RBE_EAT_LITE:
			power = 5;
			break;
		case RBE_ACID:
			power = 0;
			break;
		case RBE_ELEC:
			power = 10;
			break;
		case RBE_FIRE:
			power = 10;
			break;
		case RBE_COLD:
			power = 10;
			break;
		case RBE_BLIND:
			power = 2;
			break;
		case RBE_CONFUSE:
			power = 10;
			break;
		case RBE_TERRIFY:
			power = 10;
			break;
		case RBE_PARALYZE:
			power = 2;
			break;
		case RBE_LOSE_STR:
			power = 0;
			break;
		case RBE_LOSE_DEX:
			power = 0;
			break;
		case RBE_LOSE_CON:
			power = 0;
			break;
		case RBE_LOSE_INT:
			power = 0;
			break;
		case RBE_LOSE_WIS:
			power = 0;
			break;
		case RBE_LOSE_CHR:
			power = 0;
			break;
		case RBE_LOSE_ALL:
			power = 2;
			break;
		case RBE_SHATTER:
			power = 60;
			break;
		case RBE_EXP_10:
			power = 5;
			break;
		case RBE_EXP_20:
			power = 5;
			break;
		case RBE_EXP_40:
			power = 5;
			break;
		case RBE_EXP_80:
			power = 5;
			break;
		case RBE_DISEASE:
			power = 5;
			break;
		case RBE_TIME:
			power = 5;
			break;
		case RBE_CHAOS:
			power = 5;
			break;
		case RBE_SHARDS:
			power = 5;
			break;
		case RBE_INERTIA:
			power = 5;
			break;
		case RBE_NEXUS:
			power = 5;
			break;
		case RBE_ETHER:
			power = 5;
			break;
		case RBE_SANITY:
			power = 60;
			break;
		case RBE_HALLU:
			power = 10;
			break;
		case RBE_PARASITE:
			power = 5;
			break;
		case RBE_ABOMINATION:
			power = 20;
			break;
		case RBE_RAGNAROK:
			power = 60;
			break;
		}


		/* Monster hits player */
		if (!effect || check_hit(power, rlev))
		{
			int chance = p_ptr->dodge_chance - ((rlev * 5) / 6);

			/* Always disturbing */
			disturb(1, 0);

			if ((chance > 0) && magik(chance) && (rand_int(8) > 0) ) /* minimum failure rate --Amy */
			{
				msg_format("You dodge %s attack!", m_name);
				continue;
			}

			/* Eru can help you */
			PRAY_GOD(GOD_ERU)
			{
				s32b chance = p_ptr->grace;

				if (chance > 50000) chance = 50000;
				chance -= rlev * 300;

				if ((randint(100000) < chance) && (r_ptr->flags3 & (RF3_EVIL)))
				{
					/* Remember the Evil-ness */
					r_ptr->r_flags3 |= (RF3_EVIL);

					/* Message */
					msg_format("The hand of Eru Iluvatar stops %s blow.", m_name);

					/* Hack -- Next attack */
					continue;
				}
			}

			/* Hack -- Apply "protection from evil" */
			if ((p_ptr->protevil > 0) &&
			                (r_ptr->flags3 & (RF3_EVIL)) &&
			                (p_ptr->lev >= rlev) &&
			                ((rand_int(100) + p_ptr->lev) > 50))
			{
				/* Remember the Evil-ness */
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= (RF3_EVIL);
				}

				/* Message */
				msg_format("%^s is repelled.", m_name);

				/* Hack -- Next attack */
				continue;
			}

			/* Hack -- Apply "protection from good" */
			if ((p_ptr->protgood > 0) &&
			                (r_ptr->flags3 & (RF3_GOOD)) &&
			                (p_ptr->lev >= rlev) &&
			                ((rand_int(100) + p_ptr->lev) > 50))
			{
				/* Remember the Good-ness */
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= (RF3_GOOD);
				}

				/* Message */
				msg_format("%^s is repelled.", m_name);

				/* Hack -- Next attack */
				continue;
			}

			/* Assume no cut or stun */
			do_cut = do_stun = do_vampire = 0;

			/* Describe the attack method */
			switch (method)
			{
			case RBM_HIT:
				{
					act = "hits you.";
					do_cut = do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

			case RBM_TOUCH:
				{
					act = "touches you.";
					touched = TRUE;
					sound(SOUND_TOUCH);
					break;
				}

			case RBM_PUNCH:
				{
					act = "punches you.";
					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

			case RBM_KICK:
				{
					act = "kicks you.";
					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

			case RBM_CLAW:
				{
					act = "claws you.";
					touched = TRUE;
					do_cut = 1;
					sound(SOUND_CLAW);
					break;
				}

			case RBM_BITE:
				{
					act = "bites you.";
					do_cut = 1;
					if (magik(5) && (strstr(r_name + r_ptr->name, "Vampire") || strstr(r_name + r_ptr->name, "vampire")))
						do_vampire = TRUE;
					touched = TRUE;
					sound(SOUND_BITE);
					break;
				}

			case RBM_STING:
				{
					act = "stings you.";
					touched = TRUE;
					sound(SOUND_STING);
					break;
				}

			case RBM_FART:
				{
					if (randint(2) == 1)
						act = "produces beautiful farting noises with her sexy butt.";
					else
						act = "produces squeaky farting noises with her sexy butt.";
					/* sound(SOUND_FART); but that would require someone recording such a sound :-P --Amy */
					break;
				}

			case RBM_FARTQUIET:
				{
					if (randint(2) == 1)
						act = "produces soft farting noises with her sexy butt.";
					else
						act = "produces tender farting noises with her sexy butt.";
					/* sound(SOUND_FART); but that would require someone recording such a sound :-P --Amy */
					break;
				}

			case RBM_FARTLOUD:
				{
					if (randint(2) == 1)
						act = "produces loud farting noises with her sexy butt.";
					else
						act = "produces disgusting farting noises with her sexy butt.";
					/* sound(SOUND_FART); but that would require someone recording such a sound :-P --Amy */
					break;
				}

			case RBM_BUTT:
				{
					act = "butts you.";
					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

			case RBM_CRUSH:
				{
					act = "crushes you.";
					do_stun = 1;
					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

			case RBM_ENGULF:
				{
					act = "engulfs you.";
					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

			case RBM_CHARGE:
				{
					act = "charges you.";
					touched = TRUE;
					sound(SOUND_BUY);  /* Note! This is "charges", not "charges at". */
					break;
				}

			case RBM_CRAWL:
				{
					act = "crawls on you.";
					touched = TRUE;
					sound(SOUND_SLIME);
					break;
				}

			case RBM_DROOL:
				{
					act = "drools on you.";
					sound(SOUND_SLIME);
					break;
				}

			case RBM_SPIT:
				{
					act = "spits on you.";
					sound(SOUND_SLIME);
					break;
				}

			case RBM_EXPLODE:
				{
					act = "explodes.";
					explode = TRUE;
					break;
				}

			case RBM_GAZE:
				{
					act = "gazes at you.";
					break;
				}

			case RBM_WAIL:
				{
					act = "wails at you.";
					sound(SOUND_WAIL);
					break;
				}

			case RBM_SPORE:
				{
					act = "releases spores at you.";
					sound(SOUND_SLIME);
					break;
				}

			case RBM_BEG:
				{
					act = "begs you for money.";
					sound(SOUND_MOAN);
					break;
				}

			case RBM_INSULT:
				{
					act = desc_insult[rand_int(8)];
					sound(SOUND_MOAN);
					break;
				}

			case RBM_MOAN:
				{
					if (strstr((r_name + r_ptr->name), "Mathilde, the Science Student"))
						act = desc_moan[rand_int(3) + 4];
					else
						act = desc_moan[rand_int(4)];
					sound(SOUND_MOAN);
					break;
				}

			case RBM_SHOW:
				{
					if (randint(3) == 1)
						act = "sings 'We are a happy family.'";
					else
						act = "sings 'I love you, you love me.'";
					sound(SOUND_SHOW);
					break;
				}
			}

			/* Message */
			if (act) msg_format("%^s %s", m_name, act);
			if (method == RBM_FART || method == RBM_FARTLOUD || method == RBM_FARTQUIET) do_fart_effect();

			/* The undead can give the player the Black Breath with
			 * a successful blow. Uniques have a better chance. -LM-
			 * Nazgul have a 25% chance
			 */
			if (r_ptr->flags7 & RF7_NAZGUL)
			{
				black_breath_attack(4);
			}
			else if ((m_ptr->level >= 35) && (r_ptr->flags3 & (RF3_UNDEAD)) &&
					    (r_ptr->flags1 & (RF1_UNIQUE)))
			{
				black_breath_attack(300 - m_ptr->level);
			}
			else if ((m_ptr->level >= 40) && (r_ptr->flags3 & (RF3_UNDEAD)))
			{
				black_breath_attack(450 - m_ptr->level);
			}

			/* Amberites can cause the "curse of amber" (TY curse) */
			if ((r_ptr->flags7 & RF7_AMBERITE) && magik(25) )
			{
				msg_format("Oh no, %s puts a blood curse on you!", m_name);
				activate_ty_curse();
			}

			/* Hack -- assume all attacks are obvious */
			obvious = TRUE;

			/* Roll out the damage */
			damage = damroll(d_dice, d_side);
			/* full damage nastytrap: always do max damage --Amy */
			if (p_ptr->nastytrap59) damage = d_dice * d_side;

			/* Sometime reduce the damage */
			damage /= divis;

			/* Apply appropriate damage */
			switch (effect)
			{
			case 0:
				{
					/* Hack -- Assume obvious */
					obvious = TRUE;

					/* Hack -- No damage */
					damage = 0;

					break;
				}

			case RBE_HURT:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Player armor reduces total damage */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					break;
				}

			case RBE_ABOMINATION:
				{
					/* Obvious */
					obvious = TRUE;

					/* Morph, but let mimicry skill have a chance to stop this */
					if (magik(60 - get_skill_scale(SKILL_MIMICRY, 25)))
					{
						/* Message */
						cmsg_print(TERM_VIOLET, "You feel the dark powers twisting your body!");

						set_mimic(damage, resolve_mimic_name("Abomination"), 50);
					}
					else
					{
						/* Message */
						cmsg_print(TERM_VIOLET, "You feel the dark powers trying to twisting your body, but they fail.");
					}

					break;
				}

			case RBE_SANITY:
				{
					obvious = TRUE;

					int sanitydamage;
					if (damage >= 1) sanitydamage = damage / 2;
					else sanitydamage = 1;
					if (sanitydamage < 1) sanitydamage = 1;

					/* change by Amy: deal half the usual amount of sanity damage, but also deal full physical damage */
					take_hit(damage, ddesc);
					cmsg_print(TERM_VIOLET, "You're going insane!");
					take_sanity_hit(sanitydamage, ddesc);
					break;
				}

			case RBE_POISON:
				{
					/* Take some damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Take "poison" effect */
					if (!(p_ptr->resist_pois || p_ptr->immune_pois || p_ptr->oppose_pois) || p_ptr->nastytrap32)
					{
						if (set_poisoned(p_ptr->poisoned + randint(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_POIS);

					break;
				}

			case RBE_UN_BONUS:
				{
					/* Take some damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Allow complete resist */
					if (!p_ptr->resist_disen || p_ptr->nastytrap40)
					{
						/* Apply disenchantment */
						if (apply_disenchant(0)) obvious = TRUE;
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_DISEN);

					break;
				}

			case RBE_UN_POWER:
				{
					/* Take some damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

						/* Pick an item */
						i = rand_int(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &p_ptr->inventory[i];

						object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

						/* charge holding allows item to resist 90% of the time --Amy */
						if ((f5 & (TR5_CHARGE_HOLDING)) && randint(10) != 1) continue;

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Drain charged wands/staffs
						   Hack -- don't let artifacts get drained */
						if (((o_ptr->tval == TV_STAFF) || (o_ptr->tval == TV_ROD_MAIN) ||
						                (o_ptr->tval == TV_WAND)) &&
						                (o_ptr->pval) &&
					                     !artifact_p(o_ptr))
						{
							int drainedamount;
							int drainmitigate;

							if (o_ptr->tval == TV_ROD_MAIN) {

								/* Heal */
								j = rlev;
								m_ptr->hp += j * o_ptr->timeout * o_ptr->number;
								if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

								/* Redraw (later) if needed */
								if (health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);

								/* Uncharge, changed by Amy to be less BS */

								drainedamount = randint(o_ptr->timeout) + 1;
								if (drainedamount > o_ptr->timeout) drainedamount = o_ptr->timeout;
								drainmitigate = get_skill_scale(SKILL_FRESHMINT, 20);
								if (drainmitigate > 0) drainedamount -= randint(drainmitigate);
								if (drainedamount < 1) drainedamount = 1;

								if (p_ptr->nastytrap71) drainedamount = o_ptr->timeout;

								o_ptr->timeout -= drainedamount;
								if (o_ptr->timeout < 0) o_ptr->timeout = 0;

								/* Combine / Reorder the pack */
								p_ptr->notice |= (PN_COMBINE | PN_REORDER);

								/* Window stuff */
								p_ptr->window |= (PW_INVEN);

							} else {
								/* Heal */
								j = rlev;
								m_ptr->hp += j * o_ptr->pval * o_ptr->number;
								if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

								/* Redraw (later) if needed */
								if (health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);

								/* Uncharge, changed by Amy to be less BS */

								drainedamount = randint(o_ptr->pval) + 1;
								if (drainedamount > o_ptr->pval) drainedamount = o_ptr->pval;
								drainmitigate = get_skill_scale(SKILL_FRESHMINT, 20);
								if (drainmitigate > 0) drainedamount -= randint(drainmitigate);
								if (drainedamount < 1) drainedamount = 1;

								if (p_ptr->nastytrap71) drainedamount = o_ptr->pval;

								o_ptr->pval -= drainedamount;
								if (o_ptr->pval < 0) o_ptr->pval = 0;

								/* Combine / Reorder the pack */
								p_ptr->notice |= (PN_COMBINE | PN_REORDER);

								/* Window stuff */
								p_ptr->window |= (PW_INVEN);

							}

							/* Message */
							msg_print("Energy drains from your pack!");

							/* Obvious */
							obvious = TRUE;


							/* Done */
							break;
						}
					}

					break;
				}

			case RBE_EAT_GOLD:
				{
					/* Take some damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Obvious */
					obvious = TRUE;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!p_ptr->paralyzed && !p_ptr->nastytrap63 && (rand_int(10) > 0) &&
					                (rand_int(100) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
					                                  p_ptr->lev - m_ptr->level)))
					{
						/* Saving throw message */
						msg_print("You quickly protect your money pouch!");

						/* Occasional blink anyway */
						if (rand_int(3)) blinked = TRUE;
					}

					/* Eat gold */
					else
					{
						gold = (p_ptr->au / 10) + randint(25);
						if (gold < 2) gold = 2;
						if (gold > 5000) gold = (p_ptr->au / 20) + randint(3000);
						if (gold > p_ptr->au) gold = p_ptr->au;
						p_ptr->au -= gold;
						if (gold <= 0)
						{
							msg_print("Nothing was stolen.");
						}
						else if (p_ptr->au)
						{
							msg_print("Your purse feels lighter.");
							msg_format("%ld coins were stolen!", (long)gold);
						}
						else
						{
							msg_print("Your purse feels lighter.");
							msg_print("All of your coins were stolen!");
						}

						while (gold > 0)
						{
							object_type forge, *j_ptr = &forge;

							/* Wipe the object */
							object_wipe(j_ptr);

							/* Prepare a gold object */
							object_prep(j_ptr, lookup_kind(TV_GOLD, 9));

							/* Determine how much the treasure is "worth" */
							j_ptr->pval = (gold >= 15000) ? 15000 : gold;

							monster_carry(m_ptr, m_idx, j_ptr);

							gold -= 15000;
						}

						/* Redraw gold */
						p_ptr->redraw |= (PR_GOLD);

						/* Window stuff */
						p_ptr->window |= (PW_PLAYER);

						/* Blink away */
						blinked = TRUE;
					}

					break;
				}

			case RBE_EAT_ITEM:
				{
					/* Take some damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!p_ptr->paralyzed && !p_ptr->nastytrap63 && (rand_int(10) > 0) &&
					                (rand_int(100) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
					                                  p_ptr->lev - m_ptr->level)))
					{
						/* Saving throw message */
						msg_print("You grab hold of your backpack!");

						/* Occasional "blink" anyway */
						blinked = TRUE;

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item */
						i = rand_int(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &p_ptr->inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip artifacts */
						if (artifact_p(o_ptr) || o_ptr->art_name) continue;

						/* Get a description */
						object_desc(o_name, o_ptr, FALSE, 3);

						/* Message */
						msg_format("%sour %s (%c) was stolen!",
						           ((o_ptr->number > 1) ? "One of y" : "Y"),
						           o_name, index_to_label(i));

						/* Option */
						if (testing_carry)
						{
							s16b o_idx;

							/* Make an object */
							o_idx = o_pop();

							/* Success */
							if (o_idx)
							{
								object_type *j_ptr;

								/* Get new object */
								j_ptr = &o_list[o_idx];

								/* Copy object */
								object_copy(j_ptr, o_ptr);

								/* Modify number */
								j_ptr->number = 1;

								/* Hack -- If a wand, allocate total
								 * maximum timeouts or charges between those
								 * stolen and those missed. -LM-
								 */
								if (o_ptr->tval == TV_WAND)
								{
									j_ptr->pval = o_ptr->pval / o_ptr->number;
									o_ptr->pval -= j_ptr->pval;
								}

								/* Forget mark */
								j_ptr->marked = FALSE;

								/* Memorize monster */
								j_ptr->held_m_idx = m_idx;

								/* Build stack */
								j_ptr->next_o_idx = m_ptr->hold_o_idx;

								/* Build stack */
								m_ptr->hold_o_idx = o_idx;
							}
						}
						else
						{
							if (strstr((r_name + r_ptr->name), "black market")
							                && randint(2) != 1)
							{
								s16b o_idx;

								/* Make an object */
								o_idx = o_pop();

								/* Success */
								if (o_idx)
								{
									object_type *j_ptr;
									if (cheat_xtra || cheat_peek)
										msg_print("Moving object to black market...");

									/* Get new object */
									j_ptr = &o_list[o_idx];

									/* Copy object */
									object_copy(j_ptr, o_ptr);

									/* Modify number */
									j_ptr->number = 1;

									/* Forget mark */
									j_ptr->marked = FALSE;

									move_to_black_market(j_ptr);
								}
							}
						}

						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Blink away */
						blinked = TRUE;

						/* Done */
						break;
					}

					break;
				}

			case RBE_EAT_FOOD:
				{
					/* Take some damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Steal some food */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item from the pack */
						i = rand_int(INVEN_PACK);

						/* Get the item */
						o_ptr = &p_ptr->inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip non-food objects */
						if (o_ptr->tval != TV_FOOD && o_ptr->tval != TV_CORPSE && o_ptr->tval != TV_EGG) continue;

						/* Get a description */
						object_desc(o_name, o_ptr, FALSE, 0);

						/* Message */
						msg_format("%sour %s (%c) was eaten!",
						           ((o_ptr->number > 1) ? "One of y" : "Y"),
						           o_name, index_to_label(i));

						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					break;
				}

			case RBE_EAT_LITE:
				{
					/* Take some damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Access the lite */
					o_ptr = &p_ptr->inventory[INVEN_LITE];

					/* Drain fuel */
					if (o_ptr->timeout > 0)
					{
						/* Reduce fuel */
						o_ptr->timeout -= (250 + randint(250));
						if (o_ptr->timeout < 1) o_ptr->timeout = 1;

						/* Notice */
						if (!p_ptr->blind)
						{
							msg_print("Your light dims.");
							obvious = TRUE;
						}

						/* Window stuff */
						p_ptr->window |= (PW_EQUIP);
					}

					break;
				}

			case RBE_ACID:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
					msg_print("You are covered in acid!");

					/* Special damage */
					acid_dam(damage, ddesc);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_ACID);

					break;
				}

			case RBE_ELEC:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
					msg_print("You are struck by electricity!");

					/* Special damage */
					elec_dam(damage, ddesc);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_ELEC);

					break;
				}

			case RBE_FIRE:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
					msg_print("You are enveloped in flames!");

					/* Special damage */
					fire_dam(damage, ddesc);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FIRE);

					break;
				}

			case RBE_COLD:
				{
					/* Obvious */
					obvious = TRUE;

					/* Message */
					msg_print("You are covered with frost!");

					/* Special damage */
					cold_dam(damage, ddesc);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_COLD);

					break;
				}

			case RBE_BLIND:
				{
					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Increase "blind" */
					if (!p_ptr->resist_blind || p_ptr->nastytrap29 || (rand_int(100) < 5) )
					{
						if (set_blind(p_ptr->blind + 10 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_BLIND);

					break;
				}

			case RBE_CONFUSE:
				{
					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Increase "confused" */
					if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
					{
						if (set_confused(p_ptr->confused + 3 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_CONF);

					break;
				}

			case RBE_TERRIFY:
				{
					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Increase "afraid" */
					if (p_ptr->resist_fear && !p_ptr->nastytrap30 && (rand_int(100) > 4) )
					{
						msg_print("You stand your ground!");
						obvious = TRUE;
					}
					else if (rand_int(100) < player_actual_saving_throw())
					{
						msg_print("You stand your ground!");
						obvious = TRUE;
					}
					else
					{
						if (set_afraid(p_ptr->afraid + 3 + randint(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FEAR);

					break;
				}

			case RBE_PARALYZE:
				{
					/* Hack -- Prevent perma-paralysis via damage */
					if (p_ptr->paralyzed && (damage < 1)) damage = 1;

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Increase "paralyzed" */
					if (p_ptr->free_act && (rand_int(p_ptr->nastytrap57 ? 20 : 100) > 0) )
					{
						msg_print("You are unaffected!");
						obvious = TRUE;
					}
					else if (p_ptr->resist_nerve && magik(50))
					{
						msg_print("You are unaffected!");
						obvious = TRUE;
					}
					else if (rand_int(100) < player_actual_saving_throw())
					{
						msg_print("You resist the effects!");
						obvious = TRUE;
					}
					else
					{
						if (set_paralyzed(p_ptr->paralyzed + 3 + randint(sqrtrlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FREE);

					break;
				}

			case RBE_LOSE_STR:
				{
					/* Damage (physical) */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					lifesave_no_mortal = FALSE;
					if (!death) lifesave_no_mortal = FALSE;

					/* Damage (stat) */
					if (do_dec_stat(A_STR, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_INT:
				{
					/* Damage (physical) */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Damage (stat) */
					if (do_dec_stat(A_INT, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_WIS:
				{
					/* Damage (physical) */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Damage (stat) */
					if (do_dec_stat(A_WIS, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_DEX:
				{
					/* Damage (physical) */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Damage (stat) */
					if (do_dec_stat(A_DEX, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_CON:
				{
					/* Damage (physical) */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Damage (stat) */
					if (do_dec_stat(A_CON, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_CHR:
				{
					/* Damage (physical) */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Damage (stat) */
					if (do_dec_stat(A_CHR, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_LOSE_ALL:
				{
					/* Damage (physical) */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Damage (stats) */
					if (do_dec_stat(A_STR, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_DEX, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_CON, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_INT, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_WIS, STAT_DEC_NORMAL)) obvious = TRUE;
					if (do_dec_stat(A_CHR, STAT_DEC_NORMAL)) obvious = TRUE;

					break;
				}

			case RBE_SHATTER:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Reduce damage based on the player armor class */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Radius 8 earthquake centered at the monster */
					if (damage > 23)
					{
						/* Prevent destruction of quest levels and town */
						if (!is_quest(dun_level) || (is_quest(dun_level) == QUEST_RANDOM)) {
							msg_print("There is a rumbling sound!");
							earthquake(m_ptr->fy, m_ptr->fx, 8);
						}
					}

					break;
				}

			case RBE_EXP_10:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 95))
					{
						msg_print("You keep hold of your life force!");
					}
					else
					{
						s32b d = damroll(10, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(d / 10);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(d);
						}
					}
					break;
				}

			case RBE_EXP_20:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 90))
					{
						msg_print("You keep hold of your life force!");
					}
					else
					{
						s32b d = damroll(20, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(d / 10);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(d);
						}
					}
					break;
				}

			case RBE_EXP_40:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 75))
					{
						msg_print("You keep hold of your life force!");
					}
					else
					{
						s32b d = damroll(40, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(d / 10);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(d);
						}
					}
					break;
				}

			case RBE_EXP_80:
				{
					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 50))
					{
						msg_print("You keep hold of your life force!");
					}
					else
					{
						s32b d = damroll(80, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
						if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(d / 10);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(d);
						}
					}
					break;
				}

			case RBE_DISEASE:
				{
					/* Take some damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Take "poison" effect */
					if (!(p_ptr->resist_pois || p_ptr->immune_pois || p_ptr->oppose_pois) || p_ptr->nastytrap32)
					{
						if (set_poisoned(p_ptr->poisoned + randint(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					/* Damage CON (10% chance)*/
					if (!(p_ptr->immune_pois) && (randint(100) < 11) )
					{
						/* 1% chance for perm. damage */
						bool perm = (randint(10) == 1);
						if (dec_stat(A_CON, randint(10), perm)) obvious = TRUE;
					}

					break;
				}
			case RBE_HALLU:
				{
					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Increase "image" */
					if (!p_ptr->resist_chaos || p_ptr->nastytrap31 || (rand_int(100) < 5) )
					{
						if (set_image(p_ptr->image + 3 + randint(rlev / 2)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_CHAOS);

					break;

				}

			case RBE_CHAOS:
				{
					if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
					{
						(void)set_confused(p_ptr->confused + rand_int(20) + 10);
					}
					if (!p_ptr->resist_chaos || p_ptr->nastytrap31 || (rand_int(100) < 5) )
					{
						(void)set_image(p_ptr->image + randint(10));
					}
					if ((!p_ptr->resist_neth && !p_ptr->resist_chaos) || p_ptr->nastytrap95)
					{
						if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 75))
						{
							msg_print("You keep hold of your life force!");
						}
						else if (p_ptr->hold_life && !p_ptr->nastytrap95)
						{
							msg_print("You feel your life slipping away!");
							lose_exp(50 + (p_ptr->exp / 500) * MON_DRAIN_LIFE);
						}
						else
						{
							msg_print("You feel your life draining away!");
							lose_exp(500 + (p_ptr->exp / 50) * MON_DRAIN_LIFE);
						}
					}
					if ((!p_ptr->resist_chaos) || p_ptr->nastytrap33 || (randint(9) == 1))
					{
						if (!p_ptr->resist_chaos || p_ptr->nastytrap33 || !p_ptr->plr_invprot) inven_damage(set_elec_destroy, 2);
						if (!p_ptr->resist_chaos || p_ptr->nastytrap33 || !p_ptr->plr_invprot) inven_damage(set_fire_destroy, 2);
					}

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					break;
				}

			case RBE_SHARDS:
				{
					if (!p_ptr->resist_shard || p_ptr->nastytrap94) {
						(void)set_cut(p_ptr->cut + damage);
					}

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					break;
				}

			case RBE_INERTIA:
				{
					int inertiadamage = damage / 4;
					if (p_ptr->resist_inertia)
					{
						inertiadamage *= 6;
						inertiadamage /= (randint(6) + 6);
					}
					if (inertiadamage < 1) inertiadamage = 1;

					if ((!p_ptr->resist_inertia) || (randint(10) == 1))
					{
						(void)set_slow(p_ptr->slow + rand_int(4) + inertiadamage);
					}

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					break;
				}

			case RBE_NEXUS:
				{
					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					if (!p_ptr->resist_nexus || p_ptr->nastytrap96) {
						apply_nexus(NULL);
					}

					break;
				}

			case RBE_ETHER:
				{
					int etherdamage = damage;
					if (p_ptr->resist_ether)
					{
						etherdamage *= 6;
						etherdamage /= (randint(6) + 6);
					}
					if (etherdamage < 1) etherdamage = 1;
					contaminate(etherdamage);

					/* Take damage */
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					break;
				}

			case RBE_TIME:
				{
					if (!p_ptr->resist_time) {
						switch (randint(10))
						{
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							{
								msg_print("You feel life has clocked back.");
								lose_exp(100 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
								break;
							}

						case 6:
						case 7:
						case 8:
						case 9:
							{
								int stat = rand_int(6);

								switch (stat)
								{
								case A_STR:
									act = "strong";
									break;
								case A_INT:
									act = "bright";
									break;
								case A_WIS:
									act = "wise";
									break;
								case A_DEX:
									act = "agile";
									break;
								case A_CON:
									act = "hardy";
									break;
								case A_CHR:
									act = "beautiful";
									break;
								}

								msg_format("You're not as %s as you used to be...", act);

								p_ptr->stat_cur[stat] = (p_ptr->stat_cur[stat] * 3) / 4;
								if (p_ptr->stat_cur[stat] < 3) p_ptr->stat_cur[stat] = 3;
								p_ptr->update |= (PU_BONUS);
								break;
							}

						case 10:
							{
								msg_print("You're not as powerful as you used to be...");

								for (k = 0; k < 6; k++)
								{
									p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 3) / 4;
									if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
								}
								p_ptr->update |= (PU_BONUS);
								break;
							}
						}
					}
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;
					break;
				}
			case RBE_PARASITE:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Obvious */
					obvious = TRUE;

					if (!p_ptr->parasite) set_parasite(damage, m_ptr->r_idx);

					break;
				}
			case RBE_RAGNAROK:
				{
					/* Take damage */
					carried_monster_hit = TRUE;
					if (r_ptr->flags7 & RF7_MORTAL) lifesave_no_mortal = TRUE;
					take_hit(damage, ddesc);
					if (!death) lifesave_no_mortal = FALSE;

					/* Obvious */
					obvious = TRUE;

					if (randint(64) == 1) ragnarok();

					break;
				}

			} /* type of melee attack loop end */

			/* Hack -- only one of cut or stun */
			if (do_cut && do_stun)
			{
				/* Cancel cut */
				if (rand_int(100) < 50)
				{
					do_cut = 0;
				}

				/* Cancel stun */
				else
				{
					do_stun = 0;
				}
			}

			/* Handle cut */
			if (do_cut)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
				case 0:
					k = 0;
					break;
				case 1:
					k = randint(5);
					break;
				case 2:
					k = randint(5) + 5;
					break;
				case 3:
					k = randint(20) + 20;
					break;
				case 4:
					k = randint(50) + 50;
					break;
				case 5:
					k = randint(100) + 100;
					break;
				case 6:
					k = 300;
					break;
				default:
					k = 500;
					break;
				}

				/* Apply the cut */
				if (k) (void)set_cut(p_ptr->cut + k);
			}

			/* Handle stun */
			if (do_stun)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
				case 0:
					k = 0;
					break;
				case 1:
					k = randint(5);
					break;
				case 2:
					k = randint(10) + 10;
					break;
				case 3:
					k = randint(15) + 15;
					break;
				case 4:
					k = randint(20) + 20;
					break;
				case 5:
					k = randint(25) + 25;
					break;
				case 6:
					k = randint(30) + 30;
					break;
				default:
					k = randint(32) + 32;
					break;
				}

				/* Apply the stun */
				if (k) (void)set_stun(p_ptr->stun + k);
			}

			/* Do vampiric thingies */
			if (do_vampire)
			{
				/* Change to resist(but never total protection) */
/*				if (magik(3) || (magik(m_ptr->level - (p_ptr->lev / 2))))
					call_lua("gain_corruption", "(s)", "", "Vampire");*/
			}

			if (explode)
			{
				sound(SOUND_EXPLODE);
				if (mon_take_hit(m_idx, m_ptr->hp + 1, &fear, NULL))
				{
					blinked = FALSE;
					alive = FALSE;
				}
			}

			if (touched)
			{
				if (p_ptr->sh_fire && alive)
				{
					if (!(r_ptr->flags3 & RF3_IM_FIRE))
					{
						msg_format("%^s is suddenly very hot!", m_name);
						if (mon_take_hit(m_idx, damroll(2, 6), &fear,
						                 " turns into a pile of ash."))
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (m_ptr->ml)
							r_ptr->r_flags3 |= RF3_IM_FIRE;
					}
				}

				if (p_ptr->sh_elec && alive)
				{
					if (!(r_ptr->flags3 & RF3_IM_ELEC))
					{
						msg_format("%^s gets zapped!", m_name);
						if (mon_take_hit(m_idx, damroll(2, 6), &fear,
						                 " turns into a pile of cinder."))
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (m_ptr->ml)
							r_ptr->r_flags3 |= RF3_IM_ELEC;
					}
				}
				if (p_ptr->shield && (p_ptr->shield_opt & SHIELD_COUNTER) && alive)
				{
					msg_format("%^s gets bashed by your mystic shield!", m_name);
					if (mon_take_hit(m_idx, damroll(p_ptr->shield_power_opt, p_ptr->shield_power_opt2), &fear,
					                 " is bashed by your mystic shield."))
					{
						blinked = FALSE;
						alive = FALSE;
					}
				}
				if (p_ptr->shield && (p_ptr->shield_opt & SHIELD_FIRE) && alive)
				{
					if (!(r_ptr->flags3 & RF3_IM_FIRE))
					{
						msg_format("%^s gets burned by your fiery shield!", m_name);
						if (mon_take_hit(m_idx, damroll(p_ptr->shield_power_opt, p_ptr->shield_power_opt2), &fear,
						                 " is burned by your fiery shield."))
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (m_ptr->ml)
							r_ptr->r_flags3 |= RF3_IM_FIRE;
					}
				}
				if (p_ptr->shield && (p_ptr->shield_opt & SHIELD_GREAT_FIRE) && alive)
				{
					msg_format("%^s gets burned by your fiery shield!", m_name);
					if (mon_take_hit(m_idx, damroll(p_ptr->shield_power_opt, p_ptr->shield_power_opt2), &fear,
					                 " is burned by your fiery shield."))
					{
						blinked = FALSE;
						alive = FALSE;
					}
				}
				if (p_ptr->shield && (p_ptr->shield_opt & SHIELD_FEAR) && alive)
				{
					int tmp;

					if ((!(r_ptr->flags1 & RF1_UNIQUE)) && (damroll(p_ptr->shield_power_opt, p_ptr->shield_power_opt2) - m_ptr->level > 0))
					{
						msg_format("%^s gets scared away!", m_name);

						/* Increase fear */
						tmp = m_ptr->monfear + p_ptr->shield_power_opt;
						fear = TRUE;

						/* Set fear */
						m_ptr->monfear = (tmp < 200) ? tmp : 200;
					}
				}

				touched = FALSE;
			}
		}

		/* Monster missed player */
		else
		{
			/* Analyze failed attacks */
			switch (method)
			{
			case RBM_HIT:
			case RBM_TOUCH:
			case RBM_PUNCH:
			case RBM_KICK:
			case RBM_CLAW:
			case RBM_BITE:
			case RBM_STING:
			case RBM_BUTT:
			case RBM_CRUSH:
			case RBM_ENGULF:
			case RBM_CHARGE:

				/* Visible monsters */
				if (m_ptr->ml)
				{
					/* Disturbing */
					disturb(1, 0);

					/* Message */
					msg_format("%^s misses you.", m_name);
				}

				break;
			}
		}


		/* Analyze "visible" monsters only */
		if (visible)
		{
			/* Count "obvious" attacks (and ones that cause damage) */
			if (obvious || damage || (r_ptr->r_blows[ap_cnt] > 10))
			{
				/* Count attacks of this type */
				if (r_ptr->r_blows[ap_cnt] < MAX_UCHAR)
				{
					r_ptr->r_blows[ap_cnt]++;
				}
			}
		}
	}


	/* Blink away */
	if (blinked)
	{
		msg_print("The thief flees laughing!");
		teleport_away(m_idx, MAX_SIGHT * 2 + 5);
	}


	/* Always notice cause of death */
	if (death && (r_ptr->r_deaths < MAX_SHORT))
	{
		r_ptr->r_deaths++;
	}

	if (m_ptr->ml && fear)
	{
		sound (SOUND_FLEE);
		msg_format("%^s flees in terror!", m_name);
	}

	/* Assume we attacked */
	return (TRUE);
}

/* saving throw change by Amy: can't reach 100% anymore, but every point beyond 90 gets you infinitesimally closer */
int player_actual_saving_throw(void)
{
	int playersav = p_ptr->skill_sav;

	if (playersav > 89) {
		int diffsav = playersav - 89;

		playersav = 89 + randint(diffsav);
	}

	return playersav;
}

