#pragma once

#include <string>

#include "parse/objc3_parser_contract.h"

class Objc3AstBuilder {
 public:
  Objc3ParsedProgram BeginProgram() const;
  void SetModuleName(Objc3ParsedProgram &program, std::string module_name) const;
  void AddGlobalDecl(Objc3ParsedProgram &program, Objc3ParsedGlobalDecl decl) const;
  void AddProtocolDecl(Objc3ParsedProgram &program, Objc3ParsedProtocolDecl decl) const;
  void AddInterfaceDecl(Objc3ParsedProgram &program, Objc3ParsedInterfaceDecl decl) const;
  void AddImplementationDecl(Objc3ParsedProgram &program, Objc3ParsedImplementationDecl decl) const;
  void AddFunctionDecl(Objc3ParsedProgram &program, Objc3ParsedFunctionDecl decl) const;
};
