#include "sema/objc3_semantic_passes.h"

#include "diag/objc3_diag_utils.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "sema/objc3_pure_contract_model.inc"
#include "sema/objc3_pure_contract_effect_evidence.inc"
#include "sema/objc3_pure_contract_classification.inc"
#include "sema/objc3_pure_contract_diagnostics.inc"

void ValidatePureContractSemanticDiagnostics(
    const Objc3ParsedProgram &program,
    const std::unordered_map<std::string, FunctionInfo> &surface_functions,
    std::vector<std::string> &diagnostics) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  const std::unordered_set<std::string> globals =
      BuildPureContractGlobalNameSet(ast);

  std::unordered_set<std::string> defined_functions;
  const std::unordered_map<std::string, PureContractEffectInfo>
      function_effects =
          CollectPureContractFunctionEffects(ast, globals, defined_functions);
  const std::unordered_map<std::string, bool> pure_annotations =
      BuildPureContractAnnotationMap(surface_functions);
  const std::vector<std::string> ordered_functions =
      BuildOrderedPureContractFunctionNames(function_effects);

  std::unordered_set<std::string> impure_functions;
  std::unordered_map<std::string, PureContractCause> impure_causes;
  SeedDirectPureContractImpurities(ordered_functions, function_effects,
                                   impure_functions, impure_causes);
  PropagatePureContractImpurityCauses(ordered_functions, function_effects,
                                      defined_functions, pure_annotations,
                                      impure_functions, impure_causes);

  EmitPureContractViolationDiagnostics(ast.functions, impure_functions,
                                       impure_causes, diagnostics);
}
