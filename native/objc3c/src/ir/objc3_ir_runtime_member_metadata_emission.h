#pragma once

#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>

#include "ir/objc3_ir_frontend_metadata.h"

struct Objc3RuntimeMetadataLayoutPolicy;
struct Objc3RuntimeMetadataLayoutPolicyFamily;

struct Objc3IRRuntimeMemberMetadataEmissionOptions {
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy;
  const std::unordered_map<std::string, std::string>
      &implementation_method_symbols_by_owner_identity;
};

bool EmitObjc3IRRuntimePropertyDescriptorSection(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals,
    std::string &error);

void EmitObjc3IRRuntimeIvarDescriptorSection(
    const Objc3IRRuntimeMemberMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals);
