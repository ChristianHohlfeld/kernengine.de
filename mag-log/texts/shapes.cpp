struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Float_4_row) { Fixed<Float, 4> items = List<Float>{}; };
  KERN_STRUCT(Float_5_row) { Fixed<Float, 5> items = List<Float>{}; };
  KERN_STRUCT(Int_Bool_pair) { Int first = 0; Bool second = false; };
  KERN_STRUCT(Int_Int_pair) { Int first = 0; Int second = 0; };
  KERN_STRUCT(Vector_Float_pair) { Vector first = Vector{0.0, 0.0, 0.0}; Float second = 0.0; };
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
};
