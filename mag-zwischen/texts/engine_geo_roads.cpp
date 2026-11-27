struct engine_geo_roads : virtual kern::Library, virtual engine_geo_buildings {
  // ---- types ----
  KERN_STRUCT(GeoProjector) { f64 lon0 = 0.0; f64 lat0 = 0.0; f64 cos_lat = 1.0; };
  KERN_STRUCT(RoadOptions) { f64 center_x = 0.0; f64 center_y = 0.0; f64 radius = 800.0; f64 chunk_size = 420.0; f64 segment_length = 6.0; f64 curb_extra = 3.4; f64 curb_z = 0.09; f64 surface_z = 0.112; f64 marking_z = 0.135; f64 marking_width = 0.42; f64 dash_start = 1.0; f64 dash_length = 3.0; f64 dash_period = 7.0; f64 uv_scale = 0.2; Rgb curb_color = Rgb{0.525, 0.514, 0.486}; List<Rgb> palette = List<Rgb>{}; Callback<bool(f64, f64, f64)> deformed = Callback<bool(f64, f64, f64)>{}; f64 fine_step = 0.9; };
};
