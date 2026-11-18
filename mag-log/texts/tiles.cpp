struct tiles : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_ENUM(AreaKind, water, park, wood, grass, sand, pitch);
  KERN_STRUCT(MapFeatures_Spot) { Vector p = Vector{0.0, 0.0, 0.0}; Float a = 0.0; };
  KERN_ENUM(RoadClass, motorway, trunk, primary, secondary, tertiary, minor, service, track, path, pedestrian, river, canal, stream);
  KERN_STRUCT(TileKey) { Int z = 14; Int x = 0; Int y = 0; };
  KERN_STRUCT(MapFeatures_Area) { List<Vector_row> rings = List<Vector_row>{}; AreaKind kind = AreaKind::park; };
  KERN_STRUCT(MapFeatures_Building) { List<Vector_row> rings = List<Vector_row>{}; Float base = 0.0; Float top = 8.0; Int seed = 0; Float full_top = 0.0; Bool leveled = false; };
  KERN_STRUCT(MapFeatures_Road) { List<Vector> points = List<Vector>{}; RoadClass cls = RoadClass::minor; Bool bridge = false; };
  KERN_STRUCT(MapFeatures) { List<MapFeatures_Building> buildings = List<MapFeatures_Building>{}; List<MapFeatures_Road> roads = List<MapFeatures_Road>{}; List<MapFeatures_Area> areas = List<MapFeatures_Area>{}; List<MapFeatures_Spot> trees = List<MapFeatures_Spot>{}; List<MapFeatures_Spot> lamps = List<MapFeatures_Spot>{}; Bool procedural = false; };
};
