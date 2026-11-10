// Kern Logic: graphs to machine code at run time (the JIT).
//
// compile_wasm() writes a WebAssembly module that the host compiles with the
// browser's optimising compiler and links into the engine's function table:
// the graph then runs as native code, calling the engine's C++ node functions
// directly (wasm-to-wasm calls, no script engine in between). This is for a
// graph that is edited and played at once; a game's build compiles each
// Blueprint as its own C++ class instead (docs/blueprint_cpp.md).
//
// What is generated inline: control flow (Branch, Sequence, For / While,
// Do Once, Do N, Gate, Flip Flop), numeric variables (Get / Set / Add), maths,
// comparisons and boolean logic. Everything else (gameplay, text, vectors,
// latent nodes) is a call to the node's C++ function. Shared state (variables,
// loop counters, gates) lives in the instance's slots, so the interpreter and
// the JIT can take over from each other at any time.
//
// The program is a loop over a program counter (one case per node), which
// keeps any exec wiring (merges, cycles) valid; continuations of Sequence and
// loops go onto the instance's stack, exactly as in the interpreter.
#include <cmath>
#include <cstdio>
#include <cstring>

#include "kern/logic/logic.h"

namespace kern::logic {

namespace {

constexpr int kResume = Context::kResume;

// ---------------------------------------------------------------------------
// WebAssembly encoding

struct Bytes {
  std::vector<u8> b;
  void byte(u8 v) { b.push_back(v); }
  void uleb(u64 v) {
    do {
      u8 x = v & 0x7f;
      v >>= 7;
      if (v) x |= 0x80;
      b.push_back(x);
    } while (v);
  }
  void sleb(i64 v) {
    bool more = true;
    while (more) {
      u8 x = v & 0x7f;
      v >>= 7;
      if ((v == 0 && !(x & 0x40)) || (v == -1 && (x & 0x40))) more = false;
      else x |= 0x80;
      b.push_back(x);
    }
  }
  void f64v(f64 v) {
    u8 raw[8];
    std::memcpy(raw, &v, 8);
    for (u8 x : raw) b.push_back(x);
  }
  void name(const char* s) {
    uleb(std::strlen(s));
    for (const char* p = s; *p; ++p) b.push_back(static_cast<u8>(*p));
  }
  void append(const Bytes& o) { b.insert(b.end(), o.b.begin(), o.b.end()); }
  void section(u8 id, const Bytes& content) {
    byte(id);
    uleb(content.b.size());
    append(content);
  }
};

enum Op : u8 {
  kBlock = 0x02, kLoop = 0x03, kIf = 0x04, kElse = 0x05, kEnd = 0x0b, kBr = 0x0c, kBrTable = 0x0e, kReturn = 0x0f, kCall = 0x10, kSelect = 0x1b,
  kLocalGet = 0x20, kLocalSet = 0x21, kLocalTee = 0x22,
  kI32Load = 0x28, kF64Load = 0x2b, kI32Store = 0x36, kF64Store = 0x39,
  kI32Const = 0x41, kF64Const = 0x44,
  kI32Eqz = 0x45, kI32Eq = 0x46, kI32Ne = 0x47, kI32LtS = 0x48, kI32GtS = 0x4a, kI32LeS = 0x4c, kI32GeS = 0x4e, kI32GeU = 0x4f,
  kF64Eq = 0x61, kF64Ne = 0x62, kF64Lt = 0x63, kF64Gt = 0x64, kF64Le = 0x65, kF64Ge = 0x66,
  kI32Add = 0x6a, kI32Sub = 0x6b, kI32And = 0x71, kI32Or = 0x72, kI32Xor = 0x73, kI32Shl = 0x74,
  kF64Abs = 0x99, kF64Neg = 0x9a, kF64Floor = 0x9c, kF64Trunc = 0x9d, kF64Sqrt = 0x9f,
  kF64Add = 0xa0, kF64Sub = 0xa1, kF64Mul = 0xa2, kF64Div = 0xa3, kF64Min = 0xa4, kF64Max = 0xa5, kF64Copysign = 0xa6,
  kF64ConvertI32U = 0xb8,
};
constexpr u8 kVoid = 0x40, kTypeI32 = 0x7f, kTypeF64 = 0x7c;

// Locals of run(pc, entry).
constexpr u32 kPc = 0, kEntry = 1, kBase = 2, kR = 3, kSp = 4, kF0 = 5, kF1 = 6, kF2 = 7, kF3 = 8;
// Imported functions.
constexpr u32 kFnCallNode = 0, kFnReadPin = 1, kFnTrace = 2;

class WasmGen {
 public:
  explicit WasmGen(const Instance& inst) : in_(const_cast<Instance&>(inst)) {
    slots_ = reinterpret_cast<uintptr_t>(in_.slots());
    stack_ = reinterpret_cast<uintptr_t>(in_.stack());
    table_ = reinterpret_cast<uintptr_t>(in_.exec_table());
    counters_ = reinterpret_cast<uintptr_t>(in_.counters());
  }

