struct combat_rules : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(combat_Arms, fists, knife, gun);
  KERN_STRUCT(combat_Chaos) { Int score = 0; Int combo = 1; Float combo_until = -1.0; Float flow = 0.0; };
  KERN_STRUCT(combat_EnemySpec) { Text name = ""; Float hp = 0.0; Float speed = 0.0; Float range = 0.0; Float shot_s = 0.0; Int burst = 0; Float dmg = 0.0; Float acc = 0.0; Int score = 0; Float scale = 0.0; Vector color = Vector{0.0, 0.0, 0.0}; Vector stripe = Vector{0.0, 0.0, 0.0}; };
  KERN_ENUM(combat_EnemyType, cop, swat, heavy);
  KERN_STRUCT(combat_Fun) { Int score = 0; Int at_risk = 0; Int high_score = 0; Int stars = 0; Int max_stars = 0; Int combo = 0; Int best_combo = 0; Int momentum = 0; Float combo_expires = -1000000000.0; Float overdrive_until = -1000000000.0; Int contract_index = 0; Int contract_progress = 0; Int contracts_completed = 0; Int mercy = 0; Int last_award = 0; Float run_damage = 0.0; Bool contract_complete = false; Text last_label = "READY"; };
  KERN_ENUM(combat_FunEvent, tick, world_kill_person, world_kill_car, world_kill_bus, world_kill_player, police_kill, emp, escape, damage, drone_destroyed, player_death, cycle_contract);
  KERN_ENUM(combat_MilEvent, none, alert, cancelled, start, stop);
  KERN_STRUCT(combat_MilitaryClock) { Bool active = false; Float since5 = -1.0; Float alert_at = -1.0; Float next_reinforce = 0.0; };
  KERN_STRUCT(combat_Race) { Bool started = false; Bool finished = false; Int lap = 0; Int next = 0; Float race_start = 0.0; Float lap_start = 0.0; Float finish_time = -1.0; Float last_lap = -1.0; Float best_lap = -1.0; Float last_pass = -99.0; Bool have_prev = false; Vector prev = Vector{0.0, 0.0, 0.0}; };
  KERN_ENUM(combat_Temper, fearful, runner, aggressive);
  KERN_ENUM(combat_ZombieEvent, none, night, round_, round_done, dawn, spawn);
  KERN_STRUCT(combat_ZombieRounds) { Bool active = false; Int the_round = 0; Int to_spawn = 0; Int points = 0; Float next_spawn = 0.0; Float break_until = -1.0; };
};
