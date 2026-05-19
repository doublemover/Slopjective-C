#pragma once

#include <iosfwd>

struct Objc3IRRuntimeMetadataScaffoldEmissionOptions;
struct Objc3RuntimeMetadataLayoutPolicy;

void EmitObjc3IRRuntimeMetadataScaffoldCommentSurfaces(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    const Objc3RuntimeMetadataLayoutPolicy &layout_policy,
    bool emit_class_metaclass_bundle_payloads,
    bool emit_protocol_category_bundle_payloads,
    bool emit_member_table_payloads, std::ostringstream &out);
