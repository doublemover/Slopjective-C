#pragma once

#include <string>

struct Objc3IRModuleBodyOrchestrationOptions;

std::string AttachObjc3IRSourceLineDebugMetadata(
    const Objc3IRModuleBodyOrchestrationOptions &options,
    const std::string &ir);
