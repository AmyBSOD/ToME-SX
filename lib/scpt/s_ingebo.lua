-- handle the ingeborg school

INGEBORG_EVERYTHING_BURNS = add_spell
{
	["name"] = "Everything Burns",
	["school"] = SCHOOL_INGEBORG,
	["level"] = 1,
	["mana"] = 100,
	["mana_max"] = 1000,
	["fail"] = 10,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		return project_los(GF_FIRE, 2 + get_level(INGEBORG_EVERYTHING_BURNS, 120) )
	end,
	["info"] = function()
		return "dmg "..(2 + get_level(INGEBORG_EVERYTHING_BURNS, 120))
	end,
	["desc"] =
	{
		"Deals fire damage to every monster in your field of view.",
	},
}

INGEBORG_WILD_MAGIC = add_spell
{
	["name"] = "Wild Magic",
	["school"] = SCHOOL_INGEBORG,
	["level"] = 20,
	["mana"] = 100,
	["mana_max"] = 300,
	["fail"] = 30,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		wild_magic(randint(8) + randint(get_level(INGEBORG_WILD_MAGIC, 15)))
		return TRUE
	end,
	["info"] = function()
		return "lev "..(8 + get_level(INGEBORG_WILD_MAGIC, 15))
	end,
	["desc"] =
	{
		"Causes very random effects.",
	},
}

INGEBORG_DISINTEGRATE = add_spell
{
	["name"] = "Disintegrate",
	["school"] = SCHOOL_INGEBORG,
	["level"] = 30,
	["mana"] = 500,
	["mana_max"] = 1500,
	["fail"] = 50,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir, rad
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_DISINTEGRATE, dir, 200 + get_level(INGEBORG_DISINTEGRATE, 600), 2 + get_level(INGEBORG_DISINTEGRATE, 2) )
	end,
	["info"] = function()
		return "dmg "..(200 + get_level(INGEBORG_DISINTEGRATE, 600)).." rad "..(2 + get_level(INGEBORG_DISINTEGRATE, 2))
	end,
	["desc"] =
	{
		"Fires a ball of disintegration.",
	},
}

INGEBORG_STAR_DESTRUCTION = add_spell
{
	["name"] = "Star Destruction",
	["school"] = SCHOOL_INGEBORG,
	["level"] = 50,
	["mana"] = 10000,
	["mana_max"] = 25000,
	["fail"] = 75,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		destroy_area(player.py, player.px, 15 + get_level(INGEBORG_STAR_DESTRUCTION, 5), TRUE, FALSE)
		return TRUE
	end,
	["info"] = function()
		return "rad "..(15 + get_level(INGEBORG_STAR_DESTRUCTION, 5))
	end,
	["desc"] =
	{
		"Destroys the area around you.",
	},
}

INGEBORG_BLACK_HOLE = add_spell
{
	["name"] = "Black Hole",
	["school"] = {SCHOOL_INGEBORG, SCHOOL_UDUN},
	["level"] = 66,
	["mana"] = 500,
	["mana_max"] = 3000,
	["fail"] = 90,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir, rad
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_INERTIA, dir, 100 + get_level(INGEBORG_BLACK_HOLE, 450), 3 + get_level(INGEBORG_BLACK_HOLE, 5) )
	end,
	["info"] = function()
		return "dmg "..(100 + get_level(INGEBORG_BLACK_HOLE, 450)).." rad "..(3 + get_level(INGEBORG_BLACK_HOLE, 5))
	end,
	["desc"] =
	{
		"Fires a ball of inertia.",
	},
}

