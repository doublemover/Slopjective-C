#pragma once

#include <iosfwd>

struct Objc3RuntimeMetadataLayoutPolicy;

void EmitObjc3IRRuntimeLayoutPolicyStateFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out);
