#include "tools/objc3c_frontend_c_api_runner_repro_command_segments.h"

void AppendFrontendCApiRunnerReproEmissionDumpFlags(
    std::ostream &command,
    const FrontendCApiRunnerOptions &options,
    bool dump_playground_repro_json) {
  if (!options.emit_manifest) {
    command << " --no-emit-manifest";
  }
  if (!options.emit_ir) {
    command << " --no-emit-ir";
  }
  if (!options.emit_object) {
    command << " --no-emit-object";
  }
  if (dump_playground_repro_json) {
    command << " --dump-playground-repro-json";
  }
}
