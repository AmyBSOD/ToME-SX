-- Define the various possible mimic shapes

-- Nature shapes

-- abomination, but higher skill makes it better, by Amy
add_mimic_shape
{
	["name"] =      "Monstrosity",
	["obj_name"] =  "Monstrous Cloak",
	["desc"] = 	"Monsters are abominations of nature.",
	["realm"] =     "nature",
	["level"] =     1,
	["rarity"] =    10,
	["duration"] =  {20, 100},
	["calc"] =      function ()

			player.modify_stat(A_STR, -10 + (player.mimic_level / 8))
			player.modify_stat(A_WIS, -10 + (player.mimic_level / 8))
			player.modify_stat(A_DEX, -10 + (player.mimic_level / 8))
			player.modify_stat(A_CON, -10 + (player.mimic_level / 8))
			player.modify_stat(A_CHR, -10 + (player.mimic_level / 8))
			player.modify_stat(A_INT, -10 + (player.mimic_level / 8))

			player.pspeed = player.pspeed - 10 + (player.mimic_level / 8)

			player.aggravate = TRUE

	end,
}

add_mimic_shape
{
	["name"] =      "Mouse",
	["obj_name"] =  "Mouse Fur",
	["desc"] = 	"Mice are small, fast and very stealthy",
	["realm"] =     "nature",
	["level"] =     1,
	["rarity"] =    10,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			-- Mice run!
			player.pspeed = player.pspeed + 1 + (player.mimic_level / 12)

			-- They can crtawl under your armor to hit you ;)
			player.to_h = player.to_h + 10 + (player.mimic_level / 5)
			player.dis_to_h = player.dis_to_h + 10 + (player.mimic_level / 5)

			-- But they are not very powerfull
			player.to_d = player.to_d / 5
			player.dis_to_d = player.dis_to_d / 5

			-- But they are stealthy
			player.skill_stl = player.skill_stl + 10 + (player.mimic_level / 5)

			player.skill_sav = player.skill_sav - 10

			-- Stat mods
			player.modify_stat(A_STR, -15)
			player.modify_stat(A_DEX, 3)
			player.modify_stat(A_CON, 1)

	end,
	["power"] =     function()
			if player.mimic_level >= 30 then
				player.add_power(POWER_INVISIBILITY)
			end
	end,
}

add_mimic_shape
{
	["name"] =      "Goat",
	["obj_name"] =  "Goat Garment",
	["desc"] = 	"Goats are somewhat strong and can subsist on little food.",
	["realm"] =     "nature",
	["level"] =     15,
	["rarity"] =    10,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.slow_digest = TRUE
			player.modify_stat(A_STR, 2)
			player.modify_stat(A_INT, -10)
			player.modify_stat(A_CHR, -6)

			player.pspeed = player.pspeed + 1 + (player.mimic_level / 15)
			player.skill_srh = player.skill_srh + 1 + (player.mimic_level / 10)
			player.skill_fos = player.skill_fos + (player.mimic_level / 20)
	end,
}

add_mimic_shape
{
	["name"] =      "Insect",
	["obj_name"] =  "Insect Husk",
	["desc"] = 	"Insects are smart and fast, and can sense the presence of other insects.",
	["realm"] =     "nature",
	["level"] =     10,
	["rarity"] =    10,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, -10)
			player.modify_stat(A_INT, 5 + (player.mimic_level / 10))
			player.modify_stat(A_WIS, 5 + (player.mimic_level / 10))
			player.modify_stat(A_CON, -15)
			player.modify_stat(A_CHR, -12)
			player.pspeed = player.pspeed + 4 + (player.mimic_level / 9)
	end,
}

add_mimic_shape
{
	["name"] =      "Ape",
	["obj_name"] =  "Ape Hide",
	["desc"] = 	"Apes are pretty dextrous.",
	["realm"] =     "nature",
	["level"] =     15,
	["rarity"] =    10,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 1 + (player.mimic_level / 12))
			player.modify_stat(A_INT, -8)
			player.modify_stat(A_CHR, -5)
			player.modify_stat(A_DEX, 7 + (player.mimic_level / 5))
			player.to_h_ranged = player.to_h_ranged + (14 + (player.mimic_level / 10))
			player.to_h = player.to_h + (14 + (player.mimic_level / 10))
			player.skill_sav = player.skill_sav - 4
			player.skill_stl = player.skill_stl - 5

	end,
}

add_mimic_shape
{
	["name"] =      "Sparrow",
	["obj_name"] =  "Sparrow Feathers",
	["desc"] = 	"Sparrows can fly and are fast.",
	["realm"] =     "nature",
	["level"] =     20,
	["rarity"] =    15,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, -8)
			player.modify_stat(A_INT, -4)
			player.modify_stat(A_WIS, -4)
			player.modify_stat(A_CON, -4)
			player.modify_stat(A_CHR, 6 + (player.mimic_level / 10))
			player.pspeed = player.pspeed + 3 + (player.mimic_level / 7)
			player.fly = TRUE
			player.skill_dis = player.skill_dis - 3
			player.skill_dev = player.skill_dev - 2
			player.skill_dig = player.skill_dig - 2

	end,
}

