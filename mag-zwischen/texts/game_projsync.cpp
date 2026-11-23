struct game_projsync : virtual kern::Library, virtual mp_rules {
  // ---- types ----
  KERN_STRUCT(ProjSync_Ghost) { mp_PoseTimeline line = mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}; Vector v = Vector{0.0, 0.0, 0.0}; Float heard = 0.0; Bool gone = false; };
  KERN_STRUCT(ProjSync) { Map<Int, ProjSync_Ghost> ghosts = Map<Int, ProjSync_Ghost>{}; List<Int_Int_pair> sent = List<Int_Int_pair>{}; Float clock = 0.0; U32 sent_msgs = 0; U32 got = 0; };
};
