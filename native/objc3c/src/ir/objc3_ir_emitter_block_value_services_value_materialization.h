#pragma once

struct Objc3IRValueMaterializationContext;
struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRValueMaterializationContext
BuildObjc3IREmitterValueMaterializationContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