add_mimic_shape
{
	["name"] =      "Vampire",
	["obj_name"] =  "Opera Cloak",
	["desc"] = 	"Vampires are undead and suck blood.",
	["realm"] =     "nature",
	["level"] =     30,
	["rarity"] =    50,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 2)
			player.modify_stat(A_INT, 4)
			player.modify_stat(A_WIS, -12)
			player.modify_stat(A_DEX, 5)
			player.modify_stat(A_CON, 3)
			player.modify_stat(A_CHR, -15)
			player.hold_life = TRUE
			player.resist_dark = TRUE
			player.resist_nerve = TRUE
			player.sensible_fire = TRUE
			player.sensible_lite = TRUE
			if player.mimic_level >= 10 then player.resist_blind = TRUE end
			if player.mimic_level >= 15 then player.resist_nexus = TRUE end
			player.lite = TRUE

	end,
}

add_mimic_shape
{
	["name"] =      "Mana Ball",
	["obj_name"] =  "Mana Veil",
	["desc"] = 	"Mana balls are barely substantial.",
	["realm"] =     "nature",
	["level"] =     35,
	["rarity"] =    70,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, -8)
			player.modify_stat(A_INT, 8 + (player.mimic_level / 12))
			player.modify_stat(A_WIS, 4 + (player.mimic_level / 15))
			player.modify_stat(A_DEX, 8 + (player.mimic_level / 12))
			player.modify_stat(A_CON, -15)
			player.modify_stat(A_CHR, -15)
			player.resist_ether = TRUE
			player.ffall = TRUE
			player.invis = player.invis + (10 + (player.mimic_level / 4))
			player.to_s = player.to_s + 1
			player.teleport = TRUE
			if player.mimic_level >= 25 then player.resist_disen = TRUE end
			player.skill_dis = player.skill_dis - 10
			player.skill_srh = player.skill_srh - 5
			player.skill_fos = player.skill_fos - 10
			player.skill_dig = player.skill_dig - 25

	end,
}

add_mimic_shape
{
	["name"] =      "Fire Cloud",
	["obj_name"] =  "Fire Mantle",
	["desc"] = 	"A cloud made of pure fire.",
	["realm"] =     "nature",
	["level"] =     30,
	["rarity"] =    50,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, -8)
			player.modify_stat(A_DEX, 6 + (player.mimic_level / 12))
			player.modify_stat(A_CON, -5)
			player.modify_stat(A_CHR, -8)
			player.resist_lite = TRUE
			player.immune_fire = TRUE
			player.sh_fire = TRUE
			player.skill_dis = player.skill_dis - 10
			player.skill_srh = player.skill_srh - 5
			player.skill_fos = player.skill_fos - 10
			player.skill_dig = player.skill_dig - 25
			player.to_h = player.to_h - 10
			player.dis_to_h = player.dis_to_h - 10

	end,
}

add_mimic_shape
{
	["name"] =      "Cold Cloud",
	["obj_name"] =  "Ice Mantle",
	["desc"] = 	"A cloud made of ice.",
	["realm"] =     "nature",
	["level"] =     30,
	["rarity"] =    50,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, -8)
			player.modify_stat(A_DEX, 6 + (player.mimic_level / 12))
			player.modify_stat(A_CON, 1)
			player.modify_stat(A_CHR, -8)
			player.resist_lite = TRUE
			player.immune_cold = TRUE
			player.sh_elec = TRUE
			player.regenerate = TRUE
			player.skill_dis = player.skill_dis - 10
			player.skill_srh = player.skill_srh - 5
			player.skill_fos = player.skill_fos - 10
			player.skill_dig = player.skill_dig - 25
			player.sensible_fire = TRUE
			player.to_h = player.to_h - 15
			player.dis_to_h = player.dis_to_h - 15

	end,
}

add_mimic_shape
{
	["name"] =      "Chaos Cloud",
	["obj_name"] =  "Chaos Overgarment",
	["desc"] = 	"A cloud made of pure chaos.",
	["realm"] =     "nature",
	["level"] =     25,
	["rarity"] =    60,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 2)
			player.modify_stat(A_DEX, 6 + (player.mimic_level / 12))
			player.modify_stat(A_CON, 1)
			player.modify_stat(A_CHR, 2)
			player.modify_stat(A_INT, 1)
			player.modify_stat(A_WIS, 1)
			player.resist_disen = TRUE
			player.resist_lite = TRUE
			player.resist_chaos = TRUE
			player.immune_cold = TRUE
			player.immune_fire = TRUE
			player.sh_elec = TRUE
			player.sh_fire = TRUE
			player.skill_dis = player.skill_dis - 20
			player.skill_srh = player.skill_srh - 10
			player.skill_fos = player.skill_fos - 20
			player.skill_dig = player.skill_dig - 40
			player.to_h = player.to_h - 40
			player.dis_to_h = player.dis_to_h - 40

	end,
}

