struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Float_4_row) { Fixed<Float, 4> items = List<Float>{}; };
  KERN_STRUCT(Float_5_row) { Fixed<Float, 5> items = List<Float>{}; };
  KERN_STRUCT(Int64_Int_pair) { Int64 first = Int64{0x0}; Int second = 0; };
  KERN_STRUCT(Int_Bool_pair) { Int first = 0; Bool second = false; };
  KERN_STRUCT(Int_Float_pair) { Int first = 0; Float second = 0.0; };
  KERN_STRUCT(Int_Int_pair) { Int first = 0; Int second = 0; };
  KERN_STRUCT(Int_row) { List<Int> items = List<Int>{}; };
  KERN_STRUCT(Text_Int_pair) { Text first = ""; Int second = 0; };
  KERN_STRUCT(Text_Text_pair) { Text first = ""; Text second = ""; };
  KERN_STRUCT(Vector_Float_pair) { Vector first = Vector{0.0, 0.0, 0.0}; Float second = 0.0; };
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
};
