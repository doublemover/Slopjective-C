#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_emitter_context.h"

struct FunctionDecl;

struct Objc3IRFunctionEffectAnalysisOptions {
  const std::vector<const FunctionDecl *> &function_definitions;
  const std::unordered_set<std::string> &global_symbols;
  const std::unordered_set<std::string> &defined_functions;
  const std::unordered_set<std::string> &declared_pure_functions;
};

struct Objc3IRFunctionEffectAnalysis {
  std::unordered_set<std::string> mutable_global_symbols;
  std::unordered_map<std::string, FunctionEffectInfo> function_effects;
  std::unordered_set<std::string> impure_functions;
};

Objc3IRFunctionEffectAnalysis BuildObjc3IRFunctionEffectAnalysis(
    const Objc3IRFunctionEffectAnalysisOptions &options);

bool Objc3IRFunctionMayHaveGlobalSideEffects(
    const std::string &name,
    const std::unordered_set<std::string> &defined_functions,
    const std::unordered_set<std::string> &declared_pure_functions,
    const std::unordered_set<std::string> &impure_functions);