add_mimic_shape
{
	["name"] =      "Ghost",
	["obj_name"] =  "Ghost Veil",
	["desc"] = 	"Ghosts are undead beings who walk through walls.",
	["realm"] =     "nature",
	["level"] =     20,
	["rarity"] =    80,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, -20)
			player.modify_stat(A_INT, 1)
			player.modify_stat(A_WIS, 1)
			player.modify_stat(A_DEX, 6 + (player.mimic_level / 12))
			player.modify_stat(A_CON, 2)
			player.modify_stat(A_CHR, -20)
			player.resist_nerve = TRUE
			player.wraith_form = TRUE
			player.hold_life = TRUE
			player.skill_dev = player.skill_dev - 3
			player.to_d = player.to_d - 10
			player.dis_to_d = player.dis_to_d - 10
			player.exp_drain = TRUE
			player.drain_mana = player.drain_mana + 2

	end,
}

add_mimic_shape
{
	["name"] =      "Kobold",
	["obj_name"] =  "Kobold Cape",
	["desc"] = 	"Kobolds resist poison and are physically strong.",
	["realm"] =     "nature",
	["level"] =     20,
	["rarity"] =    30,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 3 + (player.mimic_level / 15))
			player.modify_stat(A_INT, -15)
			player.modify_stat(A_WIS, -10)
			player.modify_stat(A_CON, 3 + (player.mimic_level / 15))
			player.modify_stat(A_CHR, -10)
			player.resist_pois = TRUE

	end,
}

add_mimic_shape
{
	["name"] =      "Dragon",
	["obj_name"] =  "Dragon Scales",
	["desc"] = 	"Dragons fly and breathe the elements.",
	["realm"] =     "nature",
	["level"] =     35,
	["rarity"] =    75,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 6 + (player.mimic_level / 10))
			player.modify_stat(A_INT, 1)
			player.modify_stat(A_WIS, 1)
			player.modify_stat(A_DEX, -15)
			player.modify_stat(A_CON, 6 + (player.mimic_level / 10))
			player.modify_stat(A_CHR, -20)
			if player.mimic_level >= 20 then player.fly = TRUE end
			player.resist_fire = TRUE
			player.resist_cold = TRUE
			player.resist_elec = TRUE
			if player.mimic_level >= 25 then player.resist_dark = TRUE end
			if player.mimic_level >= 30 then player.num_blow = player.num_blow + 1 end
			if player.mimic_level >= 60 then player.num_blow = player.num_blow + 1 end
			player.skill_dis = player.skill_dis - 10
			player.skill_sav = player.skill_sav - 5
			player.skill_stl = player.skill_stl - 25
			player.to_h_ranged = player.to_h_ranged - 12
			player.to_d_ranged = player.to_d_ranged - 5
			player.rapid_hunger = TRUE

	end,
}

add_mimic_shape
{
	["name"] =      "Demon",
	["obj_name"] =  "Demon Coat",
	["desc"] = 	"Demons breathe fire and wreck their enemy.",
	["realm"] =     "nature",
	["level"] =     45,
	["rarity"] =    90,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 2)
			player.modify_stat(A_INT, 4 + (player.mimic_level / 15))
			player.modify_stat(A_WIS, -30)
			player.modify_stat(A_DEX, 6 + (player.mimic_level / 11))
			player.modify_stat(A_CON, 6 + (player.mimic_level / 11))
			player.modify_stat(A_CHR, -25)
			player.hold_life = TRUE
			player.resist_chaos = TRUE
			player.resist_neth = TRUE
			if player.mimic_level >= 30 then player.num_blow = player.num_blow + 1 end
			if player.mimic_level >= 60 then player.num_blow = player.num_blow + 1 end
			player.skill_dis = player.skill_dis - 5
			player.skill_sav = player.skill_sav - 15
			player.skill_stl = player.skill_stl - 20
			player.luck_cur = player.luck_cur - 20
			player.drain_life = player.drain_life + 1
			player.nastytrap26 = TRUE

	end,
}

add_mimic_shape
{
	["name"] =      "Hound",
	["obj_name"] =  "Hound Pelt",
	["desc"] = 	"Hounds are fast and can see pretty well.",
	["realm"] =     "nature",
	["level"] =     55,
	["rarity"] =    40,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 5 + (player.mimic_level / 10))
			player.modify_stat(A_INT, -5)
			player.modify_stat(A_WIS, -5)
			player.modify_stat(A_DEX, -5)
			player.modify_stat(A_CON, 3)
			player.modify_stat(A_CHR, -10)
			player.pspeed = player.pspeed + 1 + (player.mimic_level / 10)
			player.resist_lite = TRUE
			player.resist_dark = TRUE
			player.skill_stl = player.skill_stl - 4
			player.skill_dis = player.skill_dis - 5
			player.skill_dev = player.skill_dev - 5

	end,
}

