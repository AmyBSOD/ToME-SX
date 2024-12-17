/* File: randart.c */

/* Purpose: Randart creation code */

/*
 * Copyright (c) 2001 DarkGod
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

/* Chance of using syllables to form the name instead of the "template" files */
#define TABLE_NAME      45
#define A_CURSED        13
#define WEIRD_LUCK      12
#define ACTIVATION_CHANCE 3

/*
 * Attempt to add a power to a randart
 */
static bool grab_one_power(int *ra_idx, object_type *o_ptr, bool good, s16b *max_times)
{
	int i = 0, j;
	int *ok_ra, ok_num = 0;
	bool ret = FALSE;
	u32b f1, f2, f3, f4, f5, esp;

	C_MAKE(ok_ra, max_ra_idx, int);

	/* Grab the ok randart */
	for (i = 0; i < max_ra_idx; i++)
	{
		randart_part_type *ra_ptr = &ra_info[i];
		bool ok = FALSE;

		/* Must have the correct fields */
		for (j = 0; j < 30; j++)
		{
			if (ra_ptr->tval[j] == o_ptr->tval)
			{
				if ((ra_ptr->min_sval[j] <= o_ptr->sval) && (ra_ptr->max_sval[j] >= o_ptr->sval)) ok = TRUE;
			}

			if (ok) break;
		}
		if ((0 < ra_ptr->max_pval) && (ra_ptr->max_pval < o_ptr->pval)) ok = FALSE;
		if (!ok)
		{
			/* Doesnt count as a try*/
			continue;
		}

		/* Good should be good, bad should be bad */
		if (good && (ra_ptr->value <= 0)) continue;
		if ((!good) && (ra_ptr->value > 0)) continue;

		if (max_times[i] >= ra_ptr->max) continue;

		/* Must NOT have the antagonic flags */
		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);
		if (f1 & ra_ptr->aflags1) continue;
		if (f2 & ra_ptr->aflags2) continue;
		if (f3 & ra_ptr->aflags3) continue;
		if (f4 & ra_ptr->aflags4) continue;
		if (f5 & ra_ptr->aflags5) continue;
		if (esp & ra_ptr->aesp) continue;

		/* ok */
		ok_ra[ok_num++] = i;
	}

	/* Now test them a few times */
	for (i = 0; i < ok_num * 10; i++)
	{
		randart_part_type *ra_ptr;
		int maxposslvl;

		maxposslvl = p_ptr->lev;
		if (dun_level > maxposslvl) maxposslvl = dun_level;
		maxposslvl += get_skill(SKILL_RUNECRAFT);

		i = ok_ra[rand_int(ok_num)];
		ra_ptr = &ra_info[i];

		/* XXX XXX Enforce minimum player level (loosely)
		 * Amy edit: it's dumb if that depends on your XL, it should depend on dungeon level like ego items!!
		 * calculation now uses maxposslvl, which can also be improved by runecraft skill */
		if (ra_ptr->level > maxposslvl)
		{
			/* Acquire the "out-of-depth factor" */
			int d = (ra_ptr->level - maxposslvl);

			/* Roll for out-of-depth creation */
			if (rand_int(d) != 0)
			{
				continue;
			}
		}

		/* We must make the "rarity roll" */

		/*msg_format("Randart part %d rolling mrarity %d against rarity %d", i, ra_ptr->mrarity, ra_ptr->rarity);*/

		if (rand_int(ra_ptr->mrarity) < ra_ptr->rarity)
		{
			continue;
		}

		/* Hack -- mark the item as an ego */
		*ra_idx = i;
		max_times[i]++;

		/* Success */
		ret = TRUE;
		break;
	}

	C_FREE(ok_ra, max_ra_idx, int);

	/* Return */
	return (ret);
}

