#include "artifacts/objc3_frontend_artifact_executable_metadata_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_runtime_ingest_binary_artifacts.h"
#include "artifacts/objc3_frontend_runtime_metadata_section_artifacts.h"

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
        &executable_metadata_runtime_ingest_binary_boundary) {
  manifest
      << ",\"objc_executable_metadata_source_graph\":"
      << BuildExecutableMetadataSourceGraphJson(executable_metadata_source_graph)
      // source-to-section matrix anchor: lane-A must publish one
      // canonical node-to-emitted-section matrix that preserves the A001
      // inventory and explicitly marks interface/implementation/metaclass/
      // method rows as no-standalone-emission-yet until later work.
      << ",\"objc_runtime_metadata_source_to_section_matrix\":"
      << BuildRuntimeMetadataSourceToSectionMatrixSummaryJson(
             runtime_metadata_source_to_section_matrix)
      << ",\"objc_executable_metadata_semantic_consistency_boundary\":"
      << BuildExecutableMetadataSemanticConsistencyBoundaryJson(
             executable_metadata_semantic_consistency_boundary)
      << ",\"objc_executable_metadata_semantic_validation_surface\":"
      << BuildExecutableMetadataSemanticValidationSurfaceJson(
             executable_metadata_semantic_validation_surface)
      // lowering-handoff anchor: metadata graph lowering
      // handoff freeze must publish as a first-class semantic surface so
      // typed handoff and parse/lowering projections consume one schema.
      << ",\"objc_executable_metadata_lowering_handoff_surface\":"
      << BuildExecutableMetadataLoweringHandoffSurfaceJson(
             executable_metadata_lowering_handoff_surface)
      // typed-lowering anchor: the lowering-ready packet must
      // publish the ordered metadata graph payload itself rather than a
      // count-only summary so downstream lowering can consume one schema.
      << ",\"objc_executable_metadata_typed_lowering_handoff\":"
      << BuildExecutableMetadataTypedLoweringHandoffJson(
             executable_metadata_typed_lowering_handoff)
      // debug-projection anchor: lane-C must publish one
      // canonical metadata inspection matrix across manifest and IR-facing
      // surfaces before runtime section emission lands.
      << ",\"objc_executable_metadata_debug_projection\":"
      << BuildExecutableMetadataDebugProjectionSummaryJson(
             executable_metadata_debug_projection)
      // runtime-ingest packaging anchor: lane-D must freeze one
      // canonical manifest transport boundary over the typed handoff and
      // debug-projection packets before section emission and startup
      // registration land.
      << ",\"objc_executable_metadata_runtime_ingest_packaging_contract\":"
      << BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
             executable_metadata_runtime_ingest_packaging_contract)
      // binary-boundary anchor: lane-D must materialize a real
      // runtime-facing binary envelope over the frozen D001/C002/C003
      // packets so later section-emission/bootstrap work consumes one
      // deterministic artifact boundary instead of reparsing manifest JSON.
      // semantic-closure gate anchor: lane-E freezes the
      // aggregate the existing boundary here so the section
      // emission consumes one synchronized metadata closure proof.
      // corpus-sync anchor: integrated corpus probes must
      // observe these synchronized metadata surfaces through the real
      // frontend runner path rather than mock packets.
      << ",\"objc_executable_metadata_runtime_ingest_binary_boundary\":"
      << BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson(
             executable_metadata_runtime_ingest_binary_boundary);
}

}  // namespace objc3::artifacts::frontend
