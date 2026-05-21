#include "ir/objc3_ir_surface_serialization.h"

#include "ir/objc3_ir_deterministic_publication.h"

#include <sstream>

std::string SerializeObjc3IRRuntimeDispatchDeclarationSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state) {
  if (!state.runtime_dispatch_call_emitted) {
    return {};
  }
  std::ostringstream out;
  out << BuildObjc3IRCommentSurface(
      "runtime_dispatch_call_decl",
      {Objc3IRField("replay_key",
                    Objc3RuntimeDispatchDeclarationReplayKey(boundary))});
  out << "\n";
  return out.str();
}

std::string SerializeObjc3IRSynthesizedAccessorEmissionSurface(
    const Objc3IRSynthesizedAccessorEmissionStats &stats) {
  if (stats.getter_definition_count == 0 &&
      stats.setter_definition_count == 0) {
    return {};
  }
  return BuildObjc3IRCommentSurface(
      "synthesized_getter_setter_llvm_ir_generation_surface",
      {Objc3IRField(
           "contract_id",
           "objc3c.synthesized.getter.setter.llvm.ir.generation.v1"),
       Objc3IRField("getter_definitions", stats.getter_definition_count),
       Objc3IRField("setter_definitions", stats.setter_definition_count),
       Objc3IRField("read_current_property_calls",
                    stats.read_current_property_call_count),
       Objc3IRField("write_current_property_calls",
                    stats.write_current_property_call_count),
       Objc3IRField("exchange_current_property_calls",
                    stats.exchange_current_property_call_count),
       Objc3IRField("weak_load_current_property_calls",
                    stats.weak_load_current_property_call_count),
       Objc3IRField("weak_store_current_property_calls",
                    stats.weak_store_current_property_call_count),
       Objc3IRField("retain_calls", stats.retain_call_count),
       Objc3IRField("release_calls", stats.release_call_count),
       Objc3IRField("autorelease_calls", stats.autorelease_call_count)});
}

std::string SerializeObjc3IRMethodDispatchEmissionSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state,
    const Objc3IRMethodDispatchEmissionStats &stats) {
  if (!state.runtime_dispatch_call_emitted &&
      state.direct_dispatch_call_sites_emitted == 0 &&
      state.cache_aware_dispatch_call_sites_emitted == 0) {
    return {};
  }
  return BuildObjc3IRCommentSurface(
      "method_dispatch_and_selector_thunk_lowering_surface",
      {Objc3IRField("contract_id",
                    "objc3c.method.dispatch.selector.thunk.lowering.v1"),
       Objc3IRField("runtime_dispatch_symbol",
                    boundary.runtime_dispatch_symbol),
       Objc3IRFlag("runtime_dispatch_call_emitted",
                   state.runtime_dispatch_call_emitted),
       Objc3IRField("runtime_dispatch_call_sites",
                    state.runtime_dispatch_call_sites_emitted),
       Objc3IRField("cache_aware_dispatch_call_sites",
                    state.cache_aware_dispatch_call_sites_emitted),
       Objc3IRField("direct_dispatch_call_sites",
                    state.direct_dispatch_call_sites_emitted),
       Objc3IRField("selector_pool_gep_sites",
                    state.selector_pool_gep_sites_emitted),
       Objc3IRField("selector_pool_count", stats.selector_pool_count),
       Objc3IRField("direct_dispatch_candidate_sites",
                    stats.direct_dispatch_candidate_sites),
       Objc3IRField("dynamic_opt_out_sites", stats.dynamic_opt_out_sites),
       Objc3IRField("selector_pool_symbol",
                    Objc3IRSymbolOrNull(stats.selector_pool_has_entries,
                                        "@__objc3_sec_selector_pool"))});
}
