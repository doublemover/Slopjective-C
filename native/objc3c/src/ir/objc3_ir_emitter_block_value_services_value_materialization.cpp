#include "ir/objc3_ir_emitter_block_value_services_value_materialization.h"

#include "ir/objc3_ir_emitter_block_value_services_block_lowering.h"
#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_value_materialization.h"

Objc3IRValueMaterializationContext
BuildObjc3IREmitterValueMaterializationContext(
    const Objc3IREmitterServiceContextState &state,
    const Objc3IREmitterServiceContextCallbacks &callbacks) {
  return Objc3IRValueMaterializationContext{
      state.globals,
      state.typed_keypath_artifacts,
      callbacks.new_temp,
      callbacks.emit_unsupported_i32_value,
      [state, callbacks]() {
        return BuildObjc3IREmitterBlockLoweringContext(state, callbacks);
      }};
}
