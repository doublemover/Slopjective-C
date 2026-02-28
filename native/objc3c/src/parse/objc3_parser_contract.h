#pragma once

#include "ast/objc3_ast.h"

// Parser-to-sema contract types. Keep parser outputs wrapped so downstream
// lowering/IR/runtime consumers bind to explicit parser contracts.
struct Objc3ParsedProgram {
  Objc3Program ast;
};

using Objc3ParsedGlobalDecl = GlobalDecl;
using Objc3ParsedFunctionDecl = FunctionDecl;

inline Objc3Program &MutableObjc3ParsedProgramAst(Objc3ParsedProgram &program) {
  return program.ast;
}

inline const Objc3Program &Objc3ParsedProgramAst(const Objc3ParsedProgram &program) {
  return program.ast;
}
