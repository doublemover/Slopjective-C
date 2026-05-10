#pragma once

#include <iosfwd>

struct Objc3IRRuntimeMetadataScaffoldEmissionOptions;

void EmitObjc3IRRuntimeMetadataScaffoldExecutableObjectCommentSurfaces(
    const Objc3IRRuntimeMetadataScaffoldEmissionOptions &options,
    bool emit_class_metaclass_bundle_payloads,
    bool emit_protocol_category_bundle_payloads,
    bool emit_member_table_payloads, std::ostringstream &out);
