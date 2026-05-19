#include "diag/objc3_diag_category.h"

bool DiagnosticCategoryIsFrontendCompiler(Objc3DiagnosticCategory category) {
  return category == Objc3DiagnosticCategory::kConfiguration ||
         category == Objc3DiagnosticCategory::kLexical ||
         category == Objc3DiagnosticCategory::kParsing ||
         category == Objc3DiagnosticCategory::kSemanticAnalysis;
}
