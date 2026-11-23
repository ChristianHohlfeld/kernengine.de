struct combat_rules : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(combat_Arms, fists, knife, gun);
  KERN_STRUCT(combat_Chaos) { I32 score = 0; I32 combo = 1; Float combo_until = -1.0; Float flow = 0.0; };
  KERN_STRUCT(combat_Contract) { Text id = ""; Text name = ""; Text hint = ""; I32 target = 0; I32 reward = 0; };
  KERN_STRUCT(combat_EnemySpec) { Text name = ""; Float hp = 0.0; Float speed = 0.0; Float range = 0.0; Float shot_s = 0.0; I32 burst = 0; Float dmg = 0.0; Float acc = 0.0; I32 score = 0; Float scale = 0.0; Vector color = Vector{0.0, 0.0, 0.0}; Vector stripe = Vector{0.0, 0.0, 0.0}; };
  KERN_ENUM(combat_EnemyType, cop, swat, heavy);
  KERN_STRUCT(combat_Fun) { I32 score = 0; I32 at_risk = 0; I32 high_score = 0; I32 stars = 0; I32 max_stars = 0; I32 combo = 0; I32 best_combo = 0; I32 momentum = 0; Float combo_expires = -1000000000.0; Float overdrive_until = -1000000000.0; I32 contract_index = 0; I32 contract_progress = 0; I32 contracts_completed = 0; I32 mercy = 0; I32 last_award = 0; Float run_damage = 0.0; Bool contract_complete = false; Text last_label = "READY"; };
  KERN_ENUM(combat_FunEvent, tick, world_kill_person, world_kill_car, world_kill_bus, world_kill_player, police_kill, emp, escape, damage, drone_destroyed, player_death, cycle_contract);
  KERN_STRUCT(combat_Gate) { Text name = ""; Vector center = Vector{0.0, 0.0, 0.0}; Float width = 0.0; Float height = 0.0; Bool start = false; };
  KERN_ENUM(combat_MilEvent, none, alert, cancelled, start, stop);
  KERN_STRUCT(combat_MilitaryClock) { Bool active = false; Float since5 = -1.0; Float alert_at = -1.0; Float next_reinforce = 0.0; };
  KERN_STRUCT(combat_Race) { Bool started = false; Bool finished = false; I32 lap = 0; I32 next = 0; Float race_start = 0.0; Float lap_start = 0.0; Float finish_time = -1.0; Float last_lap = -1.0; Float best_lap = -1.0; Float last_pass = -99.0; Bool have_prev = false; Vector prev = Vector{0.0, 0.0, 0.0}; };
  KERN_ENUM(combat_Temper, fearful, runner, aggressive);
  KERN_ENUM(combat_ZombieEvent, none, night, round_, round_done, dawn, spawn);
  KERN_STRUCT(combat_ZombieRounds) { Bool active = false; I32 the_round = 0; I32 to_spawn = 0; I32 points = 0; Float next_spawn = 0.0; Float break_until = -1.0; };
};
