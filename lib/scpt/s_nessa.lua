-- handle the nessa school

NESSA_CONFUSION_FU = add_spell
{
	["name"] = "Confusion Fu",
	["school"] = SCHOOL_NESSA,
	["level"] = 1,
	["mana"] = 100,
	["mana_max"] = 200,
	["fail"] = 5,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
			return set_project(randint(20) + 20 + get_level(NESSA_CONFUSION_FU, 80),
				    GF_CONFUSION,
				    10 + get_level(NESSA_CONFUSION_FU, 10),
				    0,
				    bor(PROJECT_STOP, PROJECT_KILL))
	end,
	["info"] = function()
			return "dur "..(20 + get_level(NESSA_CONFUSION_FU, 80)).."+d20 dam "..(10 + get_level(NESSA_CONFUSION_FU, 10)).."/blow"
	end,
	["desc"] =
	{
		"Imbues your melee weapon with confusion damage",
	},
}

NESSA_SPEEDBOOST = add_spell
{
	["name"] = "Speedboost",
	["school"] = SCHOOL_NESSA,
	["level"] = 15,
	["mana"] = 300,
	["mana_max"] = 600,
	["fail"] = 25,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
			if player.fast == 0 then return set_fast(5 + randint(5) + get_level(NESSA_SPEEDBOOST, 20), 8 + get_level(NESSA_SPEEDBOOST, 35)) end
	end,
	["info"] = function()
		       	return "dur "..(5 + get_level(NESSA_SPEEDBOOST, 20)).."+d5 speed "..(8 + get_level(NESSA_SPEEDBOOST, 35))
	end,
	["desc"] =
	{
		"Speeds you up for a few turns",
	},
}

NESSA_MANA_STORM = add_spell
{
	["name"] = "Mana Storm",
	["school"] = SCHOOL_NESSA,
	["level"] = 30,
	["mana"] = 500,
	["mana_max"] = 2000,
	["fail"] = 60,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir, type
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_MANA, dir, 100 + get_level(NESSA_MANA_STORM, 500), 3 + get_level(NESSA_MANA_STORM, 4))
	end,
	["info"] = function()
		return "dam "..(100 + get_level(NESSA_MANA_STORM, 500)).." rad "..(3 + get_level(NESSA_MANA_STORM, 4))
	end,
	["desc"] =
	{
		"Casts a powerful ball of mana that can't be resisted",
	},
}

