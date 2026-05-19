#pragma once

#include <iosfwd>

struct Objc3RuntimeMetadataLayoutPolicy;

void EmitObjc3IRRuntimeLayoutPolicyOrderingFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out);
void EmitObjc3IRRuntimeLayoutPolicyObjectFormatFields(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out);
