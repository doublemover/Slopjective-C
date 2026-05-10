#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "ir/objc3_ir_frontend_metadata_block.h"
#include "ir/objc3_ir_frontend_metadata_concurrency.h"
#include "ir/objc3_ir_frontend_metadata_dispatch.h"
#include "ir/objc3_ir_frontend_metadata_dispatch_support.h"
#include "ir/objc3_ir_frontend_metadata_error_handling.h"
#include "ir/objc3_ir_frontend_metadata_interop.h"
#include "ir/objc3_ir_frontend_metadata_language_surface.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming.h"
#include "ir/objc3_ir_frontend_metadata_module_source_linkage.h"
#include "ir/objc3_ir_frontend_metadata_ownership.h"
#include "ir/objc3_ir_frontend_metadata_ownership_support.h"
#include "ir/objc3_ir_frontend_metadata_pipeline_readiness.h"
#include "ir/objc3_ir_frontend_metadata_runtime_bundles.h"
#include "ir/objc3_ir_frontend_metadata_runtime_metadata.h"
#include "ir/objc3_ir_frontend_metadata_runtime_support.h"
#include "ir/objc3_ir_frontend_metadata_semantic_surface.h"
#include "ir/objc3_ir_frontend_metadata_task_runtime_support.h"
#include "ir/objc3_ir_frontend_metadata_type_system.h"
#include "ir/objc3_ir_frontend_metadata_unsafe_intrinsics.h"
// Historical extraction contract marker:
// #include "parse/objc3_parser_contract.h"

struct Objc3Program;

struct Objc3IRFrontendMetadata : Objc3IRFrontendRuntimeSupportMetadata,
                                 Objc3IRFrontendPipelineReadinessMetadata,
                                 Objc3IRFrontendRuntimeMetadata,
                                 Objc3IRFrontendDispatchMetadata,
                                 Objc3IRFrontendOwnershipMetadata,
                                 Objc3IRFrontendBlockMetadata,
                                 Objc3IRFrontendTypeSystemMetadata,
                                 Objc3IRFrontendModuleSourceLinkageMetadata,
                                 Objc3IRFrontendErrorHandlingMetadata,
                                 Objc3IRFrontendSemanticSurfaceMetadata,
                                 Objc3IRFrontendConcurrencyMetadata,
                                 Objc3IRFrontendDispatchSupportMetadata,
                                 Objc3IRFrontendInteropMetadata,
                                 Objc3IRFrontendMetaprogrammingMetadata,
                                 Objc3IRFrontendOwnershipSupportMetadata,
                                 Objc3IRFrontendTaskRuntimeSupportMetadata,
                                 Objc3IRFrontendUnsafeIntrinsicsMetadata,
                                 Objc3IRFrontendLanguageSurfaceMetadata {};

