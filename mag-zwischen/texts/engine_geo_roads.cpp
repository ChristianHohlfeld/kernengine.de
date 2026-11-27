struct engine_geo_roads : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(GeoProjector) { f64 lon0 = 0.0; f64 lat0 = 0.0; f64 cos_lat = 1.0; };
};
