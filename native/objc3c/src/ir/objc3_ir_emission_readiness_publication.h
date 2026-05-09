#pragma once

#include <iosfwd>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IREmissionReadinessPublication(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
