-- handle the music school
-- *ALL* lasting spell must return the mana cost in the lasting function

MUSIC_STOP = add_spell
{
	["name"] =      "Stop singing(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     1,
	["mana"] =      0,
	["mana_max"] =  0,
	["fail"] =      -400,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      1,
	["blind"] =     FALSE,
	["spell"] =     function()
			player.start_lasting_spell(0)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Stops the current song, if any."
	}
}

--- Drums
MUSIC_HOLD = add_spell
{
	["name"] =      "Holding Pattern(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     1,
	["mana"] =      1,
	["mana_max"] =  10,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      1,
	["blind"] =     FALSE,
	["lasting"] =   function()
			project_los(GF_OLD_SLOW, 10 + get_level(MUSIC_HOLD, 100))
			return get_mana(MUSIC_HOLD)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_HOLD)
			return TRUE
	end,
	["info"] =      function()
			return "power "..(10 + get_level(MUSIC_HOLD, 100))
	end,
	["desc"] =      {
			"Slows down all monsters listening the song.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_CONF = add_spell
{
	["name"] =      "Illusion Pattern(II)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     5,
	["mana"] =      2,
	["mana_max"] =  15,
	["fail"] =      30,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      2,
	["blind"] =     FALSE,
	["lasting"] =   function()
			project_los(GF_OLD_CONF, 10 + get_level(MUSIC_CONF, 100))
			return get_mana(MUSIC_CONF)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_CONF)
			return TRUE
	end,
	["info"] =      function()
			return "power "..(10 + get_level(MUSIC_CONF, 100))
	end,
	["desc"] =      {
			"Tries to confuse all monsters listening the song.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_STUN = add_spell
{
	["name"] =      "Stun Pattern(IV)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     10,
	["mana"] =      3,
	["mana_max"] =  25,
	["fail"] =      45,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      4,
	["blind"] =     FALSE,
	["lasting"] =   function()
			project_los(GF_STUN, 10 + get_level(MUSIC_STUN, 90))
			return get_mana(MUSIC_STUN)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_STUN)
			return TRUE
	end,
	["info"] =      function()
			return "power "..(10 + get_level(MUSIC_STUN, 90))
	end,
	["desc"] =      {
			"Stuns all monsters listening the song.",
			"Consumes the amount of mana each turn.",
	}
}

--- Harps
MUSIC_LITE = add_spell
{
	["name"] =      "Song of the Sun(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     1,
	["mana"] =      1,
	["mana_max"] =  1,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      1,
	["lasting"] =   function()
			set_lite(5)
			return get_mana(MUSIC_LITE)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_LITE)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Provides light as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_HEAL = add_spell
{
	["name"] =      "Flow of Life(II)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     7,
	["mana"] =      5,
	["mana_max"] =  30,
	["fail"] =      35,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      2,
	["lasting"] =   function()
			hp_player(7 + get_level(MUSIC_HEAL, 100))
			return get_mana(MUSIC_HEAL)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_HEAL)
			return TRUE
	end,
	["info"] =      function()
			return "heal "..(7 + get_level(MUSIC_HEAL, 100)).."/turn"
	end,
	["desc"] =      {
			"Heals you as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_HERO = add_spell
{
	["name"] =      "Heroic Ballad(II)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     10,
	["mana"] =      4,
	["mana_max"] =  14,
	["fail"] =      45,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      2,
	["lasting"] =   function()
			set_hero(5)
			if get_level(MUSIC_HERO) >= 10 then
				set_shero(5)
			end
			if get_level(MUSIC_HERO) >= 20 then
				set_strike(5)
			end
			if get_level(MUSIC_HERO) >= 25 then
				set_oppose_cc(5)
			end
			return get_mana(MUSIC_HERO)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_HERO)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Increases melee accuracy",
			"At level 10 it increases it even more and reduces armor a bit",
			"At level 20 it increases it again",
			"At level 25 it grants protection against chaos and confusion",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_TIME = add_spell
{
	["name"] =      "Hobbit Melodies(III)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     20,
	["mana"] =      10,
	["mana_max"] =  30,
	["fail"] =      70,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      3,
	["lasting"] =   function()
			set_shield(5, 10 + get_level(MUSIC_TIME, 50), 0, 0, 0)
			if get_level(MUSIC_TIME) >= 15 then
				set_fast(5, 7 + get_level(MUSIC_TIME, 10))
			end
			return get_mana(MUSIC_TIME)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_TIME)
			return TRUE
	end,
	["info"] =      function()
			if get_level(MUSIC_TIME) >= 15 then
				return "AC "..(10 + get_level(MUSIC_TIME, 50)).." speed "..(7 + get_level(MUSIC_TIME, 10))
			else
				return "AC "..(10 + get_level(MUSIC_TIME, 50))
			end
	end,
	["desc"] =      {
			"Greatly increases your reflexes allowing you to block more melee blows.",
			"At level 15 it also makes you faster.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_MIND = add_spell
{
	["name"] =      "Clairaudience(IV)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     25,
	["mana"] =      15,
	["mana_max"] =  30,
	["fail"] =      75,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      4,
	["lasting"] =   function()
			set_tim_esp(5)
			if get_level(MUSIC_MIND) >= 20 then
				fire_ball(GF_IDENTIFY, 0, 1, 1 + get_level(MUSIC_MIND, 3, 0))
			end
			return get_mana(MUSIC_MIND)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_MIND)
			return TRUE
	end,
	["info"] =      function()
			if get_level(MUSIC_MIND) >= 20 then
				return "rad "..(1 + get_level(MUSIC_MIND, 3, 0))
			else
				return ""
			end
	end,
	["desc"] =      {
			"Allows you to sense monster minds as long as you sing.",
			"At level 20 it identifies all objects in a radius on the floor,",
			"as well as probing monsters in that radius.",
			"Consumes the amount of mana each turn.",
	}
}

--- Horns

MUSIC_BLOW = add_spell
{
	["name"] =      "Blow(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     4,
	["mana"] =      3,
	["mana_max"] =  30,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      1,
	["spell"] =     function()
			fire_ball(GF_SOUND,
				  0,
				  damroll(2 + get_level(MUSIC_BLOW, 10, 0), 4 + get_level(MUSIC_BLOW, 40, 0)),
				  1 + get_level(MUSIC_BLOW, 12, 0)
				 )
			return TRUE
	end,
	["info"] =      function()
			return "dam "..(2 + get_level(MUSIC_BLOW, 10, 0)).."d"..(4 + get_level(MUSIC_BLOW, 40, 0)).." rad "..(1 + get_level(MUSIC_BLOW, 12, 0))
	end,
	["desc"] =      {
			"Produces a powerful, blowing, sound all around you that damages enemies.",
	}
}

MUSIC_WIND = add_spell
{
	["name"] =      "Gush of Wind(II)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     14,
	["mana"] =      15,
	["mana_max"] =  45,
	["fail"] =      30,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      2,
	["spell"] =     function()
			fire_ball(GF_AWAY_ALL,
				  0,
				  10 + get_level(MUSIC_BLOW, 40, 0),
				  1 + get_level(MUSIC_BLOW, 12, 0)
				 )
			return TRUE
	end,
	["info"] =      function()
			return "dist "..(10 + get_level(MUSIC_BLOW, 40, 0)).." rad "..(1 + get_level(MUSIC_BLOW, 12, 0))
	end,
	["desc"] =      {
			"Produces a outgoing gush of wind that sends monsters away.",
	}
}

MUSIC_YLMIR = add_spell
{
	["name"] =      "Horns of Ylmir(III)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     20,
	["mana"] =      25,
	["mana_max"] =  30,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      3,
	["spell"] =     function()
			earthquake(player.py, player.px, 2 + get_level(SHAKE, 10))
			return TRUE
	end,
	["info"] =      function()
			return "rad "..(2 + get_level(SHAKE, 10))
	end,
	["desc"] =      {
			"Produces an earth shaking sound.",
	}
}

MUSIC_AMBARKANTA = add_spell
{
	["name"] =      "Ambarkanta(IV)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     25,
	["mana"] =      70,
	["mana_max"] =  70,
	["fail"] =      60,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      4,
	["spell"] =     function()
			alter_reality()
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Produces a reality shaking sound that transports you to a nearly",
			"identical reality.",
	}
}

--- Banjos
MUSIC_PEACE = add_spell
{
	["name"] =      "Song of Peace(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     1,
	["mana"] =      1,
	["mana_max"] =  1,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      1,
	["lasting"] =   function()
			set_tim_peace(5)
			return get_mana(MUSIC_PEACE)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_PEACE)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Grants peacekeeping as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_MASTER = add_spell
{
	["name"] =      "Master's Tune(II)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     8,
	["mana"] =      3,
	["mana_max"] =  3,
	["fail"] =      30,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      2,
	["lasting"] =   function()
			set_tim_device(5)
			return get_mana(MUSIC_MASTER)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_MASTER)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Improves your devices as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_ELEMENT = add_spell
{
	["name"] =      "Lull of Elements(III)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     15,
	["mana"] =      3,
	["mana_max"] =  10,
	["fail"] =      40,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      3,
	["lasting"] =   function()
			set_tim_elemlull(5, get_level(MUSIC_ELEMENT))
			return get_mana(MUSIC_ELEMENT)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_ELEMENT)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Provides resistances to elements as long as you sing.",
			"Level 1: resist plasma",
			"Level 5: resist light and dark",
			"Level 10: resist sound and shards",
			"Level 20: resist nether",
			"Level 40: resist chaos and disenchantment",
			"Level 50: resist water",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_DEPTHS = add_spell
{
	["name"] =      "Sound of the Depths(V)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     40,
	["mana"] =      10,
	["mana_max"] =  10,
	["fail"] =      60,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      5,
	["lasting"] =   function()
			set_tim_magbreath(5, get_level(MUSIC_DEPTHS))
			return get_mana(MUSIC_DEPTHS)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_DEPTHS)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Provides void breathing and poison resistance as long as you sing.",
			"At level 25 it also provides poison immunity.",
			"Consumes the amount of mana each turn.",
	}
}

