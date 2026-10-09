#pragma once

#include <string>
#include <vector>

#include "clang_foundation/interface_contract.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
class ASTContext;
}

namespace objc3::clang_foundation {

// Emits public ordinary C/Objective-C declarations from an already parsed
// Clang AST. Unsupported semantics are diagnosed and suppress both outputs.
// Module dependencies are accepted only as fully resolved interface identities.
[[nodiscard]] InterfaceEmissionArtifact EmitSemanticInterface(
    clang::ASTContext &context, llvm::StringRef canonical_module_name,
    llvm::StringRef package_identity,
    const std::vector<InterfaceDependency> &resolved_dependencies = {});

}  // namespace objc3::clang_foundation
