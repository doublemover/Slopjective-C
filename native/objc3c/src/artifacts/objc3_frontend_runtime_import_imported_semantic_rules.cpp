#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <algorithm>
#include <sstream>
#include <vector>

#include "io/json/json_writer.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {

using objc3::io::json::JsonObjectWriter;

#include "artifacts/objc3_frontend_runtime_import_imported_semantic_rules_replay_key.inc"
#include "artifacts/objc3_frontend_runtime_import_imported_semantic_rules_source_collection.inc"
#include "artifacts/objc3_frontend_runtime_import_imported_semantic_rules_type_preservation.inc"
#include "artifacts/objc3_frontend_runtime_import_imported_semantic_rules_readiness.inc"

Objc3ImportedRuntimeMetadataSemanticRulesSummary
BuildImportedRuntimeMetadataSemanticRulesSummary(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
        &source_semantic_preservation,
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces,
    const std::size_t imported_input_path_count) {
  Objc3ImportedRuntimeMetadataSemanticRulesSummary summary;
  summary.fail_closed = true;
  summary.semantic_surface_published = true;
  summary.imported_input_path_count = imported_input_path_count;
  summary.imported_runtime_surface_inputs_present = imported_input_path_count > 0u;
  summary.imported_runtime_surface_inputs_loaded = true;
  summary.source_semantic_preservation_contract_ready =
      IsReadyObjc3CrossModuleRuntimeMetadataSemanticPreservationSummary(
          source_semantic_preservation);
  summary.imported_module_count = imported_surfaces.size();
  summary.imported_module_names_lexicographic.reserve(imported_surfaces.size());

  for (const auto &surface : imported_surfaces) {
    AccumulateImportedRuntimeMetadataSourceRules(surface, summary);
    AccumulateImportedRuntimeTypeSystemPreservationRules(surface, summary);
  }

  FinalizeImportedRuntimeMetadataSemanticRuleReadiness(summary);
  return summary;
}

#include "artifacts/objc3_frontend_runtime_import_imported_semantic_rules_json.inc"

}  // namespace objc3::artifacts::frontend
