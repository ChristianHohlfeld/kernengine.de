struct mvt : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_ENUM(mvt_GeomType, unknown, point, line, polygon);
  KERN_STRUCT(mvt_Piece) { u64 at = 0; u64 end = 0; };
  KERN_ENUM(mvt_Value_Kind, none, string, number, boolean);
  KERN_STRUCT(mvt_Feature) { u64 id = 0; mvt_GeomType type = mvt_GeomType::unknown; List<u32_u32_pair> tags = List<u32_u32_pair>{}; List<Vector_row> parts = List<Vector_row>{}; };
  KERN_STRUCT(mvt_Value) { mvt_Value_Kind kind = mvt_Value_Kind::none; Text s = ""; f64 n = 0.0; bool b = false; };
  KERN_STRUCT(Text_mvt_Value_pair) { Text first = ""; mvt_Value second = mvt_Value{mvt_Value_Kind::none, "", 0.0, false}; };
  KERN_STRUCT(mvt_Layer) { Text name = ""; u32 extent = 4096; List<Text> keys = List<Text>{}; List<mvt_Value> values = List<mvt_Value>{}; List<mvt_Feature> features = List<mvt_Feature>{}; };
  KERN_STRUCT(mvt_Writer_PendingLayer) { Text name = ""; u32 extent = 4096; List<Text> keys = List<Text>{}; List<mvt_Value> values = List<mvt_Value>{}; List<u8_row> features = List<u8_row>{}; };
  KERN_STRUCT(mvt_Tile) { List<mvt_Layer> layers = List<mvt_Layer>{}; };
  KERN_STRUCT(mvt_Writer) { mvt_Writer_PendingLayer layer_ = mvt_Writer_PendingLayer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<u8_row>{}}; List<u8> out_ = List<u8>{}; };
  KERN_STRUCT(mvt_Decoder) { List<mvt_Piece> layers_ = List<mvt_Piece>{}; List<mvt_Piece> features_ = List<mvt_Piece>{}; u64 layer_ = 0; u64 feature_ = 0; bool layer_open_ = false; bool done_ = true; bool failed_ = false; mvt_Tile tile_ = mvt_Tile{List<mvt_Layer>{}}; };

  // ---- functions ----
  struct Decoder_doneResult { bool Result = false; };
  KERN_PURE Decoder_doneResult Decoder_done(const mvt_Decoder& self) {
    return {self.done_};
  }

  struct Decoder_failedResult { bool Result = false; };
  KERN_PURE Decoder_failedResult Decoder_failed(const mvt_Decoder& self) {
    return {self.failed_};
  }
};
