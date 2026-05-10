#pragma once

#include <algorithm>
#include <cstddef>

#include "sema/objc3_sema_pass_manager_contract.h"

inline std::size_t BuildObjc3ParserProtocolPropertyDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t property_count = 0u;
  for (const auto &protocol_decl : ast.protocols) {
    property_count += protocol_decl.properties.size();
  }
  return property_count;
}

inline std::size_t BuildObjc3ParserProtocolMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &protocol_decl : ast.protocols) {
    method_count += protocol_decl.methods.size();
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserProtocolClassMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &protocol_decl : ast.protocols) {
    method_count += static_cast<std::size_t>(std::count_if(
        protocol_decl.methods.begin(),
        protocol_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserProtocolInstanceMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &protocol_decl : ast.protocols) {
    method_count += static_cast<std::size_t>(std::count_if(
        protocol_decl.methods.begin(),
        protocol_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return !method_decl.is_class_method;
        }));
  }
  return method_count;
}
