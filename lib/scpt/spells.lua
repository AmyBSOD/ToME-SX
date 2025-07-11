--
-- This file takes care of the schools of magic
--

-- Create the schools
SCHOOL_MANA = add_school
{
	["name"] = "Mana", 
	["skill"] = SKILL_MANA,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Eru Iluvatar provides the Mana school at half the prayer skill
		[GOD_ERU] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Varda provides the Mana school at 1/4 the prayer skill
		[GOD_VARDA] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 4,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
	["hooks"] =
	{
		[HOOK_CALC_MANA] = function(msp)
			if get_skill(SKILL_MANA) >= 35 then
				msp = msp + (msp * ((get_skill(SKILL_MANA) - 34)) / 100)
				return TRUE, msp
			end
		end
	},
}
SCHOOL_FIRE = add_school
{
	["name"] = "Fire", 
	["skill"] = SKILL_FIRE,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- AmyBSOD provides the Fire school at 1/2 the prayer skill
		[GOD_AMYBSOD] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},

		-- Aule provides the Fire school at 3/5 the prayer skill
		[GOD_AULE] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 3,
			["div"] = 5,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},

	},
}
SCHOOL_AIR = add_school
{
	["name"] = "Air", 
	["skill"] = SKILL_AIR,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["hooks"] =
	{
		[HOOK_CALC_BONUS] = function()
			if get_skill(SKILL_AIR) >= 50 then
				player.magical_breath = TRUE
			end
		end,
	},
	["gods"] =
	{
		-- Manwe Sulimo provides the Air school at 2/3 the prayer skill
		[GOD_MANWE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 2,
			["div"] = 3,
		},
		-- Este provides the Air school at 1/4 the prayer skill
		[GOD_ESTE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 4,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}
SCHOOL_WATER = add_school
{
	["name"] = "Water", 
	["skill"] = SKILL_WATER,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["hooks"] =
	{
		[HOOK_CALC_BONUS] = function()
			if get_skill(SKILL_WATER) >= 30 then
				player.water_breath = TRUE
			end
		end,
	},
	["gods"] =
	{
		-- Yavanna Kementari provides the Water school at 1/2 the prayer skill
		[GOD_YAVANNA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Ulmo provides the Water school at 3/5 the prayer skill
		[GOD_ULMO] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 3,
			["div"] = 5,
		},
		-- Este provides the Water school at 1/3 the prayer skill
		[GOD_ESTE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}
SCHOOL_EARTH = add_school
{
	["name"] = "Earth", 
	["skill"] = SKILL_EARTH,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Tulkas provides the Earth school at 4/5 the prayer skill
		[GOD_TULKAS] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 4,
			["div"] = 5,
		},
		-- Yavanna Kementari provides the Earth school at 1/2 the prayer skill
		[GOD_YAVANNA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Aule provides the Earth school at 1/3 the prayer skill
		[GOD_AULE] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Orome provides the Earth school at 1/4 the prayer skill
		[GOD_OROME] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 4,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}
SCHOOL_CONVEYANCE = add_school
{
	["name"] = "Conveyance", 
	["skill"] = SKILL_CONVEYANCE,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Manwe Sulimo provides the Conveyance school at 1/2 the prayer skill
		[GOD_MANWE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Vana provides the Conveyance school at 1/3 the prayer skill
		[GOD_VANA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Orome provides the Conveyance school at 1/3 the prayer skill
		[GOD_OROME] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}
SCHOOL_GEOMANCY = add_school
{
	["name"] = "Geomancy",
	["skill"] = SKILL_GEOMANCY,
	["spell_power"] = TRUE,
	-- Require to wield a Mage Staff, as the spells requries the caster to stomp the floor with it
	["depend"] = function()
		-- Require at least one point in each school
		if get_skill(SKILL_FIRE) == 0 then return end
		if get_skill(SKILL_AIR) == 0 then return end
		if get_skill(SKILL_EARTH) == 0 then return end
		if get_skill(SKILL_WATER) == 0 then return end

		local obj = get_object(INVEN_WIELD)
		if (obj.k_idx > 0) and (obj.tval == TV_MSTAFF) then return TRUE end
	end,
}
SCHOOL_DIVINATION = add_school
{
	["name"] = "Divination", 
	["skill"] = SKILL_DIVINATION,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Eru Iluvatar provides the Divination school at 2/3 the prayer skill
		[GOD_ERU] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 2,
			["div"] = 3,
		},
		-- Mandos provides the Divination school at 1/3 the prayer skill
		[GOD_MANDOS] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Orome provides the Divination school at 1/3 the prayer skill
		[GOD_OROME] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Nienna provides the Divination school at 1/3 the prayer skill
		[GOD_NIENNA] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}
SCHOOL_TEMPORAL = add_school
{
	["name"] = "Temporal", 
	["skill"] = SKILL_TEMPORAL,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Yavanna Kementari provides the Temoral school at 1/6 the prayer skill
		[GOD_YAVANNA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 6,
		},
		-- Mandos provides the Temporal school at 1/4 the prayer skill
		[GOD_MANDOS] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 4,
		},
		-- Vana provides the Temporal school at 1/2 the prayer skill
		[GOD_VANA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Orome provides the Temporal school at 1/3 the prayer skill
		[GOD_OROME] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
		-- Vaire provides the Temporal school at 2/3 the prayer skill
		[GOD_VAIRE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 2,
			["div"] = 3,
		},
	},
}
SCHOOL_NATURE = add_school
{
	["name"] = "Nature", 
	["skill"] = SKILL_NATURE,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Yavanna Kementari provides the Nature school at 1/2 the prayer skill
		[GOD_YAVANNA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Este provides the Nature school at 1/2 the prayer skill
		[GOD_ESTE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Ulmo provides the Nature school at 1/2 the prayer skill
		[GOD_ULMO] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Orome provides the Nature school at 1/2 the prayer skill
		[GOD_OROME] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}
SCHOOL_META = add_school
{
	["name"] = "Meta", 
	["skill"] = SKILL_META,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Manwe Sulimo provides the Meta school at 1/3 the prayer skill
		[GOD_MANWE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Vana provides the Meta school at 1/3 the prayer skill
		[GOD_VANA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- AmyBSOD provides the Meta school at 1/2 the prayer skill
		[GOD_AMYBSOD] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Varda provides the Meta school at 1/2 the prayer skill
		[GOD_VARDA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}
SCHOOL_MIND = add_school
{
	["name"] = "Mind",
	["skill"] = SKILL_MIND,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Eru Iluvatar provides the Mind school at 1/3 the prayer skill
		[GOD_ERU] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Melkor Bauglir provides the Mind school at 1/3 the prayer skill
		[GOD_MELKOR] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}
SCHOOL_UDUN = add_school
{
	["name"] = 		"Udun",
	["skill"] = 		SKILL_UDUN,
	["bonus_level"] = 	function()
					return ((player.lev * 2) / 3)
				end,
}
SCHOOL_DEMON = add_school
{
	["name"] = "Demon",
	["skill"] = SKILL_DAEMON,
	["no_random"] = TRUE,
	["gods"] =
	{
		-- Vaire provides the Demonology school at 1/3 the prayer skill
		[GOD_VAIRE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
	},
}

-- The God specific schools, all tied to the prayer skill
SCHOOL_ERU = add_school
{
	["name"] = "Eru Iluvatar",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_ERU,
}
SCHOOL_MANWE = add_school
{
	["name"] = "Manwe Sulimo",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_MANWE,
}
SCHOOL_TULKAS = add_school
{
	["name"] = "Tulkas",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_TULKAS,
}
SCHOOL_MELKOR = add_school
{
	["name"] = "Melkor Bauglir",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_MELKOR,
}
SCHOOL_YAVANNA = add_school
{
	["name"] = "Yavanna Kementari",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_YAVANNA,
}
SCHOOL_AMYBSOD = add_school
{
	["name"] = "AmyBSOD",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_AMYBSOD,
}
SCHOOL_OROME = add_school
{
	["name"] = "Orome",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_OROME,
}
SCHOOL_INGEBORG = add_school
{
	["name"] = "Ingeborg",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_INGEBORG,
}
SCHOOL_NIENNA = add_school
{
	["name"] = "Nienna",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_NIENNA,
}
SCHOOL_ESTE = add_school
{
	["name"] = "Este",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_ESTE,
}
SCHOOL_VANA = add_school
{
	["name"] = "Vana",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_VANA,
}
SCHOOL_VAIRE = add_school
{
	["name"] = "Vaire",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_VAIRE,
}

-- New schools
SCHOOL_AULE = add_school
{
	["name"] = "Aule the Smith",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_AULE,
}
SCHOOL_VARDA = add_school
{
	["name"] = "Varda Elentari",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_VARDA,
}

SCHOOL_ULMO = add_school
{
	["name"] = "Ulmo",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_ULMO,
}

SCHOOL_MANDOS = add_school
{
	["name"] = "Mandos",
	["skill"] = SKILL_PRAY,
	["spell_power"] = TRUE,
	["god"] = GOD_MANDOS,
}

-- Not a real school, rather a palcehodler for stick only spells
SCHOOL_DEVICE = add_school
{
	["name"] = "Device",
	["skill"] = SKILL_DEVICE,
}

-- Music "spells"
SCHOOL_MUSIC = add_school
{
	["name"] = "Music",
	["skill"] = SKILL_MUSIC,
	["gods"] =
	{
		-- Vana provides the Music school at 1/3 the prayer skill
		[GOD_VANA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
	},
}

SCHOOL_CELESTIAL = add_school
{
	["name"] = "Celestial", 
	["skill"] = SKILL_CELESTIAL,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Varda provides the Celestial school at 1/2 the prayer skill
		[GOD_VARDA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Nienna provides the Celestial school at 1/2 the prayer skill
		[GOD_NIENNA] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Este provides the Celestial school at 1/3 the prayer skill
		[GOD_ESTE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
		-- Vaire provides the Celestial school at 1/3 the prayer skill
		[GOD_VAIRE] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 3,
		},
	},
}

SCHOOL_HELL = add_school
{
	["name"] = "Hell", 
	["skill"] = SKILL_HELL,
	["spell_power"] = TRUE,
	["sorcery"] = TRUE,
	["gods"] =
	{
		-- Melkor provides the Hell school at 2/5 the prayer skill
		[GOD_MELKOR] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 2,
			["div"] = 5,
		},
		-- Mandos provides the Hell school at 1/2 the prayer skill
		[GOD_MANDOS] =
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 2,
		},
		-- Ingeborg provides every school at 1/10 the prayer skill
		[GOD_INGEBORG] = 
		{
			["skill"] = SKILL_PRAY,
			["mul"] = 1,
			["div"] = 10,
		},
	},
}

-- Put some spells
tome_dofile("s_fire.lua")
tome_dofile("s_mana.lua")
tome_dofile("s_water.lua")
tome_dofile("s_air.lua")
tome_dofile("s_earth.lua")
tome_dofile("s_convey.lua")
tome_dofile("s_nature.lua")
tome_dofile("s_divin.lua")
tome_dofile("s_tempo.lua")
tome_dofile("s_meta.lua")
tome_dofile("s_mind.lua")
tome_dofile("s_udun.lua")
tome_dofile("s_geom.lua")

-- God's specific spells
tome_dofile("s_eru.lua")
tome_dofile("s_manwe.lua")
tome_dofile("s_tulkas.lua")
tome_dofile("s_melkor.lua")
tome_dofile("s_yavann.lua")
tome_dofile("s_amybsod.lua")

-- New gods' spells
tome_dofile("s_aule.lua")
tome_dofile("s_varda.lua")
tome_dofile("s_ulmo.lua")
tome_dofile("s_mandos.lua")

-- Specific schools
tome_dofile("s_demon.lua")

-- Device spells
tome_dofile("s_stick.lua")

-- Musics
tome_dofile("s_music.lua")

tome_dofile("s_hell.lua")
tome_dofile("s_celest.lua")
tome_dofile("s_orome.lua")
tome_dofile("s_ingebo.lua")
tome_dofile("s_nienna.lua")
tome_dofile("s_este.lua")
tome_dofile("s_vana.lua")
tome_dofile("s_vaire.lua")

-- List of spellbooks

-- Create the crystal of mana
school_book[0] = {
	MANATHRUST, DELCURSES, RESISTS, MANASHIELD,
}

-- The book of the eternal flame
school_book[1] = {
	GLOBELIGHT, FIREGOLEM, FIREFLASH, FIREWALL, FIERYAURA,
}

-- The book of the blowing winds
school_book[2] = {
	NOXIOUSCLOUD, POISONBLOOD, INVISIBILITY, STERILIZE, AIRWINGS, THUNDERSTORM,
}

-- The book of the impenetrable earth
school_book[3] = {
	STONESKIN, DIG, STONEPRISON, SHAKE, STRIKE,
}

-- The book of the unstopable wave
school_book[4] = {
	GEYSER, VAPOR, ENTPOTION, TIDALWAVE, ICESTORM,
}

-- Create the book of translocation
school_book[5] = {
	BLINK, TELEPORT, TELEAWAY, RECALL, PROBABILITY_TRAVEL,
}

-- Create the book of the tree
school_book[6] = {
	GROWTREE, HEALING, RECOVERY, REGENERATION, SUMMONANNIMAL,
}

-- Create the book of Knowledge
school_book[7] = {
	SENSEMONSTERS, SENSEHIDDEN, REVEALWAYS, IDENTIFY, VISION, STARIDENTIFY,
}

-- Create the book of the Time
school_book[8] = {
	MAGELOCK, SLOWMONSTER, ESSENCESPEED, BANISHMENT,
}

-- Create the book of meta spells
school_book[9] = {
	DISARM, RECHARGE, DISPERSEMAGIC, SPELLBINDER, TRACKER, INERTIA_CONTROL,
}

-- Create the book of the mind
school_book[10] = {
	CHARM, CONFUSE, ARMOROFFEAR, STUN, MIND_BOLT,
}

-- Create the book of hellflame
school_book[11] = {
	DRAIN, GENOCIDE, WRAITHFORM, FLAMEOFUDUN,
}

-- Create the book of hell
school_book[12] = {
	NETHEREYE, GOODNIGHT, NERVE_POISON, TIMELAPSE, ROCKETLAUNCHER, NUCLEARMISSILE,
}

-- Create the book of heavens
school_book[13] = {
	FORCEBOLT, CLEANINGFIRE, GROUNDATTACK, CRYSTALSHARDS, TUNGUSKA,
}

-- Create the book of eru
school_book[20] = {
	ERU_SEE, ERU_LISTEN, ERU_UNDERSTAND, ERU_PROT,
}

-- Create the book of manwe
school_book[21] = {
	MANWE_BLESS, MANWE_SHIELD, MANWE_CALL, MANWE_AVATAR,
}

-- Create the book of tulkas
school_book[22] = {
	TULKAS_AIM, TULKAS_SPIN, TULKAS_WAVE,
}

-- Create the book of melkor
school_book[23] = {
	MELKOR_CURSE, MELKOR_CORPSE_EXPLOSION, MELKOR_MIND_STEAL,
}

-- Create the book of yavanna
school_book[24] = {
	YAVANNA_CHARM_ANIMAL, YAVANNA_GROW_GRASS, YAVANNA_TREE_ROOTS, YAVANNA_WATER_BITE, YAVANNA_UPROOT,
}

-- Create the book of amybsod
school_book[25] = {
	AMYBSOD_SOUND, AMYBSOD_DARK, AMYBSOD_TROLL,
}

-- Create the book of orome
school_book[26] = {
	OROME_WILDSENSE, OROME_BULLSEYE, OROME_SNIPER, OROME_GOODHUNT, OROME_RAPIDFIRE,
}

-- Create the book of ingeborg
school_book[27] = {
	INGEBORG_EVERYTHING_BURNS, INGEBORG_WILD_MAGIC, INGEBORG_DISINTEGRATE, INGEBORG_STAR_DESTRUCTION, INGEBORG_BLACK_HOLE, 
}

-- Create the book of nienna
school_book[28] = {
	NIENNA_ALCHEMIZE, NIENNA_RUNEFORGE, NIENNA_BOMB_SQUAD_DUTY, NIENNA_BLOODY_TEARS, NIENNA_WEEPING, 
}

-- Create the book of este
school_book[29] = {
	ESTE_DANCING_LESSONS, ESTE_REVEAL_TRAP, ESTE_DESTROY_TRAP, 
}

-- Create the book of vana
school_book[30] = {
	VANA_ICY_WIND, VANA_FREEZING_CLOUD, VANA_RESTOR, VANA_PURIFY,
}

-- Create the book of vaire
school_book[40] = {
	VAIRE_TICK_TOCK, VAIRE_SPIDER_FORM, VAIRE_TIMELINE, VAIRE_DECONTAMINATE, VAIRE_TIME_TEMPEST,
}

-- Dual school books by Amy
-- The book of fire and ice
school_book[31] = {
	GLOBELIGHT, FIREGOLEM, FIREFLASH, FIREWALL, FIERYAURA,
	GEYSER, VAPOR, ENTPOTION, TIDALWAVE, ICESTORM,
}

-- The book of air and earth
school_book[32] = {
	STONESKIN, DIG, STONEPRISON, SHAKE, STRIKE,
	NOXIOUSCLOUD, POISONBLOOD, INVISIBILITY, STERILIZE, AIRWINGS, THUNDERSTORM,
}

-- The book of mana and meta
school_book[33] = {
	MANATHRUST, DELCURSES, RESISTS, MANASHIELD,
	DISARM, RECHARGE, DISPERSEMAGIC, SPELLBINDER, TRACKER, INERTIA_CONTROL,
}

-- The book of conveyance and divination
school_book[34] = {
	BLINK, TELEPORT, TELEAWAY, RECALL, PROBABILITY_TRAVEL,
	SENSEMONSTERS, SENSEHIDDEN, REVEALWAYS, IDENTIFY, VISION, STARIDENTIFY,
}

-- The book of nature and mind
school_book[35] = {
	GROWTREE, HEALING, RECOVERY, REGENERATION, SUMMONANNIMAL,
	CHARM, CONFUSE, ARMOROFFEAR, STUN, MIND_BOLT,
}

-- The book of time and udun
school_book[36] = {
	MAGELOCK, SLOWMONSTER, ESSENCESPEED, BANISHMENT,
	DRAIN, GENOCIDE, WRAITHFORM, FLAMEOFUDUN,
}

-- The book of heaven and hell
school_book[37] = {
	NETHEREYE, GOODNIGHT, NERVE_POISON, TIMELAPSE, ROCKETLAUNCHER, NUCLEARMISSILE,
	FORCEBOLT, CLEANINGFIRE, GROUNDATTACK, CRYSTALSHARDS, TUNGUSKA,
}

-- Create the book of beginner's cantrip
school_book[50] = {
	MANATHRUST, GLOBELIGHT, ENTPOTION, BLINK, SENSEMONSTERS, SENSEHIDDEN,
}

-- Create the book of teleporatation
school_book[51] = {
	BLINK, TELEPORT, TELEAWAY
}

-- Create the book of summoning
school_book[52] = {
	FIREGOLEM, SUMMONANNIMAL
}


-- Create the Armageddon Demonblade
school_book[55] = {
	DEMON_BLADE, DEMON_MADNESS, DEMON_FIELD,
}

-- Create the Shield Demonblade
school_book[56] = {
	DOOM_SHIELD, DEMON_CLOAK, UNHOLY_WORD,
}

-- Create the Control Demonblade
school_book[57] = {
	DEMON_SUMMON, DISCHARGE_MINION, CONTROL_DEMON,
}

-- Create the Drums
school_book[58] = {
	MUSIC_STOP, MUSIC_HOLD, MUSIC_CONF, MUSIC_STUN, 
}

-- Create the Harps
school_book[59] = {
	MUSIC_STOP, MUSIC_LITE, MUSIC_HERO, MUSIC_HEAL, MUSIC_TIME, MUSIC_MIND,
}

-- Create the Horns
school_book[60] = {
	MUSIC_STOP, MUSIC_BLOW, MUSIC_WIND, MUSIC_YLMIR, MUSIC_AMBARKANTA,
}

-- Book of the Player, filled in by the Library Quest
school_book[61] = { }

-- Geomancy spells, not a real book
school_book[62] = {
	CALL_THE_ELEMENTS, CHANNEL_ELEMENTS, ELEMENTAL_WAVE, VAPORIZE, GEOLYSIS, DRIPPING_TREAD, GROW_BARRIER, ELEMENTAL_MINION
}

-- Aule book
school_book[63] =
{ 
	AULE_FIREBRAND, AULE_ENCHANT_WEAPON, AULE_ENCHANT_ARMOUR, AULE_CHILD,
} 

-- Varda book
school_book[64] =
{ 
	VARDA_LIGHT_VALINOR, VARDA_CALL_ALMAREN, VARDA_EVENSTAR, VARDA_STARKINDLER,
} 

-- Ulmo book
school_book[65] =
{ 	
	ULMO_BELEGAER, ULMO_DRAUGHT_ULMONAN, ULMO_CALL_ULUMURI, ULMO_WRATH,
} 

-- Mandos book
school_book[66] =
{ 
	MANDOS_TEARS_LUTHIEN, MANDOS_SPIRIT_FEANTURI, MANDOS_TALE_DOOM, MANDOS_CALL_HALLS
}

-- Banjo
school_book[72] = {
	MUSIC_STOP, MUSIC_PEACE, MUSIC_MASTER, MUSIC_ELEMENT, MUSIC_DEPTHS
}

-- Lute
school_book[73] = {
	MUSIC_STOP, MUSIC_DODGE, MUSIC_SAVING, MUSIC_MINER, MUSIC_STEALTH
}

-- Mandolin
school_book[74] = {
	MUSIC_STOP, MUSIC_WAR, MUSIC_TANK, MUSIC_SLAUGHTER, MUSIC_MIGHT
}

