/* File: traps.c */

/* Purpose: handle traps */

/* the below copyright probably still applies, but it is heavily changed
 * copied, adapted & re-engineered by JK.
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

bool do_player_trap_call_out(void)
{
	s16b i, sn, cx, cy;
	s16b h_index = 0;
	s16b h_level = 0;
	monster_type *m_ptr;
	monster_race *r_ptr;
	char m_name[80];
	bool ident = FALSE;

	for (i = 1; i < m_max; i++)
	{
		m_ptr = &m_list[i];
		r_ptr = race_inf(m_ptr);

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		if (m_ptr->level >= h_level)
		{
			h_level = m_ptr->level;
			h_index = i;
		}
	}

	/* if the level is empty of monsters, h_index will be 0 */
	if (!h_index) return (FALSE);

	m_ptr = &m_list[h_index];

	sn = 0;
	for (i = 0; i < 8; i++)
	{
		cx = p_ptr->px + ddx[i];
		cy = p_ptr->py + ddy[i];

		/* Skip non-empty grids */
		if (!cave_valid_bold(cy, cx)) continue;
		if (cave[cy][cx].feat == FEAT_GLYPH) continue;
		if ((cx == p_ptr->px) && (cy == p_ptr->py)) continue;
		sn++;

		/* Randomize choice */
		if (rand_int(sn) > 0) continue;
		cave[cy][cx].m_idx = h_index;
		cave[m_ptr->fy][m_ptr->fx].m_idx = 0;
		m_ptr->fx = cx;
		m_ptr->fy = cy;

		/* we do not change the sublevel! */
		ident = TRUE;
		update_mon(h_index, TRUE);
		monster_desc(m_name, m_ptr, 0x08);
		msg_format("You hear a rapid-shifting wail, and %s appears!", m_name);
		break;
	}

	return (ident);
}

static bool do_trap_teleport_away(object_type *i_ptr, s16b y, s16b x)
{
	bool ident = FALSE;
	char o_name[80];

	s16b o_idx = 0;
	object_type *o_ptr;
	cave_type *c_ptr;

	s16b x1;
	s16b y1;

	if (i_ptr == NULL) return (FALSE);

	if (i_ptr->name1 == ART_POWER) return (FALSE);

	while (o_idx == 0)
	{
		x1 = rand_int(cur_wid);
		y1 = rand_int(cur_hgt);

		/* Obtain grid */
		c_ptr = &cave[y1][x1];

		/* Require floor space (or shallow terrain) -KMW- */
		if (!(f_info[c_ptr->feat].flags1 & FF1_FLOOR)) continue;

		o_idx = drop_near(i_ptr, 0, y1, x1);
	}

	o_ptr = &o_list[o_idx];

	x1 = o_ptr->ix;
	y1 = o_ptr->iy;

	if (!p_ptr->blind)
	{
		note_spot(y, x);
		lite_spot(y, x);
		ident = TRUE;
		object_desc(o_name, i_ptr, FALSE, 0);
		if (player_has_los_bold(y1, x1))
		{
			lite_spot(y1, x1);
			msg_format("The %s suddenly stands elsewhere.", o_name);

		}
		else
		{
			msg_format("You suddenly don't see the %s any more!", o_name);
		}
	}
	else
	{
		msg_print("You hear something move.");
	}
	return (ident);
}

/*
 * this handles a trap that places walls around the player
 */
static bool player_handle_trap_of_walls(void)
{
	bool ident;

	s16b dx, dy, cx, cy;
	s16b sx = 0, sy = 0, sn, i;
	cave_type *cv_ptr;
	bool map[5][5] =
	        {
	                {FALSE, FALSE, FALSE, FALSE, FALSE},
	                {FALSE, FALSE, FALSE, FALSE, FALSE},
	                {FALSE, FALSE, FALSE, FALSE, FALSE},
	                {FALSE, FALSE, FALSE, FALSE, FALSE},
	                {FALSE, FALSE, FALSE, FALSE, FALSE}
	        };

	for (dy = -2; dy <= 2; dy++)
		for (dx = -2; dx <= 2; dx++)
		{
			/* Extract the location */
			cx = p_ptr->px + dx;
			cy = p_ptr->py + dy;

			if (!in_bounds(cy, cx)) continue;

			cv_ptr = &cave[cy][cx];

			if (cv_ptr->m_idx) continue;

			/* Lose room and vault */
			cv_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);
			/* Lose light and knowledge */
			cv_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);

			/* Skip the center */
			if (!dx && !dy) continue;

			/* test for dungeon level */
			if (randint(100) > 10 + max_dlv_real[dungeon_type]) continue;

			/* Damage this grid */
			map[2 + dx][2 + dy] = TRUE;
		}

	for (dy = -2; dy <= 2; dy++)
		for (dx = -2; dx <= 2; dx++)
		{
			/* Extract the location */
			cx = p_ptr->px + dx;
			cy = p_ptr->py + dy;

			/* Skip unaffected grids */
			if (!map[2 + dx][2 + dy]) continue;

			cv_ptr = &cave[cy][cx];

			if (cv_ptr->m_idx)
			{
				monster_type *m_ptr = &m_list[cv_ptr->m_idx];
				monster_race *r_ptr = race_inf(m_ptr);

				/* Most monsters cannot co-exist with rock */
				if ((!(r_ptr->flags2 & RF2_KILL_WALL)) &&
				                (!(r_ptr->flags2 & RF2_PASS_WALL)))
				{
					char m_name[80];

					/* Assume not safe */
					sn = 0;

					/* Monster can move to escape the wall */
					if (!(r_ptr->flags1 & RF1_NEVER_MOVE))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Access the grid */
							cy = p_ptr->py + ddy[i];
							cx = p_ptr->px + ddx[i];

							/* Skip non-empty grids */
							if (!cave_clean_bold(cy, cx)) continue;

							/* Hack -- no safety on glyph of warding */
							if (cave[cy][cx].feat == FEAT_GLYPH) continue;

							/* Important -- Skip "quake" grids */
							if (map[2 + (cx - p_ptr->px)][2 + (cy - p_ptr->py)]) continue;

							/* Count "safe" grids */
							sn++;

							/* Randomize choice */
							if (rand_int(sn) > 0) continue;

							/* Save the safe grid */
							sx = cx;
							sy = cy;

							ident = TRUE;

							break;  /* discontinue for loop - safe grid found */
						}
					}

					/* Describe the monster */
					monster_desc(m_name, m_ptr, 0);

					/* Scream in pain */
					msg_format("%^s wails out in pain!", m_name);

					/* Monster is certainly awake */
					m_ptr->csleep = 0;

					/* Apply damage directly */
					m_ptr->hp -= (sn ? damroll(4, 8) : 200);

					/* Delete (not kill) "dead" monsters */
					if (m_ptr->hp < 0)
					{
						/* Message */
						msg_format("%^s is entombed in the rock!", m_name);

						/* Delete the monster */
						delete_monster_idx(cave[cy][cx].m_idx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if (sn)
					{
						s16b m_idx = cave[cy][cx].m_idx;

						/* Update the new location */
						cave[sy][sx].m_idx = m_idx;

						/* Update the old location */
						cave[cy][cx].m_idx = 0;

						/* Move the monster */
						m_ptr->fy = sy;
						m_ptr->fx = sx;

						/* do not change fz */
						/* don't make rock on that square! */
						if ((sx >= (p_ptr->px - 2)) && (sx <= (p_ptr->px + 2)) &&
						                (sy >= (p_ptr->py - 2)) && (sy <= (p_ptr->py + 2)))
						{
							map[2 + (sx - p_ptr->px)][2 + (sy - p_ptr->py)] = FALSE;
						}

						/* Update the monster (new location) */
						update_mon(m_idx, TRUE);

						/* Redraw the old grid */
						lite_spot(cy, cx);

						/* Redraw the new grid */
						lite_spot(sy, sx);
					} /* if sn */
				} /* if monster can co-exist with rock */
			} /* if monster on square */
		}

	/* Examine the quaked region */
	for (dy = -2; dy <= 2; dy++)
		for (dx = -2; dx <= 2; dx++)
		{
			/* Extract the location */
			cx = p_ptr->px + dx;
			cy = p_ptr->py + dy;

			/* Skip unaffected grids */
			if (!map[2 + dx][2 + dy]) continue;

			/* Access the cave grid */
			cv_ptr = &cave[cy][cx];

			/* Paranoia -- never affect player */
			if (!dy && !dx) continue;

			/* Destroy location (if valid) */
			if ((cx < cur_wid) && (cy < cur_hgt) && cave_valid_bold(cy, cx))
			{
				bool floor = (f_info[cave[cy][cx].feat].flags1 & FF1_FLOOR);

				/* Delete any object that is still there */
				delete_object(cy, cx);

				if (floor)
				{
					cave_set_feat(cy, cx, FEAT_WALL_OUTER);
				}
				else
				{
					/* Clear previous contents, add floor */
					cave_set_feat(cy, cx, FEAT_FLOOR);
				}
			}
		}

	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= PU_UN_VIEW;

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_FLOW | PU_MON_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_DISTANCE);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);
	handle_stuff();

	msg_print("Suddenly the cave shifts around you. The air is getting stale!");

	ident = TRUE;

	return (ident);
}

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
 * this function handles arrow & dagger traps, in various types.
 * num = number of missiles
 * tval, sval = kind of missiles
 * dd,ds = damage roll for missiles
 * poison_dam = additional poison damage
 * name = name given if you should die from it...
 *
 * return value = ident (always TRUE)
 */
static bool player_handle_missile_trap(s16b num, s16b tval, s16b sval, s16b dd, s16b ds,
                                       s16b pdam, cptr name)
{
	object_type *o_ptr, forge;
	s16b i, k_idx = lookup_kind(tval, sval);
	char i_name[80];

	o_ptr = &forge;
	object_prep(o_ptr, k_idx);
	o_ptr->number = num;
	apply_magic(o_ptr, max_dlv_real[dungeon_type], FALSE, FALSE, FALSE);
	object_desc(i_name, o_ptr, TRUE, 0);

	msg_format("Suddenly %s hit%s you!", i_name,
	           ((num == 1) ? "" : "s"));

	for (i = 0; i < num; i++)
	{
		take_hit(damroll(dd, ds), name);

		redraw_stuff();

		if (pdam > 0)
		{
			if (!(p_ptr->resist_pois || p_ptr->immune_pois || p_ptr->oppose_pois) || p_ptr->nastytrap32)
			{
				(void)set_poisoned(p_ptr->poisoned + pdam);
			}
		}
	}

	/* Amy edit: remove potential for lame farming */
	/* drop_near(o_ptr, -1, p_ptr->py, p_ptr->px); */

	return TRUE;
}

/*
 * this function handles a "breath" type trap - acid bolt, lightning balls etc.
 */
