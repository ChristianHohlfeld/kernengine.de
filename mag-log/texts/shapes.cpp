struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Int_Int_pair) { Int first = 0; Int second = 0; };
  KERN_STRUCT(Int_row) { List<Int> items = List<Int>{}; };
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
};
