#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_storage_accessor_manifest_contracts.h"

struct Objc3RuntimeBootstrapApiSummary;

namespace objc3::artifacts::frontend {

void WriteStorageAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api,
    const Objc3StorageAccessorRuntimeAbiFields
        &storage_accessor_runtime_abi_fields);

}  // namespace objc3::artifacts::frontend
