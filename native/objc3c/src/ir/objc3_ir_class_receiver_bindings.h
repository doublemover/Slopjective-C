#pragma once

#include <string>
#include <unordered_map>

#include "ir/objc3_ir_emitter_context.h"

struct Objc3Program;

std::unordered_map<std::string, int> BuildObjc3IRKnownClassReceiverConstants(
    const Objc3Program &program);

int LookupObjc3IRClassReceiverIdentityValue(
    const std::unordered_map<std::string, int> &class_receiver_constants,
    const std::string &class_name);

void SeedObjc3IRKnownClassReceiverBindings(
    const std::unordered_map<std::string, int> &class_receiver_constants,
    FunctionContext &ctx);
