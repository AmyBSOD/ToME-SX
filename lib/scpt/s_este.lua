-- handle the este school

ESTE_DANCING_LESSONS = add_spell
{
	["name"] = "Dancing Lessons",
	["school"] = SCHOOL_ESTE,
	["level"] = 1,
	["mana"] = 50,
	["mana_max"] = 300,
	["fail"] = 5,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local obvious
		obvious = is_obvious(set_tim_dancing(100 + get_level(ESTE_DANCING_LESSONS, 300), get_level(ESTE_DANCING_LESSONS) ), obvious)
		return TRUE
	end,
	["info"] = function()
		return "dur "..(100 + get_level(ESTE_DANCING_LESSONS, 300)).." level "..(get_level(ESTE_DANCING_LESSONS))
	end,
	["desc"] =
	{
		"Temporarily improves your dodging chances.",
	},
}

ESTE_REVEAL_TRAP = add_spell
{
	["name"] = "Reveal Trap",
	["school"] = SCHOOL_ESTE,
	["level"] = 30,
	["mana"] = 300,
	["mana_max"] = 800,
	["fail"] = 50,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		detect_traps_nasty(10 + get_level(ESTE_REVEAL_TRAP, 30))
		return TRUE
	end,
	["info"] = function()
		return "rad "..(10 + get_level(ESTE_REVEAL_TRAP, 30))
	end,
	["desc"] =
	{
		"Detects traps, including nasty traps.",
	},
}

ESTE_DESTROY_TRAP = add_spell
{
	["name"] = "Destroy Trap",
	["school"] = SCHOOL_ESTE,
	["level"] = 40,
	["mana"] = 500,
	["mana_max"] = 500,
	["fail"] = 80,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		destroy_traps_touch_nasty()
		return TRUE
	end,
	["info"] = function()
		return ""
	end,
	["desc"] =
	{
		"Destroys adjacent traps, including nasty traps.",
	},
}