  std::vector<u8> module() {
    Bytes code;
    body(code);
    Bytes m;
    for (u8 x : {0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00}) m.byte(x);
    {  // types
      Bytes t;
      t.uleb(4);
      auto fn = [&](std::initializer_list<u8> params, std::initializer_list<u8> results) {
        t.byte(0x60);
        t.uleb(params.size());
        for (u8 p : params) t.byte(p);
        t.uleb(results.size());
        for (u8 r : results) t.byte(r);
      };
      fn({kTypeI32, kTypeI32}, {kTypeI32});            // 0 run
      fn({kTypeI32, kTypeI32, kTypeI32}, {kTypeI32});  // 1 call_node
      fn({kTypeI32, kTypeI32, kTypeI32}, {kTypeF64});  // 2 read_pin
      fn({kTypeI32, kTypeI32}, {});                    // 3 trace
      m.section(1, t);
    }
    {  // imports
      Bytes im;
      im.uleb(4);
      auto func = [&](const char* name, u32 type) {
        im.name("env");
        im.name(name);
        im.byte(0x00);
        im.uleb(type);
      };
      func("call_node", 1);
      func("read_pin", 2);
      func("trace", 3);
      im.name("env");
      im.name("memory");
      im.byte(0x02);
      im.byte(0x00);  // limits: min only
      im.uleb(1);
      m.section(2, im);
    }
    {  // functions
      Bytes f;
      f.uleb(1);
      f.uleb(0);
      m.section(3, f);
    }
    {  // exports
      Bytes e;
      e.uleb(1);
      e.name("run");
      e.byte(0x00);
      e.uleb(3);
      m.section(7, e);
    }
    {  // code
      Bytes c;
      c.uleb(1);
      Bytes fn;
      fn.uleb(3);  // local groups
      fn.uleb(3);
      fn.byte(kTypeI32);  // base, r, sp
      fn.uleb(4);
      fn.byte(kTypeF64);  // f0..f3
      fn.uleb(0);
      fn.byte(kTypeI32);
      fn.append(code);
      c.uleb(fn.b.size());
      c.append(fn);
      m.section(10, c);
    }
    return m.b;
  }

 private:
  Instance& in_;
  uintptr_t slots_, stack_, table_, counters_;
  Bytes* o_ = nullptr;

  void op(u8 x) { o_->byte(x); }
  void i32c(i64 v) { op(kI32Const); o_->sleb(static_cast<i32>(v)); }
  void f64c(f64 v) { op(kF64Const); o_->f64v(v); }
  void lget(u32 l) { op(kLocalGet); o_->uleb(l); }
  void lset(u32 l) { op(kLocalSet); o_->uleb(l); }
  void ltee(u32 l) { op(kLocalTee); o_->uleb(l); }
  void mem(u8 o, u32 align, u32 offset) { op(o); o_->uleb(align); o_->uleb(offset); }
  void call(u32 f) { op(kCall); o_->uleb(f); }
  void br(u32 depth) { op(kBr); o_->uleb(depth); }
  void if_void() { op(kIf); op(kVoid); }
  void ret_i32(i32 v) { i32c(v); op(kReturn); }

