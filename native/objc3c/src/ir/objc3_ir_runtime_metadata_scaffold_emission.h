#pragma once

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"

struct Objc3IRRuntimeMetadataScaffoldEmissionOptions {
  const std::string &module_name;
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols;
  const std::map<std::string, std::string> &selector_pool_globals;
  const std::map<std::string, std::string> &runtime_string_pool_globals;
  const std::map<std::string, TypedKeyPathArtifact> &typed_keypath_artifacts;
  const std::vector<Objc3IRMethodDefinition> &method_definitions;
  bool emit_runtime_bootstrap_lowering = false;
  bool emit_runtime_bootstrap_registration_descriptor_image_root = false;
};

bool EmitObjc3IRRuntimeMetadataSectionScaffold(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    std::ostringstream &out, std::string &error);
