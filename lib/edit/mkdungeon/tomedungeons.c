/* Dungeon randomizer for ToME-SX (an Angband variant). Written by AmyBSOD. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Generates and prints 'count' random numbers in range [lower, upper]. */
void printRandoms(int lower, int upper, int count)
{
	int num;
	int d_id;
	int contnum;

	num = (rand() % (upper - lower + 1)) + lower;

	printf("V:2.0.0\n", num);
	printf("\n", num);

	for (d_id = 0; d_id <= 53; d_id++) {

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
				printf("W:666:676:1:0:14:160\n");
				break;
			case 7:
				printf("N:7:Submerged Ruins\n");
				printf("D:Num:a submerged way to the lost land of Numenor.\n");
				printf("W:35:50:1:0:14:160\n");
				break;
			case 8:
				printf("N:8:Halls of Mandos\n");
				printf("D:HMa:*A BUG*YOU should see this message!*\n");
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
				printf("D:Vod:a jumpgate to the Void\n");
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
			case 40:
				printf("N:40:Deep Cirith Ungol\n");
				printf("D:DCU:an entrance to Deep Cirith Ungol.\n");
				printf("W:128:228:1:0:14:160\n");
				printf("L:87:5:88:65:16:30\n");
				printf("A:97:90:16:10:56:0:16:58\n");
				printf("O:30:30:30:10\n");
				printf("E:4d4:40:POISON\n");
				printf("F:CIRCULAR_ROOMS\n");
				printf("F:FILL_METHOD_2\n");
				printf("R:20:0\n");
				printf("R:40:3\n");
				printf("M:SPIDER | R_CHAR_c | R_CHAR_a | R_CHAR_I | \n");
				printf("R:40:3\n");
				printf("M:ORC | R_CHAR_w | R_CHAR_m | R_CHAR_j\n");
				break;
			case 41:
				printf("N:41:Paths of the Undead\n");
				printf("D:PUD:the entrance to the Paths of the Undead.\n");
				printf("W:880:980:1:0:24:100\n");
				printf("L:88:85:84:15:1:0\n");
				printf("A:56:75:87:25:56:0:57:58\n");
				printf("O:30:30:30:2\n");
				printf("E:1d1:20:RAISE\n");
				printf("F:FILL_METHOD_3\n");
				printf("R:25:0\n");
				printf("R:10:3\n");
				printf("M:R_CHAR_p\n");
				printf("R:65:3\n");
				printf("M:UNDEAD | NONLIVING\n");
				break;
			case 42:
				printf("N:42:Fucktard Castle\n");
				printf("D:Ftd:an entrance to Fucktard Castle.\n");
				printf("W:400:500:1:0:24:100\n");
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
			case 43:
				printf("N:43:Deep Erebor\n");
				printf("D:DEr:a tunnel leading into depths of the Dragon Mountain.\n");
				printf("W:780:880:1:0:20:140\n");
				printf("L:88:100:1:0:1:0\n");
				printf("A:97:90:87:10:56:0:57:97\n");
				printf("O:40:40:40:40\n");
				printf("F:BIG | LAVA_RIVER | CAVERN | NO_STREAMERS\n");
				printf("F:CAVE | DOUBLE |\n");
				printf("F:FILL_METHOD_2\n");
				printf("R:40:0\n");
				printf("R:45:1\n");
				printf("M:DRAGON | R_CHAR_D\n");
				printf("R:15:1\n");
				printf("M:DRAGON | R_CHAR_d\n");
				break;
			case 44:
				printf("N:44:The Young Forest\n");
				printf("D:YFr:a path into the Young Forest.\n");
				printf("W:188:288:1:0:15:100\n");
				printf("L:88:76:84:16:199:8\n");
				printf("L:68:16:16\n");
				printf("A:96:100:56:0:56:0:202:96\n");
				printf("O:20:5:15:30\n");
				printf("F:WATER_RIVERS | NO_DOORS | FLAT | NO_STREAMERS\n");
				printf("F:RANDOM_TOWNS |\n");
				printf("F:FILL_METHOD_3\n");
				printf("R:30:0\n");
				printf("R:40:3\n");
				printf("M:ANIMAL \n");
				printf("R:30:3\n");
				printf("M:UNDEAD | R_CHAR_h\n");
				break;
			case 45:
				printf("N:45:Deep Moria\n");
				printf("D:DoM:a stone door leading to the Depths of Moria.\n");
				printf("W:248:348:1:0:40:40\n");
				printf("L:88:100:1:0:1:0\n");
				printf("A:97:100:56:0:56:0:57:97\n");
				printf("O:30:50:10:5\n");
				printf("F:WATER_RIVER | BIG | NO_STREAMERS\n");
				printf("F:RANDOM_TOWNS\n");
				printf("F:FILL_METHOD_0\n");
				printf("R:35:3\n");
				printf("M:ORC\n");
				printf("R:25:3\n");
				printf("M:TROLL | GIANT\n");
				printf("R:15:3\n");
				printf("M:DEMON\n");
				printf("R:25:0\n");
				break;
			case 46:
				printf("N:46:The Unholy Land Of Mountains\n");
				printf("D:UoM:the way to the Unholy Land of Mountains.\n");
				printf("W:740:840:1:0:14:160\n");
				printf("L:89:100:89:0:89:0\n");
				printf("A:97:100:56:0:56:0:97:97\n");
				printf("O:20:20:20:20\n");
				printf("F:RANDOM_TOWNS | FLAT | NO_STREAMERS\n");
				printf("F:FILL_METHOD_0\n");
				printf("R:60:3\n");
				printf("M:CAN_FLY\n");
				printf("R:40:0\n");
				break;
			case 47:
				printf("N:47:The Depths Of Rhun\n");
				printf("D:DoR:a way to the Depths of Rhun.\n");
				printf("W:308:408:1:0:14:160\n");
				printf("L:89:100:1:0:1:0\n");
				printf("A:89:50:96:25:84:25:57:58\n");
				printf("O:20:20:20:20\n");
				printf("F:RANDOM_TOWNS | FLAT | NO_STREAMERS |\n");
				printf("F:FILL_METHOD_1\n");
				printf("R:30:3\n");
				printf("M:R_CHAR_p | R_CHAR_h\n");
				printf("R:30:3\n");
				printf("M:ANIMAL\n");
				printf("R:40:0\n");
				break;
			case 48:
				printf("N:48:The Maggot lair\n");
				printf("D:MgL:a maggot hole.\n");
				printf("W:368:468:1:0:5:200\n");
				printf("L:91:85:94:10:93:5\n");
				printf("A:98:100:96:0:84:0:94:94\n");
				printf("O:15:5:60:20\n");
				printf("F:NO_DOORS | SAND_VEIN |\n");
				printf("F:FILL_METHOD_0\n");
				printf("R:60:3\n");
				printf("M:R_CHAR_w\n");
				printf("R:20:3\n");
				printf("S:MULTIPLY\n");
				printf("R:20:0\n");
				break;
			case 49:
				printf("N:49:Winterhold\n");
				printf("D:Who:the entrance to the Grinding Ice of the Winterhold.\n");
				printf("W:680:780:1:0:14:160\n");
				printf("L:90:0:88:70:84:30\n");
				printf("L:90:0:10\n");
				printf("A:95:0:56:100:56:0:57:58\n");
				printf("A:100:0:0\n");
				printf("O:20:20:20:20\n");
				printf("E:1d4:45:COLD\n");
				printf("F:DOUBLE | WATER_RIVER | CAVERN | NO_STREAMERS\n");
				printf("F:FILL_METHOD_2\n");
				printf("R:70:1\n");
				printf("M:IM_COLD\n");
				printf("R:30:0\n");
				break;
			case 50:
				printf("N:50:Thunder Mountain\n");
				printf("D:Thu:a path into the depths of the Thunder Mountains\n");
				printf("W:550:650:1:0:14:160\n");
				printf("L:89:45:81:5:88:50\n");
				printf("A:215:100:215:0:215:0:56:211\n");
				printf("O:50:20:10:30\n");
				printf("F:CAVE | CAVERN | CIRCULAR_ROOMS | RANDOM_TOWNS | \n");
				printf("F:NO_STREAMERS |\n");
				printf("F:FILL_METHOD_3\n");
				printf("R:20:3\n");
				printf("M:R_CHAR_k | R_CHAR_o\n");
				printf("R:80:0\n");
				break;
			case 51:
				printf("N:51:Crashing Angmar\n");
				printf("D:Cra:a dark path through the Crash King Realm of Angmar\n");
				printf("W:620:720:1:0:14:160\n");
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
			case 52:
				printf("N:52:Uber Isengard\n");
				printf("D:UIs:a passage to Uber Isengard\n");
				printf("W:428:528:1:0:14:160\n");
				printf("L:88:100:1:0:1:0\n");
				printf("A:97:100:56:0:56:0:57:97\n");
				printf("O:20:20:20:20\n");
				printf("F:CAVE | ADJUST_LEVEL_2 | NO_STREAMERS | \n");
				printf("F:FILL_METHOD_0\n");
				printf("R:30:0\n");
				printf("R:25:3\n");
				printf("M:TROLL | R_CHAR_T | \n");
				printf("R:45:3\n");
				printf("M:ORC | R_CHAR_o | R_CHAR_O\n");
				break;
			case 53:
				printf("N:53:The Open Sea\n");
				printf("D:OSe:a way to the Open Sea\n");
				printf("W:488:588:1:0:14:160\n");
				printf("L:84:60:222:40:222:0\n");
				printf("A:211:100:211:0:211:0:211:211\n");
				printf("O:1:1:1:1\n");
				printf("F:NO_DOORS | EMPTY | FLAT | \n");
				printf("F:ADJUST_LEVEL_1 | NO_STREAMERS | NO_SHAFT | \n");
				printf("F:FILL_METHOD_0\n");
				printf("R:35:0\n");
				printf("R:65:1\n");
				printf("M:R_CHAR_B\n");
				break;
		}

		switch (d_id) {
			case 1:
				printf("F:PRINCIPAL\n");
				break;
			case 2:
				printf("F:PRINCIPAL\n");
				break;
			case 3:
				printf("F:PRINCIPAL\n");
				break;
			case 4:
				printf("F:PRINCIPAL\n");
				break;
			case 6:
				printf("F:FINAL_GUARDIAN_1032 | FINAL_ARTIFACT_203 | NO_RECALL_OUT\n");
				break;
			case 7:
				printf("F:FINAL_GUARDIAN_980 | FINAL_ARTIFACT_204\n");
				break;
			case 9:
				printf("F:FINAL_GUARDIAN_481\n");
				break;
			case 10:
				printf("F:FINAL_GUARDIAN_1035\n");
				break;
			case 11:
				printf("F:NO_RECALL_OUT | FINAL_GUARDIAN_1044\n");
				break;
			case 16:
				printf("F:FINAL_GUARDIAN_804 | FINAL_ARTIFACT_91\n");
				break;
			case 17:
				printf("F:FINAL_GUARDIAN_1033 | FINAL_ARTIFACT_160\n");
				break;
			case 18:
				printf("F:FINAL_GUARDIAN_1029 | FINAL_ARTIFACT_38\n");
				break;
			case 19:
				printf("F:FINAL_OBJECT_810 | FINAL_GUARDIAN_373\n");
				break;
			case 20:
				printf("F:FINAL_GUARDIAN_715\n");
				break;
			case 21:
				printf("F:FINAL_GUARDIAN_206\n");
				break;
			case 22:
				printf("F:FINAL_GUARDIAN_872 | FORCE_DOWN\n");
				break;
			case 23:
				printf("F:FINAL_GUARDIAN_819 | FINAL_ARTIFACT_205\n");
				break;
			case 24:
				printf("F:FINAL_GUARDIAN_517\n");
				break;
			case 25:
				printf("F:FINAL_GUARDIAN_789 | FINAL_ARTIFACT_27\n");
				break;
			case 26:
				printf("F:FINAL_GUARDIAN_990\n");
				break;
			case 27:
				printf("F:FINAL_GUARDIAN_1030 | FINAL_ARTIFACT_153\n");
				break;
			case 29:
				printf("F:FINAL_GUARDIAN_1034\n");
				break;
		}

		/* randomized content for those dungeons that should have randomized content --Amy */
		if ( (d_id >= 1 && d_id <= 11) || (d_id >= 16 && d_id <= 27) || (d_id == 29) || (d_id >= 31 && d_id <= 38) ) {

			contnum = (rand() % (upper - lower + 1)) + lower;

			printf("# random type %d\n", contnum);

			/* Dol Amroth is the most "normal" one out of this bunch, so that's the default */
			switch (contnum) {
				case 1:
					printf("# based on Mirkwood\n", contnum);
					printf("L:89:95:199:5:88:0\n");
					printf("A:96:100:97:0:56:0:202:96\n");
					printf("O:20:20:20:20\n");
					printf("F:NO_DOORS | NO_DESTROY | FLAT\n");
					printf("F:FILL_METHOD_0\n");
					printf("R:100:0\n");
					break;
				case 2:
					printf("# based on Mordor\n", contnum);
					printf("L:88:67:93:33:1:0\n");
					printf("L:0:100:0\n");
					printf("A:97:50:56:50:56:0:57:97\n");
					printf("A:0:100:0\n");
					printf("O:20:20:20:20\n");
					printf("F:LAVA_RIVER | CAVERN | NO_STREAMERS\n");
					printf("F:FILL_METHOD_2\n");
					printf("R:100:0\n");
					break;
				case 3:
					printf("# based on Angband\n", contnum);
					printf("L:1:100:1:0:1:0\n");
					printf("A:56:100:56:0:56:0:57:58\n");
					printf("O:20:20:20:20\n");
					printf("F:CAVERN | NO_EASY_MOVE | NO_RECALL\n");
					printf("F:ADJUST_LEVEL_1_2 | ADJUST_LEVEL_1\n");
					printf("F:FILL_METHOD_0\n");
					printf("R:100:0\n");
					break;
				case 4:
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
				case 5:
					printf("# based on Mount Doom\n", contnum);
					printf("L:86:90:205:10:1:0\n");
					printf("A:177:100:0:0:0:0:85:87\n");
					printf("O:10:10:30:30\n");
					printf("E:2d10:20:FIRE\n");
					printf("F:CAVE | LAVA_RIVER | NO_RECALL | NO_STREAMERS\n");
					printf("F:FILL_METHOD_0 | NO_EASY_MOVE\n");
					printf("R:100:1\n");
					printf("M:IM_FIRE\n");
					break;
				case 6:
					printf("# based on Nether Realm\n", contnum);
					printf("L:102:80:86:15:85:5\n");
					printf("A:85:80:87:20:87:0:57:85\n");
					printf("A:50:50:0\n");
					printf("O:25:25:25:25\n");
					printf("E:1d10:10:NETHER\n");
					printf("F:EMPTY | FORGET | NO_BREATH | NO_EASY_MOVE | NO_SHAFT\n");
					printf("F:RANDOM_TOWNS | ADJUST_LEVEL_2 | NO_RECALL | NO_STREAMERS\n");
					printf("F:LAVA_RIVER\n");
					printf("F:FILL_METHOD_2\n");
					printf("R:5:0\n");
					printf("R:95:3\n");
					printf("M:RES_NETH | R_CHAR_G | R_CHAR_W | R_CHAR_U\n");
					break;
				case 7:
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
				case 8:
					printf("# based on Halls of Mandos\n", contnum);
					printf("L:1:100:1:0:1:0\n");
					printf("O:20:20:20:20\n");
					printf("A:56:100:56:0:56:0:57:58\n");
					printf("F:RANDOM_TOWNS | NO_RECALL | NO_SHAFT\n");
					printf("F:FILL_METHOD_0\n");
					break;
				case 9:
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
				case 10:
					printf("# based on Heart of the Earth\n", contnum);
					printf("L:1:100:1:0:1:0\n");
					printf("A:56:100:56:0:56:0:57:58\n");
					printf("O:40:10:10:20\n");
					printf("G:life\n");
					printf("F:EVOLVE | NO_RECALL | NO_SHAFT\n");
					printf("R:40:3\n");
					printf("M:R_CHAR_# | R_CHAR_X | R_CHAR_g | R_CHAR_E | \n");
					printf("R:30:3\n");
					printf("M:PASS_WALL | KILL_WALL | HURT_ROCK\n");
					printf("R:30:0\n");
					break;
				case 11:
					printf("# based on Void\n", contnum);
					printf("L:183:97:102:3:0:0\n");
					printf("A:183:90:102:10:0:0:102:102\n");
					printf("A:40:60:0\n");
					printf("O:25:25:25:25\n");
					printf("E:2d6:200:DARK\n");
					printf("F:EMPTY | FORGET | NO_BREATH | NO_EASY_MOVE | NO_RECALL |\n");
					printf("F:ADJUST_LEVEL_1_2 | ADJUST_LEVEL_1 | NO_STREAMERS | NO_SHAFT\n");
					printf("F:FILL_METHOD_2\n");
					printf("R:1:0\n");
					printf("R:99:3\n");
					printf("M:UNDEAD | DEMON | DRAGON | NONLIVING | SPIRIT\n");
					break;
				case 12:
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
				case 13:
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
				case 14:
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
				case 15:
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
				case 16:
					printf("# based on Erebor\n", contnum);
					printf("L:88:100:1:0:1:0\n");
					printf("A:97:90:87:10:56:0:57:97\n");
					printf("O:40:40:40:40\n");
					printf("F:BIG | LAVA_RIVER | CAVERN | NO_RECALL | NO_STREAMERS\n");
					printf("F:CAVE | DOUBLE | \n");
					printf("F:FILL_METHOD_2\n");
					printf("R:20:0\n");
					printf("R:55:1\n");
					printf("M:DRAGON | R_CHAR_D\n");
					printf("R:25:1\n");
					printf("M:DRAGON | R_CHAR_d\n");
					break;
				case 17:
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
				case 18:
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
				case 19:
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
				case 20:
					printf("# based on Small Water Cave\n", contnum);
					printf("L:84:100:84:0:84:0\n");
					printf("A:97:100:56:0:56:0:57:58\n");
					printf("O:10:10:30:30\n");
					printf("E:1d1:40:ACID\n");
					printf("F:NO_RECALL\n");
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
				case 21:
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
				case 22:
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
				case 23:
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
				case 24:
					printf("# based on Helcaraxe\n", contnum);
					printf("L:90:0:88:70:84:30\n");
					printf("L:90:0:10\n");
					printf("A:95:0:56:100:56:0:57:58\n");
					printf("A:100:0:0\n");
					printf("O:20:20:20:20\n");
					printf("E:1d4:45:COLD\n");
					printf("F:DOUBLE | WATER_RIVER | CAVERN | NO_STREAMERS\n");
					printf("F:FILL_METHOD_2\n");
					printf("R:100:1\n");
					printf("M:IM_COLD\n");
					break;
				case 25:
					printf("# based on Forodwaith\n", contnum);
					printf("L:90:20:93:40:88:40\n");
					printf("A:90:20:93:40:88:40:88:211\n");
					printf("O:20:20:20:20\n");
					printf("E:2d4:5:DARK\n");
					printf("F:NO_DOORS | CAVERN | COLD | NO_DESTROY | EMPTY | FLAT | \n");
					printf("F:NO_RECALL | LIFE_LEVEL | NO_STREAMERS | NO_SHAFT | \n");
					printf("F:FILL_METHOD_4\n");
					printf("R:100:3\n");
					printf("M:COLD_BLOOD | HURT_LITE | IM_COLD\n");
					break;
				case 26:
					printf("# based on Emyn Luin\n", contnum);
					printf("L:89:45:81:5:88:50\n");
					printf("A:215:100:215:0:215:0:56:211\n");
					printf("O:50:20:10:30\n");
					printf("F:CAVE | CAVERN | CIRCULAR_ROOMS | RANDOM_TOWNS | \n");
					printf("F:NO_STREAMERS | NO_RECALL | NO_DESTROY \n");
					printf("F:FILL_METHOD_3\n");
					printf("R:20:3\n");
					printf("M:R_CHAR_k | R_CHAR_o\n");
					printf("R:80:0\n");
					break;
				case 27:
				default:
					printf("# based on Dol Amroth\n", contnum);
					printf("L:1:100:1:0:1:0\n");
					printf("A:56:100:56:0:56:0:57:58\n");
					printf("O:20:20:20:20\n");
					printf("F:SMALLEST | NO_DESTROY | TOWER | RANDOM_TOWNS | \n");
					printf("F:ADJUST_LEVEL_1 | NO_STREAMERS | NO_SHAFT | NO_STAIR | \n");
					printf("F:NO_EASY_MOVE | FILL_METHOD_2 \n");
					printf("R:80:3\n");
					printf("M:R_CHAR_p | R_CHAR_P \n");
					printf("R:20:3\n");
					printf("M:SMART | TAKE_ITEM\n");
					break;
				case 28:
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
				case 29:
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
				case 30:
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
				case 31:
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
				case 32:
					printf("# based on Utumno\n", contnum);
					printf("L:1:100:1:0:1:0\n");
					printf("A:56:100:56:0:56:0:57:58\n");
					printf("O:20:20:20:20\n");
					printf("F:CAVERN | NO_EASY_MOVE | NO_RECALL \n");
					printf("F:ADJUST_LEVEL_1_2 | ADJUST_LEVEL_1 \n");
					printf("F:FILL_METHOD_0 \n");
					printf("R:100:0\n");
					break;
				case 33:
					printf("# based on Eru Temple\n", contnum);
					printf("L:88:70:89:30:1:0\n");
					printf("L:10:90:0\n");
					printf("A:97:100:56:0:56:0:57:97\n");
					printf("O:45:5:45:5\n");
					printf("F:FILL_METHOD_2 | BIG | NO_DOORS | CIRCULAR_ROOMS | EMPTY | TOWER | FLAT\n");
					printf("F:ADJUST_LEVEL_2 | ADJUST_LEVEL_1_2 | NO_SHAFT\n");
					printf("R:100:3\n");
					printf("M:EVIL | CAN_FLY\n");
					break;
				case 34:
					printf("# based on Manwe Temple\n", contnum);
					printf("L:208:85:209:15:1:0\n");
					printf("A:211:100:56:0:56:0:210:211\n");
					printf("O:15:25:5%:5\n");
					printf("F:FILL_METHOD_4 | NO_DOORS | TOWER | CAVERN | ADJUST_LEVEL_2 | NO_SHAFT\n");
					printf("R:100:3\n");
					printf("M:INVISIBLE | ORC | IM_POIS | CAN_FLY\n");
					printf("S:BR_POIS | BR_GRAV | BA_POIS\n");
					break;
				case 35:
					printf("# based on Tulkas Temple\n", contnum);
					printf("L:1:100:1:0:1:0\n");
					printf("A:56:100:56:0:56:0:58:57\n");
					printf("O:10:70:5:15\n");
					printf("F:FILL_METHOD_0 | NO_DESTROY | ADJUST_LEVEL_2\n");
					printf("R:100:3\n");
					printf("M:DEMON | EVIL\n");
					break;
				case 36:
					printf("# based on Melkor Temple\n", contnum);
					printf("L:88:45:94:45:102:10\n");
					printf("L:35:35:30\n");
					printf("A:188:100:56:0:56:0:188:57\n");
					printf("O:25:25:25:25\n");
					printf("F:FILL_METHOD_1 | SMALL | LAVA_RIVERS | ADJUST_LEVEL_1 | ADJUST_LEVEL_1_2\n");
					printf("R:100:3\n");
					printf("M:GOOD | EVIL\n");
					break;
				case 37:
					printf("# based on AmyBSOD Temple\n", contnum);
					printf("L:88:45:94:45:102:10\n");
					printf("L:35:35:30\n");
					printf("A:188:100:56:0:56:0:188:57\n");
					printf("O:25:25:25:25\n");
					printf("F:FILL_METHOD_1 | LAVA_RIVERS | BIG | ADJUST_LEVEL_2\n");
					printf("R:100:3\n");
					printf("M:GOOD | EVIL | ELDRITCH_HORROR\n");
					break;
				case 38:
					printf("# based on Yavanna Temple\n", contnum);
					printf("L:89:40:199:15:88:45\n");
					printf("A:96:100:56:0:56:0:202:96\n");
					printf("O:20:10:30:40\n");
					printf("F:FILL_METHOD_1 | NO_DOORS | WATER_RIVERS | NO_DESTROY | ADJUST_LEVEL_1 | NO_RECALL\n");
					printf("F:ADJUST_LEVEL_1_2 | NO_SHAFT | NO_GENO\n");
					printf("R:100:3\n");
					printf("M:DEMON | UNDEAD | NONLIVING\n");
					break;
				case 39:
					printf("# based on Aule Temple\n", contnum);
					printf("L:88:45:94:45:84:10\n");
					printf("L:35:35:30\n");
					printf("A:216:100:56:0:56:0:216:213\n");
					printf("O:0:50:0:50\n");
					printf("F:FILL_METHOD_1 | SMALL | NO_DESTROY | ADJUST_LEVEL_1 | NO_STREAMERS\n");
					printf("R:100:0\n");
					break;
				case 40:
					printf("# based on Varda Temple\n", contnum);
					printf("L:89:85:82:15:1:0\n");
					printf("A:211:100:56:0:56:0:210:211\n");
					printf("O:15:5:55:25\n");
					printf("F:FILL_METHOD_4 | NO_DOORS | TOWER | CAVERN | ADJUST_LEVEL_1 | NO_SHAFT\n");
					printf("R:100:3\n");
					printf("M:EMPTY_MIND | ORC | IM_POIS | CAN_FLY | WILD_VOLCANO\n");
					printf("S:BR_POIS | BR_GRAV | BA_POIS\n");
					break;
				case 41:
					printf("# based on Ulmo Temple\n", contnum);
					printf("L:88:100:1:0:1:0\n");
					printf("A:187:100:56:0:56:0:238:84\n");
					printf("O:90:0:5:5\n");
					printf("F:FILL_METHOD_0 | NO_DESTROY | ADJUST_LEVEL_2\n");
					printf("R:100:3\n");
					printf("M:RES_WATE | CAN_FLY | AQUATIC\n");
					break;
				case 42:
					printf("# based on Mandos Temple\n", contnum);
					printf("L:1:100:1:0:1:0\n");
					printf("A:56:100:56:0:56:0:58:57\n");
					printf("O:10:70:5:15\n");
					printf("F:FILL_METHOD_0 | NO_DESTROY | ADJUST_LEVEL_2\n");
					printf("R:100:3\n");
					printf("M:UNDEAD | EVIL | R_CHAR_V\n");
					break;
			}

		}

		printf("\n", num);

	} /* cycle through d_id */

}

/* Driver code */
int main()
{
	int lower = 1, upper = 42, count = 31;

	// Use current time as
	// seed for random generator
	srand(time(0));

	printRandoms(lower, upper, count);

	return 0;
}