  u32 slot_addr(int slot) const { return static_cast<u32>(slots_ + static_cast<uintptr_t>(slot) * 8); }
  void load_slot(int slot) { i32c(slot_addr(slot)); mem(kF64Load, 3, 0); }
  // value on the stack -> slot (uses kF0)
  void store_slot_from_f0(int slot) { i32c(slot_addr(slot)); lget(kF0); mem(kF64Store, 3, 0); }
  void bool_of_f64() { f64c(0); op(kF64Ne); }  // f64 -> i32 0/1
  void to_f64_from_bool() { op(kF64ConvertI32U); }
  void convert(PinType t) {
    if (t == PinType::integer) {
      // A whole number as Int holds it (logic::whole): toward zero; not a number or endless: 0;
      // beyond Int: its nearest end. (kF3 is this conversion's own.)
      op(kF64Trunc);
      lset(kF3);
      lget(kF3); f64c(0);
      lget(kF3); lget(kF3); op(kF64Sub); f64c(0); op(kF64Eq);  // finite: x - x is 0
      op(kSelect);
      f64c(-9223372036854775808.0); op(kF64Max);
      f64c(9223372036854775808.0); op(kF64Min);
    }
    else if (t == PinType::boolean) { bool_of_f64(); to_f64_from_bool(); }
  }

  // ---- values ----
  /// Input `k` of node `node` as an f64 of the input's type.
  void value(int node, int k, int depth) {
    const PinDef& pin = in_.inputs_at(node)[static_cast<std::size_t>(k)];
    const Instance::SourceRef s = in_.source_of(node, k);
    if (s.node < 0) {
      f64c(in_.literal_of(node, k).as_number());
      return;
    }
    if (depth < 24 && in_.inline_pure(s.node)) {
      // (a value that is an Int - Round, Floor - is one as the node gives it, whatever reads it)
      const PinType gives = in_.outputs_at(s.node)[static_cast<std::size_t>(s.pin)].type;
      pure(s.node, s.pin, depth + 1);
      if (gives == PinType::integer) convert(gives);
      if (pin.type != gives) convert(pin.type);
      return;
    }
    if (const int slot = in_.out_slot(s.node, s.pin); slot >= 0 && in_.def_at(s.node)->kind != NodeKind::pure) {
      load_slot(slot);
      convert(pin.type);
      return;
    }
    // Anything else: the C++ side evaluates the input (events' outputs, text, vectors...).
    i32c(in_.id());
    i32c(node);
    i32c(k);
    call(kFnReadPin);
  }

  int input_index(int node, const char* id) const {
    const auto& ins = in_.inputs_at(node);
    for (std::size_t i = 0; i < ins.size(); ++i) if (ins[i].id == id) return static_cast<int>(i);
    return -1;
  }
  void value(int node, const char* id, int depth) {
    const int k = input_index(node, id);
    if (k < 0) f64c(0);
    else value(node, k, depth);
  }

