#include "lower/objc3_lowering_contract.h"

#include <sstream>
#include <string>

std::string Objc3RuntimeBackedObjectOwnershipAttributeSurfaceSummary() {
  std::ostringstream out;
  // runtime-backed object ownership attribute surface anchor:
  // ownership-bearing property/member facts stop being manifest-only evidence
  // by flowing into the emitted property descriptor payload that the runtime
  // already consumes for property realization and reflection.
  out << "contract="
      << kObjc3RuntimeBackedObjectOwnershipAttributeSurfaceContractId
      << ";source_model="
      << kObjc3RuntimeBackedObjectOwnershipAttributeSourceModel
      << ";descriptor_model="
      << kObjc3RuntimeBackedObjectOwnershipAttributeDescriptorModel
      << ";runtime_model="
      << kObjc3RuntimeBackedObjectOwnershipAttributeRuntimeModel
      << ";fail_closed_model="
      << kObjc3RuntimeBackedObjectOwnershipAttributeFailClosedModel
      << ";non_goals=no-live-arc-hook-emission-no-source-recovery";
  return out.str();
}

std::string Objc3RetainableObjectSemanticRulesFreezeSummary() {
  std::ostringstream out;
  // retainable-object semantic-rule freeze anchor: runtime-backed
  // property/member ownership metadata and storage legality are now truthful
  // live sema-enforced surfaces, but retain/release legality,
  // autoreleasepool execution, and destruction-order behavior remain
  // summary-driven and fail-closed until the next runtime step lands.
  out << "contract=" << kObjc3RetainableObjectSemanticRulesFreezeContractId
      << ";semantic_model="
      << kObjc3RetainableObjectSemanticRulesSemanticModel
      << ";destruction_model="
      << kObjc3RetainableObjectSemanticRulesDestructionModel
      << ";failure_model=" << kObjc3RetainableObjectSemanticRulesFailClosedModel;
  return out.str();
}

std::string Objc3RuntimeBackedStorageOwnershipLegalitySummary() {
  std::ostringstream out;
  // runtime-backed storage ownership legality anchor: explicit
  // ownership qualifiers on Objective-C object properties now participate in
  // live semantic legality. Weak and unsafe-unretained qualifiers must agree
  // with the concrete runtime-backed storage modifier family before metadata
  // emission proceeds.
  out << "contract=" << kObjc3RuntimeBackedStorageOwnershipLegalityContractId
      << ";owned_storage_model="
      << kObjc3RuntimeBackedStorageOwnershipOwnedStorageModel
      << ";weak_unowned_model="
      << kObjc3RuntimeBackedStorageOwnershipWeakUnownedModel
      << ";failure_model=" << kObjc3RuntimeBackedStorageOwnershipFailClosedModel;
  return out.str();
}

std::string Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary() {
  std::ostringstream out;
  // autoreleasepool/destruction-order semantic expansion anchor:
  // autoreleasepool scopes still fail closed, but owned runtime-backed object
  // storage now upgrades that rejection into a deterministic destruction-order
  // edge diagnostic rather than leaving the ownership-sensitive case
  // indistinguishable from a plain autoreleasepool parse-only probe.
  out << "contract="
      << kObjc3RuntimeBackedAutoreleasepoolDestructionOrderContractId
      << ";autoreleasepool_model="
      << kObjc3RuntimeBackedAutoreleasepoolModel
      << ";destruction_model=" << kObjc3RuntimeBackedDestructionOrderModel
      << ";failure_model="
      << kObjc3RuntimeBackedAutoreleasepoolDestructionOrderFailClosedModel;
  return out.str();
}

std::string Objc3OwnershipLoweringBaselineSummary() {
  std::ostringstream out;
  // ownership-lowering baseline freeze anchor: runtime-backed
  // ownership metadata and sema legality are already live, but retain/release,
  // autoreleasepool, and weak/unowned execution still stop at legacy lowering
  // summaries instead of emitting a summary-only-without-live-runtime-hook
  // widening before the next runtime step.
  out << "contract=" << kObjc3OwnershipLoweringBaselineContractId
      << ";ownership_qualifier_model="
      << kObjc3OwnershipLoweringBaselineQualifierModel
      << ";runtime_hook_model="
      << kObjc3OwnershipLoweringBaselineRuntimeHookModel
      << ";autoreleasepool_model="
      << kObjc3OwnershipLoweringBaselineAutoreleasepoolModel
      << ";fail_closed_model="
      << kObjc3OwnershipLoweringBaselineFailClosedModel
      << ";ownership_qualifier_lane=" << kObjc3OwnershipQualifierLoweringLaneContract
      << ";retain_release_lane=" << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";autoreleasepool_lane=" << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract;
  return out.str();
}

