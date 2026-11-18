#pragma once

#include <span>
#include <string>
#include <vector>

namespace clang {
class ASTContext;
}

namespace objc3::clang_foundation {

struct AsyncCExportSource {
  std::string header;
  std::string definitions;
  std::vector<std::string> interface_exports;
};

// Derive both halves of the C ABI from the same checked source declarations.
// Each name identifies one non-overloaded function in the translation unit.
[[nodiscard]] AsyncCExportSource GenerateAsyncCExports(
    clang::ASTContext &context, std::span<const std::string> symbols,
    std::string &failure);

}  // namespace objc3::clang_foundation
