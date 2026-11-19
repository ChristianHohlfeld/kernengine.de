struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Int64_Int_pair) { Int64 first = Int64{0x0}; Int second = 0; };
  KERN_STRUCT(Int_Bool_pair) { Int first = 0; Bool second = false; };
  KERN_STRUCT(Int_Float_pair) { Int first = 0; Float second = 0.0; };
  KERN_STRUCT(Int_Int_pair) { Int first = 0; Int second = 0; };
  KERN_STRUCT(Text_Int_pair) { Text first = ""; Int second = 0; };
  KERN_STRUCT(Text_Text_pair) { Text first = ""; Text second = ""; };
  KERN_STRUCT(U8_row) { List<U8> items = List<U8>{}; };
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
};
