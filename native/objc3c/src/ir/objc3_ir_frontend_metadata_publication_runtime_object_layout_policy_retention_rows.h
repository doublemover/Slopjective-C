#pragma once

#include <iosfwd>

struct Objc3RuntimeMetadataLayoutPolicy;

void EmitObjc3IRRuntimeLayoutPolicyRetentionFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out);
