struct ragdoll : virtual kern::Library, virtual engine_physics_physics {
  // ---- types ----
  KERN_ENUM(Ragdolls_Bone, pelvis, chest, head, upper_arm_l, upper_arm_r, lower_arm_l, lower_arm_r, thigh_l, thigh_r, shin_l, shin_r, kBones);
  KERN_STRUCT(Ragdolls_Colors) { Vector shirt = Vector{0.35, 0.44, 0.53}; Vector pants = Vector{0.17, 0.19, 0.22}; Vector skin = Vector{0.82, 0.66, 0.53}; Vector hair = Vector{0.18, 0.11, 0.06}; Bool cap = false; Bool vest = false; };
  KERN_STRUCT(Ragdolls_Up) { Int64 key = Int64{0x0}; Vector feet = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; };
  KERN_STRUCT(Ragdolls_Draw) { Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; Vector tint = Vector{0.0, 0.0, 0.0}; Bool sphere = false; U8 bone = 0; U32 doll = 0; Mat4 pose = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_STRUCT(Ragdolls_Doll) { Bool active = false; Bool recover = false; Bool getting_up = false; Int64 key = Int64{0x0}; Fixed<physics_Body, 11> bodies = List<physics_Body>{}; Fixed<physics_Joint, 11> joints = List<physics_Joint>{}; Fixed<physics_Pose, 11> pose = List<physics_Pose>{}; Fixed<physics_Pose, 11> from = List<physics_Pose>{}; Fixed<physics_Pose, 11> stand = List<physics_Pose>{}; Ragdolls_Colors colors = Ragdolls_Colors{Vector{0.35, 0.44, 0.53}, Vector{0.17, 0.19, 0.22}, Vector{0.82, 0.66, 0.53}, Vector{0.18, 0.11, 0.06}, false, false}; Float born = 0.0; Float settled = 0.0; Float getup_at = 0.0; Float stand_yaw = 0.0; Vector stand_feet = Vector{0.0, 0.0, 0.0}; I32 group = 0; };
  KERN_STRUCT(Ragdolls) { Int world_ = 0; List<Ragdolls_Doll> dolls_ = List<Ragdolls_Doll>{}; List<Ragdolls_Up> ups_ = List<Ragdolls_Up>{}; U32 max_ = 10; U32 spawned_ = 0; I32 group_ = 0; };

  // ---- functions ----
  /// MAX_RAGDOLLS
  void Ragdolls_init(Ragdolls& self, Int w = 0, Bool mobile = false) {
    self.world_ = w;
    self.max_ = kern::to<Int>(kern::formula([](Float mobile) -> Float { return ((mobile != 0.0) ? 6.0 : 10.0); }, kern::to<Float>(mobile)));
  }

  void Ragdolls_take_ups(Ragdolls& self, List<Ragdolls_Up>& out) {
    Int place = 0;
    place = list_length<Ragdolls_Up>(out);
    list_insert_list(out, place, self.ups_);
    list_clear(self.ups_);
  }

  struct Ragdolls_spawnedResult { U32 Result = 0; };
  KERN_PURE Ragdolls_spawnedResult Ragdolls_spawned(const Ragdolls& self) {
    return {self.spawned_};
  }
};
