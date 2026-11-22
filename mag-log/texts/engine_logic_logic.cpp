struct engine_logic_logic : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_STRUCT(logic_CommentBox) { Int id = 0; Float x = 0.0; Float y = 0.0; Float w = 400.0; Float h = 200.0; Text text = "Comment"; Int color = 0; };
  KERN_STRUCT(logic_EnumType) { Text name = ""; List<Text> values = List<Text>{}; Text help = ""; Bool project = false; Text from = ""; };
  KERN_STRUCT(logic_Function) { Text name = ""; List<Int> inputs = List<Int>{}; List<Int> outputs = List<Int>{}; List<Int> locals = List<Int>{}; Bool pure = false; Bool external = false; Text remote = ""; Text from = ""; Text help = ""; };
  KERN_STRUCT(logic_Link) { Int from = 0; Text out = ""; Int to = 0; Text in = ""; };
  KERN_STRUCT(logic_State) { Int id = 0; Text name = ""; Float x = 0.0; Float y = 0.0; Bool initial = false; };
  KERN_STRUCT(logic_StructType) { Text name = ""; List<Int> fields = List<Int>{}; Text help = ""; Bool project = false; Text from = ""; };
  KERN_ENUM(logic_TriggerKind, event, timer, condition, automatic);
  KERN_STRUCT(logic_Node) { Int id = 0; Text type = ""; Float x = 0.0; Float y = 0.0; List<Text_Int_pair> values = List<Text_Int_pair>{}; List<Text_Text_pair> params = List<Text_Text_pair>{}; Text comment = ""; };
  KERN_STRUCT(logic_Transition) { Int id = 0; Int from = 0; Int to = 0; logic_TriggerKind trigger = logic_TriggerKind::event; Text event = ""; Float seconds = 1.0; Text variable = ""; Text label = ""; };
  KERN_STRUCT(logic_StateMachine) { Int id = 0; Text name = "State Machine"; List<logic_State> states = List<logic_State>{}; List<logic_Transition> transitions = List<logic_Transition>{}; };
  KERN_STRUCT(logic_Graph) { List<logic_Node> nodes = List<logic_Node>{}; List<logic_Link> links = List<logic_Link>{}; List<Int> variables = List<Int>{}; List<logic_StateMachine> machines = List<logic_StateMachine>{}; List<logic_Function> functions = List<logic_Function>{}; List<logic_EnumType> enums = List<logic_EnumType>{}; List<logic_StructType> structs = List<logic_StructType>{}; List<logic_CommentBox> comments = List<logic_CommentBox>{}; Int next_id = 1; Bool library = false; List<Text> uses = List<Text>{}; };
};
