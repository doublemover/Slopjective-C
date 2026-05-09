#pragma once

#include "lower/objc3_lowering_contract.h"

#include <string>

const char *HostRuntimeMetadataRetentionAnchorModel();

std::string BuildRuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name);
