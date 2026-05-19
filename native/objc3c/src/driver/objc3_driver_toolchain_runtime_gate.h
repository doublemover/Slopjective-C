#pragma once

#include <string>

#include "driver/objc3_driver_object_backend.h"

bool ValidateObjc3DriverToolchainRuntimeCoreFeature(
    const Objc3DriverObjectBackendResult &object_backend,
    int compile_status,
    std::string &reason);
