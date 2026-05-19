#pragma once

#include "ir/objc3_ir_frontend_metadata_metaprogramming_expansion.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_module_replay.h"

struct Objc3IRFrontendMetaprogrammingMetadata
    : Objc3IRFrontendMetaprogrammingExpansionMetadata,
      Objc3IRFrontendMetaprogrammingModuleReplayMetadata {};
