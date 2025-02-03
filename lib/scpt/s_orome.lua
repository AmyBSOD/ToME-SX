-- handle the orome school

OROME_WILDSENSE = add_spell
{
	["name"] = "Wilderness Sense",
	["school"] = SCHOOL_OROME,
	["level"] = 1,
	["mana"] = 60,
	["mana_max"] = 200,
	["fail"] = 10,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local obvious
		obvious = is_obvious(set_tim_esp_animal(10 + get_level(OROME_WILDSENSE, 75)), obvious)
		return TRUE
	end,
	["info"] = function()
		return "dur "..(10 + get_level(OROME_WILDSENSE, 75))
	end,
	["desc"] =
	{
		"Allows you to sense animal minds for a while.",
	},
}

OROME_BULLSEYE = add_spell
{
	["name"] = "Bullseye",
	["school"] = SCHOOL_OROME,
	["level"] = 10,
	["mana"] = 200,
	["mana_max"] = 800,
	["fail"] = 25,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local obvious
		obvious = is_obvious(set_tim_bullseye(20 + get_level(OROME_BULLSEYE, 60)), obvious)
		return TRUE
	end,
	["info"] = function()
		return "dur "..(20 + get_level(OROME_BULLSEYE, 60))
	end,
	["desc"] =
	{
		"Temporarily boosts your ranged weapon to-hit and damage.",
	},
}

OROME_SNIPER = add_spell
{
	["name"] = "Sniper",
	["school"] = SCHOOL_OROME,
	["level"] = 25,
	["mana"] = 300,
	["mana_max"] = 1200,
	["fail"] = 40,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local obvious
		obvious = is_obvious(set_tim_sniper(15 + get_level(OROME_SNIPER, 75)), obvious)
		return TRUE
	end,
	["info"] = function()
		return "dur "..(15 + get_level(OROME_SNIPER, 75))
	end,
	["desc"] =
	{
		"Temporarily enchants your ranged weapon with extra might.",
	},
}

OROME_RAPIDFIRE = add_spell
{
	["name"] = "Rapidfire",
	["school"] = SCHOOL_OROME,
	["level"] = 40,
	["mana"] = 200,
	["mana_max"] = 1000,
	["fail"] = 45,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local obvious
		obvious = is_obvious(set_tim_rapidfire(40 + get_level(OROME_RAPIDFIRE, 200)), obvious)
		return TRUE
	end,
	["info"] = function()
		return "dur "..(40 + get_level(OROME_RAPIDFIRE, 200))
	end,
	["desc"] =
	{
		"Temporarily enchants your ranged weapon with extra shots.",
	},
}

OROME_GOODHUNT = add_spell
{
	["name"] = "Good Hunting",
	["school"] = SCHOOL_OROME,
	["level"] = 30,
	["mana"] = 50,
	["mana_max"] = 300,
	["fail"] = 25,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
			local y, x, m_idx, tempidx
			local summons =
				{
				271,
				272,
				282,
				307,
				308,
				309,
				337,
				338,
				340,
				428, --level 27
				429, --level 27
				513, --level 33
				540, --level 35
				542, --level 35
				543, --level 35
				1038, --level 43
				724, --level 51
				725, --level 51
				726, --level 51
				779, --level 65
				}
			y, x = find_position(player.py, player.px)
			tempidx = summons[rand_range(1, 9)]
			if get_level(OROME_GOODHUNT, 50) >= 7 then
				tempidx = summons[rand_range(1, 11)]
			end
			if get_level(OROME_GOODHUNT, 50) >= 13 then
				tempidx = summons[rand_range(1, 12)]
			end
			if get_level(OROME_GOODHUNT, 50) >= 15 then
				tempidx = summons[rand_range(1, 15)]
			end
			if get_level(OROME_GOODHUNT, 50) >= 23 then
				tempidx = summons[rand_range(1, 16)]
			end
			if get_level(OROME_GOODHUNT, 50) >= 31 then
				tempidx = summons[rand_range(1, 19)]
			end
			if get_level(OROME_GOODHUNT, 50) >= 45 then
				tempidx = summons[rand_range(1, 20)]
			end
			m_idx = place_monster_one(y, x, tempidx, 0, FALSE, MSTATUS_FRIEND)
			if m_idx ~= 0 then
				monster_set_level(m_idx, 20 + get_level(OROME_GOODHUNT, 50))
				return TRUE
			end
	end,
	["info"] = function()
		return "level "..(get_level(OROME_GOODHUNT, 50) + 20)
	end,
	["desc"] =
	{
		"Summons a leveled hound to your aid.",
	},
}

