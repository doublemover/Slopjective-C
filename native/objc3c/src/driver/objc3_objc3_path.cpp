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

namespace fs = std::filesystem;

int RunObjc3LanguagePath(const Objc3CliOptions &cli_options) {
  try {
    const std::string source = ReadText(cli_options.input);
    const Objc3FrontendOptions frontend_options = BuildObjc3FrontendOptions(cli_options);
    Objc3FrontendArtifactBundle artifacts = CompileObjc3SourceForCli(cli_options.input, source, frontend_options);
    WriteDiagnosticsArtifacts(cli_options.out_dir,
                              cli_options.emit_prefix,
                              artifacts.stage_diagnostics,
                              artifacts.post_pipeline_diagnostics);
    if (!artifacts.manifest_json.empty()) {
      // M251-C001 freeze: manifest emission remains the authoritative published
      // metadata-section ABI surface until later object-section emission lands.
      // M251-C002 scaffold: manifest emission mirrors the live runtime-metadata
      // section scaffold inventory so tooling can diff JSON against emitted
      // LLVM IR/object evidence without treating the manifest as the only source.
      // M251-C003 object inspection harness: manifest emission also publishes
      // the llvm-readobj/llvm-objdump matrix so object inspection remains tied
      // to canonical emitted artifacts instead of ad hoc operator commands.
      // M251-D001 freeze: manifest emission also publishes the reserved native
      // runtime support-library surface (target/header/entrypoints/link mode)
      // so D002/D003 must preserve one canonical runtime-library contract while
      // the deterministic test shim remains non-canonical evidence only.
      // M251-D002 core feature: manifest emission also publishes the live
      // native runtime-library skeleton/build contract so the real in-tree
      // archive/header/source/probe surface stays synchronized with emitted IR
      // evidence while driver link wiring remains deferred to M251-D003.
      // M251-D003 link wiring: manifest emission remains the canonical
      // runtime-archive handoff for external executable link steps, even while
      // this driver tranche still stops at deterministic object emission.
      WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);
    }
    if (!artifacts.runtime_metadata_binary.empty()) {
      WriteRuntimeMetadataBinaryArtifact(cli_options.out_dir,
                                        cli_options.emit_prefix,
                                        artifacts.runtime_metadata_binary);
    }
    if (!artifacts.diagnostics.empty()) {
      return 1;
    }

    // M251-A003 expands the handoff so manifest projection survives fail-closed
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
      std::cerr << "toolchain/runtime readiness scaffold fail-closed: "
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
        // M254-A001 translation-unit registration surface freeze: the native
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
        } else {
          Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
              manifest_inputs;
          const auto &registration_manifest_summary =
              artifacts.runtime_translation_unit_registration_manifest_summary;
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
          // M254-D001 runtime-bootstrap-api anchor: driver-side manifest
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
          // M254-C001 bootstrap-lowering anchor: driver-side artifact
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
            // M254-A002 registration-manifest anchor: the driver now writes one
            // canonical startup-registration manifest per translation unit so
            // later lowering/bootstrap lanes consume direct constructor-root
            // ownership inputs instead of reconstructing them from ad hoc
            // sidecar parsing rules.
            // M254-C002 constructor/init-stub emission anchor: the same
            // manifest now publishes the exact derived registration-table
            // symbol alongside the init-stub symbol so emitted IR/object
            // bootstrap artifacts can be checked against one authoritative
            // manifest packet.
            // M254-C003 registration-table/image-local-init anchor: the
            // manifest also publishes the self-describing registration-table
            // layout model, ABI/version counts, and the exact derived
            // image-local init-state symbol so later runtime image-walk work
            // consumes one canonical lowering-owned boundary.
            // M254-B001 bootstrap-invariant anchor: later startup execution
            // must continue to treat this manifest as the authoritative source
            // for one constructor root per translation-unit identity, fail
            // duplicate registration closed, and preserve deterministic
            // realization order before user entry.
            // M254-B002 bootstrap-semantics anchor: the manifest now also
            // carries the live runtime duplicate/order/failure contract and
            // status-code model consumed by the runtime library probe.
            WriteRuntimeRegistrationManifestArtifact(
                cli_options.out_dir,
                cli_options.emit_prefix,
                registration_manifest_json);
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
