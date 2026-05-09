#include "tools/objc3c_frontend_c_api_runner_summary_json.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "io/objc3_cli_reporting_output_contract_scaffold.h"
#include "io/objc3_json.h"
#include "io/objc3_manifest_artifacts.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"

namespace fs = std::filesystem;

namespace {

using objc3::io::EscapeJsonString;

constexpr const char *kObjc3RuntimeArcDebugStateSnapshotSymbol =
    "objc3_runtime_copy_arc_debug_state_for_testing";

void WriteObservabilityJson(
    std::ostringstream &out,
    const std::string &indent,
    const std::string &summary_path_text,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
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
  const FrontendCApiDiagnosticTotals diagnostic_totals =
      BuildFrontendCApiDiagnosticTotals(result);
  const std::string last_attempted_stage =
      LastAttemptedFrontendCApiStageName(result);
  const std::string blocking_stage = BlockingFrontendCApiStageName(result);
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";
  out << "{\n";
  out << child_indent << "\"status_name\": \"" << FrontendCApiStatusName(status)
      << "\",\n";
  out << child_indent << "\"last_attempted_stage\": \""
      << EscapeJsonString(last_attempted_stage) << "\",\n";
  out << child_indent << "\"blocking_stage\": \""
      << EscapeJsonString(blocking_stage) << "\",\n";
  out << child_indent << "\"highest_diagnostic_severity\": \""
      << HighestFrontendCApiDiagnosticSeverity(diagnostic_totals) << "\",\n";
  out << child_indent << "\"result_error_message_present\": "
      << (!result_error_message.empty() ? "true" : "false") << ",\n";
  out << child_indent << "\"diagnostics_total\": " << diagnostic_totals.total
      << ",\n";
  out << child_indent << "\"diagnostics_notes\": " << diagnostic_totals.notes
      << ",\n";
  out << child_indent << "\"diagnostics_warnings\": "
      << diagnostic_totals.warnings << ",\n";
  out << child_indent << "\"diagnostics_errors\": " << diagnostic_totals.errors
      << ",\n";
  out << child_indent << "\"diagnostics_fatals\": " << diagnostic_totals.fatals
      << ",\n";
  out << child_indent << "\"artifact_presence\": {\n";
  out << grandchild_indent << "\"summary\": true,\n";
  out << grandchild_indent << "\"diagnostics\": "
      << (FrontendCApiRunnerPathExists(diagnostics_path_text) ? "true"
                                                              : "false")
      << ",\n";
  out << grandchild_indent << "\"manifest\": "
      << (FrontendCApiRunnerPathExists(manifest_path_text) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"ir\": "
      << (FrontendCApiRunnerPathExists(ir_path_text) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"object\": "
      << (FrontendCApiRunnerPathExists(object_path_text) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"runtime_metadata_binary\": "
      << (!runtime_metadata_binary_path_text.empty() ? "true" : "false")
      << "\n";
  out << child_indent << "},\n";
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
  out << grandchild_indent << "\"ir\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(ir_path_text))
      << "\",\n";
  out << grandchild_indent << "\"object\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(object_path_text))
      << "\"\n";
  out << child_indent << "}\n";
  out << indent << "}";
}

void WriteRuntimeInspectorJson(
    std::ostringstream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  const std::string object_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_OBJECT);
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";
  const bool available = FrontendCApiRunnerPathExists(object_path_text);
  const std::string availability_reason =
      available ? std::string() : "object artifact missing or not emitted";
  out << "{\n";
  out << child_indent << "\"contract_id\": \""
      << kObjc3RuntimeMetadataObjectInspectionContractId << "\",\n";
  out << child_indent << "\"publication_contract_id\": \""
      << kObjc3RuntimeMetadataSectionPublicationContractId << "\",\n";
  out << child_indent << "\"available\": " << (available ? "true" : "false")
      << ",\n";
  out << child_indent << "\"active_emit_prefix\": \""
      << EscapeJsonString(options.emit_prefix) << "\",\n";
  out << child_indent << "\"fixture_path\": \""
      << EscapeJsonString(kObjc3RuntimeMetadataObjectInspectionFixturePath)
      << "\",\n";
  out << child_indent << "\"object_path\": \""
      << EscapeJsonString(object_path_text) << "\",\n";
  out << child_indent << "\"section_inventory_row_key\": \""
      << EscapeJsonString(
             kObjc3RuntimeMetadataObjectInspectionSectionInventoryRowKey)
      << "\",\n";
  out << child_indent << "\"symbol_inventory_row_key\": \""
      << EscapeJsonString(
             kObjc3RuntimeMetadataObjectInspectionSymbolInventoryRowKey)
      << "\",\n";
  out << child_indent << "\"section_inventory_command\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSectionCommand,
             object_path_text))
      << "\",\n";
  out << child_indent << "\"symbol_inventory_command\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSymbolCommand,
             object_path_text))
      << "\",\n";
  out << child_indent << "\"arc_debug_state_snapshot_symbol\": \""
      << kObjc3RuntimeArcDebugStateSnapshotSymbol << "\",\n";
  out << child_indent << "\"runtime_abi_boundary_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel)
      << "\",\n";
  out << child_indent << "\"block_runtime_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiBlockModel)
      << "\",\n";
  out << child_indent << "\"arc_runtime_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiArcModel) << "\",\n";
  out << child_indent << "\"fail_closed_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiFailClosedModel)
      << "\",\n";
  out << child_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "\"object_sections\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSectionCommand,
             object_path_text))
      << "\",\n";
  out << grandchild_indent << "\"object_symbols\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSymbolCommand,
             object_path_text))
      << "\"\n";
  out << child_indent << "},\n";
  out << child_indent << "\"availability_reason\": \""
      << EscapeJsonString(availability_reason) << "\"\n";
  out << indent << "}";
}

