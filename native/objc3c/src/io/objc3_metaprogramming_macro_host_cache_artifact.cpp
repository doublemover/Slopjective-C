#include "io/objc3_process_internal.h"
#include "io/objc3_metaprogramming_macro_host_cache_document.h"
#include "io/objc3_runtime_artifact_contracts.h"

bool TryBuildObjc3MetaprogrammingMacroHostProcessCacheArtifact(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (!objc3c::io::ValidateMetaprogrammingMacroHostProcessCacheArtifactInputs(
          inputs, source_input_path, error)) {
    return false;
  }

  const std::filesystem::path host_executable =
      std::filesystem::path(inputs.host_executable_relative_path);
  if (!std::filesystem::exists(host_executable)) {
    error =
        "metaprogramming macro host process/cache host executable not found: " +
        host_executable.generic_string();
    return false;
  }

  const std::string cache_key = ComputeFnv1a64Hex(inputs.replay_key);
  const std::filesystem::path cache_root =
      std::filesystem::path(inputs.cache_root_relative_path);
  const std::filesystem::path cache_entry = cache_root / cache_key;
  const std::filesystem::path cache_summary_path =
      cache_entry / "module.host-summary.json";
  const std::filesystem::path cache_runtime_import_path =
      BuildRuntimeAwareImportModuleArtifactPath(cache_entry, "module");
  const std::filesystem::path cache_manifest_path =
      BuildManifestArtifactPath(cache_entry, "module");

  bool cache_hit = std::filesystem::exists(cache_summary_path) &&
                   std::filesystem::exists(cache_runtime_import_path) &&
                   std::filesystem::exists(cache_manifest_path);
  bool launch_attempted = false;
  int host_process_exit_code = 0;

  if (!cache_hit) {
    launch_attempted = true;
    std::error_code create_error;
    std::filesystem::create_directories(cache_entry, create_error);
    if (create_error) {
      error = "failed to create metaprogramming host/cache entry directory: " +
              cache_entry.generic_string() + ": " + create_error.message();
      return false;
    }
    const std::vector<std::string> args = {
        source_input_path.generic_string(),
        "--out-dir",
        cache_entry.generic_string(),
        "--emit-prefix",
        "module",
        "--summary-out",
        cache_summary_path.generic_string(),
        "--no-emit-ir",
        "--no-emit-object"};
    host_process_exit_code = RunProcess(host_executable.generic_string(), args);
    if (host_process_exit_code != 0) {
      error =
          "metaprogramming macro host process launch failed with exit code " +
          std::to_string(host_process_exit_code);
      return false;
    }
    cache_hit = std::filesystem::exists(cache_summary_path) &&
                std::filesystem::exists(cache_runtime_import_path) &&
                std::filesystem::exists(cache_manifest_path);
    if (!cache_hit) {
      error =
          "metaprogramming macro host process launch completed but cache artifacts are incomplete";
      return false;
    }
  }

  artifact_json =
      BuildObjc3MetaprogrammingMacroHostProcessCacheArtifactDocumentJson(
          inputs, source_input_path, cache_key, cache_entry, cache_summary_path,
          cache_runtime_import_path, cache_manifest_path, launch_attempted,
          cache_hit, host_process_exit_code);
  return true;
}
