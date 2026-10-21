#include "clang_foundation/sdk_identity.h"
#include "clang/Basic/Version.h"
#include "llvm/Support/SHA256.h"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <openssl/evp.h>
#include <vector>

namespace objc3::clang_foundation {
namespace {
std::string Hex(const std::array<std::uint8_t, 32> &bytes) {
  static constexpr char digits[] = "0123456789abcdef";
  std::string result;
  for (auto byte : bytes) {
    result += digits[byte >> 4];
    result += digits[byte & 15];
  }
  return result;
}
bool HashFile(const std::filesystem::path &path, std::string &hash) {
  std::ifstream stream(path, std::ios::binary);
  if (!stream)
    return false;
  std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> digest(
      EVP_MD_CTX_new(), &EVP_MD_CTX_free);
  if (!digest || EVP_DigestInit_ex(digest.get(), EVP_sha256(), nullptr) != 1)
    return false;
  std::array<char, 65536> buffer;
  while (stream) {
    stream.read(buffer.data(), buffer.size());
    if (EVP_DigestUpdate(digest.get(), buffer.data(),
                         static_cast<std::size_t>(stream.gcount())) != 1)
      return false;
  }
  if (!stream.eof())
    return false;
  std::array<std::uint8_t, 32> bytes;
  unsigned int length = 0;
  if (EVP_DigestFinal_ex(digest.get(), bytes.data(), &length) != 1 ||
      length != bytes.size())
    return false;
  hash = Hex(bytes);
  return true;
}
} // namespace

bool VerifyRelocatedSDK(const std::string &root, const std::string &driver_hash,
                        const std::string &resource_hash, std::string &driver,
                        std::string &resources, std::string &error) {
  namespace fs = std::filesystem;
  try {
    auto prefix = fs::canonical(root);
#if defined(_WIN32)
    auto executable = prefix / "bin/clang.exe";
#else
    auto executable = prefix / "bin/clang";
#endif
    auto resource = prefix / "lib/clang" / std::to_string(CLANG_VERSION_MAJOR);
    std::string actual_driver;
    if (!HashFile(executable, actual_driver) || actual_driver != driver_hash) {
      error = "Relocated SDK driver differs from the sealed compiler SDK";
      return false;
    }
    if (!fs::is_regular_file(resource / "include/stddef.h")) {
      error = "Relocated SDK resource headers are missing";
      return false;
    }
    std::vector<std::string> paths;
    for (const auto &entry : fs::recursive_directory_iterator(resource))
      if (entry.is_regular_file())
        paths.push_back(
            entry.path().lexically_relative(resource).generic_string());
    std::sort(paths.begin(), paths.end());
    llvm::SHA256 digest;
    for (const auto &relative : paths) {
      std::string hash;
      if (!HashFile(resource / relative, hash)) {
        error = "Cannot read relocated SDK resource " + relative;
        return false;
      }
      digest.update(std::to_string(relative.size()) + ":" + relative + hash +
                    "\n");
    }
    if (Hex(digest.final()) != resource_hash) {
      error = "Relocated SDK resources differ from the sealed compiler SDK";
      return false;
    }
    driver = executable.string();
    resources = resource.string();
    return true;
  } catch (const fs::filesystem_error &exception) {
    error = "Cannot resolve relocated SDK: " + std::string(exception.what());
    return false;
  }
}
} // namespace objc3::clang_foundation
