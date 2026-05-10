#pragma once

#include <algorithm>
#include <cstddef>

#include "sema/objc3_sema_pass_manager_contract.h"

inline std::size_t BuildObjc3ParserInterfacePropertyDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t property_count = 0u;
  for (const auto &interface_decl : ast.interfaces) {
    property_count += interface_decl.properties.size();
  }
  return property_count;
}

inline std::size_t BuildObjc3ParserInterfaceMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &interface_decl : ast.interfaces) {
    method_count += interface_decl.methods.size();
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserInterfaceClassMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &interface_decl : ast.interfaces) {
    method_count += static_cast<std::size_t>(std::count_if(
        interface_decl.methods.begin(),
        interface_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserInterfaceInstanceMethodDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  std::size_t method_count = 0u;
  for (const auto &interface_decl : ast.interfaces) {
    method_count += static_cast<std::size_t>(std::count_if(
        interface_decl.methods.begin(),
        interface_decl.methods.end(),
        [](const Objc3MethodDecl &method_decl) {
          return !method_decl.is_class_method;
        }));
  }
  return method_count;
}

inline std::size_t BuildObjc3ParserInterfaceCategoryDeclCountFromProgram(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return static_cast<std::size_t>(std::count_if(
      ast.interfaces.begin(),
      ast.interfaces.end(),
      [](const Objc3InterfaceDecl &interface_decl) {
        return interface_decl.has_category;
      }));
}
