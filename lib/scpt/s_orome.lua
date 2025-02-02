-- handle the orome school

OROME_WILDSENSE = add_spell
{
	["name"] = "Wilderness Sense",
	["school"] = SCHOOL_OROME,
	["level"] = 1,
	["mana"] = 50,
	["mana_max"] = 300,
	["fail"] = 15,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
	end,
	["info"] = function()
		return "level "..(get_level(OROME_WILDSENSE, 40) + 20)
	end,
	["desc"] =
	{
		"bla bla bla",
	},
}

OROME_BULLSEYE = add_spell
{
	["name"] = "Bullseye",
	["school"] = SCHOOL_OROME,
	["level"] = 1,
	["mana"] = 50,
	["mana_max"] = 300,
	["fail"] = 15,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
	end,
	["info"] = function()
		return "level "..(get_level(OROME_BULLSEYE, 40) + 20)
	end,
	["desc"] =
	{
		"bla bla bla",
	},
}

OROME_SNIPER = add_spell
{
	["name"] = "Sniper",
	["school"] = SCHOOL_OROME,
	["level"] = 1,
	["mana"] = 50,
	["mana_max"] = 300,
	["fail"] = 15,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
	end,
	["info"] = function()
		return "level "..(get_level(OROME_SNIPER, 40) + 20)
	end,
	["desc"] =
	{
		"bla bla bla",
	},
}

OROME_RAPIDFIRE = add_spell
{
	["name"] = "Rapidfire",
	["school"] = SCHOOL_OROME,
	["level"] = 1,
	["mana"] = 50,
	["mana_max"] = 300,
	["fail"] = 15,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
	end,
	["info"] = function()
		return "level "..(get_level(OROME_RAPIDFIRE, 40) + 20)
	end,
	["desc"] =
	{
		"bla bla bla",
	},
}

OROME_GOODHUNT = add_spell
{
	["name"] = "Good Hunting",
	["school"] = SCHOOL_OROME,
	["level"] = 1,
	["mana"] = 50,
	["mana_max"] = 300,
	["fail"] = 15,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
	end,
	["info"] = function()
		return "level "..(get_level(OROME_GOODHUNT, 40) + 20)
	end,
	["desc"] =
	{
		"bla bla bla",
	},
}

