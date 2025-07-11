/* File: store.c */

/* Purpose: Store commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


#define RUMOR_CHANCE 8

#define MAX_COMMENT_1	6

static cptr comment_1[MAX_COMMENT_1] =
{
	"Okay.",
	"Fine.",
	"Accepted!",
	"Agreed!",
	"Done!",
	"Taken!"
};

#define MAX_COMMENT_2A	2

static cptr comment_2a[MAX_COMMENT_2A] =
{
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
};

#define MAX_COMMENT_2B	12

static cptr comment_2b[MAX_COMMENT_2B] =
{
	"I can take no less than %s gold pieces.",
	"I will accept no less than %s gold pieces.",
	"Ha!  No less than %s gold pieces.",
	"You knave!  No less than %s gold pieces.",
	"That's a pittance!  I want %s gold pieces.",
	"That's an insult!  I want %s gold pieces.",
	"As if!  How about %s gold pieces?",
	"My gosh!  How about %s gold pieces?",
	"May the fleas of 1000 orcs molest you!  Try %s gold pieces.",
	"May your most favorite weapons rust!  Try %s gold pieces.",
	"May Morgoth find you tasty!  Perhaps %s gold pieces?",
	"Your mother was an Ogre!  Perhaps %s gold pieces?"
};

#define MAX_COMMENT_3A	2

static cptr comment_3a[MAX_COMMENT_3A] =
{
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
};


#define MAX_COMMENT_3B	12

static cptr comment_3b[MAX_COMMENT_3B] =
{
	"Perhaps %s gold pieces?",
	"How about %s gold pieces?",
	"I will pay no more than %s gold pieces.",
	"I can afford no more than %s gold pieces.",
	"Be reasonable.  How about %s gold pieces?",
	"I'll buy it as scrap for %s gold pieces.",
	"That is too much!  How about %s gold pieces?",
	"That looks war surplus!  Say %s gold pieces?",
	"Never!  %s is more like it.",
	"That's an insult!  %s is more like it.",
	"%s gold pieces and be thankful for it!",
	"%s gold pieces and not a copper more!"
};

#define MAX_COMMENT_4A	4

static cptr comment_4a[MAX_COMMENT_4A] =
{
	"Enough!  You have abused me once too often!",
	"Arghhh!  I have had enough abuse for one day!",
	"That does it!  You shall waste my time no more!",
	"This is getting nowhere!  I'm going to Londis!"
};

#define MAX_COMMENT_4B	4

static cptr comment_4b[MAX_COMMENT_4B] =
{
	"Leave my store!",
	"Get out of my sight!",
	"Begone, you scoundrel!",
	"Out, out, out!"
};

#define MAX_COMMENT_5	8

static cptr comment_5[MAX_COMMENT_5] =
{
	"Try again.",
	"Ridiculous!",
	"You will have to do better than that!",
	"Do you wish to do business or not?",
	"You've got to be kidding!",
	"You'd better be kidding!",
	"You try my patience.",
	"Hmmm, nice weather we're having."
};

#define MAX_COMMENT_6	4

static cptr comment_6[MAX_COMMENT_6] =
{
	"I must have heard you wrong.",
	"I'm sorry, I missed that.",
	"I'm sorry, what was that?",
	"Sorry, what was that again?"
};



/*
 * Successful haggle.
 */
static void say_comment_1(void)
{
	char rumour[80];

	msg_print(comment_1[rand_int(MAX_COMMENT_1)]);

	if (randint(RUMOR_CHANCE) == 1 && speak_unique)
	{
		msg_print("The shopkeeper whispers something into your ear:");

		/* Pick random text */
		switch (randint(100))
		{
		case 1:
			{
				get_rnd_line("chainswd.txt", rumour);

				break;
			}

		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			{
				get_rnd_line("error.txt", rumour);

				break;
			}

		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
			{
				get_rnd_line("death.txt", rumour);

				break;
			}

		default:
			{
				get_rnd_line("rumors.txt", rumour);

				break;
			}
		}

		msg_print(rumour);
	}
}


/*
 * Continue haggling (player is buying)
 */
