#pragma once

struct Objc3IRCompileTimeProofAnalysisContext;
struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRCompileTimeProofAnalysisContext
BuildObjc3IREmitterCompileTimeProofAnalysisContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
