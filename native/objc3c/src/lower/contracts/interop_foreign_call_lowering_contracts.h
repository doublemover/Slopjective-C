#pragma once

#include <cstddef>
#include <string>

// Interop foreign-call lowering owns the core interop ABI contract and the
// lifetime-preserving call boundary for C, runtime-parity, ownership bridge,
// and host-language callable surfaces.
inline constexpr const char *kObjc3InteropInteropLoweringContractId =
    "objc3c.interop.interop.lowering.and.abi.contract.v1";
inline constexpr const char *kObjc3InteropInteropLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_interop_lowering_and_abi_contract";
inline constexpr const char *kObjc3InteropInteropLoweringModel =
    "interop-foreign-callable-sema-runtime-parity-cpp-interaction-swift-isolation-and-interface-preservation-packets-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3InteropInteropLoweringDeferredModel =
    "live-ffi-call-lowering-ownership-bridge-helper-emission-error-runtime-integration-and-cross-module-runtime-consumption-remain-later-interop-runtime-work";
inline constexpr const char *kObjc3InteropInteropLoweringLaneContract =
    "objc3c.interop.interop.lowering.abi.contract.v1";

inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringContractId =
    "objc3c.interop.foreign.call.and.lifetime.lowering.v1";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_foreign_call_and_lifetime_lowering";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringModel =
    "foreign-calls-and-cpp-swift-facing-free-functions-now-lower-through-one-deterministic-interop-call-boundary-that-preserves-ownership-lifetime-and-annotation-facts-in-manifest-and-ir";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDeferredModel =
        "cross-module-runtime-consumption-live-foreign-linking-and-runnable-host-language-integration-remain-later-interop-closeout-work";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDependencyContractId =
        kObjc3InteropInteropLoweringContractId;

struct Objc3InteropInteropLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_callable_sites = 0;
  std::size_t error_surface_sites = 0;
  std::size_t async_boundary_sites = 0;
  std::size_t swift_concurrency_metadata_sites = 0;
  std::size_t interface_preserved_foreign_callable_sites = 0;
  std::size_t interface_preserved_metadata_annotation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropForeignCallLifetimeLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_sites = 0;
  std::size_t lifetime_bridge_sites = 0;
  std::size_t metadata_preservation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidObjc3InteropInteropLoweringContract(
    const Objc3InteropInteropLoweringContract &contract);
std::string Objc3InteropInteropLoweringReplayKey(
    const Objc3InteropInteropLoweringContract &contract);
bool IsValidObjc3InteropForeignCallLifetimeLoweringContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract);
std::string Objc3InteropForeignCallLifetimeLoweringReplayKey(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract);
