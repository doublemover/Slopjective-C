#include "diag/objc3_diag_catalog_data.h"

#include <array>

namespace {

constexpr std::array<NativeDiagCodeCatalogEntry, 8> kNativeDiagCodeCatalog = {{
    {'C', Objc3DiagnosticSubsystem::kCanonicalConfig,
     Objc3DiagnosticCategory::kConfiguration,
     "canonical language/configuration hard-cutover diagnostics", 1, 99},
    {'L', Objc3DiagnosticSubsystem::kLanguageSurface,
     Objc3DiagnosticCategory::kLexical,
     "lexer, language surface, lowering, and post-pipeline diagnostics", 1,
     399},
    {'P', Objc3DiagnosticSubsystem::kParser,
     Objc3DiagnosticCategory::kParsing,
     "parser grammar and typed AST construction diagnostics", 1, 399},
    {'S', Objc3DiagnosticSubsystem::kSemantic,
     Objc3DiagnosticCategory::kSemanticAnalysis,
     "semantic analysis, type system, and contract diagnostics", 1, 399},
    {'R', Objc3DiagnosticSubsystem::kRuntime,
     Objc3DiagnosticCategory::kRuntime,
     "runtime dispatch, C API, and metadata diagnostics", 1, 399},
    {'E', Objc3DiagnosticSubsystem::kFrontendApi,
     Objc3DiagnosticCategory::kFrontendApi,
     "public frontend C API and embedding diagnostics", 1, 99},
    {'A', Objc3DiagnosticSubsystem::kArtifact,
     Objc3DiagnosticCategory::kArtifact,
     "artifact, manifest, and schema diagnostics", 1, 399},
    {'T', Objc3DiagnosticSubsystem::kTooling,
     Objc3DiagnosticCategory::kTooling,
     "tooling and workflow diagnostics", 1, 399},
}};

}  // namespace

std::span<const NativeDiagCodeCatalogEntry> NativeDiagCodeCatalogData() {
  return std::span<const NativeDiagCodeCatalogEntry>(
      kNativeDiagCodeCatalog.data(),
      kNativeDiagCodeCatalog.size());
}
