#include "tools/objc3c_frontend_c_api_runner_compile_options_fields.h"

void ApplyFrontendCApiRunnerCompileEmissionOptions(
    objc3c_frontend_c_compile_options_t &compile_options,
    const FrontendCApiRunnerOptions &runner_options) {
  compile_options.emit_manifest = runner_options.emit_manifest ? 1u : 0u;
  compile_options.emit_ir = runner_options.emit_ir ? 1u : 0u;
  compile_options.emit_object = runner_options.emit_object ? 1u : 0u;
}
