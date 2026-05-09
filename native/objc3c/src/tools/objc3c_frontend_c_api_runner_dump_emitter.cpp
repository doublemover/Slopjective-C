#include "tools/objc3c_frontend_c_api_runner_dump_emitter.h"

#include <iostream>

void FrontendCApiRunnerDumpEmitter::EmitJsonPayload(
    const std::string &payload) {
  if (emitted_payload_) {
    std::cout << "\n";
  }
  std::cout << payload;
  emitted_payload_ = true;
}
