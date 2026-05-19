#pragma once

#include <iosfwd>

struct Objc3IRFrontendMetadata;

void EmitObjc3IRFrontendClassMetaclassSourceClosureIfReady(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EmitObjc3IRFrontendProtocolCategorySourceClosureIfReady(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
