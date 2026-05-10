#pragma once

#include "ir/objc3_ir_frontend_metadata_module_import_graph_core.h"
#include "ir/objc3_ir_frontend_metadata_module_source_visibility.h"

struct Objc3IRFrontendModuleImportGraphMetadata
    : Objc3IRFrontendModuleImportGraphCoreMetadata,
      Objc3IRFrontendModuleSourceVisibilityMetadata {};
