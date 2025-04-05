/* Dungeon randomizer for ToME-SX (an Angband variant). Written by AmyBSOD. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* generate random number from 1 to "maxrnd" (inclusive) */
int random_number(int maxrnd)
{
	int num = (rand() % maxrnd) + 1;
	return num;

}

/* select a random type of walkable terrain */
int randomfloortype(void)
{
	int randfloor = random_number(41);
	int floornumber = 1;

	/* small chance of a wall type */
	if (random_number(50) == 1) {
		floornumber = randomwalltype(1);
		return floornumber;
	}

	switch (randfloor) {
		case 1:
			floornumber = 1; /* open floor */
			break;
		case 2:
			switch (random_number(5)) {
				default:
				case 1:
					floornumber = 82; /* grass with niphredil flowers */
					break;
				case 2:
					floornumber = 79; /* grass with elanor flowers */
					break;
				case 3:
					floornumber = 80; /* grass with fumella flowers */
					break;
				case 4:
					floornumber = 81; /* grass with anemones */
					break;
				case 5:
					floornumber = 83; /* grass with irises */
					break;
			}
			break;
		case 3:
			floornumber = (random_number(5) == 1) ? 86 : 1; /* 1 in 5 to get a stream of shallow lava */
			break;
		case 4:
			floornumber = 88; /* dirt */
			break;
		case 5:
			floornumber = 89; /* patch of grass */
			break;
		case 6:
			floornumber = 90; /* ice */
			break;
		case 7:
			floornumber = 91; /* sand */
			break;
		case 8:
			floornumber = 93; /* ash */
			break;
		case 9:
			floornumber = 94; /* mud */
			break;
		case 10:
			floornumber = 102; /* nether mist */
			if (random_number(10) == 1) {
				int randdmg = random_number(12);
				switch (randdmg) {
					case 1:
					default:
						floornumber = 110; /* shock field */
						break;
					case 2:
						floornumber = 111; /* acid pool */
						break;
					case 3:
						floornumber = 112; /* plasma cloud */
						break;
					case 4:
						floornumber = 113; /* dark area */
						break;
					case 5:
						floornumber = 114; /* tornado */
						break;
					case 6:
						floornumber = 115; /* cartridge of having a horror */
						break;
					case 7:
						floornumber = 116; /* shifting sand */
						break;
					case 8:
						floornumber = 117; /* chaos tile */
						break;
					case 9:
						floornumber = 118; /* antimagic field */
						break;
					case 10:
						floornumber = 119; /* nexus field */
						break;
					case 11:
						floornumber = 120; /* black hole */
						break;
					case 12:
						floornumber = 121; /* nuclear waste */
						break;
				}
			}
			break;
		case 11:
			floornumber = 173; /* underground tunnel */
			break;
		case 12:
			floornumber = 174; /* stream of tainted water */
			break;
		case 13:
			floornumber = 187; /* pool of deep water */
			break;
		case 14:
			floornumber = 199; /* grass with flowers */
			break;
		case 15:
			floornumber = 200; /* cobblestone road */
			break;
		case 16:
			floornumber = 202; /* small tree */
			break;
		case 17:
			floornumber = 207; /* rocky ground */
			break;
		case 18:
			floornumber = 208; /* cloud-like vapour */
			break;
		case 19:
			floornumber = 209; /* condensing water */
			break;
		case 20:
			floornumber = 210; /* dense mist */
			break;
		case 21:
			floornumber = 222; /* water lily */
			break;
		case 22:
			floornumber = (random_number(5) == 1) ? 226 : 1; /* 1 in 5 for swamp pool */
			break;
		case 23:
		case 24:
			floornumber = 88; /* dirt */
			break;
		case 25:
			floornumber = 91; /* sand */
			break;
		case 26:
			floornumber = 93; /* ash */
			break;
		case 27:
		case 28:
			floornumber = 94; /* mud */
			break;
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
			floornumber = 1; /* open floor */
			break;

		default:
			floornumber = 1;
			break;
	}

	return floornumber;
}

/* select a random type of non-walkable or otherwise obstructing terrain */
int randomwalltype(int flags)
{
	int randfloor = random_number(40);
	int floornumber = 56;

	/* small chance of something walkable */
	if ((flags == 0) && (random_number(10) == 1) ) {
		floornumber = randomfloortype();
		return floornumber;
	}

	/* very small chance of a permanent wall */
	if ((flags == 0) && (random_number(50) == 1) ) {
		floornumber = randompermawalltype();
		return floornumber;
	}

	/* otherwise make a wall type */
	switch (randfloor) {
		case 1:
			floornumber = 16; /* web */
			break;
		case 2:
			floornumber = 50; /* magma vein */
			break;
		case 3:
			floornumber = 51; /* quartz vein */
			break;
		case 4:
			floornumber = 50; /* magma vein */
			break;
		case 5:
			floornumber = 51; /* quartz vein */
			break;
		case 6:
			floornumber = 56; /* granite wall */
			break;
		case 7:
			floornumber = 57; /* inner granite wall */
			break;
		case 8:
			floornumber = 58; /* outer granite wall */
			break;
		case 9:
			floornumber = 59; /* solid granite wall */
			break;
		case 10:
			floornumber = 87; /* dark pit */
			break;
		case 11:
			floornumber = 92; /* dead tree */
			break;
		case 12:
			floornumber = 95; /* ice wall */
			break;
		case 13:
			floornumber = 96; /* tree */
			break;
		case 14:
			floornumber = 97; /* mountain chain */
			break;
		case 15:
			floornumber = 98; /* sandwall */
			break;
		case 16:
			floornumber = 98; /* sandwall */
			break;
		case 17:
			floornumber = 189; /* illusion wall */
			break;
		case 18:
			floornumber = (random_number(5) == 1) ? 205 : 56; /* 1 in 5 for blazing fire */
			break;
		case 19:
			floornumber = 211; /* hailstone wall */
			break;
		case 20:
			floornumber = 215; /* glacial wall */
			break;
		case 21:
			floornumber = 216; /* battlement */
			break;
		case 22:
			floornumber = (random_number(5) == 1) ? 238 : 56; /* 1 in 5 for swift waterfall */
			break;
		case 23:
			floornumber = 84; /* stream of shallow water */
			break;
		case 24:
			floornumber = (random_number(5) == 1) ? 85 : 56; /* 1 in 5 for pool of deep lava */
			break;
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
			floornumber = 56; /* granite wall */
			break;
		case 31:
		case 32:
		case 33:
			floornumber = 50; /* magma vein */
			break;
		case 34:
		case 35:
			floornumber = 96; /* tree */
			break;
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
			floornumber = 56; /* granite wall */
			break;

		default:
			floornumber = 1;
			break;
	}

	return floornumber;
}

/* select a random type of permanent wall */
int randompermawalltype(void)
{
	int randfloor = random_number(14);
	int floornumber = 56;

	/* select a wall type that cannot be dug out or otherwise removed */
	switch (randfloor) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			floornumber = 60; /* permanent wall */
			break;
		case 11:
			floornumber = 177; /* lava wall */
			break;
		case 12:
			floornumber = 188; /* glass wall */
			break;
		case 13:
			floornumber = 206; /* permanent rubble */
			break;
		case 14:
			floornumber = 213; /* copper pillar */
			break;
		default:
			floornumber = 60;
			break;
	}

	return floornumber;
}

/* select a random dungeon fill method */
void randomfillmethod(void)
{
	switch (random_number(5)) {
		default:
		case 1:
			printf("F:FILL_METHOD_0\n");
			break;
		case 2:
			printf("F:FILL_METHOD_1\n");
			break;
		case 3:
			printf("F:FILL_METHOD_2\n");
			break;
		case 4:
			printf("F:FILL_METHOD_3\n");
			break;
		case 5:
			printf("F:FILL_METHOD_4\n");
			break;
	}

}

/* select a random background damage effect */
void randomdungeondamage(void)
{
	int dudrand;
	dudrand = random_number(26);

	switch (dudrand) {
		case 1:
		default:
			printf("E:1d%d:%d:NETHER\n", random_number(6), 9 + random_number(31) );
			break;
		case 2:
			printf("E:1d%d:%d:ELEC\n", random_number(6), 9 + random_number(31) );
			break;
		case 3:
			printf("E:1d%d:%d:POISON\n", random_number(6), 9 + random_number(31) );
			break;
		case 4:
			printf("E:1d%d:%d:ACID\n", random_number(6), 9 + random_number(31) );
			break;
		case 5:
			printf("E:1d%d:%d:COLD\n", random_number(6), 9 + random_number(31) );
			break;
		case 6:
			printf("E:1d%d:%d:FIRE\n", random_number(6), 9 + random_number(31) );
			break;
		case 7:
			printf("E:1d%d:%d:PLASMA\n", random_number(6), 9 + random_number(31) );
			break;
		case 8:
			printf("E:1d%d:%d:WATER\n", random_number(6), 9 + random_number(31) );
			break;
		case 9:
			printf("E:1d%d:%d:LITE\n", random_number(6), 9 + random_number(31) );
			break;
		case 10:
			printf("E:1d%d:%d:DARK\n", random_number(6), 9 + random_number(31) );
			break;
		case 11:
			printf("E:1d%d:%d:SHARDS\n", random_number(6), 9 + random_number(31) );
			break;
		case 12:
			printf("E:1d%d:%d:SOUND\n", random_number(6), 9 + random_number(31) );
			break;
		case 13:
			printf("E:1d%d:%d:CONFUSION\n", random_number(6), 9 + random_number(31) );
			break;
		case 14:
			printf("E:1d%d:%d:FORCE\n", random_number(6), 9 + random_number(31) );
			break;
		case 15:
			printf("E:1d%d:%d:INERTIA\n", random_number(6), 9 + random_number(31) );
			break;
		case 16:
			printf("E:1d%d:%d:MANA\n", random_number(6), 9 + random_number(31) );
			break;
		case 17:
			printf("E:1d%d:%d:METEOR\n", random_number(6), 9 + random_number(31) );
			break;
		case 18:
			printf("E:1d%d:%d:ICE\n", random_number(6), 9 + random_number(31) );
			break;
		case 19:
			printf("E:1d%d:%d:CHAOS\n", random_number(6), 9 + random_number(31) );
			break;
		case 20:
			printf("E:1d%d:%d:DISENCHANT\n", random_number(6), 9 + random_number(31) );
			break;
		case 21:
			printf("E:1d%d:%d:NEXUS\n", random_number(6), 9 + random_number(31) );
			break;
		case 22:
			printf("E:1d%d:%d:TIME\n", random_number(6), 9 + random_number(31) );
			break;
		case 23:
			printf("E:1d%d:%d:GRAVITY\n", random_number(6), 9 + random_number(31) );
			break;
		case 24:
			printf("E:1d%d:%d:NUKE\n", random_number(6), 9 + random_number(31) );
			break;
		case 25:
			printf("E:1d%d:%d:DISINTEGRATE\n", random_number(6), 9 + random_number(31) );
			break;
		case 26:
			printf("E:1d%d:%d:RAISE\n", random_number(6), 9 + random_number(31) );
			break;
	}

}