add_mimic_shape
{
	["name"] =      "Quylthulg",
	["obj_name"] =  "Quylthulg Peel",
	["desc"] = 	"Quylthulgs cannot move, but are capable of summoning aid.",
	["realm"] =     "nature",
	["level"] =     35,
	["rarity"] =    80,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 2)
			player.modify_stat(A_INT, 6 + (player.mimic_level / 8))
			player.modify_stat(A_WIS, -6)
			player.modify_stat(A_DEX, 7 + (player.mimic_level / 7))
			player.modify_stat(A_CON, 5 + (player.mimic_level / 9))
			player.modify_stat(A_CHR, -30)
			player.see_inv = TRUE
			player.immovable = TRUE

			player.to_h = player.to_h - 20
			player.dis_to_h = player.dis_to_h - 20
			player.to_d = player.to_d - 20
			player.dis_to_d = player.dis_to_d - 20
			player.nastytrap36 = TRUE

	end,
}

add_mimic_shape
{
	["name"] =      "Maiar",
	["obj_name"] =  "Maia Mantle",
	["desc"] = 	"Maiar are incredibly strong, but aggravate monsters.",
	["realm"] =     "nature",
	["level"] =     75,
	["rarity"] =    90,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 15 + (player.mimic_level / 8))
			player.modify_stat(A_INT, 15 + (player.mimic_level / 8))
			player.modify_stat(A_DEX, 15 + (player.mimic_level / 8))
			player.modify_stat(A_CON, 15 + (player.mimic_level / 8))
			player.modify_stat(A_WIS, 15 + (player.mimic_level / 8))
			player.modify_stat(A_CHR, 15 + (player.mimic_level / 8))
			player.resist_cold = TRUE
			player.resist_fire = TRUE
			player.resist_elec = TRUE
			player.resist_acid = TRUE
			player.resist_dark = TRUE
			player.resist_lite = TRUE
			if player.mimic_level >= 30 then player.immune_cold = TRUE end
			if player.mimic_level >= 30 then player.immune_fire = TRUE end
			if player.mimic_level >= 30 then player.immune_elec = TRUE end
			if player.mimic_level >= 30 then player.immune_acid = TRUE end
			if player.mimic_level >= 15 then player.resist_pois = TRUE end
			if player.mimic_level >= 35 then player.resist_chaos = TRUE end
			player.hold_life = TRUE
			player.ffall = TRUE
			player.regenerate = TRUE
			player.aggravate = TRUE
			if player.mimic_level >= 30 then player.num_blow = player.num_blow + 1 end
			if player.mimic_level >= 60 then player.num_blow = player.num_blow + 1 end
			if player.mimic_level >= 90 then player.num_blow = player.num_blow + 1 end
			player.skill_sav = player.skill_sav - 20
			player.skill_stl = player.skill_stl - 25
			player.skill_srh = player.skill_srh - 20
			player.skill_fos = player.skill_fos - 40
			player.anti_tele = TRUE
			player.rapid_hunger = TRUE
			player.nastytrap4 = TRUE
			player.luck_cur = player.luck_cur - 10

	end,
}

add_mimic_shape
{
	["name"] =      "Snake",
	["obj_name"] =  "Snake Skin",
	["desc"] = 	"Snakes are devious, and surprisingly fast.",
	["realm"] =     "nature",
	["level"] =     25,
	["rarity"] =    40,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, -5)
			player.modify_stat(A_INT, 3)
			player.modify_stat(A_WIS, 3)
			player.modify_stat(A_DEX, 6 + (player.mimic_level / 10))
			player.modify_stat(A_CON, 1)
			player.modify_stat(A_CHR, -8)
			player.see_infra = player.see_infra + 10
			player.pspeed = player.pspeed + 4 + (player.mimic_level / 7)
			player.skill_dis = player.skill_dis - 3
			player.skill_dev = player.skill_dev - 5
			player.skill_sav = player.skill_sav - 3
			player.to_h_ranged = player.to_h_ranged - 12

	end,
}

add_mimic_shape
{
	["name"] =      "Giant",
	["obj_name"] =  "Giant Hauberk",
	["desc"] = 	"Giants are super strong and gain many intrinsics.",
	["realm"] =     "nature",
	["level"] =     60,
	["rarity"] =    80,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_STR, 8 + (player.mimic_level / 8))
			player.modify_stat(A_INT, 1)
			player.modify_stat(A_WIS, 1)
			player.modify_stat(A_DEX, 1)
			player.modify_stat(A_CON, 9 + (player.mimic_level / 8))
			player.modify_stat(A_CHR, -10)
			player.resist_cold = TRUE
			player.resist_fire = TRUE
			player.resist_elec = TRUE
			player.resist_acid = TRUE
			player.resist_fear = TRUE
			if player.mimic_level >= 20 then player.resist_lite = TRUE end
			if player.mimic_level >= 20 then player.resist_dark = TRUE end
			if player.mimic_level >= 25 then player.resist_nexus = TRUE end
			if player.mimic_level >= 30 then player.resist_pois = TRUE end
			if player.mimic_level >= 40 then player.resist_conf = TRUE end
			if player.mimic_level >= 50 then player.resist_sound = TRUE end
			if player.mimic_level >= 50 then player.reflect = TRUE end
			if player.mimic_level >= 30 then player.num_blow = player.num_blow + 1 end
			player.pspeed = player.pspeed + 2 + (player.mimic_level / 12)
			player.rapid_hunger = TRUE
			player.skill_sav = player.skill_sav - 10
			player.skill_stl = player.skill_stl - 20
			player.skill_srh = player.skill_srh - 5
			player.skill_fos = player.skill_fos - 10
			player.to_a = player.to_a - 50
			player.dis_to_a = player.dis_to_a - 50
			player.luck_cur = player.luck_cur - 30

	end,
}

