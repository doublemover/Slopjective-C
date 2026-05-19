#pragma once

#include <functional>
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_method_definition_plan.h"

struct Objc3IRStaticDataEmissionOptions {
  const std::vector<GlobalDecl> &globals;
  const std::unordered_set<std::string> &mutable_global_symbols;
  const std::vector<Objc3IRMetaprogrammingGlobalArtifact>
      &metaprogramming_global_artifacts;
  std::unordered_map<std::string, int> &global_const_values;
  std::unordered_set<std::string> &global_nil_proven_symbols;
  std::function<bool(const Expr *expr)> is_compile_time_global_nil_expr;
};

bool EmitObjc3IRStaticData(
    const Objc3IRStaticDataEmissionOptions &options, std::ostringstream &out,
    std::string &error);
