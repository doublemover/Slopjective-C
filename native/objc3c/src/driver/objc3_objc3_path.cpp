#include "driver/objc3_objc3_path.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "ast/objc3_ast.h"
#include "driver/objc3_frontend_options.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_file_io.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"
#include "lower/objc3_lowering_contract.h"
#include "pipeline/objc3_runtime_import_surface.h"

namespace fs = std::filesystem;

namespace {

bool TryDeriveConformancePublicationPath(const fs::path &report_path,
                                        fs::path &publication_path) {
  const std::string report_name = report_path.filename().string();
  const std::string suffix =
      kObjc3VersionedConformanceReportLoweringArtifactSuffix;
  if (report_name.size() <= suffix.size() ||
      report_name.rfind(suffix) != report_name.size() - suffix.size()) {
    return false;
  }
  const std::string emit_prefix =
      report_name.substr(0u, report_name.size() - suffix.size());
  publication_path =
      BuildConformancePublicationArtifactPath(report_path.parent_path(),
                                             emit_prefix);
  return true;
}

}  // namespace

// error-model conformance gate anchor: lane-E freezes the current
// Part 6 slice by consuming the canonical integrated proof surface already
// published by the driver and artifact sidecars.
// async executable conformance gate anchor: lane-E must keep
// consuming this same emitted manifest/IR/object triplet for the runnable Part
// 7 slice instead of introducing an async-only publication channel.
// runnable async closeout matrix anchor: milestone closeout rows must
// keep consuming this same emitted manifest/IR/object triplet for the current
// Part 7 slice instead of a synthetic matrix-only publication path.
// task/executor conformance gate anchor: lane-E freezes the current
// runnable task/runtime slice by consuming the same published driver artifact
// surface while the broader front-door publication path remains fail-closed.
// runnable task/executor closeout matrix anchor: milestone closeout
// rows keep consuming this same driver artifact surface instead of inventing a
// matrix-only publication path for the current Part 7 task/runtime slice.
// strict concurrency conformance gate anchor: lane-E freezes the
// current runnable actor/isolation slice by consuming this same published
// driver artifact surface while the broader front-door actor publication path remains fail-closed.
// runnable actor/isolation closeout matrix anchor: milestone
// closeout rows keep consuming this same driver artifact surface instead of
// inventing a matrix-only publication path for the current Part 7 actor slice.
// strict system conformance gate anchor: lane-E freezes the current
// runnable Part 8 cleanup/resource/retainable slice by consuming this same
// published driver artifact surface while the broader front-door publication
// path remains fail-closed for deferred borrowed-lifetime runtime claims.
// runnable system-extension closeout matrix anchor: milestone
// closeout rows keep consuming this same driver artifact surface instead of
// inventing a matrix-only publication path for the current Part 8 slice.
// performance/dynamism conformance gate anchor: lane-E now freezes
// the current runnable Part 9 dispatch-control slice by consuming this same
// published driver artifact surface while the widened D002 runtime proof
// remains the canonical executable evidence boundary.
// runnable dispatch-control matrix closeout anchor: matrix rows keep
// consuming this same driver artifact surface while Part 9 closeout stays
// pinned to the existing D002 runtime proof instead of inventing a new lane-E
// publication channel.
// metaprogramming conformance gate anchor: lane-E freezes the
// currently supported Part 10 slice by consuming this same published driver
// artifact surface while the D002 live macro host-process/cache proof remains
// the canonical executable evidence boundary.
// runnable metaprogramming closeout matrix anchor: Part 10 closeout
// rows keep consuming this same driver artifact surface instead of inventing a
// parallel lane-E publication channel for derives/macros/property behaviors.

int RunObjc3ConformanceValidationPath(const Objc3CliOptions &cli_options) {
  if (cli_options.emit_objc3_conformance_format != "json") {
    std::cerr << "unsupported --emit-objc3-conformance-format selection: "
              << cli_options.emit_objc3_conformance_format
              << " (current validation format is json)\n";
    return 125;
  }

  if (!fs::exists(cli_options.validate_conformance_report_path)) {
    std::cerr << "conformance report not found: "
              << cli_options.validate_conformance_report_path.string() << "\n";
    return 2;
  }

  fs::path publication_path;
  if (!TryDeriveConformancePublicationPath(
          cli_options.validate_conformance_report_path, publication_path)) {
    std::cerr << "validated artifact must end with "
              << kObjc3VersionedConformanceReportLoweringArtifactSuffix << "\n";
    return 125;
  }
  if (!fs::exists(publication_path)) {
    std::cerr << "conformance publication artifact not found next to report: "
              << publication_path.string() << "\n";
    return 125;
  }

  const std::string report_json =
      ReadText(cli_options.validate_conformance_report_path);
  const std::string publication_json = ReadText(publication_path);
  std::string validation_artifact_json;
  std::string validation_error;
  if (!TryBuildObjc3ConformanceClaimValidationArtifact(
          {.report_artifact_path =
               cli_options.validate_conformance_report_path.filename().string(),
           .publication_artifact_path =
               publication_path.filename().string()},
          report_json,
          publication_json,
          validation_artifact_json,
          validation_error)) {
    std::cerr << validation_error << "\n";
    return 125;
  }

  // versioning/conformance truth-gate anchor: this validation mode
  // is the integrated operator-side consumer of the D001 publication sidecar
  // and the C001/C002 lowered/runtime capability reports.
  WriteConformanceValidationArtifact(cli_options.out_dir,
                                     cli_options.emit_prefix,
                                     validation_artifact_json);

  std::string release_evidence_operation_json;
  std::string release_evidence_operation_error;
  if (!TryBuildObjc3ReleaseEvidenceOperationArtifact(
          {.report_artifact_path =
               cli_options.validate_conformance_report_path.filename().string(),
           .publication_artifact_path = publication_path.filename().string(),
           .validation_artifact_path =
               BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                     cli_options.emit_prefix)
                   .filename()
                   .string(),
           .dashboard_artifact_path =
               BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                               cli_options.emit_prefix)
                   .filename()
                   .string()},
          report_json,
          publication_json,
          validation_artifact_json,
          release_evidence_operation_json,
          release_evidence_operation_error)) {
    std::cerr << release_evidence_operation_error << "\n";
    return 125;
  }
  WriteReleaseEvidenceOperationArtifact(cli_options.out_dir,
                                        cli_options.emit_prefix,
                                        release_evidence_operation_json);

  std::string dashboard_status_json;
  std::string dashboard_status_error;
  if (!TryBuildObjc3DashboardStatusArtifact(
          {.report_artifact_path =
               cli_options.validate_conformance_report_path.filename().string(),
           .publication_artifact_path = publication_path.filename().string(),
           .validation_artifact_path =
               BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                     cli_options.emit_prefix)
                   .filename()
                   .string(),
           .release_evidence_operation_artifact_path =
               BuildReleaseEvidenceOperationArtifactPath(
                   cli_options.out_dir, cli_options.emit_prefix)
                   .filename()
                   .string()},
          report_json,
          publication_json,
          validation_artifact_json,
          release_evidence_operation_json,
          dashboard_status_json,
          dashboard_status_error)) {
    std::cerr << dashboard_status_error << "\n";
    return 125;
  }
  WriteDashboardStatusArtifact(cli_options.out_dir,
                               cli_options.emit_prefix,
                               dashboard_status_json);
  return 0;
}

