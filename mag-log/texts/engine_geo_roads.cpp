struct engine_geo_roads : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(GeoProjector) { Float lon0 = 0.0; Float lat0 = 0.0; Float cos_lat = 1.0; };

  // ---- variables ----
  KERN_CONST Float kRad = 0.017453292519943295;
  KERN_CONST Float kEarthRadius = 6378137.0;

  // ---- functions ----
  void GeoProjector_project(const GeoProjector& self, Float& x, Float& y, Float lon = 0.0, Float lat = 0.0) {
    x = kern::formula([](Float lon, Float lon0, Float kRad, Float kEarthRadius, Float cos_lat) -> Float { return ((((lon - lon0) * kRad) * kEarthRadius) * cos_lat); }, lon, self.lon0, kRad, kEarthRadius, self.cos_lat);
    y = kern::formula([](Float lat, Float lat0, Float kRad, Float kEarthRadius) -> Float { return (((lat - lat0) * kRad) * kEarthRadius); }, lat, self.lat0, kRad, kEarthRadius);
  }
};
