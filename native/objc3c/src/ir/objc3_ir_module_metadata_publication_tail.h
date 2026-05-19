#pragma once

#include <iosfwd>
#include <string>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRModuleMetadataTailPublication(
    const Objc3IRFrontendMetadata &frontend_metadata,
    const std::string &module_name, std::ostringstream &out);