static void say_comment_2(s32b value, int annoyed)
{
	char	tmp_val[80];

	/* Prepare a string to insert */
	strnfmt(tmp_val, 80, "%ld", (long)value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_2a[rand_int(MAX_COMMENT_2A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
		msg_format(comment_2b[rand_int(MAX_COMMENT_2B)], tmp_val);
	}
}


/*
 * Continue haggling (player is selling)
 */
static void say_comment_3(s32b value, int annoyed)
{
	char	tmp_val[80];

	/* Prepare a string to insert */
	strnfmt(tmp_val, 80, "%ld", (long)value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_3a[rand_int(MAX_COMMENT_3A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
		msg_format(comment_3b[rand_int(MAX_COMMENT_3B)], tmp_val);
	}
}


/*
 * Kick 'da bum out.					-RAK-
 */
static void say_comment_4(void)
{
	msg_print(comment_4a[rand_int(MAX_COMMENT_4A)]);
	msg_print(comment_4b[rand_int(MAX_COMMENT_4B)]);
}


/*
 * You are insulting me
 */
static void say_comment_5(void)
{
	msg_print(comment_5[rand_int(MAX_COMMENT_5)]);
}


/*
 * That makes no sense.
 */
static void say_comment_6(void)
{
	msg_print(comment_6[rand_int(5)]);
}



/*
 * Messages for reacting to purchase prices.
 */

#define MAX_COMMENT_7A	4

static cptr comment_7a[MAX_COMMENT_7A] =
{
	"Arrgghh!",
	"You moron!",
	"You hear someone sobbing...",
	"The shopkeeper howls in agony!"
};

#define MAX_COMMENT_7B	4

static cptr comment_7b[MAX_COMMENT_7B] =
{
	"Darn!",
	"You fiend!",
	"The shopkeeper yells at you.",
	"The shopkeeper glares at you."
};

#define MAX_COMMENT_7C	4

static cptr comment_7c[MAX_COMMENT_7C] =
{
	"Cool!",
	"You've made my day!",
	"The shopkeeper giggles.",
	"The shopkeeper laughs loudly."
};

#define MAX_COMMENT_7D	4

static cptr comment_7d[MAX_COMMENT_7D] =
{
	"Yippee!",
	"I think I'll retire!",
	"The shopkeeper jumps for joy.",
	"The shopkeeper smiles gleefully."
};

/*
 * Let a shop-keeper React to a purchase
 *
 * We paid "price", it was worth "value", and we thought it was worth "guess"
 */
static void purchase_analyze(s32b price, s32b value, s32b guess)
{
	/* Item was worthless, but we bought it */
	if ((value <= 0) && (price > value))
	{
		/* Comment */
		msg_print(comment_7a[rand_int(MAX_COMMENT_7A)]);

		/* Sound */
		sound(SOUND_STORE1);
	}

	/* Item was cheaper than we thought, and we paid more than necessary */
	else if ((value < guess) && (price > value))
	{
		/* Comment */
		msg_print(comment_7b[rand_int(MAX_COMMENT_7B)]);

		/* Sound */
		sound(SOUND_STORE2);
	}

	/* Item was a good bargain, and we got away with it */
	else if ((value > guess) && (value < (4 * guess)) && (price < value))
	{
		/* Comment */
		msg_print(comment_7c[rand_int(MAX_COMMENT_7C)]);

		/* Sound */
		sound(SOUND_STORE3);
	}

	/* Item was a great bargain, and we got away with it */
	else if ((value > guess) && (price < value))
	{
		/* Comment */
		msg_print(comment_7d[rand_int(MAX_COMMENT_7D)]);

		/* Sound */
		sound(SOUND_STORE4);
	}
}





/*
 * We store the current "store number" here so everyone can access it
 */
static int cur_store_num = 7;

/*
 * We store the current "store page" here so everyone can access it
 */
static int store_top = 0;

/*
 * We store the current "store pointer" here so everyone can access it
 */
static store_type *st_ptr = NULL;

/*
 * We store the current "owner type" here so everyone can access it
 */
static owner_type *ot_ptr = NULL;

/*
 * and GUH I guess we also store the rank
 */
static int st_investment = 0;



/*
 * Determine the price of an item (qty one) in a store.
 *
 * This function takes into account the player's charisma, and the
 * shop-keepers friendliness, and the shop-keeper's base greed, but
 * never lets a shop-keeper lose money in a transaction.
 *
 * The "greed" value should exceed 100 when the player is "buying" the
 * item, and should be less than 100 when the player is "selling" it.
 *
 * Hack -- the black market always charges twice as much as it should.
 *
 * Charisma adjustment runs from 80 to 130
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 *
 * "flip" means you're selling stuff to the shop
 */
static s32b price_item(object_type *o_ptr, int greed, bool flip)
{
	int factor;
	int adjust;
	s32b price;

	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Get the value of one of the items */
	price = object_value(o_ptr);

	if (!flip) {
		if (object_value_shop(o_ptr) > price) price = object_value_shop(o_ptr);
	}

	/* Worthless items - edit by Amy: shops should offer them, but you shouldn't be able to sell them */
	if (price <= 0) {
		if (flip) return(0L);
		else {
			/* and they should be pricey for higher-level stuff, since buying one IDs the base type permanently */
			price = 200;

			/* but ammo shouldn't be overly expensive */
			switch (o_ptr->tval)
			{
				case TV_SHOT:
				case TV_ARROW:
				case TV_BOLT:
				{
					price = 10;
					break;
				}
				case TV_AMMO_PISTOL:
				{
					price = 25;
					break;
				}
				case TV_AMMO_RIFLE:
				{
					price = 40;
					break;
				}
				case TV_AMMO_SHOTGUN:
				{
					price = 100;
					break;
				}
				case TV_AMMO_SMG:
				{
					price = 30;
					break;
				}
				case TV_AMMO_ASSAULT:
				{
					price = 75;
					break;
				}
			} /* switch end */

			if (k_ptr->level > 1) price *= k_ptr->level;

			/* still need to account for bonuses, e.g. "rohirric ring of teleportitis {cursed}" --Amy */
			if (object_value_xtra(o_ptr) > 0) {
				if (o_ptr->tval == TV_SHOT || o_ptr->tval == TV_ARROW || o_ptr->tval == TV_BOLT || o_ptr->tval == TV_AMMO_PISTOL || o_ptr->tval == TV_AMMO_RIFLE || o_ptr->tval == TV_AMMO_SHOTGUN || o_ptr->tval == TV_AMMO_SMG || o_ptr->tval == TV_AMMO_ASSAULT) {
					int tempincrease = object_value_xtra(o_ptr);
					if (tempincrease > 0) {
						tempincrease /= 20;
						if (tempincrease < 1) tempincrease = 1;
					}
					price += tempincrease;
				} else { 
					price += object_value_xtra(o_ptr);
				}
			}

		}
	}

	/* Compute the racial factor */
	if (is_state(st_ptr, STORE_LIKED))
	{
		factor = ot_ptr->costs[STORE_LIKED];
	}
	else if (is_state(st_ptr, STORE_HATED))
	{
		factor = ot_ptr->costs[STORE_HATED];
	}
	else
	{
		factor = ot_ptr->costs[STORE_NORMAL];
	}

	/* Add in the charisma factor */
	factor += adj_chr_gold[p_ptr->stat_ind[A_CHR]];

	/* Hack - merchants have better prices */
#if 0 /* DGDGDGDG -- use a skill */
	if (cp_ptr->magic_key == MKEY_TELEKINESIS)
		factor -= p_ptr->lev / 2;
#endif
	/* Shop is buying */
	if (flip)
	{
		/* Mega Hack^3 */
		switch (o_ptr->tval)
		{
			case TV_SHOT:
			case TV_ARROW:
			case TV_BOLT:
			{
				price /= 5;
				break;
			}
			case TV_AMMO_PISTOL:
			case TV_AMMO_RIFLE:
			case TV_AMMO_SHOTGUN:
			case TV_AMMO_SMG:
			case TV_AMMO_ASSAULT:
			{
				price /= 20;
				break;
			}
		} /* switch end */

		/* Adjust for greed */
		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if (adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (st_info[st_ptr->st_idx].flags1 & SF1_ALL_ITEM) price = price / 3;

		/* ripoff nastytrap makes shopkeepers rip you off */
		if (p_ptr->nastytrap76) price /= 3;

		/* Amy edit: items just give way too much gold when you sell them!
		 * added a new skill that, if maxxed, gives the old values back */
		price *= (20 + get_skill_scale(SKILL_HAGGLING, 15) );
		price /= 50;
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if (adjust < 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (st_info[st_ptr->st_idx].flags1 & SF1_ALL_ITEM) price = price * 3;

		/* ripoff nastytrap makes shopkeepers rip you off */
		if (p_ptr->nastytrap76) price *= 3;
	}


	/* Compute the final price (with rounding) */
	price = (price * adjust + 50L) / 100L;

	/* Note -- Never become "free" */
	if (price <= 0L) return (1L);

	/* Return the price */
	return (price);
}


/*
 * Special "mass production" computation
 */
static int mass_roll(int num, int max)
{
	int i, t = 0;
	for (i = 0; i < num; i++) t += rand_int(max);
	return (t);
}


/*
 * Certain "cheap" objects should be created in "piles"
 * Some objects can be sold at a "discount" (in small piles)
 */
static void mass_produce(object_type *o_ptr)
{
	int size = 1;
	int discount = 0;

	s32b cost = object_value(o_ptr);


	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Food, Flasks, and Lites */
	case TV_FOOD:
	case TV_FLASK:
	case TV_LITE:
		{
			if (cost > 0L) {
				if (cost <= 5L) size += mass_roll(2, 5);
				if (cost <= 20L) size += mass_roll(2, 5);
			}
			size += mass_roll(2, 5);
			break;
		}

	case TV_POTION:
	case TV_POTION2:
	case TV_SCROLL:
		{
			if (cost > 0L) {
				if (cost <= 60L) size += mass_roll(2, 5);
				if (cost <= 240L) size += mass_roll(1, 5);
			}
			size += mass_roll(1, 5);
			break;
		}

	case TV_SYMBIOTIC_BOOK:
	case TV_MUSIC_BOOK:
	case TV_DRUID_BOOK:
	case TV_DAEMON_BOOK:
	case TV_BOOK:
		{
			if (cost > 0L) {
				if (cost <= 50L) size += mass_roll(1, 3);
				if (cost <= 500L) size += mass_roll(1, 3);
			}
			size += mass_roll(1, 3);
			break;
		}

	case TV_SOFT_ARMOR:
	case TV_HARD_ARMOR:
	case TV_SHIELD:
	case TV_GLOVES:
	case TV_BOOTS:
	case TV_CLOAK:
	case TV_HELM:
	case TV_CROWN:
	case TV_SWORD:
	case TV_AXE:
	case TV_POLEARM:
	case TV_HAFTED:
	case TV_DIGGING:
	case TV_BOW:
		{
			if (cost > 0L) {
				if (cost <= 10L) size += mass_roll(3, 5);
				if (cost <= 100L) size += mass_roll(3, 5);
			}
			break;
		}

	case TV_SPIKE:
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
		{
			if (cost > 0L) {
				if (cost <= 5L) size += mass_roll(4, 5);
				if (cost <= 50L) size += mass_roll(4, 5);
				if (cost <= 500L) size += mass_roll(4, 5);
			}
			size += mass_roll(8, 5);
			break;
		}

	case TV_AMMO_PISTOL:
		{
			size += mass_roll(14, 7);
			break;
		}
	case TV_AMMO_RIFLE:
		{
			size += mass_roll(10, 7);
			break;
		}
	case TV_AMMO_SHOTGUN:
		{
			size += mass_roll(6, 10);
			break;
		}
	case TV_AMMO_SMG:
		{
			size += mass_roll(8, 10);
			break;
		}
	case TV_AMMO_ASSAULT:
		{
			size += mass_roll(10, 10);
			break;
		}

		/* Because many rods (and a few wands and staffs) are useful mainly
		 * in quantity, the Black Market will occasionally have a bunch of
		 * one kind. -LM- */
	case TV_ROD:
	case TV_WAND:
	case TV_STAFF:
		{
			if (cost > 0L) {
				if (cost < 1601L) size += mass_roll(1, 5);
				else if (cost < 3201L) size += mass_roll(1, 3);
			}
			size += mass_roll(1, 3);
			break;
		}
	}


	/* Pick a discount */
	if (cost < 5)
	{
		discount = 0;
	}
	else if (rand_int(125) == 0)
	{
		discount = 25;
	}
	else if (rand_int(750) == 0)
	{
		discount = 50;
	}
	else if (rand_int(2000) == 0)
	{
		discount = 75;
	}
	else if (rand_int(50000) == 0)
	{
		discount = 90;
	}


	if (o_ptr->art_name)
	{
		if (cheat_peek && discount)
		{
			msg_print("No discount on random artifacts.");
		}
		discount = 0;
	}

	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = size - (size * discount / 100);

	if (o_ptr->number > 99) o_ptr->number = 99;
}








/*
 * Determine if a store item can "absorb" another item
 *
 * See "object_similar()" for the same function for the "player"
 */
static bool store_object_similar(object_type *o_ptr, object_type *j_ptr)
{
	/* Hack -- Identical items cannot be stacked */
	if (o_ptr == j_ptr) return (0);

	/* Different objects cannot be stacked */
	if (o_ptr->k_idx != j_ptr->k_idx) return (0);

	/* Different charges (etc) cannot be stacked, unless wands or rods. */
	if ((o_ptr->pval != j_ptr->pval) && (o_ptr->tval != TV_WAND)) return (0);

	/* Require many identical values */
	if (o_ptr->pval2 != j_ptr->pval2) return (0);
	if (o_ptr->pval3 != j_ptr->pval3) return (0);

	/* Require many identical values */
	if (o_ptr->to_h != j_ptr->to_h) return (0);
	if (o_ptr->to_d != j_ptr->to_d) return (0);
	if (o_ptr->to_a != j_ptr->to_a) return (0);

	/* Require identical "artifact" names */
	if (o_ptr->name1 != j_ptr->name1) return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->name2 != j_ptr->name2) return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->name2b != j_ptr->name2b) return (0);

	/* Do not combine different ego or normal ones */
	if (ego_item_p(o_ptr) != ego_item_p(j_ptr)) return (FALSE);

	/* Random artifacts don't stack !*/
	if (o_ptr->art_name || j_ptr->art_name) return (0);

	/* Hack -- Identical art_flags! Amy edit: who the hell made it so that 4 and 5 aren't checked *facepalm* */
	if ((o_ptr->art_flags1 != j_ptr->art_flags1) ||
	                (o_ptr->art_flags2 != j_ptr->art_flags2) ||
	                (o_ptr->art_flags3 != j_ptr->art_flags3) ||
	                (o_ptr->art_flags4 != j_ptr->art_flags4) ||
	                (o_ptr->art_flags5 != j_ptr->art_flags5) ||
	                (o_ptr->art_flags6 != j_ptr->art_flags6) ||
	                (o_ptr->art_flags7 != j_ptr->art_flags7) ||
	                (o_ptr->art_flags8 != j_ptr->art_flags8) ||
	                (o_ptr->art_flags9 != j_ptr->art_flags9) ||
	                (o_ptr->art_flags10 != j_ptr->art_flags10))
		return (0);

	/* Hack -- Never stack "powerful" items */
	if (o_ptr->xtra1 || j_ptr->xtra1) return (0);

	if (o_ptr->tval == TV_LITE)
	{
		/* Require identical "turns of light" */
		if (o_ptr->timeout != j_ptr->timeout) return (0);
	}
	else
	{
		/* Hack -- Never stack recharging items */
		if (o_ptr->timeout || j_ptr->timeout) return (0);
	}

	/* Require many identical values */
	if (o_ptr->ac	!= j_ptr->ac) return (0);
	if (o_ptr->dd	!= j_ptr->dd) return (0);
	if (o_ptr->ds	!= j_ptr->ds) return (0);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST) return (0);

	/* Require matching discounts */
	if (o_ptr->discount != j_ptr->discount) return (0);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store item to absorb another item
 */
static void store_object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > 99) ? 99 : total;

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if (o_ptr->tval == TV_WAND)
	{
		o_ptr->pval += j_ptr->pval;
	}
}


/*
 * Check to see if the shop will be carrying too many objects	-RAK-
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.	Before, one could "nuke" potions this way.
 */
static bool store_check_num(object_type *o_ptr)
{
	int i;
	object_type *j_ptr;

	/* hard limit of 255 items --Amy */
	if (st_ptr->stock_num >= 255) return FALSE;

	/* Free space is always usable */
	if (st_ptr->stock_num < (st_ptr->stock_size + st_ptr->investment) ) return TRUE;

	/* The "home" acts like the player */
	if ((cur_store_num == 7) ||
	                (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM))
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* But there was no room at the inn... */
	return (FALSE);
}

static bool store_check_fourdim(object_type *o_ptr)
{
	int i;
	object_type *j_ptr;
	int fourdimlevel = get_skill(SKILL_FOUR_DIM);

	/* Free space is always usable */
	if (st_ptr->stock_num < fourdimlevel) return TRUE;

	/* The "home" acts like the player */
	if ((cur_store_num == 7) ||
	                (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM))
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* But there was no room at the inn... */
	return (FALSE);
}


bool is_blessed(object_type *o_ptr)
{
	u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;
	object_flags_known(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
	if (f3 & TR3_BLESSED) return (TRUE);
	else return (FALSE);
}



/*
 * Determine if the current store will purchase the given item
 *
 * Note that a shop-keeper must refuse to buy "worthless" items
 */
static bool store_will_buy(object_type *o_ptr)
{
	/* Hack -- The Home is simple */
	if (cur_store_num == 7) return (TRUE);

	if (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM) return TRUE;

	/* XXX XXX XXX Ignore "worthless" items */
	if (object_value(o_ptr) <= 0) return (FALSE);

	/* can buy corpses, skeletons and chests at shops but they're expensive; shouldn't be able to sell them --Amy */
	if (o_ptr->tval == TV_CORPSE) {
		if (o_ptr->sval == SV_CORPSE_CORPSE) return FALSE;
		if (o_ptr->sval == SV_CORPSE_SKELETON) return FALSE;
		if (o_ptr->sval == SV_CORPSE_HEAD) return FALSE;
		if (o_ptr->sval == SV_CORPSE_SKULL) return FALSE;
	}
	if (o_ptr->tval == TV_SKELETON) return FALSE;
	if (o_ptr->tval == TV_CHEST) return FALSE;
	if (o_ptr->tval == TV_BATERIE) return FALSE;
	if (o_ptr->tval == TV_RUNE1) return FALSE;
	if (o_ptr->tval == TV_RUNE2) return FALSE;

	/* Lua can define things to buy */
	if (process_hooks_ret(HOOK_STORE_BUY, "d", "(d,s,O)", st_ptr->st_idx, st_info[st_ptr->st_idx].name + st_name, o_ptr))
	{
		return process_hooks_return[0].num;
	}

	/* Assume not okay */
	return (FALSE);
}



/*
 * Add the item "o_ptr" to the inventory of the "Home"
 *
 * In all cases, return the slot (or -1) where the object was placed
 *
 * Note that this is a hacked up version of "inven_carry()".
 *
 * Also note that it may not correctly "adapt" to "knowledge" bacoming
 * known, the player may have to pick stuff up and drop it again.
 */
static int home_carry(object_type *o_ptr)
{
	int slot;
	s32b	value, j_value;
	int i;
	object_type *j_ptr;

	int actualstocksize;

	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* The home acts just like the player */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	actualstocksize = (st_ptr->stock_size + st_ptr->investment);
	if (actualstocksize < 1) actualstocksize = 1;

	/* No space? */
	if (st_ptr->stock_num >= actualstocksize) return ( -1);


	/* Determine the "value" of the item */
	value = object_value(o_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that item */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Can happen in the home */
		if (!object_aware_p(o_ptr)) continue;
		if (!object_aware_p(j_ptr)) break;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;

		/* Objects in the home can be unknown */
		if (!object_known_p(o_ptr)) continue;
		if (!object_known_p(j_ptr)) break;


		/*
		 * Hack:  otherwise identical rods sort by
		 * increasing recharge time --dsb
		 */
		if (o_ptr->tval == TV_ROD_MAIN)
		{
			if (o_ptr->timeout < j_ptr->timeout) break;
			if (o_ptr->timeout > j_ptr->timeout) continue;
		}

		/* Objects sort by decreasing value */
		j_value = object_value(j_ptr);
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i - 1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	/* Return the location */
	return (slot);
}


/*
 * Add the item "o_ptr" to a real stores inventory.
 *
 * If the item is "worthless", it is thrown away (except in the home).
 *
 * If the item cannot be combined with an object already in the inventory,
 * make a new slot for it, and calculate its "per item" price.	Note that
 * this price will be negative, since the price will not be "fixed" yet.
 * Adding an item to a "fixed" price stack will not change the fixed price.
 *
 * In all cases, return the slot (or -1) where the object was placed
 */
static int store_carry(object_type *o_ptr)
{
	int i, slot;
	s32b	value, j_value;
	object_type *j_ptr;

	int actualstocksize;

	/* Evaluate the object */
	value = object_value(o_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	/*if (value <= 0) return ( -1);*/

	/* All store items are fully *identified* */
	o_ptr->ident |= IDENT_MENTAL;

	/* Erase the inscription */
	o_ptr->note = 0;

	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* Can the existing items be incremented? */
		if (store_object_similar(j_ptr, o_ptr))
		{
			/* Hack -- extra items disappear */
			store_object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	actualstocksize = st_ptr->stock_size;
	if (st_ptr->investment > 0) actualstocksize += randint(st_ptr->investment);
	if (st_ptr->investment < 0) actualstocksize += st_ptr->investment; /* reduction */
	if (actualstocksize < 1) actualstocksize = 1;

	/* No space? */
	if (st_ptr->stock_num >= actualstocksize) return ( -1);


	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that item */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;


		/*
		 * Hack:  otherwise identical rods sort by
		 * increasing recharge time --dsb
		 */
		if (o_ptr->tval == TV_ROD_MAIN)
		{
			if (o_ptr->timeout < j_ptr->timeout) break;
			if (o_ptr->timeout > j_ptr->timeout) continue;
		}

		/* Evaluate that slot */
		j_value = object_value(j_ptr);

		/* Objects sort by decreasing value */
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i - 1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	/* Return the location */
	return (slot);
}


/*
 * Increase, by a given amount, the number of a certain item
 * in a certain store.	This can result in zero items.
 */
static void store_item_increase(int item, int num)
{
	int cnt;
	object_type *o_ptr;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Verify the number */
	cnt = o_ptr->number + num;
	if (cnt > 255) cnt = 255;
	else if (cnt < 0) cnt = 0;
	num = cnt - o_ptr->number;

	/* Save the new number */
	o_ptr->number += num;
}


/*
 * Remove a slot if it is empty
 */
static void store_item_optimize(int item)
{
	int j;
	object_type *o_ptr;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Must exist */
	if (!o_ptr->k_idx) return;

	/* Must have no items */
	if (o_ptr->number) return;

	/* One less item */
	st_ptr->stock_num--;

	/* Slide everyone */
	for (j = item; j < st_ptr->stock_num; j++)
	{
		st_ptr->stock[j] = st_ptr->stock[j + 1];
	}

	/* Nuke the final slot */
	object_wipe(&st_ptr->stock[j]);
}


/*
 * This function will keep 'crap' out of the black market.
 * Crap is defined as any item that is "available" elsewhere
 * Based on a suggestion by "Lee Vogt" <lvogt@cig.mcel.mot.com>
 */
static bool black_market_crap(object_type *o_ptr)
{
	int i, j;

	/* Ego items are never crap */
	if (o_ptr->name2) return (FALSE);

	/* Good items are never crap */
	if (o_ptr->to_a > 0) return (FALSE);
	if (o_ptr->to_h > 0) return (FALSE);
	if (o_ptr->to_d > 0) return (FALSE);

	/* Check all stores */
	for (i = 0; i < max_st_idx; i++)
	{
		if (i == STORE_HOME) continue;
		if (st_info[i].flags1 & SF1_MUSEUM) continue;

		/* Check every item in the store */
		for (j = 0; j < town_info[p_ptr->town_num].store[i].stock_num; j++)
		{
			object_type *j_ptr = &town_info[p_ptr->town_num].store[i].stock[j];

			/* Duplicate item "type", assume crappy */
			if (o_ptr->k_idx == j_ptr->k_idx) return (TRUE);
		}
	}

	/* Assume okay */
	return (FALSE);
}


/*
 * Attempt to delete (some of) a random item from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
static void store_delete(void)
{
	int what, num;

	/* Pick a random slot */
	what = rand_int(st_ptr->stock_num);

	/* Determine how many items are here */
	num = st_ptr->stock[what].number;

	/* Hack -- sometimes, only destroy half the items */
	if (rand_int(100) < 50) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single item */
	if (rand_int(100) < 50) num = 1;

	/* Hack -- decrement the maximum timeouts and total charges of rods and wands. -LM- */
	if (st_ptr->stock[what].tval == TV_WAND)
	{
		st_ptr->stock[what].pval -= num * st_ptr->stock[what].pval / st_ptr->stock[what].number;
	}

	/* Actually destroy (part of) the item */
	store_item_increase(what, -num);
	store_item_optimize(what);
}

/* Analyze store flags and return a level */
int return_level()
{
	store_info_type *sti_ptr = &st_info[st_ptr->st_idx];
	int level;

	/* Amy edit: make runecraft skill not completely useless */
	int runebonus = get_skill(SKILL_FORTUNE) + p_ptr->mfind_bonus;

	/* investing slowly improves the quality of items --Amy */
	if (st_ptr->investment > 4) {
		runebonus += (st_ptr->investment / 5);
	}

	if (p_ptr->nastytrap45) runebonus = 0;

	if (sti_ptr->flags1 & SF1_RANDOM) level = 0;
	else level = rand_range(1, STORE_OBJ_LEVEL);

	if (sti_ptr->flags1 & SF1_DEPEND_LEVEL) level += dun_level;

	if (sti_ptr->flags1 & SF1_SHALLOW_LEVEL) level += 5 + rand_int(5);
	if (sti_ptr->flags1 & SF1_MODERATE_LEVEL) level += 15 + rand_int(5);
	if (sti_ptr->flags1 & SF1_MEDIUM_LEVEL) level += 25 + rand_int(25);
	if (sti_ptr->flags1 & SF1_DEEP_LEVEL) level += 45 + rand_int(45);

	if (sti_ptr->flags1 & SF1_ALL_ITEM) level += p_ptr->lev;

	if (runebonus > 0) level += runebonus;

	switch (randint(1000)) {

		case 94:
		case 95:
		case 96:
		case 97:
		case 98:
			level += rand_int(20);
			break;
		case 99:
			level += rand_int(100);
			break;
		default:
			break;

	}

	if (p_ptr->nastytrap45 && (level > 1)) level /= 2;

	return (level);
}

/* Is it an ok object ? */
static int store_tval = 0, store_level = 0;

/*
 * Hack -- determine if a template is "good"
 */
static bool kind_is_storeok(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	if (k_info[k_idx].flags3 & TR3_NORM_ART)
		return ( FALSE );

	if (k_info[k_idx].flags3 & TR3_INSTA_ART)
		return ( FALSE );

	if (!kind_is_legal(k_idx)) return FALSE;

	if (k_ptr->tval != store_tval) return (FALSE);

	/* stores that force their level usually spawn high-level items only, but allow the occasional low-level one too --Amy */
	if ((k_ptr->level < (store_level / 2)) && (randint(10) != 1) ) return (FALSE);

	return (TRUE);
}

/*
 * Creates a random item and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 *
 * Note -- the "level" given to "obj_get_num()" is a "favored"
 * level, that is, there is a much higher chance of getting
 * items with a level approaching that of the given level...
 *
 * Should we check for "permission" to have the given item?
 */
static void store_create(void)
{
	int i = 0, tries, level = 0, chance, item;

	object_type forge;
	object_type *q_ptr = NULL;
	bool obj_all_done = FALSE;

	int actualstocksize;

	actualstocksize = st_ptr->stock_size;
	if (st_ptr->investment > 0) actualstocksize += randint(st_ptr->investment);
	if (st_ptr->investment < 0) actualstocksize += st_ptr->investment; /* reduction */
	if (actualstocksize < 1) actualstocksize = 1;

	/* Paranoia -- no room left */
	if (st_ptr->stock_num >= actualstocksize) return;


	/* Hack -- consider up to four items */
	for (tries = 0; tries < 4; tries++)
	{
		obj_all_done = FALSE;

		/* Lua can define things to buy */
		if (process_hooks_ret(HOOK_STORE_STOCK, "O", "(d,s,d)", st_ptr->st_idx, st_info[st_ptr->st_idx].name + st_name, return_level()))
		{
			obj_all_done = TRUE;
			q_ptr = process_hooks_return[0].o_ptr;
		}

		/* Black Market */
		else if (st_info[st_ptr->st_idx].flags1 & SF1_ALL_ITEM)
		{
			obj_theme theme;

			/* No themes */
			theme.treasure = 100;
			theme.combat = 100;
			theme.magic = 100;
			theme.tools = 100;
			init_match_theme(theme);

			/*
			 * Even in Black Markets, illegal objects can be
			 * problematic -- Oxymoron?
			 */
			get_obj_num_hook = kind_is_legal;

			/* Rebuild the allocation table */
			get_obj_num_prep();

			/* Pick a level for object/magic */
			level = return_level();

			/* Random item (usually of given level) */
			i = get_obj_num(level);

			/* Invalidate the cached allocation table */
			alloc_kind_table_valid = FALSE;

			/* Handle failure */
			if (!i) continue;

		}

		/* Normal Store */
		else
		{
			/* Hack -- Pick an item to sell */
			item = rand_int(st_info[st_ptr->st_idx].table_num);
			i = st_info[st_ptr->st_idx].table[item][0];
			chance = st_info[st_ptr->st_idx].table[item][1];

			/* Don't allow k_info artifacts */
			if ((i <= 10000) && (k_info[i].flags3 & TR3_NORM_ART))
				continue;

			/* Does it passes the rarity check ? */
			if (!magik(chance)) continue;

			/* Hack -- fake level for apply_magic() */
			level = return_level();

			/* Hack -- i > 10000 means it's a tval and all svals are allowed */
			if (i > 10000)
			{
				obj_theme theme;

				/* No themes */
				theme.treasure = 100;
				theme.combat = 100;
				theme.magic = 100;
				theme.tools = 100;
				init_match_theme(theme);

				/* Activate restriction */
				get_obj_num_hook = kind_is_storeok;
				store_tval = i - 10000;

				/* Do we forbid too shallow items ? */
				if (st_info[st_ptr->st_idx].flags1 & SF1_FORCE_LEVEL) store_level = level;
				else store_level = 0;

				/* Prepare allocation table */
				get_obj_num_prep();

				/* Get it ! */
				i = get_obj_num(level);

				/* Invalidate the cached allocation table */
				alloc_kind_table_valid = FALSE;
			}

			if (!i) continue;
		}

		/* Only if not already done */
		if (!obj_all_done)
		{
			int luckboost = luck(-3, 3);
			if (luckboost < 0) luckboost = 0;

			/* Don't allow k_info artifacts */
			if (k_info[i].flags3 & TR3_NORM_ART)
				continue;

			/* Don't allow artifacts */
			if (k_info[i].flags3 & TR3_INSTA_ART)
				continue;

			/* Get local object */
			q_ptr = &forge;

			/* Create a new object of the chosen kind */
			object_prep(q_ptr, i);

			/* Apply some "low-level" magic (no artifacts)
			 * Amy edit: come on, stores shouldn't suck so godawfully much. have more ego items, please!
			 * especially if your luck is high! */
			if (magik(2 + luckboost)) apply_magic(q_ptr, level, FALSE, TRUE, TRUE);
			else if (magik(5)) apply_magic(q_ptr, level, FALSE, TRUE, FALSE);
			else apply_magic(q_ptr, level, FALSE, FALSE, FALSE);

			/* Hack -- Charge lite's */
			if (q_ptr->tval == TV_LITE)
			{
				u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

				object_flags(q_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				if (f4 & TR4_FUEL_LITE) q_ptr->timeout = k_info[q_ptr->k_idx].pval2;
			}

		}

		/* The item is "known" */
		object_known(q_ptr);

		/* Mark it storebought */
		q_ptr->ident |= IDENT_STOREB;

		/* Mega-Hack -- no chests in stores */
		/*if (q_ptr->tval == TV_CHEST) continue;*/

		/* Prune the black market */
		/*if (st_info[st_ptr->st_idx].flags1 & SF1_ALL_ITEM)
		{*/
			/* Hack -- No "crappy" items */
			/*if (black_market_crap(q_ptr)) continue;*/

			/* Hack -- No "cheap" items */
			/*if (object_value(q_ptr) < 10) continue;
		}*/

		/* Prune normal stores */
		/* No "worthless" items */
		/*else
		{
			if (object_value(q_ptr) <= 0) continue;
		}*/


		/* Mass produce and/or Apply discount */
		mass_produce(q_ptr);

		/* The charges an wands are per each, so multiply to get correct number */
		if (!obj_all_done && q_ptr->tval == TV_WAND)
		{
			q_ptr->pval *= q_ptr->number;
		}

		/* Attempt to carry the (known) item */
		(void)store_carry(q_ptr);

		/* Definitely done */
		break;
	}
}



/*
 * Eliminate need to bargain if player has haggled well in the past
 */
static bool noneedtobargain(s32b minprice)
{
	s32b good = st_ptr->good_buy;
	s32b bad = st_ptr->bad_buy;

	/* Cheap items are "boring" */
	if (minprice < 10L) return (TRUE);

	/* Perfect haggling */
	if (good == MAX_SHORT) return (TRUE);

	/* Reward good haggles, punish bad haggles, notice price */
	if (good > ((3 * bad) + (5 + (minprice / 50)))) return (TRUE);

	/* Return the flag */
	return (FALSE);
}


/*
 * Update the bargain info
 */
static void updatebargain(s32b price, s32b minprice)
{
	/* Hack -- auto-haggle */
	if (auto_haggle && !p_ptr->nastytrap146) return;

	/* Cheap items are "boring" */
	if (minprice < 10L) return;

	/* Count the successful haggles */
	if (price == minprice)
	{
		/* Just count the good haggles */
		if (st_ptr->good_buy < MAX_SHORT)
		{
			st_ptr->good_buy++;
		}
	}

	/* Count the failed haggles */
	else
	{
		/* Just count the bad haggles */
		if (st_ptr->bad_buy < MAX_SHORT)
		{
			st_ptr->bad_buy++;
		}
	}
}



/*
 * Re-displays a single store entry
 */
static void display_entry(int pos)
{
	int i, cur_col;
	object_type *o_ptr;
	s32b	x;

	char	o_name[80];
	char	out_val[160];


	int maxwid = 75;

	/* Get the item */
	o_ptr = &st_ptr->stock[pos];

	/* Get the "offset" */
	i = (pos % 12);

	/* Label it, clear the line --(-- */
	strnfmt(out_val, 160, "%c) ", I2A(i));
	c_prt(get_item_letter_color(o_ptr), out_val, i + 6, 0);


	cur_col = 3;
	if (show_store_graph)
	{
		byte a = object_attr(o_ptr);
		char c = object_char(o_ptr);

#ifdef AMIGA
		if (a & 0x80)
			a |= 0x40;
#endif

		if (!o_ptr->k_idx) c = ' ';

		Term_draw(cur_col, i + 6, a, c);
		if (use_bigtile)
		{
			cur_col++;
			if (a & 0x80)
				Term_draw(cur_col, i + 6, 255, 255);
		}
		cur_col += 2;
	}

	/* Describe an item in the home */
	if ((cur_store_num == 7) ||
	                (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM))
	{
		maxwid = 75;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 10;

		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';

		c_put_str(tval_to_attr[o_ptr->tval], o_name, i + 6, cur_col);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
			strnfmt(out_val, 160, "%3d.%d lb", wgt / 10, wgt % 10);
			put_str(out_val, i + 6, 68);
		}
	}

	/* Describe an item (fully) in a store */
	else
	{
		byte color = TERM_WHITE;

		/* Must leave room for the "price" */
		maxwid = 65;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 7;

		/* Describe the object (fully) */
		object_desc_store(o_name, o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';

		c_put_str(tval_to_attr[o_ptr->tval], o_name, i + 6, cur_col);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
			strnfmt(out_val, 160, "%3d.%d", wgt / 10, wgt % 10);
			put_str(out_val, i + 6, 61);
		}

		/* Display a "fixed" cost */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Can we buy one ? */
			if (x > p_ptr->au) color = TERM_L_DARK;

			/* Actually draw the price (not fixed) */
			strnfmt(out_val, 160, "%9ld F", (long)x);
			c_put_str(color, out_val, i + 6, 68);
		}

		/* Display a "taxed" cost */
		else if (auto_haggle && !p_ptr->nastytrap146)
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Hack -- Apply Sales Tax if needed */
			if (!noneedtobargain(x)) x += x / 10;

			/* Can we buy one ? */
			if (x > p_ptr->au) color = TERM_L_DARK;

			/* Actually draw the price (with tax) */
			strnfmt(out_val, 160, "%9ld  ", (long)x);
			c_put_str(color, out_val, i + 6, 68);
		}

		/* Display a "haggle" cost */
		else
		{
			/* Extrect the "maximum" price */
			x = price_item(o_ptr, ot_ptr->max_inflate, FALSE);

			/* Can we buy one ? */
			if (x > p_ptr->au) color = TERM_L_DARK;

			/* Actually draw the price (not fixed) */
			strnfmt(out_val, 160, "%9ld  ", (long)x);
			c_put_str(color, out_val, i + 6, 68);
		}
	}
}


/*
 * Displays a store's inventory 		-RAK-
 * All prices are listed as "per individual object".  -BEN-
 */
static void display_inventory(void)
{
	int i, k;

	/* Display the next 12 items */
	for (k = 0; k < 12; k++)
	{
		/* Do not display "dead" items */
		if (store_top + k >= st_ptr->stock_num) break;

		/* Display that line */
		display_entry(store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < 13; i++) prt("", i + 6, 0);

	/* Assume "no current page" */
	put_str("         ", 5, 20);

	/* Visual reminder of "more items" */
	if (st_ptr->stock_num > 12)
	{
		/* Show "more" reminder (after the last item) */
		prt("-more-", k + 6, 3);

		/* Indicate the "current page" */
		put_str(format("(Page %d) ", store_top / 12 + 1), 5, 20);
	}
}


/*
 * Displays players gold					-RAK-
 */
void store_prt_gold(void)
{
	char out_val[64];

	prt("Gold Remaining: ", 19, 53);

	strnfmt(out_val, 64, "%9ld", (long)p_ptr->au);
	prt(out_val, 19, 68);
}


/*
 * Displays store (after clearing screen)		-RAK-
 */
void display_store(void)
{
	char buf[80];


	/* Clear screen */
	Term_clear();

	/* The "Home" is special */
	if (cur_store_num == 7)
	{
		/* Put the owner name -- mega hack */
#if 0 /* DGDGDGDG -- use a skill */
		if ((cp_ptr->magic_key == MKEY_TELEKINESIS) &&
		                (p_ptr->town_num == TOWN_RANDOM)) put_str("Hole Contents", 3, 30);
		else
#endif
		if (four_dim_mode) {
			put_str("4D Pocket", 3, 30);
		} else {
			put_str("Your Home", 3, 30);
		}

		/* Label the item descriptions */
		put_str("Item Description", 5, 3);

		/* If showing weights, show label */
		if (show_weights)
		{
			put_str("Weight", 5, 70);
		}
	}

	else if (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM)
	{
		cptr store_name = (st_name + st_info[cur_store_num].name);

		/* Show the name of the store */
		strnfmt(buf, 80, "%s", store_name);
		prt(buf, 3, 30);

		/* Label the item descriptions */
		put_str("Item Description", 5, 3);

		/* If showing weights, show label */
		if (show_weights)
		{
			put_str("Weight", 5, 70);
		}
	}

	/* Normal stores */
	else
	{
		cptr store_name = (st_name + st_info[cur_store_num].name);
		cptr owner_name = (ow_name + ot_ptr->name);

		int maxprice;

		maxprice = ot_ptr->max_cost;
		maxprice += (st_investment * 100); /* reduction if investment is negative --Amy */
		if (maxprice < 1) maxprice = 1;

		/* Put the owner name and race */
		strnfmt(buf, 80, "%s", owner_name);
		put_str(buf, 3, 10);

		/* Show the max price in the store (above prices) */
		strnfmt(buf, 80, "%s (%ld, rank %d)", store_name, (long)maxprice, st_investment);
		prt(buf, 3, 50);

		/* Label the item descriptions */
		put_str("Item Description", 5, 3);

		/* If showing weights, show label */
		if (show_weights)
		{
			put_str("Weight", 5, 60);
		}

		/* Label the asking price (in stores) */
		put_str("Price", 5, 72);
	}

	/* Display the current gold */
	store_prt_gold();

	/* Draw in the inventory */
	display_inventory();
}



/*
 * Get the ID of a store item and return its value	-RAK-
 */
static int get_stock(int *com_val, cptr pmt, int i, int j)
{
	char	command;

	char	out_val[160];

#ifdef ALLOW_REPEAT /* TNB */

	/* Get the item index */
	if (repeat_pull(com_val))
	{

		/* Verify the item */
		if ((*com_val >= i) && (*com_val <= j))
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Assume failure */
	*com_val = ( -1);

	/* Build the prompt */
	strnfmt(out_val, 160, "(Items %c-%c, ESC to exit) %s",
	        I2A(i), I2A(j), pmt);

	/* Ask until done */
	while (TRUE)
	{
		int k;

		/* Escape */
		if (!get_com(out_val, &command)) break;

		/* Convert */
		k = (islower(command) ? A2I(command) : -1);

		/* Legal responses */
		if ((k >= i) && (k <= j))
		{
			*com_val = k;
			break;
		}

		/* Oops */
		bell();
	}

	/* Clear the prompt */
	prt("", 0, 0);

	/* Cancel */
	if (command == ESCAPE) return (FALSE);

#ifdef ALLOW_REPEAT /* TNB */

	repeat_push(*com_val);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


/*
 * Increase the insult counter and get angry if too many -RAK-
 */
static int increase_insults(void)
{
	/* Increase insults */
	st_ptr->insult_cur++;

	/* Become insulted */
	if (st_ptr->insult_cur > ot_ptr->insult_max)
	{
		/* Complain */
		say_comment_4();

		/* Reset insults */
		st_ptr->insult_cur = 0;
		st_ptr->good_buy = 0;
		st_ptr->bad_buy = 0;

		/* Open tomorrow */
		st_ptr->store_open = turn + 25000 + randint(25000);

		/* Closed */
		return (TRUE);
	}

	/* Not closed */
	return (FALSE);
}


/*
 * Decrease insults 				-RAK-
 */
static void decrease_insults(void)
{
	/* Decrease insults */
	if (st_ptr->insult_cur) st_ptr->insult_cur--;
}


/*
 * Have insulted while haggling 			-RAK-
 */
static int haggle_insults(void)
{
	/* Increase insults */
	if (increase_insults()) return (TRUE);

	/* Display and flush insult */
	say_comment_5();

	/* Still okay */
	return (FALSE);
}


/*
 * Mega-Hack -- Enable "increments"
 */
static bool allow_inc = FALSE;

/*
 * Mega-Hack -- Last "increment" during haggling
 */
static s32b last_inc = 0L;


/*
 * Get a haggle
 */
static int get_haggle(cptr pmt, s32b *poffer, s32b price, int final)
{
	s32b	i;

	cptr	p;

	char	buf[128];
	char	out_val[160];


	/* Clear old increment if necessary */
	if (!allow_inc) last_inc = 0L;


	/* Final offer */
	if (final)
	{
		strnfmt(buf, 128, "%s [accept] ", pmt);
	}

	/* Old (negative) increment, and not final */
	else if (last_inc < 0)
	{
		strnfmt(buf, 128, "%s [-%ld] ", pmt, (long)(ABS(last_inc)));
	}

	/* Old (positive) increment, and not final */
	else if (last_inc > 0)
	{
		strnfmt(buf, 128, "%s [+%ld] ", pmt, (long)(ABS(last_inc)));
	}

	/* Normal haggle */
	else
	{
		strnfmt(buf, 128, "%s ", pmt);
	}


	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Ask until done */
	while (TRUE)
	{
		/* Default */
		strcpy(out_val, "");

		/* Ask the user for a response */
		if (!get_string(buf, out_val, 32)) return (FALSE);

		/* Skip leading spaces */
		for (p = out_val; *p == ' '; p++) /* loop */;

		/* Empty response */
		if (*p == '\0')
		{
			/* Accept current price */
			if (final)
			{
				*poffer = price;
				last_inc = 0L;
				break;
			}

			/* Use previous increment */
			if (allow_inc && last_inc)
			{
				*poffer += last_inc;
				break;
			}
		}

		/* Normal response */
		else
		{
			/* Extract a number */
			i = atol(p);

			/* Handle "incremental" number */
			if ((*p == '+' || *p == '-'))
			{
				/* Allow increments */
				if (allow_inc)
				{
					/* Use the given "increment" */
					*poffer += i;
					last_inc = i;
					break;
				}
			}

			/* Handle normal number */
			else
			{
				/* Use the given "number" */
				*poffer = i;
				last_inc = 0L;
				break;
			}
		}

		/* Warning */
		msg_print("Invalid response.");
		msg_print(NULL);
	}

	/* Success */
	return (TRUE);
}


/*
 * Receive an offer (from the player)
 *
 * Return TRUE if offer is NOT okay
 */
static bool receive_offer(cptr pmt, s32b *poffer,
                          s32b last_offer, int factor,
                          s32b price, int final)
{
	/* Haggle till done */
	while (TRUE)
	{
		/* Get a haggle (or cancel) */
		if (!get_haggle(pmt, poffer, price, final)) return (TRUE);

		/* Acceptable offer */
		if (((*poffer) * factor) >= (last_offer * factor)) break;

		/* Insult, and check for kicked out */
		if (haggle_insults()) return (TRUE);

		/* Reject offer (correctly) */
		(*poffer) = last_offer;
	}

	/* Success */
	return (FALSE);
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool purchase_haggle(object_type *o_ptr, s32b *price)
{
	s32b	cur_ask, final_ask;
	s32b	last_offer, offer;
	s32b	x1, x2, x3;
	s32b	min_per, max_per;
	int flag, loop_flag, noneed;
	int annoyed = 0, final = FALSE;

	bool	cancel = FALSE;

	cptr	pmt = "Asking";

	char	out_val[160];


	*price = 0;


	/* Extract the starting offer and the final offer */
	cur_ask = price_item(o_ptr, ot_ptr->max_inflate, FALSE);
	final_ask = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* No need to haggle */
	if (noneed || (auto_haggle && !p_ptr->nastytrap146) )
	{
		/* No need to haggle */
		if (noneed)
		{
			/* Message summary */
			msg_print("You eventually agree upon the price.");
			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
			msg_print("You quickly agree upon the price.");
			msg_print(NULL);

			/* Apply Sales Tax */
			final_ask += final_ask / 10;
		}

		/* Final price */
		cur_ask = final_ask;

		/* Go to final offer */
		pmt = "Final Offer";
		final = TRUE;
	}


	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* Haggle parameters */
	min_per = ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value_shop(o_ptr) * o_ptr->number;
	last_offer = last_offer * (200 - (int)(ot_ptr->max_inflate)) / 100L;
	if (last_offer <= 0) last_offer = 1;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	/* Haggle until done */
	for (flag = FALSE; !flag; )
	{
		loop_flag = TRUE;

		while (!flag && loop_flag)
		{
			strnfmt(out_val, 160, "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
			cancel = receive_offer("What do you offer? ",
			                       &offer, last_offer, 1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer > cur_ask)
			{
				say_comment_6();
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}
		}

		if (!flag)
		{
			x1 = 100 * (offer - last_offer) / (cur_ask - last_offer);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1 - 2, x1 + 2);
			x3 = ((cur_ask - offer) * x2 / 100L) + 1;
			/* don't let the price go up */
			if (x3 < 0) x3 = 0;
			cur_ask -= x3;

			/* Too little */
			if (cur_ask < final_ask)
			{
				final = TRUE;
				cur_ask = final_ask;
				pmt = "Final Offer";
				annoyed++;
				if (annoyed > 3)
				{
					(void)(increase_insults());
					cancel = TRUE;
					flag = TRUE;
				}
			}
			else if (offer >= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
				strnfmt(out_val, 160, "Your last offer: %ld",
				        (long)last_offer);
				put_str(out_val, 1, 39);
				say_comment_2(cur_ask, annoyed);
			}
		}
	}

	/* Cancel */
	if (cancel) return (TRUE);

	/* Update bargaining info */
	updatebargain(*price, final_ask);

	/* Do not cancel */
	return (FALSE);
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool sell_haggle(object_type *o_ptr, s32b *price)
{
	s32b	purse, cur_ask, final_ask;
	s32b	last_offer = 0, offer = 0;
	s32b	x1, x2, x3;
	s32b	min_per, max_per;

	int flag, loop_flag, noneed;
	int annoyed = 0, final = FALSE;

	bool	cancel = FALSE;

	cptr	pmt = "Offer";

	char	out_val[160];


	*price = 0;


	/* Obtain the starting offer and the final offer */
	cur_ask = price_item(o_ptr, ot_ptr->max_inflate, TRUE);
	final_ask = price_item(o_ptr, ot_ptr->min_inflate, TRUE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* Get the owner's payout limit */
	purse = (s32b)(ot_ptr->max_cost);
	if (st_investment > 0) purse += (st_investment * 100);
	if (st_investment < 0) {
		purse += (st_investment * 100); /* reduction */
		if (purse < 1) purse = 1;
	}

	/* No need to haggle */
	if (noneed || (auto_haggle && !p_ptr->nastytrap146) || (final_ask >= purse))
	{
		/* No reason to haggle */
		if (final_ask >= purse)
		{
			/* Message */
			msg_print("You instantly agree upon the price.");
			msg_print(NULL);

			/* Offer full purse */
			final_ask = purse;
		}

		/* No need to haggle */
		else if (noneed)
		{
			/* Message */
			msg_print("You eventually agree upon the price.");
			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
			msg_print("You quickly agree upon the price.");
			msg_print(NULL);

			/* Apply Sales Tax */
			final_ask -= final_ask / 10;
		}

		/* Final price */
		cur_ask = final_ask;

		/* Final offer */
		final = TRUE;
		pmt = "Final Offer";
	}

	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* XXX XXX XXX Display commands */

	/* Haggling parameters */
	min_per = ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	last_offer = last_offer * ot_ptr->max_inflate / 100L;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	/* Haggle */
	for (flag = FALSE; !flag; )
	{
		while (1)
		{
			loop_flag = TRUE;

			strnfmt(out_val, 160, "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
			cancel = receive_offer("What price do you ask? ",
			                       &offer, last_offer, -1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer < cur_ask)
			{
				say_comment_6();
				/* rejected, reset offer for incremental haggling */
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}

			/* Stop */
			if (flag || !loop_flag) break;
		}

		if (!flag)
		{
			x1 = 100 * (last_offer - offer) / (last_offer - cur_ask);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1 - 2, x1 + 2);
			x3 = ((offer - cur_ask) * x2 / 100L) + 1;
			/* don't let the price go down */
			if (x3 < 0) x3 = 0;
			cur_ask += x3;

			if (cur_ask > final_ask)
			{
				cur_ask = final_ask;
				final = TRUE;
				pmt = "Final Offer";
				annoyed++;
				if (annoyed > 3)
				{
					flag = TRUE;
					(void)(increase_insults());
				}
			}
			else if (offer <= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
				strnfmt(out_val, 160,
				        "Your last bid %ld", (long)last_offer);
				put_str(out_val, 1, 39);
				say_comment_3(cur_ask, annoyed);
			}
		}
	}

	/* Cancel */
	if (cancel) return (TRUE);

	/* Update bargaining info */
	updatebargain(*price, final_ask);

	/* Do not cancel */
	return (FALSE);
}

/*
 * Will the owner retire?
 */
static bool retire_owner_p(void)
{
	store_info_type *sti_ptr = &st_info[town_info[p_ptr->town_num].store[cur_store_num].st_idx];

	if ((sti_ptr->owners[0] == sti_ptr->owners[1]) &&
	                (sti_ptr->owners[0] == sti_ptr->owners[2]) &&
	                (sti_ptr->owners[0] == sti_ptr->owners[3]))
	{
		/* there is no other owner */
		return FALSE;
	}

	if (rand_int(STORE_SHUFFLE) != 0)
	{
		return FALSE;
	}

	return TRUE;
}

/*
 * Stole an item from a store                   -DG-
 */
void store_stole(void)
{
	int i, amt;
	int item, item_new;

	object_type forge;
	object_type *j_ptr;

	object_type *o_ptr;

	char o_name[80];

	char out_val[160];

	if (cur_store_num == 7)
	{
		msg_print("You can't steal from your home!");
		return;
	}

	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		msg_print("There is no item to steal.");
		return;
	}


	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > 12) i = 12;

	/* Prompt */
	strnfmt(out_val, 160, "Which item do you want to steal? ");

	/* Get the item number to be bought */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Assume the player wants just one of them */
	amt = 1;

	/* Get local object */
	j_ptr = &forge;

	/* Get a copy of the object */
	object_copy(j_ptr, o_ptr);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
		msg_print("You cannot carry that many different items.");
		return;
	}

	/* Find out how many the player wants */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}

	/* Get local object */
	j_ptr = &forge;

	/* Get desired object */
	object_copy(j_ptr, o_ptr);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
		msg_print("You cannot carry that many items.");
		return;
	}

	/* Player tries to stole it */
	if ((rand_int((65 - p_ptr->stat_ind[A_DEX]) +
	                (( (j_ptr->weight + randint(5) ) * amt) / (5 + get_skill_scale(SKILL_STEALING, 15))) -
	                (get_skill_scale(SKILL_STEALING, 15))) <= 10) && (rand_int(10) > 0)) /* minimum failure rate --Amy */
	{
		/* Hack -- buying an item makes you aware of it */
		object_aware(j_ptr);

		/* Be aware of how you found it */
		j_ptr->found = OBJ_FOUND_STOLEN;
		j_ptr->found_aux1 = st_ptr->st_idx;

		/* Hack -- clear the "fixed" flag from the item */
		j_ptr->ident &= ~(IDENT_FIXED);

		/* "Hot" merchandise can't be sold back.  It doesn't make sense
		   to be able to sell back to a guy what you just stole from him.
		   Also, without the discount one could fairly easily macro himself
		   an infinite money supply */
		j_ptr->discount = 100;

		if (o_ptr->tval == TV_WAND)
		{
			j_ptr->pval = o_ptr->pval * amt / o_ptr->number;
			o_ptr->pval -= j_ptr->pval;
		}

		/* Describe the transaction */
		object_desc(o_name, j_ptr, TRUE, 3);

		/* Message */
		msg_format("You steal %s.", o_name);

		/* Erase the inscription */
		j_ptr->note = 0;

		/* Give it to the player */
		item_new = inven_carry(j_ptr, FALSE);

		/* Describe the final result */
		object_desc(o_name, &p_ptr->inventory[item_new], TRUE, 3);

		/* Message */
		msg_format("You have %s (%c).",
		           o_name, index_to_label(item_new));

		/* Handle stuff */
		handle_stuff();

		/* Note how many slots the store used to have */
		i = st_ptr->stock_num;

		/* Remove the bought items from the store */
		store_item_increase(item, -amt);
		store_item_optimize(item);

		/* Store is empty */
		if (st_ptr->stock_num == 0)
		{
			/* Shuffle */
			if (retire_owner_p())
			{
				/* Message */
				msg_print("The shopkeeper retires.");

				/* Shuffle the store */
				store_shuffle(cur_store_num);
			}

			/* Maintain */
			else
			{
				/* Message */
				msg_print("The shopkeeper brings out some new stock.");
			}

			/* New inventory */
			for (i = 0; i < 10; i++)
			{
				/* Maintain the store */
				store_maint(p_ptr->town_num, cur_store_num);
			}

			/* Start over */
			store_top = 0;

			/* Redraw everything */
			display_inventory();
		}

		/* The item is gone */
		else if (st_ptr->stock_num != i)
		{
			/* Pick the correct screen */
			if (store_top >= st_ptr->stock_num) store_top -= 12;

			/* Redraw everything */
			display_inventory();
		}

		/* Item is still here */
		else
		{
			/* Redraw the item */
			display_entry(item);
		}
	}
	else
	{
		/* Complain */
		say_comment_4();

		/* Reset insults */
		st_ptr->insult_cur = 0;
		st_ptr->good_buy = 0;
		st_ptr->bad_buy = 0;

		/* Kicked out for a LONG time */
		st_ptr->store_open = turn + 500000 + randint(500000);
		if (st_ptr->investment > -1000) {
			if (is_state(st_ptr, STORE_HATED)) st_ptr->investment -= 10;
			st_ptr->investment -= 10;
		}
	}

	/* Not kicked out */
	return;
}

/*
 * Buy an item from a store 			-RAK-
 */
void store_purchase(void)
{
	int i, amt = 1, choice;
	int item, item_new;

	s32b price, best;

	object_type forge;
	object_type *j_ptr;

	object_type *o_ptr;

	char o_name[80];

	char out_val[160];

	/* Museum? */
	if (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM)
	{
		msg_print("You cannot take items from the museum!");
		return;
	}

	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (cur_store_num == 7) msg_print("Your home is empty.");
		else msg_print("I am currently out of stock.");
		return;
	}

	if (four_dim_mode) {
		if ((wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].terrain_idx != TERRAIN_TOWN && wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].terrain_idx <= TERRAIN_MOUNTAIN) || (dun_level > 0) ) {
			msg_print("Taking items out of the 4D pocket only works while in a town!");
			return;
		}
	}

	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > 12) i = 12;

	/* Prompt */
	if (cur_store_num == 7)
	{
		strnfmt(out_val, 160, "Which item do you want to take? ");
	}
	else
	{
		strnfmt(out_val, 160, "Which item are you interested in? ");
	}

	/* Get the item number to be bought */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Get local object */
	j_ptr = &forge;

	/* Get a copy of one object to determine the price */
	object_copy(j_ptr, o_ptr);

	/* Modify quantity */
	j_ptr->number = 1;

	/* Hack -- If a wand, allocate the number of charges of one wand */
	if (j_ptr->tval == TV_WAND)
	{
		j_ptr->pval = o_ptr->pval / o_ptr->number;
	}

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
		msg_print("You cannot carry that many different items.");
		return;
	}

	/* Determine the "best" price (per item) */
	best = price_item(j_ptr, ot_ptr->min_inflate, FALSE);

	/* Find out how many the player wants */
	if (o_ptr->number > 1)
	{
		s32b q;


		/* Hack -- note cost of "fixed" items */
		if ((cur_store_num != 7) && (o_ptr->ident & (IDENT_FIXED)))
		{
			msg_format("That costs %ld gold per item.", (long)(best));
		}

		/* How many can we buy ? 99 if price is 0*/
		if (cur_store_num == STORE_HOME)
		{
			q = 99;
		}
		else if (best == 0)
		{
			q = 99;
		}
		else
		{
			if (auto_haggle && !p_ptr->nastytrap146)
				q = p_ptr->au / (best + (best / 10));
			else
				q = p_ptr->au / best;
		}
		if (o_ptr->number < q)
			q = o_ptr->number;

		/* None ? ahh too bad */
		if (!q)
		{
			msg_print("You do not have enough gold to buy one.");
			return;
		}

		/* Get a quantity */
		amt = get_quantity(NULL, q);

		/* Allow user abort */
		if (amt <= 0) return;
	}

	/* Get local object */
	j_ptr = &forge;

	/* Get desired object */
	object_copy(j_ptr, o_ptr);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf. -LM-
	 */
	if (o_ptr->tval == TV_WAND)
	{
		j_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
		msg_print("You cannot carry that many items.");
		return;
	}

	/* Attempt to buy it */
	if (cur_store_num != 7)
	{
		/* Fixed price, quick buy */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Assume accept */
			choice = 0;

			/* Go directly to the "best" deal */
			price = (best * j_ptr->number);
		}

		/* Haggle for it */
		else
		{
			/* Describe the object (fully) */
			object_desc_store(o_name, j_ptr, TRUE, 3);

			/* Message */
			msg_format("Buying %s (%c).", o_name, I2A(item));
			msg_print(NULL);

			/* Haggle for a final price */
			choice = purchase_haggle(j_ptr, &price);

			/* Hack -- Got kicked out */
			if (st_ptr->store_open >= turn) return;
		}


		/* Player wants it */
		if (choice == 0)
		{
			/* Fix the item price (if "correctly" haggled) */
			if (price == (best * j_ptr->number)) o_ptr->ident |= (IDENT_FIXED);

			/* Player can afford it */
			if (p_ptr->au >= price)
			{
				/* Say "okay" */
				say_comment_1();

				/* Make a sound */
				sound(SOUND_BUY);

				/* Be happy */
				decrease_insults();

				/* Spend the money */
				p_ptr->au -= price;

				/* Update the display */
				store_prt_gold();

				/* Hack -- buying an item makes you aware of it */
				object_aware(j_ptr);

				/* Be aware of how you found it */
				j_ptr->found = OBJ_FOUND_STORE;
				j_ptr->found_aux1 = st_ptr->st_idx;

				/* Hack -- clear the "fixed" flag from the item */
				j_ptr->ident &= ~(IDENT_FIXED);

				/* Describe the transaction */
				object_desc(o_name, j_ptr, TRUE, 3);

				/* Message */
				msg_format("You bought %s for %ld gold.", o_name, (long)price);

				/* Erase the inscription */
				j_ptr->note = 0;

				/* Hack -- If a rod or wand, allocate total maximum
				 * timeouts or charges between those picked up and 
				 * those left behind. -LM-
				 */
				if (o_ptr->tval == TV_WAND)
				{
					j_ptr->pval = o_ptr->pval * amt / o_ptr->number;
					o_ptr->pval -= j_ptr->pval;
				}

				/* Give it to the player */
				item_new = inven_carry(j_ptr, FALSE);

				/* Describe the final result */
				object_desc(o_name, &p_ptr->inventory[item_new], TRUE, 3);

				/* Message */
				msg_format("You have %s (%c).",
				           o_name, index_to_label(item_new));

				/* Handle stuff */
				handle_stuff();

				/* Note how many slots the store used to have */
				i = st_ptr->stock_num;

				/* Remove the bought items from the store */
				store_item_increase(item, -amt);
				store_item_optimize(item);

				/* Store is empty */
				if (st_ptr->stock_num == 0)
				{
					/* Shuffle */
					if (retire_owner_p())
					{
						/* Message */
						msg_print("The shopkeeper retires.");

						/* Shuffle the store */
						store_shuffle(cur_store_num);
					}

					/* Maintain */
					else
					{
						/* Message */
						msg_print("The shopkeeper brings out some new stock.");
					}

					/* New inventory */
					for (i = 0; i < 10; i++)
					{
						/* Maintain the store */
						store_maint(p_ptr->town_num, cur_store_num);
					}

					/* Start over */
					store_top = 0;
				}

				/* The item is gone */
				else if (st_ptr->stock_num != i)
				{
					/* Pick the correct screen */
					if (store_top >= st_ptr->stock_num) store_top -= 12;
				}

				/* Redraw everything */
				display_inventory();
			}

			/* Player cannot afford it */
			else
			{
				/* Simple message (no insult) */
				msg_print("You do not have enough gold.");
			}
		}
	}

	/* Home is much easier */
	else
	{
		/* Hack -- If a rod or wand, allocate total maximum
		 * timeouts or charges between those picked up and 
		 * those left behind. -LM-
		 */
		if (o_ptr->tval == TV_WAND)
		{
			j_ptr->pval = o_ptr->pval * amt / o_ptr->number;
			o_ptr->pval -= j_ptr->pval;
		}

		/* Give it to the player */
		item_new = inven_carry(j_ptr, FALSE);

		/* Describe just the result */
		object_desc(o_name, &p_ptr->inventory[item_new], TRUE, 3);

		/* Message */
		msg_format("You have %s (%c).", o_name, index_to_label(item_new));

		/* Handle stuff */
		handle_stuff();

		/* Take note if we take the last one */
		i = st_ptr->stock_num;

		/* Remove the items from the home */
		store_item_increase(item, -amt);
		store_item_optimize(item);

		/* Hack -- Item is still here */
		if (i == st_ptr->stock_num)
		{
			/* Redraw the item */
			display_entry(item);
		}

		/* The item is gone */
		else
		{
			/* Nothing left */
			if (st_ptr->stock_num == 0) store_top = 0;

			/* Nothing left on that screen */
			else if (store_top >= st_ptr->stock_num) store_top -= 12;

			/* Redraw everything */
			display_inventory();
		}
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an item to the store (or home)
 */
void store_sell(void)
{
	int choice;
	int item, item_pos;
	int amt;

	s32b price, value, dummy;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr q, s;

	char o_name[80];

	u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

	bool museum = (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM) ? TRUE : FALSE;

	/* Prepare a prompt */
	if (cur_store_num == 7) q = "Drop which item? ";
	else if (museum) q = "Donate which item?";
	else q = "Sell which item? ";

	/* Only allow items the store will buy */
	item_tester_hook = store_will_buy;

	/* Get an item */
	if (cur_store_num == STORE_HOME)
	{
		s = "You have nothing to drop. There needs to be a very looooooong message now so that your next keystroke isn't misinterpreted as something you didn't want to do at all, so as to force a --More--.";
	}
	else if (museum)
	{
		s = "You have nothing to donate. There needs to be a very looooooong message now so that your next keystroke isn't misinterpreted as something you didn't want to do at all, so as to force a --More--.";
	}
	else
	{
		s = "You have nothing that I want. There needs to be a very looooooong message now so that your next keystroke isn't misinterpreted as something you didn't want to do at all, so as to force a --More--.";
	}
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) return;

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


	object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

	/* Hack -- Cannot remove cursed items */
	if (cursed_p(o_ptr))
	{
		if (item >= INVEN_WIELD)
		{
			/* Oops */
			msg_print("Hmmm, it seems to be cursed.");

			/* Nope */
			return;
		}
		else
		{
			if ((f4 & TR4_CURSE_NO_DROP) || p_ptr->nastytrap102)
			{
				/* Oops */
				msg_print("Hmmm, you seem to be unable to drop it.");

				/* Nope */
				return;
			}
		}
	}


	/* Hack -- Cannot put a portable hole into home */
#if 0 /* DGDGDGDG -- use a skill */
	if ((cp_ptr->magic_key == MKEY_TELEKINESIS) &&
	                (o_ptr->tval == TV_TOOL) && (o_ptr->sval == SV_PORTABLE_HOLE))
	{
		msg_print("Putting it into your home has extra-dimensional problems");

		return;
	}
#endif

	/* Assume one item */
	amt = 1;

	/* Find out how many the player wants (letter means "all") */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}

	/* Get local object */
	q_ptr = &forge;

	/* Get a copy of the object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = amt;

	/* Hack -- If a rod or wand, allocate total maximum
	 * timeouts or charges to those being sold. -LM-
	 */
	if (o_ptr->tval == TV_WAND)
	{
		q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

	/* Get a full description */
	object_desc(o_name, q_ptr, TRUE, 3);

	/* Remove any inscription for stores */
	if ((cur_store_num != 7) && !museum)
	{
		q_ptr->note = 0;
	}

	/* Is there room in the store (or the home?) */
	if (!store_check_num(q_ptr))
	{
		if (cur_store_num == 7) msg_print("Your home is full.");
		else if (museum) msg_print("The museum is full.");
		else msg_print("I have not the room in my store to keep it.");
		return;
	}

	if (four_dim_mode) { /* four-dimensional */

		int weightmax = get_skill(SKILL_FOUR_DIM) * 10;

		if (!store_check_fourdim(q_ptr)) {
			msg_print("Your 4D pocket is full.");
			return;
		}

		if (o_ptr->weight > weightmax) {
			msg_format("The container can only hold items up to a weight of %d pounds.", weightmax / 10);
			return;
		}
	}


	/* Real store */
	if ((cur_store_num != 7) && !museum)
	{
		/* Describe the transaction */
		msg_format("Selling %s (%c).", o_name, index_to_label(item));
		msg_print(NULL);

		/* Haggle for it */
		choice = sell_haggle(q_ptr, &price);

		/* Kicked out */
		if (st_ptr->store_open >= turn) return;

		/* Sold... */
		if (choice == 0)
		{
			/* Say "okay" */
			say_comment_1();

			/* Make a sound */
			sound(SOUND_SELL);

			/* Be happy */
			decrease_insults();

			/* Get some money */
			p_ptr->au += price;

			/* Update the display */
			store_prt_gold();

			/* Get the "apparent" value */
			dummy = object_value(q_ptr) * q_ptr->number;

			/* Identify original item */
			object_aware(o_ptr);
			object_known(o_ptr);

			/* Combine / Reorder the pack (later) */
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

			/* Get local object */
			q_ptr = &forge;

			/* Get a copy of the object */
			object_copy(q_ptr, o_ptr);

			/* Modify quantity */
			q_ptr->number = amt;

			/*
			 * Hack -- If a rod or wand, let the shopkeeper know just 
			 * how many charges he really paid for. -LM-
			 */
			if (o_ptr->tval == TV_WAND)
			{
				q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
			}

			/* Get the "actual" value */
			value = object_value(q_ptr) * q_ptr->number;

			/* Get the description all over again */
			object_desc(o_name, q_ptr, TRUE, 3);

			/* Describe the result (in message buffer) */
			msg_format("You sold %s for %ld gold.", o_name, (long)price);

			/* Analyze the prices (and comment verbally) */
			purchase_analyze(price, value, dummy);

			/*
			 * Hack -- Allocate charges between those wands or rods sold 
			 * and retained, unless all are being sold. -LM-
			 */
			if (o_ptr->tval == TV_WAND)
			{
				q_ptr->pval = o_ptr->pval * amt / o_ptr->number;

				if (o_ptr->number > amt) o_ptr->pval -= q_ptr->pval;
			}

			/* Take the item from the player, describe the result */
			inven_item_increase(item, -amt);
			inven_item_describe(item);
			inven_item_optimize(item);

			/* Handle stuff */
			handle_stuff();

			/* The store gets that (known) item */
			item_pos = store_carry(q_ptr);

			/* Re-display if item is now in store */
			if (item_pos >= 0)
			{
				store_top = (item_pos / 12) * 12;
				display_inventory();
			}
		}
	}

	/* Player is at museum */
	else if (museum)
	{
		char o2_name[80];
		object_desc(o2_name, q_ptr, TRUE, 0);

		msg_print("Once you donate something, you cannot take it back.");
		if (!get_check(format("Do you really want to donate %s?", o2_name))) return;

		/* Identify it */
		object_aware(q_ptr);
		object_known(q_ptr);
		q_ptr->ident |= IDENT_MENTAL;

		/*
		 * Hack -- Allocate charges between those wands or rods sold 
		 * and retained, unless all are being sold. -LM-
		 */
		if (o_ptr->tval == TV_WAND)
		{
			q_ptr->pval = o_ptr->pval * amt / o_ptr->number;

			if (o_ptr->number > amt) o_ptr->pval -= q_ptr->pval;
		}


		/* Describe */
		msg_format("You donate %s (%c).", o_name, index_to_label(item));

		choice = 0;

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(q_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			store_top = (item_pos / 12) * 12;
			display_inventory();
		}
	}

	/* Player is at home */
	else
	{
		/* Describe */
		msg_format("You drop %s (%c).", o_name, index_to_label(item));

		/*
		 * Hack -- Allocate charges between those wands or rods sold 
		 * and retained, unless all are being sold. -LM-
		 */
		if (o_ptr->tval == TV_WAND)
		{
			q_ptr->pval = o_ptr->pval * amt / o_ptr->number;

			if (o_ptr->number > amt) o_ptr->pval -= q_ptr->pval;
		}

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(q_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			store_top = (item_pos / 12) * 12;
			display_inventory();
		}
	}
}



