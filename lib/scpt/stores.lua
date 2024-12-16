-- Whats shops can buy what
store_buy_list
{
	["General Store"] =
	{
		TV_CORPSE,
		TV_FOOD,
		TV_LITE,
		TV_FLASK,
		TV_SPIKE,
		TV_SHOT,
		TV_ARROW,
		TV_BOLT,
		TV_DIGGING,
		TV_CLOAK,
		TV_BOTTLE,
		TV_TRAPKIT,
	},
	["Common Shop"] =
	{
		TV_CORPSE,
		TV_FOOD,
		TV_LITE,
		TV_FLASK,
		TV_SPIKE,
		TV_SHOT,
		TV_ARROW,
		TV_BOLT,
		TV_DIGGING,
		TV_CLOAK,
		TV_BOTTLE,
		TV_TRAPKIT,
	},
	["Mining Supply store"] =
	{
		TV_CORPSE,
		TV_FOOD,
		TV_LITE,
		TV_FLASK,
		TV_SPIKE,
		TV_SHOT,
		TV_ARROW,
		TV_BOLT,
		TV_DIGGING,
		TV_CLOAK,
		TV_BOTTLE,
		TV_TRAPKIT,
	},
	["Armory"] =
	{
		TV_BOOTS,
		TV_GLOVES,
		TV_CROWN,
		TV_HELM,
		TV_SHIELD,
		TV_CLOAK,
		TV_SOFT_ARMOR,
		TV_HARD_ARMOR,
		TV_DRAG_ARMOR,
	},
	["Dragon Hunter"] =
	{
		TV_BOOTS,
		TV_GLOVES,
		TV_CROWN,
		TV_HELM,
		TV_SHIELD,
		TV_CLOAK,
		TV_SOFT_ARMOR,
		TV_HARD_ARMOR,
		TV_DRAG_ARMOR,
	},
	["Footwear Shop"] =
	{
		TV_BOOTS,
		TV_GLOVES,
		TV_CROWN,
		TV_HELM,
		TV_SHIELD,
		TV_CLOAK,
		TV_SOFT_ARMOR,
		TV_HARD_ARMOR,
		TV_DRAG_ARMOR,
	},
	["Rare Footwear Shop"] =
	{
		TV_BOOTS,
		TV_GLOVES,
		TV_CROWN,
		TV_HELM,
		TV_SHIELD,
		TV_CLOAK,
		TV_SOFT_ARMOR,
		TV_HARD_ARMOR,
		TV_DRAG_ARMOR,
	},
	["Weaponsmith"] =
	{
		TV_SHOT,
		TV_BOLT,
		TV_ARROW,
		TV_BOOMERANG,
		TV_BOW,
		TV_DIGGING,
		TV_HAFTED,
		TV_POLEARM,
		TV_SWORD,
		TV_AXE,
		TV_MSTAFF,
	},
	["Master Archer"] =
	{
		TV_SHOT,
		TV_BOLT,
		TV_ARROW,
		TV_BOOMERANG,
		TV_BOW,
		TV_DIGGING,
		TV_HAFTED,
		TV_POLEARM,
		TV_SWORD,
		TV_AXE,
		TV_MSTAFF,
	},
	["Axe Smith"] =
	{
		TV_SHOT,
		TV_BOLT,
		TV_ARROW,
		TV_BOOMERANG,
		TV_BOW,
		TV_DIGGING,
		TV_HAFTED,
		TV_POLEARM,
		TV_SWORD,
		TV_AXE,
		TV_MSTAFF,
	},
	["Hafted Smith"] =
	{
		TV_SHOT,
		TV_BOLT,
		TV_ARROW,
		TV_BOOMERANG,
		TV_BOW,
		TV_DIGGING,
		TV_HAFTED,
		TV_POLEARM,
		TV_SWORD,
		TV_AXE,
		TV_MSTAFF,
	},
	["Polearm Smith"] =
	{
		TV_SHOT,
		TV_BOLT,
		TV_ARROW,
		TV_BOOMERANG,
		TV_BOW,
		TV_DIGGING,
		TV_HAFTED,
		TV_POLEARM,
		TV_SWORD,
		TV_AXE,
		TV_MSTAFF,
	},
	["Sword Smith"] =
	{
		TV_SHOT,
		TV_BOLT,
		TV_ARROW,
		TV_BOOMERANG,
		TV_BOW,
		TV_DIGGING,
		TV_HAFTED,
		TV_POLEARM,
		TV_SWORD,
		TV_AXE,
		TV_MSTAFF,
	},
	["Craft Chief"] =
	{
		TV_SHOT,
		TV_BOLT,
		TV_ARROW,
		TV_BOOMERANG,
		TV_BOW,
		TV_DIGGING,
		TV_HAFTED,
		TV_POLEARM,
		TV_SWORD,
		TV_AXE,
		TV_MSTAFF,
	},
	-- We use a function because we want to restrict to blessed weapons and god spells
	["Temple"] = function (obj)
		if obj.tval == TV_DRUID_BOOK then return TRUE
		elseif obj.tval == TV_BOOK and obj.sval == 255 and (can_spell_random(obj.pval) == SKILL_SPIRITUALITY) then return TRUE
		elseif obj.tval == TV_SCROLL then return TRUE
		elseif obj.tval == TV_POTION2 then return TRUE
		elseif obj.tval == TV_POTION then return TRUE
		elseif obj.tval == TV_HAFTED then return TRUE
		elseif obj.tval == TV_POLEARM and is_blessed(obj) == TRUE then return TRUE
		elseif obj.tval == TV_SWORD and is_blessed(obj) == TRUE then return TRUE
		elseif obj.tval == TV_AXE and is_blessed(obj) == TRUE then return TRUE
		elseif obj.tval == TV_BOOMERANG and is_blessed(obj) == TRUE then return TRUE
		end
	end,
	["Alchemy shop"] =
	{
		TV_SCROLL,
		TV_POTION2,
		TV_POTION,
		TV_BATERIE,
		TV_BOTTLE,
	},
	["Rare Jewelry Shop"] =
	{
		TV_AMULET,
		TV_RING,
	},
	["Jewelry Shop"] =
	{
		TV_AMULET,
		TV_RING,
	},
	["Scribe"] =
	{
		TV_SCROLL,
	},
	["Master Scribe"] =
	{
		TV_SCROLL,
	},
	["Recaller"] =
	{
		TV_SCROLL,
	},
	["Potion Store"] =
	{
		TV_POTION,
		TV_POTION2,
	},
	["Super Potion Store"] =
	{
		TV_POTION,
		TV_POTION2,
	},
	["Speed Ring Market"] =
	{
		TV_AMULET,
		TV_RING,
	},
	-- We use a function because we dont want god spells
	["Magic shop"] = function (obj)
		local buy =
		{
			[TV_SYMBIOTIC_BOOK] = TRUE,
			[TV_AMULET] = TRUE,
			[TV_RING] = TRUE,
			[TV_STAFF] = TRUE,
			[TV_WAND] = TRUE,
			[TV_ROD] = TRUE,
			[TV_ROD_MAIN] = TRUE,
			[TV_SCROLL] = TRUE,
			[TV_POTION2] = TRUE,
			[TV_POTION] = TRUE,
			[TV_MSTAFF] = TRUE,
			[TV_RANDART] = TRUE,
		}

		if obj.tval == TV_BOOK and obj.sval == 255 and (can_spell_random(obj.pval) == SKILL_MAGIC) then return TRUE
		elseif obj.tval == TV_BOOK and obj.sval ~= 255 then return TRUE
		elseif buy[obj.tval] == TRUE then return TRUE
		end
	end,
	-- Black markets wants ALL!
	["Black Market"] = function (obj)
		return TRUE
	end,
	["Expensive Black Market"] = function (obj)
		return TRUE
	end,
	["Book Store"] =
	{
		TV_BOOK,
		TV_SYMBIOTIC_BOOK,
		TV_MUSIC_BOOK,
		TV_DAEMON_BOOK,
		TV_DRUID_BOOK,
		TV_INSTRUMENT,
	},
	["Wand Shop"] =
	{
		TV_ROD,
		TV_ROD_MAIN,
		TV_WAND,
		TV_STAFF,
	},
	["Misc Shop"] =
	{
		TV_MSTAFF,
		TV_EGG,
		TV_INSTRUMENT,
		TV_TRAPKIT,
		TV_FOOD,
	},
	["Library"] =
	{
		TV_BOOK,
		TV_SYMBIOTIC_BOOK,
		TV_MUSIC_BOOK,
		TV_DAEMON_BOOK,
		TV_DRUID_BOOK,
		TV_INSTRUMENT,
	},
	["Junk Shop"] =
	{
		TV_BOTTLE,
		TV_BATERIE,
		TV_SPIKE,
		TV_PARCHMENT,
		TV_RUNE1,
		TV_RUNE2,
	},
	["Butchery"] =
	{
		TV_CORPSE,
		TV_FOOD,
	},
	["Forbidden Library"] =
	{
		TV_BOOK,
		TV_SYMBIOTIC_BOOK,
		TV_MUSIC_BOOK,
		TV_DAEMON_BOOK,
		TV_DRUID_BOOK,
		TV_INSTRUMENT,
	},
	["Type of Arms"] =
	{
		TV_BOOTS,
		TV_GLOVES,
		TV_CROWN,
		TV_HELM,
		TV_SHIELD,
		TV_CLOAK,
		TV_SOFT_ARMOR,
		TV_HARD_ARMOR,
		TV_DRAG_ARMOR,
	},
	["Armor Emporium"] =
	{
		TV_BOOTS,
		TV_GLOVES,
		TV_CROWN,
		TV_HELM,
		TV_SHIELD,
		TV_CLOAK,
		TV_SOFT_ARMOR,
		TV_HARD_ARMOR,
		TV_DRAG_ARMOR,
	},
	["Pet Shop"] =
	{
		TV_EGG,
	},
}

