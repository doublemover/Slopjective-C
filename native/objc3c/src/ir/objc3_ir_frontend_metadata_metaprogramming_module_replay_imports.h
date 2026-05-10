#pragma once

#include <cstddef>

struct Objc3IRFrontendMetaprogrammingModuleReplayImportsMetadata {
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
  bool metaprogramming_module_replay_runtime_import_artifact_ready = false;
  bool metaprogramming_module_replay_separate_compilation_preservation_ready = false;
  bool deterministic_metaprogramming_module_interface_replay_handoff = false;
};
