#undef cquest
#define cquest (quest[QUEST_SHOES])

bool quest_shoes_gen_hook(char *fmt)
{
	int x, y, i;
	int xstart = 2;
	int ystart = 2;

	if (p_ptr->inside_quest != QUEST_SHOES) return FALSE;

	/* Just in case we didnt talk the the mayor */
	if (cquest.status == QUEST_STATUS_UNTAKEN)
		cquest.status = QUEST_STATUS_TAKEN;

	/* Start with perm walls */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave_set_feat(y, x, FEAT_PERM_SOLID);
		}
	}

	dun_level = quest[p_ptr->inside_quest].level;

	/* Set the correct monster hook */
	set_mon_num_hook();

	/* Prepare allocation table */
	get_mon_num_prep();

	init_flags = INIT_CREATE_DUNGEON;
	process_dungeon_file(NULL, "shoes.map", &ystart, &xstart, cur_hgt, cur_wid, TRUE);
	dungeon_flags2 |= DF2_NO_GENO;

	/* Place some random Ruea sandals */
	for (i = damroll(1, 4); i > 0; )
	{
		int flags;
		y = rand_int(21) + 3;
		x = rand_int(31) + 3;
		flags = f_info[cave[y][x].feat].flags1;
		if (!(flags & FF1_PERMANENT) && (flags & FF1_FLOOR))
		{
			place_monster_one(y, x, 1088, 0, magik(50), MSTATUS_ENEMY);
			--i;
		}
	}

	/* Place some random Klara sandals */
	for (i = damroll(1, 4); i > 0; )
	{
		int flags;
		y = rand_int(21) + 3;
		x = rand_int(31) + 3;
		flags = f_info[cave[y][x].feat].flags1;
		if (!(flags & FF1_PERMANENT) && (flags & FF1_FLOOR))
		{
			place_monster_one(y, x, 1096, 0, magik(50), MSTATUS_ENEMY);
			--i;
		}
	}

	/* Place some random Almuth sneakers */
	for (i = damroll(1, 4); i > 0; )
	{
		int flags;
		y = rand_int(21) + 3;
		x = rand_int(31) + 3;
		flags = f_info[cave[y][x].feat].flags1;
		if (!(flags & FF1_PERMANENT) && (flags & FF1_FLOOR))
		{
			place_monster_one(y, x, 1305, 0, magik(50), MSTATUS_ENEMY);
			--i;
		}
	}

	/* Place some random Bea combat boots */
	for (i = damroll(1, 4); i > 0; )
	{
		int flags;
		y = rand_int(21) + 3;
		x = rand_int(31) + 3;
		flags = f_info[cave[y][x].feat].flags1;
		if (!(flags & FF1_PERMANENT) && (flags & FF1_FLOOR))
		{
			place_monster_one(y, x, 1314, 0, magik(50), MSTATUS_ENEMY);
			--i;
		}
	}

	/* Place some random Judith pumps */
	for (i = damroll(1, 4); i > 0; )
	{
		int flags;
		y = rand_int(21) + 3;
		x = rand_int(31) + 3;
		flags = f_info[cave[y][x].feat].flags1;
		if (!(flags & FF1_PERMANENT) && (flags & FF1_FLOOR))
		{
			place_monster_one(y, x, 3784, 0, magik(50), MSTATUS_ENEMY);
			--i;
		}
	}

	/* Place a Martina combat boot */
	for (i = 1; i > 0; )
	{
		int flags;
		y = rand_int(21) + 3;
		x = rand_int(31) + 3;
		flags = f_info[cave[y][x].feat].flags1;
		if (!(flags & FF1_PERMANENT) && (flags & FF1_FLOOR))
		{
			place_monster_one(y, x, 3791, 0, magik(50), MSTATUS_ENEMY);
			--i;
		}
	}

	process_hooks_restart = TRUE;

	return TRUE;
}

bool quest_shoes_death_hook(char *fmt)
{
	int i, mcnt = 0;

	if (p_ptr->inside_quest != QUEST_SHOES) return FALSE;

	/* Process the monsters (backwards) */
	for (i = m_max - 1; i >= 1; i--)
	{
		/* Access the monster */
		monster_type *m_ptr = &m_list[i];

		/* Ignore "dead" monsters */
		if (!m_ptr->r_idx) continue;

		if (m_ptr->status <= MSTATUS_ENEMY) mcnt++;
	}

	/* Nobody left ? */
	if (mcnt <= 1)
	{
		/* Amy edit: nope, I won't let you scum this quest */
		cave_set_feat(20, 31, FEAT_LESS);
		cave[20][31].special = 0;

		quest[p_ptr->inside_quest].status = QUEST_STATUS_COMPLETED;
		del_hook(HOOK_MONSTER_DEATH, quest_shoes_death_hook);
		del_hook(HOOK_GEN_QUEST, quest_shoes_gen_hook);
		process_hooks_restart = TRUE;

		p_ptr->skill_points += 1;
		cmsg_format(TERM_L_GREEN, "You can increase %d more skills.", p_ptr->skill_points);

		cmsg_print(TERM_YELLOW, "Hobbiton is safer now.");
		return (FALSE);
	}
	return FALSE;
}

bool quest_shoes_finish_hook(char *fmt)
{
	s32b q_idx;

	q_idx = get_next_arg(fmt);

	if (q_idx != QUEST_SHOES) return FALSE;

	c_put_str(TERM_YELLOW, "Thank you for defeating those dangerous shoes!", 8, 0);
	c_put_str(TERM_YELLOW, "You can use the hut as your house as a reward.", 9, 0);

	/* End the plot */
	*(quest[q_idx].plot) = QUEST_NULL;

	return TRUE;
}

bool quest_shoes_init_hook(int q_idx)
{
	if ((cquest.status >= QUEST_STATUS_UNTAKEN) && (cquest.status < QUEST_STATUS_FINISHED))
	{
		add_hook(HOOK_MONSTER_DEATH, quest_shoes_death_hook, "shoes_monster_death");
		add_hook(HOOK_QUEST_FINISH, quest_shoes_finish_hook, "shoes_finish");
		add_hook(HOOK_GEN_QUEST, quest_shoes_gen_hook, "shoes_geb");
	}
	return (FALSE);
}
