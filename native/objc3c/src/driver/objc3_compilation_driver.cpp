#include "driver/objc3_compilation_driver.h"

#include <clang-c/Index.h>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"
#include "diag/objc3_diag_utils.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_file_io.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"
#include "libobjc3c_frontend/objc3_cli_frontend.h"

namespace fs = std::filesystem;

namespace {

std::string ToString(CXString value) {
  const char *raw = clang_getCString(value);
  std::string text = raw == nullptr ? "" : std::string(raw);
  clang_disposeString(value);
  return text;
}

CXChildVisitResult VisitSymbol(CXCursor cursor, CXCursor, CXClientData client_data) {
  auto *ctx = static_cast<SymbolContext *>(client_data);
  const CXCursorKind kind = clang_getCursorKind(cursor);
  if (kind == CXCursor_FunctionDecl || kind == CXCursor_VarDecl || kind == CXCursor_ObjCInterfaceDecl ||
      kind == CXCursor_ObjCInstanceMethodDecl || kind == CXCursor_ObjCClassMethodDecl) {
    const CXSourceLocation location = clang_getCursorLocation(cursor);
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

std::string FormatDiagnostic(CXDiagnostic diagnostic) {
  const CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
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

  const CXSourceLocation location = clang_getDiagnosticLocation(diagnostic);
  CXFile file;
  unsigned line = 0;
  unsigned column = 0;
  unsigned offset = 0;
  clang_getFileLocation(location, &file, &line, &column, &offset);
  (void)offset;

  std::ostringstream out;
  out << severity_text << ":" << line << ":" << column << ": " << ToString(clang_getDiagnosticSpelling(diagnostic));
  return out.str();
}

std::string BuildSymbolManifest(const fs::path &input, const SymbolContext &context) {
  std::ostringstream manifest;
  manifest << "{\n";
  manifest << "  \"source\": \"" << input.generic_string() << "\",\n";
  manifest << "  \"symbols\": [\n";
  for (std::size_t i = 0; i < context.rows.size(); ++i) {
    const SymbolRow &row = context.rows[i];
    manifest << "    {\"kind\":\"" << row.kind << "\",\"name\":\"" << row.name << "\",\"line\":" << row.line
             << ",\"column\":" << row.column << "}";
    if (i + 1 != context.rows.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ]\n";
  manifest << "}\n";
  return manifest.str();
}

Objc3FrontendOptions BuildFrontendOptions(const Objc3CliOptions &cli_options) {
  Objc3FrontendOptions options;
  options.lowering.max_message_send_args = cli_options.max_message_send_args;
  options.lowering.runtime_dispatch_symbol = cli_options.runtime_dispatch_symbol;
  return options;
}

int RunObjc3LanguagePath(const Objc3CliOptions &cli_options) {
  const std::string source = ReadText(cli_options.input);
  const Objc3FrontendOptions frontend_options = BuildFrontendOptions(cli_options);
  Objc3FrontendArtifactBundle artifacts = CompileObjc3SourceForCli(cli_options.input, source, frontend_options);
  WriteDiagnosticsArtifacts(cli_options.out_dir, cli_options.emit_prefix, artifacts.diagnostics);
  if (!artifacts.diagnostics.empty()) {
    return 1;
  }

  WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, artifacts.manifest_json);
  const fs::path ir_out = cli_options.out_dir / (cli_options.emit_prefix + ".ll");
  WriteText(ir_out, artifacts.ir_text);
  const fs::path object_out = cli_options.out_dir / (cli_options.emit_prefix + ".obj");
  int compile_status = 0;
  if (cli_options.ir_object_backend == Objc3IrObjectBackend::kClang) {
    compile_status = RunIRCompile(cli_options.clang_path, ir_out, object_out);
  } else {
    std::string backend_error;
    compile_status = RunIRCompileLLVMDirect(cli_options.llc_path, ir_out, object_out, backend_error);
    if (!backend_error.empty()) {
      std::cerr << backend_error << "\n";
    }
  }
  if (compile_status == 0) {
    const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");
    const std::string backend_text =
        cli_options.ir_object_backend == Objc3IrObjectBackend::kClang ? "clang\n" : "llvm-direct\n";
    WriteText(backend_out, backend_text);
  }
  return compile_status == 0 ? 0 : 3;
}

int RunObjectiveCPath(const Objc3CliOptions &cli_options) {
  const std::vector<const char *> parse_args = {"-x", "objective-c", "-std=gnu11"};
  CXIndex index = clang_createIndex(0, 0);
  CXTranslationUnit tu = clang_parseTranslationUnit(index, cli_options.input.string().c_str(), parse_args.data(),
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
      const CXDiagnosticSeverity severity = clang_getDiagnosticSeverity(diagnostic);
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
  WriteDiagnosticsArtifacts(cli_options.out_dir, cli_options.emit_prefix, diagnostics);

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

  WriteManifestArtifact(cli_options.out_dir, cli_options.emit_prefix, BuildSymbolManifest(cli_options.input, context));

  const fs::path object_out = cli_options.out_dir / (cli_options.emit_prefix + ".obj");
  const int compile_status = RunObjectiveCCompile(cli_options.clang_path, cli_options.input, object_out);

  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);
  return compile_status == 0 ? 0 : 3;
}

}  // namespace

int RunObjc3CompilationDriver(const Objc3CliOptions &cli_options) {
  if (!fs::exists(cli_options.input)) {
    std::cerr << "input file not found: " << cli_options.input.string() << "\n";
    return 2;
  }
  const std::string extension = ToLower(cli_options.input.extension().string());
  const bool needs_clang_path = extension != ".objc3" || cli_options.ir_object_backend == Objc3IrObjectBackend::kClang;
  if (needs_clang_path && cli_options.clang_path.has_root_path() && !fs::exists(cli_options.clang_path)) {
    std::cerr << "clang executable not found: " << cli_options.clang_path.string() << "\n";
    return 2;
  }
  const bool needs_llc_path = extension == ".objc3" && cli_options.ir_object_backend == Objc3IrObjectBackend::kLLVMDirect;
  if (needs_llc_path && cli_options.llc_path.has_root_path() && !fs::exists(cli_options.llc_path)) {
    std::cerr << "llc executable not found: " << cli_options.llc_path.string() << "\n";
    return 2;
  }

  if (extension == ".objc3") {
    return RunObjc3LanguagePath(cli_options);
  }
  return RunObjectiveCPath(cli_options);
}
