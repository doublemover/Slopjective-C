#pragma once

#include <string>
#include <vector>

#include "clang_foundation/interface_import.h"

namespace objc3::clang_foundation {

// A source-level import request carries the module identity selected by the
// package lock; it deliberately does not carry an artifact path.
struct InterfaceModuleRequest {
  std::string canonical_name;
  InterfaceModuleIdentity locked_identity;
  bool public_import = true;
  bool reexported = false;
  // False for entries in a source-import registry. These modules are
  // validated/prepared but become visible only when the parser encounters
  // their source-level @import directive.
  bool initially_visible = true;
  std::vector<std::string> spi_groups;
};

struct InterfaceModuleResolution {
  bool success = false;
  bool requires_coroutine_header = false;
  std::vector<InterfaceImportInput> artifacts;
  std::vector<std::string> dependency_order;
  std::vector<std::string> visible_module_names;
  std::vector<InterfaceContractDiagnostic> diagnostics;
};

// Resolve canonical module names against checked `.objc3i.json`/`.objc3i`
// pairs in the configured search roots, then load exact locked transitive
// dependencies in deterministic dependency-first order.
[[nodiscard]] InterfaceModuleResolution ResolveInterfaceModules(
    const std::vector<std::string> &search_roots,
    const std::vector<InterfaceModuleRequest> &requested_modules,
    const InterfaceImportTarget &target);

// Stable relative include spelling emitted into canonical interfaces.
[[nodiscard]] std::string BuildInterfaceIncludePath(
    llvm::StringRef canonical_module_name);

} // namespace objc3::clang_foundation
