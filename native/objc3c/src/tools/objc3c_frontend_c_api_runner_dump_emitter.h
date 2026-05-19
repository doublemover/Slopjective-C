#pragma once

#include <string>

class FrontendCApiRunnerDumpEmitter {
 public:
  void EmitJsonPayload(const std::string &payload);

 private:
  bool emitted_payload_ = false;
};
