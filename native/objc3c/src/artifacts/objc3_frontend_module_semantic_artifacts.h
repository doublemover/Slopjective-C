#pragma once

#include <string>

#include "lower/contracts/cross_module_lowering_contracts.h"
#include "sema/objc3_sema_contract_runtime_readiness_keys.h"
#include "sema/objc3_sema_parity_contract_surface.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] Objc3ModuleImportGraphLoweringContract
BuildModuleImportGraphLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3NamespaceCollisionShadowingLoweringContract
BuildNamespaceCollisionShadowingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3PublicPrivateApiPartitionLoweringContract
BuildPublicPrivateApiPartitionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3IncrementalModuleCacheInvalidationLoweringContract
BuildIncrementalModuleCacheInvalidationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] Objc3CrossModuleConformanceLoweringContract
BuildCrossModuleConformanceLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface);

[[nodiscard]] std::string
BuildCrossModuleSemanticContractsDiagnosticsSummaryJson(
    const Objc3CrossModuleSemanticContractsDiagnosticsSummary &summary);

}  // namespace objc3::artifacts::frontend
