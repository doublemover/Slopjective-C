#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

std::string BuildExecutableMetadataLoweringHandoffReplayKey(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface);

std::string BuildExecutableMetadataTypedLoweringHandoffReplayKey(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface);
