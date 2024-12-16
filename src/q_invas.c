#undef cquest
#define cquest (quest[QUEST_INVASION])

bool quest_invasion_gen_hook(char *fmt)
{
	int x, y;
	int xstart = 2;
	int ystart = 2;

	if (p_ptr->inside_quest != QUEST_INVASION) return FALSE;

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
	process_dungeon_file_full = TRUE;
	/* if the player doesn't warp to Gondolin, no longer fail the quest; instead, get harder version with traps --Amy */
	if (xtra_maeglin) {
		process_dungeon_file(NULL, "xmaeglin.map", &ystart, &xstart, cur_hgt, cur_wid, TRUE);
	} else {
		process_dungeon_file(NULL, "maeglin.map", &ystart, &xstart, cur_hgt, cur_wid, TRUE);
	}
	process_dungeon_file_full = FALSE;

	for (x = 3; x < xstart; x++)
		for (y = 3; y < ystart; y++)
		{
			if (cave[y][x].feat == FEAT_MARKER)
			{
				cquest.data[0] = y;
				cquest.data[1] = x;
				p_ptr->py = y;
				p_ptr->px = x;
				cave_set_feat(p_ptr->py, p_ptr->px, FEAT_LESS);
			}
		}

	return TRUE;
}
bool quest_invasion_ai_hook(char *fmt)
{
	monster_type *m_ptr;
	s32b m_idx;

	m_idx = get_next_arg(fmt);
	m_ptr = &m_list[m_idx];

	if (p_ptr->inside_quest != QUEST_INVASION) return FALSE;

	/* Ugly but thats better than a call to test_monster_name which is SLOW */
	if (m_ptr->r_idx == 825)
	{
		/* Oups he fleed */
		if ((m_ptr->fy == cquest.data[0]) && (m_ptr->fx == cquest.data[1]))
		{
			delete_monster_idx(m_idx);

			cmsg_print(TERM_YELLOW, "Maeglin found the way to Gondolin! All hope is lost now!");
			cquest.status = QUEST_STATUS_SCREWED;
			/*town_info[2].destroyed = TRUE;*/
			return (FALSE);
		}

		/* Attack or flee ?*/
		if (distance(m_ptr->fy, m_ptr->fx, p_ptr->py, p_ptr->px) <= 2)
		{
			return (FALSE);
		}
		else
		{
			process_hooks_return[0].num = cquest.data[0];
			process_hooks_return[1].num = cquest.data[1];
			return (TRUE);
		}
	}

	return (FALSE);
}
bool quest_invasion_turn_hook(char *fmt)
{
	bool old_quick_messages = quick_messages;

	if (cquest.status != QUEST_STATUS_UNTAKEN) return (FALSE);
	if (p_ptr->lev < 45) return (FALSE);

	/* Wait until the end of the current quest */
	if (p_ptr->inside_quest) return ( FALSE);

	/* Wait until the end of the astral mode */
	if (p_ptr->astral) return ( FALSE);

	/* Ok give the quest */
	quick_messages = FALSE;
	cmsg_print(TERM_YELLOW, "A Thunderlord appears in front of you and says:");
	cmsg_print(TERM_YELLOW, "'Hello, noble hero. I am Liron, rider of Tolan. Turgon, King of Gondolin sent me.'");
	cmsg_print(TERM_YELLOW, "'Gondolin is being invaded; he needs your help now or everything will be lost.'");
	cmsg_print(TERM_YELLOW, "'I can bring you to Gondolin, but we must go now.'");
	/* This is SO important a question that flush pending inputs */
	flush();

	if (!get_check("Will you come?"))
	{
		cmsg_print(TERM_YELLOW, "'Hero, you can't just deny your help! Gondolin will fall if you don't come to help us!'");
		if (get_check("You MUST agree to come to Gondolin!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'Hero, I'm not joking! You have to come to Gondolin at once or all is lost!'");
		if (get_check("You MUST agree to come to Gondolin!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'You are testing my patience! Say yes, and come to Gondolin now!'");
		if (get_check("You *MUST* agree to come to Gondolin!!!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'Look, I know you might be in the middle of looting a vault but you must come anyway!'");
		if (get_check("You *MUST* agree to come to Gondolin!!!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'Yes, even if you just dropped your stuff on the ground you still must come!'");
		if (get_check("You *MUST* agree to come to Gondolin!!!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'If you were to abandon us, Gondolin will be RAZED! Permanently!'");
		if (get_check("You *MUST* agree to come to Gondolin!!!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'Everything you had in your Gondolin home will be lost forever!'");
		if (get_check("You *MUST* agree to come to Gondolin!!!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'You'll lose access to all the shops and services in Gondolin if you don't come!'");
		if (get_check("You *MUST* agree to come to Gondolin!!!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'This is your last chance! Just hit the 'y' key on your keyboard, it really can't be that hard!'");
		cmsg_print(TERM_YELLOW, "'And yes I know I just broke the fourth wall but I'm serious, Gondolin will fall unless you help us!'");
		if (get_check("You ***M*U*S*T*** agree to come to Gondolin!!! Or ALL is lost!")) goto rightdecision;

		cmsg_print(TERM_YELLOW, "'Turgon overestimated you... Now Gondolin will probably fall.'");
		cmsg_print(TERM_YELLOW, "'But I beseech you, come to Gondolin before it is too late!'");
		cmsg_print(TERM_YELLOW, "'Morgoth's forces will grow stronger the longer you wait!'");

		xtra_maeglin = TRUE;

		/*cquest.status = QUEST_STATUS_FAILED;
		town_info[2].destroyed = TRUE;*/

		quest_describe(QUEST_INVASION);
		cquest.status = QUEST_STATUS_TAKEN;
		quest_invasion_init_hook(QUEST_INVASION);

		quick_messages = old_quick_messages;

		del_hook(HOOK_END_TURN, quest_invasion_turn_hook);
		process_hooks_restart = TRUE;
		return (FALSE);
	}
rightdecision:

	cmsg_print(TERM_YELLOW, "'You made the right decision! Quickly, jump on Tolan!'");
	cmsg_print(TERM_YELLOW, "'Here we are: Gondolin. You must speak with Turgon now.'");

	p_ptr->wild_mode = FALSE;
	p_ptr->wilderness_x = 49;
	p_ptr->wilderness_y = 11;
	p_ptr->town_num = 2;
	p_ptr->oldpx = p_ptr->px = 117;
	p_ptr->oldpy = p_ptr->py = 24;
	dun_level = 0;
	p_ptr->leaving = TRUE;

	cmsg_print(TERM_YELLOW, "'Turgon hails you.'");
	quest_describe(QUEST_INVASION);
	cquest.status = QUEST_STATUS_TAKEN;

	quick_messages = old_quick_messages;

	quest_invasion_init_hook(QUEST_INVASION);
	del_hook(HOOK_END_TURN, quest_invasion_turn_hook);
	process_hooks_restart = TRUE;
	return (FALSE);
}
bool quest_invasion_dump_hook(char *fmt)
{
	if (cquest.status == QUEST_STATUS_FAILED)
	{
		fprintf(hook_file, "\n You abandoned Gondolin when it most needed you, thus causing its destruction.");
	}
	if ((cquest.status == QUEST_STATUS_FINISHED) || (cquest.status == QUEST_STATUS_REWARDED) || (cquest.status == QUEST_STATUS_COMPLETED))
	{
		fprintf(hook_file, "\n You saved Gondolin from destruction.");
	}
	return (FALSE);
}
bool quest_invasion_death_hook(char *fmt)
{
	s32b r_idx, m_idx;

	m_idx = get_next_arg(fmt);
	r_idx = m_list[m_idx].r_idx;

	if (p_ptr->inside_quest != QUEST_INVASION) return FALSE;

	if (r_idx == test_monster_name("Maeglin, the Traitor of Gondolin"))
	{
		cmsg_print(TERM_YELLOW, "You did it! Gondolin will remain hidden.");

		do_get_new_skill(0);
		p_ptr->skill_points += 5;
		cmsg_format(TERM_L_GREEN, "You can increase %d more skills.", p_ptr->skill_points);

		cquest.status = QUEST_STATUS_COMPLETED;
		del_hook(HOOK_MONSTER_DEATH, quest_invasion_death_hook);
		process_hooks_restart = TRUE;
		return (FALSE);
	}

	return FALSE;
}
bool quest_invasion_stair_hook(char *fmt)
{
	cptr down;

	down = get_next_arg_str(fmt);

	if (p_ptr->inside_quest != QUEST_INVASION) return FALSE;

	if (cave[p_ptr->py][p_ptr->px].feat != FEAT_LESS) return TRUE;

	if (!strcmp(down, "up"))
	{
		if (cquest.status == QUEST_STATUS_FAILED)
		{
			cmsg_print(TERM_YELLOW, "The armies of Morgoth totally devastated Gondolin, leaving nothing but ruins...");
		}
		else if (cquest.status == QUEST_STATUS_SCREWED)
		{
			cmsg_print(TERM_YELLOW, "The armies of Morgoth are upon Gondolin, you have to get back and stop Maeglin before it is too late!");
			quest_fail_penalty(3);
			cquest.status = QUEST_STATUS_TAKEN;
			do_cmd_go_up(TRUE);
			return (TRUE);
		}
		else if (cquest.status == QUEST_STATUS_COMPLETED)
		{
			cmsg_print(TERM_YELLOW, "Turgon appears before you and speaks:");
			cmsg_print(TERM_YELLOW, "'I will never be able to thank you enough.'");
			cmsg_print(TERM_YELLOW, "'My most powerful mages will cast a powerful spell for you, giving you extra life.'");

			p_ptr->hp_mod += 150;
			p_ptr->update |= (PU_HP);
			cquest.status = QUEST_STATUS_FINISHED;
		}
		else
		{
			/* Flush input */
			flush();

			if (!get_check("Really abandon the quest?")) return TRUE;
			cmsg_print(TERM_YELLOW, "You flee away from Maeglin and his army...");
			/*cquest.status = QUEST_STATUS_FAILED;
			town_info[2].destroyed = TRUE;*/
			quest_fail_penalty(3);
			do_cmd_go_up(TRUE);
			return (TRUE);
		}
		del_hook(HOOK_STAIR, quest_invasion_stair_hook);
		process_hooks_restart = TRUE;
		return (FALSE);
	}

	return TRUE;
}
bool quest_invasion_init_hook(int q_idx)
{
	add_hook(HOOK_END_TURN, quest_invasion_turn_hook, "invasion_turn");
	add_hook(HOOK_CHAR_DUMP, quest_invasion_dump_hook, "invasion_dump");
	if ((cquest.status >= QUEST_STATUS_TAKEN) && (cquest.status < QUEST_STATUS_FINISHED))
	{
		add_hook(HOOK_MONSTER_AI, quest_invasion_ai_hook, "invasion_ai");
		add_hook(HOOK_GEN_QUEST, quest_invasion_gen_hook, "invasion_gen");
		add_hook(HOOK_MONSTER_DEATH, quest_invasion_death_hook, "invasion_death");
		add_hook(HOOK_STAIR, quest_invasion_stair_hook, "invasion_stair");
	}
	return (FALSE);
}
