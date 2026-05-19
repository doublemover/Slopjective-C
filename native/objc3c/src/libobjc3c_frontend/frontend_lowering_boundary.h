#pragma once

#include <cstddef>
#include <string>

struct Objc3FrontendOptions;

namespace objc3c::frontend {

struct Objc3FrontendLoweringBoundaryContract {
  std::size_t max_message_send_args = 4;
  std::string runtime_dispatch_symbol = "objc3_runtime_dispatch_i32";
  bool runtime_dispatch_lowering_owner_ready = true;
};

Objc3FrontendLoweringBoundaryContract BuildFrontendLoweringBoundaryContract(
    const Objc3FrontendOptions &options);

bool TryNormalizeFrontendLoweringBoundary(Objc3FrontendOptions &options,
                                          std::string &error);

}  // namespace objc3c::frontend
