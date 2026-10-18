#include "clang_foundation/sdk_identity.h"
#include <iostream>
int main(int argc, char **argv) {
  if (argc != 4) return 2;
  std::string driver, resources, error;
  if (!objc3::clang_foundation::VerifyRelocatedSDK(argv[1], argv[2], argv[3], driver, resources, error)) {
    std::cerr << error << '\n'; return 1;
  }
}
