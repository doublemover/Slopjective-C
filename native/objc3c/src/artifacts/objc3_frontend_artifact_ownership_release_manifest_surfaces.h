#pragma once

#include <iosfwd>
#include <string>

#include "lower/contracts/lowering_arc_contracts.h"

struct Objc3ArcDiagnosticsFixitLoweringContract;
struct Objc3AutoreleasePoolScopeLoweringContract;
struct Objc3OwnershipQualifierLoweringContract;
struct Objc3RetainReleaseOperationLoweringContract;
struct Objc3WeakUnownedSemanticsLoweringContract;

namespace objc3::artifacts::frontend {

void WriteOwnershipReleaseManifestSurfaces(
    std::ostream &manifest,
    const Objc3OwnershipQualifierLoweringContract
        &ownership_qualifier_lowering_contract,
    const std::string &ownership_qualifier_lowering_replay_key,
    const Objc3RetainReleaseOperationLoweringContract
        &retain_release_operation_lowering_contract,
    const std::string &retain_release_operation_lowering_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract
        &autoreleasepool_scope_lowering_contract,
    const std::string &autoreleasepool_scope_lowering_replay_key,
    const Objc3WeakUnownedSemanticsLoweringContract
        &weak_unowned_semantics_lowering_contract,
    const std::string &weak_unowned_semantics_lowering_replay_key,
    const Objc3ArcDiagnosticsFixitLoweringContract
        &arc_diagnostics_fixit_lowering_contract,
    const std::string &arc_diagnostics_fixit_lowering_replay_key);

}  // namespace objc3::artifacts::frontend
