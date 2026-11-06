#include "CodecTarget.h"
#include "clang_foundation/interface_codec.h"
#include "llvm/Support/Error.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

namespace objc3::fuzz {
using namespace clang_foundation;
namespace {
const llvm::StringRef supported[] = {"ownership.v1", "effects.async.v1", "objc3.async-entry-abi.v2"};
[[noreturn]] void Failure(const char *property) {
  std::fprintf(stderr, "OBJ3_CODEC_PROPERTY: %s\n", property);std::abort();
}
} // namespace

bool Unpack(const std::uint8_t *data, std::size_t size, std::string &source, std::string &json) {
  if (size < 4 || size > kMaxPair) return false;
  const std::uint32_t length = std::uint32_t(data[0]) | (std::uint32_t(data[1]) << 8) |
      (std::uint32_t(data[2]) << 16) | (std::uint32_t(data[3]) << 24);
  if (length > kMaxPart || length > size-4 || size-4-length > kMaxPart) return false;
  source.assign(reinterpret_cast<const char *>(data+4), length);
  json.assign(reinterpret_cast<const char *>(data+4+length), size-4-length);
  return true;
}
std::vector<std::uint8_t> Pack(const std::string &source, const std::string &json) {
  if (source.size() > kMaxPart || json.size() > kMaxPart || source.size()+json.size()+4 > kMaxPair)
    return {};
  const std::uint32_t size = source.size();
  std::vector<std::uint8_t> value{std::uint8_t(size), std::uint8_t(size >> 8), std::uint8_t(size >> 16), std::uint8_t(size >> 24)};
  value.insert(value.end(), source.begin(), source.end());value.insert(value.end(), json.begin(), json.end());
  return value;
}
Outcome Codec(const std::uint8_t *data, std::size_t size, std::string *observation) {
  if (observation) observation->clear();
  std::string source, json;
  if (!Unpack(data, size, source, json)) return Outcome::FramingRejected;
  // Exact-sized owned views make both pair boundaries sanitizer-visible;
  // neither source nor JSON acquires a C-string/strlen interpretation.
  auto source_bytes = std::make_unique<char[]>(source.size());
  auto json_bytes = std::make_unique<char[]>(json.size());
  if (!source.empty()) std::memcpy(source_bytes.get(), source.data(), source.size());
  if (!json.empty()) std::memcpy(json_bytes.get(), json.data(), json.size());
  auto decoded = DecodeSemanticInterfaceSidecar({json_bytes.get(), json.size()}, {source_bytes.get(), source.size()}, supported);
  if (!decoded) { llvm::consumeError(decoded.takeError());return Outcome::SemanticRejected; }
#if OBJC3_FUZZ_PROPERTY_FAULT == 1
  Failure("injected post-digest property failure");
#elif OBJC3_FUZZ_PROPERTY_FAULT == 2
  if (!source.empty()) {
    volatile char outside = source_bytes[source.size()];
    (void)outside;
  }
#endif
  auto encoded = EncodeSemanticInterfaceSidecar(*decoded, supported);
  if (!encoded) { llvm::consumeError(encoded.takeError());Failure("accepted decode failed encoding"); }
  auto again = DecodeSemanticInterfaceSidecar(*encoded, source, supported);
  if (!again) { llvm::consumeError(again.takeError());Failure("canonical encoding failed decoding"); }
  if (BuildSemanticFingerprintInput(*again) != BuildSemanticFingerprintInput(*decoded))
    Failure("decode/encode/decode semantic identity changed");
  auto second = EncodeSemanticInterfaceSidecar(*again, supported);
  if (!second) { llvm::consumeError(second.takeError());Failure("second encoding failed"); }
  if (*encoded != *second) Failure("canonical re-encoding changed bytes");
  auto permuted = *decoded;
  std::reverse(permuted.declarations.begin(), permuted.declarations.end());
  std::reverse(permuted.required_semantics.begin(), permuted.required_semantics.end());
  auto reordered = EncodeSemanticInterfaceSidecar(permuted, supported);
  if (!reordered) { llvm::consumeError(reordered.takeError());Failure("unordered permutation failed encoding"); }
  if (*reordered != *encoded) Failure("unordered declaration/semantic permutation changed canonical bytes");
  if (observation) *observation = ComputeInterfaceSourceSha256(*encoded);
  return Outcome::Accepted;
}
std::vector<std::uint8_t> Seed(unsigned variant) {
  std::string source = "int answer(void);\n";
  if (variant == 3) source += std::string("// embedded\0byte\n", 17);
  if (variant == 5) source.clear();
  if (variant == 6) source.assign(1, '\xff');
  if (variant == 7) return {1, 2};
  if (variant == 8) source.assign(kMaxPart, ' ');
  if (variant == 9) {
    const std::uint32_t over = kMaxPart+1;
    std::vector<std::uint8_t> value{std::uint8_t(over), std::uint8_t(over >> 8),
                                   std::uint8_t(over >> 16), std::uint8_t(over >> 24)};
    value.resize(4+over, ' ');return value;
  }
  SemanticInterfaceSidecar sidecar;
  sidecar.module = {"Probe", "semantic-probe", "x86_64-unknown-linux-gnu", "gnustep-2.2;manual-reference-counting",
                    "objc3-extension-abi-21", "202610", "codec-fuzz-profile-v1", "codec-fuzz-v1"};
  sidecar.canonical_source_sha256 = ComputeInterfaceSourceSha256(source);
  InterfaceDeclarationIdentity function{"c:@F@answer", "function", "Probe::answer", "int ()", {}};
  function.export_root = "Probe::answer";sidecar.declarations.push_back(function);
  if (variant == 1) {function.stable_id = "c:@F@other";function.qualified_name = function.export_root = "Probe::other";sidecar.declarations.push_back(function);}
  auto encoded = EncodeSemanticInterfaceSidecar(sidecar, supported);
  if (!encoded) { llvm::consumeError(encoded.takeError());Failure("reviewed seed failed encoding"); }
  if (variant == 2) return Pack(source, "{not JSON");
  if (variant == 4) source += "// digest mismatch\n";
  return Pack(source, *encoded);
}
} // namespace objc3::fuzz