/*
 * Examine an item in a store			   -JDL-
 */
void store_examine(void)
{
	int i;
	int item;

	object_type *o_ptr;

	char o_name[80];

	char out_val[160];


	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (cur_store_num == 7) msg_print("Your home is empty.");
		else if (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM) msg_print("The museum is empty.");
		else msg_print("I am currently out of stock.");
		return;
	}


	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > 12) i = 12;

	/* Prompt */
	strnfmt(out_val, 160, "Which item do you want to examine? ");

	/* Get the item number to be examined */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Debug hack */
	if (wizard)
	{
		drop_near(o_ptr, -1, p_ptr->py, p_ptr->px);
	}

	/* Require full knowledge */
	if (!(o_ptr->ident & (IDENT_MENTAL)))
	{
		/* This can only happen in the home */
		msg_print("You have no special knowledge about that item.");
		return;
	}

	/* Description */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Describe */
	msg_format("Examining %s...", o_name);

	/* Show the object's powers. */
	if (!object_out_desc(o_ptr, NULL, FALSE, TRUE))
	{
		msg_print("You see nothing special.");
	}

	/* Show spell listing for instruments, daemonwear and spellbooks. */
	if ((o_ptr->tval == TV_INSTRUMENT) || (o_ptr->tval == TV_DAEMON_BOOK)
	    || (o_ptr->tval == TV_BOOK))
	{
		do_cmd_browse_aux(o_ptr);
	}

	return;
}





