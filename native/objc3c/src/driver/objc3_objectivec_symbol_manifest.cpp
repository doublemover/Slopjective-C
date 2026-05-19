#include "driver/objc3_objectivec_symbol_manifest.h"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <utility>

#include "ast/objc3_ast.h"

namespace {

std::string ToString(CXString value) {
  const char *raw = clang_getCString(value);
  std::string text = raw == nullptr ? "" : std::string(raw);
  clang_disposeString(value);
  return text;
}

CXChildVisitResult VisitSymbol(CXCursor cursor,
                               CXCursor,
                               CXClientData client_data) {
  auto *ctx = static_cast<SymbolContext *>(client_data);
  const CXCursorKind kind = clang_getCursorKind(cursor);
  if (kind == CXCursor_FunctionDecl || kind == CXCursor_VarDecl ||
      kind == CXCursor_ObjCInterfaceDecl ||
      kind == CXCursor_ObjCInstanceMethodDecl ||
      kind == CXCursor_ObjCClassMethodDecl) {
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
  out << severity_text << ":" << line << ":" << column << ": "
      << ToString(clang_getDiagnosticSpelling(diagnostic));
  return out.str();
}

std::string BuildSymbolManifest(const std::filesystem::path &input,
                                const SymbolContext &context) {
  std::ostringstream manifest;
  manifest << "{\n";
  manifest << "  \"source\": \"" << input.generic_string() << "\",\n";
  manifest << "  \"symbols\": [\n";
  for (std::size_t i = 0; i < context.rows.size(); ++i) {
    const SymbolRow &row = context.rows[i];
    manifest << "    {\"kind\":\"" << row.kind << "\",\"name\":\""
             << row.name << "\",\"line\":" << row.line
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

std::vector<std::string> CollectObjectiveCTranslationUnitDiagnostics(
    CXTranslationUnit translation_unit,
    bool &has_errors) {
  std::vector<std::string> diagnostics;
  has_errors = false;
  if (translation_unit != nullptr) {
    const unsigned count = clang_getNumDiagnostics(translation_unit);
    diagnostics.reserve(count);
    for (unsigned i = 0; i < count; ++i) {
      CXDiagnostic diagnostic = clang_getDiagnostic(translation_unit, i);
      diagnostics.push_back(FormatDiagnostic(diagnostic));
      const CXDiagnosticSeverity severity =
          clang_getDiagnosticSeverity(diagnostic);
      if (severity == CXDiagnostic_Error || severity == CXDiagnostic_Fatal) {
        has_errors = true;
      }
      clang_disposeDiagnostic(diagnostic);
    }
  } else {
    diagnostics.push_back("fatal:0:0: unable to parse translation unit");
    has_errors = true;
  }
  return diagnostics;
}

std::string BuildObjectiveCSymbolManifest(
    const std::filesystem::path &input,
    CXTranslationUnit translation_unit) {
  SymbolContext context;
  clang_visitChildren(clang_getTranslationUnitCursor(translation_unit),
                      VisitSymbol,
                      &context);
  std::sort(context.rows.begin(),
            context.rows.end(),
            [](const SymbolRow &a, const SymbolRow &b) {
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
  return BuildSymbolManifest(input, context);
}
