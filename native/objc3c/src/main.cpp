#include <clang-c/Index.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "diag/objc3_diag_types.h"
#include "diag/objc3_diag_utils.h"
#include "driver/objc3_cli_options.h"
#include "ir/objc3_ir_emitter.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_file_io.h"
#include "io/objc3_process.h"
#include "lex/objc3_lexer.h"
#include "parse/objc3_parser.h"
#include "sema/objc3_semantic_passes.h"
#include "lower/objc3_lowering_contract.h"
#include "pipeline/objc3_frontend_types.h"
#include "token/objc3_token.h"

namespace fs = std::filesystem;

static std::string ToString(CXString value) {
  const char *raw = clang_getCString(value);
  std::string text = raw == nullptr ? "" : std::string(raw);
  clang_disposeString(value);
  return text;
}

static const char *TypeName(ValueType type) {
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

static std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

static Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                            const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  Objc3Lexer lexer(source);
  std::vector<Token> tokens = lexer.Run(result.stage_diagnostics.lexer);

  Objc3ParseResult parse_result = ParseObjc3Program(tokens);
  result.program = std::move(parse_result.program);
  result.stage_diagnostics.parser = std::move(parse_result.diagnostics);

  if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {
    result.integration_surface = BuildSemanticIntegrationSurface(result.program, result.stage_diagnostics.semantic);
    Objc3SemanticValidationOptions semantic_options;
    semantic_options.max_message_send_args = options.lowering.max_message_send_args;
    ValidateSemanticBodies(result.program, result.integration_surface, semantic_options,
                           result.stage_diagnostics.semantic);
    ValidatePureContractSemanticDiagnostics(result.program, result.integration_surface.functions,
                                            result.stage_diagnostics.semantic);
  }

  result.program.diagnostics.reserve(result.stage_diagnostics.lexer.size() + result.stage_diagnostics.parser.size() +
                                     result.stage_diagnostics.semantic.size());
  result.program.diagnostics.insert(result.program.diagnostics.end(), result.stage_diagnostics.lexer.begin(),
                                    result.stage_diagnostics.lexer.end());
  result.program.diagnostics.insert(result.program.diagnostics.end(), result.stage_diagnostics.parser.begin(),
                                    result.stage_diagnostics.parser.end());
  result.program.diagnostics.insert(result.program.diagnostics.end(), result.stage_diagnostics.semantic.begin(),
                                    result.stage_diagnostics.semantic.end());
  NormalizeDiagnostics(result.program.diagnostics);
  return result;
}

static CXChildVisitResult VisitSymbol(CXCursor cursor, CXCursor, CXClientData client_data) {
  auto *ctx = static_cast<SymbolContext *>(client_data);
  CXCursorKind kind = clang_getCursorKind(cursor);
  if (kind == CXCursor_FunctionDecl || kind == CXCursor_VarDecl || kind == CXCursor_ObjCInterfaceDecl ||
      kind == CXCursor_ObjCInstanceMethodDecl || kind == CXCursor_ObjCClassMethodDecl) {
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile file;
    unsigned line = 0;
    unsigned column = 0;
    unsigned offset = 0;
    clang_getFileLocation(location, &file, &line, &column, &offset);
    (void)offset;

    SymbolRow row{
        ToString(clang_getCursorKindSpelling(kind)),
        ToString(clang_getCursorSpelling(cursor)),
        line,
        column,
    };
    ctx->rows.push_back(std::move(row));
  }

  return CXChildVisit_Recurse;
}

static std::string FormatDiagnostic(CXDiagnostic diagnostic) {
  CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
  std::string severity_text;
  switch (severity) {
    case CXDiagnostic_Ignored:
      severity_text = "ignored";
      break;
    case CXDiagnostic_Note:
      severity_text = "note";
      break;
    case CXDiagnostic_Warning:
      severity_text = "warning";
      break;
    case CXDiagnostic_Error:
      severity_text = "error";
      break;
    case CXDiagnostic_Fatal:
      severity_text = "fatal";
      break;
    default:
      severity_text = "unknown";
      break;
  }

  CXSourceLocation location = clang_getDiagnosticLocation(diagnostic);
  CXFile file;
  unsigned line = 0;
  unsigned column = 0;
  unsigned offset = 0;
  clang_getFileLocation(location, &file, &line, &column, &offset);
  (void)offset;

  std::ostringstream oss;
  oss << severity_text << ":" << line << ":" << column << ": "
      << ToString(clang_getDiagnosticSpelling(diagnostic));
  return oss.str();
}

