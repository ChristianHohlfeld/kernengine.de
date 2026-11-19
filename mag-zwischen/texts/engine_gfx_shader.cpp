struct engine_gfx_shader : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(ExtensionAttribute) { Text key = ""; Int size = 0; Text glsl = ""; };
};
