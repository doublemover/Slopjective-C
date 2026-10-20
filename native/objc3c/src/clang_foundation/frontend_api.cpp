#include "clang_foundation/frontend_api.h"

#include "clang_foundation/compile_session.h"
#include <algorithm>
#include <memory>

struct objc3c_frontend_v2_result {
  objc3::clang_foundation::CompileResult value;
};

namespace {
int CompileFrontendOptions(const objc3c_frontend_v2_options *options,
                           const char *const *search_paths,
                           size_t search_path_count,
                           const objc3c_frontend_v3_import *imports,
                           size_t import_count, bool source_directed_imports,
                           const char *compiler_sdk_root,
                           const objc3c_frontend_v3_spi_grant *spi_grants,
                           size_t spi_grant_count,
                           objc3c_frontend_v2_result **out_result) {
  if (!out_result) return 2;
  *out_result = nullptr;
  if (!options || options->struct_size < sizeof(*options) ||
      options->abi_version != OBJC3C_FRONTEND_V2_ABI || !options->input_path ||
      !*options->input_path || options->language > 3 || options->emit > 2 ||
      options->profile > 3 || options->objc3 > 1 || options->arc > 1 ||
      options->strict_concurrency > 1 || options->emit_semantic_interface > 1 ||
      (!options->arguments && options->argument_count) ||
      (!options->source && options->source_size) ||
      (!search_paths && search_path_count) || (!imports && import_count) ||
      (compiler_sdk_root && !*compiler_sdk_root) ||
      (!spi_grants && spi_grant_count) || spi_grant_count > 4096)
    return 2;
  const bool objc = options->language == 2 || options->language == 3;
  if (((options->objc3 || options->arc ||
        (options->objc_runtime && *options->objc_runtime)) && !objc) ||
      (!options->objc3 && options->profile != 0) ||
      (options->strict_concurrency && !options->objc3) ||
      (options->emit != 0 && (!options->output_path || !*options->output_path)) ||
      (options->emit_semantic_interface &&
       (!options->module_name || !*options->module_name ||
        !options->package_identity || !*options->package_identity)) ||
      (import_count && !search_path_count) ||
      (source_directed_imports && (!objc || !options->objc3 || !import_count)))
    return 2;
  try {
    using namespace objc3::clang_foundation;
    CompileRequest request;
    request.input_path = options->input_path;
    if (options->source) request.source_text = std::string(options->source, options->source_size);
    auto string = [](const char *value) { return value ? std::string(value) : std::string(); };
    request.working_directory = string(options->working_directory);
    request.compiler_sdk_root = string(compiler_sdk_root);
    request.output_path = string(options->output_path);
    request.target_triple = string(options->target_triple);
    request.objc_runtime = string(options->objc_runtime);
    request.language = static_cast<BaseLanguage>(options->language);
    request.emit = static_cast<EmitKind>(options->emit);
    request.profile = static_cast<SemanticProfile>(options->profile);
    request.objc3 = options->objc3 != 0;
    request.arc = options->arc != 0;
    request.strict_concurrency = options->strict_concurrency != 0;
    request.emit_semantic_interface = options->emit_semantic_interface != 0;
    request.source_directed_imports = source_directed_imports;
    request.module_name = string(options->module_name);
    request.package_identity = string(options->package_identity);
    for (size_t i = 0; i < options->argument_count; ++i) {
      if (!options->arguments[i]) return 2;
      request.arguments.emplace_back(options->arguments[i]);
    }
    for (size_t i = 0; i < search_path_count; ++i) {
      if (!search_paths[i] || !*search_paths[i]) return 2;
      request.interface_search_paths.emplace_back(search_paths[i]);
    }
    for (size_t i = 0; i < import_count; ++i) {
      const auto &item = imports[i];
      if (item.struct_size < sizeof(item) || !item.canonical_name ||
          !*item.canonical_name || !item.semantic_fingerprint ||
          !*item.semantic_fingerprint || !item.package_identity ||
          !*item.package_identity || item.public_import > 1 || item.reexported > 1)
        return 2;
      InterfaceModuleRequest module;
      module.canonical_name = item.canonical_name;
      module.locked_identity.canonical_name = item.canonical_name;
      module.locked_identity.semantic_fingerprint = item.semantic_fingerprint;
      module.locked_identity.package_identity = item.package_identity;
      module.public_import = item.public_import != 0;
      module.reexported = item.reexported != 0;
      request.module_imports.push_back(std::move(module));
    }
    for (size_t i = 0; i < spi_grant_count; ++i) {
      const auto &grant = spi_grants[i];
      if (!grant.canonical_name || !grant.group ||
          !IsValidSPIGroup(grant.group))
        return 2;
      auto found =
          std::find_if(request.module_imports.begin(),
                       request.module_imports.end(), [&](const auto &m) {
                         return m.canonical_name == grant.canonical_name;
                       });
      if (found == request.module_imports.end())
        return 2;
      found->spi_groups.emplace_back(grant.group);
    }
    auto result = std::make_unique<objc3c_frontend_v2_result>();
    result->value = Compile(request);
    int status = result->value.success ? 0 : 1;
    *out_result = result.release();
    return status;
  } catch (...) {
    return 3;
  }
}
} // namespace

extern "C" int objc3c_frontend_v2_compile(
    const objc3c_frontend_v2_options *options,
    objc3c_frontend_v2_result **out_result) {
  return CompileFrontendOptions(options, nullptr, 0, nullptr, 0, false, nullptr,
                                nullptr, 0, out_result);
}