/*
 * Hack -- set this to leave the store
 */
static bool leave_store = FALSE;


/*
 * Process a command in a store
 *
 * Note that we must allow the use of a few "special" commands
 * in the stores which are not allowed in the dungeon, and we
 * must disable some commands which are allowed in the dungeon
 * but not in the stores, to prevent chaos.
 */
static bool store_process_command(void)
{
	bool validcmd = FALSE;
	int i;
	store_action_type *ba_ptr;
	bool recreate = FALSE;

#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	for (i = 0; i < 6; i++)
	{
		ba_ptr = &ba_info[st_info[st_ptr->st_idx].actions[i]];

		if (ba_ptr->letter)
		{
			if (ba_ptr->letter == command_cmd)
			{
				validcmd = TRUE;
				break;
			}
		}
		if (ba_ptr->letter_aux)
		{
			if (ba_ptr->letter_aux == command_cmd)
			{
				validcmd = TRUE;
				break;
			}
		}
	}

	if (validcmd)
	{
		recreate = bldg_process_command(st_ptr, i);
	}
	else
	{
		/* Parse the command */
		switch (command_cmd)
		{
			/* Leave */
		case ESCAPE:
			{
				leave_store = TRUE;
				break;
			}

			/* Browse */
		case ' ':
			{
				if (st_ptr->stock_num <= 12)
				{
					msg_print("Entire inventory is shown.");
				}
				else
				{
					store_top += 12;
					if (store_top >= st_ptr->stock_num) store_top = 0;
					display_inventory();
				}
				break;
			}

			/* Browse backwards */
		case '-':
			{
				if (st_ptr->stock_num <= 12)
				{
					msg_print("Entire inventory is shown.");
				}
				else
				{
					store_top -= 12;
					if (store_top < 0)
					{
						store_top = ((st_ptr->stock_num - 1) / 12) * 12;
					}
					display_inventory();
				}
			}

			/* Redraw */
		case KTRL('R'):
			{
				do_cmd_redraw();
				display_store();
				break;
			}

			/* Ignore return */
		case '\r':
			{
				break;
			}



			/*** Inventory Commands ***/

			/* Wear/wield equipment */
		case 'w':
			{
				do_cmd_wield();
				break;
			}

			/* Take off equipment */
		case 't':
			{
				do_cmd_takeoff();
				break;
			}

			/* Destroy an item */
		case 'k':
			{
				do_cmd_destroy();
				break;
			}

			/* Equipment list */
		case 'e':
			{
				do_cmd_equip();
				break;
			}

			/* Inventory list */
		case 'i':
			{
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



			/*** Use various objects ***/

			/* Browse a book */
		case 'b':
			{
				do_cmd_browse();
				break;
			}

			/* Inscribe an object */
		case '{':
			{
				do_cmd_inscribe();
				break;
			}

			/* Uninscribe an object */
		case '}':
			{
				do_cmd_uninscribe();
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
				display_store();
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

			/* Check artifacts, uniques etc. */
		case '~':
		case '|':
			{
				do_cmd_knowledge();
				break;
			}

			/* Load "screen dump" */
		case '(':
			{
				do_cmd_load_screen();
				break;
			}

			/* Save "screen dump" */
		case ')':
			{
				do_cmd_save_screen();
				break;
			}


			/* Hack -- Unknown command */
		default:
			{
				if (st_ptr->st_idx == STORE_HOME)
					msg_print("That command does not work in this home.");
				else
					msg_print("That command does not work in this store.");
				break;
			}
		}
	}

	return recreate;
}


/*
 * Enter a store, and interact with it.
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 */
void do_cmd_store(void)
{
	int which;
	int maintain_num;
	int tmp_chr;
	int i;
	bool recreate = FALSE;

	cave_type *c_ptr;


	/* Access the player grid */
	c_ptr = &cave[p_ptr->py][p_ptr->px];

	/* Verify a store */
	if (c_ptr->feat != FEAT_SHOP)
	{
		msg_print("You see no store here.");
		return;
	}

	/* Extract the store code */
	which = c_ptr->special;

	/* storelock nastytraps: everything except the home is locked */
	if (p_ptr->nastytrap77 && which != 7) {
		msg_print("The doors are locked.");
		return;
	}

	if (p_ptr->nastytrap108 && which == 7) {
		msg_print("Your home is being renovated! Come back later!");
		return;
	}

	/* Hack -- Check the "locked doors" */
	if (town_info[p_ptr->town_num].store[which].store_open >= turn)
	{
		msg_print("The doors are locked.");
		return;
	}

	/* Calculate the number of store maintainances since the last visit */
	maintain_num = (turn - town_info[p_ptr->town_num].store[which].last_visit) / (10L * STORE_TURNS);

	/* Maintain the store max. 10 times */
	if (maintain_num > 10) maintain_num = 10;

	if (maintain_num)
	{
		/* Maintain the store */
		for (i = 0; i < maintain_num; i++)
			store_maint(p_ptr->town_num, which);

		/* Save the visit */
		town_info[p_ptr->town_num].store[which].last_visit = turn;
	}

	/* Forget the lite */
	/* forget_lite(); */

	/* Forget the view */
	forget_view();


	/* Hack -- Character is in "icky" mode */
	character_icky = TRUE;


	/* No command argument */
	command_arg = 0;

	/* No repeated command */
	command_rep = 0;

	/* No automatic command */
	command_new = 0;


	/* Save the store number */
	cur_store_num = which;

	/* Save the store and owner pointers */
	st_ptr = &town_info[p_ptr->town_num].store[cur_store_num];
	ot_ptr = &ow_info[st_ptr->owner];
	st_investment = st_ptr->investment;

	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store();

	/* Mega-Hack -- Ignore keymaps on store action letters */
	for (i = 0; i < 6; i++)
	{
		store_action_type *ba_ptr =
			&ba_info[st_info[st_ptr->st_idx].actions[i]];
		request_command_ignore_keymaps[2*i] = ba_ptr->letter;
		request_command_ignore_keymaps[2*i+1] = ba_ptr->letter_aux;

	}

	/* Do not leave */
	leave_store = FALSE;

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Hack -- Check the charisma */
		tmp_chr = p_ptr->stat_use[A_CHR];

		/* Clear */
		clear_from(21);


		/* Basic commands */
		c_prt(TERM_YELLOW, " ESC.", 22, 0);
		prt(") Exit.", 22, 4);

		/* Browse if necessary */
		if (st_ptr->stock_num > 12)
		{
			c_prt(TERM_YELLOW, " SPACE", 23, 0);
			prt(") Next page", 23, 6);
		}

		/* Prompt */
		prt("You may: ", 21, 0);

		/* Show the commands */
		show_building(st_ptr);

		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		if (store_process_command()) recreate = TRUE;

		/* Hack -- Character is still in "icky" mode */
		character_icky = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/* XXX XXX XXX Pack Overflow */
		if (p_ptr->inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			object_type *o_ptr = &p_ptr->inventory[item];

			/* Hack -- Flee from the store */
			if (cur_store_num != 7)
			{
				/* Message */
				msg_print("Your pack is so full that you flee the store...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if (!store_check_num(o_ptr))
			{
				/* Message */
				msg_print("Your pack is so full that you flee your home...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type forge;
				object_type *q_ptr;

				char o_name[80];


				/* Give a message */
				msg_print("Your pack overflows!");

				/* Get local object */
				q_ptr = &forge;

				/* Grab a copy of the item */
				object_copy(q_ptr, o_ptr);

				/* Describe it */
				object_desc(o_name, q_ptr, TRUE, 3);

				/* Message */
				msg_format("You drop %s (%c).", o_name, index_to_label(item));

				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

				/* Let the home carry it */
				item_pos = home_carry(q_ptr);

				/* Redraw the home */
				if (item_pos >= 0)
				{
					store_top = (item_pos / 12) * 12;
					display_inventory();
				}
			}
		}

		/* Hack -- Redisplay store prices if charisma changes */
		if (tmp_chr != p_ptr->stat_use[A_CHR]) display_inventory();

		/* Hack -- get kicked out of the store */
		if (st_ptr->store_open >= turn) leave_store = TRUE;
	}

	/* Free turn XXX XXX XXX */
	energy_use = 0;

	/* Recreate the level only when needed */
	if (recreate)
	{
		/* Reinit wilderness to activate quests ... */
		p_ptr->oldpx = p_ptr->px;
		p_ptr->oldpy = p_ptr->py;

		p_ptr->leaving = TRUE;
	}

	/* Hack -- Character is no longer in "icky" mode */
	character_icky = FALSE;


	/* Hack -- Cancel automatic command */
	command_new = 0;

	/* Hack -- Cancel "see" mode */
	command_see = FALSE;

	/* Mega-Hack -- Clear the 'ignore-keymaps' list */
	memset(request_command_ignore_keymaps, 0, 12);

	/* Flush messages XXX XXX XXX */
	msg_print(NULL);


	/* Clear the screen */
	Term_clear();


	/* Update everything */
	p_ptr->update |= (PU_VIEW | PU_MON_LITE);
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);
}



/*
 * Shuffle one of the stores.
 */
void store_shuffle(int which)
{
	int i, j;


	/* Ignore home */
	if (which == STORE_HOME) return;

	/* Ignoer Museum */
	if (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM) return;


	/* Save the store index */
	cur_store_num = which;

	/* Activate that store */
	st_ptr = &town_info[p_ptr->town_num].store[cur_store_num];

	/* Pick a new owner */
	for (j = st_ptr->owner; j == st_ptr->owner; )
	{
		st_ptr->owner = st_info[st_ptr->st_idx].owners[rand_int(4)];
	}

	/* Activate the new owner */
	ot_ptr = &ow_info[st_ptr->owner];


	/* Reset the owner data */
	st_ptr->insult_cur = 0;
	st_ptr->store_open = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* Lose one rank of investment (resetting it to zero would be too harsh --Amy) */
	if (st_ptr->investment > 0) st_ptr->investment--;
	st_investment = st_ptr->investment;

	/* Hack -- discount all the items */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		object_type *o_ptr;

		/* Get the item */
		o_ptr = &st_ptr->stock[i];

		/* Hack -- Sell all old items for "half price" */
		if (!(o_ptr->art_name))
			o_ptr->discount = 50;

		/* Hack -- Items are no longer "fixed price" */
		o_ptr->ident &= ~(IDENT_FIXED);

		/* Mega-Hack -- Note that the item is "on sale" */
		o_ptr->note = quark_add("on sale");
	}
}


/*
 * Maintain the inventory at the stores.
 */
void store_maint(int town_num, int store_num)
{
	int j, tries = 200;

	int stocksizetemp;
	int storeabsolutemax;

	int maxkeep = STORE_MAX_KEEP; /* 120 */
	switch (randint(10)) {
		case 1:
			maxkeep = 12;
			break;
		case 2:
			maxkeep = 15;
			break;
		case 3:
			maxkeep = 18;
			break;
		case 4:
			maxkeep = 24;
			break;
		case 5:
			maxkeep = 30;
			break;
		case 6:
			maxkeep = 40;
			break;
		case 7:
			maxkeep = 60;
			break;
		case 8:
			maxkeep = 80;
			break;
		case 9:
			maxkeep = 90;
			break;
		case 10:
			break;
	}

	/*if (st_info[st_ptr->st_idx].flags1 & SF1_ALL_ITEM) tries = 500;*/ /* crash because st_ptr not set! --Amy */
	/* But unneccessary since the black market gets enough items even without this line. :-) */

	int old_rating = rating;

	cur_store_num = store_num;

	/* Ignore home */
	if (store_num == STORE_HOME) return;

	/* Activate that store */
	st_ptr = &town_info[town_num].store[store_num];

	/* Ignoer Museum */
	if (st_info[st_ptr->st_idx].flags1 & SF1_MUSEUM) return;

	/* Activate the owner */
	ot_ptr = &ow_info[st_ptr->owner];
	st_investment = st_ptr->investment;

	/* Store keeper forgives the player */
	st_ptr->insult_cur = 0;

	/* Mega-Hack -- prune the black market */
	/*if (st_info[st_ptr->st_idx].flags1 & SF1_ALL_ITEM)
	{*/
		/* Destroy crappy black market items */
		/*for (j = st_ptr->stock_num - 1; j >= 0; j--)
		{
			object_type *o_ptr = &st_ptr->stock[j];*/

			/* Destroy crappy items */
			/*if (black_market_crap(o_ptr))
			{*/
				/* Destroy the item */
				/*store_item_increase(j, 0 - o_ptr->number);
				store_item_optimize(j);
			}
		}
	}*/


	/* Choose the number of slots to keep */
	j = st_ptr->stock_num;

	/* Sell a few items */
	j = j - randint(STORE_TURNOVER);

	/* stores with little stock size have lower amounts of items --Amy */
	if (maxkeep > (st_ptr->stock_size + st_ptr->investment)) {
		maxkeep = (st_ptr->stock_size + st_ptr->investment);
		if (maxkeep < 1) maxkeep = 1;
	}

	/* well-invested shops can have more stuff --Amy */
	if (st_ptr->investment > 4) {
		maxkeep += rand_int((st_ptr->investment / 5) + 1);
	}

	/* Never keep more than "STORE_MAX_KEEP" slots
	 * Amy edit: use a more randomized variable for this */
	if (j > maxkeep) j = maxkeep;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "underflow" */
	if (j < 0) j = 0;

	/* Destroy objects until only "j" slots are left */
	while (st_ptr->stock_num > j) store_delete();

	/* Choose the number of slots to fill */
	j = st_ptr->stock_num;

	/* Buy some more items */
	j = j + randint(STORE_TURNOVER);

	/* but if the shop's stock size is low, then it'll buy less of them --Amy */
	if (j > (st_ptr->stock_size + st_ptr->investment)) {
		j = (st_ptr->stock_size + st_ptr->investment);
		if (j < 1) j = 1;
	}

	if (st_info[st_ptr->st_idx].flags1 & SF1_ALL_ITEM) {
		j = j + randint(STORE_TURNOVER);
	}

	/* and a bigger amount if the store is well-invested --Amy */
	if (st_ptr->investment > 4) {
		j += randint(st_ptr->investment / 5);
	}

	/* absolute max of 120 items, but can stock more with high rank --Amy */
	storeabsolutemax = STORE_MAX_KEEP;
	if (st_ptr->investment > 0) {
		storeabsolutemax += rand_int(st_ptr->investment + 1);
		if (storeabsolutemax > 255) storeabsolutemax = 255;
	}

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > storeabsolutemax) j = storeabsolutemax;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	stocksizetemp = st_ptr->stock_size;
	if (st_ptr->investment > 0) stocksizetemp += randint(st_ptr->investment);
	if (st_ptr->investment < 0) stocksizetemp += st_ptr->investment; /* reduction */
	if (stocksizetemp < 1) stocksizetemp = 1;

	/* Hack -- prevent "overflow" */
	if (j >= stocksizetemp) j = stocksizetemp - 1;

	/* Acquire some new items */
	while ((st_ptr->stock_num < j) && tries)
	{
		store_create();
		tries--;
	}

	/* Hack -- Restore the rating */
	rating = old_rating;
}


/*
 * Initialize the stores
 */
void store_init(int town_num, int store_num)
{
	int k;

	cur_store_num = store_num;

	/* Activate that store */
	st_ptr = &town_info[town_num].store[store_num];


	/* Pick an owner */
	st_ptr->owner = st_info[st_ptr->st_idx].owners[rand_int(4)];

	/* Activate the new owner */
	ot_ptr = &ow_info[st_ptr->owner];

	/* Initialize the store */
	st_ptr->store_open = 0;
	st_ptr->insult_cur = 0;
	st_ptr->investment = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	st_ptr->investment = 0;

	/* Nothing in stock */
	st_ptr->stock_num = 0;

	/*
	 * MEGA-HACK - Last visit to store is
	 * BEFORE player birth to enable store restocking
	 */
	st_ptr->last_visit = -100L * STORE_TURNS;

	/* Clear any old items */
	for (k = 0; k < st_ptr->stock_size; k++)
	{
		object_wipe(&st_ptr->stock[k]);
	}
}


void move_to_black_market(object_type * o_ptr)
{
	st_ptr = &town_info[p_ptr->town_num].store[6];
	o_ptr->ident |= IDENT_STOREB;
	(void)store_carry(o_ptr);
	object_wipe(o_ptr);  /* Don't leave a bogus object behind... */
}

/*
 * Enter the home, and interact with it from the dungeon (trump magic).
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 */
void do_cmd_home_trump(void)
{
	int which;
	int maintain_num;
	int tmp_chr;
	int i;
	int town_num;

	/* Extract the store code */
	which = 7;

	if (p_ptr->town_num) town_num = p_ptr->town_num;
	else town_num = 1;

	/* Hack -- Check the "locked doors" */
	if (town_info[town_num].store[which].store_open >= turn)
	{
		msg_print("The doors are locked.");
		return;
	}

	/* Calculate the number of store maintainances since the last visit */
	maintain_num = (turn - town_info[town_num].store[which].last_visit) / (10L * STORE_TURNS);

	/* Maintain the store max. 10 times */
	if (maintain_num > 10) maintain_num = 10;

	if (maintain_num)
	{
		/* Maintain the store */
		for (i = 0; i < maintain_num; i++)
			store_maint(town_num, which);

		/* Save the visit */
		town_info[town_num].store[which].last_visit = turn;
	}

	/* Forget the lite */
	/* forget_lite(); */

	/* Forget the view */
	forget_view();


	/* Hack -- Character is in "icky" mode */
	character_icky = TRUE;


	/* No command argument */
	command_arg = 0;

	/* No repeated command */
	command_rep = 0;

	/* No automatic command */
	command_new = 0;


	/* Save the store number */
	cur_store_num = which;

	/* Save the store and owner pointers */
	st_ptr = &town_info[town_num].store[cur_store_num];
	ot_ptr = &ow_info[st_ptr->owner];

	st_investment = st_ptr->investment;

	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store();

	/* Mega-Hack -- Ignore keymaps on store action letters */
	for (i = 0; i < 6; i++)
	{
		store_action_type *ba_ptr =
			&ba_info[st_info[st_ptr->st_idx].actions[i]];
		request_command_ignore_keymaps[2*i] = ba_ptr->letter;
		request_command_ignore_keymaps[2*i+1] = ba_ptr->letter_aux;
	}

	/* Do not leave */
	leave_store = FALSE;

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Hack -- Check the charisma */
		tmp_chr = p_ptr->stat_use[A_CHR];

		/* Clear */
		clear_from(21);


		/* Basic commands */
		prt(" ESC) Exit from Building.", 22, 0);

		/* Browse if necessary */
		if (st_ptr->stock_num > 12)
		{
			prt(" SPACE) Next page of stock", 23, 0);
		}

		/* Home commands */
		if (cur_store_num == 7)
		{
			prt(" g) Get an item.", 22, 31);
			prt(" d) Drop an item.", 23, 31);
		}

		/* Shop commands XXX XXX XXX */
		else
		{
			prt(" p) Purchase an item.", 22, 31);
			prt(" s) Sell an item.", 23, 31);
		}

		/* Add in the eXamine option */
		prt(" x) eXamine an item.", 22, 56);

		/* Prompt */
		prt("You may: ", 21, 0);

		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		store_process_command();

		/* Hack -- Character is still in "icky" mode */
		character_icky = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/* XXX XXX XXX Pack Overflow */
		if (p_ptr->inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			object_type *o_ptr = &p_ptr->inventory[item];

			/* Hack -- Flee from the store */
			if (cur_store_num != 7)
			{
				/* Message */
				msg_print("Your pack is so full that you flee the store...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if (!store_check_num(o_ptr))
			{
				/* Message */
				msg_print("Your pack is so full that you flee your home...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type forge;
				object_type *q_ptr;

				char o_name[80];


				/* Give a message */
				msg_print("Your pack overflows!");

				/* Get local object */
				q_ptr = &forge;

				/* Grab a copy of the item */
				object_copy(q_ptr, o_ptr);

				/* Describe it */
				object_desc(o_name, q_ptr, TRUE, 3);

				/* Message */
				msg_format("You drop %s (%c).", o_name, index_to_label(item));

				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

				/* Let the home carry it */
				item_pos = home_carry(q_ptr);

				/* Redraw the home */
				if (item_pos >= 0)
				{
					store_top = (item_pos / 12) * 12;
					display_inventory();
				}
			}
		}

		/* Hack -- Redisplay store prices if charisma changes */
		if (tmp_chr != p_ptr->stat_use[A_CHR]) display_inventory();

		/* Hack -- get kicked out of the store */
		if (st_ptr->store_open >= turn) leave_store = TRUE;
	}


	/* Hack -- Character is no longer in "icky" mode */
	character_icky = FALSE;


	/* Hack -- Cancel automatic command */
	command_new = 0;

	/* Hack -- Cancel "see" mode */
	command_see = FALSE;

	/* Mega-Hack -- Clear the 'ignore-keymaps' list */
	memset(request_command_ignore_keymaps, 0, 12);

	/* Flush messages XXX XXX XXX */
	msg_print(NULL);


	/* Clear the screen */
	Term_clear();


	/* Update everything */
	p_ptr->update |= (PU_VIEW | PU_MON_LITE);
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);
}

static void pay_for_requested_item(int value, object_type *q_ptr)
{
	msg_format("It'll cost %i gold pieces. ", value);

	if (get_check("Do you wish to pay?"))
	{
		if (p_ptr->au < value)
			msg_print("You don't have enough money for it.");
		else
		{
			if (store_carry(q_ptr) != -1)
			{
				msg_print("The item has arrived in the Black Market.");
				p_ptr->au -= value;

				p_ptr->redraw |= PR_GOLD;
			}
			else
				msg_print("There isn't enough room for it.");
		}
	}
}

/*
 * Request item for merchants
 */
void store_request_item(void)
{
	char buf[80], name[80];
	object_type forge, *q_ptr = &forge;
	store_type *ost_ptr = st_ptr;

	/* Paranoia */
#if 0 /* DGDGDGDG -- use a skill */
	if (cp_ptr->magic_key == MKEY_TELEKINESIS)
	{
		st_ptr = ost_ptr;
		return;
	}
#endif
	/* Get the Black Market */
	st_ptr = &town_info[p_ptr->town_num].store[6];

	/* Make an empty string */
	buf[0] = 0;

	/* Ask for the wish */
	if (!get_string("Request what? ", buf, 80))
	{
		st_ptr = ost_ptr;
		return;
	}

	clean_wish_name(buf, name);

	if (test_object_wish(name, q_ptr, &forge, "request"))
	{
		int value = object_value_real(q_ptr) * 5;

		/* Pay for the delivery */
		pay_for_requested_item(value, q_ptr);

		/* Don't search any more */
		st_ptr = ost_ptr;
		return;
	}

	st_ptr = ost_ptr;
}

/* Amy: 4D pocket */
void do_cmd_four_dimensional(void)
{
	int fourdimlevel = get_skill(SKILL_FOUR_DIM);

	if (p_ptr->nastytrap34) {
		msg_print("The fourth dimension is not available.");
		msg_print(NULL);
		return;
	}

	if (fourdimlevel < 1) {
		msg_print("You don't have enough skill to use that.");
		msg_print(NULL);
		return;
	}

	if (fourdimlevel > p_ptr->csp) {
		msg_print("Not enough mana.");
		msg_print(NULL);
		return;
	}

	if (fourdimlevel <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= fourdimlevel;
	}

	int which;
	int tmp_chr;
	int i;

	/* Extract the store code */
	which = 7;

	/* Forget the view */
	forget_view();


	/* Hack -- Character is in "icky" mode */
	character_icky = TRUE;


	/* No command argument */
	command_arg = 0;

	/* No repeated command */
	command_rep = 0;

	/* No automatic command */
	command_new = 0;


	/* Save the store number */
	cur_store_num = which;

	/* Save the store and owner pointers */
	st_ptr = &town_info[TOWN_FOURDIM].store[STORE_HOME];
	ot_ptr = &ow_info[st_ptr->owner];

	four_dim_mode = TRUE;

	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store();

	/* Mega-Hack -- Ignore keymaps on store action letters */
	for (i = 0; i < 6; i++)
	{
		store_action_type *ba_ptr =
			&ba_info[st_info[st_ptr->st_idx].actions[i]];
		request_command_ignore_keymaps[2*i] = ba_ptr->letter;
		request_command_ignore_keymaps[2*i+1] = ba_ptr->letter_aux;
	}

	/* Do not leave */
	leave_store = FALSE;

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Hack -- Check the charisma */
		tmp_chr = p_ptr->stat_use[A_CHR];

		/* Clear */
		clear_from(21);


		/* Basic commands */
		prt(" ESC) Exit from Building.", 22, 0);

		/* Browse if necessary */
		if (st_ptr->stock_num > 12)
		{
			prt(" SPACE) Next page of stock", 23, 0);
		}

		/* Home commands */
		if (cur_store_num == 7)
		{
			prt(" g) Get an item.", 22, 31);
			prt(" d) Drop an item.", 23, 31);
		}

		/* Shop commands XXX XXX XXX */
		else
		{
			prt(" p) Purchase an item.", 22, 31);
			prt(" s) Sell an item.", 23, 31);
		}

		/* Add in the eXamine option */
		prt(" x) eXamine an item.", 22, 56);

		/* Prompt */
		prt("You may: ", 21, 0);

		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		store_process_command();

		/* Hack -- Character is still in "icky" mode */
		character_icky = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/* XXX XXX XXX Pack Overflow */
		if (p_ptr->inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			object_type *o_ptr = &p_ptr->inventory[item];

			/* Hack -- Flee from the store */
			if (cur_store_num != 7)
			{
				/* Message */
				msg_print("Your pack is so full that you flee the store...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if (!store_check_num(o_ptr))
			{
				/* Message */
				msg_print("Your pack is so full that you flee your home...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type forge;
				object_type *q_ptr;

				char o_name[80];


				/* Give a message */
				msg_print("Your pack overflows!");

				/* Get local object */
				q_ptr = &forge;

				/* Grab a copy of the item */
				object_copy(q_ptr, o_ptr);

				/* Describe it */
				object_desc(o_name, q_ptr, TRUE, 3);

				/* Message */
				msg_format("You drop %s (%c).", o_name, index_to_label(item));

				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

				/* Let the home carry it */
				item_pos = home_carry(q_ptr);

				/* Redraw the home */
				if (item_pos >= 0)
				{
					store_top = (item_pos / 12) * 12;
					display_inventory();
				}
			}
		}

		/* Hack -- Redisplay store prices if charisma changes */
		if (tmp_chr != p_ptr->stat_use[A_CHR]) display_inventory();

		/* Hack -- get kicked out of the store */
		if (st_ptr->store_open >= turn) leave_store = TRUE;
	}


	/* Hack -- Character is no longer in "icky" mode */
	character_icky = FALSE;

	four_dim_mode = FALSE;

	/* Hack -- Cancel automatic command */
	command_new = 0;

	/* Hack -- Cancel "see" mode */
	command_see = FALSE;

	/* Mega-Hack -- Clear the 'ignore-keymaps' list */
	memset(request_command_ignore_keymaps, 0, 12);

	/* Flush messages XXX XXX XXX */
	msg_print(NULL);


	/* Clear the screen */
	Term_clear();


	/* Update everything */
	p_ptr->update |= (PU_VIEW | PU_MON_LITE);
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Take a turn */
	energy_use = 100;
}

