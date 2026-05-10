#pragma once

#include "ir/objc3_ir_frontend_metadata_error_propagation.h"
#include "ir/objc3_ir_frontend_metadata_error_recovery.h"
#include "ir/objc3_ir_frontend_metadata_error_runtime_bridge.h"

struct Objc3IRFrontendErrorHandlingMetadata
    : Objc3IRFrontendErrorPropagationMetadata,
      Objc3IRFrontendErrorRuntimeBridgeMetadata,
      Objc3IRFrontendErrorRecoveryMetadata {};