add_mimic_shape
{
	["name"] =      "Valar",
	["obj_name"] =  "Godly Wisps",
	["desc"] = 	"Valar are the supreme gods and basically indestructible.",
	["realm"] =     "nature",
	["level"] =     100,
	["rarity"] =    95,
	["duration"] =  {20, 40},
	["calc"] =      function ()
			player.modify_stat(A_WIS, 36)
			player.modify_stat(A_STR, 36)
			player.modify_stat(A_DEX, 36)
			player.modify_stat(A_CON, 36)
			player.modify_stat(A_INT, 36)
			player.modify_stat(A_CHR, 36)
			player.see_inv = TRUE
			player.free_act = TRUE
			player.slow_digest = TRUE
			player.regenerate = TRUE
			player.ffall = TRUE
			player.hold_life = TRUE
			player.lite = TRUE
			player.sustain_str = TRUE
			player.sustain_con = TRUE
			player.sustain_chr = TRUE
			player.sustain_wis = TRUE
			player.sustain_int = TRUE
			player.sustain_dex = TRUE
			player.immune_acid = TRUE
			player.immune_elec = TRUE
			player.immune_fire = TRUE
			player.immune_cold = TRUE
			player.resist_pois = TRUE
			player.resist_conf = TRUE
			player.resist_sound = TRUE
			player.resist_lite = TRUE
			player.resist_dark = TRUE
			player.resist_chaos = TRUE
			player.resist_disen = TRUE
			player.resist_shard = TRUE
			player.resist_nexus = TRUE
			player.resist_blind = TRUE
			player.resist_neth = TRUE
			player.resist_fear = TRUE
			player.resist_nerve = TRUE
			player.resist_mind = TRUE
			player.resist_ether = TRUE
			player.reflect = TRUE
			player.sh_fire = TRUE
			player.sh_elec = TRUE
			player.pspeed = player.pspeed + 5 + (player.mimic_level / 5)
			if player.mimic_level >= 20 then player.num_blow = player.num_blow + 1 end
			if player.mimic_level >= 40 then player.num_blow = player.num_blow + 1 end
			if player.mimic_level >= 60 then player.num_blow = player.num_blow + 1 end
			if player.mimic_level >= 80 then player.num_blow = player.num_blow + 1 end
			if player.mimic_level >= 90 then player.num_blow = player.num_blow + 1 end
			player.nastytrap12 = TRUE
			player.rapid_hunger = TRUE
			player.resist_time = TRUE
			player.aggravate = TRUE

	end,
}

add_mimic_shape
{
	["name"] =      "Eagle",
	["obj_name"] =  "Feathers Cloak",
	["desc"] = 	"Eagles are master of the air, good hunters with excellent vision.",
	["realm"] =     "nature",
	["level"] =     10,
	["rarity"] =    30,
	["duration"] =  {10, 50},
	["calc"] =      function ()
			player.ffall = TRUE
			player.pspeed = player.pspeed + 1 + (player.mimic_level / 20)

			player.modify_stat(A_STR, -10)
			player.modify_stat(A_DEX, 2 + (player.mimic_level / 15))
			player.modify_stat(A_CON, 4 + (player.mimic_level / 20))
			player.modify_stat(A_INT, -3)
			player.modify_stat(A_WIS, 1)
			player.modify_stat(A_CHR, -3)

			player.skill_stl = player.skill_stl - 10

			if player.mimic_level >= 20 then player.fly = TRUE end
			if player.mimic_level >= 25 then player.free_act = TRUE end
			if player.mimic_level >= 30 then player.see_inv = TRUE end
			if player.mimic_level >= 40 then player.resist_elec = TRUE end
			if player.mimic_level >= 50 then player.resist_mind = TRUE end
			if player.mimic_level >= 60 then player.sh_elec = TRUE end

	end,
}

