-- handle the lorien school

LORIEN_SLEEP_POWDER = add_spell
{
	["name"] =      "Sleep Powder",
	["school"] =    {SCHOOL_LORIEN},
	["level"] =     1,
	["mana"] =      50,
	["mana_max"] =  200,
	["fail"] =      5,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] =     function()
		local ret, dir, type
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_OLD_SLEEP, dir, 30 + get_level(LORIEN_SLEEP_POWDER, 120), 3 + get_level(LORIEN_SLEEP_POWDER, 3))
	end,
	["info"] =      function()
		return "power "..(30 + get_level(LORIEN_SLEEP_POWDER, 120)).." rad "..(3 + get_level(LORIEN_SLEEP_POWDER, 3))
	end,
	["desc"] =      {
			"Shoots a ball of sleep at monsters."
	}
}

LORIEN_COLLUSION = add_spell
{
	["name"] = 	"Collusion",
	["school"] = 	{SCHOOL_LORIEN},
	["level"] = 	15,
	["mana"] = 	200,
	["mana_max"] = 	400,
	["fail"] = 	25,
	["stick"] =
	{
			["charge"] =    { 5, 7 },
			[TV_WAND] =
			{
				["rarity"] = 		15,
				["base_level"] =	{ 1, 15 },
				["max_level"] =		{ 25, 50 },
			},
	},
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = 	function()
			local ret, dir, type

			ret, dir = get_aim_dir()
			if ret == FALSE then return end
			fire_cloud(GF_DARK, dir, 10 + get_level(LORIEN_COLLUSION, 240), 3, 5 + get_level(LORIEN_COLLUSION, 50))
			return TRUE
	end,
	["info"] = 	function()
			return "dam "..(10 + get_level(LORIEN_COLLUSION, 240)).." rad 3 dur "..(5 + get_level(LORIEN_COLLUSION, 50))
	end,
	["desc"] =	{
			"Creates a cloud of darkness",
			"The cloud will persist for some turns, damaging all monsters passing by",
	}
}

LORIEN_TIME_OUT = add_spell 
{ 
	["name"] =	"Time Out",
	["school"] =	{SCHOOL_LORIEN},
	["level"] =	30,
	["mana"] =	1000,
	["mana_max"] =	4000,
	["fail"] =	60,
	["piety"] =	TRUE,
	["stat"] =	A_WIS,
	["random"] =	SKILL_SPIRITUALITY,
	["spell"] =	function()
		local obvious
		obvious = is_obvious(fire_ball(GF_STONE_WALL, 0, 20 + get_level(LORIEN_TIME_OUT, 50), 3 + get_level(GLOBELIGHT, 2)), obvious)
		return obvious
	end,
	["info"] =	function()
		return "dam "..(20 + get_level(LORIEN_TIME_OUT, 50)).." rad "..(3 + get_level(LORIEN_TIME_OUT, 2))
	end,
	["desc"] =	{
		"Creates walls around you",
	}
}

