#include "ir/objc3_ir_module_emission_surface.h"

void EmitObjc3IRRuntimeDispatchDeclarationSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state, std::ostringstream &out) {
  if (!state.runtime_dispatch_call_emitted) {
    return;
  }
  out << "; runtime_dispatch_call_decl = "
      << Objc3RuntimeDispatchDeclarationReplayKey(boundary) << "\n\n";
}

void EmitObjc3IRSynthesizedAccessorEmissionSurface(
    const Objc3IRSynthesizedAccessorEmissionStats &stats,
    std::ostringstream &out) {
  if (stats.getter_definition_count == 0 &&
      stats.setter_definition_count == 0) {
    return;
  }
  out << "; synthesized_getter_setter_llvm_ir_generation_surface = "
      << "contract_id=objc3c.synthesized.getter.setter.llvm.ir.generation.v1"
      << ";getter_definitions=" << stats.getter_definition_count
      << ";setter_definitions=" << stats.setter_definition_count
      << ";read_current_property_calls="
      << stats.read_current_property_call_count
      << ";write_current_property_calls="
      << stats.write_current_property_call_count
      << ";exchange_current_property_calls="
      << stats.exchange_current_property_call_count
      << ";weak_load_current_property_calls="
      << stats.weak_load_current_property_call_count
      << ";weak_store_current_property_calls="
      << stats.weak_store_current_property_call_count
      << ";retain_calls=" << stats.retain_call_count
      << ";release_calls=" << stats.release_call_count
      << ";autorelease_calls=" << stats.autorelease_call_count << "\n";
}

void EmitObjc3IRMethodDispatchEmissionSurface(
    const Objc3LoweringIRBoundary &boundary,
    const Objc3IRRuntimeDispatchCallState &state,
    const Objc3IRMethodDispatchEmissionStats &stats, std::ostringstream &out) {
  if (!state.runtime_dispatch_call_emitted &&
      state.direct_dispatch_call_sites_emitted == 0) {
    return;
  }
  out << "; method_dispatch_and_selector_thunk_lowering_surface = "
      << "contract_id=objc3c.method.dispatch.selector.thunk.lowering.v1"
      << ";runtime_dispatch_symbol=" << boundary.runtime_dispatch_symbol
      << ";runtime_dispatch_call_emitted="
      << (state.runtime_dispatch_call_emitted ? "true" : "false")
      << ";runtime_dispatch_call_sites="
      << state.runtime_dispatch_call_sites_emitted
      << ";direct_dispatch_call_sites="
      << state.direct_dispatch_call_sites_emitted
      << ";selector_pool_gep_sites=" << state.selector_pool_gep_sites_emitted
      << ";selector_pool_count=" << stats.selector_pool_count
      << ";direct_dispatch_candidate_sites="
      << stats.direct_dispatch_candidate_sites
      << ";dynamic_opt_out_sites=" << stats.dynamic_opt_out_sites
      << ";selector_pool_symbol="
      << (stats.selector_pool_has_entries ? "@__objc3_sec_selector_pool"
                                          : "null")
      << "\n";
}
