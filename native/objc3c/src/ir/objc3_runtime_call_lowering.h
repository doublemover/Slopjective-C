#pragma once

#include "lower/objc3_lowering_contract.h"

#include <cstddef>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Objc3RuntimeDispatchCallRequest {
  std::string result_value;
  std::string dispatch_symbol;
  std::string receiver;
  std::string selector_ptr;
  std::vector<std::string> args;
};

struct Objc3DirectDispatchCallRequest {
  std::string result_value;
  std::string callee_symbol;
  std::vector<std::string> args;
  std::size_t explicit_arg_count = 0;
};

struct Objc3RuntimeCallLoweringState {
  std::set<std::string> runtime_dispatch_symbols_used;
  bool runtime_dispatch_call_emitted = false;
  std::size_t direct_dispatch_call_sites_emitted = 0;
  std::size_t runtime_dispatch_call_sites_emitted = 0;
  std::size_t selector_pool_gep_sites_emitted = 0;

  void Reset();
  void NoteDirectDispatchCall();
  void NoteSelectorPoolGep();
  void NoteRuntimeDispatchCall(const std::string &dispatch_symbol);
};

std::string BuildObjc3RuntimeDispatchCallIR(
    const Objc3RuntimeDispatchCallRequest &request);
std::string BuildObjc3DirectDispatchCallIR(
    const Objc3DirectDispatchCallRequest &request);
void EmitObjc3RuntimeDispatchDeclarations(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3RuntimeCallLoweringState &state, std::ostringstream &out);
