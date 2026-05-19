#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

#include "io/objc3_process.h"

bool TryValidateImportedLinkPlanInput(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    const Objc3CrossModuleRuntimeLinkPlanImportedInput &imported_input,
    std::unordered_set<std::string> &seen_translation_unit_identity_keys,
    std::unordered_set<std::uint64_t> &seen_registration_ordinals,
    std::unordered_set<std::string> &seen_error_handling_replay_keys,
    std::unordered_set<std::string> &seen_concurrency_actor_replay_keys,
    std::unordered_set<std::string> &seen_interop_ffi_replay_keys,
    std::unordered_set<std::string>
        &seen_interop_header_module_bridge_replay_keys,
    std::unordered_set<std::string> &seen_metaprogramming_host_cache_replay_keys,
    std::string &error);
