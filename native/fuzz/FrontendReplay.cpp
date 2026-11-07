#include "FrontendTarget.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/SHA256.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

namespace {
std::size_t Count(const char *directory) {
  return std::distance(std::filesystem::directory_iterator(directory),
                       std::filesystem::directory_iterator());
}
} // namespace

int main(int argc, char **argv) {
  assert(argc >= 3);
  if (std::string(argv[1]) == "--seed") {
    std::filesystem::create_directory(argv[2]);
    for (unsigned index = 0; index != 8; ++index) {
      const auto seed = objc3::fuzz::FrontendSeed(index);
      std::ofstream stream(std::filesystem::path(argv[2]) / ("source-" + std::to_string(index)),
                           std::ios::binary);
      stream.write(reinterpret_cast<const char *>(seed.data()), seed.size());
      assert(stream.good());
    }
    return 0;
  }
  assert(std::string(argv[1]) == "--replay");
  const auto descriptors_before = Count("/proc/self/fd");
  const auto threads_before = Count("/proc/self/task");
  llvm::json::Array observations;
  for (int index = 2; index != argc; ++index) {
    std::ifstream input(argv[index], std::ios::binary);
    assert(input);
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)), {});
    assert(bytes.size() <= objc3::fuzz::kMaxFrontendInput);
    const auto result = objc3::fuzz::Frontend(bytes.data(), bytes.size());
    observations.push_back(llvm::json::Object{
        {"disposition", result.disposition},
        {"diagnostics", static_cast<std::int64_t>(result.diagnostics)},
        {"result_sha256", result.digest},
        {"input_sha256", llvm::toHex(llvm::SHA256::hash({bytes.data(), bytes.size()}), true)},
        {"bytes", static_cast<std::int64_t>(bytes.size())}});
  }
  std::cout << llvm::formatv("{0}", llvm::json::Value(llvm::json::Object{
      {"observations", std::move(observations)},
      {"descriptors_before", descriptors_before},
      {"descriptors_after", Count("/proc/self/fd")},
      {"threads_before", threads_before},
      {"threads_after", Count("/proc/self/task")}})).str() << '\n';
}
