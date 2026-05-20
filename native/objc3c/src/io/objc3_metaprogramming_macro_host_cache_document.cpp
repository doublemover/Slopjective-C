#include "io/objc3_metaprogramming_macro_host_cache_document.h"

#include <sstream>
#include <vector>

#include "io/objc3_process_internal.h"

namespace {

std::vector<std::string> ImportedRuntimeSurfacePathStrings(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs) {
  std::vector<std::string> paths;
  paths.reserve(inputs.imported_runtime_surface_paths.size());
  for (const auto &path : inputs.imported_runtime_surface_paths) {
    paths.push_back(path.generic_string());
  }
  return paths;
}

}  // namespace

std::string BuildObjc3MetaprogrammingMacroHostProcessCacheArtifactDocumentJson(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    const std::string &cache_key,
    const std::filesystem::path &cache_entry,
    const std::filesystem::path &cache_summary_path,
    const std::filesystem::path &cache_runtime_import_path,
    const std::filesystem::path &cache_manifest_path,
    const std::string &cache_key_material_digest,
    const std::string &cache_validation_state,
    bool launch_attempted,
    bool cache_hit,
    int host_process_exit_code) {
  std::ostringstream out;
  JsonObjectWriter artifact(out);
  artifact.StringField("contract_id", inputs.contract_id);
  artifact.StringField("source_contract_id", inputs.source_contract_id);
  artifact.StringField("surface_path", inputs.surface_path);
  artifact.StringField("import_artifact_member_name",
                       inputs.import_artifact_member_name);
  artifact.StringField("artifact", inputs.artifact_relative_path);
  artifact.StringField("host_executable_relative_path",
                       inputs.host_executable_relative_path);
  artifact.StringField("cache_root_relative_path",
                       inputs.cache_root_relative_path);
  artifact.StringField("cache_key", cache_key);
  artifact.StringField("cache_key_material_digest", cache_key_material_digest);
  artifact.StringField("cache_entry_relative_path",
                       cache_entry.generic_string());
  artifact.StringField("cache_summary_relative_path",
                       cache_summary_path.generic_string());
  artifact.StringField("cache_runtime_import_surface_relative_path",
                       cache_runtime_import_path.generic_string());
  artifact.StringField("cache_manifest_relative_path",
                       cache_manifest_path.generic_string());
  artifact.StringField("host_model", inputs.host_model);
  artifact.StringField("toolchain_model", inputs.toolchain_model);
  artifact.StringField("cache_model", inputs.cache_model);
  artifact.StringField("invalidation_model", inputs.invalidation_model);
  artifact.StringField("sandbox_policy_model", inputs.sandbox_policy_model);
  artifact.StringField("diagnostics_model", inputs.diagnostics_model);
  artifact.StringField("fail_closed_model", inputs.fail_closed_model);
  artifact.StringField("source_input_path", source_input_path.generic_string());
  artifact.StringArrayField("imported_runtime_surface_paths",
                            ImportedRuntimeSurfacePathStrings(inputs));
  artifact.StringField("runtime_dispatch_symbol",
                       inputs.runtime_dispatch_symbol);
  artifact.UnsignedField("max_message_send_args",
                         inputs.max_message_send_args);
  artifact.UnsignedField("bootstrap_registration_order_ordinal",
                         inputs.bootstrap_registration_order_ordinal);
  artifact.BoolField("allow_live_error_runtime_surface",
                     inputs.allow_live_error_runtime_surface);
  artifact.BoolField("cache_ready", true);
  artifact.BoolField("launch_attempted", launch_attempted);
  artifact.BoolField("cache_hit", cache_hit);
  artifact.BoolField("cache_summary_present",
                     std::filesystem::exists(cache_summary_path));
  artifact.BoolField("cache_runtime_import_surface_present",
                     std::filesystem::exists(cache_runtime_import_path));
  artifact.BoolField("cache_manifest_present",
                     std::filesystem::exists(cache_manifest_path));
  artifact.StringField("cache_materialization_state",
                       launch_attempted ? "materialized" : "cache-hit");
  artifact.StringField("cache_validation_state", cache_validation_state);
  artifact.BoolField("cache_artifact_contract_checked", true);
  artifact.BoolField("provenance_integrity_checked", true);
  artifact.BoolField("sandbox_policy_checked", true);
  artifact.BoolField("diagnostic_contract_checked", true);
  artifact.IntField("host_process_exit_code", host_process_exit_code);
  artifact.BoolField("deterministic", inputs.deterministic);
  artifact.StringField("replay_key", inputs.replay_key);
  return FinishJsonObject(artifact, out);
}
