struct engine_geo_roads : virtual kern::Library, virtual engine_geo_buildings {
  // ---- types ----
  KERN_STRUCT(GeoProjector) { Float lon0 = 0.0; Float lat0 = 0.0; Float cos_lat = 1.0; };
  KERN_STRUCT(RoadOptions) { Float center_x = 0.0; Float center_y = 0.0; Float radius = 800.0; Float chunk_size = 420.0; Float segment_length = 6.0; Float curb_extra = 3.4; Float curb_z = 0.09; Float surface_z = 0.112; Float marking_z = 0.135; Float marking_width = 0.42; Float dash_start = 1.0; Float dash_length = 3.0; Float dash_period = 7.0; Float uv_scale = 0.2; Rgb curb_color = Rgb{0.525, 0.514, 0.486}; List<Rgb> palette = List<Rgb>{}; Callback<Bool(Float, Float, Float)> deformed = Callback<Bool(Float, Float, Float)>{}; Float fine_step = 0.9; };

  // ---- variables ----
  KERN_CONST Float kRad = 0.017453292519943295;
  KERN_CONST Float kEarthRadius = 6378137.0;

  // ---- functions ----
  struct GeoProjector_initResult { Bool Result = false; };
  GeoProjector_initResult GeoProjector_init(GeoProjector& self, Float lon = 0.0, Float lat = 0.0) {
    Bool fail = false;
    if (logic_or(logic_or(logic_or(logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, lon), 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, lat), 0.0))), math_compare<">">(kern::formula([](Float lon) -> Float { return kern::math::abs(lon); }, lon), 180.0)), math_compare<">">(kern::formula([](Float lat) -> Float { return kern::math::abs(lat); }, lat), 85.0))) {
      engine_fail(n10, text_format<"invalid projection origin">(-1));
      fail = n10.result;
      return {fail};
    }
    self.lon0 = lon;
    self.lat0 = lat;
    self.cos_lat = kern::formula([](Float lat, Float kRad) -> Float { return kern::math::max(0.01, kern::math::cos((lat * kRad))); }, lat, kRad);
    return {true};
  }

  void GeoProjector_project(const GeoProjector& self, Float& x, Float& y, Float lon = 0.0, Float lat = 0.0) {
    x = kern::formula([](Float lon, Float lon0, Float kRad, Float kEarthRadius, Float cos_lat) -> Float { return ((((lon - lon0) * kRad) * kEarthRadius) * cos_lat); }, lon, self.lon0, kRad, kEarthRadius, self.cos_lat);
    y = kern::formula([](Float lat, Float lat0, Float kRad, Float kEarthRadius) -> Float { return (((lat - lat0) * kRad) * kEarthRadius); }, lat, self.lat0, kRad, kEarthRadius);
  }

  // ---- what nodes give and keep ----
  struct { Bool result = false; } n10;
};