-- Test only
function out_sticks()
	local i
	for i = 0, __tmp_spells_num - 1 do
		if __tmp_spells[i].stick then
			if __tmp_spells[i].stick[TV_WAND] then
				print("Wand: " .. __tmp_spells[i].name)
			end
		end
	end
	for i = 0, __tmp_spells_num - 1 do
		if __tmp_spells[i].stick then
			if __tmp_spells[i].stick[TV_STAFF] then
				print("Staff: " .. __tmp_spells[i].name)
			end
		end
	end
end

-- Take care to have Magic shop/Temple have specific spells only
add_hooks
{
	[HOOK_STORE_STOCK] = function (index, name, level)
			if name == "Magic shop" then
				-- Books
				if magik(20) == TRUE then
					object_prep(obj_forge, lookup_kind(TV_BOOK, 255))
					local spell = get_random_spell(SKILL_MAGIC, 20)
					if spell > -1 then
						obj_forge.pval = spell
						return TRUE, obj_forge
					end
				end
			elseif name == "Temple" then
				if magik(20) == TRUE then
					object_prep(obj_forge, lookup_kind(TV_BOOK, 255))
					local spell = get_random_spell(SKILL_SPIRITUALITY, 20)
					if spell > -1 then
						obj_forge.pval = spell
						return TRUE, obj_forge
					end
				end
			end
	end,
}
