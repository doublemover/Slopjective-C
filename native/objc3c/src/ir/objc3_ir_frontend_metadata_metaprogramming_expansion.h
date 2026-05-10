#pragma once

#include "ir/objc3_ir_frontend_metadata_metaprogramming_expansion_lowering_guards.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_expansion_lowering_sites.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_synthesized_emission.h"

struct Objc3IRFrontendMetaprogrammingExpansionMetadata
    : Objc3IRFrontendMetaprogrammingExpansionLoweringSitesMetadata,
      Objc3IRFrontendMetaprogrammingExpansionLoweringGuardsMetadata,
      Objc3IRFrontendMetaprogrammingSynthesizedEmissionMetadata {};
