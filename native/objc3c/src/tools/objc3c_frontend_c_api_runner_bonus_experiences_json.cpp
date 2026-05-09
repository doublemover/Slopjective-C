#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json.h"

#include <filesystem>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"

namespace fs = std::filesystem;

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerBonusExperiencesJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text,
    const std::string &runtime_metadata_binary_path_text) {
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
  const bool compile_surface_ready = result.emit.attempted != 0;
  const bool runtime_inspector_ready =
      FrontendCApiRunnerPathExists(object_path_text) &&
      FrontendCApiRunnerPathExists(runtime_metadata_binary_path_text);
  const bool showcase_surface_ready =
      fs::exists(fs::path("showcase") / "portfolio.json") &&
      fs::exists(fs::path("showcase") / "tutorial_walkthrough.json");
  const bool tutorial_surface_ready =
      fs::exists(fs::path("docs") / "tutorials" / "build_run_verify.md") &&
      fs::exists(fs::path("docs") / "tutorials" / "guided_walkthrough.md");

  out << "{\n";
  out << child_indent << "\"contract_id\": "
      << "\"objc3c.bonus.experiences.boundary.v1\",\n";
  out << child_indent << "\"product_boundary_model\": "
      << "\"public-runner compile inspect trace showcase and tutorial flows "
         "define the current bonus experience product boundary\",\n";
  out << child_indent << "\"runtime_boundary_model\": "
      << "\"frontend-c-api summary artifacts and runtime inspection ABI "
         "snapshots define the live runtime boundary for bonus experiences\",\n";
  out << child_indent << "\"fail_closed_model\": "
      << "\"no sidecar playground service visual shell or template catalog is "
         "authoritative until it is implemented on the live public runner and "
         "checked-in example roots\",\n";
  out << child_indent << "\"playground\": {\n";
  out << grandchild_indent << "\"available\": "
      << (compile_surface_ready ? "true" : "false") << ",\n";
  out << grandchild_indent << "\"source_path\": \""
      << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << grandchild_indent << "\"summary_path\": \""
      << EscapeJsonString(summary_path_text) << "\",\n";
  out << grandchild_indent << "\"artifact_roots\": [\n";
  out << grandchild_indent << "  \"tmp/artifacts/playground\",\n";
  out << grandchild_indent << "  \"tmp/reports/playground\",\n";
  out << grandchild_indent << "  \"tmp/artifacts/showcase\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"public_actions\": [\n";
  out << grandchild_indent << "  \"materialize-playground-workspace\",\n";
  out << grandchild_indent << "  \"compile-objc3c\",\n";
  out << grandchild_indent << "  \"inspect-playground-repro\",\n";
  out << grandchild_indent << "  \"inspect-compile-observability\",\n";
  out << grandchild_indent << "  \"trace-compile-stages\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "  \"summary\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(summary_path_text))
      << "\",\n";
  out << grandchild_indent << "  \"diagnostics\": \""
      << EscapeJsonString(
             BuildFrontendCApiRunnerReadCommand(diagnostics_path_text))
      << "\",\n";
  out << grandchild_indent << "  \"manifest\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(manifest_path_text))
      << "\",\n";
  out << grandchild_indent << "  \"repro_runner\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReproCommand(
             options,
             fs::path(summary_path_text),
             true))
      << "\"\n";
  out << grandchild_indent << "}\n";
  out << child_indent << "},\n";
  out << child_indent
      << "\"runtime_inspector_and_capability_explorer\": {\n";
  out << grandchild_indent << "\"available\": "
      << (runtime_inspector_ready ? "true" : "false") << ",\n";
  out << grandchild_indent << "\"object_path\": \""
      << EscapeJsonString(object_path_text) << "\",\n";
  out << grandchild_indent << "\"runtime_metadata_binary_path\": \""
      << EscapeJsonString(runtime_metadata_binary_path_text) << "\",\n";
  out << grandchild_indent << "\"public_actions\": [\n";
  out << grandchild_indent << "  \"inspect-runtime-inspector\",\n";
  out << grandchild_indent << "  \"inspect-capability-explorer\",\n";
  out << grandchild_indent << "  \"benchmark-runtime-inspector\",\n";
  out << grandchild_indent << "  \"trace-compile-stages\",\n";
  out << grandchild_indent << "  \"validate-developer-tooling\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"capability_probe_action\": "
      << "\"npm run objc3c -- inspect-capability-explorer\",\n";
  out << grandchild_indent << "\"capability_summary_report_path\": "
      << "\"tmp/reports/objc3c-public-workflow/capability-explorer.json\",\n";
  out << grandchild_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "  \"ir\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(ir_path_text))
      << "\",\n";
  out << grandchild_indent << "  \"object\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(object_path_text))
      << "\"\n";
  out << grandchild_indent << "}\n";
  out << child_indent << "},\n";
  out << child_indent << "\"template_and_demo_harness\": {\n";
  out << grandchild_indent << "\"available\": "
      << ((showcase_surface_ready && tutorial_surface_ready) ? "true"
                                                             : "false")
      << ",\n";
  out << grandchild_indent << "\"source_roots\": [\n";
  out << grandchild_indent << "  \"showcase/portfolio.json\",\n";
  out << grandchild_indent << "  \"showcase/tutorial_walkthrough.json\",\n";
  out << grandchild_indent << "  \"docs/tutorials/build_run_verify.md\",\n";
  out << grandchild_indent << "  \"docs/tutorials/guided_walkthrough.md\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"public_actions\": [\n";
  out << grandchild_indent << "  \"validate-showcase\",\n";
  out << grandchild_indent << "  \"validate-runnable-showcase\",\n";
  out << grandchild_indent << "  \"validate-getting-started\"\n";
  out << grandchild_indent << "]\n";
  out << child_indent << "}\n";
  out << indent << "}";
}