/* create a random monster mix */
void randommonstermix(void)
{
	int specmons = 1;
	if (random_number(2) == 1) {
		specmons++; /* 2 */
		if (random_number(2) == 1) {
			specmons++; /* 3 */
			if (random_number(2) == 1) {
				specmons++; /* 4 */
				if (random_number(2) == 1) {
					specmons++; /* 5 */
					if (random_number(2) == 1) {
		specmons += random_number(5); /* 6-10 */
					}
				}
			}
		}
	}

	int regularmon = 96 - random_number(95);
	if (regularmon > 95) regularmon = 95;
	if (regularmon < 1) regularmon = 1;
	int irregularmon = 100 - regularmon;
	printf("R:%d:0\n", regularmon);

	printf("R:%d:3\n", irregularmon);

	if (specmons < 1) specmons = 1;
	if (specmons > 10) specmons = 10;
	while (specmons > 0) {
		specmons--;
		switch (random_number(285)) {
			default:
			case 1:
				printf("M:UNIQUE\n");
				break;
			case 2:
				printf("M:MALE\n");
				break;
			case 3:
				printf("M:FEMALE\n");
				break;
			case 4:
				printf("M:CHAR_CLEAR\n");
				break;
			case 5:
				printf("M:CHAR_MULTI\n");
				break;
			case 6:
				printf("M:ATTR_CLEAR\n");
				break;
			case 7:
				printf("M:ATTR_MULTI\n");
				break;
			case 8:
				printf("M:FORCE_MAXHP\n");
				break;
			case 9:
				printf("M:FORCE_SLEEP\n");
				break;
			case 10:
				printf("M:FRIEND\n");
				break;
			case 11:
				printf("M:FRIENDS\n");
				break;
			case 12:
				printf("M:ESCORT\n");
				break;
			case 13:
				printf("M:ESCORTS\n");
				break;
			case 14:
				printf("M:NEVER_BLOW\n");
				break;
			case 15:
				printf("M:NEVER_MOVE\n");
				break;
			case 16:
				printf("M:RAND_25\n");
				break;
			case 17:
				printf("M:RAND_50\n");
				break;
			case 18:
				printf("M:ONLY_GOLD\n");
				break;
			case 19:
				printf("M:ONLY_ITEM\n");
				break;
			case 20:
				printf("M:DROP_60\n");
				break;
			case 21:
				printf("M:DROP_90\n");
				break;
			case 22:
				printf("M:DROP_12\n"); /* should be DROP_1D2 but the parser is VERY stupid --Amy */
				break;
			case 23:
				printf("M:DROP_22\n");
				break;
			case 24:
				printf("M:DROP_32\n");
				break;
			case 25:
				printf("M:DROP_42\n");
				break;
			case 26:
				printf("M:DROP_GOOD\n");
				break;
			case 27:
				printf("M:DROP_GREAT\n");
				break;
			case 28:
				printf("M:STUPID\n");
				break;
			case 29:
				printf("M:SMART\n");
				break;
			case 30:
				printf("M:CAN_SPEAK\n");
				break;
			case 31:
				printf("M:REFLECTING\n");
				break;
			case 32:
				printf("M:INVISIBLE\n");
				break;
			case 33:
				printf("M:COLD_BLOOD\n");
				break;
			case 34:
				printf("M:EMPTY_MIND\n");
				break;
			case 35:
				printf("M:WEIRD_MIND\n");
				break;
			case 36:
				printf("M:REGENERATE\n");
				break;
			case 37:
				printf("M:SHAPECHANGER\n");
				break;
			case 38:
				printf("M:ATTR_ANY\n");
				break;
			case 39:
				printf("M:POWERFUL\n");
				break;
			case 40:
				printf("M:ELDRITCH_HORROR\n");
				break;
			case 41:
				printf("M:AURA_FIRE\n");
				break;
			case 42:
				printf("M:AURA_ELEC\n");
				break;
			case 43:
				printf("M:OPEN_DOOR\n");
				break;
			case 44:
				printf("M:BASH_DOOR\n");
				break;
			case 45:
				printf("M:PASS_WALL\n");
				break;
			case 46:
				printf("M:KILL_WALL\n");
				break;
			case 47:
				printf("M:MOVE_BODY\n");
				break;
			case 48:
				printf("M:KILL_BODY\n");
				break;
			case 49:
				printf("M:TAKE_ITEM\n");
				break;
			case 50:
				printf("M:KILL_ITEM\n");
				break;
			case 51:
				printf("M:ORC\n");
				break;
			case 52:
				printf("M:TROLL\n");
				break;
			case 53:
				printf("M:GIANT\n");
				break;
			case 54:
				printf("M:DRAGON\n");
				break;
			case 55:
				printf("M:DEMON\n");
				break;
			case 56:
				printf("M:UNDEAD\n");
				break;
			case 57:
				printf("M:EVIL\n");
				break;
			case 58:
				printf("M:ANIMAL\n");
				break;
			case 59:
				printf("M:THUNDERLORD\n");
				break;
			case 60:
				printf("M:GOOD\n");
				break;
			case 61:
				printf("M:AURA_COLD\n");
				break;
			case 62:
				printf("M:NONLIVING\n");
				break;
			case 63:
				printf("M:HURT_LITE\n");
				break;
			case 64:
				printf("M:HURT_ROCK\n");
				break;
			case 65:
				printf("M:SUSCEP_FIRE\n");
				break;
			case 66:
				printf("M:SUSCEP_COLD\n");
				break;
			case 67:
				printf("M:IM_ACID\n");
				break;
			case 68:
				printf("M:IM_ELEC\n");
				break;
			case 69:
				printf("M:IM_FIRE\n");
				break;
			case 70:
				printf("M:IM_COLD\n");
				break;
			case 71:
				printf("M:IM_POIS\n");
				break;
			case 72:
				printf("M:RES_TELE\n");
				break;
			case 73:
				printf("M:RES_NETH\n");
				break;
			case 74:
				printf("M:RES_WATE\n");
				break;
			case 75:
				printf("M:RES_PLAS\n");
				break;
			case 76:
				printf("M:RES_NEXU\n");
				break;
			case 77:
				printf("M:RES_DISE\n");
				break;
			case 78:
				printf("M:NO_FEAR\n");
				break;
			case 79:
				printf("M:NO_STUN\n");
				break;
			case 80:
				printf("M:NO_CONF\n");
				break;
			case 81:
				printf("M:NO_SLEEP\n");
				break;
			case 82:
				printf("M:AQUATIC\n");
				break;
			case 83:
				printf("M:CAN_SWIM\n");
				break;
			case 84:
				printf("M:CAN_FLY\n");
				break;
			case 85:
				printf("M:FRIENDLY\n");
				break;
			case 86:
				printf("M:PET\n");
				break;
			case 87:
				printf("M:MORTAL\n");
				break;
			case 88:
				printf("M:SPIDER\n");
				break;
			case 89:
				printf("M:NAZGUL\n");
				break;
			case 90:
				printf("M:DG_CURSE\n");
				break;
			case 91:
				printf("M:POSSESSOR\n");
				break;
			case 92:
				printf("M:NO_TARGET\n");
				break;
			case 93:
				printf("M:AI_ANNOY\n");
				break;
			case 94:
				printf("M:NEUTRAL\n");
				break;
			case 95:
				printf("M:NO_THEFT\n");
				break;
			case 96:
				printf("M:SPIRIT\n");
				break;
			case 97:
				printf("M:REVBOSS\n");
				break;
			case 98:
				printf("M:AMBERITE\n");
				break;
			case 99:
				printf("M:RES_TIME\n");
				break;
			case 100:
				printf("M:RES_LITE\n");
				break;
			case 101:
				printf("M:RES_SOUN\n");
				break;
			case 102:
				printf("M:RES_CHAO\n");
				break;
			case 103:
				printf("M:RES_INER\n");
				break;
			case 104:
				printf("M:RES_WALL\n");
				break;
			case 105:
				printf("M:RES_GRAV\n");
				break;
			case 106:
				printf("M:RES_DARK\n");
				break;
			case 107:
				printf("M:RES_SHAR\n");
				break;
			case 108:
				printf("M:WILD_TOWN\n");
				break;
			case 109:
				printf("M:WILD_SHORE\n");
				break;
			case 110:
				printf("M:WILD_OCEAN\n");
				break;
			case 111:
				printf("M:WILD_WASTE\n");
				break;
			case 112:
				printf("M:WILD_WOOD\n");
				break;
			case 113:
				printf("M:WILD_VOLCANO\n");
				break;
			case 114:
				printf("M:WILD_MOUNTAIN\n");
				break;
			case 115:
				printf("M:WILD_GRASS\n");
				break;
			case 116:
				printf("M:NO_CUT\n");
				break;
			case 117:
				printf("M:JOKEANGBAND\n");
				break;
			case 118:
				printf("M:WILD_TOO\n");
				break;
			case 119:
				printf("M:DROP_CORPSE\n");
				break;
			case 120:
				printf("M:DROP_SKELETON\n");
				break;
			case 121:
				printf("M:HAS_LITE\n");
				break;
			case 122:
				printf("M:MIMIC\n");
				break;
			case 123:
				printf("M:HAS_EGG\n");
				break;
			case 124:
				printf("M:SUSCEP_ACID\n");
				break;
			case 125:
				printf("M:SUSCEP_ELEC\n");
				break;
			case 126:
				printf("M:SUSCEP_POIS\n");
				break;
			case 127:
				printf("M:KILL_TREES\n");
				break;
			case 128:
				printf("M:WYRM_PROTECT\n");
				break;
			case 129:
				printf("S:SHRIEK\n");
				break;
			case 130:
				printf("S:MULTIPLY\n");
				break;
			case 131:
				printf("S:S_ANIMAL\n");
				break;
			case 132:
				printf("S:ROCKET\n");
				break;
			case 133:
				printf("S:ARROW_1\n");
				break;
			case 134:
				printf("S:ARROW_2\n");
				break;
			case 135:
				printf("S:ARROW_3\n");
				break;
			case 136:
				printf("S:ARROW_4\n");
				break;
			case 137:
				printf("S:BR_ACID\n");
				break;
			case 138:
				printf("S:BR_ELEC\n");
				break;
			case 139:
				printf("S:BR_FIRE\n");
				break;
			case 140:
				printf("S:BR_COLD\n");
				break;
			case 141:
				printf("S:BR_POIS\n");
				break;
			case 142:
				printf("S:BR_NETH\n");
				break;
			case 143:
				printf("S:BR_LITE\n");
				break;
			case 144:
				printf("S:BR_DARK\n");
				break;
			case 145:
				printf("S:BR_CONF\n");
				break;
			case 146:
				printf("S:BR_SOUN\n");
				break;
			case 147:
				printf("S:BR_CHAO\n");
				break;
			case 148:
				printf("S:BR_DISE\n");
				break;
			case 149:
				printf("S:BR_NEXU\n");
				break;
			case 150:
				printf("S:BR_TIME\n");
				break;
			case 151:
				printf("S:BR_INER\n");
				break;
			case 152:
				printf("S:BR_GRAV\n");
				break;
			case 153:
				printf("S:BR_SHAR\n");
				break;
			case 154:
				printf("S:BR_PLAS\n");
				break;
			case 155:
				printf("S:BR_WALL\n");
				break;
			case 156:
				printf("S:BR_MANA\n");
				break;
			case 157:
				printf("S:BA_NUKE\n");
				break;
			case 158:
				printf("S:BR_NUKE\n");
				break;
			case 159:
				printf("S:BA_CHAO\n");
				break;
			case 160:
				printf("S:BR_DISI\n");
				break;
			case 161:
				printf("S:BA_ACID\n");
				break;
			case 162:
				printf("S:BA_ELEC\n");
				break;
			case 163:
				printf("S:BA_FIRE\n");
				break;
			case 164:
				printf("S:BA_COLD\n");
				break;
			case 165:
				printf("S:BA_POIS\n");
				break;
			case 166:
				printf("S:BA_NETH\n");
				break;
			case 167:
				printf("S:BA_WATE\n");
				break;
			case 168:
				printf("S:BA_MANA\n");
				break;
			case 169:
				printf("S:BA_DARK\n");
				break;
			case 170:
				printf("S:DRAIN_MANA\n");
				break;
			case 171:
				printf("S:MIND_BLAST\n");
				break;
			case 172:
				printf("S:BRAIN_SMASH\n");
				break;
			case 173:
				printf("S:CAUSE_1\n");
				break;
			case 174:
				printf("S:CAUSE_2\n");
				break;
			case 175:
				printf("S:CAUSE_3\n");
				break;
			case 176:
				printf("S:CAUSE_4\n");
				break;
			case 177:
				printf("S:BO_ACID\n");
				break;
			case 178:
				printf("S:BO_ELEC\n");
				break;
			case 179:
				printf("S:BO_FIRE\n");
				break;
			case 180:
				printf("S:BO_COLD\n");
				break;
			case 181:
				printf("S:BO_POIS\n");
				break;
			case 182:
				printf("S:BO_NETH\n");
				break;
			case 183:
				printf("S:BO_WATE\n");
				break;
			case 184:
				printf("S:BO_MANA\n");
				break;
			case 185:
				printf("S:BO_PLAS\n");
				break;
			case 186:
				printf("S:BO_ICEE\n");
				break;
			case 187:
				printf("S:MISSILE\n");
				break;
			case 188:
				printf("S:SCARE\n");
				break;
			case 189:
				printf("S:BLIND\n");
				break;
			case 190:
				printf("S:CONF\n");
				break;
			case 191:
				printf("S:SLOW\n");
				break;
			case 192:
				printf("S:HOLD\n");
				break;
			case 193:
				printf("S:HASTE\n");
				break;
			case 194:
				printf("S:HAND_DOOM\n");
				break;
			case 195:
				printf("S:HEAL\n");
				break;
			case 196:
				printf("S:S_ANIMALS\n");
				break;
			case 197:
				printf("S:BLINK\n");
				break;
			case 198:
				printf("S:TPORT\n");
				break;
			case 199:
				printf("S:TELE_TO\n");
				break;
			case 200:
				printf("S:TELE_AWAY\n");
				break;
			case 201:
				printf("S:TELE_LEVEL\n");
				break;
			case 202:
				printf("S:DARKNESS\n");
				break;
			case 203:
				printf("S:TRAPS\n");
				break;
			case 204:
				printf("S:FORGET\n");
				break;
			case 205:
				printf("S:ANIM_DEAD\n");
				break;
			case 206:
				printf("S:S_BUG\n");
				break;
			case 207:
				printf("S:S_RNG\n");
				break;
			case 208:
				printf("S:S_THUNDERLORD\n");
				break;
			case 209:
				printf("S:S_KIN\n");
				break;
			case 210:
				printf("S:S_HI_DEMON\n");
				break;
			case 211:
				printf("S:S_MONSTER\n");
				break;
			case 212:
				printf("S:S_MONSTERS\n");
				break;
			case 213:
				printf("S:S_ANT\n");
				break;
			case 214:
				printf("S:S_SPIDER\n");
				break;
			case 215:
				printf("S:S_HOUND\n");
				break;
			case 216:
				printf("S:S_HYDRA\n");
				break;
			case 217:
				printf("S:S_ANGEL\n");
				break;
			case 218:
				printf("S:S_DEMON\n");
				break;
			case 219:
				printf("S:S_UNDEAD\n");
				break;
			case 220:
				printf("S:S_DRAGON\n");
				break;
			case 221:
				printf("S:S_HI_UNDEAD\n");
				break;
			case 222:
				printf("S:S_HI_DRAGON\n");
				break;
			case 223:
				printf("S:S_WRAITH\n");
				break;
			case 224:
				printf("S:S_UNIQUE\n");
				break;
			case 225:
				printf("M:R_CHAR_a\n");
				break;
			case 226:
				printf("M:R_CHAR_b\n");
				break;
			case 227:
				printf("M:R_CHAR_c\n");
				break;
			case 228:
				printf("M:R_CHAR_d\n");
				break;
			case 229:
				printf("M:R_CHAR_e\n");
				break;
			case 230:
				printf("M:R_CHAR_f\n");
				break;
			case 231:
				printf("M:R_CHAR_g\n");
				break;
			case 232:
				printf("M:R_CHAR_h\n");
				break;
			case 233:
				printf("M:R_CHAR_i\n");
				break;
			case 234:
				printf("M:R_CHAR_j\n");
				break;
			case 235:
				printf("M:R_CHAR_k\n");
				break;
			case 236:
				printf("M:R_CHAR_l\n");
				break;
			case 237:
				printf("M:R_CHAR_m\n");
				break;
			case 238:
				printf("M:R_CHAR_n\n");
				break;
			case 239:
				printf("M:R_CHAR_o\n");
				break;
			case 240:
				printf("M:R_CHAR_p\n");
				break;
			case 241:
				printf("M:R_CHAR_q\n");
				break;
			case 242:
				printf("M:R_CHAR_r\n");
				break;
			case 243:
				printf("M:R_CHAR_s\n");
				break;
			case 244:
				printf("M:R_CHAR_t\n");
				break;
			case 245:
				printf("M:R_CHAR_u\n");
				break;
			case 246:
				printf("M:R_CHAR_v\n");
				break;
			case 247:
				printf("M:R_CHAR_w\n");
				break;
			case 248:
				printf("M:R_CHAR_x\n");
				break;
			case 249:
				printf("M:R_CHAR_y\n");
				break;
			case 250:
				printf("M:R_CHAR_z\n");
				break;
			case 251:
				printf("M:R_CHAR_A\n");
				break;
			case 252:
				printf("M:R_CHAR_B\n");
				break;
			case 253:
				printf("M:R_CHAR_C\n");
				break;
			case 254:
				printf("M:R_CHAR_D\n");
				break;
			case 255:
				printf("M:R_CHAR_E\n");
				break;
			case 256:
				printf("M:R_CHAR_F\n");
				break;
			case 257:
				printf("M:R_CHAR_G\n");
				break;
			case 258:
				printf("M:R_CHAR_H\n");
				break;
			case 259:
				printf("M:R_CHAR_I\n");
				break;
			case 260:
				printf("M:R_CHAR_J\n");
				break;
			case 261:
				printf("M:R_CHAR_K\n");
				break;
			case 262:
				printf("M:R_CHAR_L\n");
				break;
			case 263:
				printf("M:R_CHAR_M\n");
				break;
			case 264:
				printf("M:R_CHAR_N\n");
				break;
			case 265:
				printf("M:R_CHAR_O\n");
				break;
			case 266:
				printf("M:R_CHAR_P\n");
				break;
			case 267:
				printf("M:R_CHAR_Q\n");
				break;
			case 268:
				printf("M:R_CHAR_R\n");
				break;
			case 269:
				printf("M:R_CHAR_S\n");
				break;
			case 270:
				printf("M:R_CHAR_T\n");
				break;
			case 271:
				printf("M:R_CHAR_U\n");
				break;
			case 272:
				printf("M:R_CHAR_V\n");
				break;
			case 273:
				printf("M:R_CHAR_W\n");
				break;
			case 274:
				printf("M:R_CHAR_X\n");
				break;
			case 275:
				printf("M:R_CHAR_Y\n");
				break;
			case 276:
				printf("M:R_CHAR_Z\n");
				break;
			case 277:
				printf("M:R_CHAR_,\n");
				break;
			case 278:
				printf("M:R_CHAR_$\n");
				break;
			case 279:
				printf("M:R_XTRACHAR_PIPE_124\n");
				break;
			case 280:
				printf("M:R_CHAR_~\n");
				break;
			case 281:
				printf("M:R_CHAR_*\n");
				break;
			case 282:
				printf("M:R_CHAR_.\n");
				break;
			case 283:
				printf("M:R_CHAR_#\n");
				break;
			case 284:
				printf("M:R_CHAR_;\n");
				break;
			case 285:
				printf("M:R_CHAR_:\n");
				break;
		}
	}

}

