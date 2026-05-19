#pragma once

struct Objc3IRStatementOrchestrationOptions;
struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRStatementOrchestrationOptions
BuildObjc3IREmitterStatementOrchestrationOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
