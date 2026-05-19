#pragma once

#include <cstddef>
#include <iosfwd>

struct Objc3IRFrontendMetadata;
struct Objc3IRRuntimeMetadataSymbols;

void EmitObjc3IRFrontendCorePublicationGroup(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EmitObjc3IRFrontendRuntimePublicationGroup(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out);
void EmitObjc3IRFrontendRuntimeSemanticsPublicationGroup(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out);
void EmitObjc3IRFrontendLoweringPublicationGroup(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
void EmitObjc3IRFrontendExtensionPublicationGroup(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
