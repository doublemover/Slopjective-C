#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>

struct Objc3IRFrontendMetadata;
struct Objc3RuntimeMetadataLayoutPolicy;
struct Objc3RuntimeMetadataLayoutPolicyFamily;

struct Objc3IRRuntimeObjectMetadataEmissionOptions {
  const Objc3IRFrontendMetadata &frontend_metadata;
  const Objc3RuntimeMetadataLayoutPolicy &layout_policy;
  const std::unordered_map<std::string, std::string>
      &method_list_symbols_by_key;
  const std::unordered_map<std::string, std::size_t>
      &method_list_entry_counts_by_key;
  const std::unordered_map<std::string, std::string>
      &protocol_descriptor_symbols_by_owner_identity;
};

void EmitObjc3IRRuntimeClassMetaclassBundleSection(
    const Objc3IRRuntimeObjectMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals);

void EmitObjc3IRRuntimeProtocolBundleSection(
    const Objc3IRRuntimeObjectMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals);

void EmitObjc3IRRuntimeCategoryBundleSection(
    const Objc3IRRuntimeObjectMetadataEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::ostringstream &out, std::vector<std::string> &retained_globals);
