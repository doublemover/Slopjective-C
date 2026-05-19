#pragma once

struct Objc3IREmitterStateInitialization;
struct Objc3IRFrontendMetadata;
struct Objc3Program;

void CountObjc3IREmitterVectorSignatureState(
    const Objc3Program &program,
    Objc3IREmitterStateInitialization &state);

bool IngestObjc3IREmitterMethodDefinitionState(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata,
    Objc3IREmitterStateInitialization &state);

void BuildObjc3IREmitterStaticModelState(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata,
    Objc3IREmitterStateInitialization &state);
