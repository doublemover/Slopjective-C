#include "ir/objc3_ir_module_identity.h"

#include <iomanip>
#include <sstream>

#include "support/objc3_identifier_safe_suffix.h"

std::string JoinStringParts(const std::vector<std::string> &parts,
                            const std::string &delimiter) {
  std::ostringstream out;
  for (std::size_t i = 0; i < parts.size(); ++i) {
    if (i != 0) {
      out << delimiter;
    }
    out << parts[i];
  }
  return out.str();
}

std::uint64_t StableRuntimeMetadataLinkerAnchorHash(
    const std::string &text) {
  std::uint64_t hash = 1469598103934665603ull;
  for (unsigned char c : text) {
    hash ^= static_cast<std::uint64_t>(c);
    hash *= 1099511628211ull;
  }
  return hash;
}

std::string LowerHex64(std::uint64_t value) {
  std::ostringstream out;
  out << std::hex << std::nouppercase << value;
  return out.str();
}

std::string MakeModuleIdentifierSafeSuffix(const std::string &text) {
  return objc3c::support::MakeIdentifierSafeSuffix(text, "module");
}

std::string EncodeBoundaryTokenValueHex(const std::string &text) {
  static constexpr char kHex[] = "0123456789abcdef";
  std::string out;
  out.reserve(text.size() * 2u);
  for (unsigned char ch : text) {
    out.push_back(kHex[(ch >> 4) & 0x0f]);
    out.push_back(kHex[ch & 0x0f]);
  }
  return out;
}