static bool player_handle_breath_trap(s16b rad, s16b type, u16b trap)
{
	trap_type *t_ptr = &t_info[trap];
	bool ident;
	s16b my_dd, my_ds, dam;

	my_dd = t_ptr->dd;
	my_ds = t_ptr->ds;

	/* multiplier depending on dungeon level: don't allow the value to become excessively high! --Amy */
	int maxdunlvlreal = max_dlv_real[dungeon_type];
	if (maxdunlvlreal > 150) maxdunlvlreal = 150;

	/* these traps gets nastier as levels progress */
	if (max_dlv_real[dungeon_type] > (2 * t_ptr->minlevel))
	{
		my_dd += (maxdunlvlreal / 30);
		my_ds += (maxdunlvlreal / 15);
	}
	dam = damroll(my_dd, my_ds);

	/* make sure stone wall affects the damn grid! --Amy */
	if (type == GF_STONE_WALL) {
		ident = project( -2, rad, p_ptr->py, p_ptr->px, dam, type, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT | PROJECT_GRID);
	} else {
		ident = project( -2, rad, p_ptr->py, p_ptr->px, dam, type, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
	}

	return (ident);
}

/*
 * This function damages the player by a trap
 */
static void trap_hit(s16b trap)
{
	s16b dam;
	trap_type *t_ptr = &t_info[trap];

	dam = damroll(t_ptr->dd, t_ptr->ds);

	take_hit(dam, t_name + t_ptr->name);
}

bool can_disarm_trap_type(int traptype)
{
	switch (traptype) {
		case TRAP_OF_FARTING:
		case TRAP_OF_FARTING_II:
		case TRAP_OF_FARTING_III:
		case TRAP_OF_FARTING_IV:
		case TRAP_OF_FARTING_V:
		case TRAP_OF_SHOES:
		case TRAP_OF_SHOES_II:
		case TRAP_OF_SHOES_III:
		case TRAP_OF_SHOES_IV:
		case TRAP_OF_SHOES_V:
		case TRAP_OF_SHIT_I:
		case TRAP_OF_SHIT_II:
		case TRAP_OF_SHIT_III:
		case TRAP_OF_SHIT_IV:
		case TRAP_OF_SHIT_V:
		case TRAP_OF_UNKNOWN:
		case TRAP_OF_UNKNOWN_II:
		case TRAP_OF_UNKNOWN_III:
		case TRAP_OF_UNKNOWN_IV:
		case TRAP_OF_UNKNOWN_V:
		case TRAP_OF_UNKNOWN_VI:
		case TRAP_OF_UNKNOWN_VII:
		case TRAP_OF_UNKNOWN_VIII:
		case TRAP_OF_UNKNOWN_OOD:
		case TRAP_OF_UNKNOWN_OOD_II:
		case TRAP_OF_UNKNOWN_OOD_III:
		case TRAP_OF_UNKNOWN_RARE:
		case TRAP_OF_UNKNOWN_RARE_II:
		case TRAP_OF_UNKNOWN_RARE_III:
		case TRAP_OF_UNKNOWN_OOD_RARE:
		case TRAP_OF_UNKNOWN_OOD_RARE_II:
		case TRAP_OF_UNKNOWN_OOD_RARE_III:
		case TRAP_NASTY1:
		case TRAP_NASTY2:
		case TRAP_NASTY3:
		case TRAP_NASTY4:
		case TRAP_NASTY5:
		case TRAP_NASTY6:
		case TRAP_NASTY7:
		case TRAP_NASTY8:
		case TRAP_NASTY9:
		case TRAP_NASTY10:
		case TRAP_NASTY11:
		case TRAP_NASTY12:
		case TRAP_NASTY13:
		case TRAP_NASTY14:
		case TRAP_NASTY15:
		case TRAP_NASTY16:
		case TRAP_NASTY17:
		case TRAP_NASTY18:
		case TRAP_NASTY19:
		case TRAP_NASTY20:
		case TRAP_NASTY21:
		case TRAP_NASTY22:
		case TRAP_NASTY23:
		case TRAP_NASTY24:
		case TRAP_NASTY25:
		case TRAP_NASTY26:
		case TRAP_NASTY27:
		case TRAP_NASTY28:
		case TRAP_NASTY29:
		case TRAP_NASTY30:
		case TRAP_NASTY31:
		case TRAP_NASTY32:
		case TRAP_NASTY33:
		case TRAP_NASTY34:
		case TRAP_NASTY35:
		case TRAP_NASTY36:
		case TRAP_NASTY37:
		case TRAP_NASTY38:
		case TRAP_NASTY39:
		case TRAP_NASTY40:
		case TRAP_NASTY41:
		case TRAP_NASTY42:
		case TRAP_NASTY43:
		case TRAP_NASTY44:
		case TRAP_NASTY45:
		case TRAP_NASTY46:
		case TRAP_NASTY47:
		case TRAP_NASTY48:
		case TRAP_NASTY49:
		case TRAP_NASTY50:
		case TRAP_NASTY51:
		case TRAP_NASTY52:
		case TRAP_NASTY53:
		case TRAP_NASTY54:
		case TRAP_NASTY55:
		case TRAP_NASTY56:
		case TRAP_NASTY57:
		case TRAP_NASTY58:
		case TRAP_NASTY59:
		case TRAP_NASTY60:
		case TRAP_NASTY61:
		case TRAP_NASTY62:
		case TRAP_NASTY63:
		case TRAP_NASTY64:
		case TRAP_NASTY65:
		case TRAP_NASTY66:
		case TRAP_NASTY67:
		case TRAP_NASTY68:
		case TRAP_NASTY69:
		case TRAP_NASTY70:
		case TRAP_NASTY71:
		case TRAP_NASTY72:
		case TRAP_NASTY73:
		case TRAP_NASTY74:
		case TRAP_NASTY75:
		case TRAP_NASTY76:
		case TRAP_NASTY77:
		case TRAP_NASTY78:
		case TRAP_NASTY79:
		case TRAP_NASTY80:
		case TRAP_NASTY81:
		case TRAP_NASTY82:
		case TRAP_NASTY83:
		case TRAP_NASTY84:
		case TRAP_NASTY85:
		case TRAP_NASTY86:
		case TRAP_NASTY87:
		case TRAP_NASTY88:
		case TRAP_NASTY89:
		case TRAP_NASTY90:
		case TRAP_NASTY91:
		case TRAP_NASTY92:
		case TRAP_NASTY93:
		case TRAP_NASTY94:
		case TRAP_NASTY95:
		case TRAP_NASTY96:
		case TRAP_NASTY97:
		case TRAP_NASTY98:
		case TRAP_NASTY99:
		case TRAP_NASTY100:
		case TRAP_NASTY101:
		case TRAP_NASTY102:
		case TRAP_NASTY103:
		case TRAP_NASTY104:
		case TRAP_NASTY105:
		case TRAP_NASTY106:
		case TRAP_NASTY107:
		case TRAP_NASTY108:
		case TRAP_NASTY109:
		case TRAP_NASTY110:
		case TRAP_NASTY111:
		case TRAP_NASTY112:
		case TRAP_NASTY113:
		case TRAP_NASTY114:
		case TRAP_NASTY115:
		case TRAP_NASTY116:
		case TRAP_NASTY117:
		case TRAP_NASTY118:
		case TRAP_NASTY119:
		case TRAP_NASTY120:
		case TRAP_NASTY121:
		case TRAP_NASTY122:
		case TRAP_NASTY123:
		case TRAP_NASTY124:
		case TRAP_NASTY125:
		case TRAP_NASTY126:
		case TRAP_NASTY127:
		case TRAP_NASTY128:
		case TRAP_NASTY129:
		case TRAP_NASTY130:
		case TRAP_NASTY131:
		case TRAP_NASTY132:
		case TRAP_NASTY133:
		case TRAP_NASTY134:
		case TRAP_NASTY135:
		case TRAP_NASTY136:
		case TRAP_NASTY137:
		case TRAP_NASTY138:
		case TRAP_NASTY139:
		case TRAP_NASTY140:
		case TRAP_NASTY141:
		case TRAP_NASTY142:
		case TRAP_NASTY143:
		case TRAP_NASTY144:
		case TRAP_NASTY145:
		case TRAP_NASTY146:
		case TRAP_NASTY147:
		case TRAP_NASTY148:
		case TRAP_NASTY149:
		case TRAP_NASTY150:
		case TRAP_NASTY151:
		case TRAP_NASTY152:
		case TRAP_NASTY153:
		case TRAP_NASTY154:
		case TRAP_NASTY155:
		case TRAP_NASTY156:
		case TRAP_NASTY157:
		case TRAP_NASTY158:
		case TRAP_NASTY159:
		case TRAP_NASTY160:
		case TRAP_NASTY161:
		case TRAP_NASTY162:
		case TRAP_NASTY163:
		case TRAP_NASTY164:
		case TRAP_NASTY165:
		case TRAP_NASTY166:
		case TRAP_NASTY167:
		case TRAP_NASTY168:
		case TRAP_NASTY169:
		case TRAP_NASTY170:
		case TRAP_NASTY171:
		case TRAP_NASTY172:
		case TRAP_NASTY173:
			return FALSE;
	}

	return TRUE;

}

bool can_detect_trap_type(int traptype)
{
	if (p_ptr->nastytrap2) return FALSE; /* ensnaring nastytrap: prevents finding any traps */

	switch (traptype) {
		case TRAP_OF_UNKNOWN:
		case TRAP_OF_UNKNOWN_II:
		case TRAP_OF_UNKNOWN_III:
		case TRAP_OF_UNKNOWN_IV:
		case TRAP_OF_UNKNOWN_V:
		case TRAP_OF_UNKNOWN_VI:
		case TRAP_OF_UNKNOWN_VII:
		case TRAP_OF_UNKNOWN_VIII:
		case TRAP_OF_UNKNOWN_OOD:
		case TRAP_OF_UNKNOWN_OOD_II:
		case TRAP_OF_UNKNOWN_OOD_III:
		case TRAP_OF_UNKNOWN_RARE:
		case TRAP_OF_UNKNOWN_RARE_II:
		case TRAP_OF_UNKNOWN_RARE_III:
		case TRAP_OF_UNKNOWN_OOD_RARE:
		case TRAP_OF_UNKNOWN_OOD_RARE_II:
		case TRAP_OF_UNKNOWN_OOD_RARE_III:
		case TRAP_OF_SHIT_I:
		case TRAP_OF_SHIT_II:
		case TRAP_OF_SHIT_III:
		case TRAP_OF_SHIT_IV:
		case TRAP_OF_SHIT_V:
		case TRAP_NASTY1:
		case TRAP_NASTY2:
		case TRAP_NASTY3:
		case TRAP_NASTY4:
		case TRAP_NASTY5:
		case TRAP_NASTY6:
		case TRAP_NASTY7:
		case TRAP_NASTY8:
		case TRAP_NASTY9:
		case TRAP_NASTY10:
		case TRAP_NASTY11:
		case TRAP_NASTY12:
		case TRAP_NASTY13:
		case TRAP_NASTY14:
		case TRAP_NASTY15:
		case TRAP_NASTY16:
		case TRAP_NASTY17:
		case TRAP_NASTY18:
		case TRAP_NASTY19:
		case TRAP_NASTY20:
		case TRAP_NASTY21:
		case TRAP_NASTY22:
		case TRAP_NASTY23:
		case TRAP_NASTY24:
		case TRAP_NASTY25:
		case TRAP_NASTY26:
		case TRAP_NASTY27:
		case TRAP_NASTY28:
		case TRAP_NASTY29:
		case TRAP_NASTY30:
		case TRAP_NASTY31:
		case TRAP_NASTY32:
		case TRAP_NASTY33:
		case TRAP_NASTY34:
		case TRAP_NASTY35:
		case TRAP_NASTY36:
		case TRAP_NASTY37:
		case TRAP_NASTY38:
		case TRAP_NASTY39:
		case TRAP_NASTY40:
		case TRAP_NASTY41:
		case TRAP_NASTY42:
		case TRAP_NASTY43:
		case TRAP_NASTY44:
		case TRAP_NASTY45:
		case TRAP_NASTY46:
		case TRAP_NASTY47:
		case TRAP_NASTY48:
		case TRAP_NASTY49:
		case TRAP_NASTY50:
		case TRAP_NASTY51:
		case TRAP_NASTY52:
		case TRAP_NASTY53:
		case TRAP_NASTY54:
		case TRAP_NASTY55:
		case TRAP_NASTY56:
		case TRAP_NASTY57:
		case TRAP_NASTY58:
		case TRAP_NASTY59:
		case TRAP_NASTY60:
		case TRAP_NASTY61:
		case TRAP_NASTY62:
		case TRAP_NASTY63:
		case TRAP_NASTY64:
		case TRAP_NASTY65:
		case TRAP_NASTY66:
		case TRAP_NASTY67:
		case TRAP_NASTY68:
		case TRAP_NASTY69:
		case TRAP_NASTY70:
		case TRAP_NASTY71:
		case TRAP_NASTY72:
		case TRAP_NASTY73:
		case TRAP_NASTY74:
		case TRAP_NASTY75:
		case TRAP_NASTY76:
		case TRAP_NASTY77:
		case TRAP_NASTY78:
		case TRAP_NASTY79:
		case TRAP_NASTY80:
		case TRAP_NASTY81:
		case TRAP_NASTY82:
		case TRAP_NASTY83:
		case TRAP_NASTY84:
		case TRAP_NASTY85:
		case TRAP_NASTY86:
		case TRAP_NASTY87:
		case TRAP_NASTY88:
		case TRAP_NASTY89:
		case TRAP_NASTY90:
		case TRAP_NASTY91:
		case TRAP_NASTY92:
		case TRAP_NASTY93:
		case TRAP_NASTY94:
		case TRAP_NASTY95:
		case TRAP_NASTY96:
		case TRAP_NASTY97:
		case TRAP_NASTY98:
		case TRAP_NASTY99:
		case TRAP_NASTY100:
		case TRAP_NASTY101:
		case TRAP_NASTY102:
		case TRAP_NASTY103:
		case TRAP_NASTY104:
		case TRAP_NASTY105:
		case TRAP_NASTY106:
		case TRAP_NASTY107:
		case TRAP_NASTY108:
		case TRAP_NASTY109:
		case TRAP_NASTY110:
		case TRAP_NASTY111:
		case TRAP_NASTY112:
		case TRAP_NASTY113:
		case TRAP_NASTY114:
		case TRAP_NASTY115:
		case TRAP_NASTY116:
		case TRAP_NASTY117:
		case TRAP_NASTY118:
		case TRAP_NASTY119:
		case TRAP_NASTY120:
		case TRAP_NASTY121:
		case TRAP_NASTY122:
		case TRAP_NASTY123:
		case TRAP_NASTY124:
		case TRAP_NASTY125:
		case TRAP_NASTY126:
		case TRAP_NASTY127:
		case TRAP_NASTY128:
		case TRAP_NASTY129:
		case TRAP_NASTY130:
		case TRAP_NASTY131:
		case TRAP_NASTY132:
		case TRAP_NASTY133:
		case TRAP_NASTY134:
		case TRAP_NASTY135:
		case TRAP_NASTY136:
		case TRAP_NASTY137:
		case TRAP_NASTY138:
		case TRAP_NASTY139:
		case TRAP_NASTY140:
		case TRAP_NASTY141:
		case TRAP_NASTY142:
		case TRAP_NASTY143:
		case TRAP_NASTY144:
		case TRAP_NASTY145:
		case TRAP_NASTY146:
		case TRAP_NASTY147:
		case TRAP_NASTY148:
		case TRAP_NASTY149:
		case TRAP_NASTY150:
		case TRAP_NASTY151:
		case TRAP_NASTY152:
		case TRAP_NASTY153:
		case TRAP_NASTY154:
		case TRAP_NASTY155:
		case TRAP_NASTY156:
		case TRAP_NASTY157:
		case TRAP_NASTY158:
		case TRAP_NASTY159:
		case TRAP_NASTY160:
		case TRAP_NASTY161:
		case TRAP_NASTY162:
		case TRAP_NASTY163:
		case TRAP_NASTY164:
		case TRAP_NASTY165:
		case TRAP_NASTY166:
		case TRAP_NASTY167:
		case TRAP_NASTY168:
		case TRAP_NASTY169:
		case TRAP_NASTY170:
		case TRAP_NASTY171:
		case TRAP_NASTY172:
		case TRAP_NASTY173:
			return FALSE;
	}

	return TRUE;
}

/* trap doesn't become visible when triggered --Amy */
bool is_nonvis_trap(int traptype)
{
	if (is_nasty_trap(traptype)) return TRUE;
	switch (traptype) {
		case TRAP_OF_UNKNOWN:
		case TRAP_OF_UNKNOWN_II:
		case TRAP_OF_UNKNOWN_III:
		case TRAP_OF_UNKNOWN_IV:
		case TRAP_OF_UNKNOWN_V:
		case TRAP_OF_UNKNOWN_VI:
		case TRAP_OF_UNKNOWN_VII:
		case TRAP_OF_UNKNOWN_VIII:
		case TRAP_OF_UNKNOWN_OOD:
		case TRAP_OF_UNKNOWN_OOD_II:
		case TRAP_OF_UNKNOWN_OOD_III:
		case TRAP_OF_UNKNOWN_RARE:
		case TRAP_OF_UNKNOWN_RARE_II:
		case TRAP_OF_UNKNOWN_RARE_III:
		case TRAP_OF_UNKNOWN_OOD_RARE:
		case TRAP_OF_UNKNOWN_OOD_RARE_II:
		case TRAP_OF_UNKNOWN_OOD_RARE_III:
			return TRUE;
	}
	return FALSE;
}

bool is_nasty_trap(int traptype)
{

	switch (traptype) {
		case TRAP_NASTY1:
		case TRAP_NASTY2:
		case TRAP_NASTY3:
		case TRAP_NASTY4:
		case TRAP_NASTY5:
		case TRAP_NASTY6:
		case TRAP_NASTY7:
		case TRAP_NASTY8:
		case TRAP_NASTY9:
		case TRAP_NASTY10:
		case TRAP_NASTY11:
		case TRAP_NASTY12:
		case TRAP_NASTY13:
		case TRAP_NASTY14:
		case TRAP_NASTY15:
		case TRAP_NASTY16:
		case TRAP_NASTY17:
		case TRAP_NASTY18:
		case TRAP_NASTY19:
		case TRAP_NASTY20:
		case TRAP_NASTY21:
		case TRAP_NASTY22:
		case TRAP_NASTY23:
		case TRAP_NASTY24:
		case TRAP_NASTY25:
		case TRAP_NASTY26:
		case TRAP_NASTY27:
		case TRAP_NASTY28:
		case TRAP_NASTY29:
		case TRAP_NASTY30:
		case TRAP_NASTY31:
		case TRAP_NASTY32:
		case TRAP_NASTY33:
		case TRAP_NASTY34:
		case TRAP_NASTY35:
		case TRAP_NASTY36:
		case TRAP_NASTY37:
		case TRAP_NASTY38:
		case TRAP_NASTY39:
		case TRAP_NASTY40:
		case TRAP_NASTY41:
		case TRAP_NASTY42:
		case TRAP_NASTY43:
		case TRAP_NASTY44:
		case TRAP_NASTY45:
		case TRAP_NASTY46:
		case TRAP_NASTY47:
		case TRAP_NASTY48:
		case TRAP_NASTY49:
		case TRAP_NASTY50:
		case TRAP_NASTY51:
		case TRAP_NASTY52:
		case TRAP_NASTY53:
		case TRAP_NASTY54:
		case TRAP_NASTY55:
		case TRAP_NASTY56:
		case TRAP_NASTY57:
		case TRAP_NASTY58:
		case TRAP_NASTY59:
		case TRAP_NASTY60:
		case TRAP_NASTY61:
		case TRAP_NASTY62:
		case TRAP_NASTY63:
		case TRAP_NASTY64:
		case TRAP_NASTY65:
		case TRAP_NASTY66:
		case TRAP_NASTY67:
		case TRAP_NASTY68:
		case TRAP_NASTY69:
		case TRAP_NASTY70:
		case TRAP_NASTY71:
		case TRAP_NASTY72:
		case TRAP_NASTY73:
		case TRAP_NASTY74:
		case TRAP_NASTY75:
		case TRAP_NASTY76:
		case TRAP_NASTY77:
		case TRAP_NASTY78:
		case TRAP_NASTY79:
		case TRAP_NASTY80:
		case TRAP_NASTY81:
		case TRAP_NASTY82:
		case TRAP_NASTY83:
		case TRAP_NASTY84:
		case TRAP_NASTY85:
		case TRAP_NASTY86:
		case TRAP_NASTY87:
		case TRAP_NASTY88:
		case TRAP_NASTY89:
		case TRAP_NASTY90:
		case TRAP_NASTY91:
		case TRAP_NASTY92:
		case TRAP_NASTY93:
		case TRAP_NASTY94:
		case TRAP_NASTY95:
		case TRAP_NASTY96:
		case TRAP_NASTY97:
		case TRAP_NASTY98:
		case TRAP_NASTY99:
		case TRAP_NASTY100:
		case TRAP_NASTY101:
		case TRAP_NASTY102:
		case TRAP_NASTY103:
		case TRAP_NASTY104:
		case TRAP_NASTY105:
		case TRAP_NASTY106:
		case TRAP_NASTY107:
		case TRAP_NASTY108:
		case TRAP_NASTY109:
		case TRAP_NASTY110:
		case TRAP_NASTY111:
		case TRAP_NASTY112:
		case TRAP_NASTY113:
		case TRAP_NASTY114:
		case TRAP_NASTY115:
		case TRAP_NASTY116:
		case TRAP_NASTY117:
		case TRAP_NASTY118:
		case TRAP_NASTY119:
		case TRAP_NASTY120:
		case TRAP_NASTY121:
		case TRAP_NASTY122:
		case TRAP_NASTY123:
		case TRAP_NASTY124:
		case TRAP_NASTY125:
		case TRAP_NASTY126:
		case TRAP_NASTY127:
		case TRAP_NASTY128:
		case TRAP_NASTY129:
		case TRAP_NASTY130:
		case TRAP_NASTY131:
		case TRAP_NASTY132:
		case TRAP_NASTY133:
		case TRAP_NASTY134:
		case TRAP_NASTY135:
		case TRAP_NASTY136:
		case TRAP_NASTY137:
		case TRAP_NASTY138:
		case TRAP_NASTY139:
		case TRAP_NASTY140:
		case TRAP_NASTY141:
		case TRAP_NASTY142:
		case TRAP_NASTY143:
		case TRAP_NASTY144:
		case TRAP_NASTY145:
		case TRAP_NASTY146:
		case TRAP_NASTY147:
		case TRAP_NASTY148:
		case TRAP_NASTY149:
		case TRAP_NASTY150:
		case TRAP_NASTY151:
		case TRAP_NASTY152:
		case TRAP_NASTY153:
		case TRAP_NASTY154:
		case TRAP_NASTY155:
		case TRAP_NASTY156:
		case TRAP_NASTY157:
		case TRAP_NASTY158:
		case TRAP_NASTY159:
		case TRAP_NASTY160:
		case TRAP_NASTY161:
		case TRAP_NASTY162:
		case TRAP_NASTY163:
		case TRAP_NASTY164:
		case TRAP_NASTY165:
		case TRAP_NASTY166:
		case TRAP_NASTY167:
		case TRAP_NASTY168:
		case TRAP_NASTY169:
		case TRAP_NASTY170:
		case TRAP_NASTY171:
		case TRAP_NASTY172:
		case TRAP_NASTY173:
			return TRUE;
	}

	return FALSE;
}

/*
 * this function activates one trap type, and returns
 * a bool indicating if this trap is now identified
 */
bool player_activate_trap_type(s16b y, s16b x, object_type *i_ptr, s16b item)
{
	bool ident = FALSE;
	s16b trap;

	cave_type *c_ptr;
	trap_type *t_ptr;
	dungeon_info_type *d_ptr = &d_info[dungeon_type];
	s16b k, l;

	/* Amy: for fart trap, determine the name; sadly the "trap" struct doesn't have extra fields here, unlike SLEX */
	int fartingnumber = x + y; /* so we're using the x,y position to determine it */
	int fartingtype = randint(2);

	trap = cave[y][x].t_idx;

	c_ptr = &cave[y][x];

	if (i_ptr != NULL)
	{
		trap = i_ptr->pval;
	}

	if ((i_ptr == NULL) && (cave[y][x].o_idx != 0))
	{
		i_ptr = &o_list[cave[y][x].o_idx];
	}

	if (trap == TRAP_OF_UNKNOWN || trap == TRAP_OF_UNKNOWN_II || trap == TRAP_OF_UNKNOWN_III || trap == TRAP_OF_UNKNOWN_IV || trap == TRAP_OF_UNKNOWN_V || trap == TRAP_OF_UNKNOWN_VI || trap == TRAP_OF_UNKNOWN_VII || trap == TRAP_OF_UNKNOWN_VIII) {
		if (c_ptr->info & (CAVE_TRDT)) {
			t_info[trap].ident = TRUE;
		}

		/* Try 10000 times */
		int cnt = 10000;
		while (cnt--)
		{
			trap = randint(max_t_idx - 1);
			t_ptr = &t_info[trap];

			/* Este piety reduces spawn rate of nasty traps */
			GOD(GOD_ESTE) {
				if (p_ptr->grace > 15000 && is_nasty_trap(trap)) {
					if (randint(p_ptr->grace) > 15000) continue;
				}
			}

			/* anti-nastiness skill by Amy: reduces odds that a new trap will be a nasty trap */
			int nastyward = get_skill(SKILL_ANTINASTY) * 2;

			int effect_level;
			int scalinglevel = 1;

			if (dun_level > 0) effect_level = dun_level;
			else effect_level = wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].level + rand_int(p_ptr->lev);
			if (monster_level > effect_level) effect_level = monster_level;

			if (p_ptr->nastytrap107) effect_level += randint(10 + p_ptr->lev);

			if (randint(50) == 1) scalinglevel = randint(p_ptr->lev);

			if (effect_level < scalinglevel) effect_level = scalinglevel;

			if (nastyward > 0) {
				int effnastyward = nastyward;
				if (effnastyward > 80) effnastyward = 80;

				if (is_nasty_trap(trap) && magik(effnastyward)) continue;

				if (nastyward > 80) {
					if (is_nasty_trap(trap) && (randint(nastyward) > 80)) continue;
				}

			}

			/* No traps below their minlevel */
			if (t_ptr->minlevel > effect_level && ( (randint(3) != 1) || (randint(t_ptr->minlevel + 1) > (effect_level + 2) ) ) ) continue;
			if ( (t_ptr->minlevel > (effect_level + 5)) && (randint(t_ptr->minlevel) != 1) ) continue;

			/* is this a correct trap now?   */
			if (!(t_ptr->flags & FTRAP_FLOOR)) continue;

			/*
			 * Hack -- No trap door at the bottom of dungeon or in flat
			 * (non dungeon) places or on quest levels
			 */
			if ((trap == TRAP_OF_SINKING || trap == TRAP_OF_SHAFT || trap == TRAP_OF_DEEP_DESCENT || trap == TRAP_OF_TELE_LEVEL) &&
			    ((d_ptr->maxdepth == dun_level) ||
			     (dungeon_flags1 & DF1_FLAT) || (is_quest(dun_level) && (is_quest(dun_level) != QUEST_RANDOM) )) )
			{
				continue;
			}

			/* How probable is this trap */
			if (rand_int(100) < t_ptr->probability)
			{
				c_ptr->t_idx = trap;
				break;
			}
		}

		if (!is_nonvis_trap(c_ptr->t_idx)) {
			msg_print("You triggered a trap!");
			pick_trap(p_ptr->py, p_ptr->px);
		}
	}

	if (trap == TRAP_OF_UNKNOWN_OOD || trap == TRAP_OF_UNKNOWN_OOD_II || trap == TRAP_OF_UNKNOWN_OOD_III) {
		if (c_ptr->info & (CAVE_TRDT)) {
			t_info[trap].ident = TRUE;
		}

		/* Try 10000 times */
		int cnt = 10000;
		while (cnt--)
		{
			trap = randint(max_t_idx - 1);
			t_ptr = &t_info[trap];

			/* Este piety reduces spawn rate of nasty traps */
			GOD(GOD_ESTE) {
				if (p_ptr->grace > 15000 && is_nasty_trap(trap)) {
					if (randint(p_ptr->grace) > 15000) continue;
				}
			}

			/* anti-nastiness skill by Amy: reduces odds that a new trap will be a nasty trap */
			int nastyward = get_skill(SKILL_ANTINASTY) * 2;

			int effect_level;
			int scalinglevel = 1;

			if (dun_level > 0) effect_level = dun_level;
			else effect_level = wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].level + rand_int(p_ptr->lev);
			if (monster_level > effect_level) effect_level = monster_level;

			effect_level += randint(25);

			if (p_ptr->nastytrap107) effect_level += randint(10 + p_ptr->lev);

			if (randint(50) == 1) scalinglevel = randint(p_ptr->lev);

			if (effect_level < scalinglevel) effect_level = scalinglevel;

			if (nastyward > 0) {
				int effnastyward = nastyward;
				if (effnastyward > 80) effnastyward = 80;

				if (is_nasty_trap(trap) && magik(effnastyward)) continue;

				if (nastyward > 80) {
					if (is_nasty_trap(trap) && (randint(nastyward) > 80)) continue;
				}

			}

			/* No traps below their minlevel */
			if (t_ptr->minlevel > effect_level && ( (randint(3) != 1) || (randint(t_ptr->minlevel + 1) > (effect_level + 2) ) ) ) continue;
			if ( (t_ptr->minlevel > (effect_level + 5)) && (randint(t_ptr->minlevel) != 1) ) continue;

			/* is this a correct trap now?   */
			if (!(t_ptr->flags & FTRAP_FLOOR)) continue;

			/*
			 * Hack -- No trap door at the bottom of dungeon or in flat
			 * (non dungeon) places or on quest levels
			 */
			if ((trap == TRAP_OF_SINKING || trap == TRAP_OF_SHAFT || trap == TRAP_OF_DEEP_DESCENT || trap == TRAP_OF_TELE_LEVEL) &&
			    ((d_ptr->maxdepth == dun_level) ||
			     (dungeon_flags1 & DF1_FLAT) || (is_quest(dun_level) && (is_quest(dun_level) != QUEST_RANDOM) )) )
			{
				continue;
			}

			/* How probable is this trap */
			if (rand_int(100) < t_ptr->probability)
			{
				c_ptr->t_idx = trap;
				break;
			}
		}

		if (!is_nonvis_trap(c_ptr->t_idx)) {
			msg_print("You triggered a trap!");
			pick_trap(p_ptr->py, p_ptr->px);
		}
	}

	if (trap == TRAP_OF_UNKNOWN_OOD_RARE || trap == TRAP_OF_UNKNOWN_OOD_RARE_II || trap == TRAP_OF_UNKNOWN_OOD_RARE_III) {
		if (c_ptr->info & (CAVE_TRDT)) {
			t_info[trap].ident = TRUE;
		}

		/* Try 10000 times */
		int cnt = 10000;
		while (cnt--)
		{
			trap = randint(max_t_idx - 1);
			t_ptr = &t_info[trap];

			/* Este piety reduces spawn rate of nasty traps */
			GOD(GOD_ESTE) {
				if (p_ptr->grace > 15000 && is_nasty_trap(trap)) {
					if (randint(p_ptr->grace) > 15000) continue;
				}
			}

			/* anti-nastiness skill by Amy: reduces odds that a new trap will be a nasty trap */
			int nastyward = get_skill(SKILL_ANTINASTY) * 2;

			int effect_level;
			int scalinglevel = 1;

			if (dun_level > 0) effect_level = dun_level;
			else effect_level = wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].level + rand_int(p_ptr->lev);
			if (monster_level > effect_level) effect_level = monster_level;

			effect_level += randint(25);

			if (p_ptr->nastytrap107) effect_level += randint(10 + p_ptr->lev);

			if (randint(50) == 1) scalinglevel = randint(p_ptr->lev);

			if (effect_level < scalinglevel) effect_level = scalinglevel;

			if (nastyward > 0) {
				int effnastyward = nastyward;
				if (effnastyward > 80) effnastyward = 80;

				if (is_nasty_trap(trap) && magik(effnastyward)) continue;

				if (nastyward > 80) {
					if (is_nasty_trap(trap) && (randint(nastyward) > 80)) continue;
				}

			}

			/* No traps below their minlevel */
			if (t_ptr->minlevel > effect_level && ( (randint(3) != 1) || (randint(t_ptr->minlevel + 1) > (effect_level + 2) ) ) ) continue;
			if ( (t_ptr->minlevel > (effect_level + 5)) && (randint(t_ptr->minlevel) != 1) ) continue;

			/* is this a correct trap now?   */
			if (!(t_ptr->flags & FTRAP_FLOOR)) continue;

			/*
			 * Hack -- No trap door at the bottom of dungeon or in flat
			 * (non dungeon) places or on quest levels
			 */
			if ((trap == TRAP_OF_SINKING || trap == TRAP_OF_SHAFT || trap == TRAP_OF_DEEP_DESCENT || trap == TRAP_OF_TELE_LEVEL) &&
			    ((d_ptr->maxdepth == dun_level) ||
			     (dungeon_flags1 & DF1_FLAT) || (is_quest(dun_level) && (is_quest(dun_level) != QUEST_RANDOM) )) )
			{
				continue;
			}

			/* ignore probabilities --Amy */
			c_ptr->t_idx = trap;
			break;
		}

		if (!is_nonvis_trap(c_ptr->t_idx)) {
			msg_print("You triggered a trap!");
			pick_trap(p_ptr->py, p_ptr->px);
		}
	}

	if (trap == TRAP_OF_UNKNOWN_RARE || trap == TRAP_OF_UNKNOWN_RARE_II || trap == TRAP_OF_UNKNOWN_RARE_III) {
		if (c_ptr->info & (CAVE_TRDT)) {
			t_info[trap].ident = TRUE;
		}

		/* Try 10000 times */
		int cnt = 10000;
		while (cnt--)
		{
			trap = randint(max_t_idx - 1);
			t_ptr = &t_info[trap];

			/* Este piety reduces spawn rate of nasty traps */
			GOD(GOD_ESTE) {
				if (p_ptr->grace > 15000 && is_nasty_trap(trap)) {
					if (randint(p_ptr->grace) > 15000) continue;
				}
			}

			/* anti-nastiness skill by Amy: reduces odds that a new trap will be a nasty trap */
			int nastyward = get_skill(SKILL_ANTINASTY) * 2;

			int effect_level;
			int scalinglevel = 1;

			if (dun_level > 0) effect_level = dun_level;
			else effect_level = wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].level + rand_int(p_ptr->lev);
			if (monster_level > effect_level) effect_level = monster_level;

			if (p_ptr->nastytrap107) effect_level += randint(10 + p_ptr->lev);

			if (randint(50) == 1) scalinglevel = randint(p_ptr->lev);

			if (effect_level < scalinglevel) effect_level = scalinglevel;

			if (nastyward > 0) {
				int effnastyward = nastyward;
				if (effnastyward > 80) effnastyward = 80;

				if (is_nasty_trap(trap) && magik(effnastyward)) continue;

				if (nastyward > 80) {
					if (is_nasty_trap(trap) && (randint(nastyward) > 80)) continue;
				}

			}

			/* No traps below their minlevel */
			if (t_ptr->minlevel > effect_level && ( (randint(3) != 1) || (randint(t_ptr->minlevel + 1) > (effect_level + 2) ) ) ) continue;
			if ( (t_ptr->minlevel > (effect_level + 5)) && (randint(t_ptr->minlevel) != 1) ) continue;

			/* is this a correct trap now?   */
			if (!(t_ptr->flags & FTRAP_FLOOR)) continue;

			/*
			 * Hack -- No trap door at the bottom of dungeon or in flat
			 * (non dungeon) places or on quest levels
			 */
			if ((trap == TRAP_OF_SINKING || trap == TRAP_OF_SHAFT || trap == TRAP_OF_DEEP_DESCENT || trap == TRAP_OF_TELE_LEVEL) &&
			    ((d_ptr->maxdepth == dun_level) ||
			     (dungeon_flags1 & DF1_FLAT) || (is_quest(dun_level) && (is_quest(dun_level) != QUEST_RANDOM) )) )
			{
				continue;
			}

			/* ignore probabilities --Amy */
			c_ptr->t_idx = trap;
			break;
		}

		if (!is_nonvis_trap(c_ptr->t_idx)) {
			msg_print("You triggered a trap!");
			pick_trap(p_ptr->py, p_ptr->px);
		}
	}

	switch (trap)
	{

	case TRAP_OF_FARTING:
	case TRAP_OF_FARTING_II:
	case TRAP_OF_FARTING_III:
	case TRAP_OF_FARTING_IV:
	case TRAP_OF_FARTING_V:

		/* one of the possible effects is trap creation... so make sure it doesn't ID some other trap! --Amy */
		if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;
		ident = FALSE;

		while (fartingnumber < 1) fartingnumber = 43;
		while (fartingnumber > 43) fartingnumber -= 43;

		switch (fartingnumber) {

			case 1:
				msg_format("Kati produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 2:
				msg_format("Maurah produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 3:
				msg_format("Eveline produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 4:
				msg_format("Larissa produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 5:
				msg_format("Sandra produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 6:
				msg_format("Meltem produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 7:
				msg_format("Kerstin produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 8:
				msg_format("Karin produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 9:
				msg_format("Ina produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 10:
				msg_format("Lou produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 11:
				msg_format("Lisa produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 12:
				msg_format("Miriam produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 13:
				msg_format("Elena produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 14:
				msg_format("Katharina produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 15:
				msg_format("Simone produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 16:
				msg_format("Jasieen produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 17:
				msg_format("Marike produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 18:
				msg_format("Sue Lyn produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 19:
				msg_format("Marleen produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 20:
				msg_format("Claudia produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 21:
				msg_format("Ksenia produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;

			case 22:
				msg_format("Nadja produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 23:
				msg_format("Mailie produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 24:
				msg_format("Elif produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 25:
				msg_format("Solvejg produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 26:
				msg_format("Sueschen produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 27:
				msg_format("Jessica produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 28:
				msg_format("Yvonne produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 29:
				msg_format("Patricia produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 30:
				msg_format("Jennifer produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 31:
				msg_format("Inge produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 32:
				msg_format("Sarah produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 33:
				msg_format("Birgit produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "tender" : "soft");
				break;

			case 34:
				msg_format("Sunali produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 35:
				msg_format("Thai produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 36:
				msg_format("Klara produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 37:
				msg_format("Ludgera produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 38:
				msg_format("Johanetta produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 39:
				msg_format("Antje produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 40:
				msg_format("Ruea produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 41:
				msg_format("Mariya produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 42:
				msg_format("Wendy produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			case 43:
				msg_format("Katia produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "loud" : "disgusting");
				break;

			default:
				msg_format("Sandra produces %s farting noises with her sexy butt.", (fartingtype == 1) ? "squeaky" : "beautiful");
				break;
		}

		do_fart_effect();
		ident = FALSE;

		if (!p_ptr->nastytrap3) {
			switch (trap) {
				case TRAP_OF_FARTING:
					msg_print("You identified that trap as Fart Trap.");
					break;
				case TRAP_OF_FARTING_II:
					msg_print("You identified that trap as Farting Trap.");
					break;
				case TRAP_OF_FARTING_III:
					msg_print("You identified that trap as Farting Noise Trap.");
					break;
				case TRAP_OF_FARTING_IV:
					msg_print("You identified that trap as Butt Trap.");
					break;
				case TRAP_OF_FARTING_V:
					msg_print("You identified that trap as Sexy Butt Trap.");
					break;
			}
		}

		break;

		/* stat traps */
	case TRAP_OF_WEAKNESS_I:
		ident = do_dec_stat(A_STR, STAT_DEC_TEMPORARY);
		break;
	case TRAP_OF_WEAKNESS_II:
		ident = do_dec_stat(A_STR, STAT_DEC_NORMAL);
		break;
	case TRAP_OF_WEAKNESS_III:
		ident = do_dec_stat(A_STR, STAT_DEC_PERMANENT);
		break;
	case TRAP_OF_WEAKNESS_IV:
		ident = do_dec_stat(A_STR, STAT_DEC_PERMANENT_NORESIST);
		break;
	case TRAP_OF_INTELLIGENCE_I:
		ident = do_dec_stat(A_INT, STAT_DEC_TEMPORARY);
		break;
	case TRAP_OF_INTELLIGENCE_II:
		ident = do_dec_stat(A_INT, STAT_DEC_NORMAL);
		break;
	case TRAP_OF_INTELLIGENCE_III:
		ident = do_dec_stat(A_INT, STAT_DEC_PERMANENT);
		break;
	case TRAP_OF_INTELLIGENCE_IV:
		ident = do_dec_stat(A_INT, STAT_DEC_PERMANENT_NORESIST);
		break;
	case TRAP_OF_WISDOM_I:
		ident = do_dec_stat(A_WIS, STAT_DEC_TEMPORARY);
		break;
	case TRAP_OF_WISDOM_II:
		ident = do_dec_stat(A_WIS, STAT_DEC_NORMAL);
		break;
	case TRAP_OF_WISDOM_III:
		ident = do_dec_stat(A_WIS, STAT_DEC_PERMANENT);
		break;
	case TRAP_OF_WISDOM_IV:
		ident = do_dec_stat(A_WIS, STAT_DEC_PERMANENT_NORESIST);
		break;
	case TRAP_OF_FUMBLING_I:
		ident = do_dec_stat(A_DEX, STAT_DEC_TEMPORARY);
		break;
	case TRAP_OF_FUMBLING_II:
		ident = do_dec_stat(A_DEX, STAT_DEC_NORMAL);
		break;
	case TRAP_OF_FUMBLING_III:
		ident = do_dec_stat(A_DEX, STAT_DEC_PERMANENT);
		break;
	case TRAP_OF_FUMBLING_IV:
		ident = do_dec_stat(A_DEX, STAT_DEC_PERMANENT_NORESIST);
		break;
	case TRAP_OF_WASTING_I:
		ident = do_dec_stat(A_CON, STAT_DEC_TEMPORARY);
		break;
	case TRAP_OF_WASTING_II:
		ident = do_dec_stat(A_CON, STAT_DEC_NORMAL);
		break;
	case TRAP_OF_WASTING_III:
		ident = do_dec_stat(A_CON, STAT_DEC_PERMANENT);
		break;
	case TRAP_OF_WASTING_IV:
		ident = do_dec_stat(A_CON, STAT_DEC_PERMANENT_NORESIST);
		break;
	case TRAP_OF_BEAUTY_I:
		ident = do_dec_stat(A_CHR, STAT_DEC_TEMPORARY);
		break;
	case TRAP_OF_BEAUTY_II:
		ident = do_dec_stat(A_CHR, STAT_DEC_NORMAL);
		break;
	case TRAP_OF_BEAUTY_III:
		ident = do_dec_stat(A_CHR, STAT_DEC_PERMANENT);
		break;
	case TRAP_OF_BEAUTY_IV:
		ident = do_dec_stat(A_CHR, STAT_DEC_PERMANENT_NORESIST);
		break;

	case TRAP_OF_STAT_DRAIN_I:
		ident = do_dec_stat(rand_int(6), STAT_DEC_TEMPORARY);
		break;
	case TRAP_OF_STAT_DRAIN_II:
		ident = do_dec_stat(rand_int(6), STAT_DEC_NORMAL);
		break;
	case TRAP_OF_STAT_DRAIN_III:
		ident = do_dec_stat(rand_int(6), STAT_DEC_PERMANENT);
		break;
	case TRAP_OF_STAT_DRAIN_IV:
		ident = do_dec_stat(rand_int(6), STAT_DEC_PERMANENT_NORESIST);
		break;
	case TRAP_OF_STAT_SAP_I:
		{
			int sappings = 5;
			while (sappings) {
				if (do_dec_stat(rand_int(6), STAT_DEC_TEMPORARY)) ident = TRUE;
				sappings--;
			}
		}
		break;
	case TRAP_OF_STAT_SAP_II:
		{
			int sappings = 5;
			while (sappings) {
				if (do_dec_stat(rand_int(6), STAT_DEC_NORMAL)) ident = TRUE;
				sappings--;
			}
		}
		break;
	case TRAP_OF_STAT_SAP_III:
		{
			int sappings = 5;
			while (sappings) {
				if (do_dec_stat(rand_int(6), STAT_DEC_PERMANENT)) ident = TRUE;
				sappings--;
			}
		}
		break;
	case TRAP_OF_STAT_SAP_IV:
		{
			int sappings = 5;
			while (sappings) {
				if (do_dec_stat(rand_int(6), STAT_DEC_PERMANENT_NORESIST)) ident = TRUE;
				sappings--;
			}
		}
		break;

	case TRAP_OF_RECALL:
		{
			if (p_ptr->word_recall) {
				ident = FALSE;
				msg_print("You are already being recalled.");
				break;
			}
			recall_player(21, 15);
			ident = TRUE;

			break;
		}

		/* Trap of Curse Weapon */
	case TRAP_OF_CURSE_WEAPON:
		{
			msg_print("You hear a terrible wail!");
			ident = curse_weapon();
			break;
		}

		/* Trap of Curse Armor */
	case TRAP_OF_CURSE_ARMOR:
		{
			msg_print("You hear a terrible roar!");
			ident = curse_armor();
			break;
		}

	case TRAP_OF_CURSE_GARMENT:
		{
			msg_print("You hear a terrible scream!");
			ident = curse_garment();
			break;
		}

	case TRAP_OF_CURSE_SHOOTER:
		{
			msg_print("You hear a terrible bang!");
			ident = curse_shooter();
			break;
		}

	case TRAP_OF_CURSE_JEWELRY:
		{
			msg_print("You hear a terrible howl!");
			ident = curse_jewelry();
			break;
		}

	case TRAP_OF_CURSE_LIGHT:
		{
			msg_print("You hear a terrible frazzle!");
			ident = curse_light();
			break;
		}

	case TRAP_OF_CURSE_AMMO:
		{
			msg_print("You hear a terrible whump!");
			ident = curse_ammo();
			break;
		}

	case TRAP_OF_CURSE_TOOL:
		{
			msg_print("You hear a terrible moan!");
			ident = curse_tool();
			break;
		}

	case TRAP_OF_CURSE_EVERYTHING:
		{
			msg_print("Terrible noises of all kinds drone all around you!");
			ident = FALSE;
			if (curse_weapon()) ident = TRUE;
			if (curse_armor()) ident = TRUE;
			if (curse_garment()) ident = TRUE;
			if (curse_shooter()) ident = TRUE;
			if (curse_jewelry()) ident = TRUE;
			if (curse_light()) ident = TRUE;
			if (curse_ammo()) ident = TRUE;
			if (curse_tool()) ident = TRUE;
			break;
		}

	case TRAP_OF_ITEM_CURSE:
		{
			msg_print("You hear a curse!");
			ident = curse_equipment(100, 0);
			break;
		}

	case TRAP_OF_HEAVY_CURSE:
		{
			msg_print("You hear a horrible curse!");
			ident = curse_equipment(100, 100);
			break;
		}

	case TRAP_OF_HEAVY_CURSE_X:
		{
			msg_print("You hear many horrible curses!");
			ident = FALSE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			if (curse_equipment(100, 100)) ident = TRUE;
			break;
		}

	case TRAP_OF_PRIME_CURSE:
		{
			msg_print("You hear a terrible incantation!");
			ident = curse_equipment_prime(100, 100);
			break;
		}

	case TRAP_OF_PRIME_CURSE_X:
		{
			msg_print("You hear several terrible incantations!");
			ident = FALSE;
			if (curse_equipment_prime(100, 100)) ident = TRUE;
			if (curse_equipment_prime(100, 100)) ident = TRUE;
			if (curse_equipment_prime(100, 100)) ident = TRUE;
			break;
		}

		/* Earthquake Trap */
	case TRAP_OF_EARTHQUAKE:
		{
			msg_print("As you touch the trap, the ground starts to shake.");
			earthquake(y, x, 10);
			ident = TRUE;
			break;
		}

		/* Super Earthquake Trap, by Amy */
	case TRAP_OF_EARTHQUAKE_X:
		{
			msg_print("As you touch the trap, the ground starts to shake.");
			earthquake(y, x, 20);
			ident = TRUE;
			break;
		}

		/* Poison Needle Trap */
	case TRAP_OF_POISON_NEEDLE:
		{
			if (!(p_ptr->resist_pois || p_ptr->immune_pois || p_ptr->oppose_pois) || p_ptr->nastytrap32)
			{
				msg_print("You prick yourself on a poisoned needle.");
				(void)set_poisoned(p_ptr->poisoned + rand_int(15) + 10);
				ident = TRUE;
			}
			else
			{
				msg_print("You prick yourself on a needle.");
				ident = TRUE;
			}
			break;
		}

	case TRAP_OF_POISON_NEEDLE_X:
		{
			msg_print("You prick yourself on a poisoned needle.");
			(void)set_poisoned(p_ptr->poisoned + rand_int(50) + 20);
			ident = TRUE;
			break;
		}

	case TRAP_OF_STIR:
		{
			ident = TRUE;
			msg_print("You hear a stirring sound.");
			for (k = 0; k < 10; k++)
			{
				(void)alloc_monster(MAX_SIGHT + 5, FALSE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Stir Trap.");
				ident = FALSE;

			}

			break;
		}

		/* Summon Monster Trap */
	case TRAP_OF_SUMMON_MONSTER:
		{
			msg_print("A spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type], 0);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Monster Trap.");
				ident = FALSE;

			}

			break;
		}

		/* Summon Squad Trap */
	case TRAP_OF_SUMMON_SQUAD:
		{
			msg_print("A preposterous spell hangs in the air.");
			for (k = 0; k < (randint(6) + randint(16)); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type], 0);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Squad Trap.");
				ident = FALSE;

			}

			break;
		}

	case TRAP_OF_OOD_SUMMON:
		{
			msg_print("A gaudy spell hangs in the air.");
			monster_level = dun_level + 20;
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type], 0);
			}
			monster_level = dun_level;

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as OOD Summon Trap.");
				ident = FALSE;

			}
			msg_print("The world whirls around you.");
			teleport_player(RATIO * 67);

			break;
		}

	case TRAP_OF_OOD_SUMMON_X:
		{
			msg_print("A hellish spell hangs in the air.");
			monster_level = dun_level + 40;
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type], 0);
			}
			monster_level = dun_level;

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Depth Summon Trap.");
				ident = FALSE;

			}
			msg_print("The world whirls around you.");
			teleport_player(RATIO * 67);

			break;
		}

		/* Summon Undead Trap */
	case TRAP_OF_SUMMON_UNDEAD:
		{
			msg_print("A mighty spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_UNDEAD);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Undead Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Demon Trap */
	case TRAP_OF_SUMMON_DEMON:
		{
			msg_print("A horrific spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_DEMON);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Demon Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Dragon Trap */
	case TRAP_OF_SUMMON_DRAGON:
		{
			msg_print("A powerful spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_DRAGON);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Dragon Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Troll Trap */
	case TRAP_OF_SUMMON_TROLL:
		{
			msg_print("A rancid spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_TROLL);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(5) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Troll Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Orc Trap */
	case TRAP_OF_SUMMON_ORC:
		{
			msg_print("A funny spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_ORC);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(5) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Orc Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Spider Trap */
	case TRAP_OF_SUMMON_SPIDER:
		{
			msg_print("A dark spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_SPIDER);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Spider Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Hound Trap */
	case TRAP_OF_SUMMON_HOUND:
		{
			msg_print("A musty spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_HOUND);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Hound Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Hydra Trap */
	case TRAP_OF_SUMMON_HYDRA:
		{
			msg_print("A salty spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_HYDRA);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Hydra Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Angel Trap */
	case TRAP_OF_SUMMON_ANGEL:
		{
			msg_print("A celestial spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_ANGEL);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Angel Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Wraith Trap */
	case TRAP_OF_SUMMON_WRAITH:
		{
			msg_print("A stinking spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_WRAITH);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Wraith Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Ant Trap */
	case TRAP_OF_SUMMON_ANT:
		{
			msg_print("A grassy spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_ANT);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Ant Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Ghost Trap */
	case TRAP_OF_SUMMON_GHOST:
		{
			msg_print("A chilly spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_GHOST);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Ghost Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Mimic Trap */
	case TRAP_OF_SUMMON_MIMIC:
		{
			msg_print("A fizzy spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_BIZARRE1);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Mimic Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Bat Trap */
	case TRAP_OF_SUMMON_BAT:
		{
			msg_print("A wispy spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_BIZARRE2);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Bat Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Vortex Trap */
	case TRAP_OF_SUMMON_VORTEX:
		{
			msg_print("A whirly spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_BIZARRE4);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Vortex Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Treasure Trap */
	case TRAP_OF_SUMMON_TREASURE:
		{
			msg_print("A clinging spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_BIZARRE6);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Treasure Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Animal Trap */
	case TRAP_OF_SUMMON_ANIMAL:
		{
			msg_print("A lively spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_ANIMAL);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Animal Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Thunderlord Trap */
	case TRAP_OF_SUMMON_THUNDERLORD:
		{
			msg_print("A majestic spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_THUNDERLORD);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Thunderlord Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Mine Trap */
	case TRAP_OF_SUMMON_MINE:
		{
			msg_print("A clicking sound can be heard.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_MINE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Mine Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Elemental Trap */
	case TRAP_OF_SUMMON_ELEMENTAL:
		{
			msg_print("An unstoppable spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_E);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Elemental Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Snake Trap */
	case TRAP_OF_SUMMON_SNAKE:
		{
			msg_print("A hissing spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_SNAKE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Snake Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Horror Trap */
	case TRAP_OF_SUMMON_HORROR:
		{
			msg_print("A horrible spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_ELDRITCH);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Horror Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Cat Trap */
	case TRAP_OF_SUMMON_CAT:
		{
			msg_print("A furry spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_CAT);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Cat Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Rat Trap */
	case TRAP_OF_SUMMON_RAT:
		{
			msg_print("A reeking spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_RAT);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Rat Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Worm Trap */
	case TRAP_OF_SUMMON_WORM:
		{
			msg_print("A long spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_WORM);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Worm Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Clothes Trap */
	case TRAP_OF_SUMMON_CLOTHES:
		{
			msg_print("A shuffling spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_CLOTHES);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Clothes Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Hybrid Trap */
	case TRAP_OF_SUMMON_HYBRID:
		{
			msg_print("A resonating spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_HYBRID);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Hybrid Trap.");
				ident = FALSE;

			}
			break;
		}

	case TRAP_OF_SUMMON_GIANT:
		{
			msg_print("A humongous spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_GIANT);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Giant Trap.");
				ident = FALSE;

			}
			break;
		}

	case TRAP_OF_SUMMON_SEXY_GIRL:
		{
			msg_print("A feminine spell hangs in the air.");
			for (k = 0; k < 1; k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_SEXY_GIRL);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(5) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Sexy Girl Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Beetle Trap */
	case TRAP_OF_SUMMON_BEETLE:
		{
			msg_print("A crunching spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_BEETLE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Beetle Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Horde Trap */
	case TRAP_OF_SUMMON_HORDE:
		{
			msg_print("A chaotic spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_HORDE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Horde Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Amberite Trap */
	case TRAP_OF_SUMMON_AMBERITE:
		{
			msg_print("A dangerous spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_AMBERITE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			t_info[trap].ident = ident;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}

			if (ident) msg_print("You identified that trap as Summon Amberite Trap.");
			ident = FALSE;

			break;
		}

		/* Summon Townie Trap */
	case TRAP_OF_SUMMON_TOWNIE:
		{
			msg_print("A bustling spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_TOWNIE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Townie Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Shore Trap */
	case TRAP_OF_SUMMON_SHORE:
		{
			msg_print("A watery spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_SHORE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Shore Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Ocean Trap */
	case TRAP_OF_SUMMON_OCEAN:
		{
			msg_print("A wavy spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_OCEAN);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Ocean Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Waste Trap */
	case TRAP_OF_SUMMON_WASTE:
		{
			msg_print("A windy spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_WASTE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Waste Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Wood Trap */
	case TRAP_OF_SUMMON_WOOD:
		{
			msg_print("A whistling spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_WOOD);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Wood Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Volcano Trap */
	case TRAP_OF_SUMMON_VOLCANO:
		{
			msg_print("A burning spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_VOLCANO);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Volcano Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Mountain Trap */
	case TRAP_OF_SUMMON_MOUNTAIN:
		{
			msg_print("A cold spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_MOUNTAIN);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Mountain Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Grass Trap */
	case TRAP_OF_SUMMON_GRASS:
		{
			msg_print("A humming spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_GRASS);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Grass Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Unique Trap */
	case TRAP_OF_SUMMON_UNIQUE:
		{
			msg_print("A terrifying spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_UNIQUE);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(2) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Unique Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Greater Undead Trap */
	case TRAP_OF_SUMMON_GREATER_UNDEAD:
		{
			msg_print("An old and evil spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_HI_UNDEAD);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Greater Undead Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Greater Demon Trap */
	case TRAP_OF_SUMMON_GREATER_DEMON:
		{
			msg_print("An old and evil spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_HI_DEMON);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Greater Demon Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Summon Greater Dragon Trap */
	case TRAP_OF_SUMMON_GREATER_DRAGON:
		{
			msg_print("An old and evil spell hangs in the air.");
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type],
				                         SUMMON_HI_DRAGON);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Greater Dragon Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Teleport Trap */
	case TRAP_OF_TELEPORT:
		{
			msg_print("The world whirls around you.");
			teleport_player(RATIO * 67);
			ident = TRUE;
			break;
		}

	case TRAP_OF_PHASE_DOOR:
		{
			msg_print("The world whirls around you.");
			teleport_player(8 + rand_int(13));
			ident = TRUE;
			break;
		}

		/* Paralyzing Trap */
	case TRAP_OF_PARALYZING:
		{
			if (!p_ptr->free_act || (rand_int(p_ptr->nastytrap57 ? 20 : 100) == 0) )
			{
				if (!p_ptr->resist_nerve || magik(50)) {
					msg_print("You touch a poisoned part and can't move.");
					(void)set_paralyzed(p_ptr->paralyzed + rand_int(10) + 10);	
					ident = TRUE;
				} else {
					msg_print("You prick yourself on a needle.");
					ident = TRUE;
				}
			}
			else
			{
				msg_print("You prick yourself on a needle.");
				ident = TRUE;
			}
			break;
		}

		/* Super Paralyzing Trap, by Amy */
	case TRAP_OF_PARALYZING_X:
		{
			msg_print("You touch a poisoned part and can't move.");
			(void)set_paralyzed(p_ptr->paralyzed + rand_int(10) + 10);
			ident = TRUE;

			break;
		}

		/* Explosive Device */
	case TRAP_OF_EXPLOSIVE_DEVICE:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev;
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel > 5) efflevel = 5;
			if (efflevel < 2) efflevel = 2;

			msg_print("A hidden explosive device explodes in your face.");
			take_hit(damroll(efflevel, 8), "an explosion");
			ident = TRUE;
			break;
		}

	case TRAP_OF_EXPLODE_I:
		{
			msg_print("A booby trap explodes in your face.");
			take_hit(damroll(2, 8), "an explosion");
			ident = TRUE;
			break;
		}

	case TRAP_OF_EXPLODE_II:
		{
			int efflevel = 2;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev;
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel > 10) efflevel = 10;
			if (efflevel < 2) efflevel = 2;

			msg_print("A hidden mine explodes in your face.");
			take_hit(damroll(efflevel, 8), "an explosion");
			ident = TRUE;
			break;
		}

	case TRAP_OF_EXPLODE_III:
		{
			int efflevel = 2;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev;
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel > 20) efflevel = 20;
			if (efflevel < 2) efflevel = 2;

			msg_print("A contact bomb explodes in your face.");
			take_hit(damroll(efflevel, 8), "an explosion");
			ident = TRUE;
			break;
		}

	case TRAP_OF_EXPLODE_IV:
		{
			int efflevel = 2;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev;
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel > 40) efflevel = 40;
			if (efflevel < 2) efflevel = 2;

			msg_print("A dynamite charge explodes in your face.");
			take_hit(damroll(efflevel, 8), "an explosion");
			ident = TRUE;
			break;
		}

	case TRAP_OF_EXPLODE_V:
		{
			int efflevel = 2;
			if (dun_level > 0) efflevel = dun_level;
			else {
				efflevel = p_ptr->lev;
				if (p_ptr->lev >= 40) efflevel = 80;
			}
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel > 80) efflevel = 80;
			if (efflevel < 2) efflevel = 2;

			msg_print("A plastic explosive charge explodes in your face.");
			take_hit(damroll(efflevel, 8), "an explosion");
			ident = TRUE;
			break;
		}

		/* pit trap, very low-level trap for early dungeons by Amy; flying or levitating char is immune */
	case TRAP_OF_PIT:
		{
			if (p_ptr->ffall || p_ptr->fly) {
				msg_print("You see a pit below you.");
				ident = TRUE;
				break;
			}

			msg_print("You fell into a pit!");
			int efflevel = 1;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev;
			if (efflevel < dun_level) efflevel = dun_level;
			take_hit(randint(5 + efflevel), "a pit");
			ident = TRUE;
			break;
		}

	case TRAP_OF_FORGET_TRAPS:
		{
			int k;
			trap_type *ft_ptr;
			msg_print("Your knowledge of traps is reduced!");

			for (k = 0; k < max_t_idx; k++)
			{
				ft_ptr = &t_info[k];
				if (magik(2)) ft_ptr->ident = FALSE;
			}

			ident = TRUE;
			break;
		}

	case TRAP_OF_FORGET_OBJECTS:
		{
			int k;
			object_kind *fk_ptr;

			msg_print("Your knowledge of objects is reduced!");

			for (k = 0; k < max_k_idx; k++)
			{
				fk_ptr = &k_info[k];
				if (fk_ptr->flavor && magik(2)) {
					fk_ptr->aware = FALSE;
					fk_ptr->tried = FALSE;
				}
			}

			ident = TRUE;
			break;
		}

	case TRAP_OF_FORGET_TRAPS_X:
		{
			int k;
			trap_type *ft_ptr;
			msg_print("Your knowledge of traps is reduced!");

			for (k = 0; k < max_t_idx; k++)
			{
				ft_ptr = &t_info[k];
				if (magik(5)) ft_ptr->ident = FALSE;
			}

			ident = TRUE;
			break;
		}

	case TRAP_OF_FORGET_OBJECTS_X:
		{
			int k;
			object_kind *fk_ptr;

			msg_print("Your knowledge of objects is reduced!");

			for (k = 0; k < max_k_idx; k++)
			{
				fk_ptr = &k_info[k];
				if (fk_ptr->flavor && magik(5)) {
					fk_ptr->aware = FALSE;
					fk_ptr->tried = FALSE;
				}
			}

			ident = TRUE;
			break;
		}

	case TRAP_OF_FORGET_OBJ_AMN:
		{
			int k;
			object_kind *fk_ptr;

			msg_print("Your knowledge of objects is reduced!");
			msg_print("Everything looks different!");

			seed_flavor = rand_int(0x10000000);
			flavor_init();

			for (k = 0; k < max_k_idx; k++)
			{
				fk_ptr = &k_info[k];
				if (fk_ptr->flavor && magik(2)) {
					fk_ptr->aware = FALSE;
					fk_ptr->tried = FALSE;
				}
			}

			ident = TRUE;
			break;
		}

	case TRAP_OF_FORGET_OBJ_AMN_X:
		{
			int k;
			object_kind *fk_ptr;

			msg_print("Your knowledge of objects is reduced!");
			msg_print("Everything looks different!");

			seed_flavor = rand_int(0x10000000);
			flavor_init();

			for (k = 0; k < max_k_idx; k++)
			{
				fk_ptr = &k_info[k];
				if (fk_ptr->flavor && magik(5)) {
					fk_ptr->aware = FALSE;
					fk_ptr->tried = FALSE;
				}
			}

			ident = TRUE;
			break;
		}

	case TRAP_OF_APPEARANCE_REROLL:
		{
			msg_print("Everything looks different!");

			seed_flavor = rand_int(0x10000000);
			flavor_init();

			ident = TRUE;
			break;
		}

	case TRAP_OF_FORGET_OBJ_TRAPS:
		{
			int k;
			trap_type *ft_ptr;
			object_kind *fk_ptr;
			msg_print("Your knowledge of traps and objects is deleted!");

			for (k = 0; k < max_t_idx; k++)
			{
				ft_ptr = &t_info[k];
				if (magik(20)) ft_ptr->ident = FALSE;
			}

			for (k = 0; k < max_k_idx; k++)
			{
				fk_ptr = &k_info[k];
				if (fk_ptr->flavor && magik(20)) {
					fk_ptr->aware = FALSE;
					fk_ptr->tried = FALSE;
				}
			}

			ident = TRUE;
			break;
		}

		/* Teleport Away Trap */
	case TRAP_OF_TELEPORT_AWAY:
		{
			int itemX, amt;
			object_type *o_ptr;

			msg_print("The floor seems to shift...");

			/* change by Amy: allow this trap to exist on the floor too and teleport items in a 3x3 area */
			if ((item == -1) || (item == -2)) {
				int xtemp = -1;
				int ytemp = -1;

				while (xtemp < 2) {
					ytemp = -1;
					while (ytemp < 2) {

						if (!in_bounds(y + ytemp, x + xtemp)) continue;

						/* teleport away all items */
						while (cave[y + ytemp][x + xtemp].o_idx != 0)
						{
							itemX = cave[y + ytemp][x + xtemp].o_idx;

							o_ptr = &o_list[itemX];

							amt = o_ptr->number;

							ident = do_trap_teleport_away(o_ptr, y + ytemp, x + xtemp);

							floor_item_increase(itemX, -amt);
							floor_item_optimize(itemX);
						}
						ytemp++;
					}
					xtemp++;
				}
				break;

			} else {

				/* teleport away all items */
				while (cave[y][x].o_idx != 0)
				{
					itemX = cave[y][x].o_idx;

					o_ptr = &o_list[itemX];

					amt = o_ptr->number;

					ident = do_trap_teleport_away(o_ptr, y, x);

					floor_item_increase(itemX, -amt);
					floor_item_optimize(itemX);
				}
				break;
			}
		}

		/* Lose Memory Trap */
	case TRAP_OF_LOSE_MEMORY:
		{
			lose_exp(p_ptr->exp / 4);

			ident |= dec_stat(A_WIS, rand_int(20) + 10, STAT_DEC_NORMAL);
			ident |= dec_stat(A_INT, rand_int(20) + 10, STAT_DEC_NORMAL);

			if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
			{
				ident |= set_confused(p_ptr->confused + rand_int(100) + 50);
			}

			if (ident)
			{
				msg_print("You suddenly don't remember what you were doing.");
			}
			else
			{
				msg_print("You feel an alien force probing your mind.");
			}
			break;
		}
		/* Bitter Regret Trap */
	case TRAP_OF_BITTER_REGRET:
		{
			msg_print("An age-old and hideous-sounding spell reverberates off the walls.");

			ident |= dec_stat(A_DEX, 25, TRUE);
			ident |= dec_stat(A_WIS, 25, TRUE);
			ident |= dec_stat(A_CON, 25, TRUE);
			ident |= dec_stat(A_STR, 25, TRUE);
			ident |= dec_stat(A_CHR, 25, TRUE);
			ident |= dec_stat(A_INT, 25, TRUE);
			break;
		}

		/* Dimming Trap */
	case TRAP_OF_DIMMING:
		{
			msg_print("The area around you seems to flicker!");

			object_type *o_ptr;

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
					ident = TRUE;
				}

				/* Window stuff */
				p_ptr->window |= (PW_EQUIP);
			}

			break;
		}

		/* as a shout-out to the ridiculous EAT_LITE bug... reduce pval of your light, by Amy */
	case TRAP_OF_BUGGED_LIGHT:
		{
			msg_print("Suddenly the lights around you are flashing!");

			object_type *o_ptr;

			/* Access the lite */
			o_ptr = &p_ptr->inventory[INVEN_LITE];

			/* Drain fuel */
			if (o_ptr->pval > 0)
			{
				/* Reduce fuel */
				o_ptr->pval--;

				msg_print("Something seems wrong with your light source...");
				ident = TRUE;

				/* Window stuff */
				p_ptr->window |= (PW_EQUIP);
			}

			break;
		}

		/* Slowing Trap */
	case TRAP_OF_SLOW_I:
		{
			msg_print("Your feet suddenly feel heavier!");

			(void)set_slow(p_ptr->slow + rand_int(4) + 4);
			ident = TRUE;

			break;
		}
	case TRAP_OF_SLOW_II:
		{
			msg_print("Your feet suddenly feel heavier!");

			(void)set_slow(p_ptr->slow + rand_int(20) + 10);
			ident = TRUE;

			break;
		}
	case TRAP_OF_SLOW_III:
		{
			msg_print("Your feet suddenly feel heavier!");

			(void)set_slow(p_ptr->slow + rand_int(100) + 50);
			ident = TRUE;

			break;
		}

		/* Amnesia Trap */
	case TRAP_OF_AMNESIA:
		{
			msg_print("Something tries to blank your mind!");
			lose_all_info();
			ident = TRUE;

			break;
		}
	case TRAP_OF_AMNESIA_X:
		{
			msg_print("Something tries to blank your mind!");
			lose_all_info_X();
			ident = TRUE;

			break;
		}

	case TRAP_OF_WOUNDS_I:
		{
			msg_print("Something seems to curse at you!");
			curse_equipment(33, 0);
			take_hit(damroll(3, 8), "a cursing trap");
			ident = TRUE;

			break;
		}

	case TRAP_OF_WOUNDS_II:
		{
			msg_print("Something seems to curse horribly at you!");
			curse_equipment(50, 5);
			take_hit(damroll(8, 8), "a cursing trap");
			ident = TRUE;

			break;
		}

	case TRAP_OF_WOUNDS_III:
		{
			msg_print("Something seems to incant terribly at you!");
			curse_equipment(80, 15);
			take_hit(damroll(10, 15), "a cursing trap");
			ident = TRUE;

			break;
		}

	case TRAP_OF_WOUNDS_IV:
		{
			msg_print("Something screams the word 'DIE!' in your direction!");
			take_hit(damroll(15, 15), "a cursing trap");
			(void)set_cut(p_ptr->cut + damroll(10, 10));
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_I:
		{
			int percentdmg = (p_ptr->chp / 20);
			if (percentdmg < 1) percentdmg = 1;
			msg_print("You trip over a wire!");
			take_hit(percentdmg, "a wire trap");
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_II:
		{
			int percentdmg = (p_ptr->chp / 10);
			if (percentdmg < 1) percentdmg = 1;
			msg_print("You're hit by a lashing whip!");
			take_hit(percentdmg, "a lash trap");
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_III:
		{
			int percentdmg = (p_ptr->chp / 5);
			if (percentdmg < 1) percentdmg = 1;
			msg_print("You're hit by a sword!");
			take_hit(percentdmg, "a sword trap");
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_IV:
		{
			int percentdmg = (p_ptr->chp / 3);
			if (percentdmg < 1) percentdmg = 1;
			msg_print("You're hit by an axe!");
			take_hit(percentdmg, "an axe trap");
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_V:
		{
			int percentdmg = (p_ptr->chp / 2);
			if (percentdmg < 5) percentdmg = 5;
			msg_print("You're caught in an iron maiden!");
			take_hit(percentdmg, "an iron maiden trap");
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_VI:
		{
			int percentdmg = (p_ptr->chp * 2 / 3);
			if (percentdmg < 5) percentdmg = 5;
			msg_print("You're nearly decapitated by a guillotine!");
			take_hit(percentdmg, "a guillotine trap");
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_VII:
		{
			int percentdmg = (p_ptr->chp * 3 / 4);
			if (percentdmg < 5) percentdmg = 5;
			msg_print("You're nearly bisected by a razor-sharp blade!");
			take_hit(percentdmg, "a bisection trap");
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_VIII:
		{
			int percentdmg = (p_ptr->chp * 4 / 5);
			if (percentdmg < 10) percentdmg = 10;
			msg_print("You're severely hurt by a rack!");
			take_hit(percentdmg, "a rack trap");
			ident = TRUE;

			break;
		}

	case TRAP_PERCENT_DMG_IX:
		{
			int percentdmg = (p_ptr->chp * 9 / 10);
			if (percentdmg < 10) percentdmg = 10;
			msg_print("Your knee is crunched extremely painfully!");
			take_hit(percentdmg, "a knee splitter trap");
			ident = TRUE;

			break;
		}

	case TRAP_OF_HAND_DOOM:
		{
			msg_print("The Hand of Doom is invoked against you!");
			curse_equipment(100, 20);
			int dummy = (((s32b) ((65 + randint(25)) * (p_ptr->chp))) / 100);
			int maxdummy = damroll(15, 30);
			if (dummy > maxdummy) dummy = maxdummy;
			msg_print("Your feel your life fade away!");
			take_hit(dummy, "the Hand of Doom");
			if (p_ptr->chp < 1) p_ptr->chp = 1;
			ident = TRUE;

			break;
		}

	case TRAP_OF_CUTS_I:
		{
			msg_print("You get a cut.");
			(void)set_cut(p_ptr->cut + 20);
			ident = TRUE;

			break;
		}

	case TRAP_OF_CUTS_II:
		{
			msg_print("You get a heavy cut.");
			(void)set_cut(p_ptr->cut + 50);
			ident = TRUE;

			break;
		}

	case TRAP_OF_CUTS_III:
		{
			msg_print("You get a nasty cut.");
			(void)set_cut(p_ptr->cut + 100);
			ident = TRUE;

			break;
		}

	case TRAP_OF_CUTS_IV:
		{
			msg_print("You get a severe cut.");
			(void)set_cut(p_ptr->cut + 200);
			ident = TRUE;

			break;
		}

	case TRAP_OF_CUTS_V:
		{
			msg_print("You get a mortal cut.");
			(void)set_cut(p_ptr->cut + 500);
			ident = TRUE;

			break;
		}

	case TRAP_OF_EXP_LOSS_I:
		{
			msg_print("You feel a loss of experience!");
			p_ptr->exp *= 9;
			p_ptr->exp /= 10;
			p_ptr->max_exp *= 9;
			p_ptr->max_exp /= 10;
			check_experience();

			ident = TRUE;

			break;
		}

	case TRAP_OF_EXP_LOSS_II:
		{
			msg_print("You feel a big loss of experience!");
			p_ptr->exp *= 4;
			p_ptr->exp /= 5;
			p_ptr->max_exp *= 4;
			p_ptr->max_exp /= 5;
			check_experience();

			ident = TRUE;

			break;
		}

	case TRAP_OF_EXP_LOSS_III:
		{
			msg_print("You feel a huge loss of experience!");
			p_ptr->exp *= 2;
			p_ptr->exp /= 3;
			p_ptr->max_exp *= 2;
			p_ptr->max_exp /= 3;
			check_experience();

			ident = TRUE;

			break;
		}

	case TRAP_OF_EXP_LOSS_IV:
		{
			msg_print("You feel an absurd loss of experience!");
			p_ptr->exp /= 2;
			p_ptr->max_exp /= 2;
			check_experience();

			ident = TRUE;

			break;
		}

	case TRAP_OF_EXP_LOSS_V:
		{
			msg_print("You feel a total loss of experience!");
			p_ptr->exp /= 10;
			p_ptr->max_exp /= 10;
			check_experience();

			ident = TRUE;

			break;
		}

		/* Destruction Trap */
	case TRAP_OF_DESTRUCTION:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;
			msg_print("Suddenly, you hear a rumbling sound!");

			/* Prevent destruction of quest levels and town */
			if (!is_quest(dun_level) || (is_quest(dun_level) == QUEST_RANDOM))
			{
				destroy_area(p_ptr->py, p_ptr->px, 5, TRUE, FALSE);
			}
			else
			{
				msg_print("The dungeon trembles...");
			}

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Destruction.");

			break;
		}
	case TRAP_OF_STAR_DESTRUCTION:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;
			msg_print("Suddenly, you hear a loud rumbling sound!");

			/* Prevent destruction of quest levels and town */
			if (!is_quest(dun_level) || (is_quest(dun_level) == QUEST_RANDOM))
			{
				destroy_area(p_ptr->py, p_ptr->px, 15, TRUE, FALSE);
			}
			else
			{
				msg_print("The dungeon trembles...");
			}

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Star Destruction.");

			break;
		}

	case TRAP_OF_WILD_MAGIC:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;
			msg_print("Wild magic is released!");
			wild_magic(randint(40));

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Wild Magic.");

			break;
		}

	case TRAP_OF_FATE:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			gain_fate(0);

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Fate.");

			break;
		}

		/* Bowel Cramps Trap */
	case TRAP_OF_BOWEL_CRAMPS:
		{
			msg_print("A wretched-smelling gas cloud upsets your stomach.");

			(void)set_food(PY_FOOD_STARVE - 1);
			(void)set_poisoned(0);

			if (!p_ptr->free_act || (rand_int(p_ptr->nastytrap57 ? 20 : 100) == 0) )
			{
				if (!p_ptr->resist_nerve || magik(50)) {
					(void)set_paralyzed(p_ptr->paralyzed + rand_int(dun_level) + 6);
				}
			}
			ident = TRUE;
			break;
		}

		/* trap that dismisses all your pets/companions, by Amy */
	case TRAP_OF_DISMISS_PETS:
		{
			int pets = 0, pet_ctr = 0;
			monster_type *m_ptr;
			int Dismissed = 0;

			ident = FALSE;

			for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				/* Access the monster */
				m_ptr = &m_list[pet_ctr];

				if (m_ptr->status >= MSTATUS_FRIEND) pets++;
			}

			msg_print("Suddenly, a call is heard in the distance...");

			for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				monster_race *r_ptr;

				/* Access the monster */
				m_ptr = &m_list[pet_ctr];
				r_ptr = &r_info[m_ptr->r_idx];

				if ((!(r_ptr->flags7 & RF7_NO_DEATH)) && ((m_ptr->status == MSTATUS_PET) || (m_ptr->status == MSTATUS_FRIEND) || (m_ptr->status == MSTATUS_COMPANION)))	/* Get rid of it! */
				{
					delete_monster_idx(pet_ctr);
					Dismissed++;
				}
			}
			if (Dismissed) {
				msg_format("%d pet%s have been dismissed.", Dismissed, (Dismissed == 1 ? "" : "s"));
				ident = TRUE;
			}

			break;
		}

	case TRAP_OF_GLUTTONY:
		{
			msg_print("Suddenly, your stomach fills with way too much food!");

			(void)set_food(PY_FOOD_MAX + 2000);

			ident = TRUE;
			break;
		}

		/* Blindness/Confusion Trap */
	case TRAP_OF_BLINDNESS_CONFUSION:
		{
			msg_print("A powerful magic protected this.");

			if (!p_ptr->resist_blind || p_ptr->nastytrap29 || (rand_int(100) < 5) )
			{
				set_blind(p_ptr->blind + rand_int(100) + 100);
			}
			if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
			{
				set_confused(p_ptr->confused + rand_int(20) + 15);
			}
			ident = TRUE;
			break;
		}

	case TRAP_OF_BLIND_CONF_X:
		{
			msg_print("A powerful magic protected this.");

			set_blind(p_ptr->blind + rand_int(100) + 100);
			set_confused(p_ptr->confused + rand_int(30) + 15);
			ident = TRUE;
			break;
		}

	case TRAP_OF_BLINDNESS:
		{
			msg_print("A flash of light hits you!");

			if (!p_ptr->resist_blind || p_ptr->nastytrap29 || (rand_int(100) < 5) )
			{
				set_blind(p_ptr->blind + rand_int(200) + 200);
			}
			ident = TRUE;
			break;
		}

	case TRAP_OF_LIFE_LOSS:
		{
			int curlvl;

			for (curlvl = 1; curlvl < PY_MAX_LEVEL; curlvl++)
			{
				int iterations = curlvl;

				while (iterations > 0) {
					if ((player_hp[curlvl] > 1) && magik(20)) player_hp[curlvl]--;
					iterations--;
				}
			}

			msg_print("Your life seems to fade!");

			/* Update and redraw hitpoints */
			p_ptr->update |= (PU_HP);
			p_ptr->redraw |= (PR_HP);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);

			/* Handle stuff */
			handle_stuff();

			ident = TRUE;
			break;
		}

	case TRAP_OF_LIFE_LOSS_II:
		{
			int curlvl;

			for (curlvl = 1; curlvl < PY_MAX_LEVEL; curlvl++)
			{
				int iterations = curlvl;

				while (iterations > 0) {
					if ((player_hp[curlvl] > 1) && magik(40)) player_hp[curlvl]--;
					iterations--;
				}
			}

			msg_print("Your life seems to fade!");

			/* Update and redraw hitpoints */
			p_ptr->update |= (PU_HP);
			p_ptr->redraw |= (PR_HP);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);

			/* Handle stuff */
			handle_stuff();

			ident = TRUE;
			break;
		}

	case TRAP_OF_LIFE_LOSS_III:
		{
			int curlvl;

			for (curlvl = 1; curlvl < PY_MAX_LEVEL; curlvl++)
			{
				int iterations = curlvl;

				while (iterations > 0) {
					if ((player_hp[curlvl] > 1) && magik(70)) player_hp[curlvl]--;
					iterations--;
				}
			}

			msg_print("Your life seems to fade!");

			/* Update and redraw hitpoints */
			p_ptr->update |= (PU_HP);
			p_ptr->redraw |= (PR_HP);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);

			/* Handle stuff */
			handle_stuff();

			ident = TRUE;
			break;
		}

	case TRAP_SKILL_LORE:
		{
			drain_skill_lore();

			ident = TRUE;
			break;

		}

	case TRAP_SKILL_DRAIN:
		{
			drain_skill_random();

			ident = TRUE;
			break;

		}

	case TRAP_SKILL_VALUE:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			swap_skill_value();

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Skill Value Swap.");
			break;
		}

	case TRAP_SKILL_MULT:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			swap_skill_mult();

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Skill Multiplier Swap.");
			break;
		}

	case TRAP_SKILL_SWAP:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			swap_skill_both();

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Skill Swap.");
			break;
		}

	case TRAP_OF_CONFUSION:
		{
			msg_print("The world is spinning!");

			if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
			{
				set_confused(p_ptr->confused + rand_int(40) + 20);
			}
			ident = TRUE;
			break;
		}

	case TRAP_OF_BLINDNESS_X:
		{
			msg_print("A flash of light hits you!");

			set_blind(p_ptr->blind + rand_int(250) + 250);
			ident = TRUE;
			break;
		}

	case TRAP_OF_CONFUSION_X:
		{
			msg_print("The world is spinning!");

			set_confused(p_ptr->confused + rand_int(60) + 30);
			ident = TRUE;
			break;
		}

		/* Aggravation Trap */
	case TRAP_OF_AGGRAVATION:
		{
			msg_print("You hear a hollow noise echoing through the dungeons.");
			aggravate_monsters(1);
			ident = TRUE;
			break;
		}

		/* Darkness Trap */
	case TRAP_OF_DARKNESS:
		{
			msg_print("The area around you gets dark!");
			if (unlite_area(10, 3)) ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_FIRE:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_FIRE, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_ACID:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_ACID, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_COLD:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_COLD, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_ELEC:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_ELEC, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_PLASMA:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_PLASMA, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_WATER:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_WATER, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_SHARDS:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_SHARDS, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_SOUND:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_SOUND, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_ICE:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_ICE, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_CHAOS:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_CHAOS, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_NETHER:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_NETHER, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_DISEN:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_DISENCHANT, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_NEXUS:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_NEXUS, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_NERVE:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_NERVE, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_MIND:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_MIND, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_ETHER:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_ETHER, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_TIME:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_TIME, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_GRAV:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_GRAVITY, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ELE_NUKE:
		{
			project( -2, 0, p_ptr->py, p_ptr->px, 1, GF_NUKE, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
			ident = TRUE;
			break;
		}

		/* Multiplication Trap */
	case TRAP_OF_MULTIPLICATION:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			msg_print("You hear a loud click.");
			for (k = -1; k <= 1; k++)
				for (l = -1; l <= 1; l++)
				{
					if ((in_bounds(p_ptr->py + l, p_ptr->px + k)) &&
					                (!cave[p_ptr->py + l][p_ptr->px + k].t_idx))
					{
						place_trap(p_ptr->py + l, p_ptr->px + k);
					}
				}

			/* if we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}

			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Multiplication Trap.");
			break;
		}

		/* Super Multiplication Trap, by Amy */
	case TRAP_OF_MULTIPLY_X:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			msg_print("You hear a loud click.");
			for (k = -2; k <= 2; k++)
				for (l = -2; l <= 2; l++)
				{
					if ((in_bounds(p_ptr->py + l, p_ptr->px + k)) &&
					                (!cave[p_ptr->py + l][p_ptr->px + k].t_idx))
					{
						place_trap(p_ptr->py + l, p_ptr->px + k);
					}
				}

			/* if we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}

			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Field Trap.");
			break;
		}

		/* cluster trap by Amy: create 5x5 field of traps somewhere on the level */
	case TRAP_OF_CLUSTER:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			int clusx, clusy;
			int attempts_left = 10000;
			/* Find a legal, unoccupied space */
			while (attempts_left--)
			{
				/* Pick a location */
				clusy = rand_int(cur_hgt);
				clusx = rand_int(cur_wid);

				/* Require empty floor grid (was "naked") */
				if (!cave_empty_bold(clusy, clusx)) continue;

				/* we've found a good location! */
				break;
			}

			if (!attempts_left) {
				msg_print("You hear a muffled click.");
				if (!p_ptr->nastytrap3) msg_print("You identified that trap as Cluster Trap.");
			}

			msg_print("You hear a distant click.");
			for (k = -2; k <= 2; k++)
				for (l = -2; l <= 2; l++)
				{
					if ((in_bounds(clusy + l, clusx + k)) &&
					                (!cave[clusy + l][clusx + k].t_idx))
					{
						place_trap(clusy + l, clusx + k);
					}
				}

			/* if we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}

			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Cluster Trap.");
			break;
		}

	case TRAP_OF_INTERLACE:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			int trapcount = 20 + randint(20);

			msg_print("You hear distant clicking sounds.");

			while (trapcount > 0) {
				trapcount--;
				alloc_trap();
			}

			/* if we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}

			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Interlace Trap.");
			break;
		}

		/* Steal Item Trap */
	case TRAP_OF_STEAL_ITEM:
		{
			/*
			 * please note that magical stealing is not so
			 * easily circumvented
			 */
			if (!p_ptr->paralyzed &&
			                (rand_int(160) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
			                                  p_ptr->lev)))
			{
				/* Saving throw message */
				msg_print("Your backpack seems to vibrate strangely!");
				break;
			}

			/* Find an item */
			for (k = 0; k < rand_int(10); k++)
			{
				char i_name[80];
				object_type *j_ptr, *q_ptr, forge;

				/* Pick an item */
				s16b i = rand_int(INVEN_PACK);

				/* Obtain the item */
				j_ptr = &p_ptr->inventory[i];

				/* Accept real items */
				if (!j_ptr->k_idx) continue;

				/* Don't steal artifacts  -CFT */
				if (artifact_p(j_ptr)) continue;

				/* Get a description */
				object_desc(i_name, j_ptr, FALSE, 3);

				/* Message */
				msg_format("%sour %s (%c) was stolen!",
				           ((j_ptr->number > 1) ? "One of y" : "Y"),
				           i_name, index_to_label(i));

				/* Create the item */
				q_ptr = &forge;
				object_copy(q_ptr, j_ptr);
				q_ptr->number = 1;

				/* Drop it somewhere */
				do_trap_teleport_away(q_ptr, y, x);

				inven_item_increase(i, -1);
				inven_item_optimize(i);
				ident = TRUE;
			}
			break;
		}

	case TRAP_OF_STEAL_ITEM_II:
		{
			/* Find an item */
			for (k = 0; k < 25; k++)
			{
				char i_name[80];
				object_type *j_ptr, *q_ptr, forge;

				/* Pick an item */
				s16b i = rand_int(INVEN_PACK);

				/* Obtain the item */
				j_ptr = &p_ptr->inventory[i];

				/* Accept real items */
				if (!j_ptr->k_idx) continue;

				/* Don't steal artifacts  -CFT */
				if (artifact_p(j_ptr)) continue;

				/* Get a description */
				object_desc(i_name, j_ptr, FALSE, 3);

				/* Message */
				msg_format("%sour %s (%c) was stolen!",
				           ((j_ptr->number > 1) ? "One of y" : "Y"),
				           i_name, index_to_label(i));

				/* Create the item */
				q_ptr = &forge;
				object_copy(q_ptr, j_ptr);
				q_ptr->number = 1;

				/* Drop it somewhere */
				do_trap_teleport_away(q_ptr, y, x);

				inven_item_increase(i, -1);
				inven_item_optimize(i);
				ident = TRUE;
			}
			break;
		}

	case TRAP_OF_STEAL_ITEM_III:
		{
			/* Find an item */
			for (k = 0; k < 200; k++)
			{
				char i_name[80];
				object_type *j_ptr, *q_ptr, forge;
				u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

				/* Pick an item */
				s16b i = rand_int(INVEN_PACK);

				/* Obtain the item */
				j_ptr = &p_ptr->inventory[i];

				/* Accept real items */
				if (!j_ptr->k_idx) continue;

				/* can also steal artifacts :-P --Amy */
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
				if(f3 & TR3_PERMA_CURSE) continue;

				/* Get a description */
				object_desc(i_name, j_ptr, FALSE, 3);

				/* Message */
				msg_format("%sour %s (%c) was stolen!",
				           ((j_ptr->number > 1) ? "One of y" : "Y"),
				           i_name, index_to_label(i));

				/* Create the item */
				q_ptr = &forge;
				object_copy(q_ptr, j_ptr);
				q_ptr->number = 1;

				/* Drop it somewhere */
				do_trap_teleport_away(q_ptr, y, x);

				inven_item_increase(i, -1);
				inven_item_optimize(i);
				ident = TRUE;
			}
			break;
		}

		/* Summon Fast Quylthulgs Trap */
	case TRAP_OF_SUMMON_FAST_QUYLTHULGS:
		{
			for (k = 0; k < randint(3); k++)
			{
				ident |= summon_specific(y, x, max_dlv_real[dungeon_type], SUMMON_QUYLTHULG);
			}

			if (ident)
			{
				msg_print("You suddenly have company.");
				(void)set_slow(p_ptr->slow + randint(25) + 15);
			}

			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Summon Fast Quylthulgs Trap.");
				ident = FALSE;

			}
			break;
		}

		/* Trap of levelporting */
	case TRAP_OF_TELE_LEVEL:
		{
			msg_print("You trigger a level teleport trap!");
			ident = TRUE;

			/* Autosave enabled */
			if (autosave_l)
			{
				is_autosave = TRUE;
				msg_print("Autosaving the game...");
				do_cmd_save_game();
				is_autosave = FALSE;
			}

			teleport_player_level_trap();

			break;
		}

		/* Trap of Sinking */
	case TRAP_OF_SINKING:
		{
			msg_print("You fell through a trap door!");
			ident = TRUE;

			if (p_ptr->ffall)
			{
				if (dungeon_flags1 & DF1_TOWER)
				{
					msg_print("You float gently down to the previous level.");
				}
				else
				{
					msg_print("You float gently down to the next level.");
				}
			}
			else
			{
				take_hit(damroll(2, 8), "a trap door");
			}

			/* Still alive and autosave enabled */
			if (autosave_l && (p_ptr->chp >= 0))
			{
				is_autosave = TRUE;
				msg_print("Autosaving the game...");
				do_cmd_save_game();
				is_autosave = FALSE;
			}

			if (dungeon_flags1 & DF1_TOWER) dun_level--;
			else dun_level++;

			/* Leaving */
			p_ptr->leaving = TRUE;
			break;
		}

		/* Shaft Trap */
	case TRAP_OF_SHAFT:
		{
			dungeon_info_type *d_ptr = &d_info[dungeon_type];

			msg_print("You fell through a shaft!");
			ident = TRUE;

			if (p_ptr->ffall)
			{
				if (dungeon_flags1 & DF1_TOWER)
				{
					msg_print("You float gently down to the previous level.");
				}
				else
				{
					msg_print("You float gently down to the next level.");
				}
			}
			else
			{
				take_hit(damroll(2, 8), "a trap door");
			}

			/* Still alive and autosave enabled */
			if (autosave_l && (p_ptr->chp >= 0))
			{
				is_autosave = TRUE;
				msg_print("Autosaving the game...");
				do_cmd_save_game();
				is_autosave = FALSE;
			}

			int i = randint(3) + 1, j;

			for (j = 1; j < i; j++)
			{
				if (dungeon_flags1 & DF1_TOWER) {
					dun_level--;
					if (is_quest(dun_level + i - 1) && (is_quest(dun_level + i - 1) != QUEST_RANDOM) ) break;
					if (d_ptr->maxdepth == dun_level) break;
				} else {
					dun_level++;
					if (is_quest(dun_level + i - 1) && (is_quest(dun_level + i - 1) != QUEST_RANDOM) ) break;
					if (d_ptr->maxdepth == dun_level) break;
				}
			}

			/* Leaving */
			p_ptr->leaving = TRUE;
			break;
		}

		/* Deep Shaft Trap */
	case TRAP_OF_DEEP_DESCENT:
		{
			dungeon_info_type *d_ptr = &d_info[dungeon_type];

			msg_print("You fell through a very deep shaft!");
			ident = TRUE;

			if (p_ptr->ffall)
			{
				if (dungeon_flags1 & DF1_TOWER)
				{
					msg_print("You float gently down to the previous level.");
				}
				else
				{
					msg_print("You float gently down to the next level.");
				}
			}
			else
			{
				take_hit(damroll(2, 8), "a trap door");
			}

			/* Still alive and autosave enabled */
			if (autosave_l && (p_ptr->chp >= 0))
			{
				is_autosave = TRUE;
				msg_print("Autosaving the game...");
				do_cmd_save_game();
				is_autosave = FALSE;
			}

			int i = 100, j;

			for (j = 1; j < i; j++)
			{
				if (dungeon_flags1 & DF1_TOWER) {
					dun_level--;
					if (is_quest(dun_level + i - 1) && (is_quest(dun_level + i - 1) != QUEST_RANDOM) ) break;
					if (d_ptr->maxdepth == dun_level) break;
				} else {
					dun_level++;
					if (is_quest(dun_level + i - 1) && (is_quest(dun_level + i - 1) != QUEST_RANDOM) ) break;
					if (d_ptr->maxdepth == dun_level) break;
				}
			}

			/* Leaving */
			p_ptr->leaving = TRUE;
			break;
		}

		/* Trap of Mana Drain */
	case TRAP_OF_MANA_DRAIN:
		{
			if (p_ptr->csp > 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				p_ptr->redraw |= (PR_MANA);
				msg_print("You sense a great loss.");
				ident = TRUE;
			}
			else if (p_ptr->msp == 0)
			{
				/* no sense saying this unless you never have mana */
				msg_format("Suddenly you feel glad you're a mere %s",
				           spp_ptr->title + c_name);
			}
			else
			{
				msg_print("Your head feels dizzy for a moment.");
			}
			break;
		}

		/* Trap of Tanker Drain - unlike old pernangband versions there's no "tank" here, so instead it temporarily halts mana regen --Amy */
	case TRAP_OF_TANKER_DRAIN:
		{
			set_tim_manavoid(p_ptr->tim_manavoid + 10 + p_ptr->msp);
			ident = TRUE;

			if (p_ptr->csp > 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				p_ptr->redraw |= (PR_MANA);
				msg_print("You sense a great loss.");
			}
			else if (p_ptr->msp == 0)
			{
				/* no sense saying this unless you never have mana */
				msg_format("Suddenly you feel glad you're a mere %s",
				           spp_ptr->title + c_name);
			}
			else
			{
				msg_print("Your head feels dizzy for a moment.");
			}
			break;
		}

	case TRAP_OF_SILENCE:
		{
			set_tim_manasilence(p_ptr->tim_manasilence + 10 + p_ptr->msp);
			ident = TRUE;

			msg_print("You've been silenced!");

			if (p_ptr->csp > 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
				p_ptr->redraw |= (PR_MANA);
				msg_print("You sense a great loss.");
			}
			else if (p_ptr->msp == 0)
			{
				/* no sense saying this unless you never have mana */
				msg_format("Suddenly you feel glad you're a mere %s",
				           spp_ptr->title + c_name);
			}
			else
			{
				msg_print("Your head feels dizzy for a moment.");
			}
			break;
		}

	case TRAP_OF_EXP_DRAIN_I:
		{
			ident = TRUE;

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

	case TRAP_OF_EXP_DRAIN_II:
		{
			ident = TRUE;

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

	case TRAP_OF_EXP_DRAIN_III:
		{
			ident = TRUE;

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

	case TRAP_OF_EXP_DRAIN_IV:
		{
			ident = TRUE;

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

	case TRAP_OF_EXP_DRAIN_V:
		{
			ident = TRUE;

			if (p_ptr->hold_life && !p_ptr->nastytrap95 && (rand_int(100) < 20))
			{
				msg_print("You keep hold of your life force!");
			}
			else
			{
				s32b d = damroll(500, 6) + (p_ptr->exp / 10) * MON_DRAIN_LIFE;
				if (p_ptr->hold_life && !p_ptr->nastytrap95)
				{
					msg_print("You feel your life slipping away!");
					lose_exp(d / 3);
				}
				else
				{
					msg_print("You feel your life draining away!");
					lose_exp(d);
				}
			}

			break;
		}

		/* Trap of Stat Scramble - like the nexus effect, by Amy */
	case TRAP_OF_STAT_SCRAMBLE:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			msg_print("Your body starts to scramble...");
			corrupt_player();

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Stat Scramble.");

			break;
		}

		/* Trap of Polymorph - like the radiation effect, by Amy */
	case TRAP_OF_POLYMORPH:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			do_poly_self();

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Polymorph.");

			break;
		}

		/* Trap of Missing Money */
	case TRAP_OF_MISSING_MONEY:
		{
			s32b gold = (p_ptr->au / 10) + randint(25);

			if (gold < 2) gold = 2;
			if (gold > 5000) gold = (p_ptr->au / 20) + randint(3000);
			if (gold > p_ptr->au) gold = p_ptr->au;

			p_ptr->au -= gold;
			if (gold <= 0)
			{
				msg_print("You feel something touching you.");
			}
			else if (p_ptr->au)
			{
				msg_print("Your purse feels lighter.");
				msg_format("%ld coins were stolen!", (long)gold);
				ident = TRUE;
			}
			else
			{
				msg_print("Your purse feels empty.");
				msg_print("All of your coins were stolen!");
				ident = TRUE;
			}
			p_ptr->redraw |= (PR_GOLD);
			break;
		}

	case TRAP_OF_MISSING_MONEY_II:
		{
			s32b gold = (p_ptr->au / 4) + randint(25);

			if (gold < 2) gold = 2;
			if (gold > 10000) gold = (p_ptr->au / 8) + randint(6000);
			if (gold > p_ptr->au) gold = p_ptr->au;

			p_ptr->au -= gold;
			if (gold <= 0)
			{
				msg_print("You feel something touching you.");
			}
			else if (p_ptr->au)
			{
				msg_print("Your purse feels lighter.");
				msg_format("%ld coins were stolen!", (long)gold);
				ident = TRUE;
			}
			else
			{
				msg_print("Your purse feels empty.");
				msg_print("All of your coins were stolen!");
				ident = TRUE;
			}
			p_ptr->redraw |= (PR_GOLD);
			break;
		}

	case TRAP_OF_MISSING_MONEY_III:
		{
			s32b gold = (p_ptr->au / 2) + randint(25);

			if (gold < 2) gold = 2;
			if (gold > 20000) gold = (p_ptr->au / 4) + randint(15000);
			if (gold > p_ptr->au) gold = p_ptr->au;

			p_ptr->au -= gold;
			if (gold <= 0)
			{
				msg_print("You feel something touching you.");
			}
			else if (p_ptr->au)
			{
				msg_print("Your purse feels lighter.");
				msg_format("%ld coins were stolen!", (long)gold);
				ident = TRUE;
			}
			else
			{
				msg_print("Your purse feels empty.");
				msg_print("All of your coins were stolen!");
				ident = TRUE;
			}
			p_ptr->redraw |= (PR_GOLD);
			break;
		}

	case TRAP_OF_MISSING_MONEY_IV:
		{
			if (p_ptr->au <= 0)
			{
				msg_print("You feel something touching you.");
			}
			else
			{
				p_ptr->au = 0;
				msg_print("Your purse feels empty.");
				msg_print("All of your coins were stolen!");
				ident = TRUE;
			}
			p_ptr->redraw |= (PR_GOLD);
			break;
		}

		/* Trap of No Return */
	case TRAP_OF_NO_RETURN:
		{
			object_type *j_ptr;
			s16b j;

			for (j = 0; j < INVEN_WIELD; j++)
			{
				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if ((j_ptr->tval == TV_SCROLL) &&
				                (j_ptr->sval == SV_SCROLL_WORD_OF_RECALL))
				{
					inven_item_increase(j, -j_ptr->number);
					inven_item_optimize(j);
					combine_pack();
					reorder_pack();
					if (!ident)
					{
						msg_print("A small fire works its way through your backpack. "
						          "Some scrolls are burnt.");
					}
					else
					{
						msg_print("The fire hasn't finished.");
					}
					ident = TRUE;
				}
				else if ((j_ptr->tval == TV_ROD_MAIN) &&
				                (j_ptr->pval == SV_ROD_RECALL))
				{
					inven_item_increase(j, -j_ptr->number);
					inven_item_optimize(j);
					combine_pack();
					reorder_pack();

					/*j_ptr->timeout = 0;*/  /* a long time */
					/* wut? that's just a hundred or so turns, lame! why not destroy that thing too? --Amy */
					if (!ident) msg_print("You feel the air stabilise around you.");
					else msg_print("Some of your rods burned up!");
					ident = TRUE;
				}
			}
			if ((!ident) && (p_ptr->word_recall))
			{
				msg_print("You feel like staying around.");
				p_ptr->word_recall = 0;
				ident = TRUE;
			}
			if (!ident) msg_print("You feel that you'll be here for a while.");
			break;
		}

	case TRAP_OF_NO_JUNK:
		{
			object_type *j_ptr;
			s16b j;

			for (j = 0; j < INVEN_WIELD; j++)
			{
				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if (j_ptr->tval == TV_RANDART)
				{
					inven_item_increase(j, -j_ptr->number);
					inven_item_optimize(j);
					combine_pack();
					reorder_pack();
					if (!ident)
					{
						msg_print("Some junk in your inventory burns up.");
					}
					else
					{
						msg_print("You lost your junkarts to a fire.");
					}
					ident = TRUE;
				}
			}
			if (!ident) msg_print("You feel happy that you're not carrying any junk.");
			break;
		}

	case TRAP_OF_NO_OIL:
		{
			object_type *j_ptr;
			s16b j;

			for (j = 0; j < INVEN_WIELD; j++)
			{
				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if (j_ptr->tval == TV_FLASK)
				{
					inven_item_increase(j, -j_ptr->number);
					inven_item_optimize(j);
					combine_pack();
					reorder_pack();
					if (!ident)
					{
						msg_print("Oh no! You forgot to pay your Yendorian Fuel Tax and your oil reserves got seized!");
					}
					else
					{
						msg_print("You have no oil left.");
					}
					ident = TRUE;
				}
			}
			if (!ident) msg_print("Some tax collector growls at you, but then decides to walk off.");
			break;
		}

	case TRAP_OF_NO_SPIKES:
		{
			object_type *j_ptr;
			s16b j;

			for (j = 0; j < INVEN_WIELD; j++)
			{
				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if (j_ptr->tval == TV_SPIKE)
				{
					inven_item_increase(j, -j_ptr->number);
					inven_item_optimize(j);
					combine_pack();
					reorder_pack();
					if (!ident)
					{
						msg_print("A malicious hand filches a bunch of spikes from your pack.");
					}
					else
					{
						msg_print("You're out of spikes.");
					}
					ident = TRUE;
				}
			}
			if (!ident) msg_print("A malicious hand tries to filch you, but doesn't find anything interesting.");
			break;
		}

	case TRAP_OF_NO_TOTEM:
		{
			object_type *j_ptr;
			s16b j;

			for (j = 0; j < INVEN_WIELD; j++)
			{
				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if (j_ptr->tval == TV_TOTEM)
				{
					inven_item_increase(j, -j_ptr->number);
					inven_item_optimize(j);
					combine_pack();
					reorder_pack();
					if (!ident)
					{
						msg_print("Your totems erupt in a column of fire and smoke!");
					}
					else
					{
						msg_print("Now you don't have your totems no more.");
					}
					ident = TRUE;
				}
			}
			if (!ident) msg_print("You have a vision of a totem pole.");
			break;
		}

	case TRAP_OF_KILL_SYMBIOTE:
		{
			object_type *j_ptr;
			s16b j;

			msg_print("You feel something probing you!");

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{
				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if (j == INVEN_CARRY) {
					cptr sym_name = symbiote_name(TRUE);
					msg_format("%s has been blown to smithereens.", sym_name);
					inven_item_increase(INVEN_CARRY, -1);
					inven_item_optimize(INVEN_CARRY);
					p_ptr->redraw |= PR_MH;
					ident = TRUE;
				}
			}
			break;
		}

		/* Trap of Silent Switching */
	case TRAP_OF_SILENT_SWITCHING:
		{
			s16b i, j, slot1, slot2;
			object_type *j_ptr, *k_ptr;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
			{
				j_ptr = &p_ptr->inventory[i];

				if (!j_ptr->k_idx) continue;

				/* Do not allow this trap to touch the One Ring */
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
				if(f3 & TR3_PERMA_CURSE) continue;

				slot1 = wield_slot(j_ptr);

				for (j = 0; j < INVEN_WIELD; j++)
				{
					k_ptr = &p_ptr->inventory[j];

					if (!k_ptr->k_idx) continue;

					/* Do not allow this trap to touch the One Ring */
					object_flags(k_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					/* this is a crude hack, but it prevent wielding 6 torches... */
					if (k_ptr->number > 1) continue;

					slot2 = wield_slot(k_ptr);

					/* a chance of 4 in 5 of switching something, then 2 in 5 to do it again */
					if ((slot1 == slot2) &&
					                (rand_int(100) < (80 - (ident * 40))))
					{
						object_type tmp_obj;

						tmp_obj = p_ptr->inventory[j];
						p_ptr->inventory[j] = p_ptr->inventory[i];
						p_ptr->inventory[i] = tmp_obj;
						ident = TRUE;
					}
				}
			}

			if (ident)
			{
				p_ptr->update |= (PU_BONUS);
				p_ptr->update |= (PU_TORCH);
				p_ptr->update |= (PU_MANA);
				msg_print("You somehow feel like another person.");
			}
			else
			{
				msg_print("You feel a lack of useful items.");
			}
			break;
		}

		/* Trap of Walls */
	case TRAP_OF_WALLS:
		{
			ident = player_handle_trap_of_walls();
			break;
		}

	case TRAP_OF_SANITY_DRAIN:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev;
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel < 2) efflevel = 2;

			int sanitydamage;

			cmsg_print(TERM_VIOLET, "You're going insane!");

			sanitydamage = randint(50);
			if (sanitydamage > (efflevel * 3)) sanitydamage = efflevel * 3;

			take_sanity_hit(sanitydamage + p_ptr->lev, "a trap of sanity draining");
			ident = TRUE;
			break;
		}

	case TRAP_OF_SANITY_SMASH:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev;
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel < 2) efflevel = 2;

			int sanitydamage;

			cmsg_print(TERM_VIOLET, "You're going insane!");

			sanitydamage = randint(100);
			if (sanitydamage > (efflevel * 4)) sanitydamage = efflevel * 4;

			take_sanity_hit(sanitydamage + (p_ptr->lev * 2), "a trap of sanity smashing");
			ident = TRUE;
			break;
		}

	case TRAP_OF_SANITY_SMASH_II:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else {
				efflevel = p_ptr->lev;
				if (p_ptr->lev >= 45) efflevel = 90;
			}
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel < 2) efflevel = 2;

			int sanitydamage;

			cmsg_print(TERM_VIOLET, "You're going insane!");

			sanitydamage = randint(200);
			if (sanitydamage > (efflevel * 4)) sanitydamage = efflevel * 4;

			take_sanity_hit(sanitydamage + (p_ptr->lev * 2), "a trap of sanity trashing");
			ident = TRUE;
			break;
		}

	case TRAP_OF_SANITY_SMASH_III:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else {
				efflevel = p_ptr->lev;
				if (p_ptr->lev >= 45) efflevel = 90;
			}
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel < 2) efflevel = 2;

			int sanitydamage;

			cmsg_print(TERM_VIOLET, "You're going insane!");

			sanitydamage = randint(400);
			if (sanitydamage > (efflevel * 4)) sanitydamage = efflevel * 4;

			take_sanity_hit(sanitydamage + (p_ptr->lev * 2), "a trap of high sanity loss");
			ident = TRUE;
			break;
		}

	case TRAP_OF_FEAR:
		{
			msg_format("You get the cold shivers.");
			if (!p_ptr->resist_fear || p_ptr->nastytrap30 || (rand_int(100) < 5) ) {
				set_afraid(p_ptr->afraid + 50 + randint(200));
			}
			ident = TRUE;
			break;
		}

	case TRAP_OF_FEAR_X:
		{
			msg_format("You get the cold shivers.");
			set_afraid(p_ptr->afraid + 50 + randint(200));
			ident = TRUE;
			break;
		}

	case TRAP_OF_STUNNING:
		{
			msg_format("You stagger...");
			(void)set_stun(p_ptr->stun + randint(50) + p_ptr->lev);
			ident = TRUE;
			break;
		}

	case TRAP_OF_STUN_X:
		{
			msg_format("You stagger...");
			(void)set_stun(p_ptr->stun + randint(100) + (p_ptr->lev * 2));
			ident = TRUE;
			break;
		}

	case TRAP_OF_CONTAMINATE_I:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev * 2;

			msg_format("Your Geiger counter is ticking!");

			contaminate(10 + (efflevel / 3));

			ident = TRUE;
			break;
		}

	case TRAP_OF_CONTAMINATE_II:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev * 2;

			msg_format("Your Geiger counter is ticking!");

			contaminate(100 + (efflevel / 2));

			ident = TRUE;
			break;
		}

	case TRAP_OF_CONTAMINATE_III:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev * 2;

			msg_format("Your Geiger counter is ticking!");

			contaminate(200 + efflevel);

			ident = TRUE;
			break;
		}

	case TRAP_OF_CONTAMINATE_IV:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev * 2;

			msg_format("Your Geiger counter is ticking!");

			contaminate(500 + (efflevel * 2));

			ident = TRUE;
			break;
		}

	case TRAP_OF_CONTAMINATE_V:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev * 2;

			msg_format("Your Geiger counter is ticking!");

			contaminate(1000 + (efflevel * 3));

			ident = TRUE;
			break;
		}

	case TRAP_OF_CONTAMINATE_VI:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev * 2;

			msg_format("Your Geiger counter is ticking!");

			contaminate(2000 + (efflevel * 5));

			ident = TRUE;
			break;
		}

	case TRAP_OF_CONTAMINATE_VII:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev * 2;

			msg_format("Your Geiger counter is ticking!");

			contaminate(10000 + (efflevel * 10));

			ident = TRUE;
			break;
		}

	case TRAP_OF_ITEM_DESTRUCT_I:
		{
			msg_print("You're hit by the elements!");
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ITEM_DESTRUCT_II:
		{
			msg_print("You're hit by the elements!");
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ITEM_DESTRUCT_III:
		{
			msg_print("You're hit by the elements!");
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			ident = TRUE;
			break;
		}

	case TRAP_OF_ITEM_DESTRUCT_IV:
		{
			msg_print("You're hit by the elements!");
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			inven_damage(set_cold_destroy, 3);
			inven_damage(set_fire_destroy, 3);
			inven_damage(set_elec_destroy, 3);
			inven_damage(set_acid_destroy, 3);
			ident = TRUE;
			break;
		}

		/* Trap of Calling Out */
	case TRAP_OF_CALLING_OUT:
		{
			ident = do_player_trap_call_out();

			if (!ident)
			{
				/* Increase "afraid" */
				if (p_ptr->resist_fear && !p_ptr->nastytrap30 && (rand_int(100) > 4) )
				{
					msg_print("You feel as if you had a nightmare!");
				}
				else if (rand_int(100) < player_actual_saving_throw())
				{
					msg_print("You remember having a nightmare!");
				}
				else
				{
					if (set_afraid(p_ptr->afraid + 3 + randint(40)))
					{
						msg_print("You have a vision of a powerful enemy.");
					}
				}
			}

			/* thwart endless farming; in this case, I myself am guilty of doing so --Amy */
			if (randint(10) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Trap of Calling Out.");
				ident = FALSE;

			}

			break;
		}

		/* Trap of Sliding */
	case TRAP_OF_SLIDING:

			/* make this trap do something --Amy */
			msg_print("You are pushed around!");
			teleport_player(5);
			/* ...unfortunately teleport_player does not return a value, even if the player has anti-teleportation,
			 * so let's just always tell them what trap it is. */
			ident = TRUE;
			break;

		break;

		/* abomination traps (turn you into a bad mimicry form) --Amy */
	case TRAP_OF_ABOMINATION_I:
			if (magik(60 - get_skill_scale(SKILL_MIMICRY, 25)))
			{
				cmsg_print(TERM_VIOLET, "You feel the dark powers twisting your body!");
				set_mimic(50, resolve_mimic_name("Abomination"), 50);
			}
			else
			{
				cmsg_print(TERM_VIOLET, "You feel the dark powers trying to twisting your body, but they fail.");
			}
			ident = TRUE;
		break;
	case TRAP_OF_ABOMINATION_II:
			if (magik(60 - get_skill_scale(SKILL_MIMICRY, 25)))
			{
				cmsg_print(TERM_VIOLET, "You feel the dark powers twisting your body!");
				set_mimic(250, resolve_mimic_name("Abomination"), 50);
			}
			else
			{
				cmsg_print(TERM_VIOLET, "You feel the dark powers trying to twisting your body, but they fail.");
			}
			ident = TRUE;
		break;
	case TRAP_OF_ABOMINATION_III:
			if (magik(60 - get_skill_scale(SKILL_MIMICRY, 25)))
			{
				cmsg_print(TERM_VIOLET, "You feel the dark powers twisting your body!");
				set_mimic(5000, resolve_mimic_name("Abomination"), 50);
			}
			else
			{
				cmsg_print(TERM_VIOLET, "You feel the dark powers trying to twisting your body, but they fail.");
			}
			ident = TRUE;
		break;

		/* unknown traps: should normally morph into something else prior to triggering */
	case TRAP_OF_UNKNOWN:
	case TRAP_OF_UNKNOWN_II:
	case TRAP_OF_UNKNOWN_III:
	case TRAP_OF_UNKNOWN_IV:
	case TRAP_OF_UNKNOWN_V:
	case TRAP_OF_UNKNOWN_VI:
	case TRAP_OF_UNKNOWN_VII:
	case TRAP_OF_UNKNOWN_VIII:
	case TRAP_OF_UNKNOWN_OOD:
	case TRAP_OF_UNKNOWN_OOD_II:
	case TRAP_OF_UNKNOWN_OOD_III:
	case TRAP_OF_UNKNOWN_RARE:
	case TRAP_OF_UNKNOWN_RARE_II:
	case TRAP_OF_UNKNOWN_RARE_III:
	case TRAP_OF_UNKNOWN_OOD_RARE:
	case TRAP_OF_UNKNOWN_OOD_RARE_II:
	case TRAP_OF_UNKNOWN_OOD_RARE_III:
			ident = FALSE;
		break;

		/* Trap of Charges Drain */
	case TRAP_OF_CHARGES_DRAIN:
		{
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			/* Find an item */
			for (k = 0; k < 10; k++)
			{
				s16b i = rand_int(INVEN_PACK);

				object_type *j_ptr = &p_ptr->inventory[i];

				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* Drain charged wands/staffs
				   Hack -- don't let artifacts get drained */
				if (((j_ptr->tval == TV_STAFF) || (j_ptr->tval == TV_ROD_MAIN) ||
				                (j_ptr->tval == TV_WAND)) &&
				                (j_ptr->pval) &&
			                     !artifact_p(j_ptr) && !( (f5 & (TR5_CHARGE_HOLDING)) && (randint(10) != 1) ) )
				{
					ident = TRUE;

					if (j_ptr->tval == TV_ROD_MAIN) {
						j_ptr->timeout = j_ptr->timeout / (randint(4) + 1);
					} else {
						j_ptr->pval = j_ptr->pval / (randint(4) + 1);
					}


					/* 60% chance of only 1 */
					if (randint(10) > 3) break;
				}
			}

			if (ident)
			{
				/* Window stuff */
				p_ptr->window |= PW_INVEN;
				/* Combine / Reorder the pack */
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				msg_print("Your backpack seems to be turned upside down.");
			}
			else
			{
				msg_print("You hear a wail of great disappointment.");
			}
			break;
		}

	case TRAP_OF_CHARGES_DRAIN_II:
		{
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			/* Find an item */
			for (k = 0; k < 10; k++)
			{
				s16b i = rand_int(INVEN_PACK);

				object_type *j_ptr = &p_ptr->inventory[i];

				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* Drain charged wands/staffs
				   Hack -- don't let artifacts get drained */
				if (((j_ptr->tval == TV_STAFF) || (j_ptr->tval == TV_ROD_MAIN) ||
				                (j_ptr->tval == TV_WAND)) &&
				                (j_ptr->pval) &&
			                     !artifact_p(j_ptr) && !( (f5 & (TR5_CHARGE_HOLDING)) && (randint(10) != 1) ) )
				{
					ident = TRUE;

					if (j_ptr->tval == TV_ROD_MAIN) {
						j_ptr->timeout = j_ptr->timeout / (randint(8) + 1);
					} else {
						j_ptr->pval = j_ptr->pval / (randint(8) + 1);
					}


					/* 20% chance of only 1 */
					if (randint(10) > 8) break;
				}
			}

			if (ident)
			{
				/* Window stuff */
				p_ptr->window |= PW_INVEN;
				/* Combine / Reorder the pack */
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				msg_print("Your backpack seems to be turned upside down.");
			}
			else
			{
				msg_print("You hear a wail of great disappointment.");
			}
			break;
		}

	case TRAP_OF_CHARGES_DRAIN_III:
		{
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			/* Find an item */
			for (k = 0; k < 10; k++)
			{
				s16b i = rand_int(INVEN_PACK);

				object_type *j_ptr = &p_ptr->inventory[i];

				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* Drain charged wands/staffs, can also affect artifacts muahahahaha --Amy */
				if (((j_ptr->tval == TV_STAFF) || (j_ptr->tval == TV_ROD_MAIN) ||
				                (j_ptr->tval == TV_WAND)) &&
				                (j_ptr->pval) && !( (f5 & (TR5_CHARGE_HOLDING)) && (randint(10) != 1) ) )
				{
					ident = TRUE;

					if (j_ptr->tval == TV_ROD_MAIN) {
						j_ptr->timeout = 0;
					} else {
						j_ptr->pval = 0;
					}
				}
			}

			if (ident)
			{
				/* Window stuff */
				p_ptr->window |= PW_INVEN;
				/* Combine / Reorder the pack */
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				msg_print("Your backpack seems to be turned upside down.");
			}
			else
			{
				msg_print("You hear a wail of great disappointment.");
			}
			break;
		}

		/* Trap of Stair Movement */
	case TRAP_OF_STAIR_MOVEMENT:
		{
			s16b cx, cy, i, j;
			s16b cnt = 0;
			s16b cnt_seen = 0;
			s16b tmps, tmpx;
			s16b tmpspecial, tmpspecial2;
			u32b tmpf;
			bool seen = FALSE;
			s16b index_x[20], index_y[20];  /* 20 stairs per level is enough? */
			cave_type *cv_ptr;

			if (max_dlv[dungeon_type] == 99)
			{
				/* no sense in relocating that stair! */
				msg_print("You have a feeling that this trap could be dangerous.");
				break;
			}

			for (cx = 0; cx < cur_wid; cx++)
				for (cy = 0; cy < cur_hgt; cy++)
				{
					cv_ptr = &cave[cy][cx];

					if ((cv_ptr->feat != FEAT_LESS) &&
					                (cv_ptr->feat != FEAT_MORE) &&
					                (cv_ptr->feat != FEAT_WAY_LESS) &&
					                (cv_ptr->feat != FEAT_WAY_MORE) &&
					                (cv_ptr->feat != FEAT_SHAFT_UP) &&
					                (cv_ptr->feat != FEAT_SHAFT_DOWN)) {
						continue;
					}

					index_x[cnt] = cx;
					index_y[cnt] = cy;
					cnt++;
				}

			if (cnt == 0)
			{
				if (wizard) msg_print("Executing moving stairs trap on level with no stairs!");
				/* Amy edit: at least give some vague message! traps that give no message when triggered suck! */
				msg_print("You feel faint movements beneath the floor.");
				break;
			}

			for (i = 0; i < cnt; i++)
			{
				seen = FALSE;

				for (j = 0; j < 10; j++) /* try 10 times to relocate */
				{
					cave_type *cv_ptr = &cave[index_y[i]][index_x[i]];
					cave_type *cv_ptr2;

					cx = rand_int(cur_wid);
					cy = rand_int(cur_hgt);

					if ((cx == index_x[i]) || (cy == index_y[i])) continue;

					cv_ptr2 = &cave[cy][cx];

					if (!cave_valid_bold(cy, cx) || cv_ptr2->o_idx != 0) continue;

					/* don't put anything in vaults */
					/* Amy edit: why not? */
					/* if (cv_ptr2->info & CAVE_ICKY) continue; */

					tmpx = cv_ptr2->mimic;
					tmps = cv_ptr2->info;
					tmpf = cv_ptr2->feat;
					tmpspecial = cv_ptr2->special;
					tmpspecial2 = cv_ptr2->special2;
					cave[cy][cx].mimic = cv_ptr->mimic;
					cave[cy][cx].info = cv_ptr->info;
					cave[cy][cx].special = cv_ptr->special;
					cave[cy][cx].special2 = cv_ptr->special2;
					cave_set_feat(cy, cx, cv_ptr->feat);
					cv_ptr->mimic = tmpx;
					cv_ptr->info = tmps;
					cv_ptr->special = tmpspecial;
					cv_ptr->special2 = tmpspecial2;
					cave_set_feat(index_y[i], index_x[i], tmpf);

					/* if we are placing walls in rooms, make them rubble instead */
					if ((cv_ptr->info & CAVE_ROOM) &&
					                (cv_ptr->feat >= FEAT_WALL_EXTRA) &&
					                (cv_ptr->feat <= FEAT_PERM_SOLID))
					{
						cave_set_feat(index_y[i], index_x[i], FEAT_RUBBLE);
					}

					if (player_has_los_bold(cy, cx))
					{
						note_spot(cy, cx);
						lite_spot(cy, cx);
						seen = TRUE;
					}
					else
					{
						cv_ptr2->info &= ~CAVE_MARK;
					}

					if (player_has_los_bold(index_y[i], index_x[i]))
					{
						note_spot(index_y[i], index_x[i]);
						lite_spot(index_y[i], index_x[i]);
						seen = TRUE;
					}
					else
					{
						cv_ptr->info &= ~CAVE_MARK;
					}
					break;
				}

				if (seen) cnt_seen++;
			}

			ident = (cnt_seen > 0);

			if ((ident) && (cnt_seen > 1))
			{
				msg_print("You see some stairs move.");
			}
			else if (ident)
			{
				msg_print("You see a stair move.");
			}
			else
			{
				msg_print("You hear distant scraping noises.");
				ident = TRUE; /* otherwise it'd be far too hard to ID this trap --Amy */
			}

			if (randint(10) == 1) { /* fix endless exploitability --Amy */
				if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

				/* if we're on a floor or on a door, place a new trap */
				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}
				ident = FALSE;
				if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Stair Movement.");
			}

			p_ptr->redraw |= PR_MAP;
			break;
		}

		/* Trap of New Trap */
	case TRAP_OF_NEW:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			/* if we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of New Trap.");
			break;
		}

		/* Trap of Acquirement */
	case TRAP_OF_ACQUIREMENT:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			/* Get a nice thing */
			msg_print("You notice something falling off the trap.");
			acquirement(y, x, 1, TRUE, FALSE);

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Acquirement.");
		}
		break;

		/* Trap of Ragnarok, by Amy */
	case TRAP_OF_RAGNAROK:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			/* create lava, summon many demons and dragons; TODO: start etherwind --Amy */
			ragnarok();

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Ragnarok.");

		break;
		}

	case TRAP_OF_GROUP:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			int attempts_left = 10000;
			int hordex, hordey;

			while (attempts_left--)
			{
				/* Pick a location */
				hordey = rand_int(cur_hgt);
				hordex = rand_int(cur_wid);

				/* Require empty floor grid (was "naked") */
				if (!cave_empty_bold(hordey, hordex)) continue;

				/* Accept far away grids */
				if (distance(hordey, hordex, p_ptr->py, p_ptr->px) > (MAX_SIGHT + 5)) break;
			}
			alloc_horde(hordey, hordex);
			msg_print("You feel like a group of monsters has arrived.");

			/* If we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* Re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You hear a noise, and then its echo.");

			/* Never known */
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Group Trap.");

		break;
		}

		/* Trap of Scatter Items */
	case TRAP_OF_SCATTER_ITEMS:
		{
			s16b i, j;
			bool message = FALSE;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = 0; i < INVEN_PACK; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				if (rand_int(10) < 3) continue;

				for (j = 0; j < 10; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 15 - rand_int(30);
					s16b cy = y + 15 - rand_int(30);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message)
					{
						msg_print("You feel light-footed.");
						message = TRUE;
					}

					if (player_has_los_bold(cy, cx))
					{
						char i_name[80];

						object_desc(i_name, &tmp_obj, TRUE, 3);
						note_spot(cy, cx);
						lite_spot(cy, cx);
						ident = TRUE;
						msg_format("Suddenly %s appear%s!", i_name,
						           (j_ptr->number > 1) ? "" : "s");
					}
					break;
				}
			}
			ident = message;
			break;
		}

	case TRAP_OF_SCATTER_ITEMS_II:
		{
			s16b i, j;
			bool message = FALSE;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = 0; i < INVEN_PACK; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				if (rand_int(10) < 2) continue;

				for (j = 0; j < 50; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 25 - rand_int(50);
					s16b cy = y + 25 - rand_int(50);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message)
					{
						msg_print("You feel light-footed.");
						message = TRUE;
					}

					if (player_has_los_bold(cy, cx))
					{
						char i_name[80];

						object_desc(i_name, &tmp_obj, TRUE, 3);
						note_spot(cy, cx);
						lite_spot(cy, cx);
						ident = TRUE;
						msg_format("Suddenly %s appear%s!", i_name,
						           (j_ptr->number > 1) ? "" : "s");
					}
					break;
				}
			}
			ident = message;
			break;
		}

	case TRAP_OF_SCATTER_ITEMS_III:
		{
			s16b i, j;
			bool message = FALSE;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = 0; i < INVEN_PACK; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				for (j = 0; j < 500; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 40 - rand_int(80);
					s16b cy = y + 40 - rand_int(80);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message)
					{
						msg_print("You feel light-footed.");
						message = TRUE;
					}

					if (player_has_los_bold(cy, cx))
					{
						char i_name[80];

						object_desc(i_name, &tmp_obj, TRUE, 3);
						note_spot(cy, cx);
						lite_spot(cy, cx);
						ident = TRUE;
						msg_format("Suddenly %s appear%s!", i_name,
						           (j_ptr->number > 1) ? "" : "s");
					}
					break;
				}
			}
			ident = message;
			break;
		}

		/* Trap of Scatter Equipment */
	case TRAP_OF_SCATTER_EQUIPMENT:
		{
			s16b i, j;
			bool message = FALSE;

			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				if (rand_int(10) < 3) continue;

				for (j = 0; j < 10; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 15 - rand_int(30);
					s16b cy = y + 15 - rand_int(30);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message)
					{
						msg_print("You feel flat-footed.");
						message = TRUE;
					}

					if (player_has_los_bold(cy, cx))
					{
						char i_name[80];

						object_desc(i_name, &tmp_obj, TRUE, 3);
						note_spot(cy, cx);
						lite_spot(cy, cx);
						ident = TRUE;
						msg_format("Suddenly %s appear%s!", i_name,
						           (j_ptr->number > 1) ? "" : "s");
					}
					break;
				}
			}
			ident = message;
			break;
		}

	case TRAP_OF_SCATTER_EQUIPMENT_II:
		{
			s16b i, j;
			bool message = FALSE;

			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				if (rand_int(10) < 2) continue;

				for (j = 0; j < 50; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 25 - rand_int(50);
					s16b cy = y + 25 - rand_int(50);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message)
					{
						msg_print("You feel flat-footed.");
						message = TRUE;
					}

					if (player_has_los_bold(cy, cx))
					{
						char i_name[80];

						object_desc(i_name, &tmp_obj, TRUE, 3);
						note_spot(cy, cx);
						lite_spot(cy, cx);
						ident = TRUE;
						msg_format("Suddenly %s appear%s!", i_name,
						           (j_ptr->number > 1) ? "" : "s");
					}
					break;
				}
			}
			ident = message;
			break;
		}

	case TRAP_OF_SCATTER_EQUIPMENT_III:
		{
			s16b i, j;
			bool message = FALSE;

			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				for (j = 0; j < 500; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 40 - rand_int(80);
					s16b cy = y + 40 - rand_int(80);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message)
					{
						msg_print("You feel flat-footed.");
						message = TRUE;
					}

					if (player_has_los_bold(cy, cx))
					{
						char i_name[80];

						object_desc(i_name, &tmp_obj, TRUE, 3);
						note_spot(cy, cx);
						lite_spot(cy, cx);
						ident = TRUE;
						msg_format("Suddenly %s appear%s!", i_name,
						           (j_ptr->number > 1) ? "" : "s");
					}
					break;
				}
			}
			ident = message;
			break;
		}

		/* Trap of Steal Equipment */
	case TRAP_OF_STEAL_EQUIPMENT:
		{
			s16b i, j;
			bool message = FALSE;

			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				if (magik(66)) continue;

				for (j = 0; j < 100; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 50 - rand_int(100);
					s16b cy = y + 50 - rand_int(100);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message) {
						msg_print("Your equipment seems to be much lighter than before!");
						message = TRUE;
					}

					break;
				}
			}
			ident = message;
			break;
		}

	case TRAP_OF_STEAL_EQUIPMENT_II:
		{
			s16b i, j;
			bool message = FALSE;

			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				if (magik(30)) continue;

				for (j = 0; j < 100; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 50 - rand_int(100);
					s16b cy = y + 50 - rand_int(100);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message) {
						msg_print("Your equipment seems to be much lighter than before!");
						message = TRUE;
					}

					break;
				}
			}
			ident = message;
			break;
		}

	case TRAP_OF_STEAL_EQUIPMENT_III:
		{
			s16b i, j;
			bool message = FALSE;

			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
			{

				if (!p_ptr->inventory[i].k_idx) continue;

				for (j = 0; j < 100; j++)
				{
					object_type tmp_obj, *j_ptr = &tmp_obj;
					s16b cx = x + 50 - rand_int(100);
					s16b cy = y + 50 - rand_int(100);

					if (!in_bounds(cy, cx)) continue;

					if (!cave_floor_bold(cy, cx)) continue;

					object_copy(j_ptr, &p_ptr->inventory[i]);

					object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);
					if(f3 & TR3_PERMA_CURSE) continue;

					inven_item_increase(i, -999);
					inven_item_optimize(i);

					p_ptr->notice |= (PN_COMBINE | PN_REORDER);

					(void)floor_carry(cy, cx, j_ptr);

					if (!message) {
						msg_print("Your equipment seems to be much lighter than before!");
						message = TRUE;
					}

					break;
				}
			}
			ident = message;
			break;
		}

		/* Trap of Invert Armor, by Amy: equipped stuff with positive AC bonus becomes negative */
	case TRAP_OF_INVERT_ARMOR:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				if (rand_int(10) < 8) continue;

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				/* does it have an AC bonus? */
				if (j_ptr->to_a > 0) {
					j_ptr->to_a *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your armor class fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}

			}
			ident = message;
			if (!message) msg_print("Your body itches strangely!");
			break;
		}

	case TRAP_OF_INVERT_ARMOR_II:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				if (rand_int(10) < 4) continue;

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				/* does it have an AC bonus? */
				if (j_ptr->to_a > 0) {
					j_ptr->to_a *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your armor class fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}

			}
			ident = message;
			if (!message) msg_print("Your body itches strangely!");
			break;
		}

	case TRAP_OF_INVERT_ARMOR_III:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				/* does it have an AC bonus? */
				if (j_ptr->to_a > 0) {
					j_ptr->to_a *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your armor class fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}

			}
			ident = message;
			if (!message) msg_print("Your body itches strangely!");
			break;
		}

		/* Trap of Invert Weapon, by Amy: equipped stuff with positive to-hit/to-dam bonus becomes negative */
	case TRAP_OF_INVERT_WEAPON:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				if (rand_int(10) < 8) continue;

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				/* does it have an AC bonus? */
				if (j_ptr->to_d > 0) {
					j_ptr->to_d *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your weapon effectivity fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}
				if (j_ptr->to_h > 0) {
					j_ptr->to_h *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your weapon effectivity fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}

			}
			ident = message;
			if (!message) msg_print("Your hands itch strangely!");
			break;
		}

	case TRAP_OF_INVERT_WEAPON_II:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				if (rand_int(10) < 4) continue;

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				/* does it have an AC bonus? */
				if (j_ptr->to_d > 0) {
					j_ptr->to_d *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your weapon effectivity fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}
				if (j_ptr->to_h > 0) {
					j_ptr->to_h *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your weapon effectivity fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}

			}
			ident = message;
			if (!message) msg_print("Your hands itch strangely!");
			break;
		}

	case TRAP_OF_INVERT_WEAPON_III:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				/* does it have an AC bonus? */
				if (j_ptr->to_d > 0) {
					j_ptr->to_d *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your weapon effectivity fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}
				if (j_ptr->to_h > 0) {
					j_ptr->to_h *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your weapon effectivity fell sharply!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
				}

			}
			ident = message;
			if (!message) msg_print("Your hands itch strangely!");
			break;
		}

		/* Trap of Trash Equipment, by Amy: equipped stuff with positive pval bonus becomes negative, artifacts are immune */
	case TRAP_OF_TRASH_EQUIPMENT:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				if (rand_int(10) < 8) continue;

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if (j_ptr->name1) continue; /* artifacts are immune */

				/* does it have an AC bonus? */
				if (j_ptr->pval > 0) {
					j_ptr->pval *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your equipment seems much less effective!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

				}

			}
			ident = message;
			if (!message) msg_print("You have a very bad feeling about your equipment...");
			break;
		}

	case TRAP_OF_TRASH_EQUIPMENT_II:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				if (rand_int(10) < 4) continue;

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if (j_ptr->name1) continue; /* artifacts are immune */

				/* does it have an AC bonus? */
				if (j_ptr->pval > 0) {
					j_ptr->pval *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your equipment seems much less effective!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

				}

			}
			ident = message;
			if (!message) msg_print("You have a very bad feeling about your equipment...");
			break;
		}

	case TRAP_OF_TRASH_EQUIPMENT_III:
		{
			s16b j;
			bool message = FALSE;
			object_type *j_ptr;

			for (j = INVEN_WIELD; j < INVEN_TOTAL; j++)
			{

				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];

				if (j_ptr->name1) continue; /* artifacts are immune */

				/* does it have an AC bonus? */
				if (j_ptr->pval > 0) {
					j_ptr->pval *= -1;

					if (object_known_p(j_ptr)) {
						if (!message) {
							msg_print("Your equipment seems much less effective!");
							message = TRUE;
						}
					}

					/* Recalculate bonuses */
					p_ptr->update |= (PU_BONUS);

					/* Recalculate mana */
					p_ptr->update |= (PU_MANA);

					/* Window stuff */
					p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

				}

			}
			ident = message;
			if (!message) msg_print("You have a very bad feeling about your equipment...");
			break;
		}

		/* Trap of Decay */
	case TRAP_OF_DECAY:
		{
			s16b i, j;
			object_type *o_ptr;
			char o_name[80];

			/* code by Amy - this trap used to do nothing at all... */

			for (j = 0; j < 10; j++)
			{
				/* Pick an item from the pack */
				i = rand_int(INVEN_PACK);

				/* Get the item */
				o_ptr = &p_ptr->inventory[i];

				/* Skip non-objects */
				if (!o_ptr->k_idx) continue;

				/* Skip non-food objects */
				if (o_ptr->tval != TV_FOOD) continue;

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
				ident = TRUE;

				/* Done */
				break;
			}
			if (!ident) msg_print("You hear a growling sound.");
		}

		break;

	case TRAP_OF_STACK_REDUCTION:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->number > 1) && rand_int(3) == 1) {
					j_ptr->number /= 2;
					ident = TRUE;
				}
			}
			if (ident)
			{
				msg_print("Something seems missing...");
			}
			else
			{
				msg_print("You hear a distorted scream.");
			}
			break;
		}

	case TRAP_OF_STACK_REDUCTION_II:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->number > 1) && rand_int(2) == 1) {
					j_ptr->number /= 2;
					ident = TRUE;
				}
			}
			if (ident)
			{
				msg_print("Something seems missing...");
			}
			else
			{
				msg_print("You hear a distorted scream.");
			}
			break;
		}

	case TRAP_OF_STACK_REDUCTION_III:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if (j_ptr->number > 1) {
					j_ptr->number /= 2;
					ident = TRUE;
				}
			}
			if (ident)
			{
				msg_print("Something seems missing...");
			}
			else
			{
				msg_print("You hear a distorted scream.");
			}
			break;
		}

		/* Trap of Wasting Wands */
	case TRAP_OF_WASTING_WANDS:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_WAND) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Wand of Nothing */
					object_prep(j_ptr, lookup_kind(TV_WAND, SV_WAND_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if ((j_ptr->tval == TV_STAFF) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Staff of Nothing */
					object_prep(j_ptr, lookup_kind(TV_STAFF, SV_STAFF_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("You have lost trust in your backpack!");
			}
			else
			{
				msg_print("You hear an echoing cry of rage.");
			}
			break;
		}

	case TRAP_OF_WASTING_WANDS_II:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_WAND) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Wand of Nothing */
					object_prep(j_ptr, lookup_kind(TV_WAND, SV_WAND_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if ((j_ptr->tval == TV_STAFF) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Staff of Nothing */
					object_prep(j_ptr, lookup_kind(TV_STAFF, SV_STAFF_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("You have lost trust in your backpack!");
			}
			else
			{
				msg_print("You hear an echoing cry of rage.");
			}
			break;
		}

	case TRAP_OF_WASTING_WANDS_III:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if (j_ptr->tval == TV_WAND)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Wand of Nothing */
					object_prep(j_ptr, lookup_kind(TV_WAND, SV_WAND_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if (j_ptr->tval == TV_STAFF)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Staff of Nothing */
					object_prep(j_ptr, lookup_kind(TV_STAFF, SV_STAFF_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("You have lost trust in your backpack!");
			}
			else
			{
				msg_print("You hear an echoing cry of rage.");
			}
			break;
		}

	case TRAP_OF_WASTING_SCROLLS:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_SCROLL) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Scroll of Nothing */
					object_prep(j_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Your booklets seem to shuffle!");
			}
			else
			{
				msg_print("You hear a grumbling voice.");
			}
			break;
		}

	case TRAP_OF_WASTING_SCROLLS_II:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_SCROLL) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Scroll of Nothing */
					object_prep(j_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Your booklets seem to shuffle!");
			}
			else
			{
				msg_print("You hear a grumbling voice.");
			}
			break;
		}

	case TRAP_OF_WASTING_SCROLLS_III:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if (j_ptr->tval == TV_SCROLL)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Scroll of Nothing */
					object_prep(j_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Your booklets seem to shuffle!");
			}
			else
			{
				msg_print("You hear a grumbling voice.");
			}
			break;
		}

	case TRAP_OF_WASTING_POTIONS:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_POTION) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Potion of Water */
					object_prep(j_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if ((j_ptr->tval == TV_POTION2) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Potion of Water */
					object_prep(j_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Oh no, your potions spilled everywhere!");
			}
			else
			{
				msg_print("You hear a popping noise.");
			}
			break;
		}

	case TRAP_OF_WASTING_POTIONS_II:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_POTION) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Potion of Water */
					object_prep(j_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if ((j_ptr->tval == TV_POTION2) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Potion of Water */
					object_prep(j_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Oh no, your potions spilled everywhere!");
			}
			else
			{
				msg_print("You hear a popping noise.");
			}
			break;
		}

	case TRAP_OF_WASTING_POTIONS_III:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if (j_ptr->tval == TV_POTION)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Potion of Water */
					object_prep(j_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if (j_ptr->tval == TV_POTION2)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Potion of Water */
					object_prep(j_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Oh no, your potions spilled everywhere!");
			}
			else
			{
				msg_print("You hear a popping noise.");
			}
			break;
		}

	case TRAP_OF_WASTING_RODS:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_ROD) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Rod Tip of Nothing */
					object_prep(j_ptr, lookup_kind(TV_ROD, SV_ROD_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if ((j_ptr->tval == TV_ROD_MAIN) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Dirt Rod (useless) */
					object_prep(j_ptr, lookup_kind(TV_ROD_MAIN, SV_ROD_DIRT));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("You have lost trust in your knapsack!");
			}
			else
			{
				msg_print("You hear a high-pitched scream.");
			}
			break;
		}

	case TRAP_OF_WASTING_RODS_II:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_ROD) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Rod Tip of Nothing */
					object_prep(j_ptr, lookup_kind(TV_ROD, SV_ROD_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if ((j_ptr->tval == TV_ROD_MAIN) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Dirt Rod (useless) */
					object_prep(j_ptr, lookup_kind(TV_ROD_MAIN, SV_ROD_DIRT));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("You have lost trust in your knapsack!");
			}
			else
			{
				msg_print("You hear a high-pitched scream.");
			}
			break;
		}

	case TRAP_OF_WASTING_RODS_III:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if (j_ptr->tval == TV_ROD)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Rod Tip of Nothing */
					object_prep(j_ptr, lookup_kind(TV_ROD, SV_ROD_NOTHING));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if (j_ptr->tval == TV_ROD_MAIN)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Dirt Rod (useless) */
					object_prep(j_ptr, lookup_kind(TV_ROD_MAIN, SV_ROD_DIRT));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("You have lost trust in your knapsack!");
			}
			else
			{
				msg_print("You hear a high-pitched scream.");
			}
			break;
		}

	case TRAP_OF_WASTING_FOOD:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_FOOD) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Mushroom of Poison */
					object_prep(j_ptr, lookup_kind(TV_FOOD, SV_FOOD_POISON));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if ((j_ptr->tval == TV_CORPSE) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Staff of Nothing */
					object_prep(j_ptr, lookup_kind(TV_FOOD, SV_FOOD_POISON));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Something in your backpack smells like decay!");
			}
			else
			{
				msg_print("You sense a foul smell.");
			}
			break;
		}

	case TRAP_OF_WASTING_FOOD_II:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_FOOD) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Mushroom of Poison */
					object_prep(j_ptr, lookup_kind(TV_FOOD, SV_FOOD_POISON));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if ((j_ptr->tval == TV_CORPSE) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Staff of Nothing */
					object_prep(j_ptr, lookup_kind(TV_FOOD, SV_FOOD_POISON));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Something in your backpack smells like decay!");
			}
			else
			{
				msg_print("You sense a foul smell.");
			}
			break;
		}

	case TRAP_OF_WASTING_FOOD_III:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if (j_ptr->tval == TV_FOOD)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Mushroom of Poison */
					object_prep(j_ptr, lookup_kind(TV_FOOD, SV_FOOD_POISON));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
				else if (j_ptr->tval == TV_CORPSE)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create a Staff of Nothing */
					object_prep(j_ptr, lookup_kind(TV_FOOD, SV_FOOD_POISON));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Something in your backpack smells like decay!");
			}
			else
			{
				msg_print("You sense a foul smell.");
			}
			break;
		}

	case TRAP_OF_WASTING_BOOKS:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_BOOK) && (rand_int(5) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create an Adventurer's Guide */
					object_prep(j_ptr, lookup_kind(TV_PARCHMENT, 20));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Your backpack suddenly feels lighter!");
			}
			else
			{
				msg_print("You have a terrible sense of loss.");
			}
			break;
		}

	case TRAP_OF_WASTING_BOOKS_II:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if ((j_ptr->tval == TV_BOOK) && (rand_int(2) == 1))
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create an Adventurer's Guide */
					object_prep(j_ptr, lookup_kind(TV_PARCHMENT, 20));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Your backpack suddenly feels lighter!");
			}
			else
			{
				msg_print("You have a terrible sense of loss.");
			}
			break;
		}

	case TRAP_OF_WASTING_BOOKS_III:
		{
			s16b i;
			object_type *j_ptr;

			for (i = 0; i < INVEN_PACK; i++)
			{
				if (!p_ptr->inventory[i].k_idx) continue;

				j_ptr = &p_ptr->inventory[i];

				if (j_ptr->tval == TV_BOOK)
				{
					if (object_known_p(j_ptr)) ident = TRUE;

					/* Create an Adventurer's Guide */
					object_prep(j_ptr, lookup_kind(TV_PARCHMENT, 20));
					hack_apply_magic_power = -99;
					apply_magic(j_ptr, 0, FALSE, FALSE, FALSE);
					j_ptr->ident &= ~IDENT_KNOWN;
					p_ptr->notice |= (PN_COMBINE | PN_REORDER);
				}
			}
			if (ident)
			{
				msg_print("Your backpack suddenly feels lighter!");
			}
			else
			{
				msg_print("You have a terrible sense of loss.");
			}
			break;
		}

		/* Trap of Filling */
	case TRAP_OF_FILLING:
		{
			if (!p_ptr->nastytrap3) t_info[trap].ident = TRUE;

			s16b nx, ny;

			for (nx = x - 8; nx <= x + 8; nx++)
				for (ny = y - 8; ny <= y + 8; ny++)
				{
					if (!in_bounds (ny, nx)) continue;

					if (rand_int(distance(ny, nx, y, x)) > 3)
					{
						place_trap(ny, nx);
					}
				}

			/* if we're on a floor or on a door, place a new trap */
			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}

			msg_print("The floor vibrates in a strange way.");
			ident = FALSE;
			if (!p_ptr->nastytrap3) msg_print("You identified that trap as Trap of Filling.");
			break;
		}

	case TRAP_OF_ANIMATION:
		{
			ident = TRUE;
			msg_print("es come to life!");

			s16b nx, ny;

			for (nx = x - 10; nx <= x + 10; nx++)
				for (ny = y - 10; ny <= y + 10; ny++)
				{
					if (!in_bounds (ny, nx)) continue;

					if (rand_int(distance(ny, nx, y, x)) > 5)
					{
						summon_specific(ny, nx, max_dlv_real[dungeon_type], 0);
					}
				}


			/* thwart endless farming, since I just know some player will be lame enough to do so --Amy */
			if (randint(3) == 1) {
				t_info[trap].ident = ident;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}

				if (ident) msg_print("You identified that trap as Animation Trap.");
				ident = FALSE;

			}
			break;
		}

	case TRAP_OF_DRAIN_SPEED:
		{
			object_type *j_ptr;
			s16b j, chance = 75;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (j = 0; j < INVEN_TOTAL; j++)
			{
				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* is it a non-artifact speed item? */
				if ((!j_ptr->name1) && (f1 & TR1_SPEED))
				{
					if (randint(100) < chance)
					{
						j_ptr->pval = j_ptr->pval / 2;
						if (j_ptr->pval == 0)
						{
							j_ptr->pval--;
						}
						chance /= 2;
						ident = TRUE;
					}
					inven_item_optimize(j);
				}
			}
			if (!ident)
			{
				msg_print("You feel some things in your pack vibrating.");
			}
			else
			{
				combine_pack();
				reorder_pack();
				msg_print("You suddenly feel you have time for self-reflection.");

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS);

				/* Recalculate mana */
				p_ptr->update |= (PU_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			}
			break;
		}

	case TRAP_OF_DRAIN_SPEED_II:
		{
			object_type *j_ptr;
			s16b j;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (j = 0; j < INVEN_TOTAL; j++)
			{
				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* is it a speed item? (can also affect artifacts :-P --Amy) */
				if (f1 & TR1_SPEED)
				{
					j_ptr->pval = j_ptr->pval / 2;
					if (j_ptr->pval == 0)
					{
						j_ptr->pval--;
					}
					ident = TRUE;
					inven_item_optimize(j);
				}
			}
			if (!ident)
			{
				msg_print("You feel some things in your pack vibrating.");
			}
			else
			{
				combine_pack();
				reorder_pack();
				msg_print("You suddenly feel you have time for self-reflection.");

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS);

				/* Recalculate mana */
				p_ptr->update |= (PU_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			}
			break;
		}

		/* trap of unworth by Amy: slaps a 100% discount on some of your items so you can no longer sell them */
	case TRAP_OF_UNWORTH:
		{
			object_type *j_ptr;
			s16b j, chance = 20;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (j = 0; j < INVEN_TOTAL; j++)
			{
				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* does it have less than 100% discount? */
				if (j_ptr->discount < 100)
				{
					if (randint(100) < chance)
					{
						j_ptr->discount = 100;
						ident = TRUE;
					}
					inven_item_optimize(j);
				}
			}
			if (!ident)
			{
				msg_print("You ponder the value of your possessions.");
			}
			else
			{
				combine_pack();
				reorder_pack();
				msg_print("You suddenly feel that your equipment isn't worth as much as it used to be...");

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS);

				/* Recalculate mana */
				p_ptr->update |= (PU_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			}
			break;
		}

	case TRAP_OF_UNWORTH_II:
		{
			object_type *j_ptr;
			s16b j, chance = 50;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (j = 0; j < INVEN_TOTAL; j++)
			{
				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* does it have less than 100% discount? */
				if (j_ptr->discount < 100)
				{
					if (randint(100) < chance)
					{
						j_ptr->discount = 100;
						ident = TRUE;
					}
					inven_item_optimize(j);
				}
			}
			if (!ident)
			{
				msg_print("You ponder the value of your possessions.");
			}
			else
			{
				combine_pack();
				reorder_pack();
				msg_print("You suddenly feel that your equipment isn't worth as much as it used to be...");

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS);

				/* Recalculate mana */
				p_ptr->update |= (PU_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			}
			break;
		}

	case TRAP_OF_UNWORTH_III:
		{
			object_type *j_ptr;
			s16b j;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (j = 0; j < INVEN_TOTAL; j++)
			{
				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* does it have less than 100% discount? */
				if (j_ptr->discount < 100)
				{
					j_ptr->discount = 100;
					ident = TRUE;
					inven_item_optimize(j);
				}
			}
			if (!ident)
			{
				msg_print("You ponder the value of your possessions.");
			}
			else
			{
				combine_pack();
				reorder_pack();
				msg_print("You suddenly feel that your equipment isn't worth as much as it used to be...");

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS);

				/* Recalculate mana */
				p_ptr->update |= (PU_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			}
			break;
		}

		/* trap of unproofing by Amy: removes "this item cannot be harmed bla-bla" flags from some items */
	case TRAP_OF_UNPROOFING:
		{
			object_type *j_ptr;
			s16b j, chance = 20;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (j = 0; j < INVEN_TOTAL; j++)
			{
				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* does it have some elemental ignore flag? */
				if (randint(100) < chance)
				{
					if (f3 & TR3_IGNORE_ACID)
					{
						ident = TRUE;
						j_ptr->art_flags3 &= ~TR3_IGNORE_ACID;
						inven_item_optimize(j);
					}
					if (f3 & TR3_IGNORE_FIRE)
					{
						ident = TRUE;
						j_ptr->art_flags3 &= ~TR3_IGNORE_FIRE;
						inven_item_optimize(j);
					}
					if (f3 & TR3_IGNORE_COLD)
					{
						ident = TRUE;
						j_ptr->art_flags3 &= ~TR3_IGNORE_COLD;
						inven_item_optimize(j);
					}
					if (f3 & TR3_IGNORE_ELEC)
					{
						ident = TRUE;
						j_ptr->art_flags3 &= ~TR3_IGNORE_ELEC;
						inven_item_optimize(j);
					}
				}
			}
			if (!ident)
			{
				msg_print("You feel afraid of the elements.");
			}
			else
			{
				combine_pack();
				reorder_pack();
				msg_print("Your equipment seems less protected!");

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS);

				/* Recalculate mana */
				p_ptr->update |= (PU_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			}
			break;
		}

	case TRAP_OF_UNPROOFING_II:
		{
			object_type *j_ptr;
			s16b j, chance = 50;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (j = 0; j < INVEN_TOTAL; j++)
			{
				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* does it have some elemental ignore flag? */
				if (randint(100) < chance)
				{
					if (f3 & TR3_IGNORE_ACID)
					{
						ident = TRUE;
						j_ptr->art_flags3 &= ~TR3_IGNORE_ACID;
						inven_item_optimize(j);
					}
					if (f3 & TR3_IGNORE_FIRE)
					{
						ident = TRUE;
						j_ptr->art_flags3 &= ~TR3_IGNORE_FIRE;
						inven_item_optimize(j);
					}
					if (f3 & TR3_IGNORE_COLD)
					{
						ident = TRUE;
						j_ptr->art_flags3 &= ~TR3_IGNORE_COLD;
						inven_item_optimize(j);
					}
					if (f3 & TR3_IGNORE_ELEC)
					{
						ident = TRUE;
						j_ptr->art_flags3 &= ~TR3_IGNORE_ELEC;
						inven_item_optimize(j);
					}
				}
			}
			if (!ident)
			{
				msg_print("You feel afraid of the elements.");
			}
			else
			{
				combine_pack();
				reorder_pack();
				msg_print("Your equipment seems less protected!");

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS);

				/* Recalculate mana */
				p_ptr->update |= (PU_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			}
			break;
		}

	case TRAP_OF_UNPROOFING_III:
		{
			object_type *j_ptr;
			s16b j;
			u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

			for (j = 0; j < INVEN_TOTAL; j++)
			{
				/* don't bother the overflow slot */
				if (j == INVEN_PACK) continue;

				if (!p_ptr->inventory[j].k_idx) continue;

				j_ptr = &p_ptr->inventory[j];
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				/* does it have some elemental ignore flag? */
				if (f3 & TR3_IGNORE_ACID)
				{
					ident = TRUE;
					j_ptr->art_flags3 &= ~TR3_IGNORE_ACID;
					inven_item_optimize(j);
				}
				if (f3 & TR3_IGNORE_FIRE)
				{
					ident = TRUE;
					j_ptr->art_flags3 &= ~TR3_IGNORE_FIRE;
					inven_item_optimize(j);
				}
				if (f3 & TR3_IGNORE_COLD)
				{
					ident = TRUE;
					j_ptr->art_flags3 &= ~TR3_IGNORE_COLD;
					inven_item_optimize(j);
				}
				if (f3 & TR3_IGNORE_ELEC)
				{
					ident = TRUE;
					j_ptr->art_flags3 &= ~TR3_IGNORE_ELEC;
					inven_item_optimize(j);
				}
			}
			if (!ident)
			{
				msg_print("You feel afraid of the elements.");
			}
			else
			{
				combine_pack();
				reorder_pack();
				msg_print("Your equipment seems less protected!");

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS);

				/* Recalculate mana */
				p_ptr->update |= (PU_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);
			}
			break;
		}

	case TRAP_NASTY1:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap1 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY2:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap2 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY3:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) {
				ident = TRUE;
				t_info[trap].ident = TRUE; /* id the trap before triggering it, since you can't id it later */

				msg_print("You identified that trap as Snaretype Trap.");
			}

			p_ptr->nastytrap3 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY4:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap4 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY5:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap5 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY6:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap6 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY7:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap7 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY8:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap8 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY9:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap9 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY10:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap10 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY11:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap11 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY12:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap12 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY13:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap13 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY14:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap14 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY15:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap15 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY16:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap16 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY17:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap17 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY18:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap18 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY19:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap19 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY20:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap20 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY21:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap21 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY22:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap22 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY23:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap23 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY24:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap24 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY25:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap25 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY26:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap26 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY27:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap27 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY28:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap28 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY29:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap29 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY30:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap30 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY31:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap31 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY32:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap32 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY33:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap33 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY34:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap34 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY35:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap35 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY36:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap36 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY37:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap37 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY38:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap38 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY39:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap39 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY40:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap40 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY41:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap41 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY42:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap42 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY43:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap43 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY44:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap44 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY45:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap45 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY46:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap46 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY47:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap47 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY48:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap48 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY49:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap49 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY50:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap50 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY51:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap51 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY52:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap52 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY53:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap53 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY54:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap54 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY55:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap55 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY56:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap56 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY57:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap57 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY58:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap58 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY59:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap59 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY60:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap60 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY61:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap61 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY62:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap62 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY63:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap63 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY64:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap64 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY65:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap65 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY66:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap66 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY67:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap67 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY68:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap68 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY69:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap69 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY70:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap70 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY71:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap71 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY72:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap72 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY73:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap73 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY74:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap74 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY75:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap75 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY76:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap76 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY77:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap77 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY78:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap78 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY79:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap79 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY80:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap80 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY81:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap81 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY82:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap82 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY83:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap83 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY84:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap84 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY85:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap85 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY86:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap86 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY87:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap87 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY88:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap88 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY89:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap89 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY90:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap90 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY91:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap91 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY92:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap92 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY93:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap93 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY94:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap94 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY95:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap95 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY96:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap96 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY97:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap97 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY98:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap98 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY99:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap99 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY100:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap100 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY101:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap101 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY102:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap102 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY103:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap103 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY104:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap104 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY105:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap105 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY106:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap106 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY107:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap107 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY108:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap108 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}
	case TRAP_NASTY109:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap109 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY110:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap110 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY111:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap111 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY112:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap112 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY113:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap113 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY114:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap114 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY115:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap115 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY116:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap116 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY117:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap117 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY118:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap118 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY119:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap119 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY120:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap120 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY121:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap121 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY122:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap122 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY123:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap123 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY124:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap124 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY125:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap125 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY126:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap126 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY127:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap127 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY128:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap128 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY129:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap129 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY130:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap130 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY131:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap131 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY132:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap132 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY133:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap133 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY134:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap134 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY135:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap135 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY136:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap136 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY137:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap137 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY138:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap138 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY139:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap139 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY140:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap140 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY141:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap141 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY142:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap142 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY143:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap143 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY144:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap144 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY145:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap145 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY146:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap146 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY147:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap147 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY148:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap148 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY149:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap149 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY150:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap150 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY151:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap151 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY152:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap152 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY153:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap153 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY154:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap154 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY155:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap155 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY156:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap156 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY157:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap157 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY158:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap158 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY159:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap159 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY160:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap160 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY161:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap161 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY162:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap162 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY163:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap163 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY164:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap164 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY165:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap165 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY166:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap166 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY167:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap167 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY168:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap168 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY169:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap169 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY170:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap170 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY171:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap171 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY172:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap172 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_NASTY173:

		{
			ident = FALSE;
			if (c_ptr->info & (CAVE_TRDT)) ident = TRUE;

			p_ptr->nastytrap173 = TRUE;
			calc_bonuses(FALSE);

			break;			
		}

	case TRAP_OF_SHIT_I:
		{
			object_type *j_ptr;
			msg_print("You stepped into a heap of shit!");
			take_hit(damroll(2, 4), "a heap of shit");
			j_ptr = &p_ptr->inventory[INVEN_FEET];
			if (j_ptr->tval != TV_BOOTS) {
				msg_print("You slip on the shit with your bare feet.");
				take_hit(damroll(2, 4), "a heap of shit");
			} else {
				int bootdamagechance = 30;
				u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				if ((f3 & (TR3_IGNORE_ACID)) && !p_ptr->nastytrap47) bootdamagechance = 3;

				if (magik(bootdamagechance)) {
					if (j_ptr->to_a > -20) {
						j_ptr->to_a--;
						msg_print("Your boots are damaged!");
					}
				}
			}

			ident = TRUE;
			break;
		}
	case TRAP_OF_SHIT_II:
	case TRAP_OF_SHIT_IV:
		{
			object_type *j_ptr;
			msg_print("You fully stepped into dog shit!");
			take_hit(damroll(3, 8), "a heap of dog shit");
			j_ptr = &p_ptr->inventory[INVEN_FEET];
			if (j_ptr->tval != TV_BOOTS) {
				msg_print("You slip on the shit with your bare feet.");
				take_hit(damroll(3, 8), "a heap of dog shit");
				(void)set_slow(p_ptr->slow + rand_int(10) + 4);
			} else {
				int bootdamagechance = 70;
				u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				if ((f3 & (TR3_IGNORE_ACID)) && !p_ptr->nastytrap47) bootdamagechance = 10;

				if (magik(bootdamagechance)) {
					if (j_ptr->to_a > -30) {
						j_ptr->to_a--;
						msg_print("Your boots are damaged!");
					}
				}
			}

			ident = TRUE;
			break;
		}
	case TRAP_OF_SHIT_III:
	case TRAP_OF_SHIT_V:
		{
			object_type *j_ptr;
			msg_print("You have stepped into icky cow dung!");
			take_hit(damroll(5, 9), "a heap of cow dung");
			j_ptr = &p_ptr->inventory[INVEN_FEET];
			if (j_ptr->tval != TV_BOOTS) {
				msg_print("You slip on the shit with your bare feet. Oh damn, it got all over your clothes!");
				take_hit(damroll(5, 9), "a heap of cow dung");
				project( -2, 0, p_ptr->py, p_ptr->px, randint(10), GF_ACID, PROJECT_KILL | PROJECT_JUMP | PROJECT_CANTREFLECT);
				(void)set_slow(p_ptr->slow + rand_int(20) + 25);
			} else {
				int bootdamagechance = 100;
				u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;
				object_flags(j_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

				if ((f3 & (TR3_IGNORE_ACID)) && !p_ptr->nastytrap47) bootdamagechance = 40;

				if (magik(bootdamagechance)) {
					if (j_ptr->to_a > -40) {
						j_ptr->to_a -= randint(3);
						msg_print("Your boots are damaged!");
					}
				}
			}

			ident = TRUE;
			break;
		}

	case TRAP_OF_SHOES:
	case TRAP_OF_SHOES_II:
	case TRAP_OF_SHOES_III:
	case TRAP_OF_SHOES_IV:
	case TRAP_OF_SHOES_V:
		{
			int shoedamage;
			int shoeextra = 0;

			if (dun_level > 1) {
				shoeextra = dun_level / 2;
				if (shoeextra > 75) shoeextra = 75;
			}

			ident = TRUE;

			while (fartingnumber < 1) fartingnumber = 8;
			while (fartingnumber > 8) fartingnumber -= 8;

			switch (fartingnumber) {

				default:
				case 1:
					msg_print("Eveline slams her cute wedge sandals into your shins!");
					shoedamage = damroll(2, 4);
					shoedamage += damroll(shoeextra, 4);

					take_hit(shoedamage, "Eveline's wedge sandals");
					break;
				case 2:
					msg_print("Anastasia slams her soft dancing shoes into your shins!");
					shoedamage = damroll(1, 2);
					shoedamage += damroll(shoeextra, 1);
					(void)set_stun(p_ptr->stun + randint(10));
					take_hit(shoedamage, "Anastasia's dancing shoes");
					break;
				case 3:
					msg_print("Solvejg slides her feminine mocassins over your hands, drawing blood!");
					shoedamage = damroll(1, 4);
					shoedamage += damroll(shoeextra, 2);
					(void)set_cut(p_ptr->cut + shoedamage);
					take_hit(shoedamage, "Solvejg's feminine mocassins");
					break;
				case 4:
					msg_print("Elif slides her soft sneakers over your hands, drawing blood!");
					shoedamage = damroll(1, 1);
					shoedamage += damroll(shoeextra, 1);
					(void)set_cut(p_ptr->cut + shoedamage);
					take_hit(shoedamage, "Elif's soft sneakers");
					break;
				case 5:
					msg_print("Juen slams her leather peep-toes into your shins!");
					shoedamage = damroll(4, 6);
					shoedamage += damroll(shoeextra, 5);
					if (!p_ptr->paralyzed) {
						if (p_ptr->free_act && (rand_int(p_ptr->nastytrap57 ? 20 : 100) > 0) ) {
							msg_print("You struggle to stay on your feet.");
						} else if (p_ptr->resist_nerve && magik(50)) {
							msg_print("You barely manage to stay on your feet.");
						} else {
							msg_print("You helplessly drop to the floor in pain.");
							set_paralyzed(p_ptr->paralyzed + randint(4) );
						}
					}
					take_hit(shoedamage, "Juen's leather peep-toes");
					break;
				case 6:
					msg_print("Wendy scratches up and down your legs with her sexy leather pumps!");
					shoedamage = damroll(3, 8);
					shoedamage += damroll(shoeextra, 6);
					do_dec_stat(A_CON, STAT_DEC_NORMAL);
					take_hit(shoedamage, "Wendy's leather pumps");
					break;
				case 7:
					msg_print("Meltem scratches over your legs with her block-heeled lady boot!");
					shoedamage = damroll(3, 4);
					shoedamage += damroll(shoeextra, 4);

					take_hit(shoedamage, "Meltem's block-heeled boots");
					break;
				case 8:
					msg_print("Claudia slams her hard wooden sandals into your shins!");
					shoedamage = damroll(2, 10);
					shoedamage += damroll(shoeextra, 8);

					take_hit(shoedamage, "Claudia's wooden sandals");
					break;
			}


			break;

		}

		/*
		 * single missile traps
		 */
	case TRAP_OF_ARROW_O:
		ident = player_handle_missile_trap(1, TV_ARROW, 6, 3, 4, 0, "Sheaf Arrow Trap");
		break;
	case TRAP_OF_ARROW_I:
		ident = player_handle_missile_trap(1, TV_ARROW, SV_AMMO_NORMAL, 4, 4, 0, "Arrow Trap");
		break;
	case TRAP_OF_ARROW_II:
		ident = player_handle_missile_trap(1, TV_BOLT, SV_AMMO_NORMAL, 5, 4, 0, "Bolt Trap");
		break;
	case TRAP_OF_ARROW_III:
		ident = player_handle_missile_trap(1, TV_ARROW, SV_AMMO_HEAVY, 6, 4, 0, "Seeker Arrow Trap");
		break;
	case TRAP_OF_ARROW_IV:
		ident = player_handle_missile_trap(1, TV_BOLT, SV_AMMO_HEAVY, 8, 5, 0, "Seeker Bolt Trap");
		break;
	case TRAP_OF_POISON_ARROW_O:
		ident = player_handle_missile_trap(1, TV_ARROW, 6, 3, 4, randint(10), "Poison Sheaf Arrow Trap");
		break;
	case TRAP_OF_POISON_ARROW_I:
		ident = player_handle_missile_trap(1, TV_ARROW, SV_AMMO_NORMAL, 4, 4, randint(20), "Poison Arrow Trap");
		break;
	case TRAP_OF_POISON_ARROW_II:
		ident = player_handle_missile_trap(1, TV_BOLT, SV_AMMO_NORMAL, 5, 4, randint(30), "Poison Bolt Trap");
		break;
	case TRAP_OF_POISON_ARROW_III:
		ident = player_handle_missile_trap(1, TV_ARROW, SV_AMMO_HEAVY, 6, 4, randint(50), "Poison Seeker Arrow Trap");
		break;
	case TRAP_OF_POISON_ARROW_IV:
		ident = player_handle_missile_trap(1, TV_BOLT, SV_AMMO_HEAVY, 8, 5, randint(70), "Poison Seeker Bolt Trap");
		break;
	case TRAP_OF_SPIKE_I:
		ident = player_handle_missile_trap(1, TV_SPIKE, 0, 1, 1, 0, "Iron Spike Trap");
		break;
	case TRAP_OF_SPIKE_II:
		ident = player_handle_missile_trap(1, TV_SPIKE, 1, 2, 2, 0, "Steel Spike Trap");
		break;
	case TRAP_OF_SPIKE_III:
		ident = player_handle_missile_trap(1, TV_SPIKE, 2, 3, 4, 0, "Lead Spike Trap");
		break;
	case TRAP_OF_SPIKE_IV:
		ident = player_handle_missile_trap(1, TV_SPIKE, 3, 5, 5, 0, "Mithril Spike Trap");
		break;
	case TRAP_OF_SPIKE_V:
		ident = player_handle_missile_trap(1, TV_SPIKE, 4, 7, 8, 0, "Adamantium Spike Trap");
		break;
	case TRAP_OF_POISON_SPIKE_I:
		ident = player_handle_missile_trap(1, TV_SPIKE, 0, 1, 1, randint(5), "Poison Iron Spike Trap");
		break;
	case TRAP_OF_POISON_SPIKE_II:
		ident = player_handle_missile_trap(1, TV_SPIKE, 1, 2, 2, randint(10), "Poison Steel Spike Trap");
		break;
	case TRAP_OF_POISON_SPIKE_III:
		ident = player_handle_missile_trap(1, TV_SPIKE, 2, 3, 4, randint(15), "Poison Lead Spike Trap");
		break;
	case TRAP_OF_POISON_SPIKE_IV:
		ident = player_handle_missile_trap(1, TV_SPIKE, 3, 5, 5, randint(22), "Poison Mithril Spike Trap");
		break;
	case TRAP_OF_POISON_SPIKE_V:
		ident = player_handle_missile_trap(1, TV_SPIKE, 4, 7, 8, randint(30), "Poison Adamantium Spike Trap");
		break;
	case TRAP_OF_SHOT_I:
		ident = player_handle_missile_trap(1, TV_SHOT, SV_AMMO_LIGHT, 3, 3, 0, "Pebble Trap");
		break;
	case TRAP_OF_SHOT_II:
		ident = player_handle_missile_trap(1, TV_SHOT, SV_AMMO_NORMAL, 4, 4, 0, "Shot Trap");
		break;
	case TRAP_OF_SHOT_III:
		ident = player_handle_missile_trap(1, TV_SHOT, SV_AMMO_HEAVY, 5, 5, 0, "Mithril Shot Trap");
		break;
	case TRAP_OF_DAGGER_I:
		ident = player_handle_missile_trap(1, TV_SWORD, SV_BROKEN_DAGGER, 2, 4, 0, "Broken Dagger Trap");
		break;
	case TRAP_OF_DAGGER_II:
		ident = player_handle_missile_trap(1, TV_SWORD, SV_DAGGER, 3, 4, 0, "Dagger Trap");
		break;
	case TRAP_OF_POISON_DAGGER_I:
		ident = player_handle_missile_trap(1, TV_SWORD, SV_BROKEN_DAGGER, 2, 4, randint(20), "Poison Broken Dagger Trap");
		break;
	case TRAP_OF_POISON_DAGGER_II:
		ident = player_handle_missile_trap(1, TV_SWORD, SV_DAGGER, 3, 4, randint(30), "Poison Dagger Trap");
		break;

	case TRAP_OF_PISTOL:
		ident = player_handle_missile_trap(1, TV_AMMO_PISTOL, SV_AMMO_NORMAL, 5, 10, 0, "Pistol Trap");
		break;
	case TRAP_OF_RIFLE:
		ident = player_handle_missile_trap(1, TV_AMMO_RIFLE, SV_AMMO_NORMAL, 6, 11, 0, "Rifle Trap");
		break;
	case TRAP_OF_SHOTGUN:
		ident = player_handle_missile_trap(1, TV_AMMO_SHOTGUN, SV_AMMO_NORMAL, 3, 25, 0, "Shotgun Trap");
		break;

		/*
		 * multiple missile traps
		 * numbers range from 2 (level 0 to 14) to 10 (level 120 and up)
		 * Amy edit: reduced amount of missiles so players aren't hopelessly instakilled
		 */
	case TRAP_OF_ARROWS_O:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_ARROW, 6, 3, 4, 0, "Sheaf Arrows Trap");
		break;
	case TRAP_OF_ARROWS_I:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_ARROW, SV_AMMO_NORMAL, 4, 4, 0, "Arrows Trap");
		break;
	case TRAP_OF_ARROWS_II:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_BOLT, SV_AMMO_NORMAL, 5, 4, 0, "Bolts Trap");
		break;
	case TRAP_OF_ARROWS_III:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_ARROW, SV_AMMO_HEAVY, 6, 4, 0, "Seeker Arrows Trap");
		break;
	case TRAP_OF_ARROWS_IV:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_BOLT, SV_AMMO_HEAVY, 8, 5, 0, "Seeker Bolts Trap");
		break;
	case TRAP_OF_POISON_ARROWS_O:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_ARROW, 6, 3, 4, randint(10), "Poison Sheaf Arrows Trap");
		break;
	case TRAP_OF_POISON_ARROWS_I:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_ARROW, SV_AMMO_NORMAL, 4, 4, randint(20), "Poison Arrows Trap");
		break;
	case TRAP_OF_POISON_ARROWS_II:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_BOLT, SV_AMMO_NORMAL, 5, 4, randint(30), "Poison Bolts Trap");
		break;
	case TRAP_OF_POISON_ARROWS_III:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_ARROW, SV_AMMO_HEAVY, 6, 4, randint(50), "Poison Seeker Arrows Trap");
		break;
	case TRAP_OF_POISON_ARROWS_IV:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_BOLT, SV_AMMO_HEAVY, 8, 5, randint(70), "Poison Seeker Bolts Trap");
		break;
	case TRAP_OF_SPIKES_I:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 0, 1, 1, 0, "Iron Spikes Trap");
		break;
	case TRAP_OF_SPIKES_II:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 1, 2, 2, 0, "Steel Spikes Trap");
		break;
	case TRAP_OF_SPIKES_III:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 2, 3, 4, 0, "Lead Spikes Trap");
		break;
	case TRAP_OF_SPIKES_IV:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 3, 5, 5, 0, "Mithril Spikes Trap");
		break;
	case TRAP_OF_SPIKES_V:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 4, 7, 8, 0, "Adamantium Spikes Trap");
		break;
	case TRAP_OF_POISON_SPIKES_I:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 0, 1, 1, randint(5), "Poison Iron Spikes Trap");
		break;
	case TRAP_OF_POISON_SPIKES_II:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 1, 2, 2, randint(10), "Poison Steel Spikes Trap");
		break;
	case TRAP_OF_POISON_SPIKES_III:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 2, 3, 4, randint(15), "Poison Lead Spikes Trap");
		break;
	case TRAP_OF_POISON_SPIKES_IV:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 3, 5, 5, randint(22), "Poison Mithril Spikes Trap");
		break;
	case TRAP_OF_POISON_SPIKES_V:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SPIKE, 4, 7, 8, randint(30), "Poison Adamantium Spikes Trap");
		break;
	case TRAP_OF_SHOTS_I:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SHOT, SV_AMMO_LIGHT, 3, 3, 0, "Pebbles Trap");
		break;
	case TRAP_OF_SHOTS_II:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SHOT, SV_AMMO_NORMAL, 4, 4, 0, "Shots Trap");
		break;
	case TRAP_OF_SHOTS_III:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SHOT, SV_AMMO_HEAVY, 5, 5, 0, "Mithril Shots Trap");
		break;
	case TRAP_OF_DAGGERS_I:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SWORD, SV_BROKEN_DAGGER, 2, 4, 0, "Broken Daggers Trap");
		break;
	case TRAP_OF_DAGGERS_II:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SWORD, SV_DAGGER, 3, 4, 0, "Daggers Trap");
		break;
	case TRAP_OF_POISON_DAGGERS_I:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SWORD, SV_BROKEN_DAGGER, 2, 4, randint(20), "Poison Broken Daggers Trap");
		break;
	case TRAP_OF_POISON_DAGGERS_II:
		ident = player_handle_missile_trap(2 + (max_dlv_real[dungeon_type] / 35), TV_SWORD, SV_DAGGER, 3, 4, randint(30), "Poison Daggers Trap");
		break;

	case TRAP_OF_SMG:
		ident = player_handle_missile_trap(3 + (max_dlv_real[dungeon_type] / 50), TV_AMMO_SMG, SV_AMMO_NORMAL, 4, 11, 0, "SMG Trap");
		break;
	case TRAP_OF_ASSAULT:
		ident = player_handle_missile_trap(4 + (max_dlv_real[dungeon_type] / 50), TV_AMMO_ASSAULT, SV_AMMO_NORMAL, 4, 12, 0, "Assault Trap");
		break;

	case TRAP_OF_DROP_ITEMS:
		{
			s16b i;
			bool message = FALSE;

			for (i = 0; i < INVEN_PACK; i++)
			{
				object_type tmp_obj;

				if (!p_ptr->inventory[i].k_idx) continue;
				if (randint(100) < 80) continue;
				if (p_ptr->inventory[i].name1 == ART_POWER) continue;

				tmp_obj = p_ptr->inventory[i];

				/* drop carefully */
				drop_near(&tmp_obj, 0, y, x);
				inven_item_increase(i, -999);
				inven_item_optimize(i);
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				if (!message)
				{
					msg_print("You are startled by a sudden sound.");
					message = TRUE;
				}
				ident = TRUE;
			}
			if (!ident)
			{
				msg_print("You hear a sudden, strange sound.");
			}
			break;
		}

	case TRAP_OF_DROP_ALL_ITEMS:
		{
			s16b i;
			bool message = FALSE;

			for (i = 0; i < INVEN_PACK; i++)
			{
				object_type tmp_obj;

				if (!p_ptr->inventory[i].k_idx) continue;
				if (randint(100) < 10) continue;
				if (p_ptr->inventory[i].name1 == ART_POWER) continue;

				tmp_obj = p_ptr->inventory[i];

				/* drop carefully */
				drop_near(&tmp_obj, 0, y, x);
				inven_item_increase(i, -999);
				inven_item_optimize(i);
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				if (!message)
				{
					msg_print("You are greatly startled by a sudden sound.");
					message = TRUE;
				}
				ident = TRUE;
			}
			if (!ident)
			{
				msg_print("You hear a sudden, strange sound.");
			}
			break;
		}

	case TRAP_OF_DROP_EVERYTHING:
		{
			s16b i;
			bool message = FALSE;

			for (i = 0; i < INVEN_TOTAL; i++)
			{
				object_type tmp_obj;
				if (!p_ptr->inventory[i].k_idx) continue;
				if (randint(100) < 30) continue;
				if (p_ptr->inventory[i].name1 == ART_POWER) continue;

				tmp_obj = p_ptr->inventory[i];
				/* drop carefully */

				drop_near(&tmp_obj, 0, y, x);
				inven_item_increase(i, -999);
				inven_item_optimize(i);
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				if (!message)
				{
					msg_print("You are completely startled by a sudden sound.");
					message = TRUE;
				}
				ident = TRUE;
			}
			if (!ident)
			{
				msg_print("You hear a sudden, strange sound.");
			}
			break;
		}

		/* trap of drop all: like drop everything, but no percentage-based saving throw --Amy */
	case TRAP_OF_DROP_X:
		{
			s16b i;
			bool message = FALSE;

			for (i = 0; i < INVEN_TOTAL; i++)
			{
				object_type tmp_obj;
				if (!p_ptr->inventory[i].k_idx) continue;
				if (p_ptr->inventory[i].name1 == ART_POWER) continue;

				tmp_obj = p_ptr->inventory[i];
				/* drop carefully */

				drop_near(&tmp_obj, 0, y, x);
				inven_item_increase(i, -999);
				inven_item_optimize(i);
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				if (!message)
				{
					msg_print("You are completely startled by a sudden sound.");
					message = TRUE;
				}
				ident = TRUE;
			}
			if (!ident)
			{
				msg_print("You hear a sudden, strange sound.");
			}
			break;
		}

		/* Bolt Trap */
	case TRAP_G_ELEC_BOLT:
		ident = player_handle_breath_trap(1, GF_ELEC, TRAP_G_ELEC_BOLT);
		break;
	case TRAP_G_POIS_BOLT:
		ident = player_handle_breath_trap(1, GF_POIS, TRAP_G_POIS_BOLT);
		break;
	case TRAP_G_ACID_BOLT:
		ident = player_handle_breath_trap(1, GF_ACID, TRAP_G_ACID_BOLT);
		break;
	case TRAP_G_COLD_BOLT:
		ident = player_handle_breath_trap(1, GF_COLD, TRAP_G_COLD_BOLT);
		break;
	case TRAP_G_FIRE_BOLT:
		ident = player_handle_breath_trap(1, GF_FIRE, TRAP_G_FIRE_BOLT);
		break;
	case TRAP_OF_ELEC_BOLT:
		ident = player_handle_breath_trap(1, GF_ELEC, TRAP_OF_ELEC_BOLT);
		break;
	case TRAP_OF_POIS_BOLT:
		ident = player_handle_breath_trap(1, GF_POIS, TRAP_OF_POIS_BOLT);
		break;
	case TRAP_OF_ACID_BOLT:
		ident = player_handle_breath_trap(1, GF_ACID, TRAP_OF_ACID_BOLT);
		break;
	case TRAP_OF_COLD_BOLT:
		ident = player_handle_breath_trap(1, GF_COLD, TRAP_OF_COLD_BOLT);
		break;
	case TRAP_OF_FIRE_BOLT:
		ident = player_handle_breath_trap(1, GF_FIRE, TRAP_OF_FIRE_BOLT);
		break;
	case TRAP_OF_PLASMA_BOLT:
		ident = player_handle_breath_trap(1, GF_PLASMA, TRAP_OF_PLASMA_BOLT);
		break;
	case TRAP_OF_WATER_BOLT:
		ident = player_handle_breath_trap(1, GF_WATER, TRAP_OF_WATER_BOLT);
		break;
	case TRAP_OF_LITE_BOLT:
		ident = player_handle_breath_trap(1, GF_LITE, TRAP_OF_LITE_BOLT);
		break;
	case TRAP_OF_DARK_BOLT:
		ident = player_handle_breath_trap(1, GF_DARK, TRAP_OF_DARK_BOLT);
		break;
	case TRAP_OF_SHARDS_BOLT:
		ident = player_handle_breath_trap(1, GF_SHARDS, TRAP_OF_SHARDS_BOLT);
		break;
	case TRAP_OF_SOUND_BOLT:
		ident = player_handle_breath_trap(1, GF_SOUND, TRAP_OF_SOUND_BOLT);
		break;
	case TRAP_OF_CONFUSION_BOLT:
		ident = player_handle_breath_trap(1, GF_CONFUSION, TRAP_OF_CONFUSION_BOLT);
		break;
	case TRAP_OF_FORCE_BOLT:
		ident = player_handle_breath_trap(1, GF_FORCE, TRAP_OF_FORCE_BOLT);
		break;
	case TRAP_OF_INERTIA_BOLT:
		ident = player_handle_breath_trap(1, GF_INERTIA, TRAP_OF_INERTIA_BOLT);
		break;
	case TRAP_OF_MANA_BOLT:
		ident = player_handle_breath_trap(1, GF_MANA, TRAP_OF_MANA_BOLT);
		break;
	case TRAP_OF_ICE_BOLT:
		ident = player_handle_breath_trap(1, GF_ICE, TRAP_OF_ICE_BOLT);
		break;
	case TRAP_OF_CHAOS_BOLT:
		ident = player_handle_breath_trap(1, GF_CHAOS, TRAP_OF_CHAOS_BOLT);
		break;
	case TRAP_OF_NETHER_BOLT:
		ident = player_handle_breath_trap(1, GF_NETHER, TRAP_OF_NETHER_BOLT);
		break;
	case TRAP_OF_DISENCHANT_BOLT:
		ident = player_handle_breath_trap(1, GF_DISENCHANT, TRAP_OF_DISENCHANT_BOLT);
		break;
	case TRAP_OF_NEXUS_BOLT:
		ident = player_handle_breath_trap(1, GF_NEXUS, TRAP_OF_NEXUS_BOLT);
		break;
	case TRAP_OF_NERVE_BOLT:
		ident = player_handle_breath_trap(1, GF_NERVE, TRAP_OF_NERVE_BOLT);
		break;
	case TRAP_OF_MIND_BOLT:
		ident = player_handle_breath_trap(1, GF_MIND, TRAP_OF_MIND_BOLT);
		break;
	case TRAP_OF_ETHER_BOLT:
		ident = player_handle_breath_trap(1, GF_ETHER, TRAP_OF_ETHER_BOLT);
		break;
	case TRAP_OF_TIME_BOLT:
		ident = player_handle_breath_trap(1, GF_TIME, TRAP_OF_TIME_BOLT);
		break;
	case TRAP_OF_GRAVITY_BOLT:
		ident = player_handle_breath_trap(1, GF_GRAVITY, TRAP_OF_GRAVITY_BOLT);
		break;
	case TRAP_OF_METEOR_BOLT:
		ident = player_handle_breath_trap(1, GF_METEOR, TRAP_OF_METEOR_BOLT);
		break;
	case TRAP_OF_ARROW_BOLT:
		ident = player_handle_breath_trap(1, GF_ARROW, TRAP_OF_ARROW_BOLT);
		break;
	case TRAP_OF_DISINT_BOLT:
		ident = player_handle_breath_trap(1, GF_DISINTEGRATE, TRAP_OF_DISINT_BOLT);
		break;
	case TRAP_OF_WALL_BOLT:
		t_info[trap].ident = TRUE;

		if ((item == -1) || (item == -2))
		{
			place_trap(y, x);
			if (player_has_los_bold(y, x))
			{
				note_spot(y, x);
				lite_spot(y, x);
			}
		}
		else
		{
			/* re-trap the chest */
			place_trap(y, x);
		}

		player_handle_breath_trap(1, GF_STONE_WALL, TRAP_OF_WALL_BOLT);

		msg_print("You identified that trap as Wall Bolt Trap.");
		ident = FALSE;
		break;

		/* Ball Trap */
	case TRAP_OF_ELEC_BALL:
		ident = player_handle_breath_trap(3, GF_ELEC, TRAP_OF_ELEC_BALL);
		break;
	case TRAP_OF_POIS_BALL:
		ident = player_handle_breath_trap(3, GF_POIS, TRAP_OF_POIS_BALL);
		break;
	case TRAP_OF_ACID_BALL:
		ident = player_handle_breath_trap(3, GF_ACID, TRAP_OF_ACID_BALL);
		break;
	case TRAP_OF_COLD_BALL:
		ident = player_handle_breath_trap(3, GF_COLD, TRAP_OF_COLD_BALL);
		break;
	case TRAP_OF_FIRE_BALL:
		ident = player_handle_breath_trap(3, GF_FIRE, TRAP_OF_FIRE_BALL);
		break;
	case TRAP_OF_PLASMA_BALL:
		ident = player_handle_breath_trap(3, GF_PLASMA, TRAP_OF_PLASMA_BALL);
		break;
	case TRAP_OF_WATER_BALL:
		ident = player_handle_breath_trap(3, GF_WATER, TRAP_OF_WATER_BALL);
		break;
	case TRAP_OF_LITE_BALL:
		ident = player_handle_breath_trap(3, GF_LITE, TRAP_OF_LITE_BALL);
		break;
	case TRAP_OF_DARK_BALL:
		ident = player_handle_breath_trap(3, GF_DARK, TRAP_OF_DARK_BALL);
		break;
	case TRAP_OF_SHARDS_BALL:
		ident = player_handle_breath_trap(3, GF_SHARDS, TRAP_OF_SHARDS_BALL);
		break;
	case TRAP_OF_SOUND_BALL:
		ident = player_handle_breath_trap(3, GF_SOUND, TRAP_OF_SOUND_BALL);
		break;
	case TRAP_OF_CONFUSION_BALL:
		ident = player_handle_breath_trap(3, GF_CONFUSION, TRAP_OF_CONFUSION_BALL);
		break;
	case TRAP_OF_FORCE_BALL:
		ident = player_handle_breath_trap(3, GF_FORCE, TRAP_OF_FORCE_BALL);
		break;
	case TRAP_OF_INERTIA_BALL:
		ident = player_handle_breath_trap(3, GF_INERTIA, TRAP_OF_INERTIA_BALL);
		break;
	case TRAP_OF_MANA_BALL:
		ident = player_handle_breath_trap(3, GF_MANA, TRAP_OF_MANA_BALL);
		break;
	case TRAP_OF_ICE_BALL:
		ident = player_handle_breath_trap(3, GF_ICE, TRAP_OF_ICE_BALL);
		break;
	case TRAP_OF_CHAOS_BALL:
		ident = player_handle_breath_trap(3, GF_CHAOS, TRAP_OF_CHAOS_BALL);
		break;
	case TRAP_OF_NETHER_BALL:
		ident = player_handle_breath_trap(3, GF_NETHER, TRAP_OF_NETHER_BALL);
		break;
	case TRAP_OF_HOLY_BALL:
		ident = player_handle_breath_trap(3, GF_HOLY_FIRE, TRAP_OF_HOLY_BALL);
		break;
	case TRAP_OF_HELL_BALL:
		ident = player_handle_breath_trap(3, GF_HELL_FIRE, TRAP_OF_HELL_BALL);
		break;
	case TRAP_OF_DISENCHANT_BALL:
		ident = player_handle_breath_trap(3, GF_DISENCHANT, TRAP_OF_DISENCHANT_BALL);
		break;
	case TRAP_OF_NEXUS_BALL:
		ident = player_handle_breath_trap(3, GF_NEXUS, TRAP_OF_NEXUS_BALL);
		break;
	case TRAP_OF_NERVE_BALL:
		ident = player_handle_breath_trap(3, GF_NERVE, TRAP_OF_NERVE_BALL);
		break;
	case TRAP_OF_MIND_BALL:
		ident = player_handle_breath_trap(3, GF_MIND, TRAP_OF_MIND_BALL);
		break;
	case TRAP_OF_ETHER_BALL:
		ident = player_handle_breath_trap(3, GF_ETHER, TRAP_OF_ETHER_BALL);
		break;
	case TRAP_OF_TIME_BALL:
		ident = player_handle_breath_trap(3, GF_TIME, TRAP_OF_TIME_BALL);
		break;
	case TRAP_OF_GRAVITY_BALL:
		ident = player_handle_breath_trap(3, GF_GRAVITY, TRAP_OF_GRAVITY_BALL);
		break;
	case TRAP_OF_METEOR_BALL:
		ident = player_handle_breath_trap(3, GF_METEOR, TRAP_OF_METEOR_BALL);
		break;
	case TRAP_OF_ARROW_BALL:
		ident = player_handle_breath_trap(3, GF_ARROW, TRAP_OF_ARROW_BALL);
		break;
	case TRAP_OF_DISINT_BALL:
		ident = player_handle_breath_trap(3, GF_DISINTEGRATE, TRAP_OF_DISINT_BALL);
		break;
	case TRAP_OF_WALL_BALL:
		t_info[trap].ident = TRUE;

		if ((item == -1) || (item == -2))
		{
			place_trap(y, x);
			if (player_has_los_bold(y, x))
			{
				note_spot(y, x);
				lite_spot(y, x);
			}
		}
		else
		{
			/* re-trap the chest */
			place_trap(y, x);
		}

		player_handle_breath_trap(3, GF_STONE_WALL, TRAP_OF_WALL_BALL);

		msg_print("You identified that trap as Wall Ball Trap.");
		ident = FALSE;
		break;

		/* Multiple Ball Trap, by Amy */
	case TRAP_OF_ELEC_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_ELEC, TRAP_OF_ELEC_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_POIS_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_POIS, TRAP_OF_POIS_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_ACID_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_ACID, TRAP_OF_ACID_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_COLD_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_COLD, TRAP_OF_COLD_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_FIRE_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_FIRE, TRAP_OF_FIRE_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_PLASMA_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_PLASMA, TRAP_OF_PLASMA_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_WATER_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_WATER, TRAP_OF_WATER_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_LITE_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_LITE, TRAP_OF_LITE_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_DARK_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_DARK, TRAP_OF_DARK_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_SHARDS_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_SHARDS, TRAP_OF_SHARDS_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_SOUND_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_SOUND, TRAP_OF_SOUND_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_CONFUSION_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_CONFUSION, TRAP_OF_CONFUSION_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_FORCE_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_FORCE, TRAP_OF_FORCE_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_INERTIA_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_INERTIA, TRAP_OF_INERTIA_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_MANA_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_MANA, TRAP_OF_MANA_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_ICE_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_ICE, TRAP_OF_ICE_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_CHAOS_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_CHAOS, TRAP_OF_CHAOS_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_NETHER_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_NETHER, TRAP_OF_NETHER_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_HOLY_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_HOLY_FIRE, TRAP_OF_HOLY_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_HELL_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_HELL_FIRE, TRAP_OF_HELL_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_DISENCHANT_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_DISENCHANT, TRAP_OF_DISENCHANT_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_NEXUS_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_NEXUS, TRAP_OF_NEXUS_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_NERVE_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_NERVE, TRAP_OF_NERVE_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_MIND_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_MIND, TRAP_OF_MIND_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_ETHER_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_ETHER, TRAP_OF_ETHER_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_TIME_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_TIME, TRAP_OF_TIME_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_GRAVITY_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_GRAVITY, TRAP_OF_GRAVITY_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_METEOR_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_METEOR, TRAP_OF_METEOR_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_ARROW_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_ARROW, TRAP_OF_ARROW_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_DISINT_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_DISINTEGRATE, TRAP_OF_DISINT_BALLS);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_NUKE_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_NUKE, trap);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_PSI_BALLS:
		{
			int ballamount = 2;
			if (max_dlv_real[dungeon_type] >= 70) ballamount = 3;
			if (max_dlv_real[dungeon_type] >= 120) ballamount = 4;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(3, GF_PSI, trap);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_DEATH_BALL:
		{
			int ballamount = 2;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(0, GF_DEATH_RAY, trap);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_DEATH_BALLS:
		{
			int ballamount = 3;
			while (ballamount > 0) {
				ident = player_handle_breath_trap(0, GF_DEATH_RAY, trap);
				ballamount--;
			}
		}
		break;
	case TRAP_OF_WALL_BALLS:
		t_info[trap].ident = TRUE;

		if ((item == -1) || (item == -2))
		{
			place_trap(y, x);
			if (player_has_los_bold(y, x))
			{
				note_spot(y, x);
				lite_spot(y, x);
			}
		}
		else
		{
			/* re-trap the chest */
			place_trap(y, x);
		}

		player_handle_breath_trap(3 + (max_dlv_real[dungeon_type] / 20), GF_STONE_WALL, TRAP_OF_WALL_BALLS);

		msg_print("You identified that trap as Wall Balls Trap.");
		ident = FALSE;
		break;

	case TRAP_OF_TERRAIN_FOREST:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 8, FEAT_TREES, 50);

		msg_print("Trees grow in the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Forest Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_FOREST_SMALL:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 9, FEAT_SMALL_TREES, 70);

		msg_print("Small trees grow in the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Small Forest Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_WATER:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_SHAL_WATER, 40);

		msg_print("Water floods the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Water Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_WATER_DEEP:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_DEEP_WATER, 40);

		msg_print("Deep water floods the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Deep Water Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_LAVA:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_SHAL_LAVA, 35);

		msg_print("Lava floods the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Lava Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_LAVA_DEEP:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_DEEP_LAVA, 35);

		msg_print("Deep lava floods the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Deep Lava Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_ICE:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_ICE, 50);

		msg_print("The dungeon becomes icy!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Ice Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_SLIPPERY:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_SLIPPERY, 50);

		msg_print("The dungeon becomes slippery!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Slippery Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_FIRE:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_FIRE, 25);

		msg_print("Suddenly, blazing fires are everywhere!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Blazing Fire Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_NETHER_MIST:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 7, FEAT_NETHER_MIST, 55);

		msg_print("Nether mist forms out of thin air!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Nether Mist Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_SWAMP:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_SWAMP_POOL, 30);

		msg_print("Swampy pools appear!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Swamp Pool Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_ANDUIN:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_ANDUIN, 40);

		msg_print("Rushing water flows into the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Anduin Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_WATERFALL:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_WATERFALL, 25);

		msg_print("Suddenly, huge waterfalls rush into the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Waterfall Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_SHOCK:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_DMG_SHOCK, 40);

		msg_print("Suddenly the ground starts sparking with high voltage!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Shock Field Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_ACID:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_DMG_ACID, 45);

		msg_print("Highly corrosive acid flows into the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Acid Pool Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_PLASMA:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_DMG_PLASMA, 40);

		msg_print("The air is filled with extremely hot plasma!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Plasma Cloud Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_DARK:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 8, FEAT_DMG_DARK, 50);

		msg_print("The entire area gets darker!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Dark Area Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_SHARD:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 5, FEAT_DMG_SHARD, 25);

		msg_print("Tornadoes sweep into the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Tornado Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_SOUND:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_DMG_SOUND, 30);

		msg_print("Auuuuuuggghhhgh, your ears...");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Cartridge Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_INERT:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_DMG_INERT, 20);

		msg_print("Quicksand holes appear in the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Shifting Sand Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_CHAOS:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_DMG_CHAOS, 35);

		msg_print("The floor becomes much more chaotic!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Chaotic Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_DISEN:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_DMG_DISEN, 25);

		msg_print("The very air seems to drain all magic!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Antimagic Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_NEXUS:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_DMG_NEXUS, 40);

		msg_print("Purple fog appears in the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Nexus Field Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_NERVE:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 7, FEAT_DMG_NERVE, 50);

		msg_print("Dangerous-looking contraptions are erected in the dungeon!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Nerve Tickle Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_MIND:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 8, FEAT_DMG_MIND, 65);

		msg_print("Mindwave emitters spring up everywhere!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Mindwave Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_ETHER:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 10, FEAT_DMG_ETHER, 30);

		msg_print("Ether mist spreads!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Ether Mist Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_TIME:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 6, FEAT_DMG_TIME, 40);

		msg_print("You seem to be caught in a time loop!");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Timelapse Trap.");
		}
		break;

	case TRAP_OF_TERRAIN_NUKE:
		ident = TRUE;
		fill_area_terrain(p_ptr->py, p_ptr->px, 10, FEAT_DMG_NUKE, 70);

		msg_print("Icky green waste floods the dungeon...");

		if (randint(5) == 1) {
			ident = FALSE;
			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}
			msg_print("You identified that trap as Nuclear Spill Trap.");
		}
		break;

		/* -SC- */
	case TRAP_OF_FEMINITY:
		{
			msg_print("Gas sprouts out... you feel yourself transmute.");
			p_ptr->psex = SEX_FEMALE;
			sp_ptr = &sex_info[p_ptr->psex];
			ident = TRUE;
			trap_hit(trap);
			break;
		}

	case TRAP_OF_MASCULINITY:
		{
			msg_print("Gas sprouts out... you feel yourself transmute.");
			p_ptr->psex = SEX_MALE;
			sp_ptr = &sex_info[p_ptr->psex];
			ident = TRUE;
			trap_hit(trap);
			break;
		}

	case TRAP_OF_NEUTRALITY:
		{
			msg_print("Gas sprouts out... you feel yourself transmute.");
			p_ptr->psex = SEX_NEUTER;
			sp_ptr = &sex_info[p_ptr->psex];
			ident = TRUE;
			trap_hit(trap);
			break;
		}

	case TRAP_OF_AGING:
		{
			msg_print("Colors are scintillating around you. "
			          "You see your past running before your eyes.");
			p_ptr->age += randint((rp_ptr->b_age + rmp_ptr->b_age) / 2);
			ident = TRUE;
			trap_hit(trap);
			break;
		}

	case TRAP_OF_YOUTH:
		{
			s16b tmp;

			msg_print("Colors are scintillating around you. "
			          "You feel like living in the past.");
			tmp = rp_ptr->b_age + rmp_ptr->b_age;

			p_ptr->age -= randint(tmp / 2);
			if (p_ptr->age <= tmp / 2) p_ptr->age = tmp / 2;

			ident = TRUE;
			trap_hit(trap);
			break;
		}

	case TRAP_OF_GROWING:
		{
			s16b tmp;

			msg_print("Heavy fumes sprout out... you feel yourself transmute.");
			if (p_ptr->psex == SEX_FEMALE) tmp = rp_ptr->f_b_ht + rmp_ptr->f_b_ht;
			else tmp = rp_ptr->m_b_ht + rmp_ptr->m_b_ht;

			p_ptr->ht += randint(tmp / 4);
			ident = TRUE;
			trap_hit(trap);
			break;
		}

	case TRAP_OF_SHRINKING:
		{
			s16b tmp;

			msg_print("Heavy fumes sprout out... you feel yourself transmute.");
			if (p_ptr->psex == SEX_FEMALE) tmp = rp_ptr->f_b_ht + rmp_ptr->f_b_ht;
			else tmp = rp_ptr->m_b_ht + rmp_ptr->m_b_ht;

			p_ptr->ht -= randint(tmp / 4);
			if (p_ptr->ht <= tmp / 4) p_ptr->ht = tmp / 4;
			ident = TRUE;
			trap_hit(trap);
			break;
		}

	case TRAP_OF_WEIGHT:
		{
			s16b tmp;

			msg_print("Heavy fumes sprout out... you feel yourself transmute.");
			if (p_ptr->psex == SEX_FEMALE) tmp = rp_ptr->f_b_wt + rmp_ptr->f_b_wt;
			else tmp = rp_ptr->m_b_wt + rmp_ptr->m_b_wt;

			p_ptr->wt += randint(tmp / 4);
			ident = TRUE;
			trap_hit(trap);
			break;
		}

	case TRAP_OF_WEIGHT_LOSS:
		{
			s16b tmp;

			msg_print("Heavy fumes sprout out... you feel yourself transmute.");
			if (p_ptr->psex == SEX_FEMALE) tmp = rp_ptr->f_b_wt + rmp_ptr->f_b_wt;
			else tmp = rp_ptr->m_b_wt + rmp_ptr->m_b_wt;

			p_ptr->wt -= randint(tmp / 4);
			if (p_ptr->wt <= tmp / 4) p_ptr->wt = tmp / 4;
			ident = TRUE;
			trap_hit(trap);
			break;
		}

		/* trap of corruption, by Amy, aka "completely fuck up that stupid player" :-P */
	case TRAP_OF_CORRUPTION:
		{
			msg_print("You feel the dark corruptions of Morgoth coming over you!");
			(void) gain_random_corruption(0);
			ident = TRUE;

			if (randint(3) == 1) { /* so that you can't just repeatedly trigger it until you have the corruptions you want --Amy */
				ident = FALSE;
				t_info[trap].ident = TRUE;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}
				msg_print("You identified that trap as Trap of Corruption.");
			}

			break;
		}

		/* trap of random corruption, by Amy, just like corrupted subrace */
	case TRAP_OF_RANDOM_CORRUPTION:
		{
			msg_print("Something seems to twist your body...");

			if (magik(45))
			{
				lose_corruption(0);
			}
			else
			{
				gain_random_corruption(0);
			}

			ident = TRUE;

			if (randint(3) == 1) { /* so that you can't just repeatedly trigger it until you have the corruptions you want --Amy */
				ident = FALSE;
				t_info[trap].ident = TRUE;

				if ((item == -1) || (item == -2))
				{
					place_trap(y, x);
					if (player_has_los_bold(y, x))
					{
						note_spot(y, x);
						lite_spot(y, x);
					}
				}
				else
				{
					/* re-trap the chest */
					place_trap(y, x);
				}
				msg_print("You identified that trap as Trap of Random Corruption.");
			}

			break;
		}

	case TRAP_OF_NASTINESS:
		{
			msg_print("Nasty!");
			give_random_nastytrap_effect();
			ident = TRUE;
			break;
		}

		/* trap of black breath, by Amy */
	case TRAP_OF_BLACK_BREATH:
		{
			msg_print("You feel the Black Breath slowly draining your life...");
			p_ptr->black_breath = TRUE;
			ident = TRUE;
			break;
		}

		/* trap of eldritch horror, by Amy */
	case TRAP_OF_ELDRITCH:
		{
			msg_print("Oh no! You see something not meant for mortal eyes!");

			switch (rand_int(100)) {

			default:
			case 0: /* Level drain */
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			{
				if (p_ptr->hold_life && !p_ptr->nastytrap95)
				{
					msg_print("You feel your life slipping away!");
					lose_exp(p_ptr->exp / 100);
				}
				else
				{
					msg_print("You feel your life draining away!");
					lose_exp(p_ptr->exp / 16);
				}
				break;
			}

			case 9: /* Mind blast */
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			{
				if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
				{
					(void)set_confused(p_ptr->confused + rand_int(4) + 4);
				}
				if ( (!p_ptr->resist_chaos || p_ptr->nastytrap31 || (rand_int(100) < 5) ) && (randint(3) == 1))
				{
					(void) set_image(p_ptr->image + rand_int(250) + 150);
				}
				break;
			}

			case 18: /* Lose int & wis */
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			{
				do_dec_stat(A_INT, STAT_DEC_NORMAL);
				do_dec_stat(A_WIS, STAT_DEC_NORMAL);
				break;
			}

			case 27: /* Insanity */
			case 28:
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			{
				msg_print("You are getting insane!");
				take_sanity_hit(rand_int(5 + dun_level), "an eldritch blast");
				break;
			}

			case 36: /* Brain smash */
			case 37:
			case 38:
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			{
				if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
				{
					(void)set_confused(p_ptr->confused + rand_int(4) + 4);
				}
				if (!p_ptr->free_act || (rand_int(p_ptr->nastytrap57 ? 20 : 100) == 0) )
				{
					if (!p_ptr->resist_nerve || magik(50)) {
						(void)set_paralyzed(p_ptr->paralyzed + rand_int(4) + 4);
					}
				}
				while ((rand_int(100) > player_actual_saving_throw()) && (randint(100) != 1) )
					(void)do_dec_stat(A_INT, STAT_DEC_NORMAL);
				while ((rand_int(100) > player_actual_saving_throw()) && (randint(100) != 1) )
					(void)do_dec_stat(A_WIS, STAT_DEC_NORMAL);
				if (!p_ptr->resist_chaos || p_ptr->nastytrap31)
				{
					(void) set_image(p_ptr->image + rand_int(250) + 150);
				}
				break;
			}

			case 46: /* Permanent lose int & wis */
			case 47:
			case 48:
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
			case 55:
			{
				bool happened = FALSE;
				if (dec_stat(A_INT, 10, TRUE)) happened = TRUE;
				if (dec_stat(A_WIS, 10, TRUE)) happened = TRUE;
				if (happened)
					msg_print("You feel much less sane than before.");
				break;
			}

			case 56: /* Stun */
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
			case 64:
			{

				msg_print("You stagger...");
				(void)set_stun(p_ptr->stun + rand_int(30) );
				break;
			}

			case 65: /* Amnesia */
			case 66:
			case 67:
			case 68:
			case 69:
			case 70:
			case 71:
			case 72:
			case 73:
			{

				if (lose_all_info())
					msg_print("You forget everything in your utmost terror!");
				break;
			}

			case 74: /* Aggravate monster */
			case 75:
			case 76:
			case 77:
			case 78:
			case 79:
			case 80:
			case 81:
			case 82:
			{

				msg_print("You let out a bloodcurdling scream of fear!");
				aggravate_monsters(1);
				break;
			}

			case 83: /* Fear */
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
			case 89:
			case 90:
			case 91:
			{

				if (!p_ptr->resist_fear || p_ptr->nastytrap30 || (rand_int(100) < 4) )
				{
					set_afraid(p_ptr->afraid + 3 + dun_level);
				}
				break;
			}

			case 92: /* Lose money */
			case 93:
			case 94:
			case 95:
			case 96:
			case 97:
			case 98:
			case 99:
			case 100:
			{
				s32b gold = (p_ptr->au / 50) + randint(15 + (20 * dun_level) );
				if (gold < 2) gold = 2;
				if (gold > 5000) gold = (p_ptr->au / 20) + randint(3000);
				if (gold > p_ptr->au) gold = p_ptr->au;

				p_ptr->au -= gold;
				if (gold <= 0)
				{
					msg_print("You are startled.");			
				}
				else if (p_ptr->au)
				{
					msg_format("You drop %ld coins in terror!", (long)gold);
				}
				else
				{
					msg_print("You drop all your money in terror!");
				}
				p_ptr->redraw |= (PR_GOLD);
				break;
			}

			} /* randint switch */

			ident = TRUE;
			break;
		}

		/* ancient morgothian trap, by Amy, aka "lol darkgod's traps weren't bad enough" :-P */
	case TRAP_OF_DG_CURSE:
		{
			msg_print("Morgoth's evil curse afflicts you.");
			activate_dg_curse();
			ident = TRUE;
			break;
		}

		/* topi ylinen trap, by Amy */
	case TRAP_OF_TOPI_YLINEN:
		{
			msg_print("An ancient foul curse takes hold of you.");
			activate_ty_curse();
			ident = TRUE;
			break;
		}

		/* Trap of Divine Intervention, by Amy */
	case TRAP_OF_DIVINE_INTER:
		{
			gain_level_reward(0);

			t_info[trap].ident = TRUE;

			if ((item == -1) || (item == -2))
			{
				place_trap(y, x);
				if (player_has_los_bold(y, x))
				{
					note_spot(y, x);
					lite_spot(y, x);
				}
			}
			else
			{
				/* re-trap the chest */
				place_trap(y, x);
			}

			msg_print("You identified that trap as Divine Intervention Trap.");
			ident = FALSE;

			break;
		}

		/* Trap of Divine Anger */
	case TRAP_OF_DIVINE_ANGER:
		{
			if (p_ptr->pgod == 0)
			{
				msg_format("Suddenly you feel glad you're a mere %s", spp_ptr->title + c_name);
			}
			else
			{
				cptr name;

				name = deity_info[p_ptr->pgod].name;
				msg_format("You feel you have angered %s.", name);
				inc_piety(p_ptr->pgod, -3000);
			}
			ident = TRUE;
			break;
		}

		/* Trap of Divine Wrath */
	case TRAP_OF_DIVINE_WRATH:
		{
			if (p_ptr->pgod == 0)
			{
				msg_format("Suddenly you feel glad you're a mere %s", spp_ptr->title + c_name);
			}
			else
			{
				cptr name;

				name = deity_info[p_ptr->pgod].name;

				msg_format("%s quakes in rage: ``Thou art supremely insolent, mortal!!''", name);
				inc_piety(p_ptr->pgod, -500 * p_ptr->lev);
			}
			ident = TRUE;
			break;
		}

		/* Trap of Divine Retribution */
	case TRAP_OF_DIVINE_RETRIBUTION:
		{
			if (p_ptr->pgod == 0)
			{
				msg_format("Suddenly you feel glad you're a mere %s", spp_ptr->title + c_name);
			}
			else
			{
				cptr name;

				name = deity_info[p_ptr->pgod].name;

				msg_format("You have a feeling that %s is angry...", name);
				if (p_ptr->pgod > 0)
					inc_piety(p_ptr->pgod, -(p_ptr->grace * 2) );
				else
					inc_piety(p_ptr->pgod, -5000);
				
			}
			ident = TRUE;
			break;
		}

		/* Trap of hallucination */
	case TRAP_OF_HALLUCINATION:
		{
			msg_print("Scintillating colors hypnotise you for a moment.");
			ident = TRUE;

			set_image(p_ptr->image + 80);
		}
		break;

		/* Super Hallu Trap by Amy */
	case TRAP_OF_HALLU_X:
		{
			msg_print("Scintillating colors hypnotise you for a moment.");
			ident = TRUE;

			set_image(p_ptr->image + 500);
		}
		break;

	case TRAP_OF_STATUS_EFFECTS:
		{
			msg_print("Suddenly you're afflicted with many status effects!");
			ident = TRUE;

			set_image(p_ptr->image + 80);
			if (!p_ptr->resist_fear || p_ptr->nastytrap30 || (rand_int(100) < 5) ) {
				set_afraid(p_ptr->afraid + 50 + randint(200));
			}
			(void)set_stun(p_ptr->stun + randint(50) + p_ptr->lev);
			if (!p_ptr->resist_blind || p_ptr->nastytrap29 || (rand_int(100) < 5) )
			{
				set_blind(p_ptr->blind + rand_int(100) + 100);
			}
			if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
			{
				set_confused(p_ptr->confused + rand_int(20) + 15);
			}
			if (!(p_ptr->resist_pois || p_ptr->immune_pois || p_ptr->oppose_pois) || p_ptr->nastytrap32)
			{
				(void)set_poisoned(p_ptr->poisoned + rand_int(15) + 10);
			}
		}
		break;

	case TRAP_OF_STATUS_EFFECTS_X:
		{
			msg_print("Suddenly you're afflicted with all the status effects!");
			ident = TRUE;

			set_image(p_ptr->image + 500);
			set_afraid(p_ptr->afraid + 50 + randint(200));
			(void)set_stun(p_ptr->stun + randint(100) + p_ptr->lev);
			set_blind(p_ptr->blind + rand_int(100) + 100);
			set_confused(p_ptr->confused + rand_int(30) + 15);
			(void)set_poisoned(p_ptr->poisoned + rand_int(50) + 20);
		}
		break;

	case TRAP_OF_MIND_BLAST:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else efflevel = p_ptr->lev;
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel < 2) efflevel = 2;

			int sanitydamage;

			msg_print("You feel something focusing on your mind.");
			ident = TRUE;

			msg_print("Your mind is blasted by psionic energy.");

			if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
			{
				(void)set_confused(p_ptr->confused + rand_int(4) + 4);
			}

			if ( (!p_ptr->resist_chaos || p_ptr->nastytrap31 || (rand_int(100) < 5) ) && (randint(3) == 1))
			{
				(void) set_image(p_ptr->image + rand_int(250) + 150);
			}

			sanitydamage = damroll(8, 8);
			if (sanitydamage > (efflevel * 2)) sanitydamage = efflevel * 2;

			take_sanity_hit(sanitydamage, "a mind blast");

		}
		break;

	case TRAP_OF_BRAIN_SMASH:
		{
			int efflevel = 99;
			if (dun_level > 0) efflevel = dun_level;
			else {
				efflevel = p_ptr->lev;
				if (p_ptr->lev > 40) efflevel = 99;
			}
			if (efflevel < dun_level) efflevel = dun_level;
			if (efflevel < 2) efflevel = 2;

			int sanitydamage;

			msg_print("You feel something focusing on your mind.");
			ident = TRUE;

			msg_print("Your mind is blasted by psionic energy.");

			sanitydamage = damroll(12, 15);
			if (sanitydamage > (efflevel * 3)) sanitydamage = efflevel * 3;

			take_sanity_hit(sanitydamage, "a mind blast");

			if (!p_ptr->resist_blind || p_ptr->nastytrap29 || (rand_int(100) < 5) )
			{
				(void)set_blind(p_ptr->blind + 8 + rand_int(8));
			}
			if (!p_ptr->resist_conf || p_ptr->nastytrap28 || (rand_int(100) < 5) )
			{
				(void)set_confused(p_ptr->confused + rand_int(4) + 4);
			}
			if (!p_ptr->free_act || (rand_int(p_ptr->nastytrap57 ? 20 : 100) == 0) )
			{
				if (!p_ptr->resist_nerve || magik(50)) {
					(void)set_paralyzed(p_ptr->paralyzed + rand_int(4) + 4);
				}
			}
			(void)set_slow(p_ptr->slow + rand_int(4) + 4);

			while ((rand_int(100) > player_actual_saving_throw()) && (randint(100) != 1) )
				(void)do_dec_stat(A_INT, STAT_DEC_NORMAL);
			while ((rand_int(100) > player_actual_saving_throw()) && (randint(100) != 1) )
				(void)do_dec_stat(A_WIS, STAT_DEC_NORMAL);

			if (!p_ptr->resist_chaos || p_ptr->nastytrap31 || (rand_int(100) < 5) )
			{
				(void) set_image(p_ptr->image + rand_int(250) + 150);
			}

		}
		break;

		/* Bolt Trap */
	case TRAP_OF_ROCKET:
		ident = player_handle_breath_trap(1, GF_ROCKET, trap);
		break;
	case TRAP_OF_NUKE_BOLT:
		ident = player_handle_breath_trap(1, GF_NUKE, trap);
		break;
	case TRAP_OF_DEATH_RAY:
		ident = player_handle_breath_trap(0, GF_DEATH_RAY, trap);
		break;
	case TRAP_OF_HOLY_FIRE:
		ident = player_handle_breath_trap(1, GF_HOLY_FIRE, trap);
		break;
	case TRAP_OF_HELL_FIRE:
		ident = player_handle_breath_trap(1, GF_HELL_FIRE, trap);
		break;
	case TRAP_OF_PSI_BOLT:
		ident = player_handle_breath_trap(1, GF_PSI, trap);
		break;
	case TRAP_OF_PSI_DRAIN:
		ident = player_handle_breath_trap(1, GF_PSI_DRAIN, trap);
		break;

		/* Ball Trap */
	case TRAP_OF_NUKE_BALL:
		ident = player_handle_breath_trap(3, GF_NUKE, trap);
		break;
	case TRAP_OF_PSI_BALL:
		ident = player_handle_breath_trap(3, GF_PSI, trap);
		break;

	default:
		{
			msg_print(format("Executing unknown trap %d", trap));
		}
	}
	return ident;
}

void player_activate_door_trap(s16b y, s16b x)
{
	cave_type *c_ptr;
	bool ident = FALSE;

	c_ptr = &cave[y][x];

	/* Return if trap or door not found */
	if ((c_ptr->t_idx == 0) ||
	                !(f_info[c_ptr->feat].flags1 & FF1_DOOR)) return;

	/* Disturb */
	disturb(0, 0);

	/* Message */
	msg_print("You found a trap!");

	/* Pick a trap */
	pick_trap(y, x);

	/* Hit the trap */
	ident = player_activate_trap_type(y, x, NULL, -1);
	if (!p_ptr->nastytrap3 && ident)
	{
		t_info[c_ptr->t_idx].ident = TRUE;
		msg_format("You identified that trap as %s.",
		           t_name + t_info[c_ptr->t_idx].name);
	}
}


/*
 * Places a random trap at the given location.
 *
 * The location must be a valid, empty, clean, floor grid.
 */
void place_trap(int y, int x)
{
	s16b trap;
	trap_type *t_ptr;
	int cnt;
	u32b flags;
	cave_type *c_ptr = &cave[y][x];
	dungeon_info_type *d_ptr = &d_info[dungeon_type];
	int effect_level;
	int scalinglevel = 1;

	/* No traps in town or on first level */
	/*if (dun_level <= 1) return;*/
	if (dun_level > 0) effect_level = dun_level;
	else effect_level = wf_info[wild_map[p_ptr->wilderness_y][p_ptr->wilderness_x].feat].level + rand_int(p_ptr->lev);
	if (monster_level > effect_level) effect_level = monster_level;

	if (p_ptr->nastytrap107) effect_level += randint(10 + p_ptr->lev);

	if (randint(50) == 1) scalinglevel = randint(p_ptr->lev);

	if (effect_level < scalinglevel) effect_level = scalinglevel;

	/*msg_format("Trap level %d.", effect_level);*/
	/*msg_format("Monster level %d.", monster_level);*/

	/*
	 * Avoid open doors -- because DOOR flag is added to make much more
	 * important processing faster
	 */
	if (c_ptr->feat == FEAT_OPEN) return;
	if (c_ptr->feat == FEAT_BROKEN) return;

	/* Traps only appears on empty floor */
	if (!cave_floor_grid(c_ptr) &&
	                !(f_info[c_ptr->feat].flags1 & (FF1_DOOR))) return;

	/* Set flags */
	if (f_info[c_ptr->feat].flags1 & FF1_DOOR) flags = FTRAP_DOOR;
	else flags = FTRAP_FLOOR;

	/* Try 10000 times */
	cnt = 10000;
	while (cnt--)
	{
		trap = randint(max_t_idx - 1);
		t_ptr = &t_info[trap];

		/* Este piety reduces spawn rate of nasty traps */
		GOD(GOD_ESTE) {
			if (p_ptr->grace > 15000 && is_nasty_trap(trap)) {
				if (randint(p_ptr->grace) > 15000) continue;
			}
		}

		/* anti-nastiness skill by Amy: reduces odds that a new trap will be a nasty trap */
		int nastyward = get_skill(SKILL_ANTINASTY) * 2;

		if (nastyward > 0) {
			int effnastyward = nastyward;
			if (effnastyward > 80) effnastyward = 80;

			if (is_nasty_trap(trap) && magik(effnastyward)) continue;

			if (nastyward > 80) {
				if (is_nasty_trap(trap) && (randint(nastyward) > 80)) continue;
			}

		}

		/* No traps below their minlevel */
		if (t_ptr->minlevel > effect_level && ( (randint(3) != 1) || (randint(t_ptr->minlevel + 1) > (effect_level + 2) ) ) ) continue;
		if ( (t_ptr->minlevel > (effect_level + 5)) && (randint(t_ptr->minlevel) != 1) ) continue;

		/* is this a correct trap now?   */
		if (!(t_ptr->flags & flags)) continue;

		/*
		 * Hack -- No trap door at the bottom of dungeon or in flat
		 * (non dungeon) places or on quest levels
		 */
		if ((trap == TRAP_OF_SINKING || trap == TRAP_OF_SHAFT || trap == TRAP_OF_DEEP_DESCENT || trap == TRAP_OF_TELE_LEVEL) &&
		    ((d_ptr->maxdepth == dun_level) ||
		     (dungeon_flags1 & DF1_FLAT) || (is_quest(dun_level) && (is_quest(dun_level) != QUEST_RANDOM) )) )
		{
			continue;
		}

		/* How probable is this trap */
		if (rand_int(100) < t_ptr->probability)
		{
			c_ptr->t_idx = trap;
			break;
		}
	}

	return;
}


/*
 * Places a random trap on the given chest.
 *
 * The object must be a valid chest.
 */
void place_trap_object(object_type *o_ptr)
{
	s16b trap;
	trap_type *t_ptr;
	int cnt;

	/* No traps in town or on first level */
	if (dun_level <= 1)
	{
		/* empty chest were already looted, therefore known */
		o_ptr->ident |= IDENT_KNOWN;
		return;
	}

	/* Try 100 times */
	cnt = 100;
	while (cnt--)
	{
		trap = randint(max_t_idx - 1);
		t_ptr = &t_info[trap];

		/* no traps below their minlevel */
		if (t_ptr->minlevel > dun_level) continue;

		/* Is this a correct trap now? */
		if (!(t_ptr->flags & FTRAP_CHEST)) continue;

		/* How probable is this trap */
		if (rand_int(100) < t_ptr->probability)
		{
			o_ptr->pval = trap;
			break;
		}
	}

	return;
}

/* Dangerous trap placing function */
void wiz_place_trap(int y, int x, int idx)
{
	cave_type *c_ptr = &cave[y][x];

	/* Dangerous enough as it is... */
	if (!cave_floor_grid(c_ptr) && (!(f_info[c_ptr->feat].flags1 & FF1_DOOR))) return;

	c_ptr->t_idx = idx;
}

/*
 * Here begin monster traps code
 */

/*
 * Hook to determine if an object is a device
 */
static bool item_tester_hook_device(object_type *o_ptr)
{
	if (((o_ptr->tval == TV_ROD_MAIN) && (o_ptr->pval != 0)) ||
	                (o_ptr->tval == TV_STAFF) ||
	                (o_ptr->tval == TV_WAND)) return (TRUE);

	/* Assume not */
	return (FALSE);
}

/*
 * Hook to determine if an object is a potion
 */
static bool item_tester_hook_potion(object_type *o_ptr)
{
	if ((o_ptr->tval == TV_POTION) ||
	                (o_ptr->tval == TV_POTION2)) return (TRUE);

	/* Assume not */
	return (FALSE);
}

/*
 * The trap setting code for rogues -MWK-
 *
 * Also, it will fail or give weird results if the tvals are resorted!
 */
void do_cmd_set_trap(void)
{
	int item_kit, item_load, i;
	int num;

	object_type *o_ptr, *j_ptr, *i_ptr;

	cptr q, s, c;

	object_type object_type_body;

	u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

	/* Check some conditions */
	if (p_ptr->blind)
	{
		msg_print("You can't see anything.");
		msg_print(NULL);
		return;
	}
	if (no_lite())
	{
		msg_print("You don't dare to set a trap in the darkness.");
		msg_print(NULL);
		return;
	}
	if (p_ptr->confused)
	{
		msg_print("You are too confused!");
		msg_print(NULL);
		return;
	}

	/* Only set traps on clean floor grids */
	if (!cave_clean_bold(p_ptr->py, p_ptr->px))
	{
		msg_print("You cannot set a trap on this.");
		msg_print(NULL);
		return;
	}

	/* Restrict choices to trapkits */
	item_tester_tval = TV_TRAPKIT;

	/* Get an item */
	q = "Use which trapping kit? ";
	s = "You have no trapping kits. There needs to be a very looooooong message now so that your next keystroke isn't misinterpreted as something you didn't want to do at all, so as to force a --More--.";
	if (!get_item(&item_kit, q, s, USE_INVEN)) return;

	o_ptr = &p_ptr->inventory[item_kit];

	/* Trap kits need a second object */
	switch (o_ptr->sval)
	{
	case SV_TRAPKIT_BOW:
		item_tester_tval = TV_ARROW;
		break;
	case SV_TRAPKIT_XBOW:
		item_tester_tval = TV_BOLT;
		break;
	case SV_TRAPKIT_SLING:
		item_tester_tval = TV_SHOT;
		break;
	case SV_TRAPKIT_POTION:
		item_tester_hook = item_tester_hook_potion;
		break;
	case SV_TRAPKIT_SCROLL:
		item_tester_tval = TV_SCROLL;
		break;
	case SV_TRAPKIT_DEVICE:
		item_tester_hook = item_tester_hook_device;
		break;
	default:
		msg_print("Unknown trapping kit type!");
		break;
	}

	/* Get the second item */
	q = "Load with what? ";
	s = "You have nothing to load that trap with. There needs to be a very looooooong message now so that your next keystroke isn't misinterpreted as something you didn't want to do at all, so as to force a --More--.";
	if (!get_item(&item_load, q, s, USE_INVEN)) return;

	/* Get the second object */
	j_ptr = &p_ptr->inventory[item_load];

	/* Assume a single object */
	num = 1;

	/* In some cases, take multiple objects to load */
	if (o_ptr->sval != SV_TRAPKIT_DEVICE)
	{
		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

		if ((f3 & TR3_XTRA_SHOTS) && (o_ptr->pval > 0)) num += o_ptr->pval;

		if (f2 & (TRAP2_AUTOMATIC_5 | TRAP2_AUTOMATIC_99)) num = 99;

		if (num > j_ptr->number) num = j_ptr->number;

		c = format("How many (1-%d)? ", num);

		/* Ask for number of items to use */
		num = get_quantity(c, num);
	}

	/* Canceled */
	if (!num) return;

	/* Take a turn */
	energy_use = 100;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Obtain local object for trap content */
	object_copy(i_ptr, j_ptr);

	/* Set number */
	i_ptr->number = num;

	/* Drop it here */
	cave[p_ptr->py][p_ptr->px].special = floor_carry(p_ptr->py, p_ptr->px, i_ptr);

	/* Obtain local object for trap trigger kit */
	object_copy(i_ptr, o_ptr);

	/* Set number */
	i_ptr->number = 1;

	/* Drop it here */
	cave[p_ptr->py][p_ptr->px].special2 = floor_carry(p_ptr->py, p_ptr->px, i_ptr);

	/* Modify, Describe, Optimize */
	inven_item_increase(item_kit, -1);
	inven_item_describe(item_kit);
	inven_item_increase(item_load, -num);
	inven_item_describe(item_load);

	for (i = 0; i < INVEN_WIELD; i++)
	{
		if (inven_item_optimize(i)) break;
	}
	for (i = 0; i < INVEN_WIELD; i++)
	{
		inven_item_optimize(i);
	}

	/* Actually set the trap */
	cave_set_feat(p_ptr->py, p_ptr->px, FEAT_MON_TRAP);
}

/*
 * Monster hitting a rod trap -MWK-
 *
 * Return TRUE if the monster died
 */
bool mon_hit_trap_aux_rod(int m_idx, object_type *o_ptr)
{
	int dam = 0, typ = 0;
	int rad = 0;
	monster_type *m_ptr = &m_list[m_idx];
	int y = m_ptr->fy;
	int x = m_ptr->fx;

	/* Depend on rod type */
	switch (o_ptr->pval)
	{
	case SV_ROD_DETECT_TRAP:
		m_ptr->smart |= SM_NOTE_TRAP;
		break;
	case SV_ROD_DETECTION:
		m_ptr->smart |= SM_NOTE_TRAP;
		break;
	case SV_ROD_ILLUMINATION:
		typ = GF_LITE_WEAK;
		dam = damroll(2, 15);
		rad = 3;
		lite_room(y, x);
		break;
	case SV_ROD_CURING:
		typ = GF_OLD_HEAL;
		dam = damroll(3, 4);  /* and heal conf? */
		break;
	case SV_ROD_HEALING:
		typ = GF_OLD_HEAL;
		dam = 300;
		break;
	case SV_ROD_SPEED:
		typ = GF_OLD_SPEED;
		dam = 50;
		break;
	case SV_ROD_TELEPORT_AWAY:
		typ = GF_AWAY_ALL;
		dam = MAX_SIGHT * 5;
		break;
	case SV_ROD_DISARMING:
		break;
	case SV_ROD_LITE:
		typ = GF_LITE_WEAK;
		dam = damroll(6, 8);
		break;
	case SV_ROD_SLEEP_MONSTER:
		typ = GF_OLD_SLEEP;
		dam = 50;
		break;
	case SV_ROD_SLOW_MONSTER:
		typ = GF_OLD_SLOW;
		dam = 50;
		break;
	case SV_ROD_DRAIN_LIFE:
		typ = GF_OLD_DRAIN;
		dam = 75;
		break;
	case SV_ROD_POLYMORPH:
		typ = GF_OLD_POLY;
		dam = 50;
		break;
	case SV_ROD_ACID_BOLT:
		typ = GF_ACID;
		dam = damroll(6, 8);
		break;
	case SV_ROD_ELEC_BOLT:
		typ = GF_ELEC;
		dam = damroll(3, 8);
		break;
	case SV_ROD_FIRE_BOLT:
		typ = GF_FIRE;
		dam = damroll(8, 8);
		break;
	case SV_ROD_COLD_BOLT:
		typ = GF_COLD;
		dam = damroll(5, 8);
		break;
	case SV_ROD_ACID_BALL:
		typ = GF_ACID;
		dam = 60;
		rad = 2;
		break;
	case SV_ROD_ELEC_BALL:
		typ = GF_ELEC;
		dam = 32;
		rad = 2;
		break;
	case SV_ROD_FIRE_BALL:
		typ = GF_FIRE;
		dam = 72;
		rad = 2;
		break;
	case SV_ROD_COLD_BALL:
		typ = GF_COLD;
		dam = 48;
		rad = 2;
		break;
	default:
		return (FALSE);
	}

	/* Actually hit the monster */
	if (typ) (void) project( -2, rad, y, x, dam, typ, PROJECT_KILL | PROJECT_ITEM | PROJECT_JUMP);
	return (cave[y][x].m_idx == 0 ? TRUE : FALSE);
}

/*
 * Monster hitting a staff trap -MWK-
 *
 * Return TRUE if the monster died
 */
bool mon_hit_trap_aux_staff(int m_idx, object_type *o_ptr)
{
	/* Monster pointer and position */
	monster_type *m_ptr = &m_list[m_idx];
	int y = m_ptr->fy;
	int x = m_ptr->fx;
	int k; /* because some turbo wierdo commented out all the item code below --Amy */
	int level = exec_lua("return get_level(o_ptr->pval2, 50, 0)");

	/* sval and base level of the staff */
	int sval = o_ptr->sval;

	/* Damage amount, type, and radius */
	int dam = 0, typ = 0;
	int rad = 0;

	/* Depend on staff type */
	switch (sval)
	{
	case SV_STAFF_IDENTIFY:
	case SV_STAFF_MANA:
	case SV_STAFF_REMOVE_CURSES:
	case SV_STAFF_REVEAL_WAYS:
	case SV_STAFF_SENSE_MONSTER:
	case SV_STAFF_VISION:
	case SV_STAFF_DISARM:
		return (FALSE);

	case SV_STAFF_LIGHT:
		lite_room(y, x);
		typ = GF_LITE_WEAK;
		dam = damroll(2, 8);
		rad = 2;
		break;

	case SV_STAFF_SUMMON:
		for (k = 0; k < randint(4) ; k++)
			(void)summon_specific(y, x, dun_level, 0);
		return (FALSE);

	case SV_STAFF_TELEPORTATION:
		typ = GF_AWAY_ALL;
		dam = 100 + 2 * level;
		break;

	case SV_STAFF_HEALING:
		typ = GF_OLD_HEAL;
		dam = m_ptr->maxhp * (150 + 7 * level) / 1000;
		break;

	case SV_STAFF_SHAKE:
		earthquake(y, x, 4 + level / 5);  /* was 10 */
		return (FALSE);

	case SV_STAFF_RECOVERY:
		m_ptr->bleeding = 0;
		m_ptr->poisoned = 0;
		return (FALSE);

	case SV_STAFF_GENOCIDE:
		{
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			genocide_aux(FALSE, r_ptr->d_char);
			/* although there's no point in a multiple genocide trap... */
			return (cave[y][x].m_idx == 0 ? TRUE : FALSE);
		}

	case SV_STAFF_SENSE_HIDDEN:
		m_ptr->smart |= SM_NOTE_TRAP;
		return (FALSE);

	case SV_STAFF_WISH:
		acquirement(y, x, randint(2) + 1, TRUE, FALSE);
		return (FALSE);

	case SV_STAFF_MITHRANDIR:
		typ = GF_HOLY_FIRE;
		dam = 50 + 6 * level;
		rad = 9;  /* instead of LOS */

		/* How to implement these ? */
	case SV_STAFF_FIERY_SHIELD:
	case SV_STAFF_WINGS_WIND:
	case SV_STAFF_PROBABILITY_TRAVEL:

	default:
		return (FALSE);
	}

	/* Actually hit the monster */
	(void) project( -2, rad, y, x, dam, typ, PROJECT_KILL | PROJECT_ITEM | PROJECT_JUMP);
	return (cave[y][x].m_idx == 0 ? TRUE : FALSE);
}

/*
 * Monster hitting a scroll trap -MWK-
 *
 * Return TRUE if the monster died
 */
bool mon_hit_trap_aux_scroll(int m_idx, int sval)
{
	monster_type *m_ptr = &m_list[m_idx];
	int dam = 0, typ = 0;
	int rad = 0;
	int y = m_ptr->fy;
	int x = m_ptr->fx;
	int k;

	/* Depend on scroll type */
	switch (sval)
	{
	case SV_SCROLL_CURSE_ARMOR:
	case SV_SCROLL_CURSE_WEAPON:
	case SV_SCROLL_TRAP_CREATION:  /* these don't work :-( */
	case SV_SCROLL_WORD_OF_RECALL:  /* should these? */
	case SV_SCROLL_IDENTIFY:
	case SV_SCROLL_STAR_IDENTIFY:
	case SV_SCROLL_MAPPING:
	case SV_SCROLL_DETECT_GOLD:
	case SV_SCROLL_DETECT_ITEM:
	case SV_SCROLL_REMOVE_CURSE:
	case SV_SCROLL_STAR_REMOVE_CURSE:
	case SV_SCROLL_ENCHANT_ARMOR:
	case SV_SCROLL_ENCHANT_WEAPON_TO_HIT:
	case SV_SCROLL_ENCHANT_WEAPON_TO_DAM:
	case SV_SCROLL_STAR_ENCHANT_ARMOR:
	case SV_SCROLL_STAR_ENCHANT_WEAPON:
	case SV_SCROLL_RECHARGING:
	case SV_SCROLL_DETECT_DOOR:
	case SV_SCROLL_DETECT_INVIS:
	case SV_SCROLL_SATISFY_HUNGER:
	case SV_SCROLL_RUNE_OF_PROTECTION:
	case SV_SCROLL_TRAP_DOOR_DESTRUCTION:
	case SV_SCROLL_PROTECTION_FROM_EVIL:
		return (FALSE);
	case SV_SCROLL_DARKNESS:
		unlite_room(y, x);
		typ = GF_DARK_WEAK;
		dam = 10;
		rad = 3;
		break;
	case SV_SCROLL_AGGRAVATE_MONSTER:
		aggravate_monsters(m_idx);
		return (FALSE);
	case SV_SCROLL_SUMMON_MONSTER:
		for (k = 0; k < randint(3) ; k++) summon_specific(y, x, dun_level, 0);
		return (FALSE);
	case SV_SCROLL_SUMMON_UNDEAD:
		for (k = 0; k < randint(3) ; k++) summon_specific(y, x, dun_level, SUMMON_UNDEAD);
		return (FALSE);
	case SV_SCROLL_PHASE_DOOR:
		typ = GF_AWAY_ALL;
		dam = 10;
		break;
	case SV_SCROLL_TELEPORT:
		typ = GF_AWAY_ALL;
		dam = 100;
		break;
	case SV_SCROLL_TELEPORT_LEVEL:
		delete_monster(y, x);
		return (TRUE);
	case SV_SCROLL_LIGHT:
		lite_room(y, x);
		typ = GF_LITE_WEAK;
		dam = damroll(2, 8);
		rad = 2;
		break;
	case SV_SCROLL_DETECT_TRAP:
		m_ptr->smart |= SM_NOTE_TRAP;
		return (FALSE);
	case SV_SCROLL_BLESSING:
		typ = GF_HOLY_FIRE;
		dam = damroll(1, 4);
		break;
	case SV_SCROLL_HOLY_CHANT:
		typ = GF_HOLY_FIRE;
		dam = damroll(2, 4);
		break;
	case SV_SCROLL_HOLY_PRAYER:
		typ = GF_HOLY_FIRE;
		dam = damroll(4, 4);
		break;
	case SV_SCROLL_MONSTER_CONFUSION:
		typ = GF_OLD_CONF;
		dam = damroll(5, 10);
		break;
	case SV_SCROLL_STAR_DESTRUCTION:
		destroy_area(y, x, 15, TRUE, FALSE);
		return (FALSE);
	case SV_SCROLL_DESTRUCTION:
		destroy_area(y, x, 5, TRUE, FALSE);
		return (FALSE);
	case SV_SCROLL_DISPEL_UNDEAD:
		typ = GF_DISP_UNDEAD;
		rad = 5;
		dam = 60;
		break;
	case SV_SCROLL_GENOCIDE:
		{
			monster_race *r_ptr = &r_info[m_ptr->r_idx];
			genocide_aux(FALSE, r_ptr->d_char);
			/* although there's no point in a multiple genocide trap... */
			return (!(r_ptr->flags1 & RF1_UNIQUE));
		}
	case SV_SCROLL_MASS_GENOCIDE:
		for (k = 0; k < 8; k++)
			delete_monster(y + ddy[k], x + ddx[k]);
		delete_monster(y, x);
		return (TRUE);
	case SV_SCROLL_ACQUIREMENT:
		acquirement(y, x, 1, TRUE, FALSE);
		return (FALSE);
	case SV_SCROLL_STAR_ACQUIREMENT:
		acquirement(y, x, randint(2) + 1, TRUE, FALSE);
		return (FALSE);
	default:
		return (FALSE);
	}

	/* Actually hit the monster */
	(void) project( -2, rad, y, x, dam, typ, PROJECT_KILL | PROJECT_ITEM | PROJECT_JUMP);
	return (cave[y][x].m_idx == 0 ? TRUE : FALSE);
}

/*
 * Monster hitting a wand trap -MWK-
 *
 * Return TRUE if the monster died
 */
bool mon_hit_trap_aux_wand(int m_idx, object_type *o_ptr)
{
	/* Monster pointer and position */
	monster_type *m_ptr = &m_list[m_idx];
	int y = m_ptr->fy;
	int x = m_ptr->fx;

	int level = exec_lua("return get_level(o_ptr->pval2, 50, 0)");

	/* sval and bonus level of the wand */
	int sval = o_ptr->sval;

	/* Damage amount, type, and radius */
	int dam = 0, typ = 0;
	int rad = 0;

	/* Depend on wand type */
	switch (sval)
	{

	case SV_WAND_MANATHRUST:
		typ = GF_MANA;
		dam = damroll(3 + level, 1 + 2 * level / 5 );
		break;

	case SV_WAND_FIREFLASH:
		typ = GF_FIRE;
		dam = 20 + level * 10;
		rad = 2 + level / 10;
		break;

	case SV_WAND_NOXIOUS_CLOUD:
		typ = GF_POIS;
		dam = 7 + 3 * level;
		rad = 3;
		break;

	case SV_WAND_THUNDERSTORM:
		typ = GF_ELEC;  /* GF_LITE, GF_SOUND ??? */
		dam = damroll(5 + level / 5, 10 + level / 2);
		break;

	case SV_WAND_DIG:
	case SV_WAND_THRAIN:
		typ = GF_KILL_WALL;
		dam = 20 + randint(30);
		break;

	case SV_WAND_STRIKE:
		typ = GF_FORCE;
		dam = 50 + level;
		break;

	case SV_WAND_TELEPORT_AWAY:
	case SV_WAND_HOME_SUMMONING:
		typ = GF_AWAY_ALL;
		dam = MAX_SIGHT * 5;
		break;

	case SV_WAND_SUMMON_ANIMAL:
		summon_specific(y, x, dun_level, SUMMON_ANIMAL);  /* friendly ?*/
		return (FALSE);

	case SV_WAND_SLOW_MONSTER:
		typ = GF_OLD_SLOW;
		dam = 40 + 16 * level / 5;
		break;

	case SV_WAND_BANISHMENT:
		typ = GF_AWAY_ALL;
		dam = 40 + 16 * level / 5;
		rad = 9;  /* instead of LOS */
		break;

	case SV_WAND_CHARM:
		typ = GF_CHARM;
		dam = 10 + 3 * level;
		break;

	case SV_WAND_CONFUSE:
		typ = GF_OLD_CONF;
		dam = 10 + 3 * level;
		break;

	case SV_WAND_HEAL_MONSTER:
		typ = GF_OLD_HEAL;
		dam = 20 + 38 * level / 5;
		break;

	case SV_WAND_SPEED:
	case SV_WAND_HASTE_MONSTER:
		typ = GF_OLD_SPEED;
		dam = damroll(5, 10);
		break;

	case SV_WAND_STONE_PRISON:
		wall_stone(y, x);
		return (FALSE);

	case SV_WAND_DISPERSE_MAGIC:
		m_ptr->confused = 0;
		m_ptr->mspeed = 0;
		return (FALSE);

	case SV_WAND_ICE_STORM:
		typ = GF_COLD;
		dam = 80 + 4 * level;
		rad = 1 + 3 * level / 50;
		break;

	case SV_WAND_TIDAL_WAVE:
		typ = GF_WAVE;
		dam = 40 + 4 * level;
		rad = 6 + level / 5;
		break;

	case SV_WAND_FIREWALL:
		typ = GF_FIRE;
		dam = 40 + 3 * level;
		rad = 1;  /*instead of beam*/

		/* Not sure about these */
	case SV_WAND_MAGELOCK:
	case SV_WAND_DEMON_BLADE:
	case SV_WAND_POISON_BLOOD:

	default:
		return (FALSE);
	}

	/* Actually hit the monster */
	(void) project( -2, rad, y, x, dam, typ, PROJECT_KILL | PROJECT_ITEM | PROJECT_JUMP);
	return (cave[y][x].m_idx == 0 ? TRUE : FALSE);
}

/*
 * Monster hitting a potions trap -MWK-
 *
 * Return TRUE if the monster died
 */
bool mon_hit_trap_aux_potion(int m_idx, object_type *o_ptr)
{
	monster_type *m_ptr = &m_list[m_idx];
	int dam = 0, typ = 0;
	int y = m_ptr->fy;
	int x = m_ptr->fx;
	int sval = o_ptr->sval;

	/* Depend on potion type */
	if (o_ptr->tval == TV_POTION)
	{
		switch (sval)
		{
			/* Nothing happens */
		case SV_POTION_WATER:
		case SV_POTION_APPLE_JUICE:
		case SV_POTION_SLIME_MOLD:
		case SV_POTION_SALT_WATER:
		case SV_POTION_DEC_STR:
		case SV_POTION_DEC_INT:
		case SV_POTION_DEC_WIS:
		case SV_POTION_DEC_DEX:
		case SV_POTION_DEC_CON:
		case SV_POTION_DEC_CHR:
		case SV_POTION_INFRAVISION:
		case SV_POTION_DETECT_INVIS:
		case SV_POTION_SLOW_POISON:
		case SV_POTION_CURE_POISON:
		case SV_POTION_RESIST_HEAT:
		case SV_POTION_RESIST_COLD:
		case SV_POTION_RESTORE_MANA:
		case SV_POTION_RESTORE_EXP:
		case SV_POTION_RES_STR:
		case SV_POTION_RES_INT:
		case SV_POTION_RES_WIS:
		case SV_POTION_RES_DEX:
		case SV_POTION_RES_CON:
		case SV_POTION_RES_CHR:
		case SV_POTION_INC_STR:
		case SV_POTION_INC_INT:
		case SV_POTION_INC_WIS:
		case SV_POTION_INC_DEX:
		case SV_POTION_INC_CON:
		case SV_POTION_INC_CHR:
		case SV_POTION_AUGMENTATION:
		case SV_POTION_RUINATION: 	/* ??? */
		case SV_POTION_ENLIGHTENMENT:
		case SV_POTION_STAR_ENLIGHTENMENT:
		case SV_POTION_SELF_KNOWLEDGE:
			return (FALSE);

		case SV_POTION_EXPERIENCE:
			if (m_ptr->level < MONSTER_LEVEL_MAX)
			{
				m_ptr->exp = MONSTER_EXP(m_ptr->level + 1);
				monster_check_experience(m_idx, FALSE);
			}
			return (FALSE);
		case SV_POTION_SLOWNESS:
			typ = GF_OLD_SLOW;
			dam = damroll(4, 6);
			break;
		case SV_POTION_POISON:
			typ = GF_POIS;
			dam = damroll(8, 6);
			break;
		case SV_POTION_CONFUSION:
			typ = GF_CONFUSION;
			dam = damroll(4, 6);
			break;
		case SV_POTION_BLINDNESS:
			typ = GF_DARK;
			dam = 10;
			break;
		case SV_POTION_SLEEP:
			typ = GF_OLD_SLEEP;
			dam = damroll (4, 6);
			break;
		case SV_POTION_LOSE_MEMORIES:
			typ = GF_OLD_CONF;
			dam = damroll(10, 10);
			break;
		case SV_POTION_DETONATIONS:
			typ = GF_DISINTEGRATE;
			dam = damroll(20, 20);
			break;
		case SV_POTION_DEATH:
			typ = GF_NETHER;
			dam = damroll(100, 20);
			break;
		case SV_POTION_BOLDNESS:
			m_ptr->monfear = 0;
			return (FALSE);
		case SV_POTION_SPEED:
			typ = GF_OLD_SPEED;
			dam = damroll(5, 10);
			break;
		case SV_POTION_HEROISM:
		case SV_POTION_BESERK_STRENGTH:
			m_ptr->monfear = 0;
			typ = GF_OLD_HEAL;
			dam = damroll(2, 10);
			break;
		case SV_POTION_CURE_LIGHT:
			typ = GF_OLD_HEAL;
			dam = damroll(3, 4);
			break;
		case SV_POTION_CURE_SERIOUS:
			typ = GF_OLD_HEAL;
			dam = damroll(4, 6);
			break;
		case SV_POTION_CURE_CRITICAL:
			typ = GF_OLD_HEAL;
			dam = damroll(6, 8);
			break;
		case SV_POTION_HEALING:
			typ = GF_OLD_HEAL;
			dam = 300;
			break;
		case SV_POTION_STAR_HEALING:
			typ = GF_OLD_HEAL;
			dam = 1000;
			break;
		case SV_POTION_LIFE:
			{
				monster_race *r_ptr = &r_info[m_ptr->r_idx];
				if (r_ptr->flags3 & RF3_UNDEAD)
				{
					typ = GF_HOLY_FIRE;
					dam = damroll(20, 20);
				}
				else
				{
					typ = GF_OLD_HEAL;
					dam = 5000;
				}
				break;
			}
		default:
			return (FALSE);

		}
	}
	else
	{}

	/* Actually hit the monster */
	(void) project_m( -2, 0, y, x, dam, typ);
	return (cave[y][x].m_idx == 0 ? TRUE : FALSE);
}

/*
 * Monster hitting a monster trap -MWK-
 * Returns True if the monster died, false otherwise
 */
bool mon_hit_trap(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
#if 0 /* DGDGDGDG */
	monster_lore *l_ptr = &l_list[m_ptr->r_idx];
#endif

	object_type *kit_o_ptr, *load_o_ptr, *j_ptr;

	u32b f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, esp;

	object_type object_type_body;

	int mx = m_ptr->fx;
	int my = m_ptr->fy;

	int difficulty;
	int smartness;

	char m_name[80];

	bool notice = FALSE;
	bool disarm = FALSE;
	bool remove = FALSE;
	bool dead = FALSE;
	bool fear = FALSE;
	s32b special = 0;

	int dam, chance, shots;
	int mul = 0;
	int breakage = -1;

	int cost = 0;

	/* Get the trap objects */
	kit_o_ptr = &o_list[cave[my][mx].special2];
	load_o_ptr = &o_list[cave[my][mx].special];
	j_ptr = &object_type_body;

	/* Get trap properties */
	object_flags(kit_o_ptr, &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &esp);

	/* Can set off check */
	/* Ghosts only set off Ghost traps */
	if ((r_ptr->flags2 & RF2_PASS_WALL) && !(f2 & TRAP2_KILL_GHOST)) return (FALSE);

	/* Some traps are specialized to some creatures */
	if (f2 & TRAP2_ONLY_MASK)
	{
		bool affect = FALSE;
		if ((f2 & TRAP2_ONLY_DRAGON) && (r_ptr->flags3 & RF3_DRAGON)) affect = TRUE;
		if ((f2 & TRAP2_ONLY_DEMON) && (r_ptr->flags3 & RF3_DEMON)) affect = TRUE;
		if ((f2 & TRAP2_ONLY_UNDEAD) && (r_ptr->flags3 & RF3_UNDEAD)) affect = TRUE;
		if ((f2 & TRAP2_ONLY_EVIL) && (r_ptr->flags3 & RF3_EVIL)) affect = TRUE;
		if ((f2 & TRAP2_ONLY_ANIMAL) && (r_ptr->flags3 & RF3_ANIMAL)) affect = TRUE;

		/* Don't set it off if forbidden */
		if (!affect) return (FALSE);
	}

	/* Get detection difficulty */
	difficulty = 25;

	/* Some traps are well-hidden */
	if (f1 & TR1_STEALTH)
	{
		difficulty += 10 * (kit_o_ptr->pval);
	}

	/* Get monster smartness for trap detection */
	/* Higher level monsters are smarter */
	smartness = r_ptr->level;

	/* Smart monsters are better at detecting traps */
	if (r_ptr->flags2 & RF2_SMART) smartness += 10;

	/* Some monsters are great at detecting traps */
#if 0 /* DGDGDGDG */
	if (r_ptr->flags2 & RF2_NOTICE_TRAP) smartness += 20;
#endif
	/* Some monsters have already noticed one of out traps */
	if (m_ptr->smart & SM_NOTE_TRAP) smartness += 20;

	/* Stupid monsters are no good at detecting traps */
	if (r_ptr->flags2 & (RF2_STUPID | RF2_EMPTY_MIND)) smartness = -150;

	/* Check if the monster notices the trap */
	if (randint(300) > (difficulty - smartness + 150)) notice = TRUE;

	/* Disarm check */
	if (notice)
	{
		/* The next traps will be easier to spot! */
		m_ptr->smart |= SM_NOTE_TRAP;

		/* Tell the player about it */
#if 0 /* DGDGDGDG */
		if (m_ptr->ml) l_ptr->r_flags2 |= (RF2_NOTICE_TRAP & r_ptr->flags2);
#endif
		/* Get trap disarming difficulty */
		difficulty = (kit_o_ptr->ac + kit_o_ptr->to_a);

		/* Get monster disarming ability */
		/* Higher level monsters are better */
		smartness = r_ptr->level / 5;

		/* Some monsters are great at disarming */
#if 0 /* DGDGDGDG */
		if (r_ptr->flags2 & RF2_DISARM_TRAP) smartness += 20;
#endif
		/* After disarming one trap, the next is easier */
#if 0 /* DGDGDGDG */
		if (m_ptr->status & STATUS_DISARM_TRAP) smartness += 20;
#endif
		/* Smart monsters are better at disarming */
		if (r_ptr->flags2 & RF2_SMART) smartness *= 2;

		/* Stupid monsters never disarm traps */
		if (r_ptr->flags2 & RF2_STUPID) smartness = -150;

		/* Nonsmart animals never disarm traps */
		if ((r_ptr->flags3 & RF3_ANIMAL) && !(r_ptr->flags2 & RF2_SMART)) smartness = -150;

		/* Check if the monster disarms the trap */
		if (randint(120) > (difficulty - smartness + 80)) disarm = TRUE;
	}

	/* If disarmed, remove the trap and print a message */
	if (disarm)
	{
		remove = TRUE;

		/* Next time disarming will be easier */
#if 0 /* DGDGDGDG */
		m_ptr->status |= STATUS_DISARM_TRAP;
#endif
		if (m_ptr->ml)
		{
			/* Get the name */
			monster_desc(m_name, m_ptr, 0);

			/* Tell the player about it */
#if 0 /* DGDGDGDG */
			l_ptr->r_flags2 |= (RF2_DISARM_TRAP & r_ptr->flags2);
#endif
			/* Print a message */
			msg_format("%^s disarms a trap!", m_name);
		}
	}

	/* Otherwise, activate the trap! */
	else
	{
		/* Message for visible monster */
		if (m_ptr->ml)
		{
			/* Get the name */
			monster_desc(m_name, m_ptr, 0);

			/* Print a message */
			msg_format("%^s sets off a trap!", m_name);
		}
		else
		{
			/* No message if monster isn't visible ? */
		}

		/* Next time be more careful */
		if (randint(100) < 80) m_ptr->smart |= SM_NOTE_TRAP;

		/* Actually activate the trap */
		switch (kit_o_ptr->sval)
		{
		case SV_TRAPKIT_BOW:
		case SV_TRAPKIT_XBOW:
		case SV_TRAPKIT_SLING:
			{
				/* Get number of shots */
				shots = 1;
				if (f3 & TR3_XTRA_SHOTS) shots += kit_o_ptr->pval;
				if (shots <= 0) shots = 1;
				if (shots > load_o_ptr->number) shots = load_o_ptr->number;

				while (shots-- && !dead)
				{
					/* Total base damage */
					dam = damroll(load_o_ptr->dd, load_o_ptr->ds) + load_o_ptr->to_d + kit_o_ptr->to_d;

					/* Total hit probability */
					chance = (kit_o_ptr->to_h + load_o_ptr->to_h + 20) * BTH_PLUS_ADJ;

					/* Damage multiplier */
					if (kit_o_ptr->sval == SV_TRAPKIT_BOW) mul = 3;
					if (kit_o_ptr->sval == SV_TRAPKIT_XBOW) mul = 4;
					if (kit_o_ptr->sval == SV_TRAPKIT_SLING) mul = 2;
					if (f3 & TR3_XTRA_MIGHT) mul += kit_o_ptr->pval;
					if (mul < 0) mul = 0;

					/* Multiply damage */
					dam *= mul;

					/* Check if we hit the monster */
					if (test_hit_fire(chance, r_ptr->ac, TRUE))
					{
						/* Assume a default death */
						cptr note_dies = " dies.";

						/* Some monsters get "destroyed" */
						if ((r_ptr->flags3 & (RF3_DEMON)) ||
						                (r_ptr->flags3 & (RF3_UNDEAD)) ||
						                (r_ptr->flags2 & (RF2_STUPID)) ||
						                (strchr("Evg", r_ptr->d_char)))
						{
							/* Special note at death */
							note_dies = " is destroyed.";
						}

						/* Message if visible */
						if (m_ptr->ml)
						{
							/* describe the monster (again, just in case :-) */
							monster_desc(m_name, m_ptr, 0);

							/* Print a message */
							msg_format("%^s is hit by a missile.", m_name);
						}

						/* Apply slays, brand, critical hits */
						dam = tot_dam_aux(load_o_ptr, dam, m_ptr, &special);
						dam = critical_shot(load_o_ptr->weight, load_o_ptr->to_h, dam, SKILL_ARCHERY);

						/* No negative damage */
						if (dam < 0) dam = 0;

						/* Hit the monster, check for death */
						if (mon_take_hit(m_idx, dam, &fear, note_dies))
						{
							/* Dead monster */
							dead = TRUE;
						}

						/* No death */
						else
						{
							/* Message */
							message_pain(m_idx, dam);

							if (special) attack_special(m_ptr, special, dam);

							/* Take note */
							if (fear && m_ptr->ml)
							{
								/* Message */
								msg_format("%^s flees in terror!", m_name);
							}
						}

					}

					/* Exploding ammo */
					if (load_o_ptr->pval2 != 0)
					{
						int rad = 0;
						int dam = (damroll(load_o_ptr->dd, load_o_ptr->ds) + load_o_ptr->to_d)*2;
						int flag = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL |
						           PROJECT_JUMP;

						switch (load_o_ptr->sval)
						{
						case SV_AMMO_LIGHT:
							rad = 2;
							dam /= 2;
							break;
						case SV_AMMO_NORMAL:
							rad = 3;
							break;
						case SV_AMMO_HEAVY:
							rad = 4;
							dam *= 2;
							break;
						}

						project(0, rad, my, mx, dam, load_o_ptr->pval2, flag);

						breakage = 100;
					}
					else
					{
						breakage = breakage_chance(load_o_ptr);
					}

					/* Copy and decrease ammo */
					object_copy(j_ptr, load_o_ptr);

					j_ptr->number = 1;

					load_o_ptr->number--;

					if (load_o_ptr->number <= 0)
					{
						remove = TRUE;
						delete_object_idx(kit_o_ptr->next_o_idx);
						kit_o_ptr->next_o_idx = 0;
					}

					/* Drop (or break) near that location */
					drop_near(j_ptr, breakage, my, mx);

				}

				break;
			}

		case SV_TRAPKIT_POTION:
			{
				/* Get number of shots */
				shots = 1;
				if (f3 & TR3_XTRA_SHOTS) shots += kit_o_ptr->pval;
				if (shots <= 0) shots = 1;
				if (shots > load_o_ptr->number) shots = load_o_ptr->number;

				while (shots-- && !dead)
				{

					/* Message if visible */
					if (m_ptr->ml)
					{
						/* describe the monster (again, just in case :-) */
						monster_desc(m_name, m_ptr, 0);

						/* Print a message */
						msg_format("%^s is hit by fumes.", m_name);
					}

					/* Get the potion effect */
					dead = mon_hit_trap_aux_potion(m_idx, load_o_ptr);

					/* Copy and decrease ammo */
					object_copy(j_ptr, load_o_ptr);

					j_ptr->number = 1;

					load_o_ptr->number--;

					if (load_o_ptr->number <= 0)
					{
						remove = TRUE;
						delete_object_idx(kit_o_ptr->next_o_idx);
						kit_o_ptr->next_o_idx = 0;
					}
				}

				break;
			}

		case SV_TRAPKIT_SCROLL:
			{
				/* Get number of shots */
				shots = 1;
				if (f3 & TR3_XTRA_SHOTS) shots += kit_o_ptr->pval;
				if (shots <= 0) shots = 1;
				if (shots > load_o_ptr->number) shots = load_o_ptr->number;

				while (shots-- && !dead)
				{

					/* Message if visible */
					if (m_ptr->ml)
					{
						/* describe the monster (again, just in case :-) */
						monster_desc(m_name, m_ptr, 0);

						/* Print a message */
						msg_format("%^s activates a spell!", m_name);
					}

					/* Get the potion effect */
					dead = mon_hit_trap_aux_scroll(m_idx, load_o_ptr->sval);

					/* Copy and decrease ammo */
					object_copy(j_ptr, load_o_ptr);

					j_ptr->number = 1;

					load_o_ptr->number--;

					if (load_o_ptr->number <= 0)
					{
						remove = TRUE;
						delete_object_idx(kit_o_ptr->next_o_idx);
						kit_o_ptr->next_o_idx = 0;
					}
				}

				break;
			}

		case SV_TRAPKIT_DEVICE:
			{
				if (load_o_ptr->tval == TV_ROD_MAIN)
				{
					/* Extract mana cost of the rod tip */
					u32b tf1, tf2, tf3, tf4, tf5, tf6, tf7, tf8, tf9, tf10, tesp;
					object_kind *tip_o_ptr = &k_info[lookup_kind(TV_ROD, load_o_ptr->pval)];
					object_flags(load_o_ptr, &tf1, &tf2, &tf3, &tf4, &tf5, &tf6, &tf7, &tf8, &tf9, &tf10, &tesp);
					cost = (tf4 & TR4_CHEAPNESS) ? tip_o_ptr->pval / 2 : tip_o_ptr->pval;
					if (cost <= 0) cost = 1;
				}

				/* Get number of shots */
				shots = 1;
				if (f3 & TR3_XTRA_SHOTS) shots += kit_o_ptr->pval;
				if (shots <= 0) shots = 1;

				if (load_o_ptr->tval == TV_ROD_MAIN)
				{
					if (shots > load_o_ptr->timeout / cost) shots = load_o_ptr->timeout / cost;
				}
				else
				{
					if (shots > load_o_ptr->pval) shots = load_o_ptr->pval;
				}

				while (shots-- && !dead)
				{
#if 0
					/* Message if visible */
					if (m_ptr->ml)
					{
						/* describe the monster (again, just in case :-) */
						monster_desc(m_name, m_ptr, 0);

						/* Print a message */
						msg_format("%^s is hit by some magic.", m_name);
					}
#endif
					/* Get the effect effect */
					switch (load_o_ptr->tval)
					{
					case TV_ROD_MAIN:
						dead = mon_hit_trap_aux_rod(m_idx, load_o_ptr);
						break;
					case TV_WAND:
						dead = mon_hit_trap_aux_wand(m_idx, load_o_ptr);
						break;
					case TV_STAFF:
						dead = mon_hit_trap_aux_staff(m_idx, load_o_ptr);
						break;
					}

					if (load_o_ptr->tval == TV_ROD_MAIN)
					{
						/* decrease stored mana (timeout) for rods */
						load_o_ptr->timeout -= cost;
					}
					else
					{
						/* decrease charges for wands and staves */
						load_o_ptr->pval--;
					}
				}

				break;
			}

		default:
			msg_print("oops! nonexistant trap!");

		}

		/* Non-automatic traps are removed */
		if (!(f2 & (TRAP2_AUTOMATIC_5 | TRAP2_AUTOMATIC_99)))
		{
			remove = TRUE;
		}
		else if (f2 & TRAP2_AUTOMATIC_5) remove = (randint(5) == 1);

	}

	/* Special trap effect -- teleport to */
	if ((f2 & TRAP2_TELEPORT_TO) && (!disarm) && (!dead))
	{
		teleport_monster_to(m_idx, p_ptr->py, p_ptr->px);
	}

	/* Remove the trap if inactive now */
	if (remove) place_floor(my, mx);

	/* did it die? */
	return (dead);
}

void give_random_nastytrap_effect(void)
{
	switch (randint(173)) {
		case 1:
			p_ptr->nastytrap1 = TRUE;
			break;
		case 2:
			p_ptr->nastytrap2 = TRUE;
			break;
		case 3:
			p_ptr->nastytrap3 = TRUE;
			break;
		case 4:
			p_ptr->nastytrap4 = TRUE;
			break;
		case 5:
			p_ptr->nastytrap5 = TRUE;
			break;
		case 6:
			p_ptr->nastytrap6 = TRUE;
			break;
		case 7:
			p_ptr->nastytrap7 = TRUE;
			break;
		case 8:
			p_ptr->nastytrap8 = TRUE;
			break;
		case 9:
			p_ptr->nastytrap9 = TRUE;
			break;
		case 10:
			p_ptr->nastytrap10 = TRUE;
			break;
		case 11:
			p_ptr->nastytrap11 = TRUE;
			break;
		case 12:
			p_ptr->nastytrap12 = TRUE;
			break;
		case 13:
			p_ptr->nastytrap13 = TRUE;
			break;
		case 14:
			p_ptr->nastytrap14 = TRUE;
			break;
		case 15:
			p_ptr->nastytrap15 = TRUE;
			break;
		case 16:
			p_ptr->nastytrap16 = TRUE;
			break;
		case 17:
			p_ptr->nastytrap17 = TRUE;
			break;
		case 18:
			p_ptr->nastytrap18 = TRUE;
			break;
		case 19:
			p_ptr->nastytrap19 = TRUE;
			break;
		case 20:
			p_ptr->nastytrap20 = TRUE;
			break;
		case 21:
			p_ptr->nastytrap21 = TRUE;
			break;
		case 22:
			p_ptr->nastytrap22 = TRUE;
			break;
		case 23:
			p_ptr->nastytrap23 = TRUE;
			break;
		case 24:
			p_ptr->nastytrap24 = TRUE;
			break;
		case 25:
			p_ptr->nastytrap25 = TRUE;
			break;
		case 26:
			p_ptr->nastytrap26 = TRUE;
			break;
		case 27:
			p_ptr->nastytrap27 = TRUE;
			break;
		case 28:
			p_ptr->nastytrap28 = TRUE;
			break;
		case 29:
			p_ptr->nastytrap29 = TRUE;
			break;
		case 30:
			p_ptr->nastytrap30 = TRUE;
			break;
		case 31:
			p_ptr->nastytrap31 = TRUE;
			break;
		case 32:
			p_ptr->nastytrap32 = TRUE;
			break;
		case 33:
			p_ptr->nastytrap33 = TRUE;
			break;
		case 34:
			p_ptr->nastytrap34 = TRUE;
			break;
		case 35:
			p_ptr->nastytrap35 = TRUE;
			break;
		case 36:
			p_ptr->nastytrap36 = TRUE;
			break;
		case 37:
			p_ptr->nastytrap37 = TRUE;
			break;
		case 38:
			p_ptr->nastytrap38 = TRUE;
			break;
		case 39:
			p_ptr->nastytrap39 = TRUE;
			break;
		case 40:
			p_ptr->nastytrap40 = TRUE;
			break;
		case 41:
			p_ptr->nastytrap41 = TRUE;
			break;
		case 42:
			p_ptr->nastytrap42 = TRUE;
			break;
		case 43:
			p_ptr->nastytrap43 = TRUE;
			break;
		case 44:
			p_ptr->nastytrap44 = TRUE;
			break;
		case 45:
			p_ptr->nastytrap45 = TRUE;
			break;
		case 46:
			p_ptr->nastytrap46 = TRUE;
			break;
		case 47:
			p_ptr->nastytrap47 = TRUE;
			break;
		case 48:
			p_ptr->nastytrap48 = TRUE;
			break;
		case 49:
			p_ptr->nastytrap49 = TRUE;
			break;
		case 50:
			p_ptr->nastytrap50 = TRUE;
			break;
		case 51:
			p_ptr->nastytrap51 = TRUE;
			break;
		case 52:
			p_ptr->nastytrap52 = TRUE;
			break;
		case 53:
			p_ptr->nastytrap53 = TRUE;
			break;
		case 54:
			p_ptr->nastytrap54 = TRUE;
			break;
		case 55:
			p_ptr->nastytrap55 = TRUE;
			break;
		case 56:
			p_ptr->nastytrap56 = TRUE;
			break;
		case 57:
			p_ptr->nastytrap57 = TRUE;
			break;
		case 58:
			p_ptr->nastytrap58 = TRUE;
			break;
		case 59:
			p_ptr->nastytrap59 = TRUE;
			break;
		case 60:
			p_ptr->nastytrap60 = TRUE;
			break;
		case 61:
			p_ptr->nastytrap61 = TRUE;
			break;
		case 62:
			p_ptr->nastytrap62 = TRUE;
			break;
		case 63:
			p_ptr->nastytrap63 = TRUE;
			break;
		case 64:
			p_ptr->nastytrap64 = TRUE;
			break;
		case 65:
			p_ptr->nastytrap65 = TRUE;
			break;
		case 66:
			p_ptr->nastytrap66 = TRUE;
			break;
		case 67:
			p_ptr->nastytrap67 = TRUE;
			break;
		case 68:
			p_ptr->nastytrap68 = TRUE;
			break;
		case 69:
			p_ptr->nastytrap69 = TRUE;
			break;
		case 70:
			p_ptr->nastytrap70 = TRUE;
			break;
		case 71:
			p_ptr->nastytrap71 = TRUE;
			break;
		case 72:
			p_ptr->nastytrap72 = TRUE;
			break;
		case 73:
			p_ptr->nastytrap73 = TRUE;
			break;
		case 74:
			p_ptr->nastytrap74 = TRUE;
			break;
		case 75:
			p_ptr->nastytrap75 = TRUE;
			break;
		case 76:
			p_ptr->nastytrap76 = TRUE;
			break;
		case 77:
			p_ptr->nastytrap77 = TRUE;
			break;
		case 78:
			p_ptr->nastytrap78 = TRUE;
			break;
		case 79:
			p_ptr->nastytrap79 = TRUE;
			break;
		case 80:
			p_ptr->nastytrap80 = TRUE;
			break;
		case 81:
			p_ptr->nastytrap81 = TRUE;
			break;
		case 82:
			p_ptr->nastytrap82 = TRUE;
			break;
		case 83:
			p_ptr->nastytrap83 = TRUE;
			break;
		case 84:
			p_ptr->nastytrap84 = TRUE;
			break;
		case 85:
			p_ptr->nastytrap85 = TRUE;
			break;
		case 86:
			p_ptr->nastytrap86 = TRUE;
			break;
		case 87:
			p_ptr->nastytrap87 = TRUE;
			break;
		case 88:
			p_ptr->nastytrap88 = TRUE;
			break;
		case 89:
			p_ptr->nastytrap89 = TRUE;
			break;
		case 90:
			p_ptr->nastytrap90 = TRUE;
			break;
		case 91:
			p_ptr->nastytrap91 = TRUE;
			break;
		case 92:
			p_ptr->nastytrap92 = TRUE;
			break;
		case 93:
			p_ptr->nastytrap93 = TRUE;
			break;
		case 94:
			p_ptr->nastytrap94 = TRUE;
			break;
		case 95:
			p_ptr->nastytrap95 = TRUE;
			break;
		case 96:
			p_ptr->nastytrap96 = TRUE;
			break;
		case 97:
			p_ptr->nastytrap97 = TRUE;
			break;
		case 98:
			p_ptr->nastytrap98 = TRUE;
			break;
		case 99:
			p_ptr->nastytrap99 = TRUE;
			break;
		case 100:
			p_ptr->nastytrap100 = TRUE;
			break;
		case 101:
			p_ptr->nastytrap101 = TRUE;
			break;
		case 102:
			p_ptr->nastytrap102 = TRUE;
			break;
		case 103:
			p_ptr->nastytrap103 = TRUE;
			break;
		case 104:
			p_ptr->nastytrap104 = TRUE;
			break;
		case 105:
			p_ptr->nastytrap105 = TRUE;
			break;
		case 106:
			p_ptr->nastytrap106 = TRUE;
			break;
		case 107:
			p_ptr->nastytrap107 = TRUE;
			break;
		case 108:
			p_ptr->nastytrap108 = TRUE;
			break;
		case 109:
			p_ptr->nastytrap109 = TRUE;
			break;
		case 110:
			p_ptr->nastytrap110 = TRUE;
			break;
		case 111:
			p_ptr->nastytrap111 = TRUE;
			break;
		case 112:
			p_ptr->nastytrap112 = TRUE;
			break;
		case 113:
			p_ptr->nastytrap113 = TRUE;
			break;
		case 114:
			p_ptr->nastytrap114 = TRUE;
			break;
		case 115:
			p_ptr->nastytrap115 = TRUE;
			break;
		case 116:
			p_ptr->nastytrap116 = TRUE;
			break;
		case 117:
			p_ptr->nastytrap117 = TRUE;
			break;
		case 118:
			p_ptr->nastytrap118 = TRUE;
			break;
		case 119:
			p_ptr->nastytrap119 = TRUE;
			break;
		case 120:
			p_ptr->nastytrap120 = TRUE;
			break;
		case 121:
			p_ptr->nastytrap121 = TRUE;
			break;
		case 122:
			p_ptr->nastytrap122 = TRUE;
			break;
		case 123:
			p_ptr->nastytrap123 = TRUE;
			break;
		case 124:
			p_ptr->nastytrap124 = TRUE;
			break;
		case 125:
			p_ptr->nastytrap125 = TRUE;
			break;
		case 126:
			p_ptr->nastytrap126 = TRUE;
			break;
		case 127:
			p_ptr->nastytrap127 = TRUE;
			break;
		case 128:
			p_ptr->nastytrap128 = TRUE;
			break;
		case 129:
			p_ptr->nastytrap129 = TRUE;
			break;
		case 130:
			p_ptr->nastytrap130 = TRUE;
			break;
		case 131:
			p_ptr->nastytrap131 = TRUE;
			break;
		case 132:
			p_ptr->nastytrap132 = TRUE;
			break;
		case 133:
			p_ptr->nastytrap133 = TRUE;
			break;
		case 134:
			p_ptr->nastytrap134 = TRUE;
			break;
		case 135:
			p_ptr->nastytrap135 = TRUE;
			break;
		case 136:
			p_ptr->nastytrap136 = TRUE;
			break;
		case 137:
			p_ptr->nastytrap137 = TRUE;
			break;
		case 138:
			p_ptr->nastytrap138 = TRUE;
			break;
		case 139:
			p_ptr->nastytrap139 = TRUE;
			break;
		case 140:
			p_ptr->nastytrap140 = TRUE;
			break;
		case 141:
			p_ptr->nastytrap141 = TRUE;
			break;
		case 142:
			p_ptr->nastytrap142 = TRUE;
			break;
		case 143:
			p_ptr->nastytrap143 = TRUE;
			break;
		case 144:
			p_ptr->nastytrap144 = TRUE;
			break;
		case 145:
			p_ptr->nastytrap145 = TRUE;
			break;
		case 146:
			p_ptr->nastytrap146 = TRUE;
			break;
		case 147:
			p_ptr->nastytrap147 = TRUE;
			break;
		case 148:
			p_ptr->nastytrap148 = TRUE;
			break;
		case 149:
			p_ptr->nastytrap149 = TRUE;
			break;
		case 150:
			p_ptr->nastytrap150 = TRUE;
			break;
		case 151:
			p_ptr->nastytrap151 = TRUE;
			break;
		case 152:
			p_ptr->nastytrap152 = TRUE;
			break;
		case 153:
			p_ptr->nastytrap153 = TRUE;
			break;
		case 154:
			p_ptr->nastytrap154 = TRUE;
			break;
		case 155:
			p_ptr->nastytrap155 = TRUE;
			break;
		case 156:
			p_ptr->nastytrap156 = TRUE;
			break;
		case 157:
			p_ptr->nastytrap157 = TRUE;
			break;
		case 158:
			p_ptr->nastytrap158 = TRUE;
			break;
		case 159:
			p_ptr->nastytrap159 = TRUE;
			break;
		case 160:
			p_ptr->nastytrap160 = TRUE;
			break;
		case 161:
			p_ptr->nastytrap161 = TRUE;
			break;
		case 162:
			p_ptr->nastytrap162 = TRUE;
			break;
		case 163:
			p_ptr->nastytrap163 = TRUE;
			break;
		case 164:
			p_ptr->nastytrap164 = TRUE;
			break;
		case 165:
			p_ptr->nastytrap165 = TRUE;
			break;
		case 166:
			p_ptr->nastytrap166 = TRUE;
			break;
		case 167:
			p_ptr->nastytrap167 = TRUE;
			break;
		case 168:
			p_ptr->nastytrap168 = TRUE;
			break;
		case 169:
			p_ptr->nastytrap169 = TRUE;
			break;
		case 170:
			p_ptr->nastytrap170 = TRUE;
			break;
		case 171:
			p_ptr->nastytrap171 = TRUE;
			break;
		case 172:
			p_ptr->nastytrap172 = TRUE;
			break;
		case 173:
			p_ptr->nastytrap173 = TRUE;
			break;

	}
}

void cure_nasty_traps(void)
{
	if (p_ptr->nastytrap127) { /* switchback trap: gets cured, but prevents the switcher from curing other nasty trap effects --Amy */
		p_ptr->nastytrap127 = FALSE;
		calc_bonuses(TRUE);
		return;
	}

	p_ptr->nastytrap1 = FALSE;
	p_ptr->nastytrap2 = FALSE;
	p_ptr->nastytrap3 = FALSE;
	p_ptr->nastytrap4 = FALSE;
	p_ptr->nastytrap5 = FALSE;
	p_ptr->nastytrap6 = FALSE;
	p_ptr->nastytrap7 = FALSE;
	p_ptr->nastytrap8 = FALSE;
	p_ptr->nastytrap9 = FALSE;
	p_ptr->nastytrap10 = FALSE;
	p_ptr->nastytrap11 = FALSE;
	p_ptr->nastytrap12 = FALSE;
	p_ptr->nastytrap13 = FALSE;
	p_ptr->nastytrap14 = FALSE;
	p_ptr->nastytrap15 = FALSE;
	p_ptr->nastytrap16 = FALSE;
	p_ptr->nastytrap17 = FALSE;
	p_ptr->nastytrap18 = FALSE;
	p_ptr->nastytrap19 = FALSE;
	p_ptr->nastytrap20 = FALSE;
	p_ptr->nastytrap21 = FALSE;
	p_ptr->nastytrap22 = FALSE;
	p_ptr->nastytrap23 = FALSE;
	p_ptr->nastytrap24 = FALSE;
	p_ptr->nastytrap25 = FALSE;
	p_ptr->nastytrap26 = FALSE;
	p_ptr->nastytrap27 = FALSE;
	p_ptr->nastytrap28 = FALSE;
	p_ptr->nastytrap29 = FALSE;
	p_ptr->nastytrap30 = FALSE;
	p_ptr->nastytrap31 = FALSE;
	p_ptr->nastytrap32 = FALSE;
	p_ptr->nastytrap33 = FALSE;
	p_ptr->nastytrap34 = FALSE;
	p_ptr->nastytrap35 = FALSE;
	p_ptr->nastytrap36 = FALSE;
	p_ptr->nastytrap37 = FALSE;
	p_ptr->nastytrap38 = FALSE;
	p_ptr->nastytrap39 = FALSE;
	p_ptr->nastytrap40 = FALSE;
	p_ptr->nastytrap41 = FALSE;
	p_ptr->nastytrap42 = FALSE;
	p_ptr->nastytrap43 = FALSE;
	p_ptr->nastytrap44 = FALSE;
	p_ptr->nastytrap45 = FALSE;
	p_ptr->nastytrap46 = FALSE;
	p_ptr->nastytrap47 = FALSE;
	p_ptr->nastytrap48 = FALSE;
	p_ptr->nastytrap49 = FALSE;
	p_ptr->nastytrap50 = FALSE;
	p_ptr->nastytrap51 = FALSE;
	p_ptr->nastytrap52 = FALSE;
	p_ptr->nastytrap53 = FALSE;
	p_ptr->nastytrap54 = FALSE;
	p_ptr->nastytrap55 = FALSE;
	p_ptr->nastytrap56 = FALSE;
	p_ptr->nastytrap57 = FALSE;
	p_ptr->nastytrap58 = FALSE;
	p_ptr->nastytrap59 = FALSE;
	p_ptr->nastytrap60 = FALSE;
	p_ptr->nastytrap61 = FALSE;
	p_ptr->nastytrap62 = FALSE;
	p_ptr->nastytrap63 = FALSE;
	p_ptr->nastytrap64 = FALSE;
	p_ptr->nastytrap65 = FALSE;
	p_ptr->nastytrap66 = FALSE;
	p_ptr->nastytrap67 = FALSE;
	p_ptr->nastytrap68 = FALSE;
	p_ptr->nastytrap69 = FALSE;
	p_ptr->nastytrap70 = FALSE;
	p_ptr->nastytrap71 = FALSE;
	p_ptr->nastytrap72 = FALSE;
	p_ptr->nastytrap73 = FALSE;
	p_ptr->nastytrap74 = FALSE;
	p_ptr->nastytrap75 = FALSE;
	p_ptr->nastytrap76 = FALSE;
	p_ptr->nastytrap77 = FALSE;
	p_ptr->nastytrap78 = FALSE;
	p_ptr->nastytrap79 = FALSE;
	p_ptr->nastytrap80 = FALSE;
	p_ptr->nastytrap81 = FALSE;
	p_ptr->nastytrap82 = FALSE;
	p_ptr->nastytrap83 = FALSE;
	p_ptr->nastytrap84 = FALSE;
	p_ptr->nastytrap85 = FALSE;
	p_ptr->nastytrap86 = FALSE;
	p_ptr->nastytrap87 = FALSE;
	p_ptr->nastytrap88 = FALSE;
	p_ptr->nastytrap89 = FALSE;
	p_ptr->nastytrap90 = FALSE;
	p_ptr->nastytrap91 = FALSE;
	p_ptr->nastytrap92 = FALSE;
	p_ptr->nastytrap93 = FALSE;
	p_ptr->nastytrap94 = FALSE;
	p_ptr->nastytrap95 = FALSE;
	p_ptr->nastytrap96 = FALSE;
	p_ptr->nastytrap97 = FALSE;
	p_ptr->nastytrap98 = FALSE;
	p_ptr->nastytrap99 = FALSE;
	p_ptr->nastytrap100 = FALSE;
	p_ptr->nastytrap101 = FALSE;
	p_ptr->nastytrap102 = FALSE;
	p_ptr->nastytrap103 = FALSE;
	p_ptr->nastytrap104 = FALSE;
	p_ptr->nastytrap105 = FALSE;
	p_ptr->nastytrap106 = FALSE;
	p_ptr->nastytrap107 = FALSE;
	p_ptr->nastytrap108 = FALSE;
	p_ptr->nastytrap109 = FALSE;
	p_ptr->nastytrap110 = FALSE;
	p_ptr->nastytrap111 = FALSE;
	p_ptr->nastytrap112 = FALSE;
	p_ptr->nastytrap113 = FALSE;
	p_ptr->nastytrap114 = FALSE;
	p_ptr->nastytrap115 = FALSE;
	p_ptr->nastytrap116 = FALSE;
	p_ptr->nastytrap117 = FALSE;
	p_ptr->nastytrap118 = FALSE;
	p_ptr->nastytrap119 = FALSE;
	p_ptr->nastytrap120 = FALSE;
	p_ptr->nastytrap121 = FALSE;
	p_ptr->nastytrap122 = FALSE;
	p_ptr->nastytrap123 = FALSE;
	p_ptr->nastytrap124 = FALSE;
	p_ptr->nastytrap125 = FALSE;
	p_ptr->nastytrap126 = FALSE;
	p_ptr->nastytrap127 = FALSE;
	p_ptr->nastytrap128 = FALSE;
	p_ptr->nastytrap129 = FALSE;
	p_ptr->nastytrap130 = FALSE;
	p_ptr->nastytrap131 = FALSE;
	p_ptr->nastytrap132 = FALSE;
	p_ptr->nastytrap133 = FALSE;
	p_ptr->nastytrap134 = FALSE;
	p_ptr->nastytrap135 = FALSE;
	p_ptr->nastytrap136 = FALSE;
	p_ptr->nastytrap137 = FALSE;
	p_ptr->nastytrap138 = FALSE;
	p_ptr->nastytrap139 = FALSE;
	p_ptr->nastytrap140 = FALSE;
	p_ptr->nastytrap141 = FALSE;
	p_ptr->nastytrap142 = FALSE;
	p_ptr->nastytrap143 = FALSE;
	p_ptr->nastytrap144 = FALSE;
	p_ptr->nastytrap145 = FALSE;
	p_ptr->nastytrap146 = FALSE;
	p_ptr->nastytrap147 = FALSE;
	p_ptr->nastytrap148 = FALSE;
	p_ptr->nastytrap149 = FALSE;
	p_ptr->nastytrap150 = FALSE;
	p_ptr->nastytrap151 = FALSE;
	p_ptr->nastytrap152 = FALSE;
	p_ptr->nastytrap153 = FALSE;
	p_ptr->nastytrap154 = FALSE;
	p_ptr->nastytrap155 = FALSE;
	p_ptr->nastytrap156 = FALSE;
	p_ptr->nastytrap157 = FALSE;
	p_ptr->nastytrap158 = FALSE;
	p_ptr->nastytrap159 = FALSE;
	p_ptr->nastytrap160 = FALSE;
	p_ptr->nastytrap161 = FALSE;
	p_ptr->nastytrap162 = FALSE;
	p_ptr->nastytrap163 = FALSE;
	p_ptr->nastytrap164 = FALSE;
	p_ptr->nastytrap165 = FALSE;
	p_ptr->nastytrap166 = FALSE;
	p_ptr->nastytrap167 = FALSE;
	p_ptr->nastytrap168 = FALSE;
	p_ptr->nastytrap169 = FALSE;
	p_ptr->nastytrap170 = FALSE;
	p_ptr->nastytrap171 = FALSE;
	p_ptr->nastytrap172 = FALSE;
	p_ptr->nastytrap173 = FALSE;
	p_ptr->nastytrap174 = FALSE;
	p_ptr->nastytrap175 = FALSE;
	p_ptr->nastytrap176 = FALSE;
	p_ptr->nastytrap177 = FALSE;
	p_ptr->nastytrap178 = FALSE;
	p_ptr->nastytrap179 = FALSE;
	p_ptr->nastytrap180 = FALSE;
	p_ptr->nastytrap181 = FALSE;
	p_ptr->nastytrap182 = FALSE;
	p_ptr->nastytrap183 = FALSE;
	p_ptr->nastytrap184 = FALSE;
	p_ptr->nastytrap185 = FALSE;
	p_ptr->nastytrap186 = FALSE;
	p_ptr->nastytrap187 = FALSE;
	p_ptr->nastytrap188 = FALSE;
	p_ptr->nastytrap189 = FALSE;
	p_ptr->nastytrap190 = FALSE;
	p_ptr->nastytrap191 = FALSE;
	p_ptr->nastytrap192 = FALSE;
	p_ptr->nastytrap193 = FALSE;
	p_ptr->nastytrap194 = FALSE;
	p_ptr->nastytrap195 = FALSE;
	p_ptr->nastytrap196 = FALSE;
	p_ptr->nastytrap197 = FALSE;
	p_ptr->nastytrap198 = FALSE;
	p_ptr->nastytrap199 = FALSE;
	p_ptr->nastytrap200 = FALSE;
	p_ptr->nastytrap201 = FALSE;
	p_ptr->nastytrap202 = FALSE;
	p_ptr->nastytrap203 = FALSE;
	p_ptr->nastytrap204 = FALSE;
	p_ptr->nastytrap205 = FALSE;
	p_ptr->nastytrap206 = FALSE;
	p_ptr->nastytrap207 = FALSE;
	p_ptr->nastytrap208 = FALSE;
	p_ptr->nastytrap209 = FALSE;
	p_ptr->nastytrap210 = FALSE;
	p_ptr->nastytrap211 = FALSE;
	p_ptr->nastytrap212 = FALSE;
	p_ptr->nastytrap213 = FALSE;
	p_ptr->nastytrap214 = FALSE;
	p_ptr->nastytrap215 = FALSE;
	p_ptr->nastytrap216 = FALSE;
	p_ptr->nastytrap217 = FALSE;
	p_ptr->nastytrap218 = FALSE;
	p_ptr->nastytrap219 = FALSE;
	p_ptr->nastytrap220 = FALSE;
	p_ptr->nastytrap221 = FALSE;
	p_ptr->nastytrap222 = FALSE;
	p_ptr->nastytrap223 = FALSE;
	p_ptr->nastytrap224 = FALSE;
	p_ptr->nastytrap225 = FALSE;
	p_ptr->nastytrap226 = FALSE;
	p_ptr->nastytrap227 = FALSE;
	p_ptr->nastytrap228 = FALSE;
	p_ptr->nastytrap229 = FALSE;
	p_ptr->nastytrap230 = FALSE;
	p_ptr->nastytrap231 = FALSE;
	p_ptr->nastytrap232 = FALSE;
	p_ptr->nastytrap233 = FALSE;
	p_ptr->nastytrap234 = FALSE;
	p_ptr->nastytrap235 = FALSE;
	p_ptr->nastytrap236 = FALSE;
	p_ptr->nastytrap237 = FALSE;
	p_ptr->nastytrap238 = FALSE;
	p_ptr->nastytrap239 = FALSE;
	p_ptr->nastytrap240 = FALSE;
	p_ptr->nastytrap241 = FALSE;
	p_ptr->nastytrap242 = FALSE;
	p_ptr->nastytrap243 = FALSE;
	p_ptr->nastytrap244 = FALSE;
	p_ptr->nastytrap245 = FALSE;
	p_ptr->nastytrap246 = FALSE;
	p_ptr->nastytrap247 = FALSE;
	p_ptr->nastytrap248 = FALSE;
	p_ptr->nastytrap249 = FALSE;
	p_ptr->nastytrap250 = FALSE;
	p_ptr->nastytrap251 = FALSE;
	p_ptr->nastytrap252 = FALSE;
	p_ptr->nastytrap253 = FALSE;
	p_ptr->nastytrap254 = FALSE;
	p_ptr->nastytrap255 = FALSE;
	p_ptr->nastytrap256 = FALSE;
	p_ptr->nastytrap257 = FALSE;
	p_ptr->nastytrap258 = FALSE;
	p_ptr->nastytrap259 = FALSE;
	p_ptr->nastytrap260 = FALSE;
	p_ptr->nastytrap261 = FALSE;
	p_ptr->nastytrap262 = FALSE;
	p_ptr->nastytrap263 = FALSE;
	p_ptr->nastytrap264 = FALSE;
	p_ptr->nastytrap265 = FALSE;
	p_ptr->nastytrap266 = FALSE;
	p_ptr->nastytrap267 = FALSE;
	p_ptr->nastytrap268 = FALSE;
	p_ptr->nastytrap269 = FALSE;
	p_ptr->nastytrap270 = FALSE;
	p_ptr->nastytrap271 = FALSE;
	p_ptr->nastytrap272 = FALSE;
	p_ptr->nastytrap273 = FALSE;
	p_ptr->nastytrap274 = FALSE;
	p_ptr->nastytrap275 = FALSE;
	p_ptr->nastytrap276 = FALSE;
	p_ptr->nastytrap277 = FALSE;
	p_ptr->nastytrap278 = FALSE;
	p_ptr->nastytrap279 = FALSE;
	p_ptr->nastytrap280 = FALSE;
	p_ptr->nastytrap281 = FALSE;
	p_ptr->nastytrap282 = FALSE;
	p_ptr->nastytrap283 = FALSE;
	p_ptr->nastytrap284 = FALSE;
	p_ptr->nastytrap285 = FALSE;
	p_ptr->nastytrap286 = FALSE;
	p_ptr->nastytrap287 = FALSE;
	p_ptr->nastytrap288 = FALSE;
	p_ptr->nastytrap289 = FALSE;
	p_ptr->nastytrap290 = FALSE;
	p_ptr->nastytrap291 = FALSE;
	p_ptr->nastytrap292 = FALSE;
	p_ptr->nastytrap293 = FALSE;
	p_ptr->nastytrap294 = FALSE;
	p_ptr->nastytrap295 = FALSE;
	p_ptr->nastytrap296 = FALSE;
	p_ptr->nastytrap297 = FALSE;
	p_ptr->nastytrap298 = FALSE;
	p_ptr->nastytrap299 = FALSE;
	p_ptr->nastytrap300 = FALSE;
	p_ptr->nastytrap301 = FALSE;
	p_ptr->nastytrap302 = FALSE;
	p_ptr->nastytrap303 = FALSE;
	p_ptr->nastytrap304 = FALSE;
	p_ptr->nastytrap305 = FALSE;
	p_ptr->nastytrap306 = FALSE;
	p_ptr->nastytrap307 = FALSE;
	p_ptr->nastytrap308 = FALSE;
	p_ptr->nastytrap309 = FALSE;
	p_ptr->nastytrap310 = FALSE;
	p_ptr->nastytrap311 = FALSE;
	p_ptr->nastytrap312 = FALSE;
	p_ptr->nastytrap313 = FALSE;
	p_ptr->nastytrap314 = FALSE;
	p_ptr->nastytrap315 = FALSE;
	p_ptr->nastytrap316 = FALSE;
	p_ptr->nastytrap317 = FALSE;
	p_ptr->nastytrap318 = FALSE;
	p_ptr->nastytrap319 = FALSE;
	p_ptr->nastytrap320 = FALSE;
	p_ptr->nastytrap321 = FALSE;
	p_ptr->nastytrap322 = FALSE;
	p_ptr->nastytrap323 = FALSE;
	p_ptr->nastytrap324 = FALSE;
	p_ptr->nastytrap325 = FALSE;
	p_ptr->nastytrap326 = FALSE;
	p_ptr->nastytrap327 = FALSE;
	p_ptr->nastytrap328 = FALSE;
	p_ptr->nastytrap329 = FALSE;
	p_ptr->nastytrap330 = FALSE;
	p_ptr->nastytrap331 = FALSE;
	p_ptr->nastytrap332 = FALSE;
	p_ptr->nastytrap333 = FALSE;
	p_ptr->nastytrap334 = FALSE;
	p_ptr->nastytrap335 = FALSE;
	p_ptr->nastytrap336 = FALSE;
	p_ptr->nastytrap337 = FALSE;
	p_ptr->nastytrap338 = FALSE;
	p_ptr->nastytrap339 = FALSE;
	p_ptr->nastytrap340 = FALSE;
	p_ptr->nastytrap341 = FALSE;
	p_ptr->nastytrap342 = FALSE;
	p_ptr->nastytrap343 = FALSE;
	p_ptr->nastytrap344 = FALSE;
	p_ptr->nastytrap345 = FALSE;
	p_ptr->nastytrap346 = FALSE;
	p_ptr->nastytrap347 = FALSE;
	p_ptr->nastytrap348 = FALSE;
	p_ptr->nastytrap349 = FALSE;
	p_ptr->nastytrap350 = FALSE;
	p_ptr->nastytrap351 = FALSE;
	p_ptr->nastytrap352 = FALSE;
	p_ptr->nastytrap353 = FALSE;
	p_ptr->nastytrap354 = FALSE;
	p_ptr->nastytrap355 = FALSE;
	p_ptr->nastytrap356 = FALSE;
	p_ptr->nastytrap357 = FALSE;
	p_ptr->nastytrap358 = FALSE;
	p_ptr->nastytrap359 = FALSE;
	p_ptr->nastytrap360 = FALSE;
	p_ptr->nastytrap361 = FALSE;
	p_ptr->nastytrap362 = FALSE;
	p_ptr->nastytrap363 = FALSE;
	p_ptr->nastytrap364 = FALSE;
	p_ptr->nastytrap365 = FALSE;
	p_ptr->nastytrap366 = FALSE;
	p_ptr->nastytrap367 = FALSE;
	p_ptr->nastytrap368 = FALSE;
	p_ptr->nastytrap369 = FALSE;
	p_ptr->nastytrap370 = FALSE;
	p_ptr->nastytrap371 = FALSE;
	p_ptr->nastytrap372 = FALSE;
	p_ptr->nastytrap373 = FALSE;
	p_ptr->nastytrap374 = FALSE;
	p_ptr->nastytrap375 = FALSE;
	p_ptr->nastytrap376 = FALSE;
	p_ptr->nastytrap377 = FALSE;
	p_ptr->nastytrap378 = FALSE;
	p_ptr->nastytrap379 = FALSE;
	p_ptr->nastytrap380 = FALSE;
	p_ptr->nastytrap381 = FALSE;
	p_ptr->nastytrap382 = FALSE;
	p_ptr->nastytrap383 = FALSE;
	p_ptr->nastytrap384 = FALSE;
	p_ptr->nastytrap385 = FALSE;
	p_ptr->nastytrap386 = FALSE;
	p_ptr->nastytrap387 = FALSE;
	p_ptr->nastytrap388 = FALSE;
	p_ptr->nastytrap389 = FALSE;
	p_ptr->nastytrap390 = FALSE;
	p_ptr->nastytrap391 = FALSE;
	p_ptr->nastytrap392 = FALSE;
	p_ptr->nastytrap393 = FALSE;
	p_ptr->nastytrap394 = FALSE;
	p_ptr->nastytrap395 = FALSE;
	p_ptr->nastytrap396 = FALSE;
	p_ptr->nastytrap397 = FALSE;
	p_ptr->nastytrap398 = FALSE;
	p_ptr->nastytrap399 = FALSE;
	p_ptr->nastytrap400 = FALSE;
	p_ptr->nastytrap401 = FALSE;
	p_ptr->nastytrap402 = FALSE;
	p_ptr->nastytrap403 = FALSE;
	p_ptr->nastytrap404 = FALSE;
	p_ptr->nastytrap405 = FALSE;
	p_ptr->nastytrap406 = FALSE;
	p_ptr->nastytrap407 = FALSE;
	p_ptr->nastytrap408 = FALSE;
	p_ptr->nastytrap409 = FALSE;
	p_ptr->nastytrap410 = FALSE;
	p_ptr->nastytrap411 = FALSE;
	p_ptr->nastytrap412 = FALSE;
	p_ptr->nastytrap413 = FALSE;
	p_ptr->nastytrap414 = FALSE;
	p_ptr->nastytrap415 = FALSE;
	p_ptr->nastytrap416 = FALSE;
	p_ptr->nastytrap417 = FALSE;
	p_ptr->nastytrap418 = FALSE;
	p_ptr->nastytrap419 = FALSE;
	p_ptr->nastytrap420 = FALSE;
	p_ptr->nastytrap421 = FALSE;
	p_ptr->nastytrap422 = FALSE;
	p_ptr->nastytrap423 = FALSE;
	p_ptr->nastytrap424 = FALSE;
	p_ptr->nastytrap425 = FALSE;
	p_ptr->nastytrap426 = FALSE;
	p_ptr->nastytrap427 = FALSE;
	p_ptr->nastytrap428 = FALSE;
	p_ptr->nastytrap429 = FALSE;
	p_ptr->nastytrap430 = FALSE;
	p_ptr->nastytrap431 = FALSE;
	p_ptr->nastytrap432 = FALSE;
	p_ptr->nastytrap433 = FALSE;
	p_ptr->nastytrap434 = FALSE;
	p_ptr->nastytrap435 = FALSE;
	p_ptr->nastytrap436 = FALSE;
	p_ptr->nastytrap437 = FALSE;
	p_ptr->nastytrap438 = FALSE;
	p_ptr->nastytrap439 = FALSE;
	p_ptr->nastytrap440 = FALSE;
	p_ptr->nastytrap441 = FALSE;
	p_ptr->nastytrap442 = FALSE;
	p_ptr->nastytrap443 = FALSE;
	p_ptr->nastytrap444 = FALSE;
	p_ptr->nastytrap445 = FALSE;
	p_ptr->nastytrap446 = FALSE;
	p_ptr->nastytrap447 = FALSE;
	p_ptr->nastytrap448 = FALSE;
	p_ptr->nastytrap449 = FALSE;
	p_ptr->nastytrap450 = FALSE;
	p_ptr->nastytrap451 = FALSE;
	p_ptr->nastytrap452 = FALSE;
	p_ptr->nastytrap453 = FALSE;
	p_ptr->nastytrap454 = FALSE;
	p_ptr->nastytrap455 = FALSE;
	p_ptr->nastytrap456 = FALSE;
	p_ptr->nastytrap457 = FALSE;
	p_ptr->nastytrap458 = FALSE;
	p_ptr->nastytrap459 = FALSE;
	p_ptr->nastytrap460 = FALSE;
	p_ptr->nastytrap461 = FALSE;
	p_ptr->nastytrap462 = FALSE;
	p_ptr->nastytrap463 = FALSE;
	p_ptr->nastytrap464 = FALSE;
	p_ptr->nastytrap465 = FALSE;
	p_ptr->nastytrap466 = FALSE;
	p_ptr->nastytrap467 = FALSE;
	p_ptr->nastytrap468 = FALSE;
	p_ptr->nastytrap469 = FALSE;
	p_ptr->nastytrap470 = FALSE;
	p_ptr->nastytrap471 = FALSE;
	p_ptr->nastytrap472 = FALSE;
	p_ptr->nastytrap473 = FALSE;
	p_ptr->nastytrap474 = FALSE;
	p_ptr->nastytrap475 = FALSE;
	p_ptr->nastytrap476 = FALSE;
	p_ptr->nastytrap477 = FALSE;
	p_ptr->nastytrap478 = FALSE;
	p_ptr->nastytrap479 = FALSE;
	p_ptr->nastytrap480 = FALSE;
	p_ptr->nastytrap481 = FALSE;
	p_ptr->nastytrap482 = FALSE;
	p_ptr->nastytrap483 = FALSE;
	p_ptr->nastytrap484 = FALSE;
	p_ptr->nastytrap485 = FALSE;
	p_ptr->nastytrap486 = FALSE;
	p_ptr->nastytrap487 = FALSE;
	p_ptr->nastytrap488 = FALSE;
	p_ptr->nastytrap489 = FALSE;
	p_ptr->nastytrap490 = FALSE;
	p_ptr->nastytrap491 = FALSE;
	p_ptr->nastytrap492 = FALSE;
	p_ptr->nastytrap493 = FALSE;
	p_ptr->nastytrap494 = FALSE;
	p_ptr->nastytrap495 = FALSE;
	p_ptr->nastytrap496 = FALSE;
	p_ptr->nastytrap497 = FALSE;
	p_ptr->nastytrap498 = FALSE;
	p_ptr->nastytrap499 = FALSE;
	p_ptr->nastytrap500 = FALSE;

	calc_bonuses(TRUE);

}
