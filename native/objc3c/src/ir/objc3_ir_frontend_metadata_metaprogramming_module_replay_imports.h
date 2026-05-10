#pragma once

#include <cstddef>

#include "ir/objc3_ir_frontend_metadata_metaprogramming_module_replay_readiness.h"

struct Objc3IRFrontendMetaprogrammingModuleReplayImportCountsMetadata {
  std::size_t metaprogramming_module_replay_imported_module_count = 0;
  std::size_t metaprogramming_module_replay_imported_derive_method_count = 0;
  std::size_t metaprogramming_module_replay_imported_macro_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_imported_interface_property_behavior_artifact_count =
          0;
  std::size_t
      metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count =
          0;
  std::size_t metaprogramming_module_replay_imported_runtime_method_list_count = 0;
};

struct Objc3IRFrontendMetaprogrammingModuleReplayImportsMetadata
    : Objc3IRFrontendMetaprogrammingModuleReplayImportCountsMetadata,
      Objc3IRFrontendMetaprogrammingModuleReplayReadinessMetadata {};
