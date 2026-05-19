#include "sema/objc3_semantic_context.h"

Objc3SemanticBodyValidationContext BuildObjc3SemanticBodyValidationContext(
    const Objc3ParsedProgram &program,
    const Objc3SemanticIntegrationSurface &surface,
    const Objc3SemanticValidationOptions &options,
    std::vector<std::string> &diagnostics) {
  Objc3SemanticBodyValidationContext context;
  context.ast = &Objc3ParsedProgramAst(program);
  context.surface = &surface;
  context.options = &options;
  context.diagnostics = &diagnostics;
  return context;
}