  void pure(int node, int pin, int depth) {
    (void)pin;
    const std::string& t = in_.def_at(node)->type;
    auto two = [&](const char* a, const char* b) {
      value(node, a, depth);
      value(node, b, depth);
      lset(kF1);
      lset(kF0);
    };
    if (t == "var.get") { load_slot(in_.var_slot(in_.node_at(node).param("variable"))); return; }
    if (t == "value.float" || t == "value.int" || t == "value.bool") { value(node, "value", depth); return; }
    if (t == "math.add") { value(node, "a", depth); value(node, "b", depth); op(kF64Add); return; }
    if (t == "math.sub") { value(node, "a", depth); value(node, "b", depth); op(kF64Sub); return; }
    if (t == "math.mul") { value(node, "a", depth); value(node, "b", depth); op(kF64Mul); return; }
    if (t == "math.min") { value(node, "a", depth); value(node, "b", depth); op(kF64Min); return; }
    if (t == "math.max") { value(node, "a", depth); value(node, "b", depth); op(kF64Max); return; }
    if (t == "math.abs") { value(node, "a", depth); op(kF64Abs); return; }
    if (t == "math.negate") { value(node, "a", depth); op(kF64Neg); return; }
    if (t == "math.sqrt") { value(node, "a", depth); f64c(0); op(kF64Max); op(kF64Sqrt); return; }
    if (t == "math.floor") { value(node, "a", depth); op(kF64Floor); return; }
    if (t == "math.round") {  // llround: half away from zero
      value(node, "a", depth);
      lset(kF0);
      lget(kF0); op(kF64Abs); f64c(0.5); op(kF64Add); op(kF64Floor); lget(kF0); op(kF64Copysign);
      return;
    }
    if (t == "math.div" || t == "math.mod") {
      two("a", "b");
      if (t == "math.div") { lget(kF0); lget(kF1); op(kF64Div); }
      else { lget(kF0); lget(kF1); lget(kF0); lget(kF1); op(kF64Div); op(kF64Trunc); op(kF64Mul); op(kF64Sub); }
      f64c(0);
      lget(kF1); f64c(0); op(kF64Ne);
      op(kSelect);
      return;
    }
    if (t == "math.clamp") {
      value(node, "value", depth); value(node, "min", depth); value(node, "max", depth);
      lset(kF2); lset(kF1); lset(kF0);
      lget(kF1); lget(kF2); op(kF64Min);
      lget(kF0); lget(kF1); lget(kF2); op(kF64Max); op(kF64Min);
      op(kF64Max);
      return;
    }
    if (t == "math.lerp") {
      value(node, "a", depth); value(node, "b", depth); value(node, "alpha", depth);
      lset(kF2); lset(kF1); lset(kF0);
      lget(kF0); lget(kF1); lget(kF0); op(kF64Sub); lget(kF2); op(kF64Mul); op(kF64Add);
      return;
    }
    if (t == "math.compare") {
      two("a", "b");
      const std::string o = in_.node_at(node).param("op", "==");
      if (o == "==" || o == "!=") { lget(kF0); lget(kF1); op(kF64Sub); op(kF64Abs); f64c(1e-9); op(o == "==" ? kF64Le : kF64Gt); }
      else { lget(kF0); lget(kF1); op(o == "<" ? kF64Lt : o == "<=" ? kF64Le : o == ">" ? kF64Gt : kF64Ge); }
      to_f64_from_bool();
      return;
    }
    if (t == "logic.not") { value(node, "a", depth); f64c(0); op(kF64Eq); to_f64_from_bool(); return; }
    if (t == "logic.and" || t == "logic.or" || t == "logic.xor") {
      value(node, "a", depth); bool_of_f64();
      value(node, "b", depth); bool_of_f64();
      op(t == "logic.and" ? kI32And : t == "logic.or" ? kI32Or : kI32Xor);
      to_f64_from_bool();
      return;
    }
    if (t == "logic.select") {
      value(node, "a", depth); value(node, "b", depth); value(node, "pick", depth); bool_of_f64();
      op(kSelect);
      return;
    }
    f64c(0);
  }

  // ---- exec helpers ----
  void next(int node, int ordinal) {
    const auto [target, entry] = in_.exec_target(node, ordinal);
    i32c(target);
    lset(kPc);
    i32c(entry);
    lset(kEntry);
  }
  void stop() { i32c(-1); lset(kPc); }
  void push(int pc, int entry) {
    const u32 st = static_cast<u32>(stack_);
    i32c(st); mem(kI32Load, 2, 0); lset(kSp);
    lget(kSp); i32c(in_.stack_capacity()); op(kI32GeS); if_void(); ret_i32(2); op(kEnd);
    i32c(st + 4); lget(kSp); i32c(3); op(kI32Shl); op(kI32Add); i32c(pc); mem(kI32Store, 2, 0);
    i32c(st + 4); lget(kSp); i32c(3); op(kI32Shl); op(kI32Add); i32c(entry); mem(kI32Store, 2, 4);
    i32c(st); lget(kSp); i32c(1); op(kI32Add); mem(kI32Store, 2, 0);
  }
  void push_exec(int node, int ordinal) {
    const auto [target, entry] = in_.exec_target(node, ordinal);
    if (target >= 0) push(target, entry);
  }
  void slot_cond_set(int slot, f64 v) { f64c(v); lset(kF0); store_slot_from_f0(slot); }
  // The loop guard (Runtime::budget): the runtime's counters - [3] loop passes this step, [1]
  // their limit, [4] a loop of this chain was refused a pass. may_pass leaves 1 on the stack when
  // the loop may make another pass.
  void may_pass() {
    const u32 ct = static_cast<u32>(counters_);
    i32c(ct + 12); mem(kI32Load, 2, 0); i32c(ct + 4); mem(kI32Load, 2, 0); op(kI32LtS);
  }
  void count_pass() {
    const u32 ct = static_cast<u32>(counters_);
    i32c(ct + 12); i32c(ct + 12); mem(kI32Load, 2, 0); i32c(1); op(kI32Add); mem(kI32Store, 2, 0);
  }
  void pass_refused() {
    const u32 ct = static_cast<u32>(counters_);
    i32c(ct + 16); i32c(1); mem(kI32Store, 2, 0);
  }

