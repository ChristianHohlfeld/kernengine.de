struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Float_2_row) { Fixed<Float, 2> items = List<Float>{}; };
  KERN_STRUCT(Float_3_row) { Fixed<Float, 3> items = List<Float>{}; };
  KERN_STRUCT(Float_4_row) { Fixed<Float, 4> items = List<Float>{}; };
  KERN_STRUCT(Float_5_row) { Fixed<Float, 5> items = List<Float>{}; };
  KERN_STRUCT(Float_Int64_pair) { Float first = 0.0; Int64 second = Int64{0x0}; };
  KERN_STRUCT(Float_Int_pair) { Float first = 0.0; Int second = 0; };
  KERN_STRUCT(Float_Vector_pair) { Float first = 0.0; Vector second = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(Int64_Int_pair) { Int64 first = Int64{0x0}; Int second = 0; };
  KERN_STRUCT(Int_4_row) { Fixed<Int, 4> items = List<Int>{}; };
  KERN_STRUCT(Int_Bool_pair) { Int first = 0; Bool second = false; };
  KERN_STRUCT(Int_Float_pair) { Int first = 0; Float second = 0.0; };
  KERN_STRUCT(Int_Int_pair) { Int first = 0; Int second = 0; };
  KERN_STRUCT(Int_row) { List<Int> items = List<Int>{}; };
  KERN_STRUCT(Text_6_row) { Fixed<Text, 6> items = List<Text>{}; };
  KERN_STRUCT(Vector_Float_pair) { Vector first = Vector{0.0, 0.0, 0.0}; Float second = 0.0; };
  KERN_STRUCT(Vector_row) { List<Vector> items = List<Vector>{}; };
  KERN_STRUCT(Vector_row_row) { List<Vector_row> items = List<Vector_row>{}; };
};
