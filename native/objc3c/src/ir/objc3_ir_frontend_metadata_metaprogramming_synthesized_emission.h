#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_metaprogramming_artifact_model.h"

struct Objc3IRFrontendMetaprogrammingSynthesizedEmissionCountersMetadata {
  std::string lowering_metaprogramming_synthesized_emission_replay_key;
  std::size_t metaprogramming_synthesized_emitted_derive_method_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_macro_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_property_behavior_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_global_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_runtime_method_list_sites = 0;
  std::size_t metaprogramming_synthesized_guard_blocked_sites = 0;
  std::size_t metaprogramming_synthesized_contract_violation_sites = 0;
  bool deterministic_metaprogramming_synthesized_emission_handoff = false;
};

struct Objc3IRFrontendMetaprogrammingSynthesizedEmissionMetadata
    : Objc3IRFrontendMetaprogrammingSynthesizedEmissionCountersMetadata,
      Objc3IRFrontendMetaprogrammingArtifactModelMetadata {};
