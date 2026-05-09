#pragma once

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"

#include <iosfwd>
#include <string>
#include <vector>

struct Objc3IRRuntimeBootstrapGlobalEmissionOptions {
  std::string module_name;
  std::string discovery_root_symbol;
  std::string linker_anchor_symbol;
  bool emit_selector_string_pools = false;
  bool emit_typed_keypath_artifacts = false;
  bool emit_registration_descriptor_image_root = false;
};

void EmitObjc3IRRuntimeBootstrapGlobals(
    const Objc3IRFrontendMetadata &frontend_metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    const Objc3IRRuntimeBootstrapGlobalEmissionOptions &options,
    std::ostringstream &out, std::vector<std::string> &retained_globals);
