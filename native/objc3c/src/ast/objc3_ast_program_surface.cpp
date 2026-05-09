#include "ast/objc3_ast_program_surface.h"

#include <sstream>

std::size_t Objc3ProgramDeclarationCount(const Objc3Program &program) {
  return program.globals.size() + program.protocols.size() +
         program.interfaces.size() + program.implementations.size() +
         program.functions.size();
}

std::string Objc3ProgramLoweringReplayKey(const Objc3Program &program) {
  std::ostringstream out;
  out << "module=" << program.module_name
      << ";globals=" << program.globals.size()
      << ";protocols=" << program.protocols.size()
      << ";interfaces=" << program.interfaces.size()
      << ";implementations=" << program.implementations.size()
      << ";functions=" << program.functions.size()
      << ";decls=" << Objc3ProgramDeclarationCount(program)
      << ";diagnostics=" << program.diagnostics.size();
  return out.str();
}
