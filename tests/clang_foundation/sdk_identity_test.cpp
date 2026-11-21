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
  // Default driver configuration is part of the header bundle contract too.
  const auto bundle=resources/"objc3-native-headers";fs::create_directory(bundle);
  const std::string config="-nostdinc++\n";
  for(const char *name:{"clang.cfg","clang++.cfg"}) {
    std::ofstream(bundle/name)<<config;
    std::ofstream(driver.parent_path()/name)<<config;
  }
  assert(VerifyNativeHeaderConfiguration(driver.string(),resources.string(),error));
  const auto native_config=driver.parent_path()/"clang.cfg";
  const auto config_time=fs::last_write_time(native_config);
  std::ofstream(native_config)<<"-nostdincXX\n";
  fs::last_write_time(native_config,config_time);
  assert(!VerifyNativeHeaderConfiguration(driver.string(),resources.string(),error));
  assert(error.find("driver configuration differs")!=std::string::npos);
  fs::remove_all(root);
}
