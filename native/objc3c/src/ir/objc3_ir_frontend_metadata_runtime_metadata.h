#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_frontend_metadata_runtime_bootstrap.h"
#include "ir/objc3_ir_frontend_metadata_runtime_export.h"
#include "ir/objc3_ir_frontend_metadata_runtime_sections.h"
#include "ir/objc3_ir_frontend_metadata_runtime_source_records.h"
#include "runtime/metadata/runtime_metadata_typed_bundles.h"

struct Objc3IRFrontendRuntimeMetadata
    : Objc3IRFrontendRuntimeBootstrapMetadata,
      Objc3IRFrontendRuntimeExportMetadata,
      Objc3IRFrontendRuntimeMemberStorageMetadata,
      Objc3IRFrontendRuntimeSectionsMetadata,
      Objc3IRFrontendRuntimeSourceClosureMetadata,
      Objc3IRFrontendRuntimeSourceRecordsMetadata {};
