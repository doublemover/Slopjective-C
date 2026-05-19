#include "ir/objc3_ir_emitter_state_initialization.h"

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emitter_state_initialization_function_state.h"
#include "ir/objc3_ir_emitter_state_initialization_static_models.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"

Objc3IREmitterStateInitialization BuildObjc3IREmitterStateInitialization(
    const Objc3Program &program,
    const Objc3LoweringContract &lowering_contract,
    const Objc3IRFrontendMetadata &frontend_metadata) {
  Objc3IREmitterStateInitialization state;
  state.runtime_metadata_symbols =
      BuildObjc3IRRuntimeMetadataSymbols(program.module_name,
                                         frontend_metadata);
  if (!TryBuildObjc3LoweringIRBoundary(
          lowering_contract, state.lowering_ir_boundary,
          state.boundary_error)) {
    return state;
  }

  CountObjc3IREmitterVectorSignatureState(program, state);
  DiscoverObjc3IREmitterFunctionState(program, state);
  if (!IngestObjc3IREmitterMethodDefinitionState(
          program, frontend_metadata, state)) {
    return state;
  }

  BuildObjc3IREmitterStaticModelState(program, frontend_metadata, state);
  BuildObjc3IREmitterFunctionEffectState(state);
  return state;
}
