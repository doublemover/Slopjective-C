#include "ir/objc3_ir_emitter_service_contexts.h"

#include <cstddef>
#include <string>
#include <utility>

namespace {

std::string NewObjc3IREmitterServiceTemp(FunctionContext &ctx) {
  return "%t" + std::to_string(ctx.temp_counter++);
}

std::string NewObjc3IREmitterServiceLabel(FunctionContext &ctx,
                                          const std::string &prefix) {
  return prefix + std::to_string(ctx.label_counter++);
}

}  // namespace

Objc3IREmitterServiceContextCallbacks
BuildObjc3IREmitterServiceContextCallbacks(
    std::function<std::string(const std::string &reason)>
        emit_unsupported_i32_value) {
  return Objc3IREmitterServiceContextCallbacks{
      NewObjc3IREmitterServiceTemp,
      NewObjc3IREmitterServiceLabel,
      std::move(emit_unsupported_i32_value)};
}
