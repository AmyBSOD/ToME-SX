-- handle the nienna school

NIENNA_ALCHEMIZE = add_spell
{
	["name"] = "Alchemize",
	["school"] = SCHOOL_NIENNA,
	["level"] = 1,
	["mana"] = 200,
	["mana_max"] = 400,
	["fail"] = 10,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		do_cmd_alchemist(get_level(NIENNA_ALCHEMIZE))
		return TRUE
	end,
	["info"] = function()
		return "level "..(get_level(NIENNA_ALCHEMIZE))
	end,
	["desc"] =
	{
		"Allows you to improve items via alchemical essences.",
	},
}

NIENNA_RUNEFORGE = add_spell
{
	["name"] = "Runeforge",
	["school"] = SCHOOL_NIENNA,
	["level"] = 1,
	["mana"] = 200,
	["mana_max"] = 400,
	["fail"] = 10,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		do_cmd_runecrafter(get_level(NIENNA_RUNEFORGE))
		return TRUE
	end,
	["info"] = function()
		return "level "..(get_level(NIENNA_RUNEFORGE))
	end,
	["desc"] =
	{
		"Allows you to improve items via runes and runestones.",
	},
}

NIENNA_BOMB_SQUAD_DUTY = add_spell
{
	["name"] = "Bomb Squad Duty",
	["school"] = SCHOOL_NIENNA,
	["level"] = 10,
	["mana"] = 100,
	["mana_max"] = 200,
	["fail"] = 20,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local obvious
		obvious = is_obvious(set_tim_bombsquad(50 + get_level(NIENNA_BOMB_SQUAD_DUTY, 200)), obvious)
		return TRUE
	end,
	["info"] = function()
		return "dur "..(50 + get_level(NIENNA_BOMB_SQUAD_DUTY, 200))
	end,
	["desc"] =
	{
		"Temporarily boosts your ability to find and disarm traps.",
	},
}

NIENNA_BLOODY_TEARS = add_spell
{
	["name"] = "Bloody Tears",
	["school"] = SCHOOL_NIENNA,
	["level"] = 15,
	["mana"] = 200,
	["mana_max"] = 500,
	["fail"] = 30,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		project_los(GF_FEAR, 10 + get_level(NIENNA_BLOODY_TEARS, 55))
		if get_level(NIENNA_BLOODY_TEARS) >= 25 then
			project_los(GF_OLD_CONF, 10 + get_level(CONFUSE, 225))
		end
		set_cut(player.cut + 50 + get_level(NIENNA_BLOODY_TEARS))
		return TRUE
	end,
	["info"] = function()
		return "pow "..(10 + get_level(NIENNA_BLOODY_TEARS, 55))
	end,
	["desc"] =
	{
		"Causes you to harm yourself and bleed, scaring off monsters who have to watch your horrible act",
		"At level 25 it can also confuse monsters",
	},
}

NIENNA_WEEPING = add_spell
{
	["name"] = "Weeping",
	["school"] = {SCHOOL_NIENNA},
	["level"] = 25,
	["mana"] = 1000,
	["mana_max"] = 1100,
	["fail"] = 60,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		heal_insanity(20 + get_level(NIENNA_WEEPING, 16))
		return TRUE
	end,
	["info"] = function()
		return "pow "..(20 + get_level(NIENNA_WEEPING, 16))
	end,
	["desc"] =
	{
		"Restores some of your sanity.",
	},
}

