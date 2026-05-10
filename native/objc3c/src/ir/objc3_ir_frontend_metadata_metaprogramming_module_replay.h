#pragma once

#include "ir/objc3_ir_frontend_metadata_metaprogramming_module_replay_imports.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_module_replay_local.h"

struct Objc3IRFrontendMetaprogrammingModuleReplayMetadata
    : Objc3IRFrontendMetaprogrammingModuleReplayLocalMetadata,
      Objc3IRFrontendMetaprogrammingModuleReplayImportsMetadata {};
