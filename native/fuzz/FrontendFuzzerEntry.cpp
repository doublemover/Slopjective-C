#include "FrontendTarget.h"

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
  (void)objc3::fuzz::Frontend(data, size);
  return 0;
}
