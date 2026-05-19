#pragma once

#include <cstddef>
#include <iosfwd>
#include <map>
#include <string>
#include <unordered_map>

#include "ir/objc3_ir_function_signature_model.h"

struct Objc3Program;

void EmitObjc3IREntryPoint(
    const Objc3Program &program,
    const std::unordered_map<std::string, std::size_t> &function_arity,
    const std::map<std::string, LoweredFunctionSignature> &function_signatures,
    std::ostringstream &out);
