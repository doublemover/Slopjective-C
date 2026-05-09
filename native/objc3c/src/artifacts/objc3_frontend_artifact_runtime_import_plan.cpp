#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"

#include <filesystem>
#include <unordered_set>
#include <utility>

#include "pipeline/objc3_runtime_import_surface.h"

namespace {

using objc3::artifacts::frontend::BuildCrossModuleBuildRuntimeOrchestrationSummary;
using objc3::artifacts::frontend::
    BuildCrossModuleRuntimeMetadataSemanticPreservationSummary;
using objc3::artifacts::frontend::BuildImportedRuntimeMetadataSemanticRulesSummary;
using objc3::artifacts::frontend::BuildModuleImportGraphLoweringContract;
using objc3::artifacts::frontend::
    BuildRuntimeAwareImportModuleFrontendClosureSummary;
using objc3::artifacts::frontend::
    BuildSerializedRuntimeMetadataArtifactReuseSummary;
using objc3::artifacts::frontend::
    BuildSerializedRuntimeMetadataImportLoweringSummary;
using objc3::artifacts::frontend::BuildSerializedRuntimeMetadataReuseRecordSet;
using objc3::artifacts::frontend::BuildSerializedRuntimeMetadataReusedModuleNames;
using objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure;

void AddFirstPostPipelineFailure(
    std::vector<Objc3FrontendArtifactPostPipelineFailure> &failures,
    bool &failure_recorded,
    const char *code,
    std::string message) {
  if (failure_recorded) {
    return;
  }
  Objc3FrontendArtifactPostPipelineFailure failure;
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
  failures.push_back(std::move(failure));
  failure_recorded = true;
}

void LoadImportedRuntimeModuleSurfaces(
    Objc3FrontendArtifactRuntimeImportPlan &plan,
    const Objc3FrontendOptions &options,
    bool &failure_recorded) {
  plan.imported_runtime_module_surfaces.reserve(
      options.imported_runtime_surface_paths.size());
  std::unordered_set<std::string> normalized_input_paths;
  std::unordered_set<std::string> imported_module_names;
  for (const auto &input_path_text : options.imported_runtime_surface_paths) {
    const std::filesystem::path raw_input_path(input_path_text);
    const std::filesystem::path absolute_input_path =
        std::filesystem::absolute(raw_input_path);
    const std::string normalized_input_path =
        absolute_input_path.lexically_normal().generic_string();
    if (!normalized_input_paths.insert(normalized_input_path).second) {
      AddFirstPostPipelineFailure(
          plan.post_pipeline_failures,
          failure_recorded,
          "O3S264",
          "imported runtime surface path was provided more than once: " +
              normalized_input_path);
      break;
    }
    Objc3ImportedRuntimeModuleSurface imported_surface;
    std::string import_surface_error;
    if (!TryLoadObjc3ImportedRuntimeModuleSurface(absolute_input_path,
                                                 imported_surface,
                                                 import_surface_error)) {
      AddFirstPostPipelineFailure(
          plan.post_pipeline_failures,
          failure_recorded,
          "O3S264",
          "imported runtime surface load failed: " + import_surface_error);
      break;
    }
    const std::string &module_name =
        imported_surface.frontend_closure_summary.module_name;
    if (!imported_module_names.insert(module_name).second) {
      AddFirstPostPipelineFailure(
          plan.post_pipeline_failures,
          failure_recorded,
          "O3S264",
          "imported runtime surface module name was provided more than once: " +
              module_name);
      break;
    }
    plan.imported_runtime_module_surfaces.push_back(std::move(imported_surface));
  }
}

}  // namespace

