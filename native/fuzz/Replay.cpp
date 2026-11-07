#include "CodecTarget.h"
#include "clang_foundation/interface_codec.h"
#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include "llvm/Support/JSON.h"
#include "llvm/Support/FormatVariadic.h"

int main(int argc, char **argv) {
  assert(argc >= 3);
  if (std::string(argv[1]) == "--seed") {
    std::filesystem::create_directory(argv[2]);
    for (unsigned index = 0; index != 10; ++index) {
      auto seed = objc3::fuzz::Seed(index);
      std::ofstream stream(std::filesystem::path(argv[2]) / ("pair-"+std::to_string(index)), std::ios::binary);
      stream.write(reinterpret_cast<const char *>(seed.data()), seed.size());assert(stream.good());
    }
    return 0;
  }
  assert(std::string(argv[1]) == "--replay");
  const auto count = [](const char *directory) {
    return std::distance(std::filesystem::directory_iterator(directory), std::filesystem::directory_iterator());
  };
  const auto descriptors_before = count("/proc/self/fd"), threads_before = count("/proc/self/task");
  std::array<unsigned, 3> outcomes{};
  llvm::json::Array observations;
  for (int index = 2; index != argc; ++index) {
    std::ifstream input(argv[index], std::ios::binary);assert(input);
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    assert(bytes.size() <= objc3::fuzz::kMaxPair);
    input.close();
    std::string digest;
    const auto outcome = objc3::fuzz::Codec(bytes.data(), bytes.size(), &digest);
    ++outcomes[static_cast<unsigned>(outcome)];
    observations.push_back(llvm::json::Object{{"outcome", static_cast<std::int64_t>(outcome)},
        {"canonical_sha256", digest},
        {"input_sha256", objc3::clang_foundation::ComputeInterfaceSourceSha256(llvm::StringRef(
             reinterpret_cast<const char *>(bytes.data()), bytes.size()))},
        {"bytes", static_cast<std::int64_t>(bytes.size())}});
  }
  std::cout << llvm::formatv("{0}", llvm::json::Value(llvm::json::Object{{"framing_rejected", outcomes[0]},
      {"semantic_rejected", outcomes[1]}, {"accepted", outcomes[2]}, {"observations", std::move(observations)},
      {"descriptors_before", descriptors_before}, {"descriptors_after", count("/proc/self/fd")},
      {"threads_before", threads_before}, {"threads_after", count("/proc/self/task")}})).str() << '\n';
}
