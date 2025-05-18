-- handle the vana school

function get_icewind_damage()
	return get_level(VANA_ICY_WIND, 10), 3 + get_level(VANA_ICY_WIND, 35)
end

VANA_ICY_WIND = add_spell
{
	["name"] = "Icy Wind",
	["school"] = SCHOOL_VANA,
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
		return fire_bolt_or_beam(2 * get_level(VANA_ICY_WIND, 85), GF_ICE, dir, damroll(get_icewind_damage()))
	end,
	["info"] = function()
		local n, d
		n, d = get_icewind_damage()
		return "dam "..n.."d"..d
	end,
	["desc"] =
	{
		"Shoots a bolt of ice from your fingertips.",
		"Sometimes it can blast through its first target."
	},
}

VANA_FREEZING_CLOUD = add_spell
{
	["name"] = 	"Freezing Cloud",
	["school"] = 	{SCHOOL_VANA},
	["level"] = 	10,
	["mana"] = 	80,
	["mana_max"] = 	700,
	["fail"] = 	25,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = 	function()
			local ret, dir

			ret, dir = get_aim_dir()
			if ret == FALSE then return end
			fire_cloud(GF_ICE, dir, 7 + get_level(VANA_FREEZING_CLOUD, 150), 3, 5 + get_level(VANA_FREEZING_CLOUD, 40))
			return TRUE
	end,
	["info"] = 	function()
			return "dam "..(7 + get_level(VANA_FREEZING_CLOUD, 150)).." rad 3 dur "..(5 + get_level(VANA_FREEZING_CLOUD, 40))
	end,
	["desc"] =	{
			"Creates a cloud of ice",
			"The cloud will persist for some turns, damaging all monsters passing by",
	}
}

VANA_RESTOR = add_spell
{
	["name"] = 	"Divine Restoration",
	["school"] = 	{SCHOOL_VANA},
	["level"] = 	20,
	["mana"] = 	400,
	["mana_max"] = 	600,
	["fail"] = 	40,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = 	function()
			local obvious
			obvious = is_obvious(do_res_stat(A_STR, TRUE), obvious)
			obvious = is_obvious(do_res_stat(A_CON, TRUE), obvious)
			obvious = is_obvious(do_res_stat(A_DEX, TRUE), obvious)
			obvious = is_obvious(do_res_stat(A_WIS, TRUE), obvious)
			obvious = is_obvious(do_res_stat(A_INT, TRUE), obvious)
			obvious = is_obvious(do_res_stat(A_CHR, TRUE), obvious)
			if get_level(VANA_RESTOR, 50) >= 10 then
				obvious = is_obvious(restore_level(), obvious)
			end
			return obvious
	end,
	["info"] = 	function()
			return ""
	end,
	["desc"] =	{
			"Restores drained stats",
			"At level 10 it restores lost experience"
	}
}

VANA_PURIFY = add_spell
{
	["name"] = 	"Purification",
	["school"] = 	{SCHOOL_VANA},
	["level"] = 	35,
	["mana"] = 	5000,
	["mana_max"] = 	5000,
	["fail"] = 	75,
	-- Uses piety to cast
	["piety"] =     TRUE,
	["stat"] =      A_WIS,
	["random"] = 	SKILL_SPIRITUALITY,
	["spell"] = 	function()
			if (player.black_breath == TRUE) then 
				player.black_breath = FALSE
				msg_print("The hold of the Black Breath on you is broken!")
			end
			return TRUE
	end,
	["info"] = 	function()
			return ""
	end,
	["desc"] =	{
			"Cures the black breath."
	}
}

