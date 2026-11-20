  // A place of the engine (Graph::Root::bound, setting "bound"): the node of the engine binds it when it runs, the
  // nodes of places name it - read whole, an entry set, added to, its length (kern/docs/blueprint_cpp.md, "Plätze der Engine").
  {
    const u32 e = add("event.custom", {{"name", "Bound"}});
    const u32 b = add("engine.game_world.events");
    g.find(b)->set_value("game_world", json::Value(3));
    wire(e, "then", b, first_exec(b, true));
    const std::string at = std::to_string(b);
    const u32 a = add("list.add", {{"bound", at}});
    g.find(a)->set_value("item", json::Value("x \"y\""));
    wire(b, first_exec(b, false), a, first_exec(a, true));
    const u32 s = add("var.set", {{"bound", at}, {"path", "[]"}});
    g.find(s)->set_value("path0", json::Value(1));
    g.find(s)->set_value("value", json::Value("z"));
    wire(a, first_exec(a, false), s, first_exec(s, true));
    const u32 r = add("var.get", {{"bound", at}});
    const u32 l = add("list.length", {{"type", "text"}});
    wire(r, "value", l, "list");
    const u32 v = add("var.set", {{"variable", "VI"}});
    wire(l, "length", v, "value");
    wire(s, first_exec(s, false), v, first_exec(v, true));
  }