Objc3FrontendArtifactRuntimeImportPlan
BuildObjc3FrontendArtifactRuntimeImportPlan(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &runtime_translation_unit_registration_manifest,
    bool prior_post_pipeline_failure_present) {
  Objc3FrontendArtifactRuntimeImportPlan plan;
  bool failure_recorded = prior_post_pipeline_failure_present;
  plan.module_import_graph_lowering_contract =
      BuildModuleImportGraphLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ModuleImportGraphLoweringContract(
          plan.module_import_graph_lowering_contract)) {
    AddFirstPostPipelineFailure(
        plan.post_pipeline_failures,
        failure_recorded,
        "O3L300",
        "LLVM IR emission failed: invalid module import graph lowering "
        "contract");
  }
  plan.module_import_graph_lowering_replay_key =
      Objc3ModuleImportGraphLoweringReplayKey(
          plan.module_import_graph_lowering_contract);
  plan.runtime_aware_import_module_frontend_closure =
      BuildRuntimeAwareImportModuleFrontendClosureSummary(
          program,
          pipeline_result.parser_contract_snapshot,
          plan.module_import_graph_lowering_contract,
          runtime_metadata_source_records);
  plan.cross_module_runtime_metadata_semantic_preservation =
      BuildCrossModuleRuntimeMetadataSemanticPreservationSummary(
          plan.runtime_aware_import_module_frontend_closure,
          runtime_metadata_source_records);
  LoadImportedRuntimeModuleSurfaces(plan, options, failure_recorded);
  plan.imported_runtime_metadata_semantic_rules =
      BuildImportedRuntimeMetadataSemanticRulesSummary(
          plan.cross_module_runtime_metadata_semantic_preservation,
          plan.imported_runtime_module_surfaces,
          options.imported_runtime_surface_paths.size());
  plan.has_imported_runtime_surface_inputs =
      !options.imported_runtime_surface_paths.empty();
  if (!failure_recorded && plan.has_imported_runtime_surface_inputs &&
      !IsReadyObjc3ImportedRuntimeMetadataSemanticRulesSummary(
          plan.imported_runtime_metadata_semantic_rules)) {
    AddFirstPostPipelineFailure(
        plan.post_pipeline_failures,
        failure_recorded,
        "O3S264",
        "imported runtime metadata semantic rules are incomplete: " +
            plan.imported_runtime_metadata_semantic_rules.failure_reason);
  }
  plan.serialized_runtime_metadata_import_lowering =
      BuildSerializedRuntimeMetadataImportLoweringSummary(
          plan.imported_runtime_metadata_semantic_rules);
  if (!failure_recorded && plan.has_imported_runtime_surface_inputs &&
      !IsReadyObjc3SerializedRuntimeMetadataImportLoweringSummary(
          plan.serialized_runtime_metadata_import_lowering)) {
    AddFirstPostPipelineFailure(
        plan.post_pipeline_failures,
        failure_recorded,
        "O3S265",
        "serialized runtime metadata import/lowering boundary is incomplete: " +
            plan.serialized_runtime_metadata_import_lowering.failure_reason);
  }
  plan.serialized_runtime_metadata_reused_module_names =
      BuildSerializedRuntimeMetadataReusedModuleNames(
          plan.runtime_aware_import_module_frontend_closure.module_name,
          plan.imported_runtime_module_surfaces);
  plan.serialized_runtime_metadata_reuse_records =
      BuildSerializedRuntimeMetadataReuseRecordSet(
          runtime_metadata_source_records,
          plan.imported_runtime_module_surfaces);
  plan.serialized_runtime_metadata_artifact_reuse =
      BuildSerializedRuntimeMetadataArtifactReuseSummary(
          plan.serialized_runtime_metadata_import_lowering,
          plan.runtime_aware_import_module_frontend_closure.module_name,
          plan.serialized_runtime_metadata_reuse_records,
          plan.serialized_runtime_metadata_reused_module_names);
  if (!failure_recorded && plan.has_imported_runtime_surface_inputs &&
      !IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(
          plan.serialized_runtime_metadata_artifact_reuse)) {
    AddFirstPostPipelineFailure(
        plan.post_pipeline_failures,
        failure_recorded,
        "O3S266",
        "serialized runtime metadata artifact reuse is incomplete: " +
            plan.serialized_runtime_metadata_artifact_reuse.failure_reason);
  }
  plan.cross_module_build_runtime_orchestration =
      BuildCrossModuleBuildRuntimeOrchestrationSummary(
          plan.serialized_runtime_metadata_artifact_reuse,
          plan.imported_runtime_metadata_semantic_rules,
          runtime_translation_unit_registration_manifest,
          options.imported_runtime_surface_paths.size());
  if (!failure_recorded && plan.has_imported_runtime_surface_inputs &&
      !IsReadyObjc3CrossModuleBuildRuntimeOrchestrationSummary(
          plan.cross_module_build_runtime_orchestration)) {
    AddFirstPostPipelineFailure(
        plan.post_pipeline_failures,
        failure_recorded,
        "O3S267",
        "cross-module build/runtime orchestration contract is incomplete: " +
            plan.cross_module_build_runtime_orchestration.failure_reason);
  }
  return plan;
}
