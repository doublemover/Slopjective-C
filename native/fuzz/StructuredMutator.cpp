#include "CodecTarget.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/JSON.h"
#include <algorithm>
#include <cstring>

extern "C" std::size_t LLVMFuzzerMutate(std::uint8_t *, std::size_t, std::size_t);
extern "C" std::size_t LLVMFuzzerCustomMutator(std::uint8_t *data, std::size_t size,
                                             std::size_t maximum, unsigned seed) {
  std::string source, json;
  if (seed % 4 == 0 || !objc3::fuzz::Unpack(data, size, source, json))
    return LLVMFuzzerMutate(data, size, maximum);
  auto value = llvm::json::parse(json);
  if (!value) {llvm::consumeError(value.takeError());return LLVMFuzzerMutate(data, size, maximum);}
  auto *root = value->getAsObject();
  if (!root) return LLVMFuzzerMutate(data, size, maximum);
  switch (seed % 5) {
  case 0: source += " ";break;
  case 1: (*root)["schema_minor"] = static_cast<std::int64_t>((seed >> 8) % 8);break;
  case 2:
    if (auto *declarations = root->getArray("declarations")) std::reverse(declarations->begin(), declarations->end());
    break;
  case 3:
    if (auto *semantics = root->getArray("required_semantics"))
      if (semantics->size() < 8) semantics->push_back("future.required.v9");
    break;
  case 4: (*root)["deliberately_unknown_field"] = static_cast<std::int64_t>(seed);break;
  }
  // This is generation only. The target/replay never repairs saved bytes.
  (*root)["source_binding_sha256"] = objc3::clang_foundation::ComputeInterfaceSourceSha256(source);
  auto packed = objc3::fuzz::Pack(source, llvm::formatv("{0}", *value).str());
  if (packed.empty() || packed.size() > maximum) return size;
  std::memcpy(data, packed.data(), packed.size());return packed.size();
}
