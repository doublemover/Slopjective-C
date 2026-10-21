#include "clang_foundation/sdk_identity.h"
#include "clang_foundation/interface_codec.h"
#include "clang/Basic/Version.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/FileSystem.h"
#include <cassert>
#include <filesystem>
#include <fstream>

int main() {
  using namespace objc3::clang_foundation;
  namespace fs = std::filesystem;
  llvm::SmallString<256> temporary;
  assert(!llvm::sys::fs::createUniqueDirectory("objc3-sdk-identity", temporary));
  fs::path root(temporary.c_str());
#if defined(_WIN32)
  auto driver = root / "bin/clang.exe";
#else
  auto driver = root / "bin/clang";
#endif
  auto resources = root / "lib/clang" / std::to_string(CLANG_VERSION_MAJOR);
  fs::create_directories(driver.parent_path());
  fs::create_directories(resources / "include");
  const std::string driver_bytes = "sealed driver";
  const std::string header_bytes = "typedef long ptrdiff_t;\n";
  std::ofstream(driver, std::ios::binary) << driver_bytes;
  std::ofstream(resources / "include/stddef.h", std::ios::binary) << header_bytes;
  std::string driver_path, resource_path, error;
  const std::string relative = "include/stddef.h";
  const auto driver_hash = ComputeInterfaceSourceSha256(driver_bytes);
  const auto resource_hash = ComputeInterfaceSourceSha256(
      std::to_string(relative.size()) + ":" + relative +
      ComputeInterfaceSourceSha256(header_bytes) + "\n");
  assert(VerifyRelocatedSDK(root.string(), driver_hash, resource_hash,
                            driver_path, resource_path, error));
  assert(fs::equivalent(driver_path, driver));
  assert(fs::equivalent(resource_path, resources));
  assert(!VerifyRelocatedSDK(root.string(), std::string(64, '0'), resource_hash,
                             driver_path, resource_path, error));
  auto header = resources / "include/stddef.h";
  const auto timestamp = fs::last_write_time(header);
  std::string changed = header_bytes;
  changed[0] = 'T';
  std::ofstream(header, std::ios::binary) << changed;
  fs::last_write_time(header, timestamp);
  assert(!VerifyRelocatedSDK(root.string(), driver_hash, resource_hash,
                             driver_path, resource_path, error));
  assert(error.find("resources differ") != std::string::npos);
  fs::remove_all(root);
}