  void node_code(int i) {
    const NodeDef* d = in_.def_at(i);
    if (!d) { stop(); return; }
    const std::string& t = d->type;
    const int base = in_.node_slot(i);
    if (!in_.inline_node(i)) {
      // C++ node: call it, then continue at the exec output it chose.
      i32c(in_.id()); lget(kPc); lget(kEntry); call(kFnCallNode); lset(kR);
      lget(kR); i32c(in_.exec_outputs(i)); op(kI32GeU);
      if_void();
      stop();
      op(kElse);
      const u32 rows = static_cast<u32>(table_) + static_cast<u32>(in_.exec_table_offset(i)) * 4;
      i32c(rows); lget(kR); i32c(3); op(kI32Shl); op(kI32Add); mem(kI32Load, 2, 0); lset(kPc);
      i32c(rows); lget(kR); i32c(3); op(kI32Shl); op(kI32Add); mem(kI32Load, 2, 4); lset(kEntry);
      op(kEnd);
      return;
    }
    if (d->kind == NodeKind::event) { next(i, 0); return; }
    if (t == "flow.branch") {
      value(i, "condition", 0); bool_of_f64();
      if_void(); next(i, 0); op(kElse); next(i, 1); op(kEnd);
      return;
    }
    if (t == "flow.sequence") {
      for (int o = in_.exec_outputs(i) - 1; o >= 1; --o) push_exec(i, o);
      next(i, 0);
      return;
    }
    if (t == "flow.for") {
      lget(kEntry); i32c(kResume); op(kI32Ne);
      if_void();
      value(i, "first", 0); lset(kF0); store_slot_from_f0(base + 0);
      value(i, "last", 0); lset(kF0); store_slot_from_f0(base + 1);
      op(kElse);
      load_slot(base + 0); f64c(1); op(kF64Add); lset(kF0); store_slot_from_f0(base + 0);
      op(kEnd);
      load_slot(base + 0); load_slot(base + 1); op(kF64Gt);
      if_void();
      next(i, 1);  // Completed (exec outputs: body, completed)
      op(kElse);
      may_pass();
      if_void();
      count_pass();
      load_slot(base + 0); lset(kF0); store_slot_from_f0(in_.out_slot(i, 1));
      push(i, kResume);
      next(i, 0);
      op(kElse);
      pass_refused();  // (the loop guard: the loop ends)
      next(i, 1);
      op(kEnd);
      op(kEnd);
      return;
    }
    if (t == "flow.while") {
      value(i, "condition", 0); bool_of_f64();
      if_void();
      may_pass();
      if_void(); count_pass(); push(i, kResume); next(i, 0); op(kElse); pass_refused(); next(i, 1); op(kEnd);
      op(kElse); next(i, 1); op(kEnd);
      return;
    }
    if (t == "flow.do_once") {
      lget(kEntry); i32c(1); op(kI32Eq);
      if_void();
      slot_cond_set(base + 0, 0); slot_cond_set(base + 1, 1); stop();
      op(kElse);
      load_slot(base + 1); f64c(0); op(kF64Eq);
      if_void(); slot_cond_set(base + 1, 1); value(i, "closed", 0); lset(kF0); store_slot_from_f0(base + 0); op(kEnd);
      load_slot(base + 0); bool_of_f64();
      if_void(); stop(); op(kElse); slot_cond_set(base + 0, 1); next(i, 0); op(kEnd);
      op(kEnd);
      return;
    }
    if (t == "flow.do_n") {
      lget(kEntry); i32c(1); op(kI32Eq);
      if_void();
      slot_cond_set(base + 0, 0); stop();
      op(kElse);
      load_slot(base + 0); value(i, "n", 0); op(kF64Ge);
      if_void(); stop();
      op(kElse);
      load_slot(base + 0); f64c(1); op(kF64Add); lset(kF0); store_slot_from_f0(base + 0); store_slot_from_f0(in_.out_slot(i, 1));
      next(i, 0);
      op(kEnd);
      op(kEnd);
      return;
    }
    if (t == "flow.gate") {
      load_slot(base + 1); f64c(0); op(kF64Eq);
      if_void(); slot_cond_set(base + 1, 1); value(i, "closed", 0); f64c(0); op(kF64Eq); to_f64_from_bool(); lset(kF0); store_slot_from_f0(base + 0); op(kEnd);
      stop();
      lget(kEntry); i32c(0); op(kI32Eq);
      if_void(); load_slot(base + 0); bool_of_f64(); if_void(); next(i, 0); op(kEnd); op(kEnd);
      lget(kEntry); i32c(1); op(kI32Eq); if_void(); slot_cond_set(base + 0, 1); op(kEnd);
      lget(kEntry); i32c(2); op(kI32Eq); if_void(); slot_cond_set(base + 0, 0); op(kEnd);
      lget(kEntry); i32c(3); op(kI32Eq);
      if_void(); load_slot(base + 0); f64c(0); op(kF64Eq); to_f64_from_bool(); lset(kF0); store_slot_from_f0(base + 0); op(kEnd);
      return;
    }
    if (t == "flow.flip_flop") {
      load_slot(base + 0); f64c(0); op(kF64Eq); to_f64_from_bool(); lset(kF0);
      store_slot_from_f0(base + 0);
      if (const int o = in_.out_slot(i, 2); o >= 0) store_slot_from_f0(o);
      lget(kF0); bool_of_f64();
      if_void(); next(i, 0); op(kElse); next(i, 1); op(kEnd);
      return;
    }
    if (t == "var.set" || t == "var.add") {
      const Variable* v = in_.graph().variable(in_.node_at(i).param("variable"));
      const int vs = in_.var_slot(in_.node_at(i).param("variable"));
      if (t == "var.set") value(i, "value", 0);
      else { load_slot(vs); value(i, "amount", 0); op(kF64Add); }
      convert(v ? v->type : PinType::number);
      lset(kF0);
      store_slot_from_f0(vs);
      for (int o = 0; o < static_cast<int>(in_.outputs_at(i).size()); ++o)
        if (const int s = in_.out_slot(i, o); s >= 0) store_slot_from_f0(s);
      next(i, 0);
      return;
    }
    stop();
  }

