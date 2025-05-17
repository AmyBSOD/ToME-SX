-- handle the hell school

NETHEREYE = add_spell
{
	["name"] = "Nether Eye",
	["school"] = SCHOOL_HELL,
	["level"] = 1,
	["mana"] = 2,
	["mana_max"] = 40,
	["fail"] = 5,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_bolt_or_beam(2 * get_level(NETHEREYE, 85), GF_NETHER, dir, damroll(get_nether_damage()))
	end,
	["info"] = function()
		local n, d
		n, d = get_nether_damage()
		return "dam "..n.."d"..d
	end,
	["desc"] =
	{
		"Shoots a bolt of nether from your fingertips.",
		"Sometimes it can blast through its first target."
	},
}

function get_nether_damage()
	return get_level(NETHEREYE, 11), 3 + get_level(NETHEREYE, 45)
end

GOODNIGHT = add_spell
{
	["name"] =      "Good Night",
	["school"] =    SCHOOL_HELL,
	["level"] =     10,
	["mana"] =      20,
	["mana_max"] =  80,
	["fail"] =      30,
	["spell"] =     function()
		local ret, dir, type
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_DARK, dir, 30 + get_level(GOODNIGHT, 500), 2 + get_level(GOODNIGHT, 4))
	end,
	["info"] =      function()
		return "dam "..(30 + get_level(GOODNIGHT, 500)).." rad "..(2 + get_level(GOODNIGHT, 4))
	end,
	["desc"] =      {
			"Fires a darkness ball at the enemy"
	}
}

NERVE_POISON = add_spell
{
	["name"] =      "Nerve Poison",
	["school"] =    {SCHOOL_HELL, SCHOOL_UDUN},
	["level"] =     20,
	["mana"] =      30,
	["mana_max"] =  100,
	["fail"] =      40,
	["spell"] =     function()
		local ret, dir, type
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_NERVE, dir, 20 + get_level(NERVE_POISON, 300), 2 + get_level(NERVE_POISON, 3))
	end,
	["info"] =      function()
		return "dam "..(20 + get_level(NERVE_POISON, 300)).." rad "..(2 + get_level(NERVE_POISON, 3))
	end,
	["desc"] =      {
			"Fires a ball of nerve gas at the enemy"
	}
}

ROCKETLAUNCHER = add_spell
{
	["name"] =      "Rocket Launcher",
	["school"] =    SCHOOL_HELL,
	["level"] =     40,
	["mana"] =      150,
	["mana_max"] =  450,
	["fail"] =      70,
	["stick"] =
	{
			["charge"] =    { 3, 4 },
			[TV_WAND] =
			{
				["rarity"] = 		80,
				["base_level"] =	{ 1, 4 },
				["max_level"] =		{ 10, 20 },
			},
	},
	["spell"] =     function()
		local ret, dir, type
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_ROCKET, dir, 250 + get_level(ROCKETLAUNCHER, 400), 2 + get_level(ROCKETLAUNCHER, 3))
	end,
	["info"] =      function()
		return "dam "..(250 + get_level(ROCKETLAUNCHER, 400)).." rad "..(2 + get_level(ROCKETLAUNCHER, 3))
	end,
	["desc"] =      {
			"Fires a powerful rocket at the enemy"
	}
}

TIMELAPSE = add_spell
{
	["name"] = 	"Timelapse",
	["school"] = 	SCHOOL_HELL,
	["level"] = 	30,
	["mana"] = 	60,
	["mana_max"] = 	300,
	["fail"] = 	50,
	["stick"] =
	{
			["charge"] =    { 4, 5 },
			[TV_WAND] =
			{
				["rarity"] = 		60,
				["base_level"] =	{ 1, 8 },
				["max_level"] =		{ 15, 25 },
			},
	},
	["spell"] = 	function()
			fire_cloud(GF_TIME, 0, 3 + get_level(TIMELAPSE, 60), 3 + get_level(TIMELAPSE, 4, 0), 5 + get_level(TIMELAPSE, 10) )
			return TRUE
	end,
	["info"] = 	function()
       			return "dam "..(3 + get_level(TIMELAPSE, 60)).." rad "..(3 + get_level(TIMELAPSE, 4, 0)).." dur "..(5 + get_level(TIMELAPSE, 10))
	end,
	["desc"] =	{
			"Creates a time field, constantly damaging enemies who walk into it."
	}
}

NUCLEARMISSILE = add_spell
{
	["name"] =      "Nuclear Missile",
	["school"] =    SCHOOL_HELL,
	["level"] =     75,
	["mana"] =      500,
	["mana_max"] =  1000,
	["fail"] =      95,
	["spell"] =     function()
		local ret, dir, type
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(GF_NUKE, dir, 1000 + get_level(NUCLEARMISSILE, 2000), 2 + get_level(NUCLEARMISSILE, 8))
	end,
	["info"] =      function()
		return "dam "..(1000 + get_level(NUCLEARMISSILE, 2000)).." rad "..(2 + get_level(NUCLEARMISSILE, 8))
	end,
	["desc"] =      {
			"Launches a highly damaging nuclear missile"
	}
}

