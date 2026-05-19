#pragma once

#include <iosfwd>

struct Objc3RuntimeMetadataLayoutPolicy;

void BeginObjc3IRRuntimeLayoutPolicyMetadataNode(
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    std::ostringstream &out);
void EndObjc3IRRuntimeLayoutPolicyMetadataNode(std::ostringstream &out);