  void body(Bytes& code) {
    o_ = &code;
    const int n = static_cast<int>(in_.node_count());
    const u32 st = static_cast<u32>(stack_), ct = static_cast<u32>(counters_);
    i32c(st); mem(kI32Load, 2, 0); lset(kBase);
    op(kLoop); op(kVoid);
    // pc < 0: continue with a continuation, or return when ours are used up.
    lget(kPc); i32c(0); op(kI32LtS);
    if_void();
    i32c(st); mem(kI32Load, 2, 0); ltee(kSp); lget(kBase); op(kI32LeS);
    if_void(); ret_i32(0); op(kEnd);
    lget(kSp); i32c(1); op(kI32Sub); lset(kSp);
    i32c(st); lget(kSp); mem(kI32Store, 2, 0);
    i32c(st + 4); lget(kSp); i32c(3); op(kI32Shl); op(kI32Add); mem(kI32Load, 2, 0); lset(kPc);
    i32c(st + 4); lget(kSp); i32c(3); op(kI32Shl); op(kI32Add); mem(kI32Load, 2, 4); lset(kEntry);
    op(kEnd);
    // Nodes run (a count for the runtime) and tracing for the editor. (The loop guard is at the loops.)
    i32c(ct); i32c(ct); mem(kI32Load, 2, 0); i32c(1); op(kI32Add); mem(kI32Store, 2, 0);
    i32c(ct + 8); mem(kI32Load, 2, 0); if_void(); i32c(in_.id()); lget(kPc); call(kFnTrace); op(kEnd);
    // Dispatch: block per node, br_table on pc.
    op(kBlock); op(kVoid);  // exit
    for (int k = n - 1; k >= 0; --k) { op(kBlock); op(kVoid); }
    lget(kPc);
    op(kBrTable);
    o_->uleb(static_cast<u32>(n));
    for (int k = 0; k < n; ++k) o_->uleb(static_cast<u32>(k));
    o_->uleb(static_cast<u32>(n));
    for (int k = 0; k < n; ++k) {
      op(kEnd);  // block k: its code follows
      node_code(k);
      br(static_cast<u32>(n - k));  // back to the loop
    }
    op(kEnd);  // exit block: unknown pc
    ret_i32(0);
    op(kEnd);  // loop
    i32c(0);
    op(kEnd);  // function
  }
};

}  // namespace

std::vector<u8> compile_wasm(const Instance& inst) { return WasmGen(inst).module(); }

}  // namespace kern::logic