add_mimic_shape
{
	["name"] =      "Yith",
	["obj_name"] =  "Yith Mantle",
	["desc"] = 	"Powerful eldritch being.",
	["realm"] =     "nature",
	["level"] =     40,
	["rarity"] =    50,
	["duration"] =  {10, 50},
	["calc"] =      function ()

			player.modify_stat(A_DEX, 2 + (player.mimic_level / 15))
			player.modify_stat(A_STR, 2 + (player.mimic_level / 15))
			player.modify_stat(A_INT, 2 + (player.mimic_level / 15))
			player.modify_stat(A_WIS, 2 + (player.mimic_level / 15))
			player.pspeed = player.pspeed - 2

			player.skill_sav = player.skill_sav + player.mimic_level
			player.skill_dis = player.skill_dis - 20
			player.skill_stl = player.skill_stl - 15
			player.skill_srh = player.skill_srh - 20
			player.skill_fos = player.skill_fos - 20

			player.sustain_str = TRUE
			player.sustain_dex = TRUE
			player.sustain_int = TRUE
			player.sustain_wis = TRUE

			player.resist_mind = TRUE
			if player.mimic_level >= 10 then player.resist_dark = TRUE end
			if player.mimic_level >= 20 then player.resist_conf = TRUE end
			if player.mimic_level >= 30 then player.see_inv = TRUE end
			if player.mimic_level >= 40 then player.resist_disen = TRUE end


	end,
}

add_mimic_shape
{
	["name"] =      "Wolf",
	["obj_name"] =  "Wolf Pelt",
	["desc"] = 	"Wolves are masters of movement, strong and have excellent eyesight.",
	["realm"] =     "nature",
	["level"] =     20,
	["rarity"] =    40,
	["duration"] =  {10, 50},
	["calc"] =      function ()
			player.modify_stat(A_STR, 2 + (player.mimic_level / 20))
			player.modify_stat(A_DEX, 3 + (player.mimic_level / 20))
			player.modify_stat(A_INT, -10)
			player.modify_stat(A_CHR, -8)

			player.pspeed = player.pspeed + 1 + (player.mimic_level / 10)

			player.skill_stl = player.skill_stl - 5
			player.to_h_ranged = player.to_h_ranged - 10

			player.free_act = TRUE
			player.resist_fear = TRUE

			if player.mimic_level >= 10 then player.resist_cold = TRUE end
			if player.mimic_level >= 30 then player.see_inv = TRUE end
			if player.mimic_level >= 40 then player.resist_dark = TRUE end
			if player.mimic_level >= 60 then player.resist_conf = TRUE end

	end,
}

add_mimic_shape
{
	["name"] =      "Lion",
	["obj_name"] =  "Lion Hide",
	["desc"] = 	"Lions are fearsome, fast hunters.",
	["realm"] =     "nature",
	["level"] =     20,
	["rarity"] =    40,
	["duration"] =  {10, 50},
	["calc"] =      function ()
			player.modify_stat(A_DEX, 4 + (player.mimic_level / 10))
			player.modify_stat(A_WIS, -5)

			player.pspeed = player.pspeed + 1 + (player.mimic_level / 8)
			player.xtra_crit = player.xtra_crit + 1 + (player.mimic_level / 10)
			--player.dodge_chance = player.dodge_chance + (player.mimic_level * 2 / 5) moved to xtra1.c

			player.to_h_ranged = player.to_h_ranged - 5
			player.to_a = player.to_a - 10
			player.dis_to_a = player.dis_to_a - 10
			player.skill_dev = player.skill_dev - 5
			player.skill_fos = player.skill_fos - 5
			player.skill_dig = player.skill_dig - 5

			player.resist_fear = TRUE

	end,
}

add_mimic_shape
{
	["name"] =      "Spider",
	["obj_name"] =  "Spider Web",
	["desc"] = 	"Spiders are clever and become good climbers.",
	["realm"] =     "nature",
	["level"] =     25,
	["rarity"] =    50,
	["duration"] =  {10, 50},
	["calc"] =      function ()
			player.modify_stat(A_STR, -12)
			player.modify_stat(A_DEX, 1 + (player.mimic_level / 8))
			player.modify_stat(A_INT, 1 + (player.mimic_level / 5))
			player.modify_stat(A_WIS, 1 + (player.mimic_level / 5))
			player.modify_stat(A_CON, -15)
			player.modify_stat(A_CHR, -30)

			player.pspeed = player.pspeed + 5

			player.resist_pois = TRUE
			player.resist_fear = TRUE
			player.resist_dark = TRUE

			if player.mimic_level >= 40 then player.climb = TRUE end

	end,
	["power"] =     function()
			if player.mimic_level >= 25 then
				player.add_power(POWER_WEB)
			end
	end,
}

add_mimic_shape
{
	["name"] =      "Elder Ent",
	["obj_name"] =  "Entish Bark",
	["desc"] = 	"Ents are powerful tree-like beings dating from the dawn of time.",
	["realm"] =     "nature",
	["level"] =     40,
	["rarity"] =    60,
	["duration"] =  {10, 30},
	["calc"] =      function ()
			player.pspeed = player.pspeed - 5 - (player.mimic_level / 10)

			player.to_a = player.to_a + 10 + player.mimic_level
			player.dis_to_a = player.dis_to_a + 10 + player.mimic_level

			player.resist_pois = TRUE
			player.resist_cold = TRUE
			player.free_act = TRUE
			player.regenerate = TRUE
			player.see_inv = TRUE
			player.sensible_fire = TRUE

			player.skill_stl = player.skill_stl - 10

			player.modify_stat(A_STR, player.mimic_level / 5)
			player.modify_stat(A_INT, - (player.mimic_level / 7))
			player.modify_stat(A_WIS, - (player.mimic_level / 7))
			player.modify_stat(A_DEX, -12)
			player.modify_stat(A_CON, player.mimic_level / 5)
			player.modify_stat(A_CHR, -20)

	end,
	["power"] =     function ()
			player.add_power(PWR_GROW_TREE)
	end,
}