int RunObjc3LanguagePath(const Objc3CliOptions &cli_options) {
  try {
    if (cli_options.emit_objc3_conformance_format != "json") {
      std::cerr << "unsupported --emit-objc3-conformance-format selection: "
                << cli_options.emit_objc3_conformance_format
                << " (current runnable emission format is json)\n";
      return 125;
    }
    if (cli_options.conformance_profile != Objc3ConformanceProfile::kCore) {
      std::cerr << "unsupported --objc3-conformance-profile selection: "
                << ConformanceProfileName(cli_options.conformance_profile)
                << " (current runnable profile is core)\n";
      return 125;
    }
    const std::string source = ReadText(cli_options.input);
    const Objc3FrontendOptions frontend_options = BuildObjc3FrontendOptions(cli_options);
    Objc3FrontendArtifactBundle artifacts = CompileObjc3SourceForCli(cli_options.input, source, frontend_options);
    // type-surface executable gate anchor: lane-E consumes the
    // emitted manifest/IR/object triplet as the canonical integrated proof for
    // the currently runnable optional/key-path slice.
    // runnable-type-surface closeout anchor: the same emitted
    // artifact triplet remains the source of truth for the milestone closeout
    // matrix runtime rows and preserved generic replay evidence.
    WriteDiagnosticsArtifacts(cli_options.out_dir,
                              cli_options.emit_prefix,
                              artifacts.stage_diagnostics,
                              artifacts.post_pipeline_diagnostics);
    if (!artifacts.manifest_json.empty()) {
      // control-flow execution gate anchor: lane-E consumes the
      // emitted manifest/IR/object triplet from this native CLI path as the
      // truthful integrated proof surface for the currently runnable Part 5
      // slice.
      // runnable control-flow matrix anchor: the milestone closeout
      // matrix continues to consume the same emitted native artifact triplet
      // rather than widening into a synthetic reporting path.
      // freeze: manifest emission remains the authoritative published
      // metadata-section ABI surface until later object-section emission lands.
      // scaffold: manifest emission mirrors the live runtime-metadata
      // section scaffold inventory so tooling can diff JSON against emitted
      // LLVM IR/object evidence without treating the manifest as the only source.
      // object inspection harness: manifest emission also publishes
      // the llvm-readobj/llvm-objdump matrix so object inspection remains tied
      // to canonical emitted artifacts instead of ad hoc operator commands.
      // freeze: manifest emission also publishes the reserved native
      // runtime support-library surface (target/header/entrypoints/link mode)
      // so D002/D003 must preserve one canonical runtime-library contract while
      // the deterministic test shim remains non-canonical evidence only.
      // core feature: manifest emission also publishes the live
      // native runtime-library skeleton/build contract so the real in-tree
      // archive/header/source/probe surface stays synchronized with emitted IR
      // evidence while driver link wiring remains deferred to the next runtime step.
      // link wiring: manifest emission remains the canonical
      // runtime-archive handoff for external executable link steps, even while
      // this driver tranche still stops at deterministic object emission.
      WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);
    }
    if (!artifacts.runtime_metadata_binary.empty()) {
      WriteRuntimeMetadataBinaryArtifact(cli_options.out_dir,
                                        cli_options.emit_prefix,
                                        artifacts.runtime_metadata_binary);
    }
    if (!artifacts.error_handling_result_bridge_artifact_replay_json.empty()) {
      WriteErrorHandlingResultBridgeArtifactReplay(
          cli_options.out_dir, cli_options.emit_prefix,
          artifacts.error_handling_result_bridge_artifact_replay_json);
    }
    if (!artifacts.diagnostics.empty()) {
      return 1;
    }

    const bool has_runtime_import_artifact =
        !artifacts.runtime_aware_import_module_artifact_json.empty();
    if (has_runtime_import_artifact &&
        !IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
            artifacts.runtime_aware_import_module_frontend_closure_summary)) {
      std::cerr << "runtime-aware import/module frontend closure not ready\n";
      return 125;
    }
    if (has_runtime_import_artifact) {
      WriteRuntimeAwareImportModuleArtifact(
          cli_options.out_dir,
          cli_options.emit_prefix,
          artifacts.runtime_aware_import_module_artifact_json);
    }
    // interop conformance gate anchor: the driver-side publication
    // path includes the live D002 bridge sidecars consumed by the lane-E gate.
    // Part 11 lane-E anchors: the driver-side publication
    // path includes the live D002 bridge sidecars consumed by the gate and the closeout matrix.
    if (!artifacts.interop_bridge_header_artifact_text.empty()) {
      WriteInteropBridgeHeaderArtifact(cli_options.out_dir, cli_options.emit_prefix,
                                      artifacts.interop_bridge_header_artifact_text);
    }
    if (!artifacts.interop_bridge_module_artifact_text.empty()) {
      WriteInteropBridgeModuleArtifact(cli_options.out_dir, cli_options.emit_prefix,
                                      artifacts.interop_bridge_module_artifact_text);
    }
    if (!artifacts.interop_bridge_artifact_json.empty()) {
      WriteInteropBridgeArtifact(cli_options.out_dir, cli_options.emit_prefix,
                                artifacts.interop_bridge_artifact_json);
    }
    if (artifacts.metaprogramming_macro_host_process_cache_runtime_integration_ready) {
      std::string metaprogramming_host_cache_artifact_json;
      std::string metaprogramming_host_cache_error;
      if (!TryBuildObjc3MetaprogrammingMacroHostProcessCacheArtifact(
              {.contract_id =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId,
               .source_contract_id =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId,
               .surface_path =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath,
               .artifact_relative_path =
                   kObjc3MetaprogrammingMacroHostProcessCacheArtifactRelativePath,
               .host_executable_relative_path =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath,
               .cache_root_relative_path =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath,
               .host_model =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel,
               .toolchain_model =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel,
               .cache_model =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel,
               .fail_closed_model =
                   kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel,
               .replay_key =
                   artifacts
                       .metaprogramming_macro_host_process_cache_runtime_integration_replay_key,
               .deterministic = true},
              cli_options.input,
              metaprogramming_host_cache_artifact_json,
              metaprogramming_host_cache_error)) {
        std::cerr << metaprogramming_host_cache_error << "\n";
        return 125;
      }
      WriteMetaprogrammingMacroHostProcessCacheArtifact(
          cli_options.out_dir, cli_options.emit_prefix,
          metaprogramming_host_cache_artifact_json);
    }
    if (!IsReadyObjc3VersionedConformanceReportLoweringSummary(
            artifacts.versioned_conformance_report_lowering_summary)) {
      std::cerr << "versioned conformance-report lowering summary not ready\n";
      return 125;
    }
    if (artifacts.versioned_conformance_report_artifact_json.empty()) {
      std::cerr << "versioned conformance-report artifact payload missing\n";
      return 125;
    }
    WriteVersionedConformanceReportArtifact(
        cli_options.out_dir,
        cli_options.emit_prefix,
        artifacts.versioned_conformance_report_artifact_json);
    // versioning/conformance truth-gate anchor: lane-E freezes this
    // emitted lowered report plus the D001 publication sidecar and D002
    // validation mode as one core/json-only fail-closed operator surface.
    // release/runtime-claim-matrix anchor: the published matrix must
    // continue to consume this exact native CLI report/publication/validation
    // surface instead of widening claims beyond the runnable core profile.
    if (cli_options.emit_objc3_conformance) {
      // D002 explicit operator parity: the native path already publishes the
      // JSON conformance sidecar by default, and this flag keeps that behavior
      // explicit/truthful for toolchain workflows that request the report
      // directly.
    }
    std::string conformance_publication_artifact_json;
    std::string conformance_publication_error;
    if (!TryBuildObjc3ConformanceReportPublicationArtifact(
            {.contract_id = "objc3c.driver.conformance.report.publication.v1",
             .schema_id = "objc3c-driver-conformance-publication-v1",
             .selected_profile =
                 ConformanceProfileName(cli_options.conformance_profile),
             .selected_profile_supported = true,
             .supported_profile_ids = {"core"},
             .rejected_profile_ids = {"strict", "strict-concurrency",
                                      "strict-system"},
             .effective_compatibility_mode =
                 (cli_options.compat_mode == Objc3CompatMode::kLegacy
                      ? "legacy"
                      : "canonical"),
             .migration_assist_enabled = cli_options.migration_assist,
             .publication_model =
                 "driver-publishes-lowered-conformance-sidecar-and-runtime-capability-sidecar-next-to-manifest",
             .publication_surface_kind = "native-cli",
             .fail_closed_diagnostic_model =
                 "core-profile-live-other-known-profiles-fail-closed-before-publication",
             .lowered_report_contract_id =
                 "objc3c.versioned.conformance.report.lowering.v1",
             .runtime_capability_contract_id =
                 "objc3c.runtime.capability.reporting.v1",
             .public_conformance_schema_id = "objc3-conformance-report/v1",
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
                 {"strict", "strict-concurrency", "strict-system"},
             .report_artifact_relative_path =
                 (cli_options.emit_prefix +
                  kObjc3VersionedConformanceReportLoweringArtifactSuffix)},
            conformance_publication_artifact_json,
            conformance_publication_error)) {
      std::cerr << conformance_publication_error << "\n";
      return 125;
    }
    WriteConformancePublicationArtifact(cli_options.out_dir,
                                        cli_options.emit_prefix,
                                        conformance_publication_artifact_json);
    std::string advanced_feature_gate_artifact_json;
    std::string advanced_feature_gate_error;
    if (!TryBuildObjc3AdvancedFeatureGateArtifact(
            {.surface_kind = "native-cli",
             .report_artifact_path =
                 (cli_options.emit_prefix +
                  kObjc3VersionedConformanceReportLoweringArtifactSuffix),
             .publication_artifact_path =
                 BuildConformancePublicationArtifactPath(cli_options.out_dir,
                                                        cli_options.emit_prefix)
                     .filename()
                     .string(),
             .validation_artifact_path =
                 BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                       cli_options.emit_prefix)
                     .filename()
                     .string(),
             .release_evidence_operation_artifact_path =
                 BuildReleaseEvidenceOperationArtifactPath(
                     cli_options.out_dir, cli_options.emit_prefix)
                     .filename()
                     .string(),
             .dashboard_artifact_path =
                 BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                                 cli_options.emit_prefix)
                     .filename()
                     .string()},
            artifacts.versioned_conformance_report_artifact_json,
            conformance_publication_artifact_json,
            advanced_feature_gate_artifact_json,
            advanced_feature_gate_error)) {
      std::cerr << advanced_feature_gate_error << "\n";
      return 125;
    }
    WriteAdvancedFeatureGateArtifact(cli_options.out_dir,
                                     cli_options.emit_prefix,
                                     advanced_feature_gate_artifact_json);
    std::string release_candidate_matrix_artifact_json;
    std::string release_candidate_matrix_error;
    if (!TryBuildObjc3ReleaseCandidateMatrixArtifact(
            {.surface_kind = "native-cli",
             .report_artifact_path =
                 (cli_options.emit_prefix +
                  kObjc3VersionedConformanceReportLoweringArtifactSuffix),
             .publication_artifact_path =
                 BuildConformancePublicationArtifactPath(cli_options.out_dir,
                                                        cli_options.emit_prefix)
                     .filename()
                     .string(),
             .advanced_feature_gate_artifact_path =
                 BuildAdvancedFeatureGateArtifactPath(cli_options.out_dir,
                                                     cli_options.emit_prefix)
                     .filename()
                     .string(),
             .validation_artifact_path =
                 BuildConformanceValidationArtifactPath(cli_options.out_dir,
                                                       cli_options.emit_prefix)
                     .filename()
                     .string(),
             .release_evidence_operation_artifact_path =
                 BuildReleaseEvidenceOperationArtifactPath(
                     cli_options.out_dir, cli_options.emit_prefix)
                     .filename()
                     .string(),
             .dashboard_artifact_path =
                 BuildDashboardStatusArtifactPath(cli_options.out_dir,
                                                 cli_options.emit_prefix)
                     .filename()
                     .string()},
            artifacts.versioned_conformance_report_artifact_json,
            conformance_publication_artifact_json,
            advanced_feature_gate_artifact_json,
            release_candidate_matrix_artifact_json,
            release_candidate_matrix_error)) {
      std::cerr << release_candidate_matrix_error << "\n";
      return 125;
    }
    WriteReleaseCandidateMatrixArtifact(cli_options.out_dir,
                                        cli_options.emit_prefix,
                                        release_candidate_matrix_artifact_json);

    // expands the handoff so manifest projection survives fail-closed
    // later lowering/object gates; native runtime linking remains a later
    // milestone.

    const fs::path ir_out = cli_options.out_dir / (cli_options.emit_prefix + ".ll");
    WriteText(ir_out, artifacts.ir_text);

    const fs::path object_out = cli_options.out_dir / (cli_options.emit_prefix + ".obj");
    const bool clang_backend_selected = cli_options.ir_object_backend == Objc3IrObjectBackend::kClang;
    const bool llvm_direct_backend_selected = cli_options.ir_object_backend == Objc3IrObjectBackend::kLLVMDirect;
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
    const bool llvm_direct_backend_enabled = true;
