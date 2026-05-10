#pragma once

#include <cstddef>

#include "ir/objc3_ir_frontend_metadata_metaprogramming_expansion_lowering_sites.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_synthesized_emission.h"

struct Objc3IRFrontendMetaprogrammingExpansionLoweringMetadata {
  std::size_t metaprogramming_expansion_lowering_guard_blocked_sites = 0;
  std::size_t metaprogramming_expansion_lowering_contract_violation_sites = 0;
  bool deterministic_metaprogramming_expansion_lowering_handoff = false;
};

struct Objc3IRFrontendMetaprogrammingExpansionMetadata
    : Objc3IRFrontendMetaprogrammingExpansionLoweringSitesMetadata,
      Objc3IRFrontendMetaprogrammingExpansionLoweringMetadata,
      Objc3IRFrontendMetaprogrammingSynthesizedEmissionMetadata {};