std::string Objc3OwnershipRuntimeHookEmissionSummary() {
  std::ostringstream out;
  // runtime hook emission anchor: synthesized accessors now execute
  // through runtime-owned helper entrypoints that operate on the current
  // runtime dispatch frame and realized property layout, while preserving the
  // existing synthesized accessor descriptor/storage artifact surface.
  out << "contract=" << kObjc3OwnershipRuntimeHookEmissionContractId
      << ";accessor_model="
      << kObjc3OwnershipRuntimeHookEmissionAccessorModel
      << ";property_context_model="
      << kObjc3OwnershipRuntimeHookEmissionPropertyContextModel
      << ";autorelease_model="
      << kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel
      << ";fail_closed_model="
      << kObjc3OwnershipRuntimeHookEmissionFailClosedModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";read_property_symbol=" << kObjc3RuntimeReadCurrentPropertyI32Symbol
      << ";write_property_symbol=" << kObjc3RuntimeWriteCurrentPropertyI32Symbol
      << ";exchange_property_symbol="
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
      << ";weak_load_symbol=" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol=" << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  return out.str();
}

std::string Objc3RuntimeMemoryManagementApiSummary() {
  std::ostringstream out;
  // runtime memory-management API freeze anchor: the public runtime
  // ABI still stops at registration/lookup/dispatch, while lowered ownership
  // helpers remain private bootstrap-internal entrypoints that runtime probes
  // and lowered IR may consume without widening the stable public header yet.
  out << "contract=" << kObjc3RuntimeMemoryManagementApiContractId
      << ";reference_model="
      << kObjc3RuntimeMemoryManagementApiReferenceModel
      << ";weak_model=" << kObjc3RuntimeMemoryManagementApiWeakModel
      << ";autoreleasepool_model="
      << kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel
      << ";fail_closed_model="
      << kObjc3RuntimeMemoryManagementApiFailClosedModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";read_property_symbol=" << kObjc3RuntimeReadCurrentPropertyI32Symbol
      << ";write_property_symbol=" << kObjc3RuntimeWriteCurrentPropertyI32Symbol
      << ";exchange_property_symbol="
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
      << ";weak_load_symbol=" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol=" << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  return out.str();
}

std::string Objc3RuntimeMemoryManagementImplementationSummary() {
  std::ostringstream out;
  // runtime memory-management implementation anchor: runtime-backed
  // object execution now owns live refcount, weak-table, and autoreleasepool
  // behavior behind private helper entrypoints and emitted autoreleasepool
  // lowering rather than the older summary-only/fail-closed lane.
  out << "contract=" << kObjc3RuntimeMemoryManagementImplementationContractId
      << ";refcount_model="
      << kObjc3RuntimeMemoryManagementImplementationRefcountModel
      << ";weak_model=" << kObjc3RuntimeMemoryManagementImplementationWeakModel
      << ";autoreleasepool_model="
      << kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel
      << ";fail_closed_model="
      << kObjc3RuntimeMemoryManagementImplementationFailClosedModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";push_autoreleasepool_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_autoreleasepool_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";weak_load_symbol=" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol=" << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  return out.str();
}

std::string Objc3OwnershipRuntimeGateSummary() {
  std::ostringstream out;
  // ownership-runtime-gate freeze anchor: lane-E now freezes the
  // supported ownership runtime slice and its non-goals using the already-live
  // C002/D001/D002 implementation surfaces as the truthful evidence boundary.
  // ownership-smoke closeout anchor: the runnable smoke matrix consumes this
  // same gate summary unchanged for closeout.
  out << "contract=" << kObjc3OwnershipRuntimeGateContractId
      << ";supported_model=" << kObjc3OwnershipRuntimeGateSupportedModel
      << ";evidence_model=" << kObjc3OwnershipRuntimeGateEvidenceModel
      << ";non_goal_model=" << kObjc3OwnershipRuntimeGateNonGoalModel
      << ";fail_closed_model=" << kObjc3OwnershipRuntimeGateFailClosedModel
      << ";ownership_hook_contract="
      << kObjc3OwnershipRuntimeHookEmissionContractId
      << ";memory_api_contract="
      << kObjc3RuntimeMemoryManagementApiContractId
      << ";memory_implementation_contract="
      << kObjc3RuntimeMemoryManagementImplementationContractId;
  return out.str();
}
