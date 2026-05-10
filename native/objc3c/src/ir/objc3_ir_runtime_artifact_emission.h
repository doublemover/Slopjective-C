#pragma once

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"

struct Objc3IRFrontendMetadata;
struct Objc3RuntimeMetadataLayoutPolicy;

struct Objc3IRRuntimeArtifactEmissionOptions {
  std::string module_name;
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy;
  const std::map<std::string, std::string> &selector_pool_globals;
  const std::map<std::string, std::string> &runtime_string_pool_globals;
  const std::map<std::string, TypedKeyPathArtifact> &typed_keypath_artifacts;
  std::string image_info_symbol;
  std::string discovery_root_symbol;
  std::string linker_anchor_symbol;
  bool emit_runtime_bootstrap_lowering = false;
  bool emit_runtime_bootstrap_registration_descriptor_image_root = false;
};

bool EmitObjc3IRRuntimeArtifacts(
    const Objc3IRRuntimeArtifactEmissionOptions &options, std::ostringstream &out,
    std::vector<std::string> &retained_globals, std::string &error);
