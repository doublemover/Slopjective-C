#pragma once
#include "clang_foundation/interface_contract.h"
#include <string>
namespace clang {
class ASTContext;
class Decl;
class NamedDecl;
class SourceLocation;
} // namespace clang
namespace objc3::clang_foundation {
// Native labels depend only on the owning module/package and normalized Clang
// declaration identity. Interface fingerprints and filesystem paths are absent.
std::string BuildModuleOwnedName(const std::string &module,
                                 const std::string &package,
                                 const std::string &stable_id);
bool HasValidModuleOwnedName(const InterfaceModuleIdentity &module,
                             const InterfaceDeclarationIdentity &declaration);
std::string NormalizeModuleSourceRoot(const clang::ASTContext &context,
                                      const std::string &root);
bool IsModuleOwnedSource(const clang::ASTContext &context,
                         clang::SourceLocation location,
                         const std::string &root = {});
std::string ReadNativeSymbolName(const clang::NamedDecl *declaration);
bool ApplyModuleOwnedNames(clang::ASTContext &context, clang::Decl *declaration,
                           const std::string &module,
                           const std::string &package,
                           const std::string &source_root);
} // namespace objc3::clang_foundation
