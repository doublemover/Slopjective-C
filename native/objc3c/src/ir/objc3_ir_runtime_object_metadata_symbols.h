#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

inline void RetainObjc3IRRuntimeObjectMetadataGlobal(
    std::vector<std::string> &retained_globals, const std::string &symbol) {
  retained_globals.push_back(symbol);
}

inline std::string BuildObjc3IRRuntimeObjectMetadataDescriptorSymbol(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    std::size_t ordinal) {
  return BuildObjc3IRRuntimeMetadataDescriptorSymbol(
      layout_policy.descriptor_symbol_prefix, family.kind, ordinal);
}

inline std::string BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const Objc3RuntimeMetadataLayoutPolicyFamily &family,
    const std::string &suffix, std::size_t ordinal) {
  return BuildObjc3IRRuntimeMetadataAuxiliarySymbol(
      layout_policy.descriptor_symbol_prefix, family.kind, suffix, ordinal);
}

inline std::string BuildObjc3IRRuntimeObjectMetadataAuxiliarySymbol(
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    const std::string &kind, const std::string &suffix, std::size_t ordinal) {
  return BuildObjc3IRRuntimeMetadataAuxiliarySymbol(
      layout_policy.descriptor_symbol_prefix, kind, suffix, ordinal);
}

inline std::string BuildObjc3IRRuntimeObjectMetadataMethodListKey(
    const std::string &owner_family_kind, const std::string &owner_identity,
    const std::string &list_kind) {
  return owner_family_kind + "|" + owner_identity + "|" + list_kind;
}
