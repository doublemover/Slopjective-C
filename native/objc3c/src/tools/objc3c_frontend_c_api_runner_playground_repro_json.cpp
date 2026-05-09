#include "tools/objc3c_frontend_c_api_runner_playground_repro_json.h"

#include <sstream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"

namespace fs = std::filesystem;

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerPlaygroundReproJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text) {
  const char *backend_name =
      options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
          ? "llvm-direct"
          : "clang";
  const std::string diagnostics_path_text =
      FrontendCApiResultArtifactPath(result,
                                     OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  const std::string manifest_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_MANIFEST);
  const std::string ir_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_IR);
  const std::string object_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_OBJECT);
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";

  out << "{\n";
  out << child_indent << "\"contract_id\": "
      << "\"objc3c.playground.repro.surface.v1\",\n";
  out << child_indent << "\"available\": "
      << (result.emit.attempted != 0 ? "true" : "false") << ",\n";
  out << child_indent << "\"source_path\": \""
      << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << child_indent << "\"summary_path\": \""
      << EscapeJsonString(summary_path_text) << "\",\n";
  out << child_indent << "\"artifact_root\": "
      << "\"" << EscapeJsonString(options.out_dir.generic_string()) << "\",\n";
  out << child_indent << "\"artifact_paths\": {\n";
  out << grandchild_indent << "\"diagnostics\": \""
      << EscapeJsonString(diagnostics_path_text) << "\",\n";
  out << grandchild_indent << "\"manifest\": \""
      << EscapeJsonString(manifest_path_text) << "\",\n";
  out << grandchild_indent << "\"ir\": \"" << EscapeJsonString(ir_path_text)
      << "\",\n";
  out << grandchild_indent << "\"object\": \""
      << EscapeJsonString(object_path_text) << "\"\n";
  out << child_indent << "},\n";
  out << child_indent << "\"compile_profile\": {\n";
  out << grandchild_indent << "\"ir_object_backend\": \"" << backend_name
      << "\",\n";
  out << grandchild_indent << "\"max_message_send_args\": "
      << options.max_message_send_args << ",\n";
  out << grandchild_indent << "\"runtime_dispatch_symbol\": \""
      << EscapeJsonString(options.runtime_dispatch_symbol) << "\",\n";
  out << grandchild_indent
      << "\"translation_unit_registration_order_ordinal\": "
      << options.translation_unit_registration_order_ordinal << "\n";
  out << child_indent << "},\n";
  out << child_indent << "\"public_actions\": [\n";
  out << child_indent << "  \"materialize-playground-workspace\",\n";
  out << child_indent << "  \"compile-objc3c\",\n";
  out << child_indent << "  \"inspect-playground-repro\",\n";
  out << child_indent << "  \"inspect-compile-observability\",\n";
  out << child_indent << "  \"trace-compile-stages\"\n";
  out << child_indent << "],\n";
  out << child_indent << "\"showcase_examples\": [\n";
  out << child_indent << "  \"showcase/auroraBoard/main.objc3\",\n";
  out << child_indent << "  \"showcase/signalMesh/main.objc3\",\n";
  out << child_indent << "  \"showcase/patchKit/main.objc3\",\n";
  out << child_indent << "  \"tests/tooling/fixtures/native/hello.objc3\"\n";
  out << child_indent << "],\n";
  out << child_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "\"summary\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(summary_path_text))
      << "\",\n";
  out << grandchild_indent << "\"diagnostics\": \""
      << EscapeJsonString(
             BuildFrontendCApiRunnerReadCommand(diagnostics_path_text))
      << "\",\n";
  out << grandchild_indent << "\"manifest\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(manifest_path_text))
      << "\",\n";
  out << grandchild_indent << "\"repro_runner\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReproCommand(
             options,
             fs::path(summary_path_text),
             true))
      << "\"\n";
  out << child_indent << "}\n";
  out << indent << "}";
}

std::string BuildFrontendCApiRunnerPlaygroundReproJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::filesystem::path &summary_path) {
  std::ostringstream dump;
  WriteFrontendCApiRunnerPlaygroundReproJson(
      dump,
      "",
      options,
      result,
      summary_path.generic_string());
  dump << "\n";
  return dump.str();
}
