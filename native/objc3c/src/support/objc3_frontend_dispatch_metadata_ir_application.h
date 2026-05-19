#pragma once

struct Objc3IRFrontendMetadata;

namespace objc3::artifacts::frontend {

struct Objc3DispatchMetadataApplication;
struct Objc3ObjectDispatchMetadataApplication;

void ApplyObjc3DispatchMetadataIrApplication(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3DispatchMetadataApplication &application);

void ApplyObjc3ObjectDispatchMetadataIrApplication(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3ObjectDispatchMetadataApplication &application);

}  // namespace objc3::artifacts::frontend
