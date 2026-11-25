struct mvt : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_ENUM(mvt_GeomType, unknown, point, line, polygon);
  KERN_STRUCT(mvt_Piece) { Int at = 0; Int end = 0; };
  KERN_ENUM(mvt_Value_Kind, none, string, number, boolean);
  KERN_STRUCT(mvt_Feature) { Int64 id = Int64{0x0}; mvt_GeomType type = mvt_GeomType::unknown; List<Int_Int_pair> tags = List<Int_Int_pair>{}; List<Vector_row> parts = List<Vector_row>{}; };
  KERN_STRUCT(mvt_Value) { mvt_Value_Kind kind = mvt_Value_Kind::none; Text s = ""; Float n = 0.0; Bool b = false; };
  KERN_STRUCT(Text_mvt_Value_pair) { Text first = ""; mvt_Value second = mvt_Value{mvt_Value_Kind::none, "", 0.0, false}; };
  KERN_STRUCT(mvt_Layer) { Text name = ""; U32 extent = 4096; List<Text> keys = List<Text>{}; List<mvt_Value> values = List<mvt_Value>{}; List<mvt_Feature> features = List<mvt_Feature>{}; };
  KERN_STRUCT(mvt_Writer_PendingLayer) { Text name = ""; U32 extent = 4096; List<Text> keys = List<Text>{}; List<mvt_Value> values = List<mvt_Value>{}; List<U8_row> features = List<U8_row>{}; };
  KERN_STRUCT(mvt_Tile) { List<mvt_Layer> layers = List<mvt_Layer>{}; };
  KERN_STRUCT(mvt_Writer) { mvt_Writer_PendingLayer layer_ = mvt_Writer_PendingLayer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<U8_row>{}}; List<U8> out_ = List<U8>{}; };
  KERN_STRUCT(mvt_Decoder) { List<mvt_Piece> layers_ = List<mvt_Piece>{}; List<mvt_Piece> features_ = List<mvt_Piece>{}; Int layer_ = 0; Int feature_ = 0; Bool layer_open_ = false; Bool done_ = true; Bool failed_ = false; mvt_Tile tile_ = mvt_Tile{List<mvt_Layer>{}}; };

  // ---- functions ----
  struct Decoder_doneResult { Bool Result = false; };
  KERN_PURE Decoder_doneResult Decoder_done(const mvt_Decoder& self) {
    return {self.done_};
  }

  struct Decoder_failedResult { Bool Result = false; };
  KERN_PURE Decoder_failedResult Decoder_failed(const mvt_Decoder& self) {
    return {self.failed_};
  }
};
