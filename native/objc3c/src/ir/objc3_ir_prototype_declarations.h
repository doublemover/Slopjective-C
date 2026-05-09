#pragma once

#include <cstddef>
#include <iosfwd>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_method_definition_plan.h"

struct Objc3Program;

struct Objc3IRPrototypeDeclarationOptions {
  const Objc3Program &program;
  const Objc3IRFrontendMetadata &frontend_metadata;
  const std::vector<Objc3IRMethodDefinition> &method_definitions;
  const std::map<std::string, LoweredFunctionSignature> &function_signatures;
  const std::unordered_set<std::string> &defined_functions;
  std::size_t synthesized_property_accessor_count = 0;
  bool emit_runtime_bootstrap_lowering = false;
};

void EmitObjc3IRPrototypeDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options, std::ostringstream &out);
