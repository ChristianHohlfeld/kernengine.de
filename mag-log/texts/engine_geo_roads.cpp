struct engine_geo_roads : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(GeoProjector) { Float lon0 = 0.0; Float lat0 = 0.0; Float cos_lat = 1.0; };
};