extern "C" int objc3c_frontend_v3_compile(
    const objc3c_frontend_v3_options *options,
    objc3c_frontend_v2_result **out_result) {
  if (!out_result) return 2;
  *out_result = nullptr;
  constexpr size_t V3MinimumSize =
      offsetof(objc3c_frontend_v3_options, source_directed_imports);
  if (!options || options->struct_size < V3MinimumSize ||
      options->abi_version != OBJC3C_FRONTEND_V3_ABI)
    return 2;
  const bool source_directed_imports =
      options->struct_size >= V3MinimumSize + sizeof(options->source_directed_imports) &&
      options->source_directed_imports != 0;
  if (options->struct_size >= V3MinimumSize + sizeof(options->source_directed_imports) &&
      options->source_directed_imports > 1)
    return 2;
  constexpr size_t SDKOffset = offsetof(objc3c_frontend_v3_options, compiler_sdk_root);
  if (options->struct_size > SDKOffset &&
      options->struct_size < SDKOffset + sizeof(options->compiler_sdk_root))
    return 2;
  const char *sdk_root = options->struct_size >= SDKOffset + sizeof(options->compiler_sdk_root)
      ? options->compiler_sdk_root : nullptr;
  constexpr size_t SPIOffset = offsetof(objc3c_frontend_v3_options, spi_grants);
  if (options->struct_size > SPIOffset &&
      options->struct_size < sizeof(*options))
    return 2;
  const bool has_spi_tail = options->struct_size >= sizeof(*options);
  return CompileFrontendOptions(
      &options->base, options->interface_search_paths,
      options->interface_search_path_count, options->imports,
      options->import_count, source_directed_imports, sdk_root,
      has_spi_tail ? options->spi_grants : nullptr,
      has_spi_tail ? options->spi_grant_count : 0, out_result);
}

extern "C" void objc3c_frontend_v2_result_destroy(objc3c_frontend_v2_result *result) {
  delete result;
}
extern "C" int objc3c_frontend_v2_result_success(const objc3c_frontend_v2_result *result) {
  return result && result->value.success;
}
extern "C" const char *objc3c_frontend_v2_result_failure(const objc3c_frontend_v2_result *result) {
  return result ? result->value.failure.c_str() : nullptr;
}
extern "C" const char *objc3c_frontend_v2_result_identity(const objc3c_frontend_v2_result *result) {
  return result ? result->value.compiler_identity.c_str() : nullptr;
}
extern "C" size_t objc3c_frontend_v2_diagnostic_count(const objc3c_frontend_v2_result *result) {
  return result ? result->value.diagnostics.size() : 0;
}
extern "C" const char *objc3c_frontend_v2_diagnostic_message(
    const objc3c_frontend_v2_result *result, size_t index) {
  return result && index < result->value.diagnostics.size()
      ? result->value.diagnostics[index].message.c_str() : nullptr;
}
extern "C" unsigned objc3c_frontend_v2_diagnostic_id(
    const objc3c_frontend_v2_result *result, size_t index) {
  return result && index < result->value.diagnostics.size()
      ? result->value.diagnostics[index].id : 0;
}
extern "C" unsigned objc3c_frontend_v2_diagnostic_severity(
    const objc3c_frontend_v2_result *result, size_t index) {
  return result && index < result->value.diagnostics.size()
      ? result->value.diagnostics[index].severity : 0;
}

extern "C" objc3c_frontend_v2_location objc3c_frontend_v2_diagnostic_spelling(
    const objc3c_frontend_v2_result *result, size_t index) {
  if (!result || index >= result->value.diagnostics.size()) return {};
  const auto &location = result->value.diagnostics[index].spelling;
  return {location.file.c_str(), location.line, location.column};
}
extern "C" objc3c_frontend_v2_location objc3c_frontend_v2_diagnostic_expansion(
    const objc3c_frontend_v2_result *result, size_t index) {
  if (!result || index >= result->value.diagnostics.size()) return {};
  const auto &location = result->value.diagnostics[index].expansion;
  return {location.file.c_str(), location.line, location.column};
}
extern "C" const char *objc3c_frontend_v2_interface_source(const objc3c_frontend_v2_result *result) {
  return result && result->value.success && result->value.interface_artifact &&
      result->value.interface_artifact->success ? result->value.interface_artifact->source_bytes.c_str() : nullptr;
}
extern "C" size_t objc3c_frontend_v2_interface_source_size(const objc3c_frontend_v2_result *result) {
  return objc3c_frontend_v2_interface_source(result) ? result->value.interface_artifact->source_bytes.size() : 0;
}
extern "C" const char *objc3c_frontend_v2_interface_sidecar(const objc3c_frontend_v2_result *result) {
  return objc3c_frontend_v2_interface_source(result) ? result->value.interface_artifact->sidecar_json.c_str() : nullptr;
}
extern "C" size_t objc3c_frontend_v2_interface_sidecar_size(const objc3c_frontend_v2_result *result) {
  return objc3c_frontend_v2_interface_sidecar(result) ? result->value.interface_artifact->sidecar_json.size() : 0;
}

extern "C" const char *objc3c_frontend_v3_result_dependency_record(
    const objc3c_frontend_v2_result *result) {
  return result && result->value.success &&
      !result->value.semantic_dependency_record.empty()
      ? result->value.semantic_dependency_record.c_str() : nullptr;
}