#else
    const bool llvm_direct_backend_enabled = false;
#endif
    const Objc3ToolchainRuntimeGaOperationsScaffold toolchain_runtime_ga_operations_scaffold =
        BuildObjc3ToolchainRuntimeGaOperationsScaffold(
            clang_backend_selected,
            llvm_direct_backend_selected,
            cli_options.clang_path,
            cli_options.llc_path,
            llvm_direct_backend_enabled,
            ir_out,
            object_out);
    std::string toolchain_runtime_scaffold_reason;
    if (!IsObjc3ToolchainRuntimeGaOperationsScaffoldReady(
            toolchain_runtime_ga_operations_scaffold,
            toolchain_runtime_scaffold_reason)) {
      std::cerr << "toolchain/runtime readiness contract fail-closed: "
                << toolchain_runtime_scaffold_reason << "\n";
      return 3;
    }

    int compile_status = 0;
    const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");
    const std::string backend_text =
        cli_options.ir_object_backend == Objc3IrObjectBackend::kClang ? "clang\n" : "llvm-direct\n";
    if (cli_options.ir_object_backend == Objc3IrObjectBackend::kClang) {
      compile_status = RunIRCompile(cli_options.clang_path, ir_out, object_out);
    } else {
      std::string backend_error;
      compile_status = RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error);
      if (!backend_error.empty()) {
        std::cerr << backend_error << "\n";
      }
    }

    bool backend_output_recorded = false;
    std::string backend_output_payload;
    if (compile_status == 0) {
      WriteText(backend_out, backend_text);
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
        std::cerr << linker_retention_error << "\n";
      } else {
        // translation-unit registration surface freeze: the native
        // driver's preregistration payload inventory for one translation unit
        // is the runtime-metadata binary plus the linker-response/discovery
        // sidecars written here. Later startup-registration work may reserve a
        // constructor root, but it must preserve these artifact boundaries and
        // keep runtime ownership centered on objc3_runtime_register_image.
        WriteRuntimeMetadataLinkerResponseArtifact(
            cli_options.out_dir,
            cli_options.emit_prefix,
            linker_retention_artifacts.linker_response_file_payload);
        WriteRuntimeMetadataDiscoveryArtifact(cli_options.out_dir,
                                             cli_options.emit_prefix,
                                             linker_retention_artifacts.discovery_json);
        if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
                artifacts
                    .runtime_translation_unit_registration_manifest_summary)) {
          compile_status = 125;
          std::cerr << "translation-unit registration manifest template not ready\n";
        } else if (!IsReadyObjc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary(
                       artifacts
                           .runtime_registration_descriptor_image_root_source_surface_summary)) {
          compile_status = 125;
          std::cerr << "registration descriptor/image-root source surface not ready\n";
        } else if (!IsReadyObjc3RuntimeRegistrationDescriptorFrontendClosureSummary(
                       artifacts
                           .runtime_registration_descriptor_frontend_closure_summary)) {
          compile_status = 125;
          std::cerr << "registration descriptor frontend closure not ready\n";
        } else {
          Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
              manifest_inputs;
          const auto &registration_manifest_summary =
              artifacts.runtime_translation_unit_registration_manifest_summary;
          const auto &registration_descriptor_source_surface_summary =
              artifacts
                  .runtime_registration_descriptor_image_root_source_surface_summary;
          const auto &registration_descriptor_frontend_closure_summary =
              artifacts.runtime_registration_descriptor_frontend_closure_summary;
          const auto &runtime_bootstrap_api_summary =
              artifacts.runtime_bootstrap_api_summary;
          const auto &runtime_bootstrap_semantics_summary =
              artifacts.runtime_bootstrap_semantics_summary;
          const auto &runtime_bootstrap_lowering_summary =
              artifacts.runtime_bootstrap_lowering_summary;
          manifest_inputs.contract_id = registration_manifest_summary.contract_id;
          manifest_inputs.translation_unit_registration_contract_id =
              registration_manifest_summary
                  .translation_unit_registration_contract_id;
          manifest_inputs.runtime_support_library_link_wiring_contract_id =
              registration_manifest_summary
                  .runtime_support_library_link_wiring_contract_id;
          manifest_inputs.manifest_payload_model =
              registration_manifest_summary.manifest_payload_model;
          manifest_inputs.manifest_artifact_relative_path =
              registration_manifest_summary.manifest_artifact_relative_path;
          manifest_inputs.runtime_owned_payload_artifacts.assign(
              registration_manifest_summary.runtime_owned_payload_artifacts.begin(),
              registration_manifest_summary.runtime_owned_payload_artifacts.end());
          manifest_inputs.runtime_support_library_archive_relative_path =
              registration_manifest_summary
                  .runtime_support_library_archive_relative_path;
          manifest_inputs.constructor_root_symbol =
              registration_manifest_summary.constructor_root_symbol;
          manifest_inputs.constructor_root_ownership_model =
              registration_manifest_summary.constructor_root_ownership_model;
          manifest_inputs.manifest_authority_model =
              registration_manifest_summary.manifest_authority_model;
          manifest_inputs.constructor_init_stub_symbol_prefix =
              registration_manifest_summary.constructor_init_stub_symbol_prefix;
          manifest_inputs.constructor_init_stub_ownership_model =
              registration_manifest_summary
                  .constructor_init_stub_ownership_model;
          manifest_inputs.constructor_priority_policy =
              registration_manifest_summary.constructor_priority_policy;
          manifest_inputs.registration_entrypoint_symbol =
              registration_manifest_summary.registration_entrypoint_symbol;
          manifest_inputs.translation_unit_identity_model =
              registration_manifest_summary.translation_unit_identity_model;
          // startup-registration gate anchor: lane-E consumes the
          // emitted registration manifest plus the A002/B002/C003/D003/D004 evidence chain
          // exactly as published here before E002 closeout broadens the gate.
          // runbook-closeout anchor: the published operator runbook
          // must stay bound to this emitted manifest contract instead of a
          // separately reconstructed launch path.
          manifest_inputs.launch_integration_contract_id =
              registration_manifest_summary.launch_integration_contract_id;
          manifest_inputs.runtime_library_resolution_model =
              registration_manifest_summary.runtime_library_resolution_model;
          manifest_inputs.driver_linker_flag_consumption_model =
              registration_manifest_summary.driver_linker_flag_consumption_model;
          manifest_inputs.compile_wrapper_command_surface =
              registration_manifest_summary.compile_wrapper_command_surface;
          manifest_inputs.compile_proof_command_surface =
              registration_manifest_summary.compile_proof_command_surface;
          manifest_inputs.execution_smoke_command_surface =
              registration_manifest_summary.execution_smoke_command_surface;
          manifest_inputs.dispatch_accessor_runtime_abi_contract_id =
              "objc3c.runtime.dispatch_accessor.abi.surface.v1";
          manifest_inputs.dispatch_accessor_runtime_abi_boundary_model =
              "public-dispatch-entrypoint-plus-private-testing-snapshot-and-property-helper-surface";
          manifest_inputs.dispatch_accessor_public_header_path =
              "native/objc3c/src/runtime/objc3_runtime.h";
          manifest_inputs.dispatch_accessor_private_header_path =
              "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h";
          manifest_inputs.dispatch_accessor_runtime_dispatch_symbol =
              runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
          manifest_inputs.dispatch_accessor_dispatch_state_snapshot_symbol =
              "objc3_runtime_copy_dispatch_state_for_testing";
          manifest_inputs
              .dispatch_accessor_method_cache_state_snapshot_symbol =
              "objc3_runtime_copy_method_cache_state_for_testing";
          manifest_inputs
              .dispatch_accessor_method_cache_entry_snapshot_symbol =
              "objc3_runtime_copy_method_cache_entry_for_testing";
          manifest_inputs
              .dispatch_accessor_property_registry_state_snapshot_symbol =
              "objc3_runtime_copy_property_registry_state_for_testing";
          manifest_inputs.dispatch_accessor_property_entry_snapshot_symbol =
              "objc3_runtime_copy_property_entry_for_testing";
          manifest_inputs.dispatch_accessor_arc_debug_state_snapshot_symbol =
              "objc3_runtime_copy_arc_debug_state_for_testing";
          manifest_inputs.dispatch_accessor_current_property_read_symbol =
              kObjc3RuntimeReadCurrentPropertyI32Symbol;
          manifest_inputs.dispatch_accessor_current_property_write_symbol =
              kObjc3RuntimeWriteCurrentPropertyI32Symbol;
          manifest_inputs.dispatch_accessor_current_property_exchange_symbol =
              kObjc3RuntimeExchangeCurrentPropertyI32Symbol;
          manifest_inputs
              .dispatch_accessor_bind_current_property_context_symbol =
              "objc3_runtime_bind_current_property_context_for_testing";
          manifest_inputs
              .dispatch_accessor_clear_current_property_context_symbol =
              "objc3_runtime_clear_current_property_context_for_testing";
          manifest_inputs.dispatch_accessor_weak_current_property_load_symbol =
              kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol;
          manifest_inputs.dispatch_accessor_weak_current_property_store_symbol =
              kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
          manifest_inputs.dispatch_accessor_retain_symbol =
              kObjc3RuntimeRetainI32Symbol;
          manifest_inputs.dispatch_accessor_release_symbol =
              kObjc3RuntimeReleaseI32Symbol;
          manifest_inputs.dispatch_accessor_autorelease_symbol =
              kObjc3RuntimeAutoreleaseI32Symbol;
          manifest_inputs.dispatch_accessor_private_testing_surface_only = true;
          manifest_inputs.dispatch_accessor_deterministic = true;
          manifest_inputs.storage_accessor_runtime_abi_contract_id =
              kObjc3RuntimeStorageAccessorAbiSurfaceContractId;
          manifest_inputs.storage_accessor_runtime_abi_boundary_model =
              "private-bootstrap-internal-property-helper-and-reflection-snapshot-surface-without-public-header-widening";
          manifest_inputs.storage_accessor_public_header_path =
              "native/objc3c/src/runtime/objc3_runtime.h";
          manifest_inputs.storage_accessor_private_header_path =
              "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h";
          manifest_inputs
              .storage_accessor_property_registry_state_snapshot_symbol =
              "objc3_runtime_copy_property_registry_state_for_testing";
          manifest_inputs.storage_accessor_property_entry_snapshot_symbol =
              "objc3_runtime_copy_property_entry_for_testing";
          manifest_inputs.storage_accessor_current_property_read_symbol =
              kObjc3RuntimeReadCurrentPropertyI32Symbol;
          manifest_inputs.storage_accessor_current_property_write_symbol =
              kObjc3RuntimeWriteCurrentPropertyI32Symbol;
          manifest_inputs.storage_accessor_current_property_exchange_symbol =
              kObjc3RuntimeExchangeCurrentPropertyI32Symbol;
          manifest_inputs
              .storage_accessor_bind_current_property_context_symbol =
              "objc3_runtime_bind_current_property_context_for_testing";
          manifest_inputs
              .storage_accessor_clear_current_property_context_symbol =
              "objc3_runtime_clear_current_property_context_for_testing";
          manifest_inputs.storage_accessor_weak_current_property_load_symbol =
              kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol;
          manifest_inputs.storage_accessor_weak_current_property_store_symbol =
              kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
          manifest_inputs.storage_accessor_private_testing_surface_only = true;
          manifest_inputs.storage_accessor_deterministic = true;
          manifest_inputs.registration_descriptor_source_contract_id =
              registration_descriptor_source_surface_summary.contract_id;
          manifest_inputs.registration_descriptor_source_surface_path =
              registration_descriptor_source_surface_summary.source_surface_path;
          manifest_inputs.registration_descriptor_pragma_name =
              registration_descriptor_source_surface_summary
                  .registration_descriptor_pragma_name;
          manifest_inputs.image_root_pragma_name =
              registration_descriptor_source_surface_summary.image_root_pragma_name;
          manifest_inputs.module_identity_source =
              registration_descriptor_source_surface_summary.module_identity_source;
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
              runtime_bootstrap_semantics_summary.duplicate_registration_policy;
          manifest_inputs.realization_order_policy =
              runtime_bootstrap_semantics_summary.realization_order_policy;
          manifest_inputs.failure_mode =
              runtime_bootstrap_semantics_summary.failure_mode;
          manifest_inputs.registration_result_model =
              runtime_bootstrap_semantics_summary.registration_result_model;
          manifest_inputs.registration_order_ordinal_model =
              runtime_bootstrap_semantics_summary
                  .registration_order_ordinal_model;
          manifest_inputs.runtime_state_snapshot_symbol =
              runtime_bootstrap_semantics_summary.runtime_state_snapshot_symbol;
          // runtime-bootstrap-api anchor: driver-side manifest
          // publication must preserve the frozen runtime header/archive/API
          // boundary rather than re-spelling bootstrap entrypoints or reset
          // hooks ad hoc in later launch-path work.
          manifest_inputs.bootstrap_runtime_api_contract_id =
              runtime_bootstrap_api_summary.contract_id;
          manifest_inputs.bootstrap_runtime_api_public_header_path =
              runtime_bootstrap_api_summary.public_header_path;
          manifest_inputs.bootstrap_runtime_api_archive_relative_path =
              runtime_bootstrap_api_summary.archive_relative_path;
          manifest_inputs.bootstrap_runtime_api_registration_status_enum_type =
              runtime_bootstrap_api_summary.registration_status_enum_type;
          manifest_inputs.bootstrap_runtime_api_image_descriptor_type =
              runtime_bootstrap_api_summary.image_descriptor_type;
          manifest_inputs.bootstrap_runtime_api_selector_handle_type =
              runtime_bootstrap_api_summary.selector_handle_type;
          manifest_inputs.bootstrap_runtime_api_registration_snapshot_type =
              runtime_bootstrap_api_summary.registration_snapshot_type;
          manifest_inputs.bootstrap_runtime_api_registration_entrypoint_symbol =
              runtime_bootstrap_api_summary.registration_entrypoint_symbol;
          manifest_inputs.bootstrap_runtime_api_selector_lookup_symbol =
              runtime_bootstrap_api_summary.selector_lookup_symbol;
          manifest_inputs.bootstrap_runtime_api_dispatch_entrypoint_symbol =
              runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
          manifest_inputs.bootstrap_runtime_api_state_snapshot_symbol =
              runtime_bootstrap_api_summary.state_snapshot_symbol;
          manifest_inputs.bootstrap_runtime_api_reset_for_testing_symbol =
              runtime_bootstrap_api_summary.reset_for_testing_symbol;
          manifest_inputs.bootstrap_registrar_contract_id =
              kObjc3RuntimeBootstrapRegistrarContractId;
          manifest_inputs.bootstrap_registrar_internal_header_path =
              kObjc3RuntimeBootstrapInternalHeaderPath;
          manifest_inputs.bootstrap_registrar_stage_registration_table_symbol =
              kObjc3RuntimeBootstrapStageRegistrationTableSymbol;
          manifest_inputs.bootstrap_registrar_image_walk_snapshot_symbol =
              kObjc3RuntimeBootstrapImageWalkSnapshotSymbol;
          manifest_inputs.bootstrap_registrar_image_walk_model =
              kObjc3RuntimeBootstrapImageWalkModel;
          manifest_inputs.bootstrap_registrar_discovery_root_validation_model =
              kObjc3RuntimeBootstrapDiscoveryRootValidationModel;
          manifest_inputs.bootstrap_registrar_selector_pool_interning_model =
              kObjc3RuntimeBootstrapSelectorPoolInterningModel;
          manifest_inputs.bootstrap_registrar_realization_staging_model =
              kObjc3RuntimeBootstrapRealizationStagingModel;
          manifest_inputs.bootstrap_reset_contract_id =
              kObjc3RuntimeBootstrapResetContractId;
          manifest_inputs.bootstrap_reset_internal_header_path =
              kObjc3RuntimeBootstrapInternalHeaderPath;
          manifest_inputs.bootstrap_reset_replay_registered_images_symbol =
              kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
          manifest_inputs.bootstrap_reset_reset_replay_state_snapshot_symbol =
              kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
          manifest_inputs.bootstrap_reset_lifecycle_model =
              kObjc3RuntimeBootstrapResetLifecycleModel;
          manifest_inputs.bootstrap_reset_replay_order_model =
              kObjc3RuntimeBootstrapReplayOrderModel;
          manifest_inputs.bootstrap_reset_image_local_init_state_reset_model =
              kObjc3RuntimeBootstrapImageLocalInitStateResetModel;
          manifest_inputs.bootstrap_reset_bootstrap_catalog_retention_model =
              kObjc3RuntimeBootstrapCatalogRetentionModel;
          // bootstrap-lowering anchor: driver-side artifact
          // materialization remains limited to the registration manifest. The
          // actual ctor-root/init-stub/registration-table IR globals stay
          // owned by lowering and must be derived from the canonical lowering
          // packet rather than reconstructed ad hoc here.
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
              runtime_bootstrap_lowering_summary.init_stub_emission_state;
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
              runtime_bootstrap_semantics_summary.out_of_order_status_code;
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
                  artifacts.runtime_metadata_binary.size(),
                  registration_manifest_json,
                  registration_manifest_error)) {
            compile_status = 125;
            std::cerr << registration_manifest_error << "\n";
          } else {
            // registration-manifest anchor: the driver now writes one
            // canonical startup-registration manifest per translation unit so
            // later lowering/bootstrap lanes consume direct constructor-root
            // ownership inputs instead of reconstructing them from ad hoc
            // sidecar parsing rules.
            // constructor/init-stub emission anchor: the same
            // manifest now publishes the exact derived registration-table
            // symbol alongside the init-stub symbol so emitted IR/object
            // bootstrap artifacts can be checked against one authoritative
            // manifest packet.
            // registration-table/image-local-init anchor: the
            // manifest also publishes the self-describing registration-table
            // layout model, ABI/version counts, and the exact derived
            // image-local init-state symbol so later runtime image-walk work
            // consumes one canonical lowering-owned boundary.
            // bootstrap-invariant anchor: later startup execution
            // must continue to treat this manifest as the authoritative source
            // for one constructor root per translation-unit identity, fail
            // duplicate registration closed, and preserve deterministic
            // realization order before user entry.
            // bootstrap-semantics anchor: the manifest now also
            // carries the live runtime duplicate/order/failure contract and
            // status-code model consumed by the runtime library probe.
            // launch-integration anchor: compile/proof/smoke command
            // surfaces consume this emitted registration manifest directly for
            // runtime archive resolution and launch/link boundary validation.
            // bootstrap-completion gate anchor: lane-E consumes this
            // emitted registration manifest plus the A002/B003/C003/D003 proof chain
            // to decide whether current single-image and multi-image bootstrap
            // completion is satisfied.
            // bootstrap-matrix closeout anchor: the published operator runbook and matrix proof
            // consume this same manifest authority without
            // inventing a separate startup source of truth.
            WriteRuntimeRegistrationManifestArtifact(
                cli_options.out_dir,
                cli_options.emit_prefix,
                registration_manifest_json);

            Objc3RuntimeRegistrationDescriptorArtifactInputs descriptor_inputs;
            descriptor_inputs.contract_id =
                registration_descriptor_frontend_closure_summary.contract_id;
            descriptor_inputs.registration_manifest_contract_id =
                registration_descriptor_frontend_closure_summary
                    .registration_manifest_contract_id;
            descriptor_inputs.source_surface_contract_id =
                registration_descriptor_frontend_closure_summary
                    .source_surface_contract_id;
            descriptor_inputs.bootstrap_lowering_contract_id =
                runtime_bootstrap_lowering_summary.contract_id;
            descriptor_inputs.payload_model =
                registration_descriptor_frontend_closure_summary.payload_model;
            descriptor_inputs.artifact_relative_path =
                registration_descriptor_frontend_closure_summary
                    .artifact_relative_path;
            descriptor_inputs.authority_model =
                registration_descriptor_frontend_closure_summary.authority_model;
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
                registration_descriptor_source_surface_summary.image_root_pragma_name;
            descriptor_inputs.module_identity_source =
                registration_descriptor_source_surface_summary.module_identity_source;
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
                registration_manifest_summary.constructor_init_stub_symbol_prefix;
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
                runtime_bootstrap_lowering_summary.init_stub_emission_state;
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
                    descriptor_inputs, linker_retention_artifacts,
                    registration_descriptor_json,
                    registration_descriptor_error)) {
              compile_status = 125;
              std::cerr << registration_descriptor_error << "\n";
            } else {
              // bootstrap-completion gate anchor: lane-E treats the
              // registration descriptor and registration manifest as one
              // canonical emitted artifact pair rather than reconstructing
              // descriptor authority from ad hoc sidecars.
              // bootstrap-matrix closeout anchor: the published
              // operator runbook and matrix proof consume this same canonical pair.
              WriteRuntimeRegistrationDescriptorArtifact(
                  cli_options.out_dir, cli_options.emit_prefix,
                  registration_descriptor_json);
            }
          }
        }
      }

      if (compile_status == 0 &&
          !cli_options.imported_runtime_surface_paths.empty()) {
        std::vector<Objc3ImportedRuntimeModuleSurface> imported_surfaces;
        imported_surfaces.reserve(cli_options.imported_runtime_surface_paths.size());
        std::vector<Objc3ImportedRuntimeModulePackagingPeerArtifacts>
            imported_peer_artifacts;
        imported_peer_artifacts.reserve(
            cli_options.imported_runtime_surface_paths.size());

        for (const auto &import_path : cli_options.imported_runtime_surface_paths) {
          Objc3ImportedRuntimeModuleSurface imported_surface;
          std::string import_surface_error;
          const fs::path absolute_import_path =
              fs::absolute(import_path).lexically_normal();
          if (!TryLoadObjc3ImportedRuntimeModuleSurface(
                  absolute_import_path, imported_surface, import_surface_error)) {
            compile_status = 125;
            std::cerr << import_surface_error << "\n";
            break;
          }
          Objc3ImportedRuntimeModulePackagingPeerArtifacts peer_artifacts;
          std::string peer_artifacts_error;
          if (!TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
                  imported_surface, peer_artifacts, peer_artifacts_error)) {
            compile_status = 125;
            std::cerr << peer_artifacts_error << "\n";
            break;
          }
          imported_surfaces.push_back(std::move(imported_surface));
          imported_peer_artifacts.push_back(std::move(peer_artifacts));
        }

        if (compile_status == 0) {
          Objc3CrossModuleRuntimeLinkPlanArtifactInputs link_plan_inputs;
          link_plan_inputs.contract_id =
              kObjc3CrossModuleRuntimeLinkPlanContractId;
          link_plan_inputs.source_orchestration_contract_id =
              kObjc3CrossModuleBuildRuntimeOrchestrationContractId;
          link_plan_inputs.import_surface_contract_id =
              kObjc3RuntimeAwareImportModuleFrontendClosureContractId;
          link_plan_inputs.registration_manifest_contract_id =
              kObjc3RuntimeTranslationUnitRegistrationManifestContractId;
          link_plan_inputs.payload_model =
              kObjc3CrossModuleRuntimeLinkPlanPayloadModel;
          link_plan_inputs.artifact_relative_path =
              kObjc3CrossModuleRuntimeLinkPlanArtifactRelativePath;
          link_plan_inputs.linker_response_artifact_relative_path =
              kObjc3CrossModuleRuntimeLinkerResponseArtifactRelativePath;
          link_plan_inputs.authority_model =
              kObjc3CrossModuleRuntimeLinkPlanAuthorityModel;
          link_plan_inputs.packaging_model =
              kObjc3CrossModuleRuntimeLinkPlanPackagingModel;
          link_plan_inputs.registration_scope_model =
              kObjc3CrossModuleRuntimeLinkPlanRegistrationScopeModel;
          link_plan_inputs.link_object_order_model =
              kObjc3CrossModuleRuntimeLinkObjectOrderModel;
          link_plan_inputs.local_module_name =
              artifacts.runtime_aware_import_module_frontend_closure_summary
                  .module_name;
          link_plan_inputs.local_import_surface_artifact_relative_path =
              fs::absolute(BuildRuntimeAwareImportModuleArtifactPath(
                               cli_options.out_dir, cli_options.emit_prefix))
                  .lexically_normal()
                  .generic_string();
          link_plan_inputs.local_registration_manifest_artifact_relative_path =
              fs::absolute(BuildRuntimeRegistrationManifestArtifactPath(
                               cli_options.out_dir, cli_options.emit_prefix))
                  .lexically_normal()
                  .generic_string();
          link_plan_inputs.local_object_artifact_relative_path =
              fs::absolute(object_out).lexically_normal().generic_string();
          link_plan_inputs.runtime_support_library_archive_relative_path =
              artifacts.runtime_translation_unit_registration_manifest_summary
                  .runtime_support_library_archive_relative_path;
          link_plan_inputs.object_format = linker_retention_artifacts.object_format;
          link_plan_inputs.local_translation_unit_identity_model =
              linker_retention_artifacts.translation_unit_identity_model;
          link_plan_inputs.local_translation_unit_identity_key =
              linker_retention_artifacts.translation_unit_identity_key;
          link_plan_inputs.local_translation_unit_registration_order_ordinal =
              artifacts.runtime_translation_unit_registration_manifest_summary
                  .translation_unit_registration_order_ordinal;
          link_plan_inputs.local_class_descriptor_count =
              artifacts.runtime_translation_unit_registration_manifest_summary
                  .class_descriptor_count;
          link_plan_inputs.local_protocol_descriptor_count =
              artifacts.runtime_translation_unit_registration_manifest_summary
                  .protocol_descriptor_count;
          link_plan_inputs.local_category_descriptor_count =
              artifacts.runtime_translation_unit_registration_manifest_summary
                  .category_descriptor_count;
          link_plan_inputs.local_property_descriptor_count =
              artifacts.runtime_translation_unit_registration_manifest_summary
                  .property_descriptor_count;
          link_plan_inputs.local_ivar_descriptor_count =
              artifacts.runtime_translation_unit_registration_manifest_summary
                  .ivar_descriptor_count;
          link_plan_inputs.local_total_descriptor_count =
              artifacts.runtime_translation_unit_registration_manifest_summary
                  .total_descriptor_count;
          const auto local_block_ownership_summary =
              artifacts.runtime_block_ownership_artifact_preservation_summary;
          link_plan_inputs.local_block_ownership_block_literal_sites =
              local_block_ownership_summary.local_block_literal_sites;
          link_plan_inputs
              .local_block_ownership_invoke_trampoline_symbolized_sites =
              local_block_ownership_summary
                  .local_invoke_trampoline_symbolized_sites;
          link_plan_inputs.local_block_ownership_copy_helper_required_sites =
              local_block_ownership_summary.local_copy_helper_required_sites;
          link_plan_inputs.local_block_ownership_dispose_helper_required_sites =
              local_block_ownership_summary.local_dispose_helper_required_sites;
          link_plan_inputs.local_block_ownership_copy_helper_symbolized_sites =
              local_block_ownership_summary.local_copy_helper_symbolized_sites;
          link_plan_inputs
              .local_block_ownership_dispose_helper_symbolized_sites =
              local_block_ownership_summary
                  .local_dispose_helper_symbolized_sites;
          link_plan_inputs.local_block_ownership_escape_to_heap_sites =
              local_block_ownership_summary.local_escape_to_heap_sites;
          link_plan_inputs.local_block_ownership_byref_layout_symbolized_sites =
              local_block_ownership_summary
                  .local_byref_layout_symbolized_sites;
          const auto local_storage_reflection_summary =
              artifacts.runtime_storage_reflection_artifact_preservation_summary;
          link_plan_inputs
              .local_storage_reflection_implementation_owned_property_entries =
              local_storage_reflection_summary
                  .implementation_owned_property_entries;
          link_plan_inputs
              .local_storage_reflection_synthesized_accessor_owner_entries =
              local_storage_reflection_summary
                  .synthesized_accessor_owner_entries;
          link_plan_inputs.local_storage_reflection_synthesized_getter_entries =
              local_storage_reflection_summary.synthesized_getter_entries;
          link_plan_inputs.local_storage_reflection_synthesized_setter_entries =
              local_storage_reflection_summary.synthesized_setter_entries;
          link_plan_inputs
              .local_storage_reflection_synthesized_accessor_entries =
              local_storage_reflection_summary.synthesized_accessor_entries;
          link_plan_inputs
              .local_storage_reflection_current_property_read_entries =
              local_storage_reflection_summary.current_property_read_entries;
          link_plan_inputs
              .local_storage_reflection_current_property_write_entries =
              local_storage_reflection_summary.current_property_write_entries;
          link_plan_inputs
              .local_storage_reflection_current_property_exchange_entries =
              local_storage_reflection_summary.current_property_exchange_entries;
          link_plan_inputs
              .local_storage_reflection_weak_current_property_load_entries =
              local_storage_reflection_summary
                  .weak_current_property_load_entries;
          link_plan_inputs
              .local_storage_reflection_weak_current_property_store_entries =
              local_storage_reflection_summary
                  .weak_current_property_store_entries;
          link_plan_inputs.local_storage_reflection_ivar_layout_entries =
              local_storage_reflection_summary.ivar_layout_entries;
          link_plan_inputs.local_storage_reflection_ivar_layout_owner_entries =
              local_storage_reflection_summary.ivar_layout_owner_entries;
          link_plan_inputs.local_driver_linker_flags = {
              linker_retention_artifacts.driver_linker_flag};

          link_plan_inputs.expected_error_handling_contract_id =
              kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId;
          link_plan_inputs.expected_error_handling_source_contract_id =
              kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId;
          link_plan_inputs.expected_concurrency_actor_contract_id =
              "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1";
          link_plan_inputs.expected_concurrency_actor_source_contract_id =
              "objc3c.concurrency.actor.lowering.and.metadata.contract.v1";
          link_plan_inputs.expected_interop_ffi_contract_id =
              kObjc3InteropFfiMetadataInterfacePreservationContractId;
          link_plan_inputs.expected_interop_ffi_source_contract_id =
              kObjc3InteropFfiMetadataInterfacePreservationSourceContractId;
          link_plan_inputs.expected_interop_ffi_preservation_contract_id =
              kObjc3InteropForeignSurfaceInterfacePreservationContractId;
          link_plan_inputs.expected_interop_header_module_bridge_contract_id =
              kObjc3InteropHeaderModuleBridgeGenerationContractId;
          link_plan_inputs.expected_interop_header_module_bridge_source_contract_id =
              kObjc3InteropHeaderModuleBridgeGenerationSourceContractId;
          link_plan_inputs
              .expected_interop_header_module_bridge_preservation_contract_id =
              kObjc3InteropHeaderModuleBridgeGenerationPreservationContractId;
          link_plan_inputs.expected_interop_bridge_header_artifact_relative_path =
              kObjc3InteropBridgeHeaderArtifactRelativePath;
          link_plan_inputs.expected_interop_bridge_module_artifact_relative_path =
              kObjc3InteropBridgeModuleArtifactRelativePath;
          link_plan_inputs.expected_interop_bridge_artifact_relative_path =
              kObjc3InteropBridgeArtifactRelativePath;
          link_plan_inputs.expected_metaprogramming_host_cache_contract_id =
              kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId;
          link_plan_inputs.expected_metaprogramming_host_cache_source_contract_id =
              kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId;
          link_plan_inputs.expected_metaprogramming_host_cache_executable_relative_path =
              kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath;
          link_plan_inputs.expected_metaprogramming_host_cache_root_relative_path =
              kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath;
          link_plan_inputs.expected_block_ownership_contract_id =
              kObjc3RuntimeBlockOwnershipArtifactPreservationContractId;
          link_plan_inputs.expected_block_ownership_source_contract_id =
              kObjc3RuntimeBlockArcLoweringHelperSurfaceContractId;
          link_plan_inputs
              .expected_block_ownership_object_invoke_thunk_lowering_contract_id =
              Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId;
          link_plan_inputs
              .expected_block_ownership_byref_helper_lowering_contract_id =
              Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId;
          link_plan_inputs
              .expected_block_ownership_escape_runtime_hook_lowering_contract_id =
              Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId;
          link_plan_inputs
              .expected_block_ownership_runtime_support_library_link_wiring_contract_id =
              kObjc3RuntimeSupportLibraryLinkWiringContractId;
          link_plan_inputs.expected_storage_reflection_contract_id =
              kObjc3RuntimeStorageReflectionArtifactPreservationContractId;
          link_plan_inputs.expected_storage_reflection_source_contract_id =
              kObjc3RuntimePropertyIvarStorageAccessorSourceSurfaceContractId;
          link_plan_inputs
              .expected_storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id =
              kObjc3DispatchAndSynthesizedAccessorLoweringSurfaceContractId;
          link_plan_inputs
              .expected_storage_reflection_executable_property_accessor_layout_lowering_contract_id =
              kObjc3ExecutablePropertyAccessorLayoutLoweringContractId;
          link_plan_inputs
              .expected_storage_reflection_executable_ivar_layout_emission_contract_id =
              kObjc3ExecutableIvarLayoutEmissionContractId;
          link_plan_inputs
              .expected_storage_reflection_executable_synthesized_accessor_property_lowering_contract_id =
              kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId;
          link_plan_inputs.expected_bootstrap_live_registration_contract_id =
              "objc3c.runtime.live.registration.discovery.replay.v1";
          link_plan_inputs.expected_bootstrap_live_restart_hardening_contract_id =
              "objc3c.runtime.live.restart.hardening.v1";
          link_plan_inputs.expected_bootstrap_replay_registered_images_symbol =
              kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol;
          link_plan_inputs.expected_bootstrap_reset_replay_state_snapshot_symbol =
              kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol;
          link_plan_inputs.expected_bootstrap_reset_for_testing_symbol =
              kObjc3RuntimeSupportLibraryResetForTestingSymbol;
          for (std::size_t index = 0; index < imported_surfaces.size(); ++index) {
            link_plan_inputs.direct_import_surface_artifact_paths.push_back(
                imported_surfaces[index].source_path.generic_string());
            const auto &peer_artifacts = imported_peer_artifacts[index];
            const auto &imported_surface = imported_surfaces[index];
            Objc3CrossModuleRuntimeLinkPlanImportedInput imported_input;
            imported_input.module_name =
                imported_surface.frontend_closure_summary.module_name;
            imported_input.import_surface_artifact_path =
                imported_surface.source_path.generic_string();
            imported_input.registration_manifest_artifact_path =
                peer_artifacts.registration_manifest_path.generic_string();
            imported_input.object_artifact_path =
                peer_artifacts.object_artifact_path.generic_string();
            imported_input.discovery_artifact_path =
                peer_artifacts.discovery_artifact_path.generic_string();
            imported_input.linker_response_artifact_path =
                peer_artifacts.linker_response_artifact_path.generic_string();
            imported_input.translation_unit_identity_model =
                peer_artifacts.translation_unit_identity_model;
            imported_input.translation_unit_identity_key =
                peer_artifacts.translation_unit_identity_key;
            imported_input.object_format = peer_artifacts.object_format;
            imported_input.runtime_support_library_archive_relative_path =
                peer_artifacts.runtime_support_library_archive_relative_path;
            imported_input.translation_unit_registration_order_ordinal =
                peer_artifacts.translation_unit_registration_order_ordinal;
            imported_input.class_descriptor_count =
                peer_artifacts.class_descriptor_count;
            imported_input.protocol_descriptor_count =
                peer_artifacts.protocol_descriptor_count;
            imported_input.category_descriptor_count =
                peer_artifacts.category_descriptor_count;
            imported_input.property_descriptor_count =
                peer_artifacts.property_descriptor_count;
            imported_input.ivar_descriptor_count =
                peer_artifacts.ivar_descriptor_count;
            imported_input.total_descriptor_count =
                peer_artifacts.total_descriptor_count;
            imported_input.driver_linker_flags =
                peer_artifacts.driver_linker_flags;
            imported_input.ready_for_live_registration_discovery_replay =
                peer_artifacts.ready_for_live_registration_discovery_replay;
            imported_input.ready_for_live_restart_hardening =
                peer_artifacts.ready_for_live_restart_hardening;
            imported_input.bootstrap_live_registration_contract_id =
                peer_artifacts.bootstrap_live_registration_contract_id;
            imported_input.bootstrap_live_restart_hardening_contract_id =
                peer_artifacts.bootstrap_live_restart_hardening_contract_id;
            imported_input.bootstrap_live_replay_registered_images_symbol =
                peer_artifacts.bootstrap_live_replay_registered_images_symbol;
            imported_input.bootstrap_live_reset_replay_state_snapshot_symbol =
                peer_artifacts
                    .bootstrap_live_reset_replay_state_snapshot_symbol;
            imported_input.bootstrap_live_restart_reset_for_testing_symbol =
                peer_artifacts
                    .bootstrap_live_restart_reset_for_testing_symbol;
            imported_input.bootstrap_live_restart_replay_registered_images_symbol =
                peer_artifacts
                    .bootstrap_live_restart_replay_registered_images_symbol;
            imported_input
                .bootstrap_live_restart_reset_replay_state_snapshot_symbol =
                peer_artifacts
                    .bootstrap_live_restart_reset_replay_state_snapshot_symbol;
            imported_input.error_handling_result_and_bridging_artifact_replay_present =
                imported_surface.error_handling_result_and_bridging_artifact_replay_present;
            imported_input.error_handling_binary_artifact_replay_ready =
                imported_surface.error_handling_binary_artifact_replay_ready;
            imported_input.error_handling_runtime_import_artifact_ready =
                imported_surface.error_handling_runtime_import_artifact_ready;
            imported_input.error_handling_separate_compilation_replay_ready =
                imported_surface.error_handling_separate_compilation_replay_ready;
            imported_input.error_handling_deterministic =
                imported_surface.error_handling_deterministic;
            imported_input.error_handling_contract_id = imported_surface.error_handling_contract_id;
            imported_input.error_handling_source_contract_id =
                imported_surface.error_handling_source_contract_id;
            imported_input.error_handling_result_and_bridging_artifact_replay_key =
                imported_surface.error_handling_result_and_bridging_artifact_replay_key;
            imported_input.error_handling_error_handling_replay_key =
                imported_surface.error_handling_error_handling_replay_key;
            imported_input.error_handling_throws_replay_key =
                imported_surface.error_handling_throws_replay_key;
            imported_input.error_handling_result_like_replay_key =
                imported_surface.error_handling_result_like_replay_key;
            imported_input.error_handling_ns_error_replay_key =
                imported_surface.error_handling_ns_error_replay_key;
            imported_input.error_handling_unwind_replay_key =
                imported_surface.error_handling_unwind_replay_key;
            imported_input.concurrency_actor_mailbox_runtime_import_present =
                imported_surface.concurrency_actor_mailbox_runtime_import_present;
            imported_input.concurrency_actor_mailbox_runtime_ready =
                imported_surface.concurrency_actor_mailbox_runtime_ready;
            imported_input.concurrency_actor_mailbox_runtime_deterministic =
                imported_surface.concurrency_actor_mailbox_runtime_deterministic;
            imported_input.concurrency_actor_contract_id =
                imported_surface.concurrency_actor_mailbox_runtime_contract_id;
            imported_input.concurrency_actor_source_contract_id =
                imported_surface.concurrency_actor_mailbox_runtime_source_contract_id;
            imported_input.concurrency_actor_mailbox_runtime_replay_key =
                imported_surface.concurrency_actor_mailbox_runtime_replay_key;
            imported_input.concurrency_actor_lowering_replay_key =
                imported_surface.concurrency_actor_lowering_replay_key;
            imported_input.concurrency_actor_isolation_lowering_replay_key =
                imported_surface.concurrency_actor_isolation_lowering_replay_key;
            imported_input.interop_ffi_metadata_interface_preservation_present =
                imported_surface.interop_ffi_metadata_interface_preservation_present;
            imported_input.interop_ffi_runtime_import_artifact_ready =
                imported_surface.interop_ffi_runtime_import_artifact_ready;
            imported_input.interop_ffi_separate_compilation_preservation_ready =
                imported_surface
                    .interop_ffi_separate_compilation_preservation_ready;
            imported_input.interop_ffi_deterministic =
                imported_surface.interop_ffi_deterministic;
            imported_input.interop_ffi_contract_id =
                imported_surface.interop_ffi_contract_id;
            imported_input.interop_ffi_source_contract_id =
                imported_surface.interop_ffi_source_contract_id;
            imported_input.interop_ffi_preservation_contract_id =
                imported_surface.interop_ffi_preservation_contract_id;
            imported_input.interop_ffi_replay_key =
                imported_surface.interop_ffi_replay_key;
            imported_input.interop_ffi_lowering_replay_key =
                imported_surface.interop_ffi_lowering_replay_key;
            imported_input.interop_ffi_preservation_replay_key =
                imported_surface.interop_ffi_preservation_replay_key;
            imported_input.interop_ffi_local_foreign_callable_count =
                imported_surface.interop_ffi_local_foreign_callable_count;
            imported_input.interop_ffi_local_metadata_preservation_sites =
                imported_surface.interop_ffi_local_metadata_preservation_sites;
            imported_input.interop_ffi_local_interface_annotation_sites =
                imported_surface.interop_ffi_local_interface_annotation_sites;
            imported_input.interop_header_module_bridge_generation_present =
                imported_surface.interop_header_module_bridge_generation_present;
            imported_input.interop_header_module_bridge_runtime_generation_ready =
                imported_surface
                    .interop_header_module_bridge_runtime_generation_ready;
            imported_input
                .interop_header_module_bridge_cross_module_packaging_ready =
                imported_surface
                    .interop_header_module_bridge_cross_module_packaging_ready;
            imported_input.interop_header_module_bridge_deterministic =
                imported_surface.interop_header_module_bridge_deterministic;
            imported_input.interop_header_module_bridge_contract_id =
                imported_surface.interop_header_module_bridge_contract_id;
            imported_input.interop_header_module_bridge_source_contract_id =
                imported_surface.interop_header_module_bridge_source_contract_id;
            imported_input.interop_header_module_bridge_preservation_contract_id =
                imported_surface
                    .interop_header_module_bridge_preservation_contract_id;
            imported_input.interop_header_module_bridge_replay_key =
                imported_surface.interop_header_module_bridge_replay_key;
            imported_input.interop_header_module_bridge_preservation_replay_key =
                imported_surface
                    .interop_header_module_bridge_preservation_replay_key;
            imported_input.interop_bridge_header_artifact_relative_path =
                imported_surface.interop_bridge_header_artifact_relative_path;
            imported_input.interop_bridge_module_artifact_relative_path =
                imported_surface.interop_bridge_module_artifact_relative_path;
            imported_input.interop_bridge_artifact_relative_path =
                imported_surface.interop_bridge_artifact_relative_path;
            imported_input.interop_header_module_bridge_local_foreign_callable_count =
                imported_surface
                    .interop_header_module_bridge_local_foreign_callable_count;
            imported_input
                .metaprogramming_macro_host_process_cache_runtime_integration_present =
                imported_surface
                    .metaprogramming_macro_host_process_cache_runtime_integration_present;
            imported_input.metaprogramming_macro_host_process_cache_runtime_ready =
                imported_surface.metaprogramming_macro_host_process_cache_runtime_ready;
            imported_input
                .metaprogramming_macro_host_process_cache_separate_compilation_ready =
                imported_surface
                    .metaprogramming_macro_host_process_cache_separate_compilation_ready;
            imported_input.metaprogramming_macro_host_process_cache_deterministic =
                imported_surface.metaprogramming_macro_host_process_cache_deterministic;
            imported_input.metaprogramming_macro_host_process_cache_contract_id =
                imported_surface.metaprogramming_macro_host_process_cache_contract_id;
            imported_input.metaprogramming_macro_host_process_cache_source_contract_id =
                imported_surface
                    .metaprogramming_macro_host_process_cache_source_contract_id;
            imported_input.metaprogramming_macro_host_process_cache_replay_key =
                imported_surface.metaprogramming_macro_host_process_cache_replay_key;
            imported_input
                .metaprogramming_macro_host_process_cache_host_executable_relative_path =
                imported_surface
                    .metaprogramming_macro_host_process_cache_host_executable_relative_path;
            imported_input.metaprogramming_macro_host_process_cache_root_relative_path =
                imported_surface
                    .metaprogramming_macro_host_process_cache_root_relative_path;
            imported_input.block_ownership_artifact_preservation_present =
                imported_surface.block_ownership_artifact_preservation_present;
            imported_input.block_ownership_runtime_import_artifact_ready =
                imported_surface.block_ownership_runtime_import_artifact_ready;
            imported_input
                .block_ownership_separate_compilation_preservation_ready =
                imported_surface
                    .block_ownership_separate_compilation_preservation_ready;
            imported_input
                .block_ownership_runtime_support_library_link_wiring_ready =
                imported_surface
                    .block_ownership_runtime_support_library_link_wiring_ready;
            imported_input.block_ownership_deterministic =
                imported_surface.block_ownership_deterministic;
            imported_input.block_ownership_contract_id =
                imported_surface.block_ownership_contract_id;
            imported_input.block_ownership_source_contract_id =
                imported_surface.block_ownership_source_contract_id;
            imported_input.block_ownership_object_invoke_thunk_lowering_contract_id =
                imported_surface
                    .block_ownership_object_invoke_thunk_lowering_contract_id;
            imported_input.block_ownership_byref_helper_lowering_contract_id =
                imported_surface
                    .block_ownership_byref_helper_lowering_contract_id;
            imported_input.block_ownership_escape_runtime_hook_lowering_contract_id =
                imported_surface
                    .block_ownership_escape_runtime_hook_lowering_contract_id;
            imported_input
                .block_ownership_runtime_support_library_link_wiring_contract_id =
                imported_surface
                    .block_ownership_runtime_support_library_link_wiring_contract_id;
            imported_input.block_ownership_replay_key =
                imported_surface.block_ownership_replay_key;
            imported_input.block_ownership_local_block_literal_sites =
                imported_surface.block_ownership_local_block_literal_sites;
            imported_input
                .block_ownership_local_invoke_trampoline_symbolized_sites =
                imported_surface
                    .block_ownership_local_invoke_trampoline_symbolized_sites;
            imported_input.block_ownership_local_copy_helper_required_sites =
                imported_surface
                    .block_ownership_local_copy_helper_required_sites;
            imported_input.block_ownership_local_dispose_helper_required_sites =
                imported_surface
                    .block_ownership_local_dispose_helper_required_sites;
            imported_input.block_ownership_local_copy_helper_symbolized_sites =
                imported_surface
                    .block_ownership_local_copy_helper_symbolized_sites;
            imported_input.block_ownership_local_dispose_helper_symbolized_sites =
                imported_surface
                    .block_ownership_local_dispose_helper_symbolized_sites;
            imported_input.block_ownership_local_escape_to_heap_sites =
                imported_surface.block_ownership_local_escape_to_heap_sites;
            imported_input.block_ownership_local_byref_layout_symbolized_sites =
                imported_surface
                    .block_ownership_local_byref_layout_symbolized_sites;
            imported_input.storage_reflection_artifact_preservation_present =
                imported_surface
                    .storage_reflection_artifact_preservation_present;
            imported_input.storage_reflection_runtime_import_artifact_ready =
                imported_surface
                    .storage_reflection_runtime_import_artifact_ready;
            imported_input
                .storage_reflection_separate_compilation_preservation_ready =
                imported_surface
                    .storage_reflection_separate_compilation_preservation_ready;
            imported_input.storage_reflection_deterministic =
                imported_surface.storage_reflection_deterministic;
            imported_input.storage_reflection_contract_id =
                imported_surface.storage_reflection_contract_id;
            imported_input.storage_reflection_source_contract_id =
                imported_surface.storage_reflection_source_contract_id;
            imported_input
                .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id =
                imported_surface
                    .storage_reflection_dispatch_and_synthesized_accessor_lowering_surface_contract_id;
            imported_input
                .storage_reflection_executable_property_accessor_layout_lowering_contract_id =
                imported_surface
                    .storage_reflection_executable_property_accessor_layout_lowering_contract_id;
            imported_input
                .storage_reflection_executable_ivar_layout_emission_contract_id =
                imported_surface
                    .storage_reflection_executable_ivar_layout_emission_contract_id;
            imported_input
                .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id =
                imported_surface
                    .storage_reflection_executable_synthesized_accessor_property_lowering_contract_id;
            imported_input.storage_reflection_replay_key =
                imported_surface.storage_reflection_replay_key;
            imported_input.storage_reflection_local_property_descriptor_count =
                imported_surface
                    .storage_reflection_local_property_descriptor_count;
            imported_input.storage_reflection_local_ivar_descriptor_count =
                imported_surface.storage_reflection_local_ivar_descriptor_count;
            imported_input
                .storage_reflection_implementation_owned_property_entries =
                imported_surface
                    .storage_reflection_implementation_owned_property_entries;
            imported_input.storage_reflection_synthesized_accessor_owner_entries =
                imported_surface
                    .storage_reflection_synthesized_accessor_owner_entries;
            imported_input.storage_reflection_synthesized_getter_entries =
                imported_surface
                    .storage_reflection_synthesized_getter_entries;
            imported_input.storage_reflection_synthesized_setter_entries =
                imported_surface
                    .storage_reflection_synthesized_setter_entries;
            imported_input.storage_reflection_synthesized_accessor_entries =
                imported_surface
                    .storage_reflection_synthesized_accessor_entries;
            imported_input.storage_reflection_current_property_read_entries =
                imported_surface
                    .storage_reflection_current_property_read_entries;
            imported_input.storage_reflection_current_property_write_entries =
                imported_surface
                    .storage_reflection_current_property_write_entries;
            imported_input.storage_reflection_current_property_exchange_entries =
                imported_surface
                    .storage_reflection_current_property_exchange_entries;
            imported_input
                .storage_reflection_weak_current_property_load_entries =
                imported_surface
                    .storage_reflection_weak_current_property_load_entries;
            imported_input
                .storage_reflection_weak_current_property_store_entries =
                imported_surface
                    .storage_reflection_weak_current_property_store_entries;
            imported_input.storage_reflection_ivar_layout_entries =
                imported_surface.storage_reflection_ivar_layout_entries;
            imported_input.storage_reflection_ivar_layout_owner_entries =
                imported_surface
                    .storage_reflection_ivar_layout_owner_entries;
            link_plan_inputs.imported_inputs.push_back(std::move(imported_input));
          }

          std::string link_plan_json;
          std::string cross_module_linker_response_payload;
          std::string link_plan_error;
          if (!TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(
                  link_plan_inputs, link_plan_json,
                  cross_module_linker_response_payload, link_plan_error)) {
            compile_status = 125;
            std::cerr << link_plan_error << "\n";
          } else {
            WriteCrossModuleRuntimeLinkPlanArtifact(cli_options.out_dir,
                                                   cli_options.emit_prefix,
                                                   link_plan_json);
            WriteCrossModuleRuntimeLinkerResponseArtifact(
                cli_options.out_dir, cli_options.emit_prefix,
                cross_module_linker_response_payload);
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
    if (!IsObjc3ToolchainRuntimeGaOperationsCoreFeatureSurfaceReady(
            toolchain_runtime_core_feature_surface,
            toolchain_runtime_core_feature_reason)) {
      std::cerr << "toolchain/runtime core feature fail-closed: "
                << toolchain_runtime_core_feature_reason << "\n";
      return 3;
    }

    return 0;
  } catch (const std::exception &io_error) {
    std::cerr << "artifact io failure: " << io_error.what() << "\n";
    return 3;
  }
}

