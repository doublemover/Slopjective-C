#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_runtime_support_evidence.h"
#include "ir/objc3_ir_frontend_metadata_runtime_support_library.h"

struct Objc3IRFrontendRuntimeSupportMetadata
    : Objc3IRFrontendRuntimeSupportEvidenceMetadata,
      Objc3IRFrontendRuntimeSupportLibraryMetadata {};
