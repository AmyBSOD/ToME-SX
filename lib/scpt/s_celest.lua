-- handle the celestial school

FORCEBOLT = add_spell
{
	["name"] = "Force Bolt",
	["school"] = SCHOOL_CELESTIAL,
	["level"] = 1,
	["mana"] = 4,
	["mana_max"] = 100,
	["fail"] = 10,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_bolt_or_beam(2 * get_level(FORCEBOLT, 50), GF_FORCE, dir, damroll(get_force_damage()))
	end,
	["info"] = function()
		local n, d
		n, d = get_force_damage()
		return "dam "..n.."d"..d
	end,
	["desc"] =
	{
		"Shoots a bolt of force from your buttocks.",
		"Sometimes it can blast through its first target."
	},
}

function get_force_damage()
	return get_level(FORCEBOLT, 5), 2 + get_level(FORCEBOLT, 30)
end

CLEANINGFIRE = add_spell
{
	["name"] = 	"Cleaning Fire",
	["school"] = 	SCHOOL_CELESTIAL,
	["level"] = 	10,
	["mana"] = 	10,
	["mana_max"] = 	60,
	["fail"] = 	35,
	["spell"] = 	function()
			fire_cloud(GF_PLASMA, 0, 2 + get_level(CLEANINGFIRE, 25), 2 + get_level(CLEANINGFIRE, 3, 0), 3 + get_level(CLEANINGFIRE, 4) )
			return TRUE
	end,
	["info"] = 	function()
       			return "dam "..(2 + get_level(CLEANINGFIRE, 25)).." rad "..(2 + get_level(CLEANINGFIRE, 3, 0)).." dur "..(3 + get_level(CLEANINGFIRE, 4))
	end,
	["desc"] =	{
			"Fills the air around you with extremely hot plasma to eradicate the enemy."
	}
}

GROUNDATTACK = add_spell
{
	["name"] =      "Ground Attack",
	["school"] =    SCHOOL_CELESTIAL,
	["level"] = 35,
	["mana"] = 100,
	["mana_max"] = 300,
	["fail"] = 40,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_bolt_or_beam(2 * get_level(GROUNDATTACK, 25), GF_GRAVITY, dir, damroll(get_ground_damage()))
	end,
	["info"] = function()
		local n, d
		n, d = get_ground_damage()
		return "dam "..n.."d"..d
	end,
	["desc"] =
	{
		"Shoots a bolt of gravity from your fingertips.",
		"Sometimes it can blast through its first target."
	},
}

function get_ground_damage()
	return get_level(GROUNDATTACK, 15), 10 + get_level(GROUNDATTACK, 40)
end


CRYSTALSHARDS = add_spell
{
	["name"] =      "Crystal Shards",
	["school"] =    SCHOOL_CELESTIAL,
	["level"] =     40,
	["mana"] =      200,
	["mana_max"] =  600,
	["fail"] =      80,
	["spell"] =     function()
		return project_los(GF_SHARDS, 80 + get_level(CRYSTALSHARDS, 150) )
	end,
	["info"] =      function()
		return "dam "..(80 + get_level(CRYSTALSHARDS, 400))
	end,
	["desc"] =      {
			"Hits everyone in the proximity with razor-sharp crystal shards"
	}
}

TUNGUSKA = add_spell
{
	["name"] =      "Tunguska Event",
	["school"] =    SCHOOL_CELESTIAL,
	["level"] =     60,
	["mana"] =      400,
	["mana_max"] =  1200,
	["fail"] =      95,
	["spell"] =     function()
		local ret, dir, type
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_METEOR, dir, 600 + get_level(TUNGUSKA, 3000), 2 + get_level(TUNGUSKA, 6))
	end,
	["info"] =      function()
		return "dam "..(600 + get_level(TUNGUSKA, 3000)).." rad "..(2 + get_level(TUNGUSKA, 6))
	end,
	["desc"] =      {
			"A gigantic meteor drops from the sky and explodes to wreck the enemy"
	}
}

