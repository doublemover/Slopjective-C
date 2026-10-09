#include "clang_foundation/frontend_api.h"

#include <memory>
#include "clang_foundation/compile_session.h"

struct objc3c_frontend_v2_result {
  objc3::clang_foundation::CompileResult value;
};

extern "C" int objc3c_frontend_v2_compile(
    const objc3c_frontend_v2_options *options,
    objc3c_frontend_v2_result **out_result) {
  if (!out_result) return 2;
  *out_result = nullptr;
  if (!options || options->struct_size < sizeof(*options) ||
      options->abi_version != OBJC3C_FRONTEND_V2_ABI || !options->input_path ||
      options->language > 3 || options->emit > 2 || options->profile > 3 ||
      options->objc3 > 1 || options->arc > 1 ||
      options->strict_concurrency > 1 || options->emit_semantic_interface > 1 ||
      (!options->arguments && options->argument_count) ||
      (!options->source && options->source_size)) return 2;
  try {
    using namespace objc3::clang_foundation;
    CompileRequest request;
    request.input_path = options->input_path;
    if (options->source) request.source_text = std::string(options->source, options->source_size);
    auto string = [](const char *value) { return value ? std::string(value) : std::string(); };
    request.working_directory = string(options->working_directory);
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
    request.module_name = string(options->module_name);
    request.package_identity = string(options->package_identity);
    for (size_t i = 0; i < options->argument_count; ++i) {
      if (!options->arguments[i]) return 2;
      request.arguments.emplace_back(options->arguments[i]);
    }
    auto result = std::make_unique<objc3c_frontend_v2_result>();
    result->value = Compile(request);
    int status = result->value.success ? 0 : 1;
    *out_result = result.release();
    return status;
  } catch (...) {
    // A host C++ failure never becomes a source-language typed error.
    return 3;
  }
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
