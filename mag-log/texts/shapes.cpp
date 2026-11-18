struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Int_Bool_pair) { Int first = 0; Bool second = false; };
  KERN_STRUCT(Int_Float_pair) { Int first = 0; Float second = 0.0; };
  KERN_STRUCT(Int_row) { List<Int> items = List<Int>{}; };
};