--- Lutes
MUSIC_DODGE = add_spell
{
	["name"] =      "Moonlight Macarena(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     1,
	["mana"] =      1,
	["mana_max"] =  3,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      1,
	["lasting"] =   function()
			set_tim_macarena(5, get_level(MUSIC_DODGE, 50))
			return get_mana(MUSIC_DODGE)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_DODGE)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Improves your dodging chance as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_SAVING = add_spell
{
	["name"] =      "Voice of Divinity(II)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     8,
	["mana"] =      2,
	["mana_max"] =  6,
	["fail"] =      35,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      2,
	["lasting"] =   function()
			set_tim_savmus(5, get_level(MUSIC_SAVING, 50))
			return get_mana(MUSIC_SAVING)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_SAVING)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Boosts your saving throw as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_MINER = add_spell
{
	["name"] =      "Miner's Song(III)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     20,
	["mana"] =      6,
	["mana_max"] =  14,
	["fail"] =      50,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      3,
	["lasting"] =   function()
			set_tim_minersong(5, get_level(MUSIC_MINER, 50))
			return get_mana(MUSIC_MINER)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_MINER)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Improves searching, disarming and tunneling as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_STEALTH = add_spell
{
	["name"] =      "Stealthy Whispers(IV)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     30,
	["mana"] =      3,
	["mana_max"] =  9,
	["fail"] =      70,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      4,
	["lasting"] =   function()
			set_tim_stealthmus(5, 5 + get_level(MUSIC_STEALTH, 20))
			return get_mana(MUSIC_STEALTH)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_STEALTH)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Boosts your stealth rating as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