static void add_xtra_arti_power(object_type *o_ptr)
{
	int attempts = 50000;

	while (attempts > 0) {
		attempts--;

		switch (randint(345)) {

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				o_ptr->art_flags1 |= TR1_STR;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				o_ptr->art_flags1 |= TR1_DEX;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				o_ptr->art_flags1 |= TR1_INT;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
				o_ptr->art_flags1 |= TR1_WIS;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 21:
			case 22:
			case 23:
			case 24:
			case 25:
				o_ptr->art_flags1 |= TR1_CON;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
				o_ptr->art_flags1 |= TR1_CHR;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 31:
			case 32:
				o_ptr->art_flags1 |= TR1_MANA;
				if (randint(2) == 1) o_ptr->pval -= randint(2);
				else o_ptr->pval += randint(2);
				return;
			case 33:
			case 34:
				o_ptr->art_flags1 |= TR1_SPELL;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
				o_ptr->art_flags1 |= TR1_STEALTH;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
				o_ptr->art_flags1 |= TR1_SEARCH;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 45:
			case 46:
			case 47:
			case 48:
			case 49:
				o_ptr->art_flags1 |= TR1_INFRA;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 50:
			case 51:
			case 52:
			case 53:
			case 54:
				o_ptr->art_flags1 |= TR1_TUNNEL;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 55:
				o_ptr->art_flags1 |= TR1_SPEED;
				if (randint(2) == 1) o_ptr->pval -= randint(3);
				else o_ptr->pval += randint(3);
				return;
			case 56:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_BLOWS;
					if (randint(2) == 1) o_ptr->pval -= randint(2);
					else o_ptr->pval += randint(2);
					return;
				}
				continue;
			case 57:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_CHAOTIC;
					return;
				}
				continue;
			case 58:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_VAMPIRIC;
					return;
				}
				continue;
			case 59:
			case 60:
			case 61:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_SLAY_ANIMAL;
					return;
				}
				continue;
			case 62:
			case 63:
			case 64:
			case 65:
			case 66:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_SLAY_EVIL;
					return;
				}
				continue;
			case 67:
			case 68:
			case 69:
			case 70:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_SLAY_UNDEAD;
					return;
				}
				continue;
			case 71:
			case 72:
			case 73:
			case 74:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_SLAY_DEMON;
					return;
				}
				continue;
			case 75:
			case 76:
			case 77:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_SLAY_ORC;
					return;
				}
				continue;
			case 78:
			case 79:
			case 80:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_SLAY_TROLL;
					return;
				}
				continue;
			case 81:
			case 82:
			case 83:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_SLAY_GIANT;
					return;
				}
				continue;
			case 84:
			case 85:
			case 86:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_SLAY_DRAGON;
					return;
				}
				continue;
			case 87:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_KILL_DRAGON;
					return;
				}
				continue;
			case 88:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_VORPAL;
					return;
				}
				continue;
			case 89:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_IMPACT;
					return;
				}
				continue;
			case 90:
			case 91:
			case 92:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_BRAND_POIS;
					return;
				}
				continue;
			case 93:
			case 94:
			case 95:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_BRAND_ACID;
					return;
				}
				continue;
			case 96:
			case 97:
			case 98:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_BRAND_ELEC;
					return;
				}
				continue;
			case 99:
			case 100:
			case 101:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_BRAND_FIRE;
					return;
				}
				continue;
			case 102:
			case 103:
			case 104:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags1 |= TR1_BRAND_COLD;
					return;
				}
				continue;
			case 105:
			case 106:
			case 107:
			case 108:
			case 109:
				o_ptr->art_flags2 |= TR2_SUST_STR;
				return;
			case 110:
			case 111:
			case 112:
			case 113:
			case 114:
				o_ptr->art_flags2 |= TR2_SUST_INT;
				return;
			case 115:
			case 116:
			case 117:
			case 118:
			case 119:
				o_ptr->art_flags2 |= TR2_SUST_WIS;
				return;
			case 120:
			case 121:
			case 122:
			case 123:
			case 124:
				o_ptr->art_flags2 |= TR2_SUST_DEX;
				return;
			case 125:
			case 126:
			case 127:
			case 128:
			case 129:
				o_ptr->art_flags2 |= TR2_SUST_CON;
				return;
			case 130:
			case 131:
			case 132:
			case 133:
			case 134:
				o_ptr->art_flags2 |= TR2_SUST_CHR;
				return;
			case 135:
			case 136:
			case 137:
				o_ptr->art_flags2 |= TR2_INVIS;
				return;
			case 138:
				o_ptr->art_flags2 |= TR2_LIFE;
				if (randint(2) == 1) o_ptr->pval -= randint(2);
				else o_ptr->pval += randint(2);
				return;
			case 139:
				o_ptr->art_flags2 |= TR2_IM_ACID;
				return;
			case 140:
				o_ptr->art_flags2 |= TR2_IM_COLD;
				return;
			case 141:
				o_ptr->art_flags2 |= TR2_IM_ELEC;
				return;
			case 142:
				o_ptr->art_flags2 |= TR2_IM_FIRE;
				return;
			case 143:
				o_ptr->art_flags2 |= TR2_SENS_FIRE;
				return;
			case 144:
				o_ptr->art_flags2 |= TR2_REFLECT;
				return;
			case 145:
			case 146:
			case 147:
			case 148:
			case 149:
			case 150:
			case 151:
			case 152:
			case 153:
			case 154:
				o_ptr->art_flags2 |= TR2_FREE_ACT;
				return;
			case 155:
			case 156:
			case 157:
				o_ptr->art_flags2 |= TR2_HOLD_LIFE;
				return;
			case 158:
			case 159:
			case 160:
			case 161:
			case 162:
				o_ptr->art_flags2 |= TR2_RES_ACID;
				return;
			case 163:
			case 164:
			case 165:
			case 166:
			case 167:
				o_ptr->art_flags2 |= TR2_RES_ELEC;
				return;
			case 168:
			case 169:
			case 170:
			case 171:
			case 172:
				o_ptr->art_flags2 |= TR2_RES_FIRE;
				return;
			case 173:
			case 174:
			case 175:
			case 176:
			case 177:
				o_ptr->art_flags2 |= TR2_RES_COLD;
				return;
			case 178:
			case 179:
			case 180:
			case 181:
				o_ptr->art_flags2 |= TR2_RES_POIS;
				return;
			case 182:
			case 183:
			case 184:
			case 185:
			case 186:
				o_ptr->art_flags2 |= TR2_RES_FEAR;
				return;
			case 187:
			case 188:
			case 189:
				o_ptr->art_flags2 |= TR2_RES_LITE;
				return;
			case 190:
			case 191:
			case 192:
				o_ptr->art_flags2 |= TR2_RES_DARK;
				return;
			case 193:
			case 194:
			case 195:
			case 196:
				o_ptr->art_flags2 |= TR2_RES_BLIND;
				return;
			case 197:
			case 198:
			case 199:
			case 200:
				o_ptr->art_flags2 |= TR2_RES_CONF;
				return;
			case 201:
			case 202:
			case 203:
				o_ptr->art_flags2 |= TR2_RES_SOUND;
				return;
			case 204:
			case 205:
			case 206:
				o_ptr->art_flags2 |= TR2_RES_SHARDS;
				return;
			case 207:
			case 208:
			case 209:
				o_ptr->art_flags2 |= TR2_RES_NETHER;
				return;
			case 210:
			case 211:
			case 212:
				o_ptr->art_flags2 |= TR2_RES_NEXUS;
				return;
			case 213:
			case 214:
			case 215:
				o_ptr->art_flags2 |= TR2_RES_CHAOS;
				return;
			case 216:
			case 217:
			case 218:
				o_ptr->art_flags2 |= TR2_RES_DISEN;
				return;
			case 219:
				o_ptr->art_flags3 |= TR3_SH_FIRE;
				return;
			case 220:
				o_ptr->art_flags3 |= TR3_SH_ELEC;
				return;
			case 221:
				o_ptr->art_flags3 |= TR3_AUTO_CURSE;
				return;
			case 222:
				o_ptr->art_flags3 |= TR3_NO_TELE;
				return;
			case 223:
				o_ptr->art_flags3 |= TR3_NO_MAGIC;
				return;
			case 224:
				if (randint(20) == 1) o_ptr->art_flags3 |= TR3_TY_CURSE;
				return;
			case 225:
			case 226:
			case 227:
			case 228:
				o_ptr->art_flags3 |= TR3_FEATHER;
				return;
			case 229:
			case 230:
			case 231:
				o_ptr->art_flags3 |= TR3_LITE1;
				return;
			case 232:
			case 233:
			case 234:
			case 235:
				o_ptr->art_flags3 |= TR3_SEE_INVIS;
				return;
			case 236:
			case 237:
			case 238:
			case 239:
				o_ptr->art_flags3 |= TR3_SLOW_DIGEST;
				return;
			case 240:
			case 241:
			case 242:
				o_ptr->art_flags3 |= TR3_REGEN;
				return;
			case 243:
			case 244:
			case 245:
				if (o_ptr->tval == TV_BOW) {
					o_ptr->art_flags3 |= TR3_XTRA_MIGHT;
				}
				return;
			case 246:
			case 247:
			case 248:
				if (o_ptr->tval == TV_BOW) {
					o_ptr->art_flags3 |= TR3_XTRA_SHOTS;
				}
				return;
			case 249:
				o_ptr->art_flags3 |= TR3_DRAIN_EXP;
				return;
			case 250:
			case 251:
			case 252:
				o_ptr->art_flags3 |= TR3_TELEPORT;
				return;
			case 253:
				o_ptr->art_flags3 |= TR3_AGGRAVATE;
				return;
			case 254:
			case 255:
			case 256:
				o_ptr->art_flags3 |= TR3_BLESSED;
				return;
			case 257:
				if (randint(100) == 1) o_ptr->art_flags4 |= TR4_PRECOGNITION;
				return;
			case 258:
				if (randint(20) == 1) o_ptr->art_flags4 |= TR4_BLACK_BREATH;
				return;
			case 259:
			case 260:
				o_ptr->art_flags4 |= TR4_FLY;
				return;
			case 261:
				if (randint(1000) == 1) o_ptr->art_flags4 |= TR4_DG_CURSE;
				return;
			case 262:
				if (randint(20) == 1) o_ptr->art_flags4 |= TR4_CLIMB;
				return;
			case 263:
				switch (randint(4)) {
					case 1: o_ptr->art_flags4 |= TR4_ANTIMAGIC_50; return;
					case 2: o_ptr->art_flags4 |= TR4_ANTIMAGIC_30; return;
					case 3: o_ptr->art_flags4 |= TR4_ANTIMAGIC_20; return;
					case 4: o_ptr->art_flags4 |= TR4_ANTIMAGIC_10; return;
				}
				return;
			case 264:
				o_ptr->art_flags4 |= TR4_IM_NETHER;
				return;
			case 265:
				if (randint(500) == 1) o_ptr->art_flags4 |= TR4_AUTO_ID;
				return;
			case 266:
				if (randint(5) == 1) o_ptr->art_flags4 |= TR4_LITE3;
				else o_ptr->art_flags4 |= TR4_LITE2;
				return;
			case 267:
				o_ptr->art_flags4 |= TR4_CURSE_NO_DROP;
				return;
			case 268:
				o_ptr->art_flags5 |= TR5_DRAIN_MANA;
				return;
			case 269:
				o_ptr->art_flags5 |= TR5_DRAIN_HP;
				return;
			case 270:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags5 |= TR5_KILL_DEMON;
					return;
				}
				continue;
			case 271:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags5 |= TR5_KILL_UNDEAD;
					return;
				}
				continue;
			case 272:
				o_ptr->art_flags5 |= TR5_CRIT;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 273:
			case 274:
			case 275:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_BOOMERANG || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags5 |= TR5_WOUNDING;
				}
				continue;
			case 276:
			case 277:
			case 278:
				o_ptr->art_flags5 |= TR5_LUCK;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 279:
				o_ptr->art_flags5 |= TR5_SPELL_CONTAIN;
				o_ptr->art_flags5 |= TR5_WIELD_CAST;
				return;
			case 280:
			case 281:
			case 282:
				if ( (o_ptr->tval >= TV_HAFTED && o_ptr->tval <= TV_AXE) || o_ptr->tval == TV_MSTAFF ) {
					o_ptr->art_flags5 |= TR5_RES_MORGUL;
				}
				continue;
			case 283:
				if (randint(1000) == 1) o_ptr->art_flags5 |= TR5_MAGIC_BREATH;
				return;
			case 284:
				if (randint(200) == 1) o_ptr->art_flags5 |= TR5_WATER_BREATH;
				return;
			case 285:
				o_ptr->art_flags5 |= TR5_RES_WATER;
				return;
			case 286:
				o_ptr->art_flags5 |= TR5_RES_INERTIA;
				return;
			case 287:
				o_ptr->art_flags5 |= TR5_IM_POISON;
				return;
			case 288:
			case 289:
			case 290:
				o_ptr->art_flags5 |= TR5_PEACEKEEPING;
				return;
			case 291:
			case 292:
			case 293:
				o_ptr->art_flags5 |= TR5_INV_PROTECT;
				return;
			case 294:
			case 295:
			case 296:
				o_ptr->art_flags5 |= TR5_DEVICE_MASTERY;
				return;
			case 297:
				o_ptr->art_flags5 |= TR5_SAVING_MALUS;
				return;
			case 298:
			case 299:
			case 300:
			case 301:
			case 302:
				o_ptr->art_flags5 |= TR5_DISARM;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 303:
			case 304:
			case 305:
			case 306:
			case 307:
				o_ptr->art_flags5 |= TR5_DODGE;
				if (randint(2) == 1) o_ptr->pval -= randint(5);
				else o_ptr->pval += randint(5);
				return;
			case 308:
			case 309:
			case 310:
			case 311:
			case 312:
				if ( (o_ptr->tval >= TV_BOOTS && o_ptr->tval <= TV_DRAG_ARMOR) ) {
					o_ptr->art_flags5 |= TR5_LITHE;
				}
				continue;
			case 313:
				o_ptr->art_flags5 |= TR5_RES_PLASMA;
				return;
			case 314:
				o_ptr->art_flags5 |= TR5_RES_DISINT;
				return;
			case 315:
				o_ptr->art_flags5 |= TR5_RES_TIME;
				return;
			case 316:
				o_ptr->art_flags5 |= TR5_RAPID_HUNGER;
				return;
			case 317:
			case 318:
			case 319:
				o_ptr->art_esp |= ESP_ORC;
				return;
			case 320:
			case 321:
			case 322:
				o_ptr->art_esp |= ESP_TROLL;
				return;
			case 323:
			case 324:
			case 325:
				o_ptr->art_esp |= ESP_DRAGON;
				return;
			case 326:
			case 327:
			case 328:
				o_ptr->art_esp |= ESP_GIANT;
				return;
			case 329:
			case 330:
				o_ptr->art_esp |= ESP_DEMON;
				return;
			case 331:
				o_ptr->art_esp |= ESP_UNDEAD;
				return;
			case 332:
			case 333:
				o_ptr->art_esp |= ESP_EVIL;
				return;
			case 334:
			case 335:
				o_ptr->art_esp |= ESP_ANIMAL;
				return;
			case 336:
				o_ptr->art_esp |= ESP_THUNDERLORD;
				return;
			case 337:
			case 338:
				o_ptr->art_esp |= ESP_GOOD;
				return;
			case 339:
				o_ptr->art_esp |= ESP_NONLIVING;
				return;
			case 340:
				o_ptr->art_esp |= ESP_UNIQUE;
				return;
			case 341:
				o_ptr->art_esp |= ESP_SPIDER;
				return;
			case 342:
				o_ptr->art_esp |= ESP_ALL;
				return;
			case 343:
				if (randint(2) == 1) o_ptr->to_h += randint(5);
				else o_ptr->to_h -= randint(5);
				return;
			case 344:
				if (randint(2) == 1) o_ptr->to_d += randint(5);
				else o_ptr->to_d -= randint(5);
				return;
			case 345:
				if (randint(2) == 1) o_ptr->to_a += randint(5);
				else o_ptr->to_a -= randint(5);
				return;

		}

	}

	return;

}

