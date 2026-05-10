#pragma once

#include <iosfwd>
#include <string>

struct Objc3InteropCppInteropInteractionSummary;
struct Objc3InteropFfiMetadataInterfacePreservationContract;
struct Objc3InteropForeignCallLifetimeLoweringContract;
struct Objc3InteropForeignSurfaceInterfacePreservationSummary;
struct Objc3InteropHeaderModuleBridgeGenerationSummary;
struct Objc3InteropInteropLoweringContract;
struct Objc3InteropInteropRuntimeParitySummary;
struct Objc3InteropInteropSemanticModelSummary;
struct Objc3InteropSwiftInteropIsolationSummary;

namespace objc3::artifacts::frontend {

void WriteInteropManifestSurfaces(
    std::ostream &manifest,
    const Objc3InteropInteropSemanticModelSummary
        &interop_interop_semantic_model_summary,
    const Objc3InteropInteropRuntimeParitySummary
        &interop_interop_runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary
        &interop_cpp_interop_interaction_summary,
    const Objc3InteropSwiftInteropIsolationSummary
        &interop_swift_interop_isolation_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &interop_foreign_surface_interface_preservation_summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary
        &interop_header_module_bridge_generation_summary,
    const Objc3InteropInteropLoweringContract &interop_interop_lowering_contract,
    const std::string &interop_interop_lowering_replay_key,
    const Objc3InteropForeignCallLifetimeLoweringContract
        &interop_foreign_call_lifetime_lowering_contract,
    const std::string &interop_foreign_call_lifetime_lowering_replay_key,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &interop_ffi_metadata_interface_preservation_contract,
    const std::string &interop_ffi_metadata_interface_preservation_replay_key);

}  // namespace objc3::artifacts::frontend
