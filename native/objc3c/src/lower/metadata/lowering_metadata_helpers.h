#pragma once

#include "lower/objc3_lowering_contract.h"

#include <array>
#include <cstddef>
#include <string>

extern const std::array<const char *,
                        kObjc3RuntimeMetadataLayoutPolicyFamilyCount>
    kCanonicalRuntimeMetadataFamilyOrder;

const char *BoolToken(bool value);
const char *HostRuntimeMetadataObjectFormat();
const char *HostRuntimeMetadataSectionSpellingModel();
const char *HostRuntimeMetadataRetentionAnchorModel();

bool IsSupportedRuntimeMetadataObjectFormat(const std::string &object_format);
std::string MapRuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section);
std::string BuildRuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name);
std::size_t CountRuntimeMetadataLayoutDescriptors(
    const std::array<Objc3RuntimeMetadataLayoutPolicyFamily,
                     kObjc3RuntimeMetadataLayoutPolicyFamilyCount> &families);
