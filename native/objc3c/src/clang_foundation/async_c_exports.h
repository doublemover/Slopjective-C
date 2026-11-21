#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace clang {
class ASTContext;
}

namespace objc3::clang_foundation {

struct CExportSource {
  std::string header;
  std::string definitions;
  std::vector<std::string> interface_exports;
  std::string swift_overlay;
};

// Derive both halves of the C ABI from the same checked source declarations.
// Each name identifies one non-overloaded function in the translation unit.
[[nodiscard]] CExportSource GenerateCExports(
    clang::ASTContext &context, std::span<const std::string> async_symbols,
    std::span<const std::string> throws_symbols, std::string &failure,
    std::string_view swift_module = {});

}  // namespace objc3::clang_foundation
