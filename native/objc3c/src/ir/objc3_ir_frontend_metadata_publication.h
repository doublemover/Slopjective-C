#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_concurrency.h"

struct Objc3RuntimeMetadataLayoutPolicy;
struct Objc3IRRuntimeMetadataSymbols;

std::string BuildObjc3IRFrontendProfileComment(
    const Objc3IRFrontendMetadata &metadata);

void EmitObjc3IRFrontendMetadataPublication(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out);

void EmitObjc3IRFrontendCoreMetadataPublication(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRRuntimeMetadataBoundaryNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRRuntimeSupportMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRRuntimeMetadataObjectPublicationNodes(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out);

void EmitObjc3IRDispatchOwnershipMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out);

void EmitObjc3IRBlockArcMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRErrorHandlingMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRTypeSymbolDispatchCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRDispatchOwnershipLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRBlockLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRTypeModuleLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRModuleGovernanceLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRErrorHandlingLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRSafetyConcurrencyLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);

void EmitObjc3IRAsyncDiagnosticLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out);
