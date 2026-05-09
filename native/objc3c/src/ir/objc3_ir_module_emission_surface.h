#pragma once

#include <cstddef>
#include <sstream>

#include "ir/objc3_ir_runtime_dispatch_state.h"
#include "lower/objc3_lowering_contract.h"

struct Objc3IRSynthesizedAccessorEmissionStats {
  std::size_t getter_definition_count = 0;
  std::size_t setter_definition_count = 0;
  std::size_t read_current_property_call_count = 0;
  std::size_t write_current_property_call_count = 0;
  std::size_t exchange_current_property_call_count = 0;
  std::size_t weak_load_current_property_call_count = 0;
  std::size_t weak_store_current_property_call_count = 0;
  std::size_t retain_call_count = 0;
  std::size_t release_call_count = 0;
  std::size_t autorelease_call_count = 0;
};

struct Objc3IRMethodDispatchEmissionStats {
  std::size_t selector_pool_count = 0;
  std::size_t direct_dispatch_candidate_sites = 0;
  std::size_t dynamic_opt_out_sites = 0;
  bool selector_pool_has_entries = false;
};

void EmitObjc3IRRuntimeDispatchDeclarationSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state, std::ostringstream &out);
void EmitObjc3IRSynthesizedAccessorEmissionSurface(
    const Objc3IRSynthesizedAccessorEmissionStats &stats,
    std::ostringstream &out);
void EmitObjc3IRMethodDispatchEmissionSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state,
    const Objc3IRMethodDispatchEmissionStats &stats, std::ostringstream &out);
