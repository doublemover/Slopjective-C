#include "ir/objc3_ir_emission_prologue.h"

#include <sstream>

std::string BuildObjc3IRModulePrologue(
    const Objc3IRModulePrologue &prologue) {
  std::ostringstream out;
  out << "; objc3c native frontend IR\n";
  out << "; lowering_ir_boundary = "
      << prologue.lowering_ir_boundary_replay_key << "\n";
  out << "; runtime_dispatch_decl = "
      << prologue.runtime_dispatch_declaration_replay_key << "\n";
  out << "; simd_vector_lowering = "
      << prologue.simd_vector_lowering_replay_key << "\n";
  return out.str();
}
