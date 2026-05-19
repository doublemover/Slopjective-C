#include "diag/objc3_diag_catalog_frontend_data.h"

#include <array>

namespace {

constexpr std::array<NativeDiagCodeCatalogEntry,
                     kNativeDiagFrontendCatalogEntryCount>
    kNativeDiagFrontendCatalog = {{
        {'C', Objc3DiagnosticSubsystem::kCanonicalConfig,
         Objc3DiagnosticCategory::kConfiguration,
         "canonical language/configuration hard-cutover diagnostics", 1, 99,
         kObjc3ParserDiagnosticCatalogOwner},
        {'L', Objc3DiagnosticSubsystem::kLanguageSurface,
         Objc3DiagnosticCategory::kLexical,
         "lexer, language surface, lowering, and post-pipeline diagnostics", 1,
         399,
         kObjc3LexerDiagnosticCatalogOwner},
        {'P', Objc3DiagnosticSubsystem::kParser,
         Objc3DiagnosticCategory::kParsing,
         "parser grammar and typed AST construction diagnostics", 1, 399,
         kObjc3ParserDiagnosticCatalogOwner},
        {'S', Objc3DiagnosticSubsystem::kSemantic,
         Objc3DiagnosticCategory::kSemanticAnalysis,
         "semantic analysis, type system, and contract diagnostics", 1, 399,
         kObjc3SemaDiagnosticCatalogOwner},
    }};

}  // namespace

std::span<const NativeDiagCodeCatalogEntry> NativeDiagFrontendCatalogData() {
  return std::span<const NativeDiagCodeCatalogEntry>(
      kNativeDiagFrontendCatalog.data(), kNativeDiagFrontendCatalog.size());
}
