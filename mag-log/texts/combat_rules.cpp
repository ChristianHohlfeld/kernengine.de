struct combat_rules : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(combat_Chaos) { Int score = 0; Int combo = 1; Float combo_until = -1.0; Float flow = 0.0; };
  KERN_ENUM(combat_EnemyType, cop, swat, heavy);
  KERN_STRUCT(combat_Fun) { Int score = 0; Int at_risk = 0; Int high_score = 0; Int stars = 0; Int max_stars = 0; Int combo = 0; Int best_combo = 0; Int momentum = 0; Float combo_expires = -1000000000.0; Float overdrive_until = -1000000000.0; Int contract_index = 0; Int contract_progress = 0; Int contracts_completed = 0; Int mercy = 0; Int last_award = 0; Float run_damage = 0.0; Bool contract_complete = false; Text last_label = "READY"; };
  KERN_STRUCT(combat_MilitaryClock) { Bool active = false; Float since5 = -1.0; Float alert_at = -1.0; Float next_reinforce = 0.0; };
  KERN_STRUCT(combat_Race) { Bool started = false; Bool finished = false; Int lap = 0; Int next = 0; Float race_start = 0.0; Float lap_start = 0.0; Float finish_time = -1.0; Float last_lap = -1.0; Float best_lap = -1.0; Float last_pass = -99.0; Bool have_prev = false; Vector prev = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(combat_ZombieRounds) { Bool active = false; Int the_round = 0; Int to_spawn = 0; Int points = 0; Float next_spawn = 0.0; Float break_until = -1.0; };
};
