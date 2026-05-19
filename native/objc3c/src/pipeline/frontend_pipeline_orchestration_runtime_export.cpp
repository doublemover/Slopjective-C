#include "pipeline/frontend_pipeline_orchestration_owners.h"

#include <string>
#include <vector>

#include "parse/objc3_ast_builder_contract.h"
#include "parse/objc3_parse_support.h"
#include "pipeline/frontend_executable_metadata_handoff.h"
#include "pipeline/frontend_executable_metadata_semantic_surface_helpers.h"
#include "pipeline/frontend_executable_metadata_source_graph_helpers.h"
#include "pipeline/frontend_phase_publication_helpers.h"
#include "pipeline/frontend_runtime_export_enforcement_helpers.h"
#include "pipeline/frontend_runtime_metadata_boundary_helpers.h"
#include "pipeline/frontend_runtime_metadata_source_record_helpers.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"

namespace objc3_frontend_pipeline_orchestration {

void PopulateRuntimeExportReadiness(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options) {
  result.runtime_metadata_source_records =
      objc3c::pipeline::orchestration::BuildRuntimeMetadataSourceRecordSet(
          Objc3ParsedProgramAst(result.program));
  result.executable_metadata_source_graph =
      objc3c::pipeline::orchestration::BuildExecutableMetadataSourceGraph(
          Objc3ParsedProgramAst(result.program),
          result.runtime_metadata_source_records);
  result.executable_metadata_semantic_consistency_boundary =
      objc3c::pipeline::orchestration::BuildExecutableMetadataSemanticConsistencyBoundary(
          result.executable_metadata_source_graph,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary);
  result.executable_metadata_semantic_validation_surface =
      objc3c::pipeline::orchestration::BuildExecutableMetadataSemanticValidationSurface(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.sema_type_metadata_handoff,
          result.class_protocol_category_linking_summary);
  result.executable_metadata_lowering_handoff_surface =
      BuildExecutableMetadataLoweringHandoffSurface(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.executable_metadata_semantic_validation_surface,
          result.sema_type_metadata_handoff,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary,
          result.sema_parity_surface);
  result.executable_metadata_typed_lowering_handoff =
      BuildExecutableMetadataTypedLoweringHandoff(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.executable_metadata_semantic_validation_surface,
          result.executable_metadata_lowering_handoff_surface);
  result.runtime_metadata_source_ownership_boundary =
      objc3c::pipeline::orchestration::BuildRuntimeMetadataSourceOwnershipBoundary(
          result.runtime_metadata_source_records,
          result.sema_type_metadata_handoff);
  result.typed_sema_to_lowering_contract_surface =
      BuildObjc3TypedSemaToLoweringContractSurface(result, options);
  result.runtime_export_legality_boundary =
      objc3c::pipeline::orchestration::BuildRuntimeExportLegalityBoundary(
          result.runtime_metadata_source_ownership_boundary,
          result.typed_sema_to_lowering_contract_surface,
          result.integration_surface,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.object_pointer_nullability_generics_summary,
          result.symbol_graph_scope_resolution_summary,
          result.sema_parity_surface);
  result.runtime_export_enforcement_summary =
      objc3c::pipeline::orchestration::BuildRuntimeExportEnforcementSummary(
          result.runtime_metadata_source_records,
          result.runtime_export_legality_boundary);
}

void PublishRuntimeExportDiagnostics(Objc3FrontendPipelineResult &result) {
  if (result.stage_diagnostics.semantic.empty() &&
      objc3c::pipeline::orchestration::HasRuntimeMetadataSourceRecords(
          result.runtime_metadata_source_records) &&
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          result.runtime_export_enforcement_summary)) {
    const std::vector<
        objc3c::pipeline::orchestration::Objc3RuntimeExportBlockingDiagnostic>
        runtime_export_blocking_diagnostics =
            objc3c::pipeline::orchestration::BuildRuntimeExportBlockingDiagnostics(
                result.runtime_metadata_source_records,
                result.runtime_export_enforcement_summary);
    if (!runtime_export_blocking_diagnostics.empty()) {
      for (const auto &diagnostic : runtime_export_blocking_diagnostics) {
        result.stage_diagnostics.semantic.push_back(
            objc3c::parse::support::MakeDiag(diagnostic.line, diagnostic.column,
                                             diagnostic.code,
                                             diagnostic.message));
      }
    } else {
      std::string runtime_export_failure_reason =
          result.runtime_export_enforcement_summary.failure_reason;
      if (runtime_export_failure_reason ==
              "runtime metadata export shape drift detected before lowering" &&
          !result.runtime_export_legality_boundary.failure_reason.empty()) {
        runtime_export_failure_reason +=
            " (" + result.runtime_export_legality_boundary.failure_reason + ")";
      }
      result.stage_diagnostics.semantic.push_back(
          objc3c::parse::support::MakeDiag(
              result.runtime_export_enforcement_summary.first_failure_line,
              result.runtime_export_enforcement_summary.first_failure_column,
              "O3S260",
              "runtime metadata export blocked: " + runtime_export_failure_reason));
    }
  }
}

void PublishTerminalPhaseResults(
    Objc3FrontendPipelineResult &result,
    const Objc3FrontendOptions &options) {
  objc3c::pipeline::orchestration::AdoptObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
      result,
      objc3c::pipeline::orchestration::BuildObjc3FrontendSemanticDiagnosticTaxonomyPhaseResult(
          result, options));
  objc3c::pipeline::orchestration::PopulateObjc3FrontendReadinessLoweringPhaseResult(
      result, options);
}

}  // namespace objc3_frontend_pipeline_orchestration
