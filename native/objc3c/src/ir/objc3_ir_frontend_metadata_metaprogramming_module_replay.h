#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_metaprogramming_module_replay_imports.h"

struct Objc3IRFrontendMetaprogrammingModuleReplayLocalMetadata {
  std::string lowering_metaprogramming_module_interface_replay_preservation_key;
  std::size_t metaprogramming_module_replay_local_derive_method_count = 0;
  std::size_t metaprogramming_module_replay_local_macro_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_local_interface_property_behavior_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_local_implementation_property_behavior_artifact_count =
          0;
  std::size_t metaprogramming_module_replay_local_runtime_method_list_count = 0;
};

struct Objc3IRFrontendMetaprogrammingModuleReplayMetadata
    : Objc3IRFrontendMetaprogrammingModuleReplayLocalMetadata,
      Objc3IRFrontendMetaprogrammingModuleReplayImportsMetadata {};
