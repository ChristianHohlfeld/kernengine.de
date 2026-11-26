struct interiors : virtual kern::Library, virtual engine_geo_mesh_data, virtual engine_physics_physics, virtual shapes {
  // ---- types ----
  KERN_STRUCT(Interiors_Closed) { Vector at = Vector{0.0, 0.0, 0.0}; Float yaw = 0.0; };
  KERN_STRUCT(Interiors_Core) { Vector o = Vector{0.0, 0.0, 0.0}; Vector u = Vector{0.0, 0.0, 0.0}; Vector v = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(Interiors_Door) { Vector c = Vector{0.0, 0.0, 0.0}; Vector d = Vector{0.0, 0.0, 0.0}; Vector n = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(Float_Interiors_Closed_pair) { Float first = 0.0; Interiors_Closed second = Interiors_Closed{Vector{0.0, 0.0, 0.0}, 0.0}; };
  KERN_STRUCT(Interiors_Plan) { List<Vector> outer = List<Vector>{}; List<Vector> inner = List<Vector>{}; List<Vector_row> holes = List<Vector_row>{}; List<Vector_row> inner_holes = List<Vector_row>{}; List<Interiors_Door> doors = List<Interiors_Door>{}; List<Float> levels = List<Float>{}; Float base = 0.0; Float top = 8.0; Float floor_h = 3.3; Float area = 0.0; Bool has_core = false; Bool roof_access = false; Interiors_Core core = Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; MeshData glass = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; List<Vector> points = List<Vector>{}; List<U32> indices = List<U32>{}; List<Vector> lamps = List<Vector>{}; U32 furniture = 0; };
  KERN_STRUCT(Interiors) { Object<kern::Renderer> r_ = Object<kern::Renderer>{}; Object<kern::physics::World> world_ = Object<kern::physics::World>{}; Bool active_ = false; Int64 tile_ = Int64{0x0}; U32 index_ = 0; Interiors_Plan plan_ = Interiors_Plan{List<Vector>{}, List<Vector>{}, List<Vector_row>{}, List<Vector_row>{}, List<Interiors_Door>{}, List<Float>{}, 0.0, 8.0, 3.3, 0.0, false, false, Interiors_Core{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}, List<Vector>{}, List<U32>{}, List<Vector>{}, 0}; U32 mesh_id_ = 0; U32 glass_id_ = 0; physics_Body body_ = physics_Body{Int64{0x0}}; List<Float> door_angle_ = List<Float>{}; List<Interiors_Closed> closed_ = List<Interiors_Closed>{}; Float next_scan_ = 0.0; Vector feet_ = Vector{0.0, 0.0, 0.0}; U32 entered_ = 0; U32 built_ = 0; Bool was_inside_ = false; };

  // ---- functions ----
  void Interiors_init(Interiors& self, Object<kern::Renderer> r = Object<kern::Renderer>{}, Object<kern::physics::World> w = Object<kern::physics::World>{}) {
    self.r_ = r;
    self.world_ = w;
  }

  struct Interiors_activeResult { Bool Result = false; };
  KERN_PURE Interiors_activeResult Interiors_active(const Interiors& self) {
    return {self.active_};
  }
};
