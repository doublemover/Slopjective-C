#pragma once

#include <iosfwd>
#include <string>
#include <unordered_map>

#include "ir/objc3_ir_statement_orchestration.h"

struct FunctionDecl;
struct Objc3IRMethodDefinition;
struct Objc3Program;
struct Objc3IRSyntheticMethodEmissionStats;

struct Objc3IRFunctionOrchestrationOptions {
  const Objc3Program &program;
  bool arc_mode_enabled = false;
  const std::unordered_map<std::string, int> &class_receiver_constants;
  Objc3IRStatementOrchestrationOptions statement_options;
  Objc3IRSyntheticMethodEmissionStats &synthetic_method_stats;
};

void EmitObjc3IRFunctionOrchestration(
    const FunctionDecl &fn,
    const Objc3IRFunctionOrchestrationOptions &options,
    std::ostringstream &out);

void EmitObjc3IRMethodOrchestration(
    const Objc3IRMethodDefinition &method_def,
    const Objc3IRFunctionOrchestrationOptions &options,
    std::ostringstream &out);