add_mimic_shape
{
	["name"] =      "Vapor",
	["obj_name"] =  "Cloak of Mist",
	["desc"] = 	"A sentient cloud, darting around",
	["realm"] =     "nature",
	["level"] =     15,
	["rarity"] =    10,
	["duration"] =  {10, 40},
	["calc"] =      function ()

			player.pspeed = player.pspeed + 5

			--Try to hit a cloud!
			player.to_a = player.to_a + 40 + player.mimic_level
			player.dis_to_a = player.dis_to_a + 40 + player.mimic_level

			--Try to hit WITH a cloud!
			player.to_h = player.to_h - 40
			player.dis_to_h = player.dis_to_h - 40

			-- But they are stealthy
			player.skill_stl = player.skill_stl + 10 + (player.mimic_level / 5)
			player.resist_pois = TRUE
			player.immune_cold = TRUE
			player.resist_shard = TRUE
			player.free_act = TRUE
			player.regenerate = TRUE
			player.see_inv = TRUE
			player.sensible_fire = TRUE
			player.levitate = TRUE
			player.ffall = TRUE
			player.resist_ether = TRUE

			player.skill_dis = player.skill_dis - 10
			player.skill_srh = player.skill_srh - 10
			player.skill_fos = player.skill_fos - 10
			player.skill_dig = player.skill_dig - 20

			-- Stat mods
			player.modify_stat(A_STR, -20)
			player.modify_stat(A_DEX, 5)
			player.modify_stat(A_CON, -20)
			player.modify_stat(A_CHR, -30)
	end,
}

add_mimic_shape
{
	["name"] =      "Serpent",
	["obj_name"] =  "Snakeskin Cloak",
	["desc"] = 	"Serpents are fast, lethal predators.",
	["realm"] =     "nature",
	["level"] =     30,
	["rarity"] =    25,
	["duration"] =  {15, 20},
	["calc"] =      function ()
			player.pspeed = player.pspeed + 1 + (player.mimic_level / 9)
			player.to_a = player.to_a + 3 + (player.mimic_level / 8)
			player.dis_to_a = player.dis_to_a + 3 + (player.mimic_level / 8)

			player.modify_stat(A_STR, player.mimic_level / 8)
			player.modify_stat(A_INT, -18)
			player.modify_stat(A_WIS, -18)
			player.modify_stat(A_DEX, -12)
			player.modify_stat(A_CON, player.mimic_level / 7)
			player.modify_stat(A_CHR, -18)

			player.skill_dis = player.skill_dis - 10
			player.skill_dig = player.skill_dig - 5

			player.resist_pois = TRUE
			if player.mimic_level >= 25 then player.free_act = TRUE end
	end,
}

add_mimic_shape
{
	["name"] =      "Mumak",
	["obj_name"] =  "Mumak Hide",
	["desc"] = 	"A giant, elaphantine form.",
	["realm"] =     "nature",
	["level"] =     40,
	["rarity"] =    40,
	["duration"] =  {15, 20},
	["calc"] =      function ()
			player.pspeed = player.pspeed - 5 - (player.mimic_level / 10)
			player.to_a = player.to_a + 10 + (player.mimic_level / 6)
			player.dis_to_a = player.dis_to_a + 10 + (player.mimic_level / 6)
			player.to_d = player.to_d + 5 + ((player.mimic_level * 2) / 3)
			player.dis_to_d = player.dis_to_d + 5 + ((player.mimic_level * 2) / 3)

			player.modify_stat(A_STR, player.mimic_level / 4)
			player.modify_stat(A_INT, -20)
			player.modify_stat(A_WIS, -12)
			player.modify_stat(A_DEX, -15)
			player.modify_stat(A_CON, player.mimic_level / 3)
			player.modify_stat(A_CHR, -30)

			player.skill_stl = player.skill_stl - 15
			player.skill_dis = player.skill_dis - 5
			player.skill_dev = player.skill_dev - 5

			if player.mimic_level >= 10 then player.resist_fear = TRUE end
			if player.mimic_level >= 30 then player.free_act = TRUE end
			if player.mimic_level >= 40 then player.resist_conf = TRUE end
			if player.mimic_level >= 60 then player.resist_nexus = TRUE end
	end,
}

--------- Extra shapes -----------

