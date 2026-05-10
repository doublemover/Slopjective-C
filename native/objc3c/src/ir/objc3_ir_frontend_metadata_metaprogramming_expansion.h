#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ir/objc3_ir_frontend_metadata_metaprogramming_bundles.h"

struct Objc3IRFrontendMetaprogrammingExpansionMetadata {
  std::string lowering_metaprogramming_expansion_replay_key;
  std::size_t metaprogramming_expansion_lowering_derive_inventory_sites = 0;
  std::size_t metaprogramming_expansion_lowering_derived_selector_artifact_sites = 0;
  std::size_t metaprogramming_expansion_lowering_macro_replay_visible_sites = 0;
  std::size_t metaprogramming_expansion_lowering_property_behavior_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_binding_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_getter_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_setter_sites = 0;
  std::size_t metaprogramming_expansion_lowering_replay_visible_metadata_sites = 0;
  std::size_t metaprogramming_expansion_lowering_guard_blocked_sites = 0;
  std::size_t metaprogramming_expansion_lowering_contract_violation_sites = 0;
  bool deterministic_metaprogramming_expansion_lowering_handoff = false;
  std::string lowering_metaprogramming_synthesized_emission_replay_key;
  std::size_t metaprogramming_synthesized_emitted_derive_method_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_macro_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_property_behavior_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_global_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_runtime_method_list_sites = 0;
  std::size_t metaprogramming_synthesized_guard_blocked_sites = 0;
  std::size_t metaprogramming_synthesized_contract_violation_sites = 0;
  bool deterministic_metaprogramming_synthesized_emission_handoff = false;
  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      metaprogramming_derived_method_bundles_lexicographic;
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
      metaprogramming_macro_artifact_bundles_lexicographic;
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
      metaprogramming_property_behavior_artifact_bundles_lexicographic;
};
