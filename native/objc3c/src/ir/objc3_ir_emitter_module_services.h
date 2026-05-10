#pragma once

#include "ir/objc3_ir_function_orchestration.h"
#include "ir/objc3_ir_module_body_orchestration.h"
#include "ir/objc3_ir_module_metadata_publication.h"

struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRFunctionOrchestrationOptions
BuildObjc3IREmitterFunctionOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);

Objc3IRModuleBodyOrchestrationOptions
BuildObjc3IREmitterModuleBodyOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state);

Objc3IRModuleBodyOrchestrationCallbacks
BuildObjc3IREmitterModuleBodyOrchestrationCallbacks(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);

Objc3IRModuleMetadataPublicationOptions
BuildObjc3IREmitterModuleMetadataPublicationOptions(
    const Objc3IREmitterServiceContextState &state);