void give_activation_power (object_type * o_ptr)
{
	/* int type = 0, chance = 0; */


	/* A type was chosen... */
#if 0 /* DGDGDGDG -- Todo later */
	o_ptr->xtra2 = type;
	o_ptr->art_flags3 |= TR3_ACTIVATE;
	o_ptr->timeout = 0;
#else
	o_ptr->xtra2 = 0;
	o_ptr->art_flags3 &= ~TR3_ACTIVATE;
	o_ptr->timeout = 0;
#endif
}


int get_activation_power()
{
	object_type *o_ptr, forge;

	o_ptr = &forge;

	give_activation_power(o_ptr);

	return o_ptr->xtra2;
}

#define MIN_NAME_LEN 5
#define MAX_NAME_LEN 9
#define S_WORD 26
#define E_WORD S_WORD

static long lprobs[S_WORD + 1][S_WORD + 1][S_WORD + 1];
static long ltotal[S_WORD + 1][S_WORD + 1];

/*
 * Use W. Sheldon Simms' random name generator.  This function builds
 * probability tables which are used later on for letter selection.  It
 * relies on the ASCII character set.
 */
void build_prob(cptr learn)
{
	int c_prev, c_cur, c_next;

	/* Build raw frequencies */
	while (1)
	{
		c_prev = c_cur = S_WORD;

		do
		{
			c_next = *learn++;
		}
		while (!isalpha(c_next) && (c_next != '\0'));

		if (c_next == '\0') break;

		do
		{
			c_next = A2I(tolower(c_next));
			lprobs[c_prev][c_cur][c_next]++;
			ltotal[c_prev][c_cur]++;
			c_prev = c_cur;
			c_cur = c_next;
			c_next = *learn++;
		}
		while (isalpha(c_next));

		lprobs[c_prev][c_cur][E_WORD]++;
		ltotal[c_prev][c_cur]++;
	}
}


