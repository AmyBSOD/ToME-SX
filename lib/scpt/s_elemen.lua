-- handle the elemental school

function get_firelightning_dam()
	return 4 + get_level(FIRELIGHTNING, 60), 2 + get_level(FIRELIGHTNING, 40)
end

FIRELIGHTNING = add_spell
{
	["name"] = 	"Fire Lightning",
	["school"] = 	SCHOOL_ELEMENTAL,
	["level"] = 	1,
	["mana"] = 	2,
	["mana_max"] =  50,
	["fail"] = 	10,
	["stick"] =
	{
			["charge"] =    { 10, 20 },
			[TV_WAND] =
			{
				["rarity"] = 		5,
				["base_level"] =	{ 1, 20 },
				["max_level"] =		{ 20, 45 },
			},
	},
	["spell"] = 	function()
			local ret, dir	

			local elemtypes =
				{
				GF_FIRE,
				GF_ELEC,
				}

			ret, dir = get_aim_dir()
			if ret == FALSE then return end
			return fire_bolt(elemtypes[rand_range(1, 2)], dir, damroll(get_firelightning_dam()))
	end,
	["info"] = 	function()
			local x, y

			x, y = get_firelightning_dam()
			return "dam "..x.."d"..y
	end,
	["desc"] =	{
			"Shoots a bolt that hits a single target",
			"It randomly deals either fire or lightning damage"
		}
}

COLDBALL = add_spell
{
	["name"] =      "Cold Ball",
	["school"] =    SCHOOL_ELEMENTAL,
	["level"] =     5,
	["mana"] =      10,
	["mana_max"] =  60,
	["fail"] =      15,
	["spell"] =     function()
		local ret, dir, type

		if (get_level(COLDBALL, 50) >= 40) then
			type = GF_ICE
		else
			type = GF_COLD
		end

		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_ball(type, dir, 12 + get_level(COLDBALL, 250), 3 + get_level(COLDBALL, 5))
	end,
	["info"] =      function()
		return "dam "..(12 + get_level(COLDBALL, 250)).." rad "..(3 + get_level(COLDBALL, 5))
	end,
	["desc"] =      {
			"Shoots a ball of cold at the enemy",
			"At level 40 it deals ice damage instead"
	}
}

ACIDRAIN = add_spell
{
	["name"] = 	"Acid Rain",
	["school"] = 	{SCHOOL_ELEMENTAL},
	["level"] = 	10,
	["mana"] = 	5,
	["mana_max"] = 	45,
	["fail"] = 	20,
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
	["spell"] = 	function()
			local ret, dir, type

			ret, dir = get_aim_dir()
			if ret == FALSE then return end
			fire_cloud(GF_ACID, dir, 8 + get_level(ACIDRAIN, 220), 3, 5 + get_level(ACIDRAIN, 15))
			return TRUE
	end,
	["info"] = 	function()
			return "dam "..(8 + get_level(ACIDRAIN, 220)).." rad 3 dur "..(5 + get_level(ACIDRAIN, 15))
	end,
	["desc"] =	{
			"Creates a cloud of acid",
			"The cloud will persist for some turns, damaging all monsters passing by"
	}
}

function get_lightningbolt_damage()
	return get_level(LIGHTNINGBOLT, 20), 10 + get_level(LIGHTNINGBOLT, 80)
end

LIGHTNINGBOLT = add_spell
{
	["name"] = "Lightning Bolt",
	["school"] = SCHOOL_ELEMENTAL,
	["level"] = 15,
	["mana"] = 18,
	["mana_max"] = 70,
	["fail"] = 30,
	["stick"] =
	{
			["charge"] =    { 10, 15 },
			[TV_WAND] =
			{
				["rarity"] = 		30,
				["base_level"] =	{ 1, 20 },
				["max_level"] =		{ 30, 50 },
			},
	},
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_beam(GF_ELEC, dir, damroll(get_lightningbolt_damage()))
	end,
	["info"] = function()
		local n, d
		n, d = get_lightningbolt_damage()
		return "dam "..n.."d"..d
	end,
	["desc"] =
	{
		"Shoots a lightning bolt from your fingertips.",
		"It hits all monsters in a straight line."
	},
}

ETHERSPREAD = add_spell
{
	["name"] = 	"Ether Spread",
	["school"] = 	{SCHOOL_ELEMENTAL},
	["level"] = 	40,
	["mana"] = 	50,
	["mana_max"] = 	250,
	["fail"] = 	70,
	["stick"] =
	{
			["charge"] =    { 5, 8 },
			[TV_WAND] =
			{
				["rarity"] = 		60,
				["base_level"] =	{ 1, 10 },
				["max_level"] =		{ 10, 30 },
			},
	},
	["spell"] = 	function()
			fire_cloud(GF_ETHER, 0, 40 + get_level(ETHERSPREAD, 50), 2 + get_level(ETHERSPREAD, 4), 3 + get_level(ETHERSPREAD, 3) )
			return TRUE
	end,
	["info"] = 	function()
       			return "dam "..(40 + get_level(ETHERSPREAD, 50)).." rad "..(2 + get_level(ETHERSPREAD, 4)).." dur "..(3 + get_level(ETHERSPREAD, 3))
	end,
	["desc"] =	{
			"Fills the air with ether to continuously damage enemies."
	}
}

