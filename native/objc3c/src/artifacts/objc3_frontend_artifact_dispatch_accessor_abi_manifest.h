#pragma once

#include <iosfwd>

#include "artifacts/objc3_frontend_artifact_dispatch_accessor_manifest_contracts.h"

namespace objc3::artifacts::frontend {

void WriteDispatchAccessorRuntimeAbiSurface(
    std::ostream &manifest,
    const Objc3DispatchAccessorRuntimeAbiFields
        &dispatch_accessor_runtime_abi_fields);

}  // namespace objc3::artifacts::frontend