--- Mandolins
MUSIC_WAR = add_spell
{
	["name"] =      "War Boogie(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     1,
	["mana"] =      1,
	["mana_max"] =  4,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      1,
	["lasting"] =   function()
			set_tim_warboogie(5, 5 + get_level(MUSIC_WAR, 25))
			return get_mana(MUSIC_WAR)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_WAR)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Improves your melee to-hit as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_TANK = add_spell
{
	["name"] =      "Tank Song(II)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     5,
	["mana"] =      3,
	["mana_max"] =  6,
	["fail"] =      35,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      2,
	["lasting"] =   function()
			set_tim_tankmus(5, 10 + get_level(MUSIC_TANK, 50))
			return get_mana(MUSIC_TANK)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_TANK)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Boosts your armor class as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_SLAUGHTER = add_spell
{
	["name"] =      "Slaughter Call(III)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     20,
	["mana"] =      10,
	["mana_max"] =  25,
	["fail"] =      70,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      3,
	["lasting"] =   function()
			set_tim_slaughtersong(5, 1 + get_level(MUSIC_SLAUGHTER, 2))
			return get_mana(MUSIC_SLAUGHTER)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_SLAUGHTER)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Increases your melee blows per round as long as you sing.",
			"Consumes the amount of mana each turn.",
	}
}

MUSIC_MIGHT = add_spell
{
	["name"] =      "Mighty March(IV)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     30,
	["mana"] =      10,
	["mana_max"] =  30,
	["fail"] =      80,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["blind"] =     FALSE,
	["pval"] =      4,
	["lasting"] =   function()
			set_tim_mightymus(5, get_level(MUSIC_MIGHT, 50))
			return get_mana(MUSIC_MIGHT)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_MIGHT)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"Increases the power of your ranged weapons as long as you sing.",
			"At level 30 it also increases your amount of shots.",
			"Consumes the amount of mana each turn.",
	}
}

--[[
MUSIC_ = add_spell
{
	["name"] =      "(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     1,
	["mana"] =      0,
	["mana_max"] =  0,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      1,
	["lasting"] =   function()
			return get_mana(MUSIC_)
	end,
	["spell"] =     function()
			player.start_lasting_spell(MUSIC_)
			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"",
			"Consumes the amount of mana each turn.",
	}
}

or

MUSIC_ = add_spell
{
	["name"] =      "(I)",
	["school"] =    {SCHOOL_MUSIC},
	["level"] =     1,
	["mana"] =      0,
	["mana_max"] =  0,
	["fail"] =      20,
	["stat"] =      A_CHR,
	["random"] =    SKILL_MUSIC,
	["pval"] =      1,
	["spell"] =     function()

			return TRUE
	end,
	["info"] =      function()
			return ""
	end,
	["desc"] =      {
			"",
	}
}
]]
