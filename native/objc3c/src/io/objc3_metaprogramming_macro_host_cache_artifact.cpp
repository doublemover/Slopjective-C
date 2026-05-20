#include "io/objc3_process_internal.h"
#include "io/objc3_metaprogramming_macro_host_cache_document.h"
#include "io/objc3_file_io.h"
#include "io/objc3_runtime_artifact_contracts.h"

namespace {

std::string BuildMetaprogrammingMacroHostCacheKeyMaterial(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs) {
  std::ostringstream out;
  out << "objc3c-metaprogramming-macro-host-cache-v2"
      << "\ncontract_id=" << inputs.contract_id
      << "\nsource_contract_id=" << inputs.source_contract_id
      << "\nsurface_path=" << inputs.surface_path
      << "\nimport_artifact_member_name=" << inputs.import_artifact_member_name
      << "\nhost_executable_relative_path="
      << inputs.host_executable_relative_path
      << "\ncache_root_relative_path=" << inputs.cache_root_relative_path
      << "\ncache_model=" << inputs.cache_model
      << "\ninvalidation_model=" << inputs.invalidation_model
      << "\nsandbox_policy_model=" << inputs.sandbox_policy_model
      << "\ndiagnostics_model=" << inputs.diagnostics_model
      << "\nruntime_dispatch_symbol=" << inputs.runtime_dispatch_symbol
      << "\nmax_message_send_args=" << inputs.max_message_send_args
      << "\nbootstrap_registration_order_ordinal="
      << inputs.bootstrap_registration_order_ordinal
      << "\nallow_live_error_runtime_surface="
      << (inputs.allow_live_error_runtime_surface ? "true" : "false");
  for (const auto &path : inputs.imported_runtime_surface_paths) {
    out << "\nimported_runtime_surface_path=" << path.generic_string();
  }
  out << "\nreplay_key=" << inputs.replay_key;
  return out.str();
}

bool TryLoadJsonObjectArtifact(const std::filesystem::path &path,
                               const std::string &label,
                               JsonValue &value,
                               std::string &error) {
  if (!std::filesystem::exists(path)) {
    error = label + " missing: " + path.generic_string();
    return false;
  }
  if (!TryParseJsonObjectText(ReadText(path), label, value, error)) {
    error = label + " failed cache validation at " + path.generic_string() +
            ": " + error;
    return false;
  }
  return true;
}

bool TryValidateNonEmptyCacheArtifact(const std::filesystem::path &path,
                                      const std::string &label,
                                      std::string &error) {
  std::error_code size_error;
  const std::uintmax_t byte_size = std::filesystem::file_size(path, size_error);
  if (size_error || byte_size == 0) {
    error = label + " is missing or empty: " + path.generic_string();
    return false;
  }
  return true;
}

bool JsonFieldEquals(const JsonValue &object,
                     std::string_view field,
                     const std::string &expected) {
  const std::optional<std::string> actual = object.GetString(field);
  return actual.has_value() && *actual == expected;
}

bool JsonFieldTrue(const JsonValue &object, std::string_view field) {
  const std::optional<bool> actual = object.GetBool(field);
  return actual.has_value() && *actual;
}

bool TryValidateRuntimeImportSurface(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const JsonValue &runtime_import_root,
    std::string &error) {
  const JsonValue *surface =
      runtime_import_root.Find(inputs.import_artifact_member_name);
  if (surface == nullptr || !surface->IsObject()) {
    error =
        "metaprogramming host/cache runtime import surface is missing or not an object";
    return false;
  }
  if (!JsonFieldEquals(*surface, "contract_id", inputs.contract_id) ||
      !JsonFieldEquals(*surface, "source_contract_id",
                       inputs.source_contract_id) ||
      !JsonFieldEquals(*surface, "host_executable_relative_path",
                       inputs.host_executable_relative_path) ||
      !JsonFieldEquals(*surface, "cache_root_relative_path",
                       inputs.cache_root_relative_path) ||
      !JsonFieldEquals(*surface, "invalidation_model",
                       inputs.invalidation_model) ||
      !JsonFieldEquals(*surface, "sandbox_policy_model",
                       inputs.sandbox_policy_model) ||
      !JsonFieldEquals(*surface, "diagnostics_model",
                       inputs.diagnostics_model) ||
      !JsonFieldEquals(*surface, "replay_key", inputs.replay_key) ||
      !JsonFieldTrue(*surface, "runtime_import_artifact_ready") ||
      !JsonFieldTrue(*surface, "separate_compilation_ready") ||
      !JsonFieldTrue(*surface, "deterministic")) {
    error =
        "metaprogramming host/cache runtime import surface drifted from cache inputs";
    return false;
  }
  return true;
}

bool TryValidateMetaprogrammingHostCacheEntry(
    const Objc3MetaprogrammingMacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &cache_summary_path,
    const std::filesystem::path &cache_runtime_import_path,
    const std::filesystem::path &cache_manifest_path,
    std::string &error) {
  JsonValue summary_root;
  JsonValue runtime_import_root;
  if (!TryLoadJsonObjectArtifact(cache_summary_path,
                                 "metaprogramming host cache summary",
                                 summary_root, error) ||
      !TryLoadJsonObjectArtifact(cache_runtime_import_path,
                                 "metaprogramming host cache runtime import",
                                 runtime_import_root, error) ||
      !TryValidateNonEmptyCacheArtifact(cache_manifest_path,
                                        "metaprogramming host cache manifest",
                                        error)) {
    return false;
  }
  if (!TryValidateRuntimeImportSurface(inputs, runtime_import_root, error)) {
    return false;
  }
  return true;
}

}  // namespace

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

  const std::string cache_key_material =
      BuildMetaprogrammingMacroHostCacheKeyMaterial(inputs);
  const std::string cache_key = ComputeFnv1a64Hex(cache_key_material);
  const std::string cache_key_material_digest =
      ComputeSha256ShapedContentDigest(cache_key_material);
  const std::filesystem::path cache_root =
      std::filesystem::path(inputs.cache_root_relative_path);
  const std::filesystem::path cache_entry = cache_root / cache_key;
  const std::filesystem::path cache_summary_path =
      cache_entry / "module.host-summary.json";
  const std::filesystem::path cache_runtime_import_path =
      BuildRuntimeAwareImportModuleArtifactPath(cache_entry, "module");
  const std::filesystem::path cache_manifest_path =
      BuildManifestArtifactPath(cache_entry, "module");

  const bool cache_summary_present = std::filesystem::exists(cache_summary_path);
  const bool cache_runtime_import_present =
      std::filesystem::exists(cache_runtime_import_path);
  const bool cache_manifest_present = std::filesystem::exists(cache_manifest_path);
  const bool any_cache_artifact_present =
      cache_summary_present || cache_runtime_import_present ||
      cache_manifest_present;
  const bool complete_cache_entry =
      cache_summary_present && cache_runtime_import_present &&
      cache_manifest_present;
  bool cache_hit = false;
  std::string cache_validation_state = "cache-miss";
  if (any_cache_artifact_present && !complete_cache_entry) {
    error =
        "metaprogramming macro host process/cache entry is incomplete and must fail closed: " +
        cache_entry.generic_string();
    return false;
  }
  if (complete_cache_entry) {
    if (!TryValidateMetaprogrammingHostCacheEntry(
            inputs, cache_summary_path, cache_runtime_import_path,
            cache_manifest_path, error)) {
      return false;
    }
    cache_hit = true;
    cache_validation_state = "validated-existing-cache-hit";
  }
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
        "--objc3-bootstrap-registration-order-ordinal",
        std::to_string(inputs.bootstrap_registration_order_ordinal),
        "--objc3-max-message-args",
        std::to_string(inputs.max_message_send_args),
        "--objc3-runtime-dispatch-symbol",
        inputs.runtime_dispatch_symbol,
        "--objc3-metaprogramming-cache-root",
        inputs.cache_root_relative_path,
        "--no-emit-ir",
        "--no-emit-object"};
    std::vector<std::string> host_args = args;
    for (const auto &path : inputs.imported_runtime_surface_paths) {
      host_args.push_back("--objc3-import-runtime-surface");
      host_args.push_back(path.generic_string());
    }
    if (inputs.allow_live_error_runtime_surface) {
      host_args.push_back("--objc3-enable-live-error-runtime-surface");
    }
    host_process_exit_code =
        RunProcess(host_executable.generic_string(), host_args);
    if (host_process_exit_code != 0) {
      error =
          "metaprogramming macro host process launch failed with exit code " +
          std::to_string(host_process_exit_code);
      return false;
    }
    if (!std::filesystem::exists(cache_summary_path) ||
        !std::filesystem::exists(cache_runtime_import_path) ||
        !std::filesystem::exists(cache_manifest_path)) {
      error =
          "metaprogramming macro host process launch completed but cache artifacts are incomplete";
      return false;
    }
    if (!TryValidateMetaprogrammingHostCacheEntry(
            inputs, cache_summary_path, cache_runtime_import_path,
            cache_manifest_path, error)) {
      return false;
    }
    cache_hit = true;
    cache_validation_state = "validated-after-materialization";
  }

  artifact_json =
      BuildObjc3MetaprogrammingMacroHostProcessCacheArtifactDocumentJson(
          inputs, source_input_path, cache_key, cache_entry, cache_summary_path,
          cache_runtime_import_path, cache_manifest_path,
          cache_key_material_digest, cache_validation_state, launch_attempted,
          cache_hit, host_process_exit_code);
  return true;
}
