#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_runtime_dispatch_state.h"
#include "ir/objc3_ir_synthetic_method_emission.h"

struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;
struct Objc3IREmitterStateInitialization;
struct Objc3IRFrontendMetadata;
struct Objc3Program;

struct Objc3IREmitterRuntimeSessionInputs {
  const Objc3Program &program;
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3IREmitterStateInitialization &initialized_state;
  std::unordered_map<std::string, int> &global_const_values;
  std::unordered_set<std::string> &global_nil_proven_symbols;
};

class Objc3IREmitterRuntimeSession {
 public:
  explicit Objc3IREmitterRuntimeSession(
      const Objc3IREmitterRuntimeSessionInputs &inputs);

  void ResetForEmission();

  Objc3IREmitterServiceContextState ServiceContextState();
  Objc3IREmitterServiceContextCallbacks ServiceContextCallbacks();

  bool UnsupportedFailClosedPathTriggered() const;
  std::string UnsupportedFailClosedError() const;
  const Objc3IRSyntheticMethodEmissionStats &SyntheticMethodStats() const;

 private:
  std::string EmitUnsupportedI32Value(const std::string &reason);

  Objc3IREmitterRuntimeSessionInputs inputs_;
  std::vector<std::string> block_function_definitions_;
  std::unordered_set<std::string> emitted_block_invoke_symbols_;
  std::unordered_set<std::string> emitted_block_descriptor_symbols_;
  std::unordered_set<std::string> emitted_block_copy_helper_symbols_;
  std::unordered_set<std::string> emitted_block_dispose_helper_symbols_;
  Objc3IRRuntimeDispatchCallState runtime_dispatch_call_state_;
  Objc3IRSyntheticMethodEmissionStats synthetic_method_stats_;
  bool unsupported_fail_closed_path_triggered_ = false;
  std::string unsupported_fail_closed_path_reason_;
};
