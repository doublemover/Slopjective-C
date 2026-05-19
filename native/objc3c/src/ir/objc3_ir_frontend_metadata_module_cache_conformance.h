#pragma once

#include "ir/objc3_ir_frontend_metadata_cross_module_conformance.h"
#include "ir/objc3_ir_frontend_metadata_module_cache_invalidation.h"

struct Objc3IRFrontendModuleCacheConformanceMetadata
    : Objc3IRFrontendModuleCacheInvalidationMetadata,
      Objc3IRFrontendCrossModuleConformanceMetadata {};