void WriteBonusExperiencesJson(
    std::ostringstream &out,
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

void WritePlaygroundReproJson(
    std::ostringstream &out,
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

void WriteStageSummaryJson(std::ostringstream &out,
                           const char *name,
                           const objc3c_frontend_c_stage_summary_t &summary,
                           bool trailing_comma) {
  out << "    \"" << name << "\": {\n";
  out << "      \"stage\": " << static_cast<unsigned>(summary.stage) << ",\n";
  out << "      \"attempted\": "
      << (summary.attempted != 0 ? "true" : "false") << ",\n";
  out << "      \"skipped\": "
      << (summary.skipped != 0 ? "true" : "false") << ",\n";
  out << "      \"diagnostics_total\": " << summary.diagnostics_total << ",\n";
  out << "      \"diagnostics_notes\": " << summary.diagnostics_notes << ",\n";
  out << "      \"diagnostics_warnings\": " << summary.diagnostics_warnings
      << ",\n";
  out << "      \"diagnostics_errors\": " << summary.diagnostics_errors
      << ",\n";
  out << "      \"diagnostics_fatals\": " << summary.diagnostics_fatals << "\n";
  out << "    }";
  if (trailing_comma) {
    out << ",";
  }
  out << "\n";
}

}  // namespace

std::string BuildFrontendCApiRunnerStageTraceJson(
    const objc3c_frontend_c_compile_result_t &result) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"mode\": \"objc3c-frontend-stage-trace-v1\",\n";
  out << "  \"semantic_skipped\": "
      << (result.semantic_skipped != 0 ? "true" : "false") << ",\n";
  out << "  \"process_exit_code\": " << result.process_exit_code << ",\n";
  out << "  \"stages\": {\n";
  WriteStageSummaryJson(out, "lex", result.lex, true);
  WriteStageSummaryJson(out, "parse", result.parse, true);
  WriteStageSummaryJson(out, "sema", result.sema, true);
  WriteStageSummaryJson(out, "lower", result.lower, true);
  WriteStageSummaryJson(out, "emit", result.emit, false);
  out << "  }\n";
  out << "}\n";
  return out.str();
}

