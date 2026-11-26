struct engine_gfx_shader : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(ExtensionAttribute) { Text key = ""; U32 size = 0; Text glsl = ""; };
};
