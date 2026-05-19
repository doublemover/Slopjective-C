#pragma once

struct Objc3IRBlockLoweringContext;
struct Objc3IREmitterServiceContextCallbacks;
struct Objc3IREmitterServiceContextState;

Objc3IRBlockLoweringContext BuildObjc3IREmitterBlockLoweringContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks);
