struct engine_geo_roads : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(GeoProjector) { Float lon0 = 0.0; Float lat0 = 0.0; Float cos_lat = 1.0; };

  // ---- variables ----
  KERN_CONST Float kRad = 0.017453292519943295;
  KERN_CONST Float kEarthRadius = 6378137.0;

  // ---- functions ----
  struct GeoProjector_initResult { Bool Result = false; };
  GeoProjector_initResult GeoProjector_init(GeoProjector& self, Float lon = 0.0, Float lat = 0.0) {
    Bool fail_text = false;
    if (logic_or(logic_or(logic_or(logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, lon), 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, lat), 0.0))), math_compare<">">(kern::formula([](Float lon) -> Float { return kern::math::abs(lon); }, lon), 180.0)), math_compare<">">(kern::formula([](Float lat) -> Float { return kern::math::abs(lat); }, lat), 85.0))) {
      engine_fail_text(n10, "invalid projection origin");
      fail_text = n10.result;
      return {fail_text};
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
