-- handle the vaire school

function get_ticktock_damage()
	return get_level(VAIRE_TICK_TOCK, 10), 3 + get_level(VAIRE_TICK_TOCK, 35)
end

VAIRE_TICK_TOCK = add_spell
{
	["name"] = "Tick Tock",
	["school"] = SCHOOL_VAIRE,
	["level"] = 1,
	["mana"] = 50,
	["mana_max"] = 400,
	["fail"] = 5,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		return fire_bolt_or_beam(2 * get_level(VAIRE_TICK_TOCK, 85), GF_TIME, dir, damroll(get_ticktock_damage()))
	end,
	["info"] = function()
		local n, d
		n, d = get_ticktock_damage()
		return "dam "..n.."d"..d
	end,
	["desc"] =
	{
		"Shoots a bolt of time from your fingertips.",
		"Sometimes it can blast through its first target."
	},
}

VAIRE_SPIDER_FORM = add_spell
{
	["name"] = 	"Spider Form",
	["school"] = 	{SCHOOL_VAIRE},
	["level"] = 	15,
	["mana"] = 	200,
	["mana_max"] = 	1000,
	["fail"] = 	30,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = 	function()
			return set_mimic(get_level(VAIRE_SPIDER_FORM, 75) + 1, resolve_mimic_name("Arachnid"), player.lev)
	end,
	["info"] = 	function()
			return "dur "..(get_level(VAIRE_SPIDER_FORM, 75)).."+1"
	end,
	["desc"] =	{
			"Temporarily turns you into a spider",
	}
}

VAIRE_TIMELINE = add_spell
{
	["name"] = 	"Timeline",
	["school"] = 	{SCHOOL_VAIRE},
	["level"] = 	20,
	["mana"] = 	250,
	["mana_max"] = 	800,
	["fail"] = 	45,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = 	function()
		local ret, dir
		ret, dir = get_aim_dir()
		if ret == FALSE then return end
		fire_wall(GF_TIME, dir, 20 + get_level(VAIRE_TIMELINE, 130), 5 + get_level(VAIRE_TIMELINE, 10))
		return TRUE
	end,
	["info"] = 	function()
		return "dam "..(20 + get_level(VAIRE_TIMELINE, 130)).." dur "..(5 + get_level(VAIRE_TIMELINE, 10))
	end,
	["desc"] =	{
			"Creates a time wall that affects monsters who walk into it",
	}
}

VAIRE_DECONTAMINATE = add_spell
{
	["name"] = 	"Decontaminate",
	["school"] = 	{SCHOOL_VAIRE},
	["level"] = 	30,
	["mana"] = 	1000,
	["mana_max"] = 	1000,
	["fail"] = 	40,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = 	function()
			if player.contamination > 0 then 
				player.contamination = player.contamination / 2
				msg_print("You feel less contaminated.")
			end
			return TRUE
	end,
	["info"] = 	function()
			return ""
	end,
	["desc"] =	{
			"Reduces your contamination."
	}
}

VAIRE_TIME_TEMPEST = add_spell
{
	["name"] = 	"Time Tempest",
	["school"] = 	{SCHOOL_VAIRE},
	["level"] = 	40,
	["mana"] = 	800,
	["mana_max"] = 	2000,
	["fail"] = 	66,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = 	function()
			if player.tim_ttempest == 0 then return set_tim_ttempest(randint(10) + 10 + get_level(VAIRE_TIME_TEMPEST, 25), 5 + get_level(VAIRE_TIME_TEMPEST, 10), 10 + get_level(VAIRE_TIME_TEMPEST, 25)) end
			return FALSE
	end,
	["info"] = 	function()
			return "dam "..(5 + get_level(VAIRE_TIME_TEMPEST, 10)).."d"..(10 + get_level(VAIRE_TIME_TEMPEST, 25)).." dur "..(10 + get_level(VAIRE_TIME_TEMPEST, 25)).."+d10"
	end,
	["desc"] =	{
			"Starts a time-based thunderstorm",
			"Each turn it hits a random monster in sight with time damage"
	}
}

