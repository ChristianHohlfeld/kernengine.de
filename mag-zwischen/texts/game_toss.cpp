struct game_toss : virtual kern::Library, virtual mp_rules {
  // ---- types ----
  KERN_STRUCT(TossState_Own) { U8 kind = 0; Int64 id = Int64{0x0}; I32 local = -1; Float still = 0.0; Float born = 0.0; U8 done_sent = 0; };
  KERN_STRUCT(TossState_Remote) { U8 from = 0; U8 kind = 0; U8 flags = 0; Int64 id = Int64{0x0}; Vector p = Vector{0.0, 0.0, 0.0}; Vector v = Vector{0.0, 0.0, 0.0}; Quat q = Quat{0.0, 0.0, 0.0, 1.0}; Float at = 0.0; I32 local = -1; Bool released = false; mp_PoseTimeline line = mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}; };
  KERN_STRUCT(TossState) { List<TossState_Own> own = List<TossState_Own>{}; List<TossState_Remote> remote = List<TossState_Remote>{}; Float clock = 0.0; U32 sent = 0; U32 got = 0; };
};
