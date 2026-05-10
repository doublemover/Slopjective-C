#pragma once

struct Objc3IRExpressionCallEmissionServices;
struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRExpressionCallEmissionServices
BuildObjc3IREmitterExpressionCallEmissionServices(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