/*
 * Use W. Sheldon Simms' random name generator.  Generate a random word using
 * the probability tables we built earlier.  Relies on the ASCII character
 * set.  Relies on European vowels (a, e, i, o, u).  The generated name should
 * be copied/used before calling this function again.
 */
static char *make_word(void)
{
	static char word_buf[90];
	int r, totalfreq;
	int tries, lnum, vow;
	int c_prev, c_cur, c_next;
	char *cp;

startover:
	vow = 0;
	lnum = 0;
	tries = 0;
	cp = word_buf;
	c_prev = c_cur = S_WORD;

	while (1)
	{
getletter:
		c_next = 0;
		r = rand_int(ltotal[c_prev][c_cur]);
		totalfreq = lprobs[c_prev][c_cur][c_next];

		while (totalfreq <= r)
		{
			c_next++;
			totalfreq += lprobs[c_prev][c_cur][c_next];
		}

		if (c_next == E_WORD)
		{
			if ((lnum < MIN_NAME_LEN) || vow == 0)
			{
				tries++;
				if (tries < 10) goto getletter;
				goto startover;
			}
			*cp = '\0';
			break;
		}

		if (lnum >= MAX_NAME_LEN) goto startover;

		*cp = I2A(c_next);

		if (is_a_vowel(*cp)) vow++;

		cp++;
		lnum++;
		c_prev = c_cur;
		c_cur = c_next;
	}

	word_buf[0] = toupper(word_buf[0]);

	return (word_buf);
}


