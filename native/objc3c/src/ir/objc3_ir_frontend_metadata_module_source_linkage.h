#pragma once

#include "ir/objc3_ir_frontend_metadata_module_cache_conformance.h"
#include "ir/objc3_ir_frontend_metadata_module_import_graph.h"

struct Objc3IRFrontendModuleSourceLinkageMetadata
    : Objc3IRFrontendModuleCacheConformanceMetadata,
      Objc3IRFrontendModuleImportGraphMetadata {};
