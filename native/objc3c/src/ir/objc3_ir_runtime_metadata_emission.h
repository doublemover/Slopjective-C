#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendMetadata;
struct Objc3RuntimeMetadataLayoutPolicy;

struct Objc3IRRuntimeMetadataSymbols {
  std::string linker_anchor_suffix;
  std::string linker_anchor_symbol;
  std::string discovery_root_symbol;
  std::string module_name_global_symbol;
  std::string translation_unit_identity_global_symbol;
  std::string image_descriptor_symbol;
  std::string registration_descriptor_identifier_safe_suffix;
  std::string image_root_identifier_safe_suffix;
  std::string registration_descriptor_name_global_symbol;
  std::string image_root_name_global_symbol;
  std::string registration_descriptor_symbol;
  std::string image_root_symbol;
  std::string init_stub_symbol;
  std::string registration_table_symbol;
  std::string image_local_init_state_symbol;
};

Objc3IRRuntimeMetadataSymbols BuildObjc3IRRuntimeMetadataSymbols(
    const std::string &module_name,
    const Objc3IRFrontendMetadata &frontend_metadata);

bool Objc3IRRuntimeMetadataSectionScaffoldReady(
    const Objc3IRFrontendMetadata &frontend_metadata);

bool Objc3IRRuntimeBootstrapLoweringReady(
    const Objc3IRFrontendMetadata &frontend_metadata);

bool Objc3IRRuntimeBootstrapRegistrationDescriptorImageRootLoweringReady(
    const Objc3IRFrontendMetadata &frontend_metadata);

bool BuildObjc3IRRuntimeMetadataLayoutPolicy(
    const Objc3IRFrontendMetadata &frontend_metadata,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error);

std::string FormatObjc3IRRuntimeMetadataDescriptorOrdinal(
    std::size_t ordinal);

std::string BuildObjc3IRRuntimeMetadataDescriptorSymbol(
    const std::string &descriptor_symbol_prefix, const std::string &kind,
    std::size_t ordinal);

std::string BuildObjc3IRRuntimeMetadataAuxiliarySymbol(
    const std::string &descriptor_symbol_prefix, const std::string &kind,
    const std::string &suffix, std::size_t ordinal);

const char *Objc3IRRuntimeBootstrapImageDescriptorType();
const char *Objc3IRRuntimeBootstrapRegistrationTableType();