-- For Beornings
add_mimic_shape
{
	["name"] =      "Bear",
	["desc"] = 	"A fierce, terrible bear.",
	["realm"] =     nil,
	["level"] =     1,
	["rarity"] =    101,
	["duration"] =  {50, 200},
	["limit"] =     TRUE,
	["calc"] =      function ()
			player.pspeed = player.pspeed - 5 + (player.mimic_level / 10)

			player.to_a = player.to_a + 5 + ((player.mimic_level * 2) / 3)
			player.dis_to_a = player.dis_to_a + 5 + ((player.mimic_level * 2) / 3)

			player.modify_stat(A_STR, player.mimic_level / 11)
			player.modify_stat(A_INT, player.mimic_level / 11)
			player.modify_stat(A_WIS, player.mimic_level / 11)
			player.modify_stat(A_DEX, -1)
			player.modify_stat(A_CON, player.mimic_level / 11)
			player.modify_stat(A_CHR, -10)

			if player.mimic_level >= 10 then player.free_act = TRUE end
			if player.mimic_level >= 20 then player.regenerate = TRUE end
			if player.mimic_level >= 40 then player.resist_conf = TRUE end
			if player.mimic_level >= 60 then player.resist_nexus = TRUE end

			-- activate the skill
			skill(SKILL_BEAR).hidden = FALSE
	end,
}

-- For balrog corruptions
add_mimic_shape
{
	["name"] =      "Balrog",
	["desc"] = 	"A corrupted maia.",
	["realm"] =     nil,
	["level"] =     1,
	["rarity"] =    101,
	["duration"] =  {30, 70},
	["limit"] =     TRUE,
	["calc"] =      function ()
			player.modify_stat(A_STR, 5 + player.mimic_level / 5)
			player.modify_stat(A_INT, player.mimic_level / 10)
			player.modify_stat(A_WIS, - ( 5 + player.mimic_level / 10))
			player.modify_stat(A_DEX, player.mimic_level / 10)
			player.modify_stat(A_CON, 5 + player.mimic_level / 5)
			player.modify_stat(A_CHR, - ( 5 + player.mimic_level / 10))

			player.immune_fire = TRUE
			player.immune_elec = TRUE
			player.immune_acid = TRUE
			player.resist_pois = TRUE
			player.resist_dark = TRUE
			player.resist_chaos = TRUE
			player.hold_life = TRUE
			player.ffall =  TRUE
			player.regenerate = TRUE
			player.sh_fire = TRUE
			return 1
	end,
}

-- For avatar spell
add_mimic_shape
{
	["name"] =      "Maia",
	["desc"] = 	"A near god-like being.",
	["realm"] =     nil,
	["level"] =     1,
	["rarity"] =    101,
	["duration"] =  {30, 70},
	["limit"] =     TRUE,
	["calc"] =      function ()
			player.modify_stat(A_STR, 5 + player.mimic_level / 10)
			player.modify_stat(A_INT, 5 + player.mimic_level / 10)
			player.modify_stat(A_WIS, 5 + player.mimic_level / 10)
			player.modify_stat(A_DEX, 5 + player.mimic_level / 10)
			player.modify_stat(A_CON, 5 + player.mimic_level / 10)
			player.modify_stat(A_CHR, 5 + player.mimic_level / 10)

			player.immune_fire = TRUE
			player.immune_elec = TRUE
			player.immune_acid = TRUE
			player.immune_cold = TRUE
			player.resist_pois = TRUE
			player.resist_lite = TRUE
			player.resist_dark = TRUE
			player.resist_chaos = TRUE
			player.hold_life = TRUE
			player.ffall = TRUE
			player.regenerate = TRUE
			return 2
	end,
}

-- For spider form spell
add_mimic_shape
{
	["name"] =      "Arachnid",
	["desc"] = 	"A very powerful spider.",
	["realm"] =     "nil",
	["level"] =     1,
	["rarity"] =    101,
	["duration"] =  {15, 75},
	["limit"] =     TRUE,
	["calc"] =      function ()
			player.modify_stat(A_STR, -4)
			player.modify_stat(A_DEX, 1 + (player.mimic_level / 6))
			player.modify_stat(A_INT, 1 + (player.mimic_level / 4))
			player.modify_stat(A_WIS, 1 + (player.mimic_level / 4))
			player.modify_stat(A_CON, -5)
			player.modify_stat(A_CHR, -7)

			player.pspeed = player.pspeed + 5

			player.resist_pois = TRUE
			player.resist_fear = TRUE
			player.resist_dark = TRUE

			if player.mimic_level >= 40 then player.climb = TRUE end

	end,
	["power"] =     function()
			if player.mimic_level >= 25 then
				player.add_power(POWER_WEB)
			end
	end,
}

-- For Geomancy
add_mimic_shape
{
	["name"] =      "Fire Elem.",
	["desc"] = 	"A towering column of flames",
	["realm"] =     nil,
	["level"] =     1,
	["rarity"] =    101,
	["duration"] =  {10, 10},
	["limit"] =     TRUE,
	["calc"] =      function ()
			player.modify_stat(A_STR, 5 + (player.mimic_level / 5))
			player.modify_stat(A_DEX, 5 + (player.mimic_level / 5))
			player.modify_stat(A_WIS, -5 - (player.mimic_level / 5))

			player.immune_fire = TRUE
			-- was immune to poison in the 3.0.0 version
			player.resist_pois = TRUE
			player.sh_fire = TRUE
			player.lite = TRUE
			return 0
	end,
}
