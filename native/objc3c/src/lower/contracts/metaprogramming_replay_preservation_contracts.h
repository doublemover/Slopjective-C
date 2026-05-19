#pragma once

#include <string>

// Metaprogramming replay preservation owns the module-interface import surface
// that keeps synthesized method, macro, and property behavior replay facts
// stable across separate compilation boundaries.
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName =
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationSourceModel =
        "runtime-import-surface-artifacts-preserve-metaprogramming-derived-method-macro-and-property-behavior-replay-facts-for-separate-compilation-and-interface-inspection";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationModel =
        "provider-and-consumer-import-surfaces-preserve-metaprogramming-synthesized-emission-counts-replay-keys-and-interface-vs-implementation-property-behavior-splits-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationFailClosedModel =
        "missing-or-drifted-metaprogramming-preservation-packets-disable-cross-module-metaprogramming-preservation-claims";

std::string Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary();
