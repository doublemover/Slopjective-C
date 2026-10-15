#include "parse/objc3_ast_builder.h"

#include <utility>

Objc3ParsedProgram Objc3AstBuilder::BeginProgram() const { return Objc3ParsedProgram{}; }

void Objc3AstBuilder::SetModuleName(Objc3ParsedProgram &program, std::string module_name) const {
  MutableObjc3ParsedProgramAst(program).module_name = std::move(module_name);
}

void Objc3AstBuilder::AddModuleImport(
    Objc3ParsedProgram &program,
    Objc3ParsedModuleImportDecl decl) const {
  MutableObjc3ParsedProgramAst(program).module_imports.push_back(std::move(decl));
}

void Objc3AstBuilder::AddGlobalDecl(Objc3ParsedProgram &program, Objc3ParsedGlobalDecl decl) const {
  MutableObjc3ParsedProgramAst(program).globals.push_back(std::move(decl));
}

void Objc3AstBuilder::AddProtocolDecl(Objc3ParsedProgram &program, Objc3ParsedProtocolDecl decl) const {
  MutableObjc3ParsedProgramAst(program).protocols.push_back(std::move(decl));
}

void Objc3AstBuilder::AddInterfaceDecl(Objc3ParsedProgram &program, Objc3ParsedInterfaceDecl decl) const {
  MutableObjc3ParsedProgramAst(program).interfaces.push_back(std::move(decl));
}

void Objc3AstBuilder::AddImplementationDecl(Objc3ParsedProgram &program, Objc3ParsedImplementationDecl decl) const {
  MutableObjc3ParsedProgramAst(program).implementations.push_back(std::move(decl));
}

void Objc3AstBuilder::AddFunctionDecl(Objc3ParsedProgram &program, Objc3ParsedFunctionDecl decl) const {
  MutableObjc3ParsedProgramAst(program).functions.push_back(std::move(decl));
}

void Objc3AstBuilder::SetDraftSyntaxSurfaceSummary(
    Objc3ParsedProgram &program,
    Objc3DraftSyntaxSurfaceSummary summary) const {
  MutableObjc3ParsedProgramAst(program).draft_syntax_surface_summary =
      std::move(summary);
}
