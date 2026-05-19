#include "io/objc3_metaprogramming_macro_host_cache_document.h"

#include <sstream>

#include "io/objc3_process_internal.h"

std::string BuildObjc3MetaprogrammingMacroHostProcessCacheArtifactDocumentJson(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    const std::string &cache_key,
    const std::filesystem::path &cache_entry,
    const std::filesystem::path &cache_summary_path,
    const std::filesystem::path &cache_runtime_import_path,
    const std::filesystem::path &cache_manifest_path,
    bool launch_attempted,
    bool cache_hit,
    int host_process_exit_code) {
  std::ostringstream out;
  JsonObjectWriter artifact(out);
  artifact.StringField("contract_id", inputs.contract_id);
  artifact.StringField("source_contract_id", inputs.source_contract_id);
  artifact.StringField("surface_path", inputs.surface_path);
  artifact.StringField("artifact", inputs.artifact_relative_path);
  artifact.StringField("host_executable_relative_path",
                       inputs.host_executable_relative_path);
  artifact.StringField("cache_root_relative_path",
                       inputs.cache_root_relative_path);
  artifact.StringField("cache_key", cache_key);
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
  artifact.StringField("fail_closed_model", inputs.fail_closed_model);
  artifact.StringField("source_input_path", source_input_path.generic_string());
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
  artifact.IntField("host_process_exit_code", host_process_exit_code);
  artifact.BoolField("deterministic", inputs.deterministic);
  artifact.StringField("replay_key", inputs.replay_key);
  return FinishJsonObject(artifact, out);
}
