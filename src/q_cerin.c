#undef cquest
#define cquest (quest[QUEST_CERIN])

bool quest_cerin_gen_hook(char *fmt)
{
	int x, y, i;
	int xstart = 2;
	int ystart = 2;

	if (p_ptr->inside_quest != QUEST_CERIN) return FALSE;

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
	process_dungeon_file(NULL, "wood.map", &ystart, &xstart, cur_hgt, cur_wid, TRUE);
	dungeon_flags2 |= DF2_NO_GENO;

	/* Place some random monsters from a specific terrain */
	for (i = damroll(5, 4); i > 0; )
	{
		int flags;
		y = rand_int(21) + 3;
		x = rand_int(31) + 3;
		flags = f_info[cave[y][x].feat].flags1;
		if (!(flags & FF1_PERMANENT) && (flags & FF1_FLOOR))
		{
			summon_specific(y, x, 15, SUMMON_WOOD);
			--i;
		}
	}

	/* Place some more monsters */
	for (i = damroll(2, 2); i > 0; )
	{
		int flags;
		y = rand_int(21) + 3;
		x = rand_int(31) + 3;
		flags = f_info[cave[y][x].feat].flags1;
		if (!(flags & FF1_PERMANENT) && (flags & FF1_FLOOR))
		{
			place_monster_one(y, x, 1307, 0, magik(50), MSTATUS_ENEMY);
			--i;
		}
	}

	process_hooks_restart = TRUE;

	return TRUE;
}

bool quest_cerin_death_hook(char *fmt)
{
	int i, mcnt = 0;

	if (p_ptr->inside_quest != QUEST_CERIN) return FALSE;

	/* Process the monsters (backwards) */
	for (i = m_max - 1; i >= 1; i--)
	{
		/* Access the monster */
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr;

		/* Ignore "dead" monsters */
		if (!m_ptr->r_idx) continue;

		r_ptr = &r_info[m_ptr->r_idx];

		/* Ignore breeders because they can spawn awake and breed out of control before you can do anything --Amy */
		if ((m_ptr->status <= MSTATUS_ENEMY) && !(r_ptr->flags4 & RF4_MULTIPLY)) mcnt++;
	}

	/* Nobody left ? */
	if (mcnt <= 1)
	{
		/* Amy edit: nope, I won't let you scum this quest */
		cave_set_feat(20, 31, FEAT_LESS);
		cave[20][31].special = 0;

		quest[p_ptr->inside_quest].status = QUEST_STATUS_COMPLETED;
		del_hook(HOOK_MONSTER_DEATH, quest_cerin_death_hook);
		del_hook(HOOK_GEN_QUEST, quest_cerin_gen_hook);
		process_hooks_restart = TRUE;

		p_ptr->skill_points += 1;
		cmsg_format(TERM_L_GREEN, "You can increase %d more skills.", p_ptr->skill_points);

		cmsg_print(TERM_YELLOW, "Cerin Amroth is safer now.");
		return (FALSE);
	}
	return FALSE;
}

bool quest_cerin_finish_hook(char *fmt)
{
	s32b q_idx;

	q_idx = get_next_arg(fmt);

	if (q_idx != QUEST_CERIN) return FALSE;

	c_put_str(TERM_YELLOW, "Thank you for rescuing our town!", 8, 0);
	c_put_str(TERM_YELLOW, "You can use the hut as your house as a reward.", 9, 0);

	/* End the plot */
	*(quest[q_idx].plot) = QUEST_NULL;

	return TRUE;
}

bool quest_cerin_init_hook(int q_idx)
{
	if ((cquest.status >= QUEST_STATUS_UNTAKEN) && (cquest.status < QUEST_STATUS_FINISHED))
	{
		/* holy hell, all those quests have the bug where if you save and load, you can summon a monster and then get the reward again --Amy */
		if (cquest.status < QUEST_STATUS_COMPLETED) add_hook(HOOK_MONSTER_DEATH, quest_cerin_death_hook, "cerin_monster_death");
		add_hook(HOOK_QUEST_FINISH, quest_cerin_finish_hook, "cerin_finish");
		add_hook(HOOK_GEN_QUEST, quest_cerin_gen_hook, "cerin_geb");
	}
	return (FALSE);
}
