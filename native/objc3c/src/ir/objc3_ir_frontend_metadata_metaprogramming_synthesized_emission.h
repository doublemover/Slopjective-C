#pragma once

#include <cstddef>

#include "ir/objc3_ir_frontend_metadata_metaprogramming_artifact_model.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_synthesized_emission_sites.h"

struct Objc3IRFrontendMetaprogrammingSynthesizedEmissionCountersMetadata {
  std::size_t metaprogramming_synthesized_guard_blocked_sites = 0;
  std::size_t metaprogramming_synthesized_contract_violation_sites = 0;
  bool deterministic_metaprogramming_synthesized_emission_handoff = false;
};

struct Objc3IRFrontendMetaprogrammingSynthesizedEmissionMetadata
    : Objc3IRFrontendMetaprogrammingSynthesizedEmissionSitesMetadata,
      Objc3IRFrontendMetaprogrammingSynthesizedEmissionCountersMetadata,
      Objc3IRFrontendMetaprogrammingArtifactModelMetadata {};
