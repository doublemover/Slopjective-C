#pragma once

#include <string>

struct Objc3IRModulePrologue {
  std::string lowering_ir_boundary_replay_key;
  std::string runtime_dispatch_declaration_replay_key;
  std::string simd_vector_lowering_replay_key;
};

[[nodiscard]] std::string BuildObjc3IRModulePrologue(
    const Objc3IRModulePrologue &prologue);
