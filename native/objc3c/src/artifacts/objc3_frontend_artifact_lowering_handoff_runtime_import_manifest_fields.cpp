#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_field_modules.h"

#include <string>

#include "artifacts/objc3_frontend_artifact_runtime_import_plan.h"
#include "lower/contracts/cross_module_lowering_contracts.h"

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffRuntimeImportManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactRuntimeImportPlan &runtime_import_plan) {
  const Objc3ModuleImportGraphLoweringContract
      &module_import_graph_lowering_contract =
          runtime_import_plan.module_import_graph_lowering_contract;
  const std::string &module_import_graph_lowering_replay_key =
      runtime_import_plan.module_import_graph_lowering_replay_key;

  manifest << ",\"deterministic_module_import_graph_lowering_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << ",\"module_import_graph_lowering_sites\":"
           << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"module_import_graph_lowering_import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"module_import_graph_lowering_namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"module_import_graph_lowering_object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"module_import_graph_lowering_pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"module_import_graph_lowering_normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"module_import_graph_lowering_contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"lowering_module_import_graph_replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\"";
}

}  // namespace objc3::artifacts::frontend
