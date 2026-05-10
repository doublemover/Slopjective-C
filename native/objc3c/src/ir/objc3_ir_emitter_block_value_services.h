#pragma once

struct Objc3IRBlockLoweringContext;
struct Objc3IRCompileTimeProofAnalysisContext;
struct Objc3IRValueMaterializationContext;
struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRBlockLoweringContext BuildObjc3IREmitterBlockLoweringContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);

Objc3IRCompileTimeProofAnalysisContext
BuildObjc3IREmitterCompileTimeProofAnalysisContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);

Objc3IRValueMaterializationContext
BuildObjc3IREmitterValueMaterializationContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
