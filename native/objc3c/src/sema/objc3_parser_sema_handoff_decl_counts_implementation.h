#pragma once

#include <algorithm>
#include <cstddef>

#include "sema/objc3_sema_pass_manager_contract.h"

inline std::size_t BuildObjc3ParserImplementationPropertyDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t property_count = 0u;
  for (const auto &implementation_decl : ast.implementations) {
    property_count += implementation_decl.properties.size();
  }
  return property_count;
}

inline std::size_t BuildObjc3ParserImplementationMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &implementation_decl : ast.implementations) {
    method_count += implementation_decl.methods.size();
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserImplementationClassMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &implementation_decl : ast.implementations) {
    method_count += static_cast<std::size_t>(std::count_if(
        implementation_decl.methods.begin(),
        implementation_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserImplementationInstanceMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &implementation_decl : ast.implementations) {
    method_count += static_cast<std::size_t>(std::count_if(
        implementation_decl.methods.begin(),
        implementation_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return !method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserImplementationCategoryDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return static_cast<std::size_t>(std::count_if(
      ast.implementations.begin(),
      ast.implementations.end(),
      [](const Objc3ImplementationDecl &implementation_decl) {
        return implementation_decl.has_category;
      }));
}