void get_random_name(char * return_name)
{
	char *word = make_word();

	if (rand_int(3) == 0)
		sprintf(return_name, "'%s'", word);
	else
		sprintf(return_name, "of %s", word);
}


bool create_artifact(object_type *o_ptr, bool a_scroll, bool get_name)
{
	char new_name[80];
	int powers = 0, i;
	s32b total_flags, total_power = 0;
	bool a_cursed = FALSE;
	u32b f1, f2, f3, f4, f5, esp;
	s16b *max_times;
	s16b pval = 0;
	bool limit_blows = FALSE;

	int xtrapowers = 0;

	strcpy(new_name, "");

	if ((!a_scroll) && (randint(A_CURSED) == 1)) a_cursed = TRUE;

	i = 0;
	while (ra_gen[i].chance)
	{
		powers += damroll(ra_gen[i].dd, ra_gen[i].ds) + ra_gen[i].plus;
		i++;
	}

	if ((!a_cursed) && (randint(30) == 1)) powers *= 2;

	/* cursed randarts should have more, not less powers, to make them worth using --Amy */
	if (a_cursed) powers *= 2;

	/* Amy edit: randarts should be a bit better! */
	powers += randint(4);

	/* more randomness by Amy */
	if (a_cursed) xtrapowers = randint(2);
	else xtrapowers = rand_int(2);

	if (randint(50) == 1) {
		xtrapowers += powers;
		powers = 0;
	}

	C_MAKE(max_times, max_ra_idx, s16b);

	/* Main loop */
	while (powers)
	{
		int ra_idx;
		randart_part_type *ra_ptr;

		powers--;

		if (!grab_one_power(&ra_idx, o_ptr, TRUE, max_times)) continue;

		ra_ptr = &ra_info[ra_idx];

		if (wizard) msg_format("Adding randart power: %d", ra_idx);

		total_power += ra_ptr->value;

		o_ptr->art_flags1 |= ra_ptr->flags1;
		o_ptr->art_flags2 |= ra_ptr->flags2;
		o_ptr->art_flags3 |= ra_ptr->flags3;
		o_ptr->art_flags4 |= ra_ptr->flags4;
		o_ptr->art_flags5 |= ra_ptr->flags5;
		o_ptr->art_esp |= ra_ptr->esp;

		add_random_ego_flag(o_ptr, ra_ptr->fego, &limit_blows);

		/* get flags */
		object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);

		/* Hack -- acquire "cursed" flag */
		if (f3 & TR3_CURSED) o_ptr->ident |= (IDENT_CURSED);

		/* Hack -- obtain bonuses */
		if (ra_ptr->max_to_h > 0) o_ptr->to_h += randint(ra_ptr->max_to_h);
		if (ra_ptr->max_to_h < 0) o_ptr->to_h -= randint( -ra_ptr->max_to_h);
		if (ra_ptr->max_to_d > 0) o_ptr->to_d += randint(ra_ptr->max_to_d);
		if (ra_ptr->max_to_d < 0) o_ptr->to_d -= randint( -ra_ptr->max_to_d);
		if (ra_ptr->max_to_a > 0) o_ptr->to_a += randint(ra_ptr->max_to_a);
		if (ra_ptr->max_to_a < 0) o_ptr->to_a -= randint( -ra_ptr->max_to_a);

		/* Hack -- obtain pval */
		if (((pval > ra_ptr->max_pval) && ra_ptr->max_pval) || (!pval)) pval = ra_ptr->max_pval;
	};
	C_FREE(max_times, max_ra_idx, s16b);

	if (pval > 0) o_ptr->pval = randint(pval);
	if (pval < 0) o_ptr->pval = randint( -pval);

	/* No insane number of blows; no longer needed --Amy */
	/*if (limit_blows && (o_ptr->art_flags1 & TR1_BLOWS))
	{
		if (o_ptr->pval > 2) o_ptr->pval = randint(2);
	}*/

	/* Just to be sure */
	o_ptr->art_flags3 |= ( TR3_IGNORE_ACID | TR3_IGNORE_ELEC |
	                       TR3_IGNORE_FIRE | TR3_IGNORE_COLD);

	total_flags = flag_cost(o_ptr, o_ptr->pval);
	if (cheat_peek) msg_format("%ld", total_flags);

	if (a_cursed) curse_artifact(o_ptr);

	while (xtrapowers > 0) {
		xtrapowers--;
		(void) add_xtra_arti_power(o_ptr);
	}

	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3, &f4, &f5, &esp);

	if (get_name)
	{
		if (a_scroll)
		{
			char dummy_name[80];

			/* Identify it fully */
			object_aware(o_ptr);
			object_known(o_ptr);
			o_ptr->ident |= (IDENT_STOREB | IDENT_MENTAL);

			strcpy(dummy_name, "");
			object_out_desc(o_ptr, NULL, FALSE, TRUE);

			if (get_string("What do you want to call the artifact? ", dummy_name, 80))
			{
				strcpy(new_name, "called '");
				strcat(new_name, dummy_name);
				strcat(new_name, "'");
			}
			else
				/* Default name = of 'player name' */
				sprintf(new_name, "of '%s'", player_name);
		}
		else
		{
			get_random_name(new_name);
		}
	}

	/* Save the inscription */
	o_ptr->art_name = quark_add(new_name);
	o_ptr->name2 = o_ptr->name2b = 0;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* HACKS for ToME */
	if (o_ptr->tval == TV_CLOAK && o_ptr->sval == SV_MIMIC_CLOAK)
	{
		s32b mimic;
		call_lua("find_random_mimic_shape", "(d,d)", "d", 127, TRUE, &mimic);
		o_ptr->pval2 = mimic;
	}
	else if (f5 & TR5_SPELL_CONTAIN)
	{
		o_ptr->pval2 = -1;
	}

	return TRUE;
}