/* Generates and prints 'count' random numbers in range [lower, upper]. */
void printRandoms(int lower, int upper, int count)
{
	int num;
	int d_id;
	int contnum;

	num = random_number(59);

	printf("V:2.0.0\n", num);
	printf("\n", num);

	for (d_id = 0; d_id <= 69; d_id++) {

		switch (d_id) {
			case 0:
				printf("N:0:Wilderness\n");
				printf("D:Wil:a way to the Wilderness\n");
				printf("W:0:0:0:0:14:500\n");
				printf("L:89:80:199:20:1:0\n");
				printf("A:96:100:56:0:56:0:57:58\n");
				printf("O:20:20:20:20\n");
				printf("F:PRINCIPAL | FLAT | NO_RECALL\n");
				printf("R:100:0\n");
				break;
			case 1:
				printf("N:1:Mirkwood\n");
				printf("D:Mkw:a way to the Mirkwood Forest.\n");
				printf("W:11:33:1:0:14:160\n");
				break;
			case 2:
				printf("N:2:Mordor\n");
				printf("D:Mdr:a door to the Land of Mordor.\n");
				printf("W:34:66:1:0:14:160\n");
				break;
			case 3:
				printf("N:3:Angband\n");
				printf("D:Ang:an entrance to the Pits of Angband.\n");
				printf("W:67:127:1:0:14:160\n");
				break;
			case 4:
				printf("N:4:Barrow-Downs\n");
				printf("D:BDw:a way to the Barrow-Downs.\n");
				printf("W:1:10:1:0:14:160\n");
				break;
			case 5:
				printf("N:5:Mount Doom\n");
				printf("D:MDm:a way to the top of the Mount Doom.\n");
				printf("W:85:99:1:0:14:160\n");
				break;
			case 6:
				printf("N:6:Nether Realm\n");
				printf("D:Nth:a magical portal to the Nether Realm.\n");
				printf("W:666:696:1:0:14:160\n");
				break;
			case 7:
				printf("N:7:Submerged Ruins\n");
				printf("D:Num:a submerged way to the lost land of Numenor.\n");
				printf("W:35:50:1:0:14:160\n");
				break;
			case 8:
				printf("N:8:Halls of Mandos\n");
				printf("D:HMa:the entrance to the Halls of Mandos.\n");
				printf("W:1:98:1:0:14:160\n");
				break;
			case 9:
				printf("N:9:Cirith Ungol\n");
				printf("D:CUg:an entrance to Cirith Ungol.\n");
				printf("W:25:50:1:0:14:160\n");
				break;
			case 10:
				printf("N:10:Heart of the Earth\n");
				printf("D:HoE:a passage leading into the very heart of the world.\n");
				printf("W:25:36:1:0:14:160\n");
				break;
			case 11:
				printf("N:11:The Void\n");
				printf("D:Vod:a jumpgate to the Void.\n");
				printf("W:128:150:1:0:20:160\n");
				break;
			case 12:
				printf("N:12:Test\n");
				printf("D:Tst:a way to test dungeon gen\n");
				printf("W:1:10:1:0:14:160\n");
				printf("L:88:78:89:18:199:4\n");
				printf("L:0:95:5\n");
				printf("A:177:100:0:0:0:0:85:87\n");
				printf("A:100:0:0\n");
				printf("O:20:20:20:20\n");
				printf("F:FILL_METHOD_3 | SMALL\n");
				printf("R:100:0\n");
				printf("G:dungeon2\n");
				break;
			case 16:
				printf("N:16:Paths of the Dead\n");
				printf("D:PoD:the entrance to the Paths of the Dead.\n");
				printf("W:40:70:1:0:24:100\n");
				break;
			case 17:
				printf("N:17:Illusory Castle\n");
				printf("D:Ill:an entrance to the Illusory Castle.\n");
				printf("W:35:52:1:0:24:100\n");
				break;
			case 18:
				printf("N:18:Maze\n");
				printf("D:Maz:a small tunnel leading to a maze of twisty little passages, all alike.\n");
				printf("W:25:37:1:0:20:160\n");
				break;
			case 19:
				printf("N:19:Orc Cave\n");
				printf("D:Orc:a dark tunnel leading to an Orc Cave.\n");
				printf("W:10:22:1:0:35:200\n");
				break;
			case 20:
				printf("N:20:Erebor\n");
				printf("D:Ere:a tunnel leading into depths of the Lonely Mountain.\n");
				printf("W:60:72:1:0:20:140\n");
				break;
			case 21:
				printf("N:21:The Old Forest\n");
				printf("D:OFr:a path into the Old Forest.\n");
				printf("W:13:25:1:0:15:100\n");
				break;
			case 22:
				printf("N:22:Moria\n");
				printf("D:MoM:a stone door leading to the Mines of Moria.\n");
				printf("W:30:50:1:0:40:40\n");
				break;
			case 23:
				printf("N:23:Dol Guldur\n");
				printf("D:TDG:a gate leading to the tower of Dol Guldur.\n");
				printf("W:57:70:1:0:24:160\n");
				break;
			case 24:
				printf("N:24:The Small Water Cave\n");
				printf("D:SWC:the entrance to a small water cave.\n");
				printf("W:32:34:1:0:14:160\n");
				break;
			case 25:
				printf("N:25:The Sacred Land Of Mountains\n");
				printf("D:LoM:the way to the Sacred Land of Mountains.\n");
				printf("W:45:70:1:0:14:160\n");
				break;
			case 26:
				printf("N:26:The Land Of Rhun\n");
				printf("D:LoR:a way to the Land of Rhun.\n");
				printf("W:26:40:1:0:14:160\n");
				break;
			case 27:
				printf("N:27:The Sandworm lair\n");
				printf("D:SwL:a sandhole.\n");
				printf("W:22:30:1:0:5:200\n");
				break;
			case 28:
				printf("N:28:Death fate\n");
				printf("D:Dth:a fated death.\n");
				printf("W:1:1:1:0:30:255\n");
				printf("L:1:100:1:0:1:0\n");
				printf("A:1:100:1:0:1:0:1:1\n");
				printf("O:1:1:1:1\n");
				printf("F:EMPTY | SMALLEST | NO_RECALL | NO_STREAMERS\n");
				printf("F:FILL_METHOD_0\n");
				printf("R:100:0\n");
				break;
			case 29:
				printf("N:29:The Helcaraxe\n");
				printf("D:Ice:the entrance to the Grinding Ice of the Helcaraxe.\n");
				printf("W:20:40:1:0:14:160\n");
				break;
			case 30:
				printf("N:30:A lost temple\n");
				printf("D:LTm:the entrance to a lost temple.\n");
				printf("W:1:50:1:0:14:160\n");
				printf("L:1:100:1:0:1:0\n");
				printf("A:56:100:56:0:56:0:57:58\n");
				printf("O:20:20:20:20\n");
				printf("F:FILL_METHOD_4 | NO_RECALL\n");
				printf("R:100:0\n");
				break;
			case 31:
				printf("N:31:Forodwaith\n");
				printf("D:NWa:a path leading through the wastelands of the North.\n");
				printf("W:75:90:1:0:14:160\n");
				break;
			case 32:
				printf("N:32:Emyn Luin\n");
				printf("D:ELu:a path into the depths of the Blue Mountains.\n");
				printf("W:60:80:1:0:14:160\n");
				break;
			case 33:
				printf("N:33:Dol Amroth\n");
				printf("D:DAm:a way to the top of the castle of Dol Amroth.\n");
				printf("W:25:35:1:0:14:160\n");
				break;
			case 34:
				printf("N:34:Angmar\n");
				printf("D:WRA:a dark path through the Witch Realm of Angmar.\n");
				printf("W:80:98:1:0:14:160\n");
				break;
			case 35:
				printf("N:35:Near Harad\n");
				printf("D:NHa:a desert path into Near Harad.\n");
				printf("W:20:45:1:0:14:160\n");
				break;
			case 36:
				printf("N:36:Isengard\n");
				printf("D:Isg:a passage to the caves beneath Isengard.\n");
				printf("W:35:60:1:0:14:160\n");
				break;
			case 37:
				printf("N:37:Tol Eressea\n");
				printf("D:TEr:a way to the Lonely Isle.\n");
				printf("W:40:65:1:0:14:160\n");
				break;
			case 38:
				printf("N:38:Utumno\n");
				printf("D:Utu:an entrance to the depths of Utumno.\n");
				printf("W:101:127:1:0:14:160\n");
				break;
			case 39:
				printf("N:39:Deep Submerged Ruins\n");
				printf("D:DNu:a submerged way to the deep submerged ruins.\n");
				printf("W:840:940:1:0:14:160\n");
				break;
			case 40:
				printf("N:40:Deep Cirith Ungol\n");
				printf("D:DCU:an entrance to Deep Cirith Ungol.\n");
				printf("W:128:228:1:0:14:160\n");
				break;
			case 41:
				printf("N:41:Paths of the Undead\n");
				printf("D:PUD:the entrance to the Paths of the Undead.\n");
				printf("W:880:980:1:0:24:100\n");
				break;
			case 42:
				printf("N:42:Fucktard Castle\n");
				printf("D:Ftd:an entrance to Fucktard Castle.\n");
				printf("W:400:500:1:0:24:100\n");
				break;
			case 43:
				printf("N:43:Deep Erebor\n");
				printf("D:DEr:a tunnel leading into depths of the Dragon Mountain.\n");
				printf("W:780:880:1:0:20:140\n");
				break;
			case 44:
				printf("N:44:The Young Forest\n");
				printf("D:YFr:a path into the Young Forest.\n");
				printf("W:188:288:1:0:15:100\n");
				break;
			case 45:
				printf("N:45:Deep Moria\n");
				printf("D:DoM:a stone door leading to the Depths of Moria.\n");
				printf("W:248:348:1:0:40:40\n");
				break;
			case 46:
				printf("N:46:The Unholy Land Of Mountains\n");
				printf("D:UoM:the way to the Unholy Land of Mountains.\n");
				printf("W:740:840:1:0:14:160\n");
				break;
			case 47:
				printf("N:47:The Depths Of Rhun\n");
				printf("D:DoR:a way to the Depths of Rhun.\n");
				printf("W:308:408:1:0:14:160\n");
				break;
			case 48:
				printf("N:48:The Maggot lair\n");
				printf("D:MgL:a maggot hole.\n");
				printf("W:368:468:1:0:5:200\n");
				break;
			case 49:
				printf("N:49:Winterhold\n");
				printf("D:Who:the entrance to the Grinding Ice of the Winterhold.\n");
				printf("W:680:780:1:0:14:160\n");
				break;
			case 50:
				printf("N:50:Thunder Mountain\n");
				printf("D:Thu:a path into the depths of the Thunder Mountains\n");
				printf("W:550:650:1:0:14:160\n");
				break;
			case 51:
				printf("N:51:Crashing Angmar\n");
				printf("D:Cra:a dark path through the Crash King Realm of Angmar\n");
				printf("W:620:720:1:0:14:160\n");
				break;
			case 52:
				printf("N:52:Uber Isengard\n");
				printf("D:UIs:a passage to Uber Isengard\n");
				printf("W:428:528:1:0:14:160\n");
				break;
			case 53:
				printf("N:53:The Open Sea\n");
				printf("D:OSe:a way to the Open Sea\n");
				printf("W:488:588:1:0:14:160\n");
				break;
			case 54:
				printf("N:54:Barad-Dur\n");
				printf("D:BDu:the entrance to the abandoned fortress Barad-Dur.\n");
				printf("W:1:20:1:0:14:160\n");
				break;
			case 55:
				printf("N:55:Amon Sul\n");
				printf("D:ASu:the entrance to the tower of Amon Sul.\n");
				printf("W:20:40:1:0:14:160\n");
				break;
			case 56:
				printf("N:56:Hornburg\n");
				printf("D:Hor:the entrance to Hornburg.\n");
				printf("W:40:60:1:0:50:160\n");
				break;
			case 57:
				printf("N:57:Fornost Erain\n");
				printf("D:For:the entrance to Fornost Erain.\n");
				printf("W:50:70:1:0:50:160\n");
				break;
			case 58:
				printf("N:58:Caras Galadhon\n");
				printf("D:Car:the entrance to the remains of Caras Galadhon.\n");
				printf("W:71:98:1:0:50:160\n");
				break;
			case 59:
				printf("N:59:R'lyeh\n");
				printf("D:Rly:the entrance to the town of horrors R'lyeh.\n");
				printf("W:55:80:1:0:30:200\n");
				break;
			case 60:
				printf("N:60:Middle Harad\n");
				printf("D:DHa:a desert path into Middle Harad.\n");
				printf("W:60:98:1:0:60:180\n");
				break;
			case 61:
				printf("N:61:Far Harad\n");
				printf("D:FHa:a desert path into Far Harad.\n");
				printf("W:40:60:1:0:20:100\n");
				break;
			case 62:
				printf("N:62:Graveyard\n");
				printf("D:Gra:a hole to the Graveyard underground.\n");
				printf("W:50:70:1:0:14:160\n");
				break;
			case 63:
				printf("N:63:Volcano\n");
				printf("D:Vol:a hole to the center of the Volcano.\n");
				printf("W:50:60:1:0:14:160\n");
				break;
			case 64:
				printf("N:64:Hell\n");
				printf("D:Hel:the stairway to the Hell.\n");
				printf("W:66:80:1:0:14:160\n");
				break;
			case 65:
				printf("N:65:Heaven\n");
				printf("D:Hea:the way to the heaven.\n");
				printf("W:55:70:1:0:18:160\n");
				break;
			case 66:
				printf("N:66:Mountain\n");
				printf("D:Mou:a way leading to the mountain.\n");
				printf("W:40:50:1:0:14:160\n");
				break;
			case 67:
				printf("N:67:Glass castle\n");
				printf("D:Gla:the entrance to the glass castle.\n");
				printf("W:40:60:1:0:14:160\n");
				break;
			case 68:
				printf("N:68:Icky cave\n");
				printf("D:Ick:the entrance to the Icky caves.\n");
				printf("W:20:35:1:0:14:240\n");
				break;
			case 69:
				printf("N:69:Mount Olympus\n");
				printf("D:Oly:the entrance to Mount Olympus.\n");
				printf("W:80:90:1:0:14:160\n");
				break;
		}

		switch (d_id) {
			case 1: /* Mirkwood */
				printf("F:PRINCIPAL\n");
				break;
			case 2: /* Mordor */
				printf("F:PRINCIPAL\n");
				break;
			case 3: /* Angband */
				printf("F:PRINCIPAL | NO_RECALL | NO_EASY_MOVE\n");
				break;
			case 4: /* Barrow-Downs */
				printf("F:PRINCIPAL\n");
				break;
			case 5: /* Mount Doom */
				printf("F:NO_RECALL | NO_EASY_MOVE\n");
				break;
			case 6: /* Nether Realm */
				printf("F:FINAL_GUARDIAN_1032 | FINAL_ARTIFACT_203 | NO_RECALL_OUT | NO_RECALL\n");
				printf("F:NO_EASY_MOVE\n");
				break;
			case 7: /* Numenor */
				printf("F:FINAL_GUARDIAN_980 | FINAL_ARTIFACT_204\n");
				break;
			case 8: /* Halls of Mandos */
				printf("F:NO_RECALL\n");
				break;
			case 9: /* Cirith Ungol */
				printf("F:FINAL_GUARDIAN_481\n");
				break;
			case 10: /* Heart of the Earth */
				printf("F:FINAL_GUARDIAN_1035 | NO_RECALL\n");
				break;
			case 11: /* Void */
				printf("F:NO_RECALL_OUT | NO_RECALL | NO_EASY_MOVE | FINAL_GUARDIAN_1044\n");
				break;
			case 16: /* Paths of the Dead */
				printf("F:FINAL_GUARDIAN_804 | FINAL_ARTIFACT_91\n");
				break;
			case 17: /* Illusory Castle */
				printf("F:FINAL_GUARDIAN_1033 | FINAL_ARTIFACT_160\n");
				break;
			case 18: /* Maze */
				printf("F:FINAL_GUARDIAN_1029 | FINAL_ARTIFACT_38\n");
				break;
			case 19: /* Orc Cave */
				printf("F:FINAL_OBJECT_810 | FINAL_GUARDIAN_373\n");
				break;
			case 20: /* Erebor */
				printf("F:FINAL_GUARDIAN_715 | NO_RECALL\n");
				break;
			case 21: /* Old Forest */
				printf("F:FINAL_GUARDIAN_206\n");
				break;
			case 22: /* Moria */
				printf("F:FINAL_GUARDIAN_872 | FORCE_DOWN\n");
				break;
			case 23: /* Dol Guldur */
				printf("F:FINAL_GUARDIAN_819 | FINAL_ARTIFACT_205\n");
				break;
			case 24: /* Small Water Cave */
				printf("F:FINAL_GUARDIAN_517 | NO_RECALL\n");
				break;
			case 25: /* Sacred Land of Mountains */
				printf("F:FINAL_GUARDIAN_789 | FINAL_ARTIFACT_27\n");
				break;
			case 26: /* Land of Rhun */
				printf("F:FINAL_GUARDIAN_990\n");
				break;
			case 27: /* Sandworm Lair */
				printf("F:FINAL_GUARDIAN_1030 | FINAL_ARTIFACT_153\n");
				break;
			case 29: /* Helcaraxe */
				printf("F:FINAL_GUARDIAN_1034\n");
				break;
			case 31: /* Forodwaith */
				printf("F:NO_RECALL | FINAL_GUARDIAN_3908\n");
				break;
			case 32: /* Emyn Luin */
				printf("F:NO_RECALL | FINAL_GUARDIAN_3909\n");
				break;
			case 33: /* Dol Amroth */
				printf("F:NO_EASY_MOVE | FINAL_GUARDIAN_3910\n");
				break;
			case 34: /* Angmar */
				printf("F:FINAL_GUARDIAN_3911\n");
				break;
			case 35: /* Near Harad */
				printf("F:FINAL_GUARDIAN_3912\n");
				break;
			case 36: /* Isengard */
				printf("F:FINAL_GUARDIAN_3913\n");
				break;
			case 37: /* Tol Eressea */
				printf("F:FINAL_GUARDIAN_3914\n");
				break;
			case 38: /* Utumno */
				printf("F:NO_RECALL | NO_EASY_MOVE | FINAL_GUARDIAN_3915\n");
				break;
			case 54: /* Barad-Dur */
				printf("F:FINAL_GUARDIAN_3916\n");
				break;
			case 55: /* Amon Sul */
				printf("F:FINAL_GUARDIAN_3917\n");
				break;
			case 56: /* Hornburg */
				printf("F:FINAL_GUARDIAN_3918\n");
				break;
			case 57: /* Fornost Erain */
				printf("F:FINAL_GUARDIAN_3919\n");
				break;
			case 58: /* Caras Galadhon */
				printf("F:NO_EASY_MOVE | FINAL_GUARDIAN_3920\n");
				break;
			case 59: /* R'lyeh */
				printf("F:FINAL_GUARDIAN_3922\n");
				break;
			case 60: /* Middle Harad */
				printf("F:FINAL_GUARDIAN_3923\n");
				break;
			case 61: /* Far Harad */
				printf("F:FINAL_GUARDIAN_4236\n");
				break;
			case 62: /* Graveyard */
				printf("F:FINAL_GUARDIAN_4237\n");
				break;
			case 63: /* Volcano */
				printf("F:FINAL_GUARDIAN_4238\n");
				break;
			case 64: /* Hell */
				printf("F:FINAL_GUARDIAN_4239\n");
				break;
			case 65: /* Heaven */
				printf("F:FINAL_GUARDIAN_4240\n");
				break;
			case 66: /* Mountain */
				printf("F:FINAL_GUARDIAN_4241\n");
				break;
			case 67: /* Glass Castle */
				printf("F:FINAL_GUARDIAN_4242\n");
				break;
			case 68: /* Icky Cave */
				printf("F:FINAL_GUARDIAN_4243\n");
				break;
			case 69: /* Mount Olympus */
				printf("F:FINAL_GUARDIAN_4244\n");
				break;
		}

		/* randomized content for those dungeons that should have randomized content --Amy */
		if ( (d_id >= 1 && d_id <= 11) || (d_id >= 16 && d_id <= 27) || (d_id == 29) || (d_id >= 31 && d_id <= 69) ) {

			if (random_number(3) == 2) {

				int numtries;

				printf("# very random\n", contnum);
				int l_first = 0;
				int l_second = 0;
				int l_third = 0;
				int a_first = 0;
				int a_second = 0;
				int a_third = 0;

				l_first = random_number(100);
				numtries = 100 - l_first;
				while (numtries > 0) {
					numtries--;
					if (l_second == 0) l_second++;
					else {
						if (random_number(3) == 2) l_third++;
						else l_second++;
					}
				}

				a_first = random_number(100);
				numtries = 100 - a_first;
				while (numtries > 0) {
					numtries--;
					if (a_second == 0) a_second++;
					else {
						if (random_number(3) == 2) a_third++;
						else a_second++;
					}
				}

				int l_firsttype = randomfloortype();
				int l_secondtype = randomfloortype();
				int l_thirdtype = randomfloortype();

				int a_firsttype = randomwalltype(0);
				int a_secondtype = randomwalltype(0);
				int a_thirdtype = randomwalltype(0);
				int a_outertype = randomwalltype(0);
				int a_innertype = randomwalltype(0);

				int o_first = 0;
				int o_second = 0;
				int o_third = 0;
				int o_fourth = 0;

				numtries = 100;
				while (numtries > 0) {
					int dudelrand;
					numtries--;

					dudelrand = random_number(5);
					switch (dudelrand) {
						default: break;
						case 1:
							o_first++;
							break;
						case 2:
							o_second++;
							break;
						case 3:
							o_third++;
							break;
						case 4:
							o_fourth++;
							break;
					}
				}

				printf("L:%d:%d:%d:%d:%d:%d\n", l_firsttype, l_first, l_secondtype, l_second, l_thirdtype, l_third);
				printf("A:%d:%d:%d:%d:%d:%d:%d:%d\n", a_firsttype, a_first, a_secondtype, a_second, a_thirdtype, a_third, a_outertype, a_innertype);
				printf("O:%d:%d:%d:%d\n", o_first, o_second, o_third, o_fourth);

				if (random_number(2) == 1) {
					printf("R:100:0\n");
				} else {
					randommonstermix();
				}

				randomfillmethod();

				if (random_number(6) == 1) printf("F:FLAT\n");
				if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
				if (random_number(8) == 1) printf("F:CAVERN\n");
				if (random_number(8) == 1) printf("F:CAVE\n");
				if (random_number(50) == 1) printf("F:FORGET\n");
				if (random_number(5) == 1) printf("F:RANDOM_TOWNS\n");
				if (random_number(15) == 1) printf("F:CIRCULAR_ROOMS\n");
				if (random_number(20) == 1) printf("F:DOUBLE\n");
				if (random_number(10) == 1) printf("F:WATER_RIVER\n");
				if (random_number(25) == 1) printf("F:WATER_RIVERS\n");
				if (random_number(40) == 1) printf("F:SAND_VEIN\n");
				if (random_number(40) == 1) printf("F:QUARTZ_VEIN\n");
				if (random_number(40) == 1) printf("F:MAGMA_VEIN\n");
				if (random_number(30) == 1) printf("F:NO_DOORS\n");
				if (random_number(20) == 1) printf("F:NO_DESTROY\n");
				if (random_number(40) == 1) printf("F:EMPTY\n");
				if (random_number(25) == 1) printf("F:NO_STREAMERS\n");
				if (random_number(10) == 1) printf("F:NO_SHAFT\n");

				if (random_number(20) == 1) {
					switch (random_number(3)) {
						case 1:
						default:
							printf("F:SMALLEST\n");
							break;
						case 2:
							printf("F:SMALL\n");
							break;
						case 3:
							printf("F:BIG\n");
							break;
					}
				}

				if (random_number(100) == 1) {
					if (random_number(3) == 1) printf("F:NO_BREATH\n");
					else printf("F:WATER_BREATH\n");
				}

				if (random_number(20) == 1) {

					randomdungeondamage();

				}

			} else {

				contnum = random_number(132);

				printf("# random type %d\n", contnum);

				/* Dol Amroth is the most "normal" one out of this bunch, so that's the default */
				switch (contnum) {
					case 1:
					case 2:
						printf("# based on Mirkwood\n", contnum);
						printf("L:89:95:199:5:88:0\n");
						printf("A:96:100:97:0:56:0:202:96\n");
						printf("O:20:20:20:20\n");
						printf("F:NO_DOORS | NO_DESTROY | FLAT\n");
						printf("F:FILL_METHOD_0\n");
						printf("R:100:0\n");
						break;
					case 3:
					case 4:
						printf("# based on Mordor\n", contnum);
						printf("L:88:67:93:33:1:0\n");
						printf("L:0:100:0\n");
						printf("A:97:50:56:50:56:0:57:97\n");
						printf("A:0:100:0\n");
						printf("O:20:20:20:20\n");
						printf("F:LAVA_RIVER | CAVERN | NO_STREAMERS\n");
						printf("F:QUARTZ_VEIN | MAGMA_VEIN\n");
						printf("F:FILL_METHOD_2\n");
						printf("R:100:0\n");
						break;
					case 5:
					case 6:
						printf("# based on Angband\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:100:56:0:56:0:57:58\n");
						printf("O:20:20:20:20\n");
						printf("F:CAVERN\n");
						printf("F:QUARTZ_VEIN | MAGMA_VEIN\n");
						printf("F:ADJUST_LEVEL_1_2 | ADJUST_LEVEL_1\n");
						printf("F:FILL_METHOD_0\n");
						printf("R:100:0\n");
						break;
					case 7:
					case 8:
						printf("# based on Barrow-Downs\n", contnum);
						printf("L:88:78:89:18:199:4\n");
						printf("L:0:95:5\n");
						printf("A:96:34:97:66:56:0:57:97\n");
						printf("A:100:0:0\n");
						printf("O:20:20:20:20\n");
						printf("F:FLAT\n");
						printf("F:FILL_METHOD_3\n");
						printf("R:25:1\n");
						printf("M:UNDEAD\n");
						printf("R:75:0\n");
						break;
					case 9:
					case 10:
						printf("# based on Mount Doom\n", contnum);
						printf("L:86:90:205:10:1:0\n");
						printf("A:177:100:0:0:0:0:85:87\n");
						printf("O:10:10:30:30\n");
						printf("E:2d10:20:FIRE\n");
						printf("F:CAVE | LAVA_RIVER | NO_STREAMERS\n");
						printf("F:FILL_METHOD_0\n");
						printf("R:95:1\n");
						printf("M:IM_FIRE\n");
						printf("R:5:0\n");
						break;
					case 11:
					case 12:
						printf("# based on Nether Realm\n", contnum);
						printf("L:102:80:86:15:85:5\n");
						printf("A:85:80:87:20:87:0:57:85\n");
						printf("A:50:50:0\n");
						printf("O:25:25:25:25\n");
						printf("E:1d10:10:NETHER\n");
						printf("F:EMPTY | FORGET | NO_BREATH | NO_SHAFT\n");
						printf("F:RANDOM_TOWNS | ADJUST_LEVEL_2 | NO_STREAMERS\n");
						printf("F:LAVA_RIVER\n");
						printf("F:FILL_METHOD_2\n");
						printf("R:5:0\n");
						printf("R:95:3\n");
						printf("M:RES_NETH | R_CHAR_G | R_CHAR_W | R_CHAR_U\n");
						break;
					case 13:
					case 14:
						printf("# based on Numenor\n", contnum);
						printf("L:84:95:187:5:1:0\n");
						printf("A:187:80:84:10:56:10:57:187\n");
						printf("A:60:0:40\n");
						printf("O:30:30:10:10\n");
						printf("E:1d1:4:ACID\n");
						printf("F:NO_STREAMERS\n");
						printf("F:FILL_METHOD_3 | WATER_BREATH\n");
						printf("R:20:0\n");
						printf("R:80:3\n");
						printf("M:AQUATIC | CAN_SWIM | CAN_FLY\n");
						break;
					case 15:
					case 16:
						printf("# based on Halls of Mandos\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("O:20:20:20:20\n");
						printf("A:56:100:56:0:56:0:57:58\n");
						printf("F:RANDOM_TOWNS | NO_SHAFT\n");
						printf("F:FILL_METHOD_0\n");
						break;
					case 17:
					case 18:
						printf("# based on Cirith Ungol\n", contnum);
						printf("L:87:5:88:65:16:30\n");
						printf("A:97:90:16:10:56:0:16:58\n");
						printf("O:30:30:30:10\n");
						printf("E:4d4:40:POISON\n");
						printf("F:CIRCULAR_ROOMS\n");
						printf("F:FILL_METHOD_2\n");
						printf("R:10:0\n");
						printf("R:45:3\n");
						printf("M:SPIDER | R_CHAR_c | R_CHAR_a | R_CHAR_I | \n");
						printf("R:45:3\n");
						printf("M:ORC | R_CHAR_w | R_CHAR_m | R_CHAR_j\n");
						break;
					case 19:
					case 20:
						printf("# based on Heart of the Earth\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:100:56:0:56:0:57:58\n");
						printf("O:40:10:10:20\n");
						printf("G:life\n");
						printf("F:EVOLVE | NO_SHAFT\n");
						printf("R:40:3\n");
						printf("M:R_CHAR_# | R_CHAR_X | R_CHAR_g | R_CHAR_E | \n");
						printf("R:30:3\n");
						printf("M:PASS_WALL | KILL_WALL | HURT_ROCK\n");
						printf("R:30:0\n");
						break;
					case 21:
					case 22:
						printf("# based on Void\n", contnum);
						printf("L:183:97:102:3:0:0\n");
						printf("A:183:90:102:10:0:0:102:102\n");
						printf("A:40:60:0\n");
						printf("O:25:25:25:25\n");
						printf("E:2d6:200:DARK\n");
						printf("F:EMPTY | FORGET | NO_BREATH |\n");
						printf("F:ADJUST_LEVEL_1_2 | ADJUST_LEVEL_1 | NO_STREAMERS | NO_SHAFT\n");
						printf("F:FILL_METHOD_2\n");
						printf("R:1:0\n");
						printf("R:99:3\n");
						printf("M:UNDEAD | DEMON | DRAGON | NONLIVING | SPIRIT\n");
						break;
					case 23:
					case 24:
						printf("# based on Paths of the Dead\n", contnum);
						printf("L:88:85:84:15:1:0\n");
						printf("A:56:75:87:25:56:0:57:58\n");
						printf("O:30:30:30:2\n");
						printf("E:1d1:20:RAISE\n");
						printf("F:FILL_METHOD_3\n");
						printf("R:5:0\n");
						printf("R:10:3\n");
						printf("M:R_CHAR_p\n");
						printf("R:85:3\n");
						printf("M:UNDEAD | NONLIVING\n");
						break;
					case 25:
					case 26:
						printf("# based on Illusory Castle\n", contnum);
						printf("L:1:98:188:2:1:0\n");
						printf("A:56:50:189:50:56:0:57:58\n");
						printf("O:50:10:20:20\n");
						printf("E:6d2:30:CONFUSION\n");
						printf("F:RANDOM_TOWNS | NO_STREAMERS\n");
						printf("F:FILL_METHOD_1\n");
						printf("R:30:0\n");
						printf("R:70:3\n");
						printf("M:STUPID | WEIRD_MIND | SHAPECHANGER | ATTR_MULTI | CHAR_MULTI | RAND_25 |\n");
						printf("M:RAND_50 | EMPTY_MIND | INVISIBLE | PASS_WALL | KILL_WALL\n");
						printf("S:BR_CONF | BR_CHAO | BA_CHAO | CONF | FORGET | TRAPS | MULTIPLY\n");
						break;
					case 27:
					case 28:
						printf("# based on Maze\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:98:48:2:56:0:57:58\n");
						printf("O:2:40:10:40\n");
						printf("G:maze\n");
						printf("F:SMALLEST | FORGET\n");
						printf("R:80:0\n");
						printf("R:20:3\n");
						printf("M:R_CHAR_p\n");
						break;
					case 29:
					case 30:
						printf("# based on Orc Cave\n", contnum);
						printf("L:88:100:1:0:1:0\n");
						printf("A:97:100:56:0:56:0:57:97\n");
						printf("O:5:50:10:25\n");
						printf("F:RANDOM_TOWNS | \n");
						printf("F:CAVE |\n");
						printf("F:FILL_METHOD_0\n");
						printf("R:30:3\n");
						printf("M:TROLL\n");
						printf("R:20:0\n");
						printf("R:50:3\n");
						printf("M:ORC | R_CHAR_k | R_CHAR_o | R_CHAR_O\n");
						break;
					case 31:
					case 32:
						printf("# based on Erebor\n", contnum);
						printf("L:88:100:1:0:1:0\n");
						printf("A:97:90:87:10:56:0:57:97\n");
						printf("O:40:40:40:40\n");
						printf("F:BIG | LAVA_RIVER | CAVERN | NO_STREAMERS\n");
						printf("F:CAVE | DOUBLE | \n");
						printf("F:FILL_METHOD_2\n");
						printf("R:20:0\n");
						printf("R:55:1\n");
						printf("M:DRAGON | R_CHAR_D\n");
						printf("R:25:1\n");
						printf("M:DRAGON | R_CHAR_d\n");
						break;
					case 33:
					case 34:
						printf("# based on Old Forest\n", contnum);
						printf("L:88:76:84:16:199:8\n");
						printf("L:68:16:16\n");
						printf("A:96:100:56:0:56:0:202:96\n");
						printf("O:20:5:15:30\n");
						printf("F:WATER_RIVERS | NO_DOORS | NO_DESTROY | FLAT | NO_STREAMERS\n");
						printf("F:RANDOM_TOWNS\n");
						printf("F:FILL_METHOD_3\n");
						printf("R:30:0\n");
						printf("R:40:3\n");
						printf("M:ANIMAL \n");
						printf("R:30:3\n");
						printf("M:UNDEAD | R_CHAR_h\n");
						break;
					case 35:
					case 36:
						printf("# based on Moria\n", contnum);
						printf("L:88:100:1:0:1:0\n");
						printf("A:97:100:56:0:56:0:57:97\n");
						printf("O:30:50:10:5\n");
						printf("F:WATER_RIVER | BIG | NO_STREAMERS\n");
						printf("F:RANDOM_TOWNS\n");
						printf("F:WILD_45_30__44_37\n");
						printf("F:FILL_METHOD_0\n");
						printf("R:40:3\n");
						printf("M:ORC\n");
						printf("R:30:3\n");
						printf("M:TROLL | GIANT\n");
						printf("R:20:3\n");
						printf("M:DEMON\n");
						printf("R:10:0\n");
						break;
					case 37:
					case 38:
						printf("# based on Dol Guldur\n", contnum);
						printf("L:1:80:174:20:1:0\n");
						printf("A:56:100:56:0:56:0:57:58\n");
						printf("O:20:1:70:9\n");
						printf("F:SMALL\n");
						printf("F:FILL_METHOD_3\n");
						printf("R:30:3\n");
						printf("M:R_CHAR_p | R_CHAR_P\n");
						printf("R:10:3\n");
						printf("M:ORC | TROLL\n");
						printf("R:20:3\n");
						printf("M:UNDEAD\n");
						printf("R:30:3\n");
						printf("M:DEMON | DRAGON\n");
						printf("R:10:0\n");
						break;
					case 39:
					case 40:
						printf("# based on Small Water Cave\n", contnum);
						printf("L:84:100:84:0:84:0\n");
						printf("A:97:100:56:0:56:0:57:58\n");
						printf("O:10:10:30:30\n");
						printf("E:1d1:40:ACID\n");
						printf("F:FILL_METHOD_0\n");
						printf("R:10:0\n");
						printf("R:10:3\n");
						printf("M:AQUATIC\n");
						printf("R:40:1\n");
						printf("M:IM_COLD\n");
						printf("S:BA_WATE\n");
						printf("R:40:3\n");
						printf("M:IM_COLD\n");
						break;
					case 41:
					case 42:
						printf("# based on Sacred Land of Mountains\n", contnum);
						printf("L:89:100:89:0:89:0\n");
						printf("A:97:100:56:0:56:0:97:97\n");
						printf("O:20:20:20:20\n");
						printf("F:RANDOM_TOWNS | FLAT | NO_STREAMERS\n");
						printf("F:FILL_METHOD_0\n");
						printf("R:60:3\n");
						printf("M:CAN_FLY\n");
						printf("R:40:0\n");
						break;
					case 43:
					case 44:
						printf("# based on Land of Rhun\n", contnum);
						printf("L:89:100:1:0:1:0\n");
						printf("A:89:50:96:25:84:25:57:58\n");
						printf("O:20:20:20:20\n");
						printf("F:RANDOM_TOWNS | FLAT | NO_STREAMERS\n");
						printf("F:FILL_METHOD_1\n");
						printf("R:30:3\n");
						printf("M:R_CHAR_p | R_CHAR_h\n");
						printf("R:30:3\n");
						printf("M:ANIMAL\n");
						printf("R:40:0\n");
						break;
					case 45:
					case 46:
						printf("# based on Sandworm Lair\n", contnum);
						printf("L:91:85:94:10:93:5\n");
						printf("A:98:100:96:0:84:0:94:94\n");
						printf("O:15:5:60:20\n");
						printf("F:NO_DOORS | SAND_VEIN |\n");
						printf("F:FILL_METHOD_0\n");
						printf("R:70:3\n");
						printf("M:R_CHAR_w\n");
						printf("R:20:3\n");
						printf("S:MULTIPLY\n");
						printf("R:10:0\n");
						break;
					case 47:
					case 48:
						printf("# based on Helcaraxe\n", contnum);
						printf("L:90:0:88:70:84:30\n");
						printf("L:90:0:10\n");
						printf("A:95:0:56:100:56:0:57:58\n");
						printf("A:100:0:0\n");
						printf("O:20:20:20:20\n");
						printf("E:1d4:45:COLD\n");
						printf("F:DOUBLE | WATER_RIVER | CAVERN | NO_STREAMERS\n");
						printf("F:FILL_METHOD_2\n");
						printf("R:95:1\n");
						printf("M:IM_COLD\n");
						printf("R:5:0\n");
						break;
					case 49:
					case 50:
						printf("# based on Forodwaith\n", contnum);
						printf("L:90:20:93:40:88:40\n");
						printf("A:90:20:93:40:88:40:88:211\n");
						printf("O:20:20:20:20\n");
						printf("E:2d4:5:DARK\n");
						printf("F:NO_DOORS | CAVERN | COLD | NO_DESTROY | EMPTY | FLAT | \n");
						printf("F:LIFE_LEVEL | NO_STREAMERS | NO_SHAFT | \n");
						printf("F:FILL_METHOD_4\n");
						printf("R:95:3\n");
						printf("M:COLD_BLOOD | HURT_LITE | IM_COLD\n");
						printf("R:5:0\n");
						break;
					case 51:
					case 52:
						printf("# based on Emyn Luin\n", contnum);
						printf("L:89:45:81:5:88:50\n");
						printf("A:215:100:215:0:215:0:56:211\n");
						printf("O:50:20:10:30\n");
						printf("F:CAVE | CAVERN | CIRCULAR_ROOMS | RANDOM_TOWNS | \n");
						printf("F:NO_STREAMERS | NO_DESTROY \n");
						printf("F:FILL_METHOD_3\n");
						printf("R:20:3\n");
						printf("M:R_CHAR_k | R_CHAR_o\n");
						printf("R:80:0\n");
						break;
					case 53:
					case 54:
					default:
						printf("# based on Dol Amroth\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:100:56:0:56:0:57:58\n");
						printf("O:20:20:20:20\n");
						printf("F:SMALLEST | NO_DESTROY | TOWER | RANDOM_TOWNS | \n");
						printf("F:ADJUST_LEVEL_1 | NO_STREAMERS | NO_SHAFT | NO_STAIR | \n");
						printf("F:FILL_METHOD_2 \n");
						printf("R:80:3\n");
						printf("M:R_CHAR_p | R_CHAR_P \n");
						printf("R:20:3\n");
						printf("M:SMART | TAKE_ITEM\n");
						break;
					case 55:
					case 56:
						printf("# based on Angmar\n", contnum);
						printf("L:93:70:174:20:226:10\n");
						printf("A:214:100:214:0:214:0:214:214\n");
						printf("O:20:20:20:20\n");
						printf("E:1d1:5:DISENCHANT\n");
						printf("F:ADJUST_LEVEL_1_2 | NO_DOORS | NO_STREAMERS | \n");
						printf("F:HOT | FLAT | NO_SHAFT | CIRCULAR_ROOMS | \n");
						printf("F:FILL_METHOD_2\n");
						printf("R:50:0\n");
						printf("R:50:3\n");
						printf("M:RES_DISE | UNDEAD | DEMON | NONLIVING \n");
						break;
					case 57:
					case 58:
						printf("# based on Near Harad\n", contnum);
						printf("L:91:100:91:0:91:0\n");
						printf("A:91:100:91:0:91:0:98:91\n");
						printf("O:20:20:20:20\n");
						printf("E:1d1:5:INERTIA\n");
						printf("F:NO_DOORS | CAVE | CAVERN | HOT | NO_DESTROY | EMPTY | FLAT\n");
						printf("F:RANDOM_TOWNS | NO_STREAMERS | NO_SHAFT | \n");
						printf("F:FILL_METHOD_4 \n");
						printf("R:30:0\n");
						printf("R:70:3\n");
						printf("M:WILD_TOO | R_CHAR_p\n");
						break;
					case 59:
					case 60:
						printf("# based on Isengard\n", contnum);
						printf("L:88:100:1:0:1:0\n");
						printf("A:97:100:56:0:56:0:57:97\n");
						printf("O:20:20:20:20\n");
						printf("F:CAVE | ADJUST_LEVEL_2 | NO_STREAMERS | \n");
						printf("F:FILL_METHOD_0\n");
						printf("R:20:0\n");
						printf("R:30:3\n");
						printf("M:TROLL | R_CHAR_T | \n");
						printf("R:50:3\n");
						printf("M:ORC | R_CHAR_o | R_CHAR_O\n");
						break;
					case 61:
					case 62:
						printf("# based on Tol Eressea\n", contnum);
						printf("L:84:60:222:40:222:0\n");
						printf("A:211:100:211:0:211:0:211:211\n");
						printf("O:1:1:1:1\n");
						printf("F:SMALLEST | NO_DOORS | NO_DESTROY | EMPTY | FLAT | \n");
						printf("F:ADJUST_LEVEL_1 | NO_STREAMERS | NO_SHAFT | \n");
						printf("F:FILL_METHOD_0\n");
						printf("R:15:0\n");
						printf("R:85:1\n");
						printf("M:R_CHAR_B\n");
						break;
					case 63:
					case 64:
						printf("# based on Utumno\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:100:56:0:56:0:57:58\n");
						printf("O:20:20:20:20\n");
						printf("F:CAVERN \n");
						printf("F:ADJUST_LEVEL_1_2 | ADJUST_LEVEL_1 \n");
						printf("F:FILL_METHOD_0 \n");
						printf("R:100:0\n");
						break;
					case 65:
						printf("# based on Eru Temple\n", contnum);
						printf("L:88:70:89:30:1:0\n");
						printf("L:10:90:0\n");
						printf("A:97:100:56:0:56:0:57:97\n");
						printf("O:45:5:45:5\n");
						printf("F:FILL_METHOD_2 | BIG | NO_DOORS | CIRCULAR_ROOMS | EMPTY | TOWER | FLAT\n");
						printf("F:ADJUST_LEVEL_2 | ADJUST_LEVEL_1_2 | NO_SHAFT\n");
						printf("R:95:3\n");
						printf("M:EVIL | CAN_FLY\n");
						printf("R:5:0\n");
						break;
					case 66:
						printf("# based on Manwe Temple\n", contnum);
						printf("L:208:85:209:15:1:0\n");
						printf("A:211:100:56:0:56:0:210:211\n");
						printf("O:15:25:5%:5\n");
						printf("F:FILL_METHOD_4 | NO_DOORS | TOWER | CAVERN | ADJUST_LEVEL_2 | NO_SHAFT\n");
						printf("R:95:3\n");
						printf("M:INVISIBLE | ORC | IM_POIS | CAN_FLY\n");
						printf("S:BR_POIS | BR_GRAV | BA_POIS\n");
						printf("R:5:0\n");
						break;
					case 67:
						printf("# based on Tulkas Temple\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:100:56:0:56:0:58:57\n");
						printf("O:10:70:5:15\n");
						printf("F:FILL_METHOD_0 | NO_DESTROY | ADJUST_LEVEL_2\n");
						printf("R:95:3\n");
						printf("M:DEMON | EVIL\n");
						printf("R:5:0\n");
						break;
					case 68:
						printf("# based on Melkor Temple\n", contnum);
						printf("L:88:45:94:45:102:10\n");
						printf("L:35:35:30\n");
						printf("A:188:100:56:0:56:0:188:57\n");
						printf("O:25:25:25:25\n");
						printf("F:FILL_METHOD_1 | SMALL | LAVA_RIVERS | ADJUST_LEVEL_1 | ADJUST_LEVEL_1_2\n");
						printf("R:95:3\n");
						printf("M:GOOD | EVIL\n");
						printf("R:5:0\n");
						break;
					case 69:
						printf("# based on AmyBSOD Temple\n", contnum);
						printf("L:88:45:94:45:102:10\n");
						printf("L:35:35:30\n");
						printf("A:188:100:56:0:56:0:188:57\n");
						printf("O:25:25:25:25\n");
						printf("F:FILL_METHOD_1 | LAVA_RIVERS | BIG | ADJUST_LEVEL_2\n");
						printf("R:95:3\n");
						printf("M:GOOD | EVIL | ELDRITCH_HORROR\n");
						printf("R:5:0\n");
						break;
					case 70:
						printf("# based on Yavanna Temple\n", contnum);
						printf("L:89:40:199:15:88:45\n");
						printf("A:96:100:56:0:56:0:202:96\n");
						printf("O:20:10:30:40\n");
						printf("F:FILL_METHOD_1 | NO_DOORS | WATER_RIVERS | NO_DESTROY | ADJUST_LEVEL_1\n");
						printf("F:ADJUST_LEVEL_1_2 | NO_SHAFT | NO_GENO\n");
						printf("R:95:3\n");
						printf("M:DEMON | UNDEAD | NONLIVING\n");
						printf("R:5:0\n");
						break;
					case 71:
						printf("# based on Aule Temple\n", contnum);
						printf("L:88:45:94:45:84:10\n");
						printf("L:35:35:30\n");
						printf("A:216:100:56:0:56:0:216:213\n");
						printf("O:0:50:0:50\n");
						printf("F:FILL_METHOD_1 | SMALL | NO_DESTROY | ADJUST_LEVEL_1 | NO_STREAMERS\n");
						printf("R:100:0\n");
						break;
					case 72:
						printf("# based on Varda Temple\n", contnum);
						printf("L:89:85:82:15:1:0\n");
						printf("A:211:100:56:0:56:0:210:211\n");
						printf("O:15:5:55:25\n");
						printf("F:FILL_METHOD_4 | NO_DOORS | TOWER | CAVERN | ADJUST_LEVEL_1 | NO_SHAFT\n");
						printf("R:95:3\n");
						printf("M:EMPTY_MIND | ORC | IM_POIS | CAN_FLY | WILD_VOLCANO\n");
						printf("S:BR_POIS | BR_GRAV | BA_POIS\n");
						printf("R:5:0\n");
						break;
					case 73:
						printf("# based on Ulmo Temple\n", contnum);
						printf("L:88:100:1:0:1:0\n");
						printf("A:187:100:56:0:56:0:238:84\n");
						printf("O:90:0:5:5\n");
						printf("F:FILL_METHOD_0 | NO_DESTROY | ADJUST_LEVEL_2\n");
						printf("R:95:3\n");
						printf("M:RES_WATE | CAN_FLY | AQUATIC\n");
						printf("R:5:0\n");
						break;
					case 74:
						printf("# based on Mandos Temple\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:100:56:0:56:0:58:57\n");
						printf("O:10:70:5:15\n");
						printf("F:FILL_METHOD_0 | NO_DESTROY | ADJUST_LEVEL_2\n");
						printf("R:95:3\n");
						printf("M:UNDEAD | EVIL | R_CHAR_V\n");
						printf("R:5:0\n");
						break;
					case 75:
					case 76:
						printf("# based on Barad-Dur\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:100:56:0:56:0:57:58\n");
						printf("O:20:20:20:20\n");
						printf("F:NO_STREAMERS | NO_SHAFT | CAVERN |\n");
						printf("F:FILL_METHOD_2\n");
						break;
					case 77:
					case 78:
						printf("# based on Amon Sul\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:80:50:20:56:0:57:50\n");
						printf("O:20:20:20:20\n");
						printf("F:NO_STREAMERS | NO_SHAFT | CAVERN |\n");
						printf("F:FILL_METHOD_3\n");
						break;
					case 79:
					case 80:
						printf("# based on Hornburg\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:56:70:50:30:56:0:57:50\n");
						printf("O:20:20:20:20\n");
						printf("F:NO_SHAFT | RANDOM_TOWNS |\n");
						printf("F:FILL_METHOD_1\n");
						break;
					case 81:
					case 82:
						printf("# based on Fornost Erain\n", contnum);
						printf("L:1:70:89:25:79:5\n");
						printf("A:56:40:96:50:226:10:57:58\n");
						printf("O:20:20:20:20\n");
						printf("F:NO_SHAFT | RANDOM_TOWNS | FLAT | CAVE | WATER_RIVER | LAVA_RIVER |\n");
						printf("F:FILL_METHOD_1\n");
						printf("E:2d5:60:SHARDS\n");
						break;
					case 83:
					case 84:
						printf("# based on Caras Galadhon\n", contnum);
						printf("L:1:80:86:20:1:0\n");
						printf("A:56:80:87:10:177:10:205:58\n");
						printf("O:20:20:20:20\n");
						printf("F:RANDOM_TOWNS | BIG | LAVA_RIVERS | CAVERN |\n");
						printf("F:FILL_METHOD_4\n");
						printf("E:3d5:30:SOUND\n");
						break;
					case 85:
					case 86:
						printf("# based on R'lyeh\n", contnum);
						printf("L:93:80:94:10:207:10\n");
						printf("A:56:100:56:0:56:0:56:56\n");
						printf("O:20:20:20:20\n");
						printf("F:RANDOM_TOWNS | NO_DOORS | LAVA_RIVER | WATER_RIVER | CAVERN |\n");
						printf("F:NO_SHAFT | MAGMA_VEIN | QUARTZ_VEIN\n");
						printf("F:FILL_METHOD_2\n");
						printf("R:40:0\n");
						printf("R:30:3\n");
						printf("M:ELDRITCH_HORROR\n");
						printf("R:20:3\n");
						printf("S:MIND_BLAST\n");
						printf("R:10:3\n");
						printf("S:BRAIN_SMASH\n");
						break;
					case 87:
					case 88:
						printf("# based on Middle Harad\n", contnum);
						printf("L:91:100:91:0:91:0\n");
						printf("A:91:100:91:0:91:0:91:91\n");
						printf("O:20:20:20:20\n");
						printf("F:NO_DOORS | CAVE | CAVERN | HOT | EMPTY | FLAT\n");
						printf("F:RANDOM_TOWNS | NO_SHAFT |\n");
						printf("F:FILL_METHOD_4\n");
						printf("E:1d1:5:INERTIA\n");
						printf("R:10:0\n");
						printf("R:40:3\n");
						printf("M:WILD_TOO | R_CHAR_p\n");
						printf("R:40:3\n");
						printf("M:WILD_WASTE\n");
						printf("R:10:3\n");
						printf("M:WILD_GRASS\n");
						break;
					case 89:
					case 90:
						printf("# based on Far Harad\n", contnum);
						printf("L:89:80:88:15:96:5\n");
						printf("A:96:90:98:5:16:5:96:96\n");
						printf("O:20:20:20:20\n");
						printf("F:NO_DOORS | CAVE | CAVERN | FLAT | BIG | WATER_RIVERS | \n");
						printf("F:RANDOM_TOWNS | NO_SHAFT | NO_DESTROY | SAND_VEIN | \n");
						printf("F:FILL_METHOD_4 \n");
						printf("R:10:0\n");
						printf("R:40:3\n");
						printf("M:ANIMAL\n");
						printf("R:30:3\n");
						printf("M:WILD_WOOD\n");
						printf("R:10:3\n");
						printf("M:WILD_GRASS\n");
						printf("R:10:3\n");
						printf("M:WILD_SWAMP\n");
						break;
					case 91:
					case 92:
						printf("# based on Graveyard\n", contnum);
						printf("L:1:85:84:15:1:0\n");
						printf("A:56:90:87:10:56:0:56:56\n");
						printf("O:20:20:20:20\n");
						printf("F:WATER_RIVER | EMPTY\n");
						printf("R:10:0\n");
						printf("R:90:3\n");
						printf("M:UNDEAD | NONLIVING\n");
						break;
					case 93:
					case 94:
						printf("# based on Volcano\n", contnum);
						printf("L:88:40:86:40:85:20\n");
						printf("A:56:90:87:10:56:0:56:56\n");
						printf("O:20:20:20:20\n");
						printf("E:2d5:15:FIRE\n");
						printf("F:CAVE | CAVERN | LAVA_RIVER |\n");
						printf("F:QUARTZ_VEIN\n");
						printf("R:10:0\n");
						printf("R:90:3\n");
						printf("M:IM_FIRE | CAN_FLY | WILD_VOLCANO\n");
						break;
					case 95:
					case 96:
						printf("# based on Hell\n", contnum);
						printf("L:86:80:85:20:1:0\n");
						printf("A:85:80:87:20:87:0:56:85\n");
						printf("O:20:20:20:20\n");
						printf("E:2d7:10:FIRE\n");
						printf("F:CAVERN | LAVA_RIVER | \n");
						printf("F:BIG\n");
						printf("R:10:0\n");
						printf("R:90:3\n");
						printf("M:IM_FIRE | EVIL\n");
						break;
					case 97:
					case 98:
						printf("# based on Heaven\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:60:100:56:0:56:0:60:56\n");
						printf("O:20:20:20:20\n");
						printf("F:WATER_RIVER | BIG | \n");
						printf("R:10:0\n");
						printf("R:90:3\n");
						printf("M:GOOD\n");
						break;
					case 99:
					case 100:
						printf("# based on Mountain\n", contnum);
						printf("L:89:100:89:0:89:0\n");
						printf("A:97:100:56:0:56:0:97:56\n");
						printf("O:20:20:20:20\n");
						printf("F:WATER_RIVER | CAVE | CAVERN | NO_DOORS | BIG |\n");
						printf("R:10:0\n");
						printf("R:40:3\n");
						printf("M:TROLL | GIANT\n");
						printf("R:40:3\n");
						printf("M:CAN_FLY | ANIMAL | WILD_MOUNTAIN\n");
						printf("R:10:3\n");
						printf("M:R_CHAR_O | R_CHAR_Y | R_CHAR_H\n");
						break;
					case 101:
					case 102:
						printf("# based on Glass Castle\n", contnum);
						printf("L:208:100:208:0:208:0\n");
						printf("A:188:100:188:0:188:0:188:188\n");
						printf("O:20:20:20:20\n");
						printf("F:EMPTY | RANDOM_TOWNS |\n");
						printf("R:10:0\n");
						printf("R:70:3\n");
						printf("M:INVISIBLE | HAS_LITE\n");
						printf("R:20:3\n");
						printf("S:BR_LITE | BR_DARK | BA_DARK\n");
						break;
					case 103:
					case 104:
						printf("# based on Icky Cave\n", contnum);
						printf("L:226:20:89:60:84:20\n");
						printf("A:56:100:56:0:56:0:56:56\n");
						printf("O:20:20:20:20\n");
						printf("F:MAGMA_VEIN | QUARTZ_VEIN\n");
						printf("R:10:0\n");
						printf("R:90:3\n");
						printf("M:R_CHAR_i | R_CHAR_j | R_CHAR_M\n");
						break;
					case 105:
					case 106:
						printf("# based on Mount Olympus\n", contnum);
						printf("L:1:100:1:0:1:0\n");
						printf("A:60:40:97:0:56:60:56:56\n");
						printf("O:20:20:20:20\n");
						printf("F:WATER_RIVER | CAVE | CAVERN | NO_DOORS | BIG\n");
						printf("R:50:0\n");
						printf("R:50:3\n");
						printf("M:GIANT\n");
						break;
					case 107:
						printf("# based on Orome Temple\n", contnum);
						printf("L:89:40:89:15:88:45\n");
						printf("A:96:100:56:0:56:0:96:96\n");
						printf("O:40:10:20:30\n");
						printf("F:NO_DOORS | WATER_RIVER | ADJUST_LEVEL_1 | FLAT | SAND_VEIN\n");
						printf("F:NO_SHAFT\n");
						printf("R:95:3\n");
						printf("M:ANIMAL\n");
						printf("S:S_ANT | S_SPIDER | S_HOUND\n");
						printf("R:5:0\n");
						break;
					case 108:
						printf("# randomized forest\n", contnum);
						printf("L:89:95:199:5:88:0\n");
						printf("A:96:100:97:0:56:0:202:96\n");
						printf("O:20:20:20:20\n");
						randomfillmethod();
						if (random_number(4) == 1) printf("F:FLAT\n");
						if (random_number(12) == 1) printf("F:CAVERN\n");
						if (random_number(15) == 1) printf("F:CAVE\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(20) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(20) == 1) printf("F:DOUBLE\n");
						if (random_number(10) == 1) printf("F:WATER_RIVER\n");
						if (random_number(25) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(40) == 1) printf("F:SAND_VEIN\n");
						if (random_number(100) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(100) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(4) == 1) printf("F:NO_DOORS\n");
						if (random_number(5) == 1) printf("F:NO_DESTROY\n");
						if (random_number(40) == 1) printf("F:EMPTY\n");
						if (random_number(25) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(3) == 1) printf("F:NO_SHAFT\n");

						if (random_number(25) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}
						if (random_number(3) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}
						if (random_number(100) == 1) {
							randomdungeondamage();
						}
						break;
					case 109:
						printf("# randomized stone dungeon\n", contnum);
						printf("L:88:67:93:33:1:0\n");
						printf("L:0:100:0\n");
						printf("A:97:50:56:50:56:0:57:97\n");
						printf("A:0:100:0\n");
						printf("O:20:20:20:20\n");
						randomfillmethod();
						if (random_number(15) == 1) printf("F:FLAT\n");
						if (random_number(40) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(10) == 1) printf("F:CAVERN\n");
						if (random_number(10) == 1) printf("F:CAVE\n");
						if (random_number(200) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(20) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(30) == 1) printf("F:DOUBLE\n");
						if (random_number(20) == 1) printf("F:WATER_RIVER\n");
						if (random_number(35) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(30) == 1) printf("F:SAND_VEIN\n");
						if (random_number(20) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(20) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(30) == 1) printf("F:NO_DOORS\n");
						if (random_number(20) == 1) printf("F:NO_DESTROY\n");
						if (random_number(40) == 1) printf("F:EMPTY\n");
						if (random_number(25) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(10) == 1) printf("F:NO_SHAFT\n");

						if (random_number(20) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(30) == 1) {
							randomdungeondamage();
						}
						break;
					case 110:
						printf("# randomized nether realm\n", contnum);
						printf("L:102:80:86:15:85:5\n");
						printf("A:85:80:87:20:87:0:57:85\n");
						printf("A:50:50:0\n");
						printf("O:25:25:25:25\n");
						randomfillmethod();
						if (random_number(10) == 1) printf("F:FLAT\n");
						if (random_number(10) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(5) == 1) printf("F:CAVERN\n");
						if (random_number(5) == 1) printf("F:CAVE\n");
						if (random_number(15) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(30) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(40) == 1) printf("F:DOUBLE\n");
						if (random_number(30) == 1) printf("F:WATER_RIVER\n");
						if (random_number(50) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(20) == 1) printf("F:SAND_VEIN\n");
						if (random_number(15) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(15) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(5) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(15) == 1) printf("F:EMPTY\n");
						if (random_number(15) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(8) == 1) printf("F:NO_SHAFT\n");

						if (random_number(10) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}
						if (random_number(15) == 1) {
							if (random_number(5) != 1) printf("F:NO_BREATH\n");
							else printf("F:WATER_BREATH\n");
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(5) == 1) {
							randomdungeondamage();
						}
						break;
					case 111:
						printf("# randomized waterworld\n", contnum);
						printf("L:84:95:187:5:1:0\n");
						printf("A:187:80:84:10:56:10:57:187\n");
						printf("A:60:0:40\n");
						printf("O:30:30:10:10\n");
						randomfillmethod();

						if (random_number(5) == 1) printf("F:FLAT\n");
						if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(10) == 1) printf("F:CAVERN\n");
						if (random_number(10) == 1) printf("F:CAVE\n");
						if (random_number(50) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(30) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(40) == 1) printf("F:DOUBLE\n");
						if (random_number(5) == 1) printf("F:WATER_RIVER\n");
						if (random_number(15) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(30) == 1) printf("F:SAND_VEIN\n");
						if (random_number(50) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(50) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(10) == 1) printf("F:NO_DOORS\n");
						if (random_number(20) == 1) printf("F:NO_DESTROY\n");
						if (random_number(20) == 1) printf("F:EMPTY\n");
						if (random_number(50) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(10) == 1) printf("F:NO_SHAFT\n");

						if (random_number(12) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}
						if (random_number(20) == 1) {
							if (random_number(20) == 1) printf("F:NO_BREATH\n");
							else printf("F:WATER_BREATH\n");
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(10) == 1) {
							randomdungeondamage();
						}
						break;
					case 112:
						printf("# randomized spider lair\n", contnum);
						printf("L:87:5:88:65:16:30\n");
						printf("A:97:90:16:10:56:0:16:58\n");
						printf("O:30:30:30:10\n");
						randomfillmethod();

						if (random_number(20) == 1) printf("F:FLAT\n");
						if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(10) == 1) printf("F:CAVERN\n");
						if (random_number(10) == 1) printf("F:CAVE\n");
						if (random_number(80) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(8) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(40) == 1) printf("F:DOUBLE\n");
						if (random_number(20) == 1) printf("F:WATER_RIVER\n");
						if (random_number(35) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(10) == 1) printf("F:SAND_VEIN\n");
						if (random_number(20) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(20) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(10) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(50) == 1) printf("F:EMPTY\n");
						if (random_number(15) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(20) == 1) printf("F:NO_SHAFT\n");

						if (random_number(15) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}
						if (random_number(100) == 1) {
							if (random_number(3) == 1) printf("F:NO_BREATH\n");
							else printf("F:WATER_BREATH\n");
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(6) == 1) {
							randomdungeondamage();
						}
						break;
					case 113:
						printf("# randomized void\n", contnum);
						printf("L:183:97:102:3:0:0\n");
						printf("A:183:90:102:10:0:0:102:102\n");
						printf("A:40:60:0\n");
						printf("O:25:25:25:25\n");
						randomfillmethod();

						if (random_number(5) == 1) printf("F:FLAT\n");
						if (random_number(30) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(15) == 1) printf("F:CAVERN\n");
						if (random_number(25) == 1) printf("F:CAVE\n");
						if (random_number(15) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(25) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(40) == 1) printf("F:DOUBLE\n");
						if (random_number(30) == 1) printf("F:WATER_RIVER\n");
						if (random_number(60) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(50) == 1) printf("F:SAND_VEIN\n");
						if (random_number(70) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(70) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(10) == 1) printf("F:NO_DOORS\n");
						if (random_number(5) == 1) printf("F:NO_DESTROY\n");
						if (random_number(5) == 1) printf("F:EMPTY\n");
						if (random_number(15) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(8) == 1) printf("F:NO_SHAFT\n");

						if (random_number(30) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}
						if (random_number(15) == 1) {
							if (random_number(5) != 1) printf("F:NO_BREATH\n");
							else printf("F:WATER_BREATH\n");
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(5) == 1) {
							randomdungeondamage();
						}
						break;
					case 114:
						printf("# randomized tomb\n", contnum);
						printf("L:88:85:84:15:1:0\n");
						printf("A:56:75:87:25:56:0:57:58\n");
						printf("O:30:30:30:2\n");
						randomfillmethod();

						if (random_number(20) == 1) printf("F:FLAT\n");
						if (random_number(30) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(20) == 1) printf("F:CAVERN\n");
						if (random_number(20) == 1) printf("F:CAVE\n");
						if (random_number(100) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(25) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(40) == 1) printf("F:DOUBLE\n");
						if (random_number(20) == 1) printf("F:WATER_RIVER\n");
						if (random_number(30) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(30) == 1) printf("F:SAND_VEIN\n");
						if (random_number(20) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(20) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(40) == 1) printf("F:NO_DOORS\n");
						if (random_number(5) == 1) printf("F:NO_DESTROY\n");
						if (random_number(60) == 1) printf("F:EMPTY\n");
						if (random_number(10) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(15) == 1) printf("F:NO_SHAFT\n");

						if (random_number(15) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(10) == 1) {
							randomdungeondamage();
						}
						break;
					case 115:
						printf("# randomized illusion cabinet\n", contnum);
						printf("L:1:98:188:2:1:0\n");
						printf("A:56:50:189:50:56:0:57:58\n");
						printf("O:50:10:20:20\n");
						randomfillmethod();

						if (random_number(30) == 1) printf("F:FLAT\n");
						if (random_number(60) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(30) == 1) printf("F:CAVERN\n");
						if (random_number(30) == 1) printf("F:CAVE\n");
						if (random_number(40) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(12) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(30) == 1) printf("F:DOUBLE\n");
						if (random_number(30) == 1) printf("F:WATER_RIVER\n");
						if (random_number(40) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(30) == 1) printf("F:SAND_VEIN\n");
						if (random_number(30) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(30) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(20) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(30) == 1) printf("F:EMPTY\n");
						if (random_number(20) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(8) == 1) printf("F:NO_SHAFT\n");

						if (random_number(20) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(5) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(10) == 1) {
							randomdungeondamage();
						}
						break;
					case 116:
						printf("# randomized cave\n", contnum);
						printf("L:88:100:1:0:1:0\n");
						printf("A:97:100:56:0:56:0:57:97\n");
						printf("O:5:50:10:25\n");
						randomfillmethod();

						if (random_number(25) == 1) printf("F:FLAT\n");
						if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(20) == 1) printf("F:CAVERN\n");
						if (random_number(12) == 1) printf("F:CAVE\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(20) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(40) == 1) printf("F:DOUBLE\n");
						if (random_number(20) == 1) printf("F:WATER_RIVER\n");
						if (random_number(30) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(20) == 1) printf("F:SAND_VEIN\n");
						if (random_number(10) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(10) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(18) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(40) == 1) printf("F:EMPTY\n");
						if (random_number(10) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(5) == 1) printf("F:NO_SHAFT\n");

						if (random_number(25) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(3) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(30) == 1) {
							randomdungeondamage();
						}
						break;
					case 117:
						printf("# randomized mountain\n", contnum);
						printf("L:88:100:1:0:1:0\n");
						printf("A:97:90:87:10:56:0:57:97\n");
						printf("O:40:40:40:40\n");
						randomfillmethod();

						if (random_number(10) == 1) printf("F:FLAT\n");
						if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(30) == 1) printf("F:CAVERN\n");
						if (random_number(30) == 1) printf("F:CAVE\n");
						if (random_number(100) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(30) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(10) == 1) printf("F:DOUBLE\n");
						if (random_number(30) == 1) printf("F:WATER_RIVER\n");
						if (random_number(40) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(10) == 1) printf("F:SAND_VEIN\n");
						if (random_number(5) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(5) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(5) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(80) == 1) printf("F:EMPTY\n");
						if (random_number(12) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(6) == 1) printf("F:NO_SHAFT\n");

						if (random_number(10) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(4) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(20) == 1) {
							randomdungeondamage();
						}
						break;
					case 118:
						printf("# randomized mines\n", contnum);
						printf("L:88:100:1:0:1:0\n");
						printf("A:97:100:56:0:56:0:57:97\n");
						printf("O:30:50:10:5\n");
						randomfillmethod();

						if (random_number(30) == 1) printf("F:FLAT\n");
						if (random_number(30) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(15) == 1) printf("F:CAVERN\n");
						if (random_number(15) == 1) printf("F:CAVE\n");
						if (random_number(50) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(25) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(40) == 1) printf("F:DOUBLE\n");
						if (random_number(10) == 1) printf("F:WATER_RIVER\n");
						if (random_number(20) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(10) == 1) printf("F:SAND_VEIN\n");
						if (random_number(8) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(8) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(20) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(50) == 1) printf("F:EMPTY\n");
						if (random_number(15) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(10) == 1) printf("F:NO_SHAFT\n");

						if (random_number(12) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(3) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(15) == 1) {
							randomdungeondamage();
						}
						break;
					case 119:
						printf("# randomized wilderness\n", contnum);
						printf("L:89:100:1:0:1:0\n");
						printf("A:89:50:96:25:84:25:57:58\n");
						printf("O:20:20:20:20\n");
						randomfillmethod();

						if (random_number(4) == 1) printf("F:FLAT\n");
						if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(20) == 1) printf("F:CAVERN\n");
						if (random_number(20) == 1) printf("F:CAVE\n");
						if (random_number(200) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(20) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(50) == 1) printf("F:DOUBLE\n");
						if (random_number(15) == 1) printf("F:WATER_RIVER\n");
						if (random_number(35) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(30) == 1) printf("F:SAND_VEIN\n");
						if (random_number(30) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(30) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(10) == 1) printf("F:NO_DOORS\n");
						if (random_number(15) == 1) printf("F:NO_DESTROY\n");
						if (random_number(10) == 1) printf("F:EMPTY\n");
						if (random_number(30) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(5) == 1) printf("F:NO_SHAFT\n");

						if (random_number(30) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(30) == 1) {
							randomdungeondamage();
						}
						break;
					case 120:
						printf("# randomized wasteland\n", contnum);
						printf("L:91:85:94:10:93:5\n");
						printf("A:98:100:96:0:84:0:94:94\n");
						printf("O:15:5:60:20\n");
						randomfillmethod();

						if (random_number(10) == 1) printf("F:FLAT\n");
						if (random_number(10) == 1) printf("F:CAVERN\n");
						if (random_number(10) == 1) printf("F:CAVE\n");
						if (random_number(100) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(20) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(40) == 1) printf("F:DOUBLE\n");
						if (random_number(20) == 1) printf("F:WATER_RIVER\n");
						if (random_number(25) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(5) == 1) printf("F:SAND_VEIN\n");
						if (random_number(10) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(10) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(5) == 1) printf("F:NO_DOORS\n");
						if (random_number(12) == 1) printf("F:NO_DESTROY\n");
						if (random_number(20) == 1) printf("F:EMPTY\n");
						if (random_number(20) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(10) == 1) printf("F:NO_SHAFT\n");

						if (random_number(10) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(3) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(40) == 1) {
							randomdungeondamage();
						}
						break;
					case 121:
						printf("# randomized ice waste\n", contnum);
						printf("L:90:0:88:70:84:30\n");
						printf("L:90:0:10\n");
						printf("A:95:0:56:100:56:0:57:58\n");
						printf("A:100:0:0\n");
						printf("O:20:20:20:20\n");
						randomfillmethod();

						if (random_number(6) == 1) printf("F:FLAT\n");
						if (random_number(25) == 1) printf("F:CAVERN\n");
						if (random_number(25) == 1) printf("F:CAVE\n");
						if (random_number(50) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(30) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(10) == 1) printf("F:DOUBLE\n");
						if (random_number(10) == 1) printf("F:WATER_RIVER\n");
						if (random_number(20) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(50) == 1) printf("F:SAND_VEIN\n");
						if (random_number(30) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(30) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(10) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(20) == 1) printf("F:EMPTY\n");
						if (random_number(30) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(10) == 1) printf("F:NO_SHAFT\n");

						if (random_number(20) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(5) == 1) {
							randomdungeondamage();
						}
						break;
					case 122:
						printf("# randomized northlands\n", contnum);
						printf("L:90:20:93:40:88:40\n");
						printf("A:90:20:93:40:88:40:88:211\n");
						printf("O:20:20:20:20\n");
						randomfillmethod();

						if (random_number(4) == 1) printf("F:FLAT\n");
						if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(20) == 1) printf("F:CAVERN\n");
						if (random_number(25) == 1) printf("F:CAVE\n");
						if (random_number(50) == 1) printf("F:FORGET\n");
						if (random_number(3) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(40) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(15) == 1) printf("F:DOUBLE\n");
						if (random_number(10) == 1) printf("F:WATER_RIVER\n");
						if (random_number(15) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(20) == 1) printf("F:SAND_VEIN\n");
						if (random_number(15) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(15) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(10) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(6) == 1) printf("F:EMPTY\n");
						if (random_number(30) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(5) == 1) printf("F:NO_SHAFT\n");

						if (random_number(20) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(6) == 1) {
							randomdungeondamage();
						}
						break;
					case 123:
						printf("# randomized blue mountain\n", contnum);
						printf("L:89:45:81:5:88:50\n");
						printf("A:215:100:215:0:215:0:56:211\n");
						printf("O:50:20:10:30\n");
						randomfillmethod();

						if (random_number(8) == 1) printf("F:FLAT\n");
						if (random_number(30) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(10) == 1) printf("F:CAVERN\n");
						if (random_number(12) == 1) printf("F:CAVE\n");
						if (random_number(50) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(20) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(10) == 1) printf("F:DOUBLE\n");
						if (random_number(15) == 1) printf("F:WATER_RIVER\n");
						if (random_number(20) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(20) == 1) printf("F:SAND_VEIN\n");
						if (random_number(10) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(10) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(10) == 1) printf("F:NO_DOORS\n");
						if (random_number(6) == 1) printf("F:NO_DESTROY\n");
						if (random_number(50) == 1) printf("F:EMPTY\n");
						if (random_number(20) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(4) == 1) printf("F:NO_SHAFT\n");

						if (random_number(15) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(20) == 1) {
							randomdungeondamage();
						}
						break;
					case 124:
						printf("# randomized desert\n", contnum);
						printf("L:91:100:91:0:91:0\n");
						printf("A:91:100:91:0:91:0:98:91\n");
						printf("O:20:20:20:20\n");
						randomfillmethod();

						if (random_number(4) == 1) printf("F:FLAT\n");
						if (random_number(60) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(10) == 1) printf("F:CAVERN\n");
						if (random_number(10) == 1) printf("F:CAVE\n");
						if (random_number(30) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(10) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(15) == 1) printf("F:DOUBLE\n");
						if (random_number(12) == 1) printf("F:WATER_RIVER\n");
						if (random_number(30) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(10) == 1) printf("F:SAND_VEIN\n");
						if (random_number(30) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(30) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(5) == 1) printf("F:NO_DOORS\n");
						if (random_number(3) == 1) printf("F:NO_DESTROY\n");
						if (random_number(6) == 1) printf("F:EMPTY\n");
						if (random_number(10) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(5) == 1) printf("F:NO_SHAFT\n");

						if (random_number(25) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(4) == 1) {
							randomdungeondamage();
						}
						break;
					case 125:
						printf("# randomized ocean\n", contnum);
						printf("L:84:60:222:40:222:0\n");
						printf("A:211:100:211:0:211:0:211:211\n");
						printf("O:1:1:1:1\n");
						randomfillmethod();

						if (random_number(5) == 1) printf("F:FLAT\n");
						if (random_number(20) == 1) printf("F:CAVERN\n");
						if (random_number(20) == 1) printf("F:CAVE\n");
						if (random_number(30) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(20) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(20) == 1) printf("F:DOUBLE\n");
						if (random_number(5) == 1) printf("F:WATER_RIVER\n");
						if (random_number(8) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(30) == 1) printf("F:SAND_VEIN\n");
						if (random_number(100) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(100) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(5) == 1) printf("F:NO_DOORS\n");
						if (random_number(4) == 1) printf("F:NO_DESTROY\n");
						if (random_number(10) == 1) printf("F:EMPTY\n");
						if (random_number(40) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(5) == 1) printf("F:NO_SHAFT\n");

						if (random_number(10) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}
						if (random_number(15) == 1) {
							if (random_number(100) == 1) printf("F:NO_BREATH\n");
							else printf("F:WATER_BREATH\n");
						}

						if (random_number(3) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(5) == 1) {
							randomdungeondamage();
						}
						break;
					case 126:
						printf("# randomized grass plains\n", contnum);
						printf("L:88:70:89:30:1:0\n");
						printf("L:10:90:0\n");
						printf("A:97:100:56:0:56:0:57:97\n");
						printf("O:45:5:45:5\n");
						randomfillmethod();

						if (random_number(5) == 1) printf("F:FLAT\n");
						if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(20) == 1) printf("F:CAVERN\n");
						if (random_number(30) == 1) printf("F:CAVE\n");
						if (random_number(100) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(8) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(30) == 1) printf("F:DOUBLE\n");
						if (random_number(10) == 1) printf("F:WATER_RIVER\n");
						if (random_number(20) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(20) == 1) printf("F:SAND_VEIN\n");
						if (random_number(50) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(50) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(5) == 1) printf("F:NO_DOORS\n");
						if (random_number(5) == 1) printf("F:NO_DESTROY\n");
						if (random_number(8) == 1) printf("F:EMPTY\n");
						if (random_number(30) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(8) == 1) printf("F:NO_SHAFT\n");

						if (random_number(25) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(20) == 1) {
							randomdungeondamage();
						}
						break;
					case 127:
						printf("# randomized cloudland\n", contnum);
						printf("L:208:85:209:15:1:0\n");
						printf("A:211:100:56:0:56:0:210:211\n");
						printf("O:15:25:5%:5\n");
						randomfillmethod();

						if (random_number(10) == 1) printf("F:FLAT\n");
						if (random_number(50) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(20) == 1) printf("F:CAVERN\n");
						if (random_number(20) == 1) printf("F:CAVE\n");
						if (random_number(40) == 1) printf("F:FORGET\n");
						if (random_number(3) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(35) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(20) == 1) printf("F:DOUBLE\n");
						if (random_number(20) == 1) printf("F:WATER_RIVER\n");
						if (random_number(35) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(50) == 1) printf("F:SAND_VEIN\n");
						if (random_number(80) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(80) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(20) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(50) == 1) printf("F:EMPTY\n");
						if (random_number(40) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(8) == 1) printf("F:NO_SHAFT\n");

						if (random_number(25) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(20) == 1) {
							randomdungeondamage();
						}
						break;
					case 128:
						printf("# randomized glass castle\n", contnum);
						printf("L:88:45:94:45:102:10\n");
						printf("L:35:35:30\n");
						printf("A:188:100:56:0:56:0:188:57\n");
						printf("O:25:25:25:25\n");
						randomfillmethod();

						if (random_number(40) == 1) printf("F:FLAT\n");
						if (random_number(10) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(8) == 1) printf("F:CAVERN\n");
						if (random_number(10) == 1) printf("F:CAVE\n");
						if (random_number(50) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(40) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(20) == 1) printf("F:DOUBLE\n");
						if (random_number(40) == 1) printf("F:WATER_RIVER\n");
						if (random_number(60) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(30) == 1) printf("F:SAND_VEIN\n");
						if (random_number(10) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(10) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(10) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(20) == 1) printf("F:EMPTY\n");
						if (random_number(10) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(5) == 1) printf("F:NO_SHAFT\n");

						if (random_number(5) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(2) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(20) == 1) {
							randomdungeondamage();
						}
						break;
					case 129:
						printf("# randomized jungle\n", contnum);
						printf("L:89:80:88:15:96:5\n");
						printf("A:96:90:98:5:16:5:96:96\n");
						printf("O:20:20:20:20\n");
						randomfillmethod();

						if (random_number(4) == 1) printf("F:FLAT\n");
						if (random_number(60) == 1) printf("F:LAVA_RIVER\n");
						if (random_number(20) == 1) printf("F:CAVERN\n");
						if (random_number(20) == 1) printf("F:CAVE\n");
						if (random_number(40) == 1) printf("F:FORGET\n");
						if (random_number(2) == 1) printf("F:RANDOM_TOWNS\n");
						if (random_number(20) == 1) printf("F:CIRCULAR_ROOMS\n");
						if (random_number(20) == 1) printf("F:DOUBLE\n");
						if (random_number(10) == 1) printf("F:WATER_RIVER\n");
						if (random_number(15) == 1) printf("F:WATER_RIVERS\n");
						if (random_number(30) == 1) printf("F:SAND_VEIN\n");
						if (random_number(40) == 1) printf("F:QUARTZ_VEIN\n");
						if (random_number(40) == 1) printf("F:MAGMA_VEIN\n");
						if (random_number(20) == 1) printf("F:NO_DOORS\n");
						if (random_number(10) == 1) printf("F:NO_DESTROY\n");
						if (random_number(50) == 1) printf("F:EMPTY\n");
						if (random_number(40) == 1) printf("F:NO_STREAMERS\n");
						if (random_number(6) == 1) printf("F:NO_SHAFT\n");

						if (random_number(15) == 1) {
							switch (random_number(3)) {
								case 1:
								default:
									printf("F:SMALLEST\n");
									break;
								case 2:
									printf("F:SMALL\n");
									break;
								case 3:
									printf("F:BIG\n");
									break;
							}
						}

						if (random_number(5) == 1) {
							printf("R:100:0\n");
						} else {
							randommonstermix();
						}

						if (random_number(20) == 1) {
							randomdungeondamage();
						}
						break;
					case 130:
						printf("# based on Ingeborg Temple\n", contnum);
						printf("L:88:40:117:15:174:45\n");
						printf("A:56:80:215:20:56:0:213:216\n");
						printf("O:10:50:10:30\n");
						printf("F:LAVA_RIVER | CAVERN\n");
						printf("F:ADJUST_LEVEL_1_2 | NO_GENO\n");
						printf("R:95:3\n");
						printf("M:EVIL\n");
						printf("R:5:0\n");
						printf("E:2d3:40:CHAOS\n");
						break;
					case 131:
						printf("# based on Nienna Temple\n", contnum);
						printf("L:93:40:93:15:88:45\n");
						printf("A:92:100:56:0:56:0:92:92\n");
						printf("O:10:10:10:10\n");
						printf("F:NO_DOORS | WATER_RIVER | ADJUST_LEVEL_1 | FLAT\n");
						printf("F:NO_SHAFT\n");
						printf("R:95:3\n");
						printf("S:BR_ACID | BR_FIRE | BR_NETH | BR_DARK | BR_CHAO | BR_DISE |\n");
						printf("S:BR_TIME | BR_SHAR | BR_PLAS | BR_NUKE | BA_NUKE | BA_CHAO |\n");
						printf("S:BA_ACID | BA_FIRE | BA_NETH | BA_DARK | BO_ACID |\n");
						printf("S:BO_FIRE | BO_NETH | BO_PLAS | BO_ICEE | BR_DISI\n");
						printf("R:5:0\n");
						break;
					case 132:
						printf("# based on Este Temple\n", contnum);
						printf("L:89:90:80:5:83:5\n");
						printf("A:51:100:56:0:56:0:51:51\n");
						printf("O:25:10:35:20\n");
						printf("F:SMALL | WATER_RIVER | ADJUST_LEVEL_1 | CAVE | CAVERN\n");
						printf("F:NO_SHAFT | NO_GENO | MAGMA_VEIN\n");
						printf("R:95:3\n");
						printf("M:ANIMAL | DEMON |\n");
						printf("R:5:0\n");
						break;
				}
			}

		}

		printf("\n", num);

	} /* cycle through d_id */

}

/* Driver code */
int main()
{
	int lower = 1, upper = 59, count = 31;

	// Use current time as
	// seed for random generator
	srand(time(0));

	printRandoms(lower, upper, count);

	return 0;
}
