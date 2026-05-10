#pragma once

struct Objc3IREmitterStateInitialization;
struct Objc3Program;

void DiscoverObjc3IREmitterFunctionState(
    const Objc3Program &program,
    Objc3IREmitterStateInitialization &state);

void BuildObjc3IREmitterFunctionEffectState(
    Objc3IREmitterStateInitialization &state);
