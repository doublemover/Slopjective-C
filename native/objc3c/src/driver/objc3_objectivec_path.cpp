#include "driver/objc3_objectivec_path.h"

#include <clang-c/Index.h>

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"
#include "diag/objc3_diag_utils.h"
#include "io/objc3_diagnostics_artifacts.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_process.h"

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

}  // namespace

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
