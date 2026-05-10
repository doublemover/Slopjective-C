#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_metaprogramming_synthesized_emission.h"

struct Objc3IRFrontendMetaprogrammingExpansionLoweringMetadata {
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
};

struct Objc3IRFrontendMetaprogrammingExpansionMetadata
    : Objc3IRFrontendMetaprogrammingExpansionLoweringMetadata,
      Objc3IRFrontendMetaprogrammingSynthesizedEmissionMetadata {};
