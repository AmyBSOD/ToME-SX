/* File: plots.h */

/* Purpose: extern plots declarations */

extern bool quest_null_hook(int q);

/******* Random Quests ********/
extern bool is_randhero(int level);
extern bool quest_random_init_hook(int q_idx);

/******* Plot main ********/
extern bool quest_necro_init_hook(int q_idx);
extern bool quest_one_init_hook(int q_idx);
extern bool quest_sauron_init_hook(int q_idx);
extern bool quest_morgoth_init_hook(int q_idx);
extern bool quest_ultra_good_init_hook(int q_idx);
extern bool quest_ultra_evil_init_hook(int q_idx);

/******* Plot Bree *********/
extern bool quest_thieves_init_hook(int q_idx);
extern bool quest_hobbit_init_hook(int q_idx);
extern bool quest_troll_init_hook(int q_idx);
extern bool quest_wight_init_hook(int q_idx);
extern bool quest_nazgul_init_hook(int q_idx);
extern bool quest_shroom_init_hook(int q_idx);

/******* Plot Lorien *********/
extern bool quest_wolves_init_hook(int q_idx);
extern bool quest_spider_init_hook(int q_idx);
extern bool quest_poison_init_hook(int q_idx);

/******* Plot Gondolin *********/
extern bool quest_dragons_init_hook(int q_idx);
extern bool quest_eol_init_hook(int q_idx);
extern bool quest_nirnaeth_init_hook(int q_idx);
extern bool quest_invasion_init_hook(int q_idx);

/******* Plot Minas Anor *********/
extern bool quest_haunted_init_hook(int q_idx);
extern bool quest_between_init_hook(int q_idx);

/******* Plot Khazad-dum *********/
extern bool quest_evil_init_hook(int q_idx);

/******* Plot Hobbiton *********/
extern bool quest_shoes_init_hook(int q_idx);

/******* Plot Osgiliath *********/
extern bool quest_eldritch_init_hook(int q_idx);

/******* Plot Edoras *********/
extern bool quest_glacier_init_hook(int q_idx);

/******* Plot Pelargir *********/
extern bool quest_ocean_init_hook(int q_idx);

/******* Plot Dale *********/
extern bool quest_dale_init_hook(int q_idx);

/******* Plot Cerin Amroth *********/
extern bool quest_cerin_init_hook(int q_idx);

/******* Plot Imladris *********/
extern bool quest_imla_init_hook(int q_idx);

/******* Plot Helm's Deep *********/
extern bool quest_helm_init_hook(int q_idx);

/******* Plot Thranduil's Halls *********/
extern bool quest_thrand_init_hook(int q_idx);

/******* Plot Henneth Annun *********/
extern bool quest_henn_init_hook(int q_idx);

/******* Plot Beorn's Halls *********/
extern bool quest_beorn_init_hook(int q_idx);

/******* Plot Esgaroth *********/
extern bool quest_esga_init_hook(int q_idx);

/******* Plot Other *********/
extern bool quest_narsil_init_hook(int q_idx);
extern bool quest_thrain_init_hook(int q_idx);