static bool EmitObjc3IR(const Objc3Program &program, const Objc3LoweringContract &lowering_contract,
                        const fs::path &output_ir, std::string &error) {
  std::string ir;
  if (!EmitObjc3IRText(program, lowering_contract, ir, error)) {
    return false;
  }
  WriteText(output_ir, ir);
  return true;
}

int main(int argc, char **argv) {
  Objc3CliOptions cli_options;
  std::string cli_error;
  if (!ParseObjc3CliOptions(argc, argv, cli_options, cli_error)) {
    std::cerr << cli_error << "\n";
    return 2;
  }

  const fs::path &input = cli_options.input;
  const fs::path &out_dir = cli_options.out_dir;
  const std::string &emit_prefix = cli_options.emit_prefix;
  const fs::path &clang_path = cli_options.clang_path;
  Objc3FrontendOptions frontend_options;
  frontend_options.lowering.max_message_send_args = cli_options.max_message_send_args;
  frontend_options.lowering.runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol;

  if (!fs::exists(input)) {
    std::cerr << "input file not found: " << input.string() << "\n";
    return 2;
  }
  if (clang_path.has_root_path() && !fs::exists(clang_path)) {
    std::cerr << "clang executable not found: " << clang_path.string() << "\n";
    return 2;
  }

  const std::string extension = ToLower(input.extension().string());
  if (extension == ".objc3") {
    const std::string source = ReadText(input);
    Objc3FrontendPipelineResult frontend_pipeline = RunObjc3FrontendPipeline(source, frontend_options);
    Objc3Program &program = frontend_pipeline.program;
    WriteDiagnosticsArtifacts(out_dir, emit_prefix, program.diagnostics);
    if (!program.diagnostics.empty()) {
      return 1;
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
    for (const auto &entry : frontend_pipeline.integration_surface.functions) {
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

    std::ostringstream manifest;
    manifest << "{\n";
    manifest << "  \"source\": \"" << input.generic_string() << "\",\n";
    manifest << "  \"module\": \"" << program.module_name << "\",\n";
    manifest << "  \"frontend\": {\n";
    manifest << "    \"max_message_send_args\":" << frontend_options.lowering.max_message_send_args << ",\n";
    manifest << "    \"pipeline\": {\n";
    manifest << "      \"semantic_skipped\": " << (frontend_pipeline.integration_surface.built ? "false" : "true")
             << ",\n";
    manifest << "      \"stages\": {\n";
    manifest << "        \"lexer\": {\"diagnostics\":" << frontend_pipeline.stage_diagnostics.lexer.size()
             << "},\n";
    manifest << "        \"parser\": {\"diagnostics\":" << frontend_pipeline.stage_diagnostics.parser.size()
             << "},\n";
    manifest << "        \"semantic\": {\"diagnostics\":" << frontend_pipeline.stage_diagnostics.semantic.size()
             << "}\n";
    manifest << "      },\n";
    manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()
             << ",\"declared_functions\":" << manifest_functions.size()
             << ",\"resolved_global_symbols\":" << frontend_pipeline.integration_surface.globals.size()
             << ",\"resolved_function_symbols\":" << frontend_pipeline.integration_surface.functions.size()
             << ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32
             << ",\"scalar_return_bool\":" << scalar_return_bool
             << ",\"scalar_return_void\":" << scalar_return_void
             << ",\"scalar_param_i32\":" << scalar_param_i32
             << ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";
    manifest << "    }\n";
    manifest << "  },\n";
    manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << frontend_options.lowering.runtime_dispatch_symbol
             << "\",\"runtime_dispatch_arg_slots\":" << frontend_options.lowering.max_message_send_args
             << ",\"selector_global_ordering\":\"lexicographic\"},\n";
    std::vector<int> resolved_global_values;
    if (!ResolveGlobalInitializerValues(program.globals, resolved_global_values) ||
        resolved_global_values.size() != program.globals.size()) {
      const std::vector<std::string> ir_diags = {
          MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: global initializer failed const evaluation")};
      WriteDiagnosticsArtifacts(out_dir, emit_prefix, ir_diags);
      return 1;
    }

    manifest << "  \"globals\": [\n";
    for (std::size_t i = 0; i < program.globals.size(); ++i) {
      manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]
               << ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column
               << "}";
      if (i + 1 != program.globals.size()) {
        manifest << ",";
      }
      manifest << "\n";
    }
    manifest << "  ],\n";
    manifest << "  \"functions\": [\n";
    for (std::size_t i = 0; i < manifest_functions.size(); ++i) {
      const auto &fn = *manifest_functions[i];
      manifest << "    {\"name\":\"" << fn.name << "\",\"params\":" << fn.params.size()
               << ",\"param_types\":[";
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
    WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest.str());

    const fs::path ir_out = out_dir / (emit_prefix + ".ll");
    std::string ir_error;
    if (!EmitObjc3IR(program, frontend_options.lowering, ir_out, ir_error)) {
      const std::vector<std::string> ir_diags = {
          MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
      WriteDiagnosticsArtifacts(out_dir, emit_prefix, ir_diags);
      return 1;
    }

    const fs::path object_out = out_dir / (emit_prefix + ".obj");
    const int compile_status = RunIRCompile(clang_path, ir_out, object_out);
    return compile_status == 0 ? 0 : 3;
  }

  const std::vector<const char *> parse_args = {"-x", "objective-c", "-std=gnu11"};
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit tu = clang_parseTranslationUnit(index, input.string().c_str(), parse_args.data(),
                                                    static_cast<int>(parse_args.size()), nullptr, 0,
                                                    CXTranslationUnit_None);

  std::vector<std::string> diagnostics;
  bool has_errors = false;
  if (tu != nullptr) {
    const unsigned count = clang_getNumDiagnostics(tu);
    diagnostics.reserve(count);
    for (unsigned i = 0; i < count; ++i) {
      CXDiagnostic diagnostic = clang_getDiagnostic(tu, i);
      diagnostics.push_back(FormatDiagnostic(diagnostic));
      CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
      if (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal) {
        has_errors = true;
      }
      clang_disposeDiagnostic(diagnostic);
    }
  } else {
    diagnostics.push_back("fatal:0:0: unable to parse translation unit");
    has_errors = true;
  }

  NormalizeDiagnostics(diagnostics);
  WriteDiagnosticsArtifacts(out_dir, emit_prefix, diagnostics);

  if (has_errors || tu == nullptr) {
    if (tu != nullptr) {
      clang_disposeTranslationUnit(tu);
    }
    clang_disposeIndex(index);
    return 1;
  }

  SymbolContext context;
  clang_visitChildren(clang_getTranslationUnitCursor(tu), VisitSymbol, &context);
  std::sort(context.rows.begin(), context.rows.end(), [](const SymbolRow &a, const SymbolRow &b) {
    if (a.line != b.line) {
      return a.line < b.line;
    }
    if (a.column != b.column) {
      return a.column < b.column;
    }
    if (a.kind != b.kind) {
      return a.kind < b.kind;
    }
    return a.name < b.name;
  });

  std::ostringstream manifest;
  manifest << "{\n";
  manifest << "  \"source\": \"" << input.generic_string() << "\",\n";
  manifest << "  \"symbols\": [\n";
  for (std::size_t i = 0; i < context.rows.size(); ++i) {
    const SymbolRow &row = context.rows[i];
    manifest << "    {\"kind\":\"" << row.kind << "\",\"name\":\"" << row.name << "\",\"line\":"
             << row.line << ",\"column\":" << row.column << "}";
    if (i + 1 != context.rows.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ]\n";
  manifest << "}\n";
  WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest.str());

  const fs::path object_out = out_dir / (emit_prefix + ".obj");
  const int compile_status = RunObjectiveCCompile(clang_path, input, object_out);

  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);

  return compile_status == 0 ? 0 : 3;
}


