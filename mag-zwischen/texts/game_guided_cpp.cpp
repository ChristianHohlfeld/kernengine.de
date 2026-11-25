struct game_guided_cpp : virtual kern::Library, virtual people {
  // ---- functions ----
  struct person_upResult { Bool Result = false; };
  KERN_PURE person_upResult person_up(const People_Person& p) {
    return {logic_and(logic_not(enum_equal<People_State>(p.state, People_State::down)), logic_not(enum_equal<People_State>(p.state, People_State::gone)))};
  }

  struct person_labelResult { Text Result = ""; };
  KERN_PURE person_labelResult person_label(const People_Person& p) {
    switch (flow_switch_enum<People_Kind>(p.kind)) {
      case People_Kind::citizen: {
        return {"PERSON"};
      } break;
      case People_Kind::officer: {
        return {"POLIZIST"};
      } break;
      case People_Kind::soldier: {
        return {"SOLDAT"};
      } break;
      case People_Kind::zombie: {
        return {"ZOMBIE"};
      } break;
      case People_Kind::enemy: {
        return {"GEGNER"};
      } break;
    }
  }
};
