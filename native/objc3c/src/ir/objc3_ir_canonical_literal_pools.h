#pragma once

#include <map>
#include <string>

#include "ir/objc3_ir_emitter_context.h"

struct Objc3IRFrontendMetadata;
struct Objc3Program;

struct Objc3IRCanonicalLiteralPools {
  std::map<std::string, std::string> selector_pool_globals;
  std::map<std::string, std::string> runtime_string_pool_globals;
  std::map<std::string, TypedKeyPathArtifact> typed_keypath_artifacts;
};

Objc3IRCanonicalLiteralPools BuildObjc3IRCanonicalLiteralPools(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata);