bool artifact_scroll(void)
{
	int item;
	bool okay = FALSE;
	object_type *o_ptr;
	char o_name[80];

	cptr q, s;


	/* Enchant weapon/armour */
	item_tester_hook = item_tester_hook_artifactable;

	/* Get an item */
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &p_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Description */
	object_desc(o_name, o_ptr, FALSE, 0);

	/* Describe */
	msg_format("%s %s radiate%s a blinding light!",
	           ((item >= 0) ? "Your" : "The"), o_name,
	           ((o_ptr->number > 1) ? "" : "s"));

	if (artifact_p(o_ptr))
	{
		msg_format("The %s %s already %s!",
		           o_name, ((o_ptr->number > 1) ? "are" : "is"),
		           ((o_ptr->number > 1) ? "artifacts" : "an artifact"));
		okay = FALSE;
	}

	else if (o_ptr->name2)
	{
		msg_format("The %s %s already %s!",
		           o_name, ((o_ptr->number > 1) ? "are" : "is"),
		           ((o_ptr->number > 1) ? "ego items" : "an ego item"));
		okay = FALSE;
	}

	else
	{
		if (o_ptr->number > 1)
		{
			msg_print("Not enough enough energy to enchant more than one object!");
			msg_format("%d of your %s %s destroyed!", (o_ptr->number) - 1, o_name, (o_ptr->number > 2 ? "were" : "was"));
			o_ptr->number = 1;
		}
		okay = create_artifact(o_ptr, TRUE, TRUE);
	}

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
		msg_print("The enchantment failed.");
	}
	else
		o_ptr->found = OBJ_FOUND_SELFMADE;

	/* Something happened */
	return (TRUE);
}


