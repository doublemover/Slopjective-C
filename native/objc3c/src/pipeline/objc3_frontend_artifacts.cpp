#include "pipeline/objc3_frontend_artifacts.h"

#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_emitter.h"

namespace {

const char *TypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::Function:
      return "function";
    default:
      return "unknown";
  }
}

const char *CompatibilityModeName(Objc3FrontendCompatibilityMode mode) {
  switch (mode) {
    case Objc3FrontendCompatibilityMode::kLegacy:
      return "legacy";
    case Objc3FrontendCompatibilityMode::kCanonical:
    default:
      return "canonical";
  }
}

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

std::vector<std::string> FlattenStageDiagnostics(const Objc3FrontendDiagnosticsBus &diagnostics_bus) {
  std::vector<std::string> diagnostics;
  diagnostics.reserve(diagnostics_bus.size());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.lexer.begin(), diagnostics_bus.lexer.end());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.parser.begin(), diagnostics_bus.parser.end());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.semantic.begin(), diagnostics_bus.semantic.end());
  return diagnostics;
}

}  // namespace

Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(const std::filesystem::path &input_path,
                                                        const Objc3FrontendPipelineResult &pipeline_result,
                                                        const Objc3FrontendOptions &options) {
  Objc3FrontendArtifactBundle bundle;
  const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);
  bundle.stage_diagnostics = pipeline_result.stage_diagnostics;
  bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);
  if (!bundle.diagnostics.empty()) {
    return bundle;
  }

  std::vector<const FunctionDecl *> manifest_functions;
  manifest_functions.reserve(program.functions.size());
  std::unordered_set<std::string> manifest_function_names;
  for (const auto &fn : program.functions) {
    if (manifest_function_names.insert(fn.name).second) {
      manifest_functions.push_back(&fn);
    }
  }

  std::size_t scalar_return_i32 = 0;
  std::size_t scalar_return_bool = 0;
  std::size_t scalar_return_void = 0;
  std::size_t scalar_param_i32 = 0;
  std::size_t scalar_param_bool = 0;
  std::size_t vector_signature_functions = 0;
  std::size_t vector_return_signatures = 0;
  std::size_t vector_param_signatures = 0;
  std::size_t vector_i32_signatures = 0;
  std::size_t vector_bool_signatures = 0;
  std::size_t vector_lane2_signatures = 0;
  std::size_t vector_lane4_signatures = 0;
  std::size_t vector_lane8_signatures = 0;
  std::size_t vector_lane16_signatures = 0;
  for (const auto &entry : pipeline_result.integration_surface.functions) {
    const FunctionInfo &signature = entry.second;
    if (signature.return_type == ValueType::Bool) {
      ++scalar_return_bool;
    } else if (signature.return_type == ValueType::Void) {
      ++scalar_return_void;
    } else {
      ++scalar_return_i32;
    }
    for (const ValueType param_type : signature.param_types) {
      if (param_type == ValueType::Bool) {
        ++scalar_param_bool;
      } else {
        ++scalar_param_i32;
      }
    }
  }
  for (const FunctionDecl *fn : manifest_functions) {
    bool has_vector_signature = false;
    if (fn->return_vector_spelling) {
      has_vector_signature = true;
      ++vector_return_signatures;
      if (fn->return_vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++vector_bool_signatures;
      } else {
        ++vector_i32_signatures;
      }
      if (fn->return_vector_lane_count == 2u) {
        ++vector_lane2_signatures;
      } else if (fn->return_vector_lane_count == 4u) {
        ++vector_lane4_signatures;
      } else if (fn->return_vector_lane_count == 8u) {
        ++vector_lane8_signatures;
      } else if (fn->return_vector_lane_count == 16u) {
        ++vector_lane16_signatures;
      }
    }
    for (const FuncParam &param : fn->params) {
      if (!param.vector_spelling) {
        continue;
      }
      has_vector_signature = true;
      ++vector_param_signatures;
      if (param.vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++vector_bool_signatures;
      } else {
        ++vector_i32_signatures;
      }
      if (param.vector_lane_count == 2u) {
        ++vector_lane2_signatures;
      } else if (param.vector_lane_count == 4u) {
        ++vector_lane4_signatures;
      } else if (param.vector_lane_count == 8u) {
        ++vector_lane8_signatures;
      } else if (param.vector_lane_count == 16u) {
        ++vector_lane16_signatures;
      }
    }
    if (has_vector_signature) {
      ++vector_signature_functions;
    }
  }

  std::vector<int> resolved_global_values;
  if (!ResolveGlobalInitializerValues(program.globals, resolved_global_values) ||
      resolved_global_values.size() != program.globals.size()) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: global initializer failed const evaluation")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }

  std::ostringstream manifest;
  manifest << "{\n";
  manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";
  manifest << "  \"module\": \"" << program.module_name << "\",\n";
  manifest << "  \"frontend\": {\n";
  manifest << "    \"language_version\":" << static_cast<unsigned>(options.language_version) << ",\n";
  manifest << "    \"compatibility_mode\":\"" << CompatibilityModeName(options.compatibility_mode) << "\",\n";
  manifest << "    \"migration_assist\":" << (options.migration_assist ? "true" : "false") << ",\n";
  manifest << "    \"migration_hints\":{\"legacy_yes\":" << pipeline_result.migration_hints.legacy_yes_count
           << ",\"legacy_no\":" << pipeline_result.migration_hints.legacy_no_count << ",\"legacy_null\":"
           << pipeline_result.migration_hints.legacy_null_count
           << ",\"legacy_total\":" << pipeline_result.migration_hints.legacy_total() << "},\n";
  manifest << "    \"language_version_pragma_contract\":{\"seen\":"
           << (pipeline_result.language_version_pragma_contract.seen ? "true" : "false")
           << ",\"directive_count\":" << pipeline_result.language_version_pragma_contract.directive_count
           << ",\"duplicate\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")
           << ",\"non_leading\":"
           << (pipeline_result.language_version_pragma_contract.non_leading ? "true" : "false")
           << ",\"first_line\":" << pipeline_result.language_version_pragma_contract.first_line
           << ",\"first_column\":" << pipeline_result.language_version_pragma_contract.first_column
           << ",\"last_line\":" << pipeline_result.language_version_pragma_contract.last_line
           << ",\"last_column\":" << pipeline_result.language_version_pragma_contract.last_column << "},\n";
  manifest << "    \"max_message_send_args\":" << options.lowering.max_message_send_args << ",\n";
  manifest << "    \"pipeline\": {\n";
  manifest << "      \"semantic_skipped\": " << (pipeline_result.integration_surface.built ? "false" : "true")
           << ",\n";
  manifest << "      \"stages\": {\n";
  manifest << "        \"lexer\": {\"diagnostics\":" << bundle.stage_diagnostics.lexer.size() << "},\n";
  manifest << "        \"parser\": {\"diagnostics\":" << bundle.stage_diagnostics.parser.size() << "},\n";
  manifest << "        \"semantic\": {\"diagnostics\":" << bundle.stage_diagnostics.semantic.size()
           << "}\n";
  manifest << "      },\n";
  manifest << "      \"sema_pass_manager\": {\"diagnostics_after_build\":"
           << pipeline_result.sema_diagnostics_after_pass[0] << ",\"diagnostics_after_validate_bodies\":"
           << pipeline_result.sema_diagnostics_after_pass[1] << ",\"diagnostics_after_validate_pure_contract\":"
           << pipeline_result.sema_diagnostics_after_pass[2] << ",\"diagnostics_emitted_by_build\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[0]
           << ",\"diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[1]
           << ",\"diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[2] << ",\"diagnostics_monotonic\":"
           << (pipeline_result.sema_parity_surface.diagnostics_after_pass_monotonic ? "true" : "false")
           << ",\"diagnostics_total\":"
           << pipeline_result.sema_parity_surface.diagnostics_total
           << ",\"deterministic_semantic_diagnostics\":"
           << (pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics ? "true" : "false")
           << ",\"deterministic_type_metadata_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")
           << ",\"deterministic_atomic_memory_order_mapping\":"
           << (pipeline_result.sema_parity_surface.deterministic_atomic_memory_order_mapping ? "true" : "false")
           << ",\"atomic_memory_order_mapping_total\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.total()
           << ",\"atomic_relaxed_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.relaxed
           << ",\"atomic_acquire_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acquire
           << ",\"atomic_release_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.release
           << ",\"atomic_acq_rel_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acq_rel
           << ",\"atomic_seq_cst_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.seq_cst
           << ",\"atomic_unmapped_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.unsupported
           << ",\"deterministic_vector_type_lowering\":"
           << (pipeline_result.sema_parity_surface.deterministic_vector_type_lowering ? "true" : "false")
           << ",\"vector_type_lowering_total\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.total()
           << ",\"vector_return_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.return_annotations
           << ",\"vector_param_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.param_annotations
           << ",\"vector_i32_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.i32_annotations
           << ",\"vector_bool_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.bool_annotations
           << ",\"vector_lane2_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane2_annotations
           << ",\"vector_lane4_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane4_annotations
           << ",\"vector_lane8_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane8_annotations
           << ",\"vector_lane16_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane16_annotations
           << ",\"vector_unsupported_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.unsupported_annotations
           << ",\"ready\":"
           << (pipeline_result.sema_parity_surface.ready ? "true" : "false")
           << ",\"parity_ready\":"
           << (IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface) ? "true" : "false")
           << ",\"globals_total\":"
           << pipeline_result.sema_parity_surface.globals_total
           << ",\"functions_total\":"
           << pipeline_result.sema_parity_surface.functions_total
           << ",\"type_metadata_global_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_global_entries
           << ",\"type_metadata_function_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_function_entries << "},\n";
  manifest << "      \"vector_signature_surface\":{\"vector_signature_functions\":" << vector_signature_functions
           << ",\"vector_return_signatures\":" << vector_return_signatures
           << ",\"vector_param_signatures\":" << vector_param_signatures
           << ",\"vector_i32_signatures\":" << vector_i32_signatures
           << ",\"vector_bool_signatures\":" << vector_bool_signatures
           << ",\"lane2\":" << vector_lane2_signatures
           << ",\"lane4\":" << vector_lane4_signatures << ",\"lane8\":" << vector_lane8_signatures
           << ",\"lane16\":" << vector_lane16_signatures << "},\n";
  manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()
           << ",\"declared_functions\":" << manifest_functions.size()
           << ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()
           << ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()
           << ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32
           << ",\"scalar_return_bool\":" << scalar_return_bool
           << ",\"scalar_return_void\":" << scalar_return_void << ",\"scalar_param_i32\":" << scalar_param_i32
           << ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";
  manifest << "    }\n";
  manifest << "  },\n";
  manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args
           << ",\"selector_global_ordering\":\"lexicographic\"},\n";
  manifest << "  \"lowering_vector_abi\":{\"replay_key\":\"" << Objc3SimdVectorTypeLoweringReplayKey()
           << "\",\"lane_contract\":\"" << kObjc3SimdVectorLaneContract
           << "\",\"vector_signature_functions\":" << vector_signature_functions << "},\n";
  manifest << "  \"globals\": [\n";
  for (std::size_t i = 0; i < program.globals.size(); ++i) {
    manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]
             << ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";
    if (i + 1 != program.globals.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"functions\": [\n";
  for (std::size_t i = 0; i < manifest_functions.size(); ++i) {
    const auto &fn = *manifest_functions[i];
    manifest << "    {\"name\":\"" << fn.name << "\",\"params\":" << fn.params.size() << ",\"param_types\":[";
    for (std::size_t p = 0; p < fn.params.size(); ++p) {
      manifest << "\"" << TypeName(fn.params[p].type) << "\"";
      if (p + 1 != fn.params.size()) {
        manifest << ",";
      }
    }
    manifest << "]"
             << ",\"return\":\"" << TypeName(fn.return_type) << "\""
             << ",\"line\":" << fn.line << ",\"column\":" << fn.column << "}";
    if (i + 1 != manifest_functions.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ]\n";
  manifest << "}\n";
  bundle.manifest_json = manifest.str();

  Objc3IRFrontendMetadata ir_frontend_metadata;
  ir_frontend_metadata.language_version = options.language_version;
  ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);
  ir_frontend_metadata.migration_assist = options.migration_assist;
  ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;
  ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;
  ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;

  std::string ir_error;
  if (!EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.ir_text.clear();
    return bundle;
  }

  return bundle;
}