std::string BuildFrontendCApiRunnerSummaryJson(
    const FrontendCApiRunnerOptions &options,
    const fs::path &summary_path,
    objc3c_frontend_c_status_t status,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &last_error,
    const std::string &result_error_message,
    const FrontendCApiRunnerOutputContract &output_contract) {
  const auto &matrix = output_contract.conformance_matrix_surface;
  const auto &corpus = output_contract.conformance_corpus_surface;
  const char *backend_name =
      options.ir_object_backend == OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT
          ? "llvm-direct"
          : "clang";
  const std::string summary_path_text = summary_path.generic_string();
  const std::string runtime_metadata_binary_path_text =
      FrontendCApiResultArtifactPath(
          result,
          OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA);
  const std::string diagnostics_path_text =
      FrontendCApiResultArtifactPath(result,
                                     OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS);
  const std::string manifest_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_MANIFEST);
  const std::string ir_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_IR);
  const std::string object_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_OBJECT);
  std::ostringstream out;
  out << "{\n";
  out << "  \"mode\": \"objc3c-frontend-c-api-runner-v1\",\n";
  out << "  \"input_path\": \""
      << EscapeJsonString(options.input_path.generic_string()) << "\",\n";
  out << "  \"out_dir\": \"" << EscapeJsonString(options.out_dir.generic_string())
      << "\",\n";
  out << "  \"emit_prefix\": \"" << EscapeJsonString(options.emit_prefix)
      << "\",\n";
  out << "  \"ir_object_backend\": \"" << backend_name << "\",\n";
  out << "  \"status\": " << static_cast<unsigned>(status) << ",\n";
  out << "  \"process_exit_code\": " << result.process_exit_code << ",\n";
  out << "  \"success\": " << (result.success != 0 ? "true" : "false")
      << ",\n";
  out << "  \"semantic_skipped\": "
      << (result.semantic_skipped != 0 ? "true" : "false") << ",\n";
  out << "  \"paths\": {\n";
  out << "    \"summary\": \"" << EscapeJsonString(summary_path_text)
      << "\",\n";
  out << "    \"diagnostics\": \"" << EscapeJsonString(diagnostics_path_text)
      << "\",\n";
  out << "    \"manifest\": \"" << EscapeJsonString(manifest_path_text)
      << "\",\n";
  out << "    \"ir\": \"" << EscapeJsonString(ir_path_text) << "\",\n";
  out << "    \"object\": \"" << EscapeJsonString(object_path_text) << "\",\n";
  out << "    \"runtime_metadata_binary\": \""
      << EscapeJsonString(runtime_metadata_binary_path_text) << "\"\n";
  out << "  },\n";
  out << "  \"last_error\": \"" << EscapeJsonString(last_error) << "\",\n";
  out << "  \"result_error_message\": \""
      << EscapeJsonString(result_error_message) << "\",\n";
  out << "  \"c_api_ownership\": {\n";
  out << "    \"result_owned_error_message\": "
      << (!result_error_message.empty() ? "true" : "false") << ",\n";
  out << "    \"diagnostics_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_DIAGNOSTICS) != 0u
              ? "true"
              : "false")
      << ",\n";
  out << "    \"manifest_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_MANIFEST) != 0u
              ? "true"
              : "false")
      << ",\n";
  out << "    \"ir_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_IR) != 0u
              ? "true"
              : "false")
      << ",\n";
  out << "    \"object_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_OBJECT) != 0u
              ? "true"
              : "false")
      << ",\n";
  out << "    \"runtime_metadata_path_borrowed\": "
      << (objc3c_frontend_c_result_has_artifact(
              &result,
              OBJC3C_FRONTEND_ARTIFACT_RUNTIME_METADATA) != 0u
              ? "true"
              : "false")
      << "\n";
  out << "  },\n";
  out << "  \"stages\": {\n";
  WriteStageSummaryJson(out, "lex", result.lex, true);
  WriteStageSummaryJson(out, "parse", result.parse, true);
  WriteStageSummaryJson(out, "sema", result.sema, true);
  WriteStageSummaryJson(out, "lower", result.lower, true);
  WriteStageSummaryJson(out, "emit", result.emit, false);
  out << "  },\n";
  out << "  \"observability\": ";
  WriteObservabilityJson(
      out,
      "  ",
      summary_path_text,
      result,
      status,
      result_error_message,
      runtime_metadata_binary_path_text);
  out << ",\n";
  out << "  \"runtime_inspector\": ";
  WriteRuntimeInspectorJson(out, "  ", options, result);
  out << ",\n";
  out << "  \"bonus_experiences\": ";
  WriteBonusExperiencesJson(
      out,
      "  ",
      options,
      result,
      summary_path_text,
      runtime_metadata_binary_path_text);
  out << ",\n";
  out << "  \"output_contract\": {\n";
  out << "    \"diagnostics_schema_version\": \""
      << kObjc3CliReportingDiagnosticsSchemaVersion << "\",\n";
  out << "    \"summary_mode\": \"" << kObjc3CliReportingSummaryMode
      << "\",\n";
  out << "    \"scaffold_key\": \"" << EscapeJsonString(matrix.scaffold_key)
      << "\",\n";
  out << "    \"core_feature_key\": \""
      << EscapeJsonString(matrix.core_feature_key) << "\",\n";
  out << "    \"core_feature_expansion_key\": \""
      << EscapeJsonString(matrix.core_feature_expansion_key) << "\",\n";
  out << "    \"edge_case_compatibility_key\": \""
      << EscapeJsonString(matrix.edge_case_compatibility_key) << "\",\n";
  out << "    \"edge_case_robustness_key\": \""
      << EscapeJsonString(matrix.edge_case_robustness_key) << "\",\n";
  out << "    \"diagnostics_hardening_key\": \""
      << EscapeJsonString(matrix.diagnostics_hardening_key) << "\",\n";
  out << "    \"recovery_determinism_key\": \""
      << EscapeJsonString(matrix.recovery_determinism_key) << "\",\n";
  out << "    \"conformance_matrix_key\": \""
      << EscapeJsonString(matrix.conformance_matrix_key) << "\",\n";
  out << "    \"summary_output_path\": \""
      << EscapeJsonString(matrix.summary_output_path) << "\",\n";
  out << "    \"diagnostics_output_path\": \""
      << EscapeJsonString(matrix.diagnostics_output_path) << "\",\n";
  out << "    \"summary_output_path_contract_consistent\": "
      << (matrix.summary_output_path_contract_consistent ? "true" : "false")
      << ",\n";
  out << "    \"diagnostics_output_path_contract_consistent\": "
      << (matrix.diagnostics_output_path_contract_consistent ? "true"
                                                             : "false")
      << ",\n";
  out << "    \"diagnostics_filename_matches_emit_prefix\": "
      << (matrix.diagnostics_filename_matches_emit_prefix ? "true" : "false")
      << ",\n";
  out << "    \"core_feature_expansion_ready\": "
      << (matrix.core_feature_expansion_ready ? "true" : "false") << ",\n";
  out << "    \"summary_output_extension_compatible\": "
      << (matrix.summary_output_extension_compatible ? "true" : "false")
      << ",\n";
  out << "    \"diagnostics_output_suffix_compatible\": "
      << (matrix.diagnostics_output_suffix_compatible ? "true" : "false")
      << ",\n";
  out << "    \"case_folded_paths_distinct\": "
      << (matrix.case_folded_paths_distinct ? "true" : "false") << ",\n";
  out << "    \"output_paths_control_char_free\": "
      << (matrix.output_paths_control_char_free ? "true" : "false") << ",\n";
  out << "    \"edge_case_compatibility_consistent\": "
      << (matrix.edge_case_compatibility_consistent ? "true" : "false")
      << ",\n";
  out << "    \"edge_case_compatibility_ready\": "
      << (matrix.edge_case_compatibility_ready ? "true" : "false") << ",\n";
  out << "    \"summary_output_parent_present\": "
      << (matrix.summary_output_parent_present ? "true" : "false") << ",\n";
  out << "    \"diagnostics_output_parent_present\": "
      << (matrix.diagnostics_output_parent_present ? "true" : "false")
      << ",\n";
  out << "    \"output_paths_within_length_budget\": "
      << (matrix.output_paths_within_length_budget ? "true" : "false")
      << ",\n";
  out << "    \"output_paths_no_trailing_space\": "
      << (matrix.output_paths_no_trailing_space ? "true" : "false") << ",\n";
  out << "    \"edge_case_expansion_consistent\": "
      << (matrix.edge_case_expansion_consistent ? "true" : "false") << ",\n";
  out << "    \"edge_case_robustness_consistent\": "
      << (matrix.edge_case_robustness_consistent ? "true" : "false") << ",\n";
  out << "    \"edge_case_robustness_ready\": "
      << (matrix.edge_case_robustness_ready ? "true" : "false") << ",\n";
  out << "    \"diagnostics_hardening_consistent\": "
      << (matrix.diagnostics_hardening_consistent ? "true" : "false")
      << ",\n";
  out << "    \"diagnostics_hardening_ready\": "
      << (matrix.diagnostics_hardening_ready ? "true" : "false") << ",\n";
  out << "    \"recovery_determinism_consistent\": "
      << (matrix.recovery_determinism_consistent ? "true" : "false") << ",\n";
  out << "    \"recovery_determinism_ready\": "
      << (matrix.recovery_determinism_ready ? "true" : "false") << ",\n";
  out << "    \"conformance_matrix_consistent\": "
      << (matrix.conformance_matrix_consistent ? "true" : "false") << ",\n";
  out << "    \"conformance_matrix_ready\": "
      << (matrix.conformance_matrix_ready ? "true" : "false") << ",\n";
  out << "    \"conformance_matrix_key_ready\": "
      << (matrix.conformance_matrix_key_ready ? "true" : "false") << ",\n";
  out << "    \"conformance_corpus_key\": \""
      << EscapeJsonString(corpus.conformance_corpus_key) << "\",\n";
  out << "    \"conformance_corpus_case_count\": "
      << corpus.conformance_corpus_case_count << ",\n";
  out << "    \"conformance_corpus_accept_case_count\": "
      << corpus.conformance_corpus_accept_case_count << ",\n";
  out << "    \"conformance_corpus_reject_case_count\": "
      << corpus.conformance_corpus_reject_case_count << ",\n";
  out << "    \"conformance_corpus_consistent\": "
      << (corpus.conformance_corpus_consistent ? "true" : "false") << ",\n";
  out << "    \"conformance_corpus_ready\": "
      << (corpus.conformance_corpus_ready ? "true" : "false") << ",\n";
  out << "    \"conformance_corpus_key_ready\": "
      << (corpus.conformance_corpus_key_ready ? "true" : "false") << ",\n";
  out << "    \"core_feature_impl_ready\": "
      << (corpus.core_feature_impl_ready ? "true" : "false") << "\n";
  out << "  }\n";
  out << "}\n";
  return out.str();
}

std::string BuildFrontendCApiRunnerObservabilityJson(
    const fs::path &summary_path,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const std::string &result_error_message,
    const std::string &runtime_metadata_binary_path_text) {
  std::ostringstream dump;
  WriteObservabilityJson(
      dump,
      "",
      summary_path.generic_string(),
      result,
      status,
      result_error_message,
      runtime_metadata_binary_path_text);
  dump << "\n";
  return dump.str();
}

std::string BuildFrontendCApiRunnerPlaygroundReproJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    const fs::path &summary_path) {
  std::ostringstream dump;
  WritePlaygroundReproJson(
      dump,
      "",
      options,
      result,
      summary_path.generic_string());
  dump << "\n";
  return dump.str();
}

std::string BuildFrontendCApiRunnerRuntimeInspectorJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  std::ostringstream dump;
  WriteRuntimeInspectorJson(dump, "", options, result);
  dump << "\n";
  return dump.str();
}
