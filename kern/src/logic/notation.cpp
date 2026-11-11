// Kern Logic: the C++ notation of a Blueprint (docs/blueprint_cpp.md).
//
// A Blueprint and its C++ text are one thing in two notations. Shape works out, from the graph
// alone, what is written where: which steps are statements of which body, where paths meet
// again, which values are written in place and which get a name. Writer writes the text from
// that; Reader reads a text back into a graph. Both follow the same rules, one node at a time,
// so every node is named exactly once and in one way.
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>

#include "kern/logic/logic.h"
#include "kern/logic/nodes.h"

namespace kern::logic {

namespace {

// ---------------------------------------------------------------------------
// names and literals

const char* const kCppKeywords[] = {
    "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t", "class",
    "compl", "concept", "const", "consteval", "constexpr", "constinit", "const_cast", "continue", "co_await", "co_return", "co_yield", "decltype", "default",
    "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int",
    "long", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public", "register",
    "reinterpret_cast", "requires", "return", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "template", "this",
    "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while",
    "xor", "xor_eq",
};
/// Names the notation itself uses inside a Blueprint's struct.
const char* const kOwnWords[] = {"Bool", "Int", "Float", "Vector", "Text", "Actor", "List", "loop", "machine", "entry", "detached", "kern"};

bool is_start(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool is_digit(char c) { return c >= '0' && c <= '9'; }
bool is_part(char c) { return is_start(c) || is_digit(c); }

bool cpp_keyword(std::string_view name) {
  for (const char* k : kCppKeywords) if (name == k) return true;
  return false;
}

/// "n12", "go12", "on12", "it12", "machine12": the names the notation gives nodes. Returns the
/// number (0: not such a name).
u32 numbered(std::string_view name, std::string_view prefix) {
  if (name.size() <= prefix.size() || name.substr(0, prefix.size()) != prefix) return 0;
  if (name[prefix.size()] == '0') return 0;  // (no leading zeros: one name per number)
  u64 v = 0;
  for (std::size_t i = prefix.size(); i < name.size(); ++i) {
    if (!is_digit(name[i])) return 0;
    v = v * 10 + static_cast<u64>(name[i] - '0');
    if (v > 999999999) return 0;
  }
  return static_cast<u32>(v);
}

const char* cpp_type(PinType t) {
  switch (t) {
    case PinType::boolean: return "Bool";
    case PinType::integer: return "Int";
    case PinType::number: return "Float";
    case PinType::vector: return "Vector";
    case PinType::text: return "Text";
    case PinType::actor: return "Actor";
    case PinType::boolean_list: return "List<Bool>";
    case PinType::integer_list: return "List<Int>";
    case PinType::number_list: return "List<Float>";
    case PinType::vector_list: return "List<Vector>";
    case PinType::text_list: return "List<Text>";
    case PinType::actor_list: return "List<Actor>";
    default: return "void";
  }
}
/// A type as the notation writes it: Float, List<Text>; an enum or a struct by its name - Mode,
/// List<Mode>.
std::string cpp_type(const TypeRef& t) {
  if (element_of(t.type) != PinType::enumeration && element_of(t.type) != PinType::structure) return cpp_type(t.type);
  return is_list(t.type) ? "List<" + t.of + ">" : t.of;
}
/// A value type by its name (a list is List<that>).
bool type_of_cpp(std::string_view name, PinType& out) {
  for (PinType t : {PinType::boolean, PinType::integer, PinType::number, PinType::vector, PinType::text, PinType::actor})
    if (name == cpp_type(t)) { out = t; return true; }
  return false;
}

/// A node type as a C++ name: "math.add" is math_add.
std::string node_name(const std::string& type) {
  std::string r = type;
  for (char& c : r) if (c == '.') c = '_';
  return r;
}

/// The shortest text that reads back as the same double; always a C++ double literal.
std::string float_text(f64 v) {
  if (!std::isfinite(v)) v = 0;
  char b[48];
  if (v == std::floor(v) && std::fabs(v) < 1e15) {
    std::snprintf(b, sizeof b, "%.1f", v);
    return b;
  }
  for (int digits = 1; digits <= 17; ++digits) {
    std::snprintf(b, sizeof b, "%.*g", digits, v);
    if (std::strtod(b, nullptr) == v) break;
  }
  std::string r = b;
  if (r.find_first_of(".e") == std::string::npos) r += ".0";
  return r;
}
/// The shortest text that reads back as the same float (a vector's components, places).
std::string f32_text(f32 v, bool whole_plain = false) {
  if (!std::isfinite(v)) v = 0;
  char b[48];
  if (v == std::floor(v) && std::fabs(v) < 1e7f) {
    std::snprintf(b, sizeof b, whole_plain ? "%.0f" : "%.1f", static_cast<f64>(v));
    if (std::strcmp(b, "-0") == 0) return "0";
    return b;
  }
  for (int digits = 1; digits <= 9; ++digits) {
    std::snprintf(b, sizeof b, "%.*g", digits, static_cast<f64>(v));
    if (static_cast<f32>(std::strtod(b, nullptr)) == v) break;
  }
  std::string r = b;
  if (r.find_first_of(".e") == std::string::npos) r += ".0";
  return r;
}
/// A place on the canvas (px): whole numbers as they are.
std::string place(f32 v) { return f32_text(v, true); }

std::string text_literal(const std::string& s) {
  std::string r = "\"";
  for (unsigned char c : s) {
    if (c == '"') r += "\\\"";
    else if (c == '\\') r += "\\\\";
    else if (c == '\n') r += "\\n";
    else if (c == '\t') r += "\\t";
    else if (c == '\r') r += "\\r";
    else if (c < 0x20 || c == 0x7f) {
      char b[8];
      std::snprintf(b, sizeof b, "\\%03o", c);
      r += b;
    } else r += static_cast<char>(c);
  }
  return r + "\"";
}
std::string literal(const Value& v) {
  switch (v.type) {
    case PinType::boolean: return v.b ? "true" : "false";
    case PinType::integer: return std::to_string(v.i);
    case PinType::number: return float_text(v.n);
    case PinType::vector: return "Vector{" + f32_text(v.v.x) + ", " + f32_text(v.v.y) + ", " + f32_text(v.v.z) + "}";
    case PinType::text: return text_literal(v.s);
    case PinType::actor: return "Actor{" + std::to_string(v.a) + "}";
    default: break;
  }
  if (!is_list(v.type)) return "0";
  // a list: List<Float>{1.0, 2.5}
  std::string r = std::string(cpp_type(v.type)) + "{";
  for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + literal(v.item(k));
  return r + "}";
}
/// A value as the notation writes it, where it may be an enum's or a struct's: `of` says which,
/// `g` has its declaration - Mode::Open, List<Mode>{Mode::Closed, Mode::Open}; a struct with
/// every field, in their order: Weapon{"Knife", Kind::Melee, 25.0}.
std::string literal(const Value& v, const std::string& of, const Graph& g, std::size_t depth = 0) {
  if (element_of(v.type) == PinType::structure) {
    const StructType* s = depth < g.structs.size() ? g.structure(of) : nullptr;  // (a struct holds structs declared above it: no deeper than there are structs)
    auto one = [&](const Value& x) {
      std::string r = of + "{";
      for (std::size_t k = 0; s && k < s->fields.size(); ++k) r += (k ? ", " : "") + literal(field_of(x, *s, k, g), s->fields[k].type.of, g, depth + 1);
      return r + "}";
    };
    if (v.type == PinType::structure) return one(v);
    std::string r = "List<" + of + ">{";
    for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + one(v.item(k));
    return r + "}";
  }
  if (element_of(v.type) != PinType::enumeration) return literal(v);
  const EnumType* e = g.enumeration(of);
  auto one = [&](i64 index) { return of + "::" + (e && enums::is_value(e->values.size(), index) ? e->values[static_cast<std::size_t>(index)] : std::string("_") + std::to_string(index)); };
  if (v.type == PinType::enumeration) return one(v.i);
  std::string r = "List<" + of + ">{";
  for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + one(v.item(k).i);
  return r + "}";
}
/// The settings of a node as they stand behind its function's name: <"smooth">. A setting that
/// names a variable is not among them - the variable itself is the call's first value.
/// A setting that names a type is that type: logic_select<Float>, list_make<Text, "2">.
std::string settings_of(const NodeDef& def, const Node& n, const Graph& g) {
  std::string r;
  for (const ParamDef& pd : def.params) {
    if (pd.kind == ParamDef::Kind::variable) continue;
    const std::string value = n.param(pd.key, pd.def);
    TypeRef type;
    r += (r.empty() ? "<" : ", ") + (pd.kind == ParamDef::Kind::type && type_from_text(value, g, type) ? cpp_type(type) : text_literal(value));
  }
  return r.empty() ? r : r + ">";
}
/// The variables a node's settings name, in their order.
std::vector<std::string> variables_of(const NodeDef& def, const Node& n) {
  std::vector<std::string> r;
  for (const ParamDef& pd : def.params)
    if (pd.kind == ParamDef::Kind::variable) r.push_back(n.param(pd.key, pd.def));
  return r;
}

// ---------------------------------------------------------------------------
// the graph as the notation sees it

/// How a step is written.
enum class Form : u8 {
  none,        // a value or an event: no statement
  plain,       // call;                         the path goes on
  end,         // call;                         the path ends (no exec output)
  test,        // if (call) { }                 one exec output it may not take
  choice,      // switch (call) { case k: }     several exec outputs
  branch,      // if (value) { } else { }       flow.branch
  sequence,    // { } { }                       flow.sequence
  loop,        // for (auto it = call; it.next();) { }
  loop_while,  // while (loop(value)) { }       flow.while
  latent,      // call(..., [this] { });        what follows runs later
  give,        // return {values};              func.return
};

constexpr int kNowhere = -1, kMember = -2;
enum CtxKind : int { kCallable, kGo, kLatent, kDetached };

struct Info {
  const Node* node = nullptr;
  const NodeDef* def = nullptr;
  std::vector<PinDef> in, out;
  std::vector<std::pair<int, int>> src;      // per input: the output it reads (node, output) or (-1, -1)
  std::vector<std::pair<int, int>> dst;      // per exec output (ordinal): the input it leads to (node, input) or (-1, -1)
  std::vector<std::pair<int, int>> readers;  // data wires leaving: (node, input)
  std::vector<int> exec_in, exec_out, data_in, data_out;  // pin indices by kind, in order
  int entered = 0;           // exec wires arriving
  bool other_entry = false;  // one of them through an exec input that is not the first
  bool pure = false, event = false;
  Form form = Form::none;
  bool root = false;         // a step no single wire leads to through its first exec input
  bool merged = false;       // a root that is written where the paths of a branching step meet again
  bool own = false;          // a step written as its own method go<id>
  bool named = false;        // a value several inputs read: it has the name n<id>
  bool decided = false, seen = false, on_path = false;
  int merge = -1;            // branching steps: the step their paths meet at (written after them)
  bool guard = false;        // a Branch whose True path always returns: its False path is written after it
  int place = kNowhere;      // the body (context) its text stands in
  bool member = false;       // it has a member n<id>: outputs read elsewhere, or memory
};

/// An exec output a list of statements leaves through: wired to `target`, or to nothing (-1). kOn:
/// no exec output at all - the graph's path has ended there (after a wait, a Sequence, a step
/// without exec output), but C++ goes on with the next statement.
struct Tail { int node, ordinal, target; };
constexpr int kOn = -2;

struct Shape {
  const Graph& g;
  const Library& lib;
  std::vector<Info> n;
  std::vector<int> by_id;                  // node indices by node id
  std::unordered_map<u32, int> index;
  std::vector<std::pair<int, int>> ctx;    // (kind, owner node)
  int detached = kNowhere;
  std::string error;
  u32 error_node = 0;

  Shape(const Graph& graph, const Library& library) : g(graph), lib(library) {}

  Info& at(int i) { return n[static_cast<std::size_t>(i)]; }
  const Info& at(int i) const { return n[static_cast<std::size_t>(i)]; }
  bool fail(const std::string& why, int node = -1) {
    if (error.empty()) {
      error = why;
      error_node = node >= 0 ? at(node).node->id : 0;
    }
    return false;
  }
  std::string title(int i) const { return lib.title(*at(i).node) + " (node " + std::to_string(at(i).node->id) + ")"; }
  int new_ctx(int kind, int owner) {
    ctx.push_back({kind, owner});
    return static_cast<int>(ctx.size()) - 1;
  }

  /// The exec outputs the statements from `x` on leave through; decides, on the way, where the
  /// paths of branching steps meet again.
  std::vector<Tail> tails(int x) {
    std::vector<int> walked;
    std::vector<Tail> result;
    for (;;) {
      at(x).seen = at(x).on_path = true;
      walked.push_back(x);
      const Form form = at(x).form;
      const std::size_t outs = at(x).exec_out.size();
      auto arm = [&](std::size_t k, std::vector<Tail>& into) {
        const int t = at(x).dst[k].first;
        if (t < 0) into.push_back({x, static_cast<int>(k), -1});
        else if (!at(t).root) {
          std::vector<Tail> v = tails(t);
          into.insert(into.end(), v.begin(), v.end());
        } else into.push_back({x, static_cast<int>(k), t});
      };
      auto inner = [&](std::size_t k) {  // a body: what leaves it stays inside
        std::vector<Tail> drop;
        arm(k, drop);
      };
      int next = -1;
      if (form == Form::plain || (at(x).event && outs > 0)) {
        next = 0;  // (an event: its chain starts at its one exec output)
      } else if (form == Form::loop || form == Form::loop_while) {
        inner(0);
        for (std::size_t k = 2; k < outs; ++k) inner(k);
        if (outs > 1) next = 1;
        else result.push_back({x, -1, kOn});
      } else if (form == Form::latent || form == Form::sequence) {
        for (std::size_t k = 0; k < outs; ++k) inner(k);
        result.push_back({x, -1, kOn});
      } else if (form == Form::end) {
        result.push_back({x, -1, kOn});
      } else if (form == Form::branch || form == Form::choice || form == Form::test) {
        std::vector<Tail> t;
        bool first_returns = false;  // (every way through the first path ends in a return)
        for (std::size_t k = 0; k < outs; ++k) {
          arm(k, t);
          if (k == 0) first_returns = t.empty();
        }
        if (!at(x).decided) {
          at(x).decided = true;
          int m = t.size() >= 2 ? t[0].target : -1;
          for (const Tail& e : t) if (e.target != m) m = -1;
          if (m >= 0 && !at(m).on_path && !at(m).other_entry && !at(m).merged && at(m).entered == static_cast<int>(t.size())) {
            at(x).merge = m;
            at(m).merged = true;
          } else if (form == Form::branch && first_returns && at(x).dst[0].first >= 0 && at(x).dst[1].first >= 0) {
            at(x).guard = true;
          }
        }
        if (at(x).merge >= 0) {
          x = at(x).merge;
          continue;
        }
        result = std::move(t);
      }
      if (next < 0) break;
      const int t = at(x).dst[static_cast<std::size_t>(next)].first;
      if (t < 0) { result.push_back({x, next, -1}); break; }
      if (at(t).root) { result.push_back({x, next, t}); break; }
      x = t;
    }
    for (int w : walked) at(w).on_path = false;
    return result;
  }

  /// Places everything the exec wires lead to from `start` (which stands in `c`).
  void walk(int start, int c) {
    std::vector<std::pair<int, int>> todo{{start, c}};
    while (!todo.empty()) {
      const auto [x, cx] = todo.back();
      todo.pop_back();
      for (std::size_t k = at(x).exec_out.size(); k-- > 0;) {
        const int t = at(x).dst[k].first;
        if (t < 0) continue;
        Info& T = at(t);
        if (T.own || T.place != kNowhere) continue;
        // What follows a latent node runs later, from the scheduler: a body of its own.
        T.place = at(x).form == Form::latent ? new_ctx(kLatent, x) : cx;
        todo.push_back({t, T.place});
      }
    }
  }

  /// Where the text that reads a value stands: a step in its body, a value where its one reader
  /// stands; a value several bodies read is a member (kMember).
  int place_of(int x) {
    Info& X = at(x);
    if (!X.pure || X.place != kNowhere) return X.place;
    if (X.readers.empty()) return X.place = detached;
    int where = kNowhere;
    for (const auto& r : X.readers) {
      const int p = place_of(r.first);
      if (where == kNowhere) where = p;
      else if (where != p) where = kMember;
    }
    return X.place = where;
  }

  Form form_of(const Info& x) const {
    if (x.pure || x.event) return Form::none;
    const std::string& t = x.def->type;
    if (t == "func.return") return Form::give;
    if (t == "flow.branch") return Form::branch;
    if (t == "flow.sequence") return Form::sequence;
    if (t == "flow.while") return Form::loop_while;
    if (x.def->latent) return Form::latent;
    if (x.def->loop && !x.exec_out.empty()) return Form::loop;
    if (x.exec_out.empty()) return Form::end;
    if (x.exec_out.size() >= 2 || t == "flow.switch_enum") return Form::choice;
    return x.def->kind == NodeKind::flow ? Form::test : Form::plain;
  }

  bool build() {
    n.resize(g.nodes.size());
    for (std::size_t i = 0; i < g.nodes.size(); ++i) {
      Info& x = n[i];
      x.node = &g.nodes[i];
      x.def = lib.find(x.node->type);
      if (!x.def) return fail("node " + std::to_string(x.node->id) + " has the unknown type \"" + x.node->type + "\"");
      if (x.node->id == 0) return fail("a node has no id");
      if (index.count(x.node->id)) return fail("node id " + std::to_string(x.node->id) + " is used twice");
      index[x.node->id] = static_cast<int>(i);
      by_id.push_back(static_cast<int>(i));
      lib.pins(*x.node, g, x.in, x.out);
      for (std::size_t k = 0; k < x.in.size(); ++k) (x.in[k].type == PinType::exec ? x.exec_in : x.data_in).push_back(static_cast<int>(k));
      for (std::size_t k = 0; k < x.out.size(); ++k) (x.out[k].type == PinType::exec ? x.exec_out : x.data_out).push_back(static_cast<int>(k));
      x.src.assign(x.in.size(), {-1, -1});
      x.dst.assign(x.exec_out.size(), {-1, -1});
      x.pure = x.def->kind == NodeKind::pure;
      x.event = x.def->kind == NodeKind::event;
      x.form = form_of(x);
    }
    std::sort(by_id.begin(), by_id.end(), [&](int a, int b) { return at(a).node->id < at(b).node->id; });
    for (const Link& l : g.links) {
      auto fi = index.find(l.from), ti = index.find(l.to);
      if (fi == index.end() || ti == index.end()) return fail("a wire leads to a node that is not there");
      Info& from = at(fi->second);
      Info& to = at(ti->second);
      int o = -1, i = -1;
      for (std::size_t k = 0; k < from.out.size(); ++k) if (from.out[k].id == l.out) o = static_cast<int>(k);
      for (std::size_t k = 0; k < to.in.size(); ++k) if (to.in[k].id == l.in) i = static_cast<int>(k);
      if (o < 0 || i < 0) return fail("a wire of " + title(ti->second) + " connects a pin that does not exist (" + l.out + " -> " + l.in + ")", ti->second);
      const bool exec = from.out[static_cast<std::size_t>(o)].type == PinType::exec;
      if (exec != (to.in[static_cast<std::size_t>(i)].type == PinType::exec)) return fail("an exec wire of " + title(ti->second) + " is connected to a data pin", ti->second);
      if (exec) {
        const std::size_t ordinal = static_cast<std::size_t>(std::find(from.exec_out.begin(), from.exec_out.end(), o) - from.exec_out.begin());
        if (from.dst[ordinal].first >= 0)
          return fail("the exec output \"" + l.out + "\" of " + title(fi->second) + " has more than one wire: a Sequence runs several things one after the other", fi->second);
        from.dst[ordinal] = {ti->second, i};
        ++to.entered;
        if (i != to.exec_in.front()) to.other_entry = true;
      } else {
        if (to.src[static_cast<std::size_t>(i)].first >= 0) return fail("the input \"" + l.in + "\" of " + title(ti->second) + " has more than one wire", ti->second);
        if (!fits({from.out[static_cast<std::size_t>(o)].type, from.out[static_cast<std::size_t>(o)].of}, {to.in[static_cast<std::size_t>(i)].type, to.in[static_cast<std::size_t>(i)].of}))
          return fail("a wire into \"" + l.in + "\" of " + title(ti->second) + " carries the wrong type", ti->second);
        to.src[static_cast<std::size_t>(i)] = {fi->second, o};
        from.readers.push_back({ti->second, i});
      }
    }
    if (!values_ok()) return false;
    // Steps: which ones a single wire leads to (written where that wire leaves), which ones are
    // written where paths meet again, which ones are methods of their own.
    for (Info& x : n) {
      if (x.pure || x.event) continue;
      x.root = x.entered != 1 || x.other_entry;
      if (x.form == Form::give && x.entered == 0) x.form = Form::end;  // (a Return nothing leads to: a call like any other)
    }
    for (int i : by_id) if (at(i).event) tails(i);
    for (int i : by_id) if (at(i).root) tails(i);
    for (int i : by_id) {
      // a circle of steps nothing leads into: its first step is a method
      Info& x = at(i);
      if (x.pure || x.event || x.seen) continue;
      x.root = true;
      tails(i);
    }
    for (Info& x : n) x.own = x.root && !x.merged;
    detached = new_ctx(kDetached, -1);
    for (int i : by_id)
      if (at(i).event) {
        at(i).place = new_ctx(kCallable, i);
        walk(i, at(i).place);
      }
    for (int i : by_id)
      if (at(i).own) {
        at(i).place = new_ctx(kGo, i);
        walk(i, at(i).place);
      }
    for (int i : by_id) place_of(i);
    // Members: what a step gives when it is read, what a node keeps between runs; an event's
    // or an entry's values when they are read outside its own body.
    for (Info& x : n) {
      if (x.pure) { x.member = x.def->memory; continue; }
      if (!x.event) { x.member = !x.readers.empty() || x.def->memory; continue; }
      for (const auto& r : x.readers)
        if (at(r.first).place != x.place) x.member = true;
    }
    return rules();
  }

  /// Values: no circles; a value with memory (Random) is read by one input, and so is everything
  /// that passes it on - every read would be another number.
  bool values_ok() {
    std::vector<u8> mark(n.size(), 0), fresh(n.size(), 0);
    for (int start : by_id) {
      if (!at(start).pure || mark[static_cast<std::size_t>(start)]) continue;
      std::vector<std::pair<int, std::size_t>> stack{{start, 0}};  // depth first: (node, next input)
      mark[static_cast<std::size_t>(start)] = 1;
      while (!stack.empty()) {
        const int x = stack.back().first;
        const Info& X = at(x);
        if (stack.back().second < X.src.size()) {
          const int f = X.src[stack.back().second++].first;
          if (f < 0 || !at(f).pure) continue;
          if (mark[static_cast<std::size_t>(f)] == 1) return fail(title(f) + " depends on itself", f);
          if (mark[static_cast<std::size_t>(f)] == 0) {
            mark[static_cast<std::size_t>(f)] = 1;
            stack.push_back({f, 0});
          }
          continue;
        }
        bool is_fresh = X.def->memory;
        for (const auto& s : X.src) if (s.first >= 0 && at(s.first).pure && fresh[static_cast<std::size_t>(s.first)]) is_fresh = true;
        fresh[static_cast<std::size_t>(x)] = is_fresh ? 1 : 0;
        mark[static_cast<std::size_t>(x)] = 2;
        stack.pop_back();
      }
    }
    for (int i : by_id) {
      Info& x = at(i);
      if (!x.pure) continue;
      x.named = x.readers.size() >= 2;
      if (x.named && fresh[static_cast<std::size_t>(i)])
        return fail(title(i) + (x.def->memory ? " is" : " passes on a value that is") + " new on every read, and " + std::to_string(x.readers.size()) +
                        " inputs read it: each would get another one. Keep it in a variable (Set), or use one node per input.",
                    i);
    }
    return true;
  }

  const Function* function_of(int i) const { return g.function(at(i).node->param("function")); }

  /// The steps a step leads to that run before it is over (not what follows a wait), and whether
  /// the path goes on through them: through Reset, Open or Close of a node it does not.
  std::vector<std::pair<int, bool>> next_steps(int x) const {
    std::vector<std::pair<int, bool>> r;
    for (std::size_t k = 0; k < at(x).dst.size(); ++k) {
      const auto& d = at(x).dst[k];
      // (after a wait: later - except what a waiting node runs with the call itself, a Tween's Update)
      if (at(x).def->latent && static_cast<int>(k) >= at(x).def->now_outputs) continue;
      if (d.first < 0) continue;
      const Info& t = at(d.first);
      r.push_back({d.first, !(t.def->first_entry_only && d.second != t.exec_in.front())});
    }
    return r;
  }

  /// No path leads back into itself without a wait: steps are repeated by a loop node (in C++ a
  /// loop is a loop - a path back would be a method calling itself, deeper with every round).
  bool paths_ok() {
    std::vector<u8> mark(n.size(), 0);
    for (int start : by_id) {
      if (at(start).pure || mark[static_cast<std::size_t>(start)]) continue;
      std::vector<std::pair<int, std::size_t>> stack{{start, 0}};  // depth first: (step, next exec output)
      mark[static_cast<std::size_t>(start)] = 1;
      while (!stack.empty()) {
        const int x = stack.back().first;
        const std::vector<std::pair<int, bool>> next = next_steps(x);
        if (stack.back().second < next.size()) {
          const auto [t, on] = next[stack.back().second++];
          if (!on) continue;  // (the path ends in that node: a Gate closing itself is no way back)
          if (mark[static_cast<std::size_t>(t)] == 1)
            return fail("the path from " + title(x) + " leads back to " + title(t) + " without a wait: a While Loop or For Loop repeats steps", x);
          if (mark[static_cast<std::size_t>(t)] == 0) {
            mark[static_cast<std::size_t>(t)] = 1;
            stack.push_back({t, 0});
          }
          continue;
        }
        mark[static_cast<std::size_t>(x)] = 2;
        stack.pop_back();
      }
    }
    return true;
  }

  /// Functions: none runs again while it runs (it has one set of values, not one per call), and
  /// one that is read as a value only computes - how often and in which order values are read
  /// then makes no difference.
  bool functions_ok() {
    const std::size_t count = g.functions.size();
    auto index_of = [&](const std::string& name) {
      for (std::size_t f = 0; f < count; ++f) if (g.functions[f].name == name) return static_cast<int>(f);
      return -1;
    };
    std::vector<std::vector<int>> steps(count);                   // per function: its steps before any wait
    std::vector<std::vector<std::pair<int, int>>> calls(count);   // per function: (the function called, the node that calls)
    std::vector<u8> value(count, 0);
    for (std::size_t f = 0; f < count; ++f) value[f] = g.functions[f].pure ? 1 : 0;
    for (int i : by_id) {
      if (at(i).def->type == "func.pure")
        if (const int f = index_of(at(i).node->param("function")); f >= 0) value[static_cast<std::size_t>(f)] = 1;
      if (at(i).def->type != "func.entry") continue;
      const int f = index_of(at(i).node->param("function"));
      if (f < 0) continue;
      std::vector<u8> seen(n.size(), 0);  // 1: it runs in the function (its values are read), 2: and the path goes on through it
      std::vector<std::pair<int, bool>> todo{{i, true}};
      while (!todo.empty()) {
        const auto [x, on] = todo.back();
        todo.pop_back();
        u8& state = seen[static_cast<std::size_t>(x)];
        const u8 want = on ? 3 : 1;
        if ((state & want) == want) continue;
        const bool first = state == 0;
        state |= want;
        if (first) {
          const std::string& t = at(x).def->type;
          if (t == "func.call" || t == "func.pure")
            if (const int callee = index_of(at(x).node->param("function")); callee >= 0) calls[static_cast<std::size_t>(f)].push_back({callee, x});
          for (const auto& src : at(x).src)  // the values it reads
            if (src.first >= 0 && at(src.first).pure) todo.push_back({src.first, true});
          if (!at(x).pure && x != i) steps[static_cast<std::size_t>(f)].push_back(x);
        }
        if (at(x).pure || !on) continue;  // (closing a Gate is something the function does - but nothing after it)
        for (const auto& next : next_steps(x)) todo.push_back(next);
      }
    }
    // a function that calls itself, directly or through others
    std::vector<u8> mark(count, 0);
    for (std::size_t start = 0; start < count; ++start) {
      if (mark[start]) continue;
      std::vector<std::pair<int, std::size_t>> stack{{static_cast<int>(start), 0}};
      mark[start] = 1;
      while (!stack.empty()) {
        const std::size_t f = static_cast<std::size_t>(stack.back().first);
        if (stack.back().second < calls[f].size()) {
          const auto [callee, node] = calls[f][stack.back().second++];
          if (mark[static_cast<std::size_t>(callee)] == 1) {
            const std::string& a = g.functions[f].name;
            const std::string& b = g.functions[static_cast<std::size_t>(callee)].name;
            return fail("function \"" + b + "\" calls itself" + (a == b ? std::string() : " (through \"" + a + "\")") +
                            ": a function does not run again while it runs - repeat steps with a loop",
                        node);
          }
          if (mark[static_cast<std::size_t>(callee)] == 0) {
            mark[static_cast<std::size_t>(callee)] = 1;
            stack.push_back({callee, 0});
          }
          continue;
        }
        mark[f] = 2;
        stack.pop_back();
      }
    }
    // a function read as a value only computes; so does every function it calls
    std::vector<int> todo;
    std::vector<std::string> because(count);
    for (std::size_t f = 0; f < count; ++f)
      if (value[f]) {
        todo.push_back(static_cast<int>(f));
        because[f] = "function \"" + g.functions[f].name + "\" is read as a value, so it only computes";
      }
    while (!todo.empty()) {
      const std::size_t f = static_cast<std::size_t>(todo.back());
      todo.pop_back();
      for (int x : steps[f]) {
        const NodeDef& d = *at(x).def;
        if (d.type == "func.call") {
          const int callee = index_of(at(x).node->param("function"));
          if (callee >= 0 && !value[static_cast<std::size_t>(callee)]) {
            value[static_cast<std::size_t>(callee)] = 1;
            because[static_cast<std::size_t>(callee)] =
                "function \"" + g.functions[static_cast<std::size_t>(callee)].name + "\" runs inside \"" + g.functions[f].name + "\", which is read as a value, so it only computes";
            todo.push_back(callee);
          }
          continue;
        }
        const bool computes = d.type == "flow.reroute" || d.type == "func.return" || (d.kind == NodeKind::flow && !d.latent && (!d.memory || d.loop));
        if (!computes)
          return fail(because[f] + ": " + title(x) + (d.latent ? " waits" : " changes something") + ". Call the function as a step instead (My Blueprint: untick Pure)", x);
      }
    }
    return true;
  }

  /// What the notation cannot write, and so no Blueprint may be.
  bool rules() {
    std::set<std::string> names, node_names;
    for (const auto& d : lib.all()) node_names.insert(node_name(d->type));
    auto name_ok = [&](const std::string& name, const std::string& what) {
      if (!cpp_name_ok(name))
        return fail(what + " \"" + name + "\" is no name C++ can write: letters, digits and _, not a word of C++ or of the notation (Float, loop, n1 ...)");
      if (node_names.count(name)) return fail(what + " \"" + name + "\" is the name of a node");
      return true;
    };
    // An enum is a type of the Blueprint's C++: its name and its values' names are C++ names, and
    // what is of its type holds one of its values.
    for (const EnumType& e : g.enums) {
      if (!name_ok(e.name, "the enum")) return false;
      if (!names.insert(e.name).second) return fail("the name \"" + e.name + "\" is used twice");
      if (e.name.size() > 5 && e.name.substr(e.name.size() - 5) == "_list") return fail("the enum \"" + e.name + "\": a name that ends in _list reads as a list");
      if (e.values.empty()) return fail("the enum " + e.name + " has no values");
      std::set<std::string> seen;
      for (const std::string& v : e.values) {
        if (!cpp_name_ok(v)) return fail("the value \"" + v + "\" of " + e.name + " is no name C++ can write: letters, digits and _, not a word of C++ or of the notation");
        if (!seen.insert(v).second) return fail("the enum " + e.name + " has two values called \"" + v + "\"");
      }
    }
    auto typed_ok = [&](const std::string& what, PinType type, const std::string& of, const json::Value& value) {
      if (element_of(type) == PinType::structure) {
        if (!g.structure(of)) return fail(what + " is of the struct \"" + of + "\", which is not declared");
        if (!is_value_of(Value::from_json(value, type, g, of), {type, of}, g)) return fail(what + " holds what is no value of " + of);
        return true;
      }
      if (element_of(type) != PinType::enumeration) return true;
      const EnumType* e = g.enumeration(of);
      if (!e) return fail(what + " is of the enum \"" + of + "\", which is not declared");
      const Value v = Value::from_json(value, type);
      for (std::size_t k = 0, count = type == PinType::enumeration ? 1 : v.size(); k < count; ++k)
        if (!enums::is_value(e->values.size(), type == PinType::enumeration ? v.i : v.item(k).i)) return fail(what + " holds what is no value of " + e->name);
      return true;
    };
    // A struct is a type of the Blueprint's C++ too: its name and its fields' names are C++ names,
    // and a field is of a type declared above the struct - no struct contains itself.
    for (std::size_t at = 0; at < g.structs.size(); ++at) {
      const StructType& st = g.structs[at];
      if (!name_ok(st.name, "the struct")) return false;
      if (!names.insert(st.name).second) return fail("the name \"" + st.name + "\" is used twice");
      if (st.name.size() > 5 && st.name.substr(st.name.size() - 5) == "_list") return fail("the struct \"" + st.name + "\": a name that ends in _list reads as a list");
      if (st.fields.empty()) return fail("the struct " + st.name + " has no fields");
      if (st.fields.size() > StructType::kMaxFields) return fail("the struct " + st.name + " has " + std::to_string(st.fields.size()) + " fields: " + std::to_string(StructType::kMaxFields) + " at most");
      std::set<std::string> seen;
      for (const StructField& f : st.fields) {
        if (!cpp_name_ok(f.name)) return fail("the field \"" + f.name + "\" of " + st.name + " is no name C++ can write: letters, digits and _, not a word of C++ or of the notation");
        if (!seen.insert(f.name).second) return fail("the struct " + st.name + " has two fields called \"" + f.name + "\"");
        if (g.enumeration(f.name) || g.structure(f.name)) return fail("the field \"" + f.name + "\" of " + st.name + " has the name of a type of this Blueprint");
        if (f.type.type == PinType::exec || f.type.type >= PinType::count) return fail("the field " + f.name + " of " + st.name + " has no type");
        if (element_of(f.type.type) == PinType::structure) {
          std::size_t inner = 0;
          while (inner < g.structs.size() && g.structs[inner].name != f.type.of) ++inner;
          if (inner == at) return fail("the struct " + st.name + " contains itself (the field " + f.name + ")");
          if (inner >= at) return fail("the field " + f.name + " of " + st.name + " is of the struct \"" + f.type.of + "\", which is not declared above it");
        }
        if (!typed_ok("the field " + f.name + " of " + st.name, f.type.type, f.type.of, f.value)) return false;
      }
    }
    for (const Variable& v : g.variables) {
      if (!name_ok(v.name, "the variable")) return false;
      if (!names.insert(v.name).second) return fail("the name \"" + v.name + "\" is used twice");
      if (!typed_ok("the variable " + v.name, v.type, v.of, v.value)) return false;
    }
    for (const Function& f : g.functions) {
      if (!name_ok(f.name, "the function")) return false;
      if (!names.insert(f.name).second) return fail("the name \"" + f.name + "\" is used twice (a variable and a function, or two functions)");
    }
    for (const Function& f : g.functions) {
      if (names.count(f.name + "Result")) return fail("\"" + f.name + "Result\" is taken by the function " + f.name + " (the type of its results in C++)");
      for (const auto* pins : {&f.inputs, &f.outputs}) {
        std::set<std::string> seen;
        for (const FunctionPin& p : *pins) {
          if (!name_ok(p.name, "the pin")) return false;
          if (!seen.insert(p.name).second) return fail("function \"" + f.name + "\" has two pins called \"" + p.name + "\"");
          if (pins == &f.inputs && names.count(p.name)) return fail("the input \"" + p.name + "\" of " + f.name + " has the name of a variable or a function");
          if (!typed_ok("\"" + p.name + "\" of " + f.name, p.type, p.of, p.value)) return false;
        }
      }
      int entries = 0;
      for (const Info& x : n) entries += x.def->type == "func.entry" && x.node->param("function") == f.name ? 1 : 0;
      if (entries != 1) return fail("function \"" + f.name + "\" needs exactly one Function Entry node (it has " + std::to_string(entries) + ")");
    }
    for (int i : by_id) {
      const Info& x = at(i);
      const std::string& t = x.def->type;
      if (t == "func.entry" || t == "func.return" || t == "func.call" || t == "func.pure")
        if (!function_of(i)) return fail(title(i) + " names a function that is not there", i);
      if (t == "var.get" || t == "var.set" || t == "var.add")
        if (!g.variable(x.node->param("variable"))) return fail(title(i) + " names a variable that is not there", i);
      if (x.def->problem)
        if (const std::string why = x.def->problem(*x.node, g); !why.empty()) return fail(title(i) + ": " + why, i);
      for (const ParamDef& pd : x.def->params) {
        if (pd.kind != ParamDef::Kind::type) continue;
        const std::string value = x.node->param(pd.key, pd.def);
        bool known = false;
        for (const TypeRef& choice : type_choices(pd, g)) known = known || value == type_text(choice);
        if (!known) return fail(title(i) + ": \"" + value + "\" is no " + (pd.enums_only ? "enum of this Blueprint" : pd.structs_only ? "struct of this Blueprint" : "type it can have"), i);
      }
      for (int k : x.data_in) {
        // (an enum's value typed into a pin is one of its values)
        const PinDef& pin = x.in[static_cast<std::size_t>(k)];
        const json::Value* typed = x.node->value(pin.id);
        if (typed && !typed_ok(title(i) + ": \"" + pin.id + "\"", pin.type, pin.of, *typed)) return false;
      }
      if (t == "math.formula") {
        const std::string text = x.node->param("formula", "a + b");
        const Formula& f = Formula::cached(text);
        for (const std::string& input : f.inputs())
          if (!cpp_name_ok(input)) return fail(title(i) + ": \"" + input + "\" cannot be an input's name (a word of C++ or of the notation)", i);
      }
      if (x.event && t != "func.entry")
        for (int k : x.data_out)
          if (names.count(x.out[static_cast<std::size_t>(k)].id))
            return fail("\"" + x.out[static_cast<std::size_t>(k)].id + "\" is an output of " + title(i) + ": a variable or function cannot have that name", i);
      if (x.form == Form::give) {
        // on a path of its own function: in that function's own body
        const bool in_body = !x.own && x.place >= 0 && ctx[static_cast<std::size_t>(x.place)].first == kCallable;
        const int owner = in_body ? ctx[static_cast<std::size_t>(x.place)].second : -1;
        if (owner < 0 || at(owner).def->type != "func.entry" || at(owner).node->param("function") != x.node->param("function"))
          return fail("Return (node " + std::to_string(x.node->id) +
                          ") is not on a path of its own function: not after a wait (Delay), not in an event, and paths that meet before it must all come from one "
                          "Branch or Switch",
                      i);
      }
      for (int k : x.data_out) {
        const std::string& id = x.out[static_cast<std::size_t>(k)].id;
        PinType type;
        if (id.empty() || !is_start(id[0]) || cpp_keyword(id) || type_of_cpp(id, type) || !std::all_of(id.begin(), id.end(), is_part))
          return fail(title(i) + ": the output \"" + id + "\" has no name C++ can write", i);
      }
    }
    return paths_ok() && functions_ok();
  }
};

// ---------------------------------------------------------------------------
// writing

struct Writer {
  Shape& s;
  std::string out;
  std::vector<int> order;  // the nodes in the order the text names them

  explicit Writer(Shape& shape) : s(shape) {}

  Info& at(int i) { return s.at(i); }
  static std::string pad(int ind) { return std::string(static_cast<std::size_t>(ind) * 2, ' '); }
  void line(int ind, const std::string& text) { out += pad(ind) + text + "\n"; }
  std::string id(int x) { return std::to_string(at(x).node->id); }

  /// The name of a node's function with its settings: flow_tween<"smooth">.
  std::string callee(int x) { return node_name(at(x).def->type) + settings_of(*at(x).def, *at(x).node, s.g); }

  std::string literal_of(int x, int k) {
    const Info& X = at(x);
    const PinDef& p = X.in[static_cast<std::size_t>(k)];
    const json::Value* typed = X.node->value(p.id);
    return literal(Value::from_json(typed ? *typed : p.def, p.type, s.g, p.of), p.of, s.g);
  }

  /// What input `k` of `x` is given, as an expression standing in the body `c`.
  std::string value(int x, int k, int c) {
    const auto [from, o] = at(x).src[static_cast<std::size_t>(k)];
    if (from < 0) return literal_of(x, k);
    const PinDef pin = at(from).out[static_cast<std::size_t>(o)];
    std::string e;
    if (at(from).pure) {
      // (a function's value and a Break name what is read, whatever they give: Calc(x).r, struct_break<Weapon>(Held).Damage)
      const std::string& kind = at(from).def->type;
      const std::string suffix = kind == "func.pure" || kind == "struct.break" || at(from).data_out.size() > 1 ? "." + pin.id : std::string();
      e = (at(from).named ? "n" + id(from) + "()" : pure(from, c)) + suffix;
    } else if (at(from).event && at(from).place == c) {
      e = pin.id;  // an event's or a function's own value, inside its own body: the parameter
    } else {
      e = "n" + id(from) + "." + pin.id;
    }
    const PinType want = at(x).in[static_cast<std::size_t>(k)].type;
    return pin.type == want ? e : std::string("kern::to<") + cpp_type(want) + ">(" + e + ")";
  }

  std::string inputs(int x, int c, const std::vector<std::string>& first = {}) {
    std::string r;
    for (const std::string& a : first) r += (r.empty() ? "" : ", ") + a;
    const std::vector<int> pins = at(x).data_in;
    for (int k : pins) {
      const std::string v = value(x, k, c);
      r += (r.empty() ? "" : ", ") + v;
    }
    return r;
  }

  /// The expression a value node is.
  std::string pure(int x, int c) {
    order.push_back(x);
    const std::string t = at(x).def->type;
    if (t == "var.get") return at(x).node->param("variable");
    if (t == "func.pure") {
      const std::string args = inputs(x, c);
      return at(x).node->param("function") + "(" + args + ")";
    }
    if (t == "math.formula") {
      const std::string text = at(x).node->param("formula", "a + b");
      const Formula& f = Formula::cached(text);
      std::string lambda = "[](";
      for (std::size_t i = 0; i < f.inputs().size(); ++i) lambda += (i ? ", Float " : "Float ") + f.inputs()[i];
      lambda += ") -> Float { return " + f.cpp() + "; }";
      const std::string args = inputs(x, c, {lambda});
      return "kern::formula(" + args + ")";
    }
    std::vector<std::string> head;
    if (at(x).member) head.push_back("n" + id(x));
    for (std::string& v : variables_of(*at(x).def, *at(x).node)) head.push_back(std::move(v));
    const std::string args = inputs(x, c, head);
    return callee(x) + "(" + args + ")";
  }

  /// The values several inputs of one body read, each after those it reads.
  std::vector<int> named_in(int c) {
    std::vector<int> sorted;
    std::set<int> done;
    // (a value reads another through values written in place: those are looked through)
    std::function<void(int)> inputs_first = [&](int x) {
      for (const auto& src : at(x).src) {
        const int f = src.first;
        if (f < 0 || !at(f).pure) continue;
        if (!at(f).named) { inputs_first(f); continue; }
        if (at(f).place != c || done.count(f)) continue;
        done.insert(f);
        inputs_first(f);
        sorted.push_back(f);
      }
    };
    for (int i : s.by_id) {
      if (!at(i).pure || !at(i).named || at(i).place != c || done.count(i)) continue;
      done.insert(i);
      inputs_first(i);
      sorted.push_back(i);
    }
    return sorted;
  }
  void named_values(int c, int ind) {
    for (int i : named_in(c)) {
      const std::string e = pure(i, c);
      if (c == kMember) line(ind, "auto n" + id(i) + "() { return " + e + "; }");
      else line(ind, "const auto n" + id(i) + " = [&] { return " + e + "; };");
    }
  }

  /// The statements from the input `entry` of `x` on, until the path ends. True: every way
  /// through them ends in a return (as the C++ compiler sees it).
  bool chain(int x, int entry, int c, int ind, bool continued = false) {
    while (x >= 0) {
      if (at(x).own) {
        line(ind, "go" + id(x) + "(" + (at(x).exec_in.size() >= 2 ? text_literal(at(x).in[static_cast<std::size_t>(entry)].id) : std::string()) + ");");
        return false;
      }
      if (at(x).merged && !continued) return false;  // (written after the step whose paths meet here)
      continued = false;
      bool returns = false;
      const int next = statement(x, entry, c, ind, false, returns);
      if (at(x).merge >= 0) {
        x = at(x).merge;
        entry = at(x).exec_in.front();
        continued = true;
        continue;
      }
      if (next < 0) return returns;
      std::tie(x, entry) = at(x).dst[static_cast<std::size_t>(next)];
    }
    return false;
  }
  bool follow(int x, std::size_t exec_ordinal, int c, int ind) {
    const auto [t, entry] = at(x).dst[exec_ordinal];
    return t >= 0 ? chain(t, entry, c, ind) : false;
  }

  /// The call of a step: its member, the exec input it is entered through when it has several,
  /// the variable it changes, then what its inputs are given.
  std::string call(int x, int entry, int c, bool definition) {
    std::vector<std::string> head;
    if (at(x).member) head.push_back("n" + id(x));
    if (at(x).exec_in.size() >= 2) head.push_back(definition ? std::string("entry") : text_literal(at(x).in[static_cast<std::size_t>(entry)].id));
    for (std::string& v : variables_of(*at(x).def, *at(x).node)) head.push_back(std::move(v));
    const std::string args = inputs(x, c, head);
    return callee(x) + "(" + args + ")";
  }

  /// Writes the step `x`; returns the exec output (ordinal) the next statement hangs on, or -1
  /// when the path ends here or has been written. `returns`: it always ends in a return.
  int statement(int x, int entry, int c, int ind, bool definition, bool& returns) {
    order.push_back(x);
    const Info& X = at(x);
    const std::string t = X.def->type;
    const std::size_t outs = X.exec_out.size();
    returns = false;
    switch (X.form) {
      case Form::give: {
        const std::string values = inputs(x, c);
        line(ind, X.data_in.empty() ? "return;" : "return {" + values + "};");
        returns = true;
        return -1;
      }
      case Form::branch: {
        if (X.guard) {
          // The True path always returns: what the False path does follows the if (no else).
          line(ind, "if (" + value(x, X.data_in.front(), c) + ") {");
          follow(x, 0, c, ind + 1);
          line(ind, "}");
          return 1;
        }
        // if / else if / else
        bool all = true;
        std::string head = "if (";
        for (int b = x;;) {
          line(ind, head + value(b, at(b).data_in.front(), c) + ") {");
          all = follow(b, 0, c, ind + 1) && all;
          const int f = at(b).dst[1].first;
          // (the False path has nothing of its own: no wire, or straight to where paths meet)
          if (f < 0 || at(f).merged) { all = false; break; }
          if (!at(f).own && at(f).form == Form::branch && at(f).merge < 0 && !at(f).guard) {
            order.push_back(f);  // the False path is one more Branch: else if
            head = "} else if (";
            b = f;
            continue;
          }
          line(ind, "} else {");
          all = follow(b, 1, c, ind + 1) && all;
          break;
        }
        line(ind, "}");
        returns = all;
        return -1;
      }
      case Form::sequence:
        for (std::size_t k = 0; k < outs; ++k) {
          line(ind, "{");
          if (follow(x, k, c, ind + 1)) returns = true;
          line(ind, "}");
        }
        return -1;
      case Form::loop_while:
        line(ind, "while (loop(" + value(x, X.data_in.front(), c) + ")) {");
        follow(x, 0, c, ind + 1);
        line(ind, "}");
        return outs > 1 ? 1 : -1;
      case Form::loop: {
        const std::string it = "it" + id(x);
        line(ind, "for (auto " + it + " = " + call(x, entry, c, definition) + "; " + it + ".next();) {");
        follow(x, 0, c, ind + 1);
        line(ind, "}");
        return outs > 1 ? 1 : -1;
      }
      case Form::latent: {
        // what follows runs later: bodies of their own (lambdas that keep nothing but this)
        std::string open = call(x, entry, c, definition);
        open.pop_back();
        if (open.back() != '(') open += ", ";
        auto later = [&](std::size_t k, int at_ind) {
          const int target = X.dst[k].first;
          if (target < 0) return;
          if (!at(target).own) named_values(at(target).place, at_ind);
          follow(x, k, at(target).own ? c : at(target).place, at_ind);
        };
        if (outs == 1) {
          line(ind, open + "[this] {");
          later(0, ind + 1);
          line(ind, "});");
          return -1;
        }
        line(ind, open + "{");
        for (std::size_t k = 0; k < outs; ++k) {
          if (X.dst[k].first < 0) continue;
          line(ind + 1, "{" + text_literal(X.out[static_cast<std::size_t>(X.exec_out[k])].id) + ", [this] {");
          later(k, ind + 2);
          line(ind + 1, "}},");
        }
        line(ind, "});");
        return -1;
      }
      case Form::test:
        line(ind, "if (" + call(x, entry, c, definition) + ") {");
        follow(x, 0, c, ind + 1);
        line(ind, "}");
        return -1;
      case Form::choice: {
        // Switch on Enum: the cases are the enum's values, by their names, and every one of them
        // is written - as C++ wants a switch over an enum to be.
        const EnumType* values = t == "flow.switch_enum" ? s.g.enumeration(X.node->param("enum")) : nullptr;
        line(ind, "switch (" + call(x, entry, c, definition) + ") {");
        for (std::size_t k = 0; k < outs; ++k) {
          const int target = X.dst[k].first;
          if (values && k < values->values.size()) {
            line(ind + 1, "case " + values->name + "::" + values->values[k] + ": {");
          } else {
            if (target < 0 || at(target).merged) continue;  // (nothing of its own on this path)
            line(ind + 1, "case " + std::to_string(k) + ": {  // " + X.out[static_cast<std::size_t>(X.exec_out[k])].id);
          }
          follow(x, k, c, ind + 2);
          line(ind + 1, "} break;");
        }
        line(ind, "}");
        return -1;
      }
      case Form::end:
        line(ind, call(x, entry, c, definition) + ";");
        return -1;
      default: break;
    }
    // a plain step
    if (t == "var.set" || t == "var.add") {
      // Heat = value;   or, when what the node gives is read: n5.value = (Heat = value);
      const std::string set = X.node->param("variable") + (t == "var.set" ? " = " : " += ") + value(x, X.data_in.front(), c);
      line(ind, X.member ? "n" + id(x) + "." + X.out[static_cast<std::size_t>(X.data_out.front())].id + " = (" + set + ");" : set + ";");
      return 0;
    }
    if (t == "struct.set_field") {
      // Held.Damage = value;   or, when what the node gives is read: n5.value = (Held.Damage = value);
      const std::string set = X.node->param("variable") + "." + X.node->param("field") + " = " + value(x, X.data_in.front(), c);
      line(ind, X.member ? "n" + id(x) + "." + X.out[static_cast<std::size_t>(X.data_out.front())].id + " = (" + set + ");" : set + ";");
      return 0;
    }
    if (t == "func.call") {
      const std::string args = inputs(x, c);
      line(ind, (X.member ? "n" + id(x) + " = " : std::string()) + X.node->param("function") + "(" + args + ");");
      return 0;
    }
    line(ind, call(x, entry, c, definition) + ";");
    return 0;
  }

  static std::string fields(const std::vector<PinDef>& pins, const std::vector<int>& which, const Graph& g) {
    std::string r;
    for (int k : which) {
      const PinDef& p = pins[static_cast<std::size_t>(k)];
      r += cpp_type(TypeRef{p.type, p.of}) + " " + p.id + " = " + literal(zero_of({p.type, p.of}, g), p.of, g) + "; ";
    }
    return r;
  }
  /// What a node's member holds of its outputs: all of a step's and an event's (a value is
  /// what its call gives - a value's member is its memory only).
  static std::vector<int> member_fields(const Info& x) { return x.pure ? std::vector<int>() : x.data_out; }
  /// The declaration of a node's member n<id>.
  static std::string member_text(const Info& x, const Graph& g) {
    const std::string id = std::to_string(x.node->id);
    const std::string base = x.def->memory && !x.event ? "kern::Node<" + id + ">" : std::string();
    if (x.def->type == "func.call") return x.node->param("function") + "Result n" + id + ";";
    if (member_fields(x).empty()) return base + " n" + id + ";";
    return "struct " + (base.empty() ? std::string() : ": " + base + " ") + "{ " + fields(x.out, member_fields(x), g) + "} n" + id + ";";
  }
  void doc(int ind, const std::string& help) {
    std::size_t from = 0;
    while (!help.empty() && from <= help.size()) {
      const std::size_t nl = help.find('\n', from);
      const std::string text = help.substr(from, nl == std::string::npos ? std::string::npos : nl - from);
      line(ind, text.empty() ? "///" : "/// " + text);
      if (nl == std::string::npos) break;
      from = nl + 1;
    }
  }
  /// The body of an event or a function: its values published when they are read elsewhere, the
  /// values its statements share, then the statements.
  bool body(int e, int ind) {
    order.push_back(e);
    if (at(e).member) {
      std::string names;
      for (int k : at(e).data_out) names += (names.empty() ? "" : ", ") + at(e).out[static_cast<std::size_t>(k)].id;
      line(ind, "n" + id(e) + " = {" + names + "};");
    }
    named_values(at(e).place, ind);
    return at(e).exec_out.empty() ? false : follow(e, 0, at(e).place, ind);
  }

  static std::string struct_name(const Graph& g, const std::string& name) {
    std::string type;
    for (char ch : name) type += is_part(ch) ? ch : '_';
    if (type.empty() || is_digit(type[0])) type = "Blueprint_" + type;
    bool taken = !cpp_name_ok(type) || g.variable(type) || g.function(type) || g.enumeration(type) || g.structure(type);
    for (const Function& f : g.functions) taken = taken || f.name + "Result" == type;
    return taken ? type + "_Blueprint" : type;
  }

  void write(const std::string& name, bool layout = true) {
    const Graph& g = s.g;
    const std::string type = struct_name(g, name);
    out += "// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two\n";
    out += "// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).\n";
    out += "#include \"kern/logic/blueprint.h\"\n\n";
    out += "struct " + type + " : kern::Blueprint {\n";
    bool any = false;
    auto section = [&](const char* title) {
      if (any) out += "\n";
      any = true;
      line(1, std::string("// ---- ") + title + " ----");
    };
    if (!g.enums.empty() || !g.structs.empty()) {
      section("types");
      for (const EnumType& e : g.enums) {
        doc(1, e.help);
        std::string values;
        for (const std::string& v : e.values) values += ", " + v;
        line(1, "KERN_ENUM(" + e.name + values + ");");
      }
      // (the structs after the enums: a field is of a type declared above it)
      for (const StructType& st : g.structs) {
        doc(1, st.help);
        line(1, "KERN_STRUCT(" + st.name + ") {");
        for (const StructField& f : st.fields) line(2, cpp_type(f.type) + " " + f.name + " = " + literal(Value::from_json(f.value, f.type.type, g, f.type.of), f.type.of, g) + ";");
        line(1, "};");
      }
    }
    if (!g.variables.empty()) {
      section("variables");
      for (const Variable& v : g.variables) {
        doc(1, v.help);
        line(1, cpp_type(TypeRef{v.type, v.of}) + " " + v.name + " = " + literal(Value::from_json(v.value, v.type, g, v.of), v.of, g) + ";");
      }
    }
    if (!g.machines.empty()) {
      section("state machines");
      for (const StateMachine& m : g.machines) {
        std::string text = "kern::Machine machine" + std::to_string(m.id) + " = machine(" + text_literal(m.name) + ")";
        const std::string br = "\n" + pad(3) + ".";
        for (const State& st : m.states)
          text += br + "state(" + std::to_string(st.id) + ", " + text_literal(st.name) + ", " + place(st.x) + ", " + place(st.y) + (st.initial ? ", true)" : ")");
        for (const Transition& t : m.transitions) {
          const std::string head = std::to_string(t.id) + ", " + std::to_string(t.from) + ", " + std::to_string(t.to);
          const std::string label = t.label.empty() ? std::string() : ", " + text_literal(t.label);
          switch (t.trigger) {
            case TriggerKind::event: text += br + "on_event(" + head + ", " + text_literal(t.event) + label + ")"; break;
            case TriggerKind::timer: text += br + "after(" + head + ", " + f32_text(t.seconds) + label + ")"; break;
            case TriggerKind::condition: text += br + "when(" + head + ", " + text_literal(t.variable) + label + ")"; break;
            case TriggerKind::automatic: text += br + "automatic(" + head + label + ")"; break;
          }
        }
        line(1, text + ";");
      }
    }
    if (!named_in(kMember).empty()) {
      section("values several bodies read");
      named_values(kMember, 1);
    }
    if (!g.functions.empty()) {
      section("functions");
      bool first = true;
      for (const Function& f : g.functions) {
        int e = -1;
        for (int i : s.by_id)
          if (at(i).def->type == "func.entry" && at(i).node->param("function") == f.name) e = i;
        if (!first) out += "\n";
        first = false;
        if (!f.outputs.empty()) {
          std::string members;
          for (const FunctionPin& p : f.outputs) members += cpp_type(TypeRef{p.type, p.of}) + " " + p.name + " = " + literal(Value::from_json(p.value, p.type, g, p.of), p.of, g) + "; ";
          line(1, "struct " + f.name + "Result { " + members + "};");
        }
        doc(1, f.help);
        std::string args;
        for (const FunctionPin& p : f.inputs) args += (args.empty() ? "" : ", ") + cpp_type(TypeRef{p.type, p.of}) + " " + p.name + " = " + literal(Value::from_json(p.value, p.type, g, p.of), p.of, g);
        line(1, std::string(f.pure ? "KERN_PURE " : "") + (f.outputs.empty() ? std::string("void") : f.name + "Result") + " " + f.name + "(" + args + ") {");
        // (a path that ends without a Return gives the outputs' defaults)
        if (!body(e, 2) && !f.outputs.empty()) line(2, "return {};");
        line(1, "}");
      }
    }
    bool events = false;
    for (int i : s.by_id) {
      if (!at(i).event || at(i).def->type == "func.entry") continue;
      if (!events) section("events");
      else out += "\n";
      events = true;
      line(1, "KERN_EVENT(on" + id(i) + ", " + callee(i) + ")");
      std::string args;
      for (int k : at(i).data_out) args += (args.empty() ? "" : ", ") + cpp_type(TypeRef{at(i).out[static_cast<std::size_t>(k)].type, at(i).out[static_cast<std::size_t>(k)].of}) + " " + at(i).out[static_cast<std::size_t>(k)].id;
      line(1, "void on" + id(i) + "(" + args + ") {");
      body(i, 2);
      line(1, "}");
    }
    bool steps = false;
    for (int i : s.by_id) {
      if (!at(i).own) continue;
      if (!steps) section("steps several wires lead to (or none)");
      else out += "\n";
      steps = true;
      const int c = at(i).place;
      line(1, "void go" + id(i) + (at(i).exec_in.size() >= 2 ? "(Text entry) {" : "() {"));
      named_values(c, 2);
      bool returns = false;
      const int next = statement(i, at(i).exec_in.empty() ? 0 : at(i).exec_in.front(), c, 2, true, returns);
      if (at(i).merge >= 0) chain(at(i).merge, at(at(i).merge).exec_in.front(), c, 2, true);
      else if (next >= 0) follow(i, static_cast<std::size_t>(next), c, 2);
      line(1, "}");
    }
    bool loose = false;
    for (int i : s.by_id) loose = loose || (at(i).pure && at(i).readers.empty());
    if (loose) {
      section("values nothing reads");
      line(1, "void detached() {");
      named_values(s.detached, 2);
      for (int i : s.by_id)
        if (at(i).pure && at(i).readers.empty()) {
          const std::string e = pure(i, s.detached);
          line(2, "(void)" + e + ";");
        }
      line(1, "}");
    }
    bool members = false;
    for (int i : s.by_id) {
      if (!at(i).member) continue;
      if (!members) section("what nodes give and keep");
      members = true;
      line(1, member_text(at(i), g));
    }
    out += "};\n";
    if (!layout) return;
    // Where the nodes are in the graph view, and what is noted on them: in the order the text
    // names them.
    out += "\nKERN_LAYOUT(" + type + ") {\n";
    for (int x : order) {
      const Node& node = *at(x).node;
      line(1, "node(" + std::to_string(node.id) + ", " + place(node.x) + ", " + place(node.y) + (node.comment.empty() ? std::string() : ", " + text_literal(node.comment)) + ");");
    }
    for (const CommentBox& c : g.comments) {
      char color[24] = "";
      if (c.color) std::snprintf(color, sizeof color, ", 0x%06x", c.color & 0xffffffu);
      line(1, "box(" + std::to_string(c.id) + ", " + place(c.x) + ", " + place(c.y) + ", " + place(c.w) + ", " + place(c.h) + ", " + text_literal(c.text) + color + ");");
    }
    line(1, "next_id(" + std::to_string(g.next_id) + ");");
    out += "}\n";
  }
};

// ---------------------------------------------------------------------------
// reading

const char* const kTypes = "a type (Bool, Int, Float, Vector, Text, Actor, an enum or a struct of the Blueprint, or List<one of them>) expected, not ";
const char* const kListTypes = "List<Bool>, List<Int>, List<Float>, List<Vector>, List<Text>, List<Actor>, List<an enum> or List<a struct of the Blueprint>";

struct Tok {
  enum Kind : u8 { id, number, text, punct, doc, end } kind = end;
  std::string s;            // the name, the digits, the text's content, the punctuation, the doc line
  bool real = false;        // number: has a point or an exponent
  std::size_t begin = 0, stop = 0;
  int line = 1, col = 1;
};

struct Reader {
  std::string_view text;
  const Library& lib;
  std::vector<Tok> toks;
  std::size_t p = 0;
  std::string error;

  // what the text declares
  Graph g;
  std::string name;
  std::unordered_map<std::string, const NodeDef*> node_fns;
  struct MemberDecl {
    bool memory = false, result = false;
    std::string result_of;
    std::vector<std::pair<std::string, TypeRef>> fields;
    std::size_t tok = 0;
  };
  std::map<u32, MemberDecl> members;
  std::map<std::string, std::pair<std::vector<std::pair<std::string, TypeRef>>, std::vector<Value>>> results;  // function -> its Result struct
  struct Item {  // a body, read in the second pass
    enum Kind : u8 { shared, function, event, go, loose } kind = shared;
    std::size_t open = 0;        // the token after its "{"
    u32 id = 0;                  // shared, event, go
    std::string fn;              // function: its name; event: the node's function
    std::vector<std::string> params;                    // event: its settings
    std::vector<std::pair<std::string, TypeRef>> args;  // event: the method's parameters
    bool entry = false;          // go: takes the exec input it is entered through
    std::size_t tok = 0;
  };
  std::vector<Item> items;

  // the nodes, in the order the text names them
  struct Made {
    Node node;
    const NodeDef* def = nullptr;
    std::vector<PinDef> in, out;
    u32 declared = 0;
    std::size_t tok = 0;
    int body = 0, column = 0, row = 0;
    bool pure = false;
  };
  std::vector<Made> made;
  std::unordered_map<u32, int> declared;
  struct Src {
    bool literal = false;
    Value value;
    int node = -1;      // a node of this text, by the order it was made in
    u32 id = 0;         // or a node by its name n<id>
    bool call = false;  // n<id>(): a value with a name
    std::string pin;    // empty: its one output
    bool cast = false;
    PinType to = PinType::number;
    std::string of;     // a value written out: of which enum (when it is an enum's)
    std::size_t tok = 0;
    bool root = false;  // the expression is one new value node (what a name can be given to)
  };
  struct DataWire { int to; std::string in; Src from; };
  struct ExecWire { int from; std::string out; int to; u32 to_id; std::string in; std::size_t tok; };
  /// An exec output that waits for its wire. node < 0 (kGone): a path that has ended in the graph
  /// where C++ would go on with the next statement (after a wait, a Sequence, go<id>()).
  struct End { int node; std::string out; };
  using Ends = std::vector<End>;
  static constexpr int kGone = -2;
  static Ends gone() { return {{kGone, std::string()}}; }
  std::vector<DataWire> data_wires;
  std::vector<ExecWire> exec_wires;
  std::set<int> published;  // events / entries (made index) whose method begins with "n<id> = {...};"

  // while a body is read
  struct Scope {
    int root = -1;                       // the event / entry node whose parameters can be read here
    const Function* function = nullptr;  // its function: return gives values here
    bool entry = false;                  // the first statement may take "entry" (a go method's exec input)
    u32 go = 0;                          // the id the first statement's node gets
    bool first = true;                   // no statement yet
    int body = 0, column = 0, row = 0;
  } scope;
  bool entry_now = false;
  int bodies = 0;

  const Graph* before = nullptr;  // apply_cpp: the Blueprint this text changes

  Reader(std::string_view t, const Library& l) : text(t), lib(l) {}

  // ---- tokens ----
  bool fail(std::size_t tok, const std::string& why) {
    if (error.empty()) {
      const Tok& t = toks[std::min(tok, toks.size() - 1)];
      error = "line " + std::to_string(t.line) + ", column " + std::to_string(t.col) + ": " + why;
    }
    return false;
  }
  bool failed() const { return !error.empty(); }
  bool lex() {
    std::size_t i = 0;
    int line = 1, col = 1;
    auto step_over = [&](std::size_t count) {
      for (std::size_t k = 0; k < count && i < text.size(); ++k, ++i) {
        if (text[i] == '\n') { ++line; col = 1; } else ++col;
      }
    };
    auto bad = [&](Tok t, const std::string& why) {
      toks.push_back(std::move(t));
      return fail(toks.size() - 1, why);
    };
    while (i < text.size()) {
      const char c = text[i];
      if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { step_over(1); continue; }
      Tok t;
      t.begin = i;
      t.line = line;
      t.col = col;
      if (c == '#') {  // (#include: nothing of the Blueprint)
        while (i < text.size() && text[i] != '\n') step_over(1);
        continue;
      }
      if (c == '/' && i + 1 < text.size() && text[i + 1] == '/') {
        const bool is_doc = i + 2 < text.size() && text[i + 2] == '/' && !(i + 3 < text.size() && text[i + 3] == '/');
        std::size_t e = i;
        while (e < text.size() && text[e] != '\n') ++e;
        if (is_doc) {
          std::size_t b = i + 3;
          if (b < e && text[b] == ' ') ++b;
          t.kind = Tok::doc;
          t.s = std::string(text.substr(b, e - b));
          if (!t.s.empty() && t.s.back() == '\r') t.s.pop_back();
          t.stop = e;
          toks.push_back(t);
        }
        step_over(e - i);
        continue;
      }
      if (c == '/' && i + 1 < text.size() && text[i + 1] == '*') {
        const std::size_t e = text.find("*/", i + 2);
        if (e == std::string_view::npos) return bad(t, "this comment does not end");
        step_over(e + 2 - i);
        continue;
      }
      if (is_start(c)) {
        std::size_t e = i;
        while (e < text.size() && is_part(text[e])) ++e;
        t.kind = Tok::id;
        t.s = std::string(text.substr(i, e - i));
        t.stop = e;
        toks.push_back(t);
        step_over(e - i);
        continue;
      }
      if (is_digit(c) || (c == '.' && i + 1 < text.size() && is_digit(text[i + 1]))) {
        std::size_t e = i;
        t.kind = Tok::number;
        if (c == '0' && e + 1 < text.size() && (text[e + 1] == 'x' || text[e + 1] == 'X')) {
          e += 2;
          while (e < text.size() && std::isxdigit(static_cast<unsigned char>(text[e]))) ++e;
        } else {
          while (e < text.size() && is_digit(text[e])) ++e;
          if (e < text.size() && text[e] == '.') {
            t.real = true;
            ++e;
            while (e < text.size() && is_digit(text[e])) ++e;
          }
          if (e < text.size() && (text[e] == 'e' || text[e] == 'E')) {
            std::size_t k = e + 1;
            if (k < text.size() && (text[k] == '+' || text[k] == '-')) ++k;
            if (k < text.size() && is_digit(text[k])) {
              while (k < text.size() && is_digit(text[k])) ++k;
              e = k;
              t.real = true;
            }
          }
        }
        t.s = std::string(text.substr(i, e - i));
        t.stop = e;
        if (e < text.size() && is_part(text[e])) return bad(t, "a number cannot go on with \"" + std::string(1, text[e]) + "\" (no suffixes)");
        toks.push_back(t);
        step_over(e - i);
        continue;
      }
      if (c == '"') {
        std::size_t e = i + 1;
        t.kind = Tok::text;
        bool closed = false;
        while (e < text.size() && text[e] != '\n') {
          const char ch = text[e];
          if (ch == '"') { closed = true; ++e; break; }
          if (ch != '\\') { t.s += ch; ++e; continue; }
          if (e + 1 >= text.size()) break;
          const char esc = text[e + 1];
          e += 2;
          if (esc == 'n') t.s += '\n';
          else if (esc == 't') t.s += '\t';
          else if (esc == 'r') t.s += '\r';
          else if (esc == '"' || esc == '\\' || esc == '\'') t.s += esc;
          else if (esc >= '0' && esc <= '7') {
            int v = esc - '0';
            for (int k = 0; k < 2 && e < text.size() && text[e] >= '0' && text[e] <= '7'; ++k) v = v * 8 + (text[e++] - '0');
            t.s += static_cast<char>(v);
          } else {
            return bad(t, std::string("a text cannot contain \\") + esc + " (write \\n, \\t, \\r, \\\", \\\\ or an octal \\ooo)");
          }
        }
        t.stop = e;
        if (!closed) return bad(t, "this text does not end (\" is missing)");
        toks.push_back(t);
        step_over(e - i);
        continue;
      }
      static const char* const kTwo[] = {"::", "->", "+=", "&&", "||", "==", "!=", "<=", ">="};
      t.kind = Tok::punct;
      t.s = std::string(1, c);
      for (const char* two : kTwo)
        if (text.substr(i, 2) == two) t.s = two;
      t.stop = i + t.s.size();
      toks.push_back(t);
      step_over(t.s.size());
    }
    Tok e;
    e.begin = e.stop = text.size();
    e.line = line;
    e.col = col;
    toks.push_back(e);
    return true;
  }
  const Tok& peek(std::size_t ahead = 0) const { return toks[std::min(p + ahead, toks.size() - 1)]; }
  bool is(std::string_view s, std::size_t ahead = 0) const {
    const Tok& t = peek(ahead);
    return (t.kind == Tok::punct || t.kind == Tok::id) && t.s == s;
  }
  bool accept(std::string_view s) {
    if (!is(s)) return false;
    ++p;
    return true;
  }
  static std::string shown(const Tok& t) {
    if (t.kind == Tok::end) return "the end of the text";
    if (t.kind == Tok::text) return "a text";
    if (t.kind == Tok::doc) return "a /// comment";
    return "\"" + t.s + "\"";
  }
  bool expect(std::string_view s, const std::string& why = {}) {
    if (accept(s)) return true;
    return fail(p, "\"" + std::string(s) + "\" expected" + (why.empty() ? std::string() : " " + why) + ", not " + shown(peek()));
  }
  bool word(std::string& out, const std::string& what) {
    if (peek().kind != Tok::id) return fail(p, what + " expected, not " + shown(peek()));
    out = toks[p++].s;
    return true;
  }
  bool integer(i64& out, const std::string& what) {
    const bool minus = accept("-");
    if (peek().kind != Tok::number || peek().real) return fail(p, what + " (a whole number) expected, not " + shown(peek()));
    out = static_cast<i64>(std::strtoull(toks[p++].s.c_str(), nullptr, 0));
    if (minus) out = -out;
    return true;
  }
  bool number(f64& out, const std::string& what) {
    const bool minus = accept("-");
    if (peek().kind != Tok::number) return fail(p, what + " (a number) expected, not " + shown(peek()));
    const Tok& t = toks[p++];
    out = t.real ? std::strtod(t.s.c_str(), nullptr) : static_cast<f64>(std::strtoull(t.s.c_str(), nullptr, 0));
    if (minus) out = -out;
    return true;
  }
  bool string(std::string& out, const std::string& what) {
    if (peek().kind != Tok::text) return fail(p, what + " (a text in quotes) expected, not " + shown(peek()));
    out = toks[p++].s;
    return true;
  }
  /// Skips to the token after the "}" that closes the "{" just read.
  bool skip_block() {
    int depth = 1;
    while (peek().kind != Tok::end) {
      if (is("{")) ++depth;
      else if (is("}") && --depth == 0) { ++p; return true; }
      ++p;
    }
    return fail(p, "\"}\" is missing");
  }

  // ---- literals ----
  /// The fields of a struct by their names: "Name, Type, Damage".
  static std::string field_names(const StructType& s) {
    std::string all;
    for (const StructField& f : s.fields) all += (all.empty() ? "" : ", ") + f.name;
    return all;
  }
  /// A literal: true, 12, -0.5, "text", Vector{x, y, z}, Actor{n}. False (nothing read): none here.
  /// A value written out. `of`: which enum or struct it is of, when it is one's value (or a list
  /// of them).
  bool literal_here(Value& v, std::string* of = nullptr) {
    const Tok& t = peek();
    if (of) of->clear();
    if (t.kind == Tok::id && is("{", 1) && g.structure(t.s)) {
      // Weapon{"Knife", Kind::Melee, 25.0}: a value for every field, in their order
      const StructType s = *g.structure(t.s);
      const std::string all = s.name + " has " + std::to_string(s.fields.size()) + " field(s) (" + field_names(s) + "): " + s.name + "{...} gives a value for each, in that order";
      p += 2;
      std::vector<Value> held;
      for (const StructField& f : s.fields) {
        if (is("}") || (!held.empty() && !accept(","))) { fail(p, all); return true; }
        Value item;
        if (!typed_literal(f.type, item, s.name + "." + f.name)) return true;
        held.push_back(std::move(item));
      }
      if (!is("}")) { fail(p, all); return true; }
      ++p;
      v = Value::structure(std::move(held));
      if (of) *of = s.name;
      return true;
    }
    if (t.kind == Tok::id && is("::", 1) && g.enumeration(t.s)) {
      // Mode::Open
      const EnumType& e = *g.enumeration(t.s);
      p += 2;
      std::string name;
      const std::size_t at = p;
      if (!word(name, "a value of " + e.name)) return true;
      const i64 index = enums::find(e.values, name);
      if (index < 0) {
        std::string all;
        for (const std::string& x : e.values) all += (all.empty() ? "" : ", ") + x;
        fail(at, e.name + " has no value \"" + name + "\" (" + all + ")");
        return true;
      }
      v = Value::enumeration(index);
      if (of) *of = e.name;
      return true;
    }
    if (t.kind == Tok::text) { v = Value::text(toks[p++].s); return true; }
    if (t.kind == Tok::id && (t.s == "true" || t.s == "false")) { v = Value::boolean(toks[p++].s == "true"); return true; }
    const bool minus = is("-") && peek(1).kind == Tok::number;
    if (t.kind == Tok::number || minus) {
      if (minus) ++p;
      const Tok& n = toks[p++];
      if (n.real) {
        const f64 x = std::strtod(n.s.c_str(), nullptr);
        v = Value::number(minus ? -x : x);
      } else {
        const u64 x = std::strtoull(n.s.c_str(), nullptr, 0);
        v = Value::integer(minus ? static_cast<i64>(0 - x) : static_cast<i64>(x));
      }
      return true;
    }
    if (t.kind == Tok::id && t.s == "Vector" && is("{", 1)) {
      p += 2;
      f64 x = 0, y = 0, z = 0;
      if (!number(x, "x") || !expect(",") || !number(y, "y") || !expect(",") || !number(z, "z") || !expect("}")) return true;
      v = Value::vector(Vec3{static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(z)});
      return true;
    }
    if (t.kind == Tok::id && t.s == "Actor" && is("{", 1)) {
      p += 2;
      i64 a = 0;
      if (!is("}") && !integer(a, "the actor's id")) return true;
      expect("}");
      v = Value::actor(static_cast<ActorRef>(a));
      return true;
    }
    if (t.kind == Tok::id && t.s == "List" && is("<", 1)) {
      // List<Float>{1.0, 2.5}
      TypeRef list;
      if (!type_here(list)) return true;
      if (!expect("{", "(a list written out is " + cpp_type(list) + "{...})")) return true;
      json::Value items = json::Value::array();
      std::vector<Value> structs;
      while (!is("}")) {
        Value item;
        if (!typed_literal({element_of(list.type), list.of}, item, "a list's item")) return true;
        if (list.type == PinType::structure_list) structs.push_back(std::move(item));
        else items.push(item.to_json());
        if (!accept(",")) break;
      }
      if (!expect("}")) return true;
      v = list.type == PinType::structure_list ? Value::structures(std::move(structs)) : Value::from_json(items, list.type);
      if (of) *of = list.of;
      return true;
    }
    return false;
  }
  /// A literal for a place of type `want` ("Float x = 1" is 1.0; nothing else converts).
  bool typed_literal(const TypeRef& want, Value& out, const std::string& what) {
    const std::size_t at = p;
    Value v;
    std::string of;
    if (!literal_here(v, &of)) return fail(p, what + ": a value of type " + cpp_type(want) + " expected, not " + shown(peek()));
    if (failed()) return false;
    if (v.type == PinType::integer && want.type == PinType::number) v = Value::number(static_cast<f64>(v.i));
    if (v.type != want.type || of != want.of)
      return fail(at, what + ": this is " + cpp_type(TypeRef{v.type, of}) + ", but " + cpp_type(want) + " is needed (like " + literal(zero_of(want, g), want.of, g) + ")");
    out = v;
    return true;
  }

  // ---- the first pass: what the struct declares ----
  /// A type: Float, an enum or a struct the Blueprint has declared, List<one of them>.
  bool type_here(TypeRef& t) {
    t = {};
    if (peek().kind != Tok::id) return false;
    if (type_of_cpp(peek().s, t.type)) { ++p; return true; }
    if (g.enumeration(peek().s) || g.structure(peek().s)) {
      t.type = g.enumeration(peek().s) ? PinType::enumeration : PinType::structure;
      t.of = toks[p++].s;
      return true;
    }
    if (is("List") && is("<", 1)) {
      // List<Float>: a list of values of one type
      const Tok inner = peek(2);
      PinType item = PinType::number;
      const bool value = inner.kind == Tok::id && type_of_cpp(inner.s, item);
      const bool of_enum = inner.kind == Tok::id && g.enumeration(inner.s), of_struct = inner.kind == Tok::id && g.structure(inner.s);
      if ((!value && !of_enum && !of_struct) || !is(">", 3)) return fail(p + 2, shown(inner) + " is no type a list holds: a list is " + kListTypes);
      p += 4;
      t = value ? TypeRef{list_of(item), {}} : TypeRef{of_enum ? PinType::enumeration_list : PinType::structure_list, inner.s};
      return true;
    }
    return false;
  }
  /// Type name = value; ... }   ("{" has been read)
  bool fields(std::vector<std::pair<std::string, TypeRef>>& out, std::vector<Value>* defaults, const std::string& what) {
    while (!is("}")) {
      TypeRef t;
      std::string field;
      if (!type_here(t)) return fail(p, what + ": " + kTypes + shown(peek()));
      if (!word(field, "the name")) return false;
      Value v = zero_of(t, g);
      const std::size_t at = p;
      if (accept("=") && !typed_literal(t, v, field)) return false;
      if (!expect(";")) return false;
      if (!defaults && !(v == zero_of(t, g))) return fail(at, "what a node gives starts empty: " + field + " = " + literal(zero_of(t, g), t.of, g));
      out.push_back({field, t});
      if (defaults) defaults->push_back(v);
    }
    return expect("}");
  }
  /// machine<id> = machine("name").state(...)...;   ("kern::Machine" has been read)
  bool machine_decl() {
    std::string var, what;
    const std::size_t at = p;
    if (!word(var, "machine<id>")) return false;
    const u32 id = numbered(var, "machine");
    if (!id) return fail(at, "a state machine is called machine<id> (machine3), not \"" + var + "\"");
    StateMachine m;
    m.id = id;
    if (!expect("=") || !expect("machine") || !expect("(") || !string(m.name, "the machine's name") || !expect(")")) return false;
    while (accept(".")) {
      const std::size_t call = p;
      if (!word(what, "state, on_event, after, when or automatic") || !expect("(")) return false;
      i64 a = 0;
      if (!integer(a, "the id")) return false;
      if (what == "state") {
        State st;
        st.id = static_cast<u32>(a);
        f64 x = 0, y = 0;
        if (!expect(",") || !string(st.name, "the state's name") || !expect(",") || !number(x, "x") || !expect(",") || !number(y, "y")) return false;
        st.x = static_cast<f32>(x);
        st.y = static_cast<f32>(y);
        if (accept(",")) {
          if (!expect("true", "(the initial state)")) return false;
          st.initial = true;
        }
        m.states.push_back(st);
      } else {
        Transition t;
        t.id = static_cast<u32>(a);
        i64 from = 0, to = 0;
        if (!expect(",") || !integer(from, "the state it leaves (0: any)") || !expect(",") || !integer(to, "the state it enters")) return false;
        t.from = static_cast<u32>(from);
        t.to = static_cast<u32>(to);
        if (what == "on_event") {
          t.trigger = TriggerKind::event;
          if (!expect(",") || !string(t.event, "the event's name")) return false;
        } else if (what == "after") {
          t.trigger = TriggerKind::timer;
          f64 seconds = 0;
          if (!expect(",") || !number(seconds, "the seconds")) return false;
          t.seconds = static_cast<f32>(seconds);
        } else if (what == "when") {
          t.trigger = TriggerKind::condition;
          if (!expect(",") || !string(t.variable, "the variable's name")) return false;
        } else if (what == "automatic") {
          t.trigger = TriggerKind::automatic;
        } else {
          return fail(call, "a state machine has state, on_event, after, when and automatic - not \"" + what + "\"");
        }
        if (accept(",") && !string(t.label, "the label")) return false;
        m.transitions.push_back(t);
      }
      if (!expect(")")) return false;
    }
    if (!expect(";")) return false;
    g.machines.push_back(std::move(m));
    return true;
  }
  /// kern::Node<id>
  bool node_base(bool& memory, u32& base_id) {
    i64 v = 0;
    if (!expect("kern") || !expect("::") || !expect("Node") || !expect("<") || !integer(v, "the node's id") || !expect(">")) return false;
    memory = true;
    base_id = static_cast<u32>(v);
    return true;
  }
  /// n<id>;   the name a member declaration ends with
  bool member_name(MemberDecl decl, u32 base_id) {
    std::string var;
    const std::size_t at = p;
    if (!word(var, "n<id>")) return false;
    const u32 id = numbered(var, "n");
    if (!id) return fail(at, "what a node gives or keeps is called n<id> (n12), not \"" + var + "\"");
    if (decl.memory && base_id != id) return fail(at, var + " keeps kern::Node<" + std::to_string(id) + ">, not kern::Node<" + std::to_string(base_id) + ">");
    if (members.count(id)) return fail(at, var + " is declared twice");
    decl.tok = at;
    members[id] = std::move(decl);
    return expect(";");
  }
  /// The settings of a node: <"a", "b"> (nothing when it has none). A setting that names a type
  /// is that type - <Float>, <List<Text>, "2"> - and is given by its name ("float"); `types`
  /// says which ones were written so.
  bool settings(std::vector<std::string>& out, std::vector<bool>* types = nullptr) {
    if (!accept("<")) return true;
    while (!is(">")) {
      std::string s;
      TypeRef t;
      const bool is_type = peek().kind == Tok::id;
      if (is_type) {
        if (!type_here(t)) return fail(p, std::string("a setting is a text (\"...\") or a type - ") + kTypes + shown(peek()));
        s = type_text(t);
      } else if (!string(s, "a setting")) {
        return false;
      }
      out.push_back(std::move(s));
      if (types) types->push_back(is_type);
      if (!accept(",")) break;
    }
    return expect(">");
  }
  bool declarations() {
    std::string docs;
    bool has_docs = false, pure = false, has_event = false;
    Item event;
    auto take_docs = [&] {
      std::string r = has_docs ? docs : std::string();
      docs.clear();
      has_docs = false;
      return r;
    };
    while (!failed()) {
      const Tok t = peek();
      if (t.kind == Tok::doc) {
        docs += (has_docs ? "\n" : "") + t.s;
        has_docs = true;
        ++p;
        continue;
      }
      if (is("}")) break;
      if (t.kind == Tok::end) return fail(p, "\"};\" is missing at the end of the struct");
      const std::size_t at = p;
      if (accept("KERN_ENUM")) {
        // KERN_ENUM(Mode, Closed, Opening, Open);   an enum of the Blueprint
        EnumType e;
        if (!expect("(") || !word(e.name, "the enum's name")) return false;
        if (g.enumeration(e.name) || g.structure(e.name) || g.variable(e.name) || g.function(e.name)) return fail(at + 2, "the name " + e.name + " is declared twice");
        while (accept(",")) {
          std::string value;
          const std::size_t where = p;
          if (!word(value, "a value's name")) return false;
          if (std::find(e.values.begin(), e.values.end(), value) != e.values.end()) return fail(where, e.name + " has the value " + value + " twice");
          e.values.push_back(std::move(value));
        }
        if (e.values.empty()) return fail(p, "an enum has values: KERN_ENUM(" + e.name + ", First, Second);");
        if (!expect(")") || !expect(";")) return false;
        e.help = take_docs();
        g.enums.push_back(std::move(e));
        continue;
      }
      if (accept("KERN_STRUCT")) {
        // KERN_STRUCT(Weapon) { Text Name = ""; Float Damage = 10.0; };   a struct of the Blueprint
        StructType s;
        if (!expect("(") || !word(s.name, "the struct's name")) return false;
        if (g.enumeration(s.name) || g.structure(s.name) || g.variable(s.name) || g.function(s.name)) return fail(at + 2, "the name " + s.name + " is declared twice");
        if (!expect(")") || !expect("{")) return false;
        while (!is("}")) {
          StructField f;
          // (a field is of a type declared above the struct: the struct itself is none of them)
          const Tok named = is("List") && is("<", 1) ? peek(2) : peek();
          if (named.kind == Tok::id && named.s == s.name) return fail(p, "the struct " + s.name + " cannot contain itself: a field is of a type declared above the struct");
          if (!type_here(f.type))
            return fail(p, "the struct " + s.name + ": " + shown(peek()) + " is no type here: a field is Bool, Int, Float, Vector, Text, Actor, an enum or a struct declared above this one, or a List of one of them");
          const std::size_t where = p;
          if (!word(f.name, "the field's name")) return false;
          if (s.field(f.name) >= 0) return fail(where, s.name + " has the field " + f.name + " twice");
          Value init = zero_of(f.type, g);
          if (accept("=") && !typed_literal(f.type, init, s.name + "." + f.name)) return false;
          if (!expect(";")) return false;
          f.value = init.to_json(g, f.type.of);
          s.fields.push_back(std::move(f));
        }
        if (s.fields.empty()) return fail(p, "a struct has fields: KERN_STRUCT(" + s.name + ") { Float Field = 0.0; };");
        if (s.fields.size() > StructType::kMaxFields) return fail(at + 2, s.name + " has " + std::to_string(s.fields.size()) + " fields: a struct has " + std::to_string(StructType::kMaxFields) + " at most");
        if (!expect("}") || !expect(";")) return false;
        s.help = take_docs();
        g.structs.push_back(std::move(s));
        continue;
      }
      TypeRef type;
      if (type_here(type)) {
        // a variable
        Variable v;
        v.type = type.type;
        v.of = type.of;
        if (!word(v.name, "the variable's name")) return false;
        Value init = zero_of(type, g);
        if (accept("=") && !typed_literal(type, init, v.name)) return false;
        if (!expect(";")) return false;
        if (g.variable(v.name)) return fail(at + 1, "the variable " + v.name + " is declared twice");
        v.value = init.to_json(g, type.of);
        v.help = take_docs();
        g.variables.push_back(std::move(v));
        continue;
      }
      if (is("kern") && is("::", 1) && is("Machine", 2)) {
        p += 3;
        take_docs();
        if (!machine_decl()) return false;
        continue;
      }
      if (is("kern") && is("::", 1) && is("Node", 2)) {
        MemberDecl d;
        u32 base = 0;
        take_docs();
        if (!node_base(d.memory, base) || !member_name(std::move(d), base)) return false;
        continue;
      }
      if (accept("struct")) {
        if (peek().kind == Tok::id) {
          // struct <Function>Result { ... };
          const std::string type_name = toks[p++].s;
          if (type_name.size() <= 6 || type_name.substr(type_name.size() - 6) != "Result")
            return fail(at + 1, "a struct with a name holds a function's results and is called <Function>Result");
          auto& r = results[type_name.substr(0, type_name.size() - 6)];
          if (!r.first.empty()) return fail(at + 1, type_name + " is declared twice");
          if (!expect("{") || !fields(r.first, &r.second, type_name) || !expect(";")) return false;
          if (r.first.empty()) return fail(at + 1, type_name + " is empty: a function without results is void");
          continue;
        }
        MemberDecl d;
        u32 base = 0;
        take_docs();
        if (accept(":") && !node_base(d.memory, base)) return false;
        if (!expect("{") || !fields(d.fields, nullptr, "what a node gives") || !member_name(std::move(d), base)) return false;
        continue;
      }
      if (accept("KERN_EVENT")) {
        std::string handler;
        take_docs();
        event = Item();
        event.kind = Item::event;
        event.tok = at;
        if (!expect("(") || !word(handler, "on<id>")) return false;
        event.id = numbered(handler, "on");
        if (!event.id) return fail(at + 2, "an event's method is called on<id> (on5), not \"" + handler + "\"");
        if (!expect(",") || !word(event.fn, "the event") || !settings(event.params) || !expect(")")) return false;
        has_event = true;
        continue;
      }
      if (accept("auto")) {
        // auto n<id>() { return value; }
        std::string var;
        take_docs();
        Item it;
        it.kind = Item::shared;
        it.tok = p;
        if (!word(var, "n<id>")) return false;
        it.id = numbered(var, "n");
        if (!it.id) return fail(at + 1, "a value several bodies read is called n<id> (n12), not \"" + var + "\"");
        if (!expect("(") || !expect(")") || !expect("{")) return false;
        it.open = p;
        items.push_back(std::move(it));
        if (!skip_block()) return false;
        continue;
      }
      if (accept("KERN_PURE")) {
        pure = true;
        continue;
      }
      if (t.kind != Tok::id) return fail(p, shown(t) + " is not part of a Blueprint");
      // void name(...) {   <Function>Result name(...) {   <Function>Result n<id>;
      std::string ret = toks[p++].s, fn;
      if (!word(fn, "a name")) return false;
      if (ret != "void" && numbered(fn, "n") && is(";")) {
        MemberDecl d;
        d.result = true;
        if (ret.size() <= 6 || ret.substr(ret.size() - 6) != "Result") return fail(at, "\"" + ret + "\" is no type here: what a called function gave is <Function>Result n<id>;");
        d.result_of = ret.substr(0, ret.size() - 6);
        --p;
        take_docs();
        if (!member_name(std::move(d), 0)) return false;
        continue;
      }
      if (ret != "void" && (ret.size() <= 6 || ret.substr(ret.size() - 6) != "Result"))
        return fail(at, "\"" + ret + "\" is no type here: a variable is Bool, Int, Float, Vector, Text, Actor, an enum or a struct declared above it, or a List of one of them; a function gives void or <Function>Result");
      if (!expect("(")) return false;
      Item it;
      if (ret == "void" && numbered(fn, "on")) {
        if (!has_event || event.id != numbered(fn, "on")) return fail(at + 1, fn + " needs its KERN_EVENT(" + fn + ", <event>) on the line above");
        it = event;
        has_event = false;
        while (!is(")")) {
          TypeRef pt;
          std::string arg;
          if (!type_here(pt)) return fail(p, kTypes + shown(peek()));
          if (!word(arg, "the parameter's name")) return false;
          it.args.push_back({arg, pt});
          if (!accept(",")) break;
        }
        take_docs();
      } else if (ret == "void" && numbered(fn, "go")) {
        it.kind = Item::go;
        it.id = numbered(fn, "go");
        if (accept("Text")) {
          if (!expect("entry", "(the exec input the step is entered through)")) return false;
          it.entry = true;
        }
        take_docs();
      } else if (ret == "void" && fn == "detached") {
        it.kind = Item::loose;
        take_docs();
      } else {
        // a function
        Function f;
        f.name = fn;
        f.pure = pure;
        if (g.function(fn)) return fail(at + 1, "the function " + fn + " is declared twice");
        if (ret != "void") {
          if (ret != fn + "Result") return fail(at, "the function " + fn + " gives " + fn + "Result (or is void), not \"" + ret + "\"");
          auto r = results.find(fn);
          if (r == results.end()) return fail(at, "struct " + ret + " { ... }; must be declared above the function");
          for (std::size_t k = 0; k < r->second.first.size(); ++k)
            f.outputs.push_back({r->second.first[k].first, r->second.first[k].second.type, r->second.second[k].to_json(g, r->second.first[k].second.of), r->second.first[k].second.of});
        }
        while (!is(")")) {
          FunctionPin in;
          TypeRef in_type;
          if (!type_here(in_type)) return fail(p, kTypes + shown(peek()));
          in.type = in_type.type;
          in.of = in_type.of;
          if (!word(in.name, "the input's name")) return false;
          Value v = zero_of(in_type, g);
          if (accept("=") && !typed_literal(in_type, v, in.name)) return false;
          in.value = v.to_json(g, in.of);
          f.inputs.push_back(std::move(in));
          if (!accept(",")) break;
        }
        f.help = take_docs();
        it.kind = Item::function;
        it.fn = fn;
        g.functions.push_back(std::move(f));
      }
      it.tok = at + 1;
      pure = false;
      if (!expect(")") || !expect("{")) return false;
      it.open = p;
      items.push_back(std::move(it));
      if (!skip_block()) return false;
    }
    if (failed()) return false;
    if (has_event) return fail(event.tok, "KERN_EVENT(on" + std::to_string(event.id) + ", ...) needs its method void on" + std::to_string(event.id) + "(...) on the next line");
    for (const auto& [fn, r] : results)
      if (!g.function(fn)) return fail(p, "struct " + fn + "Result has no function " + fn);
    return true;
  }

  // ---- nodes ----
  int make(const NodeDef* def, std::size_t tok, std::vector<std::pair<std::string, std::string>> params = {}) {
    Made m;
    m.def = def;
    m.node.type = def->type;
    m.node.params = std::move(params);
    m.tok = tok;
    m.pure = def->kind == NodeKind::pure;
    m.body = scope.body;
    if (!m.pure && def->kind != NodeKind::event) { ++scope.column; scope.row = 0; }
    m.column = scope.column;
    m.row = m.pure ? ++scope.row : 0;
    lib.pins(m.node, g, m.in, m.out);
    made.push_back(std::move(m));
    return static_cast<int>(made.size()) - 1;
  }
  int make(const char* type, std::size_t tok, std::vector<std::pair<std::string, std::string>> params = {}) {
    const NodeDef* def = lib.find(type);
    if (!def) { fail(tok, std::string("this library has no ") + type); return -1; }
    return make(def, tok, std::move(params));
  }
  static std::string settings_text(const NodeDef* def) {
    std::string want;
    std::size_t count = 0;
    for (const ParamDef& pd : def->params) {
      if (pd.kind == ParamDef::Kind::variable) continue;
      PinType type = PinType::number;
      want += (want.empty() ? "" : ", ") +
              (pd.kind == ParamDef::Kind::type ? (pd.enums_only ? std::string("AnEnum") : pd.structs_only ? std::string("AStruct") : pin_type_from_name(pd.def, type) ? std::string(cpp_type(type)) : std::string("Type"))
                                               : "\"" + pd.def + "\"");
      ++count;
    }
    return count == 0 ? " has no settings (no <...>)" : " has " + std::to_string(count) + " setting(s): " + node_name(def->type) + "<" + want + ">";
  }
  /// A node made from its function's name and settings (the name has been read). A setting that
  /// names a variable is the call's first value, the variable itself - after the member n<id>
  /// and the exec input: list_add(n7, Scores, 1.0). It is read ahead here (the node's pins
  /// depend on it) and taken in its place (variable_args).
  int make_named(const NodeDef* def, std::size_t tok) {
    std::vector<std::string> given;
    std::vector<bool> as_type;
    if (!settings(given, &as_type)) return -1;
    std::size_t written = 0, variables = 0;
    for (const ParamDef& pd : def->params) ++(pd.kind == ParamDef::Kind::variable ? variables : written);
    if (given.size() != written) { fail(tok, node_name(def->type) + settings_text(def)); return -1; }
    // (a type is written as a type, everything else as a text; and the type is one the setting can name)
    for (std::size_t k = 0, at = 0; k < def->params.size(); ++k) {
      const ParamDef& pd = def->params[k];
      if (pd.kind == ParamDef::Kind::variable) continue;
      bool fits = as_type[at] == (pd.kind == ParamDef::Kind::type);
      if (fits && pd.kind == ParamDef::Kind::type) {
        fits = false;
        for (const TypeRef& choice : type_choices(pd, g)) fits = fits || given[at] == type_text(choice);
      }
      if (!fits) { fail(tok, node_name(def->type) + settings_text(def)); return -1; }
      ++at;
    }
    std::vector<std::string> names;
    if (variables) {
      const std::string fname = node_name(def->type);
      if (!is("(")) { fail(p, fname + "(...) expected, not " + shown(peek())); return -1; }
      std::size_t q = 1;
      if (peek(q).kind == Tok::id && numbered(peek(q).s, "n") && is(",", q + 1)) q += 2;                        // (its member)
      if ((peek(q).kind == Tok::text || (peek(q).kind == Tok::id && peek(q).s == "entry")) && is(",", q + 1)) q += 2;  // (its exec input)
      for (std::size_t k = 0; k < variables; ++k, q += 2) {
        if (peek(q).kind != Tok::id || !g.variable(peek(q).s)) {
          fail(p + q, fname + " changes a variable: that variable is its first value - " + fname + "(Variable, ...) - not " + shown(peek(q)));
          return -1;
        }
        names.push_back(peek(q).s);
      }
    }
    std::vector<std::pair<std::string, std::string>> params;
    std::size_t g_at = 0, v_at = 0;
    for (const ParamDef& pd : def->params) params.push_back({pd.key, pd.kind == ParamDef::Kind::variable ? names[v_at++] : given[g_at++]});
    return make(def, tok, std::move(params));
  }
  bool declare(u32 id, int node, std::size_t tok) {
    if (id == 0 || node < 0) return !failed();
    Made& m = made[static_cast<std::size_t>(node)];
    if (m.declared && m.declared != id) return fail(tok, "this node is n" + std::to_string(m.declared) + " and n" + std::to_string(id) + " at once");
    auto it = declared.find(id);
    if (it != declared.end() && it->second != node) return fail(tok, "the number " + std::to_string(id) + " names two nodes");
    m.declared = id;
    declared[id] = node;
    return true;
  }
  std::vector<int> data_inputs(int node) const {
    std::vector<int> r;
    const Made& m = made[static_cast<std::size_t>(node)];
    for (std::size_t k = 0; k < m.in.size(); ++k) if (m.in[k].type != PinType::exec) r.push_back(static_cast<int>(k));
    return r;
  }
  std::vector<std::string> exec_pins(int node, bool inputs) const {
    std::vector<std::string> r;
    const Made& m = made[static_cast<std::size_t>(node)];
    for (const PinDef& pd : inputs ? m.in : m.out) if (pd.type == PinType::exec) r.push_back(pd.id);
    return r;
  }
  /// The exec output `k` of a node the notation has its own words for (if, while, =, a call).
  End out(int node, std::size_t k) const {
    const std::vector<std::string> outs = exec_pins(node, false);
    return {node, k < outs.size() ? outs[k] : std::string("?")};
  }
  /// Gives input `k` of `node` what `src` is.
  bool bind(int node, int k, const Src& src) {
    Made& m = made[static_cast<std::size_t>(node)];
    const PinDef pin = m.in[static_cast<std::size_t>(k)];
    if (!src.literal) {
      data_wires.push_back({node, pin.id, src});
      return true;
    }
    const TypeRef want{pin.type, pin.of};
    if (src.cast) return fail(src.tok, "a value written out needs no kern::to: write the " + cpp_type(want) + " itself");
    Value v = src.value;
    if (v.type == PinType::integer && pin.type == PinType::number) v = Value::number(static_cast<f64>(v.i));
    if (v.type != pin.type || src.of != pin.of)
      return fail(src.tok, "\"" + pin.id + "\" of " + node_name(m.def->type) + " is " + cpp_type(want) + ", this is " + cpp_type(TypeRef{v.type, src.of}) + " (a " + cpp_type(want) + " looks like " +
                               literal(zero_of(want, g), pin.of, g) + ")");
    m.node.set_value(pin.id, v.to_json(g, pin.of));
    return true;
  }
  /// Wires the exec outputs `from` to the exec input `in` of `node` ("": its first), or of the step go<id>.
  void enter(const Ends& from, int node, const std::string& in, std::size_t tok, u32 id = 0) {
    for (const End& e : from) if (e.node >= 0) exec_wires.push_back({e.node, e.out, node, id, in, tok});
  }

  // ---- values ----
  struct Args {
    Reader& r;
    bool first = true;
    /// Is there another argument? (reads the comma in front of it)
    bool more() {
      if (r.is(")")) return false;
      if (!first && !r.accept(",")) return false;
      first = false;
      return true;
    }
  };
  std::string takes(int node, const std::string& what) const {
    const Made& m = made[static_cast<std::size_t>(node)];
    std::string list;
    for (int j : data_inputs(node)) list += (list.empty() ? "" : ", ") + m.in[static_cast<std::size_t>(j)].id;
    return what + " takes " + std::to_string(data_inputs(node).size()) + " value(s)" + (list.empty() ? std::string() : ": " + list);
  }
  bool args_of(int node, Args& a, const std::string& what) {
    for (int k : data_inputs(node)) {
      if (!a.more()) return fail(p, takes(node, what));
      const Src s = value();
      if (failed() || !bind(node, k, s)) return false;
    }
    return true;
  }
  /// The member n<id> a node's call starts with (when the next argument is one).
  bool member_arg(int node, Args& a) {
    if (peek().kind != Tok::id || !numbered(peek().s, "n") || !(is(",", 1) || is(")", 1))) return true;
    a.more();
    if (!declare(numbered(peek().s, "n"), node, p)) return false;
    ++p;
    return true;
  }
  /// The variables a node's settings name, where its call has them: its first values (make_named
  /// has read them ahead).
  bool variable_args(int node, Args& a) {
    const Made& m = made[static_cast<std::size_t>(node)];
    for (const ParamDef& pd : m.def->params) {
      if (pd.kind != ParamDef::Kind::variable) continue;
      const std::string name = m.node.param(pd.key);
      if (!a.more() || peek().kind != Tok::id || peek().s != name) return fail(p, node_name(m.def->type) + ": the variable " + name + " expected, not " + shown(peek()));
      ++p;
    }
    return true;
  }
  /// A value node called by its function's name: math_add(a, b), math_random(n7, 0.0, 1.0).
  Src pure_call(const NodeDef* def, std::size_t tok) {
    Src s;
    s.tok = tok;
    const int node = make_named(def, tok);
    if (node < 0 || !expect("(")) return s;
    Args a{*this};
    if (!member_arg(node, a) || !variable_args(node, a) || !args_of(node, a, node_name(def->type))) return s;
    if (!is(")")) { fail(p, takes(node, node_name(def->type)) + " - \")\" expected, not " + shown(peek())); return s; }
    ++p;
    s.node = node;
    s.root = true;
    return s;
  }
  Src value() {
    Src s;
    s.tok = p;
    if (failed()) return s;
    Value v;
    if (literal_here(v, &s.of)) {
      s.literal = true;
      s.value = v;
      return s;
    }
    const Tok t = peek();
    if (t.kind != Tok::id) { fail(p, "a value expected, not " + shown(t)); return s; }
    if (t.s == "kern") {
      p += 1;
      if (!expect("::")) return s;
      if (accept("to")) {
        TypeRef to;
        if (!expect("<")) return s;
        if (!type_here(to)) { fail(p, "kern::to<Type>: a type expected, not " + shown(peek())); return s; }
        if (!expect(">") || !expect("(")) return s;
        Src inner = value();
        if (failed() || !expect(")")) return s;
        if (inner.cast) { fail(s.tok, "one kern::to is enough"); return s; }
        inner.cast = true;
        inner.to = to.type;
        inner.tok = s.tok;
        inner.root = false;
        return inner;
      }
      if (accept("formula")) return formula(s.tok);
      fail(p, "kern::to<Type>(value) or kern::formula(...) expected, not kern::" + (peek().kind == Tok::id ? peek().s : shown(peek())));
      return s;
    }
    if (const u32 id = numbered(t.s, "n")) {
      ++p;
      s.id = id;
      if (accept("(")) {
        if (!expect(")")) return s;
        s.call = true;
      } else if (!is(".")) {
        fail(s.tok, t.s + " alone is no value: " + t.s + ".<output> reads what the node gave, " + t.s + "() a value with that name");
        return s;
      }
      if (accept(".") && !word(s.pin, "the output's name")) return s;
      return s;
    }
    ++p;
    if (scope.root >= 0) {
      const Made& root = made[static_cast<std::size_t>(scope.root)];
      for (const PinDef& pd : root.out)
        if (pd.type != PinType::exec && pd.id == t.s) {
          s.node = scope.root;
          s.pin = pd.id;
          return s;
        }
    }
    if (const Function* f = g.function(t.s)) {
      const std::string fname = f->name;
      const std::size_t inputs = f->inputs.size();
      if (!accept("(")) { fail(s.tok, fname + " is a function: " + fname + "(...)"); return s; }
      const int node = make("func.pure", s.tok, {{"function", fname}});
      Args a{*this};
      if (node < 0 || !args_of(node, a, fname)) return s;
      if (!expect(")", "(" + fname + " takes " + std::to_string(inputs) + " value(s))")) return s;
      s.node = node;
      if (accept(".")) word(s.pin, "the result's name");
      else s.root = true;
      return s;
    }
    if (const Variable* v = g.variable(t.s)) {
      if (is(".")) {
        const std::string field = peek(1).kind == Tok::id ? peek(1).s : std::string("Field");
        if (v->type == PinType::structure) fail(p, "a struct's field is read through Break: struct_break<" + v->of + ">(" + v->name + ")." + field);
        else fail(p, v->name + " is " + cpp_type(TypeRef{v->type, v->of}) + ": it has no fields");
        return s;
      }
      s.node = make("var.get", s.tok, {{"variable", t.s}});
      s.root = true;
      return s;
    }
    auto fn = node_fns.find(t.s);
    if (fn != node_fns.end()) {
      if (fn->second->kind != NodeKind::pure) { fail(s.tok, t.s + " is a step, not a value: it stands on a line of its own"); return s; }
      if (fn->second->type == "var.get") { fail(s.tok, "a variable is read by its name"); return s; }
      if (fn->second->type == "func.pure") { fail(s.tok, "a function's value is Function(values).result"); return s; }
      if (fn->second->type == "math.formula") { fail(s.tok, "a formula is kern::formula([](Float a) -> Float { return ...; }, value)"); return s; }
      s = pure_call(fn->second, s.tok);
      if (failed()) return s;
      if (accept(".")) {
        s.root = false;
        word(s.pin, "the output's name");
      }
      return s;
    }
    if (t.s == "entry") fail(s.tok, "entry is the exec input a go method is entered through: only its first step takes it");
    else
      fail(s.tok, "\"" + t.s + "\" is not known here: no variable, no function, no node" +
                      (scope.root < 0 ? " (after a wait or in a go method, an event's or function's values are read as n<id>.<name>)" : ""));
    return s;
  }
  /// kern::formula([](Float a, Float b) -> Float { return ...; }, a's value, b's value)
  Src formula(std::size_t tok) {
    Src s;
    s.tok = tok;
    std::vector<std::string> names;
    if (!expect("(") || !expect("[") || !expect("]") || !expect("(")) return s;
    while (!is(")")) {
      std::string nm;
      if (!expect("Float", "(a formula's inputs are Float)") || !word(nm, "the input's name")) return s;
      names.push_back(nm);
      if (!accept(",")) break;
    }
    if (!expect(")") || !expect("->") || !expect("Float") || !expect("{") || !expect("return")) return s;
    const std::size_t from = peek().begin;
    int depth = 0;
    while (peek().kind != Tok::end && !(depth == 0 && (is(";") || is("}")))) {
      if (is("(")) ++depth;
      if (is(")")) --depth;
      ++p;
    }
    std::string body(text.substr(from, peek().begin - from));
    const std::size_t body_tok = p;
    if (!expect(";") || !expect("}")) return s;
    for (std::size_t at = body.find("kern::math::"); at != std::string::npos; at = body.find("kern::math::", at)) body.erase(at, 12);
    const Formula f(body);
    if (!f.ok()) { fail(body_tok, "the formula: " + f.error()); return s; }
    if (f.inputs() != names) {
      std::string want;
      for (const std::string& in : f.inputs()) want += (want.empty() ? "Float " : ", Float ") + in;
      fail(tok, "the formula reads (" + want + "): the lambda's parameters are exactly these, in this order");
      return s;
    }
    const int node = make("math.formula", tok, {{"formula", f.text()}});
    if (node < 0) return s;
    Args a{*this};
    a.first = false;  // (the lambda was the first argument)
    if (!args_of(node, a, "this formula")) return s;
    if (!expect(")", "(one value per input of the formula)")) return s;
    s.node = node;
    s.root = true;
    return s;
  }

  // ---- statements ----
  /// The node a statement is: the first one of a go method gets the method's number.
  int step(int node, std::size_t tok) {
    if (node >= 0 && scope.go) declare(scope.go, node, tok);
    scope.go = 0;
    entry_now = scope.entry;
    scope.entry = false;
    scope.first = false;
    return node;
  }
  /// The arguments of a step in front of its values: its member n<id>, the exec input it is
  /// entered through when it has several, then the variable it changes. `entry`: that input
  /// ("": the first).
  bool head(int node, Args& a, std::string& entry) { return member_and_entry(node, a, entry) && variable_args(node, a); }
  bool member_and_entry(int node, Args& a, std::string& entry) {
    const Made& m = made[static_cast<std::size_t>(node)];
    if (!member_arg(node, a)) return false;
    const std::vector<std::string> ins = exec_pins(node, true);
    entry.clear();
    if (ins.size() < 2) return true;
    std::string list;
    for (const std::string& in : ins) list += (list.empty() ? "\"" : ", \"") + in + "\"";
    if (!a.more()) return fail(p, node_name(m.def->type) + " has several exec inputs: say which one (" + list + ")");
    if (is("entry") && entry_now) {
      ++p;  // (whatever its callers name: go<id>("pin"))
      return true;
    }
    const std::size_t at = p;
    if (!string(entry, "the exec input (" + list + ")")) return false;
    if (std::find(ins.begin(), ins.end(), entry) == ins.end()) return fail(at, node_name(m.def->type) + " has no exec input \"" + entry + "\" (" + list + ")");
    return true;
  }
  /// Variable.Field = value   ("Variable" has been read, "." is next): the Set Field it is, up to
  /// and including the "=" (-1: refused).
  int field_step(const std::string& var, std::size_t at) {
    const Variable* v = g.variable(var);
    const StructType* s = v && v->type == PinType::structure ? g.structure(v->of) : nullptr;
    if (!s) { fail(p, var + " is " + (v ? cpp_type(TypeRef{v->type, v->of}) : std::string("no variable")) + ": only a struct has fields to set (Variable.Field = value;)"); return -1; }
    ++p;
    std::string field;
    const std::size_t where = p;
    if (!word(field, "a field of " + s->name + " (" + field_names(*s) + ")")) return -1;
    if (s->field(field) < 0) { fail(where, s->name + " has no field \"" + field + "\" (" + field_names(*s) + ")"); return -1; }
    if (!is("=")) { fail(p, var + "." + field + " = value; expected, not " + shown(peek())); return -1; }
    ++p;
    return step(make("struct.set_field", at, {{"variable", var}, {"field", field}}), at);
  }
  /// A body in braces: "{" has been read; reads up to and including "}".
  Ends block(Ends open, bool keep_scope = true) {
    const Scope saved = scope;
    if (!keep_scope) {
      // (a lambda that keeps nothing but this: the parameters of the method around it are gone)
      scope.root = -1;
      scope.function = nullptr;
    }
    scope.first = false;
    scope.entry = false;
    scope.go = 0;
    Ends r = list(std::move(open));
    expect("}");
    const int column = scope.column;
    scope = saved;
    scope.column = column;
    return r;
  }
  /// What follows a latent step: [this] { ... }
  bool later(int node, const std::string& out_pin) {
    if (!expect("[") || !expect("this", "(what runs later keeps nothing but this)") || !expect("]") || !expect("{")) return false;
    block({{node, out_pin}}, false);
    return !failed();
  }
  /// The statements up to the closing "}" (not read). `open`: the exec outputs that lead to the
  /// first one. Returns the exec outputs the list leaves through.
  Ends list(Ends open) {
    while (!failed() && !is("}")) {
      if (peek().kind == Tok::end) { fail(p, "\"}\" is missing"); break; }
      if (peek().kind == Tok::doc) { ++p; continue; }
      if (accept(";")) continue;
      const std::size_t at = p;
      // const auto n<id> = [&] { return value; };
      if (is("const") && is("auto", 1)) {
        p += 2;
        std::string var;
        if (!word(var, "n<id>")) break;
        const u32 id = numbered(var, "n");
        if (!id) { fail(at + 2, "a value with a name is called n<id> (n12), not \"" + var + "\""); break; }
        if (!expect("=") || !expect("[") || !expect("&") || !expect("]") || !expect("{") || !expect("return")) break;
        const Src s = value();
        if (failed()) break;
        if (!s.root) { fail(s.tok, var + " names one value node: a variable, a function's value or a node's call"); break; }
        if (!declare(id, s.node, at + 2) || !expect(";") || !expect("}") || !expect(";")) break;
        continue;
      }
      // (void)value;   a value nothing reads
      if (is("(") && is("void", 1) && is(")", 2)) {
        p += 3;
        const Src s = value();
        if (failed()) break;
        if (!s.root) { fail(s.tok, "(void) keeps a value node nothing reads: a variable, a function's value or a node's call"); break; }
        if (!expect(";")) break;
        continue;
      }
      // n<id> = {a, b};   an event's or a function's values, for whoever reads them elsewhere
      if (peek().kind == Tok::id && numbered(peek().s, "n") && is("=", 1) && is("{", 2)) {
        if (!scope.first || scope.root < 0) { fail(at, "\"" + peek().s + " = {...};\" publishes an event's or a function's values: it is the first line of its method"); break; }
        const u32 id = numbered(peek().s, "n");
        p += 3;
        std::string want, got;
        for (const PinDef& pd : made[static_cast<std::size_t>(scope.root)].out) if (pd.type != PinType::exec) want += (want.empty() ? "" : ", ") + pd.id;
        while (peek().kind == Tok::id) {
          got += (got.empty() ? "" : ", ") + toks[p++].s;
          if (!accept(",")) break;
        }
        if (got != want) { fail(at, "n" + std::to_string(id) + " = {" + want + "}; - all the method's parameters, in their order"); break; }
        if (!expect("}") || !expect(";") || !declare(id, scope.root, at)) break;
        published.insert(scope.root);
        scope.first = false;
        continue;
      }
      bool leads = scope.go != 0, ended = false;
      for (const End& e : open) (e.node >= 0 ? leads : ended) = true;
      if (!leads) {
        fail(at, "nothing leads here: the path ended above (after return, a wait, a Sequence or go<id>()). Put it before that, or into the braces it belongs to");
        break;
      }
      if (ended) {
        fail(at, "one of the paths above has ended (after a wait, a Sequence, a step without exec output or go<id>()), but C++ would go on with this line on that path too: write "
                 "it into the braces of the paths it belongs to");
        break;
      }
      open = statement(std::move(open));
    }
    return open;
  }
  Ends statement(Ends in) {
    const std::size_t at = p;
    const Tok t = peek();
    if (accept("return")) {
      scope.first = false;
      if (!scope.function) {
        // not in a function's own body: the path ends
        if (!accept(";")) fail(at, "only a function's own body returns values: not an event, not what runs after a wait, not a go method");
        return {};
      }
      const Function f = *scope.function;
      const std::string gives = "(" + f.name + " gives " + std::to_string(f.outputs.size()) + " value(s))";
      if (accept(";")) {
        if (!f.outputs.empty()) { fail(at, f.name + " gives " + std::to_string(f.outputs.size()) + " value(s): return {...}; (or return {}; for the defaults)"); return {}; }
        const int node = step(make("func.return", at, {{"function", f.name}}), at);
        if (node >= 0) enter(in, node, "", at);
        return {};
      }
      if (!expect("{")) return {};
      if (f.outputs.empty()) { fail(at, f.name + " gives nothing: return;"); return {}; }
      if (accept("}")) {
        // return {};   no Return node: the path ends and the outputs keep their defaults
        expect(";");
        return {};
      }
      const int node = step(make("func.return", at, {{"function", f.name}}), at);
      if (node < 0) return {};
      enter(in, node, "", at);
      bool first = true;
      for (int k : data_inputs(node)) {
        if (!first && !expect(",", gives)) return {};
        first = false;
        const Src s = value();
        if (failed() || !bind(node, k, s)) return {};
      }
      if (!expect("}", gives) || !expect(";")) return {};
      return {};
    }
    if (accept("if")) {
      if (!expect("(")) return {};
      auto fn = peek().kind == Tok::id ? node_fns.find(peek().s) : node_fns.end();
      if (fn != node_fns.end() && fn->second->kind != NodeKind::pure) {
        // if (flow_do_once(...)) { }   a step that decides whether the path goes on
        const NodeDef* def = fn->second;
        const std::string fname = node_name(def->type);
        const std::size_t name = p++;
        const int node = step(make_named(def, name), name);
        if (node < 0 || !expect("(")) return {};
        const std::vector<std::string> outs = exec_pins(node, false);
        if (def->type == "flow.switch_enum") { fail(name, fname + " goes on at the enum's value: switch (" + fname + "<AnEnum>(...)) { case AnEnum::Value: { } break; }"); return {}; }
        if (def->latent || def->loop || outs.size() != 1 || def->kind != NodeKind::flow) {
          fail(name, fname + " does not decide whether the path goes on: " +
                         (outs.size() >= 2 && !def->latent && !def->loop ? "switch (" + fname + "(...)) { case 0: { } break; }" : "it stands on a line of its own"));
          return {};
        }
        Args a{*this};
        std::string entry;
        if (!head(node, a, entry) || !args_of(node, a, fname) || !expect(")", "(" + takes(node, fname) + ")") || !expect(")") || !expect("{")) return {};
        enter(in, node, entry, name);
        Ends ends = block({{node, outs[0]}});
        if (is("else")) fail(p, fname + " has one way on, no else");
        return ends;
      }
      const int node = step(make("flow.branch", at), at);
      if (node < 0) return {};
      enter(in, node, "", at);
      const Src s = value();
      if (failed() || !bind(node, data_inputs(node).front(), s) || !expect(")") || !expect("{")) return {};
      Ends ends = block({out(node, 0)});
      if (failed()) return {};
      Ends other{out(node, 1)};
      if (accept("else")) {
        if (is("if")) other = statement(std::move(other));  // else if (...): an if in the else
        else if (expect("{")) other = block(std::move(other));
      }
      ends.insert(ends.end(), other.begin(), other.end());
      return ends;
    }
    if (accept("switch")) {
      if (!expect("(")) return {};
      const std::size_t name = p;
      auto fn = peek().kind == Tok::id ? node_fns.find(peek().s) : node_fns.end();
      if (fn == node_fns.end() || fn->second->kind == NodeKind::pure) { fail(p, "switch (...) takes a step with several exec outputs (flow_switch_int, flow_flip_flop ...)"); return {}; }
      ++p;
      const NodeDef* def = fn->second;
      const std::string fname = node_name(def->type);
      const int node = step(make_named(def, name), name);
      if (node < 0 || !expect("(")) return {};
      const std::vector<std::string> outs = exec_pins(node, false);
      // Switch on Enum: its cases are the enum's values, by their names (case Mode::Open:).
      const EnumType* values = def->type == "flow.switch_enum" ? g.enumeration(made[static_cast<std::size_t>(node)].node.param("enum")) : nullptr;
      if (def->latent || def->loop || (outs.size() < 2 && !values)) { fail(name, fname + " has no several ways on to switch between"); return {}; }
      Args a{*this};
      std::string entry;
      if (!head(node, a, entry) || !args_of(node, a, fname) || !expect(")", "(" + takes(node, fname) + ")") || !expect(")") || !expect("{")) return {};
      enter(in, node, entry, name);
      Ends ends;
      std::vector<bool> written(outs.size(), false);
      while (!failed() && accept("case")) {
        i64 k = 0;
        const std::size_t which = p;
        if (values) {
          if (peek().kind != Tok::id || peek().s != values->name || !is("::", 1)) {
            fail(which, "a case of " + fname + "<" + values->name + "> is one of the enum's values: case " + values->name + "::" + (values->values.empty() ? std::string("Value") : values->values[0]) + ":");
            return {};
          }
          Value v;
          literal_here(v);
          if (failed()) return {};
          k = v.i;
        } else if (!integer(k, "the exec output's number")) {
          return {};
        }
        if (k < 0 || static_cast<std::size_t>(k) >= outs.size()) { fail(which, fname + " has the exec outputs 0 to " + std::to_string(outs.size() - 1)); return {}; }
        if (written[static_cast<std::size_t>(k)]) {
          fail(which, "case " + (values ? values->name + "::" + values->values[static_cast<std::size_t>(k)] : std::to_string(k)) + " is there twice");
          return {};
        }
        written[static_cast<std::size_t>(k)] = true;
        if (!expect(":") || !expect("{")) return {};
        Ends arm = block({{node, outs[static_cast<std::size_t>(k)]}});
        ends.insert(ends.end(), arm.begin(), arm.end());
        if (failed() || !expect("break", "(every case ends with break;)") || !expect(";")) return {};
      }
      if (!expect("}")) return {};
      for (std::size_t k = 0; k < outs.size(); ++k) if (!written[k]) ends.push_back({node, outs[k]});
      return ends;
    }
    if (accept("while")) {
      const int node = step(make("flow.while", at), at);
      if (node < 0 || !expect("(") || !expect("loop", "- a While Loop is while (loop(condition)) { }") || !expect("(")) return {};
      enter(in, node, "", at);
      const Src s = value();
      if (failed() || !bind(node, data_inputs(node).front(), s) || !expect(")") || !expect(")") || !expect("{")) return {};
      block({out(node, 0)});
      return {out(node, 1)};
    }
    if (accept("for")) {
      // for (auto it<id> = flow_for(n<id>, first, last); it<id>.next();) { }
      std::string it;
      if (!expect("(") || !expect("auto") || !word(it, "it<id>")) return {};
      const u32 id = numbered(it, "it");
      if (!id) { fail(at + 3, "a loop's iterator is called it<id> (it12), not \"" + it + "\""); return {}; }
      if (!expect("=")) return {};
      const std::size_t name = p;
      auto fn = peek().kind == Tok::id ? node_fns.find(peek().s) : node_fns.end();
      if (fn == node_fns.end() || !fn->second->loop) { fail(p, "a loop expected (flow_for ...), not " + shown(peek())); return {}; }
      ++p;
      const std::string fname = node_name(fn->second->type);
      const int node = step(make_named(fn->second, name), name);
      if (node < 0 || !expect("(")) return {};
      Args a{*this};
      std::string entry;
      if (!head(node, a, entry) || !declare(id, node, at + 3) || !args_of(node, a, fname) || !expect(")", "(" + takes(node, fname) + ")") || !expect(";")) return {};
      std::string again;
      const std::size_t cond = p;
      if (!word(again, "it<id>.next()") || !expect(".") || !expect("next") || !expect("(") || !expect(")") || !expect(";") || !expect(")") || !expect("{")) return {};
      if (again != it) { fail(cond, "the loop goes on while " + it + ".next()"); return {}; }
      enter(in, node, entry, name);
      const std::vector<std::string> outs = exec_pins(node, false);
      if (outs.empty()) { fail(name, fname + " has no body"); return {}; }
      block({{node, outs[0]}});
      return outs.size() > 1 ? Ends{{node, outs[1]}} : gone();
    }
    if (accept("{")) {
      // { } { } ...   a Sequence; statements after the last block are one more block
      const int node = step(make("flow.sequence", at, {{"count", "16"}}), at);
      if (node < 0) return {};
      enter(in, node, "", at);
      std::size_t count = 0;
      for (;;) {
        if (count >= 16) { fail(at, "a Sequence has at most 16 outputs"); return {}; }
        block({out(node, count)});
        ++count;
        if (failed() || !accept("{")) break;
      }
      if (!failed() && !is("}")) {
        if (count >= 16) { fail(at, "a Sequence has at most 16 outputs"); return {}; }
        list({out(node, count)});
        ++count;
      }
      Made& m = made[static_cast<std::size_t>(node)];
      m.node.set_param("count", std::to_string(count));
      m.in.clear();
      m.out.clear();
      lib.pins(m.node, g, m.in, m.out);
      return gone();
    }
    if (t.kind != Tok::id) { fail(at, shown(t) + " starts no statement"); return {}; }
    for (const char* no : {"break", "continue", "goto", "do", "try", "throw", "else", "case", "default"})
      if (t.s == no) { fail(at, std::string("\"") + no + "\" is not part of the notation here"); return {}; }
    // go<id>();   a wire to a step that is a method of its own
    if (const u32 id = numbered(t.s, "go")) {
      p += 1;
      std::string pin;
      if (!expect("(")) return {};
      if (!is(")") && !string(pin, "the exec input")) return {};
      if (!expect(")") || !expect(";")) return {};
      enter(in, -1, pin, at, id);
      scope.first = false;
      return gone();
    }
    // n<id>.value = (Variable = value);   a Set / Add whose result is read
    if (numbered(t.s, "n") && is(".", 1) && is("=", 3) && is("(", 4)) {
      const u32 id = numbered(t.s, "n");
      const std::string pin = peek(2).s;
      p += 5;
      const std::size_t name = p;
      std::string var;
      if (!word(var, "a variable")) return {};
      if (!g.variable(var)) { fail(name, "\"" + var + "\" is no variable: n<id>.value = (Variable = value); keeps what a Set gave"); return {}; }
      int node = -1;
      if (is(".")) {
        node = field_step(var, at);  // n<id>.value = (Variable.Field = value);
      } else {
        const bool add = is("+=");
        if (!add && !is("=")) { fail(p, var + " = value or " + var + " += value expected, not " + shown(peek())); return {}; }
        ++p;
        node = step(make(add ? "var.add" : "var.set", at, {{"variable", var}}), at);
      }
      if (node < 0 || !declare(id, node, at)) return {};
      enter(in, node, "", at);
      const Src s = value();
      if (failed() || !bind(node, data_inputs(node).front(), s) || !expect(")") || !expect(";")) return {};
      bool has = false;
      for (const PinDef& pd : made[static_cast<std::size_t>(node)].out) has = has || (pd.type != PinType::exec && pd.id == pin);
      if (!has) { fail(at + 2, "a Set gives \"value\": n" + std::to_string(id) + ".value = (" + var + " ...);"); return {}; }
      return {out(node, 0)};
    }
    // n<id> = Function(...);
    if (numbered(t.s, "n") && is("=", 1)) {
      const u32 id = numbered(t.s, "n");
      p += 2;
      const std::size_t name = p;
      std::string fn;
      if (!word(fn, "a function")) return {};
      const Function* f = g.function(fn);
      if (!f) { fail(name, "\"" + fn + "\" is no function of this Blueprint (n<id> = Function(...); keeps what a called function gave)"); return {}; }
      return call_function(std::move(in), f->name, id, at);
    }
    if (const Variable* v = g.variable(t.s)) {
      // Variable = value;   Variable += value;   Variable.Field = value;
      const std::string var = v->name;
      p += 1;
      int node = -1;
      if (is(".")) {
        node = field_step(var, at);
      } else {
        const bool add = is("+=");
        if (!add && !is("=")) { fail(p, var + " = value; or " + var + " += value; expected, not " + shown(peek())); return {}; }
        ++p;
        node = step(make(add ? "var.add" : "var.set", at, {{"variable", var}}), at);
      }
      if (node < 0) return {};
      enter(in, node, "", at);
      const Src s = value();
      if (failed() || !bind(node, data_inputs(node).front(), s) || !expect(";")) return {};
      return {out(node, 0)};
    }
    if (const Function* f = g.function(t.s)) {
      p += 1;
      return call_function(std::move(in), f->name, 0, at);
    }
    auto fn = node_fns.find(t.s);
    if (fn == node_fns.end()) {
      fail(at, "\"" + t.s + "\" starts no statement: no variable, no function, no node");
      return {};
    }
    // a step called by its function's name
    const NodeDef* def = fn->second;
    const std::string fname = node_name(def->type);
    p += 1;
    if (def->type == "var.set" || def->type == "var.add") { fail(at, "a variable is set by its name: Heat = value; or Heat += value;"); return {}; }
    if (def->type == "func.call") { fail(at, "a function is called by its name: Function(values);"); return {}; }
    if (def->type == "struct.set_field") { fail(at, "a struct's field is set by its name: Variable.Field = value;"); return {}; }
    if (def->kind == NodeKind::pure) { fail(at, fname + " is a value: alone on a line it does nothing"); return {}; }
    if (def->kind == NodeKind::event) { fail(at, fname + " is an event: KERN_EVENT(on<id>, " + fname + ")"); return {}; }
    if (def->loop) { fail(at, fname + " is a loop: for (auto it<id> = " + fname + "(...); it<id>.next();) { }"); return {}; }
    if (def->type == "flow.branch") { fail(at, "a Branch is if (value) { } else { }"); return {}; }
    if (def->type == "flow.sequence") { fail(at, "a Sequence is { } { }"); return {}; }
    if (def->type == "flow.while") { fail(at, "a While Loop is while (loop(value)) { }"); return {}; }
    if (def->type == "flow.switch_enum") { fail(at, fname + " goes on at the enum's value: switch (" + fname + "<AnEnum>(...)) { case AnEnum::Value: { } break; }"); return {}; }
    const int node = step(make_named(def, at), at);
    if (node < 0 || !expect("(")) return {};
    const std::vector<std::string> outs = exec_pins(node, false);
    if (!def->latent && outs.size() >= 2) { fail(at, fname + " has several ways on: switch (" + fname + "(...)) { case 0: { } break; }"); return {}; }
    if (!def->latent && outs.size() == 1 && def->kind == NodeKind::flow) { fail(at, fname + " decides whether the path goes on: if (" + fname + "(...)) { }"); return {}; }
    Args a{*this};
    std::string entry;
    if (!head(node, a, entry) || !args_of(node, a, fname)) return {};
    enter(in, node, entry, at);
    if (def->latent) {
      if (!a.more()) { fail(p, fname + " waits: what follows it is its last argument, [this] { ... }"); return {}; }
      if (is("[")) {
        if (outs.size() != 1) { fail(p, fname + " has " + std::to_string(outs.size()) + " ways on: {{\"" + (outs.empty() ? std::string() : outs[0]) + "\", [this] { ... }}, ...}"); return {}; }
        if (!later(node, outs[0])) return {};
      } else {
        if (!expect("{", "(or [this] { ... })")) return {};
        std::set<std::string> seen;
        while (!failed() && accept("{")) {
          std::string out_pin;
          const std::size_t which = p;
          if (!string(out_pin, "the exec output")) return {};
          if (std::find(outs.begin(), outs.end(), out_pin) == outs.end()) { fail(which, fname + " has no exec output \"" + out_pin + "\""); return {}; }
          if (!seen.insert(out_pin).second) { fail(which, "\"" + out_pin + "\" is there twice"); return {}; }
          if (!expect(",") || !later(node, out_pin) || !expect("}")) return {};
          if (!accept(",")) break;
        }
        if (!expect("}")) return {};
      }
      if (!expect(")") || !expect(";")) return {};
      return gone();
    }
    if (!expect(")", "(" + takes(node, fname) + ")") || !expect(";")) return {};
    return outs.empty() ? gone() : Ends{{node, outs[0]}};
  }
  /// [n<id> = ]Function(...);   (the function's name has been read)
  Ends call_function(Ends in, const std::string& fn, u32 id, std::size_t at) {
    const int node = step(make("func.call", at, {{"function", fn}}), at);
    if (node < 0 || !expect("(")) return {};
    if (id && !declare(id, node, at)) return {};
    enter(in, node, "", at);
    Args a{*this};
    if (!args_of(node, a, fn)) return {};
    if (!expect(")", "(" + takes(node, fn) + ")") || !expect(";")) return {};
    return {out(node, 0)};
  }

  // ---- the second pass: the bodies ----
  bool bodies_pass() {
    for (const Item& it : items) {
      if (failed()) break;
      p = it.open;
      scope = Scope();
      scope.body = ++bodies;
      switch (it.kind) {
        case Item::shared: {
          if (!expect("return")) break;
          const Src s = value();
          if (failed()) break;
          if (!s.root) { fail(s.tok, "n" + std::to_string(it.id) + "() is one value node: a variable, a function's value or a node's call"); break; }
          if (!declare(it.id, s.node, it.tok) || !expect(";")) break;
          expect("}");
          break;
        }
        case Item::function: {
          const int entry = make("func.entry", it.tok, {{"function", it.fn}});
          if (entry < 0) break;
          scope.root = entry;
          scope.function = g.function(it.fn);
          list({out(entry, 0)});
          expect("}");
          break;
        }
        case Item::event: {
          auto fn = node_fns.find(it.fn);
          if (fn == node_fns.end() || fn->second->kind != NodeKind::event || fn->second->type == "func.entry") { fail(it.tok, "\"" + it.fn + "\" is no event"); break; }
          const NodeDef* def = fn->second;
          if (it.params.size() != def->params.size()) { fail(it.tok, it.fn + settings_text(def)); break; }
          std::vector<std::pair<std::string, std::string>> params;
          for (std::size_t k = 0; k < it.params.size(); ++k) params.push_back({def->params[k].key, it.params[k]});
          const int node = make(def, it.tok, std::move(params));
          if (!declare(it.id, node, it.tok)) break;
          std::string want, got;
          for (const PinDef& pd : made[static_cast<std::size_t>(node)].out)
            if (pd.type != PinType::exec) want += (want.empty() ? "" : ", ") + cpp_type(TypeRef{pd.type, pd.of}) + " " + pd.id;
          for (const auto& [arg, type] : it.args) got += (got.empty() ? "" : ", ") + cpp_type(type) + " " + arg;
          if (want != got) { fail(it.tok, "the method of " + it.fn + " is void on" + std::to_string(it.id) + "(" + want + ")"); break; }
          scope.root = node;
          const std::vector<std::string> outs = exec_pins(node, false);
          list(outs.empty() ? Ends{} : Ends{{node, outs[0]}});
          expect("}");
          break;
        }
        case Item::go:
          scope.go = it.id;
          scope.entry = it.entry;
          list({});
          expect("}");
          if (!failed() && scope.go) fail(it.tok, "go" + std::to_string(it.id) + " is empty: its first statement is the step it stands for");
          break;
        case Item::loose:
          list({});
          expect("}");
          break;
      }
    }
    return !failed();
  }

  // ---- the layout ----
  struct Placed { u32 id; f32 x, y; std::string comment; std::size_t tok; };
  std::vector<Placed> placed;
  bool has_layout = false, has_next = false;
  bool layout() {
    if (!accept("KERN_LAYOUT")) return true;
    std::string type;
    const std::size_t at = p;
    if (!expect("(") || !word(type, "the struct's name") || !expect(")") || !expect("{")) return false;
    if (type != name) return fail(at + 1, "KERN_LAYOUT(" + name + ") - the layout of this struct");
    has_layout = true;
    while (!failed() && !is("}")) {
      std::string what;
      const std::size_t call = p;
      if (!word(what, "node, box or next_id") || !expect("(")) return false;
      i64 id = 0;
      if (!integer(id, "the id")) return false;
      if (id <= 0) return fail(call + 2, "an id is 1 or more");
      if (what == "node") {
        Placed pl;
        pl.id = static_cast<u32>(id);
        pl.tok = call;
        f64 x = 0, y = 0;
        if (!expect(",") || !number(x, "x") || !expect(",") || !number(y, "y")) return false;
        pl.x = static_cast<f32>(x);
        pl.y = static_cast<f32>(y);
        if (accept(",") && !string(pl.comment, "the comment")) return false;
        placed.push_back(std::move(pl));
      } else if (what == "box") {
        CommentBox c;
        c.id = static_cast<u32>(id);
        f64 x = 0, y = 0, w = 0, h = 0;
        if (!expect(",") || !number(x, "x") || !expect(",") || !number(y, "y") || !expect(",") || !number(w, "the width") || !expect(",") || !number(h, "the height") || !expect(",") ||
            !string(c.text, "the text"))
          return false;
        c.x = static_cast<f32>(x);
        c.y = static_cast<f32>(y);
        c.w = static_cast<f32>(w);
        c.h = static_cast<f32>(h);
        if (accept(",")) {
          i64 color = 0;
          if (!integer(color, "the colour (0xRRGGBB)")) return false;
          c.color = static_cast<u32>(color) & 0xffffffu;
        }
        g.comments.push_back(std::move(c));
      } else if (what == "next_id") {
        g.next_id = static_cast<u32>(id);
        has_next = true;
      } else {
        return fail(call, "a layout has node(id, x, y), box(id, x, y, w, h, \"text\") and next_id(n) - not \"" + what + "\"");
      }
      if (!expect(")") || !expect(";")) return false;
    }
    return expect("}");
  }

  // ---- everything together ----
  bool read() {
    if (!lex()) return false;
    for (const auto& d : lib.all()) node_fns[node_name(d->type)] = d.get();
    while (peek().kind == Tok::doc) ++p;
    if (!expect("struct") || !word(name, "the Blueprint's name") || !expect(":") || !expect("kern") || !expect("::") || !expect("Blueprint") || !expect("{")) return false;
    if (!declarations() || !expect("}") || !expect(";")) return false;
    const std::size_t after = p;
    if (!bodies_pass()) return false;
    p = after;
    if (!layout()) return false;
    if (peek().kind != Tok::end) return fail(p, "the Blueprint ends here: " + shown(peek()) + " is too much");
    return ids() && wires() && checked();
  }

  u32 above_ids() const {
    u32 next = 1;
    for (const StateMachine& m : g.machines) {
      next = std::max(next, m.id + 1);
      for (const State& st : m.states) next = std::max(next, st.id + 1);
      for (const Transition& t : m.transitions) next = std::max(next, t.id + 1);
    }
    for (const CommentBox& c : g.comments) next = std::max(next, c.id + 1);
    return next;
  }
  /// Gives every node its id and its place: from the layout (which lists the nodes in the order
  /// the text names them), or new ones.
  bool ids() {
    std::set<u32> used;
    if (has_layout) {
      if (placed.size() != made.size())
        return fail(placed.empty() ? toks.size() - 1 : placed.back().tok, "the layout places " + std::to_string(placed.size()) + " node(s), the text names " + std::to_string(made.size()) +
                                                                                 ": one node(id, x, y) per node, in the order the text names them");
      for (std::size_t i = 0; i < made.size(); ++i) {
        Made& m = made[i];
        const Placed& pl = placed[i];
        if (m.declared && m.declared != pl.id)
          return fail(pl.tok, "the layout places node " + std::to_string(pl.id) + " here, but the text names n" + std::to_string(m.declared) + " at this point (" + node_name(m.def->type) +
                                  "): the nodes are listed in the order the text names them");
        if (!used.insert(pl.id).second) return fail(pl.tok, "node " + std::to_string(pl.id) + " is placed twice");
        m.node.id = pl.id;
        m.node.x = pl.x;
        m.node.y = pl.y;
        m.node.comment = pl.comment;
      }
    } else if (before) {
      adopt();
    } else {
      // No layout: the numbers the text gives, new ones for the rest, one row per body.
      u32 next = above_ids();
      for (const auto& [id, node] : declared) next = std::max(next, id + 1);
      std::map<int, int> rows;  // body -> the most values under one of its steps
      for (const Made& m : made) rows[m.body] = std::max(rows[m.body], m.row);
      std::map<int, f32> top;
      f32 y = 0;
      for (const auto& [body, deepest] : rows) {
        top[body] = y;
        y += 200.0f + 90.0f * static_cast<f32>(deepest);
      }
      for (Made& m : made) {
        m.node.id = m.declared ? m.declared : next++;
        m.node.x = 280.0f * static_cast<f32>(m.column) - (m.pure ? 120.0f : 0.0f);
        m.node.y = top[m.body] + (m.pure ? 60.0f + 90.0f * static_cast<f32>(m.row) : 0.0f);
      }
    }
    for (std::size_t i = 0; i < made.size(); ++i) declared[made[i].node.id] = static_cast<int>(i);
    if (!has_next) {
      u32 next = above_ids();
      for (const Made& m : made) next = std::max(next, m.node.id + 1);
      g.next_id = next;
    }
    return true;
  }

  /// No layout, but the Blueprint the text changes: every node it still has keeps its id, its
  /// place and its comment; new ones get new ids and stand next to the node before them.
  void adopt() {
    // The nodes of `before` in the order its own text names them.
    std::vector<const Node*> olds;
    {
      Shape shape(*before, lib);
      if (shape.build()) {
        Writer w(shape);
        w.write("Blueprint", false);
        for (int i : w.order) olds.push_back(shape.at(i).node);
      } else {
        for (const Node& n : before->nodes) olds.push_back(&n);
        std::sort(olds.begin(), olds.end(), [](const Node* a, const Node* b) { return a->id < b->id; });
      }
    }
    std::unordered_map<u32, int> old_by_id;
    for (std::size_t j = 0; j < olds.size(); ++j) old_by_id[olds[j]->id] = static_cast<int>(j);
    std::vector<int> match(made.size(), -1);
    std::vector<u8> used(olds.size(), 0);
    // 1. A node the text names (n12, on5, go9, it7) is that node - if it still is that kind of node.
    for (std::size_t i = 0; i < made.size(); ++i) {
      auto it = made[i].declared ? old_by_id.find(made[i].declared) : old_by_id.end();
      if (it == old_by_id.end()) continue;
      used[static_cast<std::size_t>(it->second)] = 1;  // (its number is spoken for either way)
      if (olds[static_cast<std::size_t>(it->second)]->type == made[i].node.type) match[i] = it->second;
    }
    // 2. The others in the order of the text: the best run of nodes of the same type that both
    //    texts have (the same settings and typed-in values count for more).
    std::vector<int> a, b;
    for (std::size_t i = 0; i < made.size(); ++i) if (match[i] < 0 && !made[i].declared) a.push_back(static_cast<int>(i));
    for (std::size_t j = 0; j < olds.size(); ++j) if (!used[j]) b.push_back(static_cast<int>(j));
    auto alike = [&](int i, int j) {
      const Node& x = made[static_cast<std::size_t>(i)].node;
      const Node& y = *olds[static_cast<std::size_t>(j)];
      if (x.type != y.type) return 0;
      int score = 2;
      bool same = true;
      for (const ParamDef& pd : made[static_cast<std::size_t>(i)].def->params) same = same && x.param(pd.key, pd.def) == y.param(pd.key, pd.def);
      score += same ? 2 : 0;
      same = x.values.size() == y.values.size();
      for (const auto& [pin, v] : x.values) {
        const json::Value* other = y.value(pin);
        same = same && other && json::stringify(*other) == json::stringify(v);
      }
      return score + (same ? 1 : 0);
    };
    if (!a.empty() && !b.empty() && a.size() * b.size() <= 4000000) {
      const std::size_t n = a.size(), m = b.size(), w = m + 1;
      std::vector<int> best((n + 1) * w, 0);
      for (std::size_t i = 1; i <= n; ++i)
        for (std::size_t j = 1; j <= m; ++j) {
          const int s = alike(a[i - 1], b[j - 1]);
          best[i * w + j] = std::max({best[(i - 1) * w + j], best[i * w + j - 1], s ? best[(i - 1) * w + j - 1] + s : 0});
        }
      for (std::size_t i = n, j = m; i > 0 && j > 0;) {
        const int s = alike(a[i - 1], b[j - 1]);
        if (s && best[i * w + j] == best[(i - 1) * w + j - 1] + s) {
          match[static_cast<std::size_t>(a[i - 1])] = b[j - 1];
          --i;
          --j;
        } else if (best[i * w + j] == best[(i - 1) * w + j]) --i;
        else --j;
      }
    } else {
      // (very large graphs: one pass, each node with the next one of its type)
      std::size_t j = 0;
      for (int i : a) {
        std::size_t k = j;
        while (k < b.size() && !alike(i, b[k])) ++k;
        if (k == b.size()) continue;
        match[static_cast<std::size_t>(i)] = b[k];
        j = k + 1;
      }
    }
    // 3. Ids and places.
    u32 next = std::max(before->next_id, above_ids());
    f32 bottom = 0;
    for (const Node& o : before->nodes) {
      next = std::max(next, o.id + 1);
      bottom = std::max(bottom, o.y);
    }
    for (const auto& [id, node] : declared) next = std::max(next, id + 1);
    std::unordered_map<int, int> last_step, last_any, values_under;  // per body: the nodes before, by the order of the text
    auto grid = [](f32 v) { return std::round(v / 8) * 8; };
    for (std::size_t i = 0; i < made.size(); ++i) {
      Made& m = made[i];
      if (match[i] >= 0) {
        const Node& o = *olds[static_cast<std::size_t>(match[i])];
        m.node.id = o.id;
        m.node.x = o.x;
        m.node.y = o.y;
        m.node.comment = o.comment;
      } else {
        m.node.id = m.declared ? m.declared : next++;
        auto step = last_step.find(m.body), any = last_any.find(m.body);
        if (m.pure && step != last_step.end()) {
          // a value: under the step before it
          const Node& s = made[static_cast<std::size_t>(step->second)].node;
          const int k = values_under[step->second]++;
          m.node.x = grid(s.x - 48);
          m.node.y = grid(s.y + 152 + 104 * static_cast<f32>(k));
        } else if (any != last_any.end()) {
          // a step: to the right of the node before it, a row lower (free of the next one)
          const Node& s = made[static_cast<std::size_t>(step != last_step.end() ? step->second : any->second)].node;
          m.node.x = grid(s.x + 288);
          m.node.y = grid(s.y + (m.pure ? 152 : 56));
        } else {
          // the first node of a new body: a new row below everything
          bottom += 280;
          m.node.x = 0;
          m.node.y = grid(bottom);
        }
        bottom = std::max(bottom, m.node.y);
      }
      last_any[m.body] = static_cast<int>(i);
      if (!m.pure) last_step[m.body] = static_cast<int>(i);
    }
    g.comments = before->comments;
    g.next_id = std::max(next, above_ids());
    has_next = true;
  }

  bool wires() {
    for (const DataWire& w : data_wires) {
      const Made& to = made[static_cast<std::size_t>(w.to)];
      int from = w.from.node;
      if (from < 0) {
        auto it = declared.find(w.from.id);
        if (it == declared.end()) return fail(w.from.tok, "n" + std::to_string(w.from.id) + " names no node of this Blueprint");
        from = it->second;
      }
      const Made& f = made[static_cast<std::size_t>(from)];
      const std::string fname = node_name(f.def->type);
      const std::string nid = "n" + std::to_string(w.from.id);
      if (w.from.node < 0 && w.from.call != f.pure)
        return fail(w.from.tok, f.pure ? nid + " is a value with a name: " + nid + "()" : nid + " is what a step gave: " + nid + ".<output> (no brackets)");
      const PinDef* out_pin = nullptr;
      std::string outs;
      int count = 0;
      for (const PinDef& pd : f.out) {
        if (pd.type == PinType::exec) continue;
        ++count;
        outs += (outs.empty() ? "" : ", ") + pd.id;
        if (w.from.pin.empty() ? count == 1 : pd.id == w.from.pin) out_pin = &pd;
      }
      if (w.from.pin.empty() && count != 1) return fail(w.from.tok, fname + (count == 0 ? " gives nothing to read" : " gives several values: say which one (." + outs + ")"));
      if (!out_pin) return fail(w.from.tok, fname + " has no output \"" + w.from.pin + "\" (" + outs + ")");
      if (f.pure) {
        // (one way to write each read: a function's value names its result, a value with one output does not)
        const bool fn_value = f.def->type == "func.pure", fields = f.def->type == "struct.break";
        if (fn_value && w.from.pin.empty()) return fail(w.from.tok, "which result: " + f.node.param("function") + "(...)." + out_pin->id);
        if (fields && w.from.pin.empty()) return fail(w.from.tok, "which field: struct_break<" + f.node.param("struct") + ">(...)." + out_pin->id);
        if (!fn_value && !fields && count == 1 && !w.from.pin.empty()) return fail(w.from.tok, fname + " gives one value: no ." + w.from.pin);
      }
      const PinDef* in = nullptr;
      for (const PinDef& pd : to.in) if (pd.id == w.in) in = &pd;
      if (!in) continue;
      const TypeRef to_type{in->type, in->of}, from_type{out_pin->type, out_pin->of};
      const std::string need = cpp_type(to_type), have = cpp_type(from_type);
      if (w.from.cast) {
        if (w.from.to != in->type) return fail(w.from.tok, "this input is " + need + (from_type == to_type ? ", and so is the value: no kern::to" : ": kern::to<" + need + ">(...)"));
        if (from_type == to_type) return fail(w.from.tok, "the value is " + need + " already: no kern::to");
        if (!fits(from_type, to_type)) return fail(w.from.tok, "a " + have + " cannot become a " + need);
      } else if (!(from_type == to_type)) {
        if (!fits(from_type, to_type)) return fail(w.from.tok, "this is " + have + ", the input is " + need + ": the two do not convert");
        return fail(w.from.tok, "this is " + have + ", the input is " + need + ": write kern::to<" + need + ">(...)");
      }
      g.links.push_back({f.node.id, out_pin->id, to.node.id, in->id});
    }
    for (const ExecWire& w : exec_wires) {
      int to = w.to;
      if (to < 0) {
        auto it = declared.find(w.to_id);
        if (it == declared.end()) return fail(w.tok, "go" + std::to_string(w.to_id) + " names no step of this Blueprint");
        to = it->second;
      }
      const Made& t = made[static_cast<std::size_t>(to)];
      const std::vector<std::string> ins = exec_pins(to, true);
      if (ins.empty()) return fail(w.tok, node_name(t.def->type) + " has no exec input: nothing can lead to it");
      const std::string in = w.in.empty() ? ins.front() : w.in;
      if (std::find(ins.begin(), ins.end(), in) == ins.end()) return fail(w.tok, node_name(t.def->type) + " has no exec input \"" + in + "\"");
      g.links.push_back({made[static_cast<std::size_t>(w.from)].node.id, w.out, t.node.id, in});
    }
    // The graph: nodes by id, wires in one order.
    std::vector<const Made*> sorted;
    for (const Made& m : made) sorted.push_back(&m);
    std::sort(sorted.begin(), sorted.end(), [](const Made* a, const Made* b) { return a->node.id < b->node.id; });
    for (const Made* m : sorted) g.nodes.push_back(m->node);
    std::sort(g.links.begin(), g.links.end(), [](const Link& a, const Link& b) { return std::tie(a.from, a.out, a.to, a.in) < std::tie(b.from, b.out, b.to, b.in); });
    g.links.erase(std::unique(g.links.begin(), g.links.end()), g.links.end());
    u32 top = 0;
    for (const Node& node : g.nodes) top = std::max(top, node.id);
    if (g.next_id <= top || g.next_id < above_ids()) return fail(toks.size() - 1, "next_id(" + std::to_string(g.next_id) + ") is not above every id in use");
    return true;
  }

  /// The graph can be written again, and the text declares what its C++ needs.
  bool checked() {
    Shape shape(g, lib);
    if (!shape.build()) {
      std::size_t tok = toks.size() - 1;
      for (const Made& m : made) if (m.node.id == shape.error_node) tok = m.tok;
      return fail(tok, shape.error);
    }
    for (const Info& x : shape.n) {
      const u32 id = x.node->id;
      const int index = declared[id];
      const Made& m = made[static_cast<std::size_t>(index)];
      const std::string nid = "n" + std::to_string(id);
      auto decl = members.find(id);
      if (x.event && x.member && !published.count(index)) {
        std::string names;
        for (int k : x.data_out) names += (names.empty() ? "" : ", ") + x.out[static_cast<std::size_t>(k)].id;
        return fail(m.tok, "its values are read outside its own method: the method begins with " + nid + " = {" + names + "};");
      }
      if (!x.member && decl == members.end()) continue;
      const std::string want = Writer::member_text(x, g);
      if (decl == members.end()) return fail(m.tok, nid + " is not declared: " + want);
      const MemberDecl& d = decl->second;
      bool same = d.memory == (x.def->memory && !x.event) && d.result == (x.def->type == "func.call");
      if (d.result) same = same && d.result_of == x.node->param("function");
      else {
        const std::vector<int> held = Writer::member_fields(x);
        same = same && d.fields.size() == held.size();
        for (std::size_t k = 0; same && k < d.fields.size(); ++k) {
          const PinDef& pd = x.out[static_cast<std::size_t>(held[k])];
          same = d.fields[k].first == pd.id && d.fields[k].second == TypeRef{pd.type, pd.of};
        }
      }
      if (!same) return fail(d.tok, nid + " is " + node_name(x.def->type) + ": " + want);
    }
    for (const auto& [id, d] : members)
      if (!shape.index.count(id)) return fail(d.tok, "n" + std::to_string(id) + " names no node of this Blueprint");
    return true;
  }
};

// ---------------------------------------------------------------------------
// the same Blueprint

/// A graph as lines that are equal exactly when two graphs are the same Blueprint.
std::vector<std::pair<std::string, std::string>> described(const Graph& g, const Library& lib) {
  std::vector<std::pair<std::string, std::string>> r;
  auto add = [&](std::string key, std::string value) { r.push_back({std::move(key), std::move(value)}); };
  add("the number of enums", std::to_string(g.enums.size()));
  for (std::size_t i = 0; i < g.enums.size(); ++i) {
    std::string values;
    for (const std::string& v : g.enums[i].values) values += (values.empty() ? "" : ", ") + v;
    add("enum " + std::to_string(i + 1), g.enums[i].name + " { " + values + " }");
    add("the description of enum " + std::to_string(i + 1), g.enums[i].help);
  }
  add("the number of structs", std::to_string(g.structs.size()));
  for (std::size_t i = 0; i < g.structs.size(); ++i) {
    std::string fields;
    for (const StructField& f : g.structs[i].fields) fields += cpp_type(f.type) + " " + f.name + " = " + literal(Value::from_json(f.value, f.type.type, g, f.type.of), f.type.of, g) + "; ";
    add("struct " + std::to_string(i + 1), g.structs[i].name + " { " + fields + "}");
    add("the description of struct " + std::to_string(i + 1), g.structs[i].help);
  }
  add("the number of variables", std::to_string(g.variables.size()));
  for (std::size_t i = 0; i < g.variables.size(); ++i) {
    const Variable& v = g.variables[i];
    add("variable " + std::to_string(i + 1), cpp_type(TypeRef{v.type, v.of}) + " " + v.name + " = " + literal(Value::from_json(v.value, v.type, g, v.of), v.of, g));
    add("the description of variable " + std::to_string(i + 1), v.help);
  }
  add("the number of functions", std::to_string(g.functions.size()));
  for (std::size_t i = 0; i < g.functions.size(); ++i) {
    const Function& f = g.functions[i];
    std::string sig = f.name + (f.pure ? " (pure)" : "") + " (";
    for (const FunctionPin& p : f.inputs) sig += cpp_type(TypeRef{p.type, p.of}) + " " + p.name + " = " + literal(Value::from_json(p.value, p.type, g, p.of), p.of, g) + "; ";
    sig += ") -> (";
    for (const FunctionPin& p : f.outputs) sig += cpp_type(TypeRef{p.type, p.of}) + " " + p.name + " = " + literal(Value::from_json(p.value, p.type, g, p.of), p.of, g) + "; ";
    add("function " + std::to_string(i + 1), sig + ")");
    add("the description of function " + std::to_string(i + 1), f.help);
  }
  add("the number of state machines", std::to_string(g.machines.size()));
  for (std::size_t i = 0; i < g.machines.size(); ++i) {
    const StateMachine& m = g.machines[i];
    const std::string key = "state machine " + std::to_string(i + 1);
    add(key, std::to_string(m.id) + " " + text_literal(m.name));
    std::string states, transitions;
    for (const State& st : m.states) states += std::to_string(st.id) + " " + text_literal(st.name) + " at " + place(st.x) + ", " + place(st.y) + (st.initial ? " initial; " : "; ");
    for (const Transition& t : m.transitions) {
      transitions += std::to_string(t.id) + ": " + std::to_string(t.from) + " -> " + std::to_string(t.to) + " " + trigger_kind_name(t.trigger) + " ";
      if (t.trigger == TriggerKind::event) transitions += text_literal(t.event);
      if (t.trigger == TriggerKind::timer) transitions += f32_text(t.seconds);
      if (t.trigger == TriggerKind::condition) transitions += text_literal(t.variable);
      transitions += " " + text_literal(t.label) + "; ";
    }
    add("the states of " + key, states);
    add("the transitions of " + key, transitions);
  }
  add("the number of comment boxes", std::to_string(g.comments.size()));
  for (std::size_t i = 0; i < g.comments.size(); ++i) {
    const CommentBox& c = g.comments[i];
    char color[16];
    std::snprintf(color, sizeof color, "%06x", c.color & 0xffffffu);
    add("comment box " + std::to_string(i + 1), std::to_string(c.id) + " at " + place(c.x) + ", " + place(c.y) + " size " + place(c.w) + ", " + place(c.h) + " " + text_literal(c.text) + " " + color);
  }
  add("the next id", std::to_string(g.next_id));
  add("the number of nodes", std::to_string(g.nodes.size()));
  std::vector<const Node*> nodes;
  for (const Node& n : g.nodes) nodes.push_back(&n);
  std::sort(nodes.begin(), nodes.end(), [](const Node* a, const Node* b) { return a->id < b->id; });
  std::map<std::pair<u32, std::string>, std::string> into, out_of;  // (node, pin) -> the wire's other end
  for (const Link& l : g.links) {
    std::string& a = into[{l.to, l.in}];
    a += (a.empty() ? "" : " and ") + std::string("node ") + std::to_string(l.from) + "." + l.out;
    std::string& b = out_of[{l.from, l.out}];
    b += (b.empty() ? "" : " and ") + std::string("node ") + std::to_string(l.to) + "." + l.in;
  }
  for (const Node* n : nodes) {
    const std::string key = "node " + std::to_string(n->id);
    const NodeDef* def = lib.find(n->type);
    add("the type of " + key, n->type);
    add("the place of " + key, place(n->x) + ", " + place(n->y));
    add("the comment on " + key, n->comment);
    if (!def) {
      Graph one;
      one.nodes.push_back(*n);
      add("the content of " + key, json::stringify(graph_to_json(one)));
      continue;
    }
    for (const ParamDef& p : def->params) {
      std::string v = n->param(p.key, p.def);
      if (n->type == "math.formula" && p.key == "formula")
        if (const Formula& f = Formula::cached(v); f.ok()) v = f.text();
      add("the setting \"" + p.key + "\" of " + key, v);
    }
    std::vector<PinDef> in, out;
    lib.pins(*n, g, in, out);
    for (const PinDef& p : in) {
      if (p.type == PinType::exec) continue;  // (exec wires: told from where they leave)
      auto wire = into.find({n->id, p.id});
      if (wire != into.end()) add("the input \"" + p.id + "\" of " + key, "from " + wire->second);
      else {
        const json::Value* typed = n->value(p.id);
        add("the input \"" + p.id + "\" of " + key, literal(Value::from_json(typed ? *typed : p.def, p.type, g, p.of), p.of, g));
      }
    }
    for (const PinDef& p : out) {
      if (p.type != PinType::exec) continue;
      auto wire = out_of.find({n->id, p.id});
      add("the exec output \"" + p.id + "\" of " + key, wire == out_of.end() ? std::string("nothing") : "to " + wire->second);
    }
  }
  return r;
}

}  // namespace

bool cpp_name_ok(std::string_view name) {
  if (name.empty() || name.size() > 64 || !is_start(name[0])) return false;
  for (char c : name) if (!is_part(c)) return false;
  if (cpp_keyword(name)) return false;
  for (const char* k : kOwnWords) if (name == k) return false;
  if (name.substr(0, 5) == "kern_" || name.substr(0, 5) == "KERN_") return false;
  if (name.find("__") != std::string_view::npos || (name[0] == '_' && name.size() > 1 && name[1] >= 'A' && name[1] <= 'Z')) return false;  // (reserved by C++)
  for (std::string_view prefix : {"n", "go", "on", "it", "machine"}) {
    // n12 and the like (with leading zeros too, so that no name comes close)
    if (name.size() > prefix.size() && name.substr(0, prefix.size()) == prefix && std::all_of(name.begin() + static_cast<std::ptrdiff_t>(prefix.size()), name.end(), is_digit)) return false;
  }
  return true;
}

std::string cpp_class_name(std::string_view owner, const Graph& g) {
  std::string base;
  bool upper = false;
  for (char ch : owner) {
    if (!is_part(ch)) { upper = !base.empty(); continue; }
    base += upper ? static_cast<char>(std::toupper(static_cast<unsigned char>(ch))) : ch;
    upper = false;
  }
  while (base.find("__") != std::string::npos) base.erase(base.find("__"), 1);
  if (base.empty() || is_digit(base[0])) base = "Blueprint" + base;
  auto inside = [&](const std::string& name) {
    for (const Variable& v : g.variables) if (v.name == name) return true;
    for (const Function& f : g.functions) if (f.name == name || f.name + "Result" == name) return true;
    return g.enumeration(name) || g.structure(name);
  };
  std::string name = base;
  for (int k = 1; !cpp_name_ok(name) || inside(name); ++k) name = base + "Blueprint" + (k == 1 ? std::string() : std::to_string(k));
  return name;
}

std::vector<std::pair<std::string, std::string>> cpp_names(Graph& g, const Library& lib) {
  std::vector<std::pair<std::string, std::string>> renamed;
  std::set<std::string> node_names, event_outs, taken;
  for (const auto& d : lib.all()) node_names.insert(node_name(d->type));
  for (const Node& n : g.nodes) {
    const NodeDef* def = lib.find(n.type);
    if (!def || def->kind != NodeKind::event || def->type == "func.entry") continue;
    std::vector<PinDef> in, out;
    lib.pins(n, g, in, out);
    for (const PinDef& p : out) if (p.type != PinType::exec) event_outs.insert(p.id);
  }
  auto fine = [&](const std::string& name) { return cpp_name_ok(name) && !node_names.count(name) && !event_outs.count(name); };
  // "Ball Seen" -> BallSeen, "max-speed" -> maxSpeed, "2nd" -> Var2nd, "Text" -> TextValue, "value" (an event's output) -> Value
  auto better = [&](const std::string& name) {
    std::string r;
    bool upper = false;
    for (char ch : name) {
      if (!is_part(ch)) { upper = !r.empty(); continue; }
      r += upper ? static_cast<char>(std::toupper(static_cast<unsigned char>(ch))) : ch;
      upper = false;
    }
    while (r.find("__") != std::string::npos) r.erase(r.find("__"), 1);
    if (r.empty() || is_digit(r[0])) r = "Var" + r;
    if (!fine(r) && r[0] >= 'a' && r[0] <= 'z') r[0] = static_cast<char>(r[0] - 'a' + 'A');
    if (!fine(r)) r += "Value";
    return r;
  };
  auto unique = [](std::string name, const std::function<bool(const std::string&)>& is_taken) {
    const std::string base = name;
    for (int i = 2; is_taken(name); ++i) name = base + std::to_string(i);
    return name;
  };
  // Names that are fine stay; then the others get names nobody has. A function also takes
  // <Function>Result.
  std::vector<std::string> fn_names(g.functions.size()), var_names(g.variables.size());
  for (std::size_t i = 0; i < g.functions.size(); ++i) {
    const std::string& name = g.functions[i].name;
    if (!fine(name) || taken.count(name) || taken.count(name + "Result")) continue;
    fn_names[i] = name;
    taken.insert(name);
    taken.insert(name + "Result");
  }
  for (std::size_t i = 0; i < g.variables.size(); ++i) {
    const std::string& name = g.variables[i].name;
    if (!fine(name) || taken.count(name)) continue;
    var_names[i] = name;
    taken.insert(name);
  }
  for (std::size_t i = 0; i < g.functions.size(); ++i) {
    if (!fn_names[i].empty()) continue;
    fn_names[i] = unique(better(g.functions[i].name), [&](const std::string& x) { return taken.count(x) || taken.count(x + "Result"); });
    taken.insert(fn_names[i]);
    taken.insert(fn_names[i] + "Result");
  }
  for (std::size_t i = 0; i < g.variables.size(); ++i) {
    if (!var_names[i].empty()) continue;
    var_names[i] = unique(better(g.variables[i].name), [&](const std::string& x) { return taken.count(x) != 0; });
    taken.insert(var_names[i]);
  }
  auto set_params = [&](ParamDef::Kind kind, const std::string& from, const std::string& to) {
    for (Node& n : g.nodes) {
      const NodeDef* def = lib.find(n.type);
      if (!def) continue;
      for (const ParamDef& p : def->params)
        if (p.kind == kind && n.param(p.key, p.def) == from) n.set_param(p.key, to);
    }
  };
  for (std::size_t i = 0; i < g.variables.size(); ++i) {
    const std::string from = g.variables[i].name, to = var_names[i];
    if (from == to) continue;
    // (uses first: two variables never swap names here, a new name is one nobody has)
    set_params(ParamDef::Kind::variable, from, to);
    for (StateMachine& m : g.machines)
      for (Transition& t : m.transitions)
        if (t.variable == from) t.variable = to;
    g.variables[i].name = to;
    renamed.push_back({from, to});
  }
  for (std::size_t i = 0; i < g.functions.size(); ++i) {
    Function& f = g.functions[i];
    const std::string from = f.name, to = fn_names[i];
    if (from != to) {
      set_params(ParamDef::Kind::function, from, to);
      f.name = to;
      renamed.push_back({from, to});
    }
    for (bool inputs : {true, false}) {
      std::vector<FunctionPin>& pins = inputs ? f.inputs : f.outputs;
      std::set<std::string> used;
      for (FunctionPin& p : pins) {
        const std::string old = p.name;
        const bool ok = cpp_name_ok(old) && !node_names.count(old) && old != "exec" && old != "then";
        const std::string name = unique(ok ? old : better(old), [&](const std::string& x) { return used.count(x) || (inputs && taken.count(x)); });
        used.insert(name);
        if (name == old) continue;
        // the pin on the function's nodes: the entry gives the inputs, the Return takes the outputs, the calls both
        for (Node& n : g.nodes) {
          if (n.param("function") != f.name) continue;
          const bool is_input_there = inputs ? n.type == "func.call" || n.type == "func.pure" : n.type == "func.return";
          const bool is_output_there = inputs ? n.type == "func.entry" : n.type == "func.call" || n.type == "func.pure";
          if (is_input_there) {
            for (auto& v : n.values) if (v.first == old) v.first = name;
            for (Link& l : g.links) if (l.to == n.id && l.in == old) l.in = name;
          }
          if (is_output_there)
            for (Link& l : g.links) if (l.from == n.id && l.out == old) l.out = name;
        }
        p.name = name;
        renamed.push_back({f.name + "." + old, f.name + "." + name});
      }
    }
  }
  return renamed;
}

std::string cpp_problem(const Graph& g, u32* node, const Library& lib) {
  Shape shape(g, lib);
  if (shape.build()) return {};
  if (node) *node = shape.error_node;
  return shape.error;
}

bool native_ready(const Graph& g, std::string* why, const Library& lib) {
  auto no = [&](std::string text) {
    if (why) *why = std::move(text);
    return false;
  };
  for (const Issue& i : check(g, lib))  // (a Blueprint that compiles: its rules as C++ are part of that)
    if (i.error) return no(i.text);
  // The nodes blueprint.h has a function for say so (NodeDef::native; the native tests compile a
  // Blueprint with each of them).
  for (const Node& node : g.nodes) {
    const NodeDef* def = lib.find(node.type);
    if (!def || !def->native) return no(lib.title(node) + " (node " + std::to_string(node.id) + ") is not yet a function of blueprint.h");
  }
  return true;
}

bool native_glue(const Graph& g, const std::string& name, std::string& out, std::string* error, const Library& lib) {
  std::string why;
  if (!native_ready(g, &why, lib)) {
    if (error) *error = why;
    return false;
  }
  auto builtin = [](PinType t) -> const char* {
    switch (t) {
      case PinType::boolean: return "bool";
      case PinType::integer: return "kern::i64";
      case PinType::number: return "kern::f64";
      case PinType::vector: return "kern::Vec3";
      case PinType::text: return "std::string";
      case PinType::boolean_list: return "kern::List<bool>";
      case PinType::integer_list: return "kern::List<kern::logic::Whole>";
      case PinType::number_list: return "kern::List<kern::f64>";
      case PinType::vector_list: return "kern::List<kern::Vec3>";
      case PinType::text_list: return "kern::List<std::string>";
      case PinType::actor_list: return "kern::List<kern::logic::ActorHandle>";
      default: return "kern::logic::ActorHandle";
    }
  };
  // (an enum and a struct are the class's own: <Class>::Mode)
  auto type = [&](PinType t, const std::string& of) -> std::string {
    if (element_of(t) != PinType::enumeration && element_of(t) != PinType::structure) return builtin(t);
    return is_list(t) ? "kern::List<" + name + "::" + of + ">" : name + "::" + of;
  };
  // (the notation's literal, outside the class: with the types' own names)
  std::function<std::string(const Value&, const std::string&)> spelled = [&](const Value& v, const std::string& of) -> std::string {
    if (v.type == PinType::enumeration) return name + "::" + literal(v, of, g);
    if (v.type == PinType::structure) {
      const StructType* s = g.structure(of);
      std::string r = name + "::" + of + "{";
      for (std::size_t k = 0; s && k < s->fields.size(); ++k) r += (k ? ", " : "") + spelled(field_of(v, *s, k, g), s->fields[k].type.of);
      return r + "}";
    }
    const std::string text = literal(v);
    if (v.type == PinType::vector) return "kern::Vec3" + text.substr(6);
    if (v.type == PinType::actor) return "kern::logic::ActorHandle" + text.substr(5);
    if (!is_list(v.type)) return text;
    std::string r = type(v.type, of) + "{";
    for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + spelled(v.item(k), of);
    return r + "}";
  };
  auto value = [&](const json::Value& j, PinType t, const std::string& of) { return spelled(Value::from_json(j, t, g, of), of); };
  // How a struct of the class is held as a value of the engine (kern::logic::Value), field by
  // field: what native_arg, to_value and native_set go through for it (nodes.h, detail::Held).
  // A value from outside that is no struct of this type is the struct's default.
  std::string structs;
  for (const StructType& st : g.structs) {
    const std::string full = "::" + name + "::" + st.name;
    auto held = [&](const StructField& f) -> std::string {
      if (element_of(f.type.type) != PinType::enumeration && element_of(f.type.type) != PinType::structure) return builtin(f.type.type);
      return is_list(f.type.type) ? "kern::List<::" + name + "::" + f.type.of + ">" : "::" + name + "::" + f.type.of;
    };
    std::string is, from, to;
    for (std::size_t k = 0; k < st.fields.size(); ++k) {
      const std::string at = "*v.field(" + std::to_string(k) + ")";
      is += " && is_value<" + held(st.fields[k]) + ">(" + at + ")";
      from += std::string(k ? ", " : "") + "from_value<" + held(st.fields[k]) + ">(" + at + ")";
      to += std::string(k ? ", " : "") + "to_value(s." + st.fields[k].name + ")";
    }
    structs += "template <> struct Held<" + full + "> {\n"
               "  static constexpr PinType type = PinType::structure;\n"
               "  static bool is(const Value& v) { return v.type == type && v.fields() == " + std::to_string(st.fields.size()) + is + "; }\n"
               "  static " + full + " from(const Value& v) {\n"
               "    if (!is(v)) return {};\n"
               "    return {" + from + "};\n"
               "  }\n"
               "  static Value to(const " + full + "& s) { return Value::structure({" + to + "}); }\n"
               "};\n";
  }
  if (!structs.empty()) structs = "namespace kern::logic::detail {\n" + structs + "}  // namespace kern::logic::detail\n";
  char hash[40];
  std::snprintf(hash, sizeof hash, "0x%016llxull", static_cast<unsigned long long>(graph_hash(g)));
  // The handlers of events: the runtime starts a chain at an event's node, the class has it as
  // the method on<id>; the event's values are the node's outputs.
  std::string events, running;
  Shape shape(g, lib);
  shape.build();
  for (std::size_t i = 0; i < g.nodes.size(); ++i) {
    const Node& n = g.nodes[i];
    const NodeDef* def = lib.find(n.type);
    if (def && def->latent) {
      // A node that waits: when the wait is over the runtime runs it again - <node>_resume.
      const std::string fn = node_name(def->type) + "_resume" + settings_of(*def, n, g);
      events += "      case " + std::to_string(i) + ": self." + fn + "(self.n" + std::to_string(n.id) + "); break;  // " + lib.title(n) + "\n";
      continue;
    }
    if (!def || def->kind != NodeKind::event || def->type == "func.entry") continue;
    std::vector<PinDef> in, outs;
    lib.pins(n, g, in, outs);
    std::string args;
    for (std::size_t o = 0; o < outs.size(); ++o) {
      if (outs[o].type == PinType::exec) continue;
      args += std::string(args.empty() ? "" : ", ") + "native_out<" + type(outs[o].type, outs[o].of) + ">(*self.kern_world.instance, " + std::to_string(i) + ", " + std::to_string(o) + ")";
    }
    const std::string id = std::to_string(n.id), call = "self.on" + id + "(" + args + ");";
    if (shape.at(static_cast<int>(i)).member) {
      // Its values are read outside its own body, from the member n<id>. A handler that raises
      // its own event: the inner run's values are the inner run's - the outer run has its own
      // again when that is over (as Runtime::fire does it with the node's outputs).
      running += "  int running" + id + " = 0;\n";
      events += "      case " + std::to_string(i) + ": {  // " + lib.title(n) + "\n        const auto outer = self.n" + id + ";\n        ++running" + id + ";\n        " + call +
                "\n        if (--running" + id + " > 0) self.n" + id + " = outer;\n      } break;\n";
    } else {
      events += "      case " + std::to_string(i) + ": " + call + " break;  // " + lib.title(n) + "\n";
    }
  }
  events = events.empty() ? std::string("    (void)node;\n") : "    switch (node) {\n" + events + "      default: break;\n    }\n";
  // What the functions hold of their parameters outside their own bodies (the members n<id> of
  // their entries): a handler that runs while the Blueprint is at work - a function raised the
  // event and is in the middle of its call - leaves them as they were (as Runtime::fire does it
  // with the entries' outputs for the other tiers).
  std::string keep, give_back;
  for (std::size_t i = 0; i < g.nodes.size(); ++i) {
    if (g.nodes[i].type != "func.entry" || !shape.at(static_cast<int>(i)).member) continue;
    const std::string id = std::to_string(g.nodes[i].id);
    keep += "    const auto outer" + id + " = self.n" + id + ";\n";
    give_back += " self.n" + id + " = outer" + id + ";";
  }
  if (!keep.empty())
    events = "    const bool at_work = self.kern_world.instance && self.kern_world.instance->chains() > 1;\n" + keep + events + "    if (at_work) {" + give_back + " }\n";
  std::string s = "// How the engine reaches the Blueprint " + name + " by name (written by logic::native_glue after its\n"
                  "// text): glue, not part of the notation - nothing to edit here.\n" +
                  structs +
                  "namespace {\n"
                  "struct " + name + "_Native final : kern::logic::NativeBlueprint {\n"
                  "  " + name + " self;\n" +
                  running +
                  "  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }\n"
                  "  void run(int node, int entry) override {\n"
                  "    using kern::logic::native_out;\n    (void)entry;\n" +
                  events +
                  "  }\n"
                  "  bool call(std::string_view function, const std::vector<kern::logic::Value>& args, std::vector<kern::logic::Value>& results) override {\n"
                  "    using kern::logic::native_arg;\n    using kern::logic::to_value;\n    (void)args;\n    results.clear();\n";
  for (const Function& f : g.functions) {
    std::string call = "self." + f.name + "(";
    for (std::size_t k = 0; k < f.inputs.size(); ++k)
      call += std::string(k ? ", " : "") + "native_arg<" + type(f.inputs[k].type, f.inputs[k].of) + ">(args, " + std::to_string(k) + ", " + value(f.inputs[k].value, f.inputs[k].type, f.inputs[k].of) + ")";
    call += ")";
    s += "    if (function == \"" + f.name + "\") {\n";
    if (f.outputs.empty()) {
      s += "      " + call + ";\n";
    } else {
      s += "      const auto out = " + call + ";\n      results = {";
      for (std::size_t k = 0; k < f.outputs.size(); ++k) s += std::string(k ? ", " : "") + "to_value(out." + f.outputs[k].name + ")";
      s += "};\n";
    }
    s += "      return true;\n    }\n";
  }
  s += "    return false;\n  }\n"
       "  bool get(std::string_view variable, kern::logic::Value& out) const override {\n    (void)out;\n";
  for (const Variable& v : g.variables) s += "    if (variable == \"" + v.name + "\") { out = kern::logic::to_value(self." + v.name + "); return true; }\n";
  s += "    return false;\n  }\n"
       "  bool set(std::string_view variable, const kern::logic::Value& value) override {\n    (void)value;\n";
  for (const Variable& v : g.variables) s += "    if (variable == \"" + v.name + "\") { kern::logic::native_set(self." + v.name + ", value); return true; }\n";
  s += "    return false;\n  }\n};\n"
       "[[maybe_unused]] const bool " + name + "_registered =\n"
       "    (kern::logic::register_native_blueprint(" + hash + ", [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new " + name + "_Native()); }), true);\n"
       "}  // namespace\n";
  out = std::move(s);
  return true;
}

bool to_cpp(const Graph& g, const std::string& name, std::string& out, std::string* error, const Library& lib, bool layout) {
  Shape shape(g, lib);
  if (!shape.build()) {
    if (error) *error = shape.error;
    return false;
  }
  Writer w(shape);
  w.write(name, layout);
  std::vector<int> once = w.order;
  std::sort(once.begin(), once.end());
  if (once.size() != g.nodes.size() || std::adjacent_find(once.begin(), once.end()) != once.end()) {
    // every node is written exactly once: anything else is a mistake of the notation itself
    if (error) *error = "the notation wrote " + std::to_string(w.order.size()) + " node(s) for " + std::to_string(g.nodes.size());
    return false;
  }
  out = std::move(w.out);
  return true;
}

bool from_cpp(std::string_view text, Graph& out, std::string* name, std::string* error, const Library& lib) {
  Reader r(text, lib);
  if (!r.read()) {
    if (error) *error = r.error.empty() ? std::string("the text is no Blueprint") : r.error;
    return false;
  }
  out = std::move(r.g);
  if (name) *name = r.name;
  return true;
}

bool apply_cpp(std::string_view text, const Graph& before, Graph& out, std::string* error, const Library& lib) {
  Reader r(text, lib);
  r.before = &before;
  if (!r.read()) {
    if (error) *error = r.error.empty() ? std::string("the text is no Blueprint") : r.error;
    return false;
  }
  out = std::move(r.g);
  return true;
}

bool canonical(const Graph& g, Graph& out, std::string* error, const Library& lib) {
  std::string text;
  if (!to_cpp(g, "Blueprint", text, error, lib)) return false;
  Graph read;
  if (!from_cpp(text, read, nullptr, error, lib)) return false;
  out = std::move(read);
  return true;
}

bool same_blueprint(const Graph& a, const Graph& b, std::string* difference, const Library& lib) {
  const auto da = described(a, lib), db = described(b, lib);
  for (std::size_t i = 0; i < da.size() && i < db.size(); ++i) {
    if (da[i] == db[i]) continue;
    if (difference) {
      if (da[i].first == db[i].first) *difference = da[i].first + " is \"" + da[i].second + "\" in one and \"" + db[i].second + "\" in the other";
      else *difference = "one has " + da[i].first + " (" + da[i].second + ") where the other has " + db[i].first + " (" + db[i].second + ")";
    }
    return false;
  }
  if (da.size() != db.size()) {
    if (difference) *difference = "one of them has more to it";
    return false;
  }
  return true;
}

}  // namespace kern::logic
