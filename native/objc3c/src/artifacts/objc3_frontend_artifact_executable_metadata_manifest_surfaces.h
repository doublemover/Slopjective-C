#pragma once

#include <iosfwd>

struct Objc3ExecutableMetadataDebugProjectionSummary;
struct Objc3ExecutableMetadataLoweringHandoffSurface;
struct Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary;
struct Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary;
struct Objc3ExecutableMetadataSemanticConsistencyBoundary;
struct Objc3ExecutableMetadataSemanticValidationSurface;
struct Objc3ExecutableMetadataSourceGraph;
struct Objc3ExecutableMetadataTypedLoweringHandoff;
struct Objc3RuntimeMetadataSourceToSectionMatrixSummary;

namespace objc3::artifacts::frontend {

void WriteExecutableRuntimeMetadataManifestSurfaces(
    std::ostream &manifest,
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary
        &runtime_metadata_source_to_section_matrix,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &executable_metadata_semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &executable_metadata_semantic_validation_surface,
    const Objc3ExecutableMetadataLoweringHandoffSurface
        &executable_metadata_lowering_handoff_surface,
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection,
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &executable_metadata_runtime_ingest_packaging_contract,
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        &executable_metadata_runtime_ingest_binary_boundary);

}  // namespace objc3::artifacts::frontend
