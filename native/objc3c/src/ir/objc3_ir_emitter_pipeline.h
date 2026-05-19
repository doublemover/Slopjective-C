#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

struct Objc3IREmitterStateInitialization;
struct Objc3IRFrontendMetadata;
struct Objc3Program;

struct Objc3IREmitterPipelineInputs {
  const Objc3Program &program;
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3IREmitterStateInitialization &initialized_state;
  std::unordered_map<std::string, int> &global_const_values;
  std::unordered_set<std::string> &global_nil_proven_symbols;
};

bool EmitObjc3IREmitterPipeline(
    const Objc3IREmitterPipelineInputs &inputs,
    std::string &ir,
    std::string &error);
