#pragma once

#include <algorithm>
#include <cstddef>

#include "sema/objc3_sema_pass_manager_contract.h"

inline std::size_t BuildObjc3ParserFunctionPrototypeCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return static_cast<std::size_t>(std::count_if(
      ast.functions.begin(),
      ast.functions.end(),
      [](const FunctionDecl &function_decl) { return function_decl.is_prototype; }));
}

inline std::size_t BuildObjc3ParserFunctionPureCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return static_cast<std::size_t>(std::count_if(
      ast.functions.begin(),
      ast.functions.end(),
      [](const FunctionDecl &function_decl) { return function_decl.is_pure; }));
}
