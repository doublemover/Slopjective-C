#pragma once

#include "lower/objc3_lowering_contract.h"

#include <string>

const char *HostRuntimeMetadataObjectFormat();
const char *HostRuntimeMetadataSectionSpellingModel();

bool IsSupportedRuntimeMetadataObjectFormat(const std::string &object_format);
std::string MapRuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section);
