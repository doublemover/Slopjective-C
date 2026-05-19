#pragma once

#include <iosfwd>

void EmitObjc3IRMemoryManagementRuntimeApiLinkFields(std::ostringstream &out);
void EmitObjc3IRMemoryManagementRuntimeImplementationLinkFields(
    std::ostringstream &out);
