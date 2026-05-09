#include "libobjc3c_frontend/objc3c_frontend.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <new>
#include <limits>
#include <mutex>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include "ast/objc3_ast.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "io/json/json_writer.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "libobjc3c_frontend/objc3c_frontend_artifact_plan.h"
#include "libobjc3c_frontend/objc3c_frontend_basic_artifacts.h"
#include "libobjc3c_frontend/objc3c_frontend_compile_contract.h"
#include "libobjc3c_frontend/objc3c_frontend_context_state.h"
#include "libobjc3c_frontend/objc3c_frontend_diagnostics.h"
#include "libobjc3c_frontend/objc3c_frontend_file_output.h"
#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"
#include "support/objc3_file_reading.h"

static void objc3c_frontend_set_error(objc3c_frontend_context_t *context, const char *message) {
  objc3c::frontend::SetFrontendContextError(context, message);
}

static objc3c_frontend_status_t CompileObjc3SourceImpl(objc3c_frontend_context_t *context,
                                                       const std::filesystem::path &input_path,
                                                       const std::string &source_text,
                                                       const objc3c_frontend_compile_options_t *options,
                                                       objc3c_frontend_compile_result_t *result) {
  objc3c::frontend::ResetCompileResultForWrite(result);
  objc3c::frontend::ClearFrontendContextResultPaths(context);

  Objc3FrontendOptions frontend_options =
      objc3c::frontend::BuildFrontendPipelineOptions(*options);
  Objc3LoweringContract normalized_lowering;
  std::string lowering_error;
  if (!TryNormalizeObjc3LoweringContract(frontend_options.lowering, normalized_lowering, lowering_error)) {
    result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
    result->process_exit_code = 2;
    result->success = 0;
    objc3c_frontend_set_error(context, lowering_error.c_str());
    return objc3c::frontend::ReturnWithFrontendContextResultPayload(context, result);
  }
  frontend_options.lowering = normalized_lowering;

  Objc3FrontendCompileProduct product = CompileObjc3SourceWithPipeline(input_path, source_text, frontend_options);

  const bool sema_attempted =
      product.pipeline_result.stage_diagnostics.lexer.empty() && product.pipeline_result.stage_diagnostics.parser.empty();
  const bool lower_attempted = sema_attempted && product.pipeline_result.stage_diagnostics.semantic.empty();
  std::vector<std::string> emit_diagnostics = product.artifact_bundle.post_pipeline_diagnostics;

  const objc3c::frontend::Objc3FrontendArtifactOutputPlan artifact_plan =
      objc3c::frontend::BuildFrontendArtifactOutputPlan(*options, input_path);
  const std::filesystem::path &out_dir = artifact_plan.out_dir;
  const bool has_out_dir = artifact_plan.has_out_dir;
  const std::string &emit_prefix = artifact_plan.emit_prefix;

  if (has_out_dir) {
    std::string retired_claim_sidecar_error;
    if (!DiagnoseObjc3RetiredClaimSidecars(
            out_dir, emit_prefix, retired_claim_sidecar_error)) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 125;
      result->success = 0;
      objc3c_frontend_set_error(context, retired_claim_sidecar_error.c_str());
      return objc3c::frontend::ReturnWithFrontendContextResultPayload(context, result);
    }
  }

  if (!objc3c::frontend::PublishFrontendBasicArtifacts(
          context, result, product, artifact_plan, options->emit_manifest != 0)) {
    return objc3c::frontend::ReturnWithFrontendContextResultPayload(context,
                                                                    result);
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && has_out_dir) {
    if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(
            product.artifact_bundle
                .versioned_conformance_report_lowering_summary)) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(
          context,
          "versioned conformance-report lowering summary not ready");
    } else if (product.artifact_bundle
                   .versioned_conformance_report_artifact_json.empty()) {
      result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(
          context,
          "versioned conformance-report artifact payload missing");
    } else {
      const std::filesystem::path conformance_report_out =
          BuildVersionedConformanceReportArtifactPath(out_dir, emit_prefix);
      std::string io_error;
      if (!objc3c::frontend::WriteFrontendTextFile(
              conformance_report_out,
              product.artifact_bundle
                  .versioned_conformance_report_artifact_json,
              io_error)) {
        result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
        result->process_exit_code = 2;
        result->success = 0;
        objc3c_frontend_set_error(context, io_error.c_str());
      } else {
        // Publish the conformance sidecar through the frontend artifact path.
        std::string conformance_publication_artifact_json;
        std::string conformance_publication_error;
        if (!TryBuildObjc3ConformanceReportPublicationArtifact(
                {.contract_id =
                     "objc3c.driver.conformance.report.publication.v1",
                 .schema_id = "objc3c-driver-conformance-publication-v1",
                 .selected_profile = "core",
                 .selected_profile_supported =
                     IsObjc3ClaimedConformanceProfile("core"),
                 .supported_profile_ids =
                     BuildObjc3ClaimedConformanceProfileIds(),
                 .rejected_profile_ids =
                     BuildObjc3RejectedConformanceProfileIds(),
                 .effective_language_profile = "canonical",
                 .canonical_literal_rejection_diagnostics_enabled = false,
                 .publication_model =
                     "driver-publishes-lowered-conformance-sidecar-and-runtime-capability-sidecar-next-to-manifest",
                 .publication_surface_kind = "frontend-c-api",
                 .fail_closed_diagnostic_model =
                     "known-profiles-claimed-json-publication-remains-fail-closed-on-unsupported-formats-and-unknown-profiles",
                 .lowered_report_contract_id =
                     "objc3c.versioned.conformance.report.lowering.v1",
                 .runtime_capability_contract_id =
                     "objc3c.runtime.capability.reporting.v1",
                 .public_conformance_schema_id =
                     "objc3-conformance-report/v1",
                 .advanced_feature_ops_contract_id =
                     "objc3c.advanced.feature.ci.runbook.dashboard.contract.v1",
                 .advanced_feature_reporting_contract_id =
                     "objc3c.tooling.feature.aware.conformance.report.emission.v1",
                 .advanced_feature_release_evidence_contract_id =
                     "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1",
                 .ci_release_evidence_gate_script_path =
                     "scripts/check_release_evidence.py",
                 .runbook_reference_path =
                     "spec/conformance/release_evidence_gate_maintenance.md",
                 .dashboard_schema_path =
                     "schemas/objc3-conformance-dashboard-status-v1.schema.json",
                 .advanced_feature_targeted_profile_ids =
                     BuildObjc3ReleaseTargetedProfileIds(),
                 .report_artifact_relative_path =
                     (emit_prefix +
                      kObjc3VersionedConformanceReportLoweringArtifactSuffix)},
                conformance_publication_artifact_json,
                conformance_publication_error)) {
          result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
          result->process_exit_code = 2;
          result->success = 0;
          objc3c_frontend_set_error(context,
                                    conformance_publication_error.c_str());
        } else {
          const std::filesystem::path conformance_publication_out =
              BuildConformancePublicationArtifactPath(out_dir, emit_prefix);
          if (!objc3c::frontend::WriteFrontendTextFile(
                  conformance_publication_out,
                  conformance_publication_artifact_json,
                  io_error)) {
            result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
            result->process_exit_code = 2;
            result->success = 0;
            objc3c_frontend_set_error(context, io_error.c_str());
          } else {
            std::string advanced_feature_gate_artifact_json;
            std::string advanced_feature_gate_error;
            if (!TryBuildObjc3AdvancedFeatureGateArtifact(
                    {.surface_kind = "frontend-c-api",
                     .report_artifact_path =
                         (emit_prefix +
                          kObjc3VersionedConformanceReportLoweringArtifactSuffix),
                     .publication_artifact_path =
                         conformance_publication_out.filename().string(),
                     .validation_artifact_path =
                         BuildConformanceValidationArtifactPath(out_dir,
                                                               emit_prefix)
                             .filename()
                             .string(),
                     .release_evidence_operation_artifact_path =
                         BuildReleaseEvidenceOperationArtifactPath(out_dir,
                                                                   emit_prefix)
                             .filename()
                             .string(),
                     .dashboard_artifact_path =
                         BuildDashboardStatusArtifactPath(out_dir, emit_prefix)
                             .filename()
                             .string()},
                    product.artifact_bundle
                        .versioned_conformance_report_artifact_json,
                    conformance_publication_artifact_json,
                    advanced_feature_gate_artifact_json,
                    advanced_feature_gate_error)) {
              result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
              result->process_exit_code = 2;
              result->success = 0;
              objc3c_frontend_set_error(context,
                                        advanced_feature_gate_error.c_str());
            } else if (!objc3c::frontend::WriteFrontendTextFile(
                           BuildAdvancedFeatureGateArtifactPath(out_dir,
                                                                emit_prefix),
                           advanced_feature_gate_artifact_json, io_error)) {
              result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
              result->process_exit_code = 2;
              result->success = 0;
              objc3c_frontend_set_error(context, io_error.c_str());
            } else {
              std::string release_candidate_matrix_artifact_json;
              std::string release_candidate_matrix_error;
              if (!TryBuildObjc3ReleaseCandidateMatrixArtifact(
                      {.surface_kind = "frontend-c-api",
                       .report_artifact_path =
                           (emit_prefix +
                            kObjc3VersionedConformanceReportLoweringArtifactSuffix),
                       .publication_artifact_path =
                           conformance_publication_out.filename().string(),
                       .advanced_feature_gate_artifact_path =
                           BuildAdvancedFeatureGateArtifactPath(out_dir,
                                                               emit_prefix)
                               .filename()
                               .string(),
                       .validation_artifact_path =
                           BuildConformanceValidationArtifactPath(out_dir,
                                                                 emit_prefix)
                               .filename()
                               .string(),
                       .release_evidence_operation_artifact_path =
                           BuildReleaseEvidenceOperationArtifactPath(out_dir,
                                                                     emit_prefix)
                               .filename()
                               .string(),
                       .dashboard_artifact_path =
                           BuildDashboardStatusArtifactPath(out_dir, emit_prefix)
                               .filename()
                               .string()},
                      product.artifact_bundle
                          .versioned_conformance_report_artifact_json,
                      conformance_publication_artifact_json,
                      advanced_feature_gate_artifact_json,
                      release_candidate_matrix_artifact_json,
                      release_candidate_matrix_error)) {
                result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
                result->process_exit_code = 2;
                result->success = 0;
                objc3c_frontend_set_error(
                    context, release_candidate_matrix_error.c_str());
              } else if (!objc3c::frontend::WriteFrontendTextFile(
                             BuildReleaseCandidateMatrixArtifactPath(out_dir,
                                                                    emit_prefix),
                             release_candidate_matrix_artifact_json,
                             io_error)) {
                result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
                result->process_exit_code = 2;
                result->success = 0;
                objc3c_frontend_set_error(context, io_error.c_str());
              }
            }
          }
        }
      }
    }
  }

  const bool wants_ir_file = artifact_plan.wants_ir_file;
  std::filesystem::path ir_out;
  if (result->status == OBJC3C_FRONTEND_STATUS_OK && wants_ir_file) {
    if (!has_out_dir) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_ir/emit_object require out_dir in compile options.");
    } else {
      ir_out = objc3c::frontend::BuildFrontendIrOutputPath(artifact_plan);
      std::string io_error;
      if (!objc3c::frontend::WriteFrontendTextFile(
              ir_out, product.artifact_bundle.ir_text, io_error)) {
        result->status = OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR;
        result->process_exit_code = 2;
        result->success = 0;
        objc3c_frontend_set_error(context, io_error.c_str());
      } else {
        objc3c::frontend::SetFrontendContextIrPath(
            context, ir_out.generic_string());
      }
    }
  }

  if (result->status == OBJC3C_FRONTEND_STATUS_OK && options->emit_object != 0) {
    const bool wants_clang_backend =
        options->ir_object_backend == static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG);
    const bool wants_llvm_direct_backend =
        options->ir_object_backend == static_cast<uint8_t>(OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT);
    if (!wants_clang_backend && !wants_llvm_direct_backend) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_object requires a valid ir_object_backend (clang|llvm-direct).");
      emit_diagnostics.push_back(
          "error:1:1: emit_object requires valid ir_object_backend (clang|llvm-direct) [O3E001]");
    } else if (wants_clang_backend &&
               objc3c::frontend::IsMissingFrontendBorrowedPath(
                   options->clang_path)) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_object requires clang_path in compile options.");
      emit_diagnostics.push_back("error:1:1: emit_object requires clang_path in compile options [O3E001]");
    } else if (wants_llvm_direct_backend &&
               objc3c::frontend::IsMissingFrontendBorrowedPath(
                   options->llc_path)) {
      result->status = OBJC3C_FRONTEND_STATUS_USAGE_ERROR;
      result->process_exit_code = 2;
      result->success = 0;
      objc3c_frontend_set_error(context, "emit_object requires llc_path in compile options for llvm-direct backend.");
      emit_diagnostics.push_back(
          "error:1:1: emit_object requires llc_path in compile options for llvm-direct backend [O3E001]");
    } else {
      const std::filesystem::path object_out =
          objc3c::frontend::BuildFrontendObjectOutputPath(artifact_plan);
      const std::filesystem::path backend_out =
          objc3c::frontend::BuildFrontendObjectBackendOutputPath(
              artifact_plan);
      const std::string backend_text = wants_clang_backend ? "clang\n" : "llvm-direct\n";
      int compile_status = 0;
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
      const bool llvm_direct_backend_enabled = true;
#else
      const bool llvm_direct_backend_enabled = false;
#endif
      const std::filesystem::path clang_path =
          objc3c::frontend::OptionalBorrowedFrontendFilesystemPath(
              options->clang_path);
      const std::filesystem::path llc_path =
          objc3c::frontend::OptionalBorrowedFrontendFilesystemPath(
              options->llc_path);
      const Objc3ToolchainRuntimeGaOperationsScaffold toolchain_runtime_ga_operations_scaffold =
          BuildObjc3ToolchainRuntimeGaOperationsScaffold(
              wants_clang_backend,
              wants_llvm_direct_backend,
              clang_path,
              llc_path,
              llvm_direct_backend_enabled,
              ir_out,
              object_out);
      std::string toolchain_runtime_scaffold_reason;
      if (!IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(
              toolchain_runtime_ga_operations_scaffold,
              toolchain_runtime_scaffold_reason)) {
        result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
        result->process_exit_code = 125;
        result->success = 0;
        const std::string emit_error =
            "error:1:1: LLVM object emission failed: toolchain/runtime readiness contract fail-closed: " +
            toolchain_runtime_scaffold_reason + " [O3E002]";
        emit_diagnostics.push_back(emit_error);
        objc3c_frontend_set_error(context, emit_error.c_str());
      } else {
        bool backend_output_recorded = false;
        std::string backend_output_payload;
        std::string backend_output_error;
        std::string backend_error;
        if (wants_clang_backend) {
          compile_status = RunIRCompile(clang_path, ir_out, object_out);
        } else {
          compile_status = RunIRCompileLLVMDirect(llc_path, ir_out, object_out, backend_error);
        }
        if (compile_status == 0) {
          if (!objc3c::frontend::WriteFrontendTextFile(
                  backend_out, backend_text, backend_output_error)) {
            compile_status = 125;
          } else {
            backend_output_recorded = true;
            backend_output_payload = backend_text;
            Objc3RuntimeMetadataLinkerRetentionArtifacts
                linker_retention_artifacts;
            std::string linker_retention_error;
            if (!TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts(
                    ir_out,
                    object_out,
                    linker_retention_artifacts,
                    linker_retention_error)) {
              compile_status = 125;
              backend_error = linker_retention_error;
            } else {
              const std::filesystem::path linker_response_out =
                  BuildRuntimeMetadataLinkerResponseArtifactPath(out_dir,
                                                                 emit_prefix);
              if (!objc3c::frontend::WriteFrontendTextFile(
                      linker_response_out,
                      linker_retention_artifacts
                          .linker_response_file_payload,
                      backend_output_error)) {
                compile_status = 125;
              } else {
                const std::filesystem::path discovery_out =
                    BuildRuntimeMetadataDiscoveryArtifactPath(out_dir,
                                                              emit_prefix);
                if (!objc3c::frontend::WriteFrontendTextFile(
                        discovery_out,
                        linker_retention_artifacts.discovery_json,
                        backend_output_error)) {
                  compile_status = 125;
                } else if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
                               product.artifact_bundle
                                   .runtime_translation_unit_registration_manifest_summary)) {
                  compile_status = 125;
                  backend_error =
                      "translation-unit registration manifest template not ready";
                } else if (!IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
                               product.artifact_bundle
                                   .runtime_registration_descriptor_image_root_source_surface_summary)) {
                  compile_status = 125;
                  backend_error =
                      "registration descriptor/image-root source surface not ready";
                } else if (!IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
                               product.artifact_bundle
                                   .runtime_registration_descriptor_frontend_closure_summary)) {
                  compile_status = 125;
                  backend_error =
                      "registration descriptor frontend closure not ready";
                } else {
                  Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
                      manifest_inputs;
                  const auto &registration_manifest_summary =
                      product.artifact_bundle
                          .runtime_translation_unit_registration_manifest_summary;
                  const auto &registration_descriptor_source_surface_summary =
                      product.artifact_bundle
                          .runtime_registration_descriptor_image_root_source_surface_summary;
                  const auto &registration_descriptor_frontend_closure_summary =
                      product.artifact_bundle
                          .runtime_registration_descriptor_frontend_closure_summary;
                  const auto &runtime_bootstrap_api_summary =
                      product.artifact_bundle.runtime_bootstrap_api_summary;
                  const auto &runtime_bootstrap_semantics_summary =
                      product.artifact_bundle
                          .runtime_bootstrap_semantics_summary;
                  const auto &runtime_bootstrap_lowering_summary =
                      product.artifact_bundle.runtime_bootstrap_lowering_summary;
                  manifest_inputs.contract_id =
                      registration_manifest_summary.contract_id;
                  manifest_inputs.translation_unit_registration_contract_id =
                      registration_manifest_summary
                          .translation_unit_registration_contract_id;
                  manifest_inputs.runtime_support_library_link_wiring_contract_id =
                      registration_manifest_summary
                          .runtime_support_library_link_wiring_contract_id;
                  manifest_inputs.manifest_payload_model =
                      registration_manifest_summary.manifest_payload_model;
                  manifest_inputs.manifest_artifact_relative_path =
                      registration_manifest_summary
                          .manifest_artifact_relative_path;
                  manifest_inputs.runtime_owned_payload_artifacts.assign(
                      registration_manifest_summary
                          .runtime_owned_payload_artifacts.begin(),
                      registration_manifest_summary
                          .runtime_owned_payload_artifacts.end());
                  manifest_inputs.runtime_support_library_archive_relative_path =
                      registration_manifest_summary
                          .runtime_support_library_archive_relative_path;
                  manifest_inputs.constructor_root_symbol =
                      registration_manifest_summary.constructor_root_symbol;
                  manifest_inputs.constructor_root_ownership_model =
                      registration_manifest_summary
                          .constructor_root_ownership_model;
                  manifest_inputs.manifest_authority_model =
                      registration_manifest_summary.manifest_authority_model;
                  manifest_inputs.constructor_init_stub_symbol_prefix =
                      registration_manifest_summary
                          .constructor_init_stub_symbol_prefix;
                  manifest_inputs.constructor_init_stub_ownership_model =
                      registration_manifest_summary
                          .constructor_init_stub_ownership_model;
                  manifest_inputs.constructor_priority_policy =
                      registration_manifest_summary
                          .constructor_priority_policy;
                  manifest_inputs.registration_entrypoint_symbol =
                      registration_manifest_summary
                          .registration_entrypoint_symbol;
                  manifest_inputs.translation_unit_identity_model =
                      registration_manifest_summary
                          .translation_unit_identity_model;
                  manifest_inputs.launch_integration_contract_id =
                      registration_manifest_summary
                          .launch_integration_contract_id;
                  manifest_inputs.runtime_library_resolution_model =
                      registration_manifest_summary
                          .runtime_library_resolution_model;
                  manifest_inputs.driver_linker_flag_consumption_model =
                      registration_manifest_summary
                          .driver_linker_flag_consumption_model;
                  manifest_inputs.compile_wrapper_command_surface =
                      registration_manifest_summary
                          .compile_wrapper_command_surface;
                  manifest_inputs.compile_proof_command_surface =
                      registration_manifest_summary
                          .compile_proof_command_surface;
                  manifest_inputs.execution_smoke_command_surface =
                      registration_manifest_summary
                          .execution_smoke_command_surface;
                  manifest_inputs.registration_descriptor_source_contract_id =
                      registration_descriptor_source_surface_summary.contract_id;
                  manifest_inputs.registration_descriptor_source_surface_path =
                      registration_descriptor_source_surface_summary
                          .source_surface_path;
                  manifest_inputs.registration_descriptor_pragma_name =
                      registration_descriptor_source_surface_summary
                          .registration_descriptor_pragma_name;
                  manifest_inputs.image_root_pragma_name =
                      registration_descriptor_source_surface_summary
                          .image_root_pragma_name;
                  manifest_inputs.module_identity_source =
                      registration_descriptor_source_surface_summary
                          .module_identity_source;
                  manifest_inputs.registration_descriptor_identifier =
                      registration_descriptor_source_surface_summary
                          .registration_descriptor_identifier;
                  manifest_inputs.registration_descriptor_identity_source =
                      registration_descriptor_source_surface_summary
                          .registration_descriptor_identity_source;
                  manifest_inputs.image_root_identifier =
                      registration_descriptor_source_surface_summary
                          .image_root_identifier;
                  manifest_inputs.image_root_identity_source =
                      registration_descriptor_source_surface_summary
                          .image_root_identity_source;
                  manifest_inputs.bootstrap_visible_metadata_ownership_model =
                      registration_descriptor_source_surface_summary
                          .bootstrap_visible_metadata_ownership_model;
                  manifest_inputs.class_descriptor_count =
                      registration_manifest_summary.class_descriptor_count;
                  manifest_inputs.protocol_descriptor_count =
                      registration_manifest_summary.protocol_descriptor_count;
                  manifest_inputs.category_descriptor_count =
                      registration_manifest_summary.category_descriptor_count;
                  manifest_inputs.property_descriptor_count =
                      registration_manifest_summary.property_descriptor_count;
                  manifest_inputs.ivar_descriptor_count =
                      registration_manifest_summary.ivar_descriptor_count;
                  manifest_inputs.total_descriptor_count =
                      registration_manifest_summary.total_descriptor_count;
                  manifest_inputs.bootstrap_semantics_contract_id =
                      runtime_bootstrap_semantics_summary.contract_id;
                  manifest_inputs.duplicate_registration_policy =
                      runtime_bootstrap_semantics_summary
                          .duplicate_registration_policy;
                  manifest_inputs.realization_order_policy =
                      runtime_bootstrap_semantics_summary
                          .realization_order_policy;
                  manifest_inputs.failure_mode =
                      runtime_bootstrap_semantics_summary.failure_mode;
                  manifest_inputs.registration_result_model =
                      runtime_bootstrap_semantics_summary
                          .registration_result_model;
                  manifest_inputs.registration_order_ordinal_model =
                      runtime_bootstrap_semantics_summary
                          .registration_order_ordinal_model;
                  manifest_inputs.runtime_state_snapshot_symbol =
                      runtime_bootstrap_semantics_summary
                          .runtime_state_snapshot_symbol;
                  manifest_inputs.bootstrap_runtime_api_contract_id =
                      runtime_bootstrap_api_summary.contract_id;
                  manifest_inputs.bootstrap_runtime_api_public_header_path =
                      runtime_bootstrap_api_summary.public_header_path;
                  manifest_inputs.bootstrap_runtime_api_archive_relative_path =
                      runtime_bootstrap_api_summary.archive_relative_path;
                  manifest_inputs
                      .bootstrap_runtime_api_registration_status_enum_type =
                      runtime_bootstrap_api_summary
                          .registration_status_enum_type;
                  manifest_inputs.bootstrap_runtime_api_image_descriptor_type =
                      runtime_bootstrap_api_summary.image_descriptor_type;
                  manifest_inputs.bootstrap_runtime_api_selector_handle_type =
                      runtime_bootstrap_api_summary.selector_handle_type;
                  manifest_inputs
                      .bootstrap_runtime_api_registration_snapshot_type =
                      runtime_bootstrap_api_summary
                          .registration_snapshot_type;
                  manifest_inputs
                      .bootstrap_runtime_api_registration_entrypoint_symbol =
                      runtime_bootstrap_api_summary
                          .registration_entrypoint_symbol;
                  manifest_inputs.bootstrap_runtime_api_selector_lookup_symbol =
                      runtime_bootstrap_api_summary.selector_lookup_symbol;
                  manifest_inputs
                      .bootstrap_runtime_api_dispatch_entrypoint_symbol =
                      runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
                  manifest_inputs.bootstrap_runtime_api_state_snapshot_symbol =
                      runtime_bootstrap_api_summary.state_snapshot_symbol;
                  manifest_inputs.bootstrap_runtime_api_reset_for_testing_symbol =
                      runtime_bootstrap_api_summary.reset_for_testing_symbol;
                  manifest_inputs.bootstrap_registrar_contract_id =
                      kObjc3RuntimeBootstrapRegistrarContractId;
                  manifest_inputs.bootstrap_registrar_internal_header_path =
                      kObjc3RuntimeBootstrapInternalHeaderPath;
                  manifest_inputs
                      .bootstrap_registrar_stage_registration_table_symbol =
                      kObjc3RuntimeBootstrapStageRegistrationTableSymbol;
                  manifest_inputs.bootstrap_registrar_image_walk_snapshot_symbol =
                      kObjc3RuntimeBootstrapImageWalkSnapshotSymbol;
                  manifest_inputs.bootstrap_registrar_image_walk_model =
                      kObjc3RuntimeBootstrapImageWalkModel;
                  manifest_inputs
                      .bootstrap_registrar_discovery_root_validation_model =
                      kObjc3RuntimeBootstrapDiscoveryRootValidationModel;
                  manifest_inputs
                      .bootstrap_registrar_selector_pool_interning_model =
                      kObjc3RuntimeBootstrapSelectorPoolInterningModel;
                  manifest_inputs.bootstrap_registrar_realization_staging_model =
                      kObjc3RuntimeBootstrapRealizationStagingModel;
                  manifest_inputs.bootstrap_reset_contract_id =
                      kObjc3RuntimeBootstrapResetContractId;
                  manifest_inputs.bootstrap_reset_internal_header_path =
                      kObjc3RuntimeBootstrapInternalHeaderPath;
                  manifest_inputs
                      .bootstrap_reset_replay_registered_images_symbol =
                      kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
                  manifest_inputs
                      .bootstrap_reset_reset_replay_state_snapshot_symbol =
                      kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
                  manifest_inputs.bootstrap_reset_lifecycle_model =
                      kObjc3RuntimeBootstrapResetLifecycleModel;
                  manifest_inputs.bootstrap_reset_replay_order_model =
                      kObjc3RuntimeBootstrapReplayOrderModel;
                  manifest_inputs
                      .bootstrap_reset_image_local_init_state_reset_model =
                      kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
                  manifest_inputs
                      .bootstrap_reset_bootstrap_catalog_retention_model =
                      kObjc3RuntimeBootstrapCatalogRetentionModel;
                  manifest_inputs.bootstrap_lowering_contract_id =
                      runtime_bootstrap_lowering_summary.contract_id;
                  manifest_inputs.bootstrap_lowering_boundary_model =
                      runtime_bootstrap_lowering_summary.lowering_boundary_model;
                  manifest_inputs.bootstrap_global_ctor_list_model =
                      runtime_bootstrap_lowering_summary.global_ctor_list_model;
                  manifest_inputs.bootstrap_registration_table_layout_model =
                      runtime_bootstrap_lowering_summary
                          .registration_table_layout_model;
                  manifest_inputs.bootstrap_image_local_initialization_model =
                      runtime_bootstrap_lowering_summary
                          .image_local_initialization_model;
                  manifest_inputs.bootstrap_constructor_root_emission_state =
                      runtime_bootstrap_lowering_summary
                          .constructor_root_emission_state;
                  manifest_inputs.bootstrap_init_stub_emission_state =
                      runtime_bootstrap_lowering_summary
                          .init_stub_emission_state;
                  manifest_inputs.bootstrap_registration_table_emission_state =
                      runtime_bootstrap_lowering_summary
                          .registration_table_emission_state;
                  manifest_inputs.bootstrap_registration_table_symbol_prefix =
                      runtime_bootstrap_lowering_summary
                          .registration_table_symbol_prefix;
                  manifest_inputs.bootstrap_image_local_init_state_symbol_prefix =
                      runtime_bootstrap_lowering_summary
                          .image_local_init_state_symbol_prefix;
                  manifest_inputs.bootstrap_registration_table_abi_version =
                      runtime_bootstrap_lowering_summary
                          .registration_table_abi_version;
                  manifest_inputs.bootstrap_registration_table_pointer_field_count =
                      runtime_bootstrap_lowering_summary
                          .registration_table_pointer_field_count;
                  manifest_inputs.success_status_code =
                      runtime_bootstrap_semantics_summary.success_status_code;
                  manifest_inputs.invalid_descriptor_status_code =
                      runtime_bootstrap_semantics_summary
                          .invalid_descriptor_status_code;
                  manifest_inputs.duplicate_registration_status_code =
                      runtime_bootstrap_semantics_summary
                          .duplicate_registration_status_code;
                  manifest_inputs.out_of_order_status_code =
                      runtime_bootstrap_semantics_summary
                          .out_of_order_status_code;
                  manifest_inputs.translation_unit_registration_order_ordinal =
                      registration_manifest_summary
                          .translation_unit_registration_order_ordinal;
                  manifest_inputs.object_artifact_relative_path =
                      object_out.filename().generic_string();
                  manifest_inputs.backend_artifact_relative_path =
                      backend_out.filename().generic_string();

                  std::string registration_manifest_json;
                  std::string registration_manifest_error;
                  if (!TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(
                          manifest_inputs,
                          linker_retention_artifacts,
                          product.artifact_bundle.runtime_metadata_binary.size(),
                          registration_manifest_json,
                          registration_manifest_error)) {
                    compile_status = 125;
                    backend_error = registration_manifest_error;
                  } else {
                    // Publish registration manifests through the frontend C API path.
                    const std::filesystem::path registration_manifest_out =
                        BuildRuntimeRegistrationManifestArtifactPath(out_dir,
                                                                     emit_prefix);
                    if (!objc3c::frontend::WriteFrontendTextFile(
                            registration_manifest_out,
                            registration_manifest_json,
                            backend_output_error)) {
                      compile_status = 125;
                    } else {
                      Objc3RuntimeRegistrationDescriptorArtifactInputs
                          descriptor_inputs;
                      descriptor_inputs.contract_id =
                          registration_descriptor_frontend_closure_summary
                              .contract_id;
                      descriptor_inputs.registration_manifest_contract_id =
                          registration_descriptor_frontend_closure_summary
                              .registration_manifest_contract_id;
                      descriptor_inputs.source_surface_contract_id =
                          registration_descriptor_frontend_closure_summary
                              .source_surface_contract_id;
                      descriptor_inputs.bootstrap_lowering_contract_id =
                          runtime_bootstrap_lowering_summary.contract_id;
                      descriptor_inputs.payload_model =
                          registration_descriptor_frontend_closure_summary
                              .payload_model;
                      descriptor_inputs.artifact_relative_path =
                          registration_descriptor_frontend_closure_summary
                              .artifact_relative_path;
                      descriptor_inputs.authority_model =
                          registration_descriptor_frontend_closure_summary
                              .authority_model;
                      descriptor_inputs.translation_unit_identity_model =
                          registration_descriptor_frontend_closure_summary
                              .translation_unit_identity_model;
                      descriptor_inputs.payload_ownership_model =
                          registration_descriptor_frontend_closure_summary
                              .payload_ownership_model;
                      descriptor_inputs.runtime_support_library_archive_relative_path =
                          registration_manifest_summary
                              .runtime_support_library_archive_relative_path;
                      descriptor_inputs.registration_entrypoint_symbol =
                          registration_manifest_summary.registration_entrypoint_symbol;
                      descriptor_inputs.registration_descriptor_pragma_name =
                          registration_descriptor_source_surface_summary
                              .registration_descriptor_pragma_name;
                      descriptor_inputs.image_root_pragma_name =
                          registration_descriptor_source_surface_summary
                              .image_root_pragma_name;
                      descriptor_inputs.module_identity_source =
                          registration_descriptor_source_surface_summary
                              .module_identity_source;
                      descriptor_inputs.registration_descriptor_identifier =
                          registration_descriptor_frontend_closure_summary
                              .registration_descriptor_identifier;
                      descriptor_inputs.registration_descriptor_identity_source =
                          registration_descriptor_frontend_closure_summary
                              .registration_descriptor_identity_source;
                      descriptor_inputs.image_root_identifier =
                          registration_descriptor_frontend_closure_summary
                              .image_root_identifier;
                      descriptor_inputs.image_root_identity_source =
                          registration_descriptor_frontend_closure_summary
                              .image_root_identity_source;
                      descriptor_inputs.bootstrap_visible_metadata_ownership_model =
                          registration_descriptor_frontend_closure_summary
                              .bootstrap_visible_metadata_ownership_model;
                      descriptor_inputs.constructor_root_symbol =
                          registration_manifest_summary.constructor_root_symbol;
                      descriptor_inputs.constructor_init_stub_symbol_prefix =
                          registration_manifest_summary
                              .constructor_init_stub_symbol_prefix;
                      descriptor_inputs.bootstrap_registration_table_symbol_prefix =
                          runtime_bootstrap_lowering_summary
                              .registration_table_symbol_prefix;
                      descriptor_inputs.bootstrap_image_local_init_state_symbol_prefix =
                          runtime_bootstrap_lowering_summary
                              .image_local_init_state_symbol_prefix;
                      descriptor_inputs.bootstrap_registration_table_layout_model =
                          runtime_bootstrap_lowering_summary
                              .registration_table_layout_model;
                      descriptor_inputs.bootstrap_image_local_initialization_model =
                          runtime_bootstrap_lowering_summary
                              .image_local_initialization_model;
                      descriptor_inputs.bootstrap_constructor_root_emission_state =
                          runtime_bootstrap_lowering_summary
                              .constructor_root_emission_state;
                      descriptor_inputs.bootstrap_init_stub_emission_state =
                          runtime_bootstrap_lowering_summary
                              .init_stub_emission_state;
                      descriptor_inputs.bootstrap_registration_table_emission_state =
                          runtime_bootstrap_lowering_summary
                              .registration_table_emission_state;
                      descriptor_inputs.bootstrap_registration_table_abi_version =
                          runtime_bootstrap_lowering_summary
                              .registration_table_abi_version;
                      descriptor_inputs.bootstrap_registration_table_pointer_field_count =
                          runtime_bootstrap_lowering_summary
                              .registration_table_pointer_field_count;
                      descriptor_inputs.class_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .class_descriptor_count;
                      descriptor_inputs.protocol_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .protocol_descriptor_count;
                      descriptor_inputs.category_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .category_descriptor_count;
                      descriptor_inputs.property_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .property_descriptor_count;
                      descriptor_inputs.ivar_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .ivar_descriptor_count;
                      descriptor_inputs.total_descriptor_count =
                          registration_descriptor_frontend_closure_summary
                              .total_descriptor_count;
                      descriptor_inputs.translation_unit_registration_order_ordinal =
                          registration_descriptor_frontend_closure_summary
                              .translation_unit_registration_order_ordinal;
                      descriptor_inputs.object_artifact_relative_path =
                          object_out.filename().generic_string();
                      descriptor_inputs.backend_artifact_relative_path =
                          backend_out.filename().generic_string();

                      std::string registration_descriptor_json;
                      std::string registration_descriptor_error;
                      if (!TryBuildObjc3RuntimeRegistrationDescriptorArtifact(
                              descriptor_inputs,
                              linker_retention_artifacts,
                              registration_descriptor_json,
                              registration_descriptor_error)) {
                        compile_status = 125;
                        backend_error = registration_descriptor_error;
                      } else {
                        const std::filesystem::path
                            registration_descriptor_out =
                                BuildRuntimeRegistrationDescriptorArtifactPath(
                                    out_dir, emit_prefix);
                        if (!objc3c::frontend::WriteFrontendTextFile(
                                registration_descriptor_out,
                                registration_descriptor_json,
                                backend_output_error)) {
                          compile_status = 125;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
        const Objc3ToolchainRuntimeGaOperationsCoreFeatureSurface toolchain_runtime_core_feature_surface =
            BuildObjc3ToolchainRuntimeGaOperationsCoreFeatureSurface(
                toolchain_runtime_ga_operations_scaffold,
                compile_status,
                backend_output_recorded,
                backend_out,
                backend_output_payload);
        std::string toolchain_runtime_core_feature_reason;
        const bool toolchain_runtime_core_feature_ready =
            IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(
                toolchain_runtime_core_feature_surface,
                toolchain_runtime_core_feature_reason);
        if (compile_status != 0) {
          result->status = OBJC3C_FRONTEND_STATUS_EMIT_ERROR;
          result->process_exit_code = compile_status;
          result->success = 0;
          std::string emit_error;
          if (!backend_output_error.empty()) {
            emit_error = "error:1:1: LLVM object emission failed: " + backend_output_error + " [O3E002]";
          } else if (!backend_error.empty()) {
            emit_error = "error:1:1: LLVM object emission failed: " + backend_error + " [O3E002]";
          } else if (wants_clang_backend) {
            emit_error =
                "error:1:1: LLVM object emission failed: clang exited with status " + std::to_string(compile_status) +
                " [O3E002]";
          } else {
            emit_error =
                "error:1:1: LLVM object emission failed: llc exited with status " + std::to_string(compile_status) +
                " [O3E002]";
          }
          emit_diagnostics.push_back(emit_error);
          objc3c_frontend_set_error(context, emit_error.c_str());
