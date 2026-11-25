// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/logic/node_type.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;


// engine.node_type.type: kern::logic::NodeType::type
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::logic::NodeType>(o, in, 0, "node_type");
  if (!self) return false;
  decltype(auto) r = self->type();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.node_type.label: kern::logic::NodeType::label
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::logic::NodeType>(o, in, 0, "node_type");
  if (!self) return false;
  decltype(auto) r = self->label();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.node_type.event: kern::logic::NodeType::event
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::logic::NodeType>(o, in, 0, "node_type");
  if (!self) return false;
  decltype(auto) r = self->event();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.node_type.category: kern::logic::NodeType::category
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::logic::NodeType>(o, in, 0, "node_type");
  if (!self) return false;
  decltype(auto) r = self->category();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.node_type.help: kern::logic::NodeType::help
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::logic::NodeType>(o, in, 0, "node_type");
  if (!self) return false;
  decltype(auto) r = self->help();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.node_type.run: kern::logic::NodeType::run
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::logic::NodeType>(o, in, 0, "node_type");
  if (!self) return false;
  self->run();
  return true;
}

// engine.logic.register_node_type: kern::logic::register_node_type
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::NodeType* a0 = ev::maybe<kern::logic::NodeType>(o, in, 0, "object");
  if (!o.error.empty()) return false;
  decltype(auto) r = kern::logic::register_node_type(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// kern::logic::NodeType: the object of the engine a Blueprint is (Graph::implements "logic_NodeType")
class Implements_logic_NodeType final : public kern::logic::NodeType {
 public:
  Implements_logic_NodeType(EngineObjects& o, std::shared_ptr<Implementer> by) : o_(o), by_(std::move(by)) {}
  decltype(std::declval<const kern::logic::NodeType&>().type()) type() const override {
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("type", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::logic::NodeType&>().type()) kern_r = ev::blank<decltype(std::declval<const kern::logic::NodeType&>().type())>();
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::logic::NodeType&>().label()) label() const override {
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("label", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::logic::NodeType&>().label()) kern_r = ev::blank<decltype(std::declval<const kern::logic::NodeType&>().label())>();
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::logic::NodeType&>().event()) event() const override {
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("event", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::logic::NodeType&>().event()) kern_r = ev::blank<decltype(std::declval<const kern::logic::NodeType&>().event())>();
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::logic::NodeType&>().category()) category() const override {
    if (!by_->has("category")) return kern::logic::NodeType::category();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("category", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::logic::NodeType&>().category()) kern_r = ev::blank<decltype(std::declval<const kern::logic::NodeType&>().category())>();
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::logic::NodeType&>().help()) help() const override {
    if (!by_->has("help")) return kern::logic::NodeType::help();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("help", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::logic::NodeType&>().help()) kern_r = ev::blank<decltype(std::declval<const kern::logic::NodeType&>().help())>();
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::logic::NodeType&>().run()) run() override {
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    by_->call("run", kern_in, kern_out);
  }

 private:
  EngineObjects& o_;
  std::shared_ptr<Implementer> by_;
};
i64 implements_logic_NodeType(EngineObjects& o, std::shared_ptr<Implementer> by) {
  return o.own<Implements_logic_NodeType, kern::logic::NodeType>(std::make_unique<Implements_logic_NodeType>(o, std::move(by)));
}

const EngineCall kCalls[] = {
    {"engine.node_type.type", &c0},
    {"engine.node_type.label", &c1},
    {"engine.node_type.event", &c2},
    {"engine.node_type.category", &c3},
    {"engine.node_type.help", &c4},
    {"engine.node_type.run", &c5},
    {"engine.logic.register_node_type", &c6},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineImplement kAdapters[] = {
    {"logic_NodeType", &implements_logic_NodeType},
};
const EngineImplements kAllAdapters(kAdapters, std::size(kAdapters));

}  // namespace
}  // namespace kern::logic
