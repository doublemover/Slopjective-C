#pragma once

#include "llvm/ADT/StringRef.h"
#include <string>
#include <utility>
#include <vector>

namespace clang {
class ASTContext;
class NamedDecl;
class ImportDecl;
} // namespace clang

namespace objc3::clang_foundation {

// A known SDK type is admitted only from the exact public header compiled into
// this frontend, with its native layout and callable fields checked as well.
bool IsQualifiedRuntimeFrameType(const clang::NamedDecl *declaration,
                                 const clang::ASTContext &context);
llvm::StringRef RuntimeInterfaceSemantic();
llvm::StringRef RuntimeInterfaceHeaderSource();
llvm::StringRef RuntimeTaskHeaderSource();
bool IsQualifiedRuntimeTaskHeader(const clang::ASTContext &context);

// System modules from the canonical SDK library inventory need no facade
// dependency artifact. Public signature types still undergo normal dependency
// checks; arbitrary Clang modules and changed/lookalike SDK headers fail closed.
bool IsQualifiedStandardLibraryImport(const clang::ImportDecl *declaration,
                                      const clang::ASTContext &context);


// Canonical interfaces reconstruct the SDK declaration without carrying the
// provider's filesystem paths into the consumer. Integer typedefs use the
// target compiler's builtin types, rather than the host libc's headers.
void AddRuntimeInterfaceFiles(
    std::vector<std::pair<std::string, std::string>> &files,
    std::vector<std::string> &arguments, bool cxx_task_definitions = false);

} // namespace objc3::clang_foundation
