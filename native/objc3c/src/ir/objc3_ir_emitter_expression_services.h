#pragma once

struct Objc3IRExpressionCallEmissionOptions;
struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRExpressionCallEmissionOptions
BuildObjc3IREmitterExpressionCallEmissionOptions(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
