#pragma once

#include <string>
#include <vector>

#include "clang_foundation/interface_contract.h"
#include "llvm/ADT/StringRef.h"

namespace clang {
class ASTContext;
class NamedDecl;
class LangOptions;
}

namespace objc3::clang_foundation {

// Exact parser dialect, standard and ownership mode used by providers and
// importers.
[[nodiscard]] std::string
ReadInterfaceLanguageProfile(const clang::LangOptions &language);

// Native declaration owning the selected export policy of a row.
[[nodiscard]] std::string
ReadInterfaceExportRoot(const clang::NamedDecl *declaration);

// SPI applies to a complete exported root, including its native members.
[[nodiscard]] std::string
ReadInterfaceSPIGroup(const clang::NamedDecl *declaration);

// Clang USR with file-scope C typedef locations normalized for artifact moves.
[[nodiscard]] std::string ReadInterfaceDeclarationStableId(
    const clang::NamedDecl *declaration);

// Read the same canonical declaration/layout/effect identity used by emission.
// Returns false when the declaration has no supported canonical identity.
[[nodiscard]] bool ReadInterfaceDeclarationIdentity(
    clang::NamedDecl *declaration, const clang::ASTContext &context,
    InterfaceDeclarationIdentity &identity);

// Emits public C/Objective-C declarations and bounded C++ data APIs from an
// already parsed Clang AST. Unsupported semantics are diagnosed and suppress
// both outputs. Module dependencies are accepted only as fully resolved
// interface identities.
[[nodiscard]] InterfaceEmissionArtifact EmitSemanticInterface(
    clang::ASTContext &context, llvm::StringRef canonical_module_name,
    llvm::StringRef package_identity,
    const std::vector<InterfaceDependency> &resolved_dependencies = {},
    const std::vector<InterfaceSourceArtifact> &dependency_sources = {},
    llvm::StringRef ownership_projection = {},
    const std::vector<std::string> &exports = {},
    const std::string &source_root = {});

// Emit a distinct consumer-mode interface, retaining the canonical producer
// proof. The native object remains compiled under the producer's ownership.
[[nodiscard]] InterfaceEmissionArtifact EmitOwnershipProjection(
    clang::ASTContext &producer, llvm::StringRef canonical_module_name,
    llvm::StringRef package_identity, bool consumer_arc,
    const std::vector<InterfaceDependency> &dependencies = {},
    const std::vector<std::string> &exports = {},
    const std::string &source_root = {});

}  // namespace objc3::clang_foundation
