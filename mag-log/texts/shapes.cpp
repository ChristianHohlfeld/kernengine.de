struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Float_Int64_pair) { Float first = 0.0; Int64 second = Int64{0x0}; };
  KERN_STRUCT(Int64_Int_pair) { Int64 first = Int64{0x0}; Int second = 0; };
  KERN_STRUCT(Int_Float_pair) { Int first = 0; Float second = 0.0; };
  KERN_STRUCT(Int_Int_pair) { Int first = 0; Int second = 0; };
  KERN_STRUCT(Int_row) { List<Int> items = List<Int>{}; };
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
};